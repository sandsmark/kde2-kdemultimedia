#include <noatunapp.h>
#include <noatunplaylist.h>
#include <qstring.h>
#include <qfile.h>
#include <qdatastream.h>
#include <iostream>
#include <kdebug.h>

#include "l33tFileInfo.h"

l33tFileInfo::l33tFileInfo(QString fileName)
{
        QFile f( fileName );
	cerr << fileName.latin1();
        f.open( IO_ReadOnly );
        QDataStream s( &f );
	s.readRawBytes(c, 4);
        f.close();
}

int l33tFileInfo::getBps()
{
        int bps = 0;
	int result;
        getbits(c[0], 0, 7, &result);
	int result2;
	getbits(c[0], 0, 3, &result2);
	cout << "\nHola, como estas?\n" << result<< "\nAdios amigo\n";
        if (result == 11111111)
	{
		switch(result2) {
		        case 0000: {
			        bps = 0;
		        }
			case 0001: {
			        bps = 32;
			}
			case 0010: {
			        bps = 40;
			}
			case 0011: {
			        bps = 48;
			}
			case 0100: {
			        bps =  56;
			}
			case 0101: {
			        bps =  64;
                        }
			case 0110: {
			        bps =  80;
			}
			case 0111: {
			        bps =  96;
			}
			case 1000: {
                                bps =  112;
			}
			case 1001: {
			        bps =  128;
			}
			case 1010: {
			        bps =  160;
			}
			case 1011: {
			        bps =  192;
			}
			case 1100: {
			        bps =  224;
			}
			case 1101: {
			        bps =  256;
			}
			case 1110: {
			        bps =  320;
			}
			case 1111: {
			        bps =  0;
			}
		}
	}
	return bps;
}

int l33tFileInfo::getKHz()
{

}

QString l33tFileInfo::getChannelMode()
{

}

int l33tFileInfo::getbits(unsigned char x, int p, int n, int *result)
{
	for (int i=n; i > p;)
	{
	        for (int i2 = p; i2 <= n; i2++)
		{
                        result[i2]=(x >> i) & 1;
			i--;
		}
	}
}
