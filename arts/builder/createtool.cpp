    /*

    Copyright (C) 1999 Stefan Westerfeld
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

#include "createtool.h"
#include "module.h"
#include "portablekde.h"

CreateTool::CreateTool(ModuleWidget *mwidget)
{
	this->mwidget = mwidget;

	oldCursor = mwidget->cursor();
	mwidget->setCursor(Qt::crossCursor);

	oldMouseTracking = mwidget->hasMouseTracking();
	mwidget->setMouseTracking(true);

	width = -1;
	height = -1;
	moduleRect = QRect(0,0,0,0);
}

CreateTool::~CreateTool()
{
	mwidget->repaint(moduleRect);
	mwidget->setCursor(oldCursor);
	mwidget->setMouseTracking(oldMouseTracking);
}

void CreateTool::mousePressEvent(QMouseEvent *e)
{
	int x = mwidget->findCol(e->x());
	int y = mwidget->findRow(e->y());

	if(x<0 || y<0) return;

	if( e->button() == Qt::LeftButton )
		performCreate(x,y);

	// well, perhaps make this an option
	mwidget->leaveTool(this);
}

void CreateTool::mouseMoveEvent(QMouseEvent *e)
{
	if(width == -1) estimateSize();

	int x = mwidget->findCol(e->x());
	int y = mwidget->findRow(e->y());

	if(x<0 || y<0) return;

	int cellx, celly, cellx2,celly2;

	bool posok = mwidget->colXPos(x,&cellx) && mwidget->rowYPos(y,&celly)
     && mwidget->colXPos(x+width,&cellx2) && mwidget->rowYPos(y+height,&celly2);
	if(!posok) return;

	mwidget->repaint(moduleRect);
	moduleRect = QRect(cellx,celly,cellx2-cellx,celly2-celly);

	QPainter *p = new QPainter;

	p->begin(mwidget);
	p->save();
	p->drawRect(moduleRect);
	p->restore();
	p->end();
}

void CreateTool::mouseReleaseEvent(QMouseEvent *e)
{
}

// module creation

void CreateModuleTool::estimateSize()
{
// yes, this is a it ugly: we create here the new module for a microsecond,
// just to see how big it is, to be able to paint an accurate preview
//
// We delete it again, and if the user finally really creates the module
// we recreate it.

	mwidget->unselectAll();

	Module *newModule = mwidget->theStructure()->createModule(minfo);
	width = newModule->width();
	height = newModule->height();
	newModule->setSelected(true);

	mwidget->theStructure()->deleteSelected();
}

void CreateModuleTool::performCreate(int x, int y)
{
	mwidget->unselectAll();
	Module *newModule = mwidget->theStructure()->createModule(minfo);
	newModule->setSelected(true);
	newModule->move(x,y);

	if(!mwidget->insertModule(newModule))
		mwidget->theStructure()->deleteSelected();
}

CreateModuleTool::CreateModuleTool(ModuleWidget *mwidget,
     const Arts::ModuleInfo& minfo) : CreateTool(mwidget)
{
	this->minfo = minfo;
}

CreateModuleTool::~CreateModuleTool()
{
}

// port creation

void CreatePortTool::estimateSize()
{
	width = height = 1;
}

void CreatePortTool::performCreate(int x, int y)
{
	mwidget->insertPort(type,x,y);
}

CreatePortTool::CreatePortTool(ModuleWidget *mwidget,
	const Arts::PortType& type) : CreateTool(mwidget)
{
	this->type = type;
}
