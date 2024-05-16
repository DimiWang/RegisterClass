
#include <python.h>
#include "converters.h"
#include "registercontainer.h"



static PyObject* Regs_init(PyObject *self, PyObject *args)
{    
    PyObject *obj;
    char *init=0;
    char *name=0;
    if(PyArg_ParseTuple(args, "Os|s",&obj ,&init ,&name))
    {
        REGISTERCONTAINER->add((unsigned int)obj,new Register(init,name));
    }

    Py_INCREF(Py_None);
    return Py_None;
}

static PyObject* Regs_del(PyObject *self, PyObject *args)
{
    PyObject *obj;
    if(PyArg_ParseTuple(args, "O",&obj))
    {
        REGISTERCONTAINER->remove((unsigned int )obj);
    }

    Py_INCREF(Py_None);
    return Py_None;
}



//static PyObject* Regs_addBit(PyObject *self, PyObject *args)
//{
//    int result = 0;
//    PyObject *obj;
//    Register *reg;
//    char *str;
//    int pos=-1;
//    if(PyArg_ParseTuple(args, "Os|i",&obj,&str,&pos))
//    {
//        if(REGISTERCONTAINER->contains((unsigned int)obj))
//        {
//            reg = REGISTERCONTAINER->reg((unsigned int)obj);
//            result = reg->addBit(str,pos);
//        }
//    }
//    return PyLong_FromLong(result);
//}

//static PyObject* Regs_removeBit(PyObject *self, PyObject *args)
//{
//    PyObject *obj;
//    Register *reg;
//    char *str;
//    if(PyArg_ParseTuple(args, "Os",&obj,&str))
//    {
//        if(REGISTERCONTAINER->contains((unsigned int)obj))
//        {
//            reg = REGISTERCONTAINER->reg((unsigned int)obj);
//            reg->removeBitByName(str);
//        }
//    }
//    Py_INCREF(Py_None);
//    return Py_None;
//}

//static PyObject* Regs_setValue(PyObject *self, PyObject *args)
//{
//    PyObject *obj;
//    Register *reg;
//    int val;
//    char *str;
//    if(PyArg_ParseTuple(args, "OsI",&obj, &str, &val))
//    {
//        if(REGISTERCONTAINER->contains((unsigned int)obj))
//        {
//            reg = REGISTERCONTAINER->reg((unsigned int)obj);
//            reg->setValue(str,(quint32)val);
//        }
//    }
//    Py_INCREF(Py_None);
//    return Py_None;
//}

//static PyObject* Regs_value(PyObject *self, PyObject *args)
//{
//    PyObject *obj;
//    Register *reg;
//    char *str;
//    if(PyArg_ParseTuple(args, "Os",&obj, &str))
//    {
//        if(REGISTERCONTAINER->contains((unsigned int)obj))
//        {
//            reg = REGISTERCONTAINER->reg((unsigned int)obj);
//            return Py_BuildValue("I",reg->value(str));
//        }
//    }
//    Py_INCREF(Py_None);
//    return Py_None;
//}

//static PyObject* Regs_setBit(PyObject *self, PyObject *args)
//{
//    PyObject *obj;
//    Register *reg;
//    int num;
//    int val;
//    if(PyArg_ParseTuple(args, "OiI",&obj, &num, &val))
//    {
//        if(REGISTERCONTAINER->contains((unsigned int)obj))
//        {
//            reg = REGISTERCONTAINER->reg((unsigned int)obj);
//            reg->setBit(num,val);
//        }
//    }
//    Py_INCREF(Py_None);
//    return Py_None;
//}

//static PyObject* Regs_bit(PyObject *self, PyObject *args)
//{
//    PyObject *obj;
//    Register *reg;
//    int num;
//    if(PyArg_ParseTuple(args, "Oi",&obj, &num))
//    {
//        if(REGISTERCONTAINER->contains((unsigned int)obj))
//        {
//            reg = REGISTERCONTAINER->reg((unsigned int)obj);
//            return Py_BuildValue("i",reg->bit(num));
//        }
//    }
//    Py_INCREF(Py_None);
//    return Py_None;
//}

//static PyObject* Regs_getItem(PyObject *self, PyObject *args)
//{
//    PyObject *obj;
//    Register *reg;
//    PyObject *item;
//    if(PyArg_ParseTuple(args, "OO",&obj, &item))
//    {
//        if(REGISTERCONTAINER->contains((unsigned int)obj))
//        {
//            reg = REGISTERCONTAINER->reg((unsigned int)obj);

//#if PY_MAJOR_VERSION == 2
//            if(PyString_Check(item)){
//                char tmp[100];
//                sprintf(tmp,"[%s]", PyString_AsString(item));
//                return Py_BuildValue("I",reg->value(tmp));
//            }
//#elif PY_MAJOR_VERSION == 3
//            if(PyUnicode_Check(item)){
//                char tmp[100];
//                sprintf(tmp,"[%s]", PyUnicode_AsUTF8(item));
//                return Py_BuildValue("I",reg->value(tmp));
//            }
//#endif
//            else if(PyLong_Check(item) )
//            {
//                return Py_BuildValue("I",reg->bit(PyLong_AsLong(item)));
//            }
//#if PY_MAJOR_VERSION == 2
//            else if(PyInt_Check(item))
//            {
//                return Py_BuildValue("I",reg->bit(PyInt_AsLong(item)));
//            }
//#endif
//            else if(PySlice_Check(item)){
//                Py_ssize_t start,stop,step;

