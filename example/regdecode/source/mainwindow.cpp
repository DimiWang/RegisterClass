
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QScrollBar>
#include <QDebug>
#include <QMessageBox>
#include <QElapsedTimer>
#include "lineedit.h"
#include <QFileDialog>
#include <QtMath>
#include <QFile>
#include <QSettings>
#include <QFileInfo>
#include <QtEndian>
#include <QInputDialog>
#include <QAction>
#include <QTextBrowser>
#include <QLabel>
#include "dlgfield.h"
#include "mainwindow.h"
#include <QFile>
#include <QDate>

const char * MainWindow::m_data_formats[]={"HEX_U32", "HEX_U8", "VMEM", "LIST", "BINARY", "CDE", "CCODE"};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QFont f = ui->teResult->font();
    f.setFamily("Consolas");
    ui->teResult->setFont(f);
   // m_map.blockSignals(1);//block forever
    ui->teConsole->setVisible(0);
    ui->pbReloadDataFile->setEnabled(0);
    // save and load action
    setupActions();
    ui->tbLoadFileAs->setDefaultAction(actLoadFileAsList[0]);
    ui->tbSaveFileAs->setDefaultAction(actSaveFileAsList[0]);
    m_data_file_path = QDir::currentPath();
    m_structure_file_path = QDir::currentPath();

    loadSettings();
    loadRecentFiles();


    actStructFileSave = new QAction("Save",0);
    QObject::connect(actStructFileSave,SIGNAL(triggered(bool)),this,SLOT(saveStructureFile()));
    actStructFileSaveAs = new QAction("SaveAs",0);
    QObject::connect(actStructFileSaveAs,SIGNAL(triggered(bool)),this,SLOT(saveAsStructureFile()));
    actStructFileRemove = new QAction("Remove",0);
    QObject::connect(actStructFileRemove,SIGNAL(triggered(bool)),this,SLOT(removeStructureFile()));

    ui->tbStructureButton->addAction(actStructFileSave);
    ui->tbStructureButton->addAction(actStructFileSaveAs);
    ui->tbStructureButton->addAction(actStructFileRemove);
    ui->tbStructureButton->setDefaultAction(actStructFileSave);

    QObject::connect(this,SIGNAL(update_ui()),this,SLOT(updateItemsList()),Qt::QueuedConnection);
    QObject::connect(this,SIGNAL(update_ui_controls()),this,SLOT(updateUiControls()),Qt::QueuedConnection);

    m_structure_file_path =  DataFormatUnknown;    

    ui->scrollArea->widget()->setLayout(new QVBoxLayout());
    ui->scrollArea->widget()->setContentsMargins(1,1,1,1);
    m_ui_spacer =0;
    ui->tabWidget->removeTab(0);//fix

    Q_EMIT update_ui_controls();

    connect(&m_map,SIGNAL(signal_Warning(QString)),this,SLOT(warning(QString)),Qt::QueuedConnection);
    connect(&m_map,SIGNAL(signal_Error(QString)),this,SLOT(error(QString)),Qt::QueuedConnection);
    connect(this,SIGNAL(sig_updateConsoleIcon()),this,SLOT(updateConsoleIcon()),Qt::QueuedConnection);
    m_cursor_position =0;

    QPalette p = ui->teResult->palette();
    p.setColor(QPalette::Highlight, QColor(Qt::blue));
    p.setColor(QPalette::HighlightedText, QColor(Qt::white));
    ui->teResult->setPalette(p);
    ui->wgFindText->setVisible(0);
    connect(ui->teResult,SIGNAL(findTextField(QString)),this,SLOT(findTextItem_InMap(QString)));
    ui->cbCaseSens->setChecked(1);    
    ui->tabWidget->setCurrentIndex(0);
    connect(ui->leValue, SIGNAL(formatChanged(QString)), this, SLOT(updateEditFieldFormat(QString)));
    connect(ui->teResult, SIGNAL(selectedWords(QStringList)),this, SLOT(selectEditFields(QStringList)));
}

void MainWindow::setupActions()
{
    //-----
    for(int i=0;i<10;i++){
        actLoadFileAsList[i] =0;
        actSaveFileAsList[i] =0;
    }

        QAction *action;
        action = new QAction("as ASCII U32",0);
        action->setObjectName("ASCII_U32");
        ui->tbSaveFileAs->addAction(action);
        QObject::connect(action,SIGNAL(triggered(bool)),this,SLOT(saveFileAsU32()));
        actSaveFileAsList[0] = action;

        action = new QAction("as ASCII U8",0);
        action->setObjectName("ASCII_U8");
        ui->tbSaveFileAs->addAction(action);
        QObject::connect(action,SIGNAL(triggered(bool)),this,SLOT(saveFileAsU8()));
        actSaveFileAsList[1] = action;

        action = new QAction("as Verilog MEM",0);
        action->setObjectName("ASCII_MEM");
        ui->tbSaveFileAs->addAction(action);
        QObject::connect(action,SIGNAL(triggered(bool)),this,SLOT(saveFileAsVMEM()));
        actSaveFileAsList[2] = action;

        action = new QAction("as LIST",0);
        action->setObjectName("LIST");
        ui->tbSaveFileAs->addAction(action);
        QObject::connect(action,SIGNAL(triggered(bool)),this,SLOT(saveFileAsList()));
        actSaveFileAsList[3] = action;

        action = new QAction("as BIN",0);
        action->setObjectName("BIN");
        ui->tbSaveFileAs->addAction(action);
        QObject::connect(action,SIGNAL(triggered(bool)),this,SLOT(saveFileAsBin()));
        actSaveFileAsList[4] = action;

        action = new QAction("as CDE",0);
        action->setObjectName("CDE");
        ui->tbSaveFileAs->addAction(action);
        QObject::connect(action,SIGNAL(triggered(bool)),this,SLOT(saveFileAsCde()));
        actSaveFileAsList[5]= action;

        action = new QAction("as C code",0);
        action->setObjectName("CCODE");
        ui->tbSaveFileAs->addAction(action);
        QObject::connect(action,SIGNAL(triggered(bool)),this,SLOT(saveFileAsCCode()));
        actSaveFileAsList[6]= action;


        //default


        action = new QAction("as ASCII U32",0);
        action->setObjectName("ASCII_U32");
        ui->tbLoadFileAs->addAction(action);
        QObject::connect(action,SIGNAL(triggered(bool)),this,SLOT(loadFileAsU32()));
        actLoadFileAsList[0] = action;

        action = new QAction("as ASCII U8",0);
        action->setObjectName("ASCII_U8");
        ui->tbLoadFileAs->addAction(action);
        QObject::connect(action,SIGNAL(triggered(bool)),this,SLOT(loadFileAsU8()));
        actLoadFileAsList[1] = action;

        action = new QAction("as Verilog MEM",0);
        action->setObjectName("ASCII_MEM");
        ui->tbLoadFileAs->addAction(action);
        QObject::connect(action,SIGNAL(triggered(bool)),this,SLOT(loadFileAsVMEM()));
        actLoadFileAsList[2] = action;

        action = new QAction("as List",0);
        action->setObjectName("LIST");
        ui->tbLoadFileAs->addAction(action);
        QObject::connect(action,SIGNAL(triggered(bool)),this,SLOT(loadFileAsList()));
        actLoadFileAsList[3] = action;

        action = new QAction("as BIN",0);
        action->setObjectName("BIN");
        ui->tbLoadFileAs->addAction(action);
        QObject::connect(action,SIGNAL(triggered(bool)),this,SLOT(loadFileAsBin()));
        actLoadFileAsList[4] = action;

        action = new QAction("as CDE",0);
        action->setObjectName("CDE");
        ui->tbLoadFileAs->addAction(action);
        QObject::connect(action,SIGNAL(triggered(bool)),this,SLOT(loadFileAsCde()));
        actLoadFileAsList[5] = action;

        action = new QAction("Set");
        ui->tbEditFieldOper->addAction(action);
        QObject::connect(action,SIGNAL(triggered(bool)),this,SLOT(setSelectedFieldValue()));
        ui->tbEditFieldOper->setDefaultAction(action);

        action = new QAction("Read");
        ui->tbEditFieldOper->addAction(action);
        QObject::connect(action,SIGNAL(triggered(bool)),this,SLOT(getSelectedFieldValue()));

        action = new QAction("11111111");
        ui->tbEditFieldOper->addAction(action);
        QObject::connect(action,SIGNAL(triggered(bool)),this,SLOT(fillMap1()));

        action = new QAction("00000000");
        ui->tbEditFieldOper->addAction(action);
        QObject::connect(action,SIGNAL(triggered(bool)),this,SLOT(fillMap0()));


}

MainWindow::~MainWindow()
{
    delete ui;
}

QString wordWrap(const QString &text, int maxSymbols) {
    QStringList wrappedLines;
    for(const QString &textLine: text.split('\n')){
        if(textLine.size()>maxSymbols) {
            QStringList words = textLine.split(' ');
            QString line;
            for (const QString &word : words) {
                if (line.length() + word.length() + 1 > maxSymbols) {
                    wrappedLines.append(line.trimmed());
                    line = word + ' ';
                } else {
                    line += word + ' ';
                }
            }
            if (!line.isEmpty()) {
                wrappedLines.append(line.trimmed());
            }
        }
        else{
            wrappedLines.append(textLine);
        }
    }
    return wrappedLines.join('\n');
}

QAction *MainWindow::actionByName(const QString &oper,const QString &action_name)
{
    if(oper=="Save"){
        for(int i=0;i<10;i++){
            if(actSaveFileAsList[i] == 0) break;
            if(actSaveFileAsList[i]->objectName() == action_name){
                return actSaveFileAsList[i];
            }
        }
    }else if(oper=="Load"){
        for(int i=0;i<10;i++){
            if(actLoadFileAsList[i] == 0) break;
            if(actLoadFileAsList[i]->objectName() == action_name){
                return actLoadFileAsList[i];
            }
        }

    }
    return 0;
}


void MainWindow::analyzeChanges()
{

}

