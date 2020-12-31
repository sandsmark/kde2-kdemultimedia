#ifndef USERINTERFACE_H
#define USERINTERFACE_H

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <plugin.h>
#include <noatunapp.h>
#include <qwidget.h>
#include "parser.h"
#include <qbitmap.h>
#include <qlist.h>
#include <iostream>
#include <qcstring.h>


class KJWidget;
class KHelpMenu;
class KJSeeker;
class NoatunPreferences;
class KJToolTip;

class KJFont;

class KJLoader :  public QWidget, public UserInterface, public Parser
{
Q_OBJECT
NOATUNPLUGIND

	friend class KJWidget;
public:
	KJLoader();
	~KJLoader();

public:
	void minimize();
	KHelpMenu *helpMenu() const { return mHelpMenu; }
	QStringList &item(const QString &key) { return *Parser::find(key); }

	QList<KJWidget> widgetsAt(const QPoint &pt) const;

	void removeChild(KJWidget *c);
	void addChild(KJWidget *c);

	static bool cleanSkin(const QString &dir);

	static QString guessRcFile(const QString &dir);

public slots:
	void loadSkin(const QString &file);

protected:
	void unloadSkin();
	
public slots:
	void timeUpdate();
	void newSong();

private slots:
	void loadSeeker();
	
protected:
	virtual void mouseMoveEvent(QMouseEvent *e);
	virtual void mousePressEvent(QMouseEvent *e);
	virtual void mouseReleaseEvent(QMouseEvent *e);
	virtual void paintEvent(QPaintEvent *e);
	virtual void closeEvent(QCloseEvent*e);
	virtual void wheelEvent(QWheelEvent *e);

	virtual void dragEnterEvent(QDragEnterEvent *event);
	virtual void dropEvent(QDropEvent *event);

public:
	static KJLoader* kjofol;
	
private:
	bool moving;
	QPoint mMousePoint;
	QList<KJWidget> subwidgets;
	KJWidget *mClickedIn;
	KHelpMenu *mHelpMenu;
	KJFont *mText, *mNumbers;
	KJToolTip *mTooltips;
	QString mCurrentSkin;
};

class KJWidget
{
public:
	KJWidget(KJLoader *);
	virtual ~KJWidget() {};
	// called when the widget should paint
	virtual void paint(QPainter *, const QRect &) {};
	// called to recieve the rect this widget is in
	virtual QRect rect() const { return mRect; }
	// called when pressed in this widget
	virtual bool mousePress(const QPoint &) {return false; }
	// called when the mouse is released after clicked in this widget
	virtual void mouseRelease(const QPoint &, bool){}
	virtual void mouseMove(const QPoint &, bool) {}
	// called with the current time (mille)
	virtual void timeUpdate(int) {}
	// called when a new song is playing, player() is ready with it too
	virtual void newFile() {}
	
	// called when the mouse is moved while clicked in this widget
	// repaint myself
	virtual void repaint(bool me=true, const QRect &rect=QRect(), bool clear=false);

	virtual QString tip() { return 0; }

public:
	static QBitmap getMask(const QImage &color, register QRgb=qRgb(255,0,255));
	
protected:
	QString backgroundPressed(const QString &bmp);
	KJLoader *parent() const {return mParent;}
	KJLoader &parser() const {return *mParent;}

	KJFont &textFont() const {return *mParent->mText;}
	KJFont &timeFont() const {return *mParent->mNumbers;}
	
	void setRect(const QRect& rect) {mRect=rect;}
	void setRect(int x, int y, int xs, int ys) {mRect=QRect(x,y,xs,ys);}
private:
	KJLoader *mParent;
	QRect mRect;
	
};

class Background : public KJWidget
{
public:
	Background(KJLoader *);
	virtual void paint(QPainter *, const QRect &rect);
	virtual bool mousePress(const QPoint &) {return false;}
	virtual void mouseRelease(const QPoint &, bool) {}

private:
	QPixmap mBackground;
};


class KJButton : public KJWidget
{
public:
	KJButton(const QStringList&, KJLoader *);
	virtual void paint(QPainter *, const QRect &rect);
	virtual bool mousePress(const QPoint &pos);
	virtual void mouseRelease(const QPoint &pos, bool);
	virtual void showPressed(bool b=true);
	
