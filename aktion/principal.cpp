#include <qdir.h>

#include "principal.h"

#include <iostream.h>
#include <kconfig.h>

#include <kglobal.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kapp.h>
#include <kmessagebox.h>
#include <kfiledialog.h>
#include <kstddirs.h>
#include <kseparator.h>

#include <qtoolbutton.h>
#include <qiconset.h>
#include <qlabel.h>
#include <qtooltip.h>
#include <qslider.h>
#include <qtimer.h>
#include <qlayout.h>
#include <qdropsite.h>
#include <qdragobject.h>

#ifdef XF86_VM
#include <unistd.h>
#include <aktionVm.h>
#endif

#include "kxanim.h"
#include "aktionConf.h"

#include <kwin.h>

Principal::Principal( const char *name)
	: KMainWindow(0, name, 0)
{
        setCaption("aKtion!");

        config=KGlobal::config();

        QWidget *mainwidget=new QWidget(this);

        setCentralWidget(mainwidget);

        setAcceptDrops(true);

        loader = KGlobal::iconLoader();

        timer = new QTimer(this, "timer");

        menu = new QPopupMenu(0L, "menu");
        menu->insertItem(i18n("Video information"),this,SLOT(click_info()) );
        menu->insertSeparator();
        menu->insertItem(SmallIconSet("fileopen"),
                         i18n("Open file..."),this,SLOT(click_open()) );
        menu->insertItem(SmallIconSet("configure"),
                         i18n("Setup..."),this,SLOT(click_config()) );
        menu->insertSeparator();
        menu->insertItem(SmallIconSet("original"),
                         i18n("Original size"),this,SLOT(click_original()) );
        menu->insertItem(SmallIconSet("half"),
                         i18n("Half size"),this,SLOT(click_half()) );
        menu->insertItem(SmallIconSet("double"),
                         i18n("Double size"),this,SLOT(click_double()) );
        menu->insertItem(SmallIconSet("ak_maximize"),
                         i18n("Maximized"),this,SLOT(maximize()) );
        menuItemFullScreen = menu->insertItem(SmallIconSet("fullscreen"),
                         i18n("Full screen"),this,SLOT(goFullScreen()) );

#ifdef XF86_VM
        videoMode=new aktionVm(mainwidget);
        inVmMode=false;
#endif
        videoContainer=new QWidget(mainwidget);
        videoContainer->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
        video = new KXAnim(videoContainer, "video");

        video->move(0,0);
        video->hide();
        video->setAutoResize(false);

        /* set the logo */
        logoWidget = new QLabel( mainwidget, "logo");
        logoWidget->setAutoResize(true);
        logoWidget->setPixmap( QPixmap(locate("data","aktion/toolbar/aktion_logo.png")) );

	line = new KSeparator( KSeparator::HLine, mainwidget);

	volumeSlider = new QSlider(mainwidget);
	volumeSlider->hide();
	volumeSlider->setOrientation(QSlider::Horizontal);
	volumeSlider->setRange(0,100);
	volumeSlider->setSteps(1,20);
	volumeSlider->setTickmarks( QSlider::Right );
	volumeSlider->setTickInterval(10);
	volumeSlider->setTracking(true);
        volumeSlider->setMaximumWidth(120);

	QToolTip::add(volumeSlider,i18n("Volume"));
	config->setGroup("audio");
        volumeSlider->setValue(config->readNumEntry("audioInitialVolume"));

        if (config->readBoolEntry("showVolumeSlider")==true) volumeSlider->show();


	abrir = new QToolButton(mainwidget, "abrir");
	abrir->setIconSet(MainBarIconSet("fileopen"));
	QToolTip::add(abrir,i18n("Open file"));

	tocar = new QToolButton(mainwidget, "tocar");
	tocar->setIconSet(MainBarIconSet("1rightarrow"));
	QToolTip::add(tocar,i18n("Play"));

	parar = new QToolButton(mainwidget, "parar");
	parar->setIconSet(MainBarIconSet("player_stop"));
	QToolTip::add(parar,i18n("Stop"));

	avanzar = new QToolButton(mainwidget, "avanzar");
	avanzar->setIconSet(MainBarIconSet("2rightarrow"));
        avanzar->setAutoRepeat( true );
	QToolTip::add(avanzar,i18n("Forward"));

	retroceder = new QToolButton(mainwidget, "retroceder");
	retroceder->setIconSet(MainBarIconSet("2leftarrow"));
        retroceder->setAutoRepeat( true );
	QToolTip::add(retroceder,i18n("Backward"));

	ayuda = new QToolButton(mainwidget, "ayuda");
	ayuda->setIconSet(MainBarIconSet("help"));
	QToolTip::add(ayuda,i18n("Help"));

	configB = new QToolButton(mainwidget, "Setup");
	configB->setIconSet(MainBarIconSet("configure"));
	QToolTip::add(configB,i18n("Setup"));

        connect(abrir,SIGNAL(clicked()),
		 this,SLOT(click_open()) );
        connect(ayuda,SIGNAL(clicked()),
		 this,SLOT(click_ayuda()) );
        connect(configB,SIGNAL(clicked()),
	        this,SLOT(click_config()) );
        connect(tocar,SIGNAL(clicked()),
                this,SLOT(click_play()) );
        connect(parar,SIGNAL(clicked()),
		 this,SLOT(click_stop()));
        connect(avanzar,SIGNAL(clicked()),
		 this,SLOT(click_forward()));
        connect(retroceder,SIGNAL(clicked()),
		 this,SLOT(click_backward()));
        connect(video,SIGNAL(stopped()),
                 this,SLOT(waitForKXanimExit()) );
        connect(video,SIGNAL(mouseClick(QMouseEvent *)),
                 this,SLOT(receiveMouseClick(QMouseEvent *)) );
        connect(volumeSlider,SIGNAL(valueChanged(int)),
                 this,SLOT(volumeSliderMoved(int)) );
        connect(timer,SIGNAL(timeout()),
                 this,SLOT(checkStates()) );

        config->setGroup("others");
        lastDir=config->readEntry("initialDirectory");
        if (!QDir(lastDir).exists())
        {
           KMessageBox::information(0L,
                                    i18n("Invalid initial directory"),
                                    i18n("aktion error!"));
	   lastDir="";
        }

        /* switch the buttons to the 'disabled' mode */
        toggleButtons(false);

        whatToDo=NOTHING;
        parametersChanged=false;
        isNewVideo=false;
        fileName="";
        externalChange=false;
        inFullScreen=false;


	QVBoxLayout *lv=new QVBoxLayout( mainwidget );

        lv->addWidget(videoContainer);
        lv->addWidget( logoWidget );

	lv->addWidget( line );
        QHBoxLayout *lh2=new QHBoxLayout( lv );
        lh2->addStretch(1);
	lh2->addWidget( volumeSlider );
	QHBoxLayout *lh=new QHBoxLayout( lv );
	lh->addWidget( abrir );
	lh->addWidget( tocar );
	lh->addWidget( parar );
	lh->addWidget( retroceder );
	lh->addWidget( avanzar );
        lh->addStretch(1);
	lh->addWidget( configB );
	lh->addWidget( ayuda );

}

