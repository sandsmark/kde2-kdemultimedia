/*  This file is part of the KDE project
    Copyright (C) 1999,2000 Kurt Granroth <granroth@kde.org>

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

*/

#include "aktion_part.h"

#include <klocale.h>
#include <kiconloader.h>
#include <kinstance.h>
#include <kaction.h>
#include <kaboutdata.h>

#include <kparts/partmanager.h>

#include "kxanim.h"
#include <qtimer.h>
#include <qlabel.h>
#include <qiconset.h>

static const char *description = I18N_NOOP("KDE Video Player");
static const char *version     = "1.99";

extern "C"
{
    /**
     * This function is the 'main' function of this part.  It takes
     * the form 'void *init_lib<library name>()'.  It always returns a
     * new factory object
     */
    void *init_libaktion()
    {
        return new AktionFactory;
    }
};

/**
 * We need one static instance of the factory for our C 'main'
 * function
 */
KInstance *AktionFactory::s_instance = 0L;

AktionFactory::AktionFactory()
{
    KGlobal::locale()->insertCatalogue( QString::fromLatin1("aktion") );
}

AktionFactory::~AktionFactory()
{
    if (s_instance)
    {
        delete s_instance->aboutData();
        delete s_instance;
    }

    s_instance = 0;
}

QObject *AktionFactory::create(QObject *parent, const char *name, const char*,
                               const QStringList& )
{
    QObject *obj = new AktionPart((QWidget*)parent, name);
    emit objectCreated(obj);
    return obj;
}

KInstance *AktionFactory::instance()
{
    if ( !s_instance )
        s_instance = new KInstance( aboutData() );
    return s_instance;
}

KAboutData *AktionFactory::aboutData()
{
  KAboutData *about = new KAboutData("aktion", I18N_NOOP("aKtion"), version,
				     description, KAboutData::License_GPL,
				     "(c) 2000 Guillermo P. Marotte");
  about->addAuthor("Guillermo P. Marotte",0, "g-marotte@usa.net");
  return about;
} 

AktionPart::AktionPart(QWidget *parent, const char *name)
    : KParts::ReadOnlyPart(parent, name)
{
    setInstance(AktionFactory::instance());

    // create a canvas to insert our widget
    QWidget *canvas = new QWidget(parent);
    canvas->setFocusPolicy(QWidget::ClickFocus);
    canvas->setSizePolicy(QSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding));
    setWidget(canvas);

    // create our animation widget
    widget = new KXAnim(canvas);
    widget->setLoop(true);
    widget->show();

    // create and connect our actions
    m_playAction = new KAction(i18n("Play"), "1rightarrow",
                               0, this,
                               SLOT(slotPlay()), actionCollection(),
                               "aktion_play");

    m_pauseAction = new KAction(i18n("Pause"), "player_pause",
                               0, this,
                               SLOT(slotPause()), actionCollection(),
                               "aktion_pause");

    m_stopAction = new KAction(i18n("Stop"), "player_stop",
                               0, this,
                               SLOT(slotStop()), actionCollection(),
                               "aktion_stop");

    m_backwardAction = new KAction(i18n("Backward"),
                                   "2leftarrow",
                                   0, this,
                                   SLOT(slotBackward()), actionCollection(),
                                   "aktion_backward");

    m_forwardAction = new KAction(i18n("Forward"), "2rightarrow",
                                  0, this,
                                  SLOT(slotForward()), actionCollection(),
                                  "aktion_forward");

    m_extension = new AktionBrowserExtension(this);
    setXMLFile("aktion_part.rc");
}

AktionPart::~AktionPart()
{
    slotStop();
}

bool AktionPart::openFile()
{
    widget->setFile(m_file);
    widget->stop();
    widget->show();
    QTimer::singleShot(2000, this, SLOT(slotPlay()));

    return true;
}

bool AktionPart::closeURL()
{
    slotStop();
    return ReadOnlyPart::closeURL();
}

void AktionPart::slotPlay()
{
    widget->play();
    m_playAction->setEnabled(false);
    m_pauseAction->setEnabled(true);
    m_stopAction->setEnabled(true);
    m_forwardAction->setEnabled(true);
    m_backwardAction->setEnabled(true);
}

void AktionPart::slotPause()
{
    widget->pause();
    m_playAction->setEnabled(true);
    m_pauseAction->setEnabled(false);
    m_stopAction->setEnabled(false);
    m_forwardAction->setEnabled(false);
    m_backwardAction->setEnabled(false);
}

void AktionPart::slotStop()
{
    widget->stop();
    m_playAction->setEnabled(true);
    m_pauseAction->setEnabled(false);
    m_stopAction->setEnabled(false);
    m_forwardAction->setEnabled(false);
    m_backwardAction->setEnabled(false);
}

void AktionPart::slotForward()
{
    widget->stepForward();
}

void AktionPart::slotBackward()
{
    widget->stepBack();
}

AktionBrowserExtension::AktionBrowserExtension(AktionPart *parent)
    : KParts::BrowserExtension(parent, "AktionBrowserExtension")
{
}

AktionBrowserExtension::~AktionBrowserExtension()
{
}
#include "aktion_part.moc"
