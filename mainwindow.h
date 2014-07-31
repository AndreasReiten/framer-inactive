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

class Image
{
public:
    Image();
    ~Image();

    void setPath(QString str);
    QString path();
    
    void setSelection(QRectF rect);
    QRectF selection();
    
private:
    QString p_path;
    QRectF p_selection;
};

class ImageFolder
{
public:
    ImageFolder();
    ~ImageFolder();
    
    void setPath(QString str);
    QString path();
    int size();
    
    void setImages(QList<Image> list);
    
    Image * current();
    Image * next();
    Image * previous();
    Image * begin();
    
private:
    QString p_path;
    QList<Image> images;
    int i;
    
};

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

signals:
    void pathChanged(QString path);
    void centerImage();
    void selectionChanged(QRectF rect);

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
    QPushButton * applySelectionToFolderPushButton;
    QPushButton * applySelectionToNextPushButton;
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
    QAction * centerImageAction;
    
    ImagePreviewWindow * imagePreviewWindow;
    SharedContextWindow * sharedContextWindow ;
    OpenCLContext * context_cl;

//    QMap<QString, QStringList> paths;
//    QMap<QString, QStringList>::iterator folder_iterator;

//    QStringList files;
//    QStringList::iterator file_iterator;

    QList<ImageFolder> folders;
    QList<ImageFolder>::iterator folder_iterator;
//    QList<Image> images;
//    QList<Image>::iterator image_iterator;
};



#endif // MAINWINDOW_H