void MainWindow::loadSettings()
{
    QSettings settings(QString("%2/%1.ini").arg(qApp->applicationName()).arg(qApp->applicationDirPath()),QSettings::IniFormat);
    m_settings_ascii_windows = settings.value("General/ascii_windows_crlf",false).toBool();        
    ui->cmDefaultFieldValue->setCurrentIndex( settings.value("General/default_value",0/*off*/).toInt());
    ui->cbUseWindowsCRLF->blockSignals(1);
    ui->cbUseWindowsCRLF->setChecked(m_settings_ascii_windows);
    ui->cbUseWindowsCRLF->blockSignals(0);

    settings.beginGroup("Repr");
    ui->cmBitRepr->setCurrentIndex(settings.value("Format").toInt());
    ui->cbDescr->setChecked(settings.value("ShowDescr").toBool());
    ui->cbTrim->setChecked(settings.value("TrimValues").toBool());
    settings.endGroup();
    settings.beginGroup("State");
    QAction *action = actionByName("Save",settings.value("DataSaveButton").toString());
    if(action!=0){
        ui->tbSaveFileAs->setDefaultAction(action);
    }
    action = actionByName("Load",settings.value("DataLoadButton").toString());
    if(action!=0){
        ui->tbLoadFileAs->setDefaultAction(action);
    }
    settings.endGroup();

    settings.beginGroup("Data_Files");
    DataFormat data_format =     static_cast<DataFormat>(settings.value("LastDataFileFormat").toInt());
    QString data_filename =settings.value("LastDataFile").toString();
    settings.endGroup();
    setLastDataFile(data_format,data_filename);

}

void MainWindow::saveSettings()
{
    QSettings settings(QString("%2/%1.ini").arg(qApp->applicationName()).arg(qApp->applicationDirPath()),QSettings::IniFormat);
    settings.setValue("General/ascii_windows_crlf", m_settings_ascii_windows );
    settings.setValue("General/default_value", ui->cmDefaultFieldValue->currentIndex() );
    // ----------------
    settings.beginGroup("Repr");
    settings.setValue("Format",ui->cmBitRepr->currentIndex());
    settings.setValue("ShowDescr",ui->cbDescr->isChecked());
    settings.setValue("TrimValues",ui->cbTrim->isChecked());
    settings.endGroup();
    settings.beginGroup("State");
    settings.setValue("DataSaveButton",ui->tbSaveFileAs->defaultAction()->objectName());
    settings.setValue("DataLoadButton",ui->tbLoadFileAs->defaultAction()->objectName());
    settings.endGroup();
}

void MainWindow::loadRecentFiles(){
    QSettings settings(QString("%2/%1.ini").arg(qApp->applicationName()).arg(qApp->applicationDirPath()));

    const QVariantList files_list = settings.value("Files/RecentFiles",QVariant()).toList();
    bool save_again = false;
    QString bak_itemname = ui->cmStructure->currentText();

    ui->cmStructure->blockSignals(1);
    ui->cmStructure->clear();
    ui->cmStructure->addItem("Load...");
    m_recent_files.clear();

    for(int i=0;i<files_list.count();i++){
        QFileInfo fi(files_list.at(i).toString());
        if(QFile::exists(fi.filePath())){
            m_recent_files[fi.baseName()]=fi.filePath();
        }
        else save_again = true;
    }


    ui->cmStructure->addItems(m_recent_files.keys());

    if(!bak_itemname.isEmpty() && ui->cmStructure->findText(bak_itemname))
        setCurrentRecentStructure(bak_itemname);

    ui->cmStructure->blockSignals(0);


    if(save_again){
        saveRecentFiles();
    }


}

void MainWindow::setCurrentRecentStructure(const QString &filename)
{
    int index = ui->cmStructure->findText(QFileInfo(filename).baseName());
    if(index >=0){
        ui->cmStructure->setToolTip(filename);
        ui->cmStructure->setCurrentIndex(index);
    }
}

void MainWindow::on_pbApply_clicked()
{                
    m_map.clear();
    applyStructure();       
    Q_EMIT update_ui();    
    update_output();    
}

void MainWindow::applyStructure(){
    QElapsedTimer t;
    t.start();
    quint32 load_options =0;

    m_map.clear();    
    if(!m_map.loadJsonData(ui->teRegister->toPlainText().toLatin1(),load_options)){
        QMessageBox::critical(0,"JSON Error","Error parsing JSON data");
        ui->pbConsole->setEnabled(1);
    }

    ui->lbFieldInfo->setText("");
    buildEditFieldList();    
}

static QString textAlign(const QString &text, int val, const QString &space = "&nbsp;"){

    QString result = text;
    const int html_space_len = space.size();
    const int space_count = text.count(space);
    const int char_count = text.count()- space_count*html_space_len;
    for(int i=(char_count+space_count);i<val;i++){
        result.append(space);
    }
    return result;
}
void textFormat( QString *text){

    QRegExp rx("\\@align\\((\\d+)\\)");
    int pos;

    if((pos = rx.indexIn(*text)) >= 0){
        text->remove(pos, rx.matchedLength());
        for(int i=0;i<(rx.cap(1).toInt()-pos);i++){
            text->insert(pos, ' ');
        }
    }
}

bool MainWindow::loadData(const QString &file_name, DataFormat format){

    QFile f;
    if(file_name.isEmpty() || !QFile::exists(file_name)) return false;

    f.setFileName(file_name);
    if(QFile::exists(file_name) && f.open(QFile::ReadOnly)){
        QByteArray file_data =f.readAll();
        switch(format){

        case DataFormatUnknown:
            break;

        case DataFormatAsU32:{// ascii 32
            quint32 addr=0;
            QByteArrayList lines;
            if(file_data.contains(QByteArray("\r\n"))){
                lines = file_data.replace("\r\n","\n").split('\n');
            }else{
                lines = file_data.split('\n');
            }
            m_map.fill(1);
            for(int i=0;i<lines.count();i++){
                bool ok= false;
                const quint32 value = lines[i].toUInt(&ok,16);
                if(!ok) break;
                if(addr<(quint32)m_map.size()){
                    m_map.setValue(addr,addr+32-1,value);
                }
                else{
                    m_map.addField( QString("_word_%3[%1]=%2").arg(32).arg(value).arg(i));
                }
                addr+=32;
            }            
        }
            break;

        case DataFormatAsU8:{ //ascii 8
            quint32 addr=0;
            QByteArrayList lines;
            if(file_data.contains(QByteArray("\r\n"))){
                lines = file_data.replace("\r\n","\n").split('\n');
            }else{
                lines = file_data.split('\n');
            }
            m_map.fill(1);
            for(int i=0;i<lines.count();i++){
                bool ok= false;
                const quint32 value = lines[i].toUInt(&ok,16);
                if(!ok) break;
                if(addr<(quint32)m_map.size()){
                    m_map.setValue(addr,addr+8-1,value);
                }
                else{
                    m_map.addField( QString("_byte_%3[%1]=%2").arg(8).arg(value).arg(i));
                }
                addr+=8;
            }            
        }
            break;

        case DataFormatAsVMEM:
        {
            QStringList lines = QString(file_data).split("\n", QString::SkipEmptyParts);
            for(int i=0;i<lines.count();i++){
                QRegExp rx("@([\\w]+)\\s([\\w]+)\\s([\\w]+)\\s([\\w]+)\\s([\\w]+)\\s([\\w]+)\\s([\\w]+)\\s([\\w]+)\\s([\\w]+)");            
                if(rx.indexIn(lines[i]) >= 0){
                    quint32 start_addr = rx.cap(1).toUInt(0,16);                    
                    for(int i=0;i<8;i++){
                        const qint32 bit_from = start_addr*32;
                        const qint32 bit_to = (start_addr+1)*32-1;
                        m_map.setValue(bit_from, bit_to, rx.cap(2+i).toUInt(0,16));
                        start_addr += 8;
                    }
                }
                else continue;
            }            
        }
        break;

        case DataFormatAsBinary:{ // binary
            if(file_data.count() *8 >m_map.size()){
                const QString the_rest = QString("_the_rest_[%1]").arg( file_data.count()*8 - m_map.size() );
                m_map.addField(the_rest);
            }
            m_map.fromByteArray(file_data);           
        }
            break;

        case DataFormatAsCDE:{ // FPGA CDE
            if(file_data.count() *8 >m_map.size()){
                const QString the_rest = QString("_the_rest_[%1]").arg( file_data.count()*8 - m_map.size() );
                m_map.addField(the_rest);
            }
            QStringList lines = QString(file_data).split("\n",QString::SkipEmptyParts);
            for(int i=0;i<lines.count();i++){

                QRegExp rx1("([0-9a-zA-Z]{8})\\s{1}([0-9a-xA-Z]{3})([0-9a-zA-Z]{32})");
                if(rx1.indexIn(lines[i])>=0){
                    QByteArray tmp = rx1.cap(3).toLatin1();
                    for(int word=4;word>0;word--){
                        const quint32 from = i*128 + word*32-1;
                        const quint32 to = i*128 + (word-1) *32;
                        m_map.sub(from,to)->setValue(tmp.mid(0,8).toUInt(0,16));
                        tmp.remove(0,8);
                    }
                }
            }                     
        }
            break;
        case DataFormatAsLIST:{
            QStringList lines = QString(file_data).split("\n",QString::SkipEmptyParts);
            for(int i=0;i<lines.count();i++){
                QRegExp rx1("([\\.\\w]+)=0x(\\w+)"),rx2("([\\.\\w]+)=\\[(\\w+)\\]");
                if(rx1.exactMatch(lines[i])){
                    BitField *field = m_map.field(rx1.cap(1));
                    if(field)
                        field->setValue(rx1.cap(2).toUInt(0,16));
                }else if(rx2.exactMatch(lines[i])){
                    BitField *field = m_map.field(rx2.cap(1));
                    if(field)
                        field->fromHex(rx2.cap(2));
                }
                else break;
            }            
        }
            break;

        }//switch


        ui->tbLoadFileAs->setDefaultAction(actLoadFileAsList[format]);
        setLastDataFile(format,QFileInfo(f).filePath());


        Q_EMIT update();

    }else{
        QMessageBox::critical(0,"reading",QString("Can't open file"));
        return false;
    }
    return true;
}

void MainWindow::setLastDataFile(DataFormat format, const QString &filename)
{
    if(format == DataFormatUnknown && filename.isEmpty()){
        ui->pbReloadDataFile->setEnabled(0);
        ui->pbReloadDataFile->setText("...");
        ui->pbReloadDataFile->setToolTip("");

    }else if(QFile::exists(filename)){
        m_data_file_path = filename;
        m_data_file_format = format;
        ui->pbReloadDataFile->setEnabled(1);
        ui->pbReloadDataFile->setText(QString("reload:%1").arg(QFileInfo(filename).fileName()));
        ui->pbReloadDataFile->setToolTip(QString("[%1]:%2").arg(m_data_formats[format]).arg(filename));
        QFontMetrics fm(ui->pbReloadDataFile->font());
        int width = fm.horizontalAdvance(ui->pbReloadDataFile->text()) + ui->pbReloadDataFile->width()/10;
        ui->pbReloadDataFile->setMinimumWidth(width +10);
        //save last file
        QSettings settings(QString("%2/%1.ini").arg(qApp->applicationName()).arg(qApp->applicationDirPath()),QSettings::IniFormat);
        settings.beginGroup("Data_Files");
        settings.setValue("LastDataFile",m_data_file_path);
        settings.setValue("LastDataFileFormat",m_data_file_format);
        settings.endGroup();
    }
}

