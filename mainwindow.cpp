﻿#include "mainwindow.h"

#include <QGridLayout>
#include <QSettings>
#include <QMessageBox>
#include <QFileDialog>
#include <QDebug>
#include <QCoreApplication>



MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      hasPendingChanges(false),
      integration_mode(0)
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
    QSettings settings("Norwegian University of Science and Technology", "framer");
    QPoint pos = settings.value("pos", QPoint(0, 0)).toPoint();
    QSize size = settings.value("size", QSize(400, 400)).toSize();
    resize(size);
    move(pos);
}

void MainWindow::writeSettings()
{
    QSettings settings("Norwegian University of Science and Technology", "framer");
    settings.setValue("pos", pos());
    settings.setValue("size", size());
}

void MainWindow::integrateSelectedMode()
{


    switch (integration_mode)
    {
        case 0: // Single
            emit integrateImage(*folderSet.current()->current()); 
            break;

        case 1: // Folder
            emit integrateFolder(*folderSet.current());
            break;

        case 2: // All
            emit integrateSet(folderSet);
            break;

        default: // Should not occur
            break;
    }
}

void MainWindow::setIntegrationMode(int value)
{
    integration_mode = value;
}

void MainWindow::setIntegrationResults(double sum, int err)
{
    QRectF selection = folderSet.current()->current()->selection();
    
    QString str;
    QString region_str = QString::number(selection.normalized().left())+" "+QString::number(selection.normalized().top())+" "+QString::number(selection.normalized().width())+" "+QString::number(selection.normalized().height());
    
    if (err == 0)
    {
        str += QString::number(sum,'E')+" "+region_str;
    }
    else 
    {
        str += "# Bad integration_sum"+region_str;;
    }
    
    emit outputTextAppended(str);
}

void MainWindow::integrateSingle()
{
    if (folderSet.size() <= 0) return;
    {
        QString str;
                
        str += "# SINGLE FRAME INTEGRATION\n";
        str += "# "+QDateTime::currentDateTime().toString("yyyy.MM.dd HH:mm:ss t")+"\n";
        str += "#\n";
        str += "#\n# Integrated intensity, origin x y, size w h\n";
        emit outputTextChanged(str);
        
        emit integrateCurrentFrame(folderSet.current()->current()->path(), folderSet.current()->current()->selection());
    }
}



