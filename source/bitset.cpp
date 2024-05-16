#include "bitset.h"
#include <QtEndian>

BitSet::BitSet(const QString &name, int size, bool owner):
    QList<Bit*>()
   ,m_bits_owner(owner)
{
    m_name = name;
    m_constant = false;
    if(size != -1)
        resize(size);
}

BitSet::BitSet(BitSet &bitset)
    :m_bits_owner(1)
{    
    setName(bitset.name());
    copy(bitset);
}

BitSet::~BitSet()
{
    clear();
}


void BitSet::resize(qint32 size_need)
{        
    while(size_need>size())
    {
        append(new Bit(false));
    }
}


bool BitSet::isSame(BitSet *preg)
{
    return (preg!=0 && preg->size() == size());
}


BitSet::operator int()
{
    return toUInt();
}

BitSet::operator unsigned int()
{
    return toUInt();
}

BitSet::operator bool()
{
    return toUInt();
}

const QString &BitSet::name() const
{
    return m_name;
}

void BitSet::setName(const QString &name)
{
    m_name = name;
}

void BitSet::clear()
{
    if(isBitsOwner()){
        while ( size() )
        {
            delete takeAt(0);
        }
    }
    QList::clear();
}

quint32 BitSet::value()
{
    return toUInt(LSB);
}

void BitSet::setValue(const quint32 value)
{
    setUInt(value,LSB);
}

void BitSet::invert()
{
    for (qint32 i = 0; i < size(); i++)
    {
        at(i)->value = !at(i)->value;
    }
}

bool BitSet::setValue(qint32 from, qint32 to, quint32 value)
{
    bool value_changed=false;
    bool result = false;
    qint32 i;
    qint32 j = 0;

    if(to >= size())
        to = size()-1;

    if(indexValid(from) && indexValid(to))
    {
        if(from >to )
            qSwap(from,to);

        for (i = from; i <= to; i++)
        {
            bool cur_bit = (bool)((value >> j) & 1);
            value_changed |= (bitValue(i) != cur_bit);
            at(i)->value = cur_bit;
            j++;
        }
        result = true;
    }
    return result;
}

void BitSet::roll()
{
    fromBitString(toBitString(), BitSet::MSB);
}


quint32 BitSet::value(qint32 from, qint32 to, bool *p_ok)
{
    quint32 result = 0;
    qint32 i, j = 0;
    if(indexValid(from) && indexValid(to))
    {
        if(from >to )
            qSwap(from,to);

        for (i = from; i <= to; i++)
        {
            if ( i > size() - 1 ) break;
            result |= (static_cast<bool>(at(i)->value) << j);
            j++;
        }
        if(p_ok) *p_ok = true;
    }
    else{
        if(p_ok) *p_ok = false;
    }
    return result;
}

quint32 BitSet::toUInt(BitOrder bitorder)
{
    quint32 result =0;
    qint32 i;
    for (i = 0; i <size(); i++)
    {
        qint32 index;
        if(bitorder == MSB)
        {
            index = size()-1-i;
        }
        else index= i;
        result |= (static_cast<bool>(at(index)->value) << i);
    }
    return result;
}

void BitSet::setUInt(quint32 value, BitOrder bitorder)
{
    bool value_changed = false;
    qint32 i;
    qint32 j = 0;

    if(!isEmpty()) {
        for (i = 0; i < size(); i++)
        {
            qint32 index;
            if(bitorder == MSB)
            {
                index = size()-1-i;
            }
            else index= i;
            bool cur_bit = (bool)((value >> j) & 1);
            value_changed |= (at(index)->value != cur_bit);
            at(index)->value = cur_bit;
            j++;
        }
    }
}

bool BitSet::setBitValue(qint32 bitn, bool value)
{
    if ( indexValid(bitn) )
    {
        at(bitn)->value = value;
        return true;
    }
    return false;
}

bool BitSet::indexValid(int index)
{
    return (index >=0) && (index<size());
}


void BitSet::rotateRight(int count, bool fill)
{
    while( count-- ){
        bool v = fill;
        for(int i=0; i<size(); i++){
            bool tmp = at(i)->value;
            at(i)->value = v;
            v = tmp;
        }
    }
}

void BitSet::rotateLeft(int count, bool fill)
{
    while( count-- ){
        bool v = fill;
        for(int i=size()-1; i>=0; i--){
            bool tmp = at(i)->value;
            at(i)->value = v;
            v = tmp;
        }
    }
}