bool MainWindow::saveData(const QString &file_name, DataFormat format)
{
    bool result =false;
    QFile f(file_name);
    if(file_name.isEmpty() ) return false;

    if( f.open(QFile::WriteOnly)){
        QByteArray data_to_save;
        switch(format){

        case DataFormatUnknown:
            break;

        case DataFormatAsCDE:{
                const quint32 sector_size_bits = 32*4;//128 bits
                quint32 sector_n=0;

                quint32 count_sectors =(quint32)m_map.size()/sector_size_bits
                        +((m_map.size()%sector_size_bits)>0);
                for(sector_n=0;sector_n<count_sectors;sector_n++){
                    QByteArray line;
                    line += QString("@%1").arg(sector_n, 8, 16, QChar('0')).toLatin1();
                    line += " 000"; //ecc
                    for(int i=4;i>0;i--){
                        const quint32 addr_from = sector_n*128 + 32*i - 1;
                        const quint32 addr_to = sector_n*128 + 32*(i-1);
                        line += QString("%1").arg(m_map.value(addr_from,addr_to),8,16,QChar('0'));

                    }
                    data_to_save += line;
                    if(m_settings_ascii_windows){
                        data_to_save +=QString("\r\n").toLatin1();
                    }
                    else {
                        data_to_save += QString("\n").toLatin1();
                    }
                }

                result = true;
            }
            break;

        case DataFormatAsU32:{
            quint32 i=0;
            const quint32 size_in_words =(quint32)m_map.size()/32;
            while(i<size_in_words){
                const qint32 addr_from =(qint32)i*32;
                const qint32 addr_to =(qint32)(i+1)*32 -1;
                const quint32 value = m_map.sub(addr_from, addr_to)->toUInt();
                if(m_settings_ascii_windows){
                    data_to_save +=QString("%1\r\n").arg(value,8,16,QChar('0')).toLatin1();
                }
                else {
                    data_to_save += QString("%1\n").arg(value,8,16,QChar('0')).toLatin1();
                }
                i++;

                // trim lines in ascii file
                if(ui->cmTrimLines->currentIndex()>0 && i >= ui->cmTrimLines->currentText().toUInt())
                    break;
            }            
            result = true;
        }
            break;

        case DataFormatAsVMEM:
        {
            const quint32 sector_size = 32*8;
            quint32 byte_addr=0;
            quint32 count_sectors =(quint32)m_map.size()/sector_size
                    +((m_map.size()%sector_size)>0);
            const quint32 end_addr = (quint32)m_map.size()/32;
            while(count_sectors--){

                data_to_save +=QString("@%1").arg(byte_addr,8,16,QChar('0')).toLatin1();
                for(int i=0;i<8;i++){
                    const qint32 bit_from =(qint32)byte_addr*32;
                    const qint32 bit_to =(qint32)(byte_addr+1)*32 -1;
                    if(bit_to < m_map.count()) {
                        const quint32 value = m_map.sub(bit_from, bit_to)->toUInt();
                        data_to_save += QString(" %1").arg(value,8,16,QChar('0')).toLatin1();
                    }else{
                        data_to_save +=" 00000000";
                    }
                    byte_addr += 32;
                }

                if(m_settings_ascii_windows){
                    data_to_save +=QString("\r\n").toLatin1();
                }
                else {
                    data_to_save += QString("\n").toLatin1();
                }
            }

            result = true;
        }
        break;

        case DataFormatAsU8:{
            qint32 i=0;
            const quint32 size_in_bytes =(quint32)m_map.size()/32;
            while(i<size_in_bytes){

                const qint32 addr_from =(qint32)i*8;
                const qint32 addr_to =(qint32)(i+1)*8 -1;
                const uint value = m_map.sub(addr_from,addr_to)->toUInt();
                if(m_settings_ascii_windows){
                    data_to_save += QString("%1\r\n").arg(value,2,16,QChar('0')).toLatin1();
                }
                else data_to_save += QString("%1\n").arg(value,2,16,QChar('0')).toLatin1();
                i++;

                // trim lines in ascii file
                if(ui->cmTrimLines->currentIndex()>0 && i >= ui->cmTrimLines->currentText().toUInt())
                    break;
            }            
        }
            break;

        case DataFormatAsBinary:
            data_to_save  = m_map.toByteArray(Register::LSB);            
            break;

        case DataFormatAsLIST:{
            const QStringList fields_list = m_map.fieldsList();
            for(int i=0;i<fields_list.count();i++){

                BitField *field = m_map.field(fields_list[i]);
                if(field->name().startsWith("_")) continue;                
                BitField field_filled = *m_map.field(fields_list[i]);
                bool ignore = false;
                if(ui->cmDefaultFieldValue->currentIndex()==1){
                    field_filled.fill(0);
                    ignore = (field_filled.toHex() ==  field->toHex());
                }else if(ui->cmDefaultFieldValue->currentIndex()==2){
                    field_filled.fill(1);
                    ignore = (field_filled.toHex() ==  field->toHex());
                }

                if(!ignore){
                    if(field->size()>32){
                        data_to_save += QString("%1=[%2]\n").arg(field->name()).arg(field->toHex()).toLatin1();
                    }else{
                        if(field->size()%4==0){
                            data_to_save += QString("%1=0x%2\n").arg(field->name()).arg(field->value(),field->size()/4,16,QChar('0')).toLatin1();
                        }else{
                            data_to_save += QString("%1=0x%2\n").arg(field->name()).arg(field->value(),field->size()/4+1,16,QChar('0')).toLatin1();
                        }
                    }
                }
            }

        }
            break;

        case DataFormatAsCCODE:{
            const QString base_name = QFileInfo(this->m_structure_file_path).baseName();
            const QByteArray data = m_map.toByteArray();
            data_to_save += QString("const unsigned char %1[]={\n").arg(base_name).toLatin1();
            for(int i=0;i<data.size();i++){
                if(i>0){
                    if(i%8 ==0) data_to_save += "\n";
                    data_to_save +=",";
                }
                data_to_save += QString("0x%1").arg((unsigned char)data[i],2,16,QChar('0')).toLatin1();
            }
            data_to_save += "\n};\n";
            data_to_save += QString("unsigned int %1_len = %2;").arg(base_name).arg(data.size()).toLatin1();
            }
            break;

        }//switch

        result = f.write(data_to_save) == data_to_save.size();
        if(result){
            ui->tbSaveFileAs->setDefaultAction(actSaveFileAsList[format]);
            m_data_file_path = QFileInfo(f).filePath();
            m_data_file_format = format;
            ui->pbReloadDataFile->setEnabled(1);
        }
        f.close();
    }
    return result;
}

int MainWindow::selectFieldsByPath(const QString &path, QStringList *fields){
    QStringList field_list = m_map.fieldsList();
    for(int i=0;i<field_list.count();i++){
        QRegExp rx(path +".*");
        if(rx.exactMatch(field_list.at(i))){
            fields->append(field_list.at(i));
        }
    }
}

void MainWindow::findTextInData()
{
    int found_count = 0;

    if(ui->cbCaseSens->isChecked())
        found_count = ui->teResult->toPlainText().count(ui->leFindText->text(),Qt::CaseSensitive);
            else found_count = ui->teResult->toPlainText().count(ui->leFindText->text(),Qt::CaseInsensitive);



    if(found_count>0){
        ui->lbFindCount->setText(QString("found:%1").arg(found_count));
        ui->lbFindCount->setStyleSheet("");

        QTextCursor cursor ;
        if(ui->cbCaseSens->isChecked())
            cursor = ui->teResult->document()->find(ui->leFindText->text(),m_cursor_position,QTextDocument::FindCaseSensitively);
        else cursor = ui->teResult->document()->find(ui->leFindText->text(),m_cursor_position,0);

        if(cursor.position()<0){
            m_cursor_position = 0;
            cursor = ui->teResult->document()->find(ui->leFindText->text(),0);
        }
        ui->teResult->setTextCursor(cursor);
    }
    else {
        ui->lbFindCount->setText(QString("not found"));
        ui->lbFindCount->setStyleSheet("color:red");
    }

}

void MainWindow::updateConsoleIcon()
{
    ui->pbConsole->setIcon(QIcon(":/console_i.png"));
}


void MainWindow::saveRecentFiles(){
    QSettings settings(QString("%2/%1.ini").arg(qApp->applicationName()).arg(qApp->applicationDirPath()),QSettings::IniFormat);
    const QStringList list = m_recent_files.values();
    settings.setValue("Files/RecentFiles",list);
}

void MainWindow::addRecentFile(const QString &filename)
{
    QFileInfo fi(filename);
    const QString key = fi.baseName();
    m_recent_files[key] =fi.filePath();
    ui->cmStructure->addItem(key);
    if(ui->cmStructure->findText(key)>=0){
        ui->cmStructure->setCurrentIndex(ui->cmStructure->findText(key));
    }
}


void MainWindow::setSelectedFieldValue()
{
    switch(ui->cmFieldEditType->currentIndex())
    {
    case 0:
        if(ui->cmEditField->currentText().contains(";")){
            const QStringList fields = ui->cmEditField->currentText().split(";",QString::SkipEmptyParts);
            qDebug()<<fields;
            Register *r = m_map.sub(fields);
            if(r->size()>32){
                r->fromHex(ui->leValue->text());
            }
            else{
                r->setValue(ui->leValue->value());
            }
        }
        else if(m_map.field(ui->cmEditField->currentText()) ){
                BitField *field = m_map.field(ui->cmEditField->currentText());
                if(field->size()>32){
                    field->fromHex(ui->leValue->text());
                }
                else{
                    field->setValue(ui->leValue->value());
                }
        }
        break;
    case 1:{
        quint32 bit_addr = ui->cmEditField->currentData().toUInt()*8;
        m_map.sub(bit_addr,bit_addr+7)->setValue(ui->leValue->value());
        break;
    }
    case 2:{
        quint32 bit_addr = ui->cmEditField->currentData().toUInt()*8;
        m_map.setValue(bit_addr,bit_addr+31,ui->leValue->value());
        break;
    }
    case 3:{
        quint32 bit_addr = ui->cmEditField->currentData().toUInt()*8;
        Register *r = m_map.sub(m_filteredFieldsList);
        if( r->size() > 32 ){
            r->fromHex(ui->leValue->text());
        }else{
            r->setValue(ui->leValue->value());
        }
        break;
    }
    }
    update_output();
}

