#include "mainwindow.h"

#include <QGridLayout>
#include <QSettings>
#include <QMessageBox>
#include <QFileDialog>
#include <QDebug>
#include <QCoreApplication>



MainWindow::MainWindow(QWidget *parent) : 
    QMainWindow(parent),
    hasPendingChanges(false)
//    integration_mode(0),
//    selection_mode(0)
{
    // Stylesheet
    QFile styleFile( ":/stylesheets/plain.qss" );
    styleFile.open( QFile::ReadOnly );
    QString style( styleFile.readAll() );
    styleFile.close();
    this->setStyleSheet(style);

    // UI
    initLayout();

    // Session settings
    readSettings();

    // Set start conditions
    setStartConditions();
}


MainWindow::~MainWindow()
{
    
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    writeSettings();
    event->accept();
}

void MainWindow::readSettings()
{
    QSettings settings("Norwegian University of Science and Technology", "Framer");
    QPoint pos = settings.value("position", QPoint(0, 0)).toPoint();
    QSize size = settings.value("size", QSize(400, 400)).toSize();
    working_dir = settings.value("working_dir").toString();
    resize(size);
    move(pos);
}

void MainWindow::writeSettings()
{
    QSettings settings("Norwegian University of Science and Technology", "Framer");
    settings.setValue("position", pos());
    settings.setValue("size", size());
    settings.setValue("working_dir", working_dir);
}

void MainWindow::integrateSelectedMode()
{
    if (selection_mode == "Series")
    {
        emit analyzeSeries();
    }
    else if (selection_mode == "Set")
    {
        emit analyzeSet();
    }
}

//void MainWindow::peakHuntSelectedMode()
//{
//    SeriesSet tmp(folderSet);
//    folderSet.current()->rememberCurrent();
//    folderSet.rememberCurrent();
    
//    switch (integration_mode)
//    {
//        case 0: // Single
//            emit peakHuntImage();
//            break;

//        case 1: // Folder
//            emit peakHuntFolder();
//            break;

//        case 2: // All
//            emit peakHuntSet();
//            break;

//        default: // Should not occur
//            break;
//    }
    
//    folderSet.restoreMemory();
//    folderSet.current()->restoreMemory();
    
//    emit imageChanged(*folderSet.current()->current());
//}


void MainWindow::applySelectionMode()
{
    if (selection_mode == "Series")
    {
        emit applySelectionToSeries();
    }
    else if (selection_mode == "Set")
    {
        emit applySelectionToSeriesSet();
    }
}

void MainWindow::setSelectionMode(QString str)
{
    selection_mode = str;
}

void MainWindow::setIntegrationMode(QString str)
{
    integration_mode = str;
}