//#if PY_MAJOR_VERSION == 2
//                PySlice_GetIndices((PySliceObject*)item,3,&start,&stop,&step);
//#elif PY_MAJOR_VERSION ==3
//                PySlice_GetIndices(item,3,&start,&stop,&step);
//#endif
//                return Py_BuildValue("I",reg->value(start,stop));
//            }
//        }
//    }
//    Py_INCREF(Py_None);
//    return Py_None;
//}



//static PyObject* Regs_setItem(PyObject *self, PyObject *args)
//{
//    PyObject *obj;
//    Register *reg;
//    PyObject *item;
//    quint32 val;
//    if(PyArg_ParseTuple(args, "OOI",&obj, &item, &val))
//    {
//        if(REGISTERCONTAINER->contains((unsigned int)obj))
//        {
//            reg = REGISTERCONTAINER->reg((unsigned int)obj);

//#if PY_MAJOR_VERSION == 2
//            if(PyString_Check(item)){
//                char tmp[100];
//                sprintf(tmp,"[%s]",PyString_AsString(item));
//                reg->setValue(tmp,val);
//            }
//#elif PY_MAJOR_VERSION == 3
//     if(PyUnicode_Check(item)){
//                char tmp[100];
//                sprintf(tmp,"[%s]",PyUnicode_AsUTF8(item));
//                reg->setValue(tmp,val);
//            }
//#endif
//            else if(PyLong_Check(item))
//            {
//                reg->setBit(PyLong_Check(item),val);
//            }
//#if PY_MAJOR_VERSION == 2
//            else if(PyInt_Check(item))
//            {
//                reg->setBit(PyInt_AsLong(item),val);
//            }
//#endif
//            else if(PySlice_Check(item)){
//                Py_ssize_t start,stop,step;

//#if PY_MAJOR_VERSION == 2
//                PySlice_GetIndices((PySliceObject*)item,3,&start,&stop,&step);
//#elif PY_MAJOR_VERSION ==3
//                PySlice_GetIndices(item,3,&start,&stop,&step);
//#endif
//                reg->setValue(start,stop,val);
//            }
//        }
//    }
//    Py_INCREF(Py_None);
//    return Py_None;
//}



//static PyObject* Regs_contains(PyObject *self, PyObject *args)
//{
//    PyObject *obj;
//    Register *reg;
//    char *name;
//    if(PyArg_ParseTuple(args, "Os",&obj, &name))
//    {
//        if(REGISTERCONTAINER->contains((unsigned int)obj))
//        {
//            reg = REGISTERCONTAINER->reg((unsigned int)obj);
//            return Py_BuildValue("i",reg->contains(name));
//        }
//    }
//    return  Py_BuildValue("i",0);
//}

//static PyObject* Regs_toByteArray(PyObject *self, PyObject *args)
//{
//    PyObject *obj;
//    Register *reg;
//    int bits_order = Register::LSB;
//    if(PyArg_ParseTuple(args, "O|i",&obj, &bits_order))
//    {
//        if(REGISTERCONTAINER->contains((unsigned int)obj))
//        {
//            reg = REGISTERCONTAINER->reg((unsigned int)obj);
//            QByteArray r =  reg->toByteArray((Register::BitOrder)bits_order);
//            return Py_BuildValue("s#", r.constData(),r.size());
//        }
//    }
//    Py_INCREF(Py_None);
//    return Py_None;
//}

//static PyObject* Regs_fromByteArray(PyObject *self, PyObject *args)
//{
//    PyObject *obj;
//    Register *reg;
//    PyObject  *ba=0;
//    char *buf;
//    int buf_len;
//    int bits=-1;
//    int bits_order = Register::LSB;
//    int cont =0;
//    if(PyArg_ParseTuple(args, "OO|iii",&obj, &ba,&bits,&bits_order,&cont))
//    {
//        buf_len = PyByteArray_Size(ba);
//        buf = PyByteArray_AsString(ba);
//        QByteArray data = QByteArray(buf,buf_len);
//        if(REGISTERCONTAINER->contains((unsigned int)obj))
//        {
//            reg = REGISTERCONTAINER->reg((unsigned int)obj);
//            reg->fromByteArray(data,bits,(Register::BitOrder)bits_order,cont);
//        }
//    }
//    Py_INCREF(Py_None);
//    return Py_None;
//}

//static PyObject* Regs_toHex(PyObject *self, PyObject *args)
//{
//    PyObject *obj;
//    Register *reg;
//    int bits_order = Register::LSB;
//    int cont=0;
//    if(PyArg_ParseTuple(args, "O|i",&obj, &bits_order))
//    {
//        if(REGISTERCONTAINER->contains((unsigned int)obj))
//        {
//            reg = REGISTERCONTAINER->reg((unsigned int)obj);
//            return Py_BuildValue("s", reg->toHex((Register::BitOrder)bits_order,cont).toLatin1().constData());
//        }
//    }
//    Py_INCREF(Py_None);
//    return Py_None;
//}

