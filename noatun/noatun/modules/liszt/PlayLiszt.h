/*
 * Copyright (C) 2000 Rik Hemsley (rikkus) <rik@kde.org>. All rights reserved.
 *
LICENSE
 */

#ifndef PLAY_LIST_H
#define PLAY_LIST_H

#include <qobject.h>
#include <qstringlist.h>

#include "Track.h"

class PlaylistItem;

class PlayLiszt : public QObject
{
  Q_OBJECT

  public:

    static PlayLiszt * instance()
    {
      if (0 == instance_)
        instance_ = new PlayLiszt;

      return instance_;
    }

    ~PlayLiszt();

    Track * track(long trackNumber);
    TrackList & filtered();
    TrackList & all();

    long count() const;
    long lastTrack() const;

    void load();
    void save();

    void addFileList(const QString & root, const QStringList &);
    void addFileList(const QStringList &);
    void addFile(const QString &);

    void increaseScore(Track *);
    void decreaseScore(Track *);

    void clear();

    long currentTrack() const { return currentTrack_; }
    void setCurrentTrack(long);

  signals:

    void changed();
    void loaded();
    void trackAdded(Track *);
    void trackListAdded(TrackList &);
    void currentTrackChanged(long);

  protected slots:

    void slotNewTrack(Track *);

  private:

    void _addFile(const QString &);
    static PlayLiszt * instance_;

    PlayLiszt();
    void _update();

    TrackList filteredTrackList_;
    TrackList trackList_;
    long currentTrack_;
};

#endif
