#include "TextBufferView.h"
#include "TextBuffer.h"

#include <kfontdialog.h>
#include <QApplication>
#include <QClipboard>
#include <QPainter>
#include <QPaintEvent>
#include <QDebug>

#include <iostream>

const unsigned int base_color_table_size = 18;
const QColor  base_color_table[] =
// The following are almost IBM standard color codes, with some slight
// gamma correction for the dim colors to compensate for bright X screens.
// It contains the 8 ansiterm/xterm colors in 2 intensities.
{
  // Fixme: could add faint colors here, also.
  // normal
  QColor(0x00,0x00,0x00),  QColor(0xB2,0x18,0x18), // Black, Red
  QColor(0x18,0xB2,0x18),  QColor(0xB2,0x68,0x18), // Green, Yellow
  QColor(0x18,0x18,0xB2),  QColor(0xB2,0x18,0xB2), // Blue,  Magenta
  QColor(0x18,0xB2,0xB2),  QColor(0xB2,0xB2,0xB2), // Cyan,  White
  // intensiv
  QColor(0x68,0x68,0x68),  QColor(0xFF,0x54,0x54),
  QColor(0x54,0xFF,0x54),  QColor(0xFF,0xFF,0x54),
  QColor(0x54,0x54,0xFF),  QColor(0xFF,0x54,0xFF),
  QColor(0x54,0xFF,0xFF),  QColor(0xFF,0xFF,0xFF),

  // default background / default foreground
  QColor(0x00,0x00,0x00),  QColor(0xB2,0xB2,0xB2)
};

TextBufferView::TextBufferView(int id, QWidget* parent, const char* name,
			       const QColor* cmap, const QFont& font,
			       TextBuffer* cTextBuffer,
			       bool fixBuffer):
  QWidget(parent, /*WRepaintNoErase | WResizeNoErase |
	  WStyle_Customize | WStyle_NormalBorder |*/ Qt::WindowTitleHint |
    Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint ),
  leftBorderWidth(2),
  offsetX(0), offsetY(0), columns(0), lines(0), 
  fixTextBuffer(true), 
  winID(id), blinking(false), isDefaultFont(true), 
  colorTable(cmap), selectingEnabled(true), selecting(false),
  colorSelectingMux(false), 
  scriptingDrag(false),
  nextCachePoint(0), maxCache(1000),
  followMode(EFM_DontFollow)
{

  charCachePixmap = QPixmap(1,1);

  setAttribute(Qt::WA_StaticContents, true);
  setObjectName(name);

  fontW = fontH = fontA = 1;
  columns = lines = 1;

  drawBuffer = new QPixmap(width(), height());

  fixTextBuffer = fixBuffer;
  if(cTextBuffer == NULL) {
    textBuffer = new TextBuffer(columns, lines, false);
    ownsBuffer = true;
  } else {
    textBuffer = cTextBuffer;
    ownsBuffer = false;
  }

  setFont(font);
  setFocusPolicy(Qt::NoFocus);

  QPalette pal(palette());
  pal.setColor(QPalette::Background, colorTable[16]);
  setAutoFillBackground(true);
  setPalette(pal);

  clipboard = QApplication::clipboard();

  connectTextBuffer();
  connect(clipboard, SIGNAL(dataChanged()), this, SLOT(slotClearSelection()));

  updateChars(0, 0, columns+1, lines+1);
}

TextBufferView::~TextBufferView() { 
  if(ownsBuffer && textBuffer != NULL)
    delete textBuffer;
}

void TextBufferView::connectTextBuffer() {
  connect(textBuffer, SIGNAL(textChanged(int,int,unsigned int, unsigned int)),
	  this, SLOT(slotTextChanged(int,int,unsigned int, unsigned int)));
  connect(textBuffer, SIGNAL(textChanged()),
	  this, SLOT(slotTextChanged()));
  connect(textBuffer, SIGNAL(scrolledLines(int)),
	  this, SLOT(slotScrolledLines(int)));
  connect(textBuffer, SIGNAL(scrolledColumns(int)),
	  this, SLOT(slotScrolledColumns(int)));
  connect(textBuffer, SIGNAL(frontCut(int)),
	  this, SLOT(slotFrontCut(int)));
}

