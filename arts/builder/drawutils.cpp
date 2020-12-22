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

#include "drawutils.h"

char *DrawUtils::cropText(QPainter *p, const char *text, int maxlen, int& textwidth)
{
	static char label[100];
	char label2[100];
	bool crop_underscore;
	int n,i,crop_pos = 0;

	strncpy(label, text, 99);
	textwidth = p->fontMetrics().width(label);
	n = strlen(label);
	while((textwidth > maxlen) && (n>0)){
		crop_underscore=false;
		for(i=0;i<n;i++)
		{
			if(label[i] == '_' && !crop_underscore)
			{
				crop_underscore = true;
				crop_pos=i;
			}
		}

		if(crop_underscore)
		{
			strcpy(label2,&label[crop_pos+1]);
			strcpy(label,label2);
			n = strlen(label);
		}
		else
		{
			label[n--] = 0;
		}
		textwidth = p->fontMetrics().width(label);
	}

	return(label);
}
