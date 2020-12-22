/*

	Copyright (C) 1998 - 2000 Stefan Westerfeld
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

#include "main.h"
#include "structureport.h"
#include "menumaker.h"
#include "session.h"
#include "dirmanager.h"
#include "moduleinfo.h"
#include "qiomanager.h"
#include "artsversion.h"

#include <list>
#include <iostream>

#include "portablekde.h"
#include "config.h"
#include <kapp.h>
#include <kcmdlineargs.h>
#include <klocale.h>
#include <kmenubar.h>
#include <kstdaccel.h>
#include <kfiledialog.h>
#include <ksimpleconfig.h>
#include <kmessagebox.h>
#include <kaboutdata.h>
#include <kstddirs.h>
#include <qlistbox.h>
#include <qscrollview.h>
#include <qtablevw.h>
#include <qpainter.h>
#include <qdrawutl.h>
#include <qwidget.h>
#include <qlayout.h>
#include <qframe.h>
#include <qpixmap.h>
#include <string.h>
#include <unistd.h>
#include <arts/debug.h>
#include "module.h"
#include "autorouter.h"
#include "portpropdlg.h"
#include "portposdlg.h"
#include "execdlg.h"
#include "namedlg.h"

using namespace std;

/*************************************/
class ArtsBuilderApp :public KApplication
{
protected:
	ModuleView *moduleview;
public:
	ArtsBuilderApp();
	ArtsBuilderApp(QString filename);
	void start();
	void end();
};

/*************************************/

Structure *ModuleWidget::theStructure()
{
	return structure;
}

void ModuleWidget::addModule ( const Arts::ModuleInfo& minfo )
{
	if(activeTool) delete activeTool;

	activeTool = new CreateModuleTool(this,minfo);
}

void ModuleWidget::addPort ( const Arts::PortType& type )
{
	if(activeTool) delete activeTool;

	activeTool = new CreatePortTool(this, type);
}

void ModuleWidget::insertPort( const Arts::PortType& type, int x, int y )
{
	StructurePort *port = structure->createStructurePort(type);
	unselectAll();
	port->move(x,y);
	port->setSelected(true);
	port->show();
}

void ModuleWidget::leaveTool(CreateTool *tool)
{
	assert(tool == activeTool);
	delete tool;
	activeTool = 0;
}

ModulePort *ModuleWidget::findPort(Module *m, int xoffset, int direction)
{
	return m->findPort(xoffset,direction);
}

bool ModuleWidget::insertModule( Module *newModule )
{
	if(hasSpace(newModule,newModule->x(),newModule->y(),true))
	{
		newModule->show();
		reRoute();
		return true;
	}
	return false;
}

void ModuleWidget::mousePressEvent( QMouseEvent *e )
{
	if(activeTool)
	{
		activeTool->mousePressEvent(e);
		return;
	}
	if ( e->button() == LeftButton )
	{
		movestart = e->pos();
		mouseButtonDown = true;

		int x = findCol(e->x());
		int y = findRow(e->y());

		int cellx = 0, celly=0;

		colXPos(x,&cellx);
		rowYPos(y,&celly);

		cellx = e->x() - cellx;
		celly = e->y() - celly;

		//arts_debug("clicked at %d,%d",cellx,celly);

		StructureComponent *c = structure->componentAt(x,y,false);
		ModulePort *port = NULL;

		if(c && (port = c->portAt(x-(c->x()),cellx,celly)))
		{
			if(port->selected)
			{
				PortPropDlg *ppd = new PortPropDlg(0,port);
				ppd->exec();
				port->selected = false;
				selectedports--;
			}
			else
			{
				port->selected = true;
				selectedports++;
			}
			if(selectedports == 2)
			{
				if(lastport->direction == port->direction)
				{
					port->selected = false;
					selectedports--;
   					KMessageBox::sorry( 0, 
					i18n("You can only connect an IN-port with an OUT-port,\n"
							"not two ports with the same direction."),
							i18n("Arts Warning") );

				}
				else
				{
					if(lastport->direction == ModulePort::in)
						connectPorts(port,lastport);
					else
						connectPorts(lastport,port);
				}
			}
			else
			{
				lastport = port;
			}
/*
				redrawModule(m);
*/
			reRoute();
			return;
		}

		beginUpdate();
		// select operations:
	
		// maintain selected group when
		// - clicking into a module that was already selected
		// - pressing the shift button
	
		bool group = (c && (c->selected() || (e->state() & ShiftButton)));
	
		// unselect all before moving, when not grouped
		if(!group) unselectAll();
	
		if(c) c->setSelected(true);

		endUpdate();
	}
}

void ModuleWidget::mouseMoveEvent( QMouseEvent *e )
{
	if(activeTool)
	{
		activeTool->mouseMoveEvent(e);
		return;
	}
	if(!mouseButtonDown) return;

	int dx = (e->x() - movestart.x())/cellsize;
	int dy = (e->y() - movestart.y())/cellsize;

	if(dx == 0 && dy == 0) return;

	list<StructureComponent *>::iterator i;
	list<StructureComponent *> *ComponentList = structure->getComponentList();

	for(i = ComponentList->begin();i != ComponentList->end();i++)
	{
		StructureComponent *c = *i;
		if(c->selected() && !hasSpace(c,c->x()+dx,c->y()+dy,true)) return;
	}

	beginUpdate();
	for(i = ComponentList->begin();i != ComponentList->end();i++)
	{
		StructureComponent *c = *i;
		if(c->selected())
			c->move(c->x()+dx,c->y()+dy);
	}
	endUpdate();
	
	movestart.setX(movestart.x()+dx*cellsize);
	movestart.setY(movestart.y()+dy*cellsize);
	reRoute();
}

