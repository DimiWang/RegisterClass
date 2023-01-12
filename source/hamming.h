#ifndef HAMMING_H
#define HAMMING_H

#include <QTypeInfo>

//
extern quint32 ham_calc_secded(quint32 value, quint32 size );

//
extern bool ham_check_secded(quint32 value, quint32 size );

//
extern quint32 ham_populate_parity(quint32 size);

//
extern quint32 ham_parity_from_data_bits(int data_bits);

//
extern quint32 ham_parity_from_total_bits(int data_bits);

// size - in data bits
// value - data
// *ls - parity bits list size
// *pl - parity data bits list
extern quint32 ham_encode(quint32 value, quint32 size, bool secded, int *ls=0, bool *pl=0);

//
extern bool ham_decode(quint32 value, quint32 *presult, quint32 size);

//
extern bool ham_check(quint32 value, quint32 size);

//
extern bool ham_fix(quint32 value_in, quint32 *pvalue_out, quint32 size);

#endif // HAMMING_H
