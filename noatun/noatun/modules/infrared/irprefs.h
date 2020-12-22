
#ifndef _IRPREFS_H_
#define _IRPREFS_H_

#include <qmap.h>
#include <noatunpref.h>

class QCheckBox;
class QListViewItem;
class KListView;
class KComboBox;
class KIntSpinBox;
class Lirc;

class IRPrefs : public CModule
{
Q_OBJECT
public:
	enum Action { None, Play, Stop, Previous, Next, VolumeDown, VolumeUp };
	
	IRPrefs(QObject *parent);
	
	virtual void save();
	virtual void reopen();

	static const QString actionName(Action);
	static Action actionFor(const QString &, const QString &, int);
	
public slots:
	static Lirc *s_lirc;

private slots:
	void slotCommandSelected(QListViewItem *);
	void slotActionActivated(int);
	void slotRepeatToggled(bool);
	void slotIntervalChanged(int);

private:
	static void readConfig();

	KListView *m_commands;
	KComboBox *m_action;
	QCheckBox *m_repeat;
	KIntSpinBox *m_interval;

	struct Command
	{
		Action action;
		int interval;
	};
	static bool s_configRead;
	static QMap<QString, Command> s_commands;
};

#endif

