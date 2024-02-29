#include "registercontainer.h"

RegisterContainer::RegisterContainer()
{
}

RegisterContainer *RegisterContainer::instance()
{
    static RegisterContainer *pinstance=0;
    if(pinstance==0){
        pinstance = new RegisterContainer();
    }
    return pinstance;
}



void RegisterContainer::add(unsigned int id,Register *preg)
{
    if(contains((unsigned int)id)) return;
    m_regs_list[(unsigned int)id] = preg;    
}


void RegisterContainer::remove(unsigned int id){
    if(!contains((unsigned int)id)) return;

    Register *preg  = m_regs_list.take((unsigned int)id);
    if(preg && !preg->isSub())
    {
        // remove subregister of this register from regs_list
        Register *psubreg = preg->sub("");
        if(psubreg && m_regs_list.values().contains(psubreg))
        {
            m_regs_list.take(m_regs_list.key(psubreg));
        }
        delete preg;
    }    
}

Register *RegisterContainer::operator [](unsigned int id)
{
    return m_regs_list[id];
}


Register *RegisterContainer::reg(unsigned int id) {
    if(contains(id)) return m_regs_list[id];
            return 0;
}

void RegisterContainer::clear() {
    qDeleteAll(m_regs_list);
    m_regs_list.clear();
}

