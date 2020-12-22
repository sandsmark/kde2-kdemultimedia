#include "kjprefs.h"
#include "kjloader.h"

#include <qcombobox.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qvariant.h>
#include <qtooltip.h>
#include <qwhatsthis.h>
#include <klocale.h>
#include <kglobal.h>
#include <kstddirs.h>
#include <kfiledialog.h>
#include <netaccess.h>
#include <kprocess.h>
#include <knotifyclient.h>
#include <qpixmap.h>
#include "parser.h"
#include "kjloader.h"

KJPrefs::KJPrefs(QObject* parent)
	: CModule(i18n("K-Jofol Skins"), i18n("Skin Selection For the K-Jofol Plugin"), parent)
{
	QVBoxLayout *vbox = new QVBoxLayout( this ); 
    vbox->setSpacing( 6 );
    vbox->setMargin( 11 );

	QHBoxLayout *hbox = new QHBoxLayout; 
	hbox->setSpacing( 6 );
	hbox->setMargin( 0 );

	mSkins = new QComboBox(false, this);
	hbox->addWidget(mSkins);

//	mAdd = new QPushButton(this);
//	mAdd->setText(i18n("&Install More..."));
//	mAdd->setSizePolicy(QSizePolicy((QSizePolicy::SizeType)0,(QSizePolicy::SizeType)0,
//	                    mAdd->sizePolicy().hasHeightForWidth()));
//	hbox->addWidget(mAdd);
	vbox->addLayout(hbox);

	QGridLayout *grid = new QGridLayout;
	grid->setSpacing(6);
	grid->setMargin(0);

	mPreview = new QLabel(this);
	mPreview->setScaledContents(true);

    grid->addWidget(mPreview, 0, 1 );
    QSpacerItem* spacer = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    grid->addItem( spacer, 0, 0 );
    QSpacerItem* spacer_2 = new QSpacerItem( 20, 20, QSizePolicy::Expanding, QSizePolicy::Minimum );
    grid->addItem( spacer_2, 0, 2 );
    QSpacerItem* spacer_3 = new QSpacerItem( 20, 20, QSizePolicy::Minimum, QSizePolicy::Expanding );
    grid->addItem( spacer_3, 1, 1 );
    vbox->addLayout( grid );

	connect(mSkins, SIGNAL(activated(const QString&)), SLOT(showPreview(const QString&)));
//	connect(mAdd, SIGNAL(clicked()), SLOT(add()));
	reopen();
}

void KJPrefs::add()
{
#if 0
	KURL file=KFileDialog::getOpenURL(0, "*.tar.gz *.tar.bz2 *.zip");
	QString target;
	if (KIO::NetAccess::download(file, target))
	{
		KNotifyClient::event("warning", i18n("File %s couldn't be opened").
		                                arg(file.path()));
		return;
	}

	QString uncompressDir=locateLocal("data", "noatun/skins/"+
	                                          target.left(target.findRev('.'))+'/');
	KProcess uncompressor;
	if (target.right(4)==".zip")

	
		
		
	KIO::NetAccess::removeTempFile(target);
#endif
	
}



void KJPrefs::reopen()
{
	QStringList skins;
	
	{
		QStringList skindirs=KGlobal::dirs()->findDirs("data", "noatun/kjskins");
		for (uint i=0; i<skindirs.count(); ++i)
		{
			QStringList files=QDir(skindirs[i]).entryList();
			
			for (uint i2=0; i2<files.count(); i2++)
				if (files[i2].left(1)!=".")
					skins+= (skindirs[i] + files[i2]);
		}
	}
	KGlobal::config()->setGroup("KJofol-Skins"); 
	QString loaded=KGlobal::config()->readEntry("SkinResource", "kjofol");

	mSkins->clear();
	loaded=loaded.mid(loaded.findRev("/")+1);
	loaded=loaded.left(loaded.find(".rc"));
	int index;
	for (QStringList::Iterator i=skins.begin(); i!=skins.end(); ++i)
	{
		*i=(*i).mid((*i).findRev("/")+1);
		mSkins->insertItem(*i);
		if ((*i)==loaded)
			index=mSkins->count()-1;
	}
	mSkins->setCurrentItem(index);
	showPreview(mSkins->currentText());
}

static QString expand(QString s)
{
	s=KGlobal::dirs()->findAllResources("data", "noatun/kjskins/"+s+"/*")[0];
	s=s.left(s.findRev("/"));
	s=KJLoader::guessRcFile(s);
	return s;
}

void KJPrefs::save()
{
	QString skin=::expand(mSkins->currentText());

	KConfig *config=KGlobal::config();
	config->setGroup("KJofol-Skins");
	config->writeEntry("SkinResource", skin);
	config->sync();
	KJLoader *l=KJLoader::kjofol;
	if (l)
		l->loadSkin(skin);
}

void KJPrefs::showPreview(const QString &_skin)
{
	QString skin= ::expand(_skin);
	
	Parser p;
	p.open(skin);
	QImage image=p.image(p["BackgroundImage"][1]);
	if (!image.isNull())
	{
		mPixmap.convertFromImage(image);
		mPixmap.setMask(KJWidget::getMask(image));
	}
	else
		mPixmap=QPixmap();
	mPreview->setPixmap(mPixmap);
}


QString filenameNoCase(const QString &filename, int badNodes)
{
	QStringList names=QStringList::split('/', filename);
	QString full;
	int number=(int)names.count();
	for (QStringList::Iterator i=names.begin(); i!=names.end(); ++i)
	{
		full+="/";
		if (number<=badNodes)
		{
			QDir d(full);
			QStringList files=d.entryList();
			files=files.grep(QRegExp("^"+ (*i) + "$", false));
			if (!files.count())
				return "";
			*i=files.grep(*i, false)[0];
		}

		full+=*i;

		number--;
	}

	if (filename.right(1)=="/")
		full+="/";
	return full;
}

#include "kjprefs.moc"


