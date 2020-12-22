    /*

    Copyright (C) 2000,2001 Stefan Westerfeld
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

#include "kwidget_impl.h"
#include <qvbox.h>

namespace Arts {

class KVBox_impl : virtual public Arts::VBox_skel,
				   virtual public Arts::KWidget_impl
{
protected:
	QVBox *qhbox;
public:
	KVBox_impl(QVBox *qhbox = 0);
};

};
