/***************************************************************************
                          kpoti.cpp  -  potentiometer widget
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

#include "kpoti.h"
#include "kpoti.moc"
#include <kdebug.h>
#include <qpainter.h>
#include <qcolor.h>
#include <qdrawutl.h>
#include <qtimer.h>
#include <qkeycode.h>
#include <math.h>
#include <iostream>
#include <qpen.h>

#define PI 3.1415926
static const int motifBorder = 2;
static const int motifLength = 30;
static const int winLength = 9; // Must be odd
static const int thresholdTime = 500;
static const int repeatTime    = 100;
static const float maxAngle = PI*135/180; // 140 degrees to both sides
static const float tickLength = 0.25;
static const int fontSize=8;

/**
  Constructs a poti.

  The \e parent and \e name arguments are sent to the QWidget constructor.
*/
KPoti::KPoti( QWidget *parent, const char *name )
    : QWidget( parent, name )
{
    init();
}



/**
  Constructs a poti.

  \arg \e minValue is the minimum slider value.
  \arg \e maxValue is the maximum slider value.
  \arg \e step is the page step value.
  \arg \e value is the initial value.

  The \e parent and \e name arguments are sent to the QWidget constructor.
*/

KPoti::KPoti( int minValue, int maxValue, int step,
		  int value, QWidget *parent, const char *name )
    : QWidget( parent, name ),
      QRangeControl( minValue, maxValue, 1, step, value )
{
    init(value);
}


void KPoti::init(int value)
{
  color.setNamedColor("red");
  timer = 0;
  potiVal = value;
  potiPos = positionFromValue(value);
  clickOffset = 0;
  state = Idle;
  track = TRUE;
  ticks = TRUE;
  label=TRUE;
  tickInt = 0;
  fontHeight=fontMetrics().height();
  
  space = QMIN( height(), width() );
  center.setX(space/2); center.setY(space/2); 
  setBackgroundMode( PaletteMid );
  
  setFocusPolicy( TabFocus );
  initTicks();
}


/**
  Does what's needed when someone changes the tickmark status
*/

void KPoti::initTicks()
{ 

  space = QMIN( height(), width() );
  center.setX(width()/2); 
  if (label==TRUE) {
    space -= fontSize -2;
  }
  if ( ticks == TRUE ) 
  {
    double spaced = space;
	spaced /= 1.0+tickLength/2;

    space=(int)spaced;
  }
  
  buttonRadius = (space-1)/2 ; center.setY(height()-buttonRadius-1);
 
}


/**
  Enables slider tracking if \e enable is TRUE, or disables tracking
  if \e enable is FALSE.

  If tracking is enabled (default), the slider emits the
  valueChanged() signal whenever the slider is being dragged.  If
  tracking is disabled, the slider emits the valueChanged() signal
  when the user releases the mouse button (unless the value happens to
  be the same as before).

  \sa tracking()
*/

void KPoti::setTracking( bool enable )
{
    track = enable;
}


/**
  \fn bool KPoti::tracking() const
  Returns TRUE if tracking is enabled, or FALSE if tracking is disabled.

  Tracking is initially enabled.

  \sa setTracking()
*/


/**
  \fn void KPoti::valueChanged( int value )
  This signal is emitted when the slider value is changed, with the
  new slider value as an argument.
*/

/**
  \fn void KPoti::sliderPressed()
  This signal is emitted when the user presses the slider with the mouse.
*/

/**
  \fn void KPoti::sliderMoved( int value )
  This signal is emitted when the slider is dragged, with the
  new slider value as an argument.
*/

/**
  \fn void KPoti::sliderReleased()
  This signal is emitted when the user releases the slider with the mouse.
*/

/**
  Calculates slider position corresponding to value \a v. Does not perform
  rounding.
*/

float KPoti::positionFromValue( int v ) const
{

    int range = maxValue() - minValue();
    return ( (v - minValue() ) *2* maxAngle) / range - maxAngle;
}