void MainWindow::integrateFolder()
{
    QString str;
            
    str += "# FOLDER INTEGRATION\n";
    str += "# "+QDateTime::currentDateTime().toString("yyyy.MM.dd HH:mm:ss t")+"\n";
    str += "#\n";
    str += "#\n# Integrated intensity, origin x y, size w h\n";
    
    emit outputTextChanged(str);

    folderSet.current()->rememberCurrent();
    
    folderSet.current()->begin();
    
    for (int i = 0; i < folderSet.current()->size(); i++)
    {
        emit integrateCurrentFrame(folderSet.current()->current()->path(), folderSet.current()->current()->selection());
        
        folderSet.current()->next();
    }
    
    folderSet.current()->restoreMemory();
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
    
    squareAreaSelectAction = new QAction(QIcon(":/art/select.png"), tr("Toggle pixel selection"), this);
    squareAreaSelectAction->setCheckable(true);
    squareAreaSelectAction->setChecked(false);
    
    centerImageAction = new QAction(QIcon(":/art/center.png"), tr("Center image"), this);
    centerImageAction->setCheckable(false);

    imageToolBar->addAction(saveProjectAction);
    imageToolBar->addAction(loadProjectAction);
    imageToolBar->addAction(centerImageAction);
    imageToolBar->addAction(squareAreaSelectAction);
    imageToolBar->addWidget(pathLineEdit);

    imageWidget->addToolBar(Qt::TopToolBarArea, imageToolBar);

    // Dock widget
    nextFramePushButton = new QPushButton;
    nextFramePushButton->setIcon(QIcon(":/art/forward.png"));
    previousFramePushButton = new QPushButton;
    previousFramePushButton->setIcon(QIcon(":/art/back.png"));
    batchForwardPushButton = new QPushButton;
    batchForwardPushButton->setIcon(QIcon(":/art/fast_forward.png"));
    batchBackwardPushButton = new QPushButton;
    batchBackwardPushButton->setIcon(QIcon(":/art/fast_back.png"));
    batch_size = 10;
    nextFolderPushButton = new QPushButton(QIcon(":/art/forward.png"),"Next folder");
    previousFolderPushButton = new QPushButton(QIcon(":/art/back.png"),"Previous folder");
    
    removeCurrentPushButton = new QPushButton(QIcon(":/art/kill.png"),"Remove frame");
    applySelectionToNextPushButton = new QPushButton("Apply selection to next frame");
    applySelectionToFolderPushButton = new QPushButton("Apply selection to folder");
    
    QGridLayout * navigationLayout = new QGridLayout;
    navigationLayout->addWidget(previousFolderPushButton,0,0,1,1);
    navigationLayout->addWidget(batchBackwardPushButton,0,1,1,1);
    navigationLayout->addWidget(previousFramePushButton,0,2,1,1);
    navigationLayout->addWidget(nextFramePushButton,0,3,1,1);
    navigationLayout->addWidget(batchForwardPushButton,0,4,1,1);
    navigationLayout->addWidget(nextFolderPushButton,0,5,1,1);
    
    navigationLayout->addWidget(removeCurrentPushButton, 4, 0, 1, 6); 
    navigationLayout->addWidget(applySelectionToNextPushButton, 5, 0, 1 , 6);
    navigationLayout->addWidget(applySelectionToFolderPushButton, 6, 0, 1 , 6); 
    
    connect(nextFramePushButton, SIGNAL(clicked()), this, SLOT(nextFrame()));
    connect(previousFramePushButton, SIGNAL(clicked()), this, SLOT(previousFrame()));
    connect(batchForwardPushButton, SIGNAL(clicked()), this, SLOT(batchForward()));
    connect(batchBackwardPushButton, SIGNAL(clicked()), this, SLOT(batchBackward()));
    connect(nextFolderPushButton, SIGNAL(clicked()), this, SLOT(nextFolder()));
    connect(previousFolderPushButton, SIGNAL(clicked()), this, SLOT(previousFolder()));
    connect(applySelectionToNextPushButton, SIGNAL(clicked()), this, SLOT(applySelectionToNext()));
    connect(applySelectionToFolderPushButton, SIGNAL(clicked()), this, SLOT(applySelectionToFolder()));
    connect(removeCurrentPushButton, SIGNAL(clicked()), this, SLOT(removeImage()));
    connect(this, SIGNAL(pathRemoved(QString)), fileSelectionModel, SLOT(removeFile(QString)));
    
    navigationWidget = new QWidget;
    navigationWidget->setLayout(navigationLayout);

    navigationDock =  new QDockWidget("Navigate");
    navigationDock->setAllowedAreas(Qt::RightDockWidgetArea | Qt::LeftDockWidgetArea);
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
    correctionCheckBox = new QCheckBox("Corrections");

    QGridLayout * settingsLayout = new QGridLayout;
    settingsLayout->addWidget(imageModeComboBox,0,1,1,2);
    settingsLayout->addWidget(tsfTextureComboBox,1,1,1,1);
    settingsLayout->addWidget(tsfAlphaComboBox,1,2,1,1);
    settingsLayout->addWidget(dataMinDoubleSpinBox,2,1,1,2);
    settingsLayout->addWidget(dataMaxDoubleSpinBox,3,1,1,2);
    settingsLayout->addWidget(logCheckBox,4,1,1,1);
    settingsLayout->addWidget(correctionCheckBox,4,2,1,1);


    settingsWidget = new QWidget;
    settingsWidget->setLayout(settingsLayout);


    settingsDock =  new QDockWidget("Settings");
    settingsDock->setAllowedAreas(Qt::RightDockWidgetArea | Qt::LeftDockWidgetArea);
    settingsDock->setWidget(settingsWidget);
    settingsDock->setFixedHeight(settingsWidget->minimumSizeHint().height()*1.2);
    imageWidget->addDockWidget(Qt::RightDockWidgetArea, settingsDock);
    
    // Dock widget
    integrationModeComboBox = new QComboBox;
    integrationModeComboBox->addItem("Single");
    integrationModeComboBox->addItem("Folder");
    integrationModeComboBox->addItem("All");

    integratePushButton = new QPushButton("Integrate");

    QGridLayout * calculationLayout = new QGridLayout;
    calculationLayout->addWidget(integrationModeComboBox,0,0,1,1);
    calculationLayout->addWidget(integratePushButton,1,0,1,1);

    calculationWidget = new QWidget;
    calculationWidget->setLayout(calculationLayout);

    calculationDock =  new QDockWidget("Calculations");
    calculationDock->setAllowedAreas(Qt::RightDockWidgetArea | Qt::LeftDockWidgetArea);
    calculationDock->setFixedHeight(calculationWidget->minimumSizeHint().height()*1.2);
    calculationDock->setWidget(calculationWidget);
    imageWidget->addDockWidget(Qt::RightDockWidgetArea, calculationDock);
    
    // Dock widget
    imageHeaderWidget = new QPlainTextEdit;
    imageHeaderWidget->setReadOnly(true);


    connect(fileTreeView, SIGNAL(fileChanged(QString)), this, SLOT(setHeader(QString)));

    headerDock = new QDockWidget("Header");
    headerDock->setWidget(imageHeaderWidget);
    headerDock->setAllowedAreas(Qt::RightDockWidgetArea | Qt::LeftDockWidgetArea);
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

    connect(this, SIGNAL(pathChanged(QString)), imagePreviewWindow->getWorker(), SLOT(setImageFromPath(QString)));
    connect(tsfTextureComboBox, SIGNAL(currentIndexChanged(int)), imagePreviewWindow->getWorker(), SLOT(setTsfTexture(int)));
    connect(tsfAlphaComboBox, SIGNAL(currentIndexChanged(int)), imagePreviewWindow->getWorker(), SLOT(setTsfAlpha(int)));
    connect(dataMinDoubleSpinBox, SIGNAL(valueChanged(double)), imagePreviewWindow->getWorker(), SLOT(setDataMin(double)));
    connect(dataMaxDoubleSpinBox, SIGNAL(valueChanged(double)), imagePreviewWindow->getWorker(), SLOT(setDataMax(double)));
    connect(logCheckBox, SIGNAL(toggled(bool)), imagePreviewWindow->getWorker(), SLOT(setLog(bool)));
    connect(correctionCheckBox, SIGNAL(toggled(bool)), imagePreviewWindow->getWorker(), SLOT(setCorrection(bool)));
    connect(imageModeComboBox, SIGNAL(currentIndexChanged(int)), imagePreviewWindow->getWorker(), SLOT(setMode(int)));
    connect(saveProjectAction, SIGNAL(triggered()), this, SLOT(saveProject()));
    connect(loadProjectAction, SIGNAL(triggered()), this, SLOT(loadProject()));
    connect(squareAreaSelectAction, SIGNAL(toggled(bool)), imagePreviewWindow->getWorker(), SLOT(setSelectionActive(bool)));
    connect(centerImageAction, SIGNAL(triggered()), imagePreviewWindow->getWorker(), SLOT(centerImage()));
    connect(this, SIGNAL(centerImage()), imagePreviewWindow->getWorker(), SLOT(centerImage()));
    connect(this, SIGNAL(selectionChanged(QRectF)), imagePreviewWindow->getWorker(), SLOT(setSelection(QRectF)));
    connect(imagePreviewWindow->getWorker(), SIGNAL(selectionChanged(QRectF)), this, SLOT(setSelection(QRectF)));
    connect(integratePushButton,SIGNAL(clicked()),this,SLOT(integrateSelectedMode()));
    connect(integrationModeComboBox,SIGNAL(currentIndexChanged(int)),this,SLOT(setIntegrationMode(int)));
    connect(imagePreviewWindow->getWorker(), SIGNAL(pathChanged(QString)), this, SLOT(setHeader(QString)));
    connect(imagePreviewWindow->getWorker(), SIGNAL(pathChanged(QString)), pathLineEdit, SLOT(setText(QString)));
    connect(this, SIGNAL(integrateImage(Image)), imagePreviewWindow->getWorker(), SLOT(integrateSingle(Image)));
    connect(this, SIGNAL(integrateFolder(ImageFolder)), imagePreviewWindow->getWorker(), SLOT(integrateFolder(ImageFolder)));
    connect(this, SIGNAL(integrateSet(FolderSet)), imagePreviewWindow->getWorker(), SLOT(integrateSet(FolderSet)));
    
    // Text output widget
    outputPlainTextEdit = new QPlainTextEdit("Output is written in plain text here");
    outputPlainTextEdit->setReadOnly(true);
    
    connect(this, SIGNAL(outputTextChanged(QString)),outputPlainTextEdit,SLOT(setPlainText(QString)));
    connect(this, SIGNAL(outputTextAppended(QString)),outputPlainTextEdit,SLOT(appendPlainText(QString)));
    connect(imagePreviewWindow->getWorker(), SIGNAL(resultFinished(QString)), outputPlainTextEdit, SLOT(setPlainText(QString)));

    // Tab widget    
    tabWidget =  new QTabWidget;
    setCentralWidget(tabWidget);
    
    tabWidget->addTab(fileWidget, "Files");
    tabWidget->addTab(imageWidget, "View");
    tabWidget->addTab(outputPlainTextEdit, "Text output");
    
    connect(tabWidget, SIGNAL(currentChanged(int)), this, SLOT(setTab(int)));
}

