#ifndef NPLUGIN_H
#define NPLUGIN_H

#include <pluginloader.h>
#include <qarray.h>
#include <vector>


namespace Noatun { class FFTScopeStereo; class FFTScope; class RawScope; 
                   class RawScopeStereo; class StereoEffectStack;
                   class Listener;
                 }
namespace Arts { class SoundServerV2; class Dispatcher; }

//backwards compatibility

#define NOATUNPLUGIND 
#define NOATUNPLUGINC(classname)  { }

class Playlist;
class Player;
class ExitNotifier;
class NoatunApp;

class Plugin
{
public:
	Plugin();
	virtual ~Plugin();

	/**
	 * called directly after the plugin, just in case
	 * you want Noatun to be "ready" with your class
	 **/
	virtual void init();

	/**
	 * unload the plugin
	 * if it returns true, return from your function
	 * immediately and don't access members of this
	 * TODO
	 **/
	virtual bool unload();

	/**
	 * make multiple inheritence work
	 * only needed with playlist plugins
	 * generally "return this" in here
	 **/
	virtual Playlist *playlist() const { return 0; }
};

class UserInterface : public Plugin
{
public:
	UserInterface();
	virtual ~UserInterface();
};


class TimerThingy;


/**
 * The base for all sorts of visualizations
 *
 * all Noatun Visualizations can be in
 * separate processes!  You must fork, and
 * then exec() to be able to use this.
 * Its perfectly safe to create
 * a visualization in a binary executed by
 * KProcess.
 **/
class Visualization
{
friend class TimerThingy;
friend class ThingThatTellsAVisualizationThatNoatunIsGone;
friend class ExitNotifier;
friend class NoatunApp;
public:
	/**
	 * interval is how frequently the rescope
	 * will occur
	 * 0 means disabled
	 *
	 * Pid, if not zero, can force the visualizaton
	 * to use another noatun's process's visualization stack,
	 * if it is zero, it'l try to guess what to use
	 **/
	Visualization(int interval=125, int pid=0);
	virtual ~Visualization();

	/**
	 * start the timer, if it's 0, then this
	 * will do nothing.
	 **/
	virtual void start();
	/**
	 * stop the timer
	 **/
	virtual void stop();

	/**
	 * how long between each rescoping in milliseconds
	 **/
	int interval() const;
	
	/**
	 * set how long to wait
	 * the change takes effect immediately,
	 * unless it hadn't been start()ed beforehand
	 **/
	virtual void setInterval(int msecs);

	/**
	 * cause a rescoping to take effect immediately
	 **/
	virtual void timeout()=0;

	Noatun::StereoEffectStack visualizationStack();
	Arts::SoundServerV2 *server();

	/**
	 * return noatun's pid, useful if you're doing remote-visualization
	 *
	 * It returns the pid to use, or -1, if there was an error, or 
	 * there isn't a noatun running. If there isn't a noatun running, the
	 * computer will crash, the trains will take the wrong tracks, time 
	 * will start moving in the wrong direction, and the whole universe will melt
	 * down.
	 **/
	static int noatunPid();

	/**
	 * test if the server is still there
	 **/
	bool connected();

	/**
	 * create a dispatcher object , does nothing if
	 * this already has been called
	 **/
	static void initDispatcher();
	
private:
	int mTimeout;
	TimerThingy *mTimerThingy;
	QCString mVisualizationStack;
	Arts::SoundServerV2 *mServer;
	static Arts::Dispatcher *mDispatcher;
	static bool internalVis;
};

class FFTScope : public Visualization
{
public:
	FFTScope(int interval, int pid=0);
	
	/**
	 * the "width" of each scopeEvent
	 **/
	virtual int bands() const=0;

	/**
	 * set the width combiner value.  in theory, 
	 * a value of 1.0 should give you exactly
	 * 1024 samples, greater values will decrease
	 * the amount quite rapidly.  by default,
	 * 1.1 is used, which gives 50.  You'll have
	 * to tweak this manually.
	 *
	 * This call will be potentially slow (since
	 * a component of it is brute-forced)
	 **/
	virtual void setBands(float n)=0;
};

class StereoFFTScope : public FFTScope
{
public:
	StereoFFTScope(int timeout=250, int pid=0);
	virtual ~StereoFFTScope();
	
	/**
	 * called according to the timeout
	 * the two floats contain the data, with @p len items
	 * you override this yourself
	 **/
	virtual void scopeEvent(float *left, float *right, int len)
		{ (void)left; (void)right; (void)len; }

