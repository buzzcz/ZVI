#ifndef DEFINEDDIRECTION_H
#define DEFINEDDIRECTION_H

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <QMainWindow>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>

namespace Ui {
class DefinedDirection;
}

class DefinedDirection : public QMainWindow
{
    Q_OBJECT

public:
    explicit DefinedDirection(cv::Mat *src, QWidget *parent = 0);
    ~DefinedDirection();

public slots:
    void detectEdges();
    void saveImage();

protected:
    void showEvent(QShowEvent *event);
    void resizeEvent(QResizeEvent *event);

private:
    Ui::DefinedDirection *ui;
    cv::Mat img, edges;

    void prepareGUI();
    void showImage();
};

#endif // DEFINEDDIRECTION_H
