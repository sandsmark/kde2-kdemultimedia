#include "kmid_part.h"

#include <kinstance.h>
#include <kiconloader.h>
#include <klocale.h>
#include <kaboutdata.h>
#include <kaction.h>
#include <qiconset.h>

#include <kparts/partmanager.h>

#include "kmidclient.h"
#include <qtimer.h>

extern "C"
{
  /**
   * This function is the 'main' function of this part.  It takes
   * the form 'void *init_lib<library name>()'.  It always returns a
   * new factory object
   */
  void *init_libkmidpart()
  {
    return new KMidFactory;
  }
};

/**
 * We need one static instance of the factory for our C 'main'
 * function
 */
KInstance *KMidFactory::s_instance = 0L;

KMidFactory::KMidFactory()
{
  s_instance=0L;
}

KMidFactory::~KMidFactory()
{
  if (s_instance)
  {
    delete s_instance->aboutData();
    delete s_instance;
  }

  s_instance = 0;
}

QObject *KMidFactory::create(QObject *parent, const char *name, const char*,
    const QStringList& )
{
  QObject *obj = new KMidPart((QWidget*)parent, name);
  emit objectCreated(obj);
  return obj;
}

KAboutData *KMidFactory::aboutData()
{
    KAboutData *aboutdata = new KAboutData("kmid", "KMid", "2.0",
        I18N_NOOP("MIDI/Karaoke file player"), KAboutData::License_GPL,
        I18N_NOOP("(C) 1997,98,99,2000, Antonio Larrosa Jimenez"),"",
        "http://perso.wanadoo.es/antlarr/kmid.html");
    aboutdata->addAuthor("Antonio Larrosa Jimenez",
        I18N_NOOP("Original Developer/Mantainer"),"larrosa@kde.org",
        "http://perso.wanadoo.es/antlarr/index.html");
    return aboutdata;
}

KInstance *KMidFactory::instance()
{
  if ( !s_instance )
    s_instance = new KInstance( aboutData() );

  return s_instance;
}

  KMidPart::KMidPart(QWidget *parent, const char *name)
: KParts::ReadOnlyPart(parent, name)
{
  setInstance(KMidFactory::instance());

  widget = new kmidClient(parent, actionCollection());
  widget->show();
  widget->setFocusPolicy(QWidget::ClickFocus);
  setWidget(widget);

  // create and connect our actions
   (void)new KAction(i18n("Play"), "1rightarrow", 0, this,
      SLOT(slotPlay()), actionCollection(),
      "play");

   (void)new KAction(i18n("Stop"), "player_stop", 0, this,
      SLOT(slotStop()), actionCollection(),
      "stop");

   (void)new KAction(i18n("Backward"),
      "2leftarrow", 0, this,
      SLOT(slotBackward()), actionCollection(),
      "backward");

   (void)new KAction(i18n("Forward"),
        "2rightarrow", 0, this,
      SLOT(slotForward()), actionCollection(),
      "forward");

  m_extension = new KMidBrowserExtension(this);

  setXMLFile("kmid_partui.rc");


}

KMidPart::~KMidPart()
{
}

bool KMidPart::openFile()
{
  widget->openURL(m_file);
  widget->stop();
  widget->show();
  QTimer::singleShot(2000, this, SLOT(slotPlay()));

  return true;
}

bool KMidPart::closeURL()
{
  slotStop();
  return true;
}

void KMidPart::slotPlay()
{
  widget->stop();
  widget->play();
}


void KMidPart::slotStop()
{
  widget->stop();
}
KMidBrowserExtension::KMidBrowserExtension(KMidPart *parent)
    : KParts::BrowserExtension(parent, "KMidBrowserExtension")
{
}

KMidBrowserExtension::~KMidBrowserExtension()
{
}
#include "kmid_part.moc"