//[1]

BitSet &BitSet::operator =(BitSet &bitset)
{        
    copy(bitset);
    return *this;
}


BitSet BitSet::operator !()
{
    BitSet r= *this;
    for (qint32 i = 0; i < size(); i++)
    {
        r.at(i)->value = !r.at(i)->value;
    }
    return r;
}

//[2]
BitSet &BitSet::operator = (quint32 val)
{
    bool value_changed=false;
    if(32 >= this->size())
    {
        for (qint32 i = 0; i < size(); i++)
        {
            value_changed |= (bitValue(i) != ((val>>i)&1));
            setBitValue(i,(val>>i)&1);
        }
    }

    return *this;
}

//[1]
BitSet &BitSet::operator |= (BitSet &reg)
{
    if (this->size() == reg.size())
    {
        for (qint32 i = 0; i < size(); i++)
        {
            bool result = at(i)->value || reg.at(i)->value;
            at(i)->value = result;
        }
    }
    return *this;
}
//[2]
BitSet &BitSet::operator |= (quint32 val)
{
    if(32 >= this->size())
    {
        for (qint32 i = 0; i < size(); i++)
        {
            bool result = at(i)->value | (bool)((val>>i)&1);
            at(i)->value = result;
        }
    }
    return *this;
}

bool BitSet::operator == (BitSet &reg)
{
    bool result = false;
    if (size() == reg.size())
    {
        result = true;
        for (qint32 i = 0; i < size(); i++)
        {
            if (at(i)->value != reg.at(i)->value)
            {
                result = false;
                break;
            }
        }
    }
    return result;
}

bool BitSet::operator == (quint32 val)
{
    bool result = true;
    for (qint32 i = 0; i < size(); i++)
    {
        if (at(i)->value != (bool)((val>>i)&1) )
        {
            result = false;
            break;
        }
    }
    return result;
}


bool BitSet::operator != (BitSet &reg)
{
    return !(operator ==(reg));
}

bool BitSet::operator [](int index)
{
    return at(index)->value;
}


bool BitSet::operator != (quint32 val)
{
    return !(operator ==(val));
}

//[1]
BitSet &BitSet::operator ^= (BitSet &reg)
{
    if (size() == reg.size())
    {
        for (qint32 i = 0; i < size(); i++)
        {
            bool result = at(i)->value ^ reg.at(i)->value;
            at(i)->value = result;
        }
    }
    return *this;
}
//[2]
BitSet &BitSet::operator ^= (quint32 val)
{
    if(32 == size())
    {
        for (qint32 i = 0; i < size(); i++)
        {
            bool result = at(i)->value ^ ((val>>i)&1);
            at(i)->value = result;
        }
    }
    return *this;
}

//[1]
BitSet &BitSet::operator &= (BitSet &reg)
{
    if (size() == reg.size())
    {
        for (qint32 i = 0; i < size(); i++)
        {
            bool result = at(i)->value && reg.at(i)->value;
            at(i)->value = result;
        }
    }
    return *this;
}
//[2]
BitSet &BitSet::operator &=(quint32 val)
{
    if(32 == size())
    {
        for (qint32 i = 0; i < size(); i++)
        {
            bool result = (bool)at(i)->value & (bool)((val>>i)&1);
            at(i)->value = result;
        }
    }
    return *this;
}

bool BitSet::bitValue(qint32 index, bool *p_ok)
{
    if( indexValid(index) ){
        if(p_ok) *p_ok = true;
        return (bool)at(index)->value;
    }
    if(p_ok) *p_ok =false;
    return false;
}

void BitSet::fill(bool value, qint32 start, qint32 count)
{
    if (count == -1 || (count +start) >size())
    {
        count = size()-start;
    }
    for (qint32 i = 0; i < count; i++)
    {
        at(start + i)->value = value;
    }
}

void BitSet::join(BitSet &reg)
{
    int i;
    for (i=0;i< reg.size();i++)
    {
        if(!isBitsOwner())
        {
            append(reg.at(i));
        }
        else {
            // adds a copy
            append(new Bit(*reg.at(i)));
        }
    }
}

