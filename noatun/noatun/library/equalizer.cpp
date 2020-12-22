#include "equalizer.h"
#include "engine.h"
#include <common.h>
#include <dynamicrequest.h>
#include <artsflow.h>
#include <noatunapp.h>
#include <player.h>
#include <soundserver.h>
#include <noatunarts.h>
#include <ktempfile.h>
#include <qdom.h>
#include <netaccess.h>
#include <kstddirs.h>
#include <qtextstream.h>
#include <kstddirs.h>
#include <math.h>
#include <kconfig.h>
#include <klocale.h>

#define EQ napp->equalizer()
#define EQBACK napp->player()->engine()->equalizer()

using std::vector;

static QString makePresetFile()
{
	QString basedir=kapp->dirs()->localkdedir()+"/share/apps/noatun/eq.preset/";
	// now append a filename that doens't exist
	KStandardDirs::makeDir(basedir);
	QString fullpath;
	int num=0;
	do
	{
		if (num)
			fullpath=basedir+"preset."+QString::number(num);
		else
			fullpath=basedir+"preset";
		num++;
	}			
	while (QFile(fullpath).exists());
	return fullpath;
}

Preset::Preset(const QString &file) : mFile(file)
{ }

Preset::Preset() : mFile(makePresetFile())
{ }

QString Preset::name() const
{
	QFile file(mFile);
	if (!file.open(IO_ReadOnly)) return 0;

	QDomDocument doc("noatunequalizer");
	if (!doc.setContent(&file)) return 0;

	QDomElement docElem = doc.documentElement();
	if (docElem.tagName()!="noatunequalizer") return 0;
	bool standard=docElem.attribute("default", "0")=="0";
	QString n=docElem.attribute("name", 0);
	
#if 0
	{ // All the translations for the presets
		i18n("Trance");
		i18n("Dance");
		i18n("Metal");
		i18n("Jazz");
		i18n("Zero");
	}
#endif
	
	if (standard)
		n=i18n(n.local8Bit());
	
	return n;
}

bool Preset::setName(const QString &name)
{
	QList<Preset> presets=EQ->presets();
	for (Preset *i=presets.first(); i!=0; i=presets.next())
	{
		if (i==this) continue;
		if (i->name()==name)
			return false;
	}
	
	QFile file(mFile);
	if (!file.open(IO_ReadOnly)) return 0;

	QDomDocument doc("noatunequalizer");
	if (!doc.setContent(&file)) return 0;

	QDomElement docElem = doc.documentElement();
	if (docElem.tagName()!="noatunequalizer") return 0;

	docElem.setAttribute("name", name);

	emit EQ->renamed(this);
	file.close();
	file.open(IO_ReadWrite | IO_Truncate);
	QTextStream s(&file);
	s << doc.toString();
	file.close();

	return true;
}

bool Preset::save() const
{
	return EQ->save(mFile, name());
}

bool Preset::load()
{
	bool b=EQ->load(mFile);
	if (b) emit EQ->changed(this);
	return b;
}

void Preset::remove()
{
	KConfig *config=kapp->config();
	config->setGroup("Equalizer");
	QStringList items=config->readListEntry("presets");
	items.remove(mFile);
	config->sync();

	emit EQ->removed(this);

	if (mFile.find(kapp->dirs()->localkdedir())==0)
	{
		QFile(mFile).remove();
	}
}

QString Preset::file() const
{
	return mFile;
}


Band::Band(int start, int end) : mLevel(0), mStart(start), mEnd(end)
{
	EQ->add(this);
	EQ->update();
	emit EQ->changed();
	emit EQ->changed(this);
}

Band::~Band()
{
	EQ->remove(this);
}

int Band::level()
{
	return mLevel;
}

void Band::setLevel(int l)
{
	mLevel=l;
	EQ->update();
	emit EQ->changed();
	emit EQ->changed(this);
}

int Band::start() const
{
	return mStart;
}

int Band::end() const
{
	return mEnd;
}

