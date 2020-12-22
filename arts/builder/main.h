	/*

	Copyright (C) 1998-1999 Stefan Westerfeld
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

#ifndef __MAIN_H__
#define __MAIN_H__

#include "config.h"
#include "structure.h"
#include "scomponent.h"
#include "menumaker.h"

#include <list>

#include <kmainwindow.h>

#include <kapp.h>
#include <kfiledialog.h>
#include <ksimpleconfig.h>
#include <qlistbox.h>
#include <qtablevw.h>
#include <qpainter.h>
#include <qdrawutl.h>
#include <qwidget.h>
#include <qlayout.h>
#include <qframe.h>
#include <qpixmap.h>
#include <string.h>
#include "module.h"
#include "autorouter.h"
#include "portpropdlg.h"
#include "execdlg.h"
#include "namedlg.h"
#include "retrievedlg.h"
#include "createtool.h"

class CreateTool;
class CreateModuleTool;
class CreatePortTool;

class ModuleWidget :public QTableView, public StructureCanvas
{
	Q_OBJECT
	friend class CreateTool;
	friend class CreateModuleTool;
	friend class CreatePortTool;

protected:
	int cellsize,cols,rows;
	bool mouseButtonDown;
	QPoint movestart;
	bool inUpdate;

	CreateTool *activeTool;
	AutoRouter *autorouter;
	Structure *structure;

// for connecting ports

	ModulePort *lastport;
	int selectedports;

	//list<StructureComponent *> ChildList;
	std::list<Module *> *ModuleList;
	std::list<QRect> UpdateList;

	void paintCell(QPainter *p, int y, int x);
	void paintCellBackground(QPainter *p, int y, int x);
	void paintConnection(QPainter *p, int x, int y, int arx, int ary);
	void paintConnections(QPainter *p, int y, int x);
	void mousePressEvent( QMouseEvent *e );
	void mouseMoveEvent( QMouseEvent *e );
	void mouseReleaseEvent( QMouseEvent *e );

	bool hasSpace(StructureComponent *c,int destx,int desty,
											bool ignore_selected=false);
	bool connectPorts(ModulePort *src, ModulePort *dest);


	void reRoute();
	void setSelectAll(bool newstate);

	void beginUpdate();
	void redrawModule(Module *m);
	void redrawCells(QRect &r);
	void redrawAll();
	void endUpdate();

// for StructureComponents:

	void redrawRect(int x, int y, int width, int height);

// for CreateTool:

	bool insertModule(Module *newModule);
	void insertPort(const Arts::PortType& type, int x, int y);

public:
	Structure *theStructure();

	void addModule(const Arts::ModuleInfo& minfo);
	void addPort(const Arts::PortType& type);
	void reInit();
	void leaveTool(CreateTool *tool);

	ModuleWidget(Structure *structure, QWidget *parent=0,
								const char *name=0, WFlags f=0);

	~ModuleWidget();

	ModulePort *findPort(Module *m, int xoffset, int direction);
public slots:
	void autoRedrawRouter();
	void setZoom(int zoom);
	void delModule();
	void selectAll();
	void unselectAll();
};

class ModuleView :public KMainWindow
{
	Q_OBJECT
private:
    ModuleWidget *modulewidget;
	int modules_menu_item;
	KMenuBar *menubar;
	QPopupMenu *m_modules, *m_file, *m_view, *m_edit, *m_ports, *m_file_new;
	MenuMaker *menumaker;
	static const int id_save = 1;
	/*
	QPopupMenu *m_modules_synth,*m_modules_gui,*m_modules_instruments,
			   *m_modules_other;
	*/

	long mbroker_updateCount;
	//list<Module *> *ModuleList;
	Structure *structure;

	ExecDlg *execDlg;
	bool modified;
	QString m_filename;

public:
	ModuleView(const char *name);
	~ModuleView();
	void clear();
	void checkName();
	QString getOpenFilename(const char *pattern, const char *initialDir = 0);
	bool isModified();
	void setModified(bool m = true);
	bool promptToSave();
	bool queryClose();

public slots:
	void addPort(int port);
	void fileNew(int what);
	void addModule(int module);
	void changePortPositions();
	void activateMenu(int item);
	void save();
	void saveAs();
	void open();
	void open(QString file);
	void openSession();
	void openExample();
	void execute();
	void endexecute();
	void rename();
	void retrieve();
	void publish();
	void quit();
};
#endif
