#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "defineddirection.h"
#include "laplace.h"
#include "canny.h"
#include "pointandline.h"
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <QMainWindow>
#include <QDir>
#include <QFileDialog>
#include <QTimer>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

private slots:
    void on_actionExit_triggered();

    void on_actionOpenCVCanny_triggered();

    void on_action_Load_triggered();

    void on_actionDefinedDirection_triggered();

    void on_actionLaplace_triggered();

    void on_actionCanny_triggered();

    void on_actionLines_and_Points_triggered();

protected:
    void resizeEvent(QResizeEvent *event);

private:
    Ui::MainWindow *ui;
    void showImage();
};

#endif // MAINWINDOW_H