Principal::~Principal()
{
}

void Principal::toggleButtons(bool state)
{
   tocar->setEnabled(state);
   parar->setEnabled(state);
   avanzar->setEnabled(state);
   retroceder->setEnabled(state);
}

void Principal::click_original() { changeSize(1.0); };
void Principal::click_half()     { changeSize(0.5); };
void Principal::click_double()   { changeSize(2.0); };

void Principal::changeSize(float zoom)
{
   /* resize the window according to a zoom factor */
   float zH, zW;

   if (inFullScreen) {
      undoFullScreen();
      KWin::clearState( winId(), NET::MaxHoriz | NET::MaxVert );
   }

   zH=(float)video->getVideoHeight()*zoom;
   zW=(float)video->getVideoWidth()*zoom;

   resize(zW,zH+volumeSlider->height()+tocar->height()+line->height()+15);
}

void Principal::maximize()
{
   /* maximizes the window */

    // KWM is dead, I hope this works as a replacement
    KWin::Info info = KWin::info( winId() ); // doesn't this look funny? :)
    KWin::setState( winId(), info.state | NET::MaxVert | NET::MaxHoriz );

#if 0
   QRect with_frame, no_frame, result;

   with_frame = KWM::geometry(winId(),true);
   no_frame   = KWM::geometry(winId(),false);
   result     = KWM::windowRegion( KWM::currentDesktop() );

   result.setWidth( result.width() - (with_frame.width() - no_frame.width()) );
   result.setHeight( result.height() - (with_frame.height() - no_frame.height()) );
   setGeometry( result );
#endif
}

