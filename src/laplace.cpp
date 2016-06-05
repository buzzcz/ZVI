#include "laplace.h"
#include "ui_laplace.h"

Laplace::Laplace(cv::Mat *src, QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::Laplace)
{
	ui->setupUi(this);
    prepareGUI();

	cv::cvtColor((*src), img, CV_BGR2GRAY);
	detectEdges();
}

Laplace::~Laplace()
{
	delete ui;
}

void Laplace::prepareGUI() {
	ui->EightRadioButton->setChecked(true);
	connect(ui->EightRadioButton, SIGNAL(clicked()), this, SLOT(detectEdges()));
	connect(ui->FourRadioButton, SIGNAL(clicked()), this, SLOT(detectEdges()));
    connect(ui->sharpCoefSpinBox, SIGNAL(valueChanged(double)), this, SLOT(detectEdges()));
    connect(ui->saveButton, SIGNAL(clicked(bool)), this, SLOT(saveImage()));
}

cv::Mat Laplace::detectEdges() {
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
    return edges;
}

void Laplace::saveImage() {
    QString fileName = QFileDialog::getSaveFileName(this, tr("Save Image"), QDir::currentPath(), tr("Image Files (*.png *.jpg *.bmp)"));
    if (!fileName.isNull() && !fileName.isEmpty()) {
        QFileInfo info(fileName);
        if (info.suffix().isNull() || info.suffix().isEmpty())
            fileName.append(".png");
        else if (QString::compare(info.suffix(), "png", Qt::CaseInsensitive) != 0 && QString::compare(info.suffix(), "jpg", Qt::CaseInsensitive) != 0 && QString::compare(info.suffix(), "bmp", Qt::CaseInsensitive) != 0)
            fileName.append(".png");
        cv::Mat img = detectEdges();
        imwrite(fileName.toStdString(), img);
        QMessageBox ok;
        ok.setWindowTitle(tr("Image saved"));
        ok.setText(tr("Image has been saved succesfully"));
        ok.exec();
    }
}
