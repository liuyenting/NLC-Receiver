#include "mainwindow.hpp"
#include "ui_mainwindow.h"
#include "camera.hpp"
#include <dc1394/types.h>
#include <QtDebug>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent), ui(new Ui::MainWindow), camera(new Camera) {
	ui->setupUi(this);

    fprintf(stderr, "inside main window constructor\n");

    // ENABLE GRAB FRAME FOR DEBUG
    ui->actionGrab->setEnabled(true);

    ui->actionRefresh->activate(QAction::Trigger);
}

MainWindow::~MainWindow() {
    delete camera;
	delete ui;
}

void MainWindow::on_actionStart_triggered() {
    ui->actionStart->setEnabled(false);
    ui->actionGrab->setEnabled(false);

    // TEMPORARY
    //camera->stopAcquisition();
    camera->setParameter(1, Camera::BusSpeed, DC1394_ISO_SPEED_3200);
    camera->setParameter(1, Camera::Resolution, 0, 0, 1280, 962);
    camera->setParameter(1, Camera::FrameRate, DC1394_FRAMERATE_30);

    fprintf(stderr, "finished configuring the camera\n");

    camera->startAcquisition();
    camera->startCaptureVideo(ui->imagePreview);

    ui->actionStop->setEnabled(true);
    ui->actionSave_Image->setEnabled(true);
}

void MainWindow::on_actionStop_triggered() {
    ui->actionStop->setEnabled(false);

    camera->stopCaptureVideo();
    camera->stopAcquisition();

    ui->actionStart->setEnabled(true);
    ui->actionGrab->setEnabled(true);
}

void MainWindow::on_actionGrab_triggered()
{
    ui->actionStart->setEnabled(false);

    // TEMPORARY
    //camera->stopAcquisition();
    camera->setParameter(1, Camera::BusSpeed, DC1394_ISO_SPEED_3200);
    camera->setParameter(1, Camera::Resolution, 0, 0, 1280, 962);
    camera->setParameter(1, Camera::FrameRate, DC1394_FRAMERATE_30);

    fprintf(stderr, "finished configuring the camera\n");

    camera->startAcquisition();

    //cv::Mat newFrame = cv::imread("/Users/Andy/Downloads/lena.png", CV_LOAD_IMAGE_COLOR);
    cv::Mat newFrame = camera->grabFrame();
    ui->imagePreview->showImage(newFrame);

    camera->stopAcquisition();

    ui->actionStart->setEnabled(true);
    if(!ui->actionSave_Image->isEnabled())
        ui->actionSave_Image->setEnabled(true);
}

void MainWindow::on_actionRefresh_triggered() {
    ui->menuDeviceList->clear();

    std::vector<uint64_t> deviceList = camera->listDevices();

    fprintf(stderr, "refresh triggered, %lu device detected\n", deviceList.size());

    // Dynamically populate the device GUIDs into the menu.
    if(deviceList.size() > 0) {
    QString newDeviceGuid;
    QSignalMapper *signalMapper = new QSignalMapper(this);
    for(size_t i = 0; i < deviceList.size(); i++) {
        newDeviceGuid = QString::number(deviceList[i]);

        // Generate new QAction.
        QAction *newDeviceAction = new QAction(newDeviceGuid, this);
        newDeviceAction->setCheckable(true);
        ui->menuDeviceList->addAction(newDeviceAction);

        // Map the signal.
        signalMapper->setMapping(newDeviceAction, newDeviceGuid);

        // Associate the signal to map slot in the signal mapper.
        connect(newDeviceAction, SIGNAL(toggled(bool)), signalMapper, SLOT(map()));
    }
    // Connect the mapper.
    connect(signalMapper, SIGNAL(mapped(const QString &)), this, SLOT(on_deviceSelected(const QString &)));
    } else {
        QAction *noValidDevice = new QAction("No device", this);
        noValidDevice->setEnabled(false);
        ui->menuDeviceList->addAction(noValidDevice);
    }
    addRefreshAction();
}

void MainWindow::on_actionSave_Image_triggered() {
    cv::Mat3b imgDump = ui->imagePreview->dumpImage();
    cv::imwrite("/Users/Andy/Desktop/frame.ppm", imgDump);
}

void MainWindow::on_actionExit_triggered() {
    camera->close();
    QApplication::quit();
}

void MainWindow::on_deviceSelected(const QString & guid_label) {
    qDebug() << "Trying to connect with" << qPrintable(guid_label);

    camera->open(guid_label.toULongLong());
    qDebug() << "... CONNECTED\n";

    // Enable the UI.
    ui->actionGrab->setEnabled(true);
    ui->actionStart->setEnabled(true);

    // Automatically start grabbing.
    ui->actionStart->activate(QAction::Trigger);
}

void MainWindow::addRefreshAction() {
    ui->menuDeviceList->addSeparator();

    QAction *actionRefresh = new QAction("Refresh", ui->menuDeviceList);
    actionRefresh->setShortcut(Qt::CTRL + Qt::Key_R);
    connect(actionRefresh, SIGNAL(triggered()), this, SLOT(on_actionRefresh_triggered()));

    ui->menuDeviceList->addAction(actionRefresh);
}
