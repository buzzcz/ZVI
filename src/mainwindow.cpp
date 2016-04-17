#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "defineddirection.h"
#include "laplace.h"
#include "canny.h"
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

#include <QDir>
#include <QFileDialog>

using namespace cv;

cv::Mat img, gray;
cv::Mat dst, detected_edges;

int edgeThresh = 1;
int lowThreshold;
int const max_lowThreshold = 100;
int ratio = 3;
int kernel_size = 3;
std::string window_name = "OpenCV Canny Edge Detector";

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

void CannyThreshold(int, void*)
{
	/// Reduce noise with a kernel 3x3
	blur( gray, detected_edges, Size(3,3) );

	/// Canny detector
	Canny( detected_edges, detected_edges, lowThreshold, lowThreshold*ratio, kernel_size );

	/// Using Canny's output as a mask, we display our result
	dst = Scalar::all(0);

	gray.copyTo( dst, detected_edges);
    imshow( window_name, dst );
}

void MainWindow::on_actionOpenCVCanny_triggered()
{
	if (!img.data) {
		return;
	}
	/// Create a matrix of the same type and size as src (for dst)
	dst.create( img.size(), img.type() );

	/// Convert the image to grayscale
	cvtColor( img, gray, CV_BGR2GRAY );

	namedWindow( window_name, WINDOW_AUTOSIZE );// Create a window for display.

	/// Create a Trackbar for user to enter threshold
    createTrackbar( "Min Threshold:", window_name, &lowThreshold, max_lowThreshold, CannyThreshold );

	/// Show the image
	CannyThreshold(0, 0);

	/// Wait until user exit program by pressing a key
	waitKey(0);
}

void MainWindow::on_action_Load_triggered()
{
    QString fileName = QFileDialog::getOpenFileName(this,
        tr("Open Image"), QDir::currentPath(), tr("Image Files (*.png *.jpg *.bmp)"));

//    QString fileName = QFileDialog::getOpenFileName(this,         //to je moje
//        tr("Open Image"), "/usr/share/backgrounds/linuxmint-rosa", tr("Image Files (*.png *.jpg *.bmp)"));

    img = imread(fileName.toStdString(), CV_LOAD_IMAGE_COLOR);

    if (img.rows > 512 || img.cols > 512 ) { // zavest konstantu
        double scale;
        double height = static_cast<double>(img.rows);
        double width = static_cast<double>(img.cols);
        if (img.rows > img.cols) {
            scale = height / 512.0;
        } else {
            scale = width / 512.0;
        }

        cv::Size size((int) round(width / scale), (int) round(height / scale));
        cv::Mat src = img;
        cv::resize(src, img, size);
    }


	QPixmap p = QPixmap::fromImage(QImage((unsigned char*) img.data, img.cols, img.rows, QImage::Format_RGB888).rgbSwapped());
	ui->label->setPixmap(p);
	ui->label->setFixedSize(p.size());
	ui->centralWidget->setFixedSize(p.size());
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
