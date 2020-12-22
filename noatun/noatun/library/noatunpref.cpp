#include "noatunpref.h"

#include <klocale.h>
#include <qlayout.h>
#include <qlabel.h>
#include "cmodule.h"

NoatunPreferences::NoatunPreferences(QWidget *parent)
    : KDialogBase(TreeList, i18n("Preferences"),
                  Ok|Apply|Close, Ok, parent, 0, false)
{
	resize(640, 480); // KDE is required to support 800x600 min.
}

void NoatunPreferences::slotOk()
{
	slotApply();
	hide();
}

void NoatunPreferences::show()
{
	for (CModule *i=mModules.first(); i != 0; i=mModules.next())
		i->reopen();
	KDialogBase::show();
}

void NoatunPreferences::slotApply()
{
	for (CModule *i=mModules.first(); i != 0; i=mModules.next())
		i->save();
}

void NoatunPreferences::add(CModule *page)
{
	mModules.append(page);
}

void NoatunPreferences::remove(CModule *page)
{
	mModules.removeRef(page);
}

CModule::CModule(const QString &name, const QString &description, QObject *owner)
	: QWidget(napp->preferencesBox()->addPage(name, description))
{
	if (owner)
		connect(owner, SIGNAL(destroyed()), SLOT(ownerDeleted()));
	napp->preferencesBox()->add(this);

	QFrame *page=static_cast<QFrame*>(parent());
	(new QHBoxLayout(page))->addWidget(this);
}

CModule::~CModule()
{
	// TODO When kdialogbase is fixed, delete the parent QFrame, and
	// all traces of the related page

	napp->preferencesBox()->remove(this);
}

void CModule::ownerDeleted()
{
	QObject *p=parent();
	delete this;
	delete p;
}

#include "noatunpref.moc"


