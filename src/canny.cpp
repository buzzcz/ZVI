#include "canny.h"
#include "ui_canny.h"
#include <iostream>

CannyDetection::CannyDetection(cv::Mat *src, QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::CannyDetection)
{
    ui->setupUi(this);
    connect(ui->MinSpinBox, SIGNAL(valueChanged(double)), this, SLOT(detectEdges()));
    connect(ui->RatioSpinBox, SIGNAL(valueChanged(int)), this, SLOT(detectEdges()));

    cv::cvtColor((*src), img, CV_BGR2GRAY);
    detectEdges();
}

CannyDetection::~CannyDetection()
{
    delete ui;
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

    nonMaximumSuppression(&G, &direction);

    hysteresis(&G, &direction);

//    doubleThresholding(&G);
//    hysteresis(&G);

    cv::convertScaleAbs(G, G);
    QPixmap p = QPixmap::fromImage(QImage((unsigned char*) G.data, G.cols, G.rows, G.step, QImage::Format_Indexed8));
    ui->label->setPixmap(p);
    ui->label->setFixedSize(p.size());
    ui->centralwidget->setFixedSize(p.size());
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

void CannyDetection::nonMaximumSuppression(cv::Mat *G, cv::Mat *direction) {
    for (int i = 1; i < G->rows - 1; i++) {
        for (int j = 1; j < G->cols - 1; j++) {
            switch (direction->at<uchar>(i, j)) {
            case 0:
                if (G->at<double>(i, j) <= G->at<double>(i, j - 1) || G->at<double>(i, j) <= G->at<double>(i, j + 1)) G->at<double>(i, j) = 0;
                break;
            case 45:
                if (G->at<double>(i, j) <= G->at<double>(i - 1, j + 1) || G->at<double>(i, j) <= G->at<double>(i + 1, j - 1)) G->at<double>(i, j) = 0;
                break;
            case 90:
                if (G->at<double>(i, j) <= G->at<double>(i - 1, j) || G->at<double>(i, j) <= G->at<double>(i + 1, j)) G->at<double>(i, j) = 0;
                break;
            default:
                if (G->at<double>(i, j) <= G->at<double>(i - 1, j - 1) || G->at<double>(i, j) <= G->at<double>(i + 1, j + 1)) G->at<double>(i, j) = 0;
                break;
            }
        }
    }
}

void CannyDetection::hysteresis(cv::Mat *G, cv::Mat *direction) {
    double min = ui->MinSpinBox->value(), max = min * ui->RatioSpinBox->value();
    for (int i = 1; i < G->rows - 1; i++) {
        for (int j = 1; j < G->cols - 1; j++) {
            if (G->at<double>(i, j) > max) G->at<double>(i, j) = 255;
        }
    }

    for (int i = 1; i < G->rows - 1; i++) {
        for (int j = 1; j < G->cols - 1; j++) {
            if (G->at<double>(i, j) == 255) {
                followEdge(i, j, G, direction);
            }
        }
    }

    for (int i = 1; i < G->rows - 1; i++) {
        for (int j = 1; j < G->cols - 1; j++) {
            if (G->at<double>(i, j) != 255) G->at<double>(i, j) = 0;
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

//void CannyDetection::doubleThresholding(cv::Mat *G) {
//    double min = ui->MinSpinBox->value(), max = min * ui->RatioSpinBox->value();
//    for (int i = 1; i < G->rows - 1; i++) {
//        for (int j = 1; j < G->cols - 1; j++) {
//            if (G->at<double>(i, j) > max) G->at<double>(i, j) = 255;
//            else if (G->at<double>(i, j) > min) G->at<double>(i, j) = 126;
//            else G->at<double>(i, j) = 0;
//        }
//    }
//}

//void CannyDetection::hysteresis(cv::Mat *G) {
//    for (int i = 1; i < G->rows - 1; i++) {
//        for (int j = 1; j < G->cols - 1; j++) {
//            if (G->at<double>(i, j) == 126) {
//                int k = -1, l = -1;
//                while (k < 2) {
//                    if (G->at<double>(i + k, j + l) == 255) {
//                        G->at<double>(i, j) = 125;
//                        break;
//                    }
//                    if (++l >= 2) {
//                        l = -1;
//                        k++;
//                    }
//                }
//                if (G->at<double>(i, j) == 126) G->at<double>(i, j) = 0;
//            }
//        }
//    }
//}
