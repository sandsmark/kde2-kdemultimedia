/*
  here we define the signals a playerplugin can define
  Copyright (C) 1999  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */


#include <guiPlugin.h>


GuiPlugin::GuiPlugin(QWidget *parent, const char *name)
  :QWidget(parent,name) {
}


GuiPlugin::~GuiPlugin() {
}


#include "guiPlugin.moc"
