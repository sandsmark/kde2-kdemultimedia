#include <klocale.h>
#include <editor.h>
#include <qlayout.h>
#include <klineedit.h>
#include <qlabel.h>
#include <id3tag.h>
#include <qfile.h>
#include <qcombobox.h>

static const char *labels[]=
{
	"Blues","ClassicRock","Country","Dance","Disco","Funk","Grunge","Hip-Hop",
	"Jazz", "Metal","NewAge","Oldies","Other","Pop","R","Rap","Reggae","Rock",
	"Techno","Industrial","Alternative","Ska","DeathMetal","Pranks","Soundtrack",
	"Euro-Techno","Ambient","Trip-Hop","Vocal","Jazz","Fusion","Trance","Classical",
	"Instrumental","Acid","House","Game","SoundClip","Gospel","Noise","AlternativeRock",
	"Bass","Soul","Punk","Space","Meditative","InstrumentalPop","InstrumentalRock",
	"Ethnic","Gothic","Darkwave","Techno-Industrial","Electronic","Pop-Folk","Eurodance",
	"Dream","SouthernRock","Comedy","Cult","Gangsta","Top40","ChristianRap","Pop",
	"Jungle","NativeUS","Cabaret","NewWave","Psychadelic","Rave","Showtunes",
	"Trailer","Lo-Fi","Tribal","AcidPunk","AcidJazz","Polka","Retro","Musical","Rock",
	"HardRock","Folk","Folk-Rock","NationalFolk","Swing","FastFusion","Bebob",
	"Latin","Revival","Celtic","Bluegrass","Avantgarde","GothicRock","ProgressiveRock",
	"PsychedelicRock","SymphonicRock","SlowRock","BigBand","Chorus","EasyListening",
	"Acoustic","Humour","Speech","Chanson","Opera","ChamberMusic","Sonata","Symphony",
	"BootyBass","Primus","PornGroove","Satire","SlowJam","Club","Tango","Samba",
	"Folklore","Ballad","PowerBallad","RhytmicSoul","Freestyle","Duet","PunkRock",
	"DrumSolo","Acapella","Euro-House","DanceHall","Goa","Drum","Club-House","Hardcore",
	"Terror","Indie","BritPop","Negerpunk","PolskPunk","Beat","ChristianGangstaRap","HeavyMetal",
	"BlackMetal","Crossover","ContemporaryChristian","ChristianRock","Merengue",
	"Salsa","TrashMetal","Anime","Jpop","Synthpop"
};



Editor::Editor(NID3 *i)
	: QWidget(), mID3Loader(i)
{
	QGridLayout *grid = new QGridLayout(this);
	grid->setColStretch(1, 1);
	grid->setColStretch(2, 1);
	grid->setSpacing(4);
	grid->setMargin(4);
	
	mFile=new QLabel(this);
	grid->addMultiCellWidget(mFile, 0, 0, 0, 3);	
	
	QPushButton *buttons;
	
	buttons = new QPushButton(i18n("Close"), this);
	connect(buttons, SIGNAL(clicked()), SLOT(hide()));
	grid->addWidget(buttons, 7, 1);

	buttons = new QPushButton(i18n("Save"), this);
	connect(buttons, SIGNAL(clicked()), SLOT(save()));
	buttons->setDefault(true);
	grid->addWidget(buttons, 7, 2);

	mSaveButton=buttons;
	
	mName = new KLineEdit(this);
	grid->addMultiCellWidget(mName, 1, 1, 1, 2);
	mName->setMaxLength(30);
	connect(mName, SIGNAL(textChanged(const QString&)), SLOT(textChanged(const QString&)));
	
	mArtist = new KLineEdit(this);
	grid->addMultiCellWidget(mArtist, 2, 2, 1, 2);
	mArtist->setMaxLength(30);
	connect(mArtist, SIGNAL(textChanged(const QString&)), SLOT(textChanged(const QString&)));
	
	mAlbum = new KLineEdit(this);
	grid->addMultiCellWidget(mAlbum, 3, 3, 1, 2);
	mAlbum->setMaxLength(30);
	connect(mAlbum, SIGNAL(textChanged(const QString&)), SLOT(textChanged(const QString&)));
	
	mYear = new KLineEdit(this);
	grid->addMultiCellWidget(mYear, 4, 4, 1, 2);
	mYear->setMaxLength(4);
	connect(mYear, SIGNAL(textChanged(const QString&)), SLOT(textChanged(const QString&)));
	
	mGenre = new QComboBox(this);
	grid->addMultiCellWidget(mGenre, 5, 5, 1, 2);
	connect(mGenre, SIGNAL(activated(const QString&)), SLOT(textChanged(const QString&)));
	
	mComment = new KLineEdit(this);
	grid->addMultiCellWidget(mComment, 6, 6, 1, 2);
	mComment->setMaxLength(30);
	connect(mComment, SIGNAL(textChanged(const QString&)), SLOT(textChanged(const QString&)));

	
	QLabel *label;
	
	label = new QLabel(i18n("Title"), this);
	grid->addWidget(label, 1, 0 );
	
	label = new QLabel(i18n("Artist"), this);
	grid->addWidget(label, 2, 0 );
	
	label = new QLabel(i18n("Album"), this);
	grid->addWidget(label, 3, 0 );

	label = new QLabel(i18n("Year"), this);
	grid->addWidget(label, 4, 0 ); 

	label = new QLabel(i18n("Genre"), this);
	grid->addWidget(label, 5, 0 );
			 
	label = new QLabel(i18n("Comment"), this);
	grid->addWidget(label, 6, 0 );

	setFixedHeight(height());

	//add items to the genre list
	mGenre->insertStrList(labels, sizeof(labels)/sizeof(char*));
}


