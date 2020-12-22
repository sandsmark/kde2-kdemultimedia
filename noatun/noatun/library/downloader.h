#ifndef _DOWNLOADER_H
#define _DOWNLOADER_H
#include <kurl.h>
#include <qobject.h>
#include <qlist.h>
#include <job.h>

class QFile;
class PlaylistItem;
class QTimer;


/**
 * download playlistitems, in a queue based fasion
 **/
class Downloader : public QObject
{
Q_OBJECT
	struct QueueItem
	{
		PlaylistItem *notifier;
		KURL file;
		QString local;
	};

public:
	Downloader(QObject *parent=0);
	virtual ~Downloader();

	QString enqueue(PlaylistItem *notifier, const KURL &file);
	void dequeue(PlaylistItem *notifier);

private:
	void getNext();
	
private slots:
	void data( KIO::Job *, const QByteArray &data);
	void percent( KIO::Job *, unsigned long percent);
	void jobDone( KIO::Job *);
	void giveUpWithThisDownloadServerIsRunningNT();
	
private:
	QList<Downloader::QueueItem> mQueue;
	QFile *localfile;
	Downloader::QueueItem *current;	
	KIO::TransferJob *mJob;
	QTimer *mTimeout;
};

#endif

