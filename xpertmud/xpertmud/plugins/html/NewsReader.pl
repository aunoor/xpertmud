# Some Exmaple script on how to use the XMHTML classes.
# This aims to be a HTML based news reader for typical MUX/MUD
# Bulletin boards.

use XMHTML;
use strict;

package NewsReader;

sub printHeader {
    my $w=shift;
    $w->write(qq[
		 <html>
		 <body bgcolor="#EEEEFF">
		 <a href="/close">close</a>
		 <a href="/">Folder List</a>
		 <table width="100%" border="1">
		 ]);
}
sub printFooter {
    my $w=shift;
    $w->write(qq[
		 </table>
		 Thank you for using NewsReader.pl and xpertmud.
		 </html>
		 ]);
}

sub fetchFolders {
    my $w=shift;
    my $b=$w->{Board};
    XM::addTrigger("Folders_START_$b",qr/^\={60,}$/, sub {
	XM::setTriggerState("^Folders_EAT_$b\$",1);
	XM::setTriggerState("^Folders_START_$b\$",0);
	XM::setTriggerState("^Folders_STOP_$b\$",1);
	$w->begin($b."news:/");
	printHeader($w);
	  $w->write(qq[
		       <tr>
		       <th>Nr.</th>
		       <th>Mode</th>
		       <th>Board Name</th>
		       <th>Last Post</th>
		       <th>Message Count</th>
		       </tr>
		 ]);

	return "";
    },1,1);
    XM::addTrigger("Folders_EAT_$b",qr/^.*$/, sub {
	my $line=$_[0];
	if ($line=~/^\s*(\d+)\s+/) {
	    my $num=substr($line,0,2)+0;
	    my $mode=substr($line,3,3);
	    my $title=substr($line,7,30);
	    my $date=substr($line,37,10);
	    my $count=substr($line,47);
	    $w->write(qq[
			 <tr>
			 <td>$num</td>
			 <td>$mode</td>
			 <td><a href="/list/$num">$title</a></td>
			 <td>$date</td>
			 <td>$count</td>
			 </tr>
			 ]);
	}
	return ""; 
    },1,0);
    XM::addTrigger("Folders_STOP_$b",qr/^\={60,}$/, sub {
	XM::delTrigger("^Folders_.*_$b\$");
	  printFooter($w);
	  $w->end;
	$w->show;
	return "";
    },1,0);
    XM::sendText("+".$b."read\n");
}

sub fetchFolder {
    my $w=shift;
    my $num=shift;
    my $b=$w->{Board};
    XM::addTrigger("Folder_START_$b",qr/^\={60,}$/, sub {
	XM::setTriggerState("^Folder_EAT_$b\$",1);
	XM::setTriggerState("^Folder_START_$b\$",0);
	XM::setTriggerState("^Folder_STOP_$b\$",1);
	$w->begin($b."news:/list/$num");
	printHeader($w);
	  $w->write(qq[
		       <tr>
		       <th>Nr.</th>
		       <th>Title</th>
		       <th>Date</th>
		       <th>By</th>
		       </tr>
		 ]);

	return "";
    },1,1);
    XM::addTrigger("Folder_EAT_$b",qr/^.*$/, sub {
	my $line=$_[0];
	if ($line=~/^\s*(\d+\/\d+)\s+/) {
	    my $num=substr($line,0,7);
	    $num=~s/\s//g;
	    my $title=substr($line,8,34);
	    my $date=substr($line,43,10);
	    my $by=substr($line,57);
	    $w->write(qq[
			 <tr>
			 <td>$num</td>
			 <td><a href="/read/$num">$title</a></td>
			 <td>$date</td>
			 <td>$by</td>
			 </tr>
			 ]);
	} elsif ($line=~/\*{4} (.*) \*{4}/) {
	    $w->setTitle($1);
	} 
	return ""; 
    },1,0);
    XM::addTrigger("Folder_STOP_$b",qr/^\={60,}$/, sub {
	XM::delTrigger("^Folder_.*_$b\$");
	  printFooter($w);
	  $w->end;
	$w->show;
	return "";
    },1,0);
    XM::sendText("+".$b."read $num\n");
}

sub readMessage {
    my $w=shift;
    my $num=shift;
    my $b=$w->{Board};
    XM::addTrigger("Folder_START_$b",qr/^\={60,}$/, sub {
	XM::setTriggerState("^Folder_EAT_$b\$",1);
	XM::setTriggerState("^Folder_START_$b\$",0);
	XM::setTriggerState("^Folder_STOP_$b\$",1);
	$w->begin($b."news:/read/$num");
	printHeader($w);
	  $w->write(qq[
		       <tr>
		       <th>Nr.</th>
		       <th>Title</th>
		       <th>Date</th>
		       <th>By</th>
		       </tr>
		 ]);

	return "";
    },1,1);
    XM::addTrigger("Folder_EAT_$b",qr/^.*$/, sub {
	my $line=$_[0];
	if ($line=~/^\s*(\d+\/\d+)\s+/) {
	    my $num=substr($line,0,7);
	    $num=~s/\s//g;
	    my $title=substr($line,8,34);
	    my $date=substr($line,43,10);
	    my $by=substr($line,57);
	    $w->write(qq[
			 <tr>
			 <td>$num</td>
			 <td><a href="/read/$num">$title</a></td>
			 <td>$date</td>
			 <td>$by</td>
			 </tr>
			 ]);
	} elsif ($line=~/\*{4} (.*) \*{4}/) {
	    $w->setTitle($1);
	} 
	return ""; 
    },1,0);
    XM::addTrigger("Folder_STOP_$b",qr/^\={60,}$/, sub {
	XM::delTrigger("^Folder_.*_$b\$");
	  printFooter($w);
	  $w->end;
	$w->show;
	return "";
    },1,0);
    XM::sendText("+".$b."read $num\n");
}


sub openURL {
    my ($w,$url)=@_;
    my $b=$w->{Board};
    my $proto=substr $url,0,7;
    if ($proto ne $b."news:") {
	print STDERR "Url Mismatch: $proto ne ${b}news:\n";
    }
    my $path=substr $url,7;
    if ($path eq "/close") {
	$w->{KeepOpen}=undef;
    } elsif ($path eq "/") {
	fetchFolders($w);
    } elsif ($path=~/^\/list\/(\d+)$/) {
	fetchFolder($w,$1);
    } elsif ($path=~/^\/list\/(\d+\/\d+)$/) {
	readMessage($w,$1);
    } else {
	print STDERR "Path: $path, URL: $url\n";
    }
}

sub start {
    my ($board)=@_;
    $board="bb" unless defined $board;
    my $w=new XMHTML("NewsReader - $board");
    $w->resize(500,400);
    $w->{Board}=$board;
    $w->{KeepOpen}=$w;
    $w->{onOpenUrl}=\&openURL;
    fetchFolders($w);
}

# alias to start it.
XM::addAlias("NewsReader-Start",qr/^\+(..)news$/,sub { start($1); 
						       return undef; });
