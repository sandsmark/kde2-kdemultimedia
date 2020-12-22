/*
  mapping from file and id to pixmap, and global player map
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */



#ifndef __WASKINMAPPING_H
#define __WASKINMAPPING_H


/**
   This file is not a header file in the normal sense.
   It define _directly_ variables, which are used in the 
   WaSkinLoader class.
   Its a bit black voodoo magic.
*/

/**
   The Skin file format was downloaded from the web.
   http://www.algonet.et.se/~daniel7

   Author: unknown
*/
#include <skinMap.h>
#include <waSkinDesc.h>

// here are the defines:
#include <waSkins.h>


static SkinMap globalMapToGui[]= {
  {_WA_MAPPING_MAIN            ,0         ,0 ,275,116},
  {_WA_MAPPING_CBUTTONS_PREV   ,16        ,88,23 ,16},
  {_WA_MAPPING_CBUTTONS_PLAY   ,16+1*23   ,88,23 ,16},
  {_WA_MAPPING_CBUTTONS_PAUSE  ,16+2*23   ,88,23 ,16},
  {_WA_MAPPING_CBUTTONS_STOP   ,16+3*23   ,88,23 ,16},
  {_WA_MAPPING_CBUTTONS_NEXT   ,16+4*23   ,88,23 ,16},
  {_WA_MAPPING_CBUTTONS_EJECT  ,16+5*23   ,88,22 ,16},
  {_WA_MAPPING_MONOSTER_STEREO ,239       ,41,29 ,12},
  {_WA_MAPPING_MONOSTER_MONO   ,212       ,41,27 ,12},
  {_WA_MAPPING_REPEAT          ,210       ,89,28 ,15},
  {_WA_MAPPING_SHUFFLE         ,164       ,88,46 ,15},
  {_WA_MAPPING_PLAYLIST        ,242       ,58,23 ,12},
  {_WA_MAPPING_EQ              ,219       ,58,23 ,12},
  {_WA_MAPPING_VOLUME_BAR      ,107       ,57,68 ,12},
  {_WA_MAPPING_VOLUME_SLIDER   ,0         ,0 ,14 ,11},
  {_WA_MAPPING_BALANCE_BAR     ,177       ,57,38 ,12},
  {_WA_MAPPING_BALANCE_SLIDER  ,0         ,0 ,14 ,11},
  {_WA_MAPPING_PLAYPAUS        ,24        ,28,9  , 9},
  {_WA_MAPPING_POS_BAR         ,16        ,72,248,10},
  {_WA_MAPPING_POS_BAR_SLIDER  ,0         ,0 , 29,10},
  {_WA_MAPPING_MINUS           ,40        ,32,5  , 1},
  {_WA_MAPPING_DIGIT_1         ,48        ,26,9  ,13},
  {_WA_MAPPING_DIGIT_2         ,60        ,26,9  ,13},
  {_WA_MAPPING_DIGIT_3         ,78        ,26,9  ,13},
  {_WA_MAPPING_DIGIT_4         ,90        ,26,9  ,13},
  {_WA_MAPPING_ANALYSER        ,24        ,43,76 ,16},
  {_WA_MAPPING_BPS             ,111       ,43,15 , 6},
  {_WA_MAPPING_FREQ            ,156       ,43,10 , 6},
  {_WA_MAPPING_INFO            ,112       ,27,152, 6},
  {_WA_MAPPING_TITLE               ,0, 0, 274, 15},
  {_WA_MAPPING_TITLE_SHADE               ,255, 3, 8, 8}
};