void TextBufferView::switchMouseMode(bool enabled) {
  if(selectingEnabled == enabled) 
    return;
  if(selectingEnabled)
    slotClearSelection();
  selectingEnabled = enabled;
}

void TextBufferView::setScriptingFont(const QFont& newFont) {
  setFont(newFont);
  isDefaultFont = false;
}

QSize TextBufferView::sizeForChars(unsigned int x, unsigned int y) {
  return  QSize((x+1)*fontW+2,(y+1)*fontH+2);
}

void TextBufferView::scrollLines(int delta) {
  if(textBuffer->isGrowBuffer()) {
    int oldOffsetY = offsetY;
    if(delta < 0)
      offsetY = std::max(offsetY+delta, 0);
    else
      offsetY = std::min
	(offsetY+delta,
	 std::max((int)textBuffer->getSizeY() - (int)lines, 0));
    if(oldOffsetY != offsetY) {
      emit offsetYChange(offsetY);
      update();
    }
  } else {
    textBuffer->scrollLines(delta);
  }
}

void TextBufferView::slotTextChanged(int column, int line,
				     unsigned int width, unsigned int height) {
  //  qDebug(QString("slotTextChanged called! %1 %2 %3 %4")
  //   .arg(column).arg(line).arg(width).arg(height));
  updateChars(column-offsetX, line-offsetY, width, height);

  emit linesCountChange(buffer()->getLines());
}

void TextBufferView::slotTextChanged() {
  updateChars(0, 0, columns+1, lines+1);
}

//static int t = 0;
void TextBufferView::slotScrolledLines(int delta) {
  recalcOffsetY(delta, false);
}

void TextBufferView::slotScrolledColumns(int) {
  update();
}

void TextBufferView::slotFrontCut(int delta) {
  recalcOffsetY(delta, true);
}

void TextBufferView::recalcOffsetY(int delta, bool cut) {
  // the offset changed... our actions depends on the follow
  // mode
  int oldOffsetY = offsetY;
  if(delta == 0) return;
  if(followMode == EFM_AlwaysFollow) {
    offsetY = std::max((int)textBuffer->getSizeY() - (int)lines, 0);
  } else if(cut) {
    // if 'cut' and LastLineFollow, than we're sure to be in
    // the last line if we can do the following:
    // Try to stay in the same line (same content visible)
    //    std::cout << "+++++++++++++ " << offsetY << " " << delta << std::endl;
    if((int)offsetY + delta > 0)
      offsetY += delta;
  } else if(followMode == EFM_LastLineFollow &&
	    offsetY == (int)textBuffer->getSizeY() - delta - (int)lines) {
    // we've been in the last line, so try to stay there
    offsetY = textBuffer->getSizeY() - lines;
  }

  if(offsetY != oldOffsetY)
    emit offsetYChange(offsetY);

  if((!cut && (offsetY != oldOffsetY)) ||
     ( cut && ((int)offsetY != (int)oldOffsetY + delta))) {
    update();
  }
}

void TextBufferView::slotColorConfigChanged() {
  QPalette pal(palette());
  pal.setColor(QPalette::Background, colorTable[16]);
  setAutoFillBackground(true);
  setPalette(pal);
  update();
}

void TextBufferView::slotClearSelection() {
  if(selecting) { // there was an other selection active... clear it
    int line = selEndY - selStartY;
    selecting = false;
    if(line)
      updateChars(0, selStartY-offsetY, columns, line+1);
    else
      updateChars(selStartX-offsetX, selStartY-offsetY, selEndX-selStartX+1, 1);
  } 
}

void TextBufferView::slotClearWindow() {
  // TODO: implement what is meant
  // this clears just the ansi pane...
  textBuffer->clearWindow();
}

void TextBufferView::slotChangeFont() {
  QFont newFont = font();
  int result = KFontDialog::getFont(newFont, true, this);
  if(result == KFontDialog::Accepted) {
    setFont(newFont);
    isDefaultFont = false;
  }
}

void TextBufferView::slotNewDefaultFont(const QFont& newFont) {
  if(isDefaultFont)
    setFont(newFont);
}

