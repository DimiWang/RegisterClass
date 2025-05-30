#include "memorymap.h"

MemoryMap::MemoryMap(): Register()
{

}

quint32 MemoryMap::pathSize(const QString &path_name)
{
    quint32 path_size =0;    
    for(int i=0;i<fieldsList().size();i++){        
        if(field(i)->extra("path").toString().toUpper().trimmed() == path_name.toUpper().trimmed())
        {            
            path_size += field(i)->size();
        }
    }

    return path_size;
}
