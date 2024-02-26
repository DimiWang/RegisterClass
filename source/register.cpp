#if 1
/**
 * @file:register.cpp   -
 * @description: this is class to maintain bits with names
 * @project:
 * @date:
 * Version 1.07
 */


#include "register.h"
#include <QDebug>
#include "bitfield.h"
#include <QStringList>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValue>

const char *Register::tag_purename    = "@purename";
const char *Register::tag_name     = "@name";
const char *Register::tag_value    = "@value";
const char *Register::tag_value_hex = "@value_hex";
const char *Register::tag_offset     = "@offset";
const char *Register::tag_descr    = "@descr";
const char *Register::tag_extras    = "@extras";
const char *Register::tag_readonly    = "@readonly";



/****************************************************************************
 * @function name: constructor
 *
 * @param:
 *
 *  void
 * @description:
 ****************************************************************************/
Register::Register(Register *parent, const QString &name, bool is_sub
                   , const QByteArray &init_json, quint32 load_options)
    : QObject()
    ,BitSet(name, -1, !is_sub)
{
    mp_parent = parent;
    m_is_sub = is_sub;
    m_offset = 0;
    mp_temporary= NULL;
    m_update_policy = UpdateOnChange;
    if(!init_json.isEmpty()){
        this->blockSignals(1);
        loadJsonData(init_json, load_options);
        this->blockSignals(0);
    }
}


/****************************************************************************
 * copy constructor
 *     const Register &reg - register
 * @description: creates register with bits without names
 ****************************************************************************/
Register::Register( Register &reg)
    : QObject()
    ,BitSet()
{    
    m_is_sub = false;
    m_offset = 0;
    mp_temporary= NULL;
    m_update_policy = UpdateOnChange;
    copy(reg);
}

Register::~Register()
{
    clear();
    emit removed();
}

void Register::copy(Register &src)
{
    m_fields.clear();
    clear();
    BitSet::copy(src);
    for(int i=0;i<src.fieldsCount();i++){
        BitField *src_f = src.field(i);
        BitField *f = new BitField(src_f->name());
        int ind_first = src.indexOf(src_f->first());
        int ind_last = src.indexOf(src_f->last());
        for(int j=ind_first; j<=ind_last;j++){
            f->append(at(j));
        }
        m_fields.append(f);
    }
}

bool Register::blockSignals(bool b)
{
    bool r = QObject::blockSignals(b);
    if(mp_temporary)  mp_temporary->blockSignals(b);
    return r;
}

void Register::clear()
{
    if(!isSub()){
        foreach(BitField *pfield, m_fields){
            delete pfield;
        }
    }
    BitSet::clear();
    m_fields.clear();
    m_offset =0;

}

bool Register::loadJsonData(const QByteArray &json_data, quint32 options )
{
    QJsonParseError parseError;
    QJsonDocument jsonDoc = QJsonDocument::fromJson(json_data, &parseError);
    if(parseError.error != QJsonParseError::NoError){
        WARNING(QString("Parse error at %1:%2").arg(parseError.offset).arg(parseError.errorString()));
        qDebug()<<QString("Parse error at %1:%2").arg(parseError.offset).arg(parseError.errorString());
        return false;
    }

    const QJsonArray fields_array = jsonDoc.array();
    for(int i=0;i<fields_array.count();i++){
        const QJsonObject field = fields_array[i].toObject();
        QStringList available_keys = field.keys();

        if(available_keys.contains("offset")){
            quint32 offset;
            if(field["offset"].isString())
                offset = Register::strToUInt(field["offset"].toString());
            else offset = (quint32)field["offset"].toInt();
            if((quint32)size() < offset+1 ){
               moveOffset(offset);

            }else{
                WARNING("Can't make offset");
            }
            available_keys.removeOne("offset");
        }

        //mandatory field
        if(!available_keys.contains("name")) {
            WARNING(QString("Line %1 ignored").arg(i));
            continue;
        }
        available_keys.removeOne("name");

        const QString field_name = field["name"].toString();
        BitField::Parser parser(field_name.toLatin1().constData());
        BitField *f = BitField::makeField(field_name, &parser);
        if(f != 0){
            f->setValue(parser.value());
            f->setConstant(parser.is_value_constant());

            if(options &AbsoluteRange && parser.ranges() == 2){
                int lsb = parser.lsb() + m_offset;
                if(size() < lsb+1 ){
                    resize(lsb);
                }
            }
            //--- todo AllowSameName

            while(available_keys.count()){
                const QString key = available_keys.first();
                if( key == "descr"){
                    f->setDescription(field[key].toString());
                }
                else if(key == "value"){
                    if(field[key].isString())
                        f->setValue(Register::strToUInt(field[key].toString()));
                    else
                        f->setValue((quint32)field[key].toInt());
                }
                else if(key == "const"){
                    f->setConstant(field["const"].toBool(false));
                }
                else{
                    f->setExtra(key, field[key].toVariant());
                }
                available_keys.removeFirst();
            }//while
            addField(f);
        }
    }
    return true;
}


