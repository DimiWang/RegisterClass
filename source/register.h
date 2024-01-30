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
    typedef enum { MSB, LSB ,MSB8 ,MSBITEM}BitOrder;
    /* zero constructor*/
    explicit  Register();
    /* bit list in format */
    Register(const QString &init_script, const QString  &name = QString());
    /* makes register with size and zero bit names */
    Register(qint32 bit_count,const QString  &name = QString(), quint32 options= AllowSameName|SaveInitScript);
    /* copy consdtructor*/
    Register( const Register &reg);
    virtual ~Register();
    bool blockSignals(bool b);
    bool makeSetup(const QString &init_script);
    const QString getSetup();
    bool parseBit_Method1(const QString &field, int index);
    bool parseBit_Method2(const QString &field, int index);

    /* adds bit by pointer*/
    void addSingleBit(Bit *pbit, qint32 put_to = -1);
    void removeBit(Bit *pbit);
    void removeBitByName(const QString &name);
    /* adds single bit with name*/
    bool addField(const QString &field, qint32 put_to = -1);
    /* adds registers as string list*/
    bool addFieldList(const QStringList &field_list, qint32 put_to = -1);

    /* sets value to field (! Field is a bit or bit band )*/
    bool setValue(const QString &field, quint32 value);
    /* sets value to field (! Field is a bit or bit band )*/
    bool setValue(quint32 from, quint32 to, quint32 value);
    /* regroups bits/fields by name. if next field is different group_id++*/
    void regroup();

    /* gets value from field*/
    quint32 value(const QString &field);
    /* gets value from field [from:to]*/
    quint32 value(qint32 from, qint32 to);
    /* gets value from bit number*/
    quint32 value(qint32 bit_number);

    /* sets single bit value by bit number  */
    bool setBit(qint32 bitn, bool value);
    /* sets single bit value by bit name */
    bool setBit(const QString &bitname, bool value);

    /* returns bit pointer by name*/
    Bit *bit(const char *bit_name);
    Bit *bit(const QString &bit_name);
    /* gets bit value by number */
    bool bit(qint32 bitn);
    /* returns pointer to bit at bitn */
    Bit *bitAt(qint32 bitn);
    /*if bit presents in register*/
    bool contains(const QString &name);
    /* joins other register*/
    void join(Register &reg);
    void join(Register *preg);
    /* converts to byte array lsb first*/
    QByteArray toByteArray(BitOrder bitorder = LSB, bool contiguously=false);
    /**/
    QString toHex(BitOrder bitorder = LSB, bool contiguously=false);
    /* loads from byte array */
    bool fromByteArray(const QByteArray &bytearray, qint32 scanchain_length=-1,BitOrder bitorder = LSB, bool contiguously=false);
    /**/
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
    int findBit(Bit *pbit);
    /* clears chain (all bits) */
    void clear(void);
    /* returns 1 if readonly*/
    bool readOnly() const;
    /* sets to readonly*/
    void setReadOnly(bool on);    
    Register &operator[] (const char *field);
    Register &operator[] (const QString &field);
    operator int();
    operator bool();
    operator unsigned int();
    /* operator = * doesnt keep names */
    Register &operator = (const Register &reg);
    Register &operator = (quint32 val);

    Register operator ! ();
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
    /* sets size of register */
    void resize(qint32 size);
    /* set register name*/
    void setName(const QString &name);

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
    /* makes sub(***) register and returns pointer to temporary. from ~ to (including from and to)*/
    Register *  sub(qint32 from, qint32 to=-1);
    /* makes sub register and returns pointer to temporary. sorts bits by name*/
    Register *  sub(const QString &bitname);
    Register *sub(const QStringList &bits);
    /* makes sub register and returns pointer to temporary. by extra parameter name with value*/
    Register *  sub(const QString &extra_name, const QString &extra_value);
    Register *  sub(const QString &extra_name, const QStringList &extra_values);
    /* is subregister */
    bool isSub() const;
    /* checks if regsiter is same has same size and same bit names*/
    bool isSame(Register *preg);
    /* if register is empty*/
    bool isEmpty() const ;
    /* copies register src to dst .name not copy. success when dst.size == 0 and register dst same as src*/
    static bool copy(Register *pdst, Register *psrc);

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
    QStringList items();

    /* encodes to 21 bit Hamming */
    bool encodeMeToHamming(bool secded = false);
    /* decodes from Hamming 21bit to 16bit*/
    bool decodeMeFromHamming(bool secded =false);
    /* scales bit array*/
    static QByteArray scaleBitArray(const QByteArray &data_in, qint32 factor);
    /* scales byte array by bits*/
    static QByteArray scaleByteArray(const QByteArray &data_in, qint32 factor);
    /* converts byte array tp bit array */
    static QByteArray convertByteArrayToBitArray(const QByteArray &data_in, qint32 size_in_bits, BitOrder bitorder = LSB);
    /* converts bit array to byte array */
    static QByteArray convertBitArrayToByteArray(const QByteArray &data_in, BitOrder bitorder = LSB, bool contiguously = false);    

    quint32 crc(int bits, quint32 seed, quint32 poly, bool padding=true, QString *ptext=0);

    QStringList virtualList() {return m_vregs.keys(); }
    Register::Virtual *virtualItem(const QString &name) {return m_vregs[name];}
    /*[extra parameter]*/
    /* returns name of bit */
    QString extra(const QString &name);
    unsigned int extraAsUInt(const QString &name,bool *ok=0);

    /* sets name of bit */
    void setExtra(const QString &name,const QString &value);
    QStringList extras() const;
    void applyValueFromExtra(const QString &extra_name);
    /**/
    static const char TAG_PURENAME[];
    static const char TAG_NAME[];
    static const char TAG_VALUE[];
    static const char TAG_VALUE_HEX[];
    static const char TAG_GROUP[];
    static const char TAG_BITN[];
    static const char TAG_DESCR[];
    static const char TAG_EXTRAS[];
    static const char TAG_READONLY[];

    void bind(QObject *pobj, const char*Set, const char *Get);
    typedef enum {UpdateAlways,UpdateOnChange} UpdatePolicy;
    void setUpdatePolicy(UpdatePolicy upd_pol);
    void setParentRegister(Register *preg) ;
    UpdatePolicy updatePolicy() const ;

    /* set as sub register */
    void setSub(bool on);

signals:

    void removed();
    void changed();
    void signal_updateSet(const QString &regname);
    void signal_updateGet(const QString &regname);
    void signal_Warning(const QString &);
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
    void makeTemporary();
    void addTemporary(Bit *pbit);
    void clearTemporary();
    void moveOffset(int index);

    /* virtual regs map*/
    QMap<QString,Virtual*> m_vregs;
    QStringList m_items;

    quint32 m_options;
    Register *mp_parent;
    Register *mp_temporary;
    int m_offset;
    /* variable is sub register*/
    bool m_isSub;
    /* read only */
    bool m_readOnly;
    /* */    
    UpdatePolicy m_update_policy;
    /**/
    QString m_setup_backup;
    /* register name*/
    QString m_name;
    /* bits list */
    QList<Bit *> m_chain;
    QHash <QString, QString> m_extra;

    /* finds maximum group id in chain*/
    qint32 findMaxGroupId();

    quint32 m_adr;
    quint32 m_page;

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