void TextBufferView::calcGeometry() {
  bool inLastLine = (offsetY == (int)textBuffer->getSizeY() - (int)lines);
  //qDebug() << QString("CalcGeometry: inLastLine = %1").arg(inLastLine);

  columns = width() / fontW;
  lines = height() / fontH;
  //qDebug() << QString("CalcGeometry: Height = %1, fontH = %2, lines = %3").arg(height()).arg(fontH).arg(lines);

  if(fixTextBuffer) {
    textBuffer->resize(columns, lines);
  }
  int oldOffsetY = offsetY;
  switch(followMode) {
  case EFM_AlwaysFollow: 
    offsetY = std::max((int)textBuffer->getSizeY() - (int)lines, 0);
    break;
  case EFM_LastLineFollow:
    if(inLastLine) {
      offsetY = textBuffer->getSizeY() - lines;
    }
    break;
  default:
    break;
  }
  if(offsetY != oldOffsetY) {
    emit offsetYChange(offsetY);
  }
}

void TextBufferView::fontChange() {
  QFontMetrics fm(font());
  fontH = fm.height();
  fontW = fm.maxWidth();
  fontA = fm.ascent();

  // invalidate cache
  charCache.clear();
  nextCachePoint = 0;

  setMinimumSize(QSize(fontW,fontH));
  calcGeometry();
  update();
}

void TextBufferView::closeEvent(QCloseEvent*) {
  showMinimized();
}

void TextBufferView::paintEvent(QPaintEvent* pe) {
  QRect rect = pe->rect();
  //  qDebug(QString("W(%1) paintEvent(%1,%2,%3,%4)").arg((int)this)
  //   .arg(rect.left()).arg(rect.top()).arg(rect.right())
  //   .arg(rect.bottom()));
  for(int y=rect.top()/fontH; y<rect.bottom()/fontH+1; ++y) {
    for(int x=(rect.left()-leftBorderWidth)/fontW; x<(rect.right()-leftBorderWidth)/fontW+1; ++x) {
      drawChar(this, x, y);
    }
  }
}

void TextBufferView::resizeEvent(QResizeEvent* re) {
  //qDebug("Resizing View!");
  calcGeometry();
  QPixmap tmpP(re->size());
  tmpP = drawBuffer->copy(0,0,drawBuffer->width(), drawBuffer->height());
  *drawBuffer=tmpP;
  update();
}

void TextBufferView::changeEvent(QEvent *event) {
  if (event->type()==QEvent::FontChange) {
    fontChange();
  }
  QWidget::changeEvent(event);
}

