#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "memorymap.h"
#include <QGroupBox>
#include <QLabel>
#include <QList>
#include <QListWidgetItem>
#include <QSpacerItem>
#include <QTextCursor>

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
        DataFormatUnknown=-1,
        DataFormatAsU32=0,
        DataFormatAsU8=1,
        DataFormatAsVMEM=2,
        DataFormatAsLIST=3,
        DataFormatAsBinary=4,
        DataFormatAsCDE=5,
        DataFormatAsCCODE=6,
    }DataFormat;
    static const char * m_data_formats[];

    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void analyzeChanges();
    void loadSettings();
    void saveSettings();
    void loadRecentFiles(bool save_again_enable= false);
    void setCurrentRecentStructure(const QString &filename);
    void saveRecentFiles();
    void addRecentFile(const QString &filename);

    // structure
    bool loadStructureFile(const QString &filename);
    void applyStructure();


    void updateFileNameTitle();

    bool loadDataFile(const QString &file_name, DataFormat format);
    bool saveDataFile(const QString &file_name, DataFormat format);

    // export to H
//    void generateClassicH(const QString &filename);
//    void generateBitFieldH(const QString &filename);
    void generateIfrMap(const QString &filename);
    void editBitField(const QString &field_name);
    bool saveHeaderFile(const QString &filename, const QString &header, const QString &bitfields);


Q_SIGNALS:
    void update_ui();
    void update_ui_controls();
    void dummyMsg(QString);
    void sig_updateOutput();
    void sig_updateConsoleIcon();
    void sig_updateEditFieldSize();
    void sig_updateRecentDataFilesList();

private slots:
    void selectEditFields(const QStringList &list);
    void fillMap1();
    void fillMap0();
    void getSelectedFieldValue();
    void setSelectedFieldValue();
    void buildEditFieldList();
    void updateEditFieldSize();
    void updateEditFieldFormat(const QString &format);
    void findTextItem_InMap(const QString &text);
    void updateConsoleIcon();
    void warning(const QString &);
    void error(const QString &);

    void updateItemsList();
    void updateUiControls();

    void saveStructureFile();
    void saveAsStructureFile();

    void loadFileAsList();
    void loadFileAsU8();
    void loadFileAsVMEM();
    void loadFileAsU32();
    void loadFileAsBin();
    void loadFileAsCde();

    void saveFileAsList();
    void saveFileAsU8();
    void saveFileAsVMEM();
    void saveFileAsU32();
    void saveFileAsBin();
    void saveFileAsCde();
    void saveFileAsCCode();

    void updateTools();

    void update_output();    
    void removeStructureFile();
    void field_label_double_click(const QString &link);

    void on_pbApply_clicked();        

    void on_cbTrim_toggled(bool checked);

    void on_cmStructure_currentIndexChanged(int index);

    void on_cmStructure_activated(int index);

    void on_cbDescr_toggled(bool checked);

    void on_cmEditField_activated(int index);

    void on_cbUseWindowsCRLF_toggled(bool checked);

    void on_pbReloadDataFile_clicked();    

    void on_cmBitRepr_currentIndexChanged(int index);    

    void on_leFilter_editingFinished();

    void on_pbReload_clicked();

    void on_pbExporToCode_clicked();       

    void on_cmFieldEditType_currentIndexChanged(int index);    

    void on_cbIgnoreSpareBits_toggled(bool checked);    

    void on_pushButton_3_clicked();

    void on_pushButton_4_clicked();    

    void on_cbPrintPath_toggled(bool checked);

    void on_cbShowDiff_toggled(bool checked);

    void on_pbCrcCalc_clicked();

    void on_pbSetCrc_clicked();

    void on_leFilter_returnPressed();

    void on_pbConsole_toggled(bool checked);

    void on_cmDefaultFieldValue_currentIndexChanged(int index);

    void on_pbConsole_clicked();

    void on_leFindText_textChanged(const QString &arg1);

    void on_cbCaseSens_toggled(bool checked);

    void on_cbBitfieldOperationEnable_toggled(bool checked);

    void on_leEditFieldValue_returnPressed();

    void clearMap();

    void on_pbClearMap_clicked();
    void updateRecentDataFiles();
    void loadRecentDataFile();
    void saveRecentDataFile();

protected:
    void closeEvent(QCloseEvent *ev);
    void keyPressEvent(QKeyEvent *const event);

private:
    QStringList m_filteredFieldsList;
    typedef struct{
        BitField *bitField;
        quint32 byteAddr;
        quint32 offset;
    }BitFieldInfo;

    typedef QMap<QString, QList<BitFieldInfo>> MemoryMapPaths_t;
    quint32 pathSize();

    bool openExcel(const QString &filename);
    int selectFieldsByPath(const QString &path, QStringList *fields);
    void generateMemoryMapPaths(QMap<QString, QList<BitFieldInfo>> *map_fields, int path_size);

    MemoryMap m_map;
    QList<QGroupBox *> m_field_labels;//??

    QMap<QString, QList<BitFieldInfo>> m_map_paths;
    QTextCursor m_backup_cursor;
    int m_cursor_position;
    void findTextInData();

    bool m_settings_ascii_windows;
    QSpacerItem  *m_ui_spacer;
    QLabel* makeFieldLabel(const BitFieldInfo &fi);
    void clearFieldLabels();
    void setupActions();
    QAction *actionByName(const QString &oper,const QString &action_name);
    QList<QAction*> actLoadFileAsList;
    int actionLoadFilesCount;
    QList<QAction*> actSaveFileAsList;
    int actionSaveFilesCount;

    QAction *actStructFileSave;
    QAction *actStructFileSaveAs;
    QAction *actStructFileRemove;

    // recent
    QHash<QString,QString> m_recent_structure_files;
    //
    typedef struct{
        QString filepath;
        DataFormat format;
        QString caption;
    }RecentFile;

    QList<RecentFile> m_recent_save_files;
    QList<RecentFile> m_recent_load_files;

    // current structure path
    QString m_structure_file_path;
    // current data file path
    QString m_data_file_path;
    // curent data file format
    DataFormat m_data_file_format;
    void setLastDataFile(DataFormat format, const QString &filename);
    Ui::MainWindow *ui;
    QString representFieldAsString(Register *preg, BitField *f, Represent represent);
    QString buildJson();
    void addRecentLoadDataFile(const RecentFile &recent_file);
    void addRecentSaveDataFile(const RecentFile &recent_file);
    QStringList editFieldsList();

    //QString parseRegChangedParams();
};
#endif // MAINWINDOW_H