void Register::cond_update(bool changed)
{
    switch(m_update_policy)
    {
    case UpdateNever:
        break;
    case UpdateOnChange:
        if(!changed)   break;
    case UpdateAlways:
        __SET(name());
        break;
    }
}


/****************************************************************************
 * @function name: Register::addField()
 *
 * @param:
 *   const QByteArray &bitname
 * @description: Adds bit with name value automatically set to zero
 *              m_chain is a list container of single bits. each bit is represented
 *              by pointer to object Bit. So m_chain index equals to bit position in list.
 *              Bit are stored from LSB to msb.
 *              Example: Register("A[8];B[8]")
 *                       in m_chain is A(0,,,7),B(0,,,7),...
 *              name is the same.
 *              There are two types of parser used
 *              I) has format "@register=NAME;@bit=A[8] @value=123 @descr=DESCRIPTION;..."
 *              II) second format "A[8]=0;B[8]=123#DESCRIPTION;,,,"
 * @return: ( int ) number of bits added
 ****************************************************************************/
bool Register::addField(const QString &fieldname, quint32 options, qint32 put_to)
{
    int result=0;
    if(!fieldname.isEmpty())
    {
        BitField::Parser parser(fieldname.toLatin1().constData());
        BitField *f = BitField::makeField(fieldname, &parser);

        if(options & AbsoluteRange){
            int lsb = parser.lsb() + m_offset;
            if(size() < lsb+1 ){
                resize(lsb);
            }
        }
        if(options & AllowSameName){
            if(contains(parser.name())){
                field(parser.name())->join(f);
                delete f;
                f=0;
            }
        }
        if(f)
            addField(f);

        emit changed();
    }

    if( !result )
        WARNING(QString("ERROR:%1").arg(fieldname));

    return result;
}


void Register::addField(BitField *bitfield)
{
    m_fields.append(bitfield);
    for(int i=0;i<bitfield->size();i++){
        append(bitfield->at(i));
    }
}



QStringList Register::fieldsList()
{
    QStringList list;
    for(int i=0;i<m_fields.count();i++){
        list.append(m_fields.at(i)->name());
    }
    return list;
}

int Register::fieldsCount() const
{
    return m_fields.count();
}

/****************************************************************************
 * @function name: Register::contains()
 *
 * @param:
 *      const QString &name
 * @description: if register contains bit name
 * @return: (Bit *) - bit pointer
 ****************************************************************************/
bool Register::contains(const QString &name)
{
    return field(name) != 0;
}


/****************************************************************************
 * @function name: Register::toString()
 *
 * @param:
 *             void
 * @description: Converts register to string
 * @return: ( const QByteArray)  -returns string
 ****************************************************************************/
const QString Register::toString(const QString &format, bool skip_empty)
{
    QString result ;

    // make lookup table
    QHash<Bit*,BitField*> m_bit_lookup;
    for(int i=0;i<m_fields.size();i++){
        for(int j=0;j<m_fields.at(i)->size();j++){
            m_bit_lookup[m_fields.at(i)->at(j)] = m_fields.at(i);
        }
    }

    for(int i=0;i<size();i++)
    {
        QString line = format;
        BitField *pfield =0;

        if(m_bit_lookup.contains(bitAt(i))){
            pfield = m_bit_lookup[bitAt(i)];
        }

        QMap<QString, QString> dict;
        // in field
        if(pfield){

            if(pfield->size()>1)
                dict["name"] = QString("%1[%2]").arg(pfield->name()).arg(pfield->size());
            else
                dict["name"] = pfield->name();
            dict["purename"] = pfield->name();
            dict["hex"] = QString::number(pfield->value(),16);
            dict["bin"] = QString("b%1").arg(pfield->value(),0,2);
            dict["value"]  = QString::number(pfield->value());
            dict["offset"] = QString("%1").arg(i,0,16);
            dict["range"] = QString("%2:%1").arg(i).arg(i+pfield->size()-1);
            dict["descr"] = pfield->description();
            dict["extras"] = pfield->extras().join('|');
            dict["readonly"] = QString::number(pfield->constant());

            // replace bit extra with name
            foreach(const QString &extraname, pfield->extras())
            {
                dict[extraname] = pfield->extra(extraname).toString();
            }
            i += pfield->size()-1;
            replaceTagsInLine(&line,dict);
            result += line;
        }
        else{
            if(!skip_empty){

                int _undef_size = 0;
                quint32 bak_i = i;
                for(;i<size();i++){
                    _undef_size++;
                    if(m_bit_lookup.contains(bitAt(i))){
                        break;
                    }
                }

                dict["name"] = QString("_undef_[%2]").arg(_undef_size);
                dict["purename"] = "_undef_";
                dict["hex"] = QString("%1").arg(at(i)->value,0,16);
                dict["bin"] = QString("b%1").arg(at(i)->value,0,2);
                dict["value"]  = QString::number(at(i)->value);
                dict["readonly"] = false;
                dict["offset"] = QString("%1").arg(bak_i,0,16);
                dict["range"] = QString("%2:%1").arg(bak_i).arg(bak_i+_undef_size-1);
                replaceTagsInLine(&line,dict);
                result += line;
            }
        }

    }
    return result;
}





