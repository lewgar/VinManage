#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
bool checkIntersection(const cv::Rect& rect, const std::vector<cv::Rect>& rectArray) {
    for (const auto& r : rectArray) {
        int intersectionArea = (rect & r).area();
        int rectArea = rect.area();
        if (intersectionArea > 0 && intersectionArea > 0.5 * rectArea) {
            return true;
        }
    }
    return false;
}
MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->doubleSpinBox->setValue(0.6);
    ui->doubleSpinBox_2->setValue(0.6);


    qDebug() << QSqlDatabase::drivers();
    qDebug() << m_demo.setModelPath(exeDir + "/best.onnx")<<m_demo.setLabelsPath(exeDir+"/classes.txt") ;
    cv::Mat demo = cv::imread("D:/manage/carManage/carManage/release/demo.jpg");
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    ClassData classData;
    qDebug()<< m_demo.setImage(demo);
    m_demo.runDetHandle(demo, classData);
    qDebug()<< "xxxxxxxx"<<endl;
    db.setDatabaseName("D:/manage/carManage/carManage/release/demo.db");
    if (!db.open()) {
       // QMessageBox::warning(this, "错误", db.lastError().text());
    }
    // 执行CREATE TABLE语句来创建表
    QSqlQuery query;
    if (!query.exec("CREATE TABLE car_records ("
                    "id INTEGER PRIMARY KEY AUTOINCREMENT,"
                    "plate_number TEXT NOT NULL,"
                    "timestamp DATETIME NOT NULL,"
                    "in_out TEXT CHECK(in_out IN ('进', '出')),"
                    "duration INTEGER)")) {
       // QMessageBox::warning(nullptr, "错误", query.lastError().text());

    }
	
	QMenu *m_equipmentMenu = ui->menuBar->addMenu("设备(M)");
	QMenu *m_FileMenu = ui->menuBar->addMenu("文件(F)");
	QMenu *m_HelpMenu = ui->menuBar->addMenu("帮助(H)");
	QMenu *m_SettingMenu = ui->menuBar->addMenu("主题(S)");
	QAction *m_resAction = m_SettingMenu->addAction("MAC");

	connect(m_resAction, &QAction::triggered, this, [=]() {
		// 加载外部样式表文件
        // 加载外部QSS文件
        QFile styleFile("D:/manage/carManage/style.qss");
        if (!styleFile.open(QIODevice::ReadOnly | QIODevice::Text))
            return -1;

        QTextStream styleStream(&styleFile);
        QString styleSheet = styleStream.readAll();
        this->setStyleSheet(styleSheet);
	});

	QAction *ubantuAction = m_SettingMenu->addAction("ubantu");

	connect(ubantuAction, &QAction::triggered, this, [=]() {
		// 加载外部样式表文件
		// 加载外部QSS文件
        QFile styleFile("D:/manage/carManage/Ubuntu.qss");
		if (!styleFile.open(QIODevice::ReadOnly | QIODevice::Text))
			return -1;

		QTextStream styleStream(&styleFile);
		QString styleSheet = styleStream.readAll();
		this->setStyleSheet(styleSheet);
	});

	QAction *Action = m_SettingMenu->addAction("黑夜");

	connect(Action, &QAction::triggered, this, [=]() {
		// 加载外部样式表文件
		// 加载外部QSS文件
        QFile styleFile("D:/manage/carManage/ElegantDark.qss");
		if (!styleFile.open(QIODevice::ReadOnly | QIODevice::Text))
			return -1;

		QTextStream styleStream(&styleFile);
		QString styleSheet = styleStream.readAll();
		this->setStyleSheet(styleSheet);
	});

	QAction *Action1 = m_SettingMenu->addAction("灯光");

	connect(Action1, &QAction::triggered, this, [=]() {
		// 加载外部样式表文件
		// 加载外部QSS文件
        QFile styleFile("D:/manage/carManage/ManjaroMix.qss");
		if (!styleFile.open(QIODevice::ReadOnly | QIODevice::Text))
			return -1;

		QTextStream styleStream(&styleFile);
		QString styleSheet = styleStream.readAll();
		this->setStyleSheet(styleSheet);
	});
	

    connect(&cameraThread, &CameraThread::imageReady, this, &MainWindow::slotupdate);

    connect(this, &MainWindow::signalUpdate, this, &MainWindow::slotupdate);
    connect(this, &MainWindow::signalUpdateImage, this, &MainWindow::updateLabelPicture);

    // 在某处定义 handleImageUpdate 函数

    // 绑定 handleImageUpdate 函数并传递给 setImageCallback
    auto imageUpdateCallback = std::bind(&MainWindow::handleImageUpdate, this, std::placeholders::_1);
    cameraThread.setImageCallback(imageUpdateCallback);

    m_pSocket = new QTcpSocket(this);
    connect(m_pSocket, &QTcpSocket::connected, [this](){
        //ui->statusbar->showMessage("Connected to server.");
		printf("");
    });
    connect(m_pSocket, &QTcpSocket::disconnected, [this](){
       // ui->statusbar->showMessage("Disconnected from server.");
    });


    m_pSocket->connectToHost("bemfa.com", 8344);
    m_pSocket->write("cmd=1&uid=898a5c82063743d7a7c188ed19fd9849&topic=V831\r\n");

    connect(m_pSocket, &QTcpSocket::readyRead, this, &MainWindow::updateSql);


}

