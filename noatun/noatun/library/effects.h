#ifndef EFFECTS_H
#define EFFECTS_H

#include <qlist.h>
#include <qcstring.h>
#include <qstrlist.h>
#include <qobject.h>

namespace Arts { class StereoEffect; }
class Engine;
class EffectConfigWidget;

/**
 * new Effect("Arts::SomeEffect");
 * then you can add,insert,bleh it with class Effects
 **/
class Effect
{
friend class Effects;
friend class EffectConfigWidget;
public:
	Effect(const char *name);
	~Effect();

	/**
	 * return the effect processed
	 * directly before this one
	 **/
	Effect *before() const;
	/**
	 * return the effect processed
	 * directly after this one
	 **/
	Effect *after() const;
	long id() const;

	/**
	 * get the Arts object.
	 * @internal
	 **/
	Arts::StereoEffect *effect() const;

	/**
	 * Get the name of the object.
	 **/
	QCString name() const;
	
	/**
	 * get the "clean" title of effect
	 **/
	QString title() const;

	/**
	 * is this effect name invalid? e.g., will it segfault
	 * if you StereoEffect::start() this?
	 **/
	bool isNull() const;

	/**
	 * show the configure dialog box for
	 * this effect.  if friendly is true, 
	 * then create a top-level window,
	 * set an icon and make it purdy. Otherwise
	 * create a plan widget that you can reparent.
	 **/
	QWidget *configure(bool friendly=true);

	/**
	 * Does this widget have a configurable
	 * dialog box.  E.g., will configure
	 * return null?
	 **/
	bool configurable() const;

	/**
	 * turn Arts::FREEVERB into just FREEVERB
	 **/
	static QString clean(const QCString &name);
private:
	long mId;
	Arts::StereoEffect *mEffect;
	QCString mName;
	QWidget *mConfig;
};

class Effects : public QObject
{
Q_OBJECT
friend class Effect;
public:
	Effects();

	bool insert(const Effect *after, Effect *item);

	/**
	 * create the Effect, by getting the proper item
	 * from the list, then append it here.
	 *
	 * for example, append(new Effect(available()[0]));
	 **/
	bool append(Effect *item);

	/**
	 * reorder the effect stack.  if after is null,
	 * it'l be first
	 **/
	void move(const Effect *after, Effect *item);
	/**
	 * remove @param item from the effect stack, but
	 * don't deallocate it unless del is true (default true)
	 **/
	void remove(Effect *item, bool del=true);
	/**
	 * @r remove() all items from the effect stack
	 * only delete them if del is true (default true)
	 **/
	void removeAll(bool del=true);

	/**
	 * a list of all available effects, by name
	 * each of these can be given to the first
	 * argument of the Effect constructor
	 **/
	QStrList available() const;

	QList<Effect> effects() const;

	/**
	 * Get the Effect that has the following id
	 **/
	Effect *findId(long id) const;

private:
	QListIterator<Effect> stackPosition() const;
	
signals:
	/**
	 * called when @param effect has been
	 * added to the effect stack
	 **/
	void added(Effect *effect);
	/**
	 * called when @param effect has been
	 * removed from the effect stack
	 **/
	void removed(Effect *effect);
	/**
	 * called when @param effect has been moved
	 **/
	void moved(Effect *effect);
	/**
	 * callsed when @p effect is about to be
	 * deleted (from memory)
	 **/
	void deleting(Effect *effect);

private:
	// stored in no specific order
	QList<Effect> mItems;
};



#endif

