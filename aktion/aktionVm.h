#ifndef AKTIONVM_H
#define AKTIONVM_H

#include "config.h"

#ifdef XF86_VM

#include <qwidget.h>

/**
* Class for VMode and DGA interaction
*
* @short Class for VMode and DGA interaction.
* @author Guillermo P. Marotte (g-marotte@usa.net)
* @version 0.1
*/

class aktionVm : public QWidget
{
private:
   int prevW;
   int prevH;
public:
   aktionVm(QWidget *);
   /**
   * This function tries to set the requested video mode
   * Parameters: width and height
   * Returns   : width and height of the closest video mode available
                 TRUE if the mode is available
                 FALSE if the mode is too big
   */
   bool setVideoMode(int*,int*);
   /**
   * Returns to the previous video mode.
   */
   void resetVideoMode();
};

#endif // XF86_VM
#endif // AKTIONVM_H

