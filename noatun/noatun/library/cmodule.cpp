#include <cmodule.h>
#include <pluginloader.h>
#include <common.h>
#include <noatunapp.h>

#include <qpushbutton.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <klocale.h>
#include <kdebug.h>
#include <klistview.h>
#include <qsplitter.h>
#include <qlabel.h>
#include <qdragobject.h>
#include <kurlrequester.h>
#include <kfiledialog.h>
#include <kdialog.h>

#include <qtextview.h>
#include <qwhatsthis.h>

#include "mimetypetree.h"

template<class T1, class T2> struct Dual
{
	Dual(const T1 &l, const T2 &r) : left(l), right(r) {}
	Dual() {}
	Dual(const Dual<T1, T2> &d) : left(d.left), right(d.right) {}
	T1 left;
	T2 right;
};
typedef Dual<QString, QString> StringDual;

static QString infoPane(const QString &title, const QValueList<StringDual> &prop)
{
	static const int cols=1;
	QString str;

	// get the title
	str+="<font size=\"+2\"><b>";
	str+=title;
	str+="</b></font>\n\n<table cols=";
	str+=QString::number(2);
	str+=" width=\"100%\"><tr>";

	// set up the properties table

	QValueList<StringDual>::ConstIterator i;
	i=prop.begin();
	
	while (i!=prop.end())
	{
		str+="<td>";
		bool first=true;
		for (int perRow=prop.count()/cols; i!=prop.end() && perRow!=0; perRow--)
		{
			if (!first)
				str+="<br>\n";
			first=false;

			str+="<b>";
			str+=(*i).left;
			str+=":</b> ";
			str+=(*i).right;
			++i;
		}
		str+="</td>";
	}
	str+="</tr></table>";
	return str;
}

class MimeListItem : public QCheckListItem
{
public:
	MimeListItem(KListView *list, const QString &mime, const QString &extension,
	             const QString &author, const QString &url, const QString &name);

public:
	QString mime() const { return mMime; }
	QString extension() const { return mExtension; }
	QString author() const { return mAuthor; }
	QString url() const { return mUrl; }
	QString name() const { return mName; }
	
private:
	QString mMime, mExtension, mAuthor, mUrl, mName;
};


MimeListItem::MimeListItem(KListView *list, const QString &mime, const QString &extension,
                           const QString &author, const QString &url, 
                           const QString &name)
	: QCheckListItem(list, name, QCheckListItem::CheckBox)
{
	setText(1, extension);
	mName=name;
	mMime=mime;
	mExtension=extension;
	mAuthor=author;
	mUrl=url;
}

Types::Types(QObject *parent) : CModule(i18n("File Types"), i18n("Configure Mime Types"), parent), first(true)
{
}

void Types::reopen()
{
	if (first)
	{
		QGridLayout *layout=new QGridLayout(this);
		
		mimes=new MimeTypeTree(this);
		mimes->setFixedWidth(120);
		layout->addMultiCellWidget(mimes, 0, 1, 0, 0);
		
		list=new KListView(this);
		layout->addWidget(list, 0, 1);

		connect(list, SIGNAL(currentChanged(QListViewItem*)), SLOT(selectionChanged(QListViewItem*)));
		
		list->show();
		list->addColumn(i18n("Name"));
		list->addColumn(i18n("Extension"));
		list->setSorting(-1);
		i18n("URL");
		i18n("Author");
		i18n("&Enable");

		list->setItemsMovable(true);
		list->setDragEnabled(true);
		list->setAcceptDrops(true);
		//	if (!q.supports("Interface","Arts::PlayObject"))
		//		return;

		text=new QTextView(this);
		layout->addWidget(text, 1, 1);
		text->show();
		text->setFixedHeight(150);
		first=false;
	}

	list->clear();
	Arts::TraderQuery q;

	std::vector<Arts::TraderOffer> *results = q.query();
	for (std::vector<Arts::TraderOffer>::iterator i=results->begin(); i != results->end(); i++)
	{
		QString name=commaVector(&*i, "Interface", true);
		QString author=commaVector(&*i, "Author");
		QString url=commaVector(&*i, "URL");
		QString extension=commaVector(&*i, "Extension");
		QString mime=commaVector(&*i, "MimeType");
		if (author.isNull() || url.isNull() || extension.isNull() || mime.isNull())
			continue;

		new MimeListItem(list, mime, extension, author, url, name);
	}
	delete results;

	selectionChanged(list->firstChild());
}

void Types::selectionChanged(QListViewItem *item)
{
	MimeListItem *type=static_cast<MimeListItem*>(item);
	QValueList<StringDual> bleh;
	
	bleh.append(StringDual(i18n("Author"), type->author()));
	bleh.append(StringDual(i18n("Web Site"), type->url()));
	bleh.append(StringDual(i18n("Extension"), type->extension()));
	bleh.append(StringDual(i18n("Mimetype"), type->mime()));
	
	text->setText(infoPane(type->name(),bleh));
}