void MainWindow::initLayout()
{
    // File widget
    fileFilter = new QLineEdit;
    
    fileSelectionModel =  new FileSelectionModel;
    fileSelectionModel->setRootPath(QDir::rootPath());
    
    fileTreeView =  new FileTreeView;
    fileTreeView->setModel(fileSelectionModel);
    
    connect(fileFilter, SIGNAL(textChanged(QString)), fileSelectionModel, SLOT(setStringFilter(QString)));
    fileFilter->setText("*.cbf");
    
    loadPathsPushButton = new QPushButton;
    loadPathsPushButton->setIcon(QIcon(":/art/download.png"));
    loadPathsPushButton->setIconSize(QSize(86,86));
    connect(loadPathsPushButton, SIGNAL(clicked()), this, SLOT(loadPaths()));
    
    QGridLayout * fileWidgetLayout = new QGridLayout;
    fileWidgetLayout->addWidget(fileFilter,0,0,1,1);
    fileWidgetLayout->addWidget(fileTreeView,1,0,1,1);
    fileWidgetLayout->addWidget(loadPathsPushButton,2,0,1,1);
    
    fileWidget =  new QWidget;
    fileWidget->setLayout(fileWidgetLayout);
    
    // Image widget
    imageWidget =  new QMainWindow;

    // Toolbar
    pathLineEdit = new QLineEdit("/path/to/file");
    pathLineEdit->setReadOnly(true);

    imageToolBar = new QToolBar("Image");

    saveProjectAction = new QAction(QIcon(":/art/save.png"), tr("Save project"), this);
    loadProjectAction = new QAction(QIcon(":/art/open.png"), tr("Load project"), this);
    
//    squareAreaSelectAlphaAction = new QAction(QIcon(":/art/select.png"), tr("Toggle pixel selection"), this);
//    squareAreaSelectAlphaAction->setCheckable(true);
//    squareAreaSelectAlphaAction->setChecked(false);

//    squareAreaSelectBetaAction = new QAction(QIcon(":/art/select2.png"), tr("Toggle background selection"), this);
//    squareAreaSelectBetaAction->setCheckable(true);
//    squareAreaSelectBetaAction->setChecked(false);
    
    centerImageAction = new QAction(QIcon(":/art/center.png"), tr("Center image"), this);
    centerImageAction->setCheckable(false);

    showWeightCenterAction = new QAction(QIcon(":/art/weight_center.png"), tr("Toggle weight center visual"), this);
    showWeightCenterAction->setCheckable(true);
    showWeightCenterAction->setChecked(false);

    imageToolBar->addAction(saveProjectAction);
    imageToolBar->addAction(loadProjectAction);
    imageToolBar->addAction(centerImageAction);
    imageToolBar->addAction(showWeightCenterAction);
//    imageToolBar->addAction(squareAreaSelectAlphaAction);
//    imageToolBar->addAction(squareAreaSelectBetaAction);
    imageToolBar->addWidget(pathLineEdit);

    imageWidget->addToolBar(Qt::TopToolBarArea, imageToolBar);

    // Dock widget
    nextFramePushButton = new QPushButton(QIcon(":/art/forward.png"),"Next");
    previousFramePushButton = new QPushButton(QIcon(":/art/back.png"),"Prev");
    batchForwardPushButton = new QPushButton(QIcon(":/art/fast_forward.png"), "Skip");
    batchBackwardPushButton = new QPushButton(QIcon(":/art/fast_back.png"), "Skip");
    nextSeriesPushButton = new QPushButton(QIcon(":/art/next_series.png"),"Next series");
    nextSeriesPushButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
    prevSeriesPushButton = new QPushButton(QIcon(":/art/prev_series.png"),"Prev series");
    prevSeriesPushButton->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::MinimumExpanding);
    
    removeCurrentPushButton = new QPushButton(QIcon(":/art/kill.png"),"Remove frame");
    
    imageSpinBox = new QSpinBox;
    imageSpinBox->setPrefix("Frame: ");
    
    batchSizeSpinBox = new QSpinBox;
    batchSizeSpinBox->setPrefix("Skip size: ");
    
    connect(batchSizeSpinBox, SIGNAL(valueChanged(int)), this, SLOT(setBatchSize(int)));
    
    generalProgressBar = new QProgressBar;
    generalProgressBar->hide();
    generalProgressBar->setFormat("%v of %m");

    QGridLayout * navigationLayout = new QGridLayout;
    
    navigationLayout->addWidget(batchBackwardPushButton,0,0,1,1);
    navigationLayout->addWidget(previousFramePushButton,0,1,1,1);
    navigationLayout->addWidget(imageSpinBox,0,2,1,2);
    navigationLayout->addWidget(nextFramePushButton,0,4,1,1);
    navigationLayout->addWidget(batchForwardPushButton,0,5,1,1);
    
    navigationLayout->addWidget(prevSeriesPushButton,1,0,2,2);
    navigationLayout->addWidget(batchSizeSpinBox,1,2,1,2);
    navigationLayout->addWidget(nextSeriesPushButton,1,4,2,2);
    
    navigationLayout->addWidget(removeCurrentPushButton, 2, 2, 1 , 2);

    navigationLayout->addWidget(generalProgressBar, 3, 0, 1 , 6);
    
    navigationWidget = new QWidget;
    navigationWidget->setLayout(navigationLayout);

    navigationDock =  new QDockWidget("Navigation");