void Register::replaceTagsInLine(QString *line, QMap<QString,QString> &dict){
    foreach(const QString &tag, dict.keys()){
        line->replace("@"+tag, dict[tag]);
    }
}

bool Register::fromString(const QString &text, const char ln_separator, const char eq_separator)
{
    bool ok=false;
    foreach(const QString &line ,text.split(ln_separator,Qt::SkipEmptyParts))
    {
        QStringList l = line.split(eq_separator);
        if(l.size() == 2)
        {
            BitField *f = field(l[0]);
            f->setValue(l[1].toUInt());

        }
        else {
            ok=false;
            break;
        }
    }
    return ok;
}


/****************************************************************************
 * @function name: Register::findFieldByName - ---
 *
 * @param:
 *
 *  const QByteArray & name
 * @description:
 * @return: ( qint32 ) returns index of bit in scanchain
 ****************************************************************************/
BitField* Register::field(const QString &name)
{
    for (qint32 i = 0; i < m_fields.size(); i++)
    {
        if (m_fields.at(i)->name() == name )
        {
            return m_fields.at(i);
        }
    }
    return 0;
}

BitField *Register::field(int index)
{
    return m_fields.at(index);
}



unsigned int Register::strToUInt(const QString &text, bool *ok)
{
    if(text.startsWith("0x")) return text.mid(2).toUInt(ok,16);
    return text.toUInt(ok,10);
}

Register *Register::makeSubRegister(Register *parent, const QString &name)
{
    Register *preg = new Register(parent, name,AllowSameName|SubRegister);
    return preg;
}


/****************************************************************************
 * @function name: Register::setValue()
 *
 * @param:
 *g
 *      QByteArray field -field name.
 *                      Format is: (I)"string[nn]" -where nn = number of bit
 *                                  'string' is not requred.this parameter
 *                                  will ignored
 *                                 (II) "string[nn:yy]" - nn:yy is a field
 *      quint32 value  -value is value to pass to a field or bit
 * @description: This function sets value to scanchain
 * @return: ( void )
 ****************************************************************************/
bool Register::setFieldValue(const QString &field, quint32 value)
{
    bool result = false;
    const QRegExp rx_fullname("(.*)\\[(.*)\\]");
    QString regname;
    QString range;
    if(rx_fullname.exactMatch(field)){
        regname = rx_fullname.cap(1);
        range = rx_fullname.cap(2);
    }else{
        range = field;
    }
    const char *templates[] = {"\\d+", "(\\d+):(\\d+)", "\\w*",0};
    QRegExp rx;
    for(int i=0; templates[i]!=0; i++)
    {
        rx.setPattern( templates[i] );

        if(rx.exactMatch(range))
        {
            switch(i)
            {
            // format reg[i]
            case 0:
            {
                bool ok;
                Bit *pbit = bitAt(range.toInt(&ok));
                if(pbit && ok) {
                    result = true;
                    pbit->value =value;
                }
            }
                break;

                // format reg[i:j]
            case 1:{
                int msb = rx.cap(1).toInt();
                int lsb = rx.cap(2).toInt();
                if(indexValid(msb) && indexValid(lsb)){
                    result = true;
                    sub(lsb, msb)->setUInt(value);
                }
            }
                break;

                // format reg[field]
            case 2:
                if(range.isEmpty()){
                    blockSignals(1);
                    setUInt(value);
                    blockSignals(0);
                }
                else if(contains(range))
                {
                    sub(range)->setUInt(value);
                }
                result = true;
                break;
            }
            break;
        }
    }
    return result;
}