static SkinDesc globalMapFromFile[] = {
  {"main.bmp",_WA_SKIN_MAIN,                              0,  0,275,116},
  {"cbuttons.bmp", _WA_SKIN_CBUTTONS_PREV_NORM,           0,  0, 23,18},
  {"cbuttons.bmp", _WA_SKIN_CBUTTONS_PREV_PRES,           0, 18, 23,18},
  {"cbuttons.bmp", _WA_SKIN_CBUTTONS_PLAY_NORM,          23,  0, 23,18},
  {"cbuttons.bmp", _WA_SKIN_CBUTTONS_PLAY_PRES,          23, 18, 23,18},
  {"cbuttons.bmp", _WA_SKIN_CBUTTONS_PAUSE_NORM,         46,  0, 23,18},
  {"cbuttons.bmp", _WA_SKIN_CBUTTONS_PAUSE_PRES,         46, 18, 23,18},
  {"cbuttons.bmp", _WA_SKIN_CBUTTONS_STOP_NORM,          69,  0, 23,18},
  {"cbuttons.bmp", _WA_SKIN_CBUTTONS_STOP_PRES,          69, 18, 23,18},
  {"cbuttons.bmp", _WA_SKIN_CBUTTONS_NEXT_NORM,          92,  0, 22,18},
  {"cbuttons.bmp", _WA_SKIN_CBUTTONS_NEXT_PRES,          92, 18, 22,18},
  {"cbuttons.bmp", _WA_SKIN_CBUTTONS_EJECT_NORM,        114,  0, 22,16},
  {"cbuttons.bmp", _WA_SKIN_CBUTTONS_EJECT_PRESS,       114, 16, 22,16},
  {"monoster.bmp", _WA_SKIN_MONOSTER_STEREO_TRUE,         0,  0, 29,12},
  {"monoster.bmp", _WA_SKIN_MONOSTER_STEREO_FALSE,        0, 12, 29,12},
  {"monoster.bmp", _WA_SKIN_MONOSTER_MONO_TRUE,          29,  0, 27,12},
  {"monoster.bmp", _WA_SKIN_MONOSTER_MONO_FALSE,         29, 12, 27,12},
  {"numbers.bmp", _WA_SKIN_NUMBERS,                       0,  0, 99,13},
  {"numbers.bmp", _WA_SKIN_NUMBERS_MINUS,                 20, 6,  5,1},
  {"shufrep.bmp", _WA_SKIN_SHUFREP_REPEAT_NOT_SET_NORM,   0,  0, 28,15},
  {"shufrep.bmp", _WA_SKIN_SHUFREP_REPEAT_NOT_SET_PRES,   0, 15, 28,15},
  {"shufrep.bmp", _WA_SKIN_SHUFREP_REPEAT_SET_NORM,       0, 30, 28,15},
  {"shufrep.bmp", _WA_SKIN_SHUFREP_REPEAT_SET_PRES,       0, 45, 28,15},
  {"shufrep.bmp", _WA_SKIN_SHUFREP_SHUFFLE_NOT_SET_NORM, 28,  0, 47,15},
  {"shufrep.bmp", _WA_SKIN_SHUFREP_SHUFFLE_NOT_SET_PRES, 28, 15, 47,15},
  {"shufrep.bmp", _WA_SKIN_SHUFREP_SHUFFLE_SET_NORM,     28, 30, 47,15},
  {"shufrep.bmp", _WA_SKIN_SHUFREP_SHUFFLE_SET_PRES,     28, 45, 47,15},
  {"shufrep.bmp", _WA_SKIN_SHUFREP_PLAYLIST_NOT_SET_NORM,23, 61, 23,12},
  {"shufrep.bmp", _WA_SKIN_SHUFREP_PLAYLIST_NOT_SET_PRES,69, 61, 23,12},
  {"shufrep.bmp", _WA_SKIN_SHUFREP_PLAYLIST_SET_NORM,    23, 73, 23,12},
  {"shufrep.bmp", _WA_SKIN_SHUFREP_PLAYLIST_SET_PRES,    69, 73, 23,12},
  {"shufrep.bmp", _WA_SKIN_SHUFREP_EQ_NOT_SET_NORM,      0,  61, 23,12},
  {"shufrep.bmp", _WA_SKIN_SHUFREP_EQ_NOT_SET_PRES,      46, 61, 23,12},
  {"shufrep.bmp", _WA_SKIN_SHUFREP_EQ_SET_NORM,           0, 73, 23,12},
  {"shufrep.bmp", _WA_SKIN_SHUFREP_EQ_SET_PRES,          46, 73, 23,12},
  {"text.bmp"   , _WA_SKIN_TEXT,                           0, 0,155,18},
  {"volume.bmp", _WA_SKIN_VOLUME_BAR_ALL_BARS,            0, 0, 68,421},
  {"volume.bmp", _WA_SKIN_VOLUME_BAR,                     0, 0, 68,13},
  {"volume.bmp", _WA_SKIN_VOLUME_SLIDER_NORM,             0,422, 14,11},
  {"volume.bmp", _WA_SKIN_VOLUME_SLIDER_PRES,            15,422, 14,11},
  {"volume.bmp", _WA_SKIN_BALANCE_BAR,                     0, 0, 38,13},
  {"volume.bmp", _WA_SKIN_BALANCE_SLIDER_NORM,             0,422, 14,11},
  {"volume.bmp", _WA_SKIN_BALANCE_SLIDER_PRES,            15,422, 14,11},
  {"posbar.bmp", _WA_SKIN_POS_BAR,                        0,  0,248,10},
  {"posbar.bmp", _WA_SKIN_POS_BAR_SLIDER_NORM,          248,  0, 29,10},
  {"posbar.bmp", _WA_SKIN_POS_BAR_SLIDER_PRES,          278,  0, 29,10},
  {"playpaus.bmp", _WA_SKIN_PLAYPAUS_PLAY,                0,  0,  9,9},
  {"playpaus.bmp", _WA_SKIN_PLAYPAUS_PAUSE,               9,  0,  9,9},
  {"playpaus.bmp", _WA_SKIN_PLAYPAUS_STOP,               18,  0,  9,9},
  {"titlebar.bmp", _WA_SKIN_TITLE,               27, 0, 274, 15},
  {"titlebar.bmp", _WA_SKIN_TITLE_SHADE_PRES,               9, 18, 8, 8},
  {"titlebar.bmp", _WA_SKIN_TITLE_SHADE_NORM,               0, 18, 8, 8}
};


