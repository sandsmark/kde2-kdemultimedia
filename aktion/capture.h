#ifndef CAPTURE_H
#define CAPTURE_H

#include <qstring.h>

class QWidget;

/**
* Class for single image capture from a widget
*
* @short Class for sinlge image capture from a widget.
* @author Guillermo P. Marotte (g-marotte@usa.net)
* @version 0.1
*/

class capture
{
private:
   int counter;
   QString baseName;
   QString extension;
   QString format;
   QString auxName;
   QString auxNum;
public:
   /**
   *  Capture a single image. Parameters: theWidget
   */
   void captureWidget(QWidget *);
   /**
   *  Set the parameters. Parameters: theDirectory, theFilename, theFormat
   */
   void setParameters(const QString &, const QString &, const QString &);
   /**
   *  Resets the file counter
   */
   void resetCounter();
};

#endif

