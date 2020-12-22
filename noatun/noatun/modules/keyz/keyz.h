#ifndef KEYZ_H
#define KEYZ_H

#include <noatunpref.h>
#include <plugin.h>

class Keyz : public QObject, public Plugin
{
	Q_OBJECT
	NOATUNPLUGIND;

public:
	Keyz();
	~Keyz();

	static KGlobalAccel *accel() { return s_accel; }

public slots:
	void slotVolumeUp();
	void slotVolumeDown();
    
	void slotForward();
	void slotBackward();
    
private:
	static KGlobalAccel *s_accel;
};


class KeyzPrefs : public CModule
{
	Q_OBJECT

public:
	KeyzPrefs( QObject *parent );
	virtual void save();

private:
	KKeyEntryMap m_keys;

};

#endif // KEYZ_H
