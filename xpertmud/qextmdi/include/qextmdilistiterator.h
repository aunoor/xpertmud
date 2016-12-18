//----------------------------------------------------------------------------
//    filename             : qextmdilistiterator.h
//----------------------------------------------------------------------------
//    Project              : Qt MDI extension
//
//    begin                : 02/2000       by Massimo Morin
//    changes              : 02/2000       by Falk Brettschneider to create an
//                           - 06/2000     stand-alone Qt extension set of
//                                         classes and a Qt-based library
//
//    copyright            : (C) 1999-2000 by Massimo Morin (mmorin@schedsys.com)
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

#ifndef _QEXTMDILISTITERATOR_H_
#define _QEXTMDILISTITERATOR_H_

#include <QList>

template <class Item>
class QextMdiListIterator : public QextMdiIterator<Item*> {
public:
   QextMdiListIterator(QList<Item>& list) {
      m_iterator = list.begin();
   }
   virtual void first() {
      m_iterator->toFirst();
   }
   virtual void last() {
      m_iterator->toLast();
   }
   virtual void next()  { ++(m_iterator); }
   virtual void prev()  { --(m_iterator); }
   virtual bool isDone() const { return m_iterator->current() == NULL; }
   virtual Item* currentItem() const { return m_iterator->current(); }

   virtual ~QextMdiListIterator() {
   }
  
private:
   QList<Item>::iterator m_iterator;
};

#endif // _QEXTMDILISTITERATOR_H_