bool ModuleWidget::hasSpace(StructureComponent *c,int destx,int desty,
											bool ignore_selected)
{
	int ddx,ddy;

	if(destx < 0) return(false);
	if(desty < 0) return(false);
	if(destx+c->width() > numCols()) return(false);
	if(desty+c->height() > numRows()) return(false);

	for (ddx = 0; ddx < c->width(); ddx++)
	{
		for (ddy = 0; ddy < c->height(); ddy++)
		{
			if(structure->componentAt(destx+ddx,desty+ddy,ignore_selected))
				return(false);
		}
	}

	return(true);
}

void ModuleWidget::mouseReleaseEvent( QMouseEvent *e )
{
	if(activeTool)
	{
		activeTool->mouseReleaseEvent(e);
		return;
	}
	mouseButtonDown = false;
}

void ModuleWidget::paintCellBackground(QPainter *p, int y, int x)
{
	QColor bgcolor;

	if((y & 1) == 1)
		bgcolor = QColor(168,168,168);
	else
		bgcolor = QColor(146,168,146);

	p->fillRect(0,0,cellsize,cellsize,QBrush(bgcolor));

	p->setPen(bgcolor.dark(115));
	p->drawLine(0,0,0,cellsize-1);
	p->drawLine(0,0,cellsize-1,0);

	if(x == (numCols()-1))
		p->drawLine(cellsize-1,0,cellsize-1,cellsize-1);
	if(y == (numRows()-1))
		p->drawLine(0,cellsize-1,cellsize-1,cellsize-1);
}

void ModuleWidget::unselectAll()
{
	setSelectAll(false);
}

void ModuleWidget::setSelectAll(bool newstate)
{
	list<StructureComponent *>::iterator module;
	
	for(module = structure->getComponentList()->begin();
        module != structure->getComponentList()->end();module++)
	{
		(*module)->setSelected(newstate);
	}
}

void ModuleWidget::beginUpdate()
{
	inUpdate = true;
}

void ModuleWidget::endUpdate()
{
	inUpdate = false;

	list<QRect>::iterator i;

	for (i = UpdateList.begin(); i != UpdateList.end(); i++)
	{
		redrawCells(*i);
	}

	UpdateList.erase(UpdateList.begin(),UpdateList.end());
}

void ModuleWidget::redrawModule(Module *m)
{
	QRect r = QRect(m->x(),m->y(),m->width(),m->height());

	if(!inUpdate)
	{
		redrawCells(r);
	}
	else
	{
		UpdateList.push_back(r);
	}
}

void ModuleWidget::redrawRect(int x, int y, int width, int height)
{
	QRect r = QRect(x,y,width,height);

	if(!inUpdate)
	{
		redrawCells(r);
	}
	else
	{
		UpdateList.push_back(r);
	}
}

void ModuleWidget::redrawCells(QRect &r)
{
	int x,y;

	for(x = r.left(); x<r.width()+r.left(); x++)
	{
		for(y = r.top(); y<r.height()+r.top(); y++)
		{
			updateCell(y,x,false);
		}
	}
}


void ModuleWidget::reRoute()
{
// clear everything
	autorouter->clear();

// add structure components (external ports/modules) to the router, so that
// cables won't be drawn over them

	list<StructureComponent *>::iterator c;
	list<ModulePort *> portlist;

	for(c = structure->getComponentList()->begin();
		c != structure->getComponentList()->end();c++)
	{
		StructureComponent *sc = *c;
		autorouter->set(sc->x()*2,sc->y()*2,
						(sc->x()+sc->width())*2-1,
						(sc->y()+sc->height())*2-1, AutoRouter::solid);

		sc->dumpPorts(portlist);
	}

	list<ModulePort *>::iterator pi;

// build a map with all input ports to find corresponding ports of connections

	map<long, ModulePort *> portmap;

	for(pi = portlist.begin();pi != portlist.end(); pi++)
	{
		ModulePort *port = *pi;

		if(port->direction == ModulePort::in) portmap[port->pdID] = port;
	}

// add connections to the router

	/*
	 * assign colors after the following algorithm:
	 *
	 * - initialize assuming that each port has its own color
	 * - if two ports are connected, they must have the same color, that
	 *   is, all references to the two colors must be made the same
	 *
	 * these colors are not used for drawing, but for making lines of
	 * different groups of ports not collide in the autorouter (ownership)
	 */
	map<ModulePort *, long> color;
	vector<long> owner(portlist.size());

	long maxcolor = 0;
	for(pi = portlist.begin();pi != portlist.end();pi++)
		color[*pi] = maxcolor++;

	for(pi = portlist.begin();pi != portlist.end();pi++)
	{
		ModulePort *src = *pi;
		unsigned long c;
		if(src->direction == ModulePort::out && src->PortDesc.isConnected())
		{
			vector<Arts::PortDesc> *conn = src->PortDesc.connections();

			for(c=0;c<conn->size();c++)
			{
				ModulePort *dest = portmap[(*conn)[c].ID()];
				long color_src = color[src];
				long color_dest = color[dest];

				if(color_src != color_dest)
				{
					list<ModulePort *>::iterator pi2;

					for(pi2 = portlist.begin();pi2 != portlist.end();pi2++)
					{
						ModulePort *p = *pi2;

						if(color[p] == color_dest) color[p] = color_src;
					}
				}
			}
		}
	}

	for(pi = portlist.begin();pi != portlist.end();pi++)
	{
		ModulePort *p = *pi;

		if(p->direction == ModulePort::out && p->PortDesc.isConnected())
		{
			ModulePort *src = p, *dest;
			long& route_owner = owner[color[src]];
			unsigned long c;

			vector<Arts::PortDesc> *conn = p->PortDesc.connections();

			for(c=0;c<conn->size();c++)
			{
				dest = portmap[(*conn)[c].ID()];
				if(dest)		// otherwise something bad has happend?
				{
/*
						arts_debug("autoroute add connection port %s.%s to %s.%s",
							src->owner->type.local8Bit().data(),src->description.local8Bit().data(),
							dest->owner->type.local8Bit().data(),dest->description.local8Bit().data());
*/

					int x1 = (src->owner->x()+src->drawsegment)*2;
					int y1 = src->owner->y()*2+1;

					int x2 = (dest->owner->x()+dest->drawsegment)*2;
					int y2 = dest->owner->y()*2;

					route_owner = autorouter->connect(x1,y1,x2,y2,route_owner);
				}
			}
			delete conn;
		}
	}

	autorouter->sync();
}

