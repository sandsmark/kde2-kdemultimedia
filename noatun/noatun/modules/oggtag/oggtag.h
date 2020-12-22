#ifndef OTAG_H
#define OTAG_H

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


class OTAG : public QObject, public Plugin
{
Q_OBJECT
NOATUNPLUGIND

public:
	OTAG();

public slots:
	void newSong();

private:
};

#endif

