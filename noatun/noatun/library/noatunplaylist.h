#ifndef NOATUNPLAYLIST_H
#define NOATUNPLAYLIST_H

#include <qobject.h>
#include <kurl.h>
#include <qdict.h>

/**
 * Do not, under any circumstances, call a Playlist method
 * when you can call a Player method, unless, of course, you
 * ARE the playlist.
 **/
class PlaylistItem
{
	friend class Downloader;
public:
	/**
	 * if dl is true, then download this ASAP
	 * dl implicitly becomes false if u==0 or
	 * it's a local file
	 **/
	PlaylistItem(const KURL &u=0, bool dl=true);
	virtual ~PlaylistItem();

	/**
	 * title of the song
	 * it may just be the ID3 tag, but I can't guarantee it
	 **/
	QString title() const;
	/**
	 * set the title of the song (ID3 loaders do this)
	 * also, note that that rather then overridering
	 * this, you should prefer modified()
	 **/
	virtual void setTitle(const QString &t);
	
	/**
	 * return the location as local, or null if 
	 * file hasn't yet started downloading
	 **/
	QString file() const;

	/**
	 * is the file completely downloaded?
	 * if so, file() will return the name
	 **/
	bool isDownloaded() const;
	/**
	 * if false, don't try to play the file
	 * if true, go ahead, but it may not
	 * be complete
	 **/
	bool playable() const;
	
	KURL url() const;
	virtual void setUrl(const KURL &u, bool dl=true);
	
	/**
	 * length as a string
	 * The format is cc:cc/ll:ll where c is current
	 * and l is length
	 **/
	QString lengthString() const;

	/**
	 * If you can identify the length of this file
	 * set it, in milliseconds.  The engine will do it
	 * itself.
	 *
	 * If unknown, the length will be -1
	 **/
	virtual void setLength(int msec);

	/**
	 * length of the song in seconds, or -1 if unknown
	 **/
	int length() const { return mLength; }

	/**
	 * retursn the mimetype for this file
	 * (potentially slow)
	 **/
	QCString mimetype() const;

	/**
	 * returns the mimetype's associated extentions
	 **/
	QStrList extension() const;

	/**
	 * exactly the same as file()
	 **/
	operator QString() const { return file(); }
	
	/**
	 * return the net-location
	 **/
	operator KURL() const { return url(); }

	/**
	 * Noatun stores file properties in the form
	 * of Atoms.  To get an atom, give a name for it,
	 * and it'l return the value, or @p def
	 * if it's not set.
	 * 
	 * To implement an atom, the keys can't conflict
	 * so, here's the format:
	 * PluginName:AtomName
	 * 
	 * The core doesn't need the "PluginName" part.
	 *
	 * Internally, we use the following <b>case sensitive</b> keys, so far:
	 *
	 * title, artist, album, date, comment
	 * 
	 * If you, the Noatun Plugin Developer wants
	 * to add a new atom, that in some way or another
	 * should be standard (Like fps for Frames/Second). <b>Contact
	 * us, or Else!</b>
	 *
	 * A Key name that ends with an underscore is not visible
	 * to the user, and the value can be generic crap encoding in
	 * any weird format
	 **/
	QString property(const QString &key, const QString &def=0) const;

	/**
	 * Set the said property for the given key
	 * Clearing it first if it already exists.
	 *
	 * Send changed can be left alone
	 **/
	void setProperty(const QString &key, const QString &property, bool sendchanged=true);

	/**
	 * Clear the property for the given key in this item
	 **/
	void clearProperty(const QString &key, bool sendchanged=false);

	/**
	 * Returns a list of all properties. the best playlist will be
	 * able to store them all between noatun sessions
	 **/
	QStringList properties() const;

	/**
	 * returns whether the given key exists as a property
	 **/
	bool isProperty(const QString &key) const;
	
protected: // methods accessed by Downloader (friend)
	/**
	 * this file is done downloading
	 **/
	virtual void doneDownloading();
	
	/**
	 * called every so often when the percent of download
	 * changes.
	 **/
	virtual void downloaded(int percent);

	/**
	 * called when the title or  length, or when
	 * the file is done downloading
	 **/
	virtual void modified();

	/**
	 * download timed out, by default, delete this
	 **/
	virtual void timeout();
	
protected:
	KURL mUrl;
	QString mFile;
	bool mDownloaded;
	int mLength;
	QDict<QString> mProperties;
};

/**
 * The playlist, which you derive from when creating
 * your own playlist.
 *
 * Do not, under any circumstances, call a Playlist method
 * when you can call a Player method, unless, of course, you
 * ARE the playlist.
 **/
class Playlist : public QObject
{
Q_OBJECT
	friend class PlaylistItem;
public:
	Playlist(QObject *parent, const char *name);
	
	/**
	 * go to the front
	 **/
	virtual void reset()=0;
	
	/**
	 * empty the list
	 **/
	virtual void clear()=0;
	/**
	 * add a file
	 */
	virtual void addFile(const KURL&, bool play=false)=0;

	/**
	 * cycle forward, return that
	 **/	
	virtual PlaylistItem *next()=0;
	/**
	 * current item
	 **/
	virtual PlaylistItem *current()=0;
	/**
	 * cycle back, return that
	 **/
	virtual PlaylistItem *previous()=0;

	/**
	 * get the first item
	 **/
	virtual PlaylistItem *getFirst() const =0;

	/**
	 * get the item after item
	 **/
	virtual PlaylistItem *getAfter(const PlaylistItem *item) const =0;
	 
	/**
	 * is the view visible?
	 **/
	virtual bool listVisible() const =0;
	/**
	 * do the KCmdLineArgs stuff
	 **/
	int handleArguments();

	/**
	 * Clear the properties in all the playlistitems
	 * in which the key is equal to the given
	 *
	 * This requires @ref getFirst and @ref getAfter to work :)
	 **/
	virtual void clearProperties(const QString &key, bool sendchanged=true);

public slots:
	/**
	 * show the list!
	 **/
	virtual void showList()=0;
	/**
	 * hide it
	 **/
	virtual void hideList()=0;
	/**
	 * toggle visibility
	 **/
	virtual void toggleList();
	/**
	 * remove the item
	 **/
	virtual void remove(PlaylistItem*)=0;

signals:
	/**
	 * when item @p item is deleted from the 
	 * playlist.  the item itself has no such
	 * signal since it's not a QObject
	 *
	 * This is called by the internals to signal
	 * a deleted file
	 **/
	void removed(PlaylistItem *item);
	/**
	 * when you want the engine to reload current()
	 **/
	void playCurrent();
	/**
	 * when the list is hidden
	 **/
	void listHidden();
	/**
	 * when the list is shown
	 **/
	void listShown();
	/**
	 * they just added one that will be played when
	 * the play button is pressed.  emit this when no file
	 * has been replace by some file, not always.
	 **/
	void newCurrent();

	/**
	 * A new item has been allocated, this is
	 * called by the internals
	 **/
	void added(PlaylistItem *item);

private:
	void deleted(PlaylistItem *item);
};

#endif

