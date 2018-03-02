#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "rpn.h"
#include <vector>
//#include "logicclass.h"
using namespace std;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

signals:
	void UpdateProgress(int);

private slots:
	void on_LoadButton_clicked();

	void on_PlotButton_clicked();

	void on_PauseButton_clicked();

	void on_SaveButton_clicked();

private:
	void ParseAll();

    Ui::MainWindow *ui;

	int OKToPlot;

	RPN X_equation;
	RPN Y_equation;
	RPN Z_equation;
	RPN RangeInput;

	double T_param,U_param,V_param;
	int T_res,U_res,V_res;
	double T_start,U_start,V_start;
	double T_inc,U_inc,V_inc;

	vector< vector< vector<float> > > XPlotData;
	vector< vector< vector<float> > > YPlotData;
	vector< vector< vector<float> > > ZPlotData;

//	LogicClass MyBarControl;
};












#endif // MAINWINDOW_H
