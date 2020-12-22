#include "cmodule.h"
#include "downloader.h"
#include "effects.h"
#include "effectview.h"
#include "engine.h"
#include "equalizer.h"
#include "equalizerview.h"
#include "noatunapp.h"
#include "noatunplaylist.h"
#include "noatunpref.h"
#include "player.h"
#include "plugin.h"
#include "pluginloader.h"

#include <common.h>
#include <dcopobject.h>
#include <dispatcher.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <kconfig.h>
#include <kcrash.h>
#include <kdebug.h>
#include <kfiledialog.h>
#include <kglobal.h>
#include <klibloader.h>
#include <klocale.h>
#include <kmainwindow.h>
#include <knotifyclient.h>
#include <kpopupmenu.h>
#include <png.h>
#include <qfile.h>
#include <qimage.h>
#include <qiomanager.h>
#include <qsessionmanager.h>
#include <qtextstream.h>
#include <signal.h>
#include <kmimetype.h>


using std::string;
using std::vector;

NoatunApp::NoatunApp()
	: KUniqueApplication(true, true, true), mPluginMenu(0), mEqualizer(0)
{
	Visualization::internalVis=true;

	mDownloader=new Downloader;
	
	Visualization::initDispatcher(); // 316
	
	// set the default config data
	KConfig *config=KGlobal::config(); // +
	config->setGroup(""); // 1
	if (!config->hasKey("Modules"))
	{
		QStringList modules;
		modules.append("excellent.plugin");
		modules.append("splitplaylist.plugin");
		modules.append("marquis.plugin");
		modules.append("systray.plugin");
		modules.append("id3tag.plugin");
		modules.append("oggtag.plugin");
		config->writeEntry("Modules", modules);
	} // 1
	// for version continuity in the future
	config->writeEntry("Version", QString(version())); // 1
	config->sync(); // 40

	mPref=new NoatunPreferences(0); // 115
	mPref->hide(); // 1
	mLibraryLoader = new LibraryLoader; // 0

	mLibraryLoader->add("dcopiface.plugin");
		
	new General(this); // 25
	new Plugins(this); // 149
//	new Types(this);

	mPlayer=new Player; // 139
	mEffects=new Effects; // 1
	mEqualizer=new Equalizer; // 0
	mEqualizer->init();  // 41
	mEffectView=new EffectView; // 859
	mEqualizerView=new EqualizerView; // 24

	if(isRestored())
	{
		mPlayer->engine()->setInitialized();
		mLibraryLoader->add("marquis.plugin");
		static_cast<SessionManagement*>(mLibraryLoader->plugins().first())->restore();
	}
	else
	{
		loadPlugins(); //  1531
		config->setGroup(""); // 0
		player()->setVolume(config->readNumEntry("Volume", 100)); // 10
		player()->loop(config->readNumEntry("LoopStyle", (int)Player::None));
		mPlayer->engine()->setInitialized(); // 0
		if(autoPlay()) player()->play(); // 118
	}
}

NoatunApp::~NoatunApp()
{
	KConfig *config = KGlobal::config();
	config->setGroup("");
	config->writeEntry("Volume", player()->volume());
	config->writeEntry("LoopStyle", player()->loopStyle());

	mPlayer->stop();
	delete mPref;
	config->sync();
	delete mLibraryLoader;
	KCrash::setCrashHandler(NoatunApp::cleverKCrashHack);
	delete mDownloader;
	delete mEffectView;
	delete mEqualizer;
	delete mEqualizerView;
	delete mEffects;
	delete mPlayer;
}

QCString NoatunApp::version() const
{
	return aboutData()->version().ascii();
}

inline bool logicalXOR(bool A, bool B)
{
	return (A || B) && !(A && B);
}

#define READBOOLOPT_EX(name, string, def, group, reversal) \
bool NoatunApp::name() const \
{ \
	KConfig *config=KGlobal::config(); \
	config->setGroup(group); \
	return logicalXOR(config->readBoolEntry(string, def), reversal); \
}

#define READBOOLOPT(name, string, def) READBOOLOPT_EX(name, string, def, "", false)