	/**
	 * get the current data
	 * pass two vector<float>*, this will do the rest.
	 * do not allocate it beforehand.
	 * you must then delete the vectors
	 * @returns the amount of elements for both left and right
	 **/
	void scopeData(std::vector<float> *&left, std::vector<float> *&right);
	virtual void timeout();

	virtual int bands() const;
	virtual void setBands(float f);
	
private:
	Noatun::FFTScopeStereo *mScope;
	long mId;
};

class MonoFFTScope : public FFTScope
{
public:
	MonoFFTScope(int timeout=250, int pid=0);
	virtual ~MonoFFTScope();
	
	/**
	 * called according to the timeout
	 * the float contains the data, with @p len items
	 * you override this yourself
	 **/
	virtual void scopeEvent(float *data, int len)
		{ (void)data; (void)len; }

	/**
	 * get the current data
	 * pass a vector<float>*, this will do the rest.
	 * do not allocate it beforehand.
	 * you must then delete the vectors
	 * @returns the amount of elements for both left and right
	 **/
	void scopeData(std::vector<float> *&data);
	
	/**
	 * reimplemented from class Visualization, you
	 * should never need to reimplement this yourself
	 **/
	virtual void timeout();

	virtual int bands() const;
	virtual void setBands(float f);
	
private:
	Noatun::FFTScope *mScope;
	long mId;
};

class Scope : public Visualization
{
public:
	Scope(int interval, int pid=0);
	
	/**
	 * the "width" of each scopeEvent
	 **/
	virtual int samples() const=0;

	virtual void setSamples(int )=0;
};

class MonoScope : public Scope
{
public:
	MonoScope(int timeout=250, int pid=0);
	virtual ~MonoScope();
	
	/**
	 * called according to the timeout
	 * the float contains the data, with @p len items
	 * you override this yourself, and process the data
	 **/
	virtual void scopeEvent(float *data, int len)
		{(void)data; (void)len; }

	/**
	 * get the current data
	 * pass a vector<float>*, this will do the rest.
	 * do not allocate it beforehand.
	 * you must then delete the vectors
	 * @returns the amount of elements for both left and right
	 **/
	void scopeData(std::vector<float> *&data);
	
	/**
	 * reimplemented from class Visualization, you
	 * should never need to reimplement this yourself
	 **/
	virtual void timeout();


	virtual int samples() const;
	virtual void setSamples(int);

private:
	Noatun::RawScope *mScope;
	long mId;
};

class StereoScope : public Scope
{
public:
	StereoScope(int timeout=250, int pid=0);
	virtual ~StereoScope();
	
	/**
	 * called according to the timeout
	 * the float contains the data, with @p len items
	 * you override this yourself, and process the data
	 **/
	virtual void scopeEvent(float *left, float *right, int len)
		{ (void)left; (void)right; (void)len; }

	/**
	 * get the current data
	 * pass a vector<float>*, this will do the rest.
	 * do not allocate it beforehand.
	 * you must then delete the vectors
	 * @returns the amount of elements for both left and right
	 **/
	void scopeData(std::vector<float> *&left, std::vector<float> *&right);
	
	/**
	 * reimplemented from class Visualization, you
	 * should never need to reimplement this yourself
	 **/
	virtual void timeout();


	virtual int samples() const;
	virtual void setSamples(int);

	
private:
	Noatun::RawScopeStereo *mScope;
	long mId;
};

class SessionManagement : public Plugin
{
public:
	SessionManagement();
	virtual ~SessionManagement();

	virtual void restore();
};

class NoatunListenerNotif;

class NoatunListener : public QObject
{
Q_OBJECT
friend class NoatunListenerNotif;

public:
	NoatunListener(QObject *parent=0);
	virtual ~NoatunListener();

signals:
	void event();
	
protected:
	virtual void message();
	
	NoatunListenerNotif *mNotif;
};

/**
 * link to libnoatun, and the signal "event" will
 * be emitted whenever noatun exists, and the best
 * part is how it doesn't matter if noatun exists
 * cleanly
 **/
class ExitNotifier : public NoatunListener
{
public:
	ExitNotifier(int pid, QObject *parent=0);
	virtual ~ExitNotifier();

private:
	QCString appid;
};

/**
 * new BoolNotifier(&noatunOk, new ExitNotifier(this), this);
 *
 * when noatunOk is false, then noatun has exited somehow
 **/
class BoolNotifier : public QObject
{
Q_OBJECT
public:
	BoolNotifier(bool *value, NoatunListener *listener, QObject *parent=0);

private slots:
	void event() {*mValue=false;}

private:
	bool *mValue;
};

#endif

