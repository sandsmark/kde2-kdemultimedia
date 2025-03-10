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

// A square wave with a programmable duty cycle. The duty cycle should
// be in the range 0..1.

class Synth_WAVE_PULSE_impl : virtual public Synth_WAVE_PULSE_skel,
							virtual public StdSynthModule
{
protected:
	float _dutycycle;
 
public:
	float dutycycle() { return _dutycycle; }

	void dutycycle(float newDutycycle) { _dutycycle = newDutycycle; }         

	void calculateBlock(unsigned long samples)
	{
		unsigned long i;
		for(i=0;i<samples;i++)
		{
			outvalue[i] = (pos[i] < _dutycycle) ? 1.0 : -1.0;
		}
	}
};

REGISTER_IMPLEMENTATION(Synth_WAVE_PULSE_impl);