/*
void TextBufferView::updateCharsUni(int lux, int luy, int w, int h, bool redraw) {
  lux = std::max(0, lux);
  luy = std::max(0, luy);
  w = std::min((int)columns, lux+w) - lux;
  h = std::min((int)lines, luy+h) - luy;
  int rlx = lux + w - 1;
  int rly = luy + h - 1;

  QPainter paint;
  paint.begin(drawBuffer);
  paint.setBackgroundMode(OpaqueMode);

  
  QChar* uniBuffer = new QChar[columns];
  for(int y=luy; y<=rly; ++y) {
    for(int x=lux; x<=rlx; ++x) {
      // search for maximum length string of equal
      // attributes
      int len = 1;
      ColorChar cc = textBuffer->getBufferChar(offsetX+x, offsetY+y);
      uniBuffer[0] = cc.getChar();
      ColorChar ccNext = textBuffer->getBufferChar(offsetX+x+len, offsetY+y);
      while(x+len <= rlx &&
	    ccNext.equalColorsAttr(cc) &&
	    (!selecting || // break if this is start/end of selection
	     ((y != selStartY || x+len != selStartX) &&
	      (y != selEndY || x+len != selEndX)))) {
	uniBuffer[len] = ccNext.getChar();
	++len;
	ccNext = textBuffer->getBufferChar(offsetX+x+len, offsetY+y);
      }
      QString uniString(uniBuffer, len);
	if (selecting && 
	    ((y>selStartY && y<selEndY) || // Line in between

	     (y==selStartY && x>=selStartX && 
	      (y!=selEndY || x<selEndX)) || // starting line

	     (y==selEndY && x<selEndX &&
	      (y!=selStartY || x>=selStartX)) // ending line
	     )) {
	  cc.setAttribute(ColorChar::A_REVERSE,
			  !cc.checkAttribute(ColorChar::A_REVERSE));
	}
	drawAttrStr(&paint,
		    QRect(fontW*x,fontH*y,fontW*len,fontH),
		    uniString, cc);
	
	x += len-1;
    }
  }

  delete[] uniBuffer;
  paint.end();

  if(redraw)
    update(fontW*lux, fontH*luy, fontW*w, fontH*h);
}

void TextBufferView::drawAttrStr(QPainter* paint, const QRect& rect,
				 QString text, ColorChar attr) {
  if((blinking && attr.checkAttribute(ColorChar::A_BLINK)) ||
     attr.checkAttribute(ColorChar::A_CONCEALED)) {
    for(unsigned int i=0; i<text.length(); ++i) 
      text[i] = QChar(' ');
  }

  // background color
  QColor bg;
  if(attr.checkAttribute(ColorChar::A_DEFAULTBG)) 
    bg = colorTable[16];
  else if(attr.getBg() < 16)
    bg = colorTable[attr.getBg()];
  else {
    // TODO 256 color mode
    bg = colorTable[16];
  }

  // pen color
  QColor pen;
  if(attr.checkAttribute(ColorChar::A_DEFAULTFG))
    pen = colorTable[17];
  else if(attr.getFg() < 8 && attr.checkAttribute(ColorChar::A_INTENSE)) 
    pen = colorTable[attr.getFg() + 8];
  else if(attr.getFg() < 8 && attr.checkAttribute(ColorChar::A_FAINT)) 
    pen = colorTable[attr.getFg()]; // TODO!!!
  else if(attr.getFg() < 16) {
    pen = colorTable[attr.getFg()];
  } else {
    // TODO 256 color mode
    pen = colorTable[17];
  }
  if(attr.checkAttribute(ColorChar::A_REVERSE))
    std::swap(pen, bg);
  
  QFont drawFont = font();
  if(attr.checkAttribute(ColorChar::A_BOLD))
    drawFont.setBold(true);
  if(attr.checkAttribute(ColorChar::A_ITALIC))
    drawFont.setItalic(true);
  if(attr.checkAttribute(ColorChar::A_UNDERLINE))
    drawFont.setUnderline(true);
  if(attr.checkAttribute(ColorChar::A_STRIKEOUT))
    drawFont.setStrikeOut(true);
  paint->setFont(drawFont);
  paint->setBackgroundColor(bg);
  paint->setPen(pen);

  
  paint->drawText(rect.x(), rect.y()+fontA, text);
}
*/

void TextBufferView::updateChars(int lux, int luy, int w, int h) {
  //  qDebug(QString("Updating (%1, %2) [%3, %4]").
  //   arg(fontW*lux).arg(fontH*luy).
  //   arg(fontW*w).arg(fontH*h));
  //  redrawForcedCheck();

  update(fontW*lux, fontH*luy, fontW*w, fontH*h);
}

void TextBufferView::drawChars(QPaintDevice* pd, int lux, int luy, int w, int h) {
  lux = std::max(0, lux);
  luy = std::max(0, luy);
  w = std::min((int)columns+1, lux+w) - lux;
  h = std::min((int)lines+1, luy+h) - luy;
  int rlx = lux + w - 1;
  int rly = luy + h - 1;
  //  qDebug(QString("Recreating chars (%1, %2) (%3, %4)").
  //	 arg(lux).arg(luy).arg(rlx).arg(rly));

  for(int y=luy; y<=rly; ++y) {
    for(int x=lux; x<=rlx; ++x) {
      drawChar(pd, x, y);
    }
  }

}