void ModuleWidget::redrawAll()
{
// redraw everything
	QRect updaterect(0,0,cols,rows);
	redrawCells(updaterect);
}

void ModuleWidget::paintConnection(QPainter *p, int x, int y, int arx, int ary)
{
	long linetype = autorouter->get(arx,ary);
	long ud_owner = -1, lr_owner = -1, lr_break = 0, ud_break = 0;

	autorouter->getowners(arx,ary,ud_owner,lr_owner);

	p->setPen(QColor(255,255,255));

/*
	if(linetype == AutoRouter::none)
	{
		p->drawPoint(x+cellsize/4,y+cellsize/4);
	}
	if(linetype & AutoRouter::solid)
	{
		QBrush whitefill(QColor(255,255,255));

		p->fillRect(x+cellsize/6,y+cellsize/6,cellsize/6,cellsize/6,whitefill);
	}
*/
	x += cellsize/4;
	y += cellsize/4;

	// both used?
	if(ud_owner != -1 && lr_owner != -1)
	{
		// and not of the same owner?
		if(ud_owner != lr_owner)
		{
			// then we'll have to paint one of them broken
			if(ud_owner > lr_owner)
				lr_break = cellsize/8;
			else
				ud_break = cellsize/8;
		}
	}

	if(linetype & AutoRouter::left)
		p->drawLine(x-cellsize/4,y,x-lr_break,y);
	if(linetype & AutoRouter::right)
		p->drawLine(x+cellsize/4,y,x+lr_break,y);
	if(linetype & AutoRouter::up)
		p->drawLine(x,y-cellsize/4,x,y-ud_break);
	if(linetype & AutoRouter::down)
		p->drawLine(x,y+cellsize/4,x,y+ud_break);
}

void ModuleWidget::paintConnections(QPainter *p, int y, int x)
{
	int dx,dy;
	for(dx = 0; dx < 2; dx++)
	{
		for(dy = 0; dy < 2; dy++)
		{
			paintConnection(p,(cellsize*dx)/2,(cellsize*dy)/2,x*2+dx,y*2+dy);
		}
	}
}

void ModuleWidget::paintCell(QPainter *p, int y, int x)
{
#if 0 /* PORT */
	if(theArtsBuilderApp->eventStackDepth() > 1)
	{
		// FIXME: set some redraw flag or something like that
		return;
	}
#endif

	list<StructureComponent *>::iterator c;
	for(c = structure->getComponentList()->begin();
		c != structure->getComponentList()->end();c++)
	{
		StructureComponent *mwc = *c;
		if(y == mwc->y() && mwc->visible())
		{
			int xoffset = x-mwc->x();

			if(xoffset >= 0 && xoffset < mwc->width())
			{
				if(mwc->drawNeedsBackground(xoffset))
					 paintCellBackground(p,y,x);

				mwc->drawSegment(p,cellsize,xoffset);
				paintConnections(p,y,x);
				return;
			}
		}
	}
	paintCellBackground(p,y,x);
	paintConnections(p,y,x);
}

bool ModuleWidget::connectPorts(ModulePort *src, ModulePort *dest)
{
	bool success = src->PortDesc.connectTo(dest->PortDesc);

	selectedports = 0;
	src->selected = false;
	dest->selected = false;

/** TODO
	if(src->owner) redrawModule(src->owner);
	if(dest->owner) redrawModule(dest->owner);
*/
	return success;
}

// ---------------------------------------------------------------------------
// public part of modulewidget
// ---------------------------------------------------------------------------

void ModuleWidget::setZoom(int zoom)
{
	cellsize = (int)(50.0 * (float)zoom/100);

	setCellHeight(cellsize);
	setCellWidth(cellsize);
	updateTableSize();
	resize(cellsize*cols,cellsize*rows);
}

void ModuleWidget::selectAll()
{
	setSelectAll(true);
}

void ModuleWidget::reInit()
{
	selectedports = 0;

	reRoute();
	redrawAll();
}

void ModuleWidget::delModule()
{
	if(!structure->countSelected()) return;

	if (KMessageBox::warningContinueCancel(0,
		i18n("Delete selected module(s), port(s) and connections? (No undo possible!)"),
		QString::null, i18n("&Delete")) == KMessageBox::Continue)
	{
		structure->deleteSelected();
		selectedports = 0;
		reRoute();
		redrawAll();
	}
}

void ModuleWidget::autoRedrawRouter()
{
	if(autorouter->needRedraw()) redrawAll();
}

