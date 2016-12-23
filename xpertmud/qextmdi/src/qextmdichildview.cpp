//----------------------------------------------------------------------------
//    filename             : qextmdichildview.cpp
//----------------------------------------------------------------------------
//    Project              : Qt MDI extension
//
//    begin                : 07/1999       by Szymon Stefanek as part of kvirc
//                                         (an IRC application)
//    changes              : 09/1999       by Falk Brettschneider to create a
//                           -06/2000      stand-alone Qt extension set of
//                                         classes and a Qt-based library
//    patches              : 02/2000       by Massimo Morin (mmorin@schedsys.com)
//                           */2000        by Lars Beikirch (Lars.Beikirch@gmx.net)
//                           02/2001       by Eva Brucherseifer (eva@rt.e-technik.tu-darmstadt.de)
//
//    copyright            : (C) 1999-2001 by Szymon Stefanek (stefanek@tin.it)
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

#include <QDateTime>

#include "qextmdichildview.h"
#include "qextmdimainfrm.h"
#include "qextmdichildfrm.h"
#include "qextmdidefines.h"

#include <QKeyEvent>

//============ QextMdiChildView ============//

QextMdiChildView::QextMdiChildView( const QString& caption, QWidget* parentWidget, const QString name, Qt::WindowFlags f)
: QWidget(parentWidget, f)
  ,m_focusedChildWidget(0L)
  ,m_firstFocusableChildWidget(0L)
  ,m_lastFocusableChildWidget(0L)
  ,m_stateChanged(true)
  ,m_bToolView(false)
  ,m_bInterruptActivation(false)
  ,m_bMainframesActivateViewIsPending(false)
  ,m_bFocusInEventIsPending(false)
{
   setObjectName(name);
   setGeometry( 0, 0, 0, 0);  // reset
   if(caption != 0L) {
      m_szCaption = caption;
   }
   else {
      m_szCaption = QString(tr("Unnamed"));
   }
   m_sTabCaption = m_szCaption;

   setFocusPolicy(Qt::ClickFocus);

   installEventFilter(this);
}

//============ QextMdiChildView ============//

QextMdiChildView::QextMdiChildView( QWidget* parentWidget, const QString name, Qt::WindowFlags f)
: QWidget(parentWidget, f)
  ,m_focusedChildWidget(0L)
  ,m_firstFocusableChildWidget(0L)
  ,m_lastFocusableChildWidget(0L)
  ,m_stateChanged(true)
  ,m_bToolView(false)
  ,m_bInterruptActivation(false)
  ,m_bMainframesActivateViewIsPending(false)
  ,m_bFocusInEventIsPending(false)
{
   setObjectName(name);
   setGeometry( 0, 0, 0, 0);  // reset
   m_szCaption = QString(tr("Unnamed"));
   m_sTabCaption = m_szCaption;

   setFocusPolicy(Qt::ClickFocus);

   installEventFilter(this);
}

//============ ~QextMdiChildView ============//

QextMdiChildView::~QextMdiChildView()
{
}

//============== internal geometry ==============//

QRect QextMdiChildView::internalGeometry() const
{
   if(mdiParent()) { // is attached
      // get the client area coordinates inside the MDI child frame
      QRect    posInFrame = geometry();
      // map these values to the parent of the MDI child frame
      // (this usually is the MDI child area) and return
      QPoint ptTopLeft = mdiParent()->mapToParent(posInFrame.topLeft());
      QSize sz = size();
      return QRect(ptTopLeft, sz);
   }
   else {
      QRect    geo = geometry();
      QRect    frameGeo = externalGeometry();
      return QRect(frameGeo.x(), frameGeo.y(), geo.width(), geo.height());
//      return geometry();
   }
}

//============== set internal geometry ==============//