//    navigationDock->setAllowedAreas(Qt::RightDockWidgetArea | Qt::LeftDockWidgetArea);
    navigationDock->setWidget(navigationWidget);
    navigationDock->setFixedHeight(navigationWidget->minimumSizeHint().height()*1.2);
    imageWidget->addDockWidget(Qt::BottomDockWidgetArea, navigationDock);
    
    // Dock widget
    imageModeComboBox = new QComboBox;
    imageModeComboBox->addItem("Normal");
    imageModeComboBox->addItem("Variance");
    imageModeComboBox->addItem("Skewness");

    tsfTextureComboBox = new QComboBox;
    tsfTextureComboBox->addItem(trUtf8("Rainbow"));
    tsfTextureComboBox->addItem(trUtf8("Hot"));
    tsfTextureComboBox->addItem(trUtf8("Hsv"));
    tsfTextureComboBox->addItem(trUtf8("Galaxy"));
    tsfTextureComboBox->addItem(trUtf8("Binary"));
    tsfTextureComboBox->addItem(trUtf8("Yranib"));

    tsfAlphaComboBox = new QComboBox;
    tsfAlphaComboBox->addItem("Linear");
    tsfAlphaComboBox->addItem("Exponential");
    tsfAlphaComboBox->addItem("Uniform");
    tsfAlphaComboBox->addItem("Opaque");

    dataMinDoubleSpinBox = new QDoubleSpinBox;
    dataMinDoubleSpinBox->setRange(-1e9,1e9);
    dataMinDoubleSpinBox->setAccelerated(true);
    dataMinDoubleSpinBox->setPrefix("Data min: ");

    dataMaxDoubleSpinBox = new QDoubleSpinBox;
    dataMaxDoubleSpinBox->setRange(-1e9,1e9);
    dataMaxDoubleSpinBox->setAccelerated(true);
    dataMaxDoubleSpinBox->setPrefix("Data max: ");
    
    logCheckBox = new QCheckBox("Log");

    QGridLayout * settingsLayout = new QGridLayout;
    settingsLayout->addWidget(imageModeComboBox,0,1,1,2);
    settingsLayout->addWidget(tsfTextureComboBox,1,1,1,1);
    settingsLayout->addWidget(tsfAlphaComboBox,1,2,1,1);
    settingsLayout->addWidget(dataMinDoubleSpinBox,2,1,1,2);
    settingsLayout->addWidget(dataMaxDoubleSpinBox,3,1,1,2);
    settingsLayout->addWidget(logCheckBox,4,1,1,1);
    


    settingsWidget = new QWidget;
    settingsWidget->setLayout(settingsLayout);


    settingsDock =  new QDockWidget("Display settings");
//    settingsDock->setAllowedAreas(Qt::RightDockWidgetArea | Qt::LeftDockWidgetArea);
    settingsDock->setWidget(settingsWidget);
    settingsDock->setFixedHeight(settingsWidget->minimumSizeHint().height()*1.2);
    imageWidget->addDockWidget(Qt::LeftDockWidgetArea, settingsDock);
    
    // Dock widget
    traceSetPushButton = new QPushButton("Trace set");
//    setSeriesBackgroundPushButton = new QPushButton("Set series b/g");
    
    correctionNoiseDoubleSpinBox = new QDoubleSpinBox;
    correctionNoiseDoubleSpinBox->setRange(0,1e4);
//    noiseCorrectionMinDoubleSpinBox->setPrefix("Noise: ");
    
    noiseCorrectionMaxDoubleSpinBox = new QDoubleSpinBox;
    noiseCorrectionMaxDoubleSpinBox->setRange(-1e6,1e6);
    
    postCorrectionMinDoubleSpinBox = new QDoubleSpinBox;
    postCorrectionMinDoubleSpinBox->setRange(-1e6,1e6);
    postCorrectionMinDoubleSpinBox->setPrefix("PCT: ");
    
    postCorrectionMaxDoubleSpinBox = new QDoubleSpinBox;
    postCorrectionMaxDoubleSpinBox->setRange(-1e6,1e6);
    
    correctionClutterSpinBox = new QSpinBox;
    correctionClutterSpinBox->setRange(0,100);
    correctionClutterSpinBox->setSuffix(" units");
            
    correctionMedianSpinBox = new QSpinBox;
    correctionMedianSpinBox->setRange(0,100);
    correctionMedianSpinBox->setPrefix("n x n: ");
            
    correctionNoiseCheckBox = new QCheckBox("Flat b/g subtract: ");
    correctionPlaneCheckBox = new QCheckBox("Planar b/g subtract");
    correctionClutterCheckBox = new QCheckBox("Clutter removal: ");
    correctionMedianCheckBox = new QCheckBox("Median filter: ");
    correctionLorentzCheckBox = new QCheckBox("Lorentz correction");
    correctionPolarizationCheckBox = new QCheckBox("Polarization correction");
    correctionFluxCheckBox = new QCheckBox("Flux normalization");
    correctionExposureCheckBox = new QCheckBox("Exposure time normalization");
    
