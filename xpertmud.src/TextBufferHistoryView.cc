#include "TextBufferHistoryView.h"
#include "TextBufferView.h"
#include "TextBuffer.h"

#include <QScrollBar>
#include <QApplication>
#include <QFrame>
#include <QMouseEvent>
#include <QStyleOptionComplex>

//#include <QStyle>
#include <QDebug>

TextBufferHistoryView::
TextBufferHistoryView(int id, QWidget* parent, const char* name, 
		      const QColor* cmap,
		      const QFont& font, unsigned int scrollBackLines):
        QWidget(parent),
#if 0
  QWidget(parent, /*WRepaintNoErase | WResizeNoErase |
	  WStyle_Customize | WStyle_NormalBorder |*/ Qt::WindowTitleHint |
    Qt::WindowSystemMenuHint | Qt::WindowMinMaxButtonsHint ),
#endif
  inUpdate(false),
  winID(id), scrollSplitEnabled(true), scrollBackLines(scrollBackLines),
  movingSplit(false)
{
  setAttribute(Qt::WA_StaticContents, true);
  setObjectName(name);

  textBuffer = new TextBuffer(0, 0, -1, true);
  ownsBuffer = true;

  mainView = new TextBufferView(0, this, "", cmap, font, textBuffer, true);
  mainView->show();
  mainView->setFollowMode(TextBufferView::EFM_LastLineFollow);
  connect(mainView,SIGNAL(scriptingMousePressEvent(int, int, int)),
	  this,SLOT(slotScriptingMousePressEvent(int, int, int )));
  connect(mainView,SIGNAL(scriptingMouseDragEvent(int, int, int)),
	  this,SLOT(slotScriptingMouseDragEvent(int, int, int)));
  connect(mainView,SIGNAL(scriptingMouseReleaseEvent(int, int, int)),
	  this,SLOT(slotScriptingMouseReleaseEvent(int, int, int)));
  connect(mainView,SIGNAL(offsetYChange(int)),
	  this,SLOT(slotMainOffsetYChange(int)));
//  connect(mainView,SIGNAL(linesCountChange(int)),
//    this,SLOT(slotLinesCountChange(int)));


  splitView = new TextBufferView(0, this, "", cmap, font, textBuffer, false);
  splitView->hide();
  splitView->setFollowMode(TextBufferView::EFM_AlwaysFollow);
  connect(splitView,SIGNAL(scriptingMousePressEvent(int, int, int)),
          this,SLOT(slotScriptingMousePressEvent(int, int, int )));
  connect(splitView,SIGNAL(scriptingMouseDragEvent(int, int, int)),
          this,SLOT(slotScriptingMouseDragEvent(int, int, int)));
  connect(splitView,SIGNAL(scriptingMouseReleaseEvent(int, int, int)),
          this,SLOT(slotScriptingMouseReleaseEvent(int, int, int)));


  splitSeperator = new QFrame( this );
  splitSeperator->setObjectName("seperator");
  splitSeperator->hide();
  splitSeperator->setFrameShape( QFrame::HLine );
  splitSeperator->setFrameShadow( QFrame::Sunken );
  splitSeperator->setCursor(QCursor(Qt::SplitVCursor));

  vscrollBar = new QScrollBar(Qt::Vertical, this);
  vscrollBar->show();
  vscrollBar->setMaximum(0);
  vscrollBar->setMaximum(0);
  vscrollBar->setFixedWidth(style()->pixelMetric(QStyle::PM_ScrollBarExtent ));

  connect(textBuffer, SIGNAL(scrolledLines(int)),
          this, SLOT(slotScrolledLines(int)));
  connect(vscrollBar, SIGNAL(valueChanged(int)),
          this, SLOT(slotVScroll(int)));

  QPalette pal = palette();
  pal.setColor(QPalette::Background, QColor(0, 0, 0));
  setPalette(pal);
  //  setEraseColor(QColor(qRgba(0,0,0,0)));
}

