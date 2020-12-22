/*
 * Copyright (C) 2000 Rik Hemsley (rikkus) <rik@kde.org>. All rights reserved.
 *
LICENSE
 */

#ifndef TRACK_H
#define TRACK_H

#include <qobject.h>
#include <qstring.h>
#include <qdatastream.h>
#include <qlist.h>
#include <qdatetime.h>

class PlaylistItem;

class Track : public QObject
{
  Q_OBJECT

  public:

    Track();

    Track(
        const QString & artist,
        const QString & album,
        const QString & name,
        const QString & filename,
        long length
    );

    Track(const Track & t);

    Track & operator = (const Track & t);

    bool operator < (const Track & t) const;

    bool operator <= (const Track & t) const
    { return (operator < (t) || operator == (t)); }

    bool operator > (const Track & t) const
    { return ! operator < (t); }

    bool operator == (const Track &) const;

    QString artist()    const { return artist_;     }
    QString album()     const { return album_;      }
    QString name()      const { return name_;       }
    QString filename()  const { return filename_;   }
    long    length()    const { return length_;     }
    int     score()     const { return score_;      }
    int     fileScore() const { return fileScore_;  }
    QString key()       const { return key_;        }

    PlaylistItem * lisztItem();

    QString lengthAsString() const { return lengthAsString_; }

    void    setArtist   (QString s) { artist_ = s;    }
    void    setAlbum    (QString s) { album_ = s;     }
    void    setName     (QString s) { name_ = s;      }
    void    setFilename (QString s) { filename_ = s;  }
    void    setLength   (long i)    { length_ = i;    }
    void    setScore    (int i);
    void    setFileScore(int i);

    friend QDataStream & operator << (QDataStream &, const Track &);
    friend QDataStream & operator >> (QDataStream & str, Track &);

  signals:

    void scoreChanged(int);

  private:

    void _recalculateKey();
    void _recalculateLengthAsString();
    void _createLisztItem();

    QString artist_;
    QString album_;
    QString name_;
    QString filename_;
    long    length_;
    int     score_;
    int     fileScore_;
    QString key_;
    QString lengthAsString_;

    PlaylistItem * lisztItem_;
};

class TrackList : public QList<Track>
{
  public:

    TrackList();
    virtual ~TrackList();

  protected:

    int compareItems(QCollection::Item, QCollection::Item);
};

typedef QListIterator<Track> TrackListIterator;

#endif // TRACK_H