void TextBufferView::drawChar(QPaintDevice* pd, int x, int y) {
#ifndef NODRAWING
  QRect rect(x*fontW, y*fontH, fontW, fontH);

  int bufX = x + offsetX;
  int bufY = y + offsetY;
  ColorChar cc = textBuffer->getBufferChar(bufX, bufY);

  if (selecting && 
      ((bufY>selStartY && bufY<selEndY) || // Line in between

       (bufY==selStartY && bufX>=selStartX &&
	(bufY!=selEndY || bufX<selEndX)) || // starting line
       
       (bufY==selEndY && bufX<selEndX &&
	(bufY!=selStartY || bufX>=selStartX)) // ending line
       )) {
    cc.setAttribute(ColorChar::A_REVERSE,
		    !cc.checkAttribute(ColorChar::A_REVERSE));
  }

  if((blinking && cc.checkAttribute(ColorChar::A_BLINK)) ||
     cc.checkAttribute(ColorChar::A_CONCEALED)) {
    cc.setChar(' ');
  }

  if(charCachePixmap.height() != fontH) {
    charCache.clear();
    if(maxCache > 0) {
      // allways use the max. size if set
      QPixmap tmpPix(fontW * maxCache, fontH);
      QPainter pntr(&tmpPix);
      pntr.drawPixmap(0,0, charCachePixmap);
      pntr.end();
      charCachePixmap = tmpPix;
    } else {
      // start with 30 and expand
      QPixmap tmpPix(fontW * 30, fontH);
      QPainter pntr(&tmpPix);
      pntr.drawPixmap(0,0, charCachePixmap);
      pntr.end();
      charCachePixmap = tmpPix;
    }
  }

  charCacheT::iterator it = charCache.find(cc);

  if(it == charCache.end()) {
    //qDebug() << QString("Creating cache entry... %1").arg(nextCachePoint);
    unsigned int offset = nextCachePoint;

    if((nextCachePoint+1) * fontW > (unsigned int)charCachePixmap.width()) {
      //qDebug() << QString("not enough space %1 vs %2").arg((nextCachePoint+1) * fontW).arg(charCachePixmap.width());

      // => not enough space in the pixmap
      maxCache=0;
      if(maxCache > 0) {
    //qDebug("limit");
    // there is a cache limit
        if((nextCachePoint+1) * fontW < (unsigned int)maxCache * fontW) {
          //qDebug() << QString("resizing to %1 (vs %2)").arg(maxCache * fontW).arg((nextCachePoint+1) * fontW);
          // but it's not full yet (perhaps a width change)
          QPixmap tmpPix(maxCache * fontW, charCachePixmap.height());
          QPainter pntr(&tmpPix);
          pntr.drawPixmap(0,0, charCachePixmap);
          pntr.end();
          charCachePixmap = tmpPix;
          //qDebug() << "pixmap size: "<<charCachePixmap.width();
          ++nextCachePoint;
        } else {
          // and it's full
          // let's overwrite something
          //qDebug() << "need overwrite";
          charCacheT::reverse_iterator del = charCache.rbegin();
          if(del != charCache.rend()) {
            // there was at least one char in the cache
            offset = del->second;
            //qDebug() << QString("overwrite %1").arg(offset);
            charCache.erase(del->first);
          } else {
            //qDebug() << QString("reset offset from %1").arg(offset);
            offset = 0;
          }
        }
      } else {
	//qDebug() << "no limit";
	// no cache limit, just go on
        QPixmap tmpPix(charCachePixmap.width() * 2, charCachePixmap.height());
        QPainter pntr(&tmpPix);
        pntr.drawPixmap(0,0, charCachePixmap);
        pntr.end();
        charCachePixmap = tmpPix;
	      ++nextCachePoint;
      }
    } else {
      //qDebug("enough space");
      // still enough space for this time
      ++nextCachePoint;
    }
    //qDebug() << QString("offset is %1").arg(offset);

    it = charCache.insert(charCacheT::value_type(cc, offset)).first;

    // background color
    QColor bg;
    if(cc.checkAttribute(ColorChar::A_DEFAULTBG)) 
      bg = colorTable[16];
    else if(cc.getBg() < 16)
      bg = colorTable[cc.getBg()];
    else {
      int base = cc.getBg() - 16;
      int red = base / 36;
      int green = (base - red*36) / 6;
      int blue = (base - red*36 - green*6);
      bg = QColor(red * 51, green * 51, blue * 51);
    }
    
    // pen color
    QColor pen;
    if(cc.checkAttribute(ColorChar::A_DEFAULTFG))
      pen = colorTable[17];
    else if(cc.getFg() < 8 && cc.checkAttribute(ColorChar::A_INTENSE)) 
      pen = colorTable[cc.getFg() + 8];
    else if(cc.getFg() < 8 && cc.checkAttribute(ColorChar::A_FAINT)) 
      pen = colorTable[cc.getFg()]; // TODO!!!
    else if(cc.getFg() < 16) {
      pen = colorTable[cc.getFg()];
    } else {
      int base = cc.getFg() - 16;
      int red = base / 36;
      int green = (base - red*36) / 6;
      int blue = (base - red*36 - green*6);
      pen = QColor(red * 51, green * 51, blue * 51);
    }
    if(cc.checkAttribute(ColorChar::A_REVERSE))
      std::swap(pen, bg);
    
    QFont drawFont = font();
    if(cc.checkAttribute(ColorChar::A_BOLD))
      drawFont.setBold(true);
    if(cc.checkAttribute(ColorChar::A_ITALIC))
      drawFont.setItalic(true);
    if(cc.checkAttribute(ColorChar::A_UNDERLINE))
      drawFont.setUnderline(true);
    if(cc.checkAttribute(ColorChar::A_STRIKEOUT))
      drawFont.setStrikeOut(true);
    
    QPainter charPaint;
    charPaint.begin(&charCachePixmap);

    charPaint.setBackgroundMode(Qt::TransparentMode);
    charPaint.setFont(drawFont);
    QBrush bb = charPaint.background();
    bb.setColor(bg);
    charPaint.setBackground(bb);
    charPaint.setPen(pen);
    
    charPaint.eraseRect(offset * fontW, 0, fontW, fontH);
    charPaint.drawText(offset * fontW, fontA, cc.getChar());
    charPaint.end();
    //qDebug() << offset * fontW <<  fontA << cc.getChar();
  }

  //qDebug() << QString("bitblitting %1").arg(it->second);
  QPainter painter(pd);
  painter.drawPixmap(rect.x()+leftBorderWidth, rect.y(), charCachePixmap, it->second * fontW, 0, fontW, fontH);
  painter.end();
/*
  bitBlt(pd, rect.x()+leftBorderWidth, rect.y(),
  	 &charCachePixmap, it->second * fontW, 0, fontW, fontH,
  	 Qt::CopyROP, true);
*/
#else
  std::cout << textBuffer->getBufferChar(offsetX+x, offsetY+y).getChar().toLatin1()
	    << std::flush;
#endif
}