void MainWindow::buildEditFieldList()
{
    ui->cmEditField->blockSignals(1);
    switch(ui->cmFieldEditType->currentIndex())
    {
    case 0:{
        ui->cmEditField->clear();
        QStringList fields_list;
        if(m_filteredFieldsList.isEmpty())
            fields_list = m_map.fieldsList();
        else
            fields_list = m_filteredFieldsList;

        for(int i=0;i<fields_list.count();i++){
            const bool is_reserved = fields_list[i].startsWith("_") || fields_list[i].toLower().startsWith("reserved");
            if(!is_reserved)
                ui->cmEditField->addItem(fields_list[i]);
        }
        break;
    }
    case 1:{
        ui->cmEditField->clear();
        quint32 max_addr_byte = m_map.size()/8 +1;
        for(quint32 adr=0;adr<max_addr_byte;adr++){
            ui->cmEditField->addItem(QString("byte[0x%1]").arg(adr,4,16,QChar('0')),adr);
        }
        break;
    }
    case 2:{
        ui->cmEditField->clear();
        quint32 max_addr_byte = m_map.size()/8 +1;
        for(quint32 adr=0;adr<max_addr_byte;adr+=4){
            ui->cmEditField->addItem(QString("word[0x%1]").arg(adr,4,16,QChar('0')),adr);
        }
        break;
    }
    case 3:{
        if(!m_filteredFieldsList.isEmpty())
            ui->cmEditField->addItems(this->m_filteredFieldsList);
        else
            ui->cmEditField->addItems(m_map.fieldsList());
        break;
    }
    }

    ui->cmEditField->blockSignals(0);
}



void MainWindow::on_cmStructure_activated(int index)
{    

    if(ui->cmStructure->currentIndex() == 0){
        QString filename = QFileDialog::getOpenFileName(0,"",QFileInfo(m_structure_file_path).path(),"JSON (*.json)",0);
        if(QFile::exists(filename)){

            if(loadStructureFile(filename)){
                addRecentFile(filename);
                saveRecentFiles();
                //load them back
                loadRecentFiles();
                applyStructure();
            }
        }
    }
    else{
        m_structure_file_path = m_recent_files[ui->cmStructure->currentText()];
        loadStructureFile(m_structure_file_path);
        applyStructure();
    }
}

bool MainWindow::loadStructureFile(const QString &filename){
    QFile f(filename);
    if(f.open(QIODevice::ReadOnly)){
        ui->teRegister->setPlainText(f.readAll());
        ui->cmStructure->setToolTip(filename);
        f.close();
        Q_EMIT update();
        return true;
    }
    else{
        QMessageBox::critical(0,"Error",QString("Can't load file %1").arg(filename));
        return false;
    }
}


quint32 MainWindow::pathSize()
{
    const quint32 path_sizes[]={1,8,8,16,32,128};
    return path_sizes[ui->cmBitRepr->currentIndex()];
}


QString MainWindow::representFieldAsString(Register *preg, BitField *pfield, Represent represent){
    QString result;
    QString format;

    const bool custom_repr = pfield->extras().contains("repr");

    QString offset;
    quint32 bitfield_offset = preg->indexOf(pfield->first());

    // OFFSET
    if(ui->cmBitRepr->currentIndex()==0){
        offset = QString("%1").arg(bitfield_offset,4,16,QChar('0'));
    }
    else if(ui->cmBitRepr->currentIndex() ==1){
        quint32 byte_offset = bitfield_offset/8;
        offset = QString("%1").arg(byte_offset,4,16,QChar('0'));
    }
    else if(ui->cmBitRepr->currentIndex() ==2){
        quint32 byte_offset = bitfield_offset/8;
        quint32 bit_offset = bitfield_offset%8;
        if(pfield->size()==1){
            offset = QString("%1[%2]").arg(byte_offset,4,16,QChar('0'))
                    .arg(bit_offset);
        }
        else if(pfield->size()%8 ==0){
            offset = QString("%1").arg(byte_offset,4,16,QChar('0'));
        }
        else{
            offset = QString("%1[%3:%2]").arg(byte_offset,4,16,QChar('0'))
                    .arg(bit_offset)
                    .arg(bit_offset+pfield->size()-1);
        }
    }
    else if(ui->cmBitRepr->currentIndex() == 3)//16
    {

        quint32 word_offset = bitfield_offset/8;
        quint32 bit_offset = bitfield_offset% 16;

        if(bit_offset ==0){
            offset = QString("%1").arg(word_offset,8,16,QChar('0'));
            if(ui->cbPrintPath->isChecked()){
                result += QString("<p style=\"background-color:black;color:white\">%1</p> ").arg(pfield->extra("path").toString());
            }
        }

        //align text
        offset = textAlign(offset,10);


        if(pfield->size()==1){
            offset += QString("[%1]")
                    .arg(bit_offset);
        }
        else {
            offset += QString("[%2:%1]")
                    .arg(bit_offset)
                    .arg(bit_offset+pfield->size()-1);
        }
        offset = textAlign(offset,20);
    }
    else if(ui->cmBitRepr->currentIndex() == 4)//32
    {

        quint32 word_offset = bitfield_offset/8;
        quint32 bit_offset = bitfield_offset% 32;

        if(bit_offset ==0){
            if(ui->cbPrintPath->isChecked()){
                result += QString("<p style=\"background-color:black;color:white\">%1</p> ").arg(pfield->extra("path").toString());
            }
            offset = QString("%1").arg(word_offset,8,16,QChar('0'));
        }

        //align text
        offset = textAlign(offset,10);


        if(pfield->size()==1){
            offset += QString("[%1]")
                    .arg(bit_offset);
        }
        else {
            offset += QString("[%2:%1]")
                    .arg(bit_offset)
                    .arg(bit_offset+pfield->size()-1);
        }
        offset = textAlign(offset,20);        
    }
    else if(ui->cmBitRepr->currentIndex() == 5)//128
    {

        quint32 word_offset = bitfield_offset/8;
        quint32 bit_offset = bitfield_offset% 128;

        if(bit_offset ==0){
            if(ui->cbPrintPath->isChecked()){
                result += QString("<p style=\"background-color:black;color:white\">%1</p> ").arg(pfield->extra("path").toString());
            }
            offset = QString("%1").arg(word_offset,8,16,QChar('0'));
        }

        //align text
        offset = textAlign(offset,10);


        if(pfield->size()==1){
            offset += QString("[%1]")
                    .arg(bit_offset);
        }
        else {
            offset += QString("[%2:%1]")
                    .arg(bit_offset)
                    .arg(bit_offset+pfield->size()-1);
        }
        offset = textAlign(offset,20);
    }

    //FORMAT-value
    switch(represent){
    case AS_CHANGED:
        format = "<b>%4:&nbsp;&nbsp;%1[%3]=<font color=red>%2</font></b>";
        break;

    case AS_UNCHANGED:
        format = "<b>%4:&nbsp;&nbsp;%1[%3]=<font color=blue>%2</font></b>";
        break;

    case AS_IGNORESTYLE:
        format = "<b>%4:&nbsp;&nbsp;%1[%3]=%2</b>";
        break;

    }

    if(pfield->extras().contains("color")){
        result += QString("<p><span style=\"background-color:%1\">").arg(pfield->extra("color").toString());
    }else{
        result += QString("<p>");
    }

    //FIELD and above
    if(custom_repr){
        Register tmp(0,"tmp",true);
        tmp.addField(pfield);
        result += QString("<b>%2:&nbsp;&nbsp;%1</b>")
                .arg(tmp.toString(pfield->extra("repr").toString()))
                .arg(offset);
    }

    //auto
    else{
        if(pfield->size()==1){
            result += QString(format)
                    .arg(pfield->name())
                    .arg(pfield->value())
                    .arg(pfield->size())
                    .arg(offset);

        }
        else if(pfield->size()<=8){
            result += QString(format)
                    .arg(pfield->name())
                    .arg(pfield->toUInt(),0,16)
                    .arg(pfield->size())
                    .arg(offset);

        }
        else if(pfield->size()<=16){
            result += QString(format)
                    .arg(pfield->name())
                    .arg(pfield->toUInt(),4,16,QChar('0'))
                    .arg(pfield->size())
                    .arg(offset);

        }
        else if(pfield->size()<=32){
            result += QString(format)
                    .arg(pfield->name())
                    .arg(pfield->toUInt(),8,16,QChar('0'))
                    .arg(pfield->size())
                    .arg(offset);

        }
        else if(pfield->size()<=128){
            result += QString(format)
                    .arg(pfield->name())
                    .arg(pfield->toUInt(),8,16,QChar('0'))
                    .arg(pfield->size())
                    .arg(offset);

        }
        else{
            QString hex_part = pfield->toHex();

            if(ui->cbTrim->isChecked() &&hex_part.size()>10){
                hex_part = hex_part.mid(0,10)+"...";
            }
            if(pfield->size()%8 ==0)
                result += QString(format)
                        .arg(pfield->name())
                        .arg(hex_part)
                        .arg(QString("0x%1 bytes").arg(pfield->size()/8,0,16))
                        .arg(offset);
            else
                result += QString(format)
                        .arg(pfield->name())
                        .arg(hex_part)
                        .arg(pfield->size())
                        .arg(offset);

        }
    }

    result += "</span></p>";

    // DESCRIPTION
    if(ui->cbDescr->isChecked() && !pfield->description().isEmpty()){
        QString description = pfield->description();
        result += QString("<p> <font color=grey>%1</font> </p>").arg(description.replace("\n","<br>"));
    }
    return result;
}

QString MainWindow::buildJson()
{
    QString result;
    QStringList fieldNameList = m_map.fieldsList();
    result += "[\n";
    Q_FOREACH(const QString &fieldName, fieldNameList ){
        BitField *field = m_map.field(fieldName);
        result += "\n{ ";
        result += QString(
                          "\n\t\"name\": \"%1\[%2]\","
                          "\n\t\"descr\": \"%3\","
                          "\n\t\"path\": \"%4\""
                          )
                .arg(field->name()).arg(field->size())
                .arg(field->description())
                .arg(field->extra("path").toString());
        if(field->extras().contains("color")){
            result += ",";
            result += QString("\n\t\"color\": \"%1\"").arg(field->extra("color").toString());
        }
        result += "\n}";
        if(fieldNameList.indexOf(fieldName) < fieldNameList.count()-1){
            result += ",";
        }
    }
    result += "]\n";
    return result;
}



