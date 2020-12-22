#include "find.h"
#include <qlayout.h>
#include <klineedit.h>
#include <qpushbutton.h>
#include <qcheckbox.h>
#include <klocale.h>

Finder::Finder(QWidget *parent) : KDialog(parent, 0, false, WDestructiveClose)
{
	setCaption(i18n("Find"));
	QGridLayout *layout=new QGridLayout(this);
	mText=new KLineEdit(this);
	layout->addMultiCellWidget(mText, 0, 0, 0, 1);
	mRegexp=new QCheckBox(i18n("Regular Expresion"), this);
	mBackwards=new QCheckBox(i18n("Find &Backwards"), this);
	layout->addWidget(mRegexp, 1, 0);
	layout->addWidget(mBackwards, 1, 1);
	mStart=new QPushButton(i18n("&Find"), this);
	mStart->setDefault(true);
	layout->addWidget(mStart, 2, 1);
	connect(mStart, SIGNAL(clicked()), SLOT(clicked()));
	connect(mText, SIGNAL(returnPressed()), SLOT(clicked()));
}

bool Finder::regexp() const
{
	return mRegexp->isChecked();
}

bool Finder::isForward() const
{
	return !mBackwards->isChecked();
}

void Finder::clicked()
{
	emit search(this);
}

QString Finder::string() const
{
	return mText->text();
}


#include "find.moc"
