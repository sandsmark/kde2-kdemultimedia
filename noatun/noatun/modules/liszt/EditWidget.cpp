/*
 * Copyright (C) 2000 Rik Hemsley (rikkus) <rik@kde.org>. All rights reserved.
 *
LICENSE
 */

#include <qstringlist.h>
#include <qlayout.h>
#include <qsplitter.h>
#include <qdragobject.h>

#include "EditWidget.h"
#include "ScoreWidget.h"
#include "PreviewWidget.h"
#include "PlayLiszt.h"

EditWidget::EditWidget()
  : QWidget(0, "Liszt edit widget")
{
  setCaption("Liszt - Noatun");

  QFont f(font());
  f.setPixelSize(12);
  setFont(f);

  QVBoxLayout * layout = new QVBoxLayout(this);

  QSplitter * split = new QSplitter(Vertical, this, "Edit split");

  layout->addWidget(split);

  scoreWidget_ = new ScoreWidget(split);
  previewWidget_ = new PreviewWidget(split);

  connect(
      previewWidget_, SIGNAL(skipToTrack(long)),
      this,           SIGNAL(skipToTrack(long)));

  setAcceptDrops(true);
}

EditWidget::~EditWidget()
{
}

  void
EditWidget::reset()
{
  previewWidget_->reset();
}

  void
EditWidget::dragEnterEvent(QDragEnterEvent * e)
{
  e->accept(QUriDrag::canDecode(e));
}

  void
EditWidget::dropEvent(QDropEvent * e)
{
  if (QUriDrag::canDecode(e)) {
    QStringList l;
    QUriDrag::decodeToUnicodeUris(e, l);
    PlayLiszt::instance()->addFileList(l);
  }
}

  void
EditWidget::closeEvent(QCloseEvent * e)
{
  emit(closed());
  QWidget::closeEvent(e);
}

#include "EditWidget.moc"
