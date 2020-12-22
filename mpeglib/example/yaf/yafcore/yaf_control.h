 

#ifndef __YAF_CONTROL_H
#define __YAF_CONTROL_H



#include "inputInterface.h"
#include "outputInterface.h"
#include <iostream.h>
#include "inputDecoder.h"

#include <sys/time.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>



// prototypes

//extern "C" void control_xplayer();
void yaf_control(InputInterface* input,
		 OutputInterface* output,
		 InputDecoder* decoder);

#endif