ModuleWidget::ModuleWidget(Structure *structure, QWidget *parent, const char *name, WFlags f)
		:QTableView(parent,name,f)
{
	arts_debug("PORT: mw; getmodulelist");
	this->ModuleList = structure->getModuleList();
	this->structure = structure;
	activeTool = 0;
	mouseButtonDown = false;
	arts_debug("PORT: mw; cols&rows");

	cols = 24;
	rows = 32;

	setNumCols(cols);
	setNumRows(rows);
	setTableFlags(Tbl_autoScrollBars);
	setZoom(100);

	arts_debug("PORT: mw; bgmode");
	setBackgroundMode(NoBackground);

	selectedports = 0;
	arts_debug("PORT: mw; new ar %d,%d", cols, rows);
	autorouter = new AutoRouter(cols*2,rows*2);
	arts_debug("PORT: mw; new ar ok - qtimer");

    QTimer *timer = new QTimer( this );
	connect( timer, SIGNAL(timeout()),
		this, SLOT(autoRedrawRouter()) );
	arts_debug("PORT: mw; tstart");
	timer->start( 100, FALSE );                 // 100 ms reoccuring check
};

ModuleWidget::~ModuleWidget()
{
	delete autorouter;
}

ModuleView::ModuleView(const char *name) : KMainWindow(0, name)
{
	KStdAccel keys;
 
	arts_debug("PORT: structure");
	structure = new Structure();
	arts_debug("PORT: structure ok");
	//ModuleList = structure->getModuleList();

	arts_debug("PORT: modulewidget");
	modulewidget = new ModuleWidget(structure, this, "mwidget");
	arts_debug("PORT: modulewidget ok");

	arts_debug("PORT: setcanvas");
	structure->setCanvas(modulewidget);
	arts_debug("PORT: setcanvas ok");

	m_file = new QPopupMenu;
	m_file_new = new QPopupMenu;
	m_edit = new QPopupMenu;
	m_view = new QPopupMenu;
	m_modules = new QPopupMenu;
	m_ports = new QPopupMenu;

/*
	m_modules_gui = new QPopupMenu;
	m_modules_synth = new QPopupMenu;
	m_modules_instruments = new QPopupMenu;
	m_modules_other = new QPopupMenu;
	*/

	arts_debug("PORT: first menu stuff");
	m_file->insertItem(i18n("Open Session..."), this , SLOT(openSession()));
    m_file->insertSeparator();
	m_file->insertItem(i18n("&New"), m_file_new);
	m_file->insertItem(i18n("&Open..."), this , SLOT(open()), keys.open());
	m_file->insertItem(i18n("Open E&xample..."), this , SLOT(openExample()));
	m_file->insertItem(i18n("Save &As..."), this , SLOT(saveAs()));
	m_file->insertItem(i18n("&Save..."), this , SLOT(save()), keys.save(), id_save);
	m_file->setItemEnabled(id_save, false);
	m_file->insertItem(i18n("&Retrieve from server..."), this, SLOT(retrieve()));
    m_file->insertSeparator();
	m_file->insertItem(i18n("&Execute Structure..."), this, SLOT(execute()));
	m_file->insertItem(i18n("&Rename Structure..."), this, SLOT(rename()));
	m_file->insertItem(i18n("&Publish Structure!"), this, SLOT(publish()),
														CTRL+Key_P);
    m_file->insertSeparator();
	m_file->insertItem(i18n("&Quit"), this , SLOT(close()), keys.quit());

	m_edit->insertItem(i18n("&Delete"), modulewidget, SLOT(delModule()), Key_Delete);
    m_edit->insertSeparator();
    m_edit->insertItem(i18n("Select &all"), modulewidget, SLOT(selectAll()),
														CTRL+Key_A);
 

	m_view->insertItem(i18n("200%"), 200);
	m_view->insertItem(i18n("150%"), 150);
	m_view->insertItem(i18n("100%"), 100);
	m_view->insertItem(i18n("50%"), 50);

	m_ports->insertItem(i18n("Create IN audio signal"), 1);
	m_ports->insertItem(i18n("Create OUT audio signal"), 2);
	m_ports->insertItem(i18n("Create IN string property"), 3);
	m_ports->insertItem(i18n("Create IN audio property"), 4);
    m_ports->insertSeparator();
	m_ports->insertItem(i18n("Change positions/names"), this, SLOT(changePortPositions())); //,0,-1);

#if 0  /* PORT */
	ModuleBroker = Synthesizer->moduleBroker();
	assert(ModuleBroker);

//---- publish my widgets on the server ----

	GUIServer=new GUIServer_impl(ModuleBroker,Synthesizer);
    arts_debug("GUIServer:\n%s",ArtsOrb->object_to_string(GUIServer));
	GUIServer->incRef();

//---- trigger autoloading of all structures that are present in my dirs ----

	list<string> datadirs = PortableKDE::globalDirs("data");
	list<string>::iterator it;

	for(it = datadirs.begin(); it != datadirs.end(); it++)
	{
		string common = *it;
		common += "artsbuilder";
		if(chdir(common.c_str()) == 0)
			ModuleBroker->addPublishingPath(common.c_str());
	}
	/*
	string common = (const char *)PortableKDE::globalDir("data");
	common += "/artsbuilder";
	arts_debug("%s",common.c_str());
	if(chdir(common.c_str()) == 0)
		ModuleBroker->addPublishingPath(common.c_str());
	*/

	// just make sure that the mapsDir exists
	(void)DirManager::mapDir();

	if(chdir(DirManager::structureDir()) == 0)	// retry
	{
		ModuleBroker->addPublishingPath(DirManager::structureDir());
		Synthesizer->addArtsDirectory(DirManager::baseDir());

	}
#endif

//----

	mbroker_updateCount = 0;
	execDlg = 0;
	KPopupMenu *m_help =  helpMenu(0, FALSE);
		       
	arts_debug("PORT: kmenubar");
	menubar = new KMenuBar (this, "menubar");
	menubar->insertItem (i18n("&File"), m_file);
	menubar->insertItem (i18n("&Edit"), m_edit);
	menubar->insertItem (i18n("&View"), m_view);
	modules_menu_item = menubar->insertItem (i18n("&Modules"), m_modules);
	menubar->insertItem (i18n("&Ports"), m_ports);
	menubar->insertSeparator();
	menubar->insertItem( i18n("&Help"), m_help);

	arts_debug("PORT: setCentralWidget");
	setCentralWidget(modulewidget);

	arts_debug("PORT: menumaker");
	menumaker = new MenuMaker(m_modules);
	//menumaker->addCategory("&Gui","Gui_");
	menumaker->addCategory(i18n("&Synthesis"),"Arts::Synth_");
	menumaker->addCategory(i18n("&Synthesis/&Arithmetic + Mixing"),"Arts::Synth_ADD$");
	menumaker->addCategory(i18n("&Synthesis/&Arithmetic + Mixing"),"Arts::Synth_AUTOPANNER$");
	menumaker->addCategory(i18n("&Synthesis/&Arithmetic + Mixing"),"Arts::Synth_MUL$");
	menumaker->addCategory(i18n("&Synthesis/&Arithmetic + Mixing"),"Arts::Synth_MULTI_ADD$");
	menumaker->addCategory(i18n("&Synthesis/&Arithmetic + Mixing"),"Arts::Synth_XFADE$");
	menumaker->addCategory(i18n("&Synthesis/&Busses"),"Arts::Synth_BUS_");
	menumaker->addCategory(i18n("&Synthesis/&Delays"),"Arts::Synth_DELAY$");
	menumaker->addCategory(i18n("&Synthesis/&Delays"),"Arts::Synth_CDELAY$");
	menumaker->addCategory(i18n("&Synthesis/&Envelopes"),"Arts::Synth_PSCALE$");
	menumaker->addCategory(i18n("&Synthesis/&Envelopes"),"Arts::Synth_ENVELOPE_");
	menumaker->addCategory(i18n("&Synthesis/Effe&cts"),"Arts::Synth_FREEVERB$");
	menumaker->addCategory(i18n("&Synthesis/Effe&cts"),"Arts::Synth_FX_");
	menumaker->addCategory(i18n("&Synthesis/Effe&cts"),"Arts::Synth_PITCH_SHIFT$");
	menumaker->addCategory(i18n("&Synthesis/Effe&cts"),"Arts::Synth_TREMOLO$");
	menumaker->addCategory(i18n("&Synthesis/&Filters"),"Arts::Synth_ATAN_SATURATE$");
	menumaker->addCategory(i18n("&Synthesis/&Filters"),"Arts::Synth_BRICKWALL_LIMITER$");
	menumaker->addCategory(i18n("&Synthesis/&Filters"),"Arts::Synth_MOOG_VCF");
	menumaker->addCategory(i18n("&Synthesis/&Filters"),"Arts::Synth_SHELVE_CUTOFF$");
	menumaker->addCategory(i18n("&Synthesis/&Filters"),"Arts::Synth_RC$");
	menumaker->addCategory(i18n("&Synthesis/&Filters"),"Arts::Synth_STD_EQUALIZER$");
	menumaker->addCategory(i18n("&Synthesis/&Midi + Sequencing"),"Arts::Synth_MIDI");
	menumaker->addCategory(i18n("&Synthesis/&Midi + Sequencing"),"Arts::Interface_MIDI");
	menumaker->addCategory(i18n("&Synthesis/&Midi + Sequencing"),"Arts::Synth_SEQUENCE$");
	menumaker->addCategory(i18n("&Synthesis/&Midi + Sequencing"),"Arts::Synth_STRUCT_KILL$");
	menumaker->addCategory(i18n("&Synthesis/Sam&ples "),"Arts::Synth_PLAY_");
	menumaker->addCategory(i18n("&Synthesis/&Sound IO"),"Arts::Synth_AMAN_");
	menumaker->addCategory(i18n("&Synthesis/&Sound IO"),"Arts::Synth_CAPTURE_WAV$");
	menumaker->addCategory(i18n("&Synthesis/&Sound IO"),"Arts::Synth_PLAY$");
	menumaker->addCategory(i18n("&Synthesis/&Sound IO"),"Arts::Synth_RECORD$");
	menumaker->addCategory(i18n("&Synthesis/&Sound IO"),"Arts::Synth_FULL_DUPLEX_");
	menumaker->addCategory(i18n("&Synthesis/&Sound IO"),"Arts::Synth_FILEPLAY");
	menumaker->addCategory(i18n("&Synthesis/&Tests"),"Arts::Synth_NIL$");
	menumaker->addCategory(i18n("&Synthesis/&Tests"),"Arts::Synth_DEBUG$");
	menumaker->addCategory(i18n("&Synthesis/&Tests"),"Arts::Synth_DATA$");
	menumaker->addCategory(i18n("&Synthesis/&Tests"),"Arts::Synth_MIDI_DEBUG$");
	menumaker->addCategory(i18n("&Synthesis/&Oscillation & Modulation"),"Arts::Synth_FREQUENCY$");
	menumaker->addCategory(i18n("&Synthesis/&Oscillation & Modulation"),"Arts::Synth_FM_SOURCE$");
	menumaker->addCategory(i18n("&Synthesis/&WaveForms"),"Arts::Synth_WAVE_");
	menumaker->addCategory(i18n("&Synthesis/&WaveForms"),"Arts::Synth_NOISE$");
	menumaker->addCategory(i18n("&Synthesis/&Internal"),"Arts::Synth_PARAM_");

	menumaker->addCategory(i18n("&Examples"),"example_");
	menumaker->addCategory(i18n("&Instruments"),"instrument_");
	menumaker->addCategory(i18n("&Mixer-Elements"),"mixer_element_");
	menumaker->addCategory(i18n("&Templates"),"template_");
	menumaker->addCategory(i18n("&Other"),"*");
	arts_debug("PORT: menumaker ok");

/*
	m_modules->insertItem(i18n("&Gui"), m_modules_gui);
	m_modules->insertItem(i18n("&Synthesis"), m_modules_synth);
	m_modules->insertItem(i18n("&Instruments"), m_modules_instruments);
	m_modules->insertItem(i18n("&Other"), m_modules_other);
 */

	connect(menubar,SIGNAL(highlighted(int)),this,SLOT(activateMenu(int)));
	connect(m_view,SIGNAL(activated(int)),modulewidget,SLOT(setZoom(int)));
	connect(m_ports,SIGNAL(activated(int)),this,SLOT(addPort(int)));
	connect(m_file_new,SIGNAL(activated(int)),this,SLOT(fileNew(int)));

	connect(menumaker,SIGNAL(activated(int)),this,SLOT(addModule(int)));
	//connect(m_modules,SIGNAL(activated(int)),this,SLOT(addModule(int)));
	/*
	connect(m_modules_synth,SIGNAL(activated(int)),this,SLOT(addModule(int)));
	connect(m_modules_gui,SIGNAL(activated(int)),this,SLOT(addModule(int)));
	connect(m_modules_instruments,SIGNAL(activated(int)),this,SLOT(addModule(int)));
	connect(m_modules_other,SIGNAL(activated(int)),this,SLOT(addModule(int)));
	*/
	connect(kapp,SIGNAL(lastWindowClosed()), this , SLOT(quit()));

	// update the modules menu once for the start
	arts_debug("PORT: activatemenu");
	activateMenu(modules_menu_item);
	arts_debug("PORT: activatemenu ok");

	m_filename = (QString) 0;
	modified = false;
}

