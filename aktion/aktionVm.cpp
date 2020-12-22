#include "aktionVm.h"

#ifdef XF86_VM

#include <iostream>

#include <X11/Xlib.h>
#include <X11/extensions/xf86dga.h>
#include <X11/extensions/xf86vmode.h>


aktionVm::aktionVm(QWidget *parent) : QWidget(parent)
{
}

bool aktionVm::setVideoMode(int*w,int*h)
{
   XF86VidModeModeInfo **modes;
   int nmodes;

   /* Get all available video modes */
   XF86VidModeGetAllModeLines( x11Display(), DefaultScreen(x11Display()), &nmodes, &modes);

   int i=0;
   int ratio=0;
   int winnerRatio=32000;
   XF86VidModeModeInfo *selected=0L;

   /* Save the original video mode settings */
   XF86DGAGetViewPortSize(x11Display(),DefaultScreen(x11Display()), &prevW, &prevH);

   /* Search for the closest video mode */
   for ( i=0; i<nmodes; ++i )
   {
      /* first check if the video mode fits the requested size */
      if ((modes[i]->hdisplay >= *w) && (modes[i]->vdisplay >= *h))
      {
         /* calculates a near-ratio to find the closest vm */
         ratio=(modes[i]->hdisplay - *w) + (modes[i]->vdisplay - *h);
         if (ratio < winnerRatio)
         {
            winnerRatio=ratio;
            selected=modes[i];
         }
      }
   }

   if ( selected!=0L )
   {
      if ((selected->hdisplay!=prevW) || (selected->vdisplay!=prevH))
      {
         /* Switch to the selected video mode */
         XF86VidModeSwitchToMode(x11Display(),DefaultScreen(x11Display()), selected);
         XF86VidModeSetViewPort(x11Display(),DefaultScreen(x11Display()),0,0);
      }
      /* return the selected video mode */
      *w=selected->hdisplay;
      *h=selected->vdisplay;
      return true;
   }
   else
      return false;
}

void aktionVm::resetVideoMode()
{
   XF86VidModeModeInfo **modes;
   int nmodes;

   /* Get all available video modes */
   XF86VidModeGetAllModeLines( x11Display(), DefaultScreen(x11Display()), &nmodes, &modes);

   int i=0;
   bool found=false;
   int w, h;

   XF86DGAGetViewPortSize(x11Display(),DefaultScreen(x11Display()), &w, &h);
   if ((w!=prevW) || (h!=prevH))
      while (!found && i<nmodes)
      {
         if ((modes[i]->hdisplay == prevW) && (modes[i]->vdisplay == prevH))
         {
            /* Switch to the previous video mode */
            XF86VidModeSwitchToMode(x11Display(),DefaultScreen(x11Display()), modes[i]);
            found=true;
         }
         else
            i++;
      }

}

#endif
