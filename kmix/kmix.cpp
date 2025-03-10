/*
 * KMix -- KDE's full featured mini mixer
 *
 * $Id: kmix.cpp 119402 2001-10-25 22:36:36Z waba $
 *
 * Copyright (C) 2000 Stefan Schimanski <schimmi@kde.org>
 * Copyright (C) 2001 Preston Brown <pbrown@kde.org>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this program; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <iostream.h>

// include files for QT
#include <qdir.h>
#include <qapplication.h>
#include <qpopupmenu.h>
#include <qtabbar.h>
#include <qinputdialog.h>
#include <qtimer.h>
#include <qmap.h>

// include files for KDE
#include <kiconloader.h>
#include <kmessagebox.h>
#include <kmenubar.h>
#include <klocale.h>
#include <kconfig.h>
#include <kaction.h>
#include <kstdaction.h>
#include <kpanelapplet.h>
#include <kpopupmenu.h>
#include <khelpmenu.h>
#include <kiconloader.h>
#include <kdebug.h>
#include <kaccel.h>

// application specific includes
#include "kmix.h"
#include "kmixerwidget.h"
#include "kmixprefdlg.h"
#include "kmixdockwidget.h"


KMixApp::KMixApp()
    : KUniqueApplication(), m_kmix( 0 )
{
}


KMixApp::~KMixApp()
{
    delete m_kmix;
}


int KMixApp::newInstance()
{
    if ( m_kmix )
        m_kmix->show();
    else {
        m_kmix = new KMixWindow;
        if ( isRestored() && KMainWindow::canBeRestored(0) )
            m_kmix->restore(0, FALSE);
    }

    return 0;
}


KMixWindow::KMixWindow()
   : KMainWindow(0, 0, 0 ), m_maxId( 0 ), m_visibleTabs( 0 ), m_dockWidget( 0L )
{
   initMixer();
   initActions();
   initWidgets();

   loadConfig();

   // create mixer widgets for unused mixers
   for (Mixer *mixer=m_mixers.first(); mixer!=0; mixer=m_mixers.next()) {

       // search for mixer widget with current mixer
       KMixerWidget *widget;
       for ( widget=m_mixerWidgets.first(); widget!=0; widget=m_mixerWidgets.next() )
           if ( widget->mixer()==mixer ) break;

       // create new widget
       if ( widget==0 ) {

           KMixerWidget *mw = new KMixerWidget( m_maxId, mixer,
                                                mixer->mixerName(),
                                                mixer->mixerNum(),
                                                false, KPanelApplet::Up,
                                                this );
           mw->setName( mixer->mixerName() );
           insertMixerWidget( mw );

           m_maxId++;
       }
   }

   initPrefDlg();

   updateDocking();

   if ( m_startVisible )
       show();
   else
       hide();
}


KMixWindow::~KMixWindow()
{
    saveConfig();
}


void KMixWindow::initActions()
{
    // file menu
    (void)new KAction( i18n("&New mixer tab"), "filenew", 0, this,
                       SLOT(newMixer()), actionCollection(), "file_new_tab" );
    (void)new KAction( i18n("&Close mixer tab"), "fileclose", 0, this,
                       SLOT(closeMixer()), actionCollection(), "file_close_tab" );
    (void)new KAction( i18n("&Restore default volumes"), 0, this, SLOT(loadVolumes()),
                       actionCollection(), "file_load_volume" );
    (void)new KAction( i18n("&Save current volumes as default"), 0, this, SLOT(saveVolumes()),
                       actionCollection(), "file_save_volume" );
    KStdAction::quit( this, SLOT(quit()), actionCollection());

    // settings menu
    KAction *a = KStdAction::showMenubar( this, SLOT(toggleMenuBar()), actionCollection());
    a->setAccel( CTRL+Key_M );
    a->plugAccel( new KAccel(this) ); // to make action working without visible menubar
    KStdAction::preferences( this, SLOT(showSettings()), actionCollection());

    createGUI( "kmixui.rc" );
}


void KMixWindow::initMixer()
{
   QTimer *timer = new QTimer( this );
   timer->start( 500 );

   // get maximum values
   KConfig *config= new KConfig("kcmkmixrc", false);
   config->setGroup("Misc");
   int maxCards = config->readNumEntry( "maxCards", 2 );
   int maxDevices = config->readNumEntry( "maxDevices", 2 );
   delete config;

   // poll for mixers
   QMap<QString,int> mixerNums;
   int drvNum = Mixer::getDriverNum();
   for( int drv=0; drv<drvNum && m_mixers.count()==0; drv++ )
       for( int dev=0; dev<maxDevices; dev++ )
           for( int card=0; card<maxCards; card++ ){

               Mixer *mixer = Mixer::getMixer( drv, dev, card );
               int mixerError = mixer->grab();
               if ( mixerError!=0 )
                   delete mixer;
               else {
                   connect( timer, SIGNAL(timeout()), mixer, SLOT(readSetFromHW()));
                   m_mixers.append( mixer );

                   // count mixer nums for every mixer name to identify mixers with equal names
                   mixerNums[mixer->mixerName()]++;
                   mixer->setMixerNum( mixerNums[mixer->mixerName()] );
               }

           }
}


void KMixWindow::initPrefDlg()
{
   m_prefDlg = new KMixPrefDlg;
   connect( m_prefDlg, SIGNAL(signalApplied(KMixPrefDlg *)),
            this, SLOT(applyPrefs(KMixPrefDlg *)) );
}


void KMixWindow::initWidgets()
{
   m_tab = new QTabWidget( this );
   setCentralWidget( m_tab );
}


void KMixWindow::updateDocking()
{
    // delete old dock widget
    if (m_dockWidget) {
        delete m_dockWidget;
        m_dockWidget = 0L;
    }

    if (m_showDockWidget) {

        // create dock widget
        m_dockWidget = new KMixDockWidget( m_mixers.first(), this );
        m_dockWidget->setPixmap( BarIcon("kmixdocked") );

        // create RMB menu
        QPopupMenu *menu = m_dockWidget->contextMenu();

        KAction *a = actionCollection()->action("options_configure");
        if (a) a->plug( menu );

        menu->insertSeparator();

        a = actionCollection()->action("help_about_app");
        if (a) a->plug( menu );

        a = actionCollection()->action("help");
        if (a) a->plug( menu );

        m_dockWidget->show();
    }
}


void KMixWindow::saveConfig()
{
   KConfig *config = kapp->config();
   config->setGroup(0);

   config->writeEntry( "Size", size() );
   config->writeEntry( "Position", pos() );
   config->writeEntry( "Visible", isVisible() );
   config->writeEntry( "Menubar", m_showMenubar );
   config->writeEntry( "AllowDocking", m_showDockWidget );
   config->writeEntry( "TrayVolumeControl", m_volumeWidget );
// commented out for usability. nolden
//   config->writeEntry( "HideOnClose", m_hideOnClose );
   config->writeEntry( "Tickmarks", m_showTicks );
   config->writeEntry( "Labels", m_showLabels );

   // save mixer widgets
   QStringList tabs;
   for (KMixerWidget *mw=m_mixerWidgets.first(); mw!=0; mw=m_mixerWidgets.next())
   {
      QString grp;
      grp.sprintf( "%i", mw->id() );
      tabs << grp;

      config->setGroup( grp );
      config->writeEntry( "Mixer", mw->mixerNum() );
      config->writeEntry( "MixerName", mw->mixerName() );
      config->writeEntry( "Name", mw->name() );

      mw->saveConfig( config, grp );
   }

   config->setGroup(0);
   config->writeEntry( "Tabs", tabs );
}


void KMixWindow::loadConfig()
{
   KConfig *config = kapp->config();
   config->setGroup(0);

   m_showDockWidget = config->readBoolEntry("AllowDocking", true);
   m_volumeWidget = config->readBoolEntry("TrayVolumeControl", true);
  //hide on close has to stay true for usability. KMixPrefDlg option commented out. nolden
   m_hideOnClose = config->readBoolEntry("HideOnClose", true);
   m_showTicks = config->readBoolEntry("Tickmarks", false);
   m_showLabels = config->readBoolEntry("Labels", false);
   m_startVisible = config->readBoolEntry("Visible", true);

   // show/hide menu bar
   m_showMenubar = config->readBoolEntry("Menubar", true);
   if ( m_showMenubar )
      menuBar()->show();
   else
      menuBar()->hide();

   KToggleAction *a = static_cast<KToggleAction*>(actionCollection()->action("options_show_menubar"));
   if (a) a->setChecked( m_showMenubar );

   // load mixer widgets
   QString tabsStr = config->readEntry( "Tabs" );
   QStringList tabs = QStringList::split( ',', tabsStr );
   m_mixerWidgets.clear();
   for ( QStringList::Iterator tab=tabs.begin(); tab!=tabs.end(); ++tab )
   {
       // set config group
       config->setGroup(*tab);

       // get id
       int id = (*tab).toInt();
       if ( id>=m_maxId ) m_maxId = id+1;

       // find mixer
       int mixerNum = config->readNumEntry( "Mixer", -1 );
       QString mixerName = config->readEntry( "MixerName", QString::null );
       QString name = config->readEntry( "Name", mixerName );
       Mixer *mixer = 0;

       if ( mixerNum>=0 ) {

           for ( mixer=m_mixers.first(); mixer!=0; mixer=m_mixers.next() ) {
               if ( mixer->mixerName()==mixerName && mixer->mixerNum()==mixerNum ) break;
           }

       }

       KMixerWidget *mw = new KMixerWidget( id, mixer, mixerName, mixerNum, 
                                            false, KPanelApplet::Up, this );
       mw->setName( name );
       mw->loadConfig( config, *tab );
       insertMixerWidget( mw );
   }

   // restore window size and position
   if ( !kapp->isRestored() ) // done by the session manager otherwise
   {
       QSize defSize( minimumWidth(), height() );
       QSize size = config->readSizeEntry("Size", &defSize );
       if(!size.isEmpty()) resize(size);

       QPoint defPos = pos();
       QPoint pos = config->readPointEntry("Position", &defPos);
       move(pos);
   }
}


void KMixWindow::insertMixerWidget( KMixerWidget *mw )
{
   m_mixerWidgets.append( mw );

   m_visibleTabs++;
   m_tab->addTab( mw, mw->name() );
   mw->setTicks( m_showTicks );
   mw->setLabels( m_showLabels );
   mw->show();
   connect( mw, SIGNAL(updateLayout()), this, SLOT(updateLayout()) );

   KAction *a = actionCollection()->action( "file_close_tab" );
   if ( a ) a->setEnabled( m_visibleTabs>1 );

   updateLayout();
}


void KMixWindow::removeMixerWidget( KMixerWidget *mw )
{
    m_visibleTabs--;
    m_tab->removePage( mw );
    m_mixerWidgets.remove( mw );

    KAction *a = actionCollection()->action( "file_close_tab" );
    if ( a ) a->setEnabled( m_visibleTabs>1 );

    updateLayout();
}


void KMixWindow::updateLayout()
{
    m_tab->setMinimumSize( m_tab->minimumSizeHint() );
}

void KMixWindow::closeEvent ( QCloseEvent * e )
{
    if ( /*m_hideOnClose &&*/ m_showDockWidget )
    {
        // <evil>
        kapp->ref(); // prevent KMainWindow from closing the app
        // </evil>
    } else
        kapp->quit(); // force the application to quit

    KMainWindow::closeEvent( e );
}


