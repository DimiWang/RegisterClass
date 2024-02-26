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


/****************************************************************************
 * @function name: constructor
 *
 * @param:
 *             void
 * @description:
 * @return: (  )
 ****************************************************************************/
BitField::BitField(const QString &name, int size ):
    BitSet(name, size, false)
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


BitField * BitField::makeField(const QString &str, BitField::Parser *parser)
{
    BitField *field=0;
    bool parser_ok =false;
    if(parser == 0) {
        parser = new BitField::Parser;
        parser_ok =  parser->load(str.toLatin1().constData());
    }else{
        parser_ok = parser->ok;
    }

    if(parser_ok){
        int bit_count;
        if(parser->has_range == 2)
            bit_count = parser->range_msb - parser->range_lsb + 1;
        else bit_count = parser->range_lsb;

        if(bit_count>0)
        {
            field = new BitField(parser->captured.name);

            // inflate with bits
            while(bit_count--){                
                field->append(new Bit(false));
            }
            if(parser->has_value)
                field->setValue(parser->value_u32);
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

static quint32 StrToUl(const char *str){
    quint32 result;
    if(str[0] == '0' && str[1]=='x')
        result = (quint32)strtol(&str[2],0,16);
    else
        result = (quint32)strtol(str,0,10);
    return result;
}

bool BitField::Parser::load(const char *pfield)
{    
    Area next_area = NAME;
    captured.area = NAME;
    captured.len = 0;
    clear();
    has_range = 0;
    has_value = false;
    by_name = false;
    const char *p_in = pfield;
    char *p_cap = captured.name;
    bool stop = false;
    while(*p_in && !stop)
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
            if(p_cap){
                if(checkAreaCorrect(c)){
                    *p_cap++ = c;
                    captured.len ++;
                }
                else stop = true;
            }
            break;
        }

        if (next_area >= captured.area)
        {
            captured.len = 0;
            captured.area = next_area;
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
    const char c = captured.range_a[0];
    by_name = ((c >= 'a' && c <= 'z') || (c>='A' && c<='Z'));

    if (!by_name  && captured.range_a[0] != '\0')
    {
        // "[A:B]"
        if(has_range == 2 && captured.range_b[0] != '\0'){

            range_lsb = StrToUl(captured.range_a);
            range_msb = StrToUl(captured.range_b);

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
            range_lsb = StrToUl(captured.range_a);
            range_msb = range_lsb;
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

        value_u32 = StrToUl(p);
    }
    return true;
}

void BitField::Parser::clear()
{
    memset(&captured,'\0',sizeof(Parser::Captured));
}


BitField::Parser::Parser(const char *p)
{
    range_msb = 0;
    range_lsb = 0;
    value_u32 = 0;
    has_range = 0;
    has_value = false;
    value_readonly = false;
    by_name = false;
    if(p)
        ok = load(p);
    else ok = false;
}

bool BitField::Parser::checkAreaCorrect(const char c)
{
    bool ok = false;
    switch(captured.area){
    case NAME:
        ok = captured.len<sizeof(captured.name)
              && ((c>='a' && c<='z')|| (c>='A' && c<='Z') || c=='_' || (c>='0' && c<='9') );
        break;
    case RANGEA:
        ok = captured.len<sizeof(captured.range_a)
              && ((c>='a' && c<='z')||(c>='A' && c<='Z') || c=='_' || (c>='0' && c<='9') );
        break;
    case RANGEB:
        ok = captured.len<sizeof(captured.range_b)
              && c>='0' &&c<='9';
        break;
    case VALUE:
        ok = captured.len<sizeof(captured.value)
              && (c == '$' // constant
                  || (c >= '0' && c <= '9') //deimal
                  || c == 'x' || c == 'X' || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') // 0x00 hex
                  || c == 'b' ); // binary
        break;
    }
    return ok;
}
