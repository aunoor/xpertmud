//----------------------------------------------------------------------------
//    filename             : qextmditaskbar.cpp
//----------------------------------------------------------------------------
//    Project              : Qt MDI extension
//
//    begin                : 07/1999       by Szymon Stefanek as part of kvirc
//                                         (an IRC application)
//    changes              : 09/1999       by Falk Brettschneider to create an
//                           - 06/2000     stand-alone Qt extension set of
//                                         classes and a Qt-based library
//    patches              : 02/2000       by Massimo Morin (mmorin@schedsys.com)
//
//    copyright            : (C) 1999-2000 by Szymon Stefanek (stefanek@tin.it)
//                                         and
//                                         Falk Brettschneider
//    email                :  gigafalk@yahoo.com (Falk Brettschneider)
//----------------------------------------------------------------------------
//
//----------------------------------------------------------------------------
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU Library General Public License as
//    published by the Free Software Foundation; either version 2 of the
//    License, or (at your option) any later version.
//
//----------------------------------------------------------------------------

#include "qextmditaskbar.h"
#include "qextmdimainfrm.h"
#include "qextmdichildview.h"
#include "qextmdidefines.h"

#include <QToolTip>
#include <QLabel>
#include <QStyle>
#include <QResizeEvent>
#include <QMouseEvent>

//#include <qnamespace.h>

/*
   @quickhelp: QextMdiTaskBar
   @widget: Taskbar
      This window lists the currently open windows.<br>
      Each button corresponds to a single MDI (child) window.<br>
      The button is enabled (clickable) when the window is docked , and can be
      pressed to bring it to the top of the other docked windows.<br>
      The button text becomes red when new output is shown in the window and it is not the active one.<br>
*/

//####################################################################
//
// QextMdiTaskBarButton
//
//####################################################################
QextMdiTaskBarButton::QextMdiTaskBarButton(QextMdiTaskBar *pTaskBar,QextMdiChildView *win_ptr)
:QPushButton(pTaskBar),
 m_actualText("")
{
   setCheckable(true);
   m_pWindow      = win_ptr;
   this->setToolTip(win_ptr->caption());
   setFocusPolicy(Qt::NoFocus);
}

QextMdiTaskBarButton::~QextMdiTaskBarButton()
{
}

void QextMdiTaskBarButton::mousePressEvent( QMouseEvent* e)
{
   switch(e->button()) {
   case Qt::LeftButton:
      emit leftMouseButtonClicked( m_pWindow);
      emit clicked( m_pWindow);
      break;
   case Qt::RightButton:
      emit rightMouseButtonClicked( m_pWindow);
      break;
   default:
     emit clicked( m_pWindow);
      break;
   }
}

/** slot version of setText */
void QextMdiTaskBarButton::setNewText(const QString& s)
{
   setText( s);
   emit buttonTextChanged( 0);
}

void QextMdiTaskBarButton::setText(const QString& s)
{
   m_actualText = s;
   QPushButton::setText( s);
}

void QextMdiTaskBarButton::fitText(const QString& origStr, int newWidth)
{
   QPushButton::setText( m_actualText);

   int actualWidth = sizeHint().width();
   int realLetterCount = origStr.length();
   int newLetterCount = (newWidth * realLetterCount) / actualWidth;
   int w = newWidth+1;
   QString s = origStr;
   while((w > newWidth) && (newLetterCount >= 1)) {
      if( newLetterCount < realLetterCount) {
         if(newLetterCount > 3)
            s = origStr.left( newLetterCount/2) + "..." + origStr.right( newLetterCount/2);
         else {
            if(newLetterCount > 1)
               s = origStr.left( newLetterCount) + "..";
            else
               s = origStr.left(1);
         }
      }
      QFontMetrics fm = fontMetrics();
      w = fm.width(s);
      newLetterCount--;
   }

   QPushButton::setText( s);
}

QString QextMdiTaskBarButton::actualText() const
{
   return m_actualText;
}

//####################################################################
//
// QextMdiTaskBar
//
//####################################################################

