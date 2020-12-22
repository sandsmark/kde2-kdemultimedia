#include "effectview.h"
#include "noatunapp.h"
#include "effects.h"

#include <kdialog.h>
#include <qlayout.h>
#include <klocale.h>
#include <qwhatsthis.h>
#include <qsplitter.h>
#include <kiconloader.h>
#include <klistview.h>
#include <qiconview.h>
#include <qpushbutton.h>
#include <qabstractlayout.h>
#include <qlabel.h>
#include <qheader.h>

class EffectIconItem : public QIconViewItem
{
public:
	EffectIconItem(QIconView *parent, const QCString &name)
		: QIconViewItem(parent, Effect::clean(name))
	{
		mName = name;
		setPixmap(DesktopIcon("effect"));
	}

	QCString effectName() const { return mName; }

private:
	QCString mName;
};


class EffectListItem : public QListViewItem
{
public:
	EffectListItem(QListView *parent, QListViewItem *after, Effect *e)
		: QListViewItem(parent, after, e->title()), mEffect(e)
	{
	}

	Effect *effect() const { return mEffect; }

private:
	Effect *mEffect;
};

EffectList::EffectList(QWidget *parent)
	: KListView(parent)
{
}

bool EffectList::acceptDrag(QDropEvent *event) const
{
	return QCString(event->format()) == "application/x-noatun-effectdrag";
}

QDragObject *EffectList::dragObject() const
{
	if (!currentItem()) return 0;
	return new QStoredDrag("application/x-noatun-effectdrag", (QWidget*)this);
}


EffectAvailable::EffectAvailable(QWidget *parent, EffectView *v)
	: KIconView(parent), mView(v)
{
	setGridY(64);
	setGridX(128);
	setResizeMode(QIconView::Adjust);
	setAutoArrange(true);
	setMode(KIconView::Select);
}

QDragObject *EffectAvailable::dragObject()
{
	if (!currentItem()) return 0;
	return new QStoredDrag("application/x-noatun-effectdrag", (QWidget*)this);
}

void EffectAvailable::contentsDropEvent(QDropEvent *e)
{
	EffectList *el = mView->active();
	if (el != e->source())
		return;
	mView->removeCurrent();
}

EffectView::EffectView()
	: KDialogBase(0, 0, false, i18n("Effects"), Help | Close, Close, true), first(true)
{

}