READBOOLOPT(autoPlay, "AutoPlay", true)
READBOOLOPT(loopList, "LoopList", true)
READBOOLOPT(hackUpPlaylist, "HackUpPlaylist", true)
READBOOLOPT(fastMixer, "FastMixer", false)
READBOOLOPT(clearOnOpen, "ClearOnOpen", false)
READBOOLOPT_EX(oneInstance, "MultipleInstances", true, "KDE", true)

#undef READBOOLOPT
#undef READBOOLOPT_EX

void NoatunApp::setHackUpPlaylist(bool b)
{
	KConfig *config=KGlobal::config();
	config->setGroup("");
	config->writeEntry("HackUpPlaylist", b);
	config->sync();
}

void NoatunApp::setFastMixer(bool b)
{
	bool whatBefore=fastMixer();
	if (whatBefore!=b)
	{
		KConfig *config=KGlobal::config();
		config->setGroup("");
		config->writeEntry("FastMixer", b);
		config->sync();
		player()->engine()->useHardwareMixer(b);
	}
}

void NoatunApp::setOneInstance(bool b)
{
	KConfig *config=KGlobal::config();
	config->setGroup("KDE");
	config->writeEntry("MultipleInstances", !b);
	config->sync();
}

void NoatunApp::setLoopList(bool b)
{
	KConfig *config=KGlobal::config();
	config->setGroup("");
	config->writeEntry("LoopList", b);
	KGlobal::config()->sync();
}

void NoatunApp::setAutoPlay(bool b)
{
	KConfig *config=KGlobal::config();
	config->setGroup("");
	config->writeEntry("AutoPlay", b);
	KGlobal::config()->sync();
}

void NoatunApp::setRememberPositions(bool b)
{
	KConfig *config=KGlobal::config();
	config->setGroup("");
	config->writeEntry("RememberPositions", b);
	KGlobal::config()->sync();
}

void NoatunApp::setSaveDirectory(const QString &s)
{
	KConfig *config=KGlobal::config();
	config->setGroup("");
	config->writeEntry("SaveDirectory", s);
	config->sync();
}

QString NoatunApp::saveDirectory() const
{
	KConfig *c=KGlobal::config();
	c->setGroup("");
	return c->readEntry("SaveDirectory", QString(getenv("HOME")));
}

void NoatunApp::setClearOnOpen(bool b)
{
	KConfig *config=KGlobal::config();
	config->setGroup("");
	config->writeEntry("ClearOnOpen", b);
	config->sync();
}

int NoatunApp::pluginMenuAdd(const QString &text, const QObject *receiver, const char *member)
{
	return pluginMenu()->insertItem(text, receiver, member);
}

void NoatunApp::pluginMenuRemove(int id)
{
	pluginMenu()->removeItem(id);
}

KPopupMenu *NoatunApp::pluginMenu(void)
{
	if(!mPluginMenu)
	{
		mPluginMenu = new KPopupMenu;
	}
	return mPluginMenu;
}

int NoatunApp::newInstance()
{
	bool playme=true;
	
	KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
	for (int i=0; i<args->count(); i++)
	{
		player()->openFile(args->url(i), false, playme), playme=false;
	}

	args->clear();
	return 0;
}

void NoatunApp::preferences()
{
	mPref->show();
	mPref->raise();
}

void NoatunApp::quit()
{
	KApplication::quit();
}

void NoatunApp::fileOpen()
{
	KURL file(KFileDialog::getOpenURL(QString::null, mimeTypes(), 0, i18n("Select a File to Play")));
	if (!file.isMalformed())
		mPlayer->openFile(file, clearOnOpen(), autoPlay());
}

void NoatunApp::effectView()
{
	mEffectView->show();
}

void NoatunApp::equalizerView()
{
	mEqualizerView->show();
}

Effects *NoatunApp::effects() const
{
	return mEffects;
}