MainWindow::~MainWindow()
{
    cameraThread.requestInterruption();
    cameraThread.quit();
    cameraThread.wait();
    delete ui;
}
void  MainWindow::onReadyRead() {

}

Mat MainWindow::CarCheck()
{

    {

        m_demo.setConfLevel(ui->doubleSpinBox->value());
        m_demo.setNMSLevel(ui->doubleSpinBox_2->value());

        ClassData classData;
		try {
			// 可能会抛出异常的代码
            qDebug()<<"111111111111";
			m_demo.runDetHandle(afterMat, classData);
            qDebug()<<"111111111111";
		}
		catch (const std::exception& e) {
			return afterMat;
		}
		catch (...) {
			return afterMat;
		}

        int x, y, w, h;
        for(int i =0; i<classData.nFindCount; i++) {
            x = classData.vecClassBox[i].x;
            y = classData.vecClassBox[i].y;
            w = classData.vecClassBox[i].width;
            h = classData.vecClassBox[i].height;
            rectangle(afterMat, Point(x, y), Point(x+w, y+h), Scalar(0, 255, 0), 4);
        }
        int nCount =0;
        for(int i =0;i< m_boundRect.size();i++) {
            qDebug()<<"111111111111";
            cv::rectangle(afterMat, m_boundRect[i], cv::Scalar(0, 0, 255), 2); // Scalar(0, 0, 255) 表示红色
            if(checkIntersection(m_boundRect[i], classData.vecClassBox)) {
                ui->labelInfo->setText(ui->labelInfo->text() + "停车位" + QString::number(i) + "有车\n");
                nCount++;
            }
            else {
                ui->labelInfo->setText(ui->labelInfo->text() + "停车位" + QString::number(i) + "没有车\n");
            }
        }
        ui->label_7->setText(QString::number(m_boundRect.size() - nCount));
    }
    return afterMat;
}
QPixmap matToPixmap(const cv::Mat& mat) {
	// 将 OpenCV 的 Mat 转换为 Qt 的 QImage
	QImage image(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_RGB888);
	image = image.rgbSwapped(); // 进行颜色通道的交换，Qt 使用的是 RGB 格式，而 OpenCV 使用的是 BGR 格式

	// 将 QImage 转换为 QPixmap
	return QPixmap::fromImage(image);
}
void MainWindow::handleImageUpdate(const Mat &frame)
{
    cv::Mat image;
    if(afterMat.empty()) {
       frontMat= frame;
    }
    else {
       frontMat= afterMat;
    }
    afterMat = frame;
    // 检查图像是否成功加载
    if (frontMat.empty() || afterMat.empty()) {
        return ;
    }

    image = CarCheck();

    image = afterMat;
    updateLabelPicture(image);

//    QPixmap originalPixmap = matToPixmap(image);
//    QSize labelSize = ui->labelPicture->size();
//	qreal scaleFactor = 0.0;
//    if (originalPixmap.width() > originalPixmap.height()) {
//        scaleFactor = labelSize.width() / static_cast<qreal>(originalPixmap.width());
//    }
//    else {
//        scaleFactor = labelSize.height() / static_cast<qreal>(originalPixmap.height());
//    }
//    QPixmap scaledPixmap = originalPixmap.scaled(originalPixmap.size() * scaleFactor, Qt::KeepAspectRatio);
//    ui->labelPicture->setPixmap(scaledPixmap);
}