/**
  Calculates value corresponding to poti position \a p. Performs rounding.
*/

int KPoti::valueFromPosition( float p ) const
{
    int range = maxValue() - minValue();
    return (int) (minValue() + ((p+maxAngle)*range)/(2*maxAngle));
}

/*!
  Implements the virtual QRangeControl function.
*/

void KPoti::rangeChange()
{
    float newPos = positionFromValue( value() );
    if ( newPos != potiPos ) {
	reallyMovePoti( newPos ); 
    }
}

/*!
  Changes the value (slot)
*/

void KPoti::valueChange()
{
    if ( potiVal != value() ) {
	float newPos = positionFromValue( value() );
	potiVal = value();
	reallyMovePoti( newPos );
    }
    emit valueChanged(value());
}


/*!
  Handles resize events for the poti.
*/

void KPoti::resizeEvent( QResizeEvent * )
{
    rangeChange();
    initTicks();
}


/*!
  Reimplements the virtual function QWidget::setPalette().

*/

void KPoti::setPalette( const QPalette &p )
{
  
    setBackgroundMode( PaletteMid );
    QWidget::setPalette( p );
}


void KPoti::setLabel(bool s)
{
  label=s;
  initTicks();
}

/**
  Sets the color of the button
  */
void KPoti::setColor( const QColor &c )
{
  color=c;
  repaint();
}


void KPoti::paintPoti( QPainter *p)
{
  QColorGroup g = colorGroup();
  p->setBrush( color );
  QCOORD x=center.x(), y=center.y();
  

  //qDrawShadePanel( p, r, g, FALSE, 2, &fill );
  p->drawEllipse(x-buttonRadius+5,y-buttonRadius+5,
		 (buttonRadius-5)*2, (buttonRadius-5)*2);
  //p->drawLine((int)( x+s*buttonRadius),(int)( y-c*buttonRadius),
  //    (int)(x+s*buttonRadius*0.3),(int)( y-c*buttonRadius*0.3));
  p->translate(x-1,y);
  p->rotate(potiPos*180/PI);
  int end=((buttonRadius -5)*4)/10;
  if (end<=2) end=0;
  qDrawShadeLine (p, 0, -buttonRadius+6,0 ,-end ,
		  g, TRUE, 1,2);
}

/*!
  Performs the actual moving of the slider.
*/

void KPoti::reallyMovePoti( float newPos )
{
  QPainter p;
  p.begin( this );  
  p.setPen(NoPen);
  potiPos = newPos;
  paintPoti(&p);
  p.end();
}




/**
  Handles paint events for the slider.
*/

void KPoti::paintEvent( QPaintEvent *e )
{
    
    QPainter p;
    QPen pen;
    p.begin( this );
    QRect paintRect = e->rect();
    p.setClipRect( paintRect );
    
    QColorGroup g = colorGroup();
	if(backgroundMode() == FixedPixmap)
	{
		p.drawTiledPixmap( 0, 0, width(), height(), *backgroundPixmap() );
	}
	else
	{
    	p.fillRect( 0, 0, width(), height(), g.background() );
	}
    
    const QColor c0 = black;
    const QColor c4 = g.light();

    QFont font;
    font=p.font(); font.setPointSize(8); p.setFont(font);
    p.drawText(0,fontSize,name());

    int interval = tickInt;
    if ( interval <= 0 ) {
	interval = 12;
    }
    if (ticks)
	drawTicks( &p, buttonRadius+2, (int)(buttonRadius*(1+tickLength)), interval );
	
 
    p.setBrush(darkGray);
    p.drawEllipse(center.x()-buttonRadius,center.y()-buttonRadius,
	       2*(buttonRadius),2*(buttonRadius));
    p.setPen(c0);
    p.drawArc(center.x()-buttonRadius,center.y()-buttonRadius,
	       2*buttonRadius,2*buttonRadius, 45*16,180*16);
    pen=p.pen();
    pen.setWidth(2);
    p.setPen(pen);
    p.drawArc(center.x()-buttonRadius+3,center.y()-buttonRadius+3,
	      2*(buttonRadius-3),2*(buttonRadius-3), 45*16,-180*16);
  
    p.setPen(c4);
    p.drawArc(center.x()-buttonRadius,center.y()-buttonRadius,
	      2*buttonRadius,2*buttonRadius, 45*16,-180*16);
    pen=p.pen();
    pen.setWidth(2);
    p.setPen(pen);
    p.drawArc(center.x()-buttonRadius+3,center.y()-buttonRadius+3,
	      2*(buttonRadius-3),2*(buttonRadius-3), 45*16,180*16);
  
    if ( hasFocus() ) {
      p.setPen( black ); p.setBrush(NoBrush);
      p.drawRect(  0,0, width(), height() );
    }


    p.setPen(NoPen);
    paintPoti( &p);

    p.end();
}


