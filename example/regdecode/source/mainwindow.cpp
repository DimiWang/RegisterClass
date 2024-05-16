
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



static Register REG(0,"",Register::AllowSameName);

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    QFont f = ui->teResult->font();
    f.setFamily("Consolas");
    ui->teResult->setFont(f);
    REG.blockSignals(1);//block forever
    loadSettings();
    loadRecentFiles();
    m_data_file_path = QDir::currentPath();
    m_format_file_path = QDir::currentPath();

    QAction *actSave = new QAction("Save",0);
    QObject::connect(actSave,SIGNAL(triggered(bool)),this,SLOT(saveStructureFile()));
    QAction *actSaveAs = new QAction("SaveAs",0);
    QObject::connect(actSaveAs,SIGNAL(triggered(bool)),this,SLOT(saveAsStructureFile()));

    ui->tbStructureButton->addAction(actSave);
    ui->tbStructureButton->addAction(actSaveAs);
    ui->tbStructureButton->setDefaultAction(actSave);


}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::analyzeChanges()
{

}

void MainWindow::loadSettings()
{
    QSettings settings(QString("%2/%1.ini").arg(qApp->applicationName()).arg(qApp->applicationDirPath()));
    m_settings_ascii_windows = settings.value("General/ascii_windows_crlf",false).toBool();
    ui->cbUseWindowsCRLF->blockSignals(1);
    ui->cbUseWindowsCRLF->setChecked(m_settings_ascii_windows);
    ui->cbUseWindowsCRLF->blockSignals(0);

    settings.beginGroup("Repr");
    ui->cmBitRepr->setCurrentIndex(settings.value("Format").toInt());
    ui->cbDescr->setChecked(settings.value("ShowDescr").toBool());
    ui->cbTrim->setChecked(settings.value("TrimValues").toBool());
    settings.endGroup();
}

void MainWindow::saveSettings()
{
    QSettings settings(QString("%2/%1.ini").arg(qApp->applicationName()).arg(qApp->applicationDirPath()));
    settings.setValue("General/ascii_windows_crlf", m_settings_ascii_windows );
    // ----------------
    settings.beginGroup("Repr");
    settings.setValue("Format",ui->cmBitRepr->currentIndex());
    settings.setValue("ShowDescr",ui->cbDescr->isChecked());
    settings.setValue("TrimValues",ui->cbTrim->isChecked());
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
    for(int i=0;i<ui->cmStructure->count();i++)
        ui->cmStructure->setToolTip(m_recent_files[ui->cmStructure->itemText(i)]);

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

    applyStructure();
    update_content();
}

void MainWindow::applyStructure(){
    QElapsedTimer t;
    t.start();
    quint32 load_options =0;

    REG.clear();

    if(!REG.loadJsonData(ui->teRegister->toPlainText().toLatin1(),load_options)){
        QMessageBox::critical(0,"Error","Error parsing file");
    }
    ui->lbDataSize->setText(QString("%1[%2h] bits %3[%4h] bytes")
                            .arg(REG.size())
                            .arg(REG.size(),4,16,QChar('0'))
                            .arg(REG.size()/8)
                            .arg(REG.size()/8,4,16,QChar('0'))
                            );
    ui->cmItems->blockSignals(1);
    ui->cmItems->clear();
    ui->cmItems->addItems(REG.fieldsList());
    ui->lbFieldInfo->setText("");
    ui->cmItems->blockSignals(0);
}