//static PyObject* Regs_fromHex(PyObject *self, PyObject *args)
//{
//    PyObject *obj;
//    Register *reg;
//    int len=0;
//    char *str;
//    int cont=0;
//    int bits_order = Register::LSB;
//    if(PyArg_ParseTuple(args, "Os|iii",&obj,&str,&len,&bits_order,&cont))
//    {
//        if(REGISTERCONTAINER->contains((unsigned int)obj))
//        {
//            reg = REGISTERCONTAINER->reg((unsigned int)obj);
//            bool ok = reg->fromHex(QString(str),len,(Register::BitOrder)bits_order,cont);
//            return Py_BuildValue("i",ok);
//        }
//    }
//    Py_INCREF(Py_None);
//    return Py_None;
//}

//static PyObject* Regs_toBitString(PyObject *self, PyObject *args)
//{
//    PyObject *obj;
//    Register *reg;
//    int bits_order = Register::LSB;
//    if(PyArg_ParseTuple(args, "O|i",&obj, &bits_order))
//    {
//        if(REGISTERCONTAINER->contains((unsigned int)obj))
//        {
//            reg = REGISTERCONTAINER->reg((unsigned int)obj);
//            PyObject *res= Py_BuildValue("s", reg->toBitString((Register::BitOrder)bits_order).constData());
//            return res;
//        }
//    }
//    Py_INCREF(Py_None);
//    return Py_None;
//}

//static PyObject* Regs_fromBitString(PyObject *self, PyObject *args)
//{
//    PyObject *obj;
//    Register *reg;
//    int bits_order = Register::LSB;
//    char *str=0;
//    if(PyArg_ParseTuple(args, "Os|i",&obj,&str, &bits_order))
//    {
//        if(REGISTERCONTAINER->contains((unsigned int)obj))
//        {
//            reg = REGISTERCONTAINER->reg((unsigned int)obj);
//            reg->fromBitString(QByteArray(str),(Register::BitOrder)bits_order);
//        }
//    }
//    Py_INCREF(Py_None);
//    return Py_None;
//}

//static PyObject* Regs_toUInt(PyObject *self, PyObject *args)
//{
//    PyObject *obj;
//    Register *reg;
//    int bits_order = Register::LSB;
//    if(PyArg_ParseTuple(args, "O|i",&obj, &bits_order))
//    {
//        if(REGISTERCONTAINER->contains((unsigned int)obj))
//        {
//            reg = REGISTERCONTAINER->reg((unsigned int)obj);
//            return Py_BuildValue("I", reg->toUInt((Register::BitOrder)bits_order));
//        }
//    }
//    Py_INCREF(Py_None);
//    return Py_None;
//}

//static PyObject* Regs_setUInt(PyObject *self, PyObject *args)
//{
//    PyObject *obj;
//    Register *reg;
//    int bits_order = Register::LSB;
//    int val=0;
//    if(PyArg_ParseTuple(args, "OI|i",&obj, &val, &bits_order))
//    {
//        if(REGISTERCONTAINER->contains((unsigned int)obj))
//        {
//            reg = REGISTERCONTAINER->reg((unsigned int)obj);
//            reg->setUInt(val,(Register::BitOrder)bits_order);
//        }
//    }
//    Py_INCREF(Py_None);
//    return Py_None;
//}

//static PyObject* Regs_toString(PyObject *self, PyObject *args)
//{
//    PyObject *obj;
//    Register *reg;
//    char *str=0;
//    int grouped;
//    int incl_virt;
//    if(PyArg_ParseTuple(args, "O|sii",&obj, &str,&grouped,&incl_virt))
//    {
//        if(REGISTERCONTAINER->contains((unsigned int)obj))
//        {
//            reg = REGISTERCONTAINER->reg((unsigned int)obj);
//            if(str){
//                return Py_BuildValue("s", reg->toString( str, grouped, incl_virt).toLatin1().constData());
//            }
//            else {
//                return Py_BuildValue("s", reg->toString().toLatin1().constData() );
//            }
//        }
//    }
//    Py_INCREF(Py_None);
//    return Py_None;
//}

//static PyObject* Regs_fromString(PyObject *self, PyObject *args)
//{
//    PyObject *obj;
//    Register *reg;
//    char *str=0;
//    int eq;
//    int ln;
//    if(PyArg_ParseTuple(args, "Os|ii",&obj, &str, &eq, &ln))
//    {
//        if(REGISTERCONTAINER->contains((unsigned int)obj))
//        {
//            reg = REGISTERCONTAINER->reg((unsigned int)obj);
//            if(str){
//                reg->fromString(str,(char)eq,(char)ln);
//            }
//            else {
//                reg->fromString(str,(char)eq,(char)ln);
//            }
//        }
//    }
//    Py_INCREF(Py_None);
//    return Py_None;
//}

//static PyObject* Regs_invert(PyObject *self, PyObject *args)
//{
//    PyObject *obj;
//    Register *reg;
//    if(PyArg_ParseTuple(args, "O",&obj))
//    {
//        if(REGISTERCONTAINER->contains((unsigned int)obj))
//        {
//            reg = REGISTERCONTAINER->reg((unsigned int)obj);
//            reg->invert();
//        }
//    }
//    Py_INCREF(Py_None);
//    return Py_None;
//}

//static PyObject* Regs_roll(PyObject *self, PyObject *args)
//{
//    PyObject *obj;
//    Register *reg;
//    if(PyArg_ParseTuple(args, "O",&obj))
//    {
//        if(REGISTERCONTAINER->contains((unsigned int)obj))
//        {
//            reg = REGISTERCONTAINER->reg((unsigned int)obj);
//            reg->roll();
//        }
//    }
//    Py_INCREF(Py_None);
//    return Py_None;
//}