Mat MainWindow::MoveCheck(Mat &frontMat, Mat &afterMat)
{
    Mat frontGray ,afterGray, diffGray;
    Mat resframe = afterMat.clone();
    //灰度处理
    cvtColor(frontMat, frontGray, CV_BGR2GRAY);
    cvtColor(afterMat, afterGray, CV_BGR2GRAY);
    //imshow("GRAY", frontGray);

    //帧差处理 找到帧与帧之间运动的物体差异
    //缺点：会把其他运动物体也算进来
    absdiff(frontGray, afterGray, diffGray);
    //imshow("absdiff", diffGray);

    //二值化：黑白分明 会产生大量白色噪点
    threshold(diffGray, diffGray, 25, 255, CV_THRESH_BINARY);
    //imshow("diff", diffGray);

    //腐蚀处理：去除白色噪点 噪点不能完全去除，反而主要物体会被腐蚀的图案都变得不明显
    Mat element = cv::getStructuringElement(MORPH_RECT, Size(3, 3));
    erode(diffGray, diffGray, element);
    //imshow("erode", diffGray);

    //膨胀处理：将白色区域变“胖”
    Mat element2 = cv::getStructuringElement(MORPH_RECT, Size(20, 20));
    dilate(diffGray, diffGray, element2);
    //imshow("dilate", diffGray);

    //动态物体标记
    vector<vector<Point>> contours; //保存关键点
    findContours(diffGray, contours, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_SIMPLE, Point(0, 0));

    //提取关键点
    vector<vector<Point>> contours_poly(contours.size());
    m_boundRect.resize(contours.size());

    int x, y, w, h;
    int num = contours.size();

    for(int i =0; i< num; i++)
    {
        approxPolyDP(Mat(contours[i]), contours_poly[i], 3, true);
        m_boundRect[i] = boundingRect(Mat(contours_poly[i]));

        x = m_boundRect[i].x;
        y = m_boundRect[i].y;
        w = m_boundRect[i].width;
        h = m_boundRect[i].height;

        //绘制
        rectangle(resframe, Point(x, y), Point(x+w, y+h), Scalar(0, 255, 0), 4);

    }
    return resframe;
}

