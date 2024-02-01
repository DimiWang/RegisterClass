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
#include <QObject>
class Register;

#include <QByteArray>
#include <QList>
#include <QMap>
#include <QString>
#include <QDebug>
#include "bit.h"
#include "hamming.h"

//format [MSB...LSB] Example: "0001" ,"1011"
//              in example 1 and 11 decimal
#define BINARY(STR)     (Register::fromBitStringToUint(QByteArray(STR)))

/* bits splitter */
#define BITS_SPLITTER (';')


#define WARNING(txt) signal_Warning(txt);
#define ERROR(txt) signal_Error(txt);


/****************************************************************************
 * @class : Register class as set of bits
 * Register
 ****************************************************************************/
class Register: public QObject
{

    Q_OBJECT

public:
    enum {
            AllowSameName=1,
            AbsoluteRange=2,
            SaveInitScript=4,
            CaseSensitive =8,/* not implemented*/
         };

    /* bit LSB(lower) or MSB(higher) */
    typedef enum { MSB, LSB ,MSB8 ,MSBITEM} BitOrder;

    /* zero constructor*/
    explicit  Register();

    /* bit list in format */
    Register(const QString &init_script, const QString  &name = QString());

    /* makes register with size and zero bit names */
    Register(qint32 bit_count,const QString  &name = QString(), quint32 options= AllowSameName|SaveInitScript);

    /* copy consdtructor*/
    Register( const Register &reg);
    virtual ~Register();

    /* set register name*/
    void setName(const QString &name);

    bool blockSignals(bool b);

    /* make setup -build register*/
    bool makeSetup(const QString &init_script);

    /* return backup setup*/
    const QString getSetup();

    /* parsing methods */
    bool parseBit_Method1(const QString &field, int index);
    bool parseBit_Method2(const QString &field, int index);

    /* adds single bit by pointer*/
    void addSingleBit(Bit *pbit, qint32 put_to = -1);

    /* remove bit by pointer*/
    void removeBit(Bit *pbit);

    /* remove all bits by name*/
    void removeByName(const QString &name);

    /* adds single bit with name*/
    bool addField(const QString &field, qint32 put_to = -1);

    /* adds registers as string list*/
    bool addFieldList(const QStringList &field_list, qint32 put_to = -1);

    /* sets value to field (! Field is a bit or bit band )*/
    bool setValue(const QString &field, quint32 value);

    /* sets value to field (! Field is a bit or bit band )*/
    bool setValue(quint32 from, quint32 to, quint32 value);

    /* sets single bit value by bit number  */
    bool setBitValue(qint32 bitn, bool value);

    /* gets value from field*/
    quint32 value(const QString &field);

    /* gets value from field [from:to]*/
    quint32 value(qint32 from, qint32 to);

    /* gets value from bit number*/
    quint32 value(qint32 bit_number);

    /* gets bit value by number */
    bool bitValue(qint32 bitn);

    /* returns bit pointer by name*/
    Bit *bit(const QString &bit_name);

    /* returns pointer to bit at bitn */
    Bit *bitAt(qint32 bitn);

    /* regroups bits/fields by name. if next field is different group_id++*/
    void regroup();

    /*if bit presents in register*/
    bool contains(const QString &name);

    /* joins other register */
    void join(Register &reg);
    void join(Register *preg);

    /* converts to byte array lsb first*/
    QByteArray toByteArray(BitOrder bitorder = LSB, bool contiguously=false);

    /* converts to hexademical string*/
    QString toHex(BitOrder bitorder = LSB, bool contiguously=false);

    /* loads from byte array */
    bool fromByteArray(const QByteArray &bytearray, qint32 scanchain_length=-1,BitOrder bitorder = LSB, bool contiguously=false);

    /* from hexademical representation */
    bool fromHex(const QString &hex, qint32 scanchain_length=-1,BitOrder bitorder = LSB, bool contiguously=false);

    /* converts to bit string format:101010111 */
    QByteArray toBitString(BitOrder bitorder = LSB);

    /* converts to unsigned int 32bit.rest(>32 bit) is ignored*/
    quint32 toUInt(BitOrder bitorder = LSB);

    /* set register as unsigned int 32bit.rest(>32 bit) is ignored*/
    void setUInt(quint32 value,BitOrder bitorder= LSB);

    /* loads from Bit String */
    void fromBitString(const QByteArray &bytearray, BitOrder bitorder = LSB);

