#include <qfile.h>
#include <kglobal.h>
#include <qdir.h>
#include <ksimpleconfig.h>
#include <kstddirs.h>
#include <knotifyclient.h>
#include <klocale.h>
#include <kurl.h>
#include <pluginloader.h>
#include <plugin.h>
#include <kdebug.h>

bool operator ==(const NoatunLibraryInfo &a, const NoatunLibraryInfo &b)
{
	// Feels like cheating, doesn't it?
	return a.specfile == b.specfile;
}

LibraryLoader::LibraryLoader() : mPlaylist(0)
{
}

LibraryLoader::~LibraryLoader()
{
	QValueList<NoatunLibraryInfo> l;

	l = loaded();
	for(QValueList<NoatunLibraryInfo>::Iterator i = l.begin(); i != l.end(); ++i)
	{
		if((*i).type != "userinterface" && (*i).type != "playlist" && (*i).type != "systray")
		{
			removeNow((*i).specfile);
		}
	}
	
	l = loaded();
	for(QValueList<NoatunLibraryInfo>::Iterator i = l.begin(); i != l.end(); ++i)
	{
		if((*i).type == "userinterface")
		{
			removeNow((*i).specfile);
		}
	}
	
	l = loaded();
	for(QValueList<NoatunLibraryInfo>::Iterator i = l.begin(); i != l.end(); ++i)
	{
		removeNow((*i).specfile);
	}
}

QValueList<NoatunLibraryInfo> LibraryLoader::available() const
{
	QValueList<NoatunLibraryInfo> items;
	QStringList files=KGlobal::dirs()->findAllResources("appdata", "*.plugin", false, true);
	for (QStringList::Iterator i=files.begin(); i!=files.end(); ++i)
		items.append(getInfo(*i));

	return items;
}

QList<Plugin> LibraryLoader::plugins() const
{
	QList<Plugin> list;
	for (QDictIterator<LibraryLoader::PluginLibrary> i(mLibHash); i.current(); ++i)
		list.append(i.current()->plugin);
	return list;
}

bool LibraryLoader::loadAll(void)
{
	KConfig *config=KGlobal::config();
	config->setGroup("");
	QStringList modules = config->readListEntry("Modules");
	return loadAll(modules);
}

bool LibraryLoader::loadAll(const QStringList &modules)
{
	// Session management...
	for(QStringList::ConstIterator i=modules.begin(); i!=modules.end(); ++i)
	{
		NoatunLibraryInfo info=getInfo(*i);
		if (!info.type.contains("sm"))
			continue;

		loadSO(*i);
	}

	// load all the playlists in the first
	for(QStringList::ConstIterator i=modules.begin(); i!=modules.end(); ++i)
	{
		NoatunLibraryInfo info=getInfo(*i);
		if (!info.type.contains("playlist"))
			continue;

		loadSO(*i);
	}

	// load all the user interfaces now
	for(QStringList::ConstIterator i=modules.begin(); i!=modules.end(); ++i)
	{
		NoatunLibraryInfo info=getInfo(*i);
		if (!info.type.contains("userinterface"))
			continue;
		loadSO(*i);
	}

	for(QStringList::ConstIterator i=modules.begin(); i!=modules.end(); ++i)
	{
		NoatunLibraryInfo info=getInfo(*i);
		if((!info.type.contains("playlist")) 
		&& (!info.type.contains("userinterface"))
		&& (!info.type.contains("sm")))
		{
		    loadSO(*i);
		}
	}
	return true;
}

NoatunLibraryInfo LibraryLoader::getInfo(const QString &spec) const
{
	NoatunLibraryInfo info;
	QString specPath = (spec[0]=='/') ? spec : KGlobal::dirs()->findResource("appdata", spec);
	if (!QFile::exists(specPath))
		return info;
	KSimpleConfig file(specPath);
	if (spec.find('/')>=0)
		info.specfile=KURL(spec).fileName();
	else
		info.specfile=spec;
	info.filename=file.readEntry("Filename");
	info.author=file.readEntry("Author");
	info.site=file.readEntry("Site");
	info.email=file.readEntry("Email");
	info.type=file.readEntry("Type");
	info.name=file.readEntry("Name");
	info.comment=file.readEntry("Comment");
	info.require=file.readListEntry("Require");
	info.license=file.readEntry("License");
	return info;
}