void MainWindow::update_output()
{    
    Register *r;
    Register *r2;
    QTextCursor bak_textCursor = ui->teResult->textCursor();

    if(ui->leFilter->text().isEmpty()) r = &m_map;
    else r = m_map.temporary();

    QElapsedTimer t;
    t.start();
    analyzeChanges();
    const int bak_scroll_bar = ui->teResult->verticalScrollBar()->value();
    ui->teResult->clear();
    Register REG2 = *r;
    if(m_map.size()==0){
    ui->teResult->appendHtml("Load some structure file...");
    }else{
    ui->teResult->appendHtml(QString("<p>%1").arg(this->m_structure_file_path));
    }

    if(ui->cmDefaultFieldValue->currentIndex() == 2)
        REG2.fill(1);
    else if (ui->cmDefaultFieldValue->currentIndex() == 1)
        REG2.fill(0);

    Q_FOREACH(const QString &item, r->fieldsList()){
        if(ui->cbIgnoreSpareBits->isChecked()
                && (item.startsWith("_") || item.toLower().startsWith("reserved"))
         ){
            continue;
        }
        if(m_map.field(item)->toByteArray() != REG2.field(item)->toByteArray()
                && ui->cbShowDiff->isChecked())
        {
            const QString repr = representFieldAsString(r, r->field(item),AS_CHANGED);
            ui->teResult->appendHtml(repr);
        }else{
            const QString repr = representFieldAsString(r, r->field(item),AS_UNCHANGED);
            ui->teResult->appendHtml(repr);
        }
    }
    // total
    if(r->size()>0){
        if(r->size()%8)
            ui->teResult->appendHtml(QString("<b>___TOTAL=%1 bytes___</b>").arg(r->size()/8));
        else
            ui->teResult->appendHtml(QString("<b>___TOTAL=%1(0x%2)bytes or %3 bits___</b>").arg(r->size()/8).arg(r->size()/8,0,16).arg(r->size()));
    }

    ui->teResult->verticalScrollBar()->setValue(bak_scroll_bar);
    ui->teResult->setTextCursor(bak_textCursor);
}



void MainWindow::on_cbTrim_toggled(bool checked)
{
    update_output();
}


void MainWindow::on_cmStructure_currentIndexChanged(int index)
{
    ui->pbApply->setEnabled(index!=0);
}

void MainWindow::on_cbDescr_toggled(bool checked)
{
    update_output();
}


void MainWindow::on_cmEditField_activated(int index)
{    
    editBitField(ui->cmEditField->itemText(index));
}

void MainWindow::getSelectedFieldValue(){
    switch(ui->cmFieldEditType->currentIndex()){
    case 0:

        if(ui->cmEditField->currentText().contains(";")){
            const QStringList fields = ui->cmEditField->currentText().split(";",QString::SkipEmptyParts);
            qDebug()<<fields;
            Register *r = m_map.sub(fields);
            if(r->size()>32){
                ui->leValue->setText(r->toHex());
            }
            else{
                ui->leValue->setValue(r->toUInt());
            }
        }
        else if(ui->cmEditField->currentIndex()>=0
                && m_map.contains(ui->cmEditField->itemText(ui->cmEditField->currentIndex())))
        {
            BitField *f = m_map.field(ui->cmEditField->itemText(ui->cmEditField->currentIndex()));
            if(f->size()>32){
                ui->leValue->setText(f->toHex());
            }else{
                ui->leValue->setValue(f->value());
            }
            ui->lbFieldInfo->setText(QString("%1 bits").arg(f->size()));
        }
        break;
    case 1:{
        quint32 addr_bit = ui->cmEditField->currentData().toUInt()*8;
        ui->leValue->setValue(m_map.value(addr_bit,addr_bit+7));
    }
        ui->lbFieldInfo->setText(QString("8 bits"));
        break;
    case 2:{
        quint32 addr_bit = ui->cmEditField->currentData().toUInt()*8;
        ui->leValue->setValue(m_map.value(addr_bit,addr_bit+31));
    }
        ui->lbFieldInfo->setText(QString("32 bits"));
        break;
    }
}


void MainWindow::removeStructureFile()
{
    if(!ui->cmStructure->currentText().isEmpty()
            && m_recent_files.contains(ui->cmStructure->currentText())){

        QMessageBox msg;
        msg.setWindowTitle("");
        msg.setText("Delete file?");
        msg.setStandardButtons(QMessageBox::Yes|QMessageBox::No);
        int r = msg.exec();
        if(r == QMessageBox::Yes)
            QFile::remove(m_recent_files[ui->cmStructure->currentText()]);

        m_recent_files.remove(ui->cmStructure->currentText());
        saveRecentFiles();
        loadRecentFiles();

    }
    loadRecentFiles();
}

void MainWindow::field_label_double_click(const QString &link)
{    
    const QString field_name = QObject::sender()->objectName().mid(2);
    // show edit dialog
    QVariantMap dlg_data;
    BitField *field = m_map.field(link);
    dlg_data["name"] = field->name();
    dlg_data["lsb"] = m_map.indexOf(field->first())%32;
    dlg_data["msb"] = m_map.indexOf(field->first())%32 + field->size()-1;
    dlg_data["path"]= field->extra("path").toString();
    dlg_data["descr"] = field->description();
    dlg_data["value"] = field->value();
    dlgField dlg(dlg_data);
    dlg.setWindowTitle("Field");
    if(dlg.exec()==QDialog::Accepted)
    {
        const QVariantMap &data = dlg.resultData();
        field->setDescription(data["descr"].toString());
        field->setName(data["name"].toString());
        field->setExtra("path",data["path"].toString());                
        field->setUInt(data["value"].toUInt());
        ui->teRegister->setPlainText( this->buildJson());
        update_output();
    }
}


void MainWindow::closeEvent(QCloseEvent *ev)
{
    saveRecentFiles();
    saveSettings();
    ev->accept();
}

void MainWindow::keyPressEvent(QKeyEvent * const event)
{
    if(event->type() == QKeyEvent::KeyPress) {
        if(event->key() == Qt::Key_F3) {
            event->accept();
            if(ui->teResult->textCursor().position()<0)
                m_cursor_position = 0;
            else
                m_cursor_position = ui->teResult->textCursor().position();
            findTextInData();
        }else if (event->key()==Qt::Key_F && (QGuiApplication::keyboardModifiers() & Qt::ControlModifier)){
            ui->leFindText->setPlaceholderText("...find text...");
            ui->leFindText->setText("");
            ui->wgFindText->setVisible(1);
            ui->leFindText->setFocus();
            m_backup_cursor = ui->teResult->textCursor();
            event->accept();
        }else if(event->key()==Qt::Key_Escape){
            ui->wgFindText->setVisible(0);
            ui->teResult->setFocus();
            ui->teResult->setTextCursor(m_backup_cursor);
            event->accept();
        }
    }
}

void MainWindow::on_cbUseWindowsCRLF_toggled(bool checked)
{
    m_settings_ascii_windows = checked;
    saveSettings();
}


void MainWindow::on_pbReloadDataFile_clicked()
{
    if(m_map.isEmpty()){
        QMessageBox::critical(0,"Error", "Structure is empty!", QMessageBox::Ok);
        return;
    }

    if(loadData(m_data_file_path, m_data_file_format) == false){

        setLastDataFile(DataFormatUnknown, "");
    }
    else update_output();
}


void MainWindow::saveStructureFile()
{
    QFile f(this->m_structure_file_path)  ;
    if(f.open(QFile::WriteOnly)){
        f.write(ui->teRegister->toPlainText().toLatin1());
    }else{
        QMessageBox::critical(0,"Error",QString("Can't save file %1").arg(m_structure_file_path));
    }
    f.close();
}

void MainWindow::saveAsStructureFile()
{
    QString filename =  QFileDialog::getSaveFileName(0,"",QFileInfo(m_structure_file_path).path(),"JSON (*.json)",0);

    if(!filename.isEmpty()){
        QFile f(filename)  ;
        if(f.open(QFile::WriteOnly)){
            f.write(ui->teRegister->toPlainText().toLatin1());
            m_structure_file_path  = filename;
        }else{
            QMessageBox::critical(0,"Error",QString("Can't save file %1").arg(m_structure_file_path));
        }
        f.close();

        QFileInfo fi(filename);
        const QString item_name = fi.baseName();
        ui->cmStructure->setToolTip(filename);
        ui->cmStructure->addItem(item_name, filename);

        addRecentFile(filename);
        saveRecentFiles();
    }
}



void MainWindow::on_cmBitRepr_currentIndexChanged(int index)
{
    update_output();
}


void MainWindow::on_leFilter_editingFinished()
{
    if(ui->leFilter->text().isEmpty()){
        update_output();
    }
}


void MainWindow::on_pbReload_clicked()
{
    on_cmStructure_activated(0);
}


static void enterBitStr(QString &code, bool &in_bit_str){
    if(!in_bit_str){
        code+= "struct{\n";
    }
    in_bit_str = 1;
}

static void exitBitStr(QString &code, bool &in_bit_str){
    if(in_bit_str){
        code += "}\n";
    }
    in_bit_str = 0;
}


//void MainWindow::generateClassicH(const QString &filename){

//    QStringList l;
//    QString code;
//    QString code2;
//    const QStringList fields = m_map.fieldsList();
//    l.reserve(fields.count());
//    bool in_bit_str = false;

//    code2 += "#define IFR0_VARIABLE_ADDR(X) *((volatile uint32_t*)(X)+IFR0_BASE_ADDRESS)";
//    QMap<QString, quint32> paths;
//    QStringList paths_list;
//    for(int i=0;i<fields.count();i++)
//    {
//        BitField *f = m_map.field(i);
//        if(f->extras().contains("path")){
//            QString path = f->extra("path").toString().trimmed();
//            if(path.contains("@name")){
//                f->setExtra("path",path.replace("@name",f->name()));
//            }
//            const int addr = m_map.indexOf(f->first())/8;

//            if(!paths.contains(path)){
//                paths_list.append(path);
//                paths[path] = addr;
//            }

//            if(path.split(".").size()!=2){
//                qDebug()<<"error2 -wrong path";
//            }
//        }else{
//            qDebug()<<"error1 - no path";
//        }
//    }

//    quint32 last_addr=0;
//    for(int i=0;i<paths_list.count();i++)
//    {
//        Register *r = m_map.sub("path",paths_list[i]);
//        const quint32 reg_ofsset = m_map.indexOf(r->first())/8;
//        const quint32 size= r->size()/8;
//        if(last_addr != reg_ofsset){
//            // insert offset;
//            code += QString("\tuint8_t RESERVED[0x%1];\t\t\t/*offset 0x%2*/\n").arg(reg_ofsset-last_addr,0,16).arg(last_addr,4,16,QChar('0'))
//                    .toLower();
//        }
//        if(r->size()%8!=0){
//            qDebug()<<QString("Error3 field%1 wrong size%2").arg(paths_list[i]).arg(r->size());
//        }
//        else{

//            const QString name = paths_list[i].split(".")[1];
//            if(size ==4){
//                code += QString("\tuint32_t %1;\t\t\t/*offset 0x%2*/\n").arg(name).arg(reg_ofsset,4,16,QChar('0'))
//                        .toLower();

//                // add masks
//                if(r->fieldsList().count()>1){

