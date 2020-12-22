#include <kcmodule.h>

#include "Liszt.h"
#include "PlayLiszt.h"
#include "EditWidget.h"

extern "C"
{
	Plugin * create_plugin()
	{
		return new Liszt;
	}
}

Liszt::Liszt()
  : Playlist(0, "Liszt"),
    Plugin()
{
}

  void
Liszt::init()
{
  editWidget_ = new EditWidget;

  connect(
      editWidget_,  SIGNAL(skipToTrack(long)),
      this,         SLOT(slotSkipToTrack(long)));

  connect(
      editWidget_,  SIGNAL(closed()),
      this,         SIGNAL(listHidden()));

  editWidget_->show();

  PlayLiszt::instance()->load();
}

Liszt::~Liszt()
{
  delete editWidget_;
}

  void
Liszt::reset()
{
  PlayLiszt::instance()->setCurrentTrack(0);
  editWidget_->reset();
}

  void
Liszt::clear()
{
  PlayLiszt::instance()->clear();
}

  void
Liszt::addFile(const KURL & url, bool)
{
  if (url.isLocalFile())
    PlayLiszt::instance()->addFile(url.path());
}

  PlaylistItem *
Liszt::next()
{
  long currentTrack = PlayLiszt::instance()->currentTrack();

  if (
      (currentTrack < 0) ||
      (currentTrack == PlayLiszt::instance()->lastTrack())
  )
    currentTrack = 0;
  else
    ++currentTrack;

  Track * t = PlayLiszt::instance()->track(currentTrack);

  if (0 == t)
    return 0;

  PlayLiszt::instance()->setCurrentTrack(currentTrack);

  return t->lisztItem();

}

  PlaylistItem *
Liszt::current()
{
  long currentTrack = PlayLiszt::instance()->currentTrack();

  if (
      (currentTrack < 0) ||
      (currentTrack > PlayLiszt::instance()->lastTrack())
  )
    currentTrack = 0;

  Track * t = PlayLiszt::instance()->track(currentTrack);

  if (0 == t)
    return 0;

  PlayLiszt::instance()->setCurrentTrack(currentTrack);

  return t->lisztItem();
}

  PlaylistItem *
Liszt::previous()
{
  long currentTrack = PlayLiszt::instance()->currentTrack();

  if (currentTrack <= 0)
    currentTrack = PlayLiszt::instance()->lastTrack();
  else
    --currentTrack;

  Track * t = PlayLiszt::instance()->track(currentTrack);

  if (0 == t)
    return 0;

  PlayLiszt::instance()->setCurrentTrack(currentTrack);

  return t->lisztItem();
}

  PlaylistItem *
Liszt::getFirst() const
{
  Track * track = PlayLiszt::instance()->all().getFirst();
  return track ? track->lisztItem() : 0;
}

  PlaylistItem *
Liszt::getAfter(const PlaylistItem * item) const
{
  for (TrackListIterator it(PlayLiszt::instance()->all()); it.current(); ++it)
  {
    if (it.current()->lisztItem() == item)
    {
      ++it;
      return (it.current() ? it.current()->lisztItem() : 0);
    }
  }
  return 0;
}

  bool
Liszt::listVisible() const
{
  return editWidget_->isVisible();
}

  void
Liszt::showList()
{
  editWidget_->show();
}

  void
Liszt::hideList()
{
  editWidget_->hide();
}

  void
Liszt::toggleList()
{
  if (editWidget_->isVisible())
    editWidget_->hide();
  else
    editWidget_->show();
}

  void
Liszt::remove(PlaylistItem *)
{
  qDebug("Liszt::remove() - STUB");
}

  void
Liszt::slotSkipToTrack(long l)
{
  PlayLiszt::instance()->setCurrentTrack(l);
  emit(newCurrent());
  emit(playCurrent());
}

#include "Liszt.moc"
