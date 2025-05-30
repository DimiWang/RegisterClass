#ifndef MEMORYMAP_H
#define MEMORYMAP_H

#include "register.h"

class MemoryMap : public Register
{    
    Q_OBJECT
    quint32 m_path_size;
public:
    MemoryMap();
    quint32 sizeBytes() const { return this->size()/8 + (this->size()%8)>1;}
    quint32 pathSize(const QString &path_name);
};

#endif // MEMORYMAP_H
