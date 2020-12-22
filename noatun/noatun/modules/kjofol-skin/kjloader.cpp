#include <qimage.h>
#include "kjloader.h"
#include <qbitmap.h>
#include <qpixmap.h>
#include <qcursor.h>
#include <qpainter.h>
#include <khelpmenu.h>
#include <kstdaction.h>
#include <noatunapp.h>
#include <kpopupmenu.h>
#include <kaction.h>
#include <klocale.h>
#include <player.h>
#include <kfiledialog.h>
#include <string.h>
#include <qvector.h>
#include <kconfig.h>
#include <kglobal.h>
#include <kstddirs.h>
#include <knotifyclient.h>
#include <qtooltip.h>
#include "kjprefs.h"
#include "equalizer.h"
#include <qdragobject.h>
#include <iostream.h>
#include <math.h>
#include <noatunstdaction.h>
#include <stdlib.h>
#include <unistd.h>

class KJToolTip : public QToolTip
{
public:
	KJToolTip(KJLoader *parent)
		: QToolTip(parent), mParent(parent)
	{}

protected:
	virtual void maybeTip(const QPoint &p)
	{
		QList<KJWidget> things=mParent->widgetsAt(p);
		for (KJWidget *i=things.first(); i!=0; i=things.next())
		{
			QString string=i->tip();
			if (string.length())
			{
				tip(i->rect(), string);
				return;
			}
		}
	}

private:
	KJLoader *mParent;
};


static int grayRgb(QRgb r)
{
	return qGray(qRed(r), qGreen(r), qBlue(r));
}

static int isGray(QRgb r)
{
	return (qRed(r)==qGreen(r)) &&
	       (qRed(r) == qBlue(r));
}

KJLoader *KJLoader::kjofol=0;

KJLoader::KJLoader()
	: QWidget(0,"KJLoader", WType_TopLevel | WStyle_NoBorder |WRepaintNoErase),
	  UserInterface(), moving(false), mClickedIn(0), mText(0), mNumbers(0)
{
	kjofol=this;
	
	mTooltips=new KJToolTip(this);
	setCaption("Noatun");
	
	subwidgets.setAutoDelete(true);
	
	KConfig *config=KGlobal::config();
	config->setGroup("KJofol-Skins");
	QString skin=config->readEntry("SkinResource", locate("data", "noatun/kjskins/kjofol/kjofol.rc"));
	if (QFile(skin).exists())
	{
		loadSkin(skin);
	}
	else
	{
		KNotifyClient::event("warning",
			i18n("There was trouble loading skin %1. Please select another skin file.").arg(skin));
		napp->preferences();
	}

	setIcon(BarIcon("noatun"));
	setAcceptDrops(true);
	
	mHelpMenu =new KHelpMenu(this, kapp->aboutData());
	connect( napp->player(), SIGNAL(timeout()), this, SLOT(timeUpdate()));
	connect( napp->player(), SIGNAL(newSong()), this, SLOT(newSong()));
	connect( napp->player(), SIGNAL(hideYourself()), this, SLOT(hide()) );
	connect( napp->player(), SIGNAL(showYourself()), this, SLOT(show()) );

	new KJPrefs(this); 
	QApplication::restoreOverrideCursor();
	newSong();
}

QList<KJWidget> KJLoader::widgetsAt(const QPoint &pt) const
{
	QList<KJWidget> things;
	for (QListIterator<KJWidget> i(subwidgets); i.current(); ++i)
		if ((*i)->rect().contains(pt))
			things.append((*i));
	return things;
		
}

void KJLoader::removeChild(KJWidget *c)
{
	if (mClickedIn==c) mClickedIn=0;
	if (subwidgets.findRef(c) != -1)
		subwidgets.take();
}

void KJLoader::addChild(KJWidget *c)
{
	subwidgets.append(c);
}

// GAMMA CORRECTION SUCKS!!! ROT IN HELL QIMAGEIO!!!

