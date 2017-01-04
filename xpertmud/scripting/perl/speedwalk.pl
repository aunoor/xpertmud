use strict;


sub speedwalk {
   my $command=$_[0]; 
   $command=~s/^\.//;
   while (length $command) {
     my $count=1;
     if ($command=~ s/^(\d+)//) {
	$count=$1;
     }
     my $cmd="";
     if ($command=~ s/^\((.+?)\)//) {
       $cmd=$1;
     } elsif ($command=~ s/^(.)//) {
       $cmd=$1;
     } else {
                statusWindow->setFGColor(RED);
                statusWindow->print("Error in speedwalk expression\n");
                statusWindow->resetAttributes();
     }
     if (length $cmd) {
       XM::send("$cmd\n" x $count);
     }
   }
   return undef;
}



addAlias("SPEEDWALK",'^\.',\&speedwalk);