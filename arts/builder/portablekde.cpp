#include "portablekde.h"

#include <kmessagebox.h>
#include <kglobal.h>
#include <kstddirs.h>
#include <kapp.h>

using namespace std;

void PortableKDE::KMsgSorry(QWidget *parent, const char *text)
{
	KMessageBox::sorry(parent,text);
}

list<string> PortableKDE::globalDirs(const char *which)
{
	QStringList qresult = KGlobal::dirs()->resourceDirs(QString(which));
	QStringList::ConstIterator it;
	list<string> result;

	for (it = qresult.begin(); it != qresult.end(); it++)
		result.push_back((const char *)*it);

	return result;
}

QString PortableKDE::caption()
{
	return kapp->caption();
}

KConfig* PortableKDE::config()
{
	return kapp->config();
}