void QextMdiChildView::setInternalGeometry(const QRect& newGeometry)
{
   if(mdiParent()) { // is attached
      // retrieve the frame size
      QRect    geo      = internalGeometry();
      QRect    frameGeo = externalGeometry();
      int      nFrameSizeTop  = geo.y() - frameGeo.y();
      int      nFrameSizeLeft = geo.x() - frameGeo.x();

      // create the new geometry that is accepted by the QWidget::setGeometry() method
      QRect    newGeoQt;
      newGeoQt.setX(newGeometry.x()-nFrameSizeLeft);
      newGeoQt.setY(newGeometry.y()-nFrameSizeTop);

      newGeoQt.setWidth(newGeometry.width()+nFrameSizeLeft+QEXTMDI_MDI_CHILDFRM_DOUBLE_BORDER/2);
      newGeoQt.setHeight(newGeometry.height()+nFrameSizeTop+QEXTMDI_MDI_CHILDFRM_DOUBLE_BORDER/2);
//      newGeoQt.setWidth(newGeometry.width()+QEXTMDI_MDI_CHILDFRM_DOUBLE_BORDER);
//      newGeoQt.setHeight(newGeometry.height()+mdiParent()->captionHeight()+QEXTMDI_MDI_CHILDFRM_DOUBLE_BORDER);

      // set the geometry
      mdiParent()->setGeometry(newGeoQt);
   }
   else {
      // retrieve the frame size
      QRect    geo      = internalGeometry();
      QRect    frameGeo = externalGeometry();
      int      nFrameSizeTop  = geo.y() - frameGeo.y();
      int      nFrameSizeLeft = geo.x() - frameGeo.x();

      // create the new geometry that is accepted by the QWidget::setGeometry() method
      QRect    newGeoQt;
#if defined(_OS_WIN32_) || defined(Q_OS_WIN32)
      newGeoQt.setX(newGeometry.x()-nFrameSizeLeft+frameGeo.width()-geo.width()-QEXTMDI_MDI_CHILDFRM_DOUBLE_BORDER+1);
      newGeoQt.setY(newGeometry.y()-nFrameSizeTop+frameGeo.height()-geo.height()-QEXTMDI_MDI_CHILDFRM_DOUBLE_BORDER+1);
#else
      newGeoQt.setX(newGeometry.x()-nFrameSizeLeft);
      newGeoQt.setY(newGeometry.y()-nFrameSizeTop);
#endif
      newGeoQt.setWidth(newGeometry.width());
      newGeoQt.setHeight(newGeometry.height());

      // set the geometry
      setGeometry(newGeoQt);
   }
}

//============== external geometry ==============//

QRect QextMdiChildView::externalGeometry() const
{
   return mdiParent() ? mdiParent()->frameGeometry() : frameGeometry();
}

//============== set external geometry ==============//

void QextMdiChildView::setExternalGeometry(const QRect& newGeometry)
{
   if(mdiParent()) { // is attached
       mdiParent()->setGeometry(newGeometry);
   }
   else {
      // retrieve the frame size
      QRect    geo      = internalGeometry();
      QRect    frameGeo = externalGeometry();
      int      nTotalFrameWidth = frameGeo.width() - geo.width();
      int      nTotalFrameHeight = frameGeo.height() - geo.height();
      int      nFrameSizeTop  = geo.y() - frameGeo.y();
      int      nFrameSizeLeft = geo.x() - frameGeo.x();

      // create the new geometry that is accepted by the QWidget::setGeometry() method
      // not attached => the window system makes the frame
      QRect    newGeoQt;
      newGeoQt.setX(newGeometry.x()+nFrameSizeLeft);
      newGeoQt.setY(newGeometry.y()+nFrameSizeTop);
      newGeoQt.setWidth(newGeometry.width()-nTotalFrameWidth);
      newGeoQt.setHeight(newGeometry.height()-nTotalFrameHeight);

      // set the geometry
      setGeometry(newGeoQt);
   }
}

//============== minimize ==============//

