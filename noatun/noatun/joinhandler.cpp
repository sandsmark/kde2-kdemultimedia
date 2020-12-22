#include "joinhandler.h"
#include "library/noatunapp.h"
#include "library/player.h"

JoinHandler::JoinHandler() : DCOPObject("NoatunJoinHandler")
{

}


void JoinHandler::addFile(const QString& f)
{
	napp->player()->openFile(f, false);
}

void JoinHandler::addFile(const QStringList &f)
{
	bool playme=true;
	for (QStringList::ConstIterator it = f.begin(); it != f.end(); ++it )
		napp->player()->openFile(*it, false, playme), playme=false;

}

