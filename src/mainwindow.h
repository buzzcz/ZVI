#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <QMainWindow>

namespace Ui {
	class MainWindow;
	}

class MainWindow : public QMainWindow
{
		Q_OBJECT

	public:
		explicit MainWindow(QWidget *parent = 0);
		~MainWindow();

	private slots:
		void on_actionExit_triggered();

		void on_actionOpenCVCanny_triggered();

		void on_action_Load_triggered();

		void on_actionDefinedDirection_triggered();

		void on_actionLaplace_triggered();

		void on_actionCanny_triggered();

	private:
		Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
