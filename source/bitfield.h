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



#define MAX_CAP_LEN (100)


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

    class Parser{
        typedef enum { NAME = 0, RANGEA , RANGEB, VALUE,AREA_COUNT}Area;
        char m_captured[AREA_COUNT][MAX_CAP_LEN];
        int m_has_range ;
        bool m_by_name;
        bool m_has_value;
        bool m_value_readonly;
        bool m_ok;
        qint32 m_range_lsb;
        qint32 m_range_msb;
        qint32 m_value_u32;
        bool checkAreaCorrect(const char c, int len, Area area);
    public:
        Parser(const char*p = 0);
        bool load(const char*);
        qint32 lsb() const {return m_range_lsb;}
        qint32 msb() const {return m_range_msb;}
        char *name() {return m_captured[0];}
        char *range_name() {return m_captured[1];}
        quint32 value() const {return m_value_u32;}
        char *cap(int n) {return m_captured[n];}
        int ranges() {return m_has_range;}
        bool range_is_name() {return m_by_name;}
        bool is_value_constant(){return m_value_readonly;}
        bool has_value() {return m_has_value;}
        bool is_ok() const {return m_ok;}
        void clear();
    };
    static BitField* makeField(const QString &str, Parser *parser);
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