//                    quint32 reg_offset = m_map.indexOf(r->first());
//                    code2 += QString("\n/* 0000'%2 %1 */\n").arg(paths_list[i]).arg(reg_ofsset,4,16,QChar('0'));
//                    code2 += "/*variables*/\n";
//                    for(int j=0;j<r->fieldsList().count();j++)
//                    {
//                        BitField *f = r->field(j);
//                        quint32 field_offset = m_map.indexOf(f->first()) - reg_offset;
//                        const QString shift = QString("0x%1u").arg(field_offset);
//                        const QString mask = QString("0x%1u").arg((quint32)qPow(2,f->size())-1,0,16);

//                        if(!f->name().startsWith("_") && f->size()<32){
//                            code2 += QString("#define %1_VALUE ((IFR0_VARIABLE_ADDR(0x%2)>>%3) & %4\n").arg(f->name().toUpper())
//                                    .arg(reg_ofsset,4,16,QChar('0'))
//                                    .arg(shift).arg(mask);
//                        }
//                    }
//                    code2 += "/*variable mask*/\n";
//                    for(int j=0;j<r->fieldsList().count();j++)
//                    {
//                        BitField *f = r->field(j);
//                        quint32 field_offset = m_map.indexOf(f->first()) - reg_offset;
//                        const QString shift = QString("0x%1u").arg(field_offset);
//                        const QString mask = QString("0x%1u").arg((quint32)qPow(2,f->size())-1,0,16);

//                        if(!f->name().startsWith("_") && f->size()<32){
//                            code2 += QString("#define %1_SHIFT (%2)\n").arg(f->name().toUpper()).arg(shift);
//                            code2 += QString("#define %1_MASK (%2 << %1_SHIFT)\n").arg(f->name().toUpper()).arg(mask);
//                        }
//                    }
//                }
//            }
//            else if(size ==1){
//                code += QString("\tuint8_t %1;\t\t\t/*offset 0x%2*/\n").arg(name).arg(reg_ofsset,4,16,QChar('0'))
//                        .toLower();
//            }
//            else if(size ==2){
//                code += QString("\tuint8_t %1[2];\t\t\t/*offset 0x%2*/\n").arg(name).arg(reg_ofsset,4,16,QChar('0'))
//                        .toLower();
//            }
//            else{
//                code += QString("\tuint8_t %1[%2];\t\t\t/*offset 0x%3*/\n").arg(name).arg(size).arg(reg_ofsset,4,16,QChar('0'))
//                        .toLower();
//            }


//        }
//        last_addr = reg_ofsset + size;
//    }

//    // save to file
//    QFile f(filename);
//    if(f.open(QFile::WriteOnly)){
//        f.write(code2.toLatin1());
//        f.write(code.toLatin1());
//    }
//}

//void MainWindow::generateBitFieldH(const QString &filename){


//    QString code_h_bitfield;
//    QString code_h_defines;

//    const QStringList fields = m_map.fieldsList();

//    code_h_bitfield += "struct regs_list_tag{\n"
//                        "\tuint16_t index;\n"
//                        "\tuint8_t shift;\n"
//                        "\tuint8_t maskn;\n"
//                        "}regs_list[] ={\n";

//    code_h_defines += "enum{\n";
//    for(int i=0;i<fields.count();i++){
//        BitField *r = m_map.field(i);
//        quint32 field_offset = m_map.indexOf(r->first());
//        quint32 field_size = r->size();
//        if(r->name().startsWith("_") || r->name().toLower() == "reserved") continue;
//        else{
//            const QString item_name = QString("%1_%2").arg(r->extra("path").toString()).arg(r->name()).replace(".","_");
//            if(r->size()<32){
//                code_h_defines += "\t," + item_name + "\n";
//                code_h_bitfield += QString("\t{ 0x%4, \t%2, \t%3}, \t\t/*%1\t*/\n")
//                                            .arg(item_name)
//                                            .arg(field_offset%32)                //msb
//                                            .arg(field_size-1)                     //lsb
//                                            .arg(field_offset/32,4,16,QChar('0'));
//            }else{
//                code_h_defines += "\t," + item_name + "\n";
//                code_h_bitfield += QString("\t{ 0x%4, \t%2, \t%3}, \t\t/*%1\t*/\n")
//                                            .arg(item_name)
//                                            .arg(0)                //msb
//                                            .arg(0)                     //lsb
//                                            .arg(field_offset/32,4,16,QChar('0'));
//            }
//        }
//    }
//    code_h_bitfield += "}\n";
//    code_h_defines += "}\n";
//    // save to file
//    QFile f(filename);
//    if(f.open(QFile::WriteOnly)){
//        f.write(code_h_defines.toLatin1());
//        f.write(code_h_bitfield.toLatin1());
//    }
//}


void MainWindow::generateIfrMap(const QString &filename)
{

    QMap<QString, QList<BitFieldInfo>>  map_paths;
    QStringList offset_list_cache;

    quint32 mem_cell_size =0;

    QString code_h_bitfield;
    QString code_h_defines;

    const QStringList fields = m_map.fieldsList();    
    const quint32 reg_size =32;

    code_h_defines += QString("\n#define %2_PAGE_BASE (0x%1)\n").arg(ui->leCFileMapBaseAddr->value(),8,16,QChar('0')).arg(ui->leCFileMapName->text());

    code_h_defines +=  "\n";

    if(ui->cmCFileFormat->currentIndex()==0) {
        generateMemoryMapPaths(&map_paths,4);    //32 bit
        mem_cell_size = 32;
    }
    else {
        generateMemoryMapPaths(&map_paths,5);    //128 bit
        mem_cell_size = 128;
    }
    Q_FOREACH(const QString &mem_path, map_paths.keys())
    {

        QStringList paths = mem_path.split(".");
        const QString &section = paths[1];
        const QString &sub_section = paths[2];
        const QString &mem_addr = paths[0];
        const quint32 path_size = m_map.pathSize(QString("%1.%2").arg(section).arg(sub_section));
        QString path_offset_text;

        // make offset
        QString offset_def = QString("%1_%2_OFFSET")
                .arg(section.trimmed().toUpper())
                .arg(sub_section.toUpper());

        const bool skip_path = sub_section.startsWith("_") || sub_section.toLower().startsWith("reserved");
        if(skip_path) continue;        

        if(offset_list_cache.contains(offset_def) ){
            continue;
        }
        // save once
        else{
            offset_list_cache.append(offset_def);

            // allign offset line
            code_h_bitfield += QString("\n#define %2 (0x%1) // [%3] bits")
                    .arg(mem_addr)
                    .arg(offset_def)
                    .arg(path_size);
        }

        // make setters/getters
        if(map_paths[mem_path].count() >= 1 && path_size <= mem_cell_size)
        {            
            foreach(const BitFieldInfo &fi, map_paths[mem_path])
            {                        
                QString field_def;
                QString description;
                BitField *f = fi.bitField;

                if(mem_cell_size >reg_size){
                    const quint32 bit_index = m_map.indexOf(f->first());
                    const quint32 path_offset = ((bit_index - (bit_index /mem_cell_size)*mem_cell_size)/32)*4;

                    if(path_offset>0)
                        path_offset_text = QString("+ 0x%1").arg(path_offset,2,16, QChar('0'));
                }

                if(m_map.sub(f->name())->size()>reg_size){
                    qWarning()<<QString("Error field %1(%2) is greater than 32").arg(f->name()).arg(m_map.sub(f->name())->size());
                    break;
                }

                const bool is_reserved = f->name().startsWith("_") || f->name().toLower().startsWith("reserved");
                if(is_reserved && ui->cbFileCIgnoreSpare->isChecked() ){
                    if(map_paths[mem_path].count() == 1) {
                        break;
                    }
                    else continue;
                }
                else if(f->size() <= mem_cell_size )
                {

                    if(!f->description().isEmpty())
                        description =  wordWrap(QString("/*\n%1\n*/").arg(QString(f->description()).trimmed()), 120);

                    QString value =  QString("\n#define %1_%2_VALUE %1_%2_GET(%3_PAGE_BASE)")
                            .arg(section.trimmed().toUpper())
                            .arg(f->name().toUpper())
                            .arg(ui->leCFileMapName->text());
                  //  textFormat(&getter);

                    quint32 msb = fi.offset%32 + f->size()-1;
                    quint32 lsb = fi.offset%32;
                    if(msb>31 || lsb >32 ){
                        qWarning()<< QString("Error %1[%2:%3]").arg(f->name()).arg(msb).arg(lsb);
                    }
                    QString setter = QString("\n#define %1_%2_SET(BASE,VALUE) FIELD_SET_VALUE((BASE) + %3%6, %4, %5, (VALUE))")
                            .arg(section.trimmed().toUpper())
                            .arg(f->name().toUpper())
                            .arg(offset_def)
                            .arg(msb)
                            .arg(lsb)
                            .arg(path_offset_text);


                    QString getter = QString("\n#define %1_%2_GET(BASE) FIELD_VALUE((BASE) + %3%6, %4, %5)")
                            .arg(section.trimmed().toUpper())
                            .arg(f->name().toUpper())
                            .arg(offset_def)
                            .arg(msb)
                            .arg(lsb)
                            .arg(path_offset_text);


                   // textFormat(&setter);
                    if(ui->cbDescriptionComment->isChecked() && !description.isEmpty()){

                            field_def += "\n" + description + setter + getter + value;
                    }
                    else{
                        field_def += "\n" + setter + getter + value;
                    }

                }else{


                }

                code_h_bitfield += field_def
                                + "\n";
            }//foreach

        }
        else{
                /*
                 * mem path can't be zero and can't be more than mem_path_size
                */
        }        
    }

    saveHeaderFile(filename, code_h_defines, code_h_bitfield);
}


bool MainWindow::saveHeaderFile(const QString &filename, const QString &header, const QString &bitfields)
{
    QFile file(filename);
    if(file.open(QFile::WriteOnly)){
        file.write(QString(ui->teCFileHeader->toPlainText())
                   .replace("@year",QString::number(QDate::currentDate().year())).toLatin1()
                   );
        file.write(QString("#ifndef %1MAP_H\n").arg(ui->leCFileMapName->text()).toLatin1());
        file.write(QString("#define %1MAP_H\n").arg(ui->leCFileMapName->text()).toLatin1());
        if(ui->cbBitfieldOperationEnable->isChecked())
            file.write(ui->teCFileBitFieldOper->toPlainText().toLatin1());
        file.write("\n");
        file.write(header.toLatin1());
        file.write(bitfields.toLatin1());
        file.write(QString("\n#endif //%1MAP_H\n").arg(ui->leCFileMapName->text()).toLatin1());
    }
}

void MainWindow::selectEditFields(const QStringList &list)
{
    QStringList fields;
    for (int i=0;i<list.count();i++ ) {
        if(m_map.fieldsList().contains(list[i])){
            fields.append(list[i]);
        }
    }
    ui->cmEditField->setEditText(fields.join(";"));
}



void MainWindow::fillMap1()
{
    m_map.fill(1);
    update_output();
}

