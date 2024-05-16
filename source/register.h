/**
 * @file:register.h   -
 * @description:  this is class to maintain bits with names.
 *              Register representation as list of bits
 * @project:
 * @date:
 *
 */

#ifndef REGISTER_H
#define REGISTER_H

#ifndef Q_FOREACH
#define Q_FOREACH
#endif

#include <QObject>
class Register;

#include <QByteArray>
#include <QList>
#include <QMap>
#include <QString>
#include <QDebug>
#include "bitfield.h"
#include "bitset.h"

// format [MSB...LSB] Example: "0001" ,"1011"
//              in example 1 and 11 decimal
#define BINARY(STR) (Register::fromBitStringToUint(QByteArray(STR)))

/* bits splitter */
#define BITS_SPLITTER (';')

#define WARNING(txt) signal_Warning(txt);
#define ERROR(txt) signal_Error(txt);

class Register : public QObject, public BitSet
{
    Q_OBJECT

public:
    enum
    {

        SubRegister = 0x10,
        AutoInflate = 0x20
    };

    typedef enum
    {
        AllowSameName = 1, /*not implemented*/
        AbsoluteRange = 2,
        Default1 = 4,
        Default0 = 8
    } LoadOptions;

    /* update policies*/
    typedef enum
    {
        UpdateNever,
        UpdateAlways,
        UpdateOnChange
    } UpdatePolicy;

    /* zero constructor*/
    explicit Register(Register *parent=0,
                      const QString &name = QString(),
                      bool is_sub = false,
                      const QByteArray &init_json = QByteArray(),
                      quint32 load_options = AllowSameName);

    /* copy constructor*/
    Register(Register &reg);

    virtual ~Register();

    void clear();
    bool loadJsonData(const QByteArray &json_data, quint32 options);

    /* copies register src to dst .name not copy. success when dst.size == 0 and register dst same as src*/
    void copy(Register &src);

    bool blockSignals(bool b);

    /* finds bit number by name*/
    BitField *field(const QString &name);
    BitField *field(int index);

    /* remove field by name*/
    void removeField(const QString &fieldname, bool include_bits);

    /* adds single bit with name*/
    bool addField(const QString &field, quint32 options=0);
    void addField(BitField *bitfield);

    /* sets value to field (! Field is a bit or bit band )*/
    bool setFieldValue(const QString &field, quint32 value);

    /* gets value from field*/
    quint32 fieldValue(const QString &field);

    /*if field presents in register*/
    bool contains(const QString &name);

    /* operator [] by field name*/
    Register &operator[](const QString &field);

    //    /* converts string to Unsigned Integer
    //     * 0x00 - is a hex
    //     * 0 - is decimal
    //     * 0b00 - is binary
    //    */
    static unsigned int strToUInt(const QString &text, bool *ok = 0);

    /*
        (***)
            Sub register is a temporary register which contains the same bits
            as former register.
            Tchnical particularity of register class instance is that sub register does
            not delete bits on destruction. Otherwise it is normal register.
            Bits which are grouped to sub register will be referenced twice.
            Example:
            Register reg("bit0;bit1;bit3");
            // ^ contains: [pts_bit0];[pts_bit1];[pts_bit2]
            reg.sub(0,1); // will make sub register which points to the same bits
            // ^ contains: [pts_bit0];[pts_bit1]
    */

    /* fill in preg with bits as sub register*/
    static Register *makeSubRegister(Register *parent, const QString &name = QString());

    /* makes sub register and returns pointer to temporary.
     *  from ~ to (including from and to)*/
    Register *sub(qint32 from, qint32 to = -1);

    /* makes sub register and returns pointer to temporary
     * . sorts bits by name*/
    Register *sub(const QString &fieldname);

    /* makes sub register and returns pointer to temporary
     * . by field list*/
    Register *sub(const QStringList &fields);

    /* makes sub register and returns pointer to temporary
     * . by extra parameter name with value*/
    Register *sub(const QString &extra_name, const QVariant &extra_value);

    /* is subregister */
    bool isSub() const
    {
        return m_is_sub;
    }