// just like gamma correction, this function is a stupid hack
bool KJLoader::cleanSkin(const QString &dir)
{
	// try to guess the current filetype:
	// BMP, JPEG, PNG
	
	QString ext;
	QDir d(dir);
	
	QStringList files=d.entryList();
	if (files.grep("png", false).count())
		return true;
	
	if (files.grep("bmp", false).count())
		ext="bmp";
	if (files.grep("jpeg", false).count())
		ext="jpeg";
	if (files.grep("jpg", false).count())
		ext="jpg";

	if (::access(dir.latin1(), W_OK)!=0) return false;

	QString command="/bin/sh "+::locate("exe", "noatun_kjofol_fixskin.sh");
	command+=" '"+dir+"' "+ext;
	::system(command.latin1());
	return true;
}

QString KJLoader::guessRcFile(const QString &dir)
{
	QString path=dir;
	if (path[path.length()-1]=='/')
		path=path.left(path.length()-1);

	QString tests=path+"/";
	QString possible=path.mid(path.findRev("/")+1);
	tests+=possible+".rc";
	tests=filenameNoCase(tests);
	if (tests.length())
		return tests;
	
	QDir d(path, "*.rc");
	
	if (d.count()) return path+"/"+d[0];

	return "";
}

void KJLoader::loadSkin(const QString &file)
{
	if (file==mCurrentSkin) return;
	mCurrentSkin=file;
	unloadSkin();
	
	if (!cleanSkin(file.left(file.findRev("/"))))
	{
		KNotifyClient::event("warning",
			i18n("There was trouble loading skin %1. Please select another skin file.").arg(file));
		napp->preferences();
		return;
	}
	
	Parser::open(filenameNoCase(file));

	
	mText = new KJFont("Font", this);
	mNumbers = new KJFont("TimeFont", this);
	
	subwidgets.append(new Background(this));

	if (exist("VolumeControlButton"))
		subwidgets.append(new KJVolumeBMP(item("VolumeControlButton"), this));
	if (exist("FilenameWindow"))
		subwidgets.append(new KJFilename(item("FilenameWindow"), this));
	if (exist("MP3TimeWindow"))
		subwidgets.append(new KJTime(item("MP3TimeWindow"), this));
	if (exist("AnalyzerWindow"))
		subwidgets.append(new KJVis(item("AnalyzerWindow"), this));
	QTimer::singleShot(0, this, SLOT(loadSeeker()));
//	subwidgets.append(new KJScope(item("AnalyzerWindow"), this));
//	subwidgets.append(new KJEqualizer(item("EqualizerWindow"), this));
	
	// all the regular buttons
	for (QDictIterator<QStringList> i(*this); i.current(); ++i)
	{
		QString d=i.currentKey();
		if(d.contains("Button") && (*i).count()==7)
			subwidgets.append(new KJButton(*i, this)); 
			
	}
	show();
	conserveMemory();

//	if (napp->player()->isPlaying())
//		newSong();
}

void KJLoader::loadSeeker()
{
	subwidgets.append(new KJSeeker(item("SeekRegion"), this));
}

void KJLoader::unloadSkin()
{
	subwidgets.clear();
	delete mText;
	delete mNumbers;	
}

void KJLoader::minimize()
{
//	setWFlags(WType_TopLevel);
	showMinimized();
}

void KJLoader::closeEvent(QCloseEvent*)
{
	unload();
}

void KJLoader::dragEnterEvent(QDragEnterEvent *event)
{
	// accept uri drops only
	event->accept(QUriDrag::canDecode(event));
}

void KJLoader::dropEvent(QDropEvent *event)
{
	QStrList uri;
	if (QUriDrag::decode(event, uri))
	{
		for (char *file=uri.first(); file != 0; file=uri.next() )
			napp->player()->openFile(KURL(file), false);
	}
}

void KJLoader::wheelEvent(QWheelEvent *e)
{
	int delta=e->delta();
	napp->player()->setVolume(napp->player()->volume()+(delta/60));
}

KJLoader::~KJLoader()
{
	delete mHelpMenu;
	delete mTooltips;
}

void KJLoader::paintEvent(QPaintEvent *e)
{
	QPainter p(this);
	for (KJWidget* i=subwidgets.first(); i!=0; i=subwidgets.next())
		if (i->rect().intersects(e->rect()))
			i->paint(&p, e->rect().intersect(i->rect()));
//	QWidget::paintEvent(e);
}

