#include "mainwindow.h"

#include <QGridLayout>
#include <QSettings>
#include <QDebug>
//#include <QSpacerItem>

Image::Image()
{
    ;
}
Image::~Image()
{
    ;
}

ImageFolder::ImageFolder()
{
    ;
}
ImageFolder::~ImageFolder()
{
    ;
}


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      folder_iterator_r(folders.constBegin()),
      image_iterator_r(images.constBegin())
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
    
    QGridLayout * fileWidgetLayout = new QGridLayout;
    fileWidgetLayout->addWidget(fileFilter,0,0,1,1);
    fileWidgetLayout->addWidget(fileTreeView,1,0,1,1);
    
    fileWidget =  new QWidget;
    fileWidget->setLayout(fileWidgetLayout);
    
    // Image widget
    imageWidget =  new QMainWindow;

    // Toolbar
    pathLineEdit = new QLineEdit("/path/to/file");
    pathLineEdit->setReadOnly(true);
    connect(this, SIGNAL(pathChanged(QString)), pathLineEdit, SLOT(setText(QString)));

    imageToolBar = new QToolBar("Image");

    squareAreaSelectAction = new QAction(QIcon(":/art/select.png"), tr("Toggle pixel selection"), this);
    squareAreaSelectAction->setCheckable(true);
    squareAreaSelectAction->setChecked(false);

    imageToolBar->addAction(squareAreaSelectAction);
    imageToolBar->addWidget(pathLineEdit);

    imageWidget->addToolBar(Qt::TopToolBarArea, imageToolBar);

    // Dock widget
    loadPathsPushButton = new QPushButton("Load files");
    nextFramePushButton = new QPushButton("Next file");
    previousFramePushButton = new QPushButton("Previous file");
    batchForwardPushButton = new QPushButton("Fast forward");
    batchBackwardPushButton = new QPushButton("Fast backward");
    batch_size = 10;
    nextFolderPushButton = new QPushButton("Next folder");
    previousFolderPushButton = new QPushButton("Previous folder");

//    frameIndexSpinBox = new QSpinBox;


    QGridLayout * navigationLayout = new QGridLayout;
    navigationLayout->addWidget(loadPathsPushButton,0,0,1,2);
    navigationLayout->addWidget(nextFramePushButton,1,1,1,1);
    navigationLayout->addWidget(previousFramePushButton,1,0,1,1);
    navigationLayout->addWidget(batchForwardPushButton,2,1,1,1);
    navigationLayout->addWidget(batchBackwardPushButton,2,0,1,1);
    navigationLayout->addWidget(nextFolderPushButton,3,1,1,1);
    navigationLayout->addWidget(previousFolderPushButton,3,0,1,1);

    connect(loadPathsPushButton, SIGNAL(clicked()), this, SLOT(loadPaths()));
    connect(nextFramePushButton, SIGNAL(clicked()), this, SLOT(nextFrame()));
    connect(previousFramePushButton, SIGNAL(clicked()), this, SLOT(previousFrame()));
    connect(batchForwardPushButton, SIGNAL(clicked()), this, SLOT(batchForward()));
    connect(batchBackwardPushButton, SIGNAL(clicked()), this, SLOT(batchBackward()));
    connect(nextFolderPushButton, SIGNAL(clicked()), this, SLOT(nextFolder()));
    connect(previousFolderPushButton, SIGNAL(clicked()), this, SLOT(previousFolder()));

    navigationWidget = new QWidget;
    navigationWidget->setLayout(navigationLayout);

    navigationDock =  new QDockWidget("Navigate");
    navigationDock->setAllowedAreas(Qt::RightDockWidgetArea | Qt::LeftDockWidgetArea);
    navigationDock->setWidget(navigationWidget);
    navigationDock->setFixedHeight(navigationWidget->minimumSizeHint().height()*1.2);
    imageWidget->addDockWidget(Qt::RightDockWidgetArea, navigationDock);
    
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

    dataMaxDoubleSpinBox = new QDoubleSpinBox;
    dataMaxDoubleSpinBox->setRange(-1e9,1e9);
    dataMaxDoubleSpinBox->setAccelerated(true);

    logCheckBox = new QCheckBox("Log");
    correctionCheckBox = new QCheckBox("Corrections");

    QGridLayout * settingsLayout = new QGridLayout;
    settingsLayout->addWidget(imageModeComboBox,0,1,1,2);
    settingsLayout->addWidget(tsfTextureComboBox,1,1,1,1);
    settingsLayout->addWidget(tsfAlphaComboBox,1,2,1,1);
    settingsLayout->addWidget(dataMinDoubleSpinBox,2,1,1,1);
    settingsLayout->addWidget(dataMaxDoubleSpinBox,2,2,1,1);
    settingsLayout->addWidget(logCheckBox,3,1,1,1);
    settingsLayout->addWidget(correctionCheckBox,3,2,1,1);


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

    connect(this, SIGNAL(pathChanged(QString)), this, SLOT(setHeader(QString)));

    headerDock = new QDockWidget("Header");
    headerDock->setWidget(imageHeaderWidget);
    headerDock->setAllowedAreas(Qt::RightDockWidgetArea | Qt::LeftDockWidgetArea);
    imageWidget->addDockWidget(Qt::RightDockWidgetArea, headerDock);

    // Set the OpenCL context
    context_cl = new OpenCLContext;

    // Set the format of the rendering context
    QSurfaceFormat format_gl;
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
    imagePreviewWindow->setAnimating(true);
    imagePreviewWindow->initializeWorker();

    // Apply the rendering surface to a widget
    imageDisplayWidget = QWidget::createWindowContainer(imagePreviewWindow);
    imageDisplayWidget->setFocusPolicy(Qt::StrongFocus);
    imageWidget->setCentralWidget(imageDisplayWidget);

    connect(this, SIGNAL(pathChanged(QString)), imagePreviewWindow->getWorker(), SLOT(setImageFromPath(QString)));
    connect(tsfTextureComboBox, SIGNAL(currentIndexChanged(int)), imagePreviewWindow->getWorker(), SLOT(setTsfTexture(int)), Qt::AutoConnection);
    connect(tsfAlphaComboBox, SIGNAL(currentIndexChanged(int)), imagePreviewWindow->getWorker(), SLOT(setTsfAlpha(int)));
    connect(dataMinDoubleSpinBox, SIGNAL(valueChanged(double)), imagePreviewWindow->getWorker(), SLOT(setDataMin(double)));
    connect(dataMaxDoubleSpinBox, SIGNAL(valueChanged(double)), imagePreviewWindow->getWorker(), SLOT(setDataMax(double)));
    connect(logCheckBox, SIGNAL(toggled(bool)), imagePreviewWindow->getWorker(), SLOT(setLog(bool)));
    connect(correctionCheckBox, SIGNAL(toggled(bool)), imagePreviewWindow->getWorker(), SLOT(setCorrection(bool)));
    connect(imageModeComboBox, SIGNAL(currentIndexChanged(int)), imagePreviewWindow->getWorker(), SLOT(setMode(int)));
    connect(squareAreaSelectAction, SIGNAL(toggled(bool)), imagePreviewWindow->getWorker(), SLOT(setSelectionActive(bool)));

    // Tab widget    
    tabWidget =  new QTabWidget;
    setCentralWidget(tabWidget);
    
    tabWidget->addTab(fileWidget, "Files");
    tabWidget->addTab(imageWidget, "View");

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


