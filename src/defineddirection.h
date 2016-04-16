#ifndef DEFINEDDIRECTION_H
#define DEFINEDDIRECTION_H

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <QMainWindow>

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

	private:
		Ui::DefinedDirection *ui;
		cv::Mat img;

		void prepareDirectionComboBox();
};

#endif // DEFINEDDIRECTION_H
