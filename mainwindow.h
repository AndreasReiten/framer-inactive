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
    const QString path() const;
    
    void setSelection(QRectF rect);
    const QRectF selection() const;
    
private:
    QString p_path;
    QRectF p_selection;
};

QDataStream &operator<<(QDataStream &out, const Image &image);
QDataStream &operator>>(QDataStream &in, Image &image);

class ImageFolder
{
public:
    ImageFolder();
    ~ImageFolder();
    
    void setPath(QString str);
    const QString path() const;
    const int size() const;
    
    void setImages(QList<Image> list);
    const QList<Image> &images() const;
    
    Image * current();
    Image * next();
    Image * previous();
    Image * begin();
    
private:
    QString p_path;
    QList<Image> p_images;
    int p_i;
    
};

QDataStream &operator<<(QDataStream &out, const ImageFolder &image_folder);
QDataStream &operator>>(QDataStream &in, ImageFolder &image_folder);

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
    
    QAction * saveProjectAction;
    QAction * loadProjectAction;
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