void TextBufferView::mousePressEvent(QMouseEvent* ev) {
  if ( !rect().contains(ev->pos()) ) return;


  if (!selectingEnabled) {
    if ( ev->button() == Qt::LeftButton) {
      // only the LMB is used for scripting

      // TODO: Check cast and math
      scrLastX=std::max(0,((int)(ev->x()))/fontW)+offsetX;
      scrLastY=std::max(0,((int)(ev->y()))/fontH)+offsetY;

      emit scriptingMousePressEvent(getID(),
				    scrLastX,
				    scrLastY);

      scriptingDrag=true;
      
      return;
    }
  }


  
  //  if (ev->button() == RightButton) {
    // !selecting...
  //    popup->popup(mapToGlobal(ev->pos()));
  //    return;
  //  }

  if (selecting && ev->button() != Qt::LeftButton) {
    // every other button removes the selection
    selecting=false; // just in case

    int l=selEndY-selStartY;

    if (l) 
      updateChars(0,selStartY-offsetY,columns+1,l+1);
    else
      updateChars(selStartX-offsetX,selStartY-offsetY,selEndX-selStartX+1,1);
    
    return;
  }

  if (selecting) { // there was an other selection active... clear it
    int l=selEndY-selStartY;
    selecting=false;
    if (l) 
      updateChars(0,selStartY-offsetY,columns+1,l+1);
    else
      updateChars(selStartX-offsetX,selStartY-offsetY,selEndX-selStartX+1,1);
  }
  // TODO: Check cast and math
  selBeginX=selEndX=selStartX=std::max(0,((int)(ev->x()))/fontW)+offsetX;
  selBeginY=selEndY=selStartY=std::max(0,((int)(ev->y()))/fontH)+offsetY;
  selecting=true;
  updateChars(selStartX-offsetX,selStartY-offsetY,1,1);
  if (ev->modifiers() & Qt::ShiftModifier)
    colorSelectingMux=true;
  else
    colorSelectingMux=false;
  //  std::cout << "SingleClick" << std::endl;
}


