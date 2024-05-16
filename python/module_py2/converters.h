#ifndef CONVERTERS_H
#define CONVERTERS_H
#include <QString>
#include <QStringList>
#include <QVariant>
#include <Python.h>

static QVariant QString_to_QVariant(const QString &str )
{
    QVariant result;
    char c = str[0].toLatin1();
    /* types f- float, i -integer, s- string */
    switch (c)
    {
    case ( 'f' ):
        if(str.count()>1 && str[1]=='#')
        {
            QVariantList array;
            Q_FOREACH (const QString &s,str.mid(2).split(';')){
                array.append(s.toDouble());
            }
            result = array;
        }else
            result = str.mid(1).toDouble();
        break;

    case ( 'i' ):
        //array of integer signed 32bit
        if(str.count()>1 && str[1]=='#')
        {
            QVariantList array;
            Q_FOREACH (const QString &s,str.mid(2).split(';')){
                array.append(s.toInt());
            }
            result = array;
        }
        // integer value signed 32 or 64bit
        else{
            long long tmp  = str.mid(1).toLongLong();
            if((unsigned long long) tmp &0x80000000)
                result = tmp;
            else
                result = str.mid(1).toInt();
        }
        break;


    case ( 's' ):
        result = str.mid(1);
        break;

    case ('x'):
        result = QByteArray::fromHex(str.mid(1).toLatin1());
        break;
    }
    return result;
}

static  QString QVariant_to_QString(const QVariant &var)
{
    QString result;
    switch (var.type())
    {

    case QVariant::Double:
        result = 'f' + var.toString().toLatin1();
        break;

    case QVariant::List:
    {
        int i=0;
        Q_FOREACH (const QVariant &v,var.toList())
        {
            if(i==0 && v.type()== QVariant::Int) {
                result += "i#";
            }
            else if(i==0 && v.type()== QVariant::Double){
                result +="f#";
            }
            result +=v.toString()+";";
            i++;
        }
        if(result.endsWith(";")) result.chop(1);
    }
        break;

    case QVariant::Bool:
        result = 'i' + QString::number(var.toInt()).toLatin1();
        break;

    case QVariant::ULongLong:
    case QVariant::LongLong:
    case QVariant::UInt:
    case QVariant::Int:
        result = 'i' + var.toString().toLatin1();
        break;

    case QVariant::String:
        result = 's' + var.toString().toLatin1();
        break;
    case QVariant::ByteArray:
        result = 'x' + var.toByteArray().toHex();
        break;
    default:
        break;
    }
    return result;
}


#if PY_MAJOR_VERSION ==3

static QVariant PyObject_to_QVariant(PyObject *pobj)
{    
    if(!pobj || pobj == Py_None) return QVariant();    
    if(PyUnicode_Check(pobj)){        
        return QString(PyUnicode_AsUTF8(pobj));
    }
    if(PyBytes_Check(pobj)){
         return QString(PyUnicode_AsUTF8(pobj));
    }
    else if(PyLong_Check(pobj) ){ 
        return PyLong_AsLong(pobj);
    }
    else if(PyFloat_Check(pobj)){        
        return QVariant(PyFloat_AsDouble(pobj));
    }
    else if(PyDict_Check(pobj)){
        QVariantMap tuple;
        PyObject *key, *value;
        Py_ssize_t pos = 0;
        while (PyDict_Next(pobj, &pos, &key, &value))
        {

            /* do interesting things here */
            // key
            QString skey;
            if(PyUnicode_Check(key)){
                skey = PyUnicode_AsUTF8(key);
            }
            else continue; //ignore if somethig not string

            //value
            QVariant svar = PyObject_to_QVariant(value) ;

            if(!skey.isEmpty() && !svar.isNull())
            {
                tuple[skey] = svar;
            }
        }
        return tuple;
    }
    else if(PyList_Check(pobj) || PyTuple_Check(pobj)){
        QVariantList list;
        int n = PyList_Size(pobj);
        for (int i=0; i<n; i++) {
            PyObject *pItem = PyList_GetItem(pobj, i);
            list.append(PyObject_to_QVariant(pItem));
        }        
        return list;
    }
    else if(PyByteArray_Check(pobj))
    {
        printf("[bytearray]");
        PyObject *p_pyByteArray = PyByteArray_FromObject(pobj);
        int buf_len = PyByteArray_Size(p_pyByteArray);
        char *buf = PyByteArray_AsString(p_pyByteArray);
        return QByteArray(buf,buf_len);
    }
    return QVariant();
}

