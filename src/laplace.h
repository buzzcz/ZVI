#ifndef LAPLACE_H
#define LAPLACE_H

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <QMainWindow>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>

namespace Ui {
class Laplace;
}

class Laplace : public QMainWindow
{
    Q_OBJECT

public:
    explicit Laplace(cv::Mat *src, QWidget *parent = 0);
    ~Laplace();

public slots:
    void detectEdges();
    void saveImage();

protected:
    void showEvent(QShowEvent *event);
    void resizeEvent(QResizeEvent *event);

private:
    Ui::Laplace *ui;
    cv::Mat img, edges;
    cv::Mat op;

    void prepareGUI();
    void showImage();
};

#endif // LAPLACE_H
