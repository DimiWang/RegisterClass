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

const char *Register::tag_purename    = "@purename";
const char *Register::tag_name     = "@name";
const char *Register::tag_value    = "@value";
const char *Register::tag_value_hex = "@value_hex";
const char *Register::tag_group    = "@group";
const char *Register::tag_bitn     = "@bitn";
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
Register::Register()
    : QObject()
    ,BitSet()
{
    m_is_sub = false;
    m_options = AllowSameName;
    m_offset = 0;
    mp_temporary= NULL;
    m_update_policy = UpdateOnChange;
}


/****************************************************************************
 * @function name: constructor
 *
 * @param:
 *
 *  const QByteArray &bit_list
 * @description: this constructor cretaes bit scan chain with names and values
 *                name can be passed as normal c names without []
 *                value can be passed with name as "bit8=1"
 ****************************************************************************/
Register::Register(const QString &init_script, const QString  &name )
    : QObject()
    ,BitSet(name)
{
    m_is_sub = false;
    m_options = AllowSameName;
    m_offset = 0;
    mp_temporary= NULL;
    m_update_policy = UpdateOnChange;
    this->blockSignals(1);
    makeSetup(init_script);
    this->blockSignals(0);
}



/****************************************************************************
 * @function name: constructor
 * @param:
 *  qint32 bit_count
 * @description: creates register with bits without names
 ****************************************************************************/
Register::Register(qint32 bit_count, const QString  &name ,  quint32 options)
    : QObject()
    ,BitSet(name)
{
    m_is_sub = false;
    m_options = options;
    m_offset = 0;
    mp_temporary= NULL;
    m_update_policy = UpdateOnChange;
    resize(bit_count);
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
    m_options = AllowSameName;
    m_offset = 0;
    mp_temporary= NULL;
    m_update_policy = UpdateOnChange;
    copy(reg);
}

Register::~Register()
{
    emit removed();
}