//    correctionBackgroundCheckBox = new QCheckBox("Background correction");
//    autoBackgroundCorrectionCheckBox = new QCheckBox("Automatic background subtraction");
    

    
    QGridLayout * correctionLayout = new QGridLayout;
    correctionLayout->addWidget(traceSetPushButton,0,0,1,2);
    correctionLayout->addWidget(correctionNoiseCheckBox,1,0,1,1);
    correctionLayout->addWidget(correctionNoiseDoubleSpinBox,1,1,1,1);
    correctionLayout->addWidget(correctionPlaneCheckBox,2,0,1,2);
    correctionLayout->addWidget(correctionClutterCheckBox,3,0,1,1);
    correctionLayout->addWidget(correctionClutterSpinBox,3,1,1,1);
    correctionLayout->addWidget(correctionMedianCheckBox,4,0,1,1);
    correctionLayout->addWidget(correctionMedianSpinBox,4,1,1,1);
    correctionLayout->addWidget(correctionLorentzCheckBox,5,0,1,2);
    correctionLayout->addWidget(correctionPolarizationCheckBox,6,0,1,2);
    correctionLayout->addWidget(correctionFluxCheckBox,7,0,1,2);
    correctionLayout->addWidget(correctionExposureCheckBox,8,0,1,2);
    
    correctionWidget = new QWidget;
    correctionWidget->setLayout(correctionLayout);
    
    correctionDock =  new QDockWidget("Corrections");
//    correctionDock->setAllowedAreas(Qt::RightDockWidgetArea | Qt::LeftDockWidgetArea);
    correctionDock->setWidget(correctionWidget);
    correctionDock->setFixedHeight(correctionWidget->minimumSizeHint().height()*1.2);
    imageWidget->addDockWidget(Qt::RightDockWidgetArea, correctionDock);
    
    
    
    
    // Dock widget
    applySelectionPushButton  = new QPushButton("Apply selection");

    selectionModeComboBox = new QComboBox;
    selectionModeComboBox->addItem("Series");
    selectionModeComboBox->addItem("Set");

    QGridLayout *selectionLayout = new QGridLayout;
    selectionLayout->addWidget(selectionModeComboBox, 0, 0, 1, 2);
    selectionLayout->addWidget(applySelectionPushButton, 1, 0, 1 , 2);

    selectionWidget = new QWidget;
    selectionWidget->setLayout(selectionLayout);

    selectionDock =  new QDockWidget("Area selection");
//    selectionDock->setAllowedAreas(Qt::RightDockWidgetArea | Qt::LeftDockWidgetArea);
    selectionDock->setFixedHeight(selectionWidget->minimumSizeHint().height()*1.2);
    selectionDock->setWidget(selectionWidget);
    imageWidget->addDockWidget(Qt::RightDockWidgetArea, selectionDock);

    // Dock widget
    integrationModeComboBox = new QComboBox;
    integrationModeComboBox->addItem("Series");
    integrationModeComboBox->addItem("Set");

    integratePushButton = new QPushButton("Analyze");
    
//    peakHuntPushButton = new QPushButton("Hunt peak");

    QGridLayout * calculationLayout = new QGridLayout;
    calculationLayout->addWidget(integrationModeComboBox,0,0,1,1);
    calculationLayout->addWidget(integratePushButton,1,0,1,1);

    calculationWidget = new QWidget;
    calculationWidget->setLayout(calculationLayout);

    calculationDock =  new QDockWidget("Calculations");
//    calculationDock->setAllowedAreas(Qt::RightDockWidgetArea | Qt::LeftDockWidgetArea);
    calculationDock->setFixedHeight(calculationWidget->minimumSizeHint().height()*1.2);
    calculationDock->setWidget(calculationWidget);
    imageWidget->addDockWidget(Qt::RightDockWidgetArea, calculationDock);
    
    // Dock widget
    imageHeaderWidget = new QPlainTextEdit;
    imageHeaderWidget->setReadOnly(true);


    connect(fileTreeView, SIGNAL(fileChanged(QString)), this, SLOT(setHeader(QString)));

    headerDock = new QDockWidget("Header");
    headerDock->setWidget(imageHeaderWidget);
//    headerDock->setAllowedAreas(Qt::RightDockWidgetArea | Qt::LeftDockWidgetArea);
    this->addDockWidget(Qt::RightDockWidgetArea, headerDock);
    
    // Set the OpenCL context
    context_cl = new OpenCLContext;

    // Set the format of the rendering context
    QSurfaceFormat format_gl;
    format_gl.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
    format_gl.setSwapInterval(1);
    format_gl.setSamples(16);
    format_gl.setRedBufferSize(8);
    format_gl.setGreenBufferSize(8);
    format_gl.setBlueBufferSize(8);
    format_gl.setAlphaBufferSize(8);

    // Use a common object to hold a shareable GL and CL context
    sharedContextWindow = new SharedContextWindow();
    sharedContextWindow->setFormat(format_gl);
    sharedContextWindow->setOpenCLContext(context_cl);
    sharedContextWindow->show();
    sharedContextWindow->initializeWorker();
    sharedContextWindow->hide();

    // Set up a rendering surface
    imagePreviewWindow = new ImagePreviewWindow();
    imagePreviewWindow->setMultiThreading(true);
    imagePreviewWindow->setSharedWindow(sharedContextWindow);
    imagePreviewWindow->setFormat(format_gl);
    imagePreviewWindow->setOpenCLContext(context_cl);
    imagePreviewWindow->initializeWorker();

    // Apply the rendering surface to a widget
    imageDisplayWidget = QWidget::createWindowContainer(imagePreviewWindow);
    imageDisplayWidget->setFocusPolicy(Qt::StrongFocus);
    imageWidget->setCentralWidget(imageDisplayWidget);