void MainWindow::updateSql()
{
    QByteArray aRecvData = m_pSocket->readAll();/*读取服务器发送的数据*/
    qDebug() << "aRecvData: " << aRecvData;
    // 假设 aRecv 是原始数据的 QByteArray


    // 将原始数据转换为 UTF-8 编码的字符串
    QString utf8Data = QString::fromUtf8(aRecvData);

    // 解析协议，提取消息后面的信息
    QStringList parts = utf8Data.split('&'); // 根据 & 分割字符串
    QString message;

    for (const QString& part : parts) {
        if (part.startsWith("msg=")) {
            // 找到以 "msg=" 开头的部分，提取消息后面的信息
            message = part.mid(4); // 提取 "msg=" 后面的部分
            break;
        }
    }

    // 删除末尾的 \r\r\n
    message.remove(QRegExp("[\r\n]+$"));

    // 输出消息
    qDebug() << "Message: " << message;
    if (message.startsWith("1/") || message.startsWith("2/")) {
        qDebug() << "Message starts with '1/' or '2/'";
    } else {
        qDebug() << "Message does not start with '1/' or '2/'";
        return;
    }

    aRecvData = message.toUtf8();
    if(aRecvData[0] == '1') {
        aRecvData.remove(0,2);
        // 插入记录
        QString plateNumber = QString::fromUtf8(aRecvData);
        QDateTime timestamp = QDateTime::currentDateTime();
        // 获取当前时间的毫秒级时间戳
        int currentMs = timestamp.toMSecsSinceEpoch();
        m_mapInTime[plateNumber.toStdString()] = currentMs;
        QString inOut = "进"; // 或者 "出"
        int duration = 0; // 填充正确的持续时间
        QSqlQuery query;
        query.prepare("INSERT INTO car_records (plate_number, timestamp, in_out, duration) "
                      "VALUES (:plate_number, :timestamp, :in_out, :duration)");
        query.bindValue(":plate_number", plateNumber);
        query.bindValue(":timestamp", timestamp);
        query.bindValue(":in_out", inOut);
        query.bindValue(":duration", duration);
        if (!query.exec()) {
            QMessageBox::warning(this, "错误", query.lastError().text());
        }
    }
    else {
        aRecvData.remove(0,2);
        // 插入记录
        QString plateNumber = QString::fromUtf8(aRecvData);
        QDateTime timestamp = QDateTime::currentDateTime();
        // 获取当前时间的毫秒级时间戳
        int currentMs = timestamp.toMSecsSinceEpoch();
        QString inOut = "出"; // 或者 "出"
        int duration = currentMs - m_mapInTime[plateNumber.toStdString()]; // 填充正确的持续时间
        QSqlQuery query;
        query.prepare("INSERT INTO car_records (plate_number, timestamp, in_out, duration) "
                      "VALUES (:plate_number, :timestamp, :in_out, :duration)");
        query.bindValue(":plate_number", plateNumber);
        query.bindValue(":timestamp", timestamp);
        query.bindValue(":in_out", inOut);
        query.bindValue(":duration", duration);
        if (!query.exec()) {
            QMessageBox::warning(this, "错误", query.lastError().text());
        }

        QString time= QString::number(currentMs/60000);
        std::string msg = "cmd=2&uid=898a5c82063743d7a7c188ed19fd9849&topic=V831&msg=" +time.toStdString()  + "min\r\n";
    //    std::cout << ui->textEdit->toPlainText().toStdString() <<std::endl;
        m_pSocket->write(msg.c_str());
    }
}

void MainWindow::on_sendButtonPicture_clicked()
{
    // 打开摄像头设备
    cv::VideoCapture cap(ui->comboBoxMac->currentIndex()); // 0表示默认摄像头设备，你也可以使用其他设备号
    // 检查摄像头设备是否成功打开
    if (!cap.isOpened()) {
        std::cerr << "Error: Unable to open camera device" << std::endl;
        return ;
    }
    // 捕获一帧图像
    cv::Mat frame;
    cap >> frame;
    // 检查图像是否成功捕获
    if (frame.empty()) {
        std::cerr << "Error: Unable to capture frame" << std::endl;
        return ;
    }
    QPixmap originalPixmap = matToPixmap(frame);
    // 获取标签的尺寸
    QSize labelSize = ui->labelPicture->size();
    // 计算缩放比例
    qreal scaleFactor = 0.0;
    if (originalPixmap.width() > originalPixmap.height()) {
        // 如果图片的宽度大于高度，则按照长边等比例缩放
        scaleFactor = labelSize.width() / static_cast<qreal>(originalPixmap.width());
    } else {
        // 如果图片的高度大于等于宽度，则按照短边等比例缩放
        scaleFactor = labelSize.height() / static_cast<qreal>(originalPixmap.height());
    }

    // 按照比例缩放图片
    QPixmap scaledPixmap = originalPixmap.scaled(originalPixmap.size() * scaleFactor, Qt::KeepAspectRatio);

    // 设置图片到标签
    ui->labelPicture->setPixmap(scaledPixmap);
}