TextBufferHistoryView::~TextBufferHistoryView() {
  if(ownsBuffer && textBuffer != NULL)
    delete textBuffer;
}

void TextBufferHistoryView::switchMouseMode(bool enabled) {
  mainView->switchMouseMode(enabled);
  splitView->switchMouseMode(enabled);
}

void TextBufferHistoryView::setScriptingFont(const QFont& newFont) {
  mainView->setFont(newFont);
  splitView->setFont(newFont);
  updateLayout();
}

QSize TextBufferHistoryView::sizeForChars(unsigned int x, unsigned int y) { 
  return mainView->sizeForChars(x, y);
}

void TextBufferHistoryView::scrollLines(int delta) {
  mainView->scrollLines(delta);
}

void TextBufferHistoryView::slotScrolledLines(int) {
    //qDebug("slotScrolledLines.");
  updateVScrollBar();
}

void TextBufferHistoryView::updateVScrollBar() {
  //#define ATONCE 1
#ifdef OWNSCROLL
  unsigned int lineCount =
    std::max((int)textBuffer->getSizeY() - (int)mainView->getLines(), 0);

  if(vscrollBar->maxValue() < 100) {
    // this is implementad as _forced_redraw_ (!) 
    vscrollBar->setMaxValue(lineCount);
  }
  // TODO!!!!!!!!!!!!!!!!!!!!
  if(lineCount > 0) {
    int newOffset = vscrollBar->maxValue() * mainView->getOffsetY() / 
      lineCount;

    if(newOffset != vscrollBar->value()) {
      vscrollBar->setValue(newOffset);
    }
  }
#else
# ifdef ATONCE
  //    qDebug("************************");
  unsigned int lineCount =
    std::max((int)textBuffer->getSizeY() - (int)mainView->getLines(), 0);
  inUpdate = true;
  vscrollBar->setMaxValue(lineCount);
  //qDebug("x");
  vscrollBar->setValue(mainView->getOffsetY());
  inUpdate = false;
  //qDebug("------------------------");
# else
  update(0,0,1,1);
# endif
#endif
}

void TextBufferHistoryView::slotScriptingMousePressEvent(int, int x, int y) {
  emit scriptingMousePressEvent(getID(), x, y);
}

void TextBufferHistoryView::slotScriptingMouseDragEvent(int, int x, int y) {
  emit scriptingMouseDragEvent(getID(), x, y);
}

void TextBufferHistoryView::slotScriptingMouseReleaseEvent(int, int x, int y) {
  emit scriptingMouseReleaseEvent(getID(), x, y);
}

void TextBufferHistoryView::slotColorConfigChanged() {
  mainView->slotColorConfigChanged();
  splitView->slotColorConfigChanged();
}

void TextBufferHistoryView::slotNewDefaultFont(const QFont & newFont) {
  mainView->slotNewDefaultFont(newFont);
  splitView->slotNewDefaultFont(newFont);
}

void TextBufferHistoryView::slotVScroll(int /*value*/) {
  if(!inUpdate) {
      //qDebug() << "slotVScroll." << vscrollBar->value() << vscrollBar->maximum();
#ifdef OWNSCROLL
    if(vscrollBar->maxValue() > 0) {
      unsigned int lineCount =
	std::max((int)textBuffer->getSizeY() - (int)mainView->getLines(), 0);
      int newOffset = lineCount * value / vscrollBar->maxValue();
      mainView->setOffsetY(newOffset);
      slotMainOffsetYChange(newOffset);
    }
#else
    mainView->setOffsetY(vscrollBar->value());
    slotMainOffsetYChange(vscrollBar->value());
#endif
  }
}

