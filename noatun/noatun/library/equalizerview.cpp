#include "equalizer.h"
#include "equalizerview.h"
#include <qlayout.h>
#include <qslider.h>
#include <qcheckbox.h>
#include <qlabel.h>
#include "noatunapp.h"
#include <kdialog.h>
#include <kiconloader.h>
#include <klocale.h>
#include <qpushbutton.h>
#include <qheader.h>

EqualizerLevel::EqualizerLevel(QWidget *parent, Band *band) : QWidget(parent){
	QVBoxLayout *layout = new QVBoxLayout(this);
	layout->setAutoAdd(true);

	mBand = band;
	connect(napp->equalizer(), SIGNAL(changed(Band *)), SLOT(changed(Band*)));

	mSlider = new QSlider(-200, 200, 50, 0, Qt::Vertical, this);
	connect(mSlider, SIGNAL(valueChanged(int)), SLOT(changed(int)));
	mLabel = new QLabel(band->format(), this);
	mLabel->setAlignment(AlignHCenter | AlignVCenter);

	setMinimumHeight(150);
	setMinimumWidth(30);
	changed(band);
}

void EqualizerLevel::changed(Band *b)
{
	if (b!=mBand) return;
	mSlider->blockSignals(true);
	mSlider->setValue(-(b->level()));
	mSlider->blockSignals(false);
}

void EqualizerLevel::changed(int l)
{
	mBand->setLevel(-l);
}



////////////////////////////////////////////////
// PresetList

PresetList::PresetList(QWidget *parent)
	: KListView(parent)
{}

void PresetList::rename(QListViewItem *item, int c)
{
	if (item->text(0)==i18n("Custom"))
		return;
	KListView::rename(item, c);
}


///////////////////////////////////////////////
// EqualizerView

EqualizerView::EqualizerView()
	: KDialogBase(0, 0, false, i18n("Equalizer"), Help | Close, Close, true),
	  first(true), mGoingPreset(false)
{
}

void EqualizerView::show()
{
	if (first)
	{
		first=false;

		QFrame *frame = makeMainWidget();
		(new QHBoxLayout(frame))->setAutoAdd(true);

		{
			QWidget *left=new QWidget(frame);
			(new QVBoxLayout(left))->setAutoAdd(true);
			{	
				QFrame *bandFrame = new QFrame(left);
				(new QHBoxLayout(bandFrame, 10))->setAutoAdd(true);

				QList<Band> bands = napp->equalizer()->bands();
				for (Band *i = bands.first(); i != 0; i = bands.next())
				{
					new EqualizerLevel(bandFrame, i);
					connect(napp->equalizer(), SIGNAL(changed(Band*)), SLOT(changed(Band*)));
				}
			}

			mPreamp = new QSlider(-200, 200, 25, 0, Qt::Horizontal, left);
			connect(mPreamp, SIGNAL(valueChanged(int)), napp->equalizer(), SLOT(setPreamp(int)));
			connect(napp->equalizer(), SIGNAL(preampChanged(int)), mPreamp, SLOT(setValue(int)));
		}
		{
			QWidget *right=new QWidget(frame);
			(new QVBoxLayout(right))->setAutoAdd(true);
			new QLabel(i18n("<b>Presets</b>"), right);

			mPresets=new PresetList(right);
			connect(mPresets, SIGNAL(currentChanged(QListViewItem*)), SLOT(changed(QListViewItem*)));
			connect(mPresets, SIGNAL(itemRenamed(QListViewItem*)), SLOT(renamed(QListViewItem*)));
			
			mPresets->setItemsRenameable(true);
			mPresets->setRenameable(0, true);
			mPresets->addColumn("");
			// create another column to store 
			mPresets->addColumn("", 0);
			mPresets->header()->hide();

			mRemovePreset=new QPushButton(i18n("&Remove Preset"), right);
			connect(mRemovePreset, SIGNAL(clicked()), SLOT(removePreset()));

			mAddPreset=new QPushButton(i18n("&Add Preset"), right);
			connect(mAddPreset, SIGNAL(clicked()), SLOT(addPreset()));

			new QListViewItem(mPresets, i18n("Custom"));

			mOn=new QCheckBox(i18n("&Enabled"), right);

			connect(mOn, SIGNAL(toggled(bool)), napp->equalizer(), SLOT(setEnabled(bool)));
		}

		connect(napp->equalizer(), SIGNAL(enabled(bool)), SLOT(qCheckBoxIsStupid(bool)));
		connect(napp->equalizer(), SIGNAL(created(Preset*)), SLOT(presetAdded(Preset*)));
		connect(napp->equalizer(), SIGNAL(removed(Preset*)), SLOT(presetRemoved(Preset*)));
		connect(napp->equalizer(), SIGNAL(renamed(Preset*)), SLOT(renamed(Preset*)));
		connect(napp->equalizer(), SIGNAL(changed()), SLOT(changedEq()));
		
		mOn->setChecked(false);
		qCheckBoxIsStupid(napp->equalizer()->isEnabled());

		setIcon(SmallIcon("noatun"));

		// show all the presets now

		QList<Preset> presets=napp->equalizer()->presets();
		presets.setAutoDelete(true);
		for (Preset *i=presets.first(); i != 0; i=presets.next())
		{
			// store the filename in QListViewItem::text(1)
			QString n=i->name();
			QString f=i->file();
			new QListViewItem(mPresets, i->name(), i->file());
		}

		// select "custom"
		changedEq();
	}
	KDialogBase::show();
}

QListViewItem *EqualizerView::listItem(const Preset *p) const
{
	for (QListViewItem *i=mPresets->firstChild(); i!=0; i=i->itemBelow())
	{
		if (i->text(1)==p->file())
			return i;
	}
	return 0;
}

void EqualizerView::qCheckBoxIsStupid(bool on)
{
	mOn->setChecked(on);
}

void EqualizerView::addPreset()
{
	Preset *p= napp->equalizer()->createPreset(i18n("New Preset"));
	p->load();
	delete p;
}

void EqualizerView::removePreset()
{
	QListViewItem *current=mPresets->currentItem();
	if (current->text(0)==i18n("Custom"))
		return;
	QListViewItem *then=current->itemAbove();
	if (!then) then=current->itemBelow();

	Preset *p=napp->equalizer()->preset(current->text(1));
	if (p) p->remove();
	delete p;
	
	if (then)
		mPresets->setSelected(then, true);
}

void EqualizerView::changed(QListViewItem *i)
{
	if (!i) return;

	mGoingPreset=true;
	Preset *p=napp->equalizer()->preset(i->text(1));
	if (p)
		p->load();
	delete p;
	mGoingPreset=false;
}

void EqualizerView::presetAdded(Preset *p)
{
	new QListViewItem(mPresets, p->name(), p->file());

}

void EqualizerView::renamed(QListViewItem *i)
{
	napp->equalizer()->preset(i->text(1))->setName(i->text(0));
	i->setText(0, napp->equalizer()->preset(i->text(1))->name());
}

void EqualizerView::renamed(Preset *p)
{
	QListViewItem *i=listItem(p);
	if (i)
		i->setText(0, p->name());
}

void EqualizerView::changedEq()
{
	if (!mGoingPreset)
	{
		QString custom=i18n("Custom");
		for (QListViewItem *i=mPresets->firstChild(); i!=0; i=i->itemBelow())
		{
			if (i->text(0)==custom)
			{
				mPresets->setSelected(i, true);
				return;
			}
		}
	}
}

void EqualizerView::presetRemoved(Preset *p)
{
	delete listItem(p);
}

#include "equalizerview.moc"
