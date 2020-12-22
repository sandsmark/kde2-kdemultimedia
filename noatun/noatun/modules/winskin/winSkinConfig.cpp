#include <qpushbutton.h>
#include <qlabel.h>
#include <qpixmap.h>
#include <noatunpref.h>
#include <kpixmap.h>
#include <kpixmapeffect.h>
#include <kcolordialog.h>
#include <kglobal.h>
#include <kconfig.h>
#include <qslider.h>
#include <qframe.h>
#include <qstringlist.h>
#include <kstddirs.h>
#include <qdir.h>

#include "winSkinConfig.h"

WinSkinConfig::WinSkinConfig(QWidget *parent, GuiSpectrumAnalyser *guiSpectrumAnalyser)
        : CModule(QString("Winskin"), QString("Winskin is a clone of the popular multimedia UI for Windows."), parent)
{
        mGuiSpectrumAnalyser = guiSpectrumAnalyser;
    QLabel *lblVisColors = new QLabel( this, "lblVisColors" );
    lblVisColors->setGeometry( QRect( 10, 10, 110, 16 ) );
    QFont lblVisColors_font(  lblVisColors->font() );
    lblVisColors_font.setPointSize( 16 );
    lblVisColors_font.setBold( TRUE );
    lblVisColors->setFont( lblVisColors_font );
    lblVisColors->setText( tr( "Visualisation:" ) );

    QLabel *lblFirstColor = new QLabel( this, "lblFirstColor" );
    lblFirstColor->setGeometry( QRect( 25, 35, 160, 16 ) );
    QFont lblFirstColor_font(  lblFirstColor->font() );
    lblFirstColor_font.setPointSize( 12 );
    lblFirstColor->setFont( lblFirstColor_font );
    lblFirstColor->setText( tr( "Visualisation scope colors:" ) );

    QPushButton *changeColor2 = new QPushButton( this, "changeColor2" );
    changeColor2->setGeometry( QRect( 115, 160, 110, 25 ) );
    changeColor2->setText( tr( "Change Color ->" ) );

    QLabel *pixlblColors = new QLabel( this, "pixlblColors" );
    pixlblColors->setGeometry( QRect( 230, 60, 16, 125 ) );

    QPushButton *changeColor1 = new QPushButton( this, "changeColor1" );
    changeColor1->setGeometry( QRect( 250, 55, 110, 25 ) );
    changeColor1->setText( tr( "<- Change Color" ) );

    QLabel *TextLabel3 = new QLabel( this, "TextLabel3" );
    TextLabel3->setGeometry( QRect( 25, 195, 205, 16 ) );
    TextLabel3->setText( tr( "Desired Visualisation Refresh Rate:" ) );

    QLabel *TextLabel11 = new QLabel( this, "TextLabel11" );
    TextLabel11->setGeometry( QRect( 175, 285, 315, 30 ) );
    TextLabel11->setText( tr( "<b>Warning:</b> High FPS may occupy high ammounts of CPU cycles. Please monitor this carefully!" ) );
    TextLabel11->setAlignment( int( QLabel::WordBreak | QLabel::AlignVCenter | QLabel::AlignRight ) );

    QLabel *TextLabel8 = new QLabel( this, "TextLabel8" );
    TextLabel8->setGeometry( QRect( 17, 225, 80, 16 ) );
    TextLabel8->setText( tr( "Minimum FPS" ) );

    QFrame *Line2 = new QFrame( this, "Line2" );
    Line2->setGeometry( QRect( 10, 240, 95, 16 ) );
    Line2->setFrameStyle( QFrame::HLine | QFrame::Sunken );

    QLabel *TextLabel9 = new QLabel( this, "TextLabel9" );
    TextLabel9->setGeometry( QRect( 12, 255, 95, 16 ) );
    TextLabel9->setText( tr( "Low CPU Usage" ) );

    QLabel *TextLabel6 = new QLabel( this, "TextLabel6" );
    TextLabel6->setGeometry( QRect( 385, 255, 96, 16 ) );
    TextLabel6->setText( tr( "High CPU Usage" ) );

    QFrame *Line4 = new QFrame( this, "Line4" );
    Line4->setGeometry( QRect( 385, 240, 100, 16 ) );
    Line4->setFrameStyle( QFrame::HLine | QFrame::Sunken );

    QLabel *TextLabel7 = new QLabel( this, "TextLabel7" );
    TextLabel7->setGeometry( QRect( 392, 225, 85, 16 ) );
    TextLabel7->setText( tr( "Maximum FPS" ) );

    sldFPS = new QSlider( this, "sldFPS" );
    sldFPS->setGeometry( QRect( 115, 240, 260, 16 ) );
    sldFPS->setMaxValue( 4 );
    sldFPS->setOrientation( QSlider::Horizontal );
    sldFPS->setTickmarks( QSlider::Right );

    lblDisplayFPS = new QLabel( this, "lblDisplayFPS" );
    lblDisplayFPS->setGeometry( QRect( 150, 255, 185, 16 ) );
    lblDisplayFPS->setText( tr( "40 Frames Per Second" ) );
    lblDisplayFPS->setAlignment( int( QLabel::AlignCenter ) );

    QLabel *TextLabel5 = new QLabel( this, "TextLabel5" );
    TextLabel5->setGeometry( QRect( 170, 330, 146, 16 ) );
    TextLabel5->setText( tr( "Current Skin:" ) );
    TextLabel5->setAlignment( int( QLabel::AlignCenter ) );

    QFrame *Line3 = new QFrame( this, "Line3" );
    Line3->setGeometry( QRect( 5, 315, 480, 16 ) );
    Line3->setFrameStyle( QFrame::HLine | QFrame::Sunken );

    QComboBox *cmbSkins = new QComboBox( FALSE, this, "cmbSkins" );
    cmbSkins->setGeometry( QRect( 145, 350, 206, 21 ) );

    QFrame *Line4_2 = new QFrame( this, "Line4_2" );
    Line4_2->setGeometry( QRect( 5, 380, 480, 16 ) );
    Line4_2->setFrameStyle( QFrame::HLine | QFrame::Sunken );

    QFrame *Line5 = new QFrame( this, "Line5" );
    Line5->setGeometry( QRect( 5, 210, 480, 16 ) );
    Line5->setFrameStyle( QFrame::HLine | QFrame::Sunken );

    QFrame *Line2_5 = new QFrame( this, "Line2" );
    Line2_5->setGeometry( QRect( 5, 240, 100, 16 ) );
    Line2_5->setFrameStyle( QFrame::HLine | QFrame::Sunken );


    QObject::connect(changeColor2, SIGNAL(clicked()), this, SLOT(color2Clicked()));
    QObject::connect(changeColor1, SIGNAL(clicked()), this, SLOT(color1Clicked()));
    QObject::connect(sldFPS, SIGNAL(sliderMoved(int)), this, SLOT(FPSChanged(int)));

    QPixmap configGradient(16, 145);
    configGradient.fill(QColor("white"));
    KPixmap kConfigGradient(configGradient);

        QColor red(255,0,0);
        QColor green(0,255,0);
        KConfig *config = KGlobal::config();
        config->setGroup("Winskin");
        firstColor = config->readColorEntry("ScopeBarColor1", &red);
        secondColor = config->readColorEntry("ScopeBarColor2", &green);
	sldFPS->setValue(config->readNumEntry("ScopeFPS", 2));
	FPSChanged(sldFPS->value());

        kConfigGradient = KPixmapEffect::gradient(kConfigGradient, firstColor, secondColor, KPixmapEffect::VerticalGradient, 4);
        pixlblColors->setPixmap(kConfigGradient);

/*	QStringList skinDir = KGlobal::dirs()->findDirs("data", "noatun/winskin");
	QDir skinQDir;
	QStringList skins = skinQDir.entryList(QDir::Dirs, QDir::Name);
	cmbSkins->insertStringList(skins);
*/
}

