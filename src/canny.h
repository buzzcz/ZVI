#ifndef CANNYDETECTION_H
#define CANNYDETECTION_H

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <QMainWindow>

namespace Ui {
	class CannyDetection;
	}

class CannyDetection : public QMainWindow
{
		Q_OBJECT

	public:
		explicit CannyDetection(cv::Mat *src, QWidget *parent = 0);
		~CannyDetection();

	public slots:
		void detectEdges();

	private:
		Ui::CannyDetection *ui;
		cv::Mat img;

		void getGradientsAndDirections(cv::Mat *Gx, cv::Mat *Gy, cv::Mat *G, cv::Mat *direction);
		void nonMaximumSuppression(cv::Mat *G, cv::Mat *direction);
		void doubleThresholding(cv::Mat *G);
};

#endif // CANNYDETECTION_H
