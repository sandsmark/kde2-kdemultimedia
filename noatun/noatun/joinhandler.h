#ifndef JOINHANDLER_H
#define JOINHANDLER_H
#include <dcopobject.h>
#include <qstringlist.h>

class JoinHandler : public DCOPObject
{
K_DCOP

public:
	JoinHandler();
	
k_dcop:
	void addFile(const QString&);
	void addFile(const QStringList &);


};

#endif
