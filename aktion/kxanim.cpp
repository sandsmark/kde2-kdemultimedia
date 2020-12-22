#include <qdir.h>
#include <qfile.h>
#include <qsize.h>
#include <qsizepolicy.h>
#include "kxanim.h"

#include <kapp.h>
#include <klocale.h>

#include <unistd.h>
#include <string.h>

#include <iostream.h>

/* some #defines */
#define KA_ERROR_NONE        0
#define KA_ERROR_FILENAME    1
#define KA_ERROR_INFO        2
#define KA_ERROR_UNSUPPORTED 3
#define KA_ERROR_ACTIVE      4
#define KA_ERROR_EXECUTABLE  5

KXAnim::KXAnim(QWidget *parent, const char *name) : QWidget( parent, name)
{
    /* default autoresize */
    autoResize = true;

    /* default loop state */
    loop = true;

    /* Get the base window ID */
    window = this->winId();

    /* Get the X11 display */
    dpy = this->x11Display();
    window_atom = XInternAtom(dpy, "XANIM_PROPERTY", 0);

    /*** connections ***/
    /* KProcess output */
    connect(&proc, SIGNAL( receivedStdout(KProcess *, char *, int)),
            this, SLOT( getOutput( KProcess *, char *, int)) );
    /* KProcess output */
    connect(&proc, SIGNAL( receivedStderr(KProcess *, char *, int)),
            this, SLOT( getOutput( KProcess *, char *, int)) );
    /* KProcess terminated */
    connect(&proc, SIGNAL( processExited(KProcess *)),
            this, SLOT( emitStopped( KProcess *)) );
    setDefaults();
    resetXAnimDefaults();
}

void KXAnim::resetXAnimDefaults()
{
        /* =========== SOUND ============ */
        audio=true;
        audioSync=true;
        audioInitialVolume=40;
        /* =========== COLOR ============ */
        colorMapping=none;
        colorAhead=5;
        /* =========== GAMMA ============ */
        gammaDisplay=1.0;
        /* =========== SCALING ============ */
        resizing=true;

        scaleFactor=1.0;
        scaleHFactor=1.0;
        scaleVFactor=1.0;
        scaleWidth=0;
        scaleHeight=0;
        scaleToBuffer=false;

        scaleFactorB=1.0;
        scaleHFactorB=1.0;
        scaleVFactorB=1.0;
        scaleWidthB=0;
        scaleHeightB=0;
        scaleToDisplay=false;
        /* =========== OTHERS ============ */
        loading=2;
        //preload=false;
        x11Shared=false;
        multiBuffer=true;
        usePixmap=false;
        x11VisualClass="default";
        pauseAt=-1;
        extras="";
        executable="xanim";
}

void KXAnim::setDefaults()
{
    /* we aren't playing anything! */
    playing = false;
    /* we don't have a file yet! */
    fileName = "";
    /* x-anim is not active */
    active = false;
    /* set the default sizes */
    videoHeight=1;
    videoWidth=1;
    /* set the default framerate */
    videoSpeed=0.0;
    /* we have no frames! */
    videoFrames=0;
    /* empty the outpu buffer */
    outBuff = "";
    /* we have no video, so: */
    videoCodec="";
    /* no errors */
    errorCode=KA_ERROR_NONE;
    errorString="";
}

void KXAnim::changeProperty( const char *c )
{
    /* send the char(s)... */
    if (active)
	  XChangeProperty(dpy, window, window_atom, XA_STRING, 8, PropModeReplace, (unsigned char *)c, strlen(c));
}

void KXAnim::updateSize()
{
   int x,y;

   x=int(videoWidth * scaleFactor);
   y=int(videoHeight * scaleFactor);
   if (scaleHFactor!=1.0)
      x=int(videoWidth * scaleHFactor);
   else
      if (scaleWidth!=0)
         x=scaleWidth;
   if (scaleVFactor!=1.0)
      y=int(videoWidth * scaleVFactor);
   else
      if (scaleHeight!=0)
         y=scaleHeight;
   resize(x,y);

//   reinterpret_cast<QWidget *>(parent())->updateGeometry();
//   if (x!=width() || y!=height()) setFixedSize(x,y);
}