void QextMdiChildView::minimize(bool bAnimate)
{
   if(mdiParent()) {
      if(!isMinimized()) {
         mdiParent()->setState(QextMdiChildFrm::Minimized,bAnimate);
      }
   } 
   else {
      showMinimized();
   }
}

void QextMdiChildView::showMinimized()
{
   //qDebug("is minimized now");
   emit isMinimizedNow();
   QWidget::showMinimized();
}

//slot:
void QextMdiChildView::minimize() { minimize(true); }

//============= maximize ==============//

void QextMdiChildView::maximize(bool bAnimate)
{
   if(mdiParent()) {
      if(!isMaximized()) {
         mdiParent()->setState(QextMdiChildFrm::Maximized, bAnimate);
         emit mdiParentNowMaximized(true);
      }
   }
   else {
      showMaximized();
   }
}

void QextMdiChildView::showMaximized()
{
   //qDebug("is maximized now");
   emit isMaximizedNow();
   QWidget::showMaximized();
}

//slot:
void QextMdiChildView::maximize() { maximize(true); }

//============== restoreGeometry ================//

QRect QextMdiChildView::restoreGeometry()
{
   if(mdiParent()) {
      return mdiParent()->restoreGeometry();
   }
   else {
      // XXX not really supported, may be we must use Windows or X11 funtions
      return geometry();
   }
}

//============== setRestoreGeometry ================//

void  QextMdiChildView::setRestoreGeometry(const QRect& newRestGeo)
{
   if(mdiParent()) {
      mdiParent()->setRestoreGeometry(newRestGeo);
   }
   else {
      // XXX not supported, may be we must use Windows or X11 funtions
   }
}

//============== attach ================//

void QextMdiChildView::attach()
{
   emit attachWindow(this,true);
}

//============== detach =================//

void QextMdiChildView::detach()
{
  // can't detach maximized or minimized
  // TODO: save the state and restore
  // it after detaching?
  if(mdiParent()) {
    mdiParent()->restorePressed();
  }
   emit detachWindow(this, true);
}

//=============== isMinimized ? =================//

bool QextMdiChildView::isMinimized() const
{
   if(mdiParent()) {
      return (mdiParent()->state() == QextMdiChildFrm::Minimized);
   }
   else {
      return QWidget::isMinimized();
   }
}

//============== isMaximized ? ==================//

bool QextMdiChildView::isMaximized() const
{
   if(mdiParent()) {
      return (mdiParent()->state() == QextMdiChildFrm::Maximized);
   }
   else {
      return QWidget::isMaximized();
   }
}

//============== restore ================//

void QextMdiChildView::restore()
{
   if(mdiParent()) {
      if(isMaximized()) {
         emit mdiParentNowMaximized(false);
      }
      if(isMinimized()||isMaximized()) {
         mdiParent()->setState(QextMdiChildFrm::Normal);
      }
   }
   else {
      showNormal();
   }
}

void QextMdiChildView::showNormal()
{
   //qDebug("is restored now");
   emit isRestoredNow();
   QWidget::showNormal();
}

//=============== youAreAttached ============//

void QextMdiChildView::youAreAttached(QextMdiChildFrm *lpC)
{
   lpC->setCaption(m_szCaption);

   emit isAttachedNow();
}

//================ youAreDetached =============//

void QextMdiChildView::youAreDetached()
{
   setCaption(m_szCaption);

   setTabCaption(m_sTabCaption);
   if(myIconPtr()) setWindowIcon(*(myIconPtr()));
   setFocusPolicy(Qt::StrongFocus);

   emit isDetachedNow();
}

//================ setCaption ================//
// this set the caption of only the window
void QextMdiChildView::setCaption(const QString& szCaption)
{
  // this will work only for window
   m_szCaption=szCaption;
   if(mdiParent()) {
     mdiParent()->setCaption(m_szCaption);
   }
   else {
     //  sorry have to call the parent one
     QWidget::setWindowTitle(m_szCaption);
   }
   emit windowCaptionChanged(m_szCaption);
}

