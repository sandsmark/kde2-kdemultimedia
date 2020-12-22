/***************************************************************************
                          kpoti.h  -  Potentiometer Widget
                             -------------------
    begin                : Wed Apr 28 23:05:05 MEST 1999

    copyright            : (C) 1999 by Martin Lorenz
    email                : lorenz@ch.tum.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef _KPOTI_H
#define _KPOTI_H

#include "qwidget.h"
#include "qrangect.h"


class QTimer;
struct QPotiData;


class KPoti : public QWidget, public QRangeControl
{
    Q_OBJECT
public:

    KPoti( QWidget *parent=0, const char *name=0 );
    KPoti( int minValue, int maxValue, int step, int value,
	     QWidget *parent=0, const char *name=0 );

    void	setTracking( bool enable );
    bool	tracking() const;

    void 	setPalette( const QPalette & );
    void 	setColor( const QColor & );

    virtual void setTickmarks( bool );
    virtual void setLabel( bool );
    bool        tickmarks() const { return ticks; }

    virtual void setTickInterval( int );
    int 	tickInterval() const { return tickInt; }

public slots:
    void	setValue( int );
    void	addStep();
    void	subtractStep();

signals:
    void	valueChanged( int value );
    void	potiPressed();
    void	potiMoved( int value );
    void	potiReleased();
    void  mouseEntered(int value);

protected:
    void	resizeEvent( QResizeEvent * );
    void	paintEvent( QPaintEvent * );

    void	keyPressEvent( QKeyEvent * );

    void	mousePressEvent( QMouseEvent * );
    void	mouseReleaseEvent( QMouseEvent * );
    void	mouseMoveEvent( QMouseEvent * );
    void  enterEvent( QEvent *);

    void	focusInEvent( QFocusEvent *e );

    void	valueChange();
    void	rangeChange();

    virtual void paintPoti( QPainter * );
    void	drawButton( QPainter *);
    void	drawTicks( QPainter *, int, int, int=1 ) const;

    virtual void wheelEvent(QWheelEvent *e);
private slots:
    void	repeatTimeout();

private:
    enum State { Idle, Dragging, TimingUp, TimingDown };

    void	init(int value=0);
    float      	positionFromValue( int ) const;
    int		valueFromPosition( float ) const;
    void	movePoti( float );
    void	reallyMovePoti( float );
    void	resetState();
    int		potiRadius() const;
    void	initTicks();

    QTimer	*timer;
    float	potiPos;
    int		potiVal;
    int 	clickOffset;
    State	state;
    bool	track;
    QPoint      center;
    bool	ticks,label;
    int		tickInt, space, buttonRadius, fontHeight;
    QColor      color;
private:	// Disabled copy constructor and operator=
    //    KPoti( const KPoti & ) {}
    //KPoti &operator=( const KPoti & ) { return *this; }
};

inline bool KPoti::tracking() const
{
    return track;
}


#endif // _KPOTI_H



