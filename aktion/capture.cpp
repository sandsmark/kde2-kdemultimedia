#include "capture.h"

#include <qpixmap.h>
#include <qwidget.h>
#include <qfileinfo.h>

#include <kmessagebox.h>
#include <klocale.h>

void capture::captureWidget(QWidget *base)
{
   QPixmap *pixmap=new QPixmap(base->width(), base->height());

   bitBlt(pixmap,0,0,base,0,0,base->width(), base->height(),Qt::CopyROP);
   auxNum.setNum(counter);
   auxName=baseName+auxNum+extension;
   if(!pixmap->save(auxName,format.ascii()))
   {
      QString s;
      s = i18n("Can't save image to file:/n%1").arg(auxName);
      KMessageBox::sorry(0L, s);
   }
   counter++;
   delete pixmap;
}

void capture::setParameters(const QString & dir, const QString & name, const QString & fmt)
{
   QFileInfo fi(name);

   baseName=dir;
   if (baseName.right(1) != "/")
      baseName += "/";
   baseName+=fi.baseName();
   format=fmt;
   extension="."+format.lower();
}

void capture::resetCounter()
{
   counter=0;
}