void TextBufferView::mouseMoveEvent(QMouseEvent* ev) {
  if ( !rect().contains(ev->pos()) ) return;

  if (!selectingEnabled && scriptingDrag) {
    // TODO: Check cast and math
    int x=std::max(0,((int)(ev->x()))/fontW)+offsetX;
    int y=std::max(0,((int)(ev->y()))/fontH)+offsetY;
    if (x!=scrLastX || y!=scrLastY) {
      emit scriptingMouseDragEvent(getID(),x,y);
      scrLastX=x;
      scrLastY=y;
    }
    return;
  }

  if (!selecting) return;

  //  if ( ev->button() != LeftButton) return;
  int x=std::max(0,((int)(ev->x()))/fontW)+offsetX;
  int y=std::max(0,((int)(ev->y()))/fontH)+offsetY;

  if (selBeginY>y ||
      (selBeginY==y && selBeginX>x)) {

    // selecting upwards

    if (x==selStartX && y==selStartY) return;
    int osx=selStartX;
    int osy=selStartY;
    selStartX=x;
    selStartY=y;

    if (selEndX!=selBeginX || selEndY!=selBeginY) {
      // we switched sides! ==> repaint whole area
      osx=selEndX;
      osy=selEndY;

      selEndX=selBeginX;
      selEndY=selBeginY;
    }

    int l=qAbs(osy-y);
    if (l)
      updateChars(0,std::min(osy,y)-offsetY,columns+1,l+1);
    else 
      updateChars(std::min(osx,x)-offsetX,std::min(osy,y)-offsetY,
		  qAbs(osx-x)+1,1);
  } else {

    // selecting downwards

    if (x==selEndX && y==selEndY) return;
    int oex=selEndX;
    int oey=selEndY;
    selEndX=x;
    selEndY=y;
    if (selStartX!=selBeginX || selStartY!=selBeginY) {
      // we switched sides ==> repaint whole area
      oex=selStartX;
      oey=selStartY;

      selStartX=selBeginX;
      selStartY=selBeginY;
    }
    int l=qAbs(oey-y);

    if (l)
      updateChars(0,std::min(oey,y)-offsetY,columns+1,l+1);
    else 
      updateChars(std::min(oex,x)-offsetX,std::min(oey,y)-offsetY,
		  qAbs(oex-x)+1,1);
  }

}


void TextBufferView::mouseReleaseEvent(QMouseEvent* ev) {
  // This can't be right... we allways want the mouseReleaseEvent... 
  //  if ( !rect().contains(ev->pos()) ) return;


  if (!selectingEnabled && scriptingDrag) {

    // TODO: Check cast and math
    emit scriptingMouseReleaseEvent(
				    getID(),
				    std::max(0,((int)(ev->x()))/fontW)+offsetX,
				    std::max(0,((int)(ev->y()))/fontH)+offsetY
				    );
    scrLastX=-1;
    scrLastY=-1;

    scriptingDrag=false;
  }

  if (!selecting) return;

  disconnect(clipboard, SIGNAL(dataChanged()),
	     this, SLOT(slotClearSelection()));

  /*if (clipboard->supportsSelection()) */{
    // SET DATA, but only on systems with mouse-Selection(X11) 
    // all others have to use the context menu
///    bool oldMode=clipboard->selectionModeEnabled();
///    clipboard->setSelectionMode(true);

    slotCopySelectedContents();
///    clipboard->setSelectionMode(oldMode);
  }

  connect(clipboard, SIGNAL(dataChanged()),
	  this, SLOT(slotClearSelection()));

  //  std::cout << "ReleaseClick" << std::endl;
}


void TextBufferView::mouseDoubleClickEvent(QMouseEvent * ev) {
  //qDebug("DoubleClickEnter");

  if ( !rect().contains(ev->pos()) ) return;
  if (!selectingEnabled) return;
  if (ev->button() != Qt::LeftButton) return;
  //qDebug("DoubleClick");
  if (!selecting) return; // we get a mousePressEvent earlier, so this should be set!

  // Debugging test ... implement word selection as soon as doubleClicks work
  selBeginX=selStartX=10;
  selBeginY=selStartY=2;
  selEndX=14;
  selEndY=3;
}

