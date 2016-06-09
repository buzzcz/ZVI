#include "canny.h"
#include "ui_canny.h"
#include <iostream>

CannyDetection::CannyDetection(cv::Mat *src, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CannyDetection)
{
    ui->setupUi(this);
    CannyDetection::resize(parent->size());
    prepareGUI();

    cv::cvtColor(*src, img, CV_BGR2GRAY);
}

CannyDetection::~CannyDetection()
{
    delete ui;
}

void CannyDetection::showEvent(QShowEvent *event) {
    QMainWindow::showEvent(event);
    detectEdges();
}

void CannyDetection::resizeEvent(QResizeEvent *event) {
    QMainWindow::resizeEvent(event);
    showImage();
}

void CannyDetection::showImage() {
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

void CannyDetection::prepareGUI() {
    connect(ui->MinSpinBox, SIGNAL(valueChanged(double)), this, SLOT(detectEdges()));
    connect(ui->RatioSpinBox, SIGNAL(valueChanged(int)), this, SLOT(detectEdges()));
    connect(ui->saveButton, SIGNAL(clicked(bool)), this, SLOT(saveImage()));
}

void CannyDetection::detectEdges() {
    cv::Mat blured;
    cv::GaussianBlur(img, blured, cv::Size(5, 5), 1.4);
    //img.copyTo(blured);

    cv::Mat Gx, Gy;
    cv::Sobel(blured, Gx, CV_64F, 1, 0);
    cv::Sobel(blured, Gy, CV_64F, 0, 1);
    //cv::convertScaleAbs(Gx, Gx);
    //cv::convertScaleAbs(Gy, Gy);

    cv::Mat G, direction;
    G.create(Gx.size(), Gx.type());
    direction.create(Gx.size(), Gx.type());
    getGradientsAndDirections(&Gx, &Gy, &G, &direction);

    edges.create(G.size(), G.type());
    nonMaximumSuppression(&G, &direction, &edges);

    hysteresis(&edges, &direction);

    //    doubleThresholding(&edges);
    //    hysteresis(&edges);

    cv::convertScaleAbs(edges, edges);
    showImage();
}

void CannyDetection::getGradientsAndDirections(cv::Mat *Gx, cv::Mat *Gy, cv::Mat *G, cv::Mat *direction) {
    for (int i = 0; i < G->rows; i++) {
        for (int j = 0; j < G->cols; j++) {
            if (i == 0 || i == G->rows - 1 || j == 0 || j == G->cols - 1) G->at<double>(i, j) = 0;
            else {
                double Gxi = Gx->at<double>(i, j);
                double Gyi = Gy->at<double>(i, j);

                G->at<double>(i, j) = sqrt(Gxi * Gxi + Gyi * Gyi);

                double angle = (atan2(Gyi, Gxi) / M_PI) * 180;
                if ((angle >= -22.5 && angle <= 22.5) || angle >= 157.5 || angle <= -157.5) {
                    direction->at<uchar>(i, j) = 0;
                } else if ((angle >= 22.5 && angle <= 67.5) || (angle >= -157.5 && angle <= -112.5)) {
                    direction->at<uchar>(i, j) = 45;
                } else if ((angle >= 67.5 && angle <= 112.5) || (angle >= -112.5 && angle <= -67.5)) {
                    direction->at<uchar>(i, j) = 90;
                } else {
                    direction->at<uchar>(i, j) = 135;
                }
            }
        }
    }
}

void CannyDetection::nonMaximumSuppression(cv::Mat *G, cv::Mat *direction, cv::Mat *edges) {
    for (int i = 1; i < G->rows - 1; i++) {
        for (int j = 1; j < G->cols - 1; j++) {
            edges->at<double>(i, j) = G->at<double>(i, j);
            switch (direction->at<uchar>(i, j)) {
            case 0:
                if (G->at<double>(i, j) <= G->at<double>(i, j - 1) || G->at<double>(i, j) <= G->at<double>(i, j + 1)) edges->at<double>(i, j) = 0;
                break;
            case 45:
                if (G->at<double>(i, j) <= G->at<double>(i - 1, j + 1) || G->at<double>(i, j) <= G->at<double>(i + 1, j - 1)) edges->at<double>(i, j) = 0;
                break;
            case 90:
                if (G->at<double>(i, j) <= G->at<double>(i - 1, j) || G->at<double>(i, j) <= G->at<double>(i + 1, j)) edges->at<double>(i, j) = 0;
                break;
            case 135:
                if (G->at<double>(i, j) <= G->at<double>(i - 1, j - 1) || G->at<double>(i, j) <= G->at<double>(i + 1, j + 1)) edges->at<double>(i, j) = 0;
                break;
            }
            //if (G->at<double>(i, j) <= G->at<double>(i, j - 1) || G->at<double>(i, j) <= G->at<double>(i, j + 1) || G->at<double>(i, j) <= G->at<double>(i - 1, j + 1) || G->at<double>(i, j) <= G->at<double>(i + 1, j - 1) || G->at<double>(i, j) <= G->at<double>(i - 1, j) || G->at<double>(i, j) <= G->at<double>(i + 1, j) || G->at<double>(i, j) <= G->at<double>(i - 1, j - 1) || G->at<double>(i, j) <= G->at<double>(i + 1, j + 1)) G->at<double>(i, j) = 0;
        }
    }
}

void CannyDetection::hysteresis(cv::Mat *edges, cv::Mat *direction) {
    double min = ui->MinSpinBox->value(), max = min * ui->RatioSpinBox->value();
    for (int i = 1; i < edges->rows - 1; i++) {
        for (int j = 1; j < edges->cols - 1; j++) {
            if (edges->at<double>(i, j) > max) edges->at<double>(i, j) = 255;
        }
    }

    for (int i = 1; i < edges->rows - 1; i++) {
        for (int j = 1; j < edges->cols - 1; j++) {
            if (edges->at<double>(i, j) == 255) {
                followEdge(i, j, edges, direction);
            }
        }
    }

    for (int i = 1; i < edges->rows - 1; i++) {
        for (int j = 1; j < edges->cols - 1; j++) {
            if (edges->at<double>(i, j) != 255) edges->at<double>(i, j) = 0;
        }
    }
}

void CannyDetection::followEdge(int i, int j, cv::Mat *G, cv::Mat *direction) {
    double min = ui->MinSpinBox->value();
    if (i != 0 && i != G->rows - 1 && j != 0 && j != G->cols - 1) {
        switch (direction->at<uchar>(i, j)) {
        case 0:
            if (G->at<double>(i, j - 1) > min) {
                G->at<double>(i, j - 1) = 255;
                followEdge(i, j - 1, G, direction);
            }
            if (G->at<double>(i, j + 1) > min) {
                G->at<double>(i, j + 1) = 255;
                followEdge(i, j + 1, G, direction);
            }
            break;
        case 45:
            if (G->at<double>(i - 1, j + 1) > min) {
                G->at<double>(i - 1, j + 1) = 255;
                followEdge(i - 1, j + 1, G, direction);
            }
            if (G->at<double>(i + 1, j - 1) > min) {
                G->at<double>(i + 1, j - 1) = 255;
                followEdge(i + 1, j - 1, G, direction);
            }
            break;
        case 90:
            if (G->at<double>(i - 1, j) > min) {
                G->at<double>(i - 1, j) = 255;
                followEdge(i - 1, j, G, direction);
            }
            if (G->at<double>(i + 1, j) > min) {
                G->at<double>(i + 1, j) = 255;
                followEdge(i + 1, j, G, direction);
            }
            break;
        default:
            if (G->at<double>(i - 1, j - 1) > min) {
                G->at<double>(i - 1, j - 1) = 255;
                followEdge(i - 1, j - 1, G, direction);
            }
            if (G->at<double>(i + 1, j + 1) > min) {
                G->at<double>(i + 1, j + 1) = 255;
                followEdge(i + 1, j + 1, G, direction);
            }
            break;
        }
    }
}

//void CannyDetection::doubleThresholding(cv::Mat *edges) {
//    double min = ui->MinSpinBox->value(), max = min * ui->RatioSpinBox->value();
//    for (int i = 1; i < edges->rows - 1; i++) {
//        for (int j = 1; j < edges->cols - 1; j++) {
//            if (edges->at<double>(i, j) > max) edges->at<double>(i, j) = 255;
//            else if (edges->at<double>(i, j) > min) edges->at<double>(i, j) = 126;
//            else edges->at<double>(i, j) = 0;
//        }
//    }
//}

//void CannyDetection::hysteresis(cv::Mat *edges) {
//    for (int i = 1; i < edges->rows - 1; i++) {
//        for (int j = 1; j < edges->cols - 1; j++) {
//            if (edges->at<double>(i, j) == 126) {
//                int k = -1, l = -1;
//                while (k < 2) {
//                    if (edges->at<double>(i + k, j + l) == 255) {
//                        edges->at<double>(i, j) = 125;
//                        break;
//                    }
//                    if (++l >= 2) {
//                        l = -1;
//                        k++;
//                    }
//                }
//                if (edges->at<double>(i, j) == 126) edges->at<double>(i, j) = 0;
//            }
//        }
//    }
//}

void CannyDetection::saveImage() {
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