void TextBufferHistoryView::slotMainOffsetYChange(int) {
  // updateLayout is _slow_, so don't call it if we
  // already know that nothin' changed!
  bool needScroll = false;
  int newMax = ((int)textBuffer->getSizeY() - (int)mainView->getLines());
  if (vscrollBar->maximum()<newMax) {
    if (vscrollBar->value()==vscrollBar->maximum()) needScroll = true;
    vscrollBar->setMaximum(newMax);
  }
  if (vscrollBar->value()!=mainView->getOffsetY()) {
    newMax = mainView->getOffsetY();
    needScroll = true;
  }

  if (needScroll) vscrollBar->setValue(newMax);

  if(scrollSplitEnabled) {
    //qDebug() << "slotMainOffsetYChange:" << mainView->getOffsetY() << ((int)textBuffer->getSizeY() - (int)mainView->getLines());
    //qDebug() << "slotMainOffsetYChange(): textBuffer->getSizeY()="<<textBuffer->getSizeY() << "mainView->getLines()="<<mainView->getLines();
    if(mainView->getOffsetY() < ((int)textBuffer->getSizeY() - (int)mainView->getLines())) {
      if(splitView->isHidden()) updateLayout();
    } else {
      if(splitView->isVisible()) updateLayout();
    }
  }
}

void TextBufferHistoryView::wheelEvent(QWheelEvent* ev) {
    vscrollBar->event(ev);
}

void TextBufferHistoryView::mousePressEvent(QMouseEvent* ev) {
  //qDebug(QString("ev->y() = %1, split->y() = %2")
  //	 .arg(ev->y()).arg(splitSeperator->y()));
  if(ev->y() >= splitSeperator->y()-1 &&
     ev->y() <= splitSeperator->y()+splitSeperator->height()+1) {
    movingSplit = true;
    setCursor(QCursor(Qt::SplitHCursor));
  }
}

void TextBufferHistoryView::mouseMoveEvent(QMouseEvent* ev) {
  if(movingSplit) {
    int linesFromTop = mainView->getLines() -
            (int)(((double)ev->y()+ ((double)mainView->getLineHeight()/2.0)) / (double)mainView->getLineHeight());

    linesFromTop = std::min(linesFromTop,(int) mainView->getLines()-1);
    linesFromTop = std::max(linesFromTop, 0);
    if(linesFromTop != (int)scrollBackLines) {
      scrollBackLines = linesFromTop;
      updateLayout();
    }
  }
}

void TextBufferHistoryView::mouseReleaseEvent(QMouseEvent* /*ev*/) {
  if(movingSplit) {
    setCursor(QCursor(Qt::ArrowCursor));
    movingSplit = false;
  }
}

void TextBufferHistoryView::resizeEvent(QResizeEvent*) {
  //  qDebug("*** RESIZING HistoryView ***");

  updateLayout();
}

void TextBufferHistoryView::paintEvent(QPaintEvent* /*pe*/) {
  return;
#ifndef ATONCE
  int lineCount = std::max((int)textBuffer->getSizeY() - (int)mainView->getLines(), 0);

  if(vscrollBar->maximum() != lineCount) {
    //    qDebug(QString("Setting maxValue to %1").arg(lineCount));
    inUpdate = true;
    vscrollBar->setMaximum(lineCount);
    vscrollBar->setValue(mainView->getOffsetY());
    inUpdate = false;
    //    qDebug("finished.");
  }
#endif
  //QRect rect = pe->rect();
  //qDebug(QString("H(%1) paintEvent(%1,%2,%3,%4)").arg((int)this)
  // .arg(rect.left()).arg(rect.top()).arg(rect.right())
  // .arg(rect.bottom()));
  //qDebug(QString("Flag: %1").arg(testWFlags(WRepaintNoErase)));
  //if(pe->erased())
  //qDebug("ERASED!!!");
  //qDebug(QString("BO: %1, WO: %2").arg(backgroundOrigin())
  // .arg(WidgetOrigin));
  //if ( !testWState(WState_BlockUpdates) ) {
  //qDebug("* REPAINT WORKS!");
  //}
}

bool TextBufferHistoryView::eventFilter( QObject */*o*/, QEvent *e ) {
  if(e->type() == QEvent::Paint ||
     e->type() == QEvent::Hide) {
    qDebug("Choking Event!");
    return true;
  }
  return false;
}

