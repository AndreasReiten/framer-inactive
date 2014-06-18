#include "mainwindow.h"

#include <QGridLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    initLayout();
}

MainWindow::~MainWindow()
{
    
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
    
    imageDock =  new QDockWidget("View");
    imageWidget->addDockWidget(Qt::LeftDockWidgetArea, imageDock);
    
    // Tab widget    
    tabWidget =  new QTabWidget;
    setCentralWidget(tabWidget);
    
    tabWidget->addTab(fileWidget, "Files");
    tabWidget->addTab(imageWidget, "View");
}
