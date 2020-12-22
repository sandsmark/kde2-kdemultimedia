#ifndef MONOSCOPE_H
#define MONOSCOPE_H

#include <plugin.h>

class Monoscope : public QWidget,  public MonoScope, public Plugin
{
Q_OBJECT
NOATUNPLUGIND

public:
	Monoscope();
	virtual ~Monoscope();

	void init();

public slots:
	void toggle(void);

protected:
	virtual void closeEvent(QCloseEvent *);
	virtual void scopeEvent(float *data, int bands);

	virtual void resizeEvent(QResizeEvent *);

private:
	QRgb mHighColor, mLowColor;
	int pluginMenuItem;
};

#endif