    /* Converts from string to integer. Example: from '1111 0000' format to 0xF0 */
    static quint32 fromBitStringToUint(const QByteArray & ba);

    /* converts to string format bit=1 */
    const QString toString(const QString &format="@name=@value;", bool grouped= true, bool include_virtual=false, bool skip_empty=true);
    bool fromString(const QString &text,const char ln_separator=';', const char eq_separator='=');

    /* bits count*/
    qint32 size(void) const {return m_chain.size();}

    /* finds bit number by name*/
    qint32 findBitByName(const QString &name);

    /* find bit by pointer*/
    int findBit(Bit *pbit);

    /* clears chain (all bits) */
    void clear(void);

    /* returns 1 if readonly*/
    bool readOnly() const;

    /* sets this register as readonly not allowed to change*/
    void setReadOnly(bool on);

    /* operator [] by field name*/
    Register &operator[] (const char *field);
    Register &operator[] (const QString &field);

    /* converts to int, bool, unsigned int*/
    operator int();
    operator bool();
    operator unsigned int();

    /* operator = * doesnt keep names */
    Register &operator = (const Register &reg);
    Register &operator = (quint32 val);

    /* operator !*/
    Register operator ! ();

    /* operator ~*/
    Register operator ~ ();

    /* AND operator */
    Register &operator &= (Register &reg);
    Register &operator &= (quint32 val);

    /* XOR operator */
    Register &operator ^= (Register &reg);
    Register &operator ^= (quint32 val);

    /* OR operator*/
    Register &operator |= (Register &reg);
    Register &operator |= (quint32 val);

    /* equal operator*/
    bool operator == ( Register &reg);
    bool operator == ( quint32 reg);

    /* not equal operator */
    bool operator != ( Register &reg);
    bool operator != ( quint32 reg);


    /* fills bits with value*/
    void fill(bool value, qint32 count = -1, qint32 start = 0);

    /* rotate register to direction "MSB->LSB - right"
     *                              "LSB->MSB - left */
    void rotateRight(int count=1, bool fill=false);
    void rotateLeft(int count=1, bool fill=false);

    /*roll register LSB <>MSB*/
    Register *roll();

    /* inverts all bits*/
    void invert();

    /* sets new size of register.
     * adds bits*/
    void resize(qint32 size);

    /* converts string to Unsigned Integer
     * 0x00 - is a hex
     * 0 - is decimal
     * 0b00 - is binary
    */
    static unsigned int strToUInt(const QString &text, bool *ok=0);

    /* get register name*/
    const QString &name() const {return m_name;}

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
    void makeSubRegister(Register *preg, qint32 group_id);

    /* fill in preg with bits as sub register*/
    void makeSubRegister(Register *preg, qint32 from, qint32 to);

     /* makes sub register and returns pointer to temporary.
      *  from ~ to (including from and to)*/
    Register *  sub(qint32 from, qint32 to=-1);

    /* makes sub register and returns pointer to temporary
     * . sorts bits by name*/
    Register * sub(const QString &fieldname);

    /* makes sub register and returns pointer to temporary
     * . by field list*/
    Register * sub(const QStringList &fields);

    /* makes sub register and returns pointer to temporary
     * . by extra parameter name with value*/
    Register * sub(const QString &extra_name, const QString &extra_value);

    /* makes sub register and returns pointer to temporary
     * . by extra values list*/
    Register * sub(const QString &extra_name, const QStringList &extra_values);

    /* is subregister */
    bool isSub() const;

    /* checks if regsiter is same has same size and same bit names*/
    bool isSame(Register *preg);

    /* if register is empty*/
    bool isEmpty() const ;

    /* copies register src to dst .name not copy. success when dst.size == 0 and register dst same as src*/
    static bool copy(Register *pdst, Register *psrc);

    /* returns items list*/
    QStringList items();

    //VREGS
    /* vrtual reg struct*/
    struct Virtual
     {
        Register *preg;
        quint32 val;
        bool readonly;
        QString descr;
        Virtual(){val=0;preg=0;readonly=false;}
        virtual ~Virtual(){}
        virtual bool set(quint32 val)=0;
        virtual quint32 get(){return val;}
    };
    /* append virtual reg */
    bool appendVirtual(const QString &name, const QString &descr,Virtual *pvreg);

    /* encodes to 21 bit Hamming */
    bool encodeMeToHamming(bool secded = false);

