#ifndef OUTPUTDATAEDITOR_H
#define OUTPUTDATAEDITOR_H

#include <QPlainTextEdit>

class OutputDataEditor : public QPlainTextEdit
{
    Q_OBJECT
public:
    OutputDataEditor(QWidget *parent);
signals:
    void findTextField(QString);
    void selectedWords(QStringList);

protected:
    void mouseDoubleClickEvent( QMouseEvent * e );
    void contextMenuEvent(QContextMenuEvent *event);

private slots:
    void readWords();
};

#endif // OUTPUTDATAEDITOR_H