void KJLoader::mouseMoveEvent(QMouseEvent *e)
{
	if (moving)
		move(QCursor::pos()-mMousePoint);
	QWidget::mouseMoveEvent(e);

	if (!moving && mClickedIn && subwidgets.findRef(mClickedIn)!=-1)
		mClickedIn->mouseMove(e->pos()-mClickedIn->rect().topLeft(),
		                      mClickedIn->rect().contains(
			                      mapFromGlobal(QCursor::pos())));
}

void KJLoader::mousePressEvent(QMouseEvent *e)
{
	QWidget::mousePressEvent(e);
	raise();
	setActiveWindow();
	
	if (e->button()==RightButton)
		NoatunStdAction::ContextMenu::showContextMenu();
	else
	{
	
		mMousePoint=mapFromGlobal(QCursor::pos());
		// try to find a KJWidget that is here
		for (KJWidget* i=subwidgets.first(); i!=0; i=subwidgets.next())
			if (i->rect().contains(mMousePoint))
			{
				if (i->mousePress(mMousePoint-i->rect().topLeft()))
				{
					mClickedIn=i;
					return;
				}
			}
		
		// can't find it, so move the window	
		moving=true;
	}
}

void KJLoader::mouseReleaseEvent(QMouseEvent *e)
{
	QWidget::mouseReleaseEvent(e);
	
	if (!moving && mClickedIn && subwidgets.findRef(mClickedIn)!=-1)
	{
		mClickedIn->mouseRelease(mapFromGlobal(QCursor::pos())-
		                                       mClickedIn->rect().topLeft(),
		                         mClickedIn->rect().contains(
			                         mapFromGlobal(QCursor::pos())));
		mClickedIn=0;
	}

	moving=false;
}

void KJLoader::timeUpdate()
{
	for (KJWidget* i=subwidgets.first(); i!=0; i=subwidgets.next())
		i->timeUpdate(napp->player()->getTime()/1000); // pass seconds to all Widgets
}

void KJLoader::newSong()
{
	if (!napp->player()->current()) return;
	for (KJWidget* i=subwidgets.first(); i!=0; i=subwidgets.next())
		i->newFile();
}


/*******************************************
 * KJWidget
 *******************************************/

KJWidget::KJWidget(KJLoader *p) : mParent(p)
{
}

// only works little endian
inline void setPixel1BPP(QImage &image, int x, int y, bool value)
{
	if (value)
		*(image.scanLine(y) + (x >> 3)) |= 1 << (x & 7);
	else
		*(image.scanLine(y) + (x >> 3)) &= ~(1 << (x & 7));	
}


QBitmap KJWidget::getMask(const QImage &_rect, register QRgb transparent)
{

	QImage result(_rect.width(), _rect.height(), 1,2, QImage::LittleEndian);
	result.setColor(0, qRgb(0,0,0));
	result.setColor(1, qRgb(255,255,255));

	for (int height=0;height<_rect.height(); height++)
	{
		for (int width=0; width<_rect.width(); width++)
			setPixel1BPP(result, width, height,
			             _rect.pixel(width, height)!=transparent);
	}

	QBitmap bm;
	bm.convertFromImage(result);
	return bm;

		/*
	register QRgb magenta=QColor(255,0,255).rgb();
	QImage rect=_rect.convertDepth(32);
	register int width=rect.width();
	register int height=rect.height();

	QImage result(width, height, 1,2, QImage::LittleEndian);
	result.setColor(0, qRgb(0,0,0));
	result.setColor(1, qRgb(255,255,255));
	
	for (register int cheight=height; cheight!=0; --cheight)
	{
		register QRgb *srcline=(QRgb*)rect.scanLine(cheight);
		register QRgb *resline=(QRgb*)result.scanLine(cheight);
		for (register int cwidth=width; cwidth!=0; --cwidth)
			setPixel1BPP(result, cwidth, cheight,srcline[cwidth]==magenta);
	}		
		
	QBitmap bm;
	bm.convertFromImage(result);
	return bm;
*/
}

void KJWidget::repaint(bool me, const QRect &r, bool clear)
{
	QPainter p(parent());
	if (me)
		paint(&p, r.isValid() ? r : rect());
	else
		parent()->repaint(r.isValid() ? r : rect(), clear);		
}

QString KJWidget::backgroundPressed(const QString &bmp)
{
	return parser()["BackgroundImagePressed"+QString::number(bmp.mid(3).toInt())][1];
}

/*******************************************
 * Background
 *******************************************/