/****************************************************************************
 * @function name: Register::value()
 *
 * @param:
 *
 *  QByteArray field - see format above
 * @description: thisfunction returns value of field or single bit
 * @return: ( quint32 ) returns value
 ****************************************************************************/
quint32 Register::fieldValue(const QString &field) // ------------------------------------remake
{
    quint32 result=0;
    const QRegExp rx_fullname("(.*)\\[(.*)\\]");
    QString regname;
    QString range;
    if(rx_fullname.exactMatch(field)){
        regname = rx_fullname.cap(1);
        range = rx_fullname.cap(2);
    }else{
        range = field;
    }
    const char *templates[] = {"\\d+", "(\\d+):(\\d+)", "\\w*",0};
    QRegExp rx;
    bool fail = false;
    for(int i=0; templates[i]!=0; i++)
    {
        rx.setPattern( templates[i] );
        if(rx.exactMatch(range))
        {
            switch(i)
            {
            case 0:
            {
                bool ok;
                Bit *pbit = at(range.toInt(&ok));
                if(pbit && ok) {
                    result =  pbit->value;
                }
                else fail = true;
            }
                break;

            case 1:{
                int lsb = rx.cap(1).toInt();
                int msb = rx.cap(2).toInt();
                if(indexValid(lsb) && indexValid(msb))
                {
                    result = sub(lsb,msb)->toUInt();
                }
                else fail = true;
            }
                break;

            case 2:
                if(range.isEmpty()){
                    result = toUInt();
                }
                else if(this->contains(range))
                {
                    Register *preg = sub(range);
                    if(!preg->isEmpty())
                        result = preg->toUInt();
                }
                else fail = true;
                break;
            }
            break;
        }
    }

    if(fail)
        WARNING(QString("Register(%1).value(%2):Error field not found")
                .arg(name()).arg(field));

    return result;
}




/****************************************************************************
 * @function name: Register::sub()
 * @param:
 *      qint32 from - index from including itself
 *      qint32 to - index to including itself
 *              if to is higher then size() or -1
 *                   will be assigned to value size()-1
 * @description: sets value to range in register
 *
 * @return: ( void )
 ****************************************************************************/
Register * Register::sub(qint32 from, qint32 to)
{
    if(mp_temporary == NULL)
    {
        makeTemporary();
    }
    mp_temporary->clear();

    qint32 lsb = qMin(from,to);
    qint32 msb = qMax(from,to);
    qint32 i;
    // cut off
    if(msb<0 || msb >= size()) msb = size()-1;

    for (i = lsb; i <= msb; i++)
    {
        mp_temporary->append(bitAt(i));
    }

    if(lsb >= size() || msb >= size())
        qWarning()<<QString("Register(%1).sub(%2,%3) - can't find msb, lsb")
                    .arg(name()).arg(from).arg(to);
    return mp_temporary;
}

/****************************************************************************
 * @function name: Register::sub()
 * @param:
 *      const QString &bitname - bits with name
 * @description: sets value to bits with given name
 *
 * @return: ( void )
 ****************************************************************************/
Register * Register::sub(const QString &fieldname)
{
    if(mp_temporary == NULL) {
        makeTemporary();
    }
    mp_temporary->clear();
    QRegExp regexp("((\\d+):(\\d+))|(\\d+)");
    if(regexp.exactMatch(fieldname) )
    {
        qint32 lsb=-1,msb=-1;
        regexp.indexIn(fieldname);
        if(regexp.cap(1).isEmpty())
        {
            lsb = msb= regexp.cap(4).toUInt();
        }
        else {
            quint32 r1 = regexp.cap(2).toUInt();
            quint32 r2 = regexp.cap(3).toUInt();
            lsb = qMin(r1,r2);
            msb = qMax(r1,r2);
        }
        for(qint32 i=lsb;i<=msb;i++)
        {
            if(indexValid(i)) mp_temporary->append(at(i));
        }
    }
    else{
        BitField *f = field(fieldname);
        mp_temporary->addField(f);

    }

    if(mp_temporary->isEmpty() ){
        WARNING(QString("Register(%1).sub(\"%2\") - can't find field")
                .arg(name())
                .arg(fieldname)
                );
    }

    return mp_temporary;
}