//    connect(this, SIGNAL(imageChanged(ImageInfo)), imagePreviewWindow->worker(), SLOT(setFrame(ImageInfo)));
//    connect(imagePreviewWindow->worker(), SIGNAL(imageChanged(ImageInfo)), this, SLOT(setImage(ImageInfo)));
    connect(tsfTextureComboBox, SIGNAL(currentIndexChanged(int)), imagePreviewWindow->worker(), SLOT(setTsfTexture(int)));
    connect(tsfAlphaComboBox, SIGNAL(currentIndexChanged(int)), imagePreviewWindow->worker(), SLOT(setTsfAlpha(int)));
    connect(dataMinDoubleSpinBox, SIGNAL(valueChanged(double)), imagePreviewWindow->worker(), SLOT(setDataMin(double)));
    connect(dataMaxDoubleSpinBox, SIGNAL(valueChanged(double)), imagePreviewWindow->worker(), SLOT(setDataMax(double)));
    connect(logCheckBox, SIGNAL(toggled(bool)), imagePreviewWindow->worker(), SLOT(setLog(bool)));
    connect(correctionLorentzCheckBox, SIGNAL(toggled(bool)), imagePreviewWindow->worker(), SLOT(setCorrectionLorentz(bool)));
//    connect(correctionBackgroundCheckBox, SIGNAL(toggled(bool)), imagePreviewWindow->worker(), SLOT(setCorrectionBackground(bool)));
//    connect(autoBackgroundCorrectionCheckBox, SIGNAL(toggled(bool)), imagePreviewWindow->worker(), SLOT(setAutoBackgroundCorrection(bool)));
    connect(imageModeComboBox, SIGNAL(currentIndexChanged(int)), imagePreviewWindow->worker(), SLOT(setMode(int)));
    connect(saveProjectAction, SIGNAL(triggered()), this, SLOT(saveProject()));
    connect(loadProjectAction, SIGNAL(triggered()), this, SLOT(loadProject()));

    
    connect(nextFramePushButton, SIGNAL(clicked()), this, SLOT(nextFrame()));
    connect(previousFramePushButton, SIGNAL(clicked()), this, SLOT(previousFrame()));
    connect(batchForwardPushButton, SIGNAL(clicked()), this, SLOT(batchForward()));
    connect(batchBackwardPushButton, SIGNAL(clicked()), this, SLOT(batchBackward()));
    connect(nextSeriesPushButton, SIGNAL(clicked()), imagePreviewWindow->worker(), SLOT(nextSeries()));
    connect(prevSeriesPushButton, SIGNAL(clicked()), imagePreviewWindow->worker(), SLOT(prevSeries()));
    connect(removeCurrentPushButton, SIGNAL(clicked()), imagePreviewWindow->worker(), SLOT(removeCurrentImage()));
    connect(imagePreviewWindow->worker(), SIGNAL(pathRemoved(QString)), fileSelectionModel, SLOT(removeFile(QString)));
    connect(this, SIGNAL(applySelectionToSeries()), imagePreviewWindow->worker(), SLOT(applySelectionToSeries()));
    connect(this, SIGNAL(applySelectionToSeriesSet()), imagePreviewWindow->worker(), SLOT(applySelectionToSeriesSet()));
    connect(imagePreviewWindow->worker(), SIGNAL(pathChanged(QString)), this, SLOT(setHeader(QString)));
    connect(imagePreviewWindow->worker(), SIGNAL(pathChanged(QString)), pathLineEdit, SLOT(setText(QString)));
    connect(imageSpinBox, SIGNAL(valueChanged(int)), imagePreviewWindow->worker(), SLOT(setFrameByIndex(int)));
    connect(imagePreviewWindow->worker(), SIGNAL(imageRangeChanged(int,int)), this, SLOT(setImageRange(int, int)));
    connect(imagePreviewWindow->worker(), SIGNAL(currentIndexChanged(int)), imageSpinBox, SLOT(setValue(int)));
    connect(this, SIGNAL(setChanged(SeriesSet)), imagePreviewWindow->worker(), SLOT(setSet(SeriesSet)));
    connect(traceSetPushButton, SIGNAL(clicked()), imagePreviewWindow->worker(), SLOT(traceSet()));
