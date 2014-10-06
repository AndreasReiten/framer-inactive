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
    void setSelection(Selection rect);
    void setBackground(Selection rect);
    void applySelectionToAll();
    void applySelectionToFolder();
    void applySelectionToNext();
    void saveProject();
    void loadProject();
    void setFiles(QMap<QString, QStringList> folder_map);
    void removeImage();
//    void analyzeSingle();
//    void analyzeFolder();
    void integrateSelectedMode();
    void peakHuntSelectedMode();
    void applySelectionMode();
    void setSelectionMode(int value);
    void setIntegrationMode(int value);
    void setImage(ImageInfo image);
//    void setIntegrationResults(double sum, int err);
    
signals:
    void pathRemoved(QString path);
    void imageChanged(QString path);
    void imageChanged(ImageInfo image);
    void centerImage();
//    void selectionChanged(QRect rect);
    void outputTextAppended(QString str);
    void outputTextChanged(QString str);
    void integrateCurrentFrame(QString path, QRect rect);
    void integrateImage(ImageInfo image);
    void analyzeFolder(ImageFolder folder);
    void analyzeSet(FolderSet set);
    void peakHuntImage(ImageInfo image);
    void peakHuntFolder(ImageFolder folder);
    void peakHuntSet(FolderSet set);

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
    QDockWidget * selectionDock;

    QWidget * navigationWidget;
    QWidget * settingsWidget;
    QWidget * calculationWidget;
    QWidget * selectionWidget;

    QPushButton * loadPathsPushButton;
    QPushButton * nextFramePushButton;
    QPushButton * previousFramePushButton;
    QPushButton * batchForwardPushButton;
    QPushButton * batchBackwardPushButton;
    size_t batch_size;
    QPushButton * nextFolderPushButton;
    QPushButton * previousFolderPushButton;
//    QPushButton * applySelectionToFolderPushButton;
//    QPushButton * applySelectionToNextPushButton;
    QPushButton * removeCurrentPushButton;
    QPushButton * applySelectionPushButton;
    
    QComboBox * selectionModeComboBox;
    QComboBox * imageModeComboBox;
    QComboBox * tsfTextureComboBox;
    QComboBox * tsfAlphaComboBox;
    QDoubleSpinBox * dataMinDoubleSpinBox;
    QDoubleSpinBox * dataMaxDoubleSpinBox;
    QCheckBox * logCheckBox;
    QCheckBox * autoBackgroundCorrectionCheckBox;
    QCheckBox * correctionLorentzCheckBox;
    QDoubleSpinBox * noiseCorrectionMinDoubleSpinBox;
    QDoubleSpinBox * noiseCorrectionMaxDoubleSpinBox;
    QDoubleSpinBox * postCorrectionMinDoubleSpinBox;
    QDoubleSpinBox * postCorrectionMaxDoubleSpinBox;
    QWidget * correctionWidget;
    QDockWidget * correctionDock;

    QComboBox * integrationModeComboBox;
    QPushButton * integratePushButton;
    QPushButton * peakHuntPushButton;
    QLineEdit * pathLineEdit;
    QToolBar * imageToolBar;
    
    QAction * saveProjectAction;
    QAction * loadProjectAction;
    QAction * squareAreaSelectAlphaAction;
    QAction * squareAreaSelectBetaAction;
    QAction * centerImageAction;
    QAction * showWeightCenterAction;
    
    ImagePreviewWindow * imagePreviewWindow;
    SharedContextWindow * sharedContextWindow ;
    OpenCLContext * context_cl;

    FolderSet folderSet;

    bool hasPendingChanges;
    int integration_mode;
    int selection_mode;
    
    // Integration
    double integration_sum;
    int error;
};



#endif // MAINWINDOW_H