void MainWindow::setStartConditions()
{
    tsfTextureComboBox->setCurrentIndex(1);
    tsfAlphaComboBox->setCurrentIndex(2);
    dataMinDoubleSpinBox->setValue(0);
    dataMaxDoubleSpinBox->setValue(1000);
    logCheckBox->setChecked(true);
    correctionCheckBox->setChecked(true);
    imageModeComboBox->setCurrentIndex(0);

}

void MainWindow::applySelectionToFolder()
{
    if (folderSet.size() > 0)
    {
        QRectF selection = folderSet.current()->current()->selection();
        
        folderSet.current()->rememberCurrent();
        
        folderSet.current()->begin();
        
        for (int i = 0; i < folderSet.current()->size(); i++)
        {
            folderSet.current()->current()->setSelection(selection);
            folderSet.current()->next();
        }
        
        folderSet.current()->restoreMemory();
    }
}

void MainWindow::applySelectionToNext()
{
    if (folderSet.size() > 0)
    {
        QRectF selection = folderSet.current()->current()->selection();
        folderSet.current()->next()->setSelection(selection);
        
        emit pathChanged(folderSet.current()->current()->path());
        emit selectionChanged(folderSet.current()->current()->selection());
    }
}

void MainWindow::setTab(int value)
{
    Q_UNUSED(value);
}

