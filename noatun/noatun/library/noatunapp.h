#ifndef NOATUN_H
#define NOATUN_H

#include <kuniqueapp.h>

class Playlist;
class Player;
class LibraryLoader;
class KPopupMenu;
class NoatunPreferences;
class Downloader;
class Effects;
class EffectView;
class Equalizer;
class EqualizerView;

class NoatunApp : public KUniqueApplication
{
Q_OBJECT
friend class Playlist;
friend class KCrash;

public:
	NoatunApp();
	~NoatunApp();

	Playlist *playlist() const;
	Player *player() const { return mPlayer; }
	NoatunPreferences *preferencesBox() const { return mPref; }

	/**
	 * get a list of mimetypes aRts (noatun) can play, KFileDialog
	 * accepts this instead of the shell globs in the static functions,
	 * so there ya go :)
	 **/
	QString mimeTypes();
	
	LibraryLoader *libraryLoader() const { return mLibraryLoader; }
	Downloader *downloader() const { return mDownloader; }
	static QImage readPNG(const QString &filename);
	Effects *effects() const;
	::Equalizer *equalizer() const { return mEqualizer; }
	EqualizerView *equalizerView() const { return mEqualizerView; }

	QCString version() const;

	virtual void commitData(QSessionManager &);
	virtual void saveState(QSessionManager &);
	
public: //options
	bool autoPlay() const;
	bool loopList() const;
	bool oneInstance() const;
	QString saveDirectory() const;
	bool clearOnOpen() const;
	bool hackUpPlaylist() const;
	bool fastMixer() const;

	void setOneInstance(bool);
	void setLoopList(bool);
	void setAutoPlay(bool);
	void setSaveDirectory(const QString &);
	void setRememberPositions(bool);
	void setClearOnOpen(bool);
	void setHackUpPlaylist(bool);
	void setFastMixer(bool);

	/**
	 * Adds an item to the plugin menu.
	 *
	 * @return the ID associated with the menu item, for use in @ref contextMenuRemove()
	 **/
	int pluginMenuAdd(const QString &text, const QObject *receiver, const char *member);

	/**
	 * Removes an item previously added to the plugin menu.
	 **/
	void pluginMenuRemove(int id);

	KPopupMenu *pluginMenu(void);

protected:
	virtual int newInstance();

public slots:
	// slots for the contextMenu
	void preferences();
	void quit();
	void fileOpen();
	void effectView();
	void equalizerView();
	
private:
	static void cleverKCrashHack(int);
	void loadPlugins();

private:
	Player *mPlayer;
	LibraryLoader *mLibraryLoader;
	KPopupMenu *mPluginMenu;
	Downloader *mDownloader;
	Effects *mEffects;
	EffectView *mEffectView;
	NoatunPreferences *mPref;
	::Equalizer *mEqualizer;
	EqualizerView *mEqualizerView;
};

#define napp (static_cast<NoatunApp*>(kapp))

// version info for the plugins
// this is MAJOR.MINOR.PATCHLEVEL
// and you developers better ignore patchlevel :)
#define NOATUN_MAJOR 1
#define NOATUN_MINOR 2
#define NOATUN_PATCHLEVEL 0

#endif
