#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include "camera.hpp"
#include <QMainWindow>
#include <QSignalMapper>
#include <QAction>

class QActionGroup;

namespace Ui
{
class MainWindow;
}

class MainWindow : public QMainWindow
{
	Q_OBJECT

public:
	explicit MainWindow(QWidget *parent = 0);
	~MainWindow();

private slots:
	void on_actionStart_triggered();

	void on_actionStop_triggered();

	void on_actionRefresh_triggered();

	void on_actionSave_Image_triggered();

	void on_actionExit_triggered();


    void on_deviceSelected(const QString & guid_lable);
    void addRefreshAction();

private:
	Ui::MainWindow *ui;
    QActionGroup *devices;

    Camera *camera;
};

#endif // MAINWINDOW_H
