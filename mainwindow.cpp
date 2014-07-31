#include "mainwindow.h"

#include <QGridLayout>
#include <QSettings>
#include <QMessageBox>
#include <QDebug>

Image::Image()
{
    p_selection = QRectF(0,0,5000,5000);
}
Image::~Image()
{
    ;
}

void Image::setPath(QString str)
{
    p_path = str;
}

QString Image::path()
{
    return p_path;
}

void Image::setSelection(QRectF rect)
{
    p_selection = rect;
}

QRectF Image::selection()
{
    return p_selection;
}


ImageFolder::ImageFolder()
{
    i = 0;
}

ImageFolder::~ImageFolder()
{
    ;
}

void ImageFolder::setPath(QString str)
{
    p_path = str;
}

QString ImageFolder::path()
{
    return p_path;
}

int ImageFolder::size()
{
    return images.size();
}

void ImageFolder::setImages(QList<Image> list)
{
    images = list;
    i = 0;
}

Image * ImageFolder::current()
{
    return &images[i];
}

Image * ImageFolder::next()
{
    if (i < images.size() - 1)
    {
        i++;
    }
    
    return &images[i];
}

Image * ImageFolder::previous()
{
    if (i > 0)
    {
        i--;
    }
    
    return &images[i];
}

Image * ImageFolder::begin()
{
    i = 0;    
    return &images[i];
}

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      folder_iterator(folders.begin())
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
    
    centerImageAction = new QAction(QIcon(":/art/center.png"), tr("Center image"), this);
    centerImageAction->setCheckable(false);

    imageToolBar->addAction(centerImageAction);
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
    
    applySelectionToNextPushButton = new QPushButton("Apply selection to next frame");
    applySelectionToFolderPushButton = new QPushButton("Apply selection to folder");
    
    QGridLayout * navigationLayout = new QGridLayout;
    navigationLayout->addWidget(loadPathsPushButton,0,0,1,2);
    navigationLayout->addWidget(nextFramePushButton,1,1,1,1);
    navigationLayout->addWidget(previousFramePushButton,1,0,1,1);
    navigationLayout->addWidget(batchForwardPushButton,2,1,1,1);
    navigationLayout->addWidget(batchBackwardPushButton,2,0,1,1);
    navigationLayout->addWidget(nextFolderPushButton,3,1,1,1);
    navigationLayout->addWidget(previousFolderPushButton,3,0,1,1);
    navigationLayout->addWidget(applySelectionToNextPushButton, 4, 0, 1 , 2);
    navigationLayout->addWidget(applySelectionToFolderPushButton, 5, 0, 1 , 2); 

    connect(loadPathsPushButton, SIGNAL(clicked()), this, SLOT(loadPaths()));
    connect(nextFramePushButton, SIGNAL(clicked()), this, SLOT(nextFrame()));
    connect(previousFramePushButton, SIGNAL(clicked()), this, SLOT(previousFrame()));
    connect(batchForwardPushButton, SIGNAL(clicked()), this, SLOT(batchForward()));
    connect(batchBackwardPushButton, SIGNAL(clicked()), this, SLOT(batchBackward()));
    connect(nextFolderPushButton, SIGNAL(clicked()), this, SLOT(nextFolder()));
    connect(previousFolderPushButton, SIGNAL(clicked()), this, SLOT(previousFolder()));
    connect(applySelectionToNextPushButton, SIGNAL(clicked()), this, SLOT(applySelectionToNext()));
    connect(applySelectionToFolderPushButton, SIGNAL(clicked()), this, SLOT(applySelectionToFolder()));
    
    
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
    connect(centerImageAction, SIGNAL(triggered()), imagePreviewWindow->getWorker(), SLOT(centerImage()));
    connect(this, SIGNAL(centerImage()), imagePreviewWindow->getWorker(), SLOT(centerImage()));
    connect(this, SIGNAL(selectionChanged(QRectF)), imagePreviewWindow->getWorker(), SLOT(setSelection(QRectF)));
    connect(imagePreviewWindow->getWorker(), SIGNAL(selectionChanged(QRectF)), this, SLOT(setSelection(QRectF)));
            
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

