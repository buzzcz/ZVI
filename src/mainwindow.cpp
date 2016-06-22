#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QDebug>

using namespace cv;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->centralWidget->addAction(ui->actionExit);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_actionExit_triggered()
{
    QApplication::quit();
}

void MainWindow::on_action_Load_triggered()
{

    QString fileName = QFileDialog::getOpenFileName(this,
                                                    tr("Open Image"), QDir::currentPath(), tr("Image Files (*.png *.jpg *.bmp)"));

    //    QString fileName = QFileDialog::getOpenFileName(this,         //to je moje
    //        tr("Open Image"), "/usr/share/backgrounds/linuxmint-rosa", tr("Image Files (*.png *.jpg *.bmp)"));

    if (!fileName.isNull() && !fileName.isEmpty()) {
        img = imread(fileName.toStdString(), CV_LOAD_IMAGE_COLOR);

        //    if (img.rows > 512 || img.cols > 512 ) { // zavest konstantu
        //        double scale;
        //        double height = static_cast<double>(img.rows);
        //        double width = static_cast<double>(img.cols);
        //        if (img.rows > img.cols) {
        //            scale = height / 512.0;
        //        } else {
        //            scale = width / 512.0;
        //        }

        //        cv::Size size((int) round(width / scale), (int) round(height / scale));
        //        cv::Mat src = img;
        //        cv::resize(src, img, size);
        //    }
        if (!img.empty()) {
        ui->label->setPixmap(QPixmap::fromImage(QImage((unsigned char*) img.data, img.cols, img.rows, img.step, QImage::Format_RGB888).rgbSwapped()));
        showImage();

        ui->actionCanny->setEnabled(true);
        ui->actionDefinedDirection->setEnabled(true);
        ui->actionLaplace->setEnabled(true);
        ui->actionLines_and_Points->setEnabled(true);
        ui->actionGradient_Method->setEnabled(true);
        ui->actionMarr_Hildreth->setEnabled(true);
        }
    }
}

void MainWindow::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);
    showImage();
}

void MainWindow::showImage() {
    if (!img.empty()) {
        cv::Mat tmp;
        double width = ui->label->width(), height = ui->label->height();
        if (width >= height) {
            double ratio = height / img.rows;
            width = img.cols * ratio;
        } else {
            double ratio = width / img.cols;
            height = img.rows * ratio;
        }
        cv::resize(img, tmp, cv::Size(width, height));
        ui->label->setPixmap(QPixmap::fromImage(QImage((unsigned char*) tmp.data, tmp.cols, tmp.rows, tmp.step, QImage::Format_RGB888).rgbSwapped()));
    }
}

void MainWindow::on_actionDefinedDirection_triggered()
{
    DefinedDirection *w = new DefinedDirection(&img, this);
    w->show();
}

void MainWindow::on_actionLaplace_triggered()
{
    Laplace *w = new Laplace(&img, this);
    w->show();
}

void MainWindow::on_actionCanny_triggered()
{
    CannyDetection *w = new CannyDetection(&img, this);
    w->show();
}

void MainWindow::on_actionLines_and_Points_triggered()
{
    PointAndLine *w = new PointAndLine(&img, this);
    w->show();
}

void MainWindow::on_actionGradient_Method_triggered()
{
	GradientMethod *w = new GradientMethod(&img, this);
	w->show();
}

void MainWindow::on_actionMarr_Hildreth_triggered()
{
		MarrHildreth *w = new MarrHildreth(&img, this);
		w->show();
}
