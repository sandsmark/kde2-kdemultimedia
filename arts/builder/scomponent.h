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

#ifndef __SCOMPONENT_H__
#define __SCOMPONENT_H__

#include <qscrollview.h>
#include <qpainter.h>
#include <qpixmap.h>
#include <list>

class ModulePort;

class StructureCanvas {
public:
	virtual void redrawRect(int x, int y, int width, int height) = 0;
};

class StructureComponent {
protected:
	StructureCanvas *canvas;

public:
	StructureComponent(StructureCanvas *canvas);
	virtual ~StructureComponent();

	// type

	enum ComponentType { ctModule, ctPort };
	virtual ComponentType type() = 0;
	
	// TODO: connection & autorouter

	virtual ModulePort *portAt(int segment, int x, int y) = 0;
	virtual void dumpPorts(std::list<ModulePort *>& ports) = 0;

	// visibility
	virtual void hide() = 0;
	virtual void show() = 0;
	virtual bool visible() = 0;

	// drawing
	virtual bool drawNeedsBackground(int segment) = 0;
	virtual void drawSegment(QPainter *dest, int cellsize, int segment) = 0;
	virtual QPixmap *pixmap() = 0;
	virtual const char *name() = 0;

	// space checking & positioning
	virtual bool move(int x, int y) = 0;
	virtual int width() = 0;
	virtual int height() = 0;
	virtual int x() = 0;
	virtual int y() = 0;

	// selection
	virtual bool selected() = 0;
	virtual void setSelected(bool newselection) = 0;

	// creation and destruction are handled via standard
	// constructors/destructors
};

#endif
