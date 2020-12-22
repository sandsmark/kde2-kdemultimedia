#ifndef LISZT_H
#define LISZT_H

#include <noatunplaylist.h>
#include <plugin.h>

class EditWidget;

class Liszt : public Playlist, public Plugin
{
  Q_OBJECT

  public:
    Liszt();
    ~Liszt();

    virtual void init();
    virtual void reset();
    virtual void clear();
    virtual void addFile(const KURL &, bool play = false);
    virtual PlaylistItem * next();
    virtual PlaylistItem * current();
    virtual PlaylistItem * previous();

    virtual PlaylistItem * getFirst() const;
    virtual PlaylistItem * getAfter(const PlaylistItem * item) const;
	
    virtual bool listVisible() const;

    virtual Playlist * playlist() const { return (Playlist*)this; }

  public slots:	

    virtual void showList();
    virtual void hideList();
    virtual void toggleList();
    virtual void remove(PlaylistItem *);

    virtual void slotSkipToTrack(long);

  private:

    EditWidget * editWidget_;
};

#endif