Register *Register::sub(const QStringList &fields)
{
    if(mp_temporary == NULL) {
        makeTemporary();
    }
    mp_temporary->clear();
    QStringList traceback_list;

    foreach(const QString &fieldname, fields)
    {
        BitField *f = field(fieldname);
        if(f)
            mp_temporary->addField(f);
        else {
            traceback_list.append(fieldname);
        }
    }

    if(mp_temporary->isEmpty() ){
        WARNING(QString("Register(%1).sub(\"%2\") - can't find fields")
                .arg(name())
                .arg(traceback_list.join(",")));
    }
    return mp_temporary;
}

Register * Register::sub(const QString &extra_name, const QVariant &extra_value)
{
    if(mp_temporary == NULL) {
        makeTemporary();
    }
    mp_temporary->clear();

    for(int i=0;i<m_fields.size();i++)
    {
        BitField *f = field(i);
        if ( f->extra(extra_name) == extra_value )
        {
            mp_temporary->addField(f);
        }
    }
    return mp_temporary;
}


/****************************************************************************
 * @function name: Register::isSame()
 * @param:
 *      Register *preg - pointer to register compares with current
 * @description: returns true if register is same.
 *      Same means when register has same bit names and size
 * @return: ( bool ) true - is similar
 ****************************************************************************/
bool Register::isSame(Register *preg)
{
    if(BitSet::isSame(preg)){
        for (int i = 0; i < m_fields.size(); i++)
        {
            BitField *field1 = m_fields.at(i);
            BitField *field2 = preg->field(field1->name());

            if(*field1 != *field2 ) break;
        }
        return true;
    }
    return false;
}


void Register::setExtra(const QString &name, const QVariant &value)
{
    m_extra[name] = value;
}

QStringList Register::extras() const
{
    return m_extra.keys();
}


QVariant Register::extra(const QString &name)
{
    if(m_extra.contains(name))
    {
        return  m_extra[name];
    }
    return QVariant();
}

unsigned int Register::extraAsUInt(const QString &name, bool *ok)
{
    return  strToUInt(extra(name).toString() ,ok);
}

Register &Register::operator [](const QString &field)
{
    return *sub(field);
}

void Register::makeTemporary()
{
    mp_temporary =  Register::makeSubRegister(this,"sub_"+name());
}


void Register::clearTemporary()
{
    mp_temporary->clear();
}

void Register::moveOffset(unsigned int offset )
{
    m_offset = offset;
    resize(offset);
}

/****************************************************************************
 * @function name: Register::removeField(const QString &name)
 * @param:
 *        const QString &name
 * @description:
 * @return: ( quint32 )
 ****************************************************************************/
void Register::removeField(const QString &fieldname, bool include_bits)
{
    BitField *f = field(fieldname);
    m_fields.removeOne(f);    
    // deleting bits
    if(include_bits)    {
        while(!f->empty()){
            Bit * pbit = f->takeFirst();
            if(!isSub())
                delete pbit;
        }
    }
    // delete field
    if(!isSub()){
        delete f;
    }
}

quint32 Register::crc(int bits, quint32 seed, quint32 poly, bool padding, QString *ptext)
{
    Register r_poly(0,QString("payload[%d]").arg(bits+1));
    r_poly.setUInt(poly);
    int sz = r_poly.size();
    quint32 crc=0;
    Register reg(*this);

    // append seed to the end because we assume that we calculate from MSB but register starts from LSB
    reg.addField(QString("seed[%1]=%2").arg(bits).arg(seed),0);

    // insert bits count according to CRC size 8,16,32...
    if(padding)
        reg.addField(QString("padding[%1]=0").arg(bits),0);

    //only for debugging purposes
    QString space;
    if(ptext){
        ptext->append(reg.toBitString(Register::MSB)+" | "+r_poly.toBitString(Register::MSB)+"\n");
    }

    for(int i=0; (i+sz) <= reg.size(); i++)
    {

        if(reg.bitValue(reg.size()-i-1) !=0) {
            int from = reg.size()-sz-i;
            int to = reg.size()-i-1;
            (*reg.sub(from, to)) ^= r_poly;

            // if need debug info
            if(ptext){
                ptext->append(QString("\n%1%2").arg(space).arg(QString(r_poly.toBitString(Register::MSB))));
                ptext->append("\n----------------");
                ptext->append("\n" + reg.toBitString(Register::MSB));
            }
        }
        // debug info
        if(ptext){            space += " ";        }
    }
    // final CRC is last bits
    crc = reg.sub(0,bits-1)->toUInt();
    return crc;
}



#endif