QextMdiTaskBar::QextMdiTaskBar(QextMdiMainFrm *parent, Qt::ToolBarArea dock)
:  KToolBar( parent, "QextMdiTaskBar", /*honor_style*/ false, /*readConfig*/ true)
   ,m_pCurrentFocusedWindow(0)
   ,m_pStretchSpace(0)
   ,m_layoutIsPending(false)
   ,m_bSwitchedOn(false)
{
   m_pFrm = parent;
   m_pButtonList = new QList<QextMdiTaskBarButton*>;
//QT30   setFontPropagation(QWidget::SameFont);
   setMinimumWidth(1);
   setFocusPolicy(Qt::NoFocus);
   //parent->moveToolBar( this, dock); //XXX obsolete!
}

QextMdiTaskBar::~QextMdiTaskBar()
{
   qDeleteAll(*m_pButtonList);
   delete m_pButtonList;
}

QextMdiTaskBarButton * QextMdiTaskBar::addWinButton(QextMdiChildView *win_ptr)
{
   if( m_pStretchSpace) {
      delete m_pStretchSpace;
      m_pStretchSpace = 0L;
      //setStretchableWidget( 0L);
   }

   QextMdiTaskBarButton *b=new QextMdiTaskBarButton( this, win_ptr);
   QObject::connect( b, SIGNAL(clicked()), win_ptr, SLOT(setFocus()) ); 
   QObject::connect( b, SIGNAL(clicked(QextMdiChildView*)), this, SLOT(setActiveButton(QextMdiChildView*)) );
   QObject::connect( b, SIGNAL(leftMouseButtonClicked(QextMdiChildView*)), m_pFrm, SLOT(activateView(QextMdiChildView*)) );
   QObject::connect( b, SIGNAL(rightMouseButtonClicked(QextMdiChildView*)), m_pFrm, SLOT(taskbarButtonRightClicked(QextMdiChildView*)) );
   QObject::connect( b, SIGNAL(buttonTextChanged(int)), this, SLOT(layoutTaskBar(int)) );
   m_pButtonList->append(b);
   b->setChecked(true);
   b->setText(win_ptr->tabCaption());
   
   layoutTaskBar();
      
   m_pStretchSpace = new QLabel(this);
   m_pStretchSpace->setObjectName("empty");
   m_pStretchSpace->setText("");
   //setStretchableWidget( m_pStretchSpace);
   m_pStretchSpace->show();

   if (m_bSwitchedOn) {
      b->show();
      show();
   }
   return b;
}

void QextMdiTaskBar::removeWinButton(QextMdiChildView *win_ptr, bool haveToLayoutTaskBar)
{
   QextMdiTaskBarButton *b=getButton(win_ptr);
   if (b){
      m_pButtonList->removeAll(b);
      if( haveToLayoutTaskBar) layoutTaskBar();
   }
   if (m_pButtonList->count() == 0) {
      if (m_pStretchSpace != 0L) {
         delete m_pStretchSpace;
         m_pStretchSpace = 0L;
         hide();
      }
   }
}

void QextMdiTaskBar::switchOn(bool bOn)
{
   m_bSwitchedOn = bOn;
   if (!bOn) {
      hide();
   }
   else {
      if (m_pButtonList->count() > 0) {
         show();
      }
      else {
         hide();
      }
   }
}

QextMdiTaskBarButton * QextMdiTaskBar::getButton(QextMdiChildView *win_ptr)
{
   foreach(QextMdiTaskBarButton *b, *m_pButtonList) {
      if(b->m_pWindow == win_ptr)return b;
   }
   return 0;
}

QextMdiTaskBarButton * QextMdiTaskBar::getNextWindowButton(bool bRight,QextMdiChildView *win_ptr)
{
   if(bRight){
      QList<QextMdiTaskBarButton*>::iterator it;
      //for(QextMdiTaskBarButton *b=m_pButtonList->first();b;b=m_pButtonList->next()){
      for(it=m_pButtonList->begin(); it!=m_pButtonList->end(); ++it){
         QextMdiTaskBarButton *b = *it;
         if(b->m_pWindow == win_ptr){
            ++it;
            if (it==m_pButtonList->end()) b = m_pButtonList->first();
            else b = *it;
            //b = m_pButtonList->next();
            //if (!b) b = m_pButtonList->first();
            if(win_ptr != b->m_pWindow) return b;
            else return 0;
         }
      }
   } else {
      QList<QextMdiTaskBarButton*>::iterator it;
      //for(QextMdiTaskBarButton *b=m_pButtonList->first();b;b=m_pButtonList->next()){
      for(it=m_pButtonList->begin(); it!=m_pButtonList->end(); ++it){
         QextMdiTaskBarButton *b = *it;
         if(b->m_pWindow == win_ptr){
            --it;
            if (it==m_pButtonList->begin()) b = m_pButtonList->last();
            else b = *it;
            //b = m_pButtonList->prev();
            //if(!b)b = m_pButtonList->last();
            if(win_ptr != b->m_pWindow)return b;
            else return 0;
         }
      }
   }
   return 0;
}

