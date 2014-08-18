#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QTabWidget>
#include <QDockWidget>
#include <QLineEdit>
#include <QPlainTextEdit>
#include <QComboBox>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QLineEdit>
#include <QToolBar>
#include <QAction>
#include <QCheckBox>
#include <QRect>

#include "lib/qxlib/qxlib.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT
public slots:
    void setTab(int value);
    void loadPaths();
    void nextFrame();
    void previousFrame();
    void batchForward();
    void batchBackward();
    void nextFolder();
    void previousFolder();
    void setHeader(QString path);
    void setSelection(QRectF rect);
    void applySelectionToFolder();
    void applySelectionToNext();
    void saveProject();
    void loadProject();
    void setFiles(QMap<QString, QStringList> folder_map);
    void removeImage();
    void integrateSingle();
    void integrateFolder();
    void integrateSelectedMode();
    void setIntegrationMode(int value);
    void setIntegrationResults(double sum, int err);
    
signals:
    void pathRemoved(QString path);
    void pathChanged(QString path);
//    void frameChanged(QString path, QRectF region);
    void centerImage();
    void selectionChanged(QRectF rect);
    void outputTextAppended(QString str);
    void outputTextChanged(QString str);
    void integrateCurrentFrame(QString path, QRectF rect);
    void integrateImage(Image image);
    void integrateFolder(ImageFolder folder);
    void integrateSet(FolderSet set);

public:
    MainWindow(QWidget *parent = 0);
    ~MainWindow();
    
protected:
    void closeEvent(QCloseEvent *event);
    
private:
    void initLayout();
    void setStartConditions();
    void readSettings();
    void writeSettings();
    
    QTabWidget * tabWidget;
    QWidget * fileWidget;
    QMainWindow * imageWidget;
    QWidget * imageInnerWidget;
    QWidget * imageDisplayWidget;
    QPlainTextEdit * imageHeaderWidget;
    QPlainTextEdit * outputPlainTextEdit;
    
    
    QLineEdit * fileFilter;
    FileTreeView * fileTreeView;
    FileSelectionModel * fileSelectionModel;
    
    QDockWidget * navigationDock;
    QDockWidget * settingsDock;
    QDockWidget * calculationDock;
    QDockWidget * headerDock;

    QWidget * navigationWidget;
    QWidget * settingsWidget;
    QWidget * calculationWidget;

    QPushButton * loadPathsPushButton;
    QPushButton * nextFramePushButton;
    QPushButton * previousFramePushButton;
    QPushButton * batchForwardPushButton;
    QPushButton * batchBackwardPushButton;
    size_t batch_size;
    QPushButton * nextFolderPushButton;
    QPushButton * previousFolderPushButton;
    QPushButton * applySelectionToFolderPushButton;
    QPushButton * applySelectionToNextPushButton;
    QPushButton * removeCurrentPushButton;
//    QSpinBox * frameIndexSpinBox;

    QComboBox * imageModeComboBox;
    QComboBox * tsfTextureComboBox;
    QComboBox * tsfAlphaComboBox;
    QDoubleSpinBox * dataMinDoubleSpinBox;
    QDoubleSpinBox * dataMaxDoubleSpinBox;
    QCheckBox * logCheckBox;
    QCheckBox * correctionCheckBox;


    QComboBox * integrationModeComboBox;
    QPushButton * integratePushButton;

    QLineEdit * pathLineEdit;
    QToolBar * imageToolBar;
    
    QAction * saveProjectAction;
    QAction * loadProjectAction;
    QAction * squareAreaSelectAction;
    QAction * centerImageAction;
    
    ImagePreviewWindow * imagePreviewWindow;
    SharedContextWindow * sharedContextWindow ;
    OpenCLContext * context_cl;

    FolderSet folderSet;

//    QList<ImageFolder> folders;
//    QList<ImageFolder>::iterator folder_iterator;
    
    bool hasPendingChanges;
    int integration_mode;
    
    // Integration
    double integration_sum;
    int error;
};



#endif // MAINWINDOW_H