void MainWindow::on_pushButtonAcquisition_clicked()
{
   // cameraThread.setIndex(ui->comboBoxMac->currentIndex());
	connect(&cameraThread, &CameraThread::imageReady, this, &MainWindow::slotupdate);
    cameraThread.start();
//    bIsrun = true;
 

//    // 打开摄像头设备
//    cv::VideoCapture cap(ui->comboBoxMac->currentIndex()); // 0表示默认摄像头设备，你也可以使用其他设备号
//    // 检查摄像头设备是否成功打开
//    if (!cap.isOpened()) {
//        std::cerr << "Error: Unable to open camera device" << std::endl;
//        return ;
//    }
//    {
//        cv::Mat frame;
//        cap >> frame; // 从摄像头设备中读取一帧图像
//        if (frame.empty()) {
//            return;
//        }
//        if (cv::waitKey(1) == 27) {
//            return;
//        }

//        cv::Mat image;

//        if(afterMat.empty()) {
//           frontMat= frame;
//        }
//        else {
//           frontMat= afterMat;
//        }
//        afterMat = frame;
//        // 检查图像是否成功加载
//        if (frontMat.empty() || afterMat.empty()) {
//            std::cerr << "Error: Failed to load image." << std::endl;
//            return ;
//        }
//        // 将两张图像调整为相同的大小
//        cv::Size targetSize(1519, 835); // 设置目标大小为640x480，你也可以根据需求设置其他大小
//        cv::resize(frontMat, frontMat, targetSize);
//        cv::resize(afterMat, afterMat, targetSize);
//        if(nMode == 1)
//            image = MoveCheck(frontMat, afterMat);
//        if(1) {
//            image = CarCheck();
//        }
//        image = afterMat;
//        // 将OpenCV的Mat转换为Qt的QImage
//        QImage qimg(image.data, image.cols, image.rows, static_cast<int>(image.step), QImage::Format_RGB888);
//        QPixmap originalPixmap = QPixmap::fromImage(qimg);
//        if (originalPixmap.isNull()) {
//            return ;
//        }
//        QSize labelSize = ui->labelPicture->size();
//        qreal scaleFactor = 0.0;
//        if (originalPixmap.width() > originalPixmap.height()) {
//            scaleFactor = labelSize.width() / static_cast<qreal>(originalPixmap.width());
//        } else {
//            scaleFactor = labelSize.height() / static_cast<qreal>(originalPixmap.height());
//        }
//        QPixmap scaledPixmap = originalPixmap.scaled(originalPixmap.size() * scaleFactor, Qt::KeepAspectRatio);
//        ui->labelPicture->setPixmap(scaledPixmap);
//    }
}

void MainWindow::on_pushButtonScan_clicked()
{
	ui->comboBoxMac->clear();
	cameras.clear();
    // 获取系统中所有的摄像头设备
    for (int i = 0; i < 10; ++i) { // 假设系统中最多有10个摄像头设备
        cv::VideoCapture cap(i);
        if (cap.isOpened()) {
            cameras.push_back(cap);
        }
    }
    // 打印摄像头设备信息
    if (cameras.empty()) {
        std::cout << "No cameras found." << std::endl;
    } else {
		for (int i = 0; i < cameras.size(); i++) {
			ui->comboBoxMac->addItem("camera" + QString::number(i));
		}
    }
}

void MainWindow::on_pushButtonStopAcquisition_clicked()
{
    cameraThread.requestInterruption();
    cameraThread.quit();
    cameraThread.wait();
}

