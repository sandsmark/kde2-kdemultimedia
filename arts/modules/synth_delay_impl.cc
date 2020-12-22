    /*

    Copyright (C) 2000 Jeff Tranter
                       tranter@pobox.com
                       Stefan Westerfeld
                       stefan@space.twc.de
                       Jens Hahn
                       Jens.Hahn@t-online.de

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

#include <math.h>
#include "artsmodules.h"
#include "stdsynthmodule.h"

using namespace Arts;

// This delays the input signal for an amount of time. The time
// specification must be between 0 and 1 for a delay between 0 seconds
// and 1 second.
//
// This kind of delay may not be used in feedback structures. This is
// because it's a variable delay. You can modify it's length while it
// is running, and even set it down to zero. But since in a feedback
// structure the own output is needed to calculate the next samples, a
// delay whose value could drop to zero during synthesis could lead to
// a stall situation.
//
// Use CDELAYs in that setup, perhaps combine a small constant delay
// (of 0.001 seconds) with a flexible delay.
//
// You can also combine a CDELAY and a DELAY to achieve a variable
// length delay with a minimum value in a feedback loop. Just make
// sure that you have a CDELAY involved.

class Synth_DELAY_impl : virtual public Synth_DELAY_skel,
							virtual public StdSynthModule
{
protected:
	static const int SAMPLINGRATE = 44100;
	static const int MAXDELAY = SAMPLINGRATE;
	float *dbuffer; // holds one second of past data
	int dbpos;
 
public:
	Synth_DELAY_impl()
	{
		dbuffer=new float[MAXDELAY];
	}
	~Synth_DELAY_impl()
	{
		delete [] dbuffer;
	}

	void streamInit()
	{
		// initialize buffer to all zeroes
		for (int i=0; i < MAXDELAY; i++)
			dbuffer[i] = 0.0;
		dbpos = 0;
	}

	// Code was taken mostly as is from aRts 0.3.4.1
	void calculateBlock(unsigned long samples)
	{
		for (unsigned long i = 0; i <samples; i++)
		{
			dbuffer[dbpos] = invalue[i];
			int position, position1;
			double error, int_pos;
			error = modf(time[i] * MAXDELAY, &int_pos);
			position = dbpos - (int) int_pos;
			if (position < 0)
				position += MAXDELAY;
			position1 = position - 1;
			if (position1 < 0)
				position1 += MAXDELAY;
			outvalue[i] = dbuffer[position] * (1 - error) + dbuffer[position1] * error;
			dbpos++;
			if(dbpos == MAXDELAY)
				dbpos = 0;
		}
	}
};

REGISTER_IMPLEMENTATION(Synth_DELAY_impl);
