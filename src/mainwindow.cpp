#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>

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
	img = imread("../lena.jpg", CV_LOAD_IMAGE_COLOR);
	QPixmap p = QPixmap::fromImage(QImage((unsigned char*) img.data, img.cols, img.rows, QImage::Format_RGB888).rgbSwapped());
	ui->label->setPixmap(p);
	ui->label->setFixedSize(p.size());
	ui->centralWidget->setFixedSize(p.size());
}