Background::Background(KJLoader *parent)
	: KJWidget(parent)
{
	QImage ibackground;
	mBackground=parent->pixmap(parser()["BackgroundImage"][1]);
	ibackground=parent->image(parser()["BackgroundImage"][1]);
	parent->setMask(getMask(ibackground));
	parent->setFixedSize(QSize(mBackground.width(), mBackground.height()));

	setRect(0,0,parent->width(),parent->height());
}

void Background::paint(QPainter *painter, const QRect &rect)
{
	QPaintDevice *dev=painter->device();
	bitBlt(dev, rect.topLeft(), &mBackground, rect, Qt::CopyROP);
//	bitBlt(dev, rect.topLeft(), parent()->pixmap("SeekImage", 1), rect, Qt::CopyROP);
}

/*******************************************
 * KJButton
 *******************************************/

KJButton::KJButton(const QStringList &i, KJLoader *parent)
	: KJWidget(parent), mTitle(i[0]), mShowPressed(false) 
{
	mPushedPixmap=(i.count()==7);
	
	// get the rectangle
	int x, y, xs, ys;
	x=i[1].toInt();
	y=i[2].toInt();
	xs=i[3].toInt()-x;
	ys=i[4].toInt()-y;
	setRect(x,y,xs,ys);

	mPressed=parent->pixmap(backgroundPressed(i[6]));
}

QString KJButton::tip()
{
	QString str;
	if (mTitle=="CloseButton")
		str=i18n("Close");
	else if (mTitle=="MinimizeButton")
		str=i18n("Minimize");
	else if (mTitle=="AboutButton")
		str=i18n("About");
	else if (mTitle=="StopButton")
		str=i18n("Stop");
	else if (mTitle=="PlayButton")
		str=i18n("Play");
	else if (mTitle=="PauseButton")
		str=i18n("Pause");
	else if (mTitle=="OpenFileButton")
		str=i18n("Open");
	else if (mTitle=="PlaylistButton")
		str=i18n("Playlist");
	else if (mTitle=="RepeatButton")
		str=i18n("Loop");
	else if (mTitle=="EqualizerButton")
		str="";
	else if (mTitle=="NextSongButton")
		str=i18n("Next");
	else if (mTitle=="PreviousSongButton")
		str=i18n("Previous");
	else if (mTitle=="PreferencesButton")
		str=i18n("Preferences");
	return str;
}

void KJButton::paint(QPainter *, const QRect &)
{
	if (mShowPressed)
		bitBlt(parent(), rect().topLeft(), &mPressed, rect(), Qt::CopyROP);
}

bool KJButton::mousePress(const QPoint &)
{
	bitBlt(parent(), rect().topLeft(), &mPressed, rect(), Qt::CopyROP);
	return true;
}

void KJButton::showPressed(bool b)
{
	mShowPressed=b;
	repaint();
}

void KJButton::mouseRelease(const QPoint &, bool in)
{
	repaint(false);
	if (!in)
		return;

	// now, find what widget I am and do the proper action
	if (mTitle=="CloseButton")
		parent()->close();
	else if (mTitle=="MinimizeButton")
		parent()->minimize();
	else if (mTitle=="AboutButton")
		parent()->helpMenu()->aboutApplication();
	else if (mTitle=="StopButton")
		napp->player()->stop();
	else if (mTitle=="PlayButton")
		napp->player()->play();
	else if (mTitle=="PauseButton")
		napp->player()->playpause();
	else if (mTitle=="OpenFileButton")
	{
		KURL file(KFileDialog::getOpenURL(0, napp->mimeTypes(), parent(),
		                                  i18n("Select a File to Play")));
		if (!file.isMalformed())
			napp->player()->openFile(file);
	}
	else if (mTitle=="PlaylistButton")
		napp->player()->toggleListView();
	else if (mTitle=="RepeatButton")
	{
		napp->player()->loop(napp->player()->loopStyle()==Player::None
		                         ? Player::Song : Player::None);
		showPressed(napp->player()->loopStyle());
	}
	else if (mTitle=="EqualizerButton")
		;
	else if (mTitle=="NextSongButton")
		napp->player()->fastForward();
	else if (mTitle=="PreviousSongButton")
		napp->player()->back();
	else if (mTitle=="PreferencesButton")
		napp->preferences();
}