void MainWindow::setHeader(QString path)
{
    DetectorFile file(path);
    imageHeaderWidget->setPlainText(file.getHeaderText());
}

void MainWindow::setSelection(QRectF rect)
{
    if (folderSet.size() > 0)
    {
        folderSet.current()->current()->setSelection(rect);
        
        hasPendingChanges = true;
    }
}


void MainWindow::saveProject()
{
    QFileDialog dialog;
    dialog.setDefaultSuffix("txt");
    QString path = dialog.getSaveFileName(this, tr("Save project"), "", tr(".qt (*.qt);; All Files (*)"));

    if (path != "")
    {
        QFile file(path);
        if (file.open(QIODevice::WriteOnly))
        {
            QDataStream out(&file);
            
            out << folderSet;
            out << tsfTextureComboBox->currentText();
            out << tsfAlphaComboBox->currentText();
            out << (double) dataMinDoubleSpinBox->value();
            out << (double) dataMaxDoubleSpinBox->value();
            out << (bool) logCheckBox->isChecked();
            out << (bool) correctionCheckBox->isChecked();            
            
            file.close();
        }
    }
    
    hasPendingChanges = false;
}

void MainWindow::loadProject()
{
    QString path = QFileDialog::getOpenFileName(this, tr("Open project"), "", tr(".qt (*.qt);; All Files (*)"));

    if (path != "")
    {
        QFile file(path);
        if (file.open(QIODevice::ReadOnly))
        {
            QString tsfTexture;
            QString tsfAlpha;
            double dataMin;
            double dataMax;
            bool log;
            bool correction;
            
            QDataStream in(&file);
            
            in >> folderSet >> tsfTexture >> tsfAlpha >> dataMin >> dataMax >> log >> correction;
            
            
            tsfTextureComboBox->setCurrentText(tsfTexture);
            tsfAlphaComboBox->setCurrentText(tsfAlpha);
            dataMinDoubleSpinBox->setValue(dataMin);
            dataMaxDoubleSpinBox->setValue(dataMax);
            logCheckBox->setChecked(log);
            correctionCheckBox->setChecked(correction);
            
            file.close();
        }
    }
}