int Band::center() const
{
	return (mStart+mEnd)/2;
}

static QString formatFreq(int f, bool withHz)
{
	QString format;
	if (f<991)
		format=QString::number(f);
	else
		format=QString::number((int)((f+500)/1000.0))+"k";

	if (withHz)
		format+="Hz";

	return format;
}

QString Band::formatStart(bool withHz) const
{
	return formatFreq(mStart, withHz);
}

QString Band::formatEnd(bool withHz) const
{
	return formatFreq(mEnd, withHz);
}

QString Band::format(bool withHz) const
{
	return formatFreq(center(), withHz);
}

/* rate 4
27       54     0-108      108
81       163    109-217    108
243      514    218-810    269
729      1621   811-2431  1620
2187     4661   2432-7290 4858
6561     13645  7291+     12708

*/
Equalizer::Equalizer() : mUpdates(true)
{
}

void Equalizer::init()
{
	for (Band *i=mBands.first(); i!=0; i=mBands.next())
		delete i;

	{
		enableUpdates(false);
		new Band(0, 108);
		new Band(109, 217);
		new Band(218, 810);
		new Band(811, 2431);
		new Band(2432, 7290);
		new Band(7291, 19999);

		enableUpdates();
		setPreamp(0);
		update(true);
	}
	
	load(napp->dirs()->saveLocation("data", "noatun/") + "equalizer");

	KConfig *config=kapp->config();
	config->setGroup("Equalizer");
	setEnabled(config->readBoolEntry("enabled", false));
}
	
QList<Preset> Equalizer::presets() const
{
	KConfig *conf=KGlobal::config();
	conf->setGroup("Equalizer");
	
	QStringList list;
	if (conf->hasKey("presets"))
	{
		list=conf->readListEntry("presets");
	}
	else
	{
		list=kapp->dirs()->findAllResources("data", "noatun/eq.preset/*");
		conf->writeEntry("presets", list);
		conf->sync();
	}
	
	QList<Preset> presets;

	for (QStringList::Iterator i = list.begin(); i!=list.end(); ++i)
	{
		QFile file(*i);
		if (!file.open(IO_ReadOnly)) continue;

		QDomDocument doc("noatunequalizer");
		if (!doc.setContent(&file)) continue;

		QDomElement docElem = doc.documentElement();
		if (docElem.tagName()!="noatunequalizer") continue;

		presets.append(new Preset(*i));
	}
	return presets;
}

Preset *Equalizer::preset(const QString &file)
{
	KConfig *conf=KGlobal::config();
	conf->setGroup("Equalizer");
	QStringList list=kapp->config()->readListEntry("presets");
	if (list.contains(file))
		return new Preset(file);
	return 0;
}

bool Equalizer::presetExists(const QString &name) const
{
	QList<Preset> list=presets();
	for (Preset *i=list.first(); i!=0; i=list.next())
	{
		if (i->name()==name)
			return true;
	}
	return false;
}

Preset *Equalizer::createPreset(const QString &name, bool smart)
{
	if (presetExists(name) && !smart) return 0;
	QString nameReal=name;
	{
		int number=1;
		while (presetExists(nameReal))
		{
			nameReal=name+" ("+QString::number(number)+')';
			number++;
		}
	}
	
	Preset *preset=new Preset;
	
	{
		save(preset->file(), nameReal);
		KConfig *config=kapp->config();
		config->setGroup("Equalizer");
		QStringList list=config->readListEntry("presets");
		list+=preset->file();
		config->writeEntry("presets", list);
		config->sync();
		
		emit created(preset);
		return preset;
	}
}

Equalizer::~Equalizer()
{
	save(napp->dirs()->saveLocation("data", "noatun/") + "equalizer", "auto");
	for (Band *i=mBands.first(); i!=0; i=mBands.next())
		delete i;
}

const QList<Band> &Equalizer::bands() const
{
	return mBands;
}