KJSeeker::KJSeeker(const QStringList &i, KJLoader *l) : KJWidget(l), g(0)
{
	mActive=parent()->image(backgroundPressed("BMP1"));
	mScale=parent()->image(parser()["SeekImage"][1]);
	QImage pixmapNoPress=parent()->image(parser()["BackgroundImage"][1]);

	int x, y, xs, ys;
	x=i[1].toInt();
	y=i[2].toInt();
	xs=i[3].toInt()-x;
	ys=i[4].toInt()-y;
	setRect(x,y,xs,ys);
	QImage transmask(xs, ys, 1, 2, QImage::LittleEndian);

	transmask.setColor(0, qRgb(0,0,0));
	transmask.setColor(1, qRgb(255,255,255));
	
	
	QImage *barmodeImages[256];
	memset(barmodeImages, 0, 256*sizeof(QImage*));
	memset(barmode, 0, 256*sizeof(QPixmap*));
	// Now do the pixel føking
	for (int iy=y;iy<y+ys; iy++)
		for (int ix=x;ix<x+xs; ix++)
		{
			QRgb checkmScale=mScale.pixel(ix, iy);
			// am I transparent?
			if (!isGray(checkmScale))
			{
				setPixel1BPP(transmask, ix-x, iy-y, 0);
				continue;
			}
			setPixel1BPP(transmask, ix-x, iy-y, 1);

			// what is the level
			int level=grayRgb(checkmScale)+1;
			if (level>255) level=255;
			// allocate the pixmap of the level proper
			// copy the color to the surface proper
			QRgb activeColor=mActive.pixel(ix,iy);
			QRgb inactiveColor=pixmapNoPress.pixel(ix,iy);
			// set this pixel and everything before it
			for(int i=0; i<level; i++)
			{
				if (!barmodeImages[i])
					barmodeImages[i]=new QImage(xs,ys, 32);
				QRgb *l=(QRgb*)barmodeImages[i]->scanLine(iy-y);
				l[ix-x]=inactiveColor;
			}
	
			do
			{
				if (!barmodeImages[level])
					barmodeImages[level]=new QImage(xs,ys, 32);
				QRgb *l=(QRgb*)barmodeImages[level]->scanLine(iy-y);
				l[ix-x]=activeColor;
			}
			while (level++<255);
			napp->processEvents();
		}

	for (int i=1; i<256; i++)
	{
		if (!barmodeImages[i])
			continue;
		barmode[i]=new QPixmap(xs,ys);
		barmode[i]->convertFromImage(*barmodeImages[i]);
		delete barmodeImages[i];
		napp->processEvents();
	}
	// create the blank one
	barmode[0]=new QPixmap(xs, ys);
	QPixmap px=parent()->pixmap(parser()["BackgroundImage"][1]);
	bitBlt(barmode[0], 0, 0, &px, x, y, xs, ys, Qt::CopyROP);
	
	px.convertFromImage(transmask);
	barModeMask=px;
}

KJSeeker::~KJSeeker()
{
	for (int i=0; i<256; i++)
		if (barmode[i])
			delete barmode[i];
}

void KJSeeker::paint(QPainter *p, const QRect &)
{
	closest();
	barmode[g]->setMask(barModeMask);
	bitBlt(p->device(), rect().topLeft().x(), rect().topLeft().y(), barmode[g], 0, 0, rect().width(), rect().height(), Qt::CopyROP);
}

bool KJSeeker::mousePress(const QPoint &pos)
{
	return (isGray(mScale.pixel(rect().topLeft().x()+pos.x(), rect().topLeft().y()+pos.y())));
}

void KJSeeker::mouseRelease(const QPoint &pos, bool in)
{
	QRgb color=mScale.pixel(rect().topLeft().x()+pos.x(), 
					         rect().topLeft().y()+pos.y());
	if (!isGray(color) || !in)
		return;
	
	g=grayRgb(color);
	repaint();

	napp->player()->skipTo(g*napp->player()->getLength()/255);
	return;

}

void KJSeeker::timeUpdate(int sec)
{
	int length = napp->player()->getLength() / 1000;
	if (length<1)
		length=1;

	if (sec > length)
		sec = length;
	else if ( sec < 0 )
		sec=0;

	g = sec * 255 / length;
//	fprintf(stderr,"sec: %d, len: %d, g: %d\n", sec, length, g);
	QPainter p(parent());
	paint(&p, rect());
}