void ModuleView::activateMenu(int)
{
	long updateCount = 3;  /* PORT: automatic update of menues missing */
	if(updateCount != mbroker_updateCount)
	{
		mbroker_updateCount = updateCount;
		//---- query all available objects ----
		Arts::TraderQuery query;
		query.supports("Buildable","true");
		vector<Arts::TraderOffer> *offers = query.query();

		menumaker->clear();
		m_file_new->clear();

		vector<Arts::TraderOffer>::iterator i;
		long n = 1;		/* TODO:PORT: is this necessary? I think not */
		for(i=offers->begin(); i != offers->end(); i++)
		{
			Arts::TraderOffer& offer = *i;
			string name = offer.interfaceName();
			menumaker->addItem(name.c_str(),n++);
			
			/* PORT: templates missing
			if(strncmp(name,"template_",strlen("template_")) == 0)
			{
				char *xname = strdup(&name[strlen("template_")]);
				int x;
				for(x=0;xname[x] != 0; x++)
					if(xname[x] == '_') xname[x] = ' ';

				m_file_new->insertItem(xname, i);
			}
			*/
		}
		delete offers;
	}
#if 0 
	if(0) /*item == modules_menu_item)  PORT!!! */
	{
		long updateCount = ModuleBroker->updateCount();

		// if the contents of the ModukeBroker changed, update our "modules"-Menu
		if(updateCount != mbroker_updateCount)
		{
			mbroker_updateCount = updateCount;
			//---- query all available objects ----
			ArtsCorba::StringSeq_var Modules = ModuleBroker->publishedModules();
			assert(Modules);

			menumaker->clear();
			m_file_new->clear();

			unsigned long i;
			for(i=0;i<Modules->length();i++)
			{
				const char *name = (*Modules)[i];
				menumaker->addItem(name,i);
			
				if(strncmp(name,"template_",strlen("template_")) == 0)
				{
					char *xname = strdup(&name[strlen("template_")]);
					int x;
					for(x=0;xname[x] != 0; x++)
						if(xname[x] == '_') xname[x] = ' ';

					m_file_new->insertItem(xname, i);
				}
			}
		}
	}
#endif
}

