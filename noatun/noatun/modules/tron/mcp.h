// mcp.h
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

#ifndef MCP_H
#define MCP_H

#include <kio/global.h>
#include <qarray.h>
#include <qdom.h>

#include "noatunplaylist.h"

namespace KIO
{
class Job;
}

class MCPItem;

class MCP : public Playlist
{
Q_OBJECT

public:
	MCP(QObject *parent, const char *name = 0);
	virtual ~MCP();

	virtual void reset(void);
	virtual void clear(void);

	void play(PlaylistItem *);
	virtual PlaylistItem *next(void);
	virtual PlaylistItem *current(void);
	virtual PlaylistItem *previous(void);

	virtual PlaylistItem *getFirst(void) const;
	virtual PlaylistItem *getAfter(const PlaylistItem *) const;
	PlaylistItem *getBefore(const PlaylistItem *) const;

	void moveAfter(PlaylistItem *item, PlaylistItem *afterThisOne);

	virtual void addFile(const KURL &, bool = false);
	PlaylistItem *addFile(const KURL &, PlaylistItem *);
	PlaylistItem *addDirectory(const KURL & , PlaylistItem * = 0);
	PlaylistItem *importWindowsPlaylist(const KURL &, PlaylistItem * = 0);
	PlaylistItem *importPlaylist(const KURL &, PlaylistItem * = 0);

	virtual bool listVisible(void) const;

	void setShuffle(bool);
	void setApplyVolume(bool);

	/**
	 * Internal
	 *
	 * To be used by MCPItem only
	 */
	void itemModified(PlaylistItem *);

public slots:
	virtual void showList(void);
	virtual void hideList(void);
	virtual void toggleList(void);
	virtual void remove(PlaylistItem *);

	void load(const KURL &, bool errorFree = false);
	void save(const KURL &);

signals:
	void cleared(void);
	void loaded(void);
	void saved(void);
	void modified(PlaylistItem *);
	void removed(PlaylistItem *);
	void moved(PlaylistItem *item, PlaylistItem *afterThis);

	/**
	 * This is emitted when the current playlist item is set or changed
	 */
	void current(PlaylistItem *);

private slots:
	void setVolume(PlaylistItem *);
	void getVolume(int);

	void listFinished(KIO::Job *);
	void listEntries(KIO::Job *, const KIO::UDSEntryList &);
private:
	PlaylistItem *listLastAdded;
	bool listDone;

	static const MCPItem *AtBottom;

	void clearInternal(void);
	MCPItem *addFileG(const KURL &_url, bool _play = false, MCPItem *_afterThis = const_cast<MCPItem *>(AtBottom));

	QDomDocument doc;
	QDomElement cur;

	bool shuffle;
	QList<MCPItem> shuffleUnused;

	bool applyVolume;
};

#endif
