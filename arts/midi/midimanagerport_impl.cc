    /*

    Copyright (C) 2000 Stefan Westerfeld
                       stefan@space.twc.de

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

#include "midimanagerport_impl.h"
#include "midimanager_impl.h"
#include "midiclient_impl.h"
#include "stdio.h"

using namespace Arts;
using namespace std;

MidiManagerPort_impl::MidiManagerPort_impl(MidiClient_impl *client)
	: client(client)
{
}
TimeStamp MidiManagerPort_impl::time()
{
	/* TODO: timing synchronized for more than one port */
	list<MidiPort> *connections = client->connections();
	if(connections->size() >= 1)
		return connections->front().time();
	else
	{
		// basically, we should always return timer.time(), and only
		// somehow map between "our time" and "the time a client port may
		// have"
		return timer.time();
	}
}

void MidiManagerPort_impl::processCommand(const MidiCommand& command)
{
	list<MidiPort> *connections = client->connections();
	list<MidiPort>::iterator i;

	for(i = connections->begin(); i != connections->end(); i++)
		i->processCommand(command);
}

void MidiManagerPort_impl::processEvent(const MidiEvent& event)
{
	list<MidiPort> *connections = client->connections();
	list<MidiPort>::iterator i;

	/* time synchronization missing */
	for(i = connections->begin(); i != connections->end(); i++)
		i->processEvent(event);
}