//============== closeEvent ================//
void QextMdiChildView::closeEvent(QCloseEvent * /*e */)
{
  // we only get _one_ close event from the windowing
  // system, then never again...
  // so before we got this right, we simply ignore the event
  //   e->ignore(); //we ignore the event , and then close later if needed.
  //   emit childWindowCloseRequest(this);
  //attach();
}

//================ myIconPtr =================//

QPixmap* QextMdiChildView::myIconPtr()
{
   return 0;
}

//============= focusInEvent ===============//

void QextMdiChildView::focusInEvent(QFocusEvent *e)
{
   QWidget::focusInEvent(e);
   return;

   // every widget get a focusInEvent when a popup menu is opened!?! -> maybe bug of QT
   if(e && ((e->reason())==Qt::PopupFocusReason)) {
      return;
   }

   m_bFocusInEventIsPending = true;
   activate();
   m_bFocusInEventIsPending = false;

   emit gotFocus(this);
}

//============= activate ===============//

void QextMdiChildView::activate()
{
   // avoid circularity
   static bool s_bActivateIsPending = false;
   if(s_bActivateIsPending) {
      return;
   }
   s_bActivateIsPending = true;

   // raise the view and push the taskbar button
   if(!m_bMainframesActivateViewIsPending) {
     emit focusInEventOccurs( this);
   }

   // if this method was called directly, check if the mainframe wants that we interrupt
   if(m_bInterruptActivation) {
      m_bInterruptActivation = false;
   }
   else {
      if(!m_bFocusInEventIsPending) {
	//setFocus();
      }
      //qDebug("QextMdiChildView::activate() called!");
      emit activated(this);
   }

   if(m_focusedChildWidget != 0L) {
     //m_focusedChildWidget->setFocus();
   }
   else {
      if(m_firstFocusableChildWidget != 0L) {
	//m_firstFocusableChildWidget->setFocus();
         m_focusedChildWidget = m_firstFocusableChildWidget;
      }
   }
   s_bActivateIsPending = false;
}

//============= focusOutEvent ===============//

void QextMdiChildView::focusOutEvent(QFocusEvent* e)
{
   QWidget::focusOutEvent(e);

   emit lostFocus( this);
}

//============= resizeEvent ===============//

void QextMdiChildView::resizeEvent(QResizeEvent* e)
{
   QWidget::resizeEvent( e);

   if(m_stateChanged) {
      m_stateChanged = false;
      if(isMaximized()) {
         //qDebug("is maximized now");
         emit isMaximizedNow();
      }
      else if(isMinimized()) {
         //qDebug("is minimized now");
         emit isMinimizedNow();
      }
      else {   // is restored
         //qDebug("is restored now");
         emit isRestoredNow();
      }
   }
}

void QextMdiChildView::slot_childDestroyed()
{
   // do what we do if a child is removed

   // if we lost a child we uninstall ourself as event filter for the lost 
   // child and its children
   const QObject* pLostChild = QObject::sender();
   if (pLostChild == NULL) return;
   QWidgetList list = pLostChild->findChildren<QWidget*>();
   foreach(QWidget* widg, list) {
      widg->removeEventFilter(this);
      if(m_firstFocusableChildWidget == widg) {
         m_firstFocusableChildWidget = 0L;   // reset first widget
      }
      if(m_lastFocusableChildWidget == widg) {
         m_lastFocusableChildWidget = 0L;    // reset last widget
      }
      if(m_focusedChildWidget == widg) {
         m_focusedChildWidget = 0L;          // reset focused widget
      }
   }
}

//============= eventFilter ===============//

