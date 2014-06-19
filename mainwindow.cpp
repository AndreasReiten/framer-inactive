#include "mainwindow.h"

#include <QGridLayout>
#include <QSettings>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
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
    
    navigationDock =  new QDockWidget("Navigate");
//    QGridLayout * navigationLayout = new QGridLayout;
//    navigationLayout->addWidget(label_texture,0,0,1,2);
//    graphicsWidget->setLayout(navigationLayout);
    imageWidget->addDockWidget(Qt::RightDockWidgetArea, navigationDock);
    
    settingsDock =  new QDockWidget("Settings");
    imageWidget->addDockWidget(Qt::RightDockWidgetArea, settingsDock);
    
    calculationDock =  new QDockWidget("Calculations");
    imageWidget->addDockWidget(Qt::RightDockWidgetArea, calculationDock);
    
    imageInnerWidget = new QWidget;
    imageDisplayWidget = new QWidget;
    imageHeaderWidget = new QWidget;
    
    imageDock =  new QDockWidget("View");
    imageDock->setWidget(renderWidget);
    
    imageWidget->addDockWidget(Qt::LeftDockWidgetArea, imageDock);
    
    // Tab widget    
    tabWidget =  new QTabWidget;
    setCentralWidget(tabWidget);
    
    tabWidget->addTab(fileWidget, "Files");
    tabWidget->addTab(imageWidget, "View");
}
