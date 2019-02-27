use strict;
use warnings;

delAlias(qr(^SCODE_.*));
delTrigger(qr(^SCODE_.*));
addAlias("SCODE_EDIT",qr/^\/edit (\S+) (.+)/,sub {
  my $obj=$1;
  my $attr=$2;
  setTriggerState(qr/^SCODE_LINE$/,1);
  return "\@decompile/dbref $obj/$attr";
});

addTrigger("SCODE_LINE",qr/^(\S+) (\d+)=(.*)$/,sub {
  my $line=shift;
  my $iline = focusedInputLine();
  $iline->setText("$1 #$2=$3");
  setTriggerState(qr/^SCODE_LINE$/,0);
  return $line;
},0,0);
