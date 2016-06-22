#include "marrhildreth.h"
#include "ui_marrhildreth.h"

#include "QDebug"

MarrHildreth::MarrHildreth(cv::Mat *src, QWidget *parent) :
		QMainWindow(parent),
		ui(new Ui::MarrHildreth)
{
		ui->setupUi(this);
		MarrHildreth::resize(parent->size());
		prepareGUI();

		cv::cvtColor(*src, img, CV_BGR2GRAY);
}

MarrHildreth::~MarrHildreth()
{
		delete ui;
}

void MarrHildreth::showEvent(QShowEvent *event) {
		QMainWindow::showEvent(event);
		detectEdges();
}

void MarrHildreth::resizeEvent(QResizeEvent *event) {
		QMainWindow::resizeEvent(event);
		showImage();
}

void MarrHildreth::showImage() {
		if (!edges.empty()) {
				cv::Mat tmp;
				double width = ui->label->width(), height = ui->label->height();
				if (width >= height) {
						double ratio = height / edges.rows;
						width = edges.cols * ratio;
				} else {
						double ratio = width / edges.cols;
						height = edges.rows * ratio;
				}
				cv::resize(edges, tmp, cv::Size(width, height));
				ui->label->setPixmap(QPixmap::fromImage(QImage((unsigned char*) tmp.data, tmp.cols, tmp.rows, tmp.step, QImage::Format_Indexed8)));
		}
}

void MarrHildreth::prepareGUI() {
		connect(ui->saveButton, SIGNAL(clicked(bool)), this, SLOT(saveImage()));
		connect(ui->tresholdSpin, SIGNAL(valueChanged(int)), this, SLOT(detectEdges()));
}

void MarrHildreth::detectEdges() {
		cv::Mat blured;
		cv::blur(img, blured, cv::Size(3,3));

		edges = blured.clone();

		int logOperator[5][5] = {
			{0, 0, -1, 0, 0},
			{0, -1, -2, -1, 0},
			{-1, -2, 16, -2, -1},
			{0, -1, -2, -1, 0},
			{0, 0, -1, 0, 0}
		};
		int l1 = 5;

		int border1 = 2;
		int border2 = 1;

		int h = edges.rows;
		int w = edges.cols;

		int tempImage[h][w];
		for (int i = 0; i < h; i++) {
			for (int j = 0; j < w; j++) {
				tempImage[i][j] = 0;
				edges.at<uchar>(i, j) = 0;
			}
		}

		//log convolution
		int value;

		for (int i = border1; i < h - border1; i++) {
			for (int j = border1; j < w - border1; j++) {
				value = 0;
				for (int k = 0; k < l1; k++) {
					for (int l = 0; l < l1; l++) {
						value = value + (logOperator[k][l] * blured.at<uchar>(i - border1 + k, j - border1 + l));
					}
				}
				tempImage[i][j] = value;
			}
		}

		//zero crossing + tresholding

		int max = 0;

		for (int k = 0;k < h; k++) {
			for (int i = 0; i < w; i++) {
				if (tempImage[k][i] > max)
					max = tempImage[k][i];
			}
		}
		int treshold = this->ui->tresholdSpin->value();

		for (int i = border2; i < h - border2; i++) {
			for (int j = border2; j < w - border2; j++) {
				if (tempImage[i][j] != 0) {
					if ((tempImage[i][j + 1] >= 0 && tempImage[i][j - 1] < 0) || (tempImage[i][j + 1] < 0 && tempImage[i][j - 1] >= 0)) {
						if (tempImage[i][j] > treshold) {
							edges.at<uchar>(i, j) = 255;
						}
					} else if ((tempImage[i + 1][j] >= 0 && tempImage[i - 1][j] < 0) || (tempImage[i + 1][j] < 0 && tempImage[i - 1][j] >= 0)) {
						if (tempImage[i][j] > treshold) {
							edges.at<uchar>(i, j) = 255;
						}
					} else if ((tempImage[i + 1][j + 1] >= 0 && tempImage[i - 1][j - 1] < 0) || (tempImage[i + 1][j + 1] < 0 && tempImage[i - 1][j - 1] >= 0)) {
						if (tempImage[i][j] > treshold) {
							edges.at<uchar>(i, j) = 255;
						}
					} else if ((tempImage[i - 1][j + 1] >= 0 && tempImage[i + 1][j - 1] < 0) || (tempImage[i - 1][j + 1] < 0 && tempImage[i + 1][j - 1] >= 0)) {
						if (tempImage[i][j] > treshold) {
							edges.at<uchar>(i, j) = 255;
						}
					}
				}
			}
		}


		showImage();
}

void MarrHildreth::saveImage() {
		QString fileName = QFileDialog::getSaveFileName(this, tr("Save Image"), QDir::currentPath(), tr("Image Files (*.png *.jpg *.bmp)"));
		if (!fileName.isNull() && !fileName.isEmpty()) {
				QFileInfo info(fileName);
				if (info.suffix().isNull() || info.suffix().isEmpty())
						fileName.append(".png");
				else if (QString::compare(info.suffix(), "png", Qt::CaseInsensitive) != 0 && QString::compare(info.suffix(), "jpg", Qt::CaseInsensitive) != 0 && QString::compare(info.suffix(), "bmp", Qt::CaseInsensitive) != 0)
						fileName.append(".png");
				imwrite(fileName.toStdString(), edges);
				QMessageBox ok;
				ok.setWindowTitle(tr("Image saved"));
				ok.setText(tr("Image has been saved succesfully"));
				ok.exec();
		}
}