bool QextMdiChildView::eventFilter(QObject *obj, QEvent *e )
{
   if(e->type() == QEvent::KeyPress && isAttached()) {
      QKeyEvent* ke = (QKeyEvent*) e;
      if(ke->key() == Qt::Key_Tab) {
         //qDebug("ChildView %i::eventFilter - TAB from %s (%s)", this, obj->name(), obj->className());
         QWidget* w = (QWidget*) obj;
         if((w->focusPolicy() == Qt::StrongFocus) ||
            (w->focusPolicy() == Qt::TabFocus   ) ||
            (w->focusPolicy() == Qt::WheelFocus ))
         {
            //qDebug("  accept TAB as setFocus change");
            if(m_lastFocusableChildWidget != 0) {
               if(w == m_lastFocusableChildWidget) {
                  if(w != m_firstFocusableChildWidget) {
                     //qDebug("  TAB: setFocus to first");
                     //m_firstFocusableChildWidget->setFocus();
                     //qDebug("  TAB: focus is set to first");
                  }
               }
            }
         }
      }
   }
   else if(e->type() == QEvent::FocusIn) {
      if(obj->isWidgetType()) {
         QWidgetList list = findChildren<QWidget*>();
         if (list.contains((QWidget*)obj)) {
            m_focusedChildWidget = (QWidget*)obj;
         }
      }
      if (!isAttached()) {   // is toplevel, for attached views activation is done by main frame event filter
         static bool m_bActivationIsPending = false;
         if(!m_bActivationIsPending) {
            m_bActivationIsPending = true;
            activate(); // sets the focus
            m_bActivationIsPending = false;
         }
      }
   }
   else if (e->type() == QEvent::ChildRemoved) {
      // if we lost a child we uninstall ourself as event filter for the lost
      // child and its children
      QObject* pLostChild = ((QChildEvent*)e)->child();
      if (pLostChild != 0L) {
         QWidgetList list = pLostChild->findChildren<QWidget*>();
         list.insert(0, (QWidget*)pLostChild);        // add the lost child to the list too, just to save code
         foreach(QWidget* widg, list) {
            widg->removeEventFilter(this);
            if((widg->focusPolicy() == Qt::StrongFocus) ||
               (widg->focusPolicy() == Qt::TabFocus   ) ||
               (widg->focusPolicy() == Qt::WheelFocus ))
            {
               if(m_firstFocusableChildWidget == widg) {
                  m_firstFocusableChildWidget = 0L;   // reset first widget
               }
               if(m_lastFocusableChildWidget == widg) {
                  m_lastFocusableChildWidget = 0L;    // reset last widget
               }
            }
         }
      }
   }
   else if (e->type() == QEvent::ChildAdded) {
      // if we got a new child and we are attached to the MDI system we
      // install ourself as event filter for the new child and its children
      // (as we did when we were added to the MDI system).
      QObject* pNewChild = ((QChildEvent*)e)->child();
      if ((pNewChild != 0L) && (pNewChild->isWidgetType()))
      {
         QWidget* pNewWidget = (QWidget*)pNewChild;
         if (pNewWidget->windowFlags().testFlag(Qt::Dialog) && pNewWidget->testAttribute(Qt::WA_ShowModal))
             return false;
         QWidgetList list = pNewWidget->findChildren<QWidget*>();
         list.insert(0, (QWidget*)pNewChild);         // add the new child to the list too, just to save code
         foreach(QWidget* widg, list) {
            widg->installEventFilter(this);
            connect(widg, SIGNAL(destroyed()), this, SLOT(slot_childDestroyed()));
            if((widg->focusPolicy() == Qt::StrongFocus) ||
               (widg->focusPolicy() == Qt::TabFocus   ) ||
               (widg->focusPolicy() == Qt::WheelFocus ))
            {
               if(m_firstFocusableChildWidget == 0) {
                  m_firstFocusableChildWidget = widg;  // first widge
               }
               m_lastFocusableChildWidget = widg; // last widget
               //qDebug("*** %s (%s)",widg->name(),widg->className());
            }
         }
      }
   }
   return false;                           // standard event processing
}

