#include "pointandline.h"
#include "ui_pointandline.h"

#include "QDebug"

PointAndLine::PointAndLine(cv::Mat *src, QWidget *parent) :
		QMainWindow(parent),
		ui(new Ui::PointAndLine)
{
		ui->setupUi(this);
		PointAndLine::resize(parent->size());
		prepareGUI();

		cv::cvtColor(*src, img, CV_BGR2GRAY);
}

PointAndLine::~PointAndLine()
{
		delete ui;
}

void PointAndLine::showEvent(QShowEvent *event) {
		QMainWindow::showEvent(event);
		detectEdges();
}

void PointAndLine::resizeEvent(QResizeEvent *event) {
		QMainWindow::resizeEvent(event);
		showImage();
}

void PointAndLine::showImage() {
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

void PointAndLine::prepareGUI() {
	this->ui->cbLineDirection->addItems(QStringList() << "Vertical" << "45°" << "-45°" << "Horizontal");
	connect(ui->saveButton,			 SIGNAL(clicked(bool)),     this, SLOT(saveImage()));

	connect(ui->cbLineDirection, SIGNAL(activated(int)),		this,	SLOT(detectEdges()));
	connect(ui->rbLine,					 SIGNAL(toggled(bool)),			this,	SLOT(detectEdges()));
	connect(ui->radioButton,		 SIGNAL(toggled(bool)),			this,	SLOT(detectEdges()));
	connect(ui->spinBox,				 SIGNAL(valueChanged(int)), this, SLOT(detectEdges()));
}

void PointAndLine::detectEdges() {
	cv::Mat blured, op;
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


		showImage();
}

void PointAndLine::saveImage() {
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