void KJSeeker::closest()
{
	int south=g, north=g;
	bool southtried=false, northtried=false;
	while (!barmode[south] && !barmode[north])
	{
		if (southtried && northtried) { g=0; return; }
		south--;
		north++;
		if (north>255) {northtried=true; north=g;}
		if (south<0) {southtried=true; south=g;}
	}
	if (barmode[south])
		g=south;
	else if (barmode[north])
		g=north;
}

KJVolumeBMP::KJVolumeBMP(const QStringList &i, KJLoader *p)
	: KJWidget(p), mVolume(0)
{
	int x, y, xs, ys;
	x=i[1].toInt();
	y=i[2].toInt();
	xs=i[3].toInt()-x;
	ys=i[4].toInt()-y;
	setRect(x,y,xs,ys);

	mWidth=parser()["VolumeControlImageXSize"][1].toInt();
	mCount=parser()["VolumeControlImageNb"][1].toInt()-1;

	mImages=parent()->pixmap(parser()["VolumeControlImage"][1]);
	mPos=parent()->image(parser()["VolumeControlImagePosition"][1]);
}

QString KJVolumeBMP::tip()
{
	return i18n("Volume");
}

void KJVolumeBMP::paint(QPainter *p, const QRect &)
{
	QRect from(mVolume*mCount/100*mWidth, 0, mWidth, mImages.height());
	bitBlt(p->device(), rect().topLeft(), &mImages, from, Qt::CopyROP);
}

bool KJVolumeBMP::mousePress(const QPoint &pos)
{
	QRgb color=mPos.pixel(rect().topLeft().x()+pos.x(),
	                       rect().topLeft().y()+pos.y());
	if (!isGray(color))
		return false;
		 
	mVolume=grayRgb(color)*100/255;
	napp->player()->setVolume(mVolume);

	repaint();
	return true;
}

void KJVolumeBMP::mouseRelease(const QPoint &, bool)
{}

void KJVolumeBMP::mouseMove(const QPoint &pos, bool in)
{
	if (!in) return;
	mousePress(pos);
}

void KJVolumeBMP::timeUpdate(int)
{
	mVolume=napp->player()->volume();
	repaint();
}


KJFont::KJFont(const QString &prefix, KJLoader *parent)
{
	if (prefix=="TimeFont")
	{
		mString[0]="0123456789: ";
		mString[1]=mString[2]=0;
		mNullChar=' ';
	}
	else
	{
		mString[0]="abcdefghijklmnopqrstuvwxyz\"@";
		mString[1]="0123456789;_:()-'!_+\\/[]*&%.=$#";
		mString[2]="åöä?*,                          ";
		mNullChar=' ';
	}
	
	mText=parent->pixmap(parent->item(prefix+"Image")[1]);
	mWidth=parent->item(prefix+"Size")[1].toInt();
	mHeight=parent->item(prefix+"Size")[2].toInt();
	mSpacing=parent->item(prefix+"Spacing")[1].toInt();
	mTransparent=(bool)parent->item(prefix+"Transparent")[1].toInt();
}

QPixmap KJFont::draw(const QCString &str, int wide, const QPoint &pos) const
{
	QPoint to(pos);
	QCString string=str.lower();
	QPixmap region(
		(string.length()*mWidth+string.length()*mSpacing > (unsigned int)wide
			? string.length()*mWidth+string.length()*mSpacing : wide),
		mHeight);
	
	for (unsigned int len=0; len<string.length(); len++)
	{
		char c=string[len];
		drawCharacter(&region, to, c);
		
		to+=QPoint(mWidth, 0);
		// draw according to "spacing"
		QPoint p=charSource(' ');
		bitBlt(&region, to, &mText, QRect(p.x(), p.y(), mSpacing, mHeight), Qt::CopyROP);
		to+=QPoint(mSpacing, 0);
	}
	for (; to.x()<=wide; to+=QPoint(mWidth, 0))
		drawCharacter(&region, to, ' ');

	return region;
}

void KJFont::drawCharacter(QPixmap *dev, const QPoint &to, char c) const
{
	QPoint src=charSource(c);
	int x=src.x();
	int y=src.y();
	int xs=mWidth;
	int ys=mHeight;
	
	bitBlt(dev, to, &mText, QRect(x,y,xs,ys), Qt::CopyROP);

}

