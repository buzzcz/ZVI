#include "defineddirection.h"
#include "ui_defineddirection.h"

DefinedDirection::DefinedDirection(cv::Mat *src, QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::DefinedDirection)
{
	ui->setupUi(this);
	connect(ui->sharpCoefSpinBox, SIGNAL(valueChanged(double)), this, SLOT(detectEdges()));
	prepareDirectionComboBox();

	cv::cvtColor((*src), img, CV_BGR2GRAY);
	//(*src).copyTo(img);
	detectEdges();
}

DefinedDirection::~DefinedDirection()
{
	delete ui;
}

void DefinedDirection::prepareDirectionComboBox() {
	QStringList directions = QStringList() << tr("Right (0)") << tr("Top-Right (1)") << tr("Top (2)") << tr("Top-Left (3)") << tr("Left (4)") << tr("Bottom-Left (5)") << tr("Bottom (6)") << tr("Bottom-Right (7)");
	ui->directionComboBox->addItems(directions);
	connect(ui->directionComboBox, SIGNAL(activated(int)), this, SLOT(detectEdges()));
}

void DefinedDirection::detectEdges() {
	double sharp = ui->sharpCoefSpinBox->value();
	int direction = ui->directionComboBox->currentIndex();
	cv::Mat edges;
	cv::Mat blured;
	cv::blur(img, blured, cv::Size(3,3));
	//cv::medianBlur(img, blured, 3);

	edges.create(blured.size(), blured.type());
	switch (direction) {
		case 0:
			for (int i = 0; i < blured.rows; i++) {
				for (int j = 0; j < blured.cols; j++) {
			if (j == 0) {
				edges.row(i).col(j) = blured.row(i).col(j);
			} else {
				edges.row(i).col(j) = sharp * abs(blured.row(i).col(j) - blured.row(i).col(j - 1));
			}
				}
			}
			break;
		case 1:
			for (int i = 0; i < blured.rows; i++) {
				for (int j = 0; j < blured.cols; j++) {
			if (j == 0 || i == 0) {
				edges.row(i).col(j) = blured.row(i).col(j);
			} else {
				edges.row(i).col(j) = sharp * abs(blured.row(i).col(j) - blured.row(i - 1).col(j - 1));
			}
				}
			}
			break;
		case 2:
			for (int i = 0; i < blured.rows; i++) {
				for (int j = 0; j < blured.cols; j++) {
			if (i == 0) {
				edges.row(i).col(j) = blured.row(i).col(j);
			} else {
				edges.row(i).col(j) = sharp * abs(blured.row(i).col(j) - blured.row(i - 1).col(j));
			}
				}
			}
			break;
		case 3:
			for (int i = 0; i < blured.rows; i++) {
				for (int j = 0; j < blured.cols; j++) {
			if (i == 0 || j == blured.cols - 1) {
				edges.row(i).col(j) = blured.row(i).col(j);
			} else {
				edges.row(i).col(j) = sharp * abs(blured.row(i).col(j) - blured.row(i - 1).col(j + 1));
			}
				}
			}
			break;
		case 4:
			for (int i = 0; i < blured.rows; i++) {
				for (int j = 0; j < blured.cols; j++) {
			if (j == blured.cols - 1) {
				edges.row(i).col(j) = blured.row(i).col(j);
			} else {
				edges.row(i).col(j) = sharp * abs(blured.row(i).col(j) - blured.row(i).col(j + 1));
			}
				}
			}
			break;
		case 5:
			for (int i = 0; i < blured.rows; i++) {
				for (int j = 0; j < blured.cols; j++) {
			if (j == blured.cols - 1 || i == blured.rows - 1) {
				edges.row(i).col(j) = blured.row(i).col(j);
			} else {
				edges.row(i).col(j) = sharp * abs(blured.row(i).col(j) - blured.row(i + 1).col(j + 1));
			}
				}
			}
			break;
		case 6:
			for (int i = 0; i < blured.rows; i++) {
				for (int j = 0; j < blured.cols; j++) {
			if (i == blured.rows - 1) {
				edges.row(i).col(j) = blured.row(i).col(j);
			} else {
				edges.row(i).col(j) = sharp * abs(blured.row(i).col(j) - blured.row(i + 1).col(j));
			}
				}
			}
			break;
		case 7:
			for (int i = 0; i < blured.rows; i++) {
				for (int j = 0; j < blured.cols; j++) {
			if (i == blured.rows - 1 || j == 0) {
				edges.row(i).col(j) = blured.row(i).col(j);
			} else {
				edges.row(i).col(j) = sharp * abs(blured.row(i).col(j) - blured.row(i + 1).col(j - 1));
			}
				}
			}
			break;
		default:
			break;
	}

	QPixmap p = QPixmap::fromImage(QImage((unsigned char*) edges.data, edges.cols, edges.rows, QImage::Format_Indexed8));
	//QPixmap p = QPixmap::fromImage(QImage((unsigned char*) edges.data, edges.cols, edges.rows, QImage::Format_RGB888).rgbSwapped());
	ui->label->setPixmap(p);
	ui->label->setFixedSize(p.size());
	ui->centralwidget->setFixedSize(p.size());
}