void Principal::changeInitialSize()
{
   /* this procedure sets the window size just after a video
      has been opened */
   config->setGroup("scaling");
   switch (config->readNumEntry("scale"))
   {
      case 0: changeSize(1.0); break;
      case 1: changeSize(0.5); break;
      case 2: changeSize(2.0); break;
      case 3: maximize(); break;
      case 4: goFullScreen(); break;
   }
}

void Principal::waitForKXanimExit()
{
    /* the kxanim's stopped signal activates this slot every time the video
       is stopped. I use the whatToDo switch to know "what to do now that
       kxanim is stopped". This is the most secure way to manage the things,
       but not the cleaner, though.
    */
    tocar->setIconSet(MainBarIconSet("1rightarrow"));

    switch (whatToDo)
    {
        case NOTHING          : if (inFullScreen)
                                {
                                   undoFullScreen();
                                };
                                break;
        case CLOSE_THE_APP    : kapp->quit(); break;
        case NEW_VIDEO        : click_open(); break;
        case CONTINUE_LOADING : continueLoading(); break;
        case CHANGE_EXECUTABLE: executableChanged(); break;
    }
}

bool Principal::queryExit()
{
        whatToDo = CLOSE_THE_APP;
        if (video->isActive())
        {
            video->stop();
            return false; // wait for signal
        }
        else
           return true;
}

void Principal::click_open()
{
    /* the idea behind this method is not very simple:
       if fileName is empty then: get a file name!
       if the video is active (here comes the hard part!):
           set 'what to do' to NEW_VIDEO, stop the video and wait that the
           waitForKXanimExit slot gets activated...
       else
           ok, now that the video is REALLY inactive, we can play with it...!
           set the video file and wait until KXAnim finishes reading info...
           (continues on continueLoading() )
    */
    KURL url;

    if ( fileName.isEmpty() )
    {
       url=KFileDialog::getOpenURL(lastDir, i18n("*.avi *.AVI *.mov *.MOV *.mpg *.MPG *.mpeg *.MPEG *.flc *.FLC *.fli *.FLI|All video formats\n"
        					     "*.avi *.AVI|AVI files (*.avi)\n"
        					     "*.mov *.MOV|QuickTime files (*.mov)\n"
        					     "*.mpg *.MPG *.mpeg *.MPEG|MPG Files (*.mpg)\n"
        					     "*.fli *.FLI *.flc *.FLC|FLI/FLC Files (*.fli *.flc)\n"
        					     "*|All Files (*)\n")
        				      ,0L,i18n("Select a movie"));
      if (url.isEmpty()) return;
    }
    else
    {
        url.setPath(fileName);
    }

    if( url.isLocalFile() )
    {
        fileName = url.path();
    }
    else
    {
        KMessageBox::sorry( 0L, i18n("Only local files are currently supported." ) );
        fileName = "";
    }

    if (video->isActive() && !fileName.isEmpty() )
    {
        whatToDo=NEW_VIDEO;
        video->stop();
    }
    else
        if (!fileName.isEmpty())
        {
	  /* remove the logo picture */
	  if (logoWidget != 0L)
	  {
	    delete logoWidget;
	    logoWidget = 0L;
	    video->show();
	  }

	  whatToDo=CONTINUE_LOADING;
	  kapp->setOverrideCursor( waitCursor );
	  setParameters();
	  parametersChanged=false;
	  video->setFile(fileName);
	}
}

