#ifndef PARSER_H
#define PARSER_H

#include <qstringlist.h>
#include <qfile.h>
#include <qpixmap.h>
#include <qimage.h>
#include <qdict.h>

class Parser : public QDict<QStringList>
{
	class ImagePixmap
	{
	public:
		ImagePixmap() : mImage(0), mPixmap(0) {}
		~ImagePixmap() {}
		QImage mImage;
		QPixmap mPixmap;
	};
		
public:
	Parser();
	void conserveMemory();
	void open(const QString &file);
	
	QString dir() const;
	QPixmap pixmap(const QString &i) const
		{ return getPair(i)->mPixmap; }
	QImage image(const QString &i) const
		{ return getPair(i)->mImage; }

	QString fileItem(const QString &file) const;

	bool exist(const QString &i) const;
	
public:
	QStringList& operator[](const QString &l) { return *find(l);}
	
private:
	ImagePixmap *getPair(const QString &i) const;

private:
	mutable QDict<ImagePixmap> mImageCache;
	QString mDir;	
};

#endif