static  PyObject* QVariant_to_PyObject(const QVariant &var)
{
    PyObject *result=0;
    if(var.isNull()){
        Py_INCREF(Py_None);
        return Py_None;
    }
    if(var.isNull()){
        result = Py_None;
    }
    if(var.type() == QVariant::Int || var.type() == QVariant::Bool || var.type() == QVariant::UInt)
    {
        result = Py_BuildValue("i",(int)var.toInt());
    }
    if(var.type() == QVariant::LongLong)
    {
        result= Py_BuildValue("L",var.toLongLong());
    }
    else if(var.type() == QVariant::String)
    {
        result = Py_BuildValue("s",var.toByteArray().constData());
    }
    else if(var.type() == QVariant::ByteArray)
    {
        result = Py_BuildValue("s#",var.toByteArray().constData()
                               ,var.toByteArray().size());
    }
    else if(var.type() == QVariant::Double)
    {
        result = Py_BuildValue("d",var.toDouble());
    }
    else if(var.type() == QVariant::List || var.type() == QVariant::StringList){ // list of QVariant
        QVariantList l = var.toList();
        PyObject *list = PyList_New(l.count());
        int i=0;
        Q_FOREACH (const QVariant &item, l)
        {
            PyObject *t = QVariant_to_PyObject(item);
            if(PyList_SetItem(list, i++, t)!=0)
            {
                break;
            }
        }
        result = list;
    }
    else if(var.type() == QVariant::Map){ // dict QString , QVariant
        QVariantMap m = var.toMap();
        PyObject *dict = PyDict_New();
        Q_FOREACH (const QString & name, m.keys()){
            PyDict_SetItem(dict, Py_BuildValue("s",name.toLatin1().constData()),QVariant_to_PyObject(m[name]));
        }
        result = dict;
    }
    return result;
}

#elif PY_MAJOR_VERSION==2

static QVariant PyObject_to_QVariant(PyObject *pobj)
{
    if(!pobj || pobj == Py_None) return QVariant();

    if(PyString_Check(pobj)){
        return QString(PyString_AsString(pobj));
    }
    else if(PyLong_Check(pobj) || PyInt_Check(pobj)){
        return PyLong_AsLong(pobj);
    }
    else if(PyFloat_Check(pobj)){
        return QVariant(PyFloat_AsDouble(pobj));
    }
    else if(PyDict_Check(pobj)){
        QVariantMap tuple;
        PyObject *key, *value;
        Py_ssize_t pos = 0;
        while (PyDict_Next(pobj, &pos, &key, &value))
        {

            /* do interesting things here */
            // key
            QString skey;
            if(PyString_Check(key)){
                skey = PyString_AsString(key);
            }
            else continue; //ignore if somethig not string

            //value
            QVariant svar = PyObject_to_QVariant(value) ;

            if(!skey.isEmpty() && !svar.isNull())
            {
                tuple[skey] = svar;
            }
        }
        return tuple;
    }
    else if(PyList_Check(pobj) || PyTuple_Check(pobj)){
        QVariantList list;
        int n = PyList_Size(pobj);
        for (int i=0; i<n; i++) {
            PyObject *pItem = PyList_GetItem(pobj, i);
            list.append(PyObject_to_QVariant(pItem));
        }        
        return list;
    }
    else if(PyByteArray_Check(pobj))
    {
        PyObject *p_pyByteArray = PyByteArray_FromObject(pobj);
        int buf_len = PyByteArray_Size(p_pyByteArray);
        char *buf = PyByteArray_AsString(p_pyByteArray);
        return QByteArray(buf,buf_len);
    }
    return QVariant();
}

static  PyObject* QVariant_to_PyObject(const QVariant &var)
{
    PyObject *result=0;
    if(var.isNull()){
        Py_INCREF(Py_None);
        return Py_None;
    }
    if(var.isNull()){
        result = Py_None;
    }
    if(var.type() == QVariant::Int || var.type() == QVariant::Bool || var.type() == QVariant::UInt)
    {
        result = Py_BuildValue("i",(int)var.toInt());
    }
    if(var.type() == QVariant::LongLong)
    {
        result= Py_BuildValue("L",var.toLongLong());
    }
    else if(var.type() == QVariant::String)
    {
        result = Py_BuildValue("s",var.toByteArray().constData());
    }
    else if(var.type() == QVariant::ByteArray)
    {
        result = Py_BuildValue("s#",var.toByteArray().constData()
                               ,var.toByteArray().size());
    }
    else if(var.type() == QVariant::Double)
    {
        result = Py_BuildValue("d",var.toDouble());
    }
    else if(var.type() == QVariant::List || var.type() == QVariant::StringList){ // list of QVariant
        QVariantList l = var.toList();
        PyObject *list = PyList_New(l.count());
        int i=0;
        foreach(const QVariant &item, l)
        {
            PyObject *t = QVariant_to_PyObject(item);
            if(PyList_SetItem(list, i++, t)!=0)
            {
                break;
            }
        }
        result = list;
    }
    else if(var.type() == QVariant::Map){ // dict QString , QVariant
        QVariantMap m = var.toMap();
        PyObject *dict = PyDict_New();
        foreach(const QString & name, m.keys()){
            PyDict_SetItem(dict, Py_BuildValue("s",name.toLatin1().constData()),QVariant_to_PyObject(m[name]));
        }
        result = dict;
    }
    return result;
}
#endif
#endif 
