#ifndef VIEW_H
#define VIEW_H

#include <qevent.h>
#include <qlist.h>
#include <klistview.h>
#include <ktmainwindow.h>
#include <qrect.h>
#include <kdirlister.h>

class Finder;
class View;
class SafeListViewItem : public QCheckListItem, public PlaylistItem
{
public:
	SafeListViewItem(QListView *parent, QListViewItem *after, const KURL &text);
	virtual ~SafeListViewItem();

protected:
	virtual void downloaded(int percent);
	virtual void modified();
	virtual void stateChange(bool s);
};

class List : public KListView
{
Q_OBJECT
friend class View;
public:
	List(View *parent);
	virtual ~List();
	QListViewItem *openGlobal(const KURL&, QListViewItem * =0);
	QListViewItem *addFile(const KURL&, bool play=false, QListViewItem * =0);
	void addDirectoryRecursive(const KURL &dir, QListViewItem *after= 0);

signals:
	void modified(void);

protected:
	virtual bool acceptDrag(QDropEvent *event) const;
	virtual void viewportPaintEvent(QPaintEvent *);
	
protected slots:
	virtual void dropEvent(QDropEvent *event, QListViewItem *after);
	void move(QList<QListViewItem> &item, QList<QListViewItem> &afterFirst, QList<QListViewItem> &afterNow);

protected:
	QListViewItem *recursiveAddAfter;
	
protected slots:
	// used when adding directories via KIO::listRecursive
	void slotResult(KIO::Job *job);
	void slotEntries(KIO::Job *job, const KIO::UDSEntryList &entries);
	void slotRedirection(KIO::Job *, const KURL & url);

protected:
	void addNextPendingDirectory();
    KURL::List pendingAddDirectories;
	KIO::ListJob *listJob;
	KURL currentJobURL;
};

class KFileDialog;
class KToggleAction;
class KToolBar;

class View : public KMainWindow
{
Q_OBJECT
public:
	View(SplitPlaylist *mother);
	// load the SM playlist
	void init();
	virtual ~View();
	List *listView() const { return list; }
	QListViewItem *addFile(const KURL &u, bool play=false)
		{ return list->addFile(u, play); }


public slots:
	void deleteSelected();
	void addFiles();
	void addDirectory();
	void save();
	void saveAs();
	void open();
	void openNew();
	void clearView() { list->clear(); }
	void setSorting(bool on, int column = 0);
	void setNoSorting() { setSorting(false); }
	void headerClicked(int column);void find();
	void findIt(Finder *);

	
private slots:
	void setModified();
	void saveState();
	
protected:
	void setupActions();
	void saveToURL(const KURL &);
	void setModified(bool);
	virtual void closeEvent(QCloseEvent*e);
	virtual void showEvent(QShowEvent *);
	virtual void hideEvent(QHideEvent *);

signals:
	void hidden();
	void shown();
	
private:
	List *list;
	KAction *mOpen, *mDelete, *mSave, *mSaveAs, *mOpenpl, *mOpenNew;
	KAction *mFind;
	KToolBar *tb;

	KURL mPlaylistFile;
	bool modified;
};

#endif
