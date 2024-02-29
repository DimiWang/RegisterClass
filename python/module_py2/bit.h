/**
 * @file:bit.h   -
 * @description: this is single bit with name. 
 *              Referenced from registers.h/cpp
 * @project: BENCH OnSemiconductor
 * @date: 
 *
 */
#ifndef BIT_H
#define BIT_H

#include <QByteArray>
#include <QHash>
#include <QVariant>
#include <QStringList>

/****************************************************************************
 * @class : Single bit with name
 * Bit
 ****************************************************************************/
class Bit
{
public:
    Bit(const QString &name=QString());
    /* returns name of bit */
    const QString &name() const;
    /* sets name of bit */
    void setName(const QString &name);
    /* gets value of bit */
    bool value() const;
    /* sets value of bit */
    void setValue(const bool value);
    /* sets bit as readOnly */
    void setConstant(const bool value) {m_constant = value;}
    /* bit is readOnly */
    bool constant() const {return m_constant;}
    /* inverts current bit */
    void invert(void);
    /* sets description */
    void setDescription(const QString &descr);
    /* description string */
    const QString &description() const;
    /* returns group id */
     qint32 group_id() const;
    /* sets group id */
    void setGroup_id(const qint32 group_id);    
    /* returns name of bit */
    QString extra(const QString &name);
    /* list of extras*/
    QStringList extras() const;
    /* sets name of bit */
    void setExtra(const QString &name,const QString &value);
    Bit &operator = (Bit &bit);
    Bit &operator = (const qint32 &val);

private:
    bool m_value;
    bool m_constant;
    qint32 m_group_id;
    QHash <QString, QString> m_extra;
    QString m_name;
    QString m_description;

};

#include <QDebug>
inline QDebug operator<< (QDebug d, const Bit &bit)
{
    d << QString("Bit(name=%1,value=%2)").arg(QString(bit.name())).arg(bit.value());
    return d;
}


#endif // BIT_H
