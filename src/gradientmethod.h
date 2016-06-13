#ifndef GradientMethod_H
#define GradientMethod_H

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <QMainWindow>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>

namespace Ui {
class GradientMethod;
}

class GradientMethod : public QMainWindow
{
    Q_OBJECT

public:
		explicit GradientMethod(cv::Mat *src, QWidget *parent = 0);
		~GradientMethod();

public slots:
    void detectEdges();
    void saveImage();

protected:
    void showEvent(QShowEvent *event);
    void resizeEvent(QResizeEvent *event);

private:
		Ui::GradientMethod *ui;
    cv::Mat img, edges;

    void prepareGUI();
    void showImage();
};

#endif // GradientMethod_H