#include <string.h>

QPoint KJFont::charSource(char c) const
{
	for (int i=0; i<3; i++)
	{
		const char *pos=strchr(mString[i], c);
		
		if (!pos) continue;
		return QPoint(mWidth*((int)(pos-mString[i])), mHeight*i);
	}

	return charSource(mNullChar);
}

KJFilename::KJFilename(const QStringList &l, KJLoader *p)
	: QObject(0), KJWidget(p)
{
	int x=l[1].toInt();
	int y=l[2].toInt();
	int xs=l[3].toInt()-x;
	int ys=l[4].toInt()-y;
	setRect(x,y,xs,ys);

	mDistance=2; // how far it moves per cycle
	
	prepareString(i18n("No File").latin1());
	killTimers();
}

void KJFilename::paint(QPainter *p, const QRect &)
{
	bitBlt(p->device(), rect().topLeft(), &mView, 
	       QRect(0,0, rect().width(), rect().height()), Qt::CopyROP);
}

void KJFilename::timerEvent(QTimerEvent *)
{
	int height=mView.height();
	int width=mView.width();
	QPixmap cycle(1, height);
	bitBlt(&cycle, 0,0, &mView, 0,0, 1, height, Qt::CopyROP);
	bitBlt(&mView, 0,0, &mView, 1, 0, width-1, height, Qt::CopyROP);
	bitBlt(&mView, width-1, 0, &cycle, 0,0, 1, height, Qt::CopyROP);
	repaint();
}

bool KJFilename::mousePress(const QPoint &)
{
	return true;
}

void KJFilename::prepareString(const QCString &str)
{
	killTimers();
	mView=textFont().draw(str, rect().width());
	startTimer(100);
}

void KJFilename::newFile()
{
	QCString timestring=napp->player()->lengthString().latin1();
	timestring=timestring.mid(timestring.find('/')+1);
	prepareString(QCString(napp->player()->current()->title().latin1())
	              + " (" + timestring + ")   ");
}

KJTime::KJTime(const QStringList &l, KJLoader *p)
	: KJWidget(p)
{
	int x=l[1].toInt();
	int y=l[2].toInt();
	int xs=l[3].toInt()-x;
	int ys=l[4].toInt()-y;
	setRect(x,y,xs,ys);

	prepareString("00:00");
}

void KJTime::paint(QPainter *p, const QRect &)
{
	bitBlt(p->device(), rect().topLeft(), &mTime, 
	       QRect(0,0, rect().width(), rect().height()), Qt::CopyROP);
}

bool KJTime::mousePress(const QPoint &)
{
	return true;
}

void KJTime::timeUpdate(int)
{
	if (!napp->player()->current())
		return;
	QCString time=napp->player()->lengthString().latin1();
	time=time.left(time.find('/'));

	prepareString(time);
}

void KJTime::prepareString(const QCString &str)
{
	if (str==mCurrentStr) return;
	mTime=timeFont().draw(str, rect().width());
	repaint();
	mCurrentStr=str;
}

void KJVisScope::swapScope(Visuals newOne)
{
	QStringList line=parent()->item("AnalyzerWindow");
	KJLoader *p=parent();
	p->removeChild(this);
	delete this;
	
	KJWidget *w;
	switch (newOne)
	{
	case Null:
		w=new KJNullScope(line, p);
		break;
	case FFT:
		w=new KJVis(line, p);
		break;
	case Mono:
		w=new KJScope(line, p);
	};

	p->addChild(w);
}

KJNullScope::KJNullScope(const QStringList &l, KJLoader *parent)
	: KJVisScope(parent)
{
	int x=l[1].toInt();
	int y=l[2].toInt();
	int xs=l[3].toInt()-x;
	int ys=l[4].toInt()-y;
	setRect(x,y,xs,ys);
}

bool KJNullScope::mousePress(const QPoint &)
{
	parent()->repaint(rect(), false);
	swapScope(FFT);
	
	return true;
}