void MainWindow::applySelectionToFolder()
{
    if (folders.size() > 0)
    {
        QRectF selection = folder_iterator->current()->selection();
        
        folder_iterator->begin();
        
        for (int i = 0; i < folder_iterator->size(); i++)
        {
            folder_iterator->current()->setSelection(selection);
            folder_iterator->next();
        }
    }
}

void MainWindow::applySelectionToNext()
{
    if (folders.size() > 0)
    {
        QRectF selection = folder_iterator->current()->selection();
        folder_iterator->next()->setSelection(selection);
        
        emit pathChanged(folder_iterator->current()->path());
        emit selectionChanged(folder_iterator->current()->selection());
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
    folder_iterator->current()->setSelection(rect);
    
//    qDebug() << "MW Rect" << rect;
//    qDebug() << "MW Test" << folder_iterator->current()->selection();
}

void MainWindow::loadPaths()
{
    QMessageBox confirmationMsgBox;
    
    confirmationMsgBox.setText("Any changes will be lost.");
    confirmationMsgBox.setInformativeText("Do you want to save them?");
    confirmationMsgBox.setStandardButtons(QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    confirmationMsgBox.setDefaultButton(QMessageBox::Save);
    int ret = confirmationMsgBox.exec();
    
    switch (ret) {
       case QMessageBox::Save:
           // Save was clicked
           break;
       case QMessageBox::Discard:
           // Discard was clicked
           break;
       case QMessageBox::Cancel:
           // Cancel was clicked
           break;
       default:
           // should never be reached
           break;
     }
    
    folders.clear();
    
    QMap<QString, QStringList> folder_map(fileSelectionModel->getPaths());
    QMap<QString, QStringList>::const_iterator i = folder_map.constBegin();
    while (i != folder_map.constEnd())
    {
        ImageFolder folder;
        folder.setPath(i.key());

        QStringList image_strings(i.value());
        QStringList::const_iterator j = image_strings.constBegin();

        QList<Image> image_list;
        while (j != image_strings.constEnd())
        {
            Image image;
//            image.setSelection(QRect(0,0,0,0));
            image.setPath(*j);

            image_list << image;

            ++j;
        }
            
        folder.setImages(image_list);
        folders << folder;

        ++i;
    }


    folder_iterator = folders.begin();
    if (folder_iterator != folders.end())
    {
        if (folders.size() > 0)
        {
            emit pathChanged(folder_iterator->current()->path());
            emit selectionChanged(folder_iterator->current()->selection());
        }
        emit centerImage();
    }
}

void MainWindow::nextFrame()
{
    if (folders.size() > 0)
    {
        emit pathChanged(folder_iterator->next()->path());
        emit selectionChanged(folder_iterator->current()->selection());
    }
}

void MainWindow::previousFrame()
{
    if (folders.size() > 0)
    {
        emit pathChanged(folder_iterator->previous()->path());
        emit selectionChanged(folder_iterator->current()->selection());
    }
}

void MainWindow::batchForward()
{
    if (folders.size() > 0)
    {
        for (size_t i = 0; i < batch_size; i++)
        {
            folder_iterator->next();
        }
        
        emit pathChanged(folder_iterator->current()->path());
        emit selectionChanged(folder_iterator->current()->selection());
    }
}
void MainWindow::batchBackward()
{
    if (folders.size() > 0)
    {
        for (size_t i = 0; i < batch_size; i++)
        {
            folder_iterator->previous();
        }
        
        emit pathChanged(folder_iterator->current()->path());
        emit selectionChanged(folder_iterator->current()->selection());
    }
}
void MainWindow::nextFolder()
{
    if (folder_iterator != folders.end())
    {
        folder_iterator++;

        if (folder_iterator != folders.end())
        {
            if (folders.size() > 0)
            {
                emit pathChanged(folder_iterator->current()->path());
                emit selectionChanged(folder_iterator->current()->selection());
            }
        }
        else
        {
            folder_iterator--;
        }
    }
}
void MainWindow::previousFolder()
{
    if (folder_iterator != folders.begin())
    {
        folder_iterator--;

        if (folders.size() > 0)
        {
            emit pathChanged(folder_iterator->current()->path());
            emit selectionChanged(folder_iterator->current()->selection());
        }
    }
}
