#ifndef DCOPIFACE_H
#define DCOPIFACE_H

#include <player.h>
#include <plugin.h>
#include <qserversocket.h>
#include <qsocket.h>
#include <unistd.h>

class Net : public QServerSocket, public Plugin
{
Q_OBJECT
NOATUNPLUGIND;

public:
	Net();
	~Net();

public slots:
	void newSong();

private slots:
	void closed();
protected:
	virtual void newConnection(int socket);

private:
	QList<QSocket> mFDs;
};

#endif

