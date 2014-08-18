#include "mainwindow.h"
#include <QApplication>
#include <fstream>
#include "lib/qxlib/qxlib.h"

void writeToLogAndPrint(QString text, QString file, bool append)
{
    QDateTime dateTime = dateTime.currentDateTime();
    QString dateTimeString = QString("["+dateTime.toString("hh:mm:ss")+"] ");

    std::ofstream myfile (file.toStdString().c_str(), std::ios::out | ((append == true) ? std::ios::app : std::ios::trunc));
    if (myfile.is_open())
    {
        myfile << dateTimeString.toStdString().c_str() << text.toStdString().c_str() << std::endl;
        std::cout << "[Log]"<< dateTimeString.toStdString().c_str() << text.toStdString().c_str() << std::endl;
    }
    else std::cout << "Unable to open log file" << std::endl;
}

void appOutput(QtMsgType type, const QMessageLogContext &context, const QString &msg)
{
    QByteArray localMsg = msg.toLocal8Bit();
    switch (type) {
    case QtDebugMsg:
        writeToLogAndPrint("Debug: "+
                    QString(localMsg.constData())+" ("+
                    QString(context.file)+":"+
                    QString::number(context.line)+", " +
                    QString(context.function)+")", "framer.log", 1);
        break;
    case QtWarningMsg:
        writeToLogAndPrint("Warning: "+
                           QString(localMsg.constData())+" ("+
                           QString(context.file)+":"+
                           QString::number(context.line)+", " +
                           QString(context.function)+")", "framer.log", 1);
        break;
    case QtCriticalMsg:
        writeToLogAndPrint("Critical: "+
                           QString(localMsg.constData())+" ("+
                           QString(context.file)+":"+
                           QString::number(context.line)+", " +
                           QString(context.function)+")", "framer.log", 1);
        break;
    case QtFatalMsg:
        writeToLogAndPrint("Fatal: "+
                           QString(localMsg.constData())+" ("+
                           QString(context.file)+":"+
                           QString::number(context.line)+", " +
                           QString(context.function)+")", "framer.log", 1);
        abort();
    }
}

int main(int argc, char *argv[])
{
    // Handle Qt messages
    qInstallMessageHandler(appOutput);

    
    qRegisterMetaType<Image>();
    qRegisterMetaType<ImageFolder>();
    qRegisterMetaType<FolderSet>();
    
    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    
    return a.exec();
}