//    connect(setSeriesBackgroundPushButton, SIGNAL(clicked()), imagePreviewWindow->worker(), SLOT(setSeriesBackgroundBuffer()));
    connect(imagePreviewWindow->worker(), SIGNAL(progressChanged(int)), generalProgressBar, SLOT(setValue(int)));
    connect(imagePreviewWindow->worker(), SIGNAL(progressRangeChanged(int,int)), generalProgressBar, SLOT(setRange(int,int)));
    connect(imagePreviewWindow->worker(), SIGNAL(visibilityChanged(bool)), generalProgressBar, SLOT(setHidden(bool)));
    connect(correctionNoiseCheckBox,SIGNAL(toggled(bool)),imagePreviewWindow->worker(),SLOT(setCorrectionNoise(bool)));
    connect(correctionPlaneCheckBox,SIGNAL(toggled(bool)),imagePreviewWindow->worker(),SLOT(setCorrectionPlane(bool)));
    connect(correctionClutterCheckBox,SIGNAL(toggled(bool)),imagePreviewWindow->worker(),SLOT(setCorrectionClutter(bool)));
    connect(correctionMedianCheckBox,SIGNAL(toggled(bool)),imagePreviewWindow->worker(),SLOT(setCorrectionMedian(bool)));
    connect(correctionPolarizationCheckBox,SIGNAL(toggled(bool)),imagePreviewWindow->worker(),SLOT(setCorrectionPolarization(bool)));
    connect(correctionFluxCheckBox,SIGNAL(toggled(bool)),imagePreviewWindow->worker(),SLOT(setCorrectionFlux(bool)));
    connect(correctionExposureCheckBox,SIGNAL(toggled(bool)),imagePreviewWindow->worker(),SLOT(setCorrectionExposure(bool)));
    

    connect(centerImageAction, SIGNAL(triggered()), imagePreviewWindow->worker(), SLOT(centerImage()));
    connect(showWeightCenterAction, SIGNAL(toggled(bool)), imagePreviewWindow->worker(), SLOT(showWeightCenter(bool)));
//    connect(this, SIGNAL(centerImage()), imagePreviewWindow->worker(), SLOT(centerImage()));
    connect(integratePushButton,SIGNAL(clicked()),this,SLOT(integrateSelectedMode()));
//    connect(peakHuntPushButton,SIGNAL(clicked()),this,SLOT(peakHuntSelectedMode()));
    connect(applySelectionPushButton,SIGNAL(clicked()),this,SLOT(applySelectionMode()));
    connect(integrationModeComboBox,SIGNAL(currentIndexChanged(QString)),this,SLOT(setIntegrationMode(QString)));
    connect(selectionModeComboBox,SIGNAL(currentIndexChanged(QString)),this,SLOT(setSelectionMode(QString)));
    connect(this, SIGNAL(integrateImage()), imagePreviewWindow->worker(), SLOT(analyzeSingle()));
    connect(this, SIGNAL(analyzeSeries()), imagePreviewWindow->worker(), SLOT(analyzeSeries()));
    connect(this, SIGNAL(analyzeSet()), imagePreviewWindow->worker(), SLOT(analyzeSet()));
//    connect(squareAreaSelectAlphaAction, SIGNAL(toggled(bool)), imagePreviewWindow->worker(), SLOT(setSelectionAlphaActive(bool)));
//    connect(squareAreaSelectBetaAction, SIGNAL(toggled(bool)), imagePreviewWindow->worker(), SLOT(setSelectionBetaActive(bool)));
//    connect(imagePreviewWindow->worker(), SIGNAL(selectionAlphaChanged(bool)), squareAreaSelectAlphaAction, SLOT(setChecked(bool)));
//    connect(imagePreviewWindow->worker(), SIGNAL(selectionBetaChanged(bool)), squareAreaSelectBetaAction, SLOT(setChecked(bool)));
    connect(correctionNoiseDoubleSpinBox,SIGNAL(valueChanged(double)), imagePreviewWindow->worker(), SLOT(setThresholdNoiseLow(double)));
    connect(noiseCorrectionMaxDoubleSpinBox,SIGNAL(valueChanged(double)), imagePreviewWindow->worker(), SLOT(setThresholdNoiseHigh(double)));
    connect(postCorrectionMinDoubleSpinBox,SIGNAL(valueChanged(double)), imagePreviewWindow->worker(), SLOT(setThresholdPostCorrectionLow(double)));
    connect(postCorrectionMaxDoubleSpinBox,SIGNAL(valueChanged(double)), imagePreviewWindow->worker(), SLOT(setThresholdPostCorrectionHigh(double)));
    connect(imagePreviewWindow->worker(), SIGNAL(noiseLowChanged(double)), correctionNoiseDoubleSpinBox, SLOT(setValue(double)));
    
    // Text output widget
    outputPlainTextEdit = new QPlainTextEdit("Output is written in plain text here");
    outputPlainTextEdit->setReadOnly(true);
    
