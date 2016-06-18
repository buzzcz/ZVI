#ifndef PointAndLine_H
#define PointAndLine_H

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <QMainWindow>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>

namespace Ui {
class PointAndLine;
}

class PointAndLine : public QMainWindow
{
    Q_OBJECT

public:
		explicit PointAndLine(cv::Mat *src, QWidget *parent = 0);
		~PointAndLine();

public slots:
    void detectEdges();
    void saveImage();

protected:
    void showEvent(QShowEvent *event);
    void resizeEvent(QResizeEvent *event);

private:
		Ui::PointAndLine *ui;
    cv::Mat img, edges;

    void prepareGUI();
    void showImage();
};

#endif // PointAndLine_H
