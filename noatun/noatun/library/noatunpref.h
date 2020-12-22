#ifndef NOATUNPREF_H
#define NOATUNPREF_H

#include <kdialogbase.h>
#include <noatunapp.h>
#include <qlist.h>

class CModule;

class NoatunPreferences : public KDialogBase
{
Q_OBJECT
friend class CModule;

public:
	NoatunPreferences(QWidget *parent);

public:
	virtual void show();

protected:
	virtual void slotOk();
	virtual void slotApply();

private:
	void add(CModule *page);
	void remove(CModule *page);
	
private:
	class NoatunPreferencesPrivate;
	NoatunPreferencesPrivate *d;

	QList<CModule> mModules;
};

class CModule : public QWidget
{
Q_OBJECT

public:
	/**
	 * arguments are short and long descriptions
	 * for this module, respectively
	 *
	 * parent is the object that is this modules virtual-parent.
	 * When that is deleted, this also will go away, automagically.
	 **/
	CModule(const QString &name, const QString &description, QObject *parent=0);

	virtual ~CModule();

public slots:
	/**
	 * save all your options, and apply them
	 **/
	virtual void save() {}
	/**
	 * reload all options (e.g., read config files)
	 **/
	virtual void reopen() {}

private slots:
	void ownerDeleted();

private:
	class CModulePrivate;
	CModulePrivate *d;
};


#endif // NOATUNPREF_H