//    connect(this, SIGNAL(outputTextChanged(QString)),outputPlainTextEdit,SLOT(setPlainText(QString)));
//    connect(this, SIGNAL(outputTextAppended(QString)),outputPlainTextEdit,SLOT(appendPlainText(QString)));
    connect(imagePreviewWindow->worker(), SIGNAL(resultFinished(QString)), outputPlainTextEdit, SLOT(setPlainText(QString)));

    // Tab widget    
    tabWidget =  new QTabWidget;
    this->setCentralWidget(tabWidget);

    tabWidget->addTab(fileWidget, "Files");
    tabWidget->addTab(imageWidget, "View");
    tabWidget->addTab(outputPlainTextEdit, "Text output");
    
    connect(tabWidget, SIGNAL(currentChanged(int)), this, SLOT(setTab(int)));
}

void MainWindow::setStartConditions()
{
    batchSizeSpinBox->setValue(10);
    tsfTextureComboBox->setCurrentIndex(1);
    tsfAlphaComboBox->setCurrentIndex(2);
    dataMinDoubleSpinBox->setValue(10);
    dataMinDoubleSpinBox->setValue(0);
    dataMaxDoubleSpinBox->setValue(1000);
    logCheckBox->setChecked(true);
//    autoBackgroundCorrectionCheckBox->setChecked(false);
    correctionLorentzCheckBox->setChecked(false);
//    correctionBackgroundCheckBox->setChecked(false);
    imageModeComboBox->setCurrentIndex(1);
    imageModeComboBox->setCurrentIndex(0);
    
    correctionNoiseDoubleSpinBox->setValue(1);
    correctionNoiseDoubleSpinBox->setValue(0);
    noiseCorrectionMaxDoubleSpinBox->setValue(1e6);
    
    postCorrectionMinDoubleSpinBox->setValue(1);
    postCorrectionMinDoubleSpinBox->setValue(0);
    postCorrectionMaxDoubleSpinBox->setValue(1e6);
}



//void MainWindow::applySelectionToNext()
//{
//    if (folderSet.size() > 0)
//    {
//        Selection selection = folderSet.current()->current()->selection();
//        Selection background = folderSet.current()->current()->background();

//        folderSet.current()->next();
//        folderSet.current()->current()->setSelection(selection);
//        folderSet.current()->current()->setBackground(background);
        
//        emit imageChanged(*folderSet.current()->current());
//    }
//}

void MainWindow::setTab(int value)
{
    Q_UNUSED(value);
}

void MainWindow::setHeader(QString path)
{
    DetectorFile file(path);
    imageHeaderWidget->setPlainText(file.getHeaderText());
}

void MainWindow::setBatchSize(int value)
{
    batch_size = value;
}

void MainWindow::setImageRange(int low, int high)
{
    imageSpinBox->setRange(low, high);
}

//void MainWindow::setImage(ImageInfo image)
//{
//    if (folderSet.size() > 0)
//    {
//        *folderSet.current()->current() = image;

//        pathLineEdit->setText(folderSet.current()->current()->path());

//        setHeader(folderSet.current()->current()->path());

//        hasPendingChanges = true;
//    }
//}

//void MainWindow::setSelection(Selection rect)
//{
//    if (folderSet.size() > 0)
//    {
//        folderSet.current()->current()->setSelection(rect);
        
//        hasPendingChanges = true;
//    }
//}

//void MainWindow::setBackground(Selection rect)
//{
//    if (folderSet.size() > 0)
//    {
//        folderSet.current()->current()->setBackground(rect);

//        hasPendingChanges = true;
//    }
//}


