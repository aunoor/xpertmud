#use vars qw($charWin $debugWin $hp $ep $mudLocation $charName $charExp
#	    @dirs $walkInvNodes);
use vars qw($mudLocation);

#easywalk data filename
my $filename = $ENV{HOME} . "/.kde/share/apps/xpertmud/perl/easywalk/easywalk_data.dpl";

print statusWindow()->print("Parsing files\n");
parse('easywalk/easywalk.pl');

walkReadData($filename); #reload easywalk data


## Commands to save easywalk data
delAlias("saveStatus");
addAlias("saveStatus", "^=save", sub {walkDumpData($filename);});

addAlias("sleep", '^sleep$', sub {
    prs("Saving client status.\n");
    walkDumpData($filename);
    XM::send("sleep\r\n");
},1);

addAlias("quit", '^quit$', sub {
    prs("Saving client status.\n");
    walkDumpData($filename);
    XM::send("quit\r\n");
},1);

#Define mudLocation
delTrigger("TrMud");
addTrigger("TrMud", '^.*$', \&parseLine);
sub parseLine {
    my $line = shift;
    my $sline = stripColors($line);
    if($sline =~ /^(\w[^(]+[^(])\(((?:[a-z]+,?\s*)*)\)\s*(?:\[((?:[a-z]+,?\s*)+)\])?/) {
	chomp $sline;
	$mudLocation = $sline;
	return ansi("%cg$sline%cn\n");
    }
       return $line;
}    
 



sub stripColors {
my $text = shift;
while($text =~ s/\x1b\[(\d+(;\d+)*)?m(.*)/$3/) {}
return $text;
}


