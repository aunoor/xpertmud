use xmlparser; #FIXED
use ansicolors; #FIXED

package helpdtd;
use strict;
use warnings;
use diagnostics;

use vars qw/@ISA/;

sub new {
  @ISA = qw/XMXmlDTD/;

  my ($this) = @_;
  my $class = ref($this) || $this;

  my $self = new XMXmlDTD();
  return undef unless defined $self;
  $self = bless $self, $class;

  my $example = new XMXmlDTDTag('example');
  $example->addAttribute('xml:space', 'preserve');
  $self->addTag($example);

  my $code = new XMXmlDTDTag('code');
  $code->addAttribute('xml:space', 'preserve');
  $self->addTag($code);

  foreach my $langId ("py", "ru", "pe") {
    my $idtag = new XMXmlDTDTag($langId);
    $idtag->addAttribute('xml:space', 'preserve');
    $self->addTag($idtag);
  }
  return $self;
}

package helplayout;
use strict;
use warnings;
use diagnostics;

use vars qw/$ansi/;

sub new {
  if(not defined $ansi) {
    $ansi = new AnsiColorTranslator();
  }
  my ($this, $mode) = @_;
  my $class = ref($this) || $this;

  my $object = {
		lang => "perl",
		Lang => "Perl",
		langID => "pe",
		allIDs => [ "py", "ru", "pe" ],
		mode => $mode,
		plainHeadingColor => $ansi->getCode('cb').$ansi->getCode('ch'),
		plainUnderlineColor => $ansi->getCode('cb'),
		plainSectionColor => $ansi->getCode('cc'),
		plainArgColor => $ansi->getCode('cr').$ansi->getCode('ch'),
		plainCodeColor => $ansi->getCode('cx').$ansi->getCode('ch'),
		plainSpace => 1,
		subHelpText => ""
	       };
  my $self = bless $object, $class;

  return $self;
}

sub setSubHelpText {
  my ($self, $text) = @_;
  $self->{subHelpText} = $text;
}

sub escapeId {
  my ($self, $string) = @_;
  my $ret = $string;
  if($self->{mode} eq "docbook") {
    $ret =~ s/ /_/g;
    $ret =~ s/#/Sharp/g;
    $ret =~ s/%/Percent/g;
  }
  return $ret;
}

sub outputSynopsis {
  my ($self, $node) = @_;
  my $synopsis = "";
  if($self->{mode} eq "docbook") {
    $synopsis .= "<para><programlisting>";
    $synopsis .= "Synopsis: ".$node->getAttribute('name')."(";
  } elsif($self->{mode} eq "plain") {
    $synopsis .= $self->{plainSectionColor};
    $synopsis .= "Synopsis:".$ansi->end()." ".
      $self->{plainCodeColor}.$node->getAttribute('name')."(";
  }

  foreach my $arg (@{ $node->getSubNodes("arg") }) {
    $synopsis .= $arg->escape($arg->getAttribute('name')).", ";
  }
  if(substr($synopsis, -1, 1) eq " ") {
    $synopsis = substr($synopsis, 0, length($synopsis)-2);
  }
  $synopsis .= ")\n";
  if($self->{mode} eq "docbook") {
    $synopsis .= "</programlisting></para>";
  } elsif($self->{mode} eq "plain") {
    $synopsis .= $ansi->end();
  }
  return $synopsis;
}

sub outputArgs {
  my ($self, $node, $prefix, $call, $depth) = @_;
  my $foundArgs = 0;
  my $s = "";
  if($self->{mode} eq "docbook") {
    $s .= "    <table>
      <title>Arguments</title>
      <tgroup cols=\"2\">
        <tbody>";
  } elsif($self->{mode} eq "plain") {
    $s .= "\n".$self->{plainSectionColor}."Arguments:\n".$ansi->end();
  }
  foreach my $arg (@{ $node->getSubNodes('arg') }) {
    if(scalar(@{ $arg->getChilds() }) > 0) {
      if($self->{mode} eq "docbook") {
	$s .= "          <row>
            <entry>".$arg->getAttribute('name')."</entry>
            <entry>";
      } elsif($self->{mode} eq "plain") {
	my $argName = $arg->getAttribute('name');
	$argName =~ s/\s*([^\s].*[^\s]?)\s*/$1/;
	$s .= "  ".$self->{plainArgColor}.$argName.": ".$ansi->end();
      }

      foreach my $c (@{ $arg->getChilds() }) {
	$s .= $self->generate($c, $prefix, $call, $depth);
      }

      if($self->{mode} eq "docbook") {
	$s .= "</entry>
          </row>";
      } elsif($self->{mode} eq "plain") {
	$s .= "\n";
      }
      $foundArgs += 1;
    }
  }
  if($self->{mode} eq "docbook") {
    $s .= "
        </tbody>
      </tgroup>
    </table>
"
  }

  if($foundArgs > 0) {
    return $s;
  }
  return "";
}