/*!
  Handles mouse press events for the slider.
*/

void KPoti::mousePressEvent( QMouseEvent *e )
{
  resetState();

  if ( e->button() == MidButton ) {
    float pos = atan2( e->pos().x()-center.x(),- e->pos().y() + center.y() );
    movePoti( pos );
    return;
  }
  if ( e->button() != LeftButton )
    return;
 
  
  int dx=e->pos().x()-center.x(), dy=e->pos().y()-center.y();
  
  if ( dx*dx+dy*dy < buttonRadius*buttonRadius ) {
    state = Dragging;
    clickOffset =  potiVal + (e->pos().y() ) ;
    emit potiPressed();
  }  else if ( e->pos().x() < width()/2 ) {
    state = TimingDown;
    subtractPage();
    if ( !timer )
      timer = new QTimer( this );
    connect( timer, SIGNAL(timeout()), SLOT(repeatTimeout()) );
    timer->start( thresholdTime, TRUE ); 
  } else  {
    state = TimingUp;
    addPage();
    if ( !timer )
      timer = new QTimer( this );
    connect( timer, SIGNAL(timeout()), SLOT(repeatTimeout()) );
    timer->start( thresholdTime, TRUE ); 
  }
}

/*!
  Handles mouse move events for the slider.
*/
void KPoti::mouseMoveEvent( QMouseEvent *e )
{

    if ( (e->state() & MidButton) ) { 		// middle button wins
      float pos = atan2( e->pos().x()-center.x(),- e->pos().y()+center.y() );
      movePoti( pos );
      return;	
    }
    if ( !(e->state() & LeftButton) )
	return;					// left mouse button is up
    if ( state != Dragging )
	return;
    
    
    movePoti( positionFromValue(- e->pos().y() + clickOffset ));
}


/*!
  Handles mouse release events for the slider.
*/

void KPoti::mouseReleaseEvent( QMouseEvent *e )
{
    if ( !(e->button() & LeftButton) )
	return;
    resetState();
}

/*!
  Handles focus in events for the slider.
*/

void KPoti::focusInEvent( QFocusEvent * )
{
    repaint( FALSE );
}


/*!
  Handles mouse entering events.
*/

void KPoti::enterEvent( QEvent * )
{
    emit mouseEntered( potiVal );
}


/*!
  Moves the left (or top) edge of the slider to position 
  \a pos. Performs snapping.
*/

void KPoti::movePoti( float pos )
{
    float newPos = QMIN( maxAngle, QMAX( -maxAngle, pos ) );
    int newVal = valueFromPosition( newPos );
    if ( potiVal != newVal ) {
	potiVal = newVal;
	emit potiMoved( potiVal );
    }
    if ( tracking() && potiVal != value() ) {
	directSetValue( potiVal );
	emit valueChanged( potiVal );
    }


    if ( potiPos != newPos )
	reallyMovePoti( newPos );
}


