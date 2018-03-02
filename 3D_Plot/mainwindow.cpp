#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <iostream>
#include <QFile>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
//#include <QProgressDialog>
#include <QObject>






MainWindow::MainWindow(QWidget *parent) :
	QMainWindow(parent),
	ui(new Ui::MainWindow)
{
	ui->setupUi(this);

//	ui->ComputeProgressBar->hide();
//	connect( &MyBarControl, SIGNAL( signalProgress(int) ), ui->ComputeProgressBar, SLOT( setValue(int) ) );

//	connect( this, SIGNAL( UpdateProgress(int) ), ui->ComputeProgressBar, SLOT( setValue(int) ) );
//	QObject::connect( this, &MainWindow::UpdateProgress , ui->ComputeProgressBar, &QProgressBar::setValue );

	ui->widget->ReadyToPlot = 0;
	ui->widget->XPlotData = &XPlotData;
	ui->widget->YPlotData = &YPlotData;
	ui->widget->ZPlotData = &ZPlotData;

	OKToPlot = 0;

	X_equation.NewParameter("t",&T_param);
	X_equation.NewParameter("u",&U_param);
	X_equation.NewParameter("v",&V_param);

	Y_equation.NewParameter("t",&T_param);
	Y_equation.NewParameter("u",&U_param);
	Y_equation.NewParameter("v",&V_param);

	Z_equation.NewParameter("t",&T_param);
	Z_equation.NewParameter("u",&U_param);
	Z_equation.NewParameter("v",&V_param);

//	ParseAll();
}

MainWindow::~MainWindow()
{
	delete ui;
}

void MainWindow::ParseAll()
{
	OKToPlot = 1;
	OKToPlot = OKToPlot && (X_equation.Compile( ui->X_input->text().toLatin1().data() ) ==0);
	if(!OKToPlot) {cout<< "Out1."<<endl;return;}
	OKToPlot = OKToPlot && (Y_equation.Compile( ui->Y_input->text().toLatin1().data() ) ==0);
	if(!OKToPlot) {cout<< "Out2."<<endl;return;}
	OKToPlot = OKToPlot && (Z_equation.Compile( ui->Z_input->text().toLatin1().data() ) ==0);
	if(!OKToPlot) {cout<< "Out3."<<endl;return;}

	OKToPlot = OKToPlot && (RangeInput.Compile( ui->T_Range->text().toLatin1().data() ) ==0);
	if(!OKToPlot) {cout<< "Out4."<<endl;return;}
	RangeInput.ResetStack();
	RangeInput();
	OKToPlot = OKToPlot && RangeInput.GetDepth()==3;
	if(!OKToPlot) {cout<< "Out5."<<endl;return;}
	T_res = (int)RangeInput[0];
	T_inc = RangeInput[1];
	T_start = RangeInput[2];
	T_inc = (T_inc - T_start)/(double)T_res;

	OKToPlot = OKToPlot && (RangeInput.Compile( ui->U_Range->text().toLatin1().data() ) ==0);
	if(!OKToPlot) {cout<< "Out6."<<endl;return;}
	RangeInput.ResetStack();
	RangeInput();
	OKToPlot = OKToPlot && RangeInput.GetDepth()==3;
	if(!OKToPlot) {cout<< "Out7."<<endl;return;}
	U_res = (int)RangeInput[0];
	U_inc = RangeInput[1];
	U_start = RangeInput[2];
	U_inc = (U_inc - U_start)/(double)U_res;

	OKToPlot = OKToPlot && (RangeInput.Compile( ui->V_Range->text().toLatin1().data() ) ==0);
	if(!OKToPlot) {cout<< "Out8."<<endl;return;}
	RangeInput.ResetStack();
	RangeInput();
	OKToPlot = OKToPlot && RangeInput.GetDepth()==3;
	if(!OKToPlot) {cout<< "Out9."<<endl;return;}
	V_res = (int)RangeInput[0];
	V_inc = RangeInput[1];
	V_start = RangeInput[2];
	V_inc = (V_inc - V_start)/(double)V_res;

	ui->widget->ReadyToPlot = 0;

	XPlotData = vector< vector< vector<float> > >(T_res,vector< vector<float> >(U_res+1,vector<float>(V_res+1,0.0f)));
	YPlotData = vector< vector< vector<float> > >(T_res,vector< vector<float> >(U_res+1,vector<float>(V_res+1,0.0f)));
	ZPlotData = vector< vector< vector<float> > >(T_res,vector< vector<float> >(U_res+1,vector<float>(V_res+1,0.0f)));

//	QProgressDialog ComputeProgress("Computing points.", QString(), 0, T_res);
//	ComputeProgress.setAutoClose(true);
//	ComputeProgress.setWindowModality(Qt::WindowModal);
//	ComputeProgress.setValue(0);


//	MyBarControl.theMax = T_res;
//	MyBarControl.theMin = 0;
//	ui->ComputeProgressBar->setRange(MyBarControl.min(),MyBarControl.max());
//	ui->ComputeProgressBar->reset();
//	ui->ComputeProgressBar->setRange(0,T_res);
//	ui->ComputeProgressBar->show();
//	emit UpdateProgress(0);

	T_param = T_start;
	for(int Time=0;Time<T_res;Time++)
	{
		U_param = U_start;
		for(int i=0;i<=U_res;i++)
		{
			V_param = V_start;
			for(int j=0;j<=V_res;j++)
			{
				X_equation.ResetStack();
				OKToPlot = OKToPlot && X_equation()==0;
				OKToPlot = OKToPlot && X_equation.GetDepth()>0;
				if(!OKToPlot) {cout<< "Out9."<<endl;return;}
				XPlotData[Time][i][j] = X_equation[0];

				Y_equation.ResetStack();
				OKToPlot = OKToPlot && Y_equation()==0;
				OKToPlot = OKToPlot && Y_equation.GetDepth()>0;
				if(!OKToPlot) {cout<< "Out9."<<endl;return;}
				YPlotData[Time][i][j] = Y_equation[0];

				Z_equation.ResetStack();
				OKToPlot = OKToPlot && Z_equation()==0;
				OKToPlot = OKToPlot && Z_equation.GetDepth()>0;
				if(!OKToPlot) {cout<< "Out9."<<endl;return;}
				ZPlotData[Time][i][j] = Z_equation[0];

				V_param += V_inc;
			}
			U_param += U_inc;
		}
		T_param += T_inc;

//		ComputeProgress.setValue(Time+1);
//		ui->ComputeProgressBar->setValue(Time+1);
//		emit UpdateProgress(Time+1);
//		MyBarControl.emitValue(Time+1);
	}
//	ComputeProgress.setValue(T_res);
	ui->widget->ReadyToPlot = 1;
//	ui->ComputeProgressBar->reset();
//	ui->ComputeProgressBar->hide();
	cout << "Computed Points." << endl;
}


