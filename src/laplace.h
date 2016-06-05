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
        cv::Mat detectEdges();
        void saveImage();

	private:
		Ui::Laplace *ui;
		cv::Mat img;
		cv::Mat op;

        void prepareGUI();
};

#endif // LAPLACE_H