void BitSet::join(BitSet *preg)
{
    int i;
    for (i=0;i< preg->size();i++)
    {
        if(!isBitsOwner())
        {
            append(preg->at(i));
        }
        else {
            append(new Bit(*preg->at(i)));
        }
    }
}
#include <QDebug>
QByteArray BitSet::toByteArray(BitOrder bitorder, Endianess endianess)
{
    QByteArray bytearray;
    quint8 byte = 0;    
    for (qint32 i = 0; i < size()+1; i++)
    {
        const bool new_byte = ((i % 8) == 0 &&i>0);
        if ( new_byte || i==size())
        {
            bytearray.append(1,byte);
            byte = 0;
        }

        qint32 index;
        switch(bitorder){
        case MSB:
            index = size() - 1 - i;
            break;
        case LSB:
            index = i;
            break;
        case MSB8:
            index = (8*(i/8+1)-i%8-1);
            break;
        }

        quint8 bit=0;
        if( indexValid(index) )
            bit = (at(index)->value != 0);        

        byte |= (bit<<    (i % 8));
    }    

    switch(endianess){
    case ENDIAN_LITTLE:
        // no chage
        break;
    case ENDIAN_BIG_16BIT:
        // TBD
        break;
    case ENDIAN_BIG_32BIT:
        // TBD
        break;
    }

    return bytearray;
}

bool BitSet::fromByteArray(const QByteArray &bytearray
                           , qint32 length_bits
                           , BitOrder bitorder
                           , Endianess endianess
                           )
{
    bool result=false;
    switch(endianess){
    case ENDIAN_LITTLE:
        // no chage
        break;
    case ENDIAN_BIG_16BIT:
        // TBD
        break;
    case ENDIAN_BIG_32BIT:
        // TBD
        break;
    }

    if(length_bits == -1 || length_bits>size() ) length_bits = size();

    if(bytearray.size()>0){
        qint32 bit_i = 0;
        for(int i=0;i<bytearray.size();i++){
            const quint8 byte = bytearray[i];
            bool stop =false;

            for(int j=0;j<8;j++){
                quint8 bit = (byte >> j) & 1;
                int index;
                switch(bitorder)
                {
                case LSB:
                    index = bit_i;
                    break;

                case MSB:
                    index = size()-1-bit_i;
                    break;

                case MSB8:
                    index = (8*(bit_i/8+1)-bit_i%8-1);
                    break;
                }

                setBitValue(index, bit );
                bit_i++;
                if(bit_i>length_bits) {stop=1;break;}
            }
            if(stop) break;
        }
        result = true;
    }
    return result;
}

bool BitSet::fromHex(const QString &hex, qint32 length_bits
                     , BitSet::BitOrder bitorder, Endianess endianess)
{
    return fromByteArray(QByteArray::fromHex(hex.toLatin1()), length_bits, bitorder, endianess);
}

QString BitSet::toHex(BitSet::BitOrder bitorder, Endianess endianess)
{
    return QString(toByteArray(bitorder,endianess).toHex());
}


QByteArray BitSet::toBitString(BitOrder bitorder)
{
    QByteArray bytearray;
    quint8 byte = 0;
    qint32 i = 0;
    for (i=0;i<size();i++)
    {
        if(bitorder == MSB)
        {
            byte = (quint8)at(size()-1-i)->value;
        }
        else
        {
            byte = (quint8)at(i)->value;
        }
        bytearray.append(QByteArray::number(byte, 10));
    }
    return bytearray;
}


quint32 BitSet::fromBitStringToUint(const QByteArray & ba)
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

void BitSet::fromBitString(const QByteArray &bytearray, BitOrder bitorder)
{    
    for (qint32 i = 0; i < bytearray.size(); i++)
    {
        qint32 index;
        if(bitorder == MSB)
        {
            index = bytearray.size() - i - 1;
        }
        else index = i;
        const bool new_value = bytearray[i] != '0';
        if(bitValue(index) != new_value){
           //--- m_value_changed = true;
        }
        setBitValue( index, new_value);
    }
}

QByteArray BitSet::convertByteArrayToBitArray(const QByteArray &data_in
                                                    ,qint32 size_in_bits
                                                    ,BitOrder bitorder )
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

QByteArray BitSet::convertBitArrayToByteArray(const QByteArray &data_in, BitOrder bitorder, bool contiguously)
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

void BitSet::copy(BitSet &bitset)
{    
    if(isBitsOwner()){
        resize(bitset.size());
        //copy values
        for(int i=0;i<size();i++){
            at(i)->value = bitset.at(i)->value;
        }
    }
    else {
        clear();
        //add bits fromanother bitset
        for(int i=0;i<bitset.size();i++){
            append(bitset.bitAt(i));
        }
    }
}


Bit *BitSet::bitAt(int index)
{
    return at(index);
}