void WinSkinConfig::color1Clicked()
{
        int result = KColorDialog::getColor(firstColor);
        if (result == KColorDialog::Accepted)
        {
                QPixmap configGradient(16, 145);
                configGradient.fill(QColor("white"));
                KPixmap kConfigGradient(configGradient);
                kConfigGradient = KPixmapEffect::gradient(kConfigGradient, firstColor, secondColor, KPixmapEffect::VerticalGradient, 4);
                pixlblColors->setPixmap(kConfigGradient);
        }
}

void WinSkinConfig::color2Clicked()
{
        int result = KColorDialog::getColor(secondColor);
        if (result == KColorDialog::Accepted)
        {
                QPixmap configGradient(16, 145);
                configGradient.fill(QColor("white"));
                KPixmap kConfigGradient(configGradient);
                kConfigGradient = KPixmapEffect::gradient(kConfigGradient, firstColor, secondColor, KPixmapEffect::VerticalGradient, 4);
                pixlblColors->setPixmap(kConfigGradient);
        }
}

void WinSkinConfig::save()
{
	KConfig *config = KGlobal::config();
	config->setGroup("Winskin");
	config->writeEntry("ScopeBarColor1", firstColor);
        config->writeEntry("ScopeBarColor2", secondColor);
	config->writeEntry("ScopeFPS", sldFPS->value());
	config->sync();
        mGuiSpectrumAnalyser->reload();
}

void WinSkinConfig::FPSChanged(int value)
{
                if (value == 0)
		{
		        lblDisplayFPS->setText("10 Frames Per Second");
                }
                if (value == 1)
		{
                        lblDisplayFPS->setText("25 Frames Per Second");
		}
		if (value == 2)
		{
                        lblDisplayFPS->setText("40 Frames Per Second");
		}
		if (value == 3)
		{
		        lblDisplayFPS->setText("55 Frames Per Second");
		}
		if (value == 4)
		{
                        lblDisplayFPS->setText("70 Frames Per Second");
		}
}