void MainWindow::fillMap0()
{
    m_map.fill(0);
    update_output();
}

void MainWindow::updateEditFieldFormat(const QString &format)
{
    ui->lbValueFormat->setText(format);
}

void MainWindow::editBitField(const QString &field_name)
{

    int index = ui->cmEditField->findText(field_name);

    if(index >=0){
        ui->cmEditField->blockSignals(1);
        ui->cmEditField->setCurrentIndex(index);
        ui->cmEditField->blockSignals(0);
    if(m_map.contains(field_name)){
        BitField *field = m_map.field(field_name);
        ui->cmEditField->setToolTip(field->description());
    }
    getSelectedFieldValue();
    }
}



void MainWindow::findTextItem_InMap(const QString &text)
{ 
    editBitField(text);
}

void MainWindow::warning(const QString &msg)
{
    ui->teConsole->appendHtml(QString("<font color=red>WARNING:%1</font>").arg(msg));
    if(!ui->teConsole->isVisible())
        Q_EMIT updateConsoleIcon();
}

void MainWindow::error(const QString &msg)
{
    ui->teConsole->appendHtml(QString("<font color=blue>ERROR:%1</font>").arg(msg));
    if(!ui->teConsole->isVisible())
        Q_EMIT updateConsoleIcon();
}

void MainWindow::generateMemoryMapPaths(QMap<QString, QList<BitFieldInfo> > *map_fields, int path_size)
{
    m_map_paths.clear();
    const QStringList fields = m_map.fieldsList();
    for(int i=0;i<fields.count();i++){
        BitField *field = m_map.field(i);
        const quint32 offset = m_map.indexOf(field->first());
        BitFieldInfo field_info;
        field_info.bitField = field;
        field_info.offset = offset;
        field_info.byteAddr = field_info.offset/8;

        QString path="pad";
        if(field->extras().contains("path")){
            path = field->extra("path").toString();
        }
        // update path size
        quint32 mask;

        switch(path_size)
        {
        case 0: // 1 bit
        case 1: // 8 bit
        case 2: // 8 bit
            mask = 0xffffffff;
            break;
        case 3: // 16 bit
            mask = 0xfffffffe;
            break;
        case 4: //32 bit
            mask = 0xfffffffc;
            break;
        case 5: //128 bit
            mask = 0xfffffff0;
            break;
        }

        const uint32_t pad_number = field_info.byteAddr&mask;
        path = QString("%2.%1").arg(path).arg(pad_number,8,16,QChar('0')).toUpper();
        //qDebug()<<field_info.byteAddr<<path;
        (*map_fields)[path].append(field_info);
    }

}
void MainWindow::updateItemsList()
{    
    switch(ui->cmBitRepr->currentIndex())
    {
    case 0:
    case 1:
    case 2: // 8 bit
        generateMemoryMapPaths(&m_map_paths, 8);
        break;
    case 3: // 16 bit
        generateMemoryMapPaths(&m_map_paths,16);
        break;
    case 4: //32 bit
        generateMemoryMapPaths(&m_map_paths,32);
        break;
    case 5: //128 bit
        generateMemoryMapPaths(&m_map_paths,128);
        break;
    }

    updateTools();
}

void MainWindow::updateUiControls()
{
    on_cmDefaultFieldValue_currentIndexChanged(ui->cmDefaultFieldValue->currentIndex());
}


void MainWindow::on_pbExporToCode_clicked() // sheet code as a concept
{
    QString filename = ui->leCFileExportFile->text().trimmed();
    if(filename.isEmpty()){
        filename = QFileDialog::getSaveFileName(0,"",qApp->applicationFilePath(),"C header file (*.h)",0);
    }
    if(filename.isEmpty() ) return;
    else {
        ui->leCFileExportFile->setText(filename);
    }

        generateIfrMap(filename);
}


void MainWindow::on_pushButton_clicked()
{
    for(int i=0;i<m_map.fieldsList().size();i++){
            qDebug()<<m_map.field(i)->name()<<m_map.field(i)->extra("path")<<m_map.field(i)->size();

    }
}


//void MainWindow::on_pbSaveJson_clicked()
//{
//    //    QString filename;
//    //    if(ui->leExportFile->text().isEmpty()){
//    //        filename = QFileDialog::getSaveFileName(0,"",qApp->applicationFilePath(),"C header file (*.h)",0);
//    //    }
//    //    if(filename.isEmpty() ) return;

//    //    QFile f(filename);

//    //    if(f.open(QFile::WriteOnly)){
//    //        f.write("[");
//    //        const QStringList fields_list = REG.fieldsList();
//    //        for(int i=0;i<fields_list.count();i++){

//    //            BitField *field = REG.field(fields_list[i]);
//    //            QByteArray item = "{";
//    //            item += QString("\"name\":\"%1[%2]\"").arg(field->name()).arg(field->size());
//    //            item += Q
//    //                                            "\"value\":\"%1\""
//    //                                            "\"descr\":\"%1\""
//    //                                            "}")
//    //            item += "},";
//    //        }
//    //        f.write("]");
//    //    }else{
//    //        QMessageBox::critical(0,"Error",QString("Can't save file %1").arg(filename));
//    //    }
//}


void MainWindow::loadFileAsList()
{
    bool load_data_ok = false;
    QString filename;
    if(m_map.isEmpty()){
        QMessageBox::critical(0,"reading",QString("Reg is empty"));
        return ;
    }

    filename = QFileDialog::getOpenFileName(0,"Load as list",m_data_file_path,"Text file(*.txt)");
    const bool dlg_cancel = filename.isEmpty();
    if(!dlg_cancel){
        load_data_ok = loadData(filename, DataFormatAsLIST);
        ui->pbApply->setEnabled(1);
        if(load_data_ok){
            m_data_file_path = QFileInfo(filename).filePath();
            m_data_file_format  = DataFormatAsLIST;            
            update_output();
            updateFileNameTitle();
        }
    }
}

void MainWindow::loadFileAsVMEM()
{
    bool load_data_ok = false;
    QString filename;
    if(m_map.isEmpty()){
        QMessageBox::critical(0,"reading",QString("Reg is empty"));
        return ;
    }

    filename = QFileDialog::getOpenFileName(0,"Load as Verilog Memory",m_data_file_path,"Verilog Memory(*.hex)");
    const bool dlg_cancel = filename.isEmpty();
    if(!dlg_cancel){
        load_data_ok = loadData(filename, DataFormatAsVMEM);
        ui->pbApply->setEnabled(1);
        if(load_data_ok){
            m_data_file_path = QFileInfo(filename).filePath();
            m_data_file_format  = DataFormatAsVMEM;            
            update_output();
            updateFileNameTitle();
        }
    }
}

void MainWindow::loadFileAsU8()
{
    bool load_data_ok = false;
    QString filename;
    if(m_map.isEmpty()){
        QMessageBox::critical(0,"reading",QString("Reg is empty"));
        return ;
    }

    filename = QFileDialog::getOpenFileName(0,"Load as U8",m_data_file_path,"Mem file ISP(*.mem)");
    const bool dlg_cancel = filename.isEmpty();
    if(!dlg_cancel){
        load_data_ok = loadData(filename, DataFormatAsU8);
        ui->pbApply->setEnabled(1);
        if(load_data_ok){
            m_data_file_path = QFileInfo(filename).filePath();
            m_data_file_format  = DataFormatAsU8;
            update_output();
            updateFileNameTitle();
        }
    }
}

void MainWindow::loadFileAsU32()
{
    bool load_data_ok = false;
    QString filename;
    if(m_map.isEmpty()){
        QMessageBox::critical(0,"reading",QString("Reg is empty"));
        return ;
    }

    filename = QFileDialog::getOpenFileName(0,"Load as ASCII U32",m_data_file_path,"Text(*.txt *.hex)");
    const bool dlg_cancel = filename.isEmpty();
    if(!dlg_cancel){
        load_data_ok = loadData(filename, DataFormatAsU32);
        ui->pbApply->setEnabled(1);
        if(load_data_ok){
            m_data_file_path = QFileInfo(filename).filePath();
            m_data_file_format  = DataFormatAsU32;            
            update_output();
            updateFileNameTitle();
        }
    }
}

void MainWindow::loadFileAsBin()
{
    bool load_data_ok = false;
    QString filename;
    if(m_map.isEmpty()){
        QMessageBox::critical(0,"reading",QString("Reg is empty"));
        return ;
    }

    filename = QFileDialog::getOpenFileName(0,"Load as binary",m_data_file_path,"Binary Image(*.bin *.sb3 *.sb4);;Other binary(*.*)");
    const bool dlg_cancel = filename.isEmpty();
    if(!dlg_cancel){
        load_data_ok = loadData(filename, DataFormatAsBinary);
        ui->pbApply->setEnabled(1);
        if(load_data_ok){
            m_data_file_path = QFileInfo(filename).filePath();
            m_data_file_format  = DataFormatAsBinary;            
            update_output();
            updateFileNameTitle();
        }
    }
}

void MainWindow::loadFileAsCde()
{
    bool load_data_ok = false;
    QString filename;
    if(m_map.isEmpty()){
        QMessageBox::critical(0,"reading",QString("Reg is empty"));
        return ;
    }

    filename = QFileDialog::getOpenFileName(0,"Load as CDE",m_data_file_path,"CDE (*.cde);;Other binary(*.*)");
    const bool dlg_cancel = filename.isEmpty();
    if(!dlg_cancel){
        load_data_ok = loadData(filename, DataFormatAsCDE);
        ui->pbApply->setEnabled(1);
        if(load_data_ok){
            m_data_file_path = QFileInfo(filename).filePath();
            m_data_file_format  = DataFormatAsCDE;            
            update_output();
            updateFileNameTitle();
        }
    }
}

void MainWindow::saveFileAsList()
{
    QString filename = QFileDialog::getSaveFileName(0,"Save As List",m_data_file_path,"Text File(*.txt)",0);
    const bool dlg_cancel = filename.isEmpty();
    if(!dlg_cancel){
        if(!saveData(filename, MainWindow::DataFormatAsLIST)){
            QMessageBox::critical(0,"Error",QString("Can't save file %1").arg(filename));
        }else{
            updateFileNameTitle();
        }
    }
}

void MainWindow::saveFileAsU8()
{
    QString filename = QFileDialog::getSaveFileName(0,"Save As Ascii U8",m_data_file_path,"Text (*.txt *.mem)",0);
    const bool dlg_cancel = filename.isEmpty();
    if(!dlg_cancel){
        if(!saveData(filename,MainWindow::DataFormatAsU8)){
            QMessageBox::critical(0,"writing",QString("Can't open file"));
        }else{
            updateFileNameTitle();
        }
    }
}