void MainWindow::on_pushButton_2_clicked()
{
    nMode = 1;

    // 打开摄像头设备
    cv::VideoCapture cap(ui->comboBoxMac->currentIndex()); // 0表示默认摄像头设备，你也可以使用其他设备号

    // 检查摄像头设备是否成功打开
    if (!cap.isOpened()) {
        std::cerr << "Error: Unable to open camera device" << std::endl;
        return ;
    }
    // 捕获一帧图像
    cv::Mat frame;
    cap >> frame;
    // 检查图像是否成功捕获
    if (frame.empty()) {
        std::cerr << "Error: Unable to capture frame" << std::endl;
        return ;
    }
    lewmat2 = frame.clone();
    ;
    ui->labelPicture->setPixmap(matToPixmap(MoveCheck(lewmat, lewmat2)));
//    matNull = frame;
//    Mat gray_image;
//    cvtColor(matNull, gray_image, COLOR_BGR2GRAY);
//    // 二值化处理
//    Mat binary_image;
//    threshold(gray_image, binary_image, 100, 255, THRESH_BINARY);
//    // 反转二值图像，提取黑色区域
//    Mat inverted_image;
//    bitwise_not(binary_image, inverted_image);
//    // 轮廓检测
//    vector<vector<Point>> contours;
//    findContours(inverted_image, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
//    for (size_t i = 0; i < contours.size(); i++) {
//        Rect bounding_rect = boundingRect(contours[i]);
//        m_boundRect.push_back(bounding_rect);
//    }
//    cv::Mat image = matNull.clone();
//    // 显示结果
    ui->label_3->setText("停车位数量:" + QString::number(m_boundRect.size()));
    ui->lineEdit->setText(QString::number(m_boundRect.size()));
//    for (size_t i = 0; i < m_boundRect.size(); i++) {
//        rectangle(image, m_boundRect[i], Scalar(0, 255, 0), 2);
//    }
//    slotupdate(image);
}

void MainWindow::on_pushButton_clicked()
{
    nMode = 0;
}

void MainWindow::slotupdate(const cv::Mat &frame)
{

    QPixmap originalPixmap = matToPixmap(frame);
    // 获取标签的尺寸
    QSize labelSize = ui->labelPicture->size();
    // 计算缩放比例
    qreal scaleFactor = 0.0;
    if (originalPixmap.width() > originalPixmap.height()) {
        // 如果图片的宽度大于高度，则按照长边等比例缩放
        scaleFactor = labelSize.width() / static_cast<qreal>(originalPixmap.width());
    } else {
        // 如果图片的高度大于等于宽度，则按照短边等比例缩放
        scaleFactor = labelSize.height() / static_cast<qreal>(originalPixmap.height());
    }

    // 按照比例缩放图片
    QPixmap scaledPixmap = originalPixmap.scaled(originalPixmap.size() * scaleFactor, Qt::KeepAspectRatio);

    // 设置图片到标签
    ui->labelPicture->setPixmap(scaledPixmap);
}

void MainWindow::threadFunc()
{
    cv::VideoCapture cap(0); // 打开默认摄像头设备
    if (!cap.isOpened()) {
        std::cerr << "Error: Unable to open camera device" << std::endl;
        return;
    }
    while (bIsrun) {
        cv::Mat frame;
        cap >> frame; // 从摄像头设备中读取一帧图像
        if (frame.empty()) {
            return;
        }
        if (cv::waitKey(1) == 27) {
            return;
        }
        cv::Mat image;

        if(afterMat.empty()) {
           frontMat= frame;
        }
        else {
           frontMat= afterMat;
        }
        afterMat = frame;
        // 检查图像是否成功加载
        if (frontMat.empty() || afterMat.empty()) {
            std::cerr << "Error: Failed to load image." << std::endl;
            return ;
        }
        // 将两张图像调整为相同的大小
        cv::Size targetSize(1519, 835); // 设置目标大小为640x480，你也可以根据需求设置其他大小
        cv::resize(frontMat, frontMat, targetSize);
        cv::resize(afterMat, afterMat, targetSize);
//        if(nMode == 1)
//            image = MoveCheck(matNull, afterMat);
        if(1) {
            image = CarCheck();
        }
        image = afterMat;
        // 将OpenCV的Mat转换为Qt的QImage
        QImage qimg(image.data, image.cols, image.rows, static_cast<int>(image.step), QImage::Format_RGB888);
        QPixmap originalPixmap = QPixmap::fromImage(qimg);
        if (originalPixmap.isNull()) {
            return ;
        }
 
    }

    cap.release();
}

