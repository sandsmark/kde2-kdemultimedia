#include "parser.h"
#include <kurl.h>
#include <qtextstream.h>
#include <qimage.h>
#include <noatunapp.h>
#include "kjprefs.h"

Parser::Parser()
{
	mImageCache.setAutoDelete(true);
	setAutoDelete(true);
}

void Parser::conserveMemory()
{
	mImageCache.clear();
}

void Parser::open(const QString &file)
{
	clear();
	mImageCache.clear();
	mDir=KURL(file).directory();
	QFile f(file);
	f.open(IO_ReadOnly);

	f.at(0);
	QTextStream stream(&f);
	while (!stream.eof())
	{
		QString line=stream.readLine();
		line=line.simplifyWhiteSpace();
		if ((!line.length()) || line[0]=='#')
			continue;
		QStringList *l=new QStringList(QStringList::split(" ", line));
		QString first=l->first();	
		insert(first, l);	
	}
}

QString Parser::fileItem(const QString &i) const
{
	return dir()+'/'+i;
}

QString Parser::dir() const
{
	return mDir;
}

Parser::ImagePixmap* Parser::getPair(const QString &filenameOld) const
{
	// is it in there?
	ImagePixmap *pair;
	{
		pair=mImageCache.find(filenameOld);
		if (pair)
			return pair;
	}
	
	QString filename=fileItem(filenameOld);

	QImage image;

	if (filename.right(4).lower()!=".png")
		image=QImage(filenameNoCase(filename));
	else
		image=NoatunApp::readPNG(filenameNoCase(filename));

	//add to the cache
	QPixmap pixmap;
	pixmap.convertFromImage(image, QPixmap::AutoColor|QPixmap::ThresholdDither|QPixmap::AvoidDither);
	pair=new Parser::ImagePixmap;
	pair->mImage=image;
	pair->mPixmap=pixmap;
	mImageCache.insert(filenameOld, pair);
	return pair;
}

bool Parser::exist(const QString &i) const
{
	return (bool)find(i);
}


