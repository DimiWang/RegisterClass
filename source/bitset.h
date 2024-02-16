#ifndef BITSET_H
#define BITSET_H

#include <QString>
#include <QList>
#include "bit.h"

class BitSet: public QList<Bit*>
{

public:

    /* bit LSB(lower) or MSB(higher) */
    typedef enum { MSB, LSB ,MSB8 } BitOrder;

    BitSet(const QString &name=QString(), int size=-1, bool owner = false);

    BitSet(BitSet &bitset);

    virtual ~BitSet();

    /* returns name of bit */
    const QString &name() const;

    /* sets name of bit */
    void setName(const QString &name);

    /* clears chain (all bits) */
    void clear(void);

    /* sets new size of register.
     * adds bits*/
    void resize(qint32 size);

    /* sets bit as readOnly */
    void setConstant(const bool value) {m_constant = value;}

    /* bit is readOnly */
    bool constant() const {return m_constant;}

    /* gets value max 32bits */
    quint32 value();

    /* gets value from field [from:to]*/
    quint32 value(qint32 from, qint32 to, bool *p_ok=0);

    /* gets bit value by number */
    bool bitValue(qint32 bitn, bool *p_ok=0);

    /* sets value max 32bits */
    void setValue(const quint32 value);

    /* inverts field */
    void invert(void);

    /* sets value to field (! Field is a bit or bit band )*/
    bool setValue(qint32 from, qint32 to, quint32 value);

    /* sets single bit value by bit number  */
    bool setBitValue(qint32 bitn, bool value);

    bool indexValid(int index);
    /* rotate register to direction "MSB->LSB - right"
     *                              "LSB->MSB - left */
    void rotateRight(int count=1, bool fill=false);
    void rotateLeft(int count=1, bool fill=false);

    /*roll register LSB <>MSB*/
    void roll();

    /* fills bits with value*/
    void fill(bool value, qint32 start = 0, qint32 count = -1);

    /* converts to int, bool, unsigned int*/
    operator int();
    operator bool();
    operator unsigned int();

    /* operator = * doesnt keep names */
    BitSet &operator = (BitSet &bitset);
    BitSet &operator = (quint32 val);

    /* operator !*/
    BitSet operator ! ();

    /* operator ~*/
    BitSet operator ~ ();

    /* AND operator */
    BitSet &operator &= (BitSet &reg);
    BitSet &operator &= (quint32 val);

    /* XOR operator */
    BitSet &operator ^= (BitSet &reg);
    BitSet &operator ^= (quint32 val);

    /* OR operator*/
    BitSet &operator |= (BitSet &reg);
    BitSet &operator |= (quint32 val);

    /* equal operator*/
    bool operator == ( BitSet &reg);
    bool operator == ( quint32 reg);

    /* not equal operator */
    bool operator != ( BitSet &reg);
    bool operator != ( quint32 reg);

    bool operator [] ( int index);

    /* joins other register */
    void join(BitSet &reg);
    void join(BitSet *preg);

    /* converts to byte array lsb first*/
    QByteArray toByteArray(BitOrder bitorder = LSB, bool contiguously=false);

    /* converts to hexademical string*/
    QString toHex(BitOrder bitorder = LSB, bool contiguously=false);

    /* loads from byte array */
    bool fromByteArray(const QByteArray &bytearray, qint32 scanchain_length=-1,BitOrder bitorder = LSB, bool contiguously=false);

    /* from hexademical representation */
    bool fromHex(const QString &hex, qint32 length_bits=-1, BitOrder bitorder = LSB, bool contiguously=false);

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

    /* converts byte array to bit array */
    static QByteArray convertByteArrayToBitArray(const QByteArray &data_in, qint32 size_in_bits, BitOrder bitorder = LSB);

    /* converts bit array to byte array */
    static QByteArray convertBitArrayToByteArray(const QByteArray &data_in, BitOrder bitorder = LSB, bool contiguously = false);


    /* encodes to 21 bit Hamming */
    bool encodeMeToHamming(bool secded = false);

    /* decodes from Hamming 21bit to 16bit*/
    bool decodeMeFromHamming(bool secded =false);

    virtual void copy(BitSet &bit_set);
    virtual bool isSame(BitSet *preg);

protected:
    bool isBitsOwner() const {return m_bits_owner;}
    void setBitsOwner(bool is_owner) {m_bits_owner = is_owner;}

private:
    bool m_constant;

    bool m_bits_owner;
    /* bit name */
    QString m_name;

};

#endif // BITSET_H
