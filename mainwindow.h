#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QLabel>
#include <QPixmap>
#include <QFileInfo>
#include <opencv2/opencv.hpp>
#include <iostream>
#include <string>
#include <io.h> // for _mkdir
#include <QDir>
#include <QMenu>
#include <QTextStream>
#include "yolo/yolov5detection.h"
#include <QDebug>
#include <QFile>
#include <QThread>
#include <thread>
#include <QTcpSocket>
#include <QSqlDatabase>
#include <functional>
#include <QSqlDatabase>
#include <QMessageBox>
#include <QSqlError>
#include <QSqlQuery>
#include <QString>
#include <QVariantList>
#include <unordered_map>
#include <QDateTime>
#include <QtSql>
#include <QTableView>
#pragma execution_character_set("utf-8")
using namespace cv;
using namespace std;

namespace Ui {
class MainWindow;
}



class CameraThread : public QThread {
    Q_OBJECT
public:
    int index =1;
    using ImageCallback = std::function<void(const cv::Mat&)>;

    explicit CameraThread(QObject *parent = nullptr) : QThread(parent), callback(nullptr) {

    }

    void setImageCallback(const ImageCallback& cb) {
        callback = cb;
    }

    int getIndex() const;
    void setIndex(int newIndex);

signals:
    void imageReady(const cv::Mat &image);

protected:
    void run() override {
        cv::VideoCapture cap(0); // Open default camera
        if (!cap.isOpened()) {
            qDebug() << "Error: Couldn't open the camera.";
            return;
        }

        while (!isInterruptionRequested()) {
            cv::Mat frame;
            cap >> frame;
            if (frame.empty())
                continue;

            if (callback)
                callback(frame);

			QThread::msleep(100);

        }
    }

private:
    ImageCallback callback;
};




class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    YOLOv5Detection m_demo;
    QString qsexeDir = QCoreApplication::applicationDirPath();
    string exeDir = QCoreApplication::applicationDirPath().toStdString();
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();
    Mat CarCheck() ;
    void func(const cv::Mat &image) {
        emit signalUpdate(image);
    }
	void handleImageUpdate(const cv::Mat& frame);

    Mat MoveCheck(Mat &frontMat, Mat &afterMat);

    vector<Rect> m_boundRect;

    cv::Mat matNull;

    void updateSql();

public slots:
    void slotupdate(const cv::Mat &image);
signals:
    void signalUpdate(const cv::Mat &image);
    void signalUpdateImage( cv::Mat &image);
private slots:
    void on_sendButtonPicture_clicked();

    void on_pushButtonAcquisition_clicked();

    void on_pushButtonScan_clicked();

    void on_pushButtonStopAcquisition_clicked();

    void on_pushButton_2_clicked();

    void on_pushButton_clicked();

    void onReadyRead() ;

    void on_pushButton_3_clicked();

    void on_connectButton_clicked();

    void on_disConnectButton_clicked();

    void on_pushButton_4_clicked();

private:
    Ui::MainWindow *ui;
    CameraThread cameraThread;
    cv::Mat lewmat,lewmat2;

    std::vector<cv::VideoCapture> cameras;

    Mat frontMat;
    Mat afterMat;
    bool bIsrun = false;
    int nMode = 1;

    void threadFunc();

    char m_aBuffer[30];
    int m_nMaxWidth;
    int m_nIsColor=0;
    int m_nMaxHeight;
    QTcpSocket *m_pSocket;
    QByteArray m_aRecvBuffer;
    bool m_bIsReceiving;
    int m_nFrameCount = 0;

    int  m_nTriggerModel;
    int m_nCameraStatus;
    unordered_map<std::string, int> m_mapInTime;
	QDialog *dialog = new QDialog();
private slots:
    void updateLabelPicture( cv::Mat& image);
};

#endif // MAINWINDOW_H
