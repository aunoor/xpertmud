// -*- c++ -*-
#ifndef REGISTERIFACE_H
#define REGISTERIFACE_H

#ifndef ABSTRACT
# define ABSTRACT 0
#endif

class QWidget;

class RegisterIface {
public:
  virtual unsigned int registerWidget(QWidget* widget) = ABSTRACT;
  virtual void unregisterWidget(QWidget* widget) = ABSTRACT;
  virtual void unregisterWidget(unsigned int id) = ABSTRACT;
  virtual QWidget* getRegisteredWidget(unsigned int id) = ABSTRACT;
};

#endif
