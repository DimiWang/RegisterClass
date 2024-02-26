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
        parser_ok = parser->is_ok();
    }

    if(parser_ok){
        int bit_count;
        if(parser->ranges() == 2)
            bit_count = parser->msb() - parser->lsb() + 1;
        else bit_count = parser->lsb();

        if(bit_count>0)
        {
            field = new BitField(parser->name());

            // inflate with bits
            while(bit_count--){                
                field->append(new Bit(false));
            }
            if(parser->has_value())
                field->setValue(parser->value());
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
    Area area = NAME;
    int len = 0;
    clear();    
    const char *p_in = pfield;
    char *p_cap = m_captured[NAME];
    bool error = false;
    while(*p_in && !error)
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

        default :
            if(p_cap){
                if(checkAreaCorrect(c,len,area)){
                    *p_cap++ = c;
                    len ++;
                }
                else {
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
            qDebug()<<"[2]";
            error =  true;
            break;
        }
    }
    if(error ) {
        qWarning() << "ERROR parsing field" << pfield;
        return false;
    }

    //Range
    m_range_lsb = 0;
    m_range_msb = 0;
    const char c = m_captured[RANGEA][0];
    m_by_name = ((c >= 'a' && c <= 'z') || (c>='A' && c<='Z'));

    if (!m_by_name  && m_captured[RANGEA][0] != '\0')
    {
        // "[A:B]"
        if(m_has_range == 2 && m_captured[RANGEB][0] != '\0'){

            m_range_lsb = StrToUl(m_captured[RANGEA]);
            m_range_msb = StrToUl(m_captured[RANGEB]);

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
            m_range_lsb = StrToUl(m_captured[RANGEA]);
            m_range_msb = m_range_lsb;
        }
    }
    /// Value
    if (m_has_value)
    {
        char *p = m_captured[VALUE];
        if(p[0] == '$')
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
    m_range_msb = 0;
    m_range_lsb = 0;
    m_value_u32 = 0;
    m_has_range = 0;
    m_has_value = false;
    m_value_readonly = false;
    m_by_name = false;
    memset(m_captured,'\0',sizeof(m_captured));
}


BitField::Parser::Parser(const char *p)
{
    clear();
    if(p)
        m_ok = load(p);
    else m_ok = false;
}

bool BitField::Parser::checkAreaCorrect(const char c, int len, Area area )
{
    bool ok = false;
    switch(area){
    case NAME:
        ok = len<MAX_CAP_LEN
              && ((c>='a' && c<='z')|| (c>='A' && c<='Z') || c=='_' || (c>='0' && c<='9') );
        break;
    case RANGEA:
        ok = len<MAX_CAP_LEN
              && ((c>='a' && c<='z')||(c>='A' && c<='Z') || c=='_' || (c>='0' && c<='9') );
        break;
    case RANGEB:
        ok = len<MAX_CAP_LEN
              && c>='0' &&c<='9';
        break;
    case VALUE:
        ok = len<MAX_CAP_LEN
              && (c == '$' // constant
                  || (c >= '0' && c <= '9') //deimal
                  || c == 'x' || c == 'X' || (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z') // 0x00 hex
                  || c == 'b' ); // binary
        break;
    }
    return ok;
}