void Register::copy(Register &src)
{
    m_fields.clear();
    clear();
    for(int i=0;i<src.fieldsCount();i++){
        BitField *f = 0;
        if(isSub()) {
            f = new BitField(src.field(i)->name(), src.field(i)->size(), false);

        }else{
            f = new BitField(src.field(i)->name(), src.field(i)->size());
        }
        *f = *src.field(i);
        addField(f);
    }
    buildReg();
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
    m_fields.clear();
    BitSet::clear();

    foreach(const QString &name, m_vregs.keys())
    {
        delete m_vregs[name];
        m_vregs.remove(name);
    }
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


///*
//Bits are separated with ';'
//To set bit name use @name, @field, @bit tags.
//FORMAT:
//   @name=BIT @value=10 @descr=test @{extra_name}={extra data};
// */
//bool Register::parseBit_Method1(const QString &field, int index)
//{
//    bool result = 0;
//    QHash <QString, QString> extras;
//    quint32  field_value=0;
//    bool add_field =false;
//    int lsb=0;
//    int msb=0;
//    bool field_readonly=false;
//    QString field_name,field_description;
//    const QStringList l1 = field.trimmed().split('@',Qt::SkipEmptyParts);
//    foreach(const QString &item, l1)
//    {
//        QRegExp rx1("(\\w+)=(.*)"),rx2("(.*)()");
//        QString par;
//        QString val;
//        if(rx1.indexIn(item) >= 0){
//            par = rx1.cap(1).trimmed();
//            val = rx1.cap(2).trimmed();
//        }
//        else if(rx2.indexIn(item) >= 0)
//        {
//            par = rx2.cap(1).trimmed();
//        }
//        else {

//            continue; // skip this parameter
//        }

//        if(par == "_offset_"){
//            unsigned int offset;
//            if(val.compare("end",Qt::CaseInsensitive)==0){
//                offset = size();
//            }else if(val.compare("begin",Qt::CaseInsensitive)==0){
//                offset = 0;
//            }else{
//                offset = strToUInt(val);
//            }
//            moveOffset(offset);
//            result = true;
//            continue;
//        }
//        else
//            if( par == "register")
//            {
//                /* set registername */
//                result = 1;
//                if(!val.isEmpty())
//                    this->setName(val);
//            }
//            else if( par == "name" || par == "field"|| par == "bit")
//            {
//                BitFieldParser parser;
//                // use standard parser for parameter in order to parse
//                // MSB,LSB
//                if(!parser.load(val.toLatin1().constData()) )
//                {
//                    field_name = QString("_%1").arg(m_chain.size());
//                    lsb=0;
//                    msb=0;
//                }
//                else{
//                    field_name = parser.name();
//                    lsb= parser.lsb();
//                    msb = parser.msb();
//                }

//                // if item doesn't contain field with this name or SameName use allowed
//                // -> this is field lets continue
//                if(!items().contains(field_name) || m_options & AllowSameName )  {
//                    add_field =true;
//                }
//                else {
//                    qWarning()<<"Field already exist or not SameName allowed";
//                }
//            }
//            else if( par == "value")
//            {
//                field_value = strToUInt(val);
//            }
//            else if(par == "descr")
//            {
//                if(add_field)
//                    field_description = val;
//                else {
//                    this->setExtra("descr",val);
//                }
//            }
//            else if( par == "readonly")
//            {
//                // field is not set just a key we assume that it is equal to 1
//                if(val.isEmpty()) {
//                    val = "1";
//                }

//                if(!add_field)
//                {
//                    this->m_readOnly = val.toUInt();
//                }
//                else
//                {
//                    extras[par] =val;
//                    field_readonly = val.toInt();
//                }
//            }
//            else {
//                if(!add_field)
//                {
//                    setExtra(par,val);
//                }
//                //is register
//                else {
//                    extras[par] =val;
//                }
//            }
//    }

//    if(add_field)
//    {
//        // AbsoluteRange
//        if( m_options & AbsoluteRange )
//        {
//            if(lsb >= 0){
//                while(lsb >= msb)
//                {
//                    addSingleBit(new Bit);
//                }
//                for(int i=lsb;i<msb+1;i++){
//                    Bit *pbit = bitAt(i);
//                    pbit->setDescription(field_description);
//                    pbit->setName(field_name);
//                    const int bitn = i - lsb;
//                    pbit->setValue((field_value & (1 << bitn )) != 0);
//                    pbit->setConstant(field_readonly);
//                    foreach(const QString &key, extras.keys()){
//                        pbit->setExtra(key, extras[key]);
//                    }
//                }
//            }
//            extras.clear();
//        }

//        // one by one from LSB
//        else
//        {
//            qint32 count  = (lsb == msb && msb > 0) ?
//                        msb :
//                        (qMax(msb,lsb) - qMin(msb,lsb)) + 1;

//            for (qint32 i = 0; i < count; i++){
//                Bit *pbit = new Bit;
//                pbit->setDescription(field_description);
//                pbit->setName(field_name);
//                pbit->setValue((field_value & (1 << i )) != 0);
//                pbit->setConstant(field_readonly);
//                foreach(const QString &key, extras.keys()){
//                    pbit->setExtra(key, extras[key]);
//                }

//                if(index<0) {
//                    addSingleBit(pbit,-1);
//                }
//                else
//                    addSingleBit(pbit, index+i);
//            }////    cond_update(true);
////    if(!result)
////        WARNING(QString("Register(%1).setValue(%2):Error setValue field not found").arg(name()).arg(field));

////    return result;
////}
//            extras.clear();
//        }
//        result = 1;
//    }
//    return result;
//}

//bool Register::parseBit_Method2(const QString &field, int index)
//{
//    BitFieldParser  parser(field.toLatin1().constData());
//    if (parser.isOk() )
//    {
//        if(m_options & AbsoluteRange && parser.lsb()>=0 && parser.msb()>=0)
//        {
//            while(parser.msb() >= this->size())
//            {
//                addSingleBit(new Bit);
//            }
//            for(int i=parser.lsb();i<parser.msb()+1;i++){
//                Bit *pbit = bitAt(i);
//                pbit->setDescription(parser.description());
//                pbit->setName(parser.name());
//                pbit->setValue((parser.value() & (1 << (i-parser.lsb()) )) != 0);
//                pbit->setConstant(parser.constant());
//            }
//        }
//        else{
//            qint32 count  = (parser.lsb() == parser.msb() && parser.msb() > 0) ?
//                        parser.msb() :
//                        (parser.msb() - parser.lsb()) + 1;
//            //qint32 count  = (msb-lsb) +1;

//            for (qint32 i = 0; i < count; i++)
//            {
//                Bit *pbit = new Bit;
//                pbit->setDescription(parser.description());
//                pbit->setName(parser.name());
//                pbit->setValue((parser.value() & (1 << i)) != 0);
//                pbit->setConstant(parser.constant());
//                if(index<0) addSingleBit(pbit);
//                else addSingleBit(pbit,index+i);
//            }
//        }
//        regroup();
//        return true;
//    }
//    return false;
//}


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
bool Register::addField(const QString &fieldname, qint32 put_to)
{
    int result=0;
    if(!fieldname.isEmpty())
    {
        if(fieldname.trimmed().startsWith('@'))
        {
            //--- result = parseBit_Method1(fieldname, put_to);
        }
        // simple parser
        else
        {
            //---  result = parseBit_Method2(fieldname, put_to);
        }
        emit changed();
    }

    if( !result )
        WARNING(QString("ERROR:%1").arg(fieldname));

    buildReg();
    return result;
}

bool Register::addField(BitField *bitfield)
{
    m_fields.append(bitfield);
    buildReg();
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
 * @function name: Register::appendVirtual()
 * @param:
 *             const QString &name,
 *             const QString &descr
 *              Virtual *pvreg
 * @description: Appends virtual register
 *      Virtual register is register is a part of register (bit/field).
 *      Such register can be accessed using setValue() value() functions.
 * @return: ( bool ) true -success
 ****************************************************************************/
bool Register::appendVirtual(const QString &name, const QString &descr, Virtual *pvreg)
{
    //    if(pvreg && !name.isEmpty() && !contains(name) )
    //    {
    //        pvreg->preg = this;
    //        //if description set in constructor
    //        if(pvreg->descr.isEmpty())  pvreg->descr= descr;
    //        m_vregs[name]= pvreg;
    //        emit changed();
    //        return true;
    //    }
    return false;
}




/****************************************************************************
 * @function name: Register::toString()
 *
 * @param:
 *             void
 * @description: Converts register to string
 * @return: ( const QByteArray)  -returns string
 ****************************************************************************/
const QString Register::toString(const QString &format, bool grouped, bool include_virtual, bool skip_empty)
{
    QString result ;
    //    if(grouped)
    //    {
    //        Register reg;
    //        reg.setSub(1);
    //        QString prev_name;
    //        qint32 range_start=0;
    //        bool unnamed = false;
    //        for(int i=0;i<=size();i++)
    //        {


    //            if((i == size() || prev_name != bitAt(i)->name()) )
    //            {
    //                QString line = format;
    //                QString bit_extras;

    //                if(line.contains(tag_extras) && reg.size()>0){
    //                    int extras_max=0;
    //                    foreach(const QString &name, reg.bitAt(0)->extras()){
    //                        if(!reg.bitAt(0)->extra(name).isEmpty()){
    //                            bit_extras += QString("%1=%2 ").arg(name).arg(reg.bitAt(0)->extra(name));
    //                        }else{
    //                            bit_extras += QString("%1 ").arg(name);
    //                        }
    //                        if(extras_max++>=5) {bit_extras +="...";break;}
    //                    }
    //                }

    //                if( reg.size()>1 )
    //                {
    //                    line.replace(tag_name, QString("%1[%2]").arg(prev_name).arg(reg.size()) )
    //                            .replace(tag_purename,prev_name)
    //                            .replace(tag_value_hex,QString::number(reg.value("[]"),16))
    //                            .replace(tag_value,QString::number(reg.value("[]")))
    //                            .replace(tag_group,QString::number(reg.bitAt(0)->group_id()))
    //                            .replace(tag_bitn,QString("%2:%1 ").arg(range_start).arg(i-1))
    //                            .replace(tag_descr,reg.bitAt(0)->description())
    //                            .replace(tag_extras,bit_extras)
    //                            .replace(tag_readonly, QString::number(reg.bitAt(0)->constant()));

    //                    // replace bit extra with name
    //                    foreach(const QString &extraname, reg.bitAt(0)->extras())
    //                    {
    //                        line.replace("@" + extraname, reg.bitAt(0)->extra(extraname));
    //                    }
    //                }
    //                else if( reg.size() == 1 )
    //                {
    //                    line.replace(tag_purename,prev_name)
    //                            .replace(tag_name, prev_name)
    //                            .replace(tag_value_hex, QString::number(reg.value("[]"),16))
    //                            .replace(tag_value, QString::number(reg.value("[]")))
    //                            .replace(tag_group, QString::number(reg.bitAt(0)->group_id()))
    //                            .replace(tag_bitn, QString("%1 ").arg(range_start))
    //                            .replace(tag_descr,reg.bitAt(0)->description())
    //                            .replace(tag_extras,bit_extras)
    //                            .replace(tag_readonly, QString::number(reg.bitAt(0)->constant()));
    //                    // replace bit extra with name
    //                    foreach(const QString &extraname, reg.bitAt(0)->extras())
    //                    {
    //                        line.replace("@" + extraname, reg.bitAt(0)->extra(extraname));
    //                    }
    //                }


    //                // replace register extra with name
    //                foreach(const QString &extraname, extras())
    //                {
    //                    line.replace("@" + extraname, extra(extraname));
    //                }

    //                //commented allow empty names
    //                if(i != 0)
    //                {
    //                    if(!(skip_empty && unnamed) )
    //                        result += line;
    //                    reg.clear();
    //                    range_start = i;
    //                }
    //            }
    //            if( i<size() )
    //            {
    //                reg.addSingleBit( bitAt(i) );
    //                unnamed = bitAt(i)->name().isEmpty();
    //                if(unnamed)  prev_name = "_";
    //                else prev_name = bitAt(i)->name();
    //            }
    //        }

    //        if(include_virtual)
    //        {
    //            foreach(const QString &name, m_vregs.keys())
    //            {
    //                QString line = format;
    //                line.replace(tag_name,name+"[]")
    //                        .replace(tag_purename,name)
    //                        .replace(tag_value,QString::number(m_vregs[name]->get()))
    //                        .replace(tag_value_hex,QString::number(m_vregs[name]->get(),16))
    //                        .replace(tag_group, "")
    //                        .replace(tag_bitn, "virtual")
    //                        .replace(tag_extras,"")
    //                        .replace(tag_descr,m_vregs[name]->descr);
    //                result += line;
    //            }
    //        }
    //    }
    //    else
    //    {
    //        for (qint32 i = 0; i <size();i++)
    //        {
    //            QString bit_extras = bitAt(i)->extras().join(" ");
    //            QString line = format;
    //            line.replace(tag_name,QString(bitAt(i)->name()))
    //                    .replace(tag_value,QString::number(bitAt(i)->value()))
    //                    .replace(tag_value_hex,QString::number(bitAt(i)->value(),16))
    //                    .replace(tag_group,QString::number(bitAt(i)->group_id()))
    //                    .replace(tag_bitn,QString::number(i))
    //                    .replace(tag_descr,bitAt(i)->description())
    //                    .replace(tag_extras,bit_extras)
    //                    .replace(tag_readonly, QString::number(bitAt(i)->constant())
    //                             );
    //            foreach(const QString &extraname, bitAt(i)->extras())
    //            {
    //                line.replace("@"+extraname,bitAt(i)->extra(extraname));
    //            }
    //            // replace register extra with name
    //            foreach(const QString &extraname, extras())
    //            {
    //                line.replace("@"+extraname,extra(extraname));
    //            }
    //            result += line;
    //        }

    //    }
    return result;
}

bool Register::fromString(const QString &text, const char ln_separator, const char eq_separator)
{
    bool ok=false;
    //    foreach(const QString &line ,text.split(ln_separator,Qt::SkipEmptyParts))
    //    {
    //        QStringList l=line.split(eq_separator);
    //        if(l.size()==2)
    //        {
    //            sub(l[0])->setUInt(l[1].toUInt());

    //        }
    //        else {ok=false;break;}
    //    }
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


/****************************************************************************
 * @function name: Register::addBits()
 * @param:
 *          const QByteArray &bit_string - bits in separated by space or
 *                                          in format bits[5:0]
 * @description:
 * @return: ( void)
 ****************************************************************************/
bool Register::addFieldList(const QStringList &field_list, qint32 put_to)
{
    foreach(const QString &item, field_list)
    {
        if( !addField(item.trimmed(), put_to) )
            return false;
    }
    return true;
}



unsigned int Register::strToUInt(const QString &text, bool *ok)
{
    if(text.startsWith("0x")) return text.mid(2).toUInt(ok,16);
    return text.toUInt(ok,10);
}


/****************************************************************************
 * @function name: Register::setValue()
 *
 * @param:
 *
 *      QByteArray field -field name.
 *                      Format is: (I)"string[nn]" -where nn = number of bit
 *                                  'string' is not requred.this parameter
 *                                  will ignored
 *                                 (II) "string[nn:yy]" - nn:yy is a field
 *      quint32 value  -value is value to pass to a field or bit
 * @description: This function sets value to scanchain
 * @return: ( void )
 ****************************************************************************/
bool Register::setValue(const QString &field, quint32 value)
{
    bool result = false;
    //    const QRegExp rx_fullname("(.*)\\[(.*)\\]");
    //    QString regname;
    //    QString range;
    //    if(rx_fullname.exactMatch(field)){
    //        regname = rx_fullname.cap(1);
    //        range = rx_fullname.cap(2);
    //    }else{
    //        range = field;
    //    }
    //    const char *templates[] = {"\\d+", "(\\d+):(\\d+)", "\\w*",0};
    //    QRegExp rx;
    //    for(int i=0; templates[i]!=0; i++)
    //    {
    //        rx.setPattern( templates[i] );

    //        if(rx.exactMatch(range))
    //        {
    //            switch(i)
    //            {
    //            // format reg[i]
    //            case 0:
    //            {
    //                bool ok;
    //                Bit *pbit = this->bitAt(range.toIsub(nt(&ok));
    //                if(pbit && ok) {
    //                    result = true;
    //                    pbit->setValue(value);
    //                }
    //            }
    //                break;

    //                // format reg[i:j]
    //            case 1:
    //                result = setValue(rx.cap(1).toInt(), rx.cap(2).toInt(),value);
    //                break;

    //                // format reg[field]
    //            case 2:
    //                if(range.isEmpty()){
    //                    blockSignals(1);
    //                    setUInt(value);
    //                    blockSignals(0);
    //                }
    //                else if(this->contains(range))
    //                {
    //                    Register *preg = sub(range);
    //                    preg->setUInt(value);
    //                }
    //                else if(m_vregs.contains(range))
    //                {
    //                    m_vregs[range]->set(value);
    //                    m_vregs[range]->val = value;
    //                }
    //                result = true;
    //                break;
    //            }
    //            break;
    //        }
    //    }
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
quint32 Register::value(const QString &field)
{
    quint32 result=0;
    //    const QRegExp rx_fullname("(.*)\\[(.*)\\]");
    //    QString regname;
    //    QString range;
    //    if(rx_fullname.exactMatch(field)){
    //        regname = rx_fullname.cap(1);
    //        range = rx_fullname.cap(2);
    //    }else{
    //        range = field;
    //    }
    //    const char *templates[] = {"\\d+", "(\\d+):(\\d+)", "\\w*",0};
    //    QRegExp rx;
    //    bool fail = false;
    //    for(int i=0; templates[i]!=0; i++)
    //    {
    //        rx.setPattern( templates[i] );
    //        if(rx.exactMatch(range))
    //        {
    //            switch(i)
    //            {
    //            case 0:
    //            {
    //                bool ok;
    //                Bit *pbit = this->bitAt(range.toInt(&ok));
    //                if(pbit && ok) {
    //                    result =  pbit->value();
    //                }
    //                else fail = true;
    //            }
    //                break;

    //            case 1:{
    //                int lsb = rx.cap(1).toInt();
    //                int msb = rx.cap(2).toInt();
    //                if(lsb<size() && msb <size())
    //                {
    //                    if(lsb<msb) qSwap(lsb,msb);
    //                    result = value(lsb,msb);
    //                }
    //                else fail = true;
    //            }
    //                break;

    //            case 2:
    //                if(range.isEmpty()){
    //                    result = toUInt();
    //                }
    //                else if(this->contains(range))
    //                {
    //                    Register *preg = sub(range);
    //                    if(!preg->isEmpty())
    //                        result = preg->toUInt();
    //                }
    //                else if(m_vregs.contains(range))
    //                {
    //                    result = m_vregs[range]->get();
    //                }
    //                else fail = true;
    //                break;
    //            }
    //            break;
    //        }
    //    }

    //    if(fail)
    //        WARNING(QString("Register(%1).value(%2):Error field not found")
    //                .arg(name()).arg(field));

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
        mp_temporary->append(at(i));
    }

    if(lsb >= size() || msb >= size())
        qWarning()<<QString("Register(%1).sub(%2,%3) - can't find msb, lsb").arg(name()).arg(from).arg(to);
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

    if(mp_temporary->isEmpty() && !m_vregs.contains(fieldname)){
        WARNING(QString("%3Register(%1).sub(\"%2\") - can't find field")
                .arg(name())
                .arg(fieldname)
                .arg(this->isSub()?"SUB_":""));
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
            mp_temporary->addField(fieldname);
        else {
            traceback_list.append(fieldname);
        }
    }

    if(mp_temporary->isEmpty() ){
        WARNING(QString("%3Register(%1).sub(\"%2\") - can't find fields")
                .arg(name())
                .arg(traceback_list.join(","))
                .arg(this->isSub()?"SUB_":""));
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


bool Register::makeSetup(const QString &init_script)
{
    m_setup_backup= init_script;
    return addFieldList(init_script.split(";", Qt::SkipEmptyParts));
}


void Register::setExtra(const QString &name, const QString &value)
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
    return QString();
}

unsigned int Register::extraAsUInt(const QString &name, bool *ok)
{
    return  strToUInt(extra(name).toString() ,ok);
}


Register &Register::operator [](const char *field)
{
    return *sub(field);
}

Register &Register::operator [](const QString &field)
{
    return *sub(field);
}

void Register::makeTemporary()
{
    // make a new sub register
    mp_temporary =  new Register;
    mp_temporary->setName(name());
    mp_temporary->setSub(1);
    mp_temporary->setParentRegister(this);
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
void Register::removeField(const QString &fieldname)
{
    BitField *f = field(fieldname);
    m_fields.removeOne(f);
    delete f;
}

quint32 Register::crc(int bits, quint32 seed, quint32 poly, bool padding, QString *ptext)
{
    Register r_poly(bits+1);
    r_poly.setUInt(poly);
    int sz = r_poly.size();
    quint32 crc=0;
    Register reg(*this);

    // append seed to the end because we assume that we calculate from MSB but register starts from LSB
    reg.addField(QString("seed[%1]=%2").arg(bits).arg(seed));

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

void Register::buildReg()
{
    clear();
    for(int i=0;i<fieldsCount();i++){
        for(int j=0;j<field(i)->size();j++)
            append(field(i)->at(j));
    }
}

const Bit *Register::bitAt(int index)
{
    return at(index);
}



#endif