void MainWindow::setTab(int value)
{
    Q_UNUSED(value);
}

void MainWindow::setHeader(QString path)
{
    DetectorFile file(path);
    imageHeaderWidget->setPlainText(file.getHeaderText());
}

void MainWindow::loadPaths()
{
    QMap<QString, QStringList> tmp(fileSelectionModel->getPaths());
    QMap<QString, QStringList>::const_iterator i = tmp.constBegin();
    while (i != tmp.constEnd())
    {
        ImageFolder folder;
        folder.path = i.key();

        QStringList tmp2(i.value());

        QStringList::const_iterator j = tmp2.constBegin();
        while (j != tmp2.constEnd())
        {
            Image image;
            image.selection = QRect(0,0,0,0);
            image.path = *j;

            folder.images << image;

            ++j;
        }

        folders << folder;

        ++i;
    }


    folder_iterator_r = folders.constBegin();
    if (folder_iterator_r != folders.constEnd())
    {
        images = folder_iterator_r->images;
        image_iterator_r = images.constBegin();
        emit pathChanged(image_iterator_r->path);
    }
}

void MainWindow::nextFrame()
{

    if (image_iterator_r != images.constEnd())
    {
        image_iterator_r++;

        if (image_iterator_r != images.constEnd())
        {
            emit pathChanged(image_iterator_r->path);
        }
        else
        {
            image_iterator_r--;
        }
    }
}

void MainWindow::previousFrame()
{
    if (image_iterator_r != images.constBegin())
    {
        image_iterator_r--;

        emit pathChanged(image_iterator_r->path);
    }
}

void MainWindow::batchForward()
{
    if (images.size() > 0)
    {
        for (size_t i = 0; i < batch_size; i++)
        {
            if (image_iterator_r != images.constEnd())
            {
                image_iterator_r++;

                if (image_iterator_r != images.constEnd())
                {
                    if (i + 1 == batch_size) emit pathChanged(image_iterator_r->path);
                }
                else
                {
                    image_iterator_r--;
                    emit pathChanged(image_iterator_r->path);
                    break;
                }
            }
        }
    }
}
void MainWindow::batchBackward()
{
    if (images.size() > 0)
    {
        for (size_t i = 0; i < batch_size; i++)
        {
            if (image_iterator_r != images.constBegin())
            {
                image_iterator_r--;

                if (i + 1 == batch_size) emit pathChanged(image_iterator_r->path);
            }
            else
            {
                emit pathChanged(image_iterator_r->path);
                break;
            }
        }
    }
}
void MainWindow::nextFolder()
{
    if (folder_iterator_r != folders.constEnd())
    {
        folder_iterator_r++;

        if (folder_iterator_r != folders.constEnd())
        {
            images = folder_iterator_r->images;
            image_iterator_r = images.constBegin();

            emit pathChanged(image_iterator_r->path);
        }
        else
        {
            folder_iterator_r--;
        }
    }
}
void MainWindow::previousFolder()
{
    if (folder_iterator_r != folders.constBegin())
    {
        folder_iterator_r--;

        images = folder_iterator_r->images;
        image_iterator_r = images.constBegin();

        emit pathChanged(image_iterator_r->path);
    }
}
