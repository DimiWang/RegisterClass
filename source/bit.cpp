/**
 * @file:bit.cpp   -
 * @description: <in header>
 * @project: BENCH OnSemiconductor
 * @date: 
 *
 */

#include "bit.h"


/****************************************************************************
 * @function name: constructor
 *
 * @param:
 *             void
 * @description:
 * @return: (  )
 ****************************************************************************/
Bit::Bit(const QString &name)
{
    m_name = name;
    m_value = false;
    m_group_id = 1;
    m_constant =0;
}


/****************************************************************************
 * @function name: Bit::name()
 *
 * @param:
 *             void
 * @description:
 * @return: ( const QString &) -returns bit name
 ****************************************************************************/
const QString &Bit::name() const
{
    return m_name;
}


/****************************************************************************
 * @function name: Bit::setName - ---
 *
 * @param:
 *
 *  const QString &name
 * @description:
 * @return: ( void )
 ****************************************************************************/
void Bit::setName(const QString &name)
{
    m_name = name;
}



/****************************************************************************
 * @function name: Bit::value()
 *
 * @param:
 *             void
 * @description:
 * @return: ( const bool) - returns bit value
 ****************************************************************************/
bool Bit::value() const
{
    return m_value;
}


/****************************************************************************
 * @function name: Bit::setValue()
 *
 * @param:
 *
 *  const bool value
 * @description:
 * @return: ( void )
 ****************************************************************************/
void Bit::setValue(const bool value)
{
    if(!m_constant)
        m_value = value;
}


/****************************************************************************
 * @function name: Bit::invert()
 *
 * @param:
 *             void
 * @description: Inverts bit
 * @return: ( void )
 ****************************************************************************/
void Bit::invert()
{
    m_value = !m_value;
}


/****************************************************************************
 * @function name: Bit::setDescription()
 *
 * @param:
 *             const QString &descr
 * @description: Inverts bit
 * @return: ( void )
 ****************************************************************************/
void Bit::setDescription(const QString &descr)
{
    m_description = descr;
}


/****************************************************************************
 * @function name: Bit::description()
 *
 * @param:
 *             void
 * @description:
 * @return: ( const QString &Bit )
 ****************************************************************************/
const QString &Bit::description() const
{
    return m_description;
}


/****************************************************************************
 * @function name: Bit::group_id()
 *
 * @param:
 *             void
 * @description:
 * @return: ( const qint32)
 ****************************************************************************/
qint32 Bit::group_id() const
{
    return m_group_id;
}


/****************************************************************************
 * @function name: Bit::setGroup_id()
 *
 * @param:
 *             const qint32 group_id
 * @description:
 * @return: ( void)
 ****************************************************************************/
void Bit::setGroup_id(const qint32 group_id)
{
    m_group_id = group_id;
}




/****************************************************************************
 * @function name: Bit::operator =
 *
 * @param:
 *             Bit &bit
 * @description: copy bit
 * @return: ( Bit &Bit)
 ****************************************************************************/
Bit &Bit::operator =(Bit &bit)
{
    this->setDescription(bit.description());
    this->m_group_id = bit.m_group_id;
    this->m_name = bit.m_name;
    this->m_value = bit.m_value;    
    if(!bit.m_extra.isEmpty())
    {
        m_extra = bit.m_extra;
    }
    return *this;
}

Bit &Bit::operator =(const qint32 &val)
{
    m_value=val;
    return *this;
}


void Bit::setExtra(const QString &name, const QString &value)
{
    m_extra[name] = value;
}

QString Bit::extra(const QString &name)
{
    if(m_extra.contains(name))
    {
        return  m_extra[name];
    }
    return QString();
}

QStringList Bit::extras() const
{
    return m_extra.keys();
}
