// mcp.cpp
//
// Copyright (C) 2000, 2001 Neil Stevens <multivac@fcmail.com>
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
// THE AUTHOR(S) BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
// AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
// 
// Except as contained in this notice, the name(s) of the author(s) shall not be
// used in advertising or otherwise to promote the sale, use or other dealings
// in this Software without prior written authorization from the author(s).

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

extern "C"
{
#include <sys/stat.h>
#include <unistd.h>
}

#include <kdebug.h>
#include <kio/global.h>
#include <kio/job.h>
#include <kio/netaccess.h>
#include <klocale.h>
#include <kmessagebox.h>
#include <krandomsequence.h>
#include <ktempfile.h>
#include <qfile.h>
#include <qtextstream.h>

#include "ksaver.h"
#include "mcp.h"
#include "noatunapp.h"
#include "player.h"

/**
 * class MCPItem is closely coupled with class MCP.  It makes sure that,
 * at all times, every PlaylistItem maintains a one-to-one relationship
 * with a QDomElement from MCP.
 *
 * Additionally, it makes sure that the QDomElement is kept updated with
 * the PlaylistItem information.
 */
class MCPItem : public PlaylistItem
{
public:
	/**
	 * Create a new item, and create an element for it
	 */
	MCPItem(QDomDocument, const KURL & = 0, bool = true);

	/**
	 * Create an item for an existing element, as from a playlist file
	 */
	MCPItem(QDomElement);

	virtual ~MCPItem();

	inline QDomElement element(void) const {return node;};
	static inline KURL getURL(const QString &path);

protected:
	virtual void modified(void);

private:
	void setPointer(void);

	void update(void);

	QDomElement node;
};

MCPItem::MCPItem(QDomElement n)
	: PlaylistItem(getURL(n.attribute("path")), n.attribute("download") == "true")
{
	node = n;
	setProperty("title", node.attribute("title"), false);
	setProperty("artist", node.attribute("artist"), false);
	setProperty("album", node.attribute("album"), false);
	setProperty("date", node.attribute("date"), false);
	setProperty("comment", node.attribute("comment"), false);
	mLength = node.attribute("length").toInt();
	// Keep compatible, despite new ms system
	if(mLength != -1) mLength *= 1000;
	setPointer();
}

MCPItem::MCPItem(QDomDocument d, const KURL &url, bool download)
	: PlaylistItem(url, download)
{
	node = d.createElement("item");
	setPointer();
	update();
}

MCPItem::~MCPItem()
{
	node.removeAttribute("MCPItem");
}

void MCPItem::modified(void)
{
	update();
	static_cast<MCP *>(napp->playlist())->itemModified(this);
}

void MCPItem::setPointer(void)
{
	node.setAttribute("MCPItem", QString::number((long)this));
}

void MCPItem::update(void)
{
	node.setAttribute("title", property("title"));
	node.setAttribute("artist", property("artist"));
	node.setAttribute("album", property("album"));
	node.setAttribute("date", property("date"));
	node.setAttribute("comment", property("comment"));
	node.setAttribute("path", mUrl.path());
	node.setAttribute("download", mDownloaded ? "true" : "false");
	// Keep compatible, despite new ms system
	int length = mLength;
	if(length != -1) length /= 1000;
	node.setAttribute("length", QString::number(length));
}

KURL MCPItem::getURL(const QString &path)
{
	KURL result(path);
	if (result.isMalformed() || result.protocol() == "file")
	{
		result.setProtocol("file");
		result.setPath(path);
	}
	return result;
}

inline MCPItem *getItemForElement(QDomElement element)
{
	return (MCPItem *)element.attribute("MCPItem").toLong();
}

/////////
// MCP //
/////////

