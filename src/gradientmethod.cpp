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
    QStringList directions = QStringList() << tr("Right (0)") << tr("Top-Right (1)") << tr("Top (2)") << tr("Top-Left (3)") << tr("Left (4)") << tr("Bottom-Left (5)") << tr("Bottom (6)") << tr("Bottom-Right (7)");
    ui->directionComboBox->addItems(directions);
    connect(ui->directionComboBox, SIGNAL(activated(int)), this, SLOT(detectEdges()));

    connect(ui->sharpCoefSpinBox, SIGNAL(valueChanged(double)), this, SLOT(detectEdges()));

    connect(ui->saveButton, SIGNAL(clicked(bool)), this, SLOT(saveImage()));
}

void GradientMethod::detectEdges() {
    double sharp = ui->sharpCoefSpinBox->value();
    int direction = ui->directionComboBox->currentIndex();
		cv::Mat blured, gradientX, gradientY;
		cv::blur(img, blured, cv::Size(3,3));

		//cv::Mat op = (cv::Mat_<int>(1,3) << -1, 0, 1);
		cv::Mat op = (cv::Mat_<int>(3,3) << -1, 0, 1, -2, 0, 2, -1, 0, 1);

		 cv::Sobel(blured, gradientX, CV_64F, 1, 0, 3);
		 cv::Sobel(blured, gradientY, CV_64F, 0, 1, 3);

		edges.create(gradientX.size(), CV_64F);

		double magnitude;
		int val;
		for (int i = 0; i < edges.rows; i++) {
			for (int j = 0; j < edges.cols; j++) {
				//edges.at<cv::Vec3b>(i, j)[0]
				magnitude = sqrt(gradientX.at<cv::Vec3b>(i, j)[0] * gradientX.at<cv::Vec3b>(i, j)[0]
											 + gradientY.at<cv::Vec3b>(i, j)[0] * gradientY.at<cv::Vec3b>(i, j)[0]);
				val = (int) round(magnitude);
				if (val > 255) val = 255;
				edges.at<cv::Vec3b>(i, j)[0] = val;
				edges.at<cv::Vec3b>(i, j)[1] = val;
				edges.at<cv::Vec3b>(i, j)[2] = val;
			}
		}



    cv::convertScaleAbs(edges, edges);
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
