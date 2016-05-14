#include "canny.h"
#include "ui_canny.h"
#include <iostream>

CannyDetection::CannyDetection(cv::Mat *src, QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::CannyDetection)
{
	ui->setupUi(this);
	connect(ui->MinSpinBox, SIGNAL(valueChanged(double)), this, SLOT(detectEdges()));
	connect(ui->RatioSpinBox, SIGNAL(valueChanged(int)), this, SLOT(detectEdges()));

	cv::cvtColor((*src), img, CV_BGR2GRAY);
	detectEdges();
}

CannyDetection::~CannyDetection()
{
	delete ui;
}

void CannyDetection::detectEdges() {
	cv::Mat blured;
	//cv::GaussianBlur(img, blured, cv::Size(5, 5), 1.4);
	img.copyTo(blured);

	cv::Mat Gx, Gy;
	cv::Sobel(blured, Gx, CV_64F, 1, 0);
	cv::Sobel(blured, Gy, CV_64F, 0, 1);
    //cv::convertScaleAbs(Gx, Gx);
    //cv::convertScaleAbs(Gy, Gy);

	cv::Mat G, direction;
    G.create(Gx.size(), Gx.type());
	direction.create(Gx.size(), Gx.type());
	getGradientsAndDirections(&Gx, &Gy, &G, &direction);

	nonMaximumSuppression(&G, &direction);

    hysteresis(&G, &direction);

    cv::convertScaleAbs(G, G);
	QPixmap p = QPixmap::fromImage(QImage((unsigned char*) G.data, G.cols, G.rows, G.step, QImage::Format_Indexed8));
	ui->label->setPixmap(p);
	ui->label->setFixedSize(p.size());
	ui->centralwidget->setFixedSize(p.size());
}

void CannyDetection::getGradientsAndDirections(cv::Mat *Gx, cv::Mat *Gy, cv::Mat *G, cv::Mat *direction) {
	for (int i = 0; i < G->rows; i++) {
		for (int j = 0; j < G->cols; j++) {
            double Gxi = Gx->at<double>(i, j);
            double Gyi = Gy->at<double>(i, j);

            G->at<double>(i, j) = sqrt(Gxi * Gxi + Gyi * Gyi);

			double angle = (atan2(Gyi, Gxi) / M_PI) * 180;
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
			switch (direction->at<uchar>(i, j)) {
				case 0:
					if (j == 0) {
                        if (G->at<double>(i, j) < G->at<double>(i, j + 1)) G->at<double>(i, j) = 0;
					} else if (j == G->cols - 1) {
                        if (G->at<double>(i, j) < G->at<double>(i, j - 1)) G->at<double>(i, j) = 0;
					} else {
                        if (G->at<double>(i, j) < G->at<double>(i, j - 1) || G->at<double>(i, j) < G->at<double>(i, j + 1)) G->at<double>(i, j) = 0;
					}
					break;
				case 45:
					if ((i == 0 && j == 0) || (i == G->rows - 1 && j == G->cols - 1)) {
						continue;
					} else if (i == 0 || j == G->cols - 1) {
                        if (G->at<double>(i, j) < G->at<double>(i + 1, j - 1)) G->at<double>(i, j) = 0;
					} else if (i == G->rows - 1 || j == 0) {
                        if (G->at<double>(i, j) < G->at<double>(i - 1, j + 1)) G->at<double>(i, j) = 0;
					} else {
                        if (G->at<double>(i, j) < G->at<double>(i - 1, j + 1) || G->at<double>(i, j) < G->at<double>(i + 1, j - 1)) G->at<double>(i, j) = 0;
					}
					break;
				case 90:
					if (i == 0) {
                        if (G->at<double>(i, j) < G->at<double>(i + 1, j)) G->at<double>(i, j) = 0;
					} else if (i == G->rows - 1) {
                        if (G->at<double>(i, j) < G->at<double>(i - 1, j)) G->at<double>(i, j) = 0;
					} else {
                        if (G->at<double>(i, j) < G->at<double>(i - 1, j) || G->at<double>(i, j) < G->at<double>(i + 1, j)) G->at<double>(i, j) = 0;
					}
					break;
				default:
					if ((i == 0 && j == G->cols - 1) || (i == G->rows - 1 && j == 0)) {
						continue;
					} else if (i == 0 || j == 0) {
                        if (G->at<double>(i, j) < G->at<double>(i + 1, j + 1)) G->at<double>(i, j) = 0;
					} else if (i == G->rows - 1 || j == G->cols - 1) {
                        if (G->at<double>(i, j) < G->at<double>(i - 1, j - 1)) G->at<double>(i, j) = 0;
					} else {
                        if (G->at<double>(i, j) < G->at<double>(i - 1, j - 1) || G->at<double>(i, j) < G->at<double>(i + 1, j + 1)) G->at<double>(i, j) = 0;
					}
					break;
			}
		}
	}
}

void CannyDetection::hysteresis(cv::Mat *G, cv::Mat *direction) {
	double min = ui->MinSpinBox->value(), ratio = ui->RatioSpinBox->value(), max = min * ratio;
	for (int i = 0; i < G->rows; i++) {
		for (int j = 0; j < G->cols; j++) {
            if (G->at<double>(i, j) < min) G->at<double>(i, j) = 0;
            else if (G->at<double>(i, j) > max) {
                G->at<double>(i, j) = 255;
				int k = i, l = j;
				while (k > 0 && k < G->rows - 1 && l > 0 && l < G->cols - 1) {
                    if (direction->at<uchar>(k, l) == 135 && G->at<double>(k - 1, l - 1) > min && G->at<double>(k - 1, l - 1) < max) {
                        G->at<double>(k - 1, l - 1) = 255;
						k--;
						l--;
                    } else if (direction->at<uchar>(k, l) == 90 && G->at<double>(k - 1, l) > min && G->at<double>(k - 1, l) < max) {
                        G->at<double>(k - 1, l) = 255;
						k--;
                    } else if (direction->at<uchar>(k, l) == 45 && G->at<double>(k - 1, l + 1) > min && G->at<double>(k - 1, l + 1) < max) {
                        G->at<double>(k - 1, l + 1) = 255;
						k--;
						l++;
                    } else if (direction->at<uchar>(k, l) == 0 && G->at<double>(k, l - 1) > min && G->at<double>(k, l - 1) < max) {
                        G->at<double>(k, l - 1) = 255;
						l--;
                    } else if (direction->at<uchar>(k, l) == 0 && G->at<double>(k, l + 1) > min && G->at<double>(k, l + 1) < max) {
                        G->at<double>(k, l + 1) = 255;
						l++;
                    } else if (direction->at<uchar>(k, l) == 45 && G->at<double>(k + 1, l - 1) > min && G->at<double>(k + 1, l - 1) < max) {
                        G->at<double>(k + 1, l - 1) = 255;
						k++;
						l--;
                    } else if (direction->at<uchar>(k, l) == 90 && G->at<double>(k + 1, l) > min && G->at<double>(k + 1, l) < max) {
                        G->at<double>(k + 1, l) = 255;
						k++;
                    } else if (direction->at<uchar>(k, l) == 135 && G->at<double>(k + 1, l + 1) > min && G->at<double>(k + 1, l + 1) < max) {
                        G->at<double>(k + 1, l + 1) = 255;
						k++;
						l++;
					} else {
						k = 0;
					}
				}
			}
		}
	}
	for (int i = 0; i < G->rows; i++) {
		for (int j = 0; j < G->cols; j++) {
            if (G->at<double>(i, j) < max) G->at<double>(i, j) = 0;
		}
	}
}
