#ifndef CUSTOMLINEEDIT_H
#define CUSTOMLINEEDIT_H

#include <QLineEdit>

/****************************************************************************
 * @class :
 * LineEdit
 ****************************************************************************/
class LineEdit : public QLineEdit
{
    Q_OBJECT
public:
    explicit LineEdit(QWidget *p);


    ~LineEdit();

    qint32 inputBase() const{return m_input_base;}

    bool editEnabled()const{return !isReadOnly();}
    void setEditEnabled(bool on){  setReadOnly(!on);}
    void setValue(quint32 v);
    quint32 value()const;
    void setInputBase(int);


    void setDescription(const QString &description);
    const QString  description();
    void setCaption(const QString & ){}
    const QString  caption(){return QString();}
    QByteArray array();
    void setArray(const QByteArray &data);
    void setErrorState(bool on);
    bool errorState() const { return m_state.error ;}

signals:
    void signal_activated();
    void  focusIn();
    void  focusOut();

protected:
    struct {
                  bool unknown;
                          bool error;
                          bool changed;
                          void reset(){
                              unknown = 1;
                              changed = 0;
                              error =0;
                          }
    }    m_state;


    qint32 m_input_base;
    bool m_allowUpdate;
    quint32 m_value;
    bool checkInputValue(qint32 *base);
    QString toText(quint32 val);
    quint32 fromText(const QString &text);

    void focusInEvent(QFocusEvent *pevent);
    void focusOutEvent(QFocusEvent *pevent);
    void contextMenuEvent(QContextMenuEvent *event);            
    void wheelEvent(QWheelEvent *pev);    
    void keyPressEvent(QKeyEvent *pev);        
    void unmark();
    bool isEdited()const {return m_state.changed ;}
    void construct();
    bool event(QEvent *e);
signals:
   void updateState();

private slots:
    void slot_TextChanged();
    void slot_triggered();
    void slot_updateState();
};


#endif