static bool loadData(const QString &file_name, int format){

    QFile f;
    if(file_name.isEmpty() || !QFile::exists(file_name)) return false;

    f.setFileName(file_name);
    if(QFile::exists(file_name) && f.open(QFile::ReadOnly)){

        switch(format){

        case 0:{// ascii 32
            quint32 addr=0;
            QByteArray file_text = f.readAll();
            QByteArrayList lines;
            if(file_text.contains(QByteArray("\r\n"))){
                lines = file_text.replace("\r\n","\n").split('\n');
            }else{
                lines = file_text.split('\n');
            }
            REG.fill(1);
            for(int i=0;i<lines.count();i++){
                 bool ok= false;
                const quint32 value = lines[i].toUInt(&ok,16);
                if(!ok) break;
                if(addr<(quint32)REG.size()){
                    REG.setValue(addr,addr+32-1,value);
                }
                else{
                    REG.addField( QString("_word_%3[%1]=%2").arg(32).arg(value).arg(i));
                }
                addr+=32;
            }
        }
            break;

        case 1:{ //ascii 8
            quint32 addr=0;
            QByteArray file_text = f.readAll();
            QByteArrayList lines;
            if(file_text.contains(QByteArray("\r\n"))){
                lines = file_text.replace("\r\n","\n").split('\n');
            }else{
                lines = file_text.split('\n');
            }
            REG.fill(1);
            for(int i=0;i<lines.count();i++){
                bool ok= false;
                const quint32 value = lines[i].toUInt(&ok,16);
                if(!ok) break;
                if(addr<(quint32)REG.size()){
                    REG.setValue(addr,addr+8-1,value);
                }
                else{
                    REG.addField( QString("_byte_%3[%1]=%2").arg(8).arg(value).arg(i));
                }
                addr+=8;
            }
        }
            break;

        case 2:{ // binary
            QByteArray file_data = f.readAll();
            if(file_data.count() *8 >REG.size()){
                const QString the_rest = QString("_the_rest_[%1]").arg( file_data.count()*8 - REG.size() );
                REG.addField(the_rest);
            }
            REG.fromByteArray(file_data);
        }
            break;


        }
    }else{
        QMessageBox::critical(0,"reading",QString("Can't open file"));
        return false;
    }
    return true;
}
void MainWindow::on_pbLoadFile_clicked()
{
    bool load_data_ok = false;
    QString filename;
    if(REG.isEmpty()){
        QMessageBox::critical(0,"reading",QString("Reg is empty"));
        return ;
    }

    if(ui->cmFormat->currentIndex()==0){
        filename = QFileDialog::getOpenFileName(0,"Load",m_data_file_path,"Text(*.txt *.hex)");
        load_data_ok = loadData(filename, 0);

    }else if(ui->cmFormat->currentIndex()==1){
        filename = QFileDialog::getOpenFileName(0,"Load",m_data_file_path,"Mem file ISP(*.mem)");
        load_data_ok = loadData(filename, 1);
    }
    else{
        filename = QFileDialog::getOpenFileName(0,"Load",m_data_file_path,"Binary (*.bin);;Sb3 files(*.sb3);;Other binary(*.*)");
        load_data_ok = loadData(filename, 2);
    }
    ui->pbApply->setEnabled(1);
    if(load_data_ok){
        QFileInfo fi(filename);
        QFontMetrics fm(ui->lbResultFile->font());
        setWindowTitle(fi.fileName());
        QString text = QString("%1 (size=%2)").arg(fi.filePath()).arg(fi.size());
        bool cutted = false;
        while(fm.horizontalAdvance(text)>ui->lbResultFile->width()){
            text.remove(0,3);
            cutted = true;
        }
        if(cutted){
            text.insert(0,"...");
        }
        ui->lbResultFile->setText("<b>" + text + "</b>");
        m_data_file_path = fi.filePath();
        m_data_file_format  = ui->cmFormat->currentIndex();
        update_content();
    }
}

void MainWindow::saveRecentFiles(){
    QSettings settings(QString("%2/%1.ini").arg(qApp->applicationName()).arg(qApp->applicationDirPath()));
    const QStringList list = m_recent_files.values();
    settings.setValue("Files/RecentFiles",list);
}

void MainWindow::addRecentFiles(const QString &filename)
{
    QFileInfo fi(filename);
    m_recent_files[fi.baseName()] =fi.filePath();
}

