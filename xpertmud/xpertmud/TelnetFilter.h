#ifndef TELNETFILTER_H
#define TELNETFILTER_H

#include <map>
#include <string>
using std::multimap;
using std::pair;
using std::string;

#include "InputStreamHandler.h"

class TelnetIacAnswer {
 public:
  /**
   * @param streamTypeChange true, if the parsing of this iac code changes the
   * structure of the input stream (for example COMPRESS). All
   * text received after this command will be parsed by the complete
   * filter chain again.
   */
  TelnetIacAnswer(bool doAnswer=false, bool answer=false, 
                  bool streamTypeChange=false):
    doAnswer(doAnswer), answer(answer), 
    streamTypeChange(streamTypeChange) {}

  void operator|=(const TelnetIacAnswer& ret) {
    streamTypeChange |= ret.streamTypeChange;
    doAnswer |= ret.doAnswer;

    // the last answer which is or'ed is taken
    if(ret.doAnswer)
      answer = ret.answer;
  } 

  bool doAnswer;
  bool answer;

  bool streamTypeChange;
};

class TelnetIacCallback {
 public:
  virtual TelnetIacAnswer iacWont(int iac) = ABSTRACT;
  virtual TelnetIacAnswer iacWill(int iac) = ABSTRACT;
  virtual TelnetIacAnswer iacDont(int iac) = ABSTRACT;
  virtual TelnetIacAnswer iacDo(int iac) = ABSTRACT;
  virtual bool iacSubNegotiation(int iac, const string& command) = ABSTRACT;
};

class StandardCallback: public TelnetIacCallback {
 public:
  StandardCallback(): debug(false) {}
  StandardCallback(bool debug): debug(debug) {}

  virtual TelnetIacAnswer iacWont(int iac);
  virtual TelnetIacAnswer iacWill(int iac);
  virtual TelnetIacAnswer iacDont(int iac);
  virtual TelnetIacAnswer iacDo(int iac);
  virtual bool iacSubNegotiation(int iac, const string& command);

 private:
  bool debug;
};

class TelnetFilter: public InputStreamFilter {
  enum EState {
    SNORMAL,
    SIAC_START,
    SDO_START,
    SDONT_START,
    SWILL_START,
    SWONT_START,
    SSB_START,
    SSB_IAC
  };

 public:
  typedef TelnetIacCallback* dataType;
  typedef pair<int, dataType> mapPairType;
  typedef multimap<int, dataType> mapType;

  TelnetFilter(SenderIface *sender,
	       InputStreamFilter *first = NULL,
	       InputStreamFilter *next = NULL);
  virtual ~TelnetFilter();

  virtual void parse(const string& str);
  void registerIac(int iac, TelnetIacCallback* call);
  /**
   * Set the string that should be in the text instead
   * of a IAC GA. Set to empty, if you don't need
   * IAC GA's at all...
   */
  void setReplaceGA(const string& s);

 private:
  void sendUntil(const string::const_iterator from,
                 const string::const_iterator to);

  /**
   * @return true, if the stream type changed.
   */
  bool sendSubCommand();
  /**
   * @return true, if the stream type changed.
   */
  bool executeIacCallback(EState state, unsigned char command);

  SenderIface *sender;

  mapType iacMap;
  EState state;

  StandardCallback standardCallback;
  string subCommand;

  // what put for the IAC GA into the string...
  string replaceGA;
};

#endif
