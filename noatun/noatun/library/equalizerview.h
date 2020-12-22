#ifndef EQUALIZERVIEW_H
#define EQUALIZERVIEW_H

#include <qwidget.h>
#include <kdialogbase.h>
#include <klistview.h>

class Band;
class QSlider;
class QLabel;
class QCheckBox;
class QPushButton;
class Preset;

class EqualizerLevel : public QWidget
{
Q_OBJECT
public:
	EqualizerLevel(QWidget *parent, Band *band);

public slots:
	void changed(Band *);
	void changed(int);
	
private:
	Band *mBand;
	QSlider *mSlider;
	QLabel *mLabel;
};

class PresetList : public KListView
{
Q_OBJECT
public:
	PresetList(QWidget *parent);

public:
	void rename(QListViewItem *item, int c);

};

class EqualizerView : public KDialogBase
{
Q_OBJECT
public:
	EqualizerView();
	virtual void show();

	QListViewItem *listItem(const Preset *) const;

public slots:
	void qCheckBoxIsStupid(bool);

	void addPreset();
	void removePreset();
	void changed(QListViewItem*);
	void changedEq();

	void presetAdded(Preset *);
	void presetRemoved(Preset *);

	void renamed(QListViewItem *i);
	void renamed(Preset *);
	
private:
	bool first;
	QCheckBox *mOn;
	QSlider *mPreamp;
	KListView *mPresets;
	bool mGoingPreset;
	QPushButton *mRemovePreset, *mAddPreset;
};

#endif

