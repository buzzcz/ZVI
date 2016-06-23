#include "defineddirection.h"
#include "ui_defineddirection.h"

DefinedDirection::DefinedDirection(cv::Mat *src, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::DefinedDirection)
{
    ui->setupUi(this);
    DefinedDirection::resize(parent->size());
    prepareGUI();

    cv::cvtColor(*src, img, CV_BGR2GRAY);
}

DefinedDirection::~DefinedDirection()
{
    delete ui;
}

void DefinedDirection::showEvent(QShowEvent *event) {
    QMainWindow::showEvent(event);
    detectEdges();
}

void DefinedDirection::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);
    showImage();
}

void DefinedDirection::showImage() {
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

void DefinedDirection::prepareGUI() {
    QStringList directions = QStringList() << tr("Right (0)") << tr("Top-Right (1)") << tr("Top (2)") << tr("Top-Left (3)") << tr("Left (4)") << tr("Bottom-Left (5)") << tr("Bottom (6)") << tr("Bottom-Right (7)");
    ui->directionComboBox->addItems(directions);
    connect(ui->directionComboBox, SIGNAL(activated(int)), this, SLOT(detectEdges()));

    connect(ui->sharpCoefSpinBox, SIGNAL(valueChanged(double)), this, SLOT(detectEdges()));

    connect(ui->saveButton, SIGNAL(clicked(bool)), this, SLOT(saveImage()));
}

void DefinedDirection::detectEdges() {
    double sharp = ui->sharpCoefSpinBox->value();
    int direction = ui->directionComboBox->currentIndex();
    cv::Mat blured;
    cv::blur(img, blured, cv::Size(3,3));

    edges.create(blured.size(), CV_64F);

    switch (direction) {
    case 0:
        for (int i = 1; i < blured.rows - 1; i++) {
            for (int j = 1; j < blured.cols -1; j++) {
                edges.at<double>(i, j) = sharp * abs(blured.at<uchar>(i, j) - blured.at<uchar>(i, j - 1));
            }
        }
        break;
    case 1:
        for (int i = 1; i < blured.rows - 1; i++) {
            for (int j = 1; j < blured.cols -1; j++) {
                edges.at<double>(i, j) = sharp * abs(blured.at<uchar>(i, j) - blured.at<uchar>(i + 1, j - 1));
            }
        }
        break;
    case 2:
        for (int i = 1; i < blured.rows - 1; i++) {
            for (int j = 1; j < blured.cols -1; j++) {
                edges.at<double>(i, j) = sharp * abs(blured.at<uchar>(i, j) - blured.at<uchar>(i + 1, j));
            }
        }
        break;
    case 3:
        for (int i = 1; i < blured.rows - 1; i++) {
            for (int j = 1; j < blured.cols -1; j++) {
                edges.at<double>(i, j) = sharp * abs(blured.at<uchar>(i, j) - blured.at<uchar>(i + 1, j + 1));
            }
        }
        break;
    case 4:
        for (int i = 1; i < blured.rows - 1; i++) {
            for (int j = 1; j < blured.cols -1; j++) {
                edges.at<double>(i, j) = sharp * abs(blured.at<uchar>(i, j) - blured.at<uchar>(i, j + 1));
            }
        }
        break;
    case 5:
        for (int i = 1; i < blured.rows - 1; i++) {
            for (int j = 1; j < blured.cols -1; j++) {
                edges.at<double>(i, j) = sharp * abs(blured.at<uchar>(i, j) - blured.at<uchar>(i - 1, j + 1));
            }
        }
        break;
    case 6:
        for (int i = 1; i < blured.rows - 1; i++) {
            for (int j = 1; j < blured.cols -1; j++) {
                edges.at<double>(i, j) = sharp * abs(blured.at<uchar>(i, j) - blured.at<uchar>(i - 1, j));
            }
        }
        break;
    case 7:
        for (int i = 1; i < blured.rows - 1; i++) {
            for (int j = 1; j < blured.cols -1; j++) {
                edges.at<double>(i, j) = sharp * abs(blured.at<uchar>(i, j) - blured.at<uchar>(i - 1, j - 1));
            }
        }
        break;
    default:
        break;
    }

    cv::convertScaleAbs(edges, edges);
    showImage();
}

void DefinedDirection::saveImage() {
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
