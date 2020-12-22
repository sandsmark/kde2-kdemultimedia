#ifndef PLAYLISTSAVER_H
#define PLAYLISTSAVER_H

#include <ksimpleconfig.h>

class PlaylistSaver : public KSimpleConfig
{
public:
	PlaylistSaver(const QString &file);
	~PlaylistSaver();
	
	QString title();
	void setTitle(const QString &title);
	QDateTime date();
	QString location();
	
	static bool isPlaylist(const QString &file);
	
	QStringList fileList() const;
	
	QString file() const;
	void setFile(const QString &file);
	
	
	
	
private:
	void setGroup(const QString &s) {KSimpleConfig::setGroup(s);}
	QString group() const { return KSimpleConfig::group(); }
	QStringList groupList() const {return KSimpleConfig::groupList();}
	
	static QString &stripFilePre(QString &);
	
	
};

#endif
