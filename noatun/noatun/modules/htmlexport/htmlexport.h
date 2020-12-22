
#ifndef _HTMLEXPORT_H_
#define _HTMLEXPORT_H_

#include <qfile.h>
#include <qtextstream.h>
#include <qlabel.h>
#include <qhgroupbox.h>
#include <qlineedit.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qtooltip.h>
#include <qwhatsthis.h>

#include <klocale.h>
#include <kpopupmenu.h>
#include <kfiledialog.h>
#include <ktempfile.h>
#include <kcolorbutton.h>
#include <kio/netaccess.h>
#include <kconfig.h>
#include <kurlrequester.h>

//#include <kdebug.h>

#include <noatunapp.h>
#include <noatunplaylist.h>
#include <noatunpref.h>

#include <plugin.h>

class HTMLExport : public QObject, public Plugin
{
Q_OBJECT
NOATUNPLUGIND
public:
	HTMLExport();
	~HTMLExport();


private:
	QString getColorByEntry(QString s);
	KConfig *config;
	int menuID;

private slots:
	void slotExport();

};

class Prefs : public CModule
{
Q_OBJECT
public:
	Prefs(QObject *parent);
	virtual void save();

	QGroupBox* colorBox;

	KColorButton* lnkColorSelect;
	KColorButton* hoverColorSelect;
	KColorButton* bgColorSelect;
	KColorButton* txtColorSelect;

	QLabel* bgColorLabel;
	QLabel* txtColorLabel;
	QLabel* lnkColorLabel;
	QLabel* hoverColorLabel;

	QGroupBox* bgPicBox;
	KURLRequester* bgPicPath;
	QCheckBox* useCSS;
	QCheckBox* useHover;

protected:
	QGridLayout* bgcolorLabel;

public slots:

signals:
	void saved();

private:

};

#endif