void MainWindow::on_pbSetValue_clicked()
{
    REG.setFieldValue(ui->cmItems->currentText(), ui->leValue->value());
    update_content();
}


void MainWindow::on_cmStructure_activated(int index)
{    

    if(ui->cmStructure->currentIndex() == 0){
        QString filename = QFileDialog::getOpenFileName(0,"",QFileInfo(m_format_file_path).path(),"JSON (*.json)",0);
        if(QFile::exists(filename)){

            if(loadStructureFile(filename)){
                addRecentFiles(filename);
                saveRecentFiles();
                loadRecentFiles();

            }
        }
    }
    else{
        m_format_file_path = m_recent_files[ui->cmStructure->currentText()];
        loadStructureFile(m_format_file_path);
    }

    applyStructure();
}

bool MainWindow::loadStructureFile(const QString &filename){
    QFile f(filename);
    if(f.open(QIODevice::ReadOnly)){
        ui->teRegister->setPlainText(f.readAll());
        f.close();
        return true;
    }
    else{
        QMessageBox::critical(0,"Error",QString("Can't load file %1").arg(filename));
        return false;
    }
}

QString MainWindow::representFieldAsString(Register *preg, BitField *pfield, Represent represent){
    QString result;
    QString format;

    if(pfield->extras().contains("color")){
        result += QString("<span style=\"background-color:%1\">").arg(pfield->extra("color").toString());
    }
    const bool custom_repr = pfield->extras().contains("repr");

    QString offset;
    quint32 bitfield_offset = preg->indexOf(pfield->first());
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
    else if(ui->cmBitRepr->currentIndex() ==3){

        quint32 word_offset = bitfield_offset/8;
        quint32 bit_offset = bitfield_offset%32;

        if(pfield->size()==1){
            offset = QString("%1[%2]").arg(word_offset,4,16,QChar('0'))
                    .arg(bit_offset);
        }
        else if(pfield->size()%32 == 0){
            offset = QString("%1").arg(word_offset,4,16,QChar('0'));
        }
        else{
            offset = QString("%1[%3:%2]").arg(word_offset,4,16,QChar('0'))
                    .arg(bit_offset)
                    .arg(bit_offset+pfield->size()-1);
        }
    }


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
            result = QString(format)
                    .arg(pfield->name())
                    .arg(pfield->toUInt(),0,16)
                    .arg(pfield->size())
                    .arg(offset);

        }
        else if(pfield->size()<=16){
            result = QString(format)
                    .arg(pfield->name())
                    .arg(pfield->toUInt(),4,16,QChar('0'))
                    .arg(pfield->size())
                    .arg(offset);

        }
        else if(pfield->size()<=32){
            result = QString(format)
                    .arg(pfield->name())
                    .arg(pfield->toUInt(),8,16,QChar('0'))
                    .arg(pfield->size())
                    .arg(offset);

        }else{
            QString hex_part = pfield->toHex();

            if(ui->cbTrim->isChecked() &&hex_part.size()>10){
                hex_part = hex_part.mid(0,10)+"...";
            }
            if(pfield->size()%8 ==0)
                result = QString(format)
                        .arg(pfield->name())
                        .arg(hex_part)
                        .arg(QString("0x%1 bytes").arg(pfield->size()/8,0,16))
                        .arg(offset);
            else
                result = QString(format)
                        .arg(pfield->name())
                        .arg(hex_part)
                        .arg(pfield->size())
                        .arg(offset);

        }
    }
    if(pfield->extras().contains("color")){
        result += "</span>";
    }

    if(ui->cbDescr->isChecked() && !pfield->description().isEmpty()){
        result += QString("<br><font color=grey>%1</font>").arg(pfield->description());
    }

    return result;
}
void MainWindow::on_pbFill1_clicked()
{
    REG.fill(1);
    update_content();
}

