/*
  Children, thank the author for letting you play in his yard with the BSD
  license.  "Thank you Mr. Samuels"

  a GUI for a spectrum analyser
  Based on KJVis (C) 2000 Charles Samuels <charles@kde.org>
  Ported to Winskin by (name redacted due to shame) <neil@qualityassistant.com>

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in
  all copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
  AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN
  AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
  CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

 */

#include <guiSpectrumAnalyser.h>
#include <plugin.h>
#include <kpixmapeffect.h>
#include <kpixmap.h>
#include <kconfig.h>
#include <qcolor.h>

static const int guiSpectrumHeight = 20;
static const int guiSpectrumWidth = 70;
static const int guiSpectrumBars = 35;
static const float guiSpectrumBarMagic = 1.125;

GuiSpectrumAnalyser::GuiSpectrumAnalyser( QWidget *parent, const char *name, int refresh)
  : QWidget(parent,name)
  , MonoFFTScope(refresh)
  , bars(guiSpectrumHeight)
{
  mFGColor.setRgb(0, 255, 0);
  mBGColor.setRgb(0, 0, 0);

  setBands(guiSpectrumBarMagic);

  int w = guiSpectrumBars;
  int wmult = w;
  while (w < guiSpectrumWidth)
    w += wmult;

  mBarWidth=w/wmult;

  // generate vis cache
  QPixmap blankBar(mBarWidth, guiSpectrumHeight);
  blankBar.fill(mBGColor);

  memoryManager.setAutoDelete(true);

  // bars[0] is blank
  {
    QPixmap *newBar = new QPixmap(blankBar);
    bars[0] = newBar;
    memoryManager.append(newBar);
  }

        QColor red(255,0,0);
        QColor green(0,255,0);
        KConfig *config = KGlobal::config();
        config->setGroup("Winskin");
        QColor firstColor = config->readColorEntry("ScopeBarColor1", &red);
        QColor secondColor = config->readColorEntry("ScopeBarColor2", &green);

  for (int i = 1; i < guiSpectrumHeight; i++) {
    QPixmap qpixmap(mBarWidth, i);
    KPixmap canvas(qpixmap);
    KPixmapEffect::gradient(canvas, firstColor, secondColor, KPixmapEffect::VerticalGradient, 4);

    QPixmap *newBar = new QPixmap(blankBar);
    bitBlt(newBar, 0, guiSpectrumHeight - i, &canvas);
    bars[i] = newBar;
    memoryManager.append(newBar);
  }
  start();
}

void GuiSpectrumAnalyser::mousePressEvent ( QMouseEvent* ) {
}

QSize GuiSpectrumAnalyser::sizeHint() const {
  return QSize(guiSpectrumWidth, guiSpectrumHeight);
}

void GuiSpectrumAnalyser::scopeEvent(float *d, int size)
{
  QPainter p(this);
  int x = 0;
  int y = 0;
  int w = width();
  int h = height();
  float *start = d;
  float *end = d + size;
  float fheight = (float)h;
  repaint(rect(), false);
  p.fillRect(0, 0, w, h, mBGColor);
  for( ; start < end; ++start)
  {
    float n = fheight * log(*start + 1.0) * 5;
    int amp = (int)n;
    if(amp < 0)
      amp = 0;
    else if(amp > h)
      amp = h;

    p.drawPixmap(x, 0, *bars[amp]);

    x += mBarWidth;
  }
}

void GuiSpectrumAnalyser::reload()
{
        stop();
        mFGColor.setRgb(0, 255, 0);
        mBGColor.setRgb(0, 0, 0);

        setBands(guiSpectrumBarMagic);

        int w = guiSpectrumBars;
        int wmult = w;
        while (w < guiSpectrumWidth)
        w += wmult;

        mBarWidth=w/wmult;

        // generate vis cache
        QPixmap blankBar(mBarWidth, guiSpectrumHeight);
        blankBar.fill(mBGColor);

        memoryManager.setAutoDelete(true);

        // bars[0] is blank
        {
                QPixmap *newBar = new QPixmap(blankBar);
                bars[0] = newBar;
                memoryManager.append(newBar);
        }

        QColor red(255,0,0);
        QColor green(0,255,0);
        KConfig *config = KGlobal::config();
        config->setGroup("Winskin");
        QColor firstColor = config->readColorEntry("ScopeBarColor1", &red);
        QColor secondColor = config->readColorEntry("ScopeBarColor2", &green);
        setInterval(config->readNumEntry("ScopeFPS", 2));
	
        for (int i = 1; i < guiSpectrumHeight; i++) {
                QPixmap qpixmap(mBarWidth, i);
                KPixmap canvas(qpixmap);
                KPixmapEffect::gradient(canvas, firstColor, secondColor, KPixmapEffect::VerticalGradient, 4);

                QPixmap *newBar = new QPixmap(blankBar);
                bitBlt(newBar, 0, guiSpectrumHeight - i, &canvas);
                bars[i] = newBar;
                memoryManager.append(newBar);
        }
        start();
}
#include "guiSpectrumAnalyser.moc"

// TODO: fix this
// See http://derkarl.org/s-and-s.html for details
// vim: ts=2 et
