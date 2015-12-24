#include "mainwindow.hpp"
#include "camera.hpp"

#include <QApplication>

int main(int argc, char *argv[]) {
    //Camera camera;
    //std::vector<uint64_t> deviceList = camera.listDevices();

    QApplication application(argc, argv);
    MainWindow window;
    window.show();

    return application.exec();
}