//static PyObject* Regs_size(PyObject *self, PyObject *args)
//{
//    PyObject *obj;
//    Register *reg;
//    if(PyArg_ParseTuple(args, "O",&obj))
//    {
//        if(REGISTERCONTAINER->contains((unsigned int)obj))
//        {
//            reg = REGISTERCONTAINER->reg((unsigned int)obj);
//            return Py_BuildValue("i",reg->size());
//        }
//    }
//    Py_INCREF(Py_None);
//    return Py_None;
//}


//static PyObject* Regs_rotateLeft(PyObject *self, PyObject *args)
//{
//    PyObject *obj;
//    Register *reg;
//    int count=1;
//    int fill=0;
//    if(PyArg_ParseTuple(args, "O|ii",&obj,&count,&fill))
//    {
//        if(REGISTERCONTAINER->contains((unsigned int)obj))
//        {
//            reg = REGISTERCONTAINER->reg((unsigned int)obj);
//            reg->rotateLeft(count,fill);
//        }
//    }
//    Py_INCREF(obj);
//    return obj;
//}

//static PyObject* Regs_rotateRight(PyObject *self, PyObject *args)
//{
//    PyObject *obj;
//    Register *reg;
//    int count=1;
//    int fill=0;
//    if(PyArg_ParseTuple(args, "O|ii",&obj,&count,&fill))
//    {
//        if(REGISTERCONTAINER->contains((unsigned int)obj))
//        {
//            reg = REGISTERCONTAINER->reg((unsigned int)obj);
//            reg->rotateRight(count,fill);
//        }
//    }
//    Py_INCREF(obj);
//    return obj;
//}

//static PyObject* Regs_crc(PyObject *self, PyObject *args)
//{

//    PyObject *obj;
//    Register *reg;
//    int padding =0;
//    int seed =0;
//    int bits = 0;
//    int poly =0;
//    if(PyArg_ParseTuple(args, "Oii|ii",&obj,&poly, &bits,&padding,&seed))
//    {
//        if(REGISTERCONTAINER->contains((unsigned int)obj))
//        {
//            reg = REGISTERCONTAINER->reg((unsigned int)obj);
//            int result = reg->crc(bits,(quint32)seed,(quint32)poly,padding);
//            return Py_BuildValue("I",result);
//        }
//    }
//    Py_INCREF(Py_None);
//    return Py_None;
//}

//static PyObject* Regs_fill(PyObject *self, PyObject *args)
//{
//    PyObject *obj;
//    Register *reg;
//    int value;
//    int count=-1;
//    int start=0;
//    if(PyArg_ParseTuple(args, "Oi|ii",&obj,&value,&count,&start))
//    {
//        if(REGISTERCONTAINER->contains((unsigned int)obj))
//        {
//            reg = REGISTERCONTAINER->reg((unsigned int)obj);
//            reg->fill(value,count,start);
//        }
//    }
//    Py_INCREF(Py_None);
//    return Py_None;
//}


//static PyObject* Regs_clear(PyObject *self, PyObject *args)
//{
//    PyObject *obj;
//    Register *reg;
//    if(PyArg_ParseTuple(args, "O",&obj))
//    {
//        if(REGISTERCONTAINER->contains((unsigned int)obj))
//        {
//            reg = REGISTERCONTAINER->reg((unsigned int)obj);
//            reg->clear();
//        }
//    }
//    Py_INCREF(Py_None);
//    return Py_None;
//}

//static PyObject* Regs_join(PyObject *self, PyObject *args)
//{
//    PyObject *obj;
//    Register *reg;
//    PyObject *reg2;
//    if(PyArg_ParseTuple(args, "OO",&obj,&reg2))
//    {
//        if(REGISTERCONTAINER->contains((unsigned int)obj))
//        {
//            reg = REGISTERCONTAINER->reg((unsigned int)obj);
//            if(REGISTERCONTAINER->contains((unsigned int)reg2))
//            {
//                reg->join(*REGISTERCONTAINER->reg((unsigned int)reg2));
//            }
//        }
//    }
//    Py_INCREF(Py_None);
//    return Py_None;
//}

//static PyObject* Regs_and(PyObject *self, PyObject *args)
//{
//    PyObject *obj;
//    Register *reg;
//    PyObject *reg2;
//    if(PyArg_ParseTuple(args, "OO",&obj,&reg2))
//    {
//        if(REGISTERCONTAINER->contains((unsigned int)obj))
//        {
//            reg = REGISTERCONTAINER->reg((unsigned int)obj);
//            if(REGISTERCONTAINER->contains((unsigned int)reg2))
//            {
//                reg->operator &=(*REGISTERCONTAINER->reg((unsigned int)reg2));
//            }
//            else if( PyLong_Check(reg2) )
//            {
//                reg->operator &=(PyLong_AsLong(reg2));//
//            }
//#if PY_MAJOR_VERSION ==2
//            else if( PyInt_Check(reg2) )
//            {
//                reg->operator &=(PyInt_AsLong(reg2));//
//            }
//#endif
//        }
//    }
//    Py_INCREF(obj);
//    return obj;
//}