void MainWindow::saveFileAsVMEM()
{
    QString filename = QFileDialog::getSaveFileName(0,"Save As Verilog Memory",m_data_file_path,"Verilog Memory (*.hex)",0);
    const bool dlg_cancel = filename.isEmpty();
    if(!dlg_cancel){
        if(!saveData(filename,MainWindow::DataFormatAsVMEM)){
            QMessageBox::critical(0,"writing",QString("Can't open file"));
        }else{
            updateFileNameTitle();
        }
    }
}


void MainWindow::saveFileAsU32()
{
    const QString filename = QFileDialog::getSaveFileName(0,"Save As ASCII U32",m_data_file_path,"Text (*.txt *.hex)",0);
    const bool dlg_cancel = filename.isEmpty();
    if(!dlg_cancel){
        if(!saveData(filename, MainWindow::DataFormatAsU32)){
            QMessageBox::critical(0,"writing",QString("Can't save file"));
        }else{
            updateFileNameTitle();
        }
    }
}

void MainWindow::saveFileAsBin()
{
    const QString filename = QFileDialog::getSaveFileName(0,"Save As BIN",m_data_file_path,"Binary Images(*.bin *.sb3 *.sb4);;Other binary(*.*)",0);
    const bool dlg_cancel = filename.isEmpty();
    if(!dlg_cancel){
        if(!saveData(filename, MainWindow::DataFormatAsBinary)){
            QMessageBox::critical(0,"writing",QString("Can't save file"));
        }else{
            updateFileNameTitle();
        }
    }
}

void MainWindow::saveFileAsCde()
{
    const QString filename = QFileDialog::getSaveFileName(0,"Save As CDE",m_data_file_path,"CDE (*.cde)",0);
    const bool dlg_cancel = filename.isEmpty();
    if(!dlg_cancel){
        if(!saveData(filename, MainWindow::DataFormatAsCDE)){
            QMessageBox::critical(0,"writing",QString("Can't save file"));
        }else{
            updateFileNameTitle();
        }
    }
}

void MainWindow::saveFileAsCCode()
{
    const QString filename = QFileDialog::getSaveFileName(0,"Save As C Code",m_data_file_path,"C file (*.c)",0);
    const bool dlg_cancel = filename.isEmpty();
    if(!dlg_cancel){
        if(!saveData(filename, MainWindow::DataFormatAsCCODE)){
            QMessageBox::critical(0,"writing",QString("Can't save file"));
        }else{
            updateFileNameTitle();
        }
    }
}

void MainWindow::updateTools()
{
    QStringList items;
    for(int i=0;i<m_map.size()/8;i++){
        items<<QString("0x%1").arg(i,4,16,QChar('0'));

    }
    ui->cmCrcAdrFrom->clear();
    ui->cmCrcAdrTo->clear();
    ui->cmCrcAdrFrom->addItems(items);
    ui->cmCrcAdrTo->addItems(items);
    ui->cmCrcAdrFrom->setCurrentIndex(0);
    ui->cmCrcAdrTo->setCurrentIndex(ui->cmCrcAdrTo->count()-1);
}


void MainWindow::on_cmFieldEditType_currentIndexChanged(int index)
{
    buildEditFieldList();
    if(ui->cmFieldEditType->currentIndex() == 3){
        ui->cmEditField->setVisible(0);
        ui->leValue->setEditTextMode(1);
    }else{
        ui->cmEditField->setVisible(1);
    }
}


void MainWindow::updateFileNameTitle()
{
    QFileInfo fi(m_data_file_path);
    QFontMetrics fm(font());
    QString text = QString("%1 (size=%2)").arg(fi.filePath()).arg(fi.size());
    bool cutted = false;
    while(fm.horizontalAdvance(text) > width()){
        text.remove(0,3);
        cutted = true;
    }
    if(cutted){
        text.insert(0,"...");
    }
    ui->lbFileInfo->setText("<b>Data file " + text + "</b>");
}


void MainWindow::on_cbIgnoreSpareBits_toggled(bool checked)
{
    update_output();
}



void MainWindow::on_pushButton_2_clicked()
{    
    m_map.clear();    
    qDeleteAll(m_field_labels.begin(),m_field_labels.end());    
    m_field_labels.clear();    
    emit update_ui();
    update_output();
}


QLabel* MainWindow::makeFieldLabel(const BitFieldInfo &fi)
{

    BitField *field = fi.bitField;
    QLabel * lb = new QLabel();

    if(!field->name().isEmpty())
        lb->setObjectName("lb" + field->name());

    if(!field->name().startsWith("_")){
        //lb->setText(QString("%1[%2]")
        lb->setText(QString("<a href=\"%1\">%1[%2:%3]</a>")
                    .arg(field->name()).arg(fi.offset%32).arg(fi.offset%32+fi.bitField->size()-1));

        if(field->extras().contains("color")){
            lb->setStyleSheet(QString("background-color: %1;").arg(field->extra("color").toString()));
        }else{
            lb->setStyleSheet(QString("background-color: white;"));
        }
        lb->setToolTip( QString("<p>adr:<font color='blue'>%1</font><p>"
                                "<p>name:<b>%2[%3:%4]</p>"
                                "<p>%5</p>")
                .arg(fi.byteAddr&0xfffffffC,8,16,QChar('0'))
                .arg(field->name())
                .arg(fi.offset%32+ field->size()-1)
                .arg(fi.offset%32)
                .arg(field->description()));
    }else{
        lb->setText(QString("[%1]").arg(field->size()/8));
        lb->setStyleSheet(QString("background-color: lightgray;"));
        lb->setToolTip(QString("[%1:%2] <br>*** reserved ***")
                .arg(fi.offset%32+ field->size()-1)
                .arg(fi.offset%32));
    }

    lb->setAlignment(Qt::AlignHCenter);
    //lb->setStyleSheet(QString("background-color: %1;").arg(color.name()));
    lb->setMinimumHeight(30);
    lb->setCursor(Qt::PointingHandCursor);
    connect(lb, &QLabel::linkActivated,
            this,&MainWindow::field_label_double_click);
    return lb;
}


void MainWindow::clearFieldLabels()
{
    qDeleteAll(m_field_labels.begin(),m_field_labels.end());
    m_field_labels.clear();
}

void MainWindow::on_pushButton_3_clicked()
{

    //    QGroupBox *grp =0;
    //    QVBoxLayout *grp_layout=0;
    //    ui->scrollArea->widget()->blockSignals(1);
    //    Q_FOREACH(const QString &mem_path, m_memory_map_fields.keys())
    //    {
    //        qDebug()<<mem_path;
    //        grp = new QGroupBox(ui->scrollArea->widget());
    //        grp_layout = new QVBoxLayout();
    //        grp->setLayout(grp_layout);
    //        grp_layout->setContentsMargins(1,1,1,1);
    //        //grp->setMinimumHeight(40);
    //        grp->setTitle(mem_path.mid(9));
    //        ui->scrollArea->widget()->layout()->addWidget(grp);
    //        ui->scrollArea->widget()->layout()->setContentsMargins(1,1,1,1);
    //        grp->setStyleSheet(QString("QGroupBox::title{"
    //                                   "font-weight: bold; "
    //                                   "color: #A0A0A0; "
    //                                   "background-color: #434343;"
    //                                   "}"));
    //        m_field_labels.append(grp);
    //        bool first = true;
    //        Q_FOREACH(const BitFieldInfo &fi, m_memory_map_fields[mem_path] )
    //        {
    //            if(first)
    //            {
    //                first = false;
    //                QLabel *lb_address = new QLabel(QString("0x%1").arg(fi.byteAddr,8,16,QChar('0')));
    //                lb_address->setFixedWidth(80);
    //                grp_layout->addWidget(lb_address);
    //            }
    //            BitField *field = fi.bitField;

    //            QLabel *label = makeFieldLabel(fi);
    //           // label->setParent(grp);
    //                grp_layout->addWidget(label);

    //        }
    //    }

    //    ui->scrollArea->widget()->blockSignals(0);
    //    ui->scrollArea->widget()->adjustSize();
}


void MainWindow::on_pushButton_4_clicked()
{
     clearFieldLabels();
}




void MainWindow::on_cbPrintPath_toggled(bool checked)
{
   update_output();
}


void MainWindow::on_cbShowDiff_toggled(bool checked)
{
    update_output();
}

#include "crc32.h"
void MainWindow::on_pbCrcCalc_clicked()
{
    crc32_data_t crc32Config;
    uint32_t calc_checksum = 0;

    crc32_init(&crc32Config);

    /* Calculate 32-bit CRC checksum on first #NUM_BYTES_WKUP_CRC bytes */
    Register *tmp = m_map.sub(ui->cmCrcAdrFrom->currentData().toInt(),ui->cmCrcAdrTo->currentData().toInt());

    crc32_update(&crc32Config, (const uint8_t *)m_map.toByteArray().constData(),0x1ff0 );

    crc32_finalize(&crc32Config, &calc_checksum);
    ui->leCrcValue->setValue(calc_checksum);
}


void MainWindow::on_pbSetCrc_clicked()
{
    m_map.setFieldValue(ui->cmCrcField->currentText(),ui->leCrcValue->value());
    update_output();
}


void MainWindow::on_leFilter_returnPressed()
{
    m_filteredFieldsList.clear();
    if(!ui->leFilter->text().isEmpty()){
        QStringList l = m_map.fieldsList();
        QRegExp match(ui->leFilter->text());
        match.setPatternSyntax(QRegExp::Wildcard);
        int i=0;
        while(i<l.count()){
            if(!match.exactMatch(l[i])) l.removeAt(i);
            else i++;
        }
        m_filteredFieldsList = l;
        m_map.sub(l);
    }
    update_output();
}


void MainWindow::on_pbConsole_toggled(bool checked)
{
    ui->teConsole->setVisible(checked);
}


void MainWindow::on_cmDefaultFieldValue_currentIndexChanged(int index)
{
    ui->cbShowDiff->setChecked(0);
    if(index == 0){
        ui->cbShowDiff->setText("diff");
        ui->cbShowDiff->setEnabled(0);
    }
    else if(index >0){
        ui->cbShowDiff->setText(QString("diff from %1").arg(ui->cmDefaultFieldValue->currentText()));
        ui->cbShowDiff->setEnabled(1);
    }
}


void MainWindow::on_pbConsole_clicked()
{
    ui->pbConsole->setIcon(QIcon(":/console.png"));
}


void MainWindow::on_leFindText_textChanged(const QString &arg1)
{
    findTextInData();
}


void MainWindow::on_cbCaseSens_toggled(bool checked)
{
    if(!ui->leFindText->text().isEmpty()){
        findTextInData();
    }
}


void MainWindow::on_cbBitfieldOperationEnable_toggled(bool checked)
{
    ui->teCFileBitFieldOper->setEnabled(checked);
}


void MainWindow::on_leValue_returnPressed()
{
    setSelectedFieldValue();
}

