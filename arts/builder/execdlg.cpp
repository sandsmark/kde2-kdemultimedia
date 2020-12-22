	/*

	Copyright (C) 1998 Stefan Westerfeld
                       stefan@space.twc.de

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

#include <qfile.h>
#include <qlayout.h>
#include <qlabel.h>
#include <qbuttongroup.h>
#include <qradiobutton.h>
#include <qlineedit.h>

#include <kbuttonbox.h>
#include <kfiledialog.h>
#include <kseparator.h>
#include <kapp.h>


#include <unistd.h>
#include "portablekde.h"
#include "execdlg.h"
#include "dirmanager.h"
#include <arts/debug.h>

#ifndef KDE_USE_FINAL
static void min_size(QWidget *w) {
  w->setMinimumSize(w->sizeHint());
} 
#endif

ExecDlg::ExecDlg(QWidget *parent, ExecutableStructure *structure)
		:QDialog(parent,"X")
/*, TRUE)*/
{
	this->structure = structure;

	setCaption(i18n("Arts Module Execution"));

	mainlayout = new QVBoxLayout(this);

// caption label: "Synthesis running..."

	mainlayout->addSpacing(5);
	QLabel *captionlabel = new QLabel(this);
	QFont labelfont(captionlabel->font());
	labelfont.setPointSize(labelfont.pointSize()*3/2);
	captionlabel->setFont(labelfont);
	captionlabel->setText(QString(" ")+i18n("Synthesis running...")+QString(" "));
	captionlabel->setAlignment(AlignCenter);
	min_size(captionlabel);
	mainlayout->addWidget(captionlabel);

	cpuusagelabel = new QLabel(this);
	cpuusagelabel->setText(i18n("cpu usage: unknown"));

    cpuusagetimer = new QTimer( this );
	connect( cpuusagetimer, SIGNAL(timeout()),
		this, SLOT(updateCpuUsage()) );
	connect( cpuusagetimer, SIGNAL(timeout()),
		this, SLOT(guiServerTick()) );
	cpuusagetimer->start( 2000, false );

	min_size(cpuusagelabel);
	mainlayout->addWidget(cpuusagelabel);

//	ruler above the sliderlayout

	mainlayout->addSpacing(5);
        KSeparator* sep = new KSeparator( KSeparator::HLine, this);
	mainlayout->addWidget(sep);
	mainlayout->addSpacing(5);

//	sliders, controlpanels

	sliderlayout = new QVBoxLayout;
	mainlayout->addLayout(sliderlayout);

#if 0 /* PORT */
	this->GUIServer = GUIServer;
	GUIServer->setGlobalParent(this);
	GUIServer->setGlobalLayout(sliderlayout);
#endif

//  hruler below the sliderlayout

	mainlayout->addSpacing(5);
        sep = new KSeparator( KSeparator::HLine, this);
	mainlayout->addWidget(sep);
	mainlayout->addSpacing(5);

// buttons

	QHBoxLayout *buttonlayout = new QHBoxLayout;
	mainlayout->addSpacing(5);
	mainlayout->addLayout(buttonlayout);
	mainlayout->addSpacing(5);

	buttonlayout->addSpacing(5);
	KButtonBox *bbox = new KButtonBox(this);

	bbox->addButton(i18n("Help"));

	bbox->addStretch(1);

	QButton *savebutton = bbox->addButton(i18n("&Save..."));
	connect( savebutton, SIGNAL( clicked() ), SLOT(saveSession() ) );

	QButton *okbutton = bbox->addButton(i18n("&Quit"));
	connect( okbutton, SIGNAL( clicked() ), SLOT(accept() ) );

	bbox->layout();
	//min_size(bbox);

	buttonlayout->addWidget(bbox);
	buttonlayout->addSpacing(5);

//	mainlayout->freeze();
};

void ExecDlg::start()
{
	mainlayout->freeze();
}

void ExecDlg::guiServerTick()
{
#if 0 /* TODO:PORT */
	GUIServer->tick();
#endif
}

void ExecDlg::updateCpuUsage()
{
#if 0 /* TODO:PORT */
	char cpuusage[100];

	ArtsCorba::Status s = Synthesizer->getStatus();
	if(s.halted)
	{
		cpuusagetimer->stop();
		accept();
		PortableKDE::KMsgSorry(this,i18n("Your synthesis has been interrupted due to excessive cpu load."));
		/*
		KMsgBox::message(this,i18n("Error"),
		  i18n("Your synthesis has been interrupted due to too high cpu load."),
		  KMsgBox::STOP);
		*/
		// warning: this is invalid after accept();
		return;
	}
	sprintf(cpuusage,"%s%3.2f%%",
		(const char *)i18n("cpu usage: "),s.cpu_usage*100);

	cpuusagelabel->setText(cpuusage);

	if(!structure->isExecuting()) accept();
	// warning: this is invalid after accept();
#endif
}

void ExecDlg::done( int r )
{
	structure->stopExecute();
	QDialog::done(r);
	emit ready();
}

void ExecDlg::saveSession()
{
	chdir(DirManager::sessionDir());

	QString filename = KFileDialog::getSaveFileName(0,"*.arts-session",this);
	if(!filename.isEmpty())
	{
		arts_debug("save... %s",filename.local8Bit().data());
		structure->saveSession(QFile::encodeName(filename));
	}
}
#include "execdlg.moc"
