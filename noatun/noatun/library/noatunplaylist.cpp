#include "noatunplaylist.h"
#include "noatunapp.h"
#include "player.h"
#include "downloader.h"
#include <kcmdlineargs.h>
#include <kfile.h>
#include "pluginloader.h"
#include <kmimetype.h>

PlaylistItem::PlaylistItem(const KURL &u, bool dl)
	: mLength(-1), mProperties(7)
{
		
	setUrl(u, dl);
	mDownloaded= u.isLocalFile();

	QString f(u.fileName());

	//format the filename
	if(napp->hackUpPlaylist())
	{
		int pos=f.findRev('.'); // strip the extension
		f=f.left(pos);

		f=f.replace(QRegExp("_"), " "); //replace underscores with spaces

		// capitalize the first of every word
		f[0]=f[0].upper();

		for (unsigned int pos=1; pos< f.length(); pos++)
		{
			if (f[pos]==' ')
				if (pos+1<f.length())
					f[pos+1]=f[pos+1].upper();
		}
	}
	setProperty("title", f, false);
	napp->playlist()->added(this);
	mProperties.setAutoDelete(true);
}

PlaylistItem::~PlaylistItem()
{
	napp->downloader()->dequeue(this);
	napp->playlist()->deleted(this);
	
	// don't delete me if we need to access me
	// well, copy me and set a delete after use flag ;)
	if (napp->player()->mCurrent==this && !napp->player()->mDeleteMe)
	{
		napp->player()->mDeleteMe=true;
		// screw a copy constructor;
		PlaylistItem *i;
		napp->player()->mCurrent=i=new PlaylistItem(*this, false);
		i->setProperty("title", property("title"), false);
		i->mUrl=mUrl;
		i->mFile=mFile;
		i->mDownloaded=mDownloaded;
		i->mLength=mLength;
	}
}

QString PlaylistItem::title() const
{
	return property("title", mUrl.fileName());
}

void PlaylistItem::setTitle(const QString &t)
{
	setProperty("title", t);
}

KURL PlaylistItem::url() const
{
	return mUrl;
}

void PlaylistItem::setUrl(const KURL &u, bool dl)
{
	mUrl.setPath(u.path());

	if (u.isMalformed()) dl=false;
	else if (u.isLocalFile())
	{
		dl=false;
		mDownloaded=true;
		mFile=u.path();
	}
	if (dl)
	{
		mFile=napp->downloader()->enqueue(this, u);
		mUrl.setPath(mFile);
	}
	setTitle(u.fileName());
}

void PlaylistItem::setLength(int msecs)
{
	if (mLength != msecs)
	{
		mLength=msecs;
		modified();
	}
}

QString PlaylistItem::file() const
{
	return mFile;
}

void PlaylistItem::doneDownloading()
{
	mDownloaded=true;
	modified();
}

bool PlaylistItem::isDownloaded() const
{
	return mDownloaded;
}

bool PlaylistItem::playable() const
{
	return QFile(mFile).size()!=0;
}

void PlaylistItem::downloaded(int)
{
}

QString PlaylistItem::property(const QString &key, const QString &def) const
{
	QString *v=mProperties[key];
	if (!v)
		return def;
	return *v;
}

void PlaylistItem::setProperty(const QString &key, const QString &property, bool c)
{
	QString oldProperty;
	if(mProperties[key]) oldProperty = *mProperties[key];

	mProperties.remove(key);
	mProperties.insert(key, new QString(property));

	if (c && oldProperty != property)
		modified();
}

void PlaylistItem::clearProperty(const QString &key, bool c)
{
	mProperties.remove(key);

	if (c)
		modified();
}

QStringList PlaylistItem::properties() const
{
	QStringList props;
	for (QDictIterator<QString> i(mProperties); i.current(); ++i)
	{
		QString key=i.currentKey();
		props+=key;
	}
	return props;
}

bool PlaylistItem::isProperty(const QString &key) const
{
	return (bool)mProperties[key];
}

void PlaylistItem::modified()
{}

void PlaylistItem::timeout()
{
	delete this;
}

QString PlaylistItem::lengthString() const
{
	if ( length() == -1 ) // no file loaded
		return QString("00:-1");

	int secs = length()/1000; // convert milliseconds -> seconds
	int seconds = secs % 60;
	return QString().sprintf("%.2d:%.2d", ((secs-seconds)/60), seconds);
}

QCString PlaylistItem::mimetype() const
{
	KMimeType::Ptr mimetype = KMimeType::findByURL(mUrl);

	return mimetype->name().latin1();
}

QStrList PlaylistItem::extension() const
{
	QStrList extensions;
	KMimeType::Ptr mimetype = KMimeType::findByURL(mUrl);

	const QStringList &patterns = mimetype->patterns();
	QString extension;
	for(QStringList::ConstIterator i = patterns.begin(); i != patterns.end(); ++i)
	{
		int pos = (*i).find(".");
		if( pos != -1 )
			extensions.append((*i).right( (*i).length() - pos - 1 ).latin1());
	}
	return extensions;
}

Playlist::Playlist(QObject *parent, const char *name) : QObject(parent, name)
{
	napp->player()->connect(this, SIGNAL(playCurrent()), SLOT(playCurrent()));
	napp->player()->connect(this, SIGNAL(listHidden()), SIGNAL(playlistHidden()));
	napp->player()->connect(this, SIGNAL(listShown()), SIGNAL(playlistShown()));
	napp->player()->connect(this, SIGNAL(newCurrent()), SLOT(newCurrent()));
}

void Playlist::toggleList()
{
	if (listVisible())
		hideList();
	else
		showList();
}

int Playlist::handleArguments()
{
	KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
	int i;
	bool play= napp->autoPlay();

	for (i=0; i < args->count(); i++)
	{
		KURL u(args->url(i));
		if (!u.isMalformed())
			addFile(u, play);
		play=false;
	}
	args->clear();
	return i;
}

void Playlist::clearProperties(const QString &key, bool sendchanged)
{
	for (PlaylistItem *i=getFirst(); i!=0; i=getAfter(i))
	{
		i->clearProperty(key, sendchanged);
	}
}

void Playlist::deleted(PlaylistItem *item)
{
	emit removed(item);
}

#include "noatunplaylist.moc"