void TextBufferHistoryView::updateLayout() {
  if(mainView->width() != width() - vscrollBar->width() || mainView->height() != height()) {
    mainView->resize(width()-vscrollBar->width(), height());
  }

  //qDebug() << "updateLayout():" << mainView->getOffsetY() << ((int)textBuffer->getSizeY() - (int)mainView->getLines());
  //qDebug() << "updateLayout():"<< "getSizeY():" << (int)textBuffer->getSizeY() << "getLines():" <<(int)mainView->getLines();
  if(mainView->getOffsetY() < ((int)textBuffer->getSizeY() - (int)mainView->getLines())) {
    if(scrollSplitEnabled && splitView->isHidden()) {
      splitView->show();
      //qDebug(QString("SplitView Lines: %1").arg(splitView->getLines()));
      splitSeperator->show();
    }
  } else if(splitView->isVisible()) {
    splitView->hide();
    splitSeperator->hide();
  }
  /*int oldViewY = splitView->y();
    int oldWidth = splitView->width();*/
  unsigned int splitViewY = 0;
  if(scrollBackLines > 0) {
    splitViewY = 
      (mainView->getLines() - scrollBackLines) * mainView->getLineHeight();
  } else {
    splitViewY = height();
  }
  if(splitView->isVisible()) {
    //qDebug("Setting Mask");
    mainView->setMask(QRegion(0, 0, mainView->width(), splitViewY));
  } else {
    mainView->clearMask();
  }

  //  if((oldViewY != splitViewY ||
  //      oldWidth != width()-vscrollBar->width()) &&
  //     (splitView->isShown() || splitView2->isShown())) {

    //mainView->setMask(QRegion(0, 0, width(), splitViewY));
  //      splitView->setGeometry(0, splitViewY,
  //			     width()-vscrollBar->width(), 
  //			     height() - splitViewY);
      //      splitView->show();
      //      splitView2->hide();
      //    }
      //
      //  } else {
      //    mainView->clearMask();
      //  }



  //  splitView->repaint(false);


  splitSeperator->move(0, splitViewY-2);
  splitSeperator->resize(width()-vscrollBar->width(), 2);

  vscrollBar->move(QPoint(width()-vscrollBar->width(),0));

  // TODO: migrate to Qt3
  /*
  vscrollBar->resize(QApplication::style().scrollBarExtent().width(),
		     height() - QApplication::style().scrollBarExtent().height());
  */
  QRect rect=QApplication::style()->subControlRect(QStyle::CC_ScrollBar, new QStyleOptionComplex(), QStyle::SC_ScrollBarGroove);
  vscrollBar->resize(rect.width(), height()-rect.height());
  updateVScrollBar();

  //  splitView->move(0, splitViewY);
  //  splitView->resize(width()-vscrollBar->width(), 
  //  		    height() - splitViewY);
  //  qApp->processEvents();
  //  qDebug("Events processed. Move hopefully complete...?");

  //  qDebug("DISABLING UPDATES!");
  //  setUpdatesEnabled(false);
  //  repaint();
  //  qDebug("AFTER REPAINT CHECK!");
  //  mainView->setUpdatesEnabled(false);
  //  qApp->installEventFilter(this);
  
  splitView->setGeometry(0, splitViewY,
      			 width()-vscrollBar->width(), 
    			 height() - splitViewY);
  //  if(oldViewY != splitViewY) 
  //    qApp->processEvents();
  //  qApp->removeEventFilter(this);
  //  qDebug("BEFORE REPAINT CHECK!");
  //  repaint();
  //  qDebug("ENABLING UPDATES!");
  //  mainView->setUpdatesEnabled(true);
  //  setUpdatesEnabled(true);
  //  splitView->setUpdatesEnabled(true);
  //  mainView->setUpdatesEnabled(true);
  //  setUpdatesEnabled(true);
  //  update();
} 