void EffectView::show()
{
	if (first)
	{
		setCaption(i18n("Effects"));
		setIcon(SmallIcon("effect"));

		QFrame *box = makeMainWidget();
		(new QHBoxLayout(box, KDialog::marginHint(), KDialog::spacingHint()))->setAutoAdd(true);
		QSplitter *split = new QSplitter(box);
		QPushButton *buttons;

		// left side
		QWidget *col = new QWidget(split);
		(new QVBoxLayout(col, KDialog::marginHint(), KDialog::spacingHint()))->setAutoAdd(true);
		col->setMinimumWidth( (new QLabel(i18n("<b>Available Effects</b>"), col))->width());
		mAvailable = new EffectAvailable(col, this);
		QWhatsThis::add(mAvailable, i18n("This shows all available effects.\n\nDrag files from here to the Active pane to the right to activate a plugin."));

		// center
		col = new QWidget(split);
		QVBoxLayout *vbox = new QVBoxLayout(col, KDialog::marginHint(), KDialog::spacingHint());
		vbox->addItem(new QSpacerItem(20, 20, QSizePolicy::Minimum, 
					QSizePolicy::Expanding));

		(mLeft = buttons=new QPushButton(col))->setFixedSize(22, 22);
		buttons->setPixmap(BarIcon("forward", KIcon::SizeSmall));
		QWhatsThis::add(buttons, i18n("This will place the selected effect the bottom of your chain."));
		vbox->addWidget(buttons);
		connect(buttons, SIGNAL(clicked()), SLOT(moveRight()));

		(mRight = buttons=new QPushButton(col))->setFixedSize(22, 22);
		buttons->setPixmap(BarIcon("back", KIcon::SizeSmall));
		QWhatsThis::add(buttons, i18n("This will remove the selected effect from your chain."));
		vbox->addWidget(buttons);
		connect(buttons, SIGNAL(clicked()), SLOT(moveLeft()));

#ifdef HELPBUTTONS
		(mLeftHelp = buttons=new QPushButton(col))->setFixedSize(22, 22);
		buttons->setPixmap(BarIcon("help", KIcon::SizeSmall));
		QWhatsThis::add(buttons, i18n("This button will show information on the selected effect."));
		vbox->addWidget(buttons);
		connect(buttons, SIGNAL(clicked()), SLOT(leftHelp()));
#endif

		vbox->addItem(new QSpacerItem(20, 20, QSizePolicy::Minimum, 
					QSizePolicy::Expanding));

		col->setFixedWidth(buttons->width() + 2 * KDialog::marginHint());

		// right
		QWidget *col2 = new QWidget(split);
		(new QHBoxLayout(col2))->setAutoAdd(true);

		col = new QWidget(col2);
		(new QVBoxLayout(col, KDialog::marginHint(), KDialog::spacingHint()))->setAutoAdd(true);
		col->setMinimumWidth( (new QLabel(i18n("<b>Active Effects</b>"), col))->width());
		mActive = new EffectList(col);
		QWhatsThis::add(mActive, i18n("This shows your effect chain.  Noatun supports an unlimited amount of effects in any order.  You can even have the same effect twice!\n\nDrag the items to and from here to add and remove them, respectively.  You may also reorder them with drag-and-drop.  These actions can also be performed with the buttons to the right."));

		// far right
		col = new QWidget(col2);
		vbox = new QVBoxLayout(col, KDialog::marginHint(), KDialog::spacingHint());
		vbox->addItem(new QSpacerItem(20, 20, QSizePolicy::Minimum, 
					QSizePolicy::Expanding));

		(mUp = buttons=new QPushButton(col))->setFixedSize(22, 22);
		buttons->setPixmap(BarIcon("up", KIcon::SizeSmall));
		QWhatsThis::add(buttons, i18n("Move the currently selected effect up in the chain."));
		vbox->addWidget(buttons);
		connect(buttons, SIGNAL(clicked()), SLOT(moveUp()));

		(mDown = buttons=new QPushButton(col))->setFixedSize(22, 22);
		buttons->setPixmap(BarIcon("down", KIcon::SizeSmall));
		QWhatsThis::add(buttons, i18n("Move the currently selected effect down in the chain."));
		vbox->addWidget(buttons);
		connect(buttons, SIGNAL(clicked()), SLOT(moveDown()));

//#ifdef HELPBUTTONS
		(mRightHelp = buttons=new QPushButton(col))->setFixedSize(22, 22);
		buttons->setPixmap(BarIcon("configure", KIcon::SizeSmall));
		QWhatsThis::add(buttons, i18n("Configure the currently selected effect.\n\nYou can change things such as intensity from here."));
		vbox->addWidget(buttons);
		connect(buttons, SIGNAL(clicked()), SLOT(rightHelp()));
//#endif

		vbox->addItem(new QSpacerItem(20, 20, QSizePolicy::Minimum, 
					QSizePolicy::Expanding));

		QStrList av = napp->effects()->available();
		// scan and add the effects
		for (char *i = av.first(); i!=0; i=av.next())
			new EffectIconItem(mAvailable, i);

		mActive->setAcceptDrops(true);
		mActive->addColumn("");
		mActive->header()->hide();
		mActive->setSorting(-1);
		mActive->setDropVisualizer(true);
		mActive->setItemsMovable(true);
		mActive->setSelectionMode(QListView::Single);
		mActive->setDragEnabled(true);

		// DND for the KListView
		connect(mActive, SIGNAL(dropped(QDropEvent*, QListViewItem*)), SLOT(dropEvent(QDropEvent*, QListViewItem*)));

		// when a new effect is added
		connect(napp->effects(), SIGNAL(added(Effect*)), SLOT(added(Effect*)));
		connect(napp->effects(), SIGNAL(removed(Effect*)), SLOT(removed(Effect*)));
		connect(napp->effects(), SIGNAL(moved(Effect*)), SLOT(moved(Effect*)));

		connect(mActive, SIGNAL(currentChanged(QListViewItem*)), SLOT(activeChanged(QListViewItem*)));
		connect(mAvailable, SIGNAL(currentChanged(QIconViewItem*)), SLOT(availableChanged(QIconViewItem*)));
		mActive->setCurrentItem(0);
		mAvailable->setCurrentItem(0);
		availableChanged(mAvailable->currentItem());
		activeChanged(mActive->currentItem());

		first=false;
	}
	KDialogBase::show();
}

