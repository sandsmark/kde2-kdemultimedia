#include <qapplication.h>
#include <noatunapp.h>
#include "joinhandler.h"
#include <kuniqueapp.h>
#include <dcopclient.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <klocale.h>
#include <qdatetime.h>
#include <qfile.h>
#include <kmessagebox.h>

void noMessageOutput(QtMsgType, const char *)
{
}

static const char *description=I18N_NOOP("\"It just sounds better in Noatun\"");
static const char *version="1.2.0";

static KCmdLineOptions options[] =
{
	{ "+[URL]",          I18N_NOOP("Files/URLs to Open."),     0 },
	{ "playlist <file>", I18N_NOOP("Open a Playlist"),         0 },
	{ "qdebug",          I18N_NOOP("Turn on Qt Debug output, for pluginwriting"), 0 },
	{ 0, 0, 0}
};

int main(int argc, char **argv)
{
	KAboutData aboutData("noatun", I18N_NOOP("Noatun"),
	                     version, description, KAboutData::License_BSD,
	                     "(c) 2000-2001, The Noatun Developers", 0, "http://noatun.kde.org");
	aboutData.addAuthor("Charles Samuels", I18N_NOOP("Noatun Developer"),
	                    "charles@kde.org");
	aboutData.addAuthor("Neil Stevens", I18N_NOOP("Standards and usability complainer"),
	                    "neil@qualityassistant.com");
	aboutData.addAuthor("Stefan Westerfeld", I18N_NOOP("Patron of the aRts"),
	                    "stefan@space.twc.de");
	aboutData.addAuthor("Martin Vogt", I18N_NOOP("MPEG Codec and OGG Vorbis Support"),
	                    "mvogt@rhrk.uni-kl.de");
	aboutData.addAuthor("Malte Starostik", I18N_NOOP("Infrared Control Support and HTML playlist export"),
	                    "malte@kde.org");
	aboutData.addAuthor("Nikolas Zimmermann", I18N_NOOP("HTML playlist export and Plugin System"),
	                    "wildfox@kde.org");
	aboutData.addAuthor("Stefan Schimanski", I18N_NOOP("Kaiman Skin Support"),
	                    "1Stein@gmx.de" );

	aboutData.addCredit("Bill Huey", I18N_NOOP("Special help with the equalizer"));
	
	KCmdLineArgs::init( argc, argv, &aboutData );
	KCmdLineArgs::addCmdLineOptions(options);
	NoatunApp::addCmdLineOptions();

	NoatunApp app;

	if(!KCmdLineArgs::parsedArgs()->isSet("qdebug"))
		qInstallMsgHandler(noMessageOutput);

	return app.exec();
}

