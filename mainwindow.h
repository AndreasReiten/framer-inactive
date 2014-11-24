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
#include <QProgressBar>
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
    void setHeader(QString path);
//    void setSelection(Selection rect);
//    void setBackground(Selection rect);
//    void applySelectionToNext();
    void saveProject();
    void loadProject();
    void setFiles(QMap<QString, QStringList> folder_map);
//    void removeImage();
    void applySelection();
    void applyAnalytics();
//    void peakHuntSelectedMode();
//    void applySelection();
    void setApplyMode(QString str);
    void applyPlaneMarker();
//    void analyze(QString);
//    void setIntegrationMode(QString str);
//    void setImage(ImageInfo image);
    void setBatchSize(int value);
    void setImageRange(int low, int high);
    
signals:
//    void pathRemoved(QString path);
//    void imageChanged(QString path);
//    void imageChanged(ImageInfo image);
//    void centerImage();
//    void outputTextAppended(QString str);
//    void outputTextChanged(QString str);
//    void integrateCurrentFrame(QString path, QRect rect);
//    void integrateImage();
//    void analyzeSeries();
//    void analyzeSet();
//    void peakHuntImage();
//    void peakHuntFolder();
//    void peakHuntSet();
//    void applySelectionToSeriesSet();
//    void applySelectionToSeries();
    void setSelection(QString str);
    void setPlaneMarkers(QString str);
    void analyze(QString str);
    void setChanged(SeriesSet set);
    
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
    
    QString working_dir;
    
    QTabWidget * tabWidget;
    QWidget * fileWidget;
    QMainWindow * imageWidget;
    QWidget * imageInnerWidget;
    QWidget * imageDisplayWidget;
    QPlainTextEdit * imageHeaderWidget;
    QPlainTextEdit * outputPlainTextEdit;
    QProgressBar * generalProgressBar;
    
    
    QLineEdit * fileFilter;
    FileTreeView * fileTreeView;
    FileSelectionModel * fileSelectionModel;
    
    QDockWidget * navigationDock;
    QDockWidget * settingsDock;
//    QDockWidget * calculationDock;
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
    QPushButton * nextSeriesPushButton;
    QPushButton * prevSeriesPushButton;
    QPushButton * removeCurrentPushButton;
    QPushButton * applySelectionPushButton;
    QPushButton * applyPlaneMarkerPushButton;
    QSpinBox * batchSizeSpinBox;
    QSpinBox * imageSpinBox;
    
    QComboBox * selectionModeComboBox;
    QComboBox * imageModeComboBox;
    QComboBox * tsfTextureComboBox;
    QComboBox * tsfAlphaComboBox;
    QDoubleSpinBox * dataMinDoubleSpinBox;
    QDoubleSpinBox * dataMaxDoubleSpinBox;
    QCheckBox * logCheckBox;
//    QCheckBox * autoBackgroundCorrectionCheckBox;
    QCheckBox * correctionLorentzCheckBox;
    QCheckBox * correctionNoiseCheckBox;
    QCheckBox * correctionPlaneCheckBox;
    QCheckBox * correctionClutterCheckBox;
    QCheckBox * correctionMedianCheckBox;
    QCheckBox * correctionPolarizationCheckBox;
    QCheckBox * correctionFluxCheckBox;
    QCheckBox * correctionExposureCheckBox;
    
//    QCheckBox * correctionLorentzCheckBox;
//    QCheckBox * correctionBackgroundCheckBox;
    QDoubleSpinBox * correctionNoiseDoubleSpinBox;
    QDoubleSpinBox * noiseCorrectionMaxDoubleSpinBox;
    QDoubleSpinBox * postCorrectionMinDoubleSpinBox;
    QDoubleSpinBox * postCorrectionMaxDoubleSpinBox;
    QSpinBox * correctionClutterSpinBox;
    QSpinBox * correctionMedianSpinBox;
    
    QWidget * correctionWidget;
    QDockWidget * correctionDock;

//    QComboBox * integrationModeComboBox;
    QPushButton * integratePushButton;
//    QPushButton * peakHuntPushButton;
    QPushButton * traceSetPushButton;
//    QPushButton * setSeriesBackgroundPushButton;
    QLineEdit * pathLineEdit;
    QToolBar * imageToolBar;
    
    QAction * saveProjectAction;
    QAction * loadProjectAction;
//    QAction * squareAreaSelectAlphaAction;
//    QAction * squareAreaSelectBetaAction;
    QAction * centerImageAction;
    QAction * showWeightCenterAction;
    
    ImagePreviewWindow * imagePreviewWindow;
    SharedContextWindow * sharedContextWindow ;
    OpenCLContext * context_cl;

//    SeriesSet folderSet;

    bool hasPendingChanges;
//    QString integration_mode;
    QString apply_mode;
    
    // Integration
    double integration_sum;
    int error;
};



#endif // MAINWINDOW_H