void KMixWindow::quit()
{
   kapp->quit();
}


void KMixWindow::showSettings()
{
   if (!m_prefDlg->isVisible())
   {
      m_prefDlg->m_dockingChk->setChecked( m_showDockWidget );
      m_prefDlg->m_volumeChk->setChecked(m_volumeWidget);
//      m_prefDlg->m_hideOnCloseChk->setChecked( m_hideOnClose );
      m_prefDlg->m_showTicks->setChecked( m_showTicks );
      m_prefDlg->m_showLabels->setChecked( m_showLabels );

      m_prefDlg->show();
   }
}


void KMixWindow::showHelp()
{
   actionCollection()->action( "help_contents" )->activate();
}


void KMixWindow::showAbout()
{
   actionCollection()->action( "help_about_app" )->activate();
}


void KMixWindow::closeMixer()
{
   if (m_mixerWidgets.count()<=1) return;
   removeMixerWidget( (KMixerWidget *)m_tab->currentPage() );
}


void KMixWindow::newMixer()
{
   QStringList lst;

   int n=1;
   for (Mixer *mixer=m_mixers.first(); mixer!=0; mixer=m_mixers.next())
   {
      QString s;
      s.sprintf("%i. %s", n, mixer->mixerName().ascii());
      lst << s;
      n++;
   }

   bool ok = FALSE;
   QString res = QInputDialog::getItem( i18n("Mixers"),
                                        i18n( "Available mixers" ), lst,
                                        1, TRUE, &ok, this );
   if ( ok )
   {
       // valid mixer?
       Mixer *mixer = m_mixers.at( lst.findIndex( res ) );
       if (!mixer) {
           KMessageBox::sorry( this, i18n("Invalid mixer entered.") );
           return;
       }

       // ask for description
       QString name = QInputDialog::getText( i18n("Description"), i18n( "Description" ),
                                             mixer->mixerName(), &ok, this );
      if ( ok ) {

          // create mixer widget
          KMixerWidget *mw = new KMixerWidget( m_maxId, mixer, mixer->mixerName(), mixer->mixerNum(),
                                               false, KPanelApplet::Up, this );
          m_maxId++;
          mw->setName( name );
          insertMixerWidget( mw );
      }
   }
}


