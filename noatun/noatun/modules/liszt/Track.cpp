/*
 * Copyright (C) 2000 Rik Hemsley (rikkus) <rik@kde.org>. All rights reserved.
 *
LICENSE
 */

#include <qfile.h>
#include <noatunplaylist.h>
#include "Track.h"

Track::Track()
  : QObject     (),
    length_     (0L),
    score_      (0),
    fileScore_  (0),
    lisztItem_  (0)
{
}

Track::Track(
    const QString & artist,
    const QString & album,
    const QString & name,
    const QString & filename,
    long            length
)
  : QObject       (),
    artist_       (artist),
    album_        (album),
    name_         (name),
    filename_     (filename),
    length_       (length),
    score_        (0),
    fileScore_    (0),
    lisztItem_    (0)
{
  _recalculateLengthAsString();
  _recalculateKey();
  _createLisztItem();
}

Track::Track(const Track & t)
  : QObject         (),
    artist_         (t.artist_),
    album_          (t.album_),
    name_           (t.name_),
    filename_       (t.filename_),
    length_         (t.length_),
    score_          (t.score_),
    fileScore_      (t.fileScore_),
    key_            (t.key_),
    lengthAsString_ (t.lengthAsString_),
    lisztItem_      (t.lisztItem_)
{
}

Track & Track::operator = (const Track & t)
{
  if (this == &t) return *this;

  artist_         = t.artist_;
  album_          = t.album_;
  name_           = t.name_;
  filename_       = t.filename_;
  length_         = t.length_;
  score_          = t.score_;
  fileScore_      = t.fileScore_;
  key_            = t.key_;
  lengthAsString_ = t.lengthAsString_;
  lisztItem_      = t.lisztItem_;

  return *this;
}

  bool
Track::operator == (const Track & t) const
{
  return (filename_ == t.filename_);
}

QDataStream & operator << (QDataStream & str, const Track & t)
{
  str
    << t.artist_
    << t.album_
    << t.name_
    << t.filename_
    << t.length_
    << t.score_
    << t.fileScore_;

  return str;
}

QDataStream & operator >> (QDataStream & str, Track & t)
{
  str
    >> t.artist_
    >> t.album_
    >> t.name_
    >> t.filename_
    >> t.length_
    >> t.score_
    >> t.fileScore_;

  t._recalculateLengthAsString();
  t._recalculateKey();
  t._createLisztItem();

  return str;
}

  bool
Track::operator < (const Track & t) const
{
  return key_ < t.key_;

}

  void
Track::_recalculateKey()
{
  key_.sprintf("%032d%032d", score_, 0xffffffff - fileScore_);
}

  void
Track::_recalculateLengthAsString()
{
  int m = length_ / 60;
  int s = length_ - m * 60;

  lengthAsString_ = QString::number(m) + ':';

  if (s < 10) lengthAsString_ += "0";
  lengthAsString_ += QString::number(s);
}

  void
Track::setScore(int i)
{
  score_ = i;
  _recalculateKey();
  emit(scoreChanged(score_));
}

  void
Track::setFileScore(int i)
{
  fileScore_ = i;
  _recalculateKey();
  emit(scoreChanged(score_));
}

  void
Track::_createLisztItem()
{
  if (0 == lisztItem_)
    lisztItem_ = new PlaylistItem(KURL("file:" + filename_), false);

  lisztItem_->setTitle(artist_ + " - " + name_);
  lisztItem_->setLength(length_);
}

  PlaylistItem *
Track::lisztItem()
{
  return lisztItem_;
}

TrackList::TrackList()
  : QList<Track>()
{
  // Empty.
}

TrackList::~TrackList()
{
  // Empty.
}

  int
TrackList::compareItems(QCollection::Item item1, QCollection::Item item2)
{
  return     static_cast<Track *>(item1)->key()
    .compare(static_cast<Track *>(item2)->key());
}

#include "Track.moc"
