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

signals:
    void pathChanged(QString path);

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

    QAction * squareAreaSelectAction;

    ImagePreviewWindow * imagePreviewWindow;
    SharedContextWindow * sharedContextWindow ;
    OpenCLContext * context_cl;

    QMap<QString, QStringList> paths;
    QMap<QString, QStringList>::const_iterator folder_iterator;

    QStringList files;
    QStringList::const_iterator file_iterator;
};

#endif // MAINWINDOW_H
