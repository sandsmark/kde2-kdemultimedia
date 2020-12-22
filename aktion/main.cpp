#include "principal.h"
#include "aktion_part.h"
#include <kapp.h>
#include <kaboutdata.h>
#include <kcmdlineargs.h>
#include <klocale.h>

static KCmdLineOptions options[] =
{
  { "+[file]",   I18N_NOOP("File to open."), 0 },
  { 0, 0, 0}
};

int main(int argc,char *argv[])
{
    KCmdLineArgs::init(argc, argv, AktionFactory::aboutData() );
    KCmdLineArgs::addCmdLineOptions( options ); // Add our own options.
    KApplication app;

    Principal * mainWindow = new Principal("topWidget");
	
    app.setMainWidget(mainWindow);
    mainWindow->show();

    KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
    if (args->count() == 1)
        mainWindow->loadFile(args->arg(0));

    return app.exec();
}