Band *Equalizer::band(int num) const
{
	// can't use QList::at since it sets current

	QListIterator<Band> item(mBands);
	item+=(unsigned int)num;
	return *item;
}

int Equalizer::bandCount() const
{
	return (int)mBands.count();
}

int Equalizer::preamp() const
{
	return mPreamp;
}

bool Equalizer::isEnabled() const
{
	return (bool)EQBACK->enabled();

}

void Equalizer::setPreamp(int p)
{
	mPreamp=p;
	EQBACK->preamp(pow(2,(float)p/100.0));
	emit changed();
	emit preampChanged();
	emit preampChanged(p);
}

void Equalizer::enable()
{
	setEnabled(true);
}

void Equalizer::disable()
{
	setEnabled(false);
}

void Equalizer::setEnabled(bool e)
{
	EQBACK->enabled((long)e);
	KConfig *config=kapp->config();
	config->setGroup("Equalizer");
	config->writeEntry("enabled", e);
	config->sync();

	emit enabled(e);
	if (e)
		emit enabled();
	else
		emit disabled();
}

bool Equalizer::save(const KURL &filename, const QString &name) const
{
	QDomDocument doc("noatunequalizer");
	doc.setContent(QString("<!DOCTYPE NoatunEqualizer><noatunequalizer/>"));
	QDomElement docElem = doc.documentElement();

	{
		docElem.setAttribute("level", preamp());
		docElem.setAttribute("name", name);
		docElem.setAttribute("version", napp->version());
	}

	for (QListIterator<Band> i(mBands); i.current(); ++i)
	{
		QDomElement elem = doc.createElement("band");
		elem.setAttribute("start", (*i)->start());
		elem.setAttribute("end", (*i)->end());
		elem.setAttribute("level", (*i)->level());

		docElem.appendChild(elem);
	}

	KTempFile file;

	*(file.textStream()) << doc.toString();
	file.close();

	return KIO::NetAccess::upload(file.name(), filename);
}



bool Equalizer::load(const KURL &filename)
{
	QString dest;
	if(KIO::NetAccess::download(filename, dest))
	{
		QFile file(dest);
		if (!file.open(IO_ReadOnly))
			return false;

		QDomDocument doc("noatunequalizer");
		if (!doc.setContent(&file))
			return false;

		QDomElement docElem = doc.documentElement();
		if (docElem.tagName()!="noatunequalizer")
			return false;

		enableUpdates(false);

//		blockSignals(true);
		setPreamp(docElem.attribute("level", "0").toInt());
		Band *bandIter=mBands.first();
		for (QDomNode n = docElem.firstChild(); !n.isNull(); n = n.nextSibling())
		{
			QDomElement e = n.toElement();
			if(e.isNull()) continue;
			if (e.tagName()!="band") continue;

//			int start=e.attribute("start","0").toInt();
//			int end=e.attribute("end", "108").toInt();
			int level=e.attribute("level", "0").toInt();
			bandIter->setLevel(level);
			bandIter=mBands.next();
		}
		enableUpdates();
//		blockSignals(false);
		update(true);
//		emit changed();
		return true;
	}
	return false;
}

void Equalizer::add(Band *b)
{
	mBands.append(b);
}

void Equalizer::remove(Band *b)
{
	mBands.removeRef(b);
}

void Equalizer::update(bool full)
{
	if (!mUpdates)
		return;

	vector<float> levels;
	vector<float> mids;
	vector<float> widths;

	for (Band *i=mBands.first(); i!=0; i=mBands.next())
	{
		levels.push_back(pow(2, (float)i->mLevel/50.0));
		if (full)
		{
			int mid=i->mStart+i->mEnd;
			mids.push_back(((float)mid)*0.5);
			widths.push_back((float)(i->mEnd-i->mStart));
		}
	}
	if (full)
		EQBACK->set(levels, mids, widths);
	else
		EQBACK->levels(levels);
}

void Equalizer::enableUpdates(bool on)
{
	mUpdates=on;
}

#undef EQ
#undef EQBACK

#include "equalizer.moc"

