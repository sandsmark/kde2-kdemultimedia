#ifndef USERINTERFACE_H
#define USERINTERFACE_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <plugin.h>
#include <noatunapp.h>

// Pref dialog pointer global now for position saving
#include "noatunpref.h"

class Player;
class QSlider;
class QPushButton;
class KStatusBar;

/**
 * @short Main window class
 * @author Charles Samuels <charles@kde.org>
 * @version 0.1
 */
class MilkChocolate : public QWidget, public UserInterface
{
Q_OBJECT
public:
	MilkChocolate();
	virtual ~MilkChocolate();
	void load(const QString& url);

protected:
	virtual void dragEnterEvent(QDragEnterEvent *event);
	virtual void dropEvent(QDropEvent *event);
	virtual void closeEvent(QCloseEvent*);
	virtual void showEvent(QShowEvent*e);
	virtual void mouseReleaseEvent(QMouseEvent *);
	virtual bool eventFilter(QObject*, QEvent*);
	virtual void wheelEvent(QWheelEvent *e);

protected:
	QSlider *seeker() const { return mSeeker; }
	KStatusBar *statusBar() const { return mStatusBar; }

public slots:
	void slotPlaying();
	void slotStopped();
	void slotPaused();

	void playlistShown();
	void playlistHidden();

	void slotTimeout();
	void sliderMoved(int seconds);
	void changeLoopType(int t);
	void skipToWrapper(int second);

signals:
	void skipTo( int ); // emitted by skipToWrapper()

private slots:
	void changeStatusbar(const QString& text, const QString &text2=0);
	void changeCaption(const QString& text);

private:
	QPushButton *mBack, *mStop, *mPlay, *mForward, *mPlaylist, *mRemoveCurrent, *mLoop;
	QSlider *mSeeker, *mVolume;
	KStatusBar *mStatusBar;
};

#endif
