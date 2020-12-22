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

#include "midiclient_impl.h"
#include "midimanager_impl.h"
#include "midimanagerport_impl.h"

using namespace Arts;
using namespace std;

MidiClient_impl::MidiClient_impl(const MidiClientInfo& info,
	MidiManager_impl *manager) :_info(info), manager(manager)
{
}

MidiClient_impl::~MidiClient_impl()
{
	while(!_info.connections.empty())
		disconnect(manager->findClient(_info.connections[0]));

	manager->removeClient(this);
}

MidiClientInfo MidiClient_impl::info()
{
	return _info;
}

void MidiClient_impl::title(const string &newvalue)
{
	_info.title = newvalue;
}

string MidiClient_impl::title()
{
	return _info.title;
}

void MidiClient_impl::addInputPort(MidiPort port)
{
	assert(_info.direction == mcdRecord);

	ports.push_back(port);
}

MidiPort MidiClient_impl::addOutputPort()
{
	assert(_info.direction == mcdPlay);

	MidiPort port = MidiPort::_from_base(new MidiManagerPort_impl(this));
	ports.push_back(port);

	return port;
}

void MidiClient_impl::removePort(MidiPort port)
{
}

// FIXME: this is currently being called every time a client "just" wants to
// get a time, which is probably not the most performant way to do this ;)

list<MidiPort> *MidiClient_impl::connections()
{
	_connections.clear();

	vector<long>::iterator li;
	for(li = _info.connections.begin(); li != _info.connections.end(); li++)
	{
		MidiClient_impl *other = manager->findClient(*li);
		assert(other);
		
		list<MidiPort>::iterator pi;
		for(pi = other->ports.begin(); pi != other->ports.end(); pi++)
		{
			_connections.push_back(*pi);
		}
	}
	return &_connections;
}

static void removeElement(vector<long>& vec, long el)
{
	vector<long> tmp;
	vec.swap(tmp);
	vector<long>::iterator i;
	for(i = tmp.begin(); i != tmp.end(); i++)
		if(*i != el) vec.push_back(*i);
}

void MidiClient_impl::connect(MidiClient_impl *dest)
{
	assert(_info.direction != dest->_info.direction);

	disconnect(dest);

	_info.connections.push_back(dest->ID());
	dest->_info.connections.push_back(ID());
}

void MidiClient_impl::disconnect(MidiClient_impl *dest)
{
	assert(_info.direction != dest->_info.direction);

	removeElement(_info.connections,dest->ID());
	removeElement(dest->_info.connections,ID());
}