void MainWindow::updateLabelPicture( Mat &image)
{
    QPixmap originalPixmap = matToPixmap(image);
    QSize labelSize = ui->labelPicture->size();
    qreal scaleFactor = 0.0;
    if (originalPixmap.width() > originalPixmap.height()) {
        scaleFactor = labelSize.width() / static_cast<qreal>(originalPixmap.width());
    }
    else {
        scaleFactor = labelSize.height() / static_cast<qreal>(originalPixmap.height());
    }
    QPixmap scaledPixmap = originalPixmap.scaled(originalPixmap.size() * scaleFactor, Qt::KeepAspectRatio);
    ui->labelPicture->setPixmap(scaledPixmap);
}



void MainWindow::on_pushButton_3_clicked()
{
    // 初始化数据库连接
    QSqlDatabase db = QSqlDatabase::addDatabase("QSQLITE");
    db.setDatabaseName(qsexeDir + "/demo.db");
    if (!db.open()) {
        QMessageBox::critical(nullptr, "错误", "无法打开数据库连接");
        return ;
    }

    // 创建 QSqlTableModel
    QSqlTableModel model;
    model.setTable("car_records");
    model.select();

    // 创建 QTableView
    QTableView tableView;
    tableView.setModel(&model);
    tableView.resizeColumnsToContents();
    tableView.setWindowTitle("数据库信息");

    // 创建对话框，并将 QTableView 添加到对话框中

    QVBoxLayout layout(dialog);
    layout.addWidget(&tableView);
    dialog->setLayout(&layout);
    dialog->resize(600, 400);
    // 显示对话框
    dialog->exec();
}

void MainWindow::on_connectButton_clicked()
{
    QString sIp = ui->ipLineEdit->text();
    int nPort = ui->portLineEdit->text().toInt();
    m_pSocket->connectToHost(sIp, nPort);
}

void MainWindow::on_disConnectButton_clicked()
{
    m_pSocket->disconnectFromHost();
}

void MainWindow::on_pushButton_4_clicked()
{
    // 打开摄像头设备
    cv::VideoCapture cap(ui->comboBoxMac->currentIndex()); // 0表示默认摄像头设备，你也可以使用其他设备号
    // 检查摄像头设备是否成功打开
    if (!cap.isOpened()) {
        std::cerr << "Error: Unable to open camera device" << std::endl;
        return ;
    }
    // 捕获一帧图像
    cv::Mat frame;
    cap >> frame;
    // 检查图像是否成功捕获
    if (frame.empty()) {
        std::cerr << "Error: Unable to capture frame" << std::endl;
        return ;
    }
    QPixmap originalPixmap = matToPixmap(frame);
    lewmat = frame.clone();
    // 获取标签的尺寸
    QSize labelSize = ui->labelPicture->size();
    // 计算缩放比例
    qreal scaleFactor = 0.0;
    if (originalPixmap.width() > originalPixmap.height()) {
        // 如果图片的宽度大于高度，则按照长边等比例缩放
        scaleFactor = labelSize.width() / static_cast<qreal>(originalPixmap.width());
    } else {
        // 如果图片的高度大于等于宽度，则按照短边等比例缩放
        scaleFactor = labelSize.height() / static_cast<qreal>(originalPixmap.height());
    }

    // 按照比例缩放图片
    QPixmap scaledPixmap = originalPixmap.scaled(originalPixmap.size() * scaleFactor, Qt::KeepAspectRatio);

    // 设置图片到标签
    ui->labelPicture->setPixmap(scaledPixmap);
}

int CameraThread::getIndex() const
{
    return index;
}

void CameraThread::setIndex(int newIndex)
{
    index = newIndex;
}
