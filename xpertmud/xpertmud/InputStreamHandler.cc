#include "InputStreamHandler.h"

InputStreamFilter::InputStreamFilter(InputStreamFilter *first, 
				     InputStreamFilter *next):
  first(first), next(next)
{
}

InputStreamFilter::~InputStreamFilter() {
}

void InputStreamFilter::setNext(InputStreamFilter *next) {
  this->next = next;
}

void InputStreamFilter::setFirst(InputStreamFilter *first) {
  this->first = first;
}

/*
InputStreamHandler::InputStreamHandler() {
}

InputStreamHandler::~InputStreamHandler() {
  for(vector<InputStreamFilter*>::iterator it=filters.begin();
      it != filters.end(); ++it) {
    delete *it;
  }
}

void InputStreamHandler::parse(const string& str) {
  if(!filters.empty())
    filters[0]->parse(str);
}

void InputStreamHandler::pushBack(InputStreamFilter *filter) {
  filters.push_back(filter);
}
*/
