use strict;

my $linecounter=0;

addAlias("PIPE_LC",'\|lc$',sub {
	   my $line=shift;
	   $line=~ s/\|lc$//;
	   setTriggerState("PIPE_LC",1);
	   addDelayed("PIPE_LC",5,sub { 
			setTriggerState("PIPE_LC",0);
			statusWindow->setFGColor(BLUE);
			statusWindow->print("Line count: $linecounter.\n");
			statusWindow->resetAttributes();
		      });
	   $linecounter=0;
	   return $line;
	 }
	);

addTrigger("PIPE_LC","^.+\$",sub {
	     $linecounter++;
	     setDelayed("PIPE_LC",1);
	     return $_[0];
	   },0,0);


my $expression=".?";

addAlias("PIPE_GREP",'\|grep\s+(.+)$',sub {
	   my $line=shift;
	   my $exp=$1;
	   $line=~ s/\|grep\s+(.+)$//;
	   statusWindow->setFGColor(BLUE);
	   statusWindow->print("Grep active: /$exp/.\n");
	   statusWindow->resetAttributes();
	   setTriggerState("PIPE_GREP",1);
	   addDelayed("PIPE_GREP",5,sub {
			setTriggerState("PIPE_GREP",0);
			statusWindow->setFGColor(BLUE);
			statusWindow->print("Grep Finished. $linecounter lines matched\n");
			statusWindow->resetAttributes();
			
		      });
	   $expression=$exp;
	   $linecounter=0;
	   return $line;
	 }
	);

addTrigger("PIPE_GREP",".?",sub {
	     my $line=shift;
	     setDelayed("PIPE_GREP",1);
	     if ($line=~/$expression/) {
	       $linecounter++;
	       return $line;
	     }
	     return undef;
	   },0,0);