struct id3tag
{
    char field[3];
    char name[30];
    char artist[30];
    char album[30];
    char year[4];
    char comment[30];
    unsigned char genre;
};
   
static QString parse(char *d, int len=30)
{
        char *temp=new char[len+1];
        memcpy(temp, d, len);
        temp[len]='\0';
        QString str(QString::fromLocal8Bit(temp));
        delete [] temp;
        str=str.simplifyWhiteSpace();
        return str;
}

static QString getGenre(unsigned char d)
{

	if (d>=(sizeof(labels)/sizeof(char*)))
		return i18n("(No Genre)");
	return QString(labels[d]);
}

void Editor::open(const QString &filen)
{
    mFile->setText(filen);
    if (filen.right(4).lower()!=".mp3")
        return;
    QFile file(filen);
    if (!file.open(IO_ReadOnly))
        return;
    file.at(file.size()-128);
    id3tag data;

    file.readBlock((char*)&data, sizeof(data));

    QString name, artist, album, year, comment, genre;

	if (parse(data.field, 3)!="TAG") return;

	mName->setText(parse(data.name));
	mArtist->setText(parse(data.artist));
	mAlbum->setText(parse(data.album));
	mComment->setText(parse(data.comment));
	mYear->setText(parse(data.year,4));
	mGenre->setCurrentItem((int)(data.genre));
	mSaveButton->setEnabled(false);
}

void Editor::save()
{
	QFile file(mFile->text());
	if (!file.open(IO_ReadWrite)) return;
	file.at(file.size()-128);
	id3tag data;
	file.readBlock((char*)&data, 3);

	bool append=parse(data.field, 3)!="TAG";

	memcpy(data.field, "TAG", 3);
	QString n=mName->text();
	n=n.leftJustify(30, ' ', true);
	memcpy(data.name, n.local8Bit(), 30);

	n=mArtist->text().leftJustify(30, ' ', true);
	memcpy(data.artist, n.local8Bit(), 30);

	n=mAlbum->text().leftJustify(30, ' ', true);
	memcpy(data.album, n.local8Bit(), 30);

	n=mYear->text().rightJustify(4, ' ', true);
	memcpy(data.year, n.local8Bit(), 4);

	n=mComment->text().leftJustify(30, ' ', true);
	memcpy(data.comment, n.local8Bit(), 30);

	data.genre=(unsigned char)mGenre->currentItem();

	if (append)
		file.at(file.size());
	else
		file.at(file.size()-128);

	QDataStream stream(&file);
	stream.writeRawBytes((char*)&data, sizeof(data));

	mSaveButton->setEnabled(false);

	emit saved();
}

void Editor::textChanged(const QString&)
{
	mSaveButton->setEnabled(true);
}

void Editor::hideEvent(QHideEvent *e)
{
	QWidget::hideEvent(e);
	if (mID3Loader->editor()!=this)
		delete this;
}

QString Editor::name() const
{
	return mName->text();
}

QString Editor::artist() const
{
	return mArtist->text();
}

QString Editor::album() const
{
	return mAlbum->text();
}

QString Editor::year() const
{
	return mYear->text();
}

QString Editor::genre() const
{
	return mGenre->currentText();
}

QString Editor::comment() const
{
	return mComment->text();
}

void Editor::show()
{
	if (mFile->text().right(4)!=".mp3")
		return;
	QWidget::show();

}

#include "editor.moc"
