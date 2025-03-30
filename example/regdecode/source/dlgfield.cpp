#include "dlgfield.h"
#include "ui_dlgfield.h"

dlgField::dlgField(const QVariantMap &data, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::dlgField)
{
    ui->setupUi(this);
    ui->leByteOffset->setVisible(0);
    ui->lbByteOffset->setVisible(0);
    ui->leValue->setValue(data["value"].toUInt());
    ui->leName->setText(data["name"].toString());
    ui->leLSB->setText(data["lsb"].toString());
    ui->leMSB->setText(data["msb"].toString());
    ui->teDescr->setPlainText(data["descr"].toString());
    ui->lePath->setText(data["path"].toString());

}

dlgField::~dlgField()
{
    delete ui;
}

void dlgField::on_cmOffset_activated(int index)
{
    switch(index){
    case 0:
        ui->leByteOffset->setVisible(0);
        ui->lbByteOffset->setVisible(0);
        break;
    case 1:
        ui->leByteOffset->setVisible(0);
        ui->lbByteOffset->setVisible(0);
        break;
    case 2:
        ui->leByteOffset->setVisible(1);
        ui->lbByteOffset->setVisible(1);
        break;
    }
}

QVariantMap dlgField::resultData()
{
    QVariantMap data;
    data["name"] = ui->leName->text();
    data["lsb"] = ui->leLSB->text().toInt();
    data["msb"] = ui->leMSB->text().toInt();
    data["descr"] = ui->teDescr->toPlainText();
    data["path"] = ui->lePath->text();
    data["value"] = ui->leValue->value();
    return data;
}