MCP::MCP(QObject *parent, const char *name)
	: Playlist(parent, name)
	, shuffle(false)
	, applyVolume(false)
{
	clear();
	shuffleUnused.setAutoDelete(false);
	connect(this, SIGNAL(current(PlaylistItem *)), this, SLOT(setVolume(PlaylistItem *)));

	// I just checked.  napp->player() should exist at this point.
	connect(napp->player(), SIGNAL(volumeChanged(int)), this, SLOT(getVolume(int)));
}

MCP::~MCP()
{
	// Only You can help prevent memory leaks
	clearInternal();
}

void MCP::reset(void)
{
	if(shuffle)
	{
		setShuffle(true);
	}
	else
	{
		cur = doc.documentElement().namedItem("item").toElement();
		emit current(getItemForElement(cur));
	}
}

void MCP::clear(void)
{
	clearInternal();
	emit cleared();
}

void MCP::clearInternal(void)
{
	QDomElement element = doc.documentElement().namedItem("item").toElement();

	while(!element.isNull())
	{
		MCPItem *item = getItemForElement(element);
		delete item;
		element = element.nextSibling().toElement();
	}

	doc.clear();
	doc.setContent(QString("<!DOCTYPE NoatunPlaylist><playlist/>"));

	if(napp->player())
		napp->player()->stop();

	if(shuffle)
		shuffleUnused.clear();
}

const MCPItem *MCP::AtBottom = (MCPItem *)-1;

MCPItem *MCP::addFileG(const KURL &_url, bool _play, MCPItem *_afterThis)
{
	MCPItem *item = new MCPItem(doc, _url);

	if(_afterThis == AtBottom)
		doc.documentElement().appendChild(item->element());
	else
		moveAfter(item, _afterThis);

	if(_play) play(item);

	if(shuffle)
	{
		shuffleUnused.append(item);
		KRandomSequence rand;
		rand.randomize(&shuffleUnused);
	}

	return item;
}

void MCP::addFile(const KURL &_url, bool _play)
{
	(void)addFileG(_url, _play);
}

PlaylistItem *MCP::addFile(const KURL &_url, PlaylistItem *_afterThis)
{
	return addFileG(_url, false, static_cast<MCPItem *>(_afterThis));
}

PlaylistItem *MCP::next(void)
{
	if(shuffle)
	{
		PlaylistItem *item = shuffleUnused.take();
		if(!item)
		{
			setShuffle(true);
			item = shuffleUnused.take();
		}

		if(item)
			cur = static_cast<MCPItem *>(item)->element();
		else
			cur = QDomElement();

		return item;
	}
	else
	{
		QDomElement element = cur.nextSibling().toElement();
		if(element.isNull())
		{
			return 0;
		}
		else
		{
			cur = element;
			emit current(getItemForElement(cur));
			return getItemForElement(cur);
		}
	}
}

PlaylistItem *MCP::current(void)
{
	return getItemForElement(cur);
}

PlaylistItem *MCP::previous(void)
{
	if(shuffle)
		return next();

	QDomElement element = cur.previousSibling().toElement();
	if(element.isNull())
	{
		return 0;
	}
	else
	{
		cur = element;
		emit current(getItemForElement(cur));
		return getItemForElement(cur);
	}
}

PlaylistItem *MCP::getFirst(void) const
{
	return getItemForElement(doc.documentElement().namedItem("item").toElement());
}

PlaylistItem *MCP::getAfter(const PlaylistItem *_item) const
{
	const MCPItem *item = static_cast<const MCPItem *>(_item);
	QDomElement element = item->element();
	element = element.nextSibling().toElement();
	return getItemForElement(element);
}

PlaylistItem *MCP::getBefore(const PlaylistItem *_item) const
{
	const MCPItem *item = static_cast<const MCPItem *>(_item);
	QDomElement element = item->element();
	element = element.previousSibling().toElement();
	return getItemForElement(element);
}

bool MCP::listVisible(void) const
{
	return !static_cast<QWidget *>(parent())->isHidden();
}