    /* decodes from Hamming 21bit to 16bit*/
    bool decodeMeFromHamming(bool secded =false);

    /* scales bit array*/
    static QByteArray scaleBitArray(const QByteArray &data_in, qint32 factor);

    /* scales byte array by bits*/
    static QByteArray scaleByteArray(const QByteArray &data_in, qint32 factor);

    /* converts byte array to bit array */
    static QByteArray convertByteArrayToBitArray(const QByteArray &data_in, qint32 size_in_bits, BitOrder bitorder = LSB);

    /* converts bit array to byte array */
    static QByteArray convertBitArrayToByteArray(const QByteArray &data_in, BitOrder bitorder = LSB, bool contiguously = false);    

    /* calculates CRC for register.
    If need to define a range use Sub reg*/
    quint32 crc(int bits, quint32 seed, quint32 poly, bool padding=true, QString *ptext=0);

    QStringList virtualList() {return m_vregs.keys(); }
    Register::Virtual *virtualItem(const QString &name) {return m_vregs[name];}

    /*[extra parameter]*/
    /* returns register extra value as string*/
    QString extra(const QString &name);

    /* returns extra value as integer*/
    unsigned int extraAsUInt(const QString &name,bool *ok=0);

    /* sets extra value*/
    void setExtra(const QString &name,const QString &value);

    /* returns extras list*/
    QStringList extras() const;

    /* apply current values from some extra value*/
    void applyValueFromExtra(const QString &extra_name);

    /**/
    static const char *tag_purename;
    static const char *tag_name;
    static const char *tag_value;
    static const char *tag_value_hex;
    static const char *tag_group;
    static const char *tag_bitn;
    static const char *tag_descr;
    static const char *tag_extras;
    static const char *tag_readonly;

    /**/
    void bind(QObject *pobj, const char*Set, const char *Get);
    /* update policies*/
    typedef enum {
        UpdateNever,
        UpdateAlways,
        UpdateOnChange} UpdatePolicy;

    /* set update policy*/
    void setUpdatePolicy(UpdatePolicy upd_pol);
    /* save parent register pointer*/
    void setParentRegister(Register *preg) ;

    /* set this register as sub-register */
    void setSub(bool on);

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

#define __SET(name) do{\
    if(isSub())  emit mp_parent->signal_updateSet(name);\
    else signal_updateSet(name);\
    }while(0);

#define __GET(name) do{\
    if(isSub()) emit mp_parent->signal_updateGet(name);\
    else signal_updateGet(name);\
    }while(0);

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

    /* virtual register map*/
    QMap<QString,Virtual*> m_vregs;

    /* items list is generated when build*/
    QStringList m_items;

    /* options in constructor*/
    quint32 m_options;

    /* pointer to parent register.
     * Used when this instance is sub registr*/
    Register *mp_parent;    

    /* pointer to temporary register.
     * Usually sub register*/
    Register *mp_temporary;

    /* current bit offset. used for register build */
    int m_offset;

    /* variable is sub register. register is a subset
     * of pointers to the bits which is owned by another register*/
    bool m_isSub;

    /* read only - register is readonly*/
    bool m_readOnly;

    /* update policy tells when gnerate changed signal*/
    UpdatePolicy m_update_policy;

    /* setup data backup for rebuild*/
    QString m_setup_backup;

    /* register name*/
    QString m_name;

    /* bits list */
    QList<Bit *> m_chain;

    /* list with extras */
    QHash <QString, QString> m_extra;

    /* finds maximum group id in chain*/
    qint32 findMaxGroupId();

    void cond_update(bool changed);

};

class SubRegister:public Register{
public:
    SubRegister()
        :Register(){ setSub(1);  }
// [not neccessary]
//    SubRegister(const QString &init_script,const QString  &name = QString())
//        :Register(init_script,name){setSub(1);}
//    SubRegister(qint32 bit_count,const QString  &name = QString(), quint32 options= AllowSameName)
//        :Register(bit_count,name,options){setSub(1);}
//    SubRegister( const Register &reg)
//        :Register(reg){setSub(1);}
};




/* map of register can be accessed by name */
typedef QMap<QString,Register> RegisterMap;

/* debug setup*/
#include <QDebug>
inline QDebug operator<< (QDebug d, const Register &reg)
{
    d << QString("Register(name=%1, bits=%2)").arg(reg.name()).arg(reg.size());
    return d;
}

Q_DECLARE_METATYPE(Register *)

#endif // SCANCHAIN_H