void KMixWindow::loadVolumes()
{
   KConfig *cfg = new KConfig( "kmixctrlrc", true );
   for (Mixer *mixer=m_mixers.first(); mixer!=0; mixer=m_mixers.next())
      mixer->volumeLoad( cfg );
   delete cfg;
}


void KMixWindow::saveVolumes()
{
   KConfig *cfg = new KConfig( "kmixctrlrc", false );
   for (Mixer *mixer=m_mixers.first(); mixer!=0; mixer=m_mixers.next())
      mixer->volumeSave( cfg );
   delete cfg;
}


void KMixWindow::applyPrefs( KMixPrefDlg *prefDlg )
{
   m_showDockWidget = prefDlg->m_dockingChk->isChecked();
   m_volumeWidget = prefDlg->m_volumeChk->isChecked();
//   m_hideOnClose = prefDlg->m_hideOnCloseChk->isChecked();
   m_showTicks = prefDlg->m_showTicks->isChecked();
   m_showLabels = prefDlg->m_showLabels->isChecked();

   for (KMixerWidget *mw=m_mixerWidgets.first(); mw!=0; mw=m_mixerWidgets.next())
   {
      mw->setTicks( m_showTicks );
      mw->setLabels( m_showLabels );
   }

   updateDocking();

   // avoid invisible and unaccessible main window
   if( !m_showDockWidget && !isVisible() )
      show();

   saveConfig();
}


void KMixWindow::toggleMenuBar()
{
    m_showMenubar = !m_showMenubar;
   if( m_showMenubar )
      menuBar()->show();
   else
      menuBar()->hide();
}


void KMixWindow::toggleVisibility()
{
   if ( isVisible() ) hide(); else show();
}

#include "kmix.moc"