void ModuleView::quit()
{
	if(execDlg) return;
	arts_debug(">> ModuleView::quit() called");
	kapp->quit();
	arts_debug("<< leaving ModuleView::quit()");
}

ModuleView::~ModuleView()
{
	delete structure;
}

void ModuleView::clear()
{
	structure->clear();
	setModified(false);
}

void ModuleView::publish()
{
	checkName();
	structure->publish();
	KMessageBox::information(0,
		QString(i18n("The structure has been published as: '"))
		+QString(structure->name().c_str())+QString(i18n("' on the server.")));
}

QString ModuleView::getOpenFilename(const char *pattern, const char *initialDir)
{
	arts_debug(">>>>> getOpenFilename");
	QString filename = KFileDialog::getOpenFileName(initialDir,pattern,this);
	arts_debug(">>>>> opendlg closed");
	if(!filename.isEmpty())
	{
		arts_debug("open... %s",filename.local8Bit().data());

		// check that the file is ok:

		FILE *infile = fopen(filename,"r");

		if(infile)
		{
			fclose(infile);
			return(filename);
		}
	}
	return QString("");
}

void ModuleView::open()
{
	if (!promptToSave())
		return;

	QString	filename = getOpenFilename("*.arts", DirManager::structureDir());
	open(filename);
	m_filename = filename;
	m_file->setItemEnabled(id_save, true);
	setModified(false);
	setCaption(m_filename);
}

