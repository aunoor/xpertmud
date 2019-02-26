#ifndef REGISTRY_H
#define REGISTRY_H

#include <QString>
#include <windows.h>

#include <iostream>
using std::cout;
using std::endl;

#include <string>
#include <stdexcept>
using std::runtime_error;

/**
   Example:
  Registry::writeEntry
  ("HKEY_CURRENT_USER/Software/Xpertmud/Default Font Name",
  "MyFont");
  Registry::writeEntry
  ("HKEY_CURRENT_USER/Software/Xpertmud/Default Font Size",
  15);*/
class Registry {
 public:
  static void writeEntry(const QString& entry,
			 const QString& value) {
    int pos = entry.lastIndexOf('/');
    QString keyName = entry.left(pos);
    QString entryName = entry.mid(pos+1);
    HKEY key = createKeyEx(keyName);
    setValueEx(key, entryName, value);
    close(key);
  }

  static void writeEntry(const QString& entry,
			 int value) {
    int pos = entry.lastIndexOf('/');
    QString keyName = entry.left(pos);
    QString entryName = entry.mid(pos+1);
    HKEY key = createKeyEx(keyName);
    setValueEx(key, entryName, value);
    close(key);
  }

  static QString readString(const QString& entry,
			    const QString& def) {
    int pos = entry.lastIndexOf('/');
    QString keyName = entry.left(pos);
    QString entryName = entry.mid(pos+1);
    HKEY key = createKeyEx(keyName);
    QString retVal = readValueExString(key, entryName, def);
    close(key);
    return retVal;
  }

  static int readInt(const QString& entry, int def) {
    int pos = entry.lastIndexOf('/');
    QString keyName = entry.left(pos);
    QString entryName = entry.mid(pos+1);
    HKEY key = createKeyEx(keyName);
    int retVal = readValueExInt(key, entryName, def);
    close(key);
    return retVal;
  }
 private:
  static HKEY createKeyEx(const QString& expression) {
    int pos = expression.indexOf('/');
    QString regHead = expression.left(pos);
    QString regTail = expression.mid(pos+1);
    HKEY parent;
    if(regHead == "HKEY_CURRENT_USER")
      parent = HKEY_CURRENT_USER;
    else
      throw runtime_error
	(QString("Unsupported Top Level Key: %1").arg(regHead).toLatin1().data());

    return createKeyEx(regTail, parent);
  }
  static HKEY createKeyEx(const QString& expression, HKEY parent) {
    int pos = expression.indexOf('/');
    QString regHead = expression.left(pos);
    QString regTail = expression.mid(pos+1);

    HKEY keyResult;
    DWORD disposition;
    LONG result =
#ifdef UNICODE
      RegCreateKeyEx(parent, (LPCTSTR)(regHead+QChar(0)).unicode(),
#else
      RegCreateKeyEx(parent, regHead.toLatin1().data(),
#endif
		     0, (LPTSTR)"REG_SZ", REG_OPTION_NON_VOLATILE,
		     KEY_ALL_ACCESS, 0, &keyResult,
		     &disposition);
    if(result != ERROR_SUCCESS) {
      throw runtime_error
	(QString("Failure while generating key: %1").arg(regHead).toLatin1().data());
    }
    if(pos == -1) 
      return keyResult;
    return createKeyEx(regTail, keyResult);
  }

  static QString readValueExString(HKEY key, 
				   const QString& name,
				   const QString& def) {
    DWORD length;
    DWORD type;
    LONG result =
#ifdef UNICODE
      RegQueryValueEx(key, (LPCTSTR)(name+QChar(0)).unicode(), 0,
#else
      RegQueryValueEx(key, name.toLatin1().data(), 0,
#endif
		      &type, 0, &length);
    if(type != REG_SZ || result != ERROR_SUCCESS || length <= 0)
      return def;

    char *buffer = new char[length];
    result =
#ifdef UNICODE
      RegQueryValueEx(key, (LPCTSTR)(name+QChar(0)).unicode(), 0,
#else
      RegQueryValueEx(key, name.toLatin1().data(), 0,
#endif
		      &type, (unsigned char *)buffer, &length);
    if(result != ERROR_SUCCESS) {
      delete[] buffer;
      throw runtime_error
	(QString("Failure while reading entry \"%1\"").arg(name).toLatin1().data());
    }
    QString retVal(buffer);
    delete[] buffer;
    return retVal;
  }

  static int readValueExInt(HKEY key, 
			    const QString& name, int def) {
    DWORD length;
    DWORD type;
    LONG result =
#ifdef UNICODE
      RegQueryValueEx(key, (LPCTSTR)(name+QChar(0)).unicode(), 0,
#else
      RegQueryValueEx(key, name.toLatin1().data(), 0,
#endif
		      &type, 0, &length);
    if(type != REG_DWORD || result != ERROR_SUCCESS || length <= 0)
      return def;
    DWORD value;
    result =
#ifdef UNICODE
      RegQueryValueEx(key, (LPCTSTR)(name+QChar(0)).unicode(), 0,
#else
      RegQueryValueEx(key, name.toLatin1().data(), 0,
#endif
		      &type, (unsigned char *)&value, &length);
    if(result != ERROR_SUCCESS) {
      throw runtime_error
	(QString("Failure while reading entry \"%1\"").arg(name).toLatin1().data());
    }
    return value;
  }

  static void setValueEx(HKEY key, 
			 const QString& name,
			 const QString& value) {
    const unsigned char *vp = (const unsigned char *)value.toLatin1().data();
    int vlen = value.length()+1;
    if(vp == NULL) {
      vp = (const unsigned char*)"";
      vlen = 1;
    }
    LONG result =
#ifdef UNICODE
      RegSetValueEx(key, (LPCTSTR)(name+QChar(0)).unicode(), 0,
#else
      RegSetValueEx(key, name.toLatin1().data(), 0,
#endif
		    REG_SZ, vp, vlen);
    if(result != ERROR_SUCCESS) {
      throw runtime_error
	(QString("Failure while writing entry \"%1\"").arg(name).toLatin1().data());
    }
  }

  static void setValueEx(HKEY key, 
			 const QString& name,
			 DWORD value) {
    LONG result =
#ifdef UNICODE
      RegSetValueEx(key, (LPCTSTR)(name+QChar(0)).unicode(), 0,
#else
      RegSetValueEx(key, name.toLatin1().data(), 0,
#endif
		    REG_DWORD, 
		    (CONST BYTE *)&value, 
		    sizeof(value));
    if(result != ERROR_SUCCESS) {
      throw runtime_error
	(QString("Failure while writing entry \"%1\"").arg(name).toLatin1().data());
    }
  }

  static void close(HKEY key) {
    LONG result =
      RegCloseKey(key);
    if(result != ERROR_SUCCESS) {
      throw runtime_error
	(QString("Failure while closing key!").toLatin1().data());
    }
  }
};

#endif
