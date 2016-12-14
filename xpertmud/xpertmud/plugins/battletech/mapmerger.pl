#!/usr/bin/perl -w
use strict;

use IO::File;

if(scalar(@ARGV) != 3) {
  print "Usage: mapmerger.pl <map1> <map2> <outmap>\n";
  exit(0);
}

my $inmap = new IO::File;

unless($inmap->open($ARGV[0],"<")) {
  print STDERR "Unable to open ".$ARGV[0]."\n";
  exit(1);
}

my $inmap2 = new IO::File;

unless($inmap2->open($ARGV[1],"<")) {
  print STDERR "Unable to open ".$ARGV[1]."\n";
  exit(1);
}


my $outmap = new IO::File;

unless($outmap->open($ARGV[2],">")) {
  print STDERR "Unable to open ".$ARGV[2]."\n";
  exit(1);
}


my $line;
unless($line  = <$inmap>) {
  print STDERR "Unable to read from File\n";
  exit(1);
}

chomp $line;
my ($x,$y) = split /\s+/, $line;

unless($line  = <$inmap2>) {
  print STDERR "Unable to read from File\n";
  exit(1);
}

chomp $line;

my ($x2,$y2) = split /\s+/, $line;


$x=$x2 if $x2 > $x;
$y=$y2 if $y2 > $y;

print $outmap "$x $y\n";

my ($line2,$outline);

while(1) {
  $outline = "";
  $line = <$inmap>;
  $line2 = <$inmap2>;
  unless($line || $line2) {
    $inmap->close();
    $inmap2->close();
    $outmap->close();
    exit(0);
  }
  $line = "" unless $line;
  $line2 = "" unless $line2;
  chomp $line;
  chomp $line2;
  if(length($line) < $x*2) {
    $line .= '?'x($x*2 - length($line));
  }
  if(length($line2) < $x*2) {
    $line2 .= '?'x($x*2 - length($line2));
  }
  for(my $i=0;$i < ($x)*2;++$i)  {
    if(substr($line,$i,1) ne '?') {
      length(substr($line,$i,1)) == 1 || die($i);
      $outline .= substr($line,$i,1);
    } else {
      length(substr($line2,$i,1)) == 1 || die($i);
      $outline .= substr($line2,$i,1);
    }
  }
  print length($outline)."\n";
  print $outmap $outline."\n";
}