void QextMdiTaskBar::setActiveButton(QextMdiChildView *win_ptr)
{
   QextMdiTaskBarButton* newPressedButton = 0L;
   QextMdiTaskBarButton* oldPressedButton = 0L;
   foreach(QextMdiTaskBarButton *b, *m_pButtonList){
      if( b->m_pWindow == win_ptr)
         newPressedButton = b;
      if( b->m_pWindow == m_pCurrentFocusedWindow)
         oldPressedButton = b;
   }
   
   if( newPressedButton != 0L && newPressedButton != oldPressedButton) {
         if( oldPressedButton != 0L)
            oldPressedButton->toggle();// switch off
         newPressedButton->toggle();   // switch on
         m_pCurrentFocusedWindow = win_ptr;
   }
}

void QextMdiTaskBar::layoutTaskBar( int taskBarWidth)
{
   if (m_layoutIsPending) return;
   m_layoutIsPending = true;

   if( !taskBarWidth)
      // no width is given
      taskBarWidth = width();

   // calculate current width of all taskbar buttons
   int allButtonsWidth = 0;
   foreach(QextMdiTaskBarButton *b, *m_pButtonList){
      allButtonsWidth += b->width();
   }
   
   // calculate actual width of all taskbar buttons
   int allButtonsWidthHint = 0;
   foreach(QextMdiTaskBarButton *b, *m_pButtonList){
      QFontMetrics fm = b->fontMetrics();
      QString s = b->actualText();
      QSize sz = fm.size(Qt::TextShowMnemonic, s);
      int w = sz.width()+6;
      int h = sz.height()+sz.height()/8+10;
      w += h;
      allButtonsWidthHint += w;
   } 

   // if there's enough space, use actual width
   int buttonCount = m_pButtonList->count();
   int tbHandlePixel;
   tbHandlePixel = style()->pixelMetric(QStyle::PM_DockWidgetHandleExtent, NULL, this);
   int buttonAreaWidth = taskBarWidth - tbHandlePixel - style()->pixelMetric(QStyle::PM_DefaultFrameWidth, NULL, this) - 5;
   if( ((allButtonsWidthHint) <= buttonAreaWidth) || (width() < parentWidget()->width())) {
      foreach(QextMdiTaskBarButton *b, *m_pButtonList){
         b->setText( b->actualText());
         if (b->width() != b->sizeHint().width()) {
            b->setFixedWidth( b->sizeHint().width());
            b->show();
         }
      }
   }
   else {
      // too many buttons for actual width
      int newButtonWidth;
      if( buttonCount != 0)
         newButtonWidth = buttonAreaWidth / buttonCount;
      else
         newButtonWidth = 0;

      if( orientation() == Qt::Vertical)
         newButtonWidth = 80;

      if(newButtonWidth > 0)
         foreach(QextMdiTaskBarButton *b, *m_pButtonList){
            b->fitText( b->actualText(), newButtonWidth);
            if (b->width() != newButtonWidth) {
               b->setFixedWidth( newButtonWidth);
               b->show();
            }
         }
   }
   m_layoutIsPending = false;
}

void QextMdiTaskBar::resizeEvent( QResizeEvent* rse)
{
   if (!m_layoutIsPending) {
      if (m_pButtonList->count() != 0) {
         layoutTaskBar( rse->size().width());
      }
   }
   KToolBar::resizeEvent( rse);
}

#ifndef NO_INCLUDE_MOCFILES
#include "qextmditaskbar.moc"
#endif