void MCP::setShuffle(bool b)
{
	shuffle = b;
	if(shuffle)
	{
		shuffleUnused.clear();

		QDomElement element = doc.documentElement().namedItem("item").toElement();
		while(!element.isNull())
		{
			shuffleUnused.append(getItemForElement(element));
			element = element.nextSibling().toElement();
		}
		KRandomSequence rand;
		rand.randomize(&shuffleUnused);
	}
}

void MCP::setApplyVolume(bool b)
{
	applyVolume = b;
	if(!cur.isNull()) setVolume(getItemForElement(cur));
}

void MCP::moveAfter(PlaylistItem *_item, PlaylistItem *_moveAfter)
{
	MCPItem *item = static_cast<MCPItem *>(_item);
	QDomElement element = item->element();

	MCPItem *moveAfter = static_cast<MCPItem *>(_moveAfter);
	if(moveAfter)
	{
		QDomElement after = moveAfter->element();
		after.parentNode().insertAfter(element, after);
		emit moved(_item, _moveAfter);
	}
	else
	{
		QDomNode parent = doc.documentElement();
		parent.insertBefore(element, parent.firstChild());
		emit moved(_item, 0);
	}
}

void MCP::play(PlaylistItem *_item)
{
	MCPItem *item = static_cast<MCPItem *>(_item);
	QDomElement element = item->element();

	cur = element;
	emit current(item);
	emit playCurrent();
}

void MCP::itemModified(PlaylistItem *item)
{
	emit modified(item);
}

void MCP::showList(void)
{
	if(!listVisible()) static_cast<QWidget *>(parent())->show();
	emit listShown();
}

void MCP::hideList(void)
{
	if(listVisible()) static_cast<QWidget *>(parent())->hide();
	emit listHidden();
}

void MCP::toggleList(void)
{
	QWidget *widget = static_cast<QWidget *>(parent());
	if(widget->isHidden())
	{
		widget->show();
		emit listShown();
	}
	else
	{
		widget->hide();
		emit listHidden();
	}
}

void MCP::remove(PlaylistItem *_item)
{
	MCPItem *item = static_cast<MCPItem *>(_item);
	QDomElement element = item->element();

	if(shuffle)
	{
		shuffleUnused.removeRef(item);
	}

	if(element == cur)
	{
		if(cur.nextSibling().isNull())
			napp->player()->stop();
		else
			previous();
	}

	delete item;
	element.parentNode().removeChild(element);
	element.clear();

	emit removed(item);
}

static inline void MCPError(QObject *obj, QString err)
{
	KMessageBox::error(static_cast<QWidget *>(obj), err);
}

void MCP::load(const KURL &url, bool errorFree)
{
	QString dest;
	if(!KIO::NetAccess::download(url, dest))
	{
		if(!errorFree)
			MCPError(this, i18n("Failed to load %1.").arg(url.prettyURL()));
	}
	else
	{
		QFile file(dest);
		file.open(IO_ReadOnly);
		QTextStream stream(&file);
		QString data = stream.read();

		QDomDocument newDoc;
		if(!newDoc.setContent(data))
		{
			MCPError(this, i18n("%1 is not a valid Noatun playlist.").arg(url.prettyURL()));
		}
		else
		{
			clearInternal();
			doc.setContent(data);
			QDomElement element = doc.documentElement().namedItem("item").toElement();
			while(!element.isNull())
			{
				(void)new MCPItem(element);
				element = element.nextSibling().toElement();
			}
			emit loaded();
			setShuffle(shuffle);
		}
		KIO::NetAccess::removeTempFile(dest);
	}
}

void MCP::save(const KURL &url)
{
	KSaver saver(url);
	if(!saver.open())
	{
		MCPError(this, i18n("Failed to save to %1.").arg(url.prettyURL()));
		return;
	}

	saver.textStream() << doc.toString();

	if(!saver.close())
	{
		MCPError(this, i18n("Failed to save to %1.").arg(url.prettyURL()));
		return;
	}

	emit saved();
}

