#ifndef NOATUN_EQUALIZER_H
#define NOATUN_EQUALIZER_H

#include <qlist.h>
#include <qobject.h>
#include <kurl.h>

namespace Arts { class StereoEffect; }
class Engine;
class Equalizer;

class Preset
{
friend class Equalizer;
	Preset(const QString &file);
	Preset();
public:
	QString name() const;
	bool setName(const QString &name);
	bool save() const;
	bool load();

	void remove();

	QString file() const;
private:
	QString mFile;
};

class Band
{
friend class Equalizer;
friend class QList<Band>;

private:
	Band();
	Band(int start, int end);
	virtual ~Band();
public:
	/**
	 * the intensity of the change.
	 * it's logarithmic.  0 is no change
	 * negative numbers are loss in intensity
	 * positive numbers are a gain
	 * And +-100 is the most you'd need to go
	 **/
	int level();
	void setLevel(int l);
	
	int start() const;
	int end() const;

	/**
	 * the middle between start and end
	 **/
	int center() const;

	QString formatStart(bool withHz=true) const;
	QString formatEnd(bool withHz=true) const;
	/**
	 * return the format for center()
	 **/
	QString format(bool withHz=true) const;
	
private:
	int mLevel;
	int mStart, mEnd;
};

class Equalizer : public QObject
{
friend class Band;
friend class Preset;
friend class Engine;
Q_OBJECT
public:
	Equalizer();
	~Equalizer();
	
	const QList<Band> &bands() const;
	Band *band(int num) const;
	int bandCount() const;

	int preamp() const;
	bool isEnabled() const;
	
	void init();

public slots:
	/**
	 * set the preamplification
	 * it's logarithmic.  0 is no change
	 * negative numbers are loss in intensity
	 * positive numbers are a gain
	 * And +-100 is the most you'd need to go
	 **/
	void setPreamp(int p);
	void enable();
	void disable();
	void setEnabled(bool e);


public:
// saving eq stuff
	/**
	 * save the current levels
	 * all noatun equalizer files have the "*.noatunequalizer"
	 * pattern.  Nevertheless, the file can be identified
	 * by magic, so it's not required
	 **/
	bool save(const KURL &file, const QString &friendly) const;

	/**
	 * restore the EQ settings from this file
	 **/
	bool load(const KURL &file);

	/**
	 * create a preset with such a name
	 * and remember that it'l return zero
	 * if the name already exists
	 *
	 * If smart is true, append a number to the end
	 * of the name, if one already exists by the given
	 **/
	Preset *createPreset(const QString &name, bool smart=true);

	/**
	 * return all the presets
	 * remember to setAutoDelete on this
	 **/
	QList<Preset> presets() const;
	
	Preset *preset(const QString &file);
	bool presetExists(const QString &name) const;
		
signals:
	void changed(Band *band);
	void changed();
	void enabled();
	void disabled();
	void enabled(bool e);

	void preampChanged(int p);
	void preampChanged();

	/**
	 * the preset with the given name
	 * was selected
	 **/
	void changed(Preset *);

	/**
	 * when a new preset has been created
	 **/
	void created(Preset*);

	/**
	 * when @p preset has been renamed to @p newname
	 **/ 
	void renamed(Preset *);

	/**
	 * the given preset has been removed
	 **/
	void removed(Preset *);
	
private:
	void add(Band*);
	void remove(Band*);
	// apply the data to artsd
	void enableUpdates(bool on=true);
	void update(bool full=false);
	
private:
	QList<Band> mBands;
	bool mUpdates;
	int mPreamp;
};



#endif

