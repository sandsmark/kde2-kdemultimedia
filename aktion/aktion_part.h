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
#ifndef __aktion_part_h__
#define __aktion_part_h__

#include <kparts/browserextension.h>
#include <klibloader.h>

class KInstance;
class KXAnim;
class KAction;
class AktionBrowserExtension;
class QLabel;
class KAboutData;

class AktionFactory : public KLibFactory
{
    Q_OBJECT
public:
    AktionFactory();
    virtual ~AktionFactory();

    virtual QObject* create(QObject* parent = 0, const char* name = 0,
                            const char* classname = "QObject",
                            const QStringList &args = QStringList());

    static KInstance *instance();
    static KAboutData *aboutData();

private:
    static KInstance *s_instance;
};

class AktionPart: public KParts::ReadOnlyPart
{
    Q_OBJECT
public:
    AktionPart(QWidget *parent = 0, const char *name = 0);
    virtual ~AktionPart();

    KXAnim *kxanim() const { return widget; }

    virtual bool closeURL();

protected:
    virtual bool openFile();

protected slots:
    void slotPlay();
    void slotPause();
    void slotStop();
    void slotForward();
    void slotBackward();

private:
    KXAnim *widget;
    QLabel *label;
    AktionBrowserExtension *m_extension;

    KAction *m_playAction;
    KAction *m_pauseAction;
    KAction *m_stopAction;
    KAction *m_forwardAction;
    KAction *m_backwardAction;
};

class AktionBrowserExtension : public KParts::BrowserExtension
{
    Q_OBJECT
    friend class AktionPart;
public:
    AktionBrowserExtension(AktionPart *parent);
    virtual ~AktionBrowserExtension();
};
#endif
