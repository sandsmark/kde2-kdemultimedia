/*
 * Copyright (C) 2000 Rik Hemsley (rikkus) <rik@kde.org>. All rights reserved.
 *
LICENSE
 */

#include <qbitmap.h>
#include "pixmaps.h"
#include "Static.h"

Static * Static::instance_ = 0L;

Static::Static()
{
  pix_["close"]     = QPixmap((const char **)(px_close));
  pix_["playlist"]  = QPixmap((const char **)(px_playlist));
  pix_["album"]     = QPixmap((const char **)(px_album));
  pix_["artist"]    = QPixmap((const char **)(px_artist));
  pix_["track"]     = QPixmap((const char **)(px_track));
  pix_["next"]      = QPixmap((const char **)(px_next));
  pix_["stop"]      = QPixmap((const char **)(px_stop));
  pix_["previous"]  = QPixmap((const char **)(px_previous));
  pix_["play"]      = QPixmap((const char **)(px_play));
  pix_["pause"]     = QPixmap((const char **)(px_pause));
  pix_["x"]         = QPixmap((const char **)(px_x));
  pix_["plus"]      = QPixmap((const char **)(px_plus));
  pix_["minus"]     = QPixmap((const char **)(px_minus));
  pix_["config"]    = QPixmap((const char **)(px_config));
}

