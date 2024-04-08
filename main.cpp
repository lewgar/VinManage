#include "mainwindow.h"
#include <QApplication>
#include "yolo/yolov5detection.h"
#include <QDebug>
#include<QFile>
#include <QSqlDatabase>
#include <QMessageBox>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariantList>
#include <QTextCodec>
#include "dllKey/appkey.h"
#include <QMessageBox>
bool connect(const QString &dbName)
{
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
//    db.setHostName("host");
//    db.setDatabaseName("dbname");
//    db.setUserName("username");
//    db.setPassword("password");
    db.setDatabaseName(dbName);
    if (!db.open()) {
        QMessageBox::critical(0, QObject::tr("Database Error"),
                              db.lastError().text());
        return false;
    }
    return true;
}
int main(int argc, char *argv[])
{

    #if (QT_VERSION < QT_VERSION_CHECK(5,0,0))
    #if _MSC_VER
        QTextCodec *codec = QTextCodec::codecForName("gbk");
    #else
        QTextCodec *codec = QTextCodec::codecForName("utf-8");
    #endif
        QTextCodec::setCodecForLocale(codec);
        QTextCodec::setCodecForCStrings(codec);
        QTextCodec::setCodecForTr(codec);
    #else
        QTextCodec *codec = QTextCodec::codecForName("utf-8");
        QTextCodec::setCodecForLocale(codec);
    #endif

    QApplication a(argc, argv);
    MainWindow w;
    w.show();
    //AppKey::Instance()->start();

    return a.exec();
}