void Principal::continueLoading()
{
   QString title;
   int pos;

   whatToDo=NOTHING;
   if (video->getErrorCode()==0)
   {
       /* Everything seems to be fine... */
       /* enable the buttons */
       toggleButtons(true);
       pos=fileName.findRev('/');
       title=fileName.mid(pos+1,fileName.length()-(pos+1));
       // title+=" - aKtion!";
       // preserve the caption for future use (in fullscreen)
       saveCaption = title;
       setCaption( title );
       lastDir=fileName.mid(0,pos+1);
       isNewVideo=true;

       theCapturer.resetCounter();
       config->setGroup("capture");
       theCapturer.setParameters(config->readEntry("outputDir"),fileName,
                                 config->readEntry("outputFormat"));

       kapp->restoreOverrideCursor();
       changeInitialSize();
       click_play();
   }
   else
   {
       /* something is wrong... */
       /* disable buttons */
       toggleButtons(false);
       kapp->restoreOverrideCursor();
       KMessageBox::sorry(0L,video->getErrorString());
   }
   fileName="";
}

void Principal::dragEnterEvent( QDragEnterEvent *event )
{
  event->accept( QUriDrag::canDecode(event) );
}

void Principal::dropEvent( QDropEvent *event )
{
    if ( QUriDrag::canDecode(event) )
    {
        QStrList  urls;
        if ( QUriDrag::decode(event, urls) )
        {
            char *urlStr=urls.first();
            KURL url(urlStr);
            fileName = url.path();
            click_open() ;
        }
    }
}

void Principal::click_ayuda()
{
    kapp->invokeHelp();
}

void Principal::click_play()
{
    /* maube do we need to resize the whole application? */
    if (parametersChanged==true && video->isActive()==false)
    {
       setParameters();
       parametersChanged=false;
    }
    /* autoplay or not */
    config->setGroup("others");
    if (isNewVideo==true && config->readBoolEntry("autoplay")==false)
    {
       video->setPauseAt(0);
    }
    else video->setPauseAt(-1);
    isNewVideo=false;

    /* set the play button pixmap */
    if (video->isPlaying()==true)
       tocar->setIconSet(MainBarIconSet("1rightarrow"));
    else
       tocar->setIconSet(MainBarIconSet("player_pause"));

    // timer->start(100);
    //updateGeometry();
    video->play();
}

void Principal::click_forward()
{
   if (video->isPlaying()==true)
      tocar->setIconSet(MainBarIconSet("1rightarrow"));
   video->stepForward();
}

void Principal::click_backward()
{
   if (video->isPlaying()==true)
      tocar->setIconSet(MainBarIconSet("1rightarrow"));
   video->stepBack();
}

void Principal::click_stop()
{
   video->stop();
}

void Principal::click_config()
{
   QString oldExecutable, newExecutable;

   config->setGroup("others");
   oldExecutable=config->readEntry("executable");

   Setup dialogo(0L, "setup");
   if (dialogo.exec()==1)
   {
      parametersChanged=true;

      config->setGroup("capture");
      theCapturer.setParameters(config->readEntry("outputDir"),fileName,
                                config->readEntry("outputFormat"));

      config->setGroup("others");
      newExecutable=config->readEntry("executable");

      if (oldExecutable!=newExecutable)
         executableChanged();
   }
}

void Principal::setParameters()
{
    config->setGroup("audio");
    video->setAudio(config->readBoolEntry("enable"));
    video->setAudioSync(config->readBoolEntry("audioSync"));
    video->setInitialVolume(volumeSlider->value());

    config->setGroup("color");
    video->setColorAhead(config->readNumEntry("colorAhead"));
    switch (config->readNumEntry("colorMapping"))
    {
       case 0: video->setColorMapping(KXAnim::static332); break;
       case 1: video->setColorMapping(KXAnim::lookupTable); break;
       case 2: video->setColorMapping(KXAnim::grayScale); break;
       case 3: video->setColorMapping(KXAnim::none); break;
    }
    video->setGammaDisplay(float(config->readDoubleNumEntry("gammaDisp")));

    config->setGroup("scaling");
    /* I always wants "resinzing on the fly"

    video->setResizing(true);
    is not necessary 'cause it's true by default.
    */

//    video->setScaleFactor( float(config->readNumEntry("dispFactor"))/100 );
//    video->setScaleFactorB( float(config->readNumEntry("bufFactor"))/100);

    config->setGroup("others");
    video->setLoading(config->readNumEntry("loading"));
    video->setX11Shared(config->readBoolEntry("x11shared"));
    video->setMultiBuffer(config->readBoolEntry("multiBuffer"));
    video->setUsePixmap(config->readBoolEntry("pixmap"));
/*
    switch (config->readNumEntry("visualClass"))
    {
       case 0: video->setX11VisualClass("default"); break;
       case 1: video->setX11VisualClass("staticgray"); break;
       case 2: video->setX11VisualClass("grayscale"); break;
       case 3: video->setX11VisualClass("staticcolor"); break;
       case 4: video->setX11VisualClass("pseudocolor"); break;
       case 5: video->setX11VisualClass("truecolor"); break;
       case 6: video->setX11VisualClass("directcolor"); break;
    }
*/
    video->setLoop(config->readBoolEntry("loop"));
    video->setExtraParameters( config->readEntry("extras").ascii());
    video->setExecutable( QFile::encodeName(config->readEntry("executable")) );
}

