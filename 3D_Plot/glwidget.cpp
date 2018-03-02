#include "glwidget.h"
#include <GLUT/glut.h>
#include <QCursor>

GLWidget::GLWidget(QWidget *parent) :
    QGLWidget(parent)
{
    connect(&timer,SIGNAL(timeout()),this,SLOT(updateGL()));
    timer.start(1);
	TimeIndex = 0;
	PausedNow = 0;
	IsGrabMouse = 0;
	setMouseTracking(true);
}


void GLWidget::initializeGL()
{
	glShadeModel(GL_SMOOTH);
	glClearColor(0.0,0.0,0.0,0.0);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );

	glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
	glTranslatef(0.0f,0,-3.0f);
	glGetFloatv(GL_MODELVIEW_MATRIX,TransformMatrix);
}

void GLWidget::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glMatrixMode(GL_MODELVIEW);


    glLoadIdentity();
    glMultMatrixf(TransformMatrix);

	if(ReadyToPlot && (*XPlotData).size()>0 && (*XPlotData)[0].size()>0 && (*XPlotData)[0][0].size()>0 )
	{
		int T_res,U_res,V_res;
		T_res = XPlotData->size();
		U_res = (*XPlotData)[0].size()-1;
		V_res = (*XPlotData)[0][0].size()-1;
		if(TimeIndex >= T_res) TimeIndex=0;
		glBegin(GL_TRIANGLE_STRIP);
		for(int i=0;i<U_res;i++)
		{
			glColor3f((float)i/(float)U_res,0,0.0);
			glVertex3f((*XPlotData)[TimeIndex][i][0],(*YPlotData)[TimeIndex][i][0],(*ZPlotData)[TimeIndex][i][0]);
			for(int j=0;j<=V_res;j++)
			{
				glColor3f((float)(i+1)/(float)(U_res+1)
					,(float)j/(float)(V_res+1),0.0);
				glVertex3f((*XPlotData)[TimeIndex][i][j],(*YPlotData)[TimeIndex][i][j],(*ZPlotData)[TimeIndex][i][j]);
				glColor3f((float)i/(float)(U_res+1)
					,(float)j/(float)(V_res+1),0.0);
				glVertex3f((*XPlotData)[TimeIndex][i+1][j],(*YPlotData)[TimeIndex][i+1][j],(*ZPlotData)[TimeIndex][i+1][j]);
			}
			glColor3f((float)(i+1)/(float)U_res,1.0,0.0);
			glVertex3f((*XPlotData)[TimeIndex][i+1].back(),(*YPlotData)[TimeIndex][i+1].back(),(*ZPlotData)[TimeIndex][i+1].back());
		}
		glEnd();
		if(!PausedNow) TimeIndex++;
	}
	else
		glutWireTeapot(0.6);
}

void GLWidget::resizeGL(int w,int h)
{
    glViewport(0,0,w,h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45,(float)w/h , 0.01 , 100.0 );
}


void GLWidget::mousePressEvent(QMouseEvent *event)
{
	if(IsGrabMouse==0)
	{
		grabMouse();
		QApplication::setOverrideCursor( QCursor( Qt::BlankCursor ) );
		QCursor::setPos(mapToGlobal(QPoint(width() / 2, height() / 2)));
		IsGrabMouse = 1;
	}
	else
	{
		releaseMouse();
		QApplication::restoreOverrideCursor();
		IsGrabMouse = 0;
	}
//	lastPos = event->pos();
	lastPos = QPoint(width() / 2, height() / 2);
}

void GLWidget::mouseMoveEvent(QMouseEvent *event)
{
    int dx = event->x() - lastPos.x();
    int dy = event->y() - lastPos.y();

//	if (event->buttons() & Qt::LeftButton)
	if (IsGrabMouse)
	{
        glMatrixMode(GL_MODELVIEW);

        glLoadIdentity();
        if( int((Qt::ShiftModifier)&(QApplication::keyboardModifiers())) == 0 )
        {
            glTranslatef((float)dx/100.0f,0,(float)dy/100.0f);
        }
        else
        {
            glRotatef((float)dy/5.0f,-1,0,0);
            glRotatef((float)dx/5.0f,0,-1,0);
        }
        glMultMatrixf(TransformMatrix);
        glGetFloatv(GL_MODELVIEW_MATRIX,TransformMatrix);
		QCursor::setPos(mapToGlobal(QPoint(width() / 2, height() / 2)));
	}
}

void GLWidget::wheelEvent(QWheelEvent *event)
{
	int dx = event->pixelDelta().x();
	int dy = event->pixelDelta().y();

//	if (IsGrabMouse)
	{
		glMatrixMode(GL_MODELVIEW);

		glLoadIdentity();
		glRotatef((float)dy/5.0f,-1,0,0);
		glRotatef((float)dx/5.0f,0,-1,0);
		glMultMatrixf(TransformMatrix);
		glGetFloatv(GL_MODELVIEW_MATRIX,TransformMatrix);
	}
}


int GLWidget::InvertPause()
{
	return PausedNow = !PausedNow;
}









