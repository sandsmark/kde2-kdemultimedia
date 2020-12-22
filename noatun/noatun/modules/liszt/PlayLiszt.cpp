/*
 * Copyright (C) 2000 Rik Hemsley (rikkus) <rik@kde.org>. All rights reserved.
 *
LICENSE
 */

#include <qapplication.h>
#include <qstring.h>
#include <qdir.h>
#include <qfile.h>
#include <qfileinfo.h>
#include <qtl.h>

#include <klocale.h>

#include <noatunplaylist.h>

#include "PlayLiszt.h"

static Track * createTrackVorbis(const QString & fn)
{
#ifdef I_AM_A_WINDOWS_USER
  if (fn.right(3) != "ogg") {
    qDebug("File does not have 'ogg' extension");
    return 0;
  }
#endif

  QFile f(fn);

  if (!f.open(IO_ReadOnly))
    return 0;

  const int checkblock = 16384;
  QByteArray a(checkblock);

  if (checkblock != f.readBlock(a.data(), checkblock))
    return 0;

  if (a[0] != 'O' || a[1] != 'g' || a[2] != 'g')
    return 0;

  QString artist, album, title;

  for (int i = 10; i < checkblock - 10; i++) {

    if (a[i] == 'A' || a[i] == 'a') {

      // Now, see if you can make _this_ faster ;)
      if (a[i + 5] != 'T' && a[i + 5] != 't') {
        i += 5;
        continue;
      }

    if (QString::fromUtf8(a.data() + i, 6).lower() == "artist")
      artist = QString::fromUtf8(a.data() + i + 7, int(a[i - 4]) - 7);

    }
  }

  for (int i = 10; i < checkblock - 10; i++) {

    if (a[i] == 'A' || a[i] == 'a') {

      if (a[i + 4] != 'M' && a[i + 4] != 'm') {
        i += 4;
        continue;
      }

      if (QString::fromUtf8(a.data() + i, 5).lower() == "album")
        album = QString::fromUtf8(a.data() + i + 6, int(a[i - 4]) - 6);
    }
  }

  for (int i = 10; i < checkblock - 10; i++) {

    if (a[i] == 'T' || a[i] == 't') {

      if (a[i + 4] != 'E' && a[i + 4] != 'e') {
        i += 4;
        continue;
      }

      if (QString::fromUtf8(a.data() + i, 5).lower() == "title")
        title = QString::fromUtf8(a.data() + i + 6, int(a[i - 4]) - 6);
    }
  }

  if (!artist)
    artist = i18n("Unknown");

  if (!album)
    album = i18n("Unknown");

  if (!title)
    title = fn;

  return new Track(artist, album, title, fn, 0);
}

// --------------------------------------------------------------------------
// MP3 info reading
// --------------------------------------------------------------------------

static Track * createTrackMP3(const QString & fn)
{
#ifdef I_AM_A_WINDOWS_USER
  if (fn.right(3) != "mp3") {
    qDebug("File does not have 'mp3' extension");
    return 0;
  }
#endif

  QFile f(fn);

  if (!f.open(IO_ReadOnly))
    return 0;

  char check[3];

  if (3 != f.readBlock(check, 3))
    return 0;

  bool normalMpeg1Layer3Tag = check[0] == (char)0xff || check[1] & 0xf2;
  bool id3Tag = check[0] == 'I' || check[1] == 'D' || check[2] == '3';

  if (id3Tag && !normalMpeg1Layer3Tag)
    return 0;

  if (!normalMpeg1Layer3Tag)
    return 0;

  f.at(f.size() - 128);

  QByteArray buf(128);

  if (128 != f.readBlock(buf.data(), 128))
    return 0;

  if (buf[0] != 'T' || buf[1] != 'A' || buf[2] != 'G')
    return 0;

  QString artist, album, title;

  title = QString::fromLatin1(buf.data() + 3, 30);
  artist = QString::fromLatin1(buf.data() + 33, 30);
  album = QString::fromLatin1(buf.data() + 63, 30);
  return new Track(artist, album, title, fn, 0);
}

// --------------------------------------------------------------------------

const Q_UINT32 playlistVersion = 2;

PlayLiszt * PlayLiszt::instance_ = 0L;

PlayLiszt::PlayLiszt()
  : QObject(),
    currentTrack_(0)
{
}

