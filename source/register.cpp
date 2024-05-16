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

bool Register::parseJsonObjectAsField(const QJsonObject &field_obj, quint32 options){
    QStringList available_keys = field_obj.keys();

    QString field_tag;
    //mandatory field
    if(available_keys.contains("name")){
        field_tag = "name";
    }
    else if(available_keys.contains("field")) {
        field_tag = "field";
    }else{
        // not field
        return false;
    }
    // start to create a field and give a name    
    const QString field_name = field_obj[field_tag].toString();
    available_keys.removeOne(field_tag);

    if(available_keys.contains("offset")){
        quint32 offset;

        // offset is a string for hexademical (0x...)
        if(field_obj["offset"].isString())
        {
            const QString offset_value = field_obj["offset"].toString();
            if(offset_value.startsWith('B')){
                offset = Register::strToUInt(offset_value.mid(1))*8;
            }else
                offset = Register::strToUInt(offset_value);
        }
        // offset is an integer value
        else {
            offset = (quint32)field_obj["offset"].toInt();
        }

        // offset should be always forward
        if((quint32)size() < offset+1 ){
            moveOffset(offset);

        }else{
            WARNING("Can't make offset");
        }

        // this key is processed remove it from list
        available_keys.removeOne("offset");
    }

    BitField::Parser parser(field_name.toLatin1().constData());
    BitField *f = 0;

    const bool name_exist = contains( parser.name() );

    // on AllowSame name when we add bits to existing
    if( options &AllowSameName){
        if(name_exist)
            f = field(parser.name());
        else
            //first field when AllowSameName mode
            f = BitField::makeField(field_name, &parser);
    }

    // always make a new field but rename if exist
    else{
        f = BitField::makeField(field_name, &parser);
        // make another field with the same name
        int i=0;
        if(name_exist){
            QString another_name = parser.name();
            while(contains(another_name))  {
                another_name = QString("%1_%2").arg(parser.name()).arg(i);
                i++;
            }
            //rename
            f->setName(another_name);
        }
    }



    if(f != 0){
        if(options &AbsoluteRange && parser.ranges() >0){
            int lsb = parser.lsb() + m_offset;
            if(size() < lsb+1 ){
                resize(lsb);
            }
        }

        if(options &Default1) f->fill(1);
        if(options &Default0) f->fill(0);


        while(available_keys.count()){
            const QString key = available_keys.first();
            if( key == "descr"){
                f->setDescription(field_obj[key].toString());
            }
            else if(key == "value"){
                if(field_obj[key].isString())
                    f->setValue(Register::strToUInt(field_obj[key].toString()));
                else
                    f->setValue((quint32)field_obj[key].toInt());
            }
            else if(key == "const"){
                f->setConstant(field_obj["const"].toBool(false));
            }
            else{
                f->setExtra(key, field_obj[key].toVariant());
            }
            available_keys.removeFirst();
        }//while
        addField(f);
        return true;
    }
    return false;
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

        // first text field is a name
        if(i==0 && fields_array[i].isString()){
               setName(fields_array[i].toString());
        }

        // otherwise this is normal field parse (should contain 'name' identifier)
        else if(fields_array[i].isObject()){
            const QJsonObject field_obj = fields_array[i].toObject();

            // process As BIT FIELD
            if(field_obj.keys().contains("name"))
                if(!parseJsonObjectAsField(field_obj,  options))
                     WARNING(QString("Line %1 ignored").arg(i));
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
        Q_EMIT __SET(name());
        break;
    }
}



bool Register::addField(const QString &fieldname, quint32 options)
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


bool Register::contains(const QString &name)
{
    return field(name) != 0;
}



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

            if(pfield->size()>1){
                dict["name"] = QString("%1[%2]").arg(pfield->name()).arg(pfield->size());
                dict["offset-name"] = QString("%1[%2]")
                                        .arg(i/8,4,16,QChar('0'))
                                        .arg(i%8);
            }
            else{
                dict["name"] = pfield->name();
                dict["offset-name"] = QString("%1[%2:%3]")
                                        .arg(i/8,4,16,QChar('0'))
                                        .arg(i%8)
                                        .arg(i%8+pfield->size());
            }

            dict["purename"] = pfield->name();
            dict["hexbyte"] = pfield->toHex();
            dict["ascii"] = QString::fromStdString(pfield->toByteArray().toStdString());
            dict["hex-value"] = QString::number(pfield->value(),16);
            dict["binary"] = QString("b%1").arg(pfield->value(),0,2);
            dict["value"]  = QString::number(pfield->value());
            dict["dec-value"]  = QString::number(pfield->value());
            dict["offset"] = QString("%1").arg(i,0,16);
            dict["range"] = QString("%2:%1").arg(i).arg(i+pfield->size()-1);
            dict["descr"] = pfield->description();
            dict["extras"] = pfield->extras().join('|');
            dict["rdonly"] = QString::number(pfield->constant());

            // replace bit extra with name
            foreach(const QString &extraname, pfield->extras())
            {
                dict[extraname] = pfield->extra(extraname).toString();
            }
            i += pfield->size()-1;
            replaceTagsInLine(&line,dict);
            result += line;
        }
        // not assigned to field
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
                dict["hexbyte"] = pfield->toHex();
                dict["hex-value"] = QString("%1").arg(at(i)->value,0,16);
                dict["dec-value"]  = QString::number(pfield->value());
                dict["binary"] = QString("b%1").arg(at(i)->value,0,2);
                dict["value"]  = QString::number(at(i)->value);
                dict["rdonly"] = "false";
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
    foreach(const QString &line ,text.split(ln_separator,QString::SkipEmptyParts))
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

