#include "gradientmethod.h"
#include "ui_gradientmethod.h"

#include "QDebug"

GradientMethod::GradientMethod(cv::Mat *src, QWidget *parent) :
		QMainWindow(parent),
		ui(new Ui::GradientMethod)
{
		ui->setupUi(this);
		GradientMethod::resize(parent->size());
		prepareGUI();

		cv::cvtColor(*src, img, CV_BGR2GRAY);
}

GradientMethod::~GradientMethod()
{
		delete ui;
}

void GradientMethod::showEvent(QShowEvent *event) {
		QMainWindow::showEvent(event);
		detectEdges();
}

void GradientMethod::resizeEvent(QResizeEvent *event) {
		QMainWindow::resizeEvent(event);
		showImage();
}

void GradientMethod::showImage() {
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

void GradientMethod::prepareGUI() {
		connect(ui->saveButton, SIGNAL(clicked(bool)), this, SLOT(saveImage()));
}

void GradientMethod::detectEdges() {
		cv::Mat blured;
		cv::blur(img, blured, cv::Size(3,3));

		edges = img.clone();

		double magnitude;
		int val, rows = img.rows - 1, cols = img.cols - 1;
		double dx, dy;

		for (int i = 1; i < rows; i++) {
			for (int j = 1; j < cols; j++) {
                dx = (double) (img.at<uchar>(i+1, j) - img.at<uchar>(i-1, j)) / 2.0;
                dy = (double) (img.at<uchar>(i, j+1) - img.at<uchar>(i, j-1)) / 2.0;

				magnitude = sqrt(dx * dx + dy * dy);

				val = (int) round(magnitude);
				if (val > 255) val = 255;

                edges.at<uchar>(i-1, j-1) = val;
			}
		}


		showImage();
}

void GradientMethod::saveImage() {
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
