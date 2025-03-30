#ifndef DLGFIELD_H
#define DLGFIELD_H

#include <QDialog>

namespace Ui {
class dlgField;
}

class dlgField : public QDialog
{
    Q_OBJECT

public:
    explicit dlgField(const QVariantMap &data, QWidget *parent = nullptr);
    ~dlgField();
    QVariantMap resultData();
private slots:
    void on_cmOffset_activated(int index);

private:
    Ui::dlgField *ui;

};

#endif // DLGFIELD_H
