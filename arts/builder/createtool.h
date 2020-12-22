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

#ifndef _CREATETOOL_H
#define _CREATETOOL_H

#include <qwidget.h>
#include <qcursor.h>
#include "main.h"

class ModuleWidget;

class CreateTool
{
protected:
	QCursor oldCursor;
	bool oldMouseTracking;
	QRect moduleRect;
	ModuleWidget *mwidget;
	int width, height;

public:
	CreateTool(ModuleWidget *mwidget);
	virtual ~CreateTool();

	virtual void estimateSize() = 0;
	virtual void performCreate(int x, int y) = 0;

	void mousePressEvent(QMouseEvent *e);
	void mouseMoveEvent(QMouseEvent *e);
	void mouseReleaseEvent(QMouseEvent *e);
};

class CreateModuleTool :public CreateTool
{
protected:
	Arts::ModuleInfo minfo;

public:
	CreateModuleTool(ModuleWidget *mwidget, const Arts::ModuleInfo& minfo);
	~CreateModuleTool();

	void estimateSize();
	void performCreate(int x, int y);
};

class CreatePortTool :public CreateTool
{
protected:
	Arts::PortType type;

public:
	CreatePortTool(ModuleWidget *widget, const Arts::PortType& type);

	void estimateSize();
	void performCreate(int x, int y);
};

#endif