static int bmpToGui[][2]=  {
  {_WA_SKIN_MAIN,_WA_MAPPING_MAIN},
  {_WA_SKIN_CBUTTONS_PREV_NORM,_WA_MAPPING_CBUTTONS_PREV},
  {_WA_SKIN_CBUTTONS_PREV_PRES,_WA_MAPPING_CBUTTONS_PREV},
  {_WA_SKIN_CBUTTONS_PLAY_NORM,_WA_MAPPING_CBUTTONS_PLAY},
  {_WA_SKIN_CBUTTONS_PLAY_PRES,_WA_MAPPING_CBUTTONS_PLAY},
  {_WA_SKIN_CBUTTONS_PAUSE_NORM,_WA_MAPPING_CBUTTONS_PAUSE},
  {_WA_SKIN_CBUTTONS_PAUSE_PRES,_WA_MAPPING_CBUTTONS_PAUSE},
  {_WA_SKIN_CBUTTONS_STOP_NORM,_WA_MAPPING_CBUTTONS_STOP},
  {_WA_SKIN_CBUTTONS_STOP_PRES,_WA_MAPPING_CBUTTONS_STOP},
  {_WA_SKIN_CBUTTONS_NEXT_NORM,_WA_MAPPING_CBUTTONS_NEXT},
  {_WA_SKIN_CBUTTONS_NEXT_PRES,_WA_MAPPING_CBUTTONS_NEXT},
  {_WA_SKIN_CBUTTONS_EJECT_NORM,_WA_MAPPING_CBUTTONS_EJECT},
  {_WA_SKIN_CBUTTONS_EJECT_PRESS,_WA_MAPPING_CBUTTONS_EJECT},
  {_WA_SKIN_MONOSTER_STEREO_TRUE,_WA_MAPPING_MONOSTER_STEREO},
  {_WA_SKIN_MONOSTER_STEREO_FALSE,_WA_MAPPING_MONOSTER_STEREO},
  {_WA_SKIN_MONOSTER_MONO_TRUE,_WA_MAPPING_MONOSTER_MONO},
  {_WA_SKIN_MONOSTER_MONO_FALSE,_WA_MAPPING_MONOSTER_MONO},
  {_WA_SKIN_NUMBERS,-1},
  {_WA_SKIN_NUMBERS_MINUS,_WA_MAPPING_MINUS},
  {_WA_SKIN_SHUFREP_REPEAT_NOT_SET_NORM,_WA_MAPPING_REPEAT},
  {_WA_SKIN_SHUFREP_REPEAT_NOT_SET_PRES,_WA_MAPPING_REPEAT},
  {_WA_SKIN_SHUFREP_REPEAT_SET_NORM,_WA_MAPPING_REPEAT},
  {_WA_SKIN_SHUFREP_REPEAT_SET_PRES,_WA_MAPPING_REPEAT},
  {_WA_SKIN_SHUFREP_SHUFFLE_NOT_SET_NORM,_WA_MAPPING_SHUFFLE},
  {_WA_SKIN_SHUFREP_SHUFFLE_NOT_SET_PRES,_WA_MAPPING_SHUFFLE},
  {_WA_SKIN_SHUFREP_SHUFFLE_SET_NORM,_WA_MAPPING_SHUFFLE},
  {_WA_SKIN_SHUFREP_SHUFFLE_SET_PRES,_WA_MAPPING_SHUFFLE},
  {_WA_SKIN_SHUFREP_PLAYLIST_NOT_SET_NORM,_WA_MAPPING_PLAYLIST},
  {_WA_SKIN_SHUFREP_PLAYLIST_NOT_SET_PRES,_WA_MAPPING_PLAYLIST},
  {_WA_SKIN_SHUFREP_PLAYLIST_SET_NORM,_WA_MAPPING_PLAYLIST},
  {_WA_SKIN_SHUFREP_PLAYLIST_SET_PRES,_WA_MAPPING_PLAYLIST},
  {_WA_SKIN_SHUFREP_EQ_NOT_SET_NORM,_WA_MAPPING_EQ},
  {_WA_SKIN_SHUFREP_EQ_NOT_SET_PRES,_WA_MAPPING_EQ},
  {_WA_SKIN_SHUFREP_EQ_SET_NORM,_WA_MAPPING_EQ},
  {_WA_SKIN_SHUFREP_EQ_SET_PRES,_WA_MAPPING_EQ},
  {_WA_SKIN_TEXT,-1},
  {_WA_SKIN_VOLUME_BAR_ALL_BARS,-1},
  {_WA_SKIN_VOLUME_BAR,_WA_MAPPING_VOLUME_BAR},
  {_WA_SKIN_VOLUME_SLIDER_NORM,-1},
  {_WA_SKIN_VOLUME_SLIDER_PRES,-1},
  {_WA_SKIN_BALANCE_BAR,_WA_MAPPING_BALANCE_BAR},
  {_WA_SKIN_BALANCE_SLIDER_NORM,-1},
  {_WA_SKIN_BALANCE_SLIDER_PRES,-1},
  {_WA_SKIN_POS_BAR,_WA_MAPPING_POS_BAR},
  {_WA_SKIN_POS_BAR_SLIDER_NORM,-1},
  {_WA_SKIN_POS_BAR_SLIDER_PRES,-1},
  {_WA_SKIN_PLAYPAUS_PLAY,_WA_MAPPING_PLAYPAUS},
  {_WA_SKIN_PLAYPAUS_PAUSE,_WA_MAPPING_PLAYPAUS},
  {_WA_SKIN_PLAYPAUS_STOP,_WA_MAPPING_PLAYPAUS},
  {_WA_SKIN_TITLE,_WA_MAPPING_TITLE},
  {_WA_SKIN_TITLE_SHADE_NORM,_WA_MAPPING_TITLE_SHADE},
  {_WA_SKIN_TITLE_SHADE_PRES,_WA_MAPPING_TITLE_SHADE}
};


#endif

