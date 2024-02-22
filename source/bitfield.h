/**
 * @file:bit.h   -
 * @description: this is single bit with name.
 *              Referenced from registers.h/cpp
 * @project: BENCH OnSemiconductor
 * @date:
 *
 */
#ifndef BITFIELD_H
#define BITFIELD_H

#include <QByteArray>
#include <QHash>
#include <QVariant>
#include <QStringList>
#include "bitset.h"

/****************************************************************************
 * @class : Single bit with name
 * Bit
 ****************************************************************************/





class BitField: public BitSet
{
public:
    BitField(const QString &name=QString(), int size=-1);

    ~BitField();

    /* sets description */
    void setDescription(const QString &descr);

    /* description string */
    const QString &description() const;

    /* returns name of bit */
    QVariant extra(const QString &name);

    /* list of extras*/
    QStringList extras() const;

    /* sets name of bit */
    void setExtra(const QString &name,const QVariant &value);

    void setExtras(const QHash <QString, QVariant> &extras);

    struct Parser{
        typedef enum { NAME = 0, RANGEA , RANGEB, VALUE}Area;
        struct Captured{
            char name[100];
            char range_a[100];
            char range_b[10];
            char value[10];
            unsigned int len;
            Area area;
        }captured;        
        int has_range ;
        bool by_name;
        bool has_value;
        bool value_readonly;
        bool ok;
        qint32 range_lsb;
        qint32 range_msb;
        qint32 value_u32;
        Parser();
        bool checkAreaCorrect(const char c);
        bool load(const char*);
        void clear();
    };
    static BitField* makeField(const QString &str,
                               bool value = false,
                               const QString &description= QString(),
                               bool constant = false,
                               const QHash<QString, QVariant> &extras= QHash<QString,QVariant>());
protected:

    /* sets group id */
    void setId(const qint32 group_id);
    /* value is constant*/
    qint32 m_id;
    QHash <QString, QVariant> m_extra;

    /* bit description*/
    QString m_description;

private:        



};

//#include <QDebug>
//inline QDebug operator<< (QDebug d, const Bit &bit)
//{
//    d << QString("Bit(name=%1,value=%2)").arg(QString(bit.name())).arg(bit.value());
//    return d;
//}





#endif // BITFIELD_H
