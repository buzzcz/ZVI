#ifndef LAPLACE_H
#define LAPLACE_H

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <QMainWindow>

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

	private:
		Ui::Laplace *ui;
		cv::Mat img;
		cv::Mat op;

		void prepareRadioButtons();
};

#endif // LAPLACE_H
