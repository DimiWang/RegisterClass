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
#define IS_ALPHA(C) ((C >= 'a' && C <= 'z') || (C>='A' && C<='Z'))

/****************************************************************************
 * @function name: constructor
 *
 * @param:
 *             void
 * @description:
 * @return: (  )
 ****************************************************************************/
BitField::BitField(const QString &name, int size, bool bit_owner ):
    BitSet(name, size, bit_owner)
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
    foreach(const QString &name, extras.keys()){
        m_extra[name] = extras[name];
    }
}


BitField * BitField::makeField(const QString &str,
                               bool value,
                               const QString &description,
                               bool readonly,
                               const QHash<QString, QVariant> &extras )
{
    BitField *field=0;
    BitField::Parser parser;
    if(parser.load(str.toLatin1().constData())){

        int bit_count=0;

        bit_count = (parser.range_lsb == parser.range_msb  && parser.range_msb  > 0) ?
                    parser.range_msb  :
                    (qMax(parser.range_msb ,parser.range_lsb) - qMin(parser.range_msb ,parser.range_lsb))
                    + 1;

        if(bit_count>0)
        {
            field = new BitField(parser.captured.name);
            field->setDescription(description);
            field->setConstant(readonly);
            field->setExtras(extras);

            // inflate with bits
            while(bit_count--){                
                field->append(new Bit(value));
            }
            if(parser.has_value)
                field->setValue(parser.value_u32);
        }

    }
    return field;
}


QVariant BitField::extra(const QString &name)
{
    if(m_extra.contains(name))
    {
        return  m_extra[name];
    }
    return QString();
}

QStringList BitField::extras() const
{
    return m_extra.keys();
}


bool BitField::Parser::load(const char *pfield)
{
    enum Area{ NAME = 0, RANGEA , RANGEB, RANGE_NAME, VALUE};
    Area next_area = NAME;
    Area area=NAME;
    clear();
    has_range = 0;
    has_value = false;
    by_name = false;
    const char *p_in = pfield;
    char *p_cap = captured.name;
    while(*p_in)
    {
        const char c = *p_in++;
        switch (c)
        {
        case ('['):
            next_area = RANGEA;
            has_range++;
            p_cap = captured.range_a;
            break;

        case (':'):
            has_range++;
            next_area = RANGEB;
            p_cap = captured.range_b;
            break;

        case (']'):
            break;

        case ('='):
            next_area = VALUE;
            has_value = true;
            p_cap = captured.value;
            break;

        default :
            if(p_cap)
                *p_cap++ = c;
            break;
        }

        if (next_area >= area)
        {
            area = next_area;
        }
        else
        {
            qWarning() << "ERROR parsing field" << pfield;
            return false;
        }
    }

    //Range
    range_lsb = 0;
    range_msb = 0;
    by_name = IS_ALPHA(captured.range_a[0]);
    if (!by_name  && captured.range_a[0] != '\0')
    {
        // "[A:B]"
        if(has_range == 2 && captured.range_b[0] != '\0'){
            range_lsb = strtol(captured.range_a,0,10);
            range_msb = strtol(captured.range_b,0,10);
            if (range_lsb > range_msb)
            {
                qint32 tmp = range_msb;
                range_msb = range_lsb;
                range_lsb = tmp;
            }
        }
        // "[A]"
        else
        {
            range_lsb = range_msb =  strtol(captured.range_a,0,10);
        }
    }
    /// Value
    if (has_value)
    {
        char *p = captured.value;
        if(p[0] == '$')
        {
            value_readonly = true;
            p++;
        }

        if(p[0] == '0' && p[1] == 'x')
        {
            value_u32 = strtoul(&p[2],0,16);
        }
        else {
            value_u32 =strtoul(&p[0],0,10);
        }
    }
    return true;
}

void BitField::Parser::clear()
{
    memset(&captured,'\0',sizeof(Parser::Captured));
}


BitField::Parser::Parser()
{
    range_msb = 0;
    range_lsb = 0;
    value_u32 = 0;
    has_range = 0;
    has_value = false;
    value_readonly = false;
    by_name = false;
    ok = false;
}
