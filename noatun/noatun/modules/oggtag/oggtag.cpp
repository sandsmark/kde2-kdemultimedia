#include "oggtag.h"
#include <klocale.h>
#include <noatunapp.h>
#include <qfile.h>
#include <kglobal.h>
#include <string.h>
#include <qlabel.h>
#include <klineedit.h>
#include <kglobal.h>
#include <klocale.h>
#include <kconfig.h>
#include <qlayout.h>
#include <kaction.h>
#include <kpopupmenu.h>

extern "C"
{
	Plugin *create_plugin()
	{
		return new OTAG();
	}
}


OTAG::OTAG() : QObject(0, "OTAG"), Plugin()
{
	NOATUNPLUGINC(OTAG);
	connect(napp->player(), SIGNAL(newSong()), SLOT(newSong()));

	connect(new IDPrefs(this), SIGNAL(saved()), SLOT(newSong()));
}

// read 4 bytes and return the little endian total
static unsigned int readNoEndian(const char *d)
{
	const unsigned char *ud=(const unsigned char*)d;
	unsigned int t;
	t =ud[3];
	t<<=24;
	t+=ud[2];
	t<<=16;
	t+=ud[1];
	t<<=8;
	t+=ud[0];
	return t;
}
static QString parseFormat(const QString &format, const QString &artist, const QString &album, const QString &comment,
                           const QString &copyright, const QString &date, const QString &genre, const QString &location,
						   const QString &organization, const QString &title);

static void addToString(QString &str, const QString &src)
{
	if (str.length())
		str+=", ";
	str+=src;
}

// sucks and is broken and will work
static int find(const char *haystack, int hlen, const char *needle, int nlen)
{
	int nlenc=0;
	for (int c=0; c<hlen; c++)
	{
		if (nlen==nlenc)
			return c-nlenc;
		
		if (haystack[c]==needle[nlenc])
			nlenc++;
	}
	return -1;
}


void OTAG::newSong()
{
	PlaylistItem *item=napp->player()->current();
	if (!item) return;
	
	if (item->file().right(4).lower()!=".ogg")
		return;

	QFile file(item->file());
	if (!file.open(IO_ReadOnly)) return;
	char ar[4096*4];
	file.readBlock(ar, sizeof(ar));
	int pos=find(ar, sizeof(ar), "\x03vorbis", 7);
	if (pos<0) return;

	
	unsigned int skip=readNoEndian(ar+7+pos);
	if (skip==0) return;
	skip += (unsigned int)pos+7+4;

	QString artist, album, comment, copyright, date, genre, location, 
	        organization, title;	
	
	int sections=readNoEndian(ar+skip);
	if (sections<=0) return;
	skip+=4;
	while (sections--)
	{
		int length=readNoEndian(ar+skip);
		skip+=4;
		char *buf=new char[length+1];
		memcpy(buf, ar+skip, length);
		buf[length]='\0';
		QString str = QString().fromUtf8(buf);
		delete [] buf;
		skip+=length;

		int eq=str.find('=');
		if (eq<0) continue;

		QString key(str.left(eq));
		QString value(str.mid(eq + 1));
		key=key.lower();
		if (key=="artist")
		{
			item->setProperty("artist", value);
			addToString(artist, value);
		}
		else if (key=="album")
		{
			item->setProperty("album", value);
			addToString(album, value);
		}
		else if (key=="comment")
		{
			item->setProperty("comment", value);
			addToString(comment, value);
		}
		else if (key=="copyright")
			addToString(copyright, value);
		else if (key=="date")
		{
			item->setProperty("date", value);
			addToString(date, value);
		}
		else if (key=="genre")
			addToString(genre, value);
		else if (key=="location")
			addToString(location, value);
		else if (key=="organization")
			addToString(organization, value);
		else if (key=="title")
		{
			item->setProperty("title", value);
			addToString(title, value);
		}
	}
	
	if (!title.length()) return;
	
	KGlobal::config()->setGroup("OGG Tags");
	QString format=KGlobal::config()->readEntry("OggFormat", "%t");
	
	format= parseFormat(format, artist, album, comment, copyright, date, genre,
	                    location, organization, title);

   // some titles/authors/songs might contain a / which is encoded as %2f
   // so replace the encoding with the /
   format.replace( QRegExp("%2f"), "/" );
	if (format.length())
		 napp->player()->current()->setTitle(format);
}

static QString parseFormat(const QString &format, const QString &artist, const QString &album, const QString &comment,
                           const QString &copyright, const QString &date, const QString &genre, const QString &location,
						   const QString &organization, const QString &title)
{
	QString parsed(format);
	parsed=' ' +parsed;
	for (unsigned int pos=1; pos< parsed.length()-1; pos++)
	{
		if ((parsed[pos]=='%') && (parsed[pos-1]!='%'))
		{
			switch (parsed[pos+1].latin1())
			{
			case('%'):
				parsed.replace(pos, 2, "%");
				break;
			case('a'):
				parsed.replace(pos, 2, artist);
				break;
			case('A'):
				parsed.replace(pos, 2, album);
				break;
			case('c'):
				parsed.replace(pos, 2, comment);
				break;
			case('C'):
				parsed.replace(pos, 2, copyright);
				break;
			case('d'):
				parsed.replace(pos, 2, date);
				break;
			case('g'):
				parsed.replace(pos, 2, genre);
				break;
			case('l'):
				parsed.replace(pos, 2, location);
				break;
			case('o'):
				parsed.replace(pos, 2, organization);
				break;
			case('t'):
				parsed.replace(pos, 2, title);
				break;

			}	
		}
	}
	return parsed.remove(0, 1);
}



IDPrefs::IDPrefs(QObject *parent)
	: CModule(i18n("Ogg Tag Loading"), i18n("Format Strings for OggVorbis Tag Loading"), parent)
{
	(new QVBoxLayout(this))->setAutoAdd(true);
	
	KGlobal::config()->setGroup("OGG Tags");
	mString=new KLineEdit(KGlobal::config()->readEntry("OggFormat", "%t"), this);
	(mPreview=new QLineEdit(this))->setReadOnly(true);
	QLabel *l=new QLabel(i18n("Enter the format for the tag.  Every %[something] will be "
	                          "replaced with the following field, with the exception of '%%', which is replaced with '%'."),
	                     this);
	l->setAlignment(AlignLeft | AlignVCenter | ExpandTabs | WordBreak);
	

	new QLabel(i18n("%a - Artist"), this);
	new QLabel(i18n("%A - Album"), this);
	new QLabel(i18n("%c - Comment/Description"), this);
	new QLabel(i18n("%C - Copyright"), this);
	new QLabel(i18n("%d - Date"), this);
	new QLabel(i18n("%g - Genre"), this);
	new QLabel(i18n("%l - Location"), this);
	new QLabel(i18n("%o - Organization"), this);
	new QLabel(i18n("%t - Title"), this);
	
	
	connect(mString, SIGNAL(textChanged(const QString &)), SLOT(changed(const QString&)));
	changed(mString->text());
}

void IDPrefs::changed(const QString &t)
{
	mPreview->setText(parseFormat(t, "Enigma", "MCMXC a.D.", "I'm Guilty", "Virgin Schallplatten", "1990", "New Age", "Unknown", "Enigma", "Mea Culpa"));
}

void IDPrefs::save()
{
	KConfig *config=KGlobal::config();
	config->setGroup("OGG Tags");
	config->writeEntry("OggFormat", mString->text());
	config->sync();
	emit saved();
}


#include "oggtag.moc"