void MainWindow::on_LoadButton_clicked()
{
	QString fileName = QFileDialog::getOpenFileName(this,
			tr("Open Equation File"), "",
			tr("All Files (*)"));
	if (fileName.isEmpty())
	{
		return;
	}

	QFile file(fileName);

	if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
	{
		QMessageBox::information(this, tr("Unable to open file"),file.errorString());
		return;
	}

	QTextStream in(&file);

	if(in.atEnd()) return;
	QString line = in.readLine();
	ui->X_input->setText(line);

	if(in.atEnd()) return;
	line = in.readLine();
	ui->Y_input->setText(line);

	if(in.atEnd()) return;
	line = in.readLine();
	ui->Z_input->setText(line);

	if(in.atEnd()) return;
	line = in.readLine();
	ui->T_Range->setText(line);

	if(in.atEnd()) return;
	line = in.readLine();
	ui->U_Range->setText(line);

	if(in.atEnd()) return;
	line = in.readLine();
	ui->V_Range->setText(line);
}


void MainWindow::on_PlotButton_clicked()
{
	ParseAll();
}

void MainWindow::on_PauseButton_clicked()
{
	if(ui->widget->InvertPause())
		ui->PauseButton->setText("Restart");
	else
		ui->PauseButton->setText("Pause");
}

void MainWindow::on_SaveButton_clicked()
{
	QString fileName = QFileDialog::getSaveFileName(this,
			tr("Save Equation File."), "",
			tr("All Files (*)"));
	if (fileName.isEmpty())
	{
		return;
	}
	else
	{
		QFile file(fileName);
		if ( !file.open(QIODevice::WriteOnly | QIODevice::Text| QIODevice::Truncate) )
		{
			QMessageBox::information(this, tr("Unable to open file"),
			file.errorString());
			return;
		}

		QTextStream out(&file);
		out << ui->X_input->text() << endl;
		out << ui->Y_input->text() << endl;
		out << ui->Z_input->text() << endl;
		out << ui->T_Range->text() << endl;
		out << ui->U_Range->text() << endl;
		out << ui->V_Range->text() << endl;
	}
}
