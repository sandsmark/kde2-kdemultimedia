#ifndef EFFECTS_VIEW_H
#define EFFECTS_VIEW_H

#include <kdialogbase.h>
#include <klistview.h>
#include <kiconview.h>
class EffectList;
class QDragObject;
class Effect;

class EffectView : public KDialogBase
{
Q_OBJECT
public:
	EffectView();

	QListViewItem *toListItem(Effect *e) const;
	EffectList *active() const {return mActive;}

	virtual void show();
	
protected slots:
	void dropEvent(QDropEvent*, QListViewItem*);

public slots:
	void added(Effect *item);
	void removed(Effect *item);
	void moved(Effect *item);

	// buttons
	void moveDown();
	void moveUp();
	void moveLeft();
	void moveRight();
	void removeCurrent();
	void leftHelp();
	void rightHelp();

	void activeChanged(QListViewItem*);
	void availableChanged(QIconViewItem*);
	
private:
	QIconView *mAvailable;
	EffectList *mActive;
	
	QPushButton *mUp, *mDown, *mLeft, *mRight, *mLeftHelp, *mRightHelp;
	bool first;
};


class EffectList : public KListView
{
Q_OBJECT
public:
	EffectList(QWidget *parent);
	virtual bool acceptDrag(QDropEvent *) const;
	virtual QDragObject *dragObject() const;
};

class EffectAvailable : public KIconView
{
Q_OBJECT
public:
	EffectAvailable(QWidget *parent, EffectView*);
	virtual QDragObject *dragObject();

protected:
	virtual void contentsDropEvent(QDropEvent*);

private:
	EffectView *mView;
};

#endif