sub outputOverview {
  my ($self, $node, $prefix, $call, $depth) = @_;
  my $s = "";
  my $tagNames = $node->getChildTagNames();
  for(my $i=0; $i<scalar(@$tagNames); ++$i) {
    while($i<scalar(@$tagNames) &&
	  ($tagNames->[$i] eq "description" ||
	   $tagNames->[$i] eq "example")) {
      splice(@$tagNames, $i, 1);
    }
  }
  foreach my $tagName (@$tagNames) {
    my $title = "";
    if($self->{mode} eq "plain") {
      $title .= $self->{plainHeadingColor};
    }
    if($tagName eq "function") {
      $title .= "Functions";
    } elsif($tagName eq "topic") {
      $title .= "Topics";
    } elsif($tagName eq "class") {
      $title .= "Classes";
    } elsif($tagName eq "module") {
      $title .= "Modules";
    }
    if($self->{mode} eq "plain") {
      $title .= ":".$ansi->end();
      $s .= "\n".$title."\n";
    }
    my $i = 0;
    my $line = "";
    foreach my $child (@{ $node->getSubNodes($tagName) }) {
      if((not $child->hasAttribute('restriction')) ||
	 ($child->getAttribute('restriction') eq $self->{langID})) {
	if($self->{mode} eq "plain") {
	  $line .= $child->getAttribute('name');
	}
	$i += 1;
	if($i % 3 == 0) {
	  $i = 0;
	  if($self->{mode} eq "plain") {
	    $s .= $line ."\n";
	    $line = "";
	  }
	} else {
	  if($self->{mode} eq "plain") {
	    while(length($line) % 30 != 0) {
	      $line .= " ";
	    }
	  }
	}
      }
    }
    if($line ne "") {
      $s .= $line . "\n";
    }
  }
  if($self->{mode} eq "plain") {
    $s .= "\n";
  }
  return $s;
}

sub outputContainer {
  my ($self, $node, $prefix, $call, $depth) = @_;
  my $s = "";

  my $name = $node->getAttribute('name');
  my $myprefix = $name;
  if($prefix ne "") {
    $myprefix = $prefix . "_" . $name;
  }
  my $mycall = $name;
  if($call ne "") {
    $mycall = $call . "::" . $name;
  }

  my $title = "";
  if($node->getTagName() eq "module") {
    $title = "Module " . $name;
  } elsif($node->getTagName() eq "topic") {
    $title = "Topic " . $name;
  } elsif($node->getTagName() eq "class") {
    $title = "Class " . $name;
  } elsif($node->getTagName() eq "function") {
    $title = $name;
  } else {
    $title = $self->{Lang} . " Developer's Guide to Xpertmud";
  }
  my $sect = "sect" . $depth;
  if($depth == 0) {
    $sect = "chapter";
  }

  if($self->{mode} eq "docbook") {
    $s .= "  <".$sect." id=\"".$self->escapeId($myprefix)."\">
    <title>".$title."</title>
";
  } elsif($self->{mode} eq "plain") {
    $s .= $self->{plainHeadingColor}.$title.":".$ansi->end()."\n".$self->{plainUnderlineColor}.("*" x (length($title)+1)).$ansi->end()."\n";
  }

  if((not $self->{mode} eq "docbook") && (not $node->getTagName() eq "function")) {
    if($self->{subHelpText} ne "") {
      $s .= $self->{subHelpText} . "\n";
    }
    foreach my $sub (@{ $node->getSubNodes('description') }) {
      $s .= $self->generate($sub, $myprefix, $mycall, $depth+1);
    }
    foreach my $sub (@{ $node->getSubNodes('example') }) {
      $s .= $self->generate($sub, $myprefix, $mycall, $depth+1);
    }
  }

  if($node->getTagName() eq "function") {
    $s .= $self->outputSynopsis($node);
    $s .= $self->outputArgs($node, $myprefix, $mycall, $depth+1);
  } else {
    $s .= $self->outputOverview($node, $myprefix, $mycall, $depth+1);
  }

  if(($self->{mode} eq "docbook") || ($node->getTagName() eq "function")) {
    foreach my $tag (@{ $node->getChildTagNames() }) {
      if($tag ne "arg") {
	foreach my $sub (@{ $node->getSubNodes($tag) }) {
	  $s .= $self->generate($sub, $myprefix, $mycall, $depth+1);
	}
      }
    }
  }

  if($self->{mode} eq "docbook") {
    $s .= "  </".$sect.">
";
  }
  return $s;
}

sub whiteSpaceKiller {
  my ($self, $s) = @_;
  my $ret = $s;
  $ret =~ s/[\s\n]+/ /g;
  if($self->{plainSpace}) {
    $ret =~ s/^\s//;
  }
  return $ret;
}

