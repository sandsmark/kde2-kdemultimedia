    /*

    Copyright (C) 2000 Jeff Tranter
                       tranter@pobox.com

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

#include "artsmodules.h"
#include "stdsynthmodule.h"

using namespace Arts;

// This delays the input signal for an amount of time. The time
// specification must be between 0 and 1 for a delay between 0 and 1
// second. The delay is constant during the calculation, that means it
// can't be modified. This saves computing time as no interpolation is
// done, and is useful for recursive structures. See the description
// for Synth_DELAY.

class Synth_CDELAY_impl : virtual public Synth_CDELAY_skel,
							virtual public StdSynthModule
{
protected:
	float _time;
	static const int SAMPLINGRATE = 44100;
	static const int MAXDELAY = SAMPLINGRATE;
	float *dbuffer; // holds one second of past data
 
public:
	Synth_CDELAY_impl()
	{
		dbuffer=new float[MAXDELAY];
	}
	~Synth_CDELAY_impl()
	{
		delete [] dbuffer;
	}
	
	float time() { return _time; }

	void time(float newTime) { _time = newTime; }         

	void streamInit()
	{
		// initialize buffer to all zeroes
		for (int i=0; i < MAXDELAY; i++)
			dbuffer[i] = 0.0;
	}

	void calculateBlock(unsigned long samples)
	{
		// make sure delay is in range
		if (_time < 0.0) _time = 0.0;
		if (_time > 1.0) _time = 1.0;

		// calculate delay in samples
		unsigned long delay = (unsigned long) (_time * SAMPLINGRATE);

// Calculating output data
//
// 0                 MAXDELAY 0         samples
// +------------------------+ +---------+
// |        dbuffer         | | invalue |
// +------------------------+ +---------+
//                    <-delay->
//                    +----------+
//                    | outvalue |
//                    +----------+
//                    0         samples

		for (unsigned long i=0; i<samples; i++)
		{
			if (i >= delay)
				outvalue[i] = invalue[i - delay];
			else
				outvalue[i] = dbuffer[MAXDELAY - delay + i];
		}

// Updating delay buffer
//
// 0                 MAXDELAY 0         samples
// +------------------------+ +---------+
// |        dbuffer         | | invalue |
// +------------------------+ +---------+
//             +------------------------+
//             |        dbuffer         |
//             +------------------------+
//             0                      MAXDELAY

		// shift dbuffer
		for (unsigned long i=0; i < MAXDELAY - samples; i++)
		{
			dbuffer[i] = dbuffer[i + samples];
		}

		// copy input buffer to the end of it
		for (unsigned long i=0; i<samples; i++)
		{
			dbuffer[MAXDELAY - samples + i] = invalue[i];
		}
	}
};

REGISTER_IMPLEMENTATION(Synth_CDELAY_impl);
