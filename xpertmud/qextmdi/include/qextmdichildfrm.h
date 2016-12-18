//----------------------------------------------------------------------------
//    filename             : qextmdichildfrm.h
//----------------------------------------------------------------------------
//    Project              : Qt MDI extension
//
//    begin                : 07/1999       by Szymon Stefanek as part of kvirc
//                                         (an IRC application)
//    changes              : 09/1999       by Falk Brettschneider to create an
//                           - 06/2000     stand-alone Qt extension set of
//                                         classes and a Qt-based library
//    patches              : */2000        Lars Beikirch (Lars.Beikirch@gmx.net)
//
//    copyright            : (C) 1999-2000 by Falk Brettschneider
//                                         and
//                                         Szymon Stefanek (stefanek@tin.it)
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
//------------------------------------------------------------------------------
#ifndef _QEXTMDICHILDFRM_H_
#define _QEXTMDICHILDFRM_H_

#include <QList>
#include <QPixMap>
#include <QMenu>
#include <QPushButton>
#include <QToolButton>
#include <QLabel>
#include <QDateTime>
#include <QLayout>

#include "qextmdichildfrmcaption.h"

class QextMdiChildArea;
class QextMdiChildView;

//==============================================================================
/**
  * @short Internal class, only used on Win32.
  * This class provides a label widget that can process mouse click events.
  */
class DLL_IMP_EXP_QEXTMDICLASS QextMdiWin32IconButton : public QLabel
{
   Q_OBJECT
public:
   QextMdiWin32IconButton( QWidget* parent, const char* name = 0);
   virtual void mousePressEvent( QMouseEvent*);

signals:
   void pressed();
};

//==============================================================================
/* some special events, see qetxtmdidefines.h
*/
//------------------------------------------------------------------------------

class QCustomEvent : public QEvent {
public:
    QCustomEvent (QEvent::Type type, void * data = NULL) : QEvent(type) {
       m_data = data;
    }
    void * data() {return m_data;}
protected:
    void *m_data;
};

/**
 * @short a QCustomEvent for move
 * This special event will be useful, to inform view about child frame event.
 */
class DLL_IMP_EXP_QEXTMDICLASS QextMdiChildFrmMoveEvent : public QCustomEvent
{
public:
   /**
   * Constructs a new customer event of type QEvent::User + QextMdi::EV_DragMove
   */
   QextMdiChildFrmMoveEvent( QMoveEvent *e) : QCustomEvent( QEvent::Type(QEvent::User + int(QextMdi::EV_Move)), e) {};
};

//------------------------------------------------------------------------------
/**
 * @short a QCustomEvent for begin of dragging
 * This special event will be useful, to inform view about child frame event.
 */
class DLL_IMP_EXP_QEXTMDICLASS QextMdiChildFrmDragBeginEvent : public QCustomEvent
{
public:
   /**
   * Constructs a new customer event of type QEvent::User + QextMdi::EV_DragBegin
   */
   QextMdiChildFrmDragBeginEvent( QMouseEvent *e) : QCustomEvent( QEvent::Type(QEvent::User + int(QextMdi::EV_DragBegin)), e) {};
};

//------------------------------------------------------------------------------
/**
 * @short a QCustomEvent for end of dragging
 * This special event will be useful, to inform view about child frame event.
 */
class DLL_IMP_EXP_QEXTMDICLASS QextMdiChildFrmDragEndEvent : public QCustomEvent
{
public:
   /**
   * Constructs a new customer event of type QEvent::User + QextMdi::EV_DragEnd
   */
   QextMdiChildFrmDragEndEvent( QMouseEvent *e) : QCustomEvent( QEvent::Type(QEvent::User + int(QextMdi::EV_DragEnd)), e) {};
};

//------------------------------------------------------------------------------
/**
 * @short a QCustomEvent for begin of resizing
 * This special event will be useful, to inform view about child frame event.
 */
class DLL_IMP_EXP_QEXTMDICLASS QextMdiChildFrmResizeBeginEvent : public QCustomEvent
{
public:
   /**
   * Constructs a new customer event of type QEvent::User + QextMdi::EV_ResizeBegin
   */
   QextMdiChildFrmResizeBeginEvent( QMouseEvent *e) : QCustomEvent( QEvent::Type(QEvent::User + int(QextMdi::EV_ResizeBegin)), e) {};
};

//------------------------------------------------------------------------------
/**
 * @short a QCustomEvent for end of resizing
 * This special event will be useful, to inform view about child frame event.
 */
class DLL_IMP_EXP_QEXTMDICLASS QextMdiChildFrmResizeEndEvent : public QCustomEvent
{
public:
   /**
   * Constructs a new customer event of type QEvent::User + QextMdi::EV_ResizeEnd
   */
   QextMdiChildFrmResizeEndEvent( QMouseEvent *e) : QCustomEvent( QEvent::Type(QEvent::User + int(QextMdi::EV_ResizeEnd)), e) {};
};

