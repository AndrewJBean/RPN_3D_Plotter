#ifndef GLWIDGET_H
#define GLWIDGET_H

#include <QGLWidget>
#include <QTimer>
#include <QMouseEvent>
#include <QWheelEvent>
#include <QApplication>
#include <vector>
using namespace std;

class GLWidget : public QGLWidget
{
    Q_OBJECT
public:
    explicit GLWidget(QWidget *parent = 0);

    void initializeGL();
    void paintGL();
    void resizeGL(int w,int h);
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
	void wheelEvent(QWheelEvent *event);
	int ReadyToPlot;
	vector< vector< vector< float > > > * XPlotData;
	vector< vector< vector< float > > > * YPlotData;
	vector< vector< vector< float > > > * ZPlotData;
	int InvertPause();

private:
    QTimer timer;
    float TransformMatrix[16];
    QPoint lastPos;
	int TimeIndex;
	int PausedNow;

	int IsGrabMouse;

signals:

public slots:

};

#endif // GLWIDGET_H
