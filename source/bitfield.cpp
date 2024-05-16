/**
 * @file:bit.cpp   -
 * @description: <in header>
 * @project: BENCH OnSemiconductor
 * @date:
 *
 */
#include <QList>
#include "bitfield.h"
#include <QDebug>

BitField::BitField(const QString &name, int size)
    : BitSet(name, size, false)
{
}

BitField::~BitField()
{
}

void BitField::setDescription(const QString &descr)
{
    m_description = descr;
}

const QString &BitField::description() const
{
    return m_description;
}

void BitField::setExtra(const QString &name, const QVariant &value)
{
    m_extra[name] = value;
}

void BitField::setExtras(const QHash<QString, QVariant> &extras)
{
    foreach (const QString &name, extras.keys())
    {
        m_extra[name] = extras[name];
    }
}

BitField *BitField::makeField(const QString &str, BitField::Parser *parser, BitField *bitfield)
{
    bool parser_ok = false;
    if (parser == 0)
    {
        parser = new BitField::Parser;
        parser_ok = parser->load(str.toLatin1().constData());
    }
    else
    {
        parser_ok = parser->is_ok();
    }

    if (parser_ok)
    {
        int bit_count = parser->bits_count();

        if (bit_count > 0)
        {
            if (bitfield == 0)
                bitfield = new BitField(parser->name());

            // inflate with bits
            while (bit_count--)
            {
                bitfield->append(new Bit(false));
            }
            if (parser->has_value())
                bitfield->setValue(parser->value());

            if (parser->is_value_constant())
                bitfield->setConstant(true);
        }
    }
    // field should be deleted by user
    return bitfield;
}

QVariant BitField::extra(const QString &name)
{
    if (m_extra.contains(name))
    {
        return m_extra[name];
    }
    return QString();
}

QStringList BitField::extras() const
{
    return m_extra.keys();
}

static quint32 StrToUl(const char *str)
{
    quint32 result;
    if (str[0] == '0' && str[1] == 'x')
        result = (quint32)strtol(&str[2], 0, 16);
    else
        result = (quint32)strtol(str, 0, 10);
    return result;
}

bool BitField::Parser::load(const char *pfield, bool absolute_range)
{
    Area next_area = NAME;
    Area area = NAME;
    int len = 0;
    clear();
    const char *p_in = pfield;
    char *p_cap = m_captured[NAME];
    bool error = false;
    m_ok = false;
    while (*p_in && !error)
    {
        const char c = *p_in++;
        switch (c)
        {
            case ('['):
                next_area = RANGEA;
                m_has_range++;
                p_cap = m_captured[RANGEA];
                break;

            case (':'):
                m_has_range++;
                next_area = RANGEB;
                p_cap = m_captured[RANGEB];
                break;

            case (']'):
                break;

            case ('='):
                next_area = VALUE;
                m_has_value = true;
                p_cap = m_captured[VALUE];
                break;

            case (' '):
                break;

            default:
                if (p_cap)
                {
                    if (checkAreaCorrect(c, len, area))
                    {
                        *p_cap++ = c;
                        len++;
                    }
                    else
                    {
                        error = true;
                    }
                }
                break;
        }

        if (next_area >= area)
        {
            len = 0;
            area = next_area;
        }
        else
        {
            error = true;
            break;
        }
    }
    if (error)
    {
        qWarning() << "ERROR parsing field" << pfield;
        return false;
    }

    // Range
    const char rng_a_1symb = m_captured[RANGEA][0];
    const char rng_b_1symb = m_captured[RANGEB][0];
    m_by_name = ((rng_a_1symb >= 'a' && rng_a_1symb <= 'z') || (rng_a_1symb >= 'A' && rng_a_1symb <= 'Z')) && rng_a_1symb != 'B';

    if (!m_by_name)
    {        
        // "[A:B]"
        if (m_has_range == 2 && rng_a_1symb != '\0' && rng_b_1symb != '\0')
        {
            m_range_lsb = StrToUl(m_captured[RANGEA]);
            m_range_msb = StrToUl(m_captured[RANGEB]);

            if (m_range_lsb < 0 || m_range_msb < 0)
                return false;

            // maybe swap MSB <>LSB
            if (m_range_lsb > m_range_msb)
            {
                qint32 tmp = m_range_msb;
                m_range_msb = m_range_lsb;
                m_range_lsb = tmp;
            }
            m_bits_count = m_range_msb - m_range_lsb + 1;
        }
        // "[A]"
        else if (m_has_range == 1 && rng_a_1symb != '\0')
        {
            // in bytes
           if(rng_a_1symb == 'B'){
                m_bits_count =8;
                const int cnt = StrToUl(&m_captured[RANGEA][1]);
                if(cnt>0) m_bits_count *= cnt;
           }
           else{
                if (absolute_range)
                {
                    m_bits_count = 1;
                    m_range_lsb = StrToUl(m_captured[RANGEA]);
                }
                else
                {
                    m_bits_count = StrToUl(m_captured[RANGEA]);
                }
            }
        }
        else
        {
            m_bits_count = 1;
        }
    }
    /// Value
    if (m_has_value)
    {
        char *p = m_captured[VALUE];
        if (p[0] == '$')
        {
            m_value_readonly = true;
            p++;
        }

        m_value_u32 = StrToUl(p);
    }
    m_ok = true;
    return true;
}

void BitField::Parser::clear()
{
    m_ok = false;
    m_range_msb = -1;
    m_range_lsb = -1;
    m_value_u32 = 0;
    m_has_range = 0;
    m_has_value = false;
    m_value_readonly = false;
    m_by_name = false;
    m_bits_count = 0;
    memset(m_captured, '\0', sizeof(m_captured));
}

BitField::Parser::Parser(const char *p, bool absolute_range)
{
    clear();
    if (p)
        m_ok = load(p, absolute_range);
    else
        m_ok = false;
}

bool BitField::Parser::checkAreaCorrect(const char c, int len, Area area)
{
    bool ok = false;
    switch (area)
    {
        case NAME:
            ok = len < MAX_CAP_LEN &&
                 ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_' || (c >= '0' && c <= '9') || c=='.');
            break;
        case RANGEA:
            ok = len < MAX_CAP_LEN &&
                 ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || c == '_' || (c >= '0' && c <= '9') || c=='B');
            break;
        case RANGEB:
            ok = len < MAX_CAP_LEN && c >= '0' && c <= '9';
            break;
        case VALUE:
            ok = len < MAX_CAP_LEN &&
                 (c == '$'                                                                    // constant
                  || (c >= '0' && c <= '9')                                                   // deimal
                  || c == 'x' || c == 'X' || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') // 0x00 hex
                  || c == 'b');                                                               // binary
            break;
    }
    return ok;
}