PlaylistItem *MCP::importWindowsPlaylist(const KURL &url, PlaylistItem *_item)
{
	MCPItem *item = static_cast<MCPItem *>(_item);

	QString dest;
	if(!KIO::NetAccess::download(url, dest))
	{
		MCPError(this, i18n("Failed to load %1.").arg(url.prettyURL()));
	}
	else
	{
		QFile file(dest);
		file.open(IO_ReadOnly);
		QTextStream stream(&file);
		QString data = stream.readLine();
		while(!data.isNull())
		{
			// happy Moose feature
			if(KURL::isRelativeURL(data)) data = url.path(1) + data;
			item = addFileG( MCPItem::getURL(data), false, item);
			data = stream.readLine();
		}
		KIO::NetAccess::removeTempFile(dest);
	}

	return item;
}

PlaylistItem *MCP::importPlaylist(const KURL &url, PlaylistItem *_item)
{
	MCPItem *item = static_cast<MCPItem *>(_item);

	QString dest;
	if(!KIO::NetAccess::download(url, dest))
	{
		MCPError(this, i18n("Failed to load %1.").arg(url.prettyURL()));
	}
	else
	{
		QFile file(dest);
		file.open(IO_ReadOnly);
		QTextStream stream(&file);
		QString data = stream.read();

		QDomDocument list;
		list.setContent(data);

		QDomElement cur = list.documentElement().namedItem("item").toElement();

		while(!cur.isNull())
		{
			QDomElement element = doc.createElement("item");
			QDomNamedNodeMap attrs = cur.attributes();

			for(unsigned i = 0; i < attrs.length(); i++)
			{
				QDomAttr attr = attrs.item(i).toAttr();
				element.setAttribute( attr.name(), attr.value() );
			}
				
			MCPItem *item = new MCPItem(element);
			moveAfter(item, _item);
			emit modified(item);
			_item = item;

			cur = cur.nextSibling().toElement();
		}
			
		KIO::NetAccess::removeTempFile(dest);
	}

	return item;
}

PlaylistItem *MCP::addDirectory(const KURL &_url, PlaylistItem *_item)
{
	listLastAdded = _item;
	listDone = false;

	KIO::ListJob *job = KIO::listRecursive(_url, false);
	connect(job, SIGNAL(result(KIO::Job *)), this, SLOT(listFinished(KIO::Job *)));
	connect(job, SIGNAL(entries(KIO::Job *, const KIO::UDSEntryList &)), this, SLOT(listEntries(KIO::Job *, const KIO::UDSEntryList &)));

	while(!listDone) napp->processEvents();

	return listLastAdded;
}

void MCP::listFinished(KIO::Job *)
{
	listDone = true;
}

void MCP::listEntries(KIO::Job *job, const KIO::UDSEntryList &list)
{
	for(KIO::UDSEntryListConstIterator i = list.begin(); i != list.end(); ++i)
	{
		QValueList<KIO::UDSAtom>::ConstIterator j;
		for(j = (*i).begin(); (*j).m_uds != KIO::UDS_FILE_TYPE && j != (*i).end(); ++j);

		if( j != (*i).end() && S_ISREG((*j).m_long) )
		{
			for(j = (*i).begin(); (*j).m_uds != KIO::UDS_NAME && j != (*i).end(); ++j);
			if(j != (*i).end())
			{
				KURL url = static_cast<KIO::ListJob *>(job)->url();
				url.addPath((*j).m_str);
				listLastAdded = addFile(url, listLastAdded);
			}
		}
	}
}

void MCP::setVolume(PlaylistItem *_item)
{
	if(!applyVolume) return;

	MCPItem *item = static_cast<MCPItem *>(_item);
	QDomElement node = item->element();
	if(node.hasAttribute("volume"))
		napp->player()->setVolume(node.attribute("volume").toInt());
	else
		getVolume(napp->player()->volume());
}

void MCP::getVolume(int _volume)
{
	cur.setAttribute("volume", QString::number(_volume));
}

#include "mcp.moc"