void EffectView::dropEvent(QDropEvent *e, QListViewItem *pafter)
{
	EffectListItem *after(static_cast<EffectListItem*>(pafter));
	if (e->source() == mAvailable)
	{ // add
		napp->effects()->insert(after ? after->effect() : 0, 
		                        new Effect(static_cast<EffectIconItem*>(mAvailable->currentItem())->effectName()));
//		QCString name = static_cast<EffectIconItem*>(mAvailable->currentItem())->effectName();
//		Effect *e = new Effect(name);
//		napp->effects()->append(e);
	}
	else if (e->source() == mActive)
	{ // move
		napp->effects()->move(after ? after->effect() : 0, 
		                      static_cast<EffectListItem*>(mActive->currentItem())->effect());
	}
	activeChanged(mActive->currentItem());
}

QListViewItem *EffectView::toListItem(Effect *e) const
{
	for (QListViewItem *i = mActive->firstChild(); i!=0; i=i->itemBelow())
		if (static_cast<EffectListItem*>(i)->effect() == e)
			return i;
	return 0;
}

void EffectView::added(Effect *item)
{
	new EffectListItem(mActive, toListItem(item->before()), item);
	activeChanged(mActive->currentItem());
}

void EffectView::moved(Effect *item)
{
	delete toListItem(item);
	added(item);
}

void EffectView::removed(Effect *item)
{
	delete toListItem(item);
	activeChanged(mActive->currentItem());
}

void EffectView::moveDown()
{
	Effect *e = static_cast<EffectListItem*>(mActive->currentItem())->effect();

	if (e->after())
		napp->effects()->move(e->after(), e);
	mActive->setCurrentItem(toListItem(e));
	mActive->setSelected(toListItem(e), true);
	activeChanged(mActive->currentItem());
}

void EffectView::moveUp()
{
	Effect *e = static_cast<EffectListItem*>(mActive->currentItem())->effect();
	if (e->before())
	{
		if (e->before()->before())
			napp->effects()->move(e->before()->before(), e);
		else
			napp->effects()->move(0, e);
	}
	mActive->setCurrentItem(toListItem(e));
	mActive->setSelected(toListItem(e), true);
	activeChanged(mActive->currentItem());
}

void EffectView::moveLeft()
{
	removeCurrent();
}

void EffectView::moveRight()
{
	napp->effects()->append(new Effect(static_cast<EffectIconItem*>(mAvailable->currentItem())->effectName()));
	activeChanged(mActive->currentItem());
}

void EffectView::removeCurrent()
{
	EffectListItem *item = static_cast<EffectListItem*>(mActive->currentItem());
	napp->effects()->remove(item->effect());
	activeChanged(mActive->currentItem());
}

void EffectView::leftHelp()
{

}

void EffectView::rightHelp()
{
	Effect *e = static_cast<EffectListItem*>(mActive->currentItem())->effect();
	if (!e) return;
	
	QWidget *c=e->configure();
	if (c)
		c->show();
}

void EffectView::activeChanged(QListViewItem *i)
{
	if(i)
	{
		mUp->setEnabled(i->itemAbove());
		mDown->setEnabled(i->itemBelow());
		mRight->setEnabled(true);
		
		Effect *e = static_cast<EffectListItem*>(mActive->currentItem())->effect();
		mRightHelp->setEnabled(e->configurable());
	}
	else
	{
		mUp->setEnabled(false);
		mDown->setEnabled(false);
		mRight->setEnabled(false);
		mRightHelp->setEnabled(false);
	}
}

void EffectView::availableChanged(QIconViewItem *i)
{
	if(i)
	{
		mLeft->setEnabled(true);
	}
	else
	{
		mLeft->setEnabled(false);
	}
}

#include "effectview.moc"
