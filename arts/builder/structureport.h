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

#ifndef __EXTPORT_H__
#define __EXTPORT_H__
#include "scomponent.h"
#include "module.h"

class StructurePort :public StructureComponent {
protected:
	Arts::StructurePortDesc SPortDesc;
	Arts::StructureDesc StructureDesc;
	ModulePort *_port;
	bool _selected, _visible;
	int _x,_y;
public:
	StructurePort(Arts::StructurePortDesc SPortDesc,
    			Arts::StructureDesc StructureDesc, StructureCanvas *canvas);
	~StructurePort();

	//--

	void raisePosition();
	void lowerPosition();
	void rename(const char *newname);
	long id();
	long position();
	ModulePort *port();

	// Component Interface
	ComponentType type();

	ModulePort *portAt(int segment, int x, int y);
	void dumpPorts(std::list<ModulePort *>& ports);

	bool visible();
	void show();
	void hide();

	bool drawNeedsBackground(int segment);
	void drawSegment(QPainter *dest, int cellsize, int segment);
	QPixmap *pixmap();
	const char *name();

	bool selected();
	void setSelected(bool newselection);

	// in cells
	bool move(int x, int y);
	int width();
	int height();
	int x();
	int y();
};
#endif