void Principal::keyPressEvent( QKeyEvent *key)
{
   switch(key->key())
   {
      case Key_Space : click_play(); break;
      case Key_Period: click_forward(); break;
      case Key_Comma : click_backward(); break;
      case Key_Enter : click_open(); break;
      case Key_3     : if(volumeSlider->value() <= 99)
                       {
                          externalChange=true;
                          volumeSlider->setValue( volumeSlider->value() + 1);
                          externalChange=false;
                          video->volumeIncrement();
                       }
                       break;
      case Key_2     : if(volumeSlider->value() >= 1)
                       {
                          externalChange=true;
                          volumeSlider->setValue( volumeSlider->value() - 1);
                          externalChange=false;
                          video->volumeDecrement();
                       }
                       break;
      case Key_F1    : click_ayuda(); break;
      case Key_Escape: this->close(); break;
      case Key_C     : theCapturer.captureWidget(video); break;
      default        : if (inFullScreen)
                       {
                          undoFullScreen();
                          /* restore the geometry */
                          // KWM::setGeometry( winId(), oldGeometry);
			  KWin::clearState( winId(), NET::MaxHoriz | NET::MaxVert );
                       }
                       break;
   }
   key->accept();
}

void Principal::receiveMouseClick( QMouseEvent *mouse)
{
   if (mouse->button()==RightButton)
   {
      menu->move(this->x()+mouse->x(),this->y()+mouse->y());
      menu->show();
   }
   else
   {
     if (mouse->button()==LeftButton && inFullScreen)
     {
        undoFullScreen();
        // KWM::setGeometry( winId(), oldGeometry);
	KWin::clearState( winId(), NET::MaxHoriz | NET::MaxVert );
     }
   }
}

void Principal::loadFile( const char *name)
{
   fileName=name;
   click_open();
}

void Principal::click_info()
{
   QString codec(video->getVideoCodec());

   QString s;
   s = i18n("File: %1\n"
             "%2\n"
             "%3x%4\n"
             "%5 frames - %6 fps")
             .arg(caption())
             .arg(codec)
             .arg(video->getVideoWidth())
             .arg(video->getVideoHeight())
             .arg(video->getVideoFrames())
             .arg(KGlobal::locale()->formatNumber(video->getVideoSpeed(), 2));
   KMessageBox::information(this, s, "aktion!");
}

void Principal::volumeSliderMoved(int v)
{
   if (!externalChange)
       video->setVolume(v);
}

void Principal::resizeEvent( QResizeEvent *)
{
   int w=videoContainer->width();
   int h=videoContainer->height();
   float video_ar, space_ar, scaled;
   int videoW, videoH;

   if (inFullScreen==false)
   {
        config->setGroup("audio");
        if (config->readBoolEntry("showVolumeSlider")==true)
        {
	   volumeSlider->show();
	}
	else
	{
	   volumeSlider->hide();
	}
   }

   /* Here comes the resizing-moving process for the video widget.
      It'll maintain his aspect ratio. Steps are:
      .The available space for the video is defined w and h.
      .Resize the video widget as much as possible:
        .Calculate video and left space aspect ratio.
        .If video aspect ratio is smaller:
           expand in height
         else
           .If video aspect ratio is bigger
              expand in width
            else
              expand all
   */

   videoW=video->getVideoWidth();
   if (videoW==0) videoW=1;
   videoH=video->getVideoHeight();
   if (videoH==0) videoH=1;

   video_ar=(float)videoW/(float)videoH; // video aspect ratio
   space_ar=(float)w/(float)h;           // available space aspect ratio

   if (video_ar<space_ar)
   {
      scaled=(float)videoW*( (float)h/(float)videoH );
      video->resize( (int)scaled,h);
      scaled=((float)w - scaled) /2;
      video->move( (int)scaled, 0);

   }
   else
   {
      if (video_ar>space_ar)
      {
         scaled=(float)videoH*( (float)w/(float)videoW );
         video->resize(w, (int)scaled);
         scaled=((float)h - scaled) /2;
         video->move(0, (int)scaled);
      }
      else
      {
         video->resize(w,h);
         video->move(0,0);
      }
   }

}


