use strict;
use warnings;

use vars qw($completionRequest);

#TODO: efficient storage
my @words;


sub addCompletionWord {
  my $word=shift;
  unless (grep {$word eq $_} @words) {
    #prevent duplicates
    push @words,$word;
  }
}

sub complete {
  my $word=shift;

  my @matches=grep {$word eq substr $_,0,length $word; } @words;
  if (scalar(@matches)==1) {
    $word=$matches[0];
  } elsif (scalar(@matches)>1) {
     my $shortest=$matches[0];
     foreach my $match (@matches) {
       my $ns=$word;
       # hmm, geht das schneller?
       for(my $i=length($word); $i < length($shortest) and $i < length($match); ++$i) {
	 if (substr($shortest,$i,1) eq substr($match,$i,1)) {
	   $ns.=substr($shortest,$i,1);
	 } else {
	   last;
	 }
       }
       if (length($ns) < length($shortest)) {
	 $shortest=$ns;
       }
     }
     if ($word eq $shortest) {
       statusWindow()->setFGColor(GREEN);
       statusWindow()->setIntensive(1);
       statusWindow()->print("\nPossible Completions:\n");
       statusWindow()->setIntensive(0);
       statusWindow()->print('"'.join('", "',@matches)."\"\n");
       statusWindow()->resetAttributes();
     }
    $word=$shortest;
  }
  return $word;
}

$completionRequest=sub {
  my $iline = focusedInputLine();
  my ($text,$line,$col)=($iline->getText(), $iline->getCursorY(),
			 $iline->getCursorX());

  my @lines=split /\n/,$text;
  $lines[$line]="" if not defined $lines[$line];

  my $front=substr $lines[$line],0,$col;
  my $back=substr $lines[$line],$col;
  my $word="";
  if ($front =~ s/(\w+)$//) {
    $word=complete($1);
    

  }

  $lines[$line]=$front.$word.$back;
  $col=length($front) + length($word);
  $text=join "\n",@lines;

  $iline->setText($text);
  $iline->setCursorY($line);
  $iline->setCursorX($col);
};

delKeyBinding('^Completion$');
addKeyBinding("Completion", ".000.... Tab", $completionRequest);

# example for an Alias filling the lexicon.
#addAlias("COMPLETION",qr/.?/,sub {
#	   my $line=shift;
#	   foreach my $word (split /\W+/,$line) {
#	     addCompletionWord($word) if length($word)>2;
#	   }
#	   return $line;
#	 });