//==============================================================================
/**
  * @short Internal class.
  * It's an MDI child frame widget. It contains a view widget and a frame caption. Usually you derive from its view.
  */
//------------------------------------------------------------------------------
class DLL_IMP_EXP_QEXTMDICLASS QextMdiChildFrm : public QFrame
{
   friend class QextMdiChildArea;
   friend class QextMdiChildFrmCaption;
   Q_OBJECT

// attributes  
public:
   enum MdiWindowState { Normal,Maximized,Minimized };
                           //positions same in h and cpp for fast order check
   QextMdiChildView*       m_pClient;

protected:
   QextMdiChildArea*       m_pManager;
   QextMdiChildFrmCaption* m_pCaption;
   QextMdiWin32IconButton* m_pWinIcon;
   QToolButton*            m_pUnixIcon;
   QToolButton*            m_pMinimize;
   QToolButton*            m_pMaximize;
   QToolButton*            m_pClose;
   QToolButton*            m_pUndock;
   MdiWindowState          m_state;
   QRect                   m_restoredRect;
   int                     m_iResizeCorner;
   int                     m_iLastCursorCorner;
   bool                    m_bResizing;
   bool                    m_bDragging;
   QPixmap*                m_pIconButtonPixmap;
   QPixmap*                m_pMinButtonPixmap;
   QPixmap*                m_pMaxButtonPixmap;
   QPixmap*                m_pRestoreButtonPixmap;
   QPixmap*                m_pCloseButtonPixmap;
   QPixmap*                m_pUndockButtonPixmap;
   /** 
   * Every child frame window has an temporary ID in the Window menu of the child area. 
   */
   int                     m_windowMenuID;
   /** 
   * Imitates a system menu for child frame windows 
   */
   QPopupMenu*             m_pSystemMenu;
   QSize                   m_oldClientMinSize;
   QSize                   m_oldClientMaxSize;
//   QLayout::ResizeMode     m_oldLayoutResizeMode;
   QTime                   m_timeMeasure;

// methods
public:
   /**
   * Creates a new QextMdiChildFrm class.<br>
   */
   QextMdiChildFrm(QextMdiChildArea *parent);
   /**
   * Delicato : destroys this QextMdiChildFrm
   * If a child is still here managed (no recreation was made) it is destroyed too.
   */
   ~QextMdiChildFrm();  
   /**
   * Reparents the widget w to this QextMdiChildFrm (if this is not already done)
   * Installs an event filter to catch focus events.
   * Resizes this mdi child in a way that the child fits perfectly in.
   */
   void setClient(QextMdiChildView *w);
   /**
   * Reparents the client widget to 0 (desktop), moves with an offset from the original position
   * Removes the event filter.
   */
   void unsetClient( QPoint positionOffset = QPoint(0,0));
   /**
   * Sets the window icon pointer.
   */
   void setIcon(const QPixmap &pxm);
   /**
   * Returns the child frame icon.
   */
   QPixmap* icon();
   /**
   * Enables or disables the close button
   */
   void enableClose(bool bEnable);
   /**
   * Sets the caption of this window
   */
   void setCaption(const QString& text);
   /**
   * Returns the caption of this mdi child.
   * Cool to have it inline...
   */
   const QString& caption(){ return m_pCaption->m_szCaption; };
   /**
   * Minimizes , Maximizes or restores the window.
   */
   void setState(MdiWindowState state,bool bAnimate=true);
   /**
   * Returns the current state of the window
   * Cool to have it inline...
   */
   inline MdiWindowState state(){ return m_state; };
   /**
   * Returns the inner client area of the parent of this (which is QextMdiChildArea).
   */
   QRect mdiAreaContentsRect() const;
   /**
   * Returns the geometry that will be restored by calling restore().
   */
   QRect restoreGeometry() const;
   /**
   * Sets the geometry that will be restored by calling restore().
   */
   void  setRestoreGeometry(const QRect& newRestGeo);
   /**
   * Forces updating the rects of the caption and so...
   * It may be useful when setting the mdiCaptionFont of the MdiManager
   */
   void updateRects(){ resizeEvent(0); };
   /**
   * Returns the system menu.
   */
   QPopupMenu* systemMenu();
   /**
   * Returns the caption bar height 
   */
   inline int captionHeight() { return m_pCaption->height(); };
   inline void showCaption() { m_pCaption->show(); doResize(); }
   inline void hideCaption() { m_pCaption->hide(); doResize(); }
   /**
   * sets new raise behaviour and pixmaps of the buttons depending on the current decoration style
   */
   void redecorateButtons();

