    /*

    Copyright (C) 2001 Stefan Westerfeld
                       stefan@space.twc.de

    This library is free software; you can redistribute it and/or
    modify it under the terms of the GNU Library General Public
    License as published by the Free Software Foundation; either
    version 2 of the License, or (at your option) any later version.
  
    This library is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
    Library General Public License for more details.
   
    You should have received a copy of the GNU Library General Public License
    along with this library; see the file COPYING.LIB.  If not, write to
    the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
    Boston, MA 02111-1307, USA.

    */

#include "artsmodules.h"
#include "debug.h"
#include "connect.h"

using namespace std;
using namespace Arts;

namespace Arts {

class FreeverbGuiFactory_impl : public FreeverbGuiFactory_skel
{
public:
	Widget createGui(Object freeverb);
};

REGISTER_IMPLEMENTATION(FreeverbGuiFactory_impl);

};

Widget FreeverbGuiFactory_impl::createGui(Object object)
{
	arts_return_val_if_fail(!object.isNull(), Arts::Widget::null());

	Synth_FREEVERB freeverb = DynamicCast(object);
	arts_return_val_if_fail(!freeverb.isNull(), Arts::Widget::null());

	Widget panel;
	panel.width(330); panel.height(80); panel.show();

	Poti roomsize;
	roomsize.x(20); roomsize.y(10); roomsize.text("roomsize");
	roomsize.color("red"); roomsize.min(0); roomsize.max(1);
	roomsize.value(freeverb.roomsize());
	roomsize.parent(panel);
	roomsize.show();
	connect(roomsize,"value_changed", freeverb, "roomsize");
	panel._addChild(roomsize,"roomsizeWidget");

	Poti damp;
	damp.x(80); damp.y(10); damp.text("damp");
	damp.color("red"); damp.min(0); damp.max(1);
	damp.value(freeverb.damp());
	damp.parent(panel);
	damp.show();
	connect(damp,"value_changed", freeverb, "damp");
	panel._addChild(damp,"dampWidget");

	Poti wet;
	wet.x(140); wet.y(10); wet.text("wet");
	wet.color("red"); wet.min(0); wet.max(1);
	wet.value(freeverb.wet());
	wet.parent(panel);
	wet.show();
	connect(wet,"value_changed", freeverb, "wet");
	panel._addChild(wet,"wetWidget");

	Poti dry;
	dry.x(200); dry.y(10); dry.text("dry");
	dry.color("red"); dry.min(0); dry.max(1);
	dry.value(freeverb.dry());
	dry.parent(panel);
	dry.show();
	connect(dry,"value_changed", freeverb, "dry");
	panel._addChild(dry,"dryWidget");

	Poti width;
	width.x(260); width.y(10); width.text("width");
	width.color("red"); width.min(0); width.max(1);
	width.value(freeverb.width());
	width.parent(panel);
	width.show();
	connect(width,"value_changed", freeverb, "width");
	panel._addChild(width,"widthWidget");

	return panel;
}