//static PyObject* Regs_or(PyObject *self, PyObject *args)
//{
//    PyObject *obj;
//    Register *reg;
//    PyObject *reg2;
//    if(PyArg_ParseTuple(args, "OO",&obj,&reg2))
//    {
//        if(REGISTERCONTAINER->contains((unsigned int)obj))
//        {
//            reg = REGISTERCONTAINER->reg((unsigned int)obj);
//            if(reg)
//            {
//                if(REGISTERCONTAINER->contains((unsigned int)reg2))
//                {
//                    reg->operator |=(*REGISTERCONTAINER->reg((unsigned int)reg2));
//                }
//                else if( PyLong_Check(reg2))
//                {
//                    reg->operator |= ((quint32)PyLong_AsLong(reg2));
//                }
//#if PY_MAJOR_VERSION == 2
//                else if( PyInt_Check(reg2))
//                {
//                    reg->operator |= ((quint32)PyInt_AsLong(reg2));
//                }
//#endif
//            }
//        }
//    }
//    Py_INCREF(obj);
//    return obj;
//}

//static PyObject* Regs_xor(PyObject *self, PyObject *args)
//{
//    PyObject *obj;
//    Register *reg;
//    PyObject *reg2;
//    if(PyArg_ParseTuple(args, "OO",&obj,&reg2))
//    {
//        if(REGISTERCONTAINER->contains((unsigned int)obj))
//        {
//            reg = REGISTERCONTAINER->reg((unsigned int)obj);
//                if(REGISTERCONTAINER->contains((unsigned int)reg2))
//                {
//                    reg->operator ^=(*REGISTERCONTAINER->reg((unsigned int)reg2));
//                }
//                else if( PyLong_Check(reg2))
//                {
//                    reg->operator ^= ((quint32)PyLong_AsLong(reg2));
//                }
//#if PY_MAJOR_VERSION ==2
//                else if( PyInt_Check(reg2))
//                {
//                    reg->operator ^= ((quint32)PyInt_AsLong(reg2));
//                }
//#endif
//        }
//    }
//    Py_INCREF(obj);
//    return obj;
//}

//static PyObject* Regs_eq(PyObject *self, PyObject *args)
//{
//    PyObject *obj;
//    Register *reg;
//    PyObject *reg2;
//    if(PyArg_ParseTuple(args, "OO",&obj,&reg2))
//    {
//        if(REGISTERCONTAINER->contains((unsigned int)obj))
//        {
//            reg = REGISTERCONTAINER->reg((unsigned int)obj);
//                if(REGISTERCONTAINER->contains((unsigned int)reg2))
//                {
//                    return  Py_BuildValue("i",reg->operator == (*REGISTERCONTAINER->reg((unsigned int)reg2)));
//                }
//        }
//    }
//    Py_INCREF(Py_None);
//    return Py_None;
//}


//static PyObject* Regs_ne(PyObject *self, PyObject *args)
//{
//    PyObject *obj;
//    Register *reg;
//    PyObject *reg2;
//    if(PyArg_ParseTuple(args, "OO",&obj,&reg2))
//    {
//        if(REGISTERCONTAINER->contains((unsigned int)obj))
//        {
//            reg = REGISTERCONTAINER->reg((unsigned int)obj);
//            if(REGISTERCONTAINER->contains((unsigned int)reg2))
//            {
//                Py_BuildValue("i",reg->operator !=(*REGISTERCONTAINER->reg((unsigned int)reg2)));
//            }
//        }
//    }
//    Py_INCREF(Py_None);
//    return Py_None;
//}




//static PyObject* Regs_findBitByName(PyObject *self, PyObject *args)
//{
//    PyObject *obj;
//    Register *reg;
//    char *name;
//    if(PyArg_ParseTuple(args, "Os",&obj,&name))
//    {
//        if(REGISTERCONTAINER->contains((unsigned int)obj))
//        {
//            reg = REGISTERCONTAINER->reg((unsigned int)obj);
//            return Py_BuildValue("i",reg->findBitByName(name));
//        }
//    }
//    Py_INCREF(Py_None);
//    return Py_None;
//}


//static PyObject* Regs_items(PyObject *self, PyObject *args)
//{
//    PyObject *obj;
//    Register *reg;
//    if(PyArg_ParseTuple(args, "O",&obj))
//    {
//        if(REGISTERCONTAINER->contains((unsigned int)obj))
//        {
//            reg = REGISTERCONTAINER->reg((unsigned int)obj);
//            return QVariant_to_PyObject(reg->items());
//        }
//    }
//    Py_INCREF(Py_None);
//    return Py_None;
//}

//static PyObject* Regs_extras(PyObject *self, PyObject *args)
//{
//    PyObject *obj;
//    Register *reg;
//    char *name;
//    if(PyArg_ParseTuple(args, "O|s",&obj,&name))
//    {
//        if(REGISTERCONTAINER->contains((unsigned int)obj))
//        {
//            reg = REGISTERCONTAINER->reg((unsigned int)obj);
//            if(name )
//                if(reg->bit(name))
//                    return QVariant_to_PyObject(reg->bit(name)->extras());
//                else
//                    return QVariant_to_PyObject(reg->extras());
//        }
//    }
//    Py_INCREF(Py_None);
//    return Py_None;
//}