void TextBufferView::slotCopySelectedContents() {
  if (!selecting) return;

  QString sel;
  int space_count=0;
  int lastFG=7;
  int lastBG=0;
  bool lastIntense=false;
  static const char fgcols[]="xrgybmcw";
  static const char bgcols[]="XRGYBMCW";

#define isIntense() checkAttribute(ColorChar::A_INTENSE)

  for (int x=selStartX, y=selStartY;
       (x!=selEndX || y!=selEndY) && y<=selEndY; 
       ++x) {
    if (x==(int)columns) {
      x=0;
      ++y;
      if (space_count) {
	sel.truncate(sel.length()-space_count);
	space_count=0;
      }
      sel.append('\n');
    }
    ColorChar next=textBuffer->getColorCharAt(x,y);
    if (colorSelectingMux) {

      if ((space_count>1) && 
	  (((next.getFg() & 0x07) != lastFG) ||
	   (next.getBg() != lastBG) ||
	   (next.isIntense() != lastIntense))) {

	sel.truncate(sel.length()-space_count);
	QString space(" %c");
	space += fgcols[lastFG];
	for (int i=0; i< space_count; ++i)
	  sel.append(space);
	space_count=0;
      }

      if (next.isIntense() != lastIntense) {
	lastIntense=next.isIntense();
	if (lastIntense) {
	  sel.append("%ch");
	} else {
	  sel.append("%cn");
	  lastFG=7;
	  lastBG=0;
	}
	space_count=0;
      }

      if ((next.getFg() & 0x07) != lastFG) {
	lastFG=next.getFg() & 0x07;
	sel.append("%c");
	sel.append(fgcols[lastFG]);
	space_count=0;
      }

      if (next.getBg() != lastBG) {
	lastBG=next.getBg();
	sel.append("%c");
	sel.append(bgcols[lastBG]);
	space_count=0;
      }
    }
    if (next.getChar() == ' ') {
      space_count++;
    } else {
      if (colorSelectingMux && (space_count>1)) {
	sel.truncate(sel.length()-space_count);
	QString space("% ");
	for (int i=0; i< space_count; ++i)
	  sel.append(space);
      }
      space_count=0;
      if (colorSelectingMux && next.getChar()=='%')
	sel.append('%');
      if (colorSelectingMux && next.getChar()=='\\')
	sel.append('\\');
    }
    sel.append(next.getChar());
  }
  if (space_count) {
    sel.truncate(sel.length()-space_count);
  }
  clipboard->setText(sel);

}

//void TextBufferView::setGeometry( int x, int y, int w, int h ) {
//  if(isVisible()) {
    //qDebug("setting Geometry here!");
    //  switch(followMode) {
    //ase EFM_AlwaysFollow: 
    /*  offsetY = std::max((int)textBuffer->getSizeY() - (int)lines, 0);
	break;
	case EFM_LastLineFollow:
	if(offsetY + 1 == (int)textBuffer->getSizeY() - (int)lines) {
	offsetY = textBuffer->getSizeY() - lines;*/
    //    clearWState(WState_Toplevel);
    //    clearWFlags(WType_TopLevel);
    //    int oldwinid = winid;
    //    winid = 0;
    //    hide();
    //    winid = oldwinid;
    //    QPoint oldPos(pos());
    //    internalSetGeometry(x, y, w, h, false);
    //    QMoveEvent e( pos(), oldPos );
    //    QApplication::sendEvent( this, &e );

    //    QPixmap* pcopy= new QPixmap(width(), height());
    //    pcopy->fill(QColor(0,0,255));
    //    bitBlt(pcopy, 0, 0, this, 0, 0, width(), height(), Qt::CopyROP);
    //  QWidget* savedBuffer = new CopyWidget(pcopy, parentWidget());
    //    QWidget* savedBuffer = new CopyWidget(pcopy, NULL);
    //    savedBuffer->setPaletteBackgroundColor(QColor(0, 255, 0));
    //    savedBuffer->setErasePixmap(*pcopy);

    //    savedBuffer->resize(width(), height());
    //    savedBuffer->show();
    //    qApp->processEvents();
    //    savedBuffer->resize(width(), height());
    //    savedBuffer->stackUnder(this);
    //    savedBuffer->setGeometry(geometry());
    //    savedBuffer->show();
    //    bitBlt(savedBuffer, 0, 0,
    //    	   this, 0, 0, width(), height(), Qt::CopyROP);
    //    qApp->processEvents();
    //stackUnder(savedBuffer);
    //    qApp->processEvents();
//  hide();
//  QWidget::setGeometry(x, y, w, h);
//  show();
    //    savedBuffer->stackUnder(this);
    //    qApp->processEvents();
    //    delete savedBuffer;
    //    delete pcopy;
    //    qApp->processEvents();
    //    update();
    //    qApp->processEvents();
    
    //    QWidget::move(x, y);
    //  setWState(WState_Visible);
    //  update();
    //    show();
    //    update();
//}
//}