QString Types::commaVector(Arts::TraderOffer *t, const char *field, bool limit)
{
	std::vector<std::string> *prop=t->getProperty(field);
	QString str;
	bool comma=false;

	for (std::vector<std::string>::iterator i=prop->begin(); i != prop->end(); i++)
	{
		if (!(*i).length()) continue;
		if (comma)
			str+=", ";
		comma=true;
		str+=(*i).c_str();
		if (limit) break;
	}
	delete prop;
	return str;
}

Types::~Types()
{

}

void Types::save()
{

}


/*****************************************************************
 * General options
 *****************************************************************/

General::General(QObject *parent) : CModule(i18n("Options"), i18n("General Options"), parent)
{
	mLoopList=new QCheckBox(i18n("&Return to Start of Playlist on Finish"), this);
	mLoopList->setChecked(napp->loopList());
	QWhatsThis::add(mLoopList, i18n("When the playlist is done playing, return to the start, but don't start playing."));

	mAutoPlay=new QCheckBox(i18n("&Automatically Play First File"), this);
	mAutoPlay->setChecked(napp->autoPlay());
	QWhatsThis::add(mAutoPlay, i18n("Start playing the playlist as soon as Noatun is started."));

	mOneInstance=new QCheckBox(i18n("&Allow Only One Instance of Noatun"), this);
	mOneInstance->setChecked(napp->oneInstance());
	QWhatsThis::add(mOneInstance, i18n("Starting noatun a second time will cause it to just append items from the start to the current instance."));

	mClearOnOpen = new QCheckBox(i18n("Clear playlist when opening a file"), this);
	mClearOnOpen->setChecked(napp->clearOnOpen());
	QWhatsThis::add(mClearOnOpen, i18n("Opening a file with the global Open menu item will clear the playlist first."));

	mHackUpPlaylist = new QCheckBox(i18n("Process Playlist Items for Display"), this);
	mHackUpPlaylist->setChecked(napp->hackUpPlaylist());

	QWhatsThis::add(mHackUpPlaylist, i18n("Process filenames (e.g. removing spaces and the filename extension) and set them as the playlist item titles."));

	mFastVolume=new QCheckBox(i18n("Use Fast Hardware Volume Control"), this);
	mFastVolume->setChecked(napp->fastMixer());
	QWhatsThis::add(mFastVolume, i18n("Use the hardware mixer instead of aRts's.  It affects all streams, not just Noatun's, but is a little faster."));
		
	QFrame *dlSaverFrame = new QFrame(this);
	QLabel *dlsaver=new QLabel(i18n("&Download Directory"), dlSaverFrame);
	mDlSaver=new KURLRequester(napp->saveDirectory(), dlSaverFrame);
	dlsaver->setBuddy(mDlSaver);
	connect( mDlSaver, SIGNAL( openFileDialog( KURLRequester * )),
		 this, SLOT( slotRequesterClicked( KURLRequester * )));
	QWhatsThis::add(mDlSaver, i18n("When opening a non-local file, download it to the selected directory."));

	QVBoxLayout *layout = new QVBoxLayout(this, KDialog::marginHint(), KDialog::spacingHint());
	layout->addWidget(mLoopList);
	layout->addWidget(mAutoPlay);
	layout->addWidget(mOneInstance);
	layout->addWidget(mClearOnOpen);
	layout->addWidget(mHackUpPlaylist);
	layout->addWidget(mFastVolume);
	layout->addWidget(dlSaverFrame);
	layout->addStretch();

	QHBoxLayout *layoutSaver = new QHBoxLayout(dlSaverFrame, KDialog::marginHint(), KDialog::spacingHint());
	layoutSaver->addWidget(dlsaver);
	layoutSaver->addWidget(mDlSaver);
}

void General::save()
{
	napp->setLoopList(mLoopList->isChecked());
	napp->setAutoPlay(mAutoPlay->isChecked());
	napp->setOneInstance(mOneInstance->isChecked());
	napp->setClearOnOpen(mClearOnOpen->isChecked());
	napp->setSaveDirectory(mDlSaver->url());
	napp->setHackUpPlaylist(mHackUpPlaylist->isChecked());
	napp->setFastMixer(mFastVolume->isChecked());
}

void General::slotRequesterClicked( KURLRequester * )
{
	mDlSaver->fileDialog()->setMode(
		(KFile::Mode)(KFile::Directory | KFile::ExistingOnly | KFile::LocalOnly));
}

#include "cmodule.moc"