QString NoatunApp::mimeTypes()
{
	QString full;
	Arts::TraderQuery q;
	vector<Arts::TraderOffer> *results = q.query();
	QString str;

	bool comma=false;

	for (vector<Arts::TraderOffer>::iterator i=results->begin();
	     i != results->end(); i++)
	{
		vector<string> *prop=(*i).getProperty("MimeType");

		for (vector<string>::iterator istr=prop->begin();
		     istr != prop->end(); istr++)
		{
			if (!(*istr).length()) continue;
			comma=true;
			const char *m=(*istr).c_str();
			
			if ((KServiceType::serviceType(m)))
				str+=m;
			str+=' ';
		}
		delete prop;
	}
	delete results;
	kdDebug() << "mimeTypes() " << str.latin1() << endl;
	return str;
}

void NoatunApp::loadPlugins()
{
	mLibraryLoader->loadAll();
}

void NoatunApp::cleverKCrashHack(int)
{
	// do nothing

	// Understand that the KDE libraries have a memory leak, and
	// the playlist cannot be unloaded without causing a crash
	// in QApplication::windowMapper() or something similar.
	// this is just to prevent the KCrash window from appearing
	// and bugging the user regularly

	// someone fix the libraries.
	kdDebug() << "Crashed." << endl;
	_exit(255);
}

Playlist *NoatunApp::playlist() const
{
	return mLibraryLoader->playlist();
}

void NoatunApp::commitData(QSessionManager &)
{
}

void NoatunApp::saveState(QSessionManager &sm)
{
	QStringList restartCommand = sm.restartCommand();
	sm.setRestartCommand( restartCommand );

	KApplication::saveState(sm);
}

/*********************************************
 * The following code (c) Troll Tech, AS
 * It was stolen from qpngio.cpp
 * these functions are not used by noatun itself
 * and are merely helper classes for any
 * modules that may be written.  Since
 * noatun links with QT, you shall assume
 * that the following is a part of QT
 * and for that reason, its license
 * varies with the module's usage of QT.
 * It, additionally, should be treated
 * like a part of QT in all ways.
 *
 * If the Noatun library itself uses these
 * functions, it will be under the QT Public
 * License.
 **********************************************/

static void iod_read_fn(png_structp png_ptr, png_bytep data, png_size_t length)
{
	QFile* in=(QFile*)png_get_io_ptr(png_ptr);

	while (length)
	{
		int nr = in->readBlock((char*)data, length);
		if (nr <= 0)
		{
			png_error(png_ptr, "Read Error");
			return;
		}
		length -= nr;
	}
}

