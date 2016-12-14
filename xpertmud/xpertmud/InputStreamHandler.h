// -*- c++ -*-

#ifndef INPUTSTREAMHANDLER_H
#define INPUTSTREAMHANDLER_H

#ifndef ABSTRACT
#define ABSTRACT 0
#endif

#ifdef WIN32
#pragma warning ( disable:4786 )
#endif

#include <string>

class SenderIface {
 public:
  virtual void sendByte(char byte) = ABSTRACT;
  virtual void send(const std::string& str) = ABSTRACT;
  virtual void send(const char* str, int length) = ABSTRACT;
};

class InputStreamFilter {
 public:
  /**
   * If somewhere in the stream some filter finds
   * something which causes the complete stream
   * layout to change (for example COMPRESS), then
   * the filter has to parse the rest after the
   * chars that caused the change to be reparsed
   * completely from the beginning by calling
   * parent->parse(rest);
   * Therefor the first pointer is needed.
   */
  InputStreamFilter(InputStreamFilter *first = NULL, 
		    InputStreamFilter *next = NULL);
  virtual ~InputStreamFilter();

  virtual void parse(const std::string& str) = ABSTRACT;
  void setNext(InputStreamFilter *next);
  void setFirst(InputStreamFilter *first);

 protected:
  InputStreamFilter *first;
  InputStreamFilter *next;
};

#endif
