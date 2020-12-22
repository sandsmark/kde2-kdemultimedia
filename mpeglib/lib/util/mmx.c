/*
  wrapper for MMX calls
  Copyright (C) 2000  Martin Vogt

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU Library General Public License as published by
  the Free Software Foundation.

  For more information look at the file COPYRIGHT in this package

 */



#include "mmx.h"

static int mmSupport=-1;

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif


#ifndef INTEL
int emms() {
  printf("urgs! emms() never should happen\n");
  exit(0);
}
  
int mm_support() {
  return 0;
}

#endif


#ifdef INTEL

extern unsigned int cpu_flags(void);

int emms() {
  __asm__  ("emms");
  return 1;
}

#define MMX_CPU              0x800000

int mm_support() {

  int val;
  int* rval = &val;

  if (mmSupport == -1) {

    val=cpu_flags(); 
    if (val & MMX_CPU) {
      mmSupport=1;
    } else {
      mmSupport=0;
    }
    
  }
  /* Return */
  return(mmSupport);
}





#endif
