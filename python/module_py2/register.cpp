/**
 * @file:register.cpp   -
 * @description: this is class to maintain bits with names
 * @project: BENCH OnSemiconductor
 * @date: 2019\03\07 16-06
 * Version 1.07
 */


#include "register.h"
#include "bitfieldparser.h"
#include <QStringList>

const char Register::TAG_PURENAME[]     = "@purename";
const char Register::TAG_NAME[]     = "@name";
const char Register::TAG_VALUE[]    = "@value";
const char Register::TAG_VALUE_HEX[] = "@value_hex";
const char Register::TAG_GROUP[]    = "@group";
const char Register::TAG_BITN[]     = "@bitn";
const char Register::TAG_DESCR[]    = "@descr";
const char Register::TAG_EXTRAS[]    = "@extras";
const char Register::TAG_READONLY[]    = "@readonly";






/****************************************************************************
 * copy constructor
 *
 *     const Register &reg - register
 *
 * @description: creates register with bits without names
 ****************************************************************************/
Register::Register( const Register &reg)
{
    m_options = AllowSameName;
    m_isSub = false;
    m_readOnly = false;
    mp_temporary= NULL;
    m_update_policy = UpdateOnChange;
    copy(this,const_cast<Register*> (&reg));
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
{
    m_options = AllowSameName;
    m_isSub = false;
    m_readOnly = false;
    m_name = name;
    mp_temporary= NULL;
    m_update_policy = UpdateOnChange;    
    makeSetup(init_script);    
}


/****************************************************************************
 * @function name: constructor
 *
 * @param:
 *
 *  void
 * @description:
 ****************************************************************************/
Register::Register()
{
    m_options = AllowSameName;
    m_isSub = false;
    m_readOnly = false;
    mp_temporary= NULL;
    m_update_policy = UpdateOnChange;
}

/****************************************************************************
 * @function name: constructor
 *
 * @param:
 *
 *  qint32 bit_count
 * @description: creates register with bits without names
 ****************************************************************************/
Register::Register(qint32 bit_count, const QString  &name , quint32 options)
{
    m_options = options;
    m_isSub = false;
    m_readOnly = false;
    mp_temporary= NULL;
    m_update_policy = UpdateOnChange;
    resize(bit_count);
    m_name = name;
}


/****************************************************************************
 * @function name: Register::addBit()
 *
 * @param:
 *
 *          Bit *pbit
 * @description: Adds bit by pointer
 * @return: ( void )
 ****************************************************************************/
void Register::addBit(Bit *pbit, qint32 put_to)
{
    if(put_to <0)
    {
        m_chain.append(pbit);
    }
    else
    {
        m_chain.insert(put_to,pbit);
    }
}

void Register::removeBit(Bit *pbit)
{
    if(pbit)
    {
        int i = m_chain.indexOf(pbit);
        if(i>=0)
            delete m_chain.takeAt(i);
    }
}

/****************************************************************************
 * @function name: Register::findMaxGroupId()
 *
 * @param:
 *
 * @description: Finds maximum group id number in chain
 * @return: ( qint32 )
 ****************************************************************************/
qint32 Register::findMaxGroupId()
{
    qint32 result =0;
    for(qint32 i=0;i<m_chain.size();i++)
    {
        if(m_chain.at(i)->group_id()>result)
        {
            result = m_chain.at(i)->group_id();
        }
    }
    return result;
}


/*
Bits are separated with ';'
To set bit name use @name, @field, @bit tags.
FORMAT:
   @name=BIT @value=10 @descr=test @{extra_name}={extra data};
 */
bool Register::parseBit_Method1(const QString &field, int index)
{
    bool result = 0;    
    QHash <QString, QString> extras;
    quint32  field_value=0;
    bool is_field =false;
    int lsb=0;
    int msb=0;
    bool field_readonly=false;
    QString field_name,field_description;
    QStringList l1 = field.trimmed().split('@',QString::SkipEmptyParts);
    foreach(const QString &item, l1)
    {
        QRegExp rx1("(\\w+)=(.*)"),rx2("(.*)()");
        QString par;
        QString val;
        if(rx1.indexIn(item) >= 0){         
            par = rx1.cap(1).trimmed();
            val = rx1.cap(2).trimmed();
        }
        else if(rx2.indexIn(item) >= 0)
        {            
            par = rx2.cap(1).trimmed();
        }
        else {

            continue; // skip this parameter
        }


        if( par == "name" || par == "field"|| par == "bit")
        {
            BitFieldParser parser;
            // use standard parser for parameter in order to parse
            // MSB,LSB
            if(!parser.load(val.toLatin1().constData()) )
            {
                field_name = QString("_%1").arg(m_chain.size());
                lsb=0;
                msb=0;
            }
            else{
                field_name = parser.name();
                lsb= parser.lsb();
                msb = parser.msb();
            }

            // if item doesn't contain field with this name or SameName use allowed
            // -> this is field lets continue
            if(!items().contains(field_name) || m_options & AllowSameName )  {
                is_field =true;
            }            
        }
        else if( par == "value")
        {            
            field_value = strToUInt(val);
        }
        else if(par == "descr")
        {
            if(is_field)
                field_description = val;
            else {
                this->setExtra("descr",val);
            }
        }
        else if( par == "register")
        {
            /* set registername */
            result = 1;
            if(!val.isEmpty())
                this->setName(val);
        }
        else if( par == "readonly")
        {
            // field is not set just a key we assume that it is equal to 1
            if(val.isEmpty()) {
                val = "1";
            }

            if(!is_field)
            {
                this->m_readOnly = val.toUInt();
            }
            else
            {
                extras[par] =val;
                field_readonly = val.toInt();
            }
        }
        else {
            if(!is_field)
            {
                setExtra(par,val);
            }
            //is register
            else {
                extras[par] =val;
            }
        }
    }

    if(is_field)
    {
        // AbsoluteRange
        if( m_options & AbsoluteRange )
        {
            if(lsb >= 0){
                while(lsb >= msb)
                {
                    addBit(new Bit);
                }
                for(int i=lsb;i<msb+1;i++){
                    Bit *pbit = bitAt(i);
                    pbit->setDescription(field_description);
                    pbit->setName(field_name);
                    const int bitn = i - lsb;
                    pbit->setValue((field_value & (1 << bitn )) != 0);
                    pbit->setConstant(field_readonly);
                    foreach(const QString &key, extras.keys()){
                        pbit->setExtra(key, extras[key]);
                    }
                }
            }
            extras.clear();
        }

        // one by one from LSB
        else
        {
            qint32 count  = (lsb == msb && msb > 0) ?
                        msb :
                        (qMax(msb,lsb) - qMin(msb,lsb)) + 1;

            for (qint32 i = 0; i < count; i++){
                Bit *pbit = new Bit;
                pbit->setDescription(field_description);
                pbit->setName(field_name);
                pbit->setValue((field_value & (1 << i )) != 0);
                pbit->setConstant(field_readonly);
                foreach(const QString &key, extras.keys()){
                    pbit->setExtra(key, extras[key]);
                }
                if(index<0) addBit(pbit,-1);
                else  addBit(pbit, index+i);
            }
            extras.clear();
        }
        result = 1;
    }    
    return result;
}

bool Register::parseBit_Method2(const QString &field, int index)
{
    BitFieldParser  parser(field.toLatin1().constData());
    if (parser.isOk() )
    {
        if(m_options & AbsoluteRange && parser.lsb()>=0 && parser.msb()>=0)
        {
            while(parser.msb() >= this->size())
            {
                addBit(new Bit);
            }
            for(int i=parser.lsb();i<parser.msb()+1;i++){
                Bit *pbit = bitAt(i);
                pbit->setDescription(parser.description());
                pbit->setName(parser.name());
                pbit->setValue((parser.value() & (1 << (i-parser.lsb()) )) != 0);
                pbit->setConstant(parser.constant());
            }
        }
        else{
            qint32 count  = (parser.lsb() == parser.msb() && parser.msb() > 0) ?
                        parser.msb() :
                        (parser.msb() - parser.lsb()) + 1;
            //qint32 count  = (msb-lsb) +1;

            for (qint32 i = 0; i < count; i++)
            {
                Bit *pbit = new Bit;
                pbit->setDescription(parser.description());
                pbit->setName(parser.name());
                pbit->setValue((parser.value() & (1 << i)) != 0);
                pbit->setConstant(parser.constant());
                if(index<0) addBit(pbit);
                else addBit(pbit,index+i);
            }
        }
        regroup();        
        return true;
    }
    return false;
}
/****************************************************************************
 * @function name: Register::addBit()
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
bool Register::addBit(const QString &bitname, qint32 put_to)
{    
    int result=0;
    if(!bitname.isEmpty())
    {
        if(bitname.trimmed().startsWith('@'))
        {
            result = parseBit_Method1(bitname, put_to);
        }
        // simple parser
        else
        {
            result = parseBit_Method2(bitname, put_to);
        }
        regroup();
    }
    
    return result;
}


/****************************************************************************
 * @function name: Constructor
 *
 * @param:
 *             void
 * @description:
 * @return: (  )
 ****************************************************************************/
Register::~Register()
{    
    clear();
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
                    Bit *pbit = this->bitAt(range.toInt(&ok));
                    if(pbit && ok) {
                            result = true;
                            pbit->setValue(value);
                    }
                }
                break;

                // format reg[i:j]
                case 1:
                    result = setValue(rx.cap(1).toInt(), rx.cap(2).toInt(),value);
                break;

              // format reg[field]
                case 2:                                       
                        if(range.isEmpty()){                            
                            setUInt(value);                            
                        }
                        else if(this->contains(range))
                        {
                            Register *preg = sub(range);
                            preg->setUInt(value);
                        }
                        else if(m_vregs.contains(range))
                        {
                            m_vregs[range]->set(value);
                            m_vregs[range]->val = value;
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
 * @function name: Register::setValue()
 *
 * @param:
 *      quint32 from - index from
 *      quint32 to - index to
 *      quint32 value - value is value to pass to a field or bit
 * @description: This function sets value to scanchain
 * @return: ( void )
 ****************************************************************************/
bool Register::setValue(quint32 from, quint32 to,quint32 value)
{
    bool upd=false;
    bool result = false;
    quint32 i;
    quint32 j = 0;

    if(from >to )
        qSwap(from,to);

    if(to >= (quint32)size())
        to = size()-1;

    for (i = from; i <= to; i++)
    {
        bool cur_bit = (bool)((value >> j) & 1);
        upd |= (bit(i) != cur_bit);
        setBit(i,cur_bit);
        j++;
        result = true;
    }
    
    
    return result;
}

void Register::regroup()
{
    int group_id=-1;
    QString previous;
    for(int i=0;i<m_chain.count();i++)
    {
        if (m_chain.at(i)->name() != previous) group_id ++;
        m_chain.at(i)->setGroup_id(group_id);
        previous = m_chain.at(i)->name();
    }
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
                    Bit *pbit = this->bitAt(range.toInt(&ok));
                    if(pbit && ok) {                        
                            result =  pbit->value();
                    }
                    else fail = true;
                }
                break;

            case 1:{
                    int lsb = rx.cap(1).toInt();
                    int msb = rx.cap(2).toInt();
                    if(lsb<size() && msb <size())
                    {
                        if(lsb<msb) qSwap(lsb,msb);
                        result = value(lsb,msb);
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
                    else if(m_vregs.contains(range))
                    {
                        result = m_vregs[range]->get();
                    }
                    else fail = true;
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
 *  qint32 from
 *  qint32 to
 *
 * @description: thisfunction returns value of field or single bit
 * @return: ( quint32 ) returns value
 ****************************************************************************/
quint32 Register::value(qint32 from, qint32 to)
{
    quint32 result = 0;
    qint32 i, j = 0;
    if(from <size() && to <size())
    {
        if(from >to )
            qSwap(from,to);
        for (i = from; i <= to; i++)
        {
            if ( i > m_chain.size() - 1 ) break;
            result |= (static_cast<bool>(m_chain.at(i)->value()) << j);
            j++;
        }
    }
    return result;
}

/****************************************************************************
 * @function name: Register::toUInt()
 *
 * @param:
 *      BitOrder bitorder
 * @description: converts register to unsigned 32bit unsigned int
 * @return: ( quint32 ) returns value
 ****************************************************************************/
quint32 Register::toUInt(BitOrder bitorder)
{    
    quint32 result =0;
    qint32 i;
    for (i = 0; i <m_chain.size(); i++)
    {
        qint32 index;
        if(bitorder == MSB)
        {
            index = m_chain.size()-1-i;
        }
        else index= i;
        result |= (static_cast<bool>(m_chain.at(index)->value()) << i);
    }
    return result;
}

/****************************************************************************
 * @function name: Register::setUInt()
 *
 * @param:
 *      quint32 value
 *      BitOrder bitorder
 * @description: sets value to register as unsigned int 32 bit
 * @return: ( void )
 ****************************************************************************/
void Register::setUInt(quint32 value, BitOrder bitorder)
{
    bool upd = false;
    qint32 i;
    qint32 j = 0;
    if(isEmpty()) {        
        return;
    }
    for (i = 0; i < m_chain.size(); i++)
    {
        qint32 index;
        if(bitorder == MSB)
        {
            index = m_chain.size()-1-i;
        }
        else index= i;
        bool cur_bit = (bool)((value >> j) & 1);
        upd |= bit(index)!=cur_bit;     
        setBit(index, cur_bit);
        j++;
    }    
}


/****************************************************************************
 * @function name: Register::value()
 *
 * @param:
 *      qint32 field_number
 * @description: this function returns value of single bit
 * @return: ( quint32 ) returns value
 ****************************************************************************/
quint32 Register::value(qint32 bit_number)
{
    return bit(bit_number);
}


/****************************************************************************
 * @function name: Register::setBit()
 *
 * @param:
 *      qint32 bitn -bit number to set
 *      bool value  - bit value
 * @description: This function sets bit by its number
 * @return: ( void )
 ****************************************************************************/
bool Register::setBit(qint32 bitn, bool value)
{
    if ( (bitn >= 0) && (bitn < m_chain.size()) && m_chain.at(bitn))
    {
        m_chain.at(bitn)->setValue(value);
        return true;
    }    
    return false;
}


/****************************************************************************
 * @function name: Register::setBit()
 *
 * @param:
 *      const QByteArray &bitname -bit name to set
 *      bool value  - bit value
 * @description: This function sets bit by its number
 * @return: ( void )
 ****************************************************************************/
bool Register::setBit(const QString &bitname, bool value)
{
    bool result = false;
    if ( !bitname.isEmpty())
    {
        qint32 index = findBitByName(bitname);
        if (index >= 0 && index < m_chain.size())
        {
            Bit *pbit = m_chain.at(index);
            pbit->setValue(value);
            result = true;
        }
    }
    return result;
}

/****************************************************************************
 * @function name: Register::bit()
 *
 * @param:
 *      const QByteArray &bit_name
 * @description: this function gets bit
 * @return: ( bool ) - bit value
 ****************************************************************************/

Bit *Register::bit(const char *bit_name)
{    
    int bitn= this->findBitByName(bit_name);
    if(bitn>=0)
    {
        return this->m_chain[bitn];
    }    
     return NULL;
}

Bit *Register::bit(const QString &bit_name)
{
    return bit(bit_name.toLatin1().constData());
}
/****************************************************************************
 * @function name: Register::bit()
 *
 * @param:
 *      qint32 bitn
 * @description: this function gets bit
 * @return: ( bool ) - bit value
 ****************************************************************************/
bool Register::bit(qint32 bitn)
{    
    if(bitn<m_chain.size())
        return (bool)bitAt(bitn)->value();
    return false;
}


/****************************************************************************
 * @function name: Register::bitAt()
 *
 * @param:
 *      qint32 bitn - bit number
 * @description: tis function returns pointer to a bit
 * @return: (Bit *) - bit pointer
 ****************************************************************************/
Bit *Register::bitAt(qint32 bitn)
{
    if(bitn<m_chain.size())
        return m_chain.at(bitn);
    return 0;
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
    int bitn = findBitByName(name);
    return bitn>=0;
}

/****************************************************************************
 * @function name: Register::join()
 *
 * @param:
 *      const Register &reg
 * @description: this joins (appends ) to the end
 *          if register is subregister - function will append just pointer(s) of bit
 * @return: (Bit *) - bit pointer
 ****************************************************************************/
void Register::join(Register &reg)
{
    int i;
    for (i=0;i< reg.size();i++)
    {
        if(this->isSub())
        {
            addBit(reg.bitAt(i));
        }
        else {
            addBit(new Bit(*reg.bitAt(i)));
        }
    }
}

void Register::join(Register *preg)
{
    int i;
    for (i=0;i< preg->size();i++)
    {
        if(this->isSub())
        {
            addBit(preg->bitAt(i));
        }
        else {
            addBit(new Bit(*preg->bitAt(i)));
        }
    }
}



/****************************************************************************
 * @function name: Register::toByteArray()
 *
 * @param:
 *        BitOrder bitorder
 *
 * @description: This function converts current scanchain to bytearray
 * @return: (QByteArray ) - returns number of bits in bytearray
 ****************************************************************************/
QByteArray Register::toByteArray(BitOrder bitorder,bool contiguously)
{        
    QByteArray bytearray;
    quint8 byte = 0;
    for (qint32 byte_i = 0; byte_i < size(); )
    {
        quint8 bit;
        qint32 index = byte_i;
        if(bitorder == MSB)
        {
            index = size() - 1 - byte_i;
        }
        bit = (m_chain.at(index)->value() != 0);
        if(contiguously)
        {
            byte |= (bit<< (7-(byte_i % 8)));
        }
        else{
            byte |= (bit<<    (byte_i % 8));
        }
        byte_i++;
        if ( (byte_i % 8) == 0 || byte_i >= size() )
        {
            bytearray.append(byte);
            byte = 0;
        }
    }
    return bytearray;
}

QString Register::toHex(Register::BitOrder bitorder, bool contiguously)
{
    return QString(toByteArray(bitorder,contiguously).toHex());
}


/****************************************************************************
 * @function name: Register::fromByteArray()
 *
 * @param:
 *       const QByteArray &bytearray - data array
 *       qint32 scanchain_length - length in bits
 *       BitOrder bitorder (default = LSB)
 *       bool contiguously  bits go like this 7.6.5..1.0.7.
 *
 * @description: This function populates scan chain by values from bytearray
 *
 *              !!!if register is Empty (size ==0)
 *                      will be resized and then populated
 * @return: ( bool ) -if success
 ****************************************************************************/
bool Register::fromByteArray(const QByteArray &bytearray
                             , qint32 scanchain_length
                             , BitOrder bitorder
                             ,bool contiguously
                             )
{
    bool result=false;
    qint32 bit_i = 0;
    qint32 byte_i;
    quint8 byte = bytearray[0];
    if(scanchain_length == -1) scanchain_length=bytearray.size() * 8;
    if( size() == 0 )
    {
        resize(scanchain_length);
    }
    if (( bytearray.size() * 8) >= scanchain_length)
    {
        int j=-1;

        for (qint32 i = 0; i < scanchain_length; i++)
        {
            if(i>=size()) break;

            byte_i = i / 8;
            bit_i = i % 8;
            byte = bytearray[byte_i];
            quint8 bit;
            if(contiguously)
            {
                bit = (byte >> (7-bit_i)) & 1;
            }
            else {
                bit = (byte >> bit_i) & 1;
            }

            switch(bitorder)
            {
                case LSB:
                    setBit(i , bit);
                break;

                case MSB:
                    setBit(size()-1-i , bit);
                break;

                case MSB8:
                    setBit((8*(i/8+1)-i%8-1),bit);
                    break;

                case MSBITEM:
                if( bitAt(i)!=0)
                {
                        QString item = bitAt(i)->name();
                        if(sub(item)->size() ==0)  break;
                        if( sub(item)->bitAt(0) == bitAt(i)){
                            j = sub(item)->size()-1;
                        }
                        if(sub(item)->bitAt(j) !=0 )
                            sub(item)->setBit(j,bit);
                        j--;
                    }
                    break;

            }

        }
        result=true;         
    }    

    return result;
}

bool Register::fromHex(const QString &hex, qint32 scanchain_length, Register::BitOrder bitorder, bool contiguously)
{
    return fromByteArray(QByteArray::fromHex(hex.toLatin1()), scanchain_length, bitorder, contiguously);
}


/****************************************************************************
 * @function name: Register::toBitString()
 *
 * @param:
 *
 *        BitOrder bitorder
 * @description: This function converts current scanchain to bit string
 *                "101111" (MSB...LSB)
 * @return: ( QByteArray ) - returns number of bits in bytearray
 ****************************************************************************/
QByteArray Register::toBitString(BitOrder bitorder)
{
    QByteArray bytearray;
    quint8 byte = 0;
    qint32 i = 0;
    for (i=0;i<m_chain.size();i++)
    {
        if(bitorder ==MSB)
        {
            byte = (quint8)m_chain.at(m_chain.size()-1-i)->value();
        }
        else
        {
            byte = (quint8)m_chain.at(i)->value();
        }
        bytearray.append(QByteArray::number(byte, 10));
    }
    return bytearray;
}



/****************************************************************************
 * @function name: Register::fromBitStringToUint()
 *
 * @param:
 *
 *       onst QByteArray & ba  - data array
 * @description: This function converts current bit string to uint
 *               ByteArray("x7....x0")
 *               spaces or '.' as separators are available
 * @return: ( qint32 ) - returns value
 ****************************************************************************/
quint32 Register::fromBitStringToUint(const QByteArray & ba)
{
    quint32 result = 0;
    quint32 i = ba.size() - 1;
    qint32 bitn = 0;
    while(bitn<ba.size())
    {
        char c = ba.at(i);
        if(c == '1')
        {
            result |= 1 << bitn;
        }
        i--;
        if(c ==' ' || c == '.')
        {
            continue;
        }
        bitn++;
    }
    return result;
}

/****************************************************************************
 * @function name: Register::fromBitString()
 *
 * @param:
 *       QByteArray &bytearray  - datat to load
 *       BitOrder bitorder
 *
 * @description: This function loads "101111" (MSB...LSB)  text
 *               to current scanchain
 * @return: ( void )
 ****************************************************************************/
void Register::fromBitString(const QByteArray &bytearray, BitOrder bitorder)
{
    for (qint32 i = 0; i < bytearray.size(); i++)
    {
        qint32 index;
        if(bitorder == MSB)
        {
            index = bytearray.size() - i - 1;
        }
        else index = i;
        setBit( index, bytearray[i] != '0');
    }
}






/****************************************************************************
 * @function name: Register::clear()
 *
 * @param:
 *             void
 * @description: clear bits from chain. Deletes bits
 * @return: ( void )
 ****************************************************************************/
void Register::clear()
{
    qint32 bits_count = m_chain.size();
    while ( bits_count--)
    {
        Bit *pbit = m_chain.first();
        m_chain.removeFirst();
        if(! isSub())
            delete pbit;
    }
    foreach(const QString &name, m_vregs.keys())
    {
        delete m_vregs[name];
        m_vregs.remove(name);
    }
}


/****************************************************************************
 * @function name: Register::readOnly()
 *
 * @param:
 *            void
 * @description:
 * @return: ( bool )  -returns true if readonly
 ****************************************************************************/
bool Register::readOnly() const
{
    return m_readOnly;
}


/****************************************************************************
 * @function name: Register::setReadOnly()
 *
 * @param:
 *             bool
 * @description: -set true if readonly
 * @return: ( void)
 ****************************************************************************/
void Register::setReadOnly(bool on)
{
    m_readOnly = on;
}

Register &Register::operator [](const char *field)
{
    return *sub(field);
}

Register &Register::operator [](const QString &field)
{
    return *sub(field);
}

/****************************************************************************
 * @function name: Register::copy()
 *
 * @param:
 *             bool
 * @description: -copies register. Copies same register or if
 *                register is zero size
 * @return: ( bool ) true -success
 ****************************************************************************/
bool Register::copy(Register *pdst, Register *psrc)
{
    bool result =false;
    qint32 src_size =psrc->size();

    if( pdst->isSame(psrc) )
    {
        for (qint32 i = 0; i <src_size; i++)
        {
            pdst->setBit(i,psrc->bit(i));
        }
        result = true;
    }
    else if(pdst->m_chain.size() == 0 )
    {
        pdst->resize(src_size);
        for (qint32 i = 0; i < src_size; i++)
        {
            *(pdst->m_chain.at(i)) = *(psrc->bitAt(i));
        }
        result = true;
    }
    return result;
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
    if(pvreg && !name.isEmpty() && !contains(name) )
    {
        pvreg->preg = this;
        //if description set in constructor
        if(pvreg->descr.isEmpty())  pvreg->descr= descr;
        m_vregs[name]= pvreg;
        return true;
    }
    return false;
}

QStringList Register::items()
{
    QStringList list;
    for(int i=0;i<this->m_chain.count();i++){
        const QString &name = m_chain[i]->name();
        if(!list.contains(name) ) list.append(name);
    }
    return list;
}





/****************************************************************************
 * @function name: Register::operator =()
 *
 * @param: Register
 *
 * @description: copies register. IF register is similar (contains the same bits),
 *              it will just copy values. if register is not similar and is zero size
 *              function will make a copy of given register
 ****************************************************************************/
//[1]

Register &Register::operator =(const Register &reg)
{
    copy(this,const_cast<Register*>(&reg));    
    return *this;
}

/****************************************************************************
 * @function name: Register::operator !()
 *
 * @param: Register
 *
 * @description: operator not
 * @return: ( Register )  - returns not value
 ****************************************************************************/

Register Register::operator !()
{
    Register r= *this;
    for (qint32 i = 0; i < size(); i++)
    {
        r.bitAt(i)->setValue(!r.bitAt(i)->value());
    }
    
    return r;
}

//[2]
/****************************************************************************
 * @function name: Register::operator =()
 *
 * @param: quint32
 *
 * @description: fills register with value. doesnot change register structure
 * @return: ( qint32 )  -returns number of bits in scanchain
 ****************************************************************************/
Register &Register::operator = (quint32 val)
{
    bool upd=false;
    if(32 >= this->size())
    {
        for (qint32 i = 0; i < size(); i++)
        {            
            upd |= (bit(i) != ((val>>i)&1));
            setBit(i,(val>>i)&1);
        }
    }

    return *this;
}




/****************************************************************************
 * @function name: Register::operator &=
 *
 * @param:
 *              Register &reg)
 * @description:
 * @return: ( Bit * )  - returns pointer to bit
 ****************************************************************************/
//[1]
Register &Register::operator &= (Register &reg)
{
    if (this->size() == reg.size())
    {
        for (qint32 i = 0; i < size(); i++)
        {
            bool result = m_chain.at(i)->value() & reg.bitAt(i)->value();
            m_chain.at(i)->setValue(result);
        }
    }    
    return *this;
}
//[2]
Register &Register::operator &=(quint32 val)
{
    if(32 == this->size())
    {
        for (qint32 i = 0; i < size(); i++)
        {
            bool result = (bool)m_chain.at(i)->value() & (bool)((val>>i)&1);
            m_chain.at(i)->setValue(result);
        }
    }
    
    return *this;
}


/****************************************************************************
 * @function name: Register::operator ^=
 *
 * @param:
 *
 * @description:
 * @return: ( Bit * )  - returns pointer to bit
 ****************************************************************************/
//[1]
Register &Register::operator ^= (Register &reg)
{
    if (this->size() == reg.size())
    {
        for (qint32 i = 0; i < size(); i++)
        {
            bool result = m_chain.at(i)->value() ^ reg.bitAt(i)->value();
            m_chain.at(i)->setValue(result);
        }
    }    
    return *this;
}
//[2]
Register &Register::operator ^= (quint32 val)
{
    if(32 == this->size())
    {
        for (qint32 i = 0; i < size(); i++)
        {
            bool result = m_chain.at(i)->value() ^ ((val>>i)&1);
            m_chain.at(i)->setValue(result);
        }
    }    
    return *this;
}


/****************************************************************************
 * @function name: Register::operator |=
 *
 * @param:
 *
 * @description:
 * @return: ( Bit * )  - returns pointer to bit
 ****************************************************************************/
//[1]
Register &Register::operator |= (Register &reg)
{
    if (this->size() == reg.size())
    {
        for (qint32 i = 0; i < size(); i++)
        {
            bool result = m_chain.at(i)->value() | reg.bitAt(i)->value();
            m_chain.at(i)->setValue(result);
        }
    }
    return *this;
}
//[2]
Register &Register::operator |= (quint32 val)
{
    if(32 >= this->size())
    {
        for (qint32 i = 0; i < size(); i++)
        {
            bool result = m_chain.at(i)->value() | (bool)((val>>i)&1);
            m_chain.at(i)->setValue(result);
        }
    }
    return *this;
}


/****************************************************************************
 * @function name: Register::operator ==
 *
 * @param:
 *          Register &reg)
 * @description:
 * @return: ( bool )
 ****************************************************************************/
bool Register::operator == (Register &reg)
{
    bool result = false;
    if (size() == reg.size())
    {
        result = true;
        for (qint32 i = 0; i < size(); i++)
        {
            if (m_chain.at(i)->value() != reg.bitAt(i)->value())
            {
                result = false;
                break;
            }
        }
    }
    return result;
}


/****************************************************************************
 * @function name: Register::operator ==
 *
 * @param:
 *              quint32 val
 * @description:
 * @return: ( bool )
 ****************************************************************************/
bool Register::operator == (quint32 val)
{
    bool result ;
    result = true;
    for (qint32 i = 0; i < size(); i++)
    {
        if (m_chain.at(i)->value() != (bool)((val>>i)&1) )
        {
            result = false;
            break;
        }
    }
    return result;
}


/****************************************************************************
 * @function name: Register::operator !=
 *
 * @param:
 *              Register &reg
 * @description: operator not equal
 * @return: ( bool )
 ****************************************************************************/
bool Register::operator != (Register &reg)
{
    return !(operator ==(reg));
}


/****************************************************************************
 * @function name: Register::operator !=
 *
 * @param:
 *              quint32 vl
 * @description: operator not equal
 * @return: ( bool )
 ****************************************************************************/
bool Register::operator != (quint32 val)
{
    return !(operator ==(val));
}



/****************************************************************************
 * @function name: Register::fill()
 *
 * @param:
 *          bool value - value of bit
 *          qint32 count    -count of bit from start
 *          qint32 start - start bit number
 * @description: fills register bits with value from start
 * @return: ( void )
 ****************************************************************************/
void Register::fill(bool value, qint32 count, qint32 start)
{
    if (count == -1)
    {
        count = size();
    }
    for (qint32 i = start; i < count; i++)
    {
        setBit(i, value);
    }
}

void Register::rotateRight(int count, bool fill)
{
    while( count-- ){
        bool v = fill;
        for(int i=0; i<m_chain.count(); i++){
            bool tmp = m_chain.at(i)->value();
            m_chain.at(i)->setValue(v);
            v = tmp;
        }
    }
}

void Register::rotateLeft(int count, bool fill)
{
    while( count-- ){
        bool v = fill;
        for(int i=m_chain.count()-1; i>=0; i--){
            bool tmp = m_chain.at(i)->value();
            m_chain.at(i)->setValue(v);
            v = tmp;
        }
    }
}

/****************************************************************************
 * @function name: Register::invert()
 *
 * @description: inverts register
 * @return: ( void )
 ****************************************************************************/
void Register::invert()
{
    for (qint32 i = 0; i < size(); i++)
    {
        m_chain.at(i)->setValue(!m_chain.at(i)->value());
    }
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
    QByteArray result ;
    if(grouped)
    {
        Register reg;
        reg.setSub(1);
        QString prev_name;
        qint32 range_start=0;
        bool unnamed = false;
        for(int i=0;i<=size();i++)
        {
            if((i == size() || prev_name != bitAt(i)->name()) )
            {
                QString line = format;
                QString bit_extras;

                if(line.contains(TAG_EXTRAS) && reg.size()>0){
                        int extras_max=0;
                        foreach(const QString &name, reg.bitAt(0)->extras()){
                            if(!reg.bitAt(0)->extra(name).isEmpty()){
                                bit_extras += QString("%1=%2 ").arg(name).arg(reg.bitAt(0)->extra(name));
                            }else{
                                bit_extras += QString("%1 ").arg(name);
                            }
                            if(extras_max++>=5) {bit_extras +="...";break;}
                        }
                }

                if( reg.size()>1 )
                {             
                    line.replace(TAG_NAME, QString("%1[%2]").arg(prev_name).arg(reg.size()) )
                            .replace(TAG_PURENAME,prev_name)
                            .replace(TAG_VALUE_HEX,QString::number(reg.value("[]"),16))
                            .replace(TAG_VALUE,QString::number(reg.value("[]")))                            
                            .replace(TAG_GROUP,QString::number(reg.bitAt(0)->group_id()))
                            .replace(TAG_BITN,QString("%2:%1 ").arg(range_start).arg(i-1))
                            .replace(TAG_DESCR,reg.bitAt(0)->description())
                            .replace(TAG_EXTRAS,bit_extras)
                            .replace(TAG_READONLY, QString::number(reg.bitAt(0)->constant()));

                    // replace bit extra with name
                    foreach(const QString &extraname, reg.bitAt(0)->extras())
                    {
                        line.replace("@" + extraname, reg.bitAt(0)->extra(extraname));
                    }
                }
                else if( reg.size() == 1 )
                {                    
                    line.replace(TAG_PURENAME,prev_name)
                            .replace(TAG_NAME, prev_name)
                            .replace(TAG_VALUE_HEX, QString::number(reg.value("[]"),16))
                            .replace(TAG_VALUE, QString::number(reg.value("[]")))
                            .replace(TAG_GROUP, QString::number(reg.bitAt(0)->group_id()))
                            .replace(TAG_BITN, QString("%1 ").arg(range_start))
                            .replace(TAG_DESCR,reg.bitAt(0)->description())
                            .replace(TAG_EXTRAS,bit_extras)
                            .replace(TAG_READONLY, QString::number(reg.bitAt(0)->constant()));
                    // replace bit extra with name
                    foreach(const QString &extraname, reg.bitAt(0)->extras())
                    {
                        line.replace("@" + extraname, reg.bitAt(0)->extra(extraname));
                    }
                }


                // replace register extra with name
                foreach(const QString &extraname, extras())
                {
                    line.replace("@" + extraname, extra(extraname));
                }

               //commented allow empty names
               if(i != 0)
                {
                   if(!(skip_empty && unnamed) )
                    result += line;
                    reg.clear();
                    range_start = i;
                }
            }
            if( i<size() )
            {
                reg.addBit( bitAt(i) );
                unnamed = bitAt(i)->name().isEmpty();
                if(unnamed)  prev_name = "_";
                else prev_name = bitAt(i)->name();
            }
        }

        if(include_virtual)
        {
            foreach(const QString &name, m_vregs.keys())
            {
                QString line = format;
                    line.replace(TAG_NAME,name+"[]")
                        .replace(TAG_PURENAME,name)
                        .replace(TAG_VALUE,QString::number(m_vregs[name]->get()))
                        .replace(TAG_VALUE_HEX,QString::number(m_vregs[name]->get(),16))
                        .replace(TAG_GROUP, "")
                        .replace(TAG_BITN, "virtual")
                        .replace(TAG_EXTRAS,"")
                        .replace(TAG_DESCR,m_vregs[name]->descr);
                result += line;
            }
        }
    }
    else
    {
        for (qint32 i = 0; i <size();i++)
        {
            QString bit_extras = bitAt(i)->extras().join(" ");
            QString line = format;
            line.replace(TAG_NAME,QString(bitAt(i)->name()))
                    .replace(TAG_VALUE,QString::number(bitAt(i)->value()))
                    .replace(TAG_VALUE_HEX,QString::number(bitAt(i)->value(),16))
                    .replace(TAG_GROUP,QString::number(bitAt(i)->group_id()))
                    .replace(TAG_BITN,QString::number(i))
                    .replace(TAG_DESCR,bitAt(i)->description())
                    .replace(TAG_EXTRAS,bit_extras)
                    .replace(TAG_READONLY, QString::number(bitAt(i)->constant())
                             );
            foreach(const QString &extraname, bitAt(i)->extras())
            {
                line.replace("@"+extraname,bitAt(i)->extra(extraname));
            }
            // replace register extra with name
            foreach(const QString &extraname, extras())
            {
                line.replace("@"+extraname,extra(extraname));
            }
            result += line;
        }
    }
    return result;
}

bool Register::fromString(const QString &text, const char ln_separator, const char eq_separator)
{
    bool ok=false;
    foreach(const QString &line ,text.split(ln_separator,QString::SkipEmptyParts))
    {
        QStringList l=line.split(eq_separator);
        if(l.size()==2)
        {
             sub(l[0])->setUInt(l[1].toUInt());

        }
        else {ok=false;break;}
    }
    return ok;
}


/****************************************************************************
 * @function name: Register::findBitByName - ---
 *
 * @param:
 *
 *  const QByteArray & name
 * @description:
 * @return: ( qint32 ) returns index of bit in scanchain
 ****************************************************************************/
qint32 Register::findBitByName(const QString &name)
{
    for (qint32 i = 0; i < m_chain.size(); i++)
    {
        if (m_chain.at(i)->name() == name )
        {
            return i;
        }
    }
    return -1 ;
}

int Register::findBit(Bit *pbit)
{
    return m_chain.indexOf(pbit);
}


/****************************************************************************
 * @function name: Register::addBits()
 * @param:
 *          const QByteArray &bit_string - bits in separated by space or
 *                                          in format bits[5:0]
 * @description:
 * @return: ( void)
 ****************************************************************************/
bool Register::addBits(const QString &bit_string, qint32 put_to)
{
    QStringList l = bit_string.split(";",QString::SkipEmptyParts);
    foreach(const QString &item, l)
    {
       if( !addBit(item.trimmed(), put_to) ) return false;
    }
    return true;
}


/****************************************************************************
 * @function name: Register::resize()
 * @param:
 *      qint32 size -size of register
 * @description: resizes register if size of current register is higher than
 *                  size it will be cleared
 * @return: ( void )
 ****************************************************************************/
void Register::resize(qint32 size_need)
{
    if(size_need >size())
    {
        size_need -= size();
        for (qint32 i = 0; i < size_need; i++)
        {
            addBit(new Bit("_"));
        }
    }
}


/****************************************************************************
 * @function name: Register::setName()
 * @param:
 *      const QString &name - register name
 * @description: sets name of register
 * @return: ( void )
 ****************************************************************************/
void Register::setName(const QString &name)
{
    m_name =  name;
}

unsigned int Register::strToUInt(const QString &text, bool *ok)
{
    if(text.startsWith("0x")) return text.mid(2).toUInt(ok,16);
    return text.toUInt(ok,10);
}



/****************************************************************************
 * @function name: Register::makeSubRegister()
 * @param:
 *      Register *preg - preg to make it as sub
 *      qint32 group_id - gropup id of new reg
 * @description: makes subregister from register passed by pointer (preg)
 * @return: ( void )
 ****************************************************************************/
void Register::makeSubRegister(Register *preg, qint32 group_id)
{
    if(preg)
    {
        preg->clear();
        preg->setSub(true);
        for (qint32 i = 0; i < m_chain.size(); i++)
        {
            if (m_chain.at(i)->group_id() == group_id)
            {
                preg->addBit( m_chain.at(i) );
            }
        }
    }
}

/****************************************************************************
 * @function name: Register::makeSubRegister()
 * @param:
 *      Register *preg - preg to make it as sub
 *      qint32 group_id - gropup id of new reg
 * @description: makes subregister from register passed by pointer (preg)
 * @return: ( void )
 ****************************************************************************/
void Register::makeSubRegister(Register *preg, qint32 from, qint32 to)
{
    if(preg)
    {
        preg->clear();
        preg->setSub(true);
        if(to >= m_chain.size() || to<0) to = m_chain.size()-1;
        for (qint32 i = from; i <= to; i++)
        {
            preg->addBit( m_chain.at(i) );
        }
    }
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
    if(msb<0 || msb >= m_chain.size()) msb = m_chain.size()-1;

    for (i = lsb; i <= msb; i++)
    {
        mp_temporary->addBit(m_chain.at(i));
    }
    
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
Register * Register::sub(const QString &bitname)
{
    if(mp_temporary == NULL) {
         makeTemporary();
    }
    mp_temporary->clear();
    if(bitname.contains(';'))
    {
        sub(bitname.split(';'));
    }
    else {        
        for(int i = 0; i < m_chain.size(); i++)
        {
            if ( m_chain.at(i)->name() == bitname )
            {
                mp_temporary->addBit(m_chain.at(i));
            }
        }
    }
    
    return mp_temporary;
}

Register *Register::sub(const QStringList &bits)
{
    if(mp_temporary == NULL) {
         makeTemporary();
    }    
    mp_temporary->clear();
    qint32 lsb=-1,msb=-1;
    foreach(const QString &bitname, bits)
    {
        QRegExp rx("((\\d+):(\\d+))|(\\d+)");
        if(rx.exactMatch(bitname) )
        {
            rx.indexIn(bitname);
            if(rx.cap(1).isEmpty())
            {
                lsb = msb= rx.cap(4).toUInt();
            }
            else {
                quint32 r1 = rx.cap(2).toUInt();
                quint32 r2 = rx.cap(3).toUInt();
                lsb = qMin(r1,r2);
                msb = qMax(r1,r2);
            }
            for(qint32 i=lsb;i<=msb;i++)
            {
                if(i>=0 && i <size()) mp_temporary->addBit(m_chain.at(i));
            }
        }
        else
        {
            for(qint32 i = 0; i < m_chain.size(); i++)
            {
                if ( m_chain.at(i)->name() == bitname )
                {
                    mp_temporary->addBit(m_chain.at(i));
                }
            }
        }
    }
    return mp_temporary;
}

Register * Register::sub(const QString &extra_name, const QString &extra_value)
{
    if(mp_temporary == NULL) {
        makeTemporary();
    }
    mp_temporary->clear();
    for(qint32 i = 0; i < m_chain.size(); i++)
    {
        if ( m_chain.at(i)->extra(extra_name) == extra_value )
        {
            mp_temporary->addBit(m_chain.at(i));
        }
    }
    return mp_temporary;
}

Register * Register::sub(const QString &extra_name, const QStringList &extra_values)
{
    if(mp_temporary == NULL) {
        makeTemporary();
    }
    mp_temporary->clear();
    foreach(const QString &extra_value,extra_values)
    {
        for(qint32 i = 0; i < m_chain.size(); i++)
        {
            if (  m_chain.at(i)->extra(extra_name) == extra_value )
            {
                mp_temporary->addBit(m_chain.at(i));
            }
        }
    }
    return mp_temporary;
}
/****************************************************************************
 * @function name: Register::setSub()
 * @param:
 *      bool on - on sub mode
 * @description: setup register as sub register.
 *                   !!! sub register doesnot care about  bits on delete
 * @return: ( void )
 ****************************************************************************/
void Register::setSub(bool on)
{
    m_isSub = on;
}


/****************************************************************************
 * @function name: Register::isSub()
 * @description: returns true if register is sub register
 * @return: ( bool )
 ****************************************************************************/
bool Register::isSub() const
{
    return m_isSub;
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
    bool result = false;
    if(preg && preg->size() == this->size())
    {
        for (qint32 i = 0; i < m_chain.size(); i++)
        {
            Bit *pbit1 = m_chain.at(i);
            Bit *pbit2 = preg->bitAt(i);
            result = (pbit1->name() == pbit2->name() );
            if(result == false) break;
        }
    }
    return result;
}

/****************************************************************************
 * @function name: Register::isEmpty()
 * @param:
 * @description: register is empty . has no bits in it
 * @return: ( bool )
 ****************************************************************************/
bool Register::isEmpty() const
{
    return m_chain.size()==0;
}


/****************************************************************************
 * @function name: Register::scaleBitArray()
 * @param:
 *          const QByteArray &data_in
 *          qint32 factor - scale factor
 * @description: this function scales bit array
 *              Example: input 10011010, factor = 2
 *                      output 1100001111001100
 * @return: ( QByteArray )  output array
 ****************************************************************************/
QByteArray Register::scaleBitArray(const QByteArray &data_in, qint32 factor)
{
    QByteArray result;
    if(factor>0)
    {
        for(int i=0;i<data_in.size();i++)
        {
            char bit = data_in.at(i);
            for (int j=0;j<factor;j++) result.append(bit);
        }
    }
    return result;
}

/****************************************************************************
 * @function name: Register::scaleByteArray()
 * @param:
 *          const QByteArray &data_in
 *          qint32 factor - scale factor
 * @description: this function scales byte array by bit
 *              Example: input bytes hex : f0 01  , factor = 2
 *                      output bytes: ff 00 00 03
 *                      (in this example function will double each bit and return
 *                          it as byte array. SizeOut = SizeIn*factor )
 * @return: ( QByteArray ) result array
 ****************************************************************************/
QByteArray Register::scaleByteArray(const QByteArray &data_in,  qint32 factor)
{
    QByteArray result;
    if(factor>0)
    {
        result.resize(data_in.size()*factor);
        int j=0,i=0;
        quint8 mask1,mask2;
        quint8 byte;
        while(j<(data_in.size()*8*factor))
        {
            i += (j%factor)==0 && j>0;
            mask1 = 1<<(7-i%8);
            mask2 = 1<<(7-j%8);
            byte = result[j/8];
            if((quint8)data_in.at(i/8) &mask1)
                byte|= mask2;
            else
                byte &= ~mask2;
            result[j/8] = byte;
            j++;
        }
    }
    return result;
}

/****************************************************************************
 * @function name: convertByteArrayToBitArray( )
 * @param:
 *        const QByteArray &data_in
 *        qint32 size_in_bits
 *        BitOrder bitorder
 * @description: 
 * @return: ( quint32 )
 ****************************************************************************/
QByteArray Register::convertByteArrayToBitArray(const QByteArray &data_in
							, qint32 size_in_bits,BitOrder bitorder )
{
    QByteArray result;
    result.resize(size_in_bits);
    qint32 bit_i = 0;
    qint32 byte_i;
    quint8 byte = data_in[0];

    if (data_in.size() * 8 >= size_in_bits)
    {
        for (qint32 i = 0; i < size_in_bits; i++)
        {
            byte_i = i / 8;
            bit_i = i % 8;
            byte = data_in[byte_i];
            quint8 bit = (byte >> bit_i) & 1;
            if(bitorder == LSB)
            {
                result[i] = bit?'1':'0';
            }
            else
            {
                result[size_in_bits-1-i]=bit?'1':'0';
            }
        }
    }
    return result;
}

QByteArray Register::convertBitArrayToByteArray(const QByteArray &data_in,BitOrder bitorder ,bool contiguously)
{
    QByteArray result;
    qint32 size_in_bits = data_in.size();
    quint8 byte = 0;
    for (qint32 byte_i = 0; byte_i < size_in_bits; )
    {
        quint8 bit;
        qint32 index = byte_i;
        if(bitorder == MSB)
        {
            index = size_in_bits - 1 - byte_i;
        }
        bit = (data_in[index]=='1');
        if(contiguously)
        {
            byte |= (bit<< (7-(byte_i % 8)));
        }
        else{
            byte |= (bit<<    (byte_i % 8));
        }
        byte_i++;
        if ( ((byte_i % 8) == 0) || (byte_i >= size_in_bits) )
        {
            result.append(byte);
            byte = 0;
        }
    }
    return result;
}


quint32 Register::crc(int bits, quint32 seed, quint32 poly, bool padding, QString *ptext)
{    
    Register r_poly(bits+1);
    r_poly.setUInt(poly);
    int sz = r_poly.size();
    quint32 crc=0;
    Register reg(*this);

    // append seed to the end because we assume that we calculate from MSB but register starts from LSB
    reg.addBits(QString("seed[%1]=%2").arg(bits).arg(seed));

    // insert bits count according to CRC size 8,16,32...
    if(padding)
        reg.addBits(QString("padding[%1]=0").arg(bits),0);

    //only for debugging purposes
    QString space;
    if(ptext){
            ptext->append(reg.toBitString(Register::MSB)+" | "+r_poly.toBitString(Register::MSB)+"\n");
    }

    for(int i=0; (i+sz) <= reg.size(); i++)
    {

        if(reg.bit(reg.size()-i-1) !=0) {
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





bool Register::makeSetup(const QString &init_script)
{
    m_setup_script = init_script;
    return addBits(init_script);
}

const QString Register::getSetup()
{
    return m_setup_script;
}

void Register::setExtra(const QString &name, const QString &value)
{
    m_extra[name] = value;
}

QStringList Register::extras() const
{
    return m_extra.keys();
}

void Register::applyValueFromExtra(const QString &extra_name)
{
    for(int i=0;i<this->size();i++)
    {              
            bool ok = false;
            quint32 v = bitAt(i)->extra(extra_name).toUInt(&ok);
            if(ok)
                bitAt(i)->setValue(v);
    }
}

void Register::bind(QObject *pobj, const char *Set, const char *Get)
{    
    if(mp_temporary == NULL){
        makeTemporary();        
    }
}

void Register::setUpdatePolicy(Register::UpdatePolicy upd_pol)
{
    m_update_policy = upd_pol;
}

void Register::setParentRegister(Register *preg)
{
    mp_parent = preg;
}

Register::UpdatePolicy Register::updatePolicy() const
{
    if(isSub()) return mp_parent->updatePolicy();
    else return m_update_policy;
}

QString Register::extra(const QString &name)
{
    if(m_extra.contains(name))
    {
        return  m_extra[name];
    }
    return QString();
}

unsigned int Register::extraAsUInt(const QString &name, bool *ok)
{
    return  strToUInt(extra(name) ,ok);
}


Register *Register::roll()
{
    QByteArray tmp = toBitString();
    this->fromBitString(tmp,Register::MSB);
    return this;
}

Register::operator int()
{
    return toUInt();
}

Register::operator unsigned int()
{
    return toUInt();
}

Register::operator bool()
{
     return toUInt();
}

void Register::makeTemporary()
{
    mp_temporary =  new Register;
    mp_temporary->setSub(1);    
    mp_temporary->setName(name());
    mp_temporary->setParentRegister(this);
}

void Register::addTemporary(Bit *pbit)
{
    if(mp_temporary)
        mp_temporary->addBit(pbit);
}

void Register::clearTemporary()
{
    mp_temporary->clear();
}

/****************************************************************************
 * @function name: Register::removeBitByName(const QString &name)
 * @param:
 *        const QString &name 
 * @description: 
 * @return: ( quint32 )
 ****************************************************************************/
void Register::removeBitByName(const QString &name)
{
    Bit *pbit=0;
    do{
        pbit = bit(name.toLatin1().constData());
        if(pbit) removeBit(pbit);
    }while(pbit);
}


