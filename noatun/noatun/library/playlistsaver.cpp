#include "playlistsaver.h"


PlaylistSaver::PlaylistSaver(const QString &file)
	: KSimpleConfig(file)
{
	writeEntry("NoatunPlaylist", true);
	writeEntry("FormatVersion", "0.99");
}

PlaylistSaver::~PlaylistSaver()
{

}

QString PlaylistSaver::title()
{
	setGroup("");
	return readEntry("title");
}

void PlaylistSaver::setTitle(const QString &title)
{
	setGroup("");
	writeEntry("title", title);
}

QDateTime PlaylistSaver::date()
{
	setGroup("");
	return readDateTimeEntry("date");
}

QString PlaylistSaver::location()
{
	setGroup("");
	return readEntry("date");
}

bool PlaylistSaver::isPlaylist(const QString &file)
{
	KSimpleConfig f(file);
	return f.readBoolEntry("NoatunPlaylist");
}

QStringList PlaylistSaver::fileList() const
{
	QStringList list=groupList().grep("File-.*");
	for(QStringList::Iterator it=list.begin(); it != list.end(); ++it )
		stripFilePre(*it);
	return list;	
}

QString &PlaylistSaver::stripFilePre(QString &f)
{
	if (f.right(5)=="File-")
		return f=f.left(f.length()-5);
	return f;
}

QString PlaylistSaver::file() const
{
	QString gr=group();
	return stripFilePre(gr);
}

void PlaylistSaver::setFile(const QString &file)
{
	setGroup("File-"+file);
}