	virtual QString tip();

private:
	QPixmap mBackground;
	bool mPushedPixmap;
	QPixmap mPressed;
	QString mTitle;
	bool mShowPressed;
};


class KJSeeker : public KJWidget
{
public:
	KJSeeker(const QStringList &i, KJLoader *);
	~KJSeeker();
	
	
	virtual void paint(QPainter *, const QRect &rect);
	virtual bool mousePress(const QPoint &pos);
	virtual void mouseRelease(const QPoint &pos, bool);

	void timeUpdate(int mille);

	void closest();
	
private:
	QImage mScale;
	QImage mActive;
	QPixmap *barmode[256];
	QBitmap barModeMask;
	int g;
};

class KJVolumeBMP : public KJWidget
{
public:
	KJVolumeBMP(const QStringList &, KJLoader *parent);

	virtual void paint(QPainter *, const QRect &rect);
	virtual bool mousePress(const QPoint &pos);
	virtual void mouseRelease(const QPoint &pos, bool);
	virtual void timeUpdate(int);
	virtual void mouseMove(const QPoint &pos, bool);

	virtual QString tip();
private:
	QPixmap mImages;
	QImage mPos;
	int mVolume;
	int mWidth, mCount;
};

class KJFont 
{
public:
	KJFont(const QString &prefix, KJLoader *parent);
	// draw the string str to dev at position pos, within rectangle limit in relation to pos
	QPixmap draw(const QCString &str, int wide, const QPoint &pt=QPoint(0,0)) const;
	QPixmap draw(const QString &str, int wide, const QPoint &pt=QPoint(0,0)) const
		{ return draw(QCString(str.latin1()), wide, pt); }

protected:
	void drawCharacter(QPixmap *dev, const QPoint &to, char c) const;

	QPoint charSource(char c) const;

private:
	QPixmap mText;
	int mWidth, mHeight;
	int mSpacing;
	bool mTransparent;

	const char *mString[3];
	char mNullChar;
};

class KJFilename : public QObject, public KJWidget
{
Q_OBJECT
public:
	KJFilename(const QStringList &, KJLoader *parent);

	virtual void paint(QPainter *, const QRect &rect);
	virtual bool mousePress(const QPoint &pos);
	virtual void newFile();
	void prepareString(const QCString &str);

	virtual void timerEvent(QTimerEvent *);

private:
	int mDistance;
	int mWidth;
	int mTickerPos;
	QPixmap mView;
};

class KJTime : public KJWidget
{
public:
	KJTime(const QStringList &, KJLoader *parent);

	virtual void paint(QPainter *, const QRect &rect);
	virtual bool mousePress(const QPoint &pos);
	virtual void timeUpdate(int);

	void prepareString(const QCString &time);
private:
	QCString mCurrentStr;
	int mWidth;
	QPixmap mTime;
};

class KJVisScope : public KJWidget
{
public:
	KJVisScope(KJLoader *parent) : KJWidget(parent) {}
	enum Visuals { Null=0, FFT, Mono };
	void swapScope(Visuals newOne);
};

class KJNullScope : public KJVisScope
{
public:
	KJNullScope(const QStringList &, KJLoader *parent);
	virtual bool mousePress(const QPoint&);
};

class KJVis : public KJVisScope, public MonoFFTScope
{
public:
	KJVis(const QStringList &, KJLoader *parent);
	virtual void scopeEvent(float *d, int size);
	
	virtual bool mousePress(const QPoint&);

private:
	QColor mColor;
	int mMultiples;
};

class KJScope : public KJVisScope, public MonoScope
{
public:
	KJScope(const QStringList &, KJLoader *parent);
	virtual void scopeEvent(float *d, int size);
	
	virtual bool mousePress(const QPoint&);

private:
	QColor mColor;
	int mMultiples;
	int mWidth;
	int mHeight;
};

class KJEqualizer : public QObject, public KJWidget
{
Q_OBJECT
public:
	KJEqualizer(const QStringList &, KJLoader *parent);
	
	virtual void mouseMove(const QPoint &pos, bool);
	virtual bool mousePress(const QPoint&);
	virtual void paint(QPainter *, const QRect &rect);
	int barNum(const QPoint &pos) const;
	int level(const QPoint &pos) const;

public slots:
	void updateBuffer();
	
private:
	int mBands;
	int mXSpace;

	int mXSize;
	int mNumber;
	QPixmap mBars;
	QPixmap mBuffer;
};


#endif