void Principal::checkStates()
{
//   cout<<video->iconText()<<endl;
}

void Principal::goFullScreen()
{
   int sizeW=0, sizeH=0;

   /* change the popup menu */
   menu->setItemEnabled(menuItemFullScreen,false);

   /* Hide all widgets (except the video widget) */
   parar->hide();
   tocar->hide();
   abrir->hide();
   avanzar->hide();
   retroceder->hide();
   ayuda->hide();
   configB->hide();
   line->hide();
   volumeSlider->hide();

#ifdef XF86_VM
   bool vmExtensionsOk=false;
   inVmMode=false;

   config->setGroup("scaling");
   if (config->readBoolEntry("usevm"))
   {
      if (true)//geteuid()==0)
      {
         sizeW=video->getVideoWidth();
         sizeH=video->getVideoHeight();
         vmExtensionsOk=videoMode->setVideoMode(&sizeW,&sizeH);
         if (vmExtensionsOk)
            inVmMode=true;
      }
      else
          KMessageBox::sorry(0L, i18n("You must be root to use the XFree86-VidMode extensions\n"
                                      "Switching to normal full-screen..."));
   }

   if (!vmExtensionsOk)
   {
#endif
      /* Make the window FULL SCREEN in the traditional way */
      sizeW = QApplication::desktop()->width();
      sizeH = QApplication::desktop()->height();
#ifdef XF86_VM
   }
#endif

   inFullScreen = true;

   config->setGroup("scaling");
   if (config->readBoolEntry("grabmouse"))
   {
      kapp->setOverrideCursor( blankCursor );
      QCursor::setPos(0,0);
   }

   /* Save the window's background color and geometry */
   saveBackground = backgroundColor();
   videoContainer->setBackgroundColor( black );

   showFullScreen();
   this->setCaption(saveCaption);

   setGeometry( 0, 0, sizeW, sizeH );
}

void Principal::undoFullScreen()
{
#ifdef XF86_VM
   if (inVmMode)
   {
      videoMode->resetVideoMode();
      inVmMode=false;
   }
#endif

   /* change the popup menu */
   menu->setItemEnabled(menuItemFullScreen,true);

   /* Show all widgets  */
   parar->show();
   tocar->show();
   abrir->show();
   avanzar->show();
   retroceder->show();
   ayuda->show();
   configB->show();
   line->show();

   /* Restore background color */
   videoContainer->setBackgroundColor( saveBackground );

   inFullScreen = false;

   config->setGroup("scaling");
   if (config->readBoolEntry("grabmouse"))
      kapp->restoreOverrideCursor();

//   KWM::setDecoration( winId(), KWM::normalDecoration );

//   QPoint p;
//   p.setX(50);
//   p.setY(50);
//   reparent(0,0,p,true);//WStyle_Customize | WStyle_NormalBorder | WType_TopLevel, p, false);
//   KWin::setType(winId(), NET::Normal);
//   KWin::clearState(winId(), NET::StaysOnTop);

   showNormal();
   this->setCaption(saveCaption);
}

void Principal::executableChanged()
{
   if (video->isActive())
   {
      whatToDo=CHANGE_EXECUTABLE;
      video->stop();
   }
   else
   {
      whatToDo=NOTHING;
      fileName=video->getVideoFileName();
      if (!fileName.isEmpty())
         click_open();
   }
}
#include "principal.moc"
