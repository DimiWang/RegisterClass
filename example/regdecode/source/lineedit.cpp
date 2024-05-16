#include "lineedit.h"
#include <QApplication>
#include <QMenu>
#include <QContextMenuEvent>


LineEdit::LineEdit(QWidget *p): QLineEdit(p)
{
    m_allowUpdate = false;
    setFocusPolicy(Qt::WheelFocus);

    m_input_base = 10;    
    m_state.reset();
    connect(this,SIGNAL(textChanged(QString)),this,SLOT(slot_TextChanged()));    
    connect(this,SIGNAL(updateState()), this ,SLOT(slot_updateState()), Qt::QueuedConnection);

    // send value
    connect(this,SIGNAL(returnPressed()),this,SLOT(slot_triggered()));
    connect(this,SIGNAL(editingFinished()),this,SLOT(slot_triggered()));

}

LineEdit::~LineEdit(){}

void LineEdit::setValue(quint32 v)
{    
    blockSignals(1);
    m_value = v;
    QLineEdit::setText( toText(v));
    blockSignals(0);
    m_state.changed = 0;
    emit updateState();
}

quint32 LineEdit::value() const
{
    return m_value;
}

void LineEdit::setInputBase(int input_base)
{
    quint32 v =  value();
    m_input_base = input_base;
    setValue(v);
}



void LineEdit::focusInEvent(QFocusEvent *pevent)
{
    QLineEdit::focusInEvent(pevent);
    m_allowUpdate = true;
    if (!(QApplication::mouseButtons() & Qt::LeftButton)){
        this->clearFocus();
    }
    emit focusIn();
}

void LineEdit::focusOutEvent(QFocusEvent *pevent)
{
    QLineEdit::focusOutEvent(pevent);
    m_allowUpdate = false;
     emit focusOut();
}

void LineEdit::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu menu;
    QAction *actionHex = menu.addAction("HEX");
    QAction *actionBin =  menu.addAction("BIN");
    QAction *actionDec =  menu.addAction("DEC");
    QActionGroup gr(this);
    gr.addAction(actionHex);
    gr.addAction(actionBin);
    gr.addAction(actionDec);
    actionDec->setCheckable(true);
    actionHex->setCheckable(true);
    actionBin->setCheckable(true);

        if(inputBase()==2){
            actionBin->setChecked(1);
        }
        else if(inputBase()==10)
        {
            actionDec->setChecked(1);
        }
        else if(inputBase()==16)
        {
            actionHex->setChecked(1);
        }

    menu.exec(event->globalPos());

    if (actionBin->isChecked())
    {
        setInputBase(2);
    }
    else if (actionDec->isChecked())
    {
         setInputBase(10);
    }
    else if (actionHex->isChecked())
    {
         setInputBase(16);
    }
}


void LineEdit::wheelEvent(QWheelEvent *pev)
{
    static qint32 delta = 0;
    if(!isReadOnly() && m_allowUpdate)
    {
        delta += pev->delta();

        if (delta >= 120 || delta <= -120)
        {
            setValue(this->value() + delta / 120);
            delta %= 120;
            m_state.changed =1;
            emit updateState();
        }
    }
}

void LineEdit::keyPressEvent(QKeyEvent *pev)
{
    QLineEdit::keyPressEvent(pev);
    if (pev->key() == Qt::Key_Up)
    {
        setValue(this->value() + 1);
    }
    else if (pev->key() == Qt::Key_Down)
    {
        setValue(this->value() - 1);
    }
}


void LineEdit::slot_TextChanged()
{
    if(checkInputValue(&m_input_base)){
        m_state.error =0;
    }else m_state.error =1;

    m_state.changed =1;
    emit updateState();
}

#include <QToolTip>
bool LineEdit::event(QEvent *e)
{
    if(e->type()  == QEvent::ToolTip){
        setToolTip(  QString("<font color=blue>%4</><font color=magenta>=%1"
                             "<br>0x%2"
                             "<br>0b%3</>"
                             )

                    .arg((int)m_value)
                    .arg(QString::number((int)m_value,16).toUpper())
                    .arg(QString::number((int)m_value,2))
                    .arg(this->accessibleName())                    
                    );
    }

    return QLineEdit::event(e);
}

void LineEdit::slot_triggered(){

    if(m_state.changed) {
        emit signal_activated();
        m_state.reset();
        emit updateState();
    }
}

void LineEdit::setErrorState(bool on){
     m_state.error = on;
     emit updateState();
}

bool LineEdit::checkInputValue(qint32 *base)
{
    bool ok=0;
    if(text().isEmpty()){
        m_value = 0;
        m_state.reset();
        ok =1;
    }
    else if(text().startsWith("0x")) {
        *base=16;
         m_value = (double)text().mid(2).toUInt(&ok, inputBase());
    }
    else if(text().startsWith("0b")) {
        *base = 2;
        m_value = (double)text().mid(2).toUInt(&ok, inputBase());
    }
    else {
        *base =10;
        m_value = (double)text().toUInt(&ok, inputBase());
    }
    return ok;
}

QString LineEdit::toText(quint32 val)
{
    switch(m_input_base)
    {
        case 16:
            return "0x"+QString::number((ulong)val,16).toUpper();

        case 2:
            return "0b"+QString::number((ulong)val,2).toUpper();

        case 10:
            return QString::number((ulong)val,10).toUpper();
    }
    return QString();
}

quint32 LineEdit::fromText(const QString &text)
{

    if(text.startsWith("0x")) {
         return (double)text.mid(2).toUInt(0, inputBase());
    }
    else if(text.startsWith("0b")) {
        return (double)text.mid(2).toUInt(0, inputBase());
    }
    else {
        return (double)text.toUInt(0, inputBase());
    }
    return 0;
}

void LineEdit::slot_updateState()
{
    if(m_state.error)  setStyleSheet("color:red");
    else if(m_state.changed) setStyleSheet("color:blue");
    else
        setStyleSheet("");
}

