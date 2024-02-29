#ifndef REGISTERCONTAINER_H
#define REGISTERCONTAINER_H

#include "register.h"

class RegisterContainer
{
    explicit RegisterContainer();

public:
    QMap<unsigned int,Register*> m_regs_list;

    static RegisterContainer * instance();

    void add(unsigned int id,Register *preg);
    void remove(unsigned int id);
    void clear() ;

    bool contains(unsigned int id) const {     return m_regs_list.contains(id);  }
    bool containsReg(Register *preg) {return m_regs_list.values().contains(preg);}

    unsigned int regId(Register *preg) {return m_regs_list.key(preg);}
    Register *reg(unsigned int id);
    Register *operator[](unsigned int id);

};

#define REGISTERCONTAINER RegisterContainer::instance()
#endif // REGS_H
