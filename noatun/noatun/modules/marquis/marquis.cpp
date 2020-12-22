// marquis.cpp
//
// Copyright (C) 2000 Neil Stevens <multivac@fcmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// THE AUTHOR(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
// AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
// 
// Except as contained in this notice, the name(s) of the author(s) shall not be
// used in advertising or otherwise to promote the sale, use or other dealings
// in this Software without prior written authorization from the author(s).

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <kconfig.h>
#include <kdebug.h>
#include "pluginloader.h"
#include "marquis.h"
#include "player.h"

Marquis::Marquis()
	: KMainWindow()
	, SessionManagement()
{
	NOATUNPLUGINC(Marquis);
	hide();
	kdDebug() << "number of members == " << memberList->count() << endl;
}

Marquis::~Marquis()
{
}

void Marquis::restore(void)
{
	KConfig *config = kapp->sessionConfig();
	kdDebug() << "Marquis::restore()" << endl;
	readGlobalProperties(config);
	config->setGroup(QString::fromLatin1("Number"));
	config->writeEntry(QString::fromLatin1("NumberOfWindows"), 1);
}

// unload every window, and save ihe config a QStringList of those loaded
void Marquis::saveGlobalProperties(KConfig *c)
{
	kdDebug() << "Marquis::saveGlobalProperties()" << endl;
	// borrowed from Plugin config dialog
	QStringList specList;
	QValueList<NoatunLibraryInfo> loaded = napp->libraryLoader()->loaded();
	for( QValueList<NoatunLibraryInfo>::Iterator i = loaded.begin(); i != loaded.end(); ++i)
	{
		if(!specList.contains((*i).specfile)
		   && napp->libraryLoader()->isLoaded((*i).specfile)
		   && (*i).specfile != "marquis.plugin")
		{
			specList += (*i).specfile;
			napp->libraryLoader()->remove((*i).specfile);
		}
	}
	c->writeEntry("Loaded Plugins", specList);
	c->writeEntry("Volume", napp->player()->volume());
}

// get the list of loaded plugins from the config, and load them
void Marquis::readGlobalProperties(KConfig *c)
{
	c->setGroup(QString::null);
	QStringList list = c->readListEntry("Loaded Plugins");

	kdDebug() << "Marquis::readGlobalProperties()" << endl;
	for(QStringList::ConstIterator i=list.begin(); i!=list.end(); ++i)
		kdDebug() << *i << endl;
	kdDebug() << "Marquis::readGlobalProperties() there we go" << endl;

	napp->libraryLoader()->loadAll(list);

	napp->player()->setVolume( c->readNumEntry("Volume", 100) );
}

#include "marquis.moc"