    /* checks if regsiter is same has same size and same bit names*/
    virtual bool isSame(Register *preg);

    /* returns field list*/
    QStringList fieldsList();
    int fieldsCount() const;

    /*[extra parameter]*/
    /* returns register extra value as string*/
    QVariant extra(const QString &name);

    /* returns extra value as integer*/
    unsigned int extraAsUInt(const QString &name, bool *ok = 0);

    /* sets extra value*/
    void setExtra(const QString &name, const QVariant &value);

    /* returns extras list*/
    QStringList extras() const;

    /* apply current values from some extra value*/
    void applyValueFromExtra(const QString &extra_name);


    /* converts to string format bit=1 */
    const QString toString(const QString &format = "@name=@value;", bool skip_empty = true);
    bool fromString(const QString &text, const char ln_separator = ';', const char eq_separator = '=');

    /* set update policy*/
    void setUpdatePolicy(UpdatePolicy upd_pol)
    {
        m_update_policy = upd_pol;
    }

    /* calculates CRC for register.
     If need to define a range use Sub reg*/
    quint32 crc(int bits, quint32 seed, quint32 poly, bool padding = true, QString *ptext = 0);

    Register *temporary() {return this->mp_temporary;}
signals:
    /* signal register is removed*/
    void removed();
    /* signal when register content is changed*/
    void changed();

    void signal_updateSet(const QString &regname);
    void signal_updateGet(const QString &regname);

    /* register generates warning message.
     * Usually on build or access of wrong items*/
    void signal_Warning(const QString &);

    /* register generates error message*/
    void signal_Error(const QString &);

#define __SET(name)                                 \
    do                                              \
    {                                               \
        if (isSub())                                \
            emit mp_parent->signal_updateSet(name); \
        else                                        \
            signal_updateSet(name);                 \
    } while (0);

#define __GET(name)                                 \
    do                                              \
    {                                               \
        if (isSub())                                \
            emit mp_parent->signal_updateGet(name); \
        else                                        \
            signal_updateGet(name);                 \
    } while (0);

private:
    BitField *findFieldByBit(Bit *pbit);   
    bool parseJsonObjectAsField(const QJsonObject &field_obj, quint32 options);

protected:
    /* create temporary register when is not*/
    void makeTemporary();

    /* add custom temporary register*/
    void addTemporary(Bit *pbit);

    /* clear temporary register.
     * Not delete */
    void clearTemporary();

    /* changes current offset.
     *  Used in register build*/
    void moveOffset(unsigned int index);

    void cond_update(bool changed);

    /* options in constructor*/
    bool m_is_sub;

    /* pointer to parent register.
     * Used when this instance is sub register*/
    Register *mp_parent;

    /* pointer to temporary register.
     * Usually sub register*/
    Register *mp_temporary;

    /* current bit offset. used for register build */
    int m_offset;

    /* update policy tells when gnerate changed signal*/
    UpdatePolicy m_update_policy;

    /* field list */
    QList<BitField *> m_fields;

    /* list with extras */
    QHash<QString, QVariant> m_extra;

    void replaceTagsInLine(QString *line, QMap<QString, QString> &dict);
};

////class SubRegister:public Register{
////public:
////    SubRegister()
////        :Register(){ setSub(1);  }
////// [not neccessary]
//////    SubRegister(const QString &init_script,const QString  &name = QString())
//////        :Register(init_script,name){setSub(1);}
//////    SubRegister(qint32 bit_count,const QString  &name = QString(), quint32 options= AllowSameName)
//////        :Register(bit_count,name,options){setSub(1);}
//////    SubRegister( const Register &reg)
//////        :Register(reg){setSub(1);}
////};

///* map of register can be accessed by name */
// typedef QMap<QString,Register> RegisterMap;

///* debug setup*/
////#include <QDebug>
////inline QDebug operator<< (QDebug d, const Register &reg)
////{
////    d << QString("Register(name=%1, bits=%2)").arg(reg.name()).arg(reg.size());
////    return d;
////}

Q_DECLARE_METATYPE(Register *)

#endif // REGISTER_H