PlayLiszt::~PlayLiszt()
{
  trackList_.setAutoDelete(true);
  trackList_.clear();
}

  void
PlayLiszt::_update()
{
  filteredTrackList_.clear();

  for (TrackListIterator it(trackList_); it.current(); ++it)
    if (it.current()->score() > 0)
      filteredTrackList_.append(it.current());

  filteredTrackList_.sort();

  emit(changed());
}

  void
PlayLiszt::clear()
{
  trackList_.clear();
  filteredTrackList_.clear();
  emit(changed());
}

  long
PlayLiszt::count() const
{
  return filteredTrackList_.count();
}

  long
PlayLiszt::lastTrack() const
{
  return filteredTrackList_.count() - 1;
}

  Track *
PlayLiszt::track(long trackNumber)
{
  return filteredTrackList_.at(trackNumber);
}

  TrackList &
PlayLiszt::filtered()
{
  return filteredTrackList_;
}

  TrackList &
PlayLiszt::all()
{
  return trackList_;
}

  void
PlayLiszt::load()
{
  QString path = QDir::homeDirPath() + "/.charlatan/playlist";

  QFile f(path);

  if (!f.open(IO_ReadOnly)) { // That's ok - it's just not been made yet.
    if (f.exists()) // That's not ok.
      qWarning("Could not open playlist. Wrong permissions ?");
    emit(loaded());
    return;
  }

  QDataStream d(&f);

  int savedPlayLisztVersion;

  d >> savedPlayLisztVersion;

  if (unsigned(savedPlayLisztVersion) != playlistVersion) {
    qWarning("Saved playlist version doesn't match my version");
    emit(loaded());
    return;
  }


  while (!d.atEnd()) {
    Track * t = new Track;
    d >> *t;
    trackList_.append(t);
  }

  emit(trackListAdded(trackList_));

  emit(loaded());
  _update();
}

  void
PlayLiszt::save()
{
  QDir dir(QDir::homeDirPath() + "/.charlatan");

  if (!dir.exists())
    if (!dir.mkdir(QDir::homeDirPath() + "/.charlatan")) {
      qWarning("Could not make user's charlatan dir");
      return;
    }

  QString path = QDir::homeDirPath() + "/.charlatan/playlist";

  QFile f(path);

  if (!f.open(IO_WriteOnly)) {
    qWarning("Could not open playlist for writing.");
    return;
  }

  QDataStream d(&f);

  d << playlistVersion;

  for (TrackListIterator it(trackList_); it.current(); ++it)
    d << *it.current();

  f.flush();
  f.close();
}

  void
PlayLiszt::addFile(const QString & s)
{
  _addFile(s);
  save();
}

  void
PlayLiszt::addFileList(const QStringList & l)
{
  for (QStringList::ConstIterator it(l.begin()); it != l.end(); ++it)
    _addFile(*it);
  save();
}

  void
PlayLiszt::addFileList(const QString & root, const QStringList & l)
{
  for (QStringList::ConstIterator it(l.begin()); it != l.end(); ++it)
    if ((*it)[0] != '.')
      _addFile(root + "/" + *it);
}

  void
PlayLiszt::_addFile(const QString & s)
{
  if (s[0] == '.')
    return;

  QString filename(s);

  filename.replace(QRegExp("^file:"), "");

  if (QFileInfo(filename).isDir())
    return addFileList(filename, QDir(filename).entryList());

  Track * t = createTrackVorbis(filename);

  if (0 == t)
    t = createTrackMP3(filename);

  qApp->processEvents();

  if (0 == t)
    return;
  else
    slotNewTrack(t);
}

  void
PlayLiszt::increaseScore(Track * t)
{
  t->setScore(t->score() + 1);
  _update();
}

  void
PlayLiszt::decreaseScore(Track * t)
{
  if (t->score() != 0)
    t->setScore(t->score() - 1);
  _update();
}

  void
PlayLiszt::slotNewTrack(Track * t)
{
  for (TrackListIterator it(trackList_); it.current(); ++it) {
    if (it.current() == t)
      return;
  }

  t->setFileScore(trackList_.count());
  trackList_.append(t);

  emit(trackAdded(t));
}

  void
PlayLiszt::setCurrentTrack(long l)
{
  currentTrack_ = l;
  emit(currentTrackChanged(l));
}

#include "PlayLiszt.moc"
