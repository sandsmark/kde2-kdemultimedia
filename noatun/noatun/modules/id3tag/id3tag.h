#ifndef NID3_H
#define NID3_H

#include <player.h>
#include <plugin.h>
#include <qobject.h>
#include <noatunpref.h>

class KLineEdit;
class QLineEdit;
class Editor;

class IDPrefs : public CModule
{
Q_OBJECT
public:
	IDPrefs(QObject *parent);
	virtual void save();

public slots:
	void changed(const QString &);

signals:
	void saved();

private:
	KLineEdit *mString;
	QLineEdit *mPreview;
};


class NID3 : public QObject, public Plugin
{
Q_OBJECT
NOATUNPLUGIND

public:
	NID3();
	virtual ~NID3();

	Editor *editor() const {return mEditor;}
public slots:
	void newSong();
	void editID3();

private:
	Editor *mEditor;
	int menuID;
};

#endif