void MainWindow::update_content()
{    
    Register *r;
    Register *r2;

    if(ui->leFilter->text().isEmpty()) r = &REG;
    else r = REG.temporary();

    QElapsedTimer t;
    t.start();
    analyzeChanges();
    const int bak_scroll_bar = ui->teResult->verticalScrollBar()->value();
    ui->teResult->clear();
    Register REG2 = *r;
    REG2.fill(1);

    foreach(const QString &item, r->fieldsList()){
        if(REG.field(item)->toByteArray() != REG2.field(item)->toByteArray()
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
            ui->teResult->appendHtml(QString("<b>___total=%1 bytes___</b>").arg(r->size()/8));
        else
            ui->teResult->appendHtml(QString("<b>___TOTAL=%1(0x%2)bytes or %3 bits___</b>").arg(r->size()/8).arg(r->size()/8,0,16).arg(r->size()));
    }

    ui->teResult->verticalScrollBar()->setValue(bak_scroll_bar);
}


void MainWindow::on_pbSaveToFile_clicked()
{
    QFile f;


    if(ui->cmFormat->currentIndex()==0){
        f.setFileName(QFileDialog::getSaveFileName(0,"",m_data_file_path,"Text (*.txt *.hex)",0));
        if(!f.fileName().isEmpty() ){
            m_data_file_path = QFileInfo(f).filePath();
            const QByteArray data_to_save = REG.toByteArray(Register::LSB);
            if(f.open(QIODevice::WriteOnly)){
                qint32 i=0;
                while(i<(quint32)REG.size()/32){

                    const qint32 addr_from =(qint32)i*32;
                    const qint32 addr_to =(qint32)(i+1)*32 -1;
                    const uint value = REG.sub(addr_from,addr_to)->toUInt();
                    if(m_settings_ascii_windows){
                        f.write(QString("%1\r\n").arg(value,8,16,QChar('0')).toLatin1());
                    }
                    else f.write(QString("%1\n").arg(value,8,16,QChar('0')).toLatin1());
                    i++;

                    // trim lines in ascii file
                    if(ui->cmTrimLines->isEnabled() && i>=ui->cmTrimLines->currentText().toUInt())
                        break;
                }
            }
        }
        else{
            QMessageBox::critical(0,"writing",QString("Can't open file"));
        }
    }
    // 8 bit
    else if(ui->cmFormat->currentIndex()==1){

    }
    // binary
    else {
        f.setFileName(QFileDialog::getSaveFileName(0,"",m_data_file_path,"Binary (*.bin);;Sb3 files(*.sb3);;Other binary(*.*)",0));
        if(!f.fileName().isEmpty()){
            m_data_file_path = QFileInfo(f).filePath();
            if(f.open(QIODevice::WriteOnly)){
                QByteArray data = REG.toByteArray(Register::LSB);
                f.write(data);
            }
        }
    }
    f.close();
}

typedef struct {
    QString filename;
    QString     data;
}HexFile;
QList<HexFile> m_hex_files;

//QString MainWindow::parseRegChangedParams(){
//    QString result;
//    QStringList items = REG.fieldsList();
//    Register REG2 = REG;
//    REG2.fill(1);
//    foreach(const QString &item,items){
//        if(REG.sub(item)->toByteArray() != REG2.sub(item)->toByteArray()){
//            result += representFieldAsString(REG.field(item),AS_IGNORESTYLE)+"\n";
//        }
//    }
//    return result;
//}


void MainWindow::on_cbTrim_toggled(bool checked)
{
    update_content();
}


void MainWindow::on_cmStructure_currentIndexChanged(int index)
{
    ui->pbApply->setEnabled(index!=0);
}

void MainWindow::on_cbDescr_toggled(bool checked)
{
    update_content();
}


void MainWindow::on_cmItems_activated(int index)
{
    if(REG.contains(ui->cmItems->itemText(index))){
        BitField *f = REG.field(ui->cmItems->itemText(index));
        ui->leValue->setValue(f->value());
        ui->lbFieldInfo->setText(QString("%1 bits").arg(f->size()));
    }
}


void MainWindow::on_cmProcessAll_clicked()
{
    //    QDir directory("../ifr0");
    //    const QStringList txt_files = directory.entryList(QStringList() << "*.txt",QDir::Files);
    //    foreach(const QString &csv_filename, txt_files) {
    //        //do whatever you need to do

    //        if(loadData(QString("../ifr0/%2").arg(csv_filename),0)){
    //            HexFile hex_file;
    //            hex_file.filename = csv_filename;
    //            hex_file.data = parseRegChangedParams();
    //            m_hex_files.append(hex_file);
    //        }
    //    }

    //    QFile f("output.csv");
    //    if(f.open(QIODevice::WriteOnly)){
    //        foreach(const HexFile &hex, m_hex_files){
    //            const QString line = QString("\"%1\",\"%2\"\n").arg(hex.filename).arg(hex.data);
    //            f.write(line.toLatin1());
    //        }
    //    }
    //    f.close();
}
//[
//{"name":"imageType[7:0]"},
//{"name":"reserved[12:8]"},
//{"name":"tzm_preset[1]"},
//{"name":"tzm_image_type[1]"},
//{"name":"reserved[1]"},
//{"name":"reserved[31:16]"}
//]




void MainWindow::on_pbRemoveFormat_clicked()
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


void MainWindow::on_cmFormat_currentIndexChanged(int index)
{
    ui->cmTrimLines->setEnabled(1);
    if(index>1){
        ui->cmTrimLines->setEnabled(0);
    }
}

void MainWindow::closeEvent(QCloseEvent *ev)
{
    saveRecentFiles();
    saveSettings();
    ev->accept();
}


void MainWindow::on_cbUseWindowsCRLF_toggled(bool checked)
{
    m_settings_ascii_windows = checked;
    saveSettings();
}


void MainWindow::on_pbReloadFile_clicked()
{
    loadData(m_data_file_path, m_data_file_format);
    update_content();
}


void MainWindow::saveStructureFile()
{
    QFile f(this->m_format_file_path)  ;
    if(f.open(QFile::WriteOnly)){
        f.write(ui->teRegister->toPlainText().toLatin1());
    }else{
        QMessageBox::critical(0,"Error",QString("Can't save file %1").arg(m_format_file_path));
    }
    f.close();
}

void MainWindow::saveAsStructureFile()
{
    QString filename =  QFileDialog::getSaveFileName(0,"",QFileInfo(m_format_file_path).path(),"JSON (*.json)",0);

    if(!filename.isEmpty()){
        QFile f(filename)  ;
        if(f.open(QFile::WriteOnly)){
            f.write(ui->teRegister->toPlainText().toLatin1());
            m_format_file_path  = filename;
        }else{
            QMessageBox::critical(0,"Error",QString("Can't save file %1").arg(m_format_file_path));
        }
        f.close();

        QFileInfo fi(filename);
        const QString item_name = fi.baseName();
        ui->cmStructure->setToolTip(filename);
        ui->cmStructure->addItem(item_name, filename);

        addRecentFiles(filename);
        saveRecentFiles();
    }
}


void MainWindow::on_pbFill0_clicked()
{
    REG.fill(0);
    update_content();
}


void MainWindow::on_cmBitRepr_currentIndexChanged(int index)
{
    update_content();
}



void MainWindow::on_pbFilter_clicked()
{
    if(!ui->leFilter->text().isEmpty()){
        QStringList l = REG.fieldsList();
        QRegExp match(ui->leFilter->text());
        match.setPatternSyntax(QRegExp::Wildcard);
        int i=0;
        while(i<l.count()){
            if(!match.exactMatch(l[i])) l.removeAt(i);
            else i++;
        }
        REG.sub(l);
    }
    update_content();
}


void MainWindow::on_leFilter_editingFinished()
{
    if(ui->leFilter->text().isEmpty()){
        update_content();
    }
}




void MainWindow::on_pbReload_clicked()
{
    REG.clear();

    applyStructure();
    update_content();
}