//static PyObject* Regs_extra(PyObject *self, PyObject *args)
//{
//    PyObject *obj;
//    Register *reg;
//    char *name;
//    if(PyArg_ParseTuple(args, "Os",&obj,&name))
//    {
//        if(REGISTERCONTAINER->contains((unsigned int)obj))
//        {
//            reg = REGISTERCONTAINER->reg((unsigned int)obj);
//            return QVariant_to_PyObject(reg->extra(name));
//        }
//    }
//    Py_INCREF(Py_None);
//    return Py_None;
//}


//static PyObject* Regs_setExtra(PyObject *self, PyObject *args)
//{
//    PyObject *obj;
//    Register *reg;
//    char *name;
//    char *val;
//    if(PyArg_ParseTuple(args, "Oss",&obj,&name,&val))
//    {
//        if(REGISTERCONTAINER->contains((unsigned int)obj))
//        {
//            reg = REGISTERCONTAINER->reg((unsigned int)obj);
//            reg->setExtra(name,val);
//        }
//    }
//    Py_INCREF(Py_None);
//    return Py_None;
//}

//static PyObject* Regs_setBitExtra(PyObject *self, PyObject *args)
//{
//    PyObject *obj;
//    Register *reg;
//    char *bitname;
//    char *extra_name;
//    char *val;
//    if(PyArg_ParseTuple(args, "OOss",&obj,&extra_name,&val))
//    {
//        if(REGISTERCONTAINER->contains((unsigned int)obj))
//        {
//            reg = REGISTERCONTAINER->reg((unsigned int)obj);
//#if PY_MAJOR_VERSION ==3
//                if(PyUnicode_Check(obj)){
//                    if(reg->findBitByName(bitname)>=0) {
//                        Register *psubreg =  reg->sub(QString(PyUnicode_AsUTF8(obj)));
//                        for(int i=0;i<psubreg->size();i++){
//                            psubreg->bitAt(i)->setExtra(extra_name,val);
//                        }
//                    }
//                }
//#elif PY_MAJOR_VERSION ==2
//                if(PyString_Check(obj)){
//                    if(reg->findBitByName(bitname)>=0) {
//                        Register *psubreg =  reg->sub(QString(PyString_AsString(obj)));
//                        for(int i=0;i<psubreg->size();i++){
//                            psubreg->bitAt(i)->setExtra(extra_name,val);
//                        }
//                    }
//                }
//#endif
//                else if(PyLong_Check(obj) ){
//                    int n= PyLong_AsLong(obj);
//                    if(n<=reg->size()){
//                        reg->bitAt(n)->setExtra(extra_name,val);
//                    }
//                }
//#if PY_MAJOR_VERSION ==2
//                else if(PyInt_Check(obj)){
//                    int n= PyInt_AsLong(obj);
//                    if(n<=reg->size()){
//                        reg->bitAt(n)->setExtra(extra_name,val);
//                    }
//                }
//#endif
//        }
//    }
//    Py_INCREF(Py_None);
//    return Py_None;
//}

//static PyObject* Regs_bitExtra(PyObject *self, PyObject *args)
//{
//    PyObject *obj;
//    Register *reg;
//    PyObject *bit;
//    char *extra_name;
//    if(PyArg_ParseTuple(args, "OOs", &obj, &bit, &extra_name))
//    {
//        if(REGISTERCONTAINER->contains((unsigned int)obj))
//        {
//            reg = REGISTERCONTAINER->reg((unsigned int)obj);

//#if PY_MAJOR_VERSION == 3
//            if(PyUnicode_Check(obj)){
//                QString bitname = QString(PyUnicode_AsUTF8(obj));
//                if(reg->findBitByName(bitname)>=0) {
//                    return QVariant_to_PyObject(reg->bit(bitname)->extra(extra_name));
//                }
//            }
//#elif PY_MAJOR_VERSION ==2
//            if(PyString_Check(obj)){
//                QString bitname = QString(PyString_AsString(obj));
//                if(reg->findBitByName(bitname)>=0) {
//                    return QVariant_to_PyObject(reg->bit(bitname)->extra(extra_name));
//                }
//            }
//#endif
//            else if(PyLong_Check(obj) ){
//                int n= PyLong_AsLong(obj);
//                if(n<=reg->size()){
//                    return QVariant_to_PyObject(reg->bitAt(n)->extra(extra_name));
//                }
//            }
//#if PY_MAJOR_VERSION ==2
//            else if(PyInt_Check(obj)){
//                int n= PyInt_AsLong(obj);
//                if(n<=reg->size()){
//                    return QVariant_to_PyObject(reg->bitAt(n)->extra(extra_name));
//                }
//            }
//#endif
//        }
//    }
//    Py_INCREF(Py_None);
//    return Py_None;
//}

//static PyObject* Regs_sub(PyObject *self, PyObject *args)
//{
//    // PyObject *obj;
//    // Register *reg;
//    // char *str;
//    // if(PyArg_ParseTuple(args, "Os",&obj,&str))
//    // {
//    //     if(REGISTERCONTAINER->contains((unsigned int)obj))
//    //     {
//    //         reg = REGISTERCONTAINER->reg((unsigned int)obj);
//    //         Register *psubreg = reg->sub("");
//    //         if(!REGISTERCONTAINER->containsReg(psubreg))
//    //         {
//    //             PyObject* pModule = PyImport_ImportModule("regs");
//    //             PyObject* pDict = PyModule_GetDict(pModule);
//    //             PyObject *pClass = PyDict_GetItemString(pDict, "Register");
//    //             PyObject *arg = Py_BuildValue("ss", "", "");

