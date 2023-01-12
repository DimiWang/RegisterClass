#ifndef BITFIELDPARSER_H
#define BITFIELDPARSER_H
#include <QByteArray>

class BitFieldParser
{
public :

/* empty constructor*/
    BitFieldParser();
    /* constructor with data*/
    BitFieldParser(const char *pfield);

    /* loads string and parses it*/
    bool load(const char *pfield);
    /* name */
    const char* name();
    /* */
    bool rangeIsName() const;
    /**/
    const char* range_name();
    qint32 value() const;
    bool constant() const;
    const char*  description();
    const bool hasDoubleRange()const;
    qint32 lsb() const;
    qint32 msb() const;
    bool isOk() const;

private:
    void init();
    const char *field(qint32 n);
    char data[1000];
    bool has_double_range ;
    bool has_range ;
    bool bit_by_name;
    bool has_value;
    bool value_readonly;
    bool range_all;
    bool m_ok;
    qint32 m_range_lsb;
    qint32 m_range_msb;
    qint32 m_value;
};




#endif // BITFIELDPARSER_H
