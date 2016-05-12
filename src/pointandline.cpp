#include "pointandline.h"
#include "ui_pointandline.h"

#include <QDebug>

PointAndLine::PointAndLine(cv::Mat *src, QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::PointAndLine)
{
	ui->setupUi(this);

	connect(this->ui->btnProcess, SIGNAL(clicked()), this, SLOT(process()));

	this->ui->cbLineDirection->addItems(QStringList() << "Vertical" << "45°" << "-45°" << "Horizontal");

	cv::cvtColor((*src), img, CV_BGR2GRAY);
	process();
}

PointAndLine::~PointAndLine()
{
	delete ui;
}

void PointAndLine::process()
{
	cv::Mat edges;
	cv::Mat blured;
	cv::blur(img, blured, cv::Size(3,3));

	int direction = this->ui->cbLineDirection->currentIndex();
	int val = this->ui->spinBox->value();

	edges.create(blured.size(), blured.type());

	if (this->ui->rbLine->isChecked()) {
		int negative = -1 * val;
		int positive = 2 * val;

		switch (direction) {
		/* vertical */
		case 0: op = (cv::Mat_<int>(3,3) << negative, positive, negative,
																				negative, positive, negative,
																				negative, positive, negative); break;
		/* 45° */
		case 1: op = (cv::Mat_<int>(3,3) << negative, negative, positive,
																				negative, positive, negative,
																				positive, negative, negative); break;
		/* -45° */
		case 2: op = (cv::Mat_<int>(3,3) << positive, negative, negative,
																				negative, positive, negative,
																				negative, negative, positive); break;
		/* Horizontal */
		case 3: op = (cv::Mat_<int>(3,3) << negative, negative, negative,
																				positive, positive, positive,
																				negative, negative, negative); break;
		}
	} else {
		int negative = -8 * val;
		int positive = 1 * val;

		op = (cv::Mat_<int>(3,3) << positive, positive, positive, positive, negative, positive, positive, positive, positive);
	}

	cv::filter2D(blured, edges, -1, op);

	QPixmap p = QPixmap::fromImage(QImage((unsigned char*) edges.data, edges.cols, edges.rows, edges.step, QImage::Format_Indexed8));
	ui->label->setPixmap(p);
	ui->label->setFixedSize(p.size());
	ui->centralwidget->setFixedSize(p.size());
}
