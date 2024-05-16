#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "register.h"


typedef enum {
    AS_CHANGED,
    AS_UNCHANGED,
    AS_IGNORESTYLE
}Represent;

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void analyzeChanges();
    void loadSettings();
    void saveSettings();
    void loadRecentFiles();
    void setCurrentRecentStructure(const QString &filename);
    void saveRecentFiles();
    void addRecentFiles(const QString &filename);
    bool loadStructureFile(const QString &filename);

    void applyStructure();
private slots:
    void on_pbApply_clicked();

    void on_pbLoadFile_clicked();

    void on_pbSetValue_clicked();

    void on_pbFill1_clicked();

    void update_content();

    void on_pbSaveToFile_clicked();

    void on_cbTrim_toggled(bool checked);

    void on_cmStructure_currentIndexChanged(int index);

    void on_cmStructure_activated(int index);

    void on_cbDescr_toggled(bool checked);

    void on_cmItems_activated(int index);

    void on_cmProcessAll_clicked();    

    void on_pbRemoveFormat_clicked();

    void on_cmFormat_currentIndexChanged(int index);
    void on_cbUseWindowsCRLF_toggled(bool checked);

    void on_pbReloadFile_clicked();

    void saveStructureFile();
    void saveAsStructureFile();

    void on_pbFill0_clicked();

    void on_cmBitRepr_currentIndexChanged(int index);

    void on_pbFilter_clicked();

    void on_leFilter_editingFinished();

    void on_pbReload_clicked();

protected:
    void closeEvent(QCloseEvent *ev);

private:
    bool m_settings_ascii_windows;

    QHash<QString,QString> m_recent_files;
    QString m_format_file_path;
    QString m_data_file_path;
    int m_data_file_format;

    Ui::MainWindow *ui;
    QString representFieldAsString(Register *preg, BitField *f, Represent represent);
    //QString parseRegChangedParams();
};
#endif // MAINWINDOW_H
