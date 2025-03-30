#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "memorymap.h"
#include <QGroupBox>
#include <QLabel>
#include <QList>
#include <QListWidgetItem>
#include <QSpacerItem>

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
    typedef enum {
        DataFormatUnknown,
        DataFormatAsBinary,
        DataFormatAsU8,
        DataFormatAsVMEM,
        DataFormatAsU32,
        DataFormatAsLIST,
    }DataFormat;

    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void analyzeChanges();
    void loadSettings();
    void saveSettings();
    void loadRecentFiles();
    void setCurrentRecentStructure(const QString &filename);
    void saveRecentFiles();
    void addRecentFile(const QString &filename);

    // structure
    bool loadStructureFile(const QString &filename);
    void applyStructure();

    void getSelectedFieldValue();
    void setSelectedFieldValue();
    void buildEditFieldList();
    void updateFileNameTitle();

    bool loadData(const QString &file_name, DataFormat format);
    bool saveData(const QString &file_name, DataFormat format);

    // export to H
    void generateClassicH(const QString &filename);
    void generateBitFieldH(const QString &filename);
    void generateVariant3_H(const QString &filename);


Q_SIGNALS:
    void update_ui();

private slots:
    void updateItemsList();

    void saveStructureFile();
    void saveAsStructureFile();

    void loadFileAsList();
    void loadFileAsU8();
    void loadFileAsVMEM();
    void loadFileAsU32();
    void loadFileAsBin();

    void saveFileAsList();
    void saveFileAsU8();
    void saveFileAsVMEM();
    void saveFileAsU32();
    void saveFileAsBin();

    void update_output();    
    void removeStructureFile();
    void field_label_double_click(const QString &link);

    void on_pbApply_clicked();    

    void on_pbSetValue_clicked();

    void on_pbFill1_clicked();

    void on_cbTrim_toggled(bool checked);

    void on_cmStructure_currentIndexChanged(int index);

    void on_cmStructure_activated(int index);

    void on_cbDescr_toggled(bool checked);

    void on_cmEditField_activated(int index);

    void on_cbUseWindowsCRLF_toggled(bool checked);

    void on_pbReloadFile_clicked();    

    void on_pbFill0_clicked();

    void on_cmBitRepr_currentIndexChanged(int index);

    void on_pbFilter_clicked();

    void on_leFilter_editingFinished();

    void on_pbReload_clicked();

    void on_pbGetValue_clicked();

    void on_pbExporToCode_clicked();

    void on_pushButton_clicked();    

    void on_cmFieldEditType_currentIndexChanged(int index);

    void on_cmFieldEditType_activated(int index);

    void on_cbIgnoreSpareBits_toggled(bool checked);

    void on_pushButton_2_clicked();

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();    

    void on_cbPrintPath_toggled(bool checked);

protected:
    void closeEvent(QCloseEvent *ev);

private:
    bool openExcel(const QString &filename);
    int selectFieldsByPath(const QString &path, QStringList *fields);
    MemoryMap m_map;
    QList<QGroupBox *> m_field_labels;//??
    typedef struct{
        BitField *bitField;
        quint32 byteAddr;
        quint32 offset;
    }BitFieldInfo;
    QMap<QString, QList<BitFieldInfo>> m_memory_map_fields;

    bool m_settings_ascii_windows;
    QSpacerItem  *m_ui_spacer;
    QLabel* makeFieldLabel(const BitFieldInfo &fi);
    void clearFieldLabels();

    QAction *actSaveDataFileAsU32;
    QAction *actSaveDataFileAsU8;
    QAction *actSaveDataFileAsVMEM;
    QAction *actSaveDataFileAsList;
    QAction *actSaveDataFileAsBin;

    QAction *actLoadDataFileAsU32;
    QAction *actLoadDataFileAsU8;
    QAction *actLoadDataFileAsVMEM;
    QAction *actLoadDataFileAsList;
    QAction *actLoadDataFileAsBin;

    QAction *actStructFileSave;
    QAction *actStructFileSaveAs;
    QAction *actStructFileRemove;

    QHash<QString,QString> m_recent_files;
    QString m_format_file_path;
    QString m_data_file_path;
    DataFormat m_data_file_format;

    Ui::MainWindow *ui;
    QString representFieldAsString(Register *preg, BitField *f, Represent represent);
    QString buildJson();


    //QString parseRegChangedParams();
};
#endif // MAINWINDOW_H