bool LibraryLoader::isLoaded(const QString &spec) const
{
	PluginLibrary *lib=mLibHash[spec];
	if (!lib) return false;
	return lib->plugin;
}

bool LibraryLoader::loadSO(const QString &spec)
{
	if(!isLoaded(spec))
	{
		NoatunLibraryInfo info = getInfo(spec);
		if (info.specfile != spec)
			return false;
	
		for (QStringList::ConstIterator it = info.require.begin(); it != info.require.end(); ++it)
			loadSO(*it);
	
		// get the library loader instance
		KLibLoader *loader = KLibLoader::self();

		PluginLibrary *listitem=mLibHash[spec];
	
		if (!listitem)
		{
			KLibrary *lib = loader->library(QFile::encodeName(info.filename));
			if (!lib)
				return false;
			listitem=new PluginLibrary;
			listitem->library=lib;
			mLibHash.insert(spec, listitem);
		}

		void *create=listitem->library->symbol("create_plugin");
		if (!create)
			return false;

		Plugin* (*plugInStart)();
		plugInStart= (Plugin* (*)()) create;
		listitem->plugin=plugInStart();

		if (getInfo(spec).type=="playlist")
			mPlaylist=listitem->plugin->playlist();
		listitem->plugin->init();
	
		return true;
	}
	else
		return false;
}

void LibraryLoader::add(const QString &spec)
{
	PluginLibrary *lib=mLibHash[spec];
	if (lib)
		if (lib->plugin) return;

	loadSO(spec);
}

void LibraryLoader::setModules(const QStringList &mods)
{
	KConfig *config=KGlobal::config();
	config->setGroup(0);
	config->writeEntry("Modules", mods);
	KGlobal::config()->sync();
}

bool LibraryLoader::remove(const QString &spec)
{
	removeNow(spec);

	// exit if this is the last UI
	if (getInfo(spec).type=="userinterface")
	{
		QValueList<NoatunLibraryInfo> l=loaded();
		bool isanotherui=false;
		for (QValueList<NoatunLibraryInfo>::Iterator i=l.begin(); i!=l.end(); ++i)
		{
			if ((*i).specfile!=spec && (*i).type=="userinterface")
				isanotherui=true;
		}		
		if (!isanotherui)
			kapp->exit();
	}

	return true;
}

bool LibraryLoader::remove(const PluginLibrary *pl)
{
	for (QDictIterator<PluginLibrary> i(mLibHash); i.current(); ++i)
	{
		if (i.current()==pl)
			return remove(i.currentKey());
	}
	return false;
}

bool LibraryLoader::remove(const Plugin *plugin)
{
	for (QDictIterator<PluginLibrary> i(mLibHash); i.current(); ++i)
	{
		if (i.current()->plugin==plugin)
			return remove(i.currentKey());
	}
	return false;

}

Playlist *LibraryLoader::playlist() const
{
        return mPlaylist;
}

QValueList<NoatunLibraryInfo> LibraryLoader::loaded() const
{
	QValueList<NoatunLibraryInfo> items;

	for (QDictIterator<PluginLibrary> i(mLibHash); i.current(); ++i)
		if (isLoaded(i.currentKey()))
			items.append(getInfo(i.currentKey()));

	return items;
}

void LibraryLoader::removeNow(const QString &spec)
{
	NoatunLibraryInfo info = getInfo(spec);
	if (info.specfile == spec)
	{
		QValueList<NoatunLibraryInfo> l = loaded();
		for (QValueList<NoatunLibraryInfo>::Iterator i = l.begin(); i != l.end(); ++i)
		{
			for (QStringList::ConstIterator it = (*i).require.begin(); it != (*i).require.end(); ++it)
				if (*it == spec)
				removeNow((*i).specfile);
		}
	}
	
	PluginLibrary *lib=mLibHash[spec];

	if (!lib) return;

	delete lib->plugin;
	lib->plugin=0;
	mLibHash.remove(spec);
//	delete lib->library;
//	delete lib;
}