   /**
   * returns the mouse state "In Drag"
   */
   bool isInDrag() const { return m_bDragging; }
   /**
   * returns the mouse state "In Resize"
   */
   bool isInResize() const { return m_bResizing; }
   /** Internally called from the signal focusInEventOccurs.
   * It raises the MDI childframe to the top of all other MDI child frames and sets the focus on it.
   */
   void raiseAndActivate();

   /** 
   * Sets the minimum size of the widget to w by h pixels.
   * It extends it's base clase method in a way that the minimum size of
   * the child area will be set additionally if the view is maximized. 
   */
   virtual void setMinimumSize ( int minw, int minh );

public slots:
   /**
   *
   */
   void slot_resizeViaSystemMenu();

protected:
   /** Reimplemented from its base class.
   * Resizes the captionbar, relayouts the position of the system buttons,
   * and calls resize for its embedded client @ref QextMdiChildView with the proper size 
   */
   virtual void resizeEvent(QResizeEvent *);
   /** Reimplemented from its base class.
   * Detects if the mouse is on the edge of window and what resize cursor must be set.
   * Calls QextMdiChildFrm::resizeWindow if it is in m_bResizing. 
   */
   virtual void mouseMoveEvent(QMouseEvent *e);
   /** Reimplemented from its base class.
   * Colours the caption, raises the childfrm widget and
   * turns to resize mode if it is on the edge (resize-sensitive area) 
   */
   virtual void mousePressEvent(QMouseEvent *e);
   /** Reimplemented from its base class.
   * Sets a normal cursor and leaves the resize mode. 
   */
   virtual void mouseReleaseEvent(QMouseEvent *);
   /** Reimplemented from its base class.
   * give its child view the chance to notify a childframe move... that's why it sends
   * a @ref QextMdiChildMovedEvent to the embedded @ref QextMdiChildView . 
   */
   virtual void moveEvent(QMoveEvent* me);
   /** 
   * Reimplemented from its base class. If not in resize mode, it sets the mouse cursor to normal appearance. 
   */
   virtual void leaveEvent(QEvent *);
   /** Reimplemented from its base class.
   * Additionally it catches<UL>
   * <LI>the client's mousebutton press events and raises and activates the childframe then</LI>
   * <LI>the client's resize event and resizes its childframe widget (this) as well</LI></UL> 
   */
   virtual bool eventFilter(QObject*, QEvent*);//focusInEvent(QFocusEvent *);
   /** Calculates the new geometry from the new mouse position given as parameters
   * and calls QextMdiChildFrm::setGeometry 
   */
   void resizeWindow(int resizeCorner, int x, int y);
   /** 
   * Override the cursor appearance depending on the widget corner given as parameter 
   */
   void setResizeCursor(int resizeCorner);
   /** 
   * changes from the resize cursor to the normal (previous) cursor
   */
   void unsetResizeCursor();
   /** That means to show a mini window showing the childframe's caption bar, only.
   * It cannot be resized. 
   */
   virtual void switchToMinimizeLayout();
   /**
   * Does the actual resize. Called from various places but from resizeEvent in general.
   */
   void doResize();

public slots:
   /** 
   * Handles a click on the Maximize button 
   */
   void maximizePressed();
   /** 
   * Handles a click on the Restore (Normalize) button 
   */
   void restorePressed();
   /** 
   * Handles a click on the Minimize button. 
   */
   void minimizePressed();
   /** 
   * Handles a click on the Close button. 
   */
   void closePressed();
   /** 
   * Handles a click on the Undock (Detach) button 
   */
   void undockPressed();
   /**
   * Shows a system menu for child frame windows.
   */
   void showSystemMenu();

protected:
   /** Restore the focus policies for _all_ widgets in the view using the list given as parameter.
   * Install the event filter for all direct child widgets of this. (See @ref QextMdiChildFrm::eventFilter ) 
   */
   void linkChildren( QHash<QString, Qt::FocusPolicy>* pFocPolDict);
   /** Backups all focus policies of _all_ child widgets in the MDI childview since they get lost during a reparent.
   * Remove all event filters for all direct child widgets of this. (See @ref QextMdiChildFrm::eventFilter ) 
   */
   QHash<QString, Qt::FocusPolicy>* unlinkChildren();
   /** Calculates the corner id for the resize cursor. The return value can be tested for:
   * QEXTMDI_RESIZE_LEFT, QEXTMDI_RESIZE_RIGHT, QEXTMDI_RESIZE_TOP, QEXTMDI_RESIZE_BOTTOM
   * or an OR'd variant of them for the corners. 
   */
   int getResizeCorner(int ax,int ay);
};

#endif //_QEXTMDICHILDFRM_H_