void MainWindow::removeImage()
{
    if (folderSet.size() > 0)
    {
        emit pathRemoved(folderSet.current()->current()->path());
        
        folderSet.current()->removeCurrent();

        if (folderSet.current()->size() == 0) folderSet.removeCurrent();
        
        if (folderSet.size() > 0)
        {
            emit pathChanged(folderSet.current()->next()->path());
            emit selectionChanged(folderSet.current()->current()->selection());
        }
    }
}

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
    folderSet.clear();
    
    QMap<QString, QStringList>::const_iterator i = folder_map.constBegin();
    while (i != folder_map.constEnd())
    {
        ImageFolder folder;
        folder.setPath(i.key());

        QStringList image_strings(i.value());
        QStringList::const_iterator j = image_strings.constBegin();

        while (j != image_strings.constEnd())
        {
            Image image;

            image.setPath(*j);

            folder << image;
            ++j;
        }
            
        folderSet << folder;

        ++i;
    }
    
    if (folderSet.size() > 0) 
    {
        emit pathChanged(folderSet.current()->current()->path());
        emit selectionChanged(folderSet.current()->current()->selection());
        emit centerImage();
    }
    
    hasPendingChanges = true;
}


void MainWindow::nextFrame()
{
    if (folderSet.size() > 0)
    {
        emit pathChanged(folderSet.current()->next()->path());
        emit selectionChanged(folderSet.current()->current()->selection());
    }
}

void MainWindow::previousFrame()
{
    if (folderSet.size() > 0)
    {
        emit pathChanged(folderSet.current()->previous()->path());
        emit selectionChanged(folderSet.current()->current()->selection());
    }
}

void MainWindow::batchForward()
{
    if (folderSet.size() > 0)
    {
        for (size_t i = 0; i < batch_size; i++)
        {
            folderSet.current()->next();
        }
        
        emit pathChanged(folderSet.current()->current()->path());
        emit selectionChanged(folderSet.current()->current()->selection());
    }
}
void MainWindow::batchBackward()
{
    if (folderSet.size() > 0)
    {
        for (size_t i = 0; i < batch_size; i++)
        {
            folderSet.current()->previous();
        }
        
        emit pathChanged(folderSet.current()->current()->path());
        emit selectionChanged(folderSet.current()->current()->selection());
    }
}
void MainWindow::nextFolder()
{
    if (folderSet.size() > 0)
    {
        emit pathChanged(folderSet.next()->current()->path());
        emit selectionChanged(folderSet.current()->current()->selection());
    }
}
void MainWindow::previousFolder()
{
    if (folderSet.size() > 0)
    {
        emit pathChanged(folderSet.previous()->current()->path());
        emit selectionChanged(folderSet.current()->current()->selection());
    }
}