//    //             PyObject *pClassName = PyUnicode_FromString(name);
//    //             PyObject *pClassBases = PyTuple_New(0); // An empty tuple for bases is equivalent to `(object,)`

//    //             PyObject *pClassDic = PyDict_New();
//    //             PyObject *pInstance = PyObject_CallFunctionObjArgs(pClass, pClassName, pClassBases, pClassDic, NULL);

//    //             if(REGISTERCONTAINER->contains((int)pInstance)){
//    //                 REGISTERCONTAINER->remove((int)pInstance);
//    //                 REGISTERCONTAINER->add((int)pInstance,reg->sub(str));
//    //                 return pInstance;
//    //             }
//    //         }
//    //         else{
//    //             return (PyObject*)REGISTERCONTAINER->regId(psubreg);
//    //         }
//    //     }
//    // }
//    Py_INCREF(Py_None);
//    return Py_None;
//}

//static PyObject* Regs_subExtra(PyObject *self, PyObject *args)
//{
//    // PyObject *obj;
//    // Register *reg;
//    // char *str;
//    // char *val;
//    // if(PyArg_ParseTuple(args, "Oss",&obj,&str,&val))
//    // {
//    //     if(REGISTERCONTAINER->contains((unsigned int)obj))
//    //     {
//    //         reg = REGISTERCONTAINER->reg((unsigned int)obj);
//    //         Register *psubreg = reg->sub("");
//    //         if(!REGISTERCONTAINER->containsReg(psubreg))
//    //         {
//    //             if(reg){
//    //                 PyObject* pModule = PyImport_ImportModule("regs");
//    //                 PyObject* pDict = PyModule_GetDict(pModule);
//    //                 PyObject *pClass = PyDict_GetItemString(pDict, "Register");
//    //                 PyObject *arg = Py_BuildValue("ss", "", "");

//    //                 PyObject* pInstance = PyInstance_New(pClass, arg, NULL);

//    //                 if(REGISTERCONTAINER->contains((int)pInstance)){
//    //                     REGISTERCONTAINER->remove((int)pInstance)  ;
//    //                     REGISTERCONTAINER->add((int)pInstance, reg->sub(str,val));
//    //                     return pInstance;
//    //                 }
//    //             }
//    //         }
//    //         else{
//    //             return (PyObject*)REGISTERCONTAINER->regId(psubreg);
//    //         }
//    //     }
//    // }
//    Py_INCREF(Py_None);
//    return Py_None;
//}


//static PyObject* Regs_test(PyObject *self, PyObject *args)
//{
////    PyObject* pModule = PyImport_ImportModule("regs");
////    PyObject* pDict = PyObject_GetAttr(pModule);
////                PyObject *pClass = PyDict_GetItemString(pDict, "Register");
////                PyObject *arg = Py_BuildValue("ss", "", "");

////                PyObject *pClassName = PyUnicode_FromString("ttt");
// //               PyObject *pClassBases = PyTuple_New(0); // An empty tuple for bases is equivalent to `(object,)`

// //               PyObject *pClassDic = PyDict_New();
// //               PyObject *pInstance = PyObject_CallFunctionObjArgs(pClass, pClassName, pClassBases, arg, NULL);
// //               printf("%x\n",pInstance);

          
//    Py_INCREF(Py_None);
//    return Py_None;

//    // error exmple
//    // PyErr_SetString(PyExc_NameError, "Test");
    
//    // return NULL;
//}