void ModuleView::open(QString filename)
{
	if (!promptToSave())
		return;

	if(!filename.isEmpty())
	{
		structure->load(filename);
		modulewidget->reInit();
		if(!structure->valid())
		{
			KMessageBox::sorry( 0, 
			i18n("The structure couldn't be loaded correctly. Maybe some of\n"
				 "the modules used in the file aren't available in this\n"
				 "version of aRts."),
				i18n("Arts Warning"));
		}
		m_filename = filename;
		m_file->setItemEnabled(id_save, true);
		setModified(false);
		setCaption(m_filename);
	}
}

void ModuleView::openSession()
{
	if (!promptToSave())
		return;

	QString filename = getOpenFilename("*.arts-session", DirManager::sessionDir());

	if(!filename.isEmpty())
	{
		Session *session = new Session();
		session->loadSession(filename);

		assert(!execDlg);
		execDlg = new ExecDlg(0,session);
		assert(execDlg);

		// this will create the widgets that will eventually get into the
		// execdlg
		session->startExecute();

		execDlg->start();
		execDlg->show();

		connect(execDlg,SIGNAL( ready() ), this, SLOT( endexecute() ));

		hide();
		setModified(false);
	}
}

void ModuleView::openExample()
{
	if (!promptToSave())
		return;

	QString dir = locate("data", "artsbuilder/examples/");
	if (!dir)
		KMessageBox::sorry(
			0, 
			i18n("Unable to find the examples directory.\nUsing the current directory instead."),
			i18n("Arts Warning"));

	QString	filename = getOpenFilename("*.arts", dir);
	open(filename);
	m_filename = filename;
	m_file->setItemEnabled(id_save, true);
	setModified(false);
	setCaption(m_filename);
}

void ModuleView::saveAs()
{
	checkName();
	chdir(DirManager::structureDir());
    QString filename;
	KFileDialog *dlg = new KFileDialog(0,"*.arts",this,0,true /*,false TODO: acceptURLs */);
	string defaultname = string(structure->name())+string(".arts");

	dlg->setSelection(defaultname.c_str());
	dlg->setCaption(i18n("Open"));

	if (dlg->exec() == QDialog::Accepted)
		filename = dlg->selectedFile();

	delete dlg;
	// QString filename = KFileDialog::getSaveFileName(0,"*.arts",this);
	// filename.detach();

	if(!filename.isEmpty())
	{
		arts_debug("save as... %s",filename.local8Bit().data());

		// clear the file first:

		FILE *outfile = fopen(filename,"w");

		if (!outfile) {
			KMessageBox::sorry(
				0, 
				i18n("The file could not be opened for writing."),
				i18n("Arts Warning"));
			return;
		}

		fclose(outfile);

		bool status = structure->save(filename);
		if (!status) {
			KMessageBox::sorry(
				0, 
				i18n("Unable to save to file."),
				i18n("Arts Warning"));
			return;
		}

		setModified(false);
		m_filename = filename;
		m_file->setItemEnabled(id_save, true);
		setCaption(m_filename);
	}
}

void ModuleView::save()
{
	arts_debug("save... %s", m_filename.local8Bit().data());

	// clear the file first:

	FILE *outfile = fopen(m_filename,"w");

	if (!outfile) {
		KMessageBox::sorry(
			0, 
			i18n("The file could not be opened for writing."),
			i18n("Arts Warning"));
		return;
	}

	fclose(outfile);

	bool status = structure->save(m_filename);
	if (!status) {
		KMessageBox::sorry(
			0, 
			i18n("Unable to save to file."),
			i18n("Arts Warning"));
		return;
	}

	setModified(false);
}

void ModuleView::checkName()
{
	if(strncmp(structure->name().c_str(),"template_",strlen("template_")) == 0)
		rename();
}

void ModuleView::rename()
{
	NameDlg *nd = new NameDlg(0,structure->name().c_str(),i18n("Rename Structure"));
	assert(nd);

	if(nd->exec())
	{
		arts_debug("rename OK...");
		structure->rename(nd->name());
	}
	delete nd;
	setModified(true);
}

void ModuleView::retrieve()
{
	if (!promptToSave())
		return;

	RetrieveDlg *rd = new RetrieveDlg(0);
	assert(rd);

	if(rd->exec())
	{
		const char *result = rd->result();
		if(result)
		{
			structure->retrieve(result);
			modulewidget->reInit();
		}
	}
	delete rd;
	setModified(true);
}

void ModuleView::execute()
{
	assert(!execDlg);
	execDlg = new ExecDlg(0,structure);
	assert(execDlg);

	// this will create the widgets that will eventually get into the
	// execdlg
	if(structure->startExecute())
	{
		execDlg->start();
		execDlg->show();

		connect(execDlg,SIGNAL( ready() ), this, SLOT( endexecute() ));

		hide();
	}
	else
	{
		delete execDlg;
		execDlg = 0;

		KMessageBox::sorry( 0, 
			i18n("Couldn't execute your structure. Make sure that the\n"
				 "sound server (artsd) is running.\n"), i18n("Arts Warning"));
	}
}

void ModuleView::endexecute()
{
	show();
	assert(execDlg);
	delete execDlg;
	// will be done by the execDlg itself now
	//structure->stopExecute();

	execDlg = 0;
}

void ModuleView::fileNew(int what)
{
	if (!promptToSave())
		return;

	const char *name = menumaker->findID(what);
	assert(name);
	structure->retrieve(name);
	modulewidget->reInit();
	setModified(false);
}

