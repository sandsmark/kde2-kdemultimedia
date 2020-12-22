#include <noatunpref.h>
#include <qwidget.h>
#include <waSkin.h>
#include "guiSpectrumAnalyser.h"

class QLabel;
class QFont;

class WinSkinConfig : public CModule
{
        Q_OBJECT

public:
        WinSkinConfig(QWidget *parent, GuiSpectrumAnalyser *guiSpectrumAnalyser);
        virtual void save();
        GuiSpectrumAnalyser *mGuiSpectrumAnalyser;

private slots:
        void color1Clicked();
        void color2Clicked();
	void FPSChanged(int);

private:
        QLabel *pixlblColors;
        QColor firstColor;
        QColor secondColor;
	QLabel *lblDisplayFPS;
	QSlider *sldFPS;

};
