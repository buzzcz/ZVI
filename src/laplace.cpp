#include "laplace.h"
#include "ui_laplace.h"

Laplace::Laplace(cv::Mat *src, QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::Laplace)
{
	ui->setupUi(this);
	prepareRadioButtons();

	cv::cvtColor((*src), img, CV_BGR2GRAY);
	detectEdges();
}

Laplace::~Laplace()
{
	delete ui;
}

void Laplace::prepareRadioButtons() {
	ui->EightRadioButton->setChecked(true);
	connect(ui->EightRadioButton, SIGNAL(clicked()), this, SLOT(detectEdges()));
	connect(ui->FourRadioButton, SIGNAL(clicked()), this, SLOT(detectEdges()));
    connect(ui->sharpCoefSpinBox, SIGNAL(valueChanged(double)), this, SLOT(detectEdges()));
}

void Laplace::detectEdges() {
	cv::Mat edges;
	cv::Mat blured;
	cv::blur(img, blured, cv::Size(3,3));
	//cv::medianBlur(img, blured, 3);

    edges.create(blured.size(), CV_64F);
	if (ui->EightRadioButton->isChecked()) op = (cv::Mat_<int>(3,3) << -1, -1, -1, -1, 8, -1, -1, -1, -1);
	else op = (cv::Mat_<int>(3,3) << 0, -1, 0, -1, 4, -1, 0, -1, 0);
	cv::filter2D(blured, edges, -1, op);
    edges = edges * ui->sharpCoefSpinBox->value();

    cv::convertScaleAbs(edges, edges);
	QPixmap p = QPixmap::fromImage(QImage((unsigned char*) edges.data, edges.cols, edges.rows, edges.step, QImage::Format_Indexed8));
	ui->label->setPixmap(p);
	ui->label->setFixedSize(p.size());
	ui->centralwidget->setFixedSize(p.size());
}
