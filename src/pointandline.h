#ifndef POINTANDLINE_H
#define POINTANDLINE_H

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <QMainWindow>

namespace Ui {
class PointAndLine;
}

class PointAndLine : public QMainWindow
{
	Q_OBJECT

public:
	/*
		Constructor of this class.
		src - pointer to the given original image
	*/
	explicit PointAndLine(cv::Mat *src, QWidget *parent = 0);
	~PointAndLine();

private:
	Ui::PointAndLine *ui;
	/* Original image */
	cv::Mat img;
	/* Convolution matrix for the detection method */
	cv::Mat op;

public slots:

	/*
		Launches the edge detection in given image.
		Detects line/point given to the selected radio button.
		Takes direction from the combo box if line detection selected
	*/
	void process();
};

#endif // POINTANDLINE_H
