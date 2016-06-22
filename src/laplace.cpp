#include "laplace.h"
#include "ui_laplace.h"
#include <iostream>

Laplace::Laplace(cv::Mat *src, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Laplace)
{
    ui->setupUi(this);
    Laplace::resize(parent->size());
    prepareGUI();

    cv::cvtColor((*src), img, CV_BGR2GRAY);
}

Laplace::~Laplace()
{
    delete ui;
}

void Laplace::showEvent(QShowEvent *event) {
    QMainWindow::showEvent(event);
    detectEdges();
}

void Laplace::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);
    showImage();
}

void Laplace::showImage() {
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

void Laplace::prepareGUI() {
    connect(ui->EightRadioButton, SIGNAL(clicked()), this, SLOT(detectEdges()));
    connect(ui->FourRadioButton, SIGNAL(clicked()), this, SLOT(detectEdges()));
    connect(ui->sharpCoefSpinBox, SIGNAL(valueChanged(double)), this, SLOT(detectEdges()));
    connect(ui->saveButton, SIGNAL(clicked(bool)), this, SLOT(saveImage()));
}

void Laplace::detectEdges() {
    cv::Mat blured;
    cv::blur(img, blured, cv::Size(3,3));
    //cv::medianBlur(img, blured, 3);

    edges.create(blured.size(), CV_64F);
    if (ui->EightRadioButton->isChecked()) op = (cv::Mat_<int>(3,3) << 1, 1, 1, 1, -8, 1, 1, 1, 1);
    else op = (cv::Mat_<int>(3,3) << 0, 1, 0, 1, -4, 1, 0, 1, 0);
    cv::filter2D(blured, edges, -1, op);
    edges = edges * ui->sharpCoefSpinBox->value();

    cv::convertScaleAbs(edges, edges);
    showImage();
}

void Laplace::saveImage() {
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
