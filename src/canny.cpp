#include "canny.h"
#include "ui_canny.h"

CannyDetection::CannyDetection(cv::Mat *src, QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::CannyDetection)
{
	ui->setupUi(this);
	connect(ui->MinSpinBox, SIGNAL(valueChanged(double)), this, SLOT(detectEdges()));
	connect(ui->MaxSpinBox, SIGNAL(valueChanged(double)), this, SLOT(detectEdges()));

	cv::cvtColor((*src), img, CV_BGR2GRAY);
	detectEdges();
}

CannyDetection::~CannyDetection()
{
	delete ui;
}

void CannyDetection::detectEdges() {
	cv::Mat blured;
	cv::GaussianBlur(img, blured, cv::Size(5, 5), 1.4);

	cv::Mat Gx, Gy;
	cv::Sobel(blured, Gx, -1, 1, 0);
	cv::Sobel(blured, Gy, -1, 0, 1);

	cv::Mat G, direction;
	G.create(Gx.size(), Gx.type());
	direction.create(Gx.size(), Gx.type());
	getGradientsAndDirections(&Gx, &Gy, &G, &direction);

	//TODO creates fragmented lines
	nonMaximumSuppression(&G, &direction);

	doubleThresholding(&G);

	QPixmap p = QPixmap::fromImage(QImage((unsigned char*) G.data, G.cols, G.rows, G.step, QImage::Format_Indexed8));
	ui->label->setPixmap(p);
	ui->label->setFixedSize(p.size());
	ui->centralwidget->setFixedSize(p.size());
}

void CannyDetection::getGradientsAndDirections(cv::Mat *Gx, cv::Mat *Gy, cv::Mat *G, cv::Mat *direction) {
	for (int i = 0; i < G->rows; i++) {
		for (int j = 0; j < G->cols; j++) {
			double Gxi = Gx->at<uchar>(i, j);
			double Gyi = Gy->at<uchar>(i, j);

			G->row(i).col(j) = sqrt(Gxi * Gxi + Gyi * Gyi);

			double angle = (atan2(Gxi, Gyi) / M_PI) * 180;
			if ((angle > -22.5 && angle < 22.5) || angle > 157.5 || angle < -157.5) {
				direction->at<uchar>(i, j) = 0;
			} else if ((angle > 22.5 && angle < 67.5) || (angle > -157.5 && angle < -112.5)) {
				direction->at<uchar>(i, j) = 45;
			} else if ((angle > 67.5 && angle < 112.5) || (angle > -112.5 && angle < -67.5)) {
				direction->at<uchar>(i, j) = 90;
			} else {
				direction->at<uchar>(i, j) = 135;
			}
		}
	}
}

void CannyDetection::nonMaximumSuppression(cv::Mat *G, cv::Mat *direction) {
	for (int i = 0; i < G->rows; i++) {
		for (int j = 0; j < G->cols; j++) {
			if (direction->at<uchar>(i, j) == 0) {
				if (j == 0) {
					if (G->at<uchar>(i, j) < G->at<uchar>(i, j + 1)) G->at<uchar>(i, j) = 0;
				} else if (j == G->cols - 1) {
					if (G->at<uchar>(i, j) < G->at<uchar>(i, j - 1)) G->at<uchar>(i, j) = 0;
				} else {
					if (G->at<uchar>(i, j) < G->at<uchar>(i, j - 1) || G->at<uchar>(i, j) < G->at<uchar>(i, j + 1)) G->at<uchar>(i, j) = 0;
				}
			} else if (direction->at<uchar>(i, j) == 45) {
				if ((i == 0 && j == 0) || (i == G->rows - 1 && j == G->cols - 1)) {
					continue;
				} else if (i == 0 || j == G->cols - 1) {
					if (G->at<uchar>(i, j) < G->at<uchar>(i + 1, j - 1)) G->at<uchar>(i, j) = 0;
				} else if (i == G->rows - 1 || j == 0) {
					if (G->at<uchar>(i, j) < G->at<uchar>(i - 1, j + 1)) G->at<uchar>(i, j) = 0;
				} else {
					if (G->at<uchar>(i, j) < G->at<uchar>(i - 1, j + 1) || G->at<uchar>(i, j) < G->at<uchar>(i + 1, j - 1)) G->at<uchar>(i, j) = 0;
				}
			} else if (direction->at<uchar>(i, j) == 90) {
				if (i == 0) {
					if (G->at<uchar>(i, j) < G->at<uchar>(i + 1, j)) G->at<uchar>(i, j) = 0;
				} else if (i == G->rows - 1) {
					if (G->at<uchar>(i, j) < G->at<uchar>(i - 1, j)) G->at<uchar>(i, j) = 0;
				} else {
					if (G->at<uchar>(i, j) < G->at<uchar>(i - 1, j) || G->at<uchar>(i, j) < G->at<uchar>(i + 1, j)) G->at<uchar>(i, j) = 0;
				}
			} else {
				if ((i == 0 && j == G->cols - 1) || (i == G->rows - 1 && j == 0)) {
					continue;
				} else if (i == 0 || j == 0) {
					if (G->at<uchar>(i, j) < G->at<uchar>(i + 1, j + 1)) G->at<uchar>(i, j) = 0;
				} else if (i == G->rows - 1 || j == G->cols - 1) {
					if (G->at<uchar>(i, j) < G->at<uchar>(i - 1, j - 1)) G->at<uchar>(i, j) = 0;
				} else {
					if (G->at<uchar>(i, j) < G->at<uchar>(i - 1, j - 1) || G->at<uchar>(i, j) < G->at<uchar>(i + 1, j + 1)) G->at<uchar>(i, j) = 0;
				}
			}
		}
	}
}

void CannyDetection::doubleThresholding(cv::Mat *G) {
	double min = ui->MinSpinBox->value(), max = ui->MaxSpinBox->value();
	for (int i = 0; i < G->rows; i++) {
		for (int j = 0; j < G->cols; j++) {
			if (G->at<uchar>(i, j) < min) G->at<uchar>(i, j) = 0;
			else if (G->at<uchar>(i, j) > max) G->at<uchar>(i, j) = 255;
			else G->at<uchar>(i, j) = 127;
		}
	}
}
