#ifndef __PORTABLEKDE_H__
#define __PORTABLEKDE_H__

#include "config.h"

#include <qwidget.h>
#include <qpopupmenu.h>
#include <kconfig.h>
#include <klocale.h>
#include <qcolor.h>
#include <qpen.h>
#include <list>
#include <string>

/* remaining from the KDE1<->KDE2 portability thing and should disappear */

class PortableKDE {
public:
	/*
	static QPopupMenu* helpMenu(bool bAboutQtMenu, const QString& appAboutText);
	*/
	static KConfig* config();
	static QString caption();
	static void KMsgSorry(QWidget *parent, const char *text);
	static std::list<std::string> globalDirs(const char *which);
};

#define KAppNameType QCString

#endif /* __PORTABLEKDE_H__ */