void KXAnim::play()
{
    QString s;

    if (!fileName.isEmpty())
    {
        if (active == false)
        /* Activate x-anim */
        {
            outBuff="";
            proc.clearArguments();
            proc << executable;
            proc << "-Zr";
            s.sprintf("+W%d",winId());
            proc << s;
            parseParameters();
            if (autoResize)
               /* resize the widget properly */
               updateSize();
            proc << fileName;

            proc.start(KProcess::NotifyOnExit,KProcess::Stdout);
            active = true;
        }
        playing = !playing;
        changeProperty(" ");
    }
}

void KXAnim::pause()
{
    if (playing==true)
    {
        playing = false;
        play();
    }
}

void KXAnim::stop()
{
    changeProperty("q");
}

void KXAnim::stepForward()
{
    playing = false;
    changeProperty(".");
}

void KXAnim::stepBack()
{
    playing = false;
    changeProperty(",");
}

void KXAnim::stepForwardWA()
{
    playing = false;
    changeProperty("/");
}

void KXAnim::stepBackWA()
{
    playing = false;
    changeProperty("m");
}

void KXAnim::toggleSound()
{
    changeProperty("s");
}

void KXAnim::volumeIncrement()
{
   changeProperty("3");
}

void KXAnim::volumeDecrement()
{
   changeProperty("2");
}

void KXAnim::setVolume(int v)
{
    QCString s;

    s.sprintf("v%d",v);
    changeProperty(s);
}

void KXAnim::faster()
{
    changeProperty("-");
}

void KXAnim::slower()
{
    changeProperty("=");
}

void KXAnim::resetSpeed()
{
    changeProperty("0");
}

void KXAnim::setFile(QString file)
{
    errorCode=KA_ERROR_NONE;

    if (!file.isEmpty() && QFile::exists(file) && !QDir(file).exists())
    {
        if (active == false)
        /* Activate x-anim */
        {
            fileName = file;
            outBuff="";
            proc.clearArguments();
            proc << executable;
            proc << "+Zv" << "+v" << "+f" << "-Ae";
            proc << fileName;
            /* just to get the file information */
            disconnect(&proc, SIGNAL( processExited(KProcess *)),
                       this, SLOT( emitStopped( KProcess *)) );
            connect(&proc, SIGNAL( processExited(KProcess *)),
                    this, SLOT( checkOutput( KProcess *)) );
            if (proc.start(KProcess::NotifyOnExit,KProcess::AllOutput)==false)
            {
                /* can't finde the xanim executable! */
                errorCode = KA_ERROR_EXECUTABLE;
                setErrorString( errorCode, fileName );
                emit stopped();
            }
        }
        else
        {
            errorCode = KA_ERROR_ACTIVE;
            setErrorString( errorCode, file );
            emit stopped();
        }
    }
    else
    {
       errorCode = KA_ERROR_FILENAME;
       setErrorString( errorCode, file );
       emit stopped();
    }
}

void KXAnim::checkOutput( KProcess *)
{
/*
  the xanim has stopped. Check the output and emit the stopped signal
*/
    disconnect(&proc, SIGNAL( processExited(KProcess *)),
               this, SLOT( checkOutput( KProcess *)) );
    connect(&proc, SIGNAL( processExited(KProcess *)),
            this, SLOT( emitStopped( KProcess *)) );

    playing = false;
    active = false;
    errorCode=getVideoInfo();
    if (errorCode==0)
       if (autoResize)
          /* resize the widget properly */
          updateSize();
    setErrorString( errorCode, fileName );
    emit stopped();
}

