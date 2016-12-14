extern "C" {
#include <EXTERN.h>
#include <perl.h>
#include <XSUB.h>
}
#include <iostream>

PerlInterpreter *perlInt;

void start() {
  perlInt = perl_alloc();

  PL_perl_destruct_level = 1;
  perl_construct(perlInt);

  char *embedding[] = 
    { "", "-w", "-e", "use strict; print \"Hallo\\n\"; no strict;" }; 
  perl_parse(perlInt, NULL, 4, embedding, NULL);   
  perl_run(perlInt);
  
  cout << "destructing..." << flush;
  perl_destruct(perlInt);
  cout << "done" << endl;

  perl_free(perlInt);
}

int main() {

  start(); 
  start(); 
  start(); 
  start(); 
  start(); 
  start(); 
  start(); 
  start(); 
  start(); 
  start(); 
  start(); 

  return 0;
}
