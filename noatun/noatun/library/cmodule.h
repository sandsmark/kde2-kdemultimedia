#ifndef CMODULE_H
#define CMODULE_H

#include <qframe.h>
#include <klistview.h>
#include <noatunpref.h>

class KListView;
class QSplitter;
class QListViewItem;
class NoatunLibraryInfo;
class QTextView;
class MimeTypeTree;

namespace Arts {class TraderOffer;}

class Types : public CModule
{
Q_OBJECT
public:
	Types(QObject *parent=0);
	virtual ~Types();
	virtual void save();
	virtual void reopen();
	
	static QString commaVector(Arts::TraderOffer *t, const char *field, bool limit=false);

public slots:
	void selectionChanged(QListViewItem *item); 
	
private:
	KListView *list;
	bool first;
	QTextView *text;
	MimeTypeTree *mimes;
};

class QCheckBox;
class KURLRequester;

class General : public CModule
{
Q_OBJECT
public:
	General(QObject *parent=0);
	virtual void save();

private slots:
	void slotRequesterClicked( KURLRequester * );

private:
	QCheckBox *mAutoPlay, *mLoopList, *mOneInstance, *mRememberPositions,
	          *mClearOnOpen, *mHackUpPlaylist, *mFastVolume;
	KURLRequester *mDlSaver;

};

// I'm too lazy to grep - Neil
#include "pluginmodule.h"

#endif
