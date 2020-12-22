    /*

    Copyright (C) 1999 Stefan Westerfeld
                       stefan@space.twc.de

    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

    */

#include <vector>
#include <string>

int parse_line(const char *in, char *& cmd, char *& param);

void sqprintf(std::vector<std::string> *list, const char *fmt, ...);
void addSubStringSeq(std::vector<std::string> *target, const std::vector<std::string> *source);
void appendStringSeq(std::vector<std::string> *target, const std::vector<std::string> *source);
int parse_line(const std::string& in, std::string& cmd, std::string& param);
std::vector<std::string> *getSubStringSeq(const std::vector<std::string> *seq,unsigned long& i);
