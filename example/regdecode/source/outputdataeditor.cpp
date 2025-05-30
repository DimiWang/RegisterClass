#include "outputdataeditor.h"
#include <QDebug>
#include <QMenu>

OutputDataEditor::OutputDataEditor(QWidget *parent)
    :QPlainTextEdit(parent)
{

}
void OutputDataEditor::mouseDoubleClickEvent(QMouseEvent *event)
{
    //select
    QPlainTextEdit::mouseDoubleClickEvent(event);
    //send to find
    if(event->button() == Qt::LeftButton){
        const QString text = textCursor().selectedText();
        Q_EMIT findTextField(text);
    }

}

void OutputDataEditor::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu *menu = createStandardContextMenu();
    QAction *customAction = menu->addAction("use");
    QObject::connect(customAction, SIGNAL(triggered(bool)),this, SLOT(readWords()));
     QPoint p = mapToGlobal(event->pos());
    menu->exec(p);
}

void OutputDataEditor::readWords()
{    
    QStringList wordList;
    QString text = this->textCursor().selectedText();
    QRegExp regex("[0-9\\w\\_\\.]+",Qt::CaseInsensitive);
    int pos =0;
    while((pos = regex.indexIn(text,pos))!=-1){
        wordList.append(regex.cap(0));
        pos+=regex.matchedLength();
    }
    emit selectedWords(wordList);
}