int KXAnim::getVideoInfo()
{
    int pos, pos2, pos3;
    QString s;

    /*** check the output of xanim to see if the executable is valid ***/
    if ( (pos=outBuff.find("XAnim Rev",0,false)) == -1)
        return KA_ERROR_EXECUTABLE;

    /*** check the output of xanim to get the file info ***/
    if ( (pos=outBuff.find("Video Codec:",0,false)) != -1)
    {
        /* check the video codec */
        if ( (pos2=outBuff.find("unsupported by this executable",pos+12,false)) == -1)
        {
            if ( (pos3=outBuff.find("depth=",pos+13,false)) != -1)
            {
                videoCodec=outBuff.mid(pos+13,pos3-(pos+13));
            }
            else return KA_ERROR_INFO;
        }
        else return KA_ERROR_UNSUPPORTED;
    } else videoCodec="";

//    if ( (pos=outBuff.find("Frame Stats:",0,false)) != -1)
//    {
        /* check the size */
        pos2=outBuff.find("Size",0,false);
        if (pos2==-1)
        {
            pos2=outBuff.findRev("MPG",-1,false);
            if (pos2!=-1) pos2--;
        }
        if (pos2!=-1)
        {
            if ( (pos3=outBuff.find("x",pos2+5,false)) != -1)
            {
                s=outBuff.mid(pos2+5,pos3-(pos2+5));
                videoWidth=s.toInt();
                if ( (pos2=outBuff.find(" ",pos3,false)) != -1)
                {
                    s=outBuff.mid(pos3+1,pos2-(pos3+1));
                    videoHeight=s.toInt();
                }
                else return KA_ERROR_INFO;
            }
            else return KA_ERROR_INFO;
        }
        else return KA_ERROR_INFO;

        /* check the number of frames */
        if ( (pos2=outBuff.find("frames",0,false)) != -1)
        {
            if ( (pos3=outBuff.find(" ",pos2+8,false)) != -1)
            {
                s=outBuff.mid(pos2+7,pos3-(pos2+7));
                videoFrames=s.toInt();
            }
            else return KA_ERROR_INFO;
        }
        else videoFrames=0;

        /* check the video speed */
        if ( (pos2=outBuff.find("fps=",0,false)) != -1)
        {
            if ( (pos3=outBuff.find("\n",pos2+4,false)) != -1)
            {
                s=outBuff.mid(pos2+4,pos3-(pos2+4));
                videoSpeed=s.toFloat();
            }
            else return KA_ERROR_INFO;
        }
        else videoSpeed=0;
//    }
//    else return KA_ERROR_INFO;

    /* no errors! */
    return KA_ERROR_NONE;
}

void KXAnim::getOutput( KProcess *, char *text, int len)
{
    temp.fill(' ',len+1);
    temp.replace(0,len,text);
    temp[len]='\0';
    outBuff.append(temp);
}

void KXAnim::emitStopped( KProcess *)
{
/*
  the xanim has stopped the playing of a video
  emit the stopped signal
*/
    playing = false;
    active = false;
    emit stopped();
}

void KXAnim::setErrorString( int n, QString file )
{
   /* no errors */
   errorString=QString::null;
   switch (n)
   {
       case KA_ERROR_FILENAME   : errorString=i18n("%1: wrong file name.").arg(file); break;
       case KA_ERROR_INFO       : errorString=i18n("Error reading file info."); break;
       case KA_ERROR_UNSUPPORTED: errorString=i18n("Unsupported video codec."); break;
       case KA_ERROR_ACTIVE     : errorString=i18n("The video is active!"); break;
       case KA_ERROR_EXECUTABLE : errorString=i18n("Can't find the xanim executable:\n%1").arg(executable); break;
   }
}

void KXAnim::closeEvent( QCloseEvent *e)
{
    if (active)
        stop();
    while (active);
    e->accept();
}

KXAnim::~KXAnim()
{
}