void ModuleView::addPort(int port)
{
	Arts::PortType pt;

	arts_debug("selected (port=%d)", port);
	switch(port)
	{
		case 1:		// data that goes into the structure
			pt.direction = Arts::output;
			pt.dataType = "float";
			pt.connType = Arts::conn_stream;
			break;
		case 2:		// data that goes out of the structure
			pt.direction = Arts::input;
			pt.dataType = "float";
			pt.connType = Arts::conn_stream;
			break;
		case 3:		// data that goes into the structure
			pt.direction = Arts::output;
			pt.dataType = "string";
			pt.connType = Arts::conn_property;
			break;
		case 4:		// data that goes into the structure
			pt.direction = Arts::output;
			pt.dataType = "float";
			pt.connType = Arts::conn_property;
			break;
		default:	// forget it, no add intended
			return;
	}
	modulewidget->addPort(pt);
	setModified(true);
}

void ModuleView::changePortPositions()
{
	PortPosDlg *ppd = new PortPosDlg(this,structure);
	ppd->exec();
	setModified(true);
	// XXX: delete ppd?
}

void ModuleView::addModule(int module)
{
	const char *name = menumaker->findID(module);
	assert(name);

	arts_debug("selected (%s) (module=%d)", name, module);
	Arts::ModuleInfo minfo = makeModuleInfo(name);

	if(minfo.name != "")
		modulewidget->addModule(minfo);
#if 0
	const char *name = menumaker->findID(module);
	assert(name);

	arts_debug("selected (%s) (module=%d)", name, module);

	ArtsCorba::ModuleBroker_var ModuleBroker = Synthesizer->moduleBroker();
	ArtsCorba::ModuleInfo_var minfo = ModuleBroker->lookupModule(name);


	if(minfo)
	{
		modulewidget->addModule(minfo);

/*
		Module *m = structure->createModule(minfo);
		modulewidget->addModule(m);
*/
	}
#endif
	setModified(true);
}

bool ModuleView::isModified()
{
	return modified;
}

void ModuleView::setModified(bool m)
{
	modified = m;
}

bool ModuleView::queryClose()
{
	return promptToSave();
}

bool ModuleView::promptToSave()
{
	bool result;
	int query;

	if (!isModified())
		return true;

	query = KMessageBox::warningYesNoCancel(
		0,
		i18n("The current document has been modified.\nWould you like to save it?"));

	switch (query)
	{
	  case KMessageBox::Yes:
		  saveAs();
		  result = true;
		  setModified(false);
		  break;
	  case KMessageBox::No:
		  result = true;
		  setModified(false);
		  break;
	  case KMessageBox::Cancel:
		  result = false;
		  break;
	}
	return result;
}

ArtsBuilderApp::ArtsBuilderApp()
{
	start();
}

ArtsBuilderApp::ArtsBuilderApp(QString filename)
{
	start();
	if (QFile::exists(filename))
	{
		moduleview->open(filename);
	} else {
		KMessageBox::sorry( 0, 
			i18n("The specified file does not exist."),
			i18n("Arts Warning"));
	}
}

void ArtsBuilderApp::start()
{
	arts_debug("PORT: moduleview");
	moduleview = new ModuleView("main");

	arts_debug("PORT: moduleview ok");
	moduleview->resize(680,500);
	arts_debug("PORT: moduleview show");
	moduleview->show();
	arts_debug("PORT: moduleview show ok");
#if 0 /* PORT */
	ArtsCorba::ModuleBroker_var ModuleBroker = theSynthesizer->moduleBroker();
	assert(ModuleBroker);
#endif

	//setMainWidget(moduleview);
	setTopWidget(moduleview);
}

void ArtsBuilderApp::end()
{
	delete moduleview;

	arts_debug(">>>>>decref now");
	fflush(stdout);
	arts_debug("<<<<<decref ok");
	fflush(stdout);
}


static KCmdLineOptions options[] = 
{
	{ "+[file]", I18N_NOOP("Optional .arts file to be loaded."), 0 },
	{ 0, 0, 0 }
};

#ifdef COMMON_BINARY
int artsbuilder_main(int argc, char **argv)
#else
int main(int argc, char **argv)
#endif
{
	KAboutData aboutData("artsbuilder",
						 I18N_NOOP("artsbuilder"),
						 ARTS_VERSION,
						 I18N_NOOP("Arts synthesizer designer"),
						 KAboutData::License_GPL,
						 "(C) 1998-2001, Stefan Westerfeld",
						 I18N_NOOP("The analog real-time synthesizer graphical design tool."),
						 "http://www.arts-project.org/",
						 "submit@bugs.kde.org");

	aboutData.addAuthor( "Stefan Westerfeld", I18N_NOOP( "Author" ), "stefan@twc.de" );
	aboutData.addCredit("Jens Hahn", 0, "Jens.Hahn@t-online.de");
	aboutData.addCredit("Martin Lorenz", 0, "lorenz@ch.tum.de");
	aboutData.addCredit("Waldo Bastian", 0, "bastian@kde.org");
	aboutData.addCredit("Jeff Tranter", 0, "tranter@pobox.com");

	KCmdLineArgs::init(argc, argv, &aboutData);
	KCmdLineArgs::addCmdLineOptions(options);
	Arts::QIOManager iomanager;
	Arts::Dispatcher dispatcher(&iomanager);

	Arts::ObjectManager::the()->provideCapability("kdegui");

	// check for one optional filename argument
	KCmdLineArgs *args = KCmdLineArgs::parsedArgs();
	if (args->count() > 1) {
		args->usage("");
	}
	if (args->count() > 0)
	{
		ArtsBuilderApp Application(QFile::decodeName(args->arg(0)));
		args->clear();
		return Application.exec();
	} else {
		ArtsBuilderApp Application;
		args->clear();
		return Application.exec();
	}
}
#include "main.moc"
