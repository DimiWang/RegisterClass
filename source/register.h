#if 1
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
#include "bitfield.h"
#include "bitset.h"
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
class Register:  public QObject, public BitSet
{

    Q_OBJECT

public:
    enum {
        AllowSameName=1,
        AbsoluteRange=2,
        SaveInitScript=4,
        CaseSensitive =8,/* not implemented*/
    };

    /* update policies*/
    typedef enum {
        UpdateNever,
        UpdateAlways,
        UpdateOnChange} UpdatePolicy;

    /* zero constructor*/
    explicit  Register();

    /* bit list in format */
    Register(const QString &init_script, const QString  &name = QString());

    /* makes register with size and zero bit names */
    Register(qint32 bit_count, const QString  &name = QString(), quint32 options= AllowSameName|SaveInitScript);

    /* copy constructor*/
    Register(Register &reg);

    virtual ~Register();

    void clear();

    /* copies register src to dst .name not copy. success when dst.size == 0 and register dst same as src*/
    virtual void copy(Register &src);

    bool blockSignals(bool b);

    /* make setup -build register*/
    bool makeSetup(const QString &init_script);


    /* parsing methods */
    bool parseBit_Method1(const QString &field, int index);
    bool parseBit_Method2(const QString &field, int index);

    /* finds bit number by name*/
    BitField* field(const QString &name);
    BitField* field(int index);

    /* remove all bits by name*/
    void removeField(const QString &fieldname);

    /* adds single bit with name*/
    bool addField(const QString &field, qint32 put_to = -1);
    bool addField(BitField *bitfield);

    /* adds registers as string list*/
    bool addFieldList(const QStringList &field_list, qint32 put_to = -1);

    /* sets value to field (! Field is a bit or bit band )*/
    bool setValue(const QString &field, quint32 value);

    bool setBitValue(qint32 bitn, bool value);


    /* gets value from field*/
    quint32 value(const QString &field);


    /* returns bit pointer by name*/
    Bit *bit(const QString &bit_name);

    /*if bit presents in register*/
    bool contains(const QString &name);




    /* operator [] by field name*/
    Register &operator[] (const char *field);
    Register &operator[] (const QString &field);

    //    /* converts string to Unsigned Integer
    //     * 0x00 - is a hex
    //     * 0 - is decimal
    //     * 0b00 - is binary
    //    */
    static unsigned int strToUInt(const QString &text, bool *ok=0);


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
    Register * sub(const QString &extra_name, const QVariant &extra_value);

    /* makes sub register and returns pointer to temporary
     * . by extra values list*/
    Register * sub(const QString &extra_name, const QStringList &extra_values);

    /* is subregister */
    bool isSub() const {return m_is_sub;};

    /* checks if regsiter is same has same size and same bit names*/
    virtual bool isSame(Register *preg);


    /* returns items list*/
    QStringList fieldsList();
    int fieldsCount()const ;

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


    QStringList virtualList() {return m_vregs.keys(); }
    Register::Virtual *virtualItem(const QString &name) {return m_vregs[name];}

    /*[extra parameter]*/
    /* returns register extra value as string*/
    QVariant extra(const QString &name);

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

    /* converts to string format bit=1 */
    const QString toString(const QString &format="@name=@value;", bool grouped= true, bool include_virtual=false, bool skip_empty=true);
    bool fromString(const QString &text,const char ln_separator=';', const char eq_separator='=');


//    /**/
//    void bind(QObject *pobj, const char*Set, const char *Get);

    /* set update policy*/
    void setUpdatePolicy(UpdatePolicy upd_pol) {m_update_policy = upd_pol;}
    /* save parent register pointer*/
    void setParentRegister(Register *preg) { mp_parent = preg;}


    /* calculates CRC for register.
     If need to define a range use Sub reg*/
    quint32 crc(int bits, quint32 seed, quint32 poly, bool padding=true, QString *ptext=0);

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
    void buildReg();
    /**/
    void setSub(bool is_sub) {m_is_sub = is_sub;}
    /**/
    const Bit *bitAt(int index);

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

    bool m_is_sub;

    /* virtual register map*/
    QMap<QString,Virtual*> m_vregs;

    /* options in constructor*/
    quint32 m_options;

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

    /* setup data backup for rebuild*/
    QString m_setup_backup;

    /* field list */
    QList<BitField*> m_fields;

    /* list with extras */
    QHash <QString, QVariant> m_extra;

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
//typedef QMap<QString,Register> RegisterMap;

///* debug setup*/
////#include <QDebug>
////inline QDebug operator<< (QDebug d, const Register &reg)
////{
////    d << QString("Register(name=%1, bits=%2)").arg(reg.name()).arg(reg.size());
////    return d;
////}

Q_DECLARE_METATYPE(Register *)

#endif // REGISTER_H
#endif