void MainWindow::saveProject()
{
    QFileDialog dialog;
    dialog.setDefaultSuffix("txt");
    QString file_name = dialog.getSaveFileName(this, tr("Save project"), working_dir, tr(".qt (*.qt);; All Files (*)"));

    if (file_name != "")
    {
        QFileInfo info(file_name);
        working_dir = info.absoluteDir().path();
        
        QFile file(file_name);
        if (file.open(QIODevice::WriteOnly))
        {
            QDataStream out(&file);
            
            out << imagePreviewWindow->worker()->set();
            out << imageModeComboBox->currentText();
            out << tsfTextureComboBox->currentText();
            out << tsfAlphaComboBox->currentText();
            out << (double) dataMinDoubleSpinBox->value();
            out << (double) dataMaxDoubleSpinBox->value();
            out << (bool) logCheckBox->isChecked();
            out << (bool) correctionLorentzCheckBox->isChecked();  
            out << (double) correctionNoiseDoubleSpinBox->value();
            
            file.close();
        }
    }
    
    hasPendingChanges = false;
}

void MainWindow::loadProject()
{
    QString file_name = QFileDialog::getOpenFileName(this, tr("Open project"), working_dir, tr(".qt (*.qt);; All Files (*)"));

    if (file_name != "")
    {
        QFileInfo info(file_name);
        working_dir = info.absoluteDir().path();
        
        QFile file(file_name);
        if (file.open(QIODevice::ReadOnly))
        {
            SeriesSet set;
            
            QString mode;
            QString tsfTexture;
            QString tsfAlpha;
            
            double dataMin;
            double dataMax;
            bool log;
            bool lorentzCorrection;
            double noise;
            
            QDataStream in(&file);
            
            in >> set >> mode >> tsfTexture >> tsfAlpha >> dataMin >> dataMax >> log >> lorentzCorrection >> noise;
            
            emit setChanged(set);            
            
            imageModeComboBox->setCurrentText(mode);
            tsfTextureComboBox->setCurrentText(tsfTexture);
            tsfAlphaComboBox->setCurrentText(tsfAlpha);
            dataMinDoubleSpinBox->setValue(dataMin);
            dataMaxDoubleSpinBox->setValue(dataMax);
            logCheckBox->setChecked(log);
            correctionLorentzCheckBox->setChecked(lorentzCorrection);
            correctionNoiseDoubleSpinBox->setValue(noise);
            
            file.close();
        }
    }
}

//void MainWindow::removeImage()
//{
//    if (folderSet.size() > 0)
//    {
//        emit pathRemoved(folderSet.current()->current()->path());
        
//        folderSet.current()->removeCurrent();

//        if (folderSet.current()->size() == 0) folderSet.removeCurrent();
        
//        if (folderSet.size() > 0)
//        {
//            emit imageChanged(*folderSet.current()->next());
//        }
//    }
//}

void MainWindow::loadPaths()
{
    QMessageBox confirmationMsgBox;
    
    confirmationMsgBox.setWindowTitle("framer");
    confirmationMsgBox.setIcon(QMessageBox::Question);
    confirmationMsgBox.setText("Unsaved changes will be lost.");
    confirmationMsgBox.setInformativeText("Save first?");
    confirmationMsgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    confirmationMsgBox.setDefaultButton(QMessageBox::Save);
    
    int ret = QMessageBox::Discard;
    
    if (hasPendingChanges) ret = confirmationMsgBox.exec();
    
    switch (ret) 
    {
        case QMessageBox::Save:
            // Save was clicked
            saveProject();
            setFiles(fileSelectionModel->getPaths());
            break;
        case QMessageBox::Discard:
            // Discard was clicked
            setFiles(fileSelectionModel->getPaths());
            break;
        case QMessageBox::Cancel:
            // Cancel was clicked
            break;
        default:
            // should never be reached
            break;
    }
}

void MainWindow::setFiles(QMap<QString, QStringList> folder_map)
{
    SeriesSet set;
    
    QMap<QString, QStringList>::const_iterator i = folder_map.constBegin();
    while (i != folder_map.constEnd())
    {
        ImageSeries folder;
        folder.setPath(i.key());

        QStringList image_strings(i.value());
        QStringList::const_iterator j = image_strings.constBegin();

        while (j != image_strings.constEnd())
        {
            ImageInfo image;

            image.setPath(*j);

            folder << image;
            ++j;
        }
            
        set << folder;

        ++i;
    }
    
    if (!set.isEmpty()) 
    {
        emit setChanged(set);
    }
}


void MainWindow::nextFrame()
{
    imageSpinBox->setValue(imageSpinBox->value()+1);    
}

void MainWindow::previousFrame()
{
    imageSpinBox->setValue(imageSpinBox->value()-1);
}

void MainWindow::batchForward()
{
    imageSpinBox->setValue(imageSpinBox->value()+batch_size);
}
void MainWindow::batchBackward()
{
    imageSpinBox->setValue(imageSpinBox->value()-batch_size);
}