KJVis::KJVis(const QStringList &l, KJLoader *parent)
	: KJVisScope(parent), MonoFFTScope(100)
{
	int x=l[1].toInt();
	int y=l[2].toInt();
	int xs=l[3].toInt()-x;
	int ys=l[4].toInt()-y;
	QStringList &col=parser()["AnalyzerColor"];
	mColor.setRgb(col[1].toInt(), col[2].toInt(), col[3].toInt());	

	start();
	
	int w=bands();
	int wmult=w;
	while (w<xs)
		w+=wmult;

	// center the difference
	x+=(xs-w)/2;
	xs=w;
	
	mMultiples=w/wmult;
	setRect(x,y,xs,ys);

}

void KJVis::scopeEvent(float *d, int size)
{
	QPainter p(parent());
	int x=rect().x();
	int y=rect().y();
	int height=rect().height();
	float *start=d;
	float *end=d+size;
	float fheight=(float)height;
	parent()->repaint(rect(), false);
	for ( ; start<end; ++start)
	{
		// d[i]     amp
		// 1.5  =  height
		float n=(*start)+1.0;
		n=log(n)*fheight*8;
		int amp=(int)n;
		if (amp<0) amp=0;
		if (amp>height) amp=height;
		p.fillRect(x, y+(height-amp), mMultiples, amp, mColor);
		x+=mMultiples;

	}
}

bool KJVis::mousePress(const QPoint &)
{
	parent()->repaint(rect(), false);
	swapScope(Mono);
	
	return true;
}

KJScope::KJScope(const QStringList &l, KJLoader *parent)
	: KJVisScope(parent), MonoScope(100)
{
	int x=l[1].toInt();
	int y=l[2].toInt();
	int xs=mWidth=l[3].toInt()-x;
	int ys=mHeight=l[4].toInt()-y;
	QStringList &col=parser()["AnalyzerColor"];
	mColor.setRgb(col[1].toInt(), col[2].toInt(), col[3].toInt());	
	setRect(x,y, xs, ys);

	start();
	// set the samplewidth to the largest integer divisible by mWidth
	setSamples(mWidth-1);
}

void KJScope::scopeEvent(float *d, int size)
{
	QPainter p(parent());
	p.setPen(mColor);
	float *end=d+size;
	parent()->repaint(rect(), false);
	int x=rect().x();
	int y=rect().y();
	int heightHalf=mHeight/2;
	y+=heightHalf;

	while (d<end)
	{
		// clip
		float n=*d;
	
		n *= heightHalf*1.5;
		int amp=(int)n;
		if (amp>heightHalf) amp=heightHalf;
		else if (amp<-heightHalf) amp=-heightHalf;

		p.drawLine(x, y, x, y+amp);
		d++;
		x++;
	}
}


bool KJScope::mousePress(const QPoint &)
{
	parent()->repaint(rect(), false);
	swapScope(Null);
	return true;
}

KJEqualizer::KJEqualizer(const QStringList &l, KJLoader *parent)
	: QObject(0), KJWidget(parent)
{
	int x=l[1].toInt();
	int y=l[2].toInt();
	int xs=l[3].toInt()-x;
	int ys=l[4].toInt()-y;
	setRect(x,y,xs,ys);

	mBars=parent->pixmap(parser()["EqualizerBmp"][3]);

	mBands=l[6].toInt();
	mXSpace=l[7].toInt();
	mBuffer.resize(rect().size());

	mXSize=parser()["EqualizerBmp"][1].toInt();
	mNumber=parser()["EqualizerBmp"][2].toInt();

}

int KJEqualizer::barNum(const QPoint &pos) const
{
	int x=pos.x();
	x = x/ mXSpace;
	return napp->equalizer()->bandCount()*x / mBands;
}

int KJEqualizer::level(const QPoint &pos) const
{
	int y=-pos.y();
	y+=rect().height()/2;
	y*=200/(rect().height()/2);
	return y;
}

void KJEqualizer::paint(QPainter *, const QRect &)
{
	
}

void KJEqualizer::updateBuffer()
{
	QPainter paint(&mBuffer);
	for (int band=0; band< mBands; band++)
	{
		
		
	}
		
}

void KJEqualizer::mouseMove(const QPoint &pos, bool in)
{
	if (!in) return;

	napp->equalizer()->band(barNum(pos))->setLevel(level(pos));
}

bool KJEqualizer::mousePress(const QPoint &p)
{
	mouseMove(p, true);
	return true;
}


#include "kjloader.moc"

