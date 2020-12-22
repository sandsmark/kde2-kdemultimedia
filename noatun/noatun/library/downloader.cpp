#include "downloader.h"
#include "noatunapp.h"
#include <qfile.h>
#include "noatunplaylist.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <qtimer.h>

Downloader::Downloader(QObject *parent)
	: QObject(parent), localfile(0), current(0), mJob(0), mTimeout(0)
{
}

Downloader::~Downloader()
{

}

static QString nonExistantFile(const QString &file)
{
	int i=0;
	QString f(file);
	while (QFile(f).exists())
	{
		i++;
		f=file;
		f.insert(f.findRev('.'), '_'+QString::number(i));
	}
	return f;	
}

QString Downloader::enqueue(PlaylistItem *notifier, const KURL &file)
{
	if (file.isLocalFile()) return 0;
	QueueItem *i=new QueueItem;
	i->notifier=notifier;
	i->file=file;
	i->local=nonExistantFile(napp->saveDirectory()+'/'+file.fileName());
			
	mQueue.append(i);
	getNext();
	return i->local;
}

void Downloader::dequeue(PlaylistItem *notifier)
{
	if (current && notifier==current->notifier)
	{
		mJob->kill();
		jobDone(mJob);
		return;
	}
	for (QListIterator<Downloader::QueueItem> i(mQueue); i.current(); ++i)
		if ((*i)->notifier==notifier)
		{
			mQueue.removeRef(*i);
			delete *i;
		}
}

void Downloader::getNext()
{
	if (current) return;

	current=mQueue.take(0);
	if (!current)
		return;
		
	// open the QFile
	localfile=new QFile(current->local);
	localfile->open(IO_ReadWrite | IO_Append);
	
	mJob= KIO::get(current->file, true, false);
	connect(mJob, SIGNAL(data(KIO::Job*, const QByteArray&)), SLOT(data(KIO::Job*, const QByteArray&)));
		
	connect(mJob, SIGNAL(percent(KIO::Job*, unsigned long)), SLOT(percent(KIO::Job*, unsigned long)));

	if (mTimeout)
		delete mTimeout;
	mTimeout=new QTimer(this);
	mTimeout->start(30000, true);
	connect(mTimeout, SIGNAL(timeout()), SLOT(giveUpWithThisDownloadServerIsRunningNT()));
}

void Downloader::data(KIO::Job *j, const QByteArray &data)
{
	if (!data.size())
	{ // file is done downloading
		jobDone(j);
		delete mTimeout;
		mTimeout=0;
		return;
	}
	localfile->writeBlock(data);
	localfile->flush();
	delete mTimeout;
	mTimeout=0;
}

void Downloader::jobDone(KIO::Job *)
{
	current->notifier->doneDownloading();
	delete current;
	current=0;
	mJob=0;
	getNext();
}

void Downloader::giveUpWithThisDownloadServerIsRunningNT()
{
	delete mTimeout;
	mTimeout=0;
	PlaylistItem *old=current->notifier;
	dequeue(current->notifier);
	old->timeout();
}

void Downloader::percent( KIO::Job *, unsigned long percent)
{
	if (current && current->notifier)
		current->notifier->downloaded((int)percent);
}

#include "downloader.moc"