/*!
  Resets all state information and stops my timer.
*/

void KPoti::resetState()
{
    if ( timer ) {
	timer->stop();
	timer->disconnect();
    }
    switch ( state ) {
    case TimingUp:
    case TimingDown:
	break;
    case Dragging: {
	setValue( valueFromPosition( potiPos ) );
	emit potiReleased();
	break;
    }
    case Idle:
	break;
    default:
	kdWarning() << "KPoti: in wrong state" << endl;
    }
    state = Idle;
}


/*!
  Handles key press events for the slider.
*/

void KPoti::keyPressEvent( QKeyEvent *e )
{
 
    switch ( e->key() ) {
    case Key_Left:
      subtractLine();
      break;
    case Key_Right:
      addLine();
      break;
    case Key_Up:
      addLine();
      break;
    case Key_Down:
      subtractLine();
      break;
    case Key_Prior:
      subtractPage();
      break;
    case Key_Next:
      addPage();
      break;
    case Key_Home:
      setValue( minValue() );
      break;
    case Key_End:
      setValue( maxValue() );
      break;
    default:
	e->ignore();
	return;
    }
    e->accept();
}





/*!
  Makes QRangeControl::setValue() available as a slot.
*/

void KPoti::setValue( int value )
{
    QRangeControl::setValue( value );
}


/*!
  Moves the slider one pageStep() upwards.
*/

void KPoti::addStep()
{
    addPage();
}


/*!
  Moves the slider one pageStep() downwards.
*/

void KPoti::subtractStep()
{
    subtractPage();
}


/*!
  Waits for autorepeat.
*/

void KPoti::repeatTimeout()
{
    ASSERT( timer );
    timer->disconnect();
    if ( state == TimingDown )
	connect( timer, SIGNAL(timeout()), SLOT(subtractStep()) );
    else if ( state == TimingUp )
	connect( timer, SIGNAL(timeout()), SLOT(addStep()) );
    timer->start( repeatTime, FALSE );   
}




/*!  
  Using \a p, draws tickmarks at a distance of \a d from the edge
  of the widget, using \a w pixels and \a i intervals.  
  */

void KPoti::drawTicks( QPainter *p, int d, int w, int i ) const
{
  p->setPen( colorGroup().foreground() );
  float angle,s,c;
  for (int v=0; v<=i; v++) {
    angle=-maxAngle+2*maxAngle*v/i;
    s=sin(angle); c=cos(angle);

    p->drawLine( (int)(center.x()-s*d), (int)(center.y()-c*d),
		 (int)(center.x()-s*w), (int)(center.y()-c*w) );
    }
}

void KPoti::wheelEvent(QWheelEvent *e)
{
	setValue(value()+e->delta()/120*8);
}


/*!
  Sets the way tickmarks are displayed by the slider. \a s can take 
  the following values:
  <ul>
  <li> \c NoMarks
  <li> \c Above
  <li> \c Left
  <li> \c Below
  <li> \c Right
  <li> \c Both
  </ul>
  The initial value is \c NoMarks.
  \sa tickmarks(), setTickInterval()
*/

void KPoti::setTickmarks( bool s )
{
    ticks = s;
    initTicks();
    update();
}



/*!
  Sets the interval between tickmarks to \a i. This is a value interval, 
  not a pixel interval. If \a i is 0, the slider
  will choose between lineStep() and pageStep(). The initial value of 
  tickInterval() is 0.
  \sa tickInterval(), QRangeControl::lineStep(), QRangeControl::pageStep()
*/

void KPoti::setTickInterval( int i )
{
    tickInt = QMAX( 0, i );
    update();
}


/*!
  \fn int KPoti::tickInterval() const
  Returns the interval between tickmarks. Returns 0 if the slider
  chooses between pageStep() and lineStep().
  \sa setTickInterval()
*/
















