#include "bitfieldparser.h"
#include <QDebug>

#define IS_ALPHA(C) ((C >= 'a' && C <= 'z') || (C>='A' && C<='Z'))

enum { NAME = 0, RANGEA , RANGEB, VALUE, DESCR , STOP, COUNT };


BitFieldParser::BitFieldParser()
{
    init();
}

BitFieldParser::BitFieldParser(const char *pfield)
{
    init();
    m_ok =  load(pfield);
}

void BitFieldParser::init()
{
    m_range_msb = 0;
    m_range_lsb = 0;
    m_value = 0;
    has_double_range = false;
    has_range = false;
    has_value = false;
    value_readonly = false;
    bit_by_name = false;
    m_ok = false;
    range_all = false;
}

bool BitFieldParser::load(const char *pfield)
{

    qint32 next_area = NAME;
    qint32 area = NAME;
    has_double_range = false;
    has_range = false;
    has_value = false;
    bit_by_name = false;
    qint32 j = 0;
    int i=0;
    for (i = 0; (i < 10000 && area != STOP) ; i++)
    {
        switch (pfield[i])
        {
        case ('['):
            next_area = RANGEA;
            has_range = true;
            break;
        case (':'):
            has_double_range = true;
            next_area = RANGEB;
            break;
        case (']'):
            break;
        case ('='):
            next_area = VALUE;
            has_value = true;
            break;
        case ('#'):
            next_area = DESCR;
            break;
        case ('\0'):
            if(i==0) return false;
            next_area = STOP;
            break;
        default :
            data[j] = pfield[i];
            j++;
            break;
        }
        if (next_area >= area)
        {
            qint32 count = next_area - area;
            while ( count > 0)
            {
                data[j++] = '\0';
                count--;
            }
            area = next_area;
        }
        else
        {
            qWarning() << "ERROR parsing field" << pfield;
            return false;
        }

    }

    const char *p_ra = field(RANGEA);
    const char *p_rb = field(RANGEB);
    char c = *p_ra;
    bit_by_name = IS_ALPHA(c);
    if (has_range && !bit_by_name)
    {
        // "[]"
        if (*p_ra == '\0')
        {
            m_range_lsb = 0;
            m_range_msb = 10000;
            range_all = true;
        }
        // "[A:B]"
        else if (has_double_range)
        {
            m_range_lsb = atoi(p_ra);
            m_range_msb = atoi(p_rb);
            if (m_range_lsb > m_range_msb)
            {
                qint32 tmp = m_range_msb;
                m_range_msb = m_range_lsb;
                m_range_lsb = tmp;
            }
        }
        // "[A]"
        else
        {
            m_range_lsb = m_range_msb =  atoi(p_ra);
        }
    }
    if (has_value)
    {
        QString str(field(VALUE));
        if(str.startsWith("$"))
        {
            value_readonly = true;
            str.remove(0,1);
        }

        if(str.startsWith("0x"))
        {
            m_value = str.toUInt(0,16);
        }
        else {
            m_value =str.toUInt();
        }
    }
    return true;
}

const char * BitFieldParser::field(qint32 n)
{
    const char *pdata = data;
    while (n)
    {
        if (*pdata++ == '\0') { n--; }
    }
    return pdata;
}

const char* BitFieldParser::name(){    return field(NAME);}
bool BitFieldParser::rangeIsName() const{    return bit_by_name;}

const char* BitFieldParser::range_name(){    return field(RANGEA);}
qint32 BitFieldParser::value() const{    return m_value;}
bool BitFieldParser::constant() const { return value_readonly;}

const char*  BitFieldParser::description(){    return field(DESCR);}

const bool BitFieldParser::hasDoubleRange()const{    return has_double_range;}

qint32 BitFieldParser::lsb() const{    return m_range_lsb;}
qint32 BitFieldParser::msb() const{    return m_range_msb;}
bool BitFieldParser::isOk() const{    return m_ok;}
