#include "id3tag.h"
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
#include "editor.h"

extern "C"
{
	Plugin *create_plugin()
	{
		return new NID3();
	}
}


NID3::NID3() : QObject(0, "NID3"), Plugin(), mEditor(0)
{
	NOATUNPLUGINC(NID3);
	connect(napp->player(), SIGNAL(newSong()), SLOT(newSong()));
	menuID = napp->pluginMenuAdd(i18n("&ID3 Edit"), this, SLOT(editID3()));
	connect(new IDPrefs(this), SIGNAL(saved()), SLOT(newSong()));
}

NID3::~NID3()
{
	napp->pluginMenuRemove(menuID);
}

void NID3::editID3()
{
	if (mEditor)
		if (mEditor->isVisible())
			mEditor->hide();
		else
			mEditor->show();


}

static QString parseFormat(const QString &format, const QString &name, const QString &artist, const QString &album,
                           const QString &comment, const QString &year, const QString &genre);

void NID3::newSong()
{
	PlaylistItem *item=napp->player()->current();
	if (!item) return;
	
	if (mEditor && !mEditor->isVisible())
	{
		delete mEditor;
		mEditor=0;
	}

	{
		mEditor=new Editor(this);
		mEditor->open(item->file());
		connect(mEditor, SIGNAL(saved()), SLOT(newSong()));
	}

	if (!mEditor->name().stripWhiteSpace().length()) return;

	KConfig *config = KGlobal::config();
	KConfigGroupSaver saver(config, "ID3 Tags");
	QString format=config->readEntry("ID3Format", "%t");

	QString title = parseFormat(format,mEditor->name(), mEditor->artist(),
	                            mEditor->album(), mEditor->comment(), mEditor->year(), mEditor->genre());
	// some titles/authors/songs might contain a / which is encoded as %2f
	// so replace the encoding with the /
	title.replace( QRegExp("%2f"), "/" );
	item->setTitle( title );
	
//	item->setProperty("title", mEditor->name());
	item->setProperty("artist", mEditor->artist());
	item->setProperty("album", mEditor->album());
	item->setProperty("date", mEditor->year());
	item->setProperty("comment", mEditor->comment());
}

static QString parseFormat(const QString &format, const QString &name, const QString &artist, const QString &album,
                           const QString &comment, const QString &year, const QString &genre)
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
			case('t'):
				parsed.replace(pos, 2, name);
				break;
			case('y'):
				parsed.replace(pos, 2, year);
				break;
			case('Y'):
				parsed.replace(pos, 2, year.right(2));
				break;
			case('g'):
				parsed.replace(pos, 2, genre);
				break;
			}
		}
	}
	return parsed.remove(0, 1);
}


IDPrefs::IDPrefs(QObject *parent)
	: CModule(i18n("ID3 Tag Loading"), i18n("Format Strings for ID3 Tag Loading"), parent)
{
	(new QVBoxLayout(this))->setAutoAdd(true);

	KConfig *config = KGlobal::config();
	KConfigGroupSaver saver(config, "ID3 Tags");
	mString=new KLineEdit(config->readEntry("ID3Format", "%t"), this);
	(mPreview=new QLineEdit(this))->setReadOnly(true);
	QLabel *l=new QLabel(i18n("Enter the format for the ID Tag.  Every %[something] will be "
	                          "replaced with the following field, with the exception of '%%', which is replaced with '%'"),
	                     this);
	l->setAlignment(AlignLeft | AlignVCenter | ExpandTabs | WordBreak);

	new QLabel(i18n("%a - Artist"), this);
	new QLabel(i18n("%A - Album"), this);
	new QLabel(i18n("%c - Comment"), this);
	new QLabel(i18n("%t - Title"), this);
	new QLabel(i18n("%y - Year (4 Digits)"), this);
	new QLabel(i18n("%Y - Year (2 Digits)"), this);
	new QLabel(i18n("%g - Genre"), this);

	connect(mString, SIGNAL(textChanged(const QString &)), SLOT(changed(const QString&)));
	changed(mString->text());
}

void IDPrefs::changed(const QString &t)
{
	mPreview->setText(parseFormat(t, "Mea Culpa", "Enigma", "MCMXC a.D.", "I'm guilty", "1990", "New Age"));
}

void IDPrefs::save()
{
	KConfig *config=KGlobal::config();
	KConfigGroupSaver saver(config, "ID3 Tags");
	config->writeEntry("ID3Format", mString->text());
	config->sync();
	emit saved();
}


#include "id3tag.moc"
