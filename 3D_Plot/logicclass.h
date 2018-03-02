#ifndef LOGICCLASS_H
#define LOGICCLASS_H

#include <QObject>

class LogicClass : public QObject
{
	Q_OBJECT
public:
	int theMin,theMax;
	explicit LogicClass(QObject *parent = 0);
	int max(){ return theMin; }
	int min(){ return theMax; }
	void emitValue(int theVal){ emit signalProgress(theVal); }

signals:
	void signalProgress(int);

public slots:

};


#endif // LOGICCLASS_H