void KXAnim::parseParameters()
{
    QString s;
    int pos,pos2;

    if (!loop)
        proc<<"+Ze";

    /* =========== SOUND ============ */
    if (audio==false) proc<<"-Ae";
    if (audioSync==false) proc<<"-Ak";
    if (audioInitialVolume!=40)
    {
       s.sprintf("+Av%d",audioInitialVolume);
       proc<<s;
    }
    /* =========== COLOR ============ */
    switch (colorMapping)
    {
       case none:        break;
       case static332:   proc<<"+C3"; break;
       case lookupTable: proc<<"+CF4"; break;
       case grayScale:   proc<<"+Cg"; break;
    }
    if (colorMapping==lookupTable && colorAhead!=5)
    {
       s.sprintf("+Cs%d",colorAhead);
       proc<<s;
    }
 
    /* =========== GAMMA ============ */
    if (gammaDisplay!=1.0)
    {
        s.sprintf("+Gd%1.5f",gammaDisplay);
        proc<<s;
    }

    /* =========== SCALING ============ */
    if (resizing==true) proc<<"+Sr";

    if (!autoResize)
    {
        s.sprintf("+Sx%d", width());
        proc << s;
        s.sprintf("+Sy%d", height());
        proc << s;
    }
    else
    {
        if (scaleFactor != 1.0)
        {
            s.sprintf("+Ss%2.5f",scaleFactor);
            proc<<s;
        }
        else
        {
            if (scaleHFactor != 1.0)
            {
               s.sprintf("+Sh%10.8f",scaleHFactor);
               proc<<s;
            }
            if (scaleVFactor != 1.0)
            {
               s.sprintf("+Sv%10.8f",scaleVFactor);
               proc<<s;
            }
        }
        if (scaleWidth!=0)
        {
            s.sprintf("+Sx%d",scaleWidth);
            proc<<s;
        }
        if (scaleHeight!=0)
        {
            s.sprintf("+Sy%d",scaleHeight);
            proc<<s;
        }
        if (scaleToBuffer==true) proc<<"+Sc";
    }
 
    if (scaleFactorB != 1.0)
    {
        s.sprintf("+SS%10.8f",scaleFactorB);
        proc<<s;
    }
    else
    {
        if (scaleHFactorB != 1.0)
        {
           s.sprintf("+SH%10.8f",scaleHFactorB);
           proc<<s;
        }
        if (scaleVFactorB != 1.0)
        {
           s.sprintf("+SV%10.8f",scaleVFactorB);
           proc<<s;
        }
    }
    if (scaleWidthB!=0)
    {
        s.sprintf("+SX%d",scaleWidthB);
        proc<<s;
    }
    if (scaleHeightB!=0)
    {
        s.sprintf("+SY%d",scaleHeightB);
        proc<<s;
    }
    if (scaleToDisplay==true) proc<<"+SC";

    /* =========== OTHERS ============ */
    switch (loading)
    {
       case 0: break;
       case 1: proc<<"+b"; break;
       case 2: proc<<"+f"; break;
    }
    if (x11Shared==true && loading!=1) proc<<"+B";
    if (multiBuffer==false) proc<<"-D";
    if (usePixmap==true) proc<<"+p";
    if (x11VisualClass.contains("default",false)==0)
    {
       s="+V";
       s+=x11VisualClass;
       proc<<s;
    }
    if (pauseAt!=-1)
    {
        s.sprintf("+Zp%d",pauseAt);
        proc<<s;
    }
    extras.simplifyWhiteSpace();
    if (!extras.isEmpty())
    {
       pos=-1;
       do
       {
          pos2=extras.find(' ',pos+1);
          if (pos2!=-1)
             s=extras.mid(pos+1,pos2-(pos+1));
          else
             s=extras.mid(pos+1,extras.length()-(pos+1));
          proc<<s;
          pos=pos2;
       } while (pos2!=-1);       
    }
}

void KXAnim::mousePressEvent( QMouseEvent *mouse)
{
   emit mouseClick(mouse);
}

QSize KXAnim::sizeHint() const
{
  int x,y;

  x=int(videoWidth * scaleFactor);
  y=int(videoHeight * scaleFactor);
  if (scaleHFactor!=1.0)
    x=int(videoWidth * scaleHFactor);
  else
    if (scaleWidth!=0)
      x=scaleWidth;

  if (scaleVFactor!=1.0)
    y=int(videoWidth * scaleVFactor);
  else
    if (scaleHeight!=0)
      y=scaleHeight;

  return QSize(x,y);
}

QSize KXAnim::minimumSizeHint() const
{
  return QSize(16,16);
}

QSizePolicy KXAnim::sizePolicy() const
{
  return QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
}


#include "kxanim.moc"