sub generate {
  my ($self, $node, $prefix, $call, $depth, $cwhite) = @_;
  $prefix = $prefix || $self->{lang};
  $call = $call || "";
  $depth = $depth || 0;
  $cwhite = $cwhite || 0;

  if(not ref $node) {
    if($self->{mode} eq "docbook") {
      return new XMXmlNode("")->escape($node);
    } else {
      if($cwhite) {
	$self->{plainSpace} = 0;
	return $node;
      } else {
	my $ret = $self->whiteSpaceKiller($node);
	if($ret eq " ") {
	  $ret = "";
	}
	$self->{plainSpace} = 0;
	return $ret;
      }
    }
  }

  my $s = "";
  my $tagName = $node->getTagName();
  if($node->getTagName() eq "module" ||
     $node->getTagName() eq "class" ||
     $node->getTagName() eq "topic" ||
     $node->getTagName() eq "help" ||
     $node->getTagName() eq "function") {
    if((not $node->getTagName() eq "function") ||
       (not $node->hasAttribute('restriction')) ||
       ($node->getAttribute('restriction') eq $self->{langID})) {
      $s .= $self->outputContainer($node, $prefix, $call, $depth);
    }
  } elsif(grep(/$tagName/, @{ $self->{allIDs} }) &&
	  $node->getTagName() ne $self->{langID}) {
    ; # pass
  } elsif($node->getTagName() eq $self->{langID}) {
    foreach my $c (@{ $node->getChilds() }) {
      $s .= $self->generate($c, $prefix, $call, $depth, 1);
    }
  } elsif($node->getTagName() eq "alt") {
    foreach my $c (@{ $node->getChilds() }) {
      if(ref $c) {
	$s .= $self->generate($c, $prefix, $call, $depth);
      }
    }
  } elsif($node->getTagName() eq "description") {
    if(scalar(@{ $node->getSubNodes("para") }) == 0) {
      if($self->{mode} eq "docbook") {
	$s .= "<para>";
      }
      for my $c (@{ $node->getChilds() }) {
	if($self->{mode} eq "docbook") {
	  $s .= $self->generate($c, $prefix, $call, $depth);
	} elsif($self->{mode} eq "plain") {
	  $s .= $self->generate($c, $prefix, $call, $depth);
	}
      }
      if($self->{mode} eq "docbook") {
	$s .= "</para>";
      } elsif($self->{mode} eq "plain") {
	$s .= "\n";
	$self->{plainSpace} = 1;
      }
    } else {
      foreach my $c (@{ $node->getChilds() }) {
	if((not ref $c) || $c->getTagName() eq "para") {
	  if($self->{mode} eq "docbook") {
	    $s .= "<para>";
	  }
	}
	if($self->{mode} eq "docbook") {
	  $s .= $self->generate($c, $prefix, $call, $depth);
	} elsif($self->{mode} eq "plain") {
	  $s .= $self->generate($c, $prefix, $call, $depth);
	  my $lastChild =
	    $node->getChilds()->[scalar(@{ $node->getChilds() })-1];
	  if(((not ref $lastChild) || $c !=  $lastChild) &&
	     ref $c &&
	     $c->getTagName() eq "para") {
	    $s .= "\n";
	    $self->{plainSpace} = 1;
	  }
	}
	if((not ref $c) || $c->getTagName() eq "para") {
	  if($self->{mode} eq "docbook") {
	    $s .= "</para>";
	  }
	}
      }
    }
  } elsif($node->getTagName() eq "code") {
    if($self->{mode} eq "docbook") {
      $s .= "<programlisting>";
    } elsif($self->{mode} eq "plain") {
      $s .= "\n" . $self->{plainCodeColor};
    }
    foreach my $c (@{ $node->getChilds() }) {
      $s .= $self->generate($c, $prefix, $call, $depth, 1);
    }
    if($self->{mode} eq "docbook") {
      $s .= "</programlisting>";
    } elsif($self->{mode} eq "plain") {
      $s .= $ansi->end() . "\n";
      $self->{plainSpace} = 1;
    }
  } elsif($node->getTagName() eq "example") {
    if($self->{mode} eq "docbook") {
      $s .= "<para><programlisting>Example:\n";
    } elsif($self->{mode} eq "plain") {
      $s .= "\n".$self->{plainSectionColor}."Example:\n".$ansi->end().$self->{plainCodeColor};
    }
    foreach my $c (@{ $node->getChilds() }) {
      $s .= $self->generate($c, $prefix, $call, $depth, 1);
    }
    if($self->{mode} eq "docbook") {
      $s .= "</programlisting></para>";
    } elsif($self->{mode} eq "plain") {
      $s .= $ansi->end() . "\n";
      $self->{plainSpace} = 1;
    }
  } elsif(grep(/$tagName/, $ansi->getKeys())) {
    if($self->{mode} eq "plain") {
      $s .= $ansi->getCode($node->getTagName());
    }
    foreach my $c (@{ $node->getChilds() }) {
      $s .= $self->generate($c, $prefix, $call, $depth);
    }
    if($self->{mode} eq "plain") {
      $s .= $ansi->end();
    }
  } else {
    foreach my $c (@{ $node->getChilds() }) {
      $s .= $self->generate($c, $prefix, $call, $depth);
    }
  }
  return $s;
}

1;
