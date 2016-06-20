#ifndef MarrHildreth_H
#define MarrHildreth_H

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <QMainWindow>
#include <QDir>
#include <QFileDialog>
#include <QMessageBox>

namespace Ui {
class MarrHildreth;
}

class MarrHildreth : public QMainWindow
{
		Q_OBJECT

public:
		explicit MarrHildreth(cv::Mat *src, QWidget *parent = 0);
		~MarrHildreth();

public slots:
		void detectEdges();
		void saveImage();

protected:
		void showEvent(QShowEvent *event);
		void resizeEvent(QResizeEvent *event);

private:
		Ui::MarrHildreth *ui;
		cv::Mat img, edges;

		void prepareGUI();
		void showImage();
};

#endif // GradientMethod_H