/** Switches interposing in event loop of all current child widgets off. */
void QextMdiChildView::removeEventFilterForAllChildren()
{
   QWidgetList list = findChildren<QWidget*>();
   foreach(QWidget* widg, list){
      widg->removeEventFilter(this);
   }
}

QWidget* QextMdiChildView::focusedChildWidget()
{
   return m_focusedChildWidget;
}

void QextMdiChildView::setFirstFocusableChildWidget(QWidget* firstFocusableChildWidget)
{
   m_firstFocusableChildWidget = firstFocusableChildWidget;
}

void QextMdiChildView::setLastFocusableChildWidget(QWidget* lastFocusableChildWidget)
{
   m_lastFocusableChildWidget = lastFocusableChildWidget;
}
/** Set a new value of  the task bar button caption  */
void QextMdiChildView::setTabCaption (const QString& stbCaption) {

  m_sTabCaption = stbCaption;
  emit tabCaptionChanged(m_sTabCaption);

}
void QextMdiChildView::setMDICaption (const QString& caption) {
  setCaption(caption);
  setTabCaption(caption);
}

/** sets an ID  */
void QextMdiChildView::setWindowMenuID( int id)
{
   m_windowMenuID = id;
}

//============= slot_clickedInWindowMenu ===============//

/** called if someone click on the "Window" menu item for this child frame window */
void QextMdiChildView::slot_clickedInWindowMenu()
{
   emit clickedInWindowMenu( m_windowMenuID);
}

//============= slot_clickedInDockMenu ===============//

/** called if someone click on the "Dock/Undock..." menu item for this child frame window */
void QextMdiChildView::slot_clickedInDockMenu()
{
   emit clickedInDockMenu( m_windowMenuID);
}

//============= setMinimumSize ===============//

void QextMdiChildView::setMinimumSize( int minw, int minh)
{
   QWidget::setMinimumSize( minw, minh);
   if ( (mdiParent() != 0L) && (mdiParent()->state() != QextMdiChildFrm::Minimized) ) {
      mdiParent()->setMinimumSize( minw + QEXTMDI_MDI_CHILDFRM_DOUBLE_BORDER,
                                   minh + QEXTMDI_MDI_CHILDFRM_DOUBLE_BORDER
                                        + QEXTMDI_MDI_CHILDFRM_SEPARATOR
                                        + mdiParent()->captionHeight());
   }
}

//============= setMaximumSize ===============//

void QextMdiChildView::setMaximumSize( int maxw, int maxh)
{
   if ( (mdiParent() != 0L) && (mdiParent()->state() == QextMdiChildFrm::Normal) ) {
      int w = maxw + QEXTMDI_MDI_CHILDFRM_DOUBLE_BORDER;
      if(w > QWIDGETSIZE_MAX) { w = QWIDGETSIZE_MAX; }
      int h = maxh + QEXTMDI_MDI_CHILDFRM_DOUBLE_BORDER + QEXTMDI_MDI_CHILDFRM_SEPARATOR + mdiParent()->captionHeight();
      if(h > QWIDGETSIZE_MAX) { h = QWIDGETSIZE_MAX; }
      mdiParent()->setMaximumSize( w, h);
   }
   QWidget::setMaximumSize( maxw, maxh);
}

//============= show ===============//

void QextMdiChildView::show()
{
   QWidget* pParent = mdiParent();
   if(pParent != 0L) {
      pParent->show();
   }
   QWidget::show();
}

//============= hide ===============//

void QextMdiChildView::hide()
{
   if(mdiParent() != 0L) {
      mdiParent()->hide();
   }
   QWidget::hide();
}

//============= raise ===============//

void QextMdiChildView::raise()
{
   if(mdiParent() != 0L) {
      mdiParent()->raise();
      // XXXCTL what's about Z-order? L.B. 2002/02/10
   }
   QWidget::raise();
}


#ifndef NO_INCLUDE_MOCFILES
#include "qextmdichildview.moc"
#endif