static void setup_qt( QImage& image, png_structp png_ptr, png_infop info_ptr )
{
	if ( png_get_valid(png_ptr, info_ptr, PNG_INFO_gAMA) )
	{
		double file_gamma;
		png_get_gAMA(png_ptr, info_ptr, &file_gamma);
	}

	png_uint_32 width;
	png_uint_32 height;
	int bit_depth;
	int color_type;
	png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
	             0, 0, 0);

    if ( color_type == PNG_COLOR_TYPE_GRAY )
	{
		// Black & White or 8-bit greyscale
		if ( bit_depth == 1 && info_ptr->channels == 1 )
		{
			png_set_invert_mono( png_ptr );
			png_read_update_info( png_ptr, info_ptr );
			image.create( width, height, 1, 2, QImage::BigEndian );
			image.setColor( 1, qRgb(0,0,0) );
			image.setColor( 0, qRgb(255,255,255) );
		}
		else
		{
			if ( bit_depth == 16 )
				png_set_strip_16(png_ptr);
			else if ( bit_depth < 8 )
				png_set_packing(png_ptr);
			int ncols = bit_depth < 8 ? 1 << bit_depth : 256;
			png_read_update_info(png_ptr, info_ptr);
			image.create(width, height, 8, ncols);
			for (int i=0; i<ncols; i++)
			{
				int c = i*255/(ncols-1);
				image.setColor( i, qRgba(c,c,c,0xff) );
			}
			if ( png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS) )
			{
				int g = info_ptr->trans_values.gray;
				if ( bit_depth > 8 )
				{
					// transparency support disabled for now
				}
				else
				{
					image.setAlphaBuffer( TRUE );
				    image.setColor(g, RGB_MASK & image.color(g));
				}
	    	}
		}
	}
	else if ( color_type == PNG_COLOR_TYPE_PALETTE
	         && png_get_valid(png_ptr, info_ptr, PNG_INFO_PLTE)
	         && info_ptr->num_palette <= 256 )
	{
	// 1-bit and 8-bit color
	if ( bit_depth != 1 )
		png_set_packing( png_ptr );
	png_read_update_info( png_ptr, info_ptr );
	png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type, 0, 0, 0);
	image.create(width, height, bit_depth, info_ptr->num_palette, QImage::BigEndian);
	int i = 0;
	if ( png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS) )
	{
	    image.setAlphaBuffer( TRUE );
	    while ( i < info_ptr->num_trans ) {
		image.setColor(i, qRgba(
		    info_ptr->palette[i].red,
		    info_ptr->palette[i].green,
		    info_ptr->palette[i].blue,
		    info_ptr->trans[i]
		    )
		);
		i++;
	    }
	}
	while ( i < info_ptr->num_palette )
	{
		image.setColor(i, qRgba(
			info_ptr->palette[i].red,
			info_ptr->palette[i].green,
			info_ptr->palette[i].blue,
			0xff
			)
		);
		i++;
	}
}
    else {
	// 32-bit
	if ( bit_depth == 16 )
	    png_set_strip_16(png_ptr);

	png_set_expand(png_ptr);

	if ( color_type == PNG_COLOR_TYPE_GRAY_ALPHA )
	    png_set_gray_to_rgb(png_ptr);

	image.create(width, height, 32);

	// Only add filler if no alpha, or we can get 5 channel data.
	if (!(color_type & PNG_COLOR_MASK_ALPHA)
	   && !png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) {
	    png_set_filler(png_ptr, 0xff,
		QImage::systemByteOrder() == QImage::BigEndian ?
		    PNG_FILLER_BEFORE : PNG_FILLER_AFTER);
	    // We want 4 bytes, but it isn't an alpha channel
	} else {
	    image.setAlphaBuffer(TRUE);
	}

	if ( QImage::systemByteOrder() == QImage::BigEndian )
	    png_set_swap_alpha(png_ptr);

	png_read_update_info(png_ptr, info_ptr);
    }

    // Qt==ARGB==Big(ARGB)==Little(BGRA)
    if ( QImage::systemByteOrder() == QImage::LittleEndian )
    {
	png_set_bgr(png_ptr);
    }
}

static bool read_png_image(QFile *file, QImage &image)
{
	png_structp png_ptr;
	png_infop info_ptr;
	png_infop end_info;
	png_bytep* row_pointers;

	png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING,0,0,0);
	if (!png_ptr)
		return false;

	info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr)
	{
		png_destroy_read_struct(&png_ptr, 0, 0);
		return false;
	}

	end_info = png_create_info_struct(png_ptr);
	if (!end_info)
	{
		png_destroy_read_struct(&png_ptr, &info_ptr, 0);
		return false;
	}

	if (setjmp(png_ptr->jmpbuf))
	{
		png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
		return false;
	}

	png_set_read_fn(png_ptr, (void*)file, iod_read_fn);
	png_read_info(png_ptr, info_ptr);

	setup_qt(image, png_ptr, info_ptr);

	png_uint_32 width;
	png_uint_32 height;
	int bit_depth;
	int color_type;
	png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
	             0, 0, 0);

	uchar** jt = image.jumpTable();
	row_pointers=new png_bytep[height];

	for (uint y=0; y<height; y++)
		row_pointers[y]=jt[y];

	png_read_image(png_ptr, row_pointers);

	delete [] row_pointers;

	png_read_end(png_ptr, end_info);
	png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);

	return true;
}


QImage NoatunApp::readPNG(const QString &filename)
{
	QFile file(filename);
	file.open(IO_ReadOnly);
	QImage image;
	read_png_image(&file, image);
	file.close();
	return image;
}

#include "noatunapp.moc"