static PyMethodDef regs_class_methods[] =
{
    {"__init__", Regs_init, METH_VARARGS, ""},
    {"__del__", Regs_del, METH_VARARGS, ""},
//    {"addBit", Regs_addBit, METH_VARARGS, ""},
//    {"removeBit", Regs_removeBit, METH_VARARGS, ""},
//    {"setValue", Regs_setValue, METH_VARARGS, ""},
//    {"value", Regs_value, METH_VARARGS, ""},

//    {"setBit", Regs_setBit, METH_VARARGS, ""},
//    {"__setitem__", Regs_setItem, METH_VARARGS, ""},

//    {"bit", Regs_bit, METH_VARARGS, ""},
//    {"__getitem__", Regs_getItem, METH_VARARGS, ""},

//    {"contains", Regs_contains, METH_VARARGS, ""},
//    {"__contains__", Regs_contains, METH_VARARGS, ""},
//    {"toByteArray", Regs_toByteArray, METH_VARARGS, ""},
//    {"fromByteArray", Regs_fromByteArray, METH_VARARGS, ""},
//    {"toHex", Regs_toHex, METH_VARARGS, ""},
//    {"fromHex", Regs_fromHex, METH_VARARGS, ""},
//    {"toBitString", Regs_toBitString, METH_VARARGS, ""},
//    {"fromBitString", Regs_fromBitString, METH_VARARGS, ""},
//    {"toUInt", Regs_toUInt, METH_VARARGS, ""},
//    {"setUInt", Regs_setUInt, METH_VARARGS, ""},
//    {"toString", Regs_toString, METH_VARARGS, ""},
//    {"__str__", Regs_toString, METH_VARARGS, ""},
//    {"fromString", Regs_fromString, METH_VARARGS, ""},
//    {"invert", Regs_invert, METH_VARARGS, ""},
//    {"roll", Regs_roll, METH_VARARGS, ""},
//    {"size", Regs_size, METH_VARARGS, ""},
//    {"__len__", Regs_size, METH_VARARGS, ""},
//    {"rotateLeft", Regs_rotateLeft, METH_VARARGS, ""},
//    {"__lshift__", Regs_rotateLeft, METH_VARARGS, ""},
//    {"rotateRight", Regs_rotateRight, METH_VARARGS, ""},
//    {"__rshift__", Regs_rotateRight, METH_VARARGS, ""},
//    {"crc", Regs_crc, METH_VARARGS, ""},


//    {"fill", Regs_fill, METH_VARARGS, ""},
//    {"clear", Regs_clear, METH_VARARGS, ""},
//    {"join", Regs_join, METH_VARARGS, ""},
//    {"AND", Regs_and, METH_VARARGS, ""},
//    {"__and__", Regs_and, METH_VARARGS, ""},
//    {"OR", Regs_or, METH_VARARGS, ""},
//    {"__or__", Regs_or, METH_VARARGS, ""},
//    {"XOR", Regs_xor, METH_VARARGS, ""},
//    {"__xor__", Regs_xor, METH_VARARGS, ""},
//    {"__eq__", Regs_eq, METH_VARARGS, ""},
//    {"__ne__", Regs_ne, METH_VARARGS, ""},

//    {"findBitByName", Regs_findBitByName, METH_VARARGS, ""},

//    {"items", Regs_items, METH_VARARGS, ""},// no
//    {"extras", Regs_extras, METH_VARARGS, ""},// no
//    {"setExtra", Regs_setExtra, METH_VARARGS, ""},// no
//    {"extra", Regs_extra, METH_VARARGS, ""},//no

//    //not implemented
//    {"crc", Regs_crc, METH_VARARGS, ""},// no

//    {"sub", Regs_sub, METH_VARARGS, ""},// sub by bit name
//    {"subExtra", Regs_subExtra, METH_VARARGS, ""},// sub by extra



    {0, 0},
};

static PyMethodDef regs_methods[] = {
{"test", Regs_test, METH_NOARGS, ""},
    {0, 0} };


#if PY_MAJOR_VERSION ==3

static struct PyModuleDef regs_module =
{
    PyModuleDef_HEAD_INIT,
    "Regs", /* name of module */
    "",          /* module documentation, may be NULL */
    -1,          /* size of per-interpreter state of the module, or -1 if the module keeps state in global variables. */
    regs_methods
};

PyObject *createClassObject(const char *name, PyMethodDef methods[])
{
    PyObject *pClassName = PyUnicode_FromString(name);
    PyObject *pClassBases = PyTuple_New(0); // An empty tuple for bases is equivalent to `(object,)`
    PyObject *pClassDic = PyDict_New();


    PyMethodDef *def;
    // add methods to class 
    for (def = methods; def->ml_name != NULL; def++)
    {        
        PyObject *func = PyCFunction_New(def, NULL);
        PyObject *method = PyInstanceMethod_New(func);
        PyDict_SetItemString(pClassDic, def->ml_name, method);
        Py_DECREF(func);
        Py_DECREF(method);
    }

    // pClass = type(pClassName, pClassBases, pClassDic)
    PyObject *pClass = PyObject_CallFunctionObjArgs((PyObject *)&PyType_Type, pClassName, pClassBases, pClassDic, NULL);

    Py_DECREF(pClassName);
    Py_DECREF(pClassBases);
    Py_DECREF(pClassDic);


    return pClass;
}


 PyMODINIT_FUNC
 	PyInit_regs(void)
 {


     PyObject *module = PyModule_Create( &regs_module);

    PyObject * c = createClassObject("regs", regs_class_methods);

    PyModule_AddIntConstant(module, "LSB", 1);
    PyModule_AddIntConstant(module, "MSB", 0);
    PyModule_AddIntConstant(module, "CONTINUOUS", 0);

    PyModule_AddObject(module, "Register", c );
     return module;
 }

#elif PY_MAJOR_VERSION ==2
PyMODINIT_FUNC
    initregs(void)
{
    
    /* create new module and class objects */
    PyObject *module = Py_InitModule("regs", regs_methods);
    PyObject *moduleDict = PyModule_GetDict(module);
    PyObject *classDict = PyDict_New(  );
    PyObject *className = PyString_FromString("Register");
    PyObject *fooClass = PyClass_New(NULL, classDict, className);
    PyDict_SetItemString(moduleDict, "Register", fooClass);

    PyModule_AddIntConstant(module, "LSB", 1);
    PyModule_AddIntConstant(module, "MSB", 0);
    PyModule_AddIntConstant(module, "CONTINUOUS", 0);
    PyMethodDef *def=0;
    /* add methods to class */
    for (def = regs_class_methods; def->ml_name != NULL; def++) {
        PyObject *func = PyCFunction_New(def, NULL);
        PyObject *method = PyMethod_New(func, NULL, fooClass);
        PyDict_SetItemString(classDict, def->ml_name, method);
        Py_DECREF(func);
        Py_DECREF(method);
    }
    Py_DECREF(classDict);
    Py_DECREF(className);
    Py_DECREF(fooClass);

}
#endif






