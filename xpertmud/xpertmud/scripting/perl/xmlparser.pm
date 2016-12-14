package XMXmlNode;
use strict;
use warnings;
use diagnostics;

sub new {
  my ($this, $tagName, $dtd) = @_;
  my $class = ref($this) || $this;

  my $object = {
		childs => [],
		attributes => {},
		tagName => $tagName
	       };
  my $self = bless $object, $class;

  if(ref $dtd) {
    my $tag = $dtd->getTag($self->{tagName});
    foreach my $defaultAttr (@{ $tag->getDefaultAttributes() }) {
      $self->setAttribute($defaultAttr, $tag->getDefault($defaultAttr));
    }
  }

  return $self;
}

sub setAttribute {
  my ($self, $aname, $value) = @_;
  $self->{attributes}->{$aname} = $value;
}

sub getAttribute {
  my ($self, $name) = @_;
  return $self->{attributes}->{$name};
}

sub hasAttribute {
  my ($self, $name) = @_;
  return exists $self->{attributes}->{$name};
}

sub getChildTagNames {
  my ($self) = @_;
  my $ret = [];
  foreach my $child (@{ $self->{childs} }) {
    my $childTagName = undef;
    if(ref $child) {
      $childTagName = $child->getTagName();
    }
    if((ref $child) && (not grep(/$childTagName/, @$ret))) {
      push(@$ret, $child->getTagName());
    }
  }
  return $ret;
}

sub removeChild {
  my ($self, $child) = @_;
  for(my $i=0; $i<scalar(@{ $self->{childs} }); ++$i) {
    my $inchild = $self->{childs}->[$i];
    while($i<scalar(@{ $self->{childs} }) &&
	  (ref $child && ref $inchild && $child == $inchild) ||
	  (not ref $child && not ref $inchild && $child eq $inchild)) {
      splice(@{ $self->{childs} }, $i);
    }
  }
}

sub addChild {
  my ($self, $child) = @_;
  if((not ref $child) && ($child eq "")) {
    return;
  }

  my $childNum = scalar(@{ $self->{childs} });
  if((not ref $child) && ($childNum > 0) &&
     (not ref $self->{childs}->[$childNum-1])) {
    $self->{childs}->[$childNum-1] .= $child;
    return;
  }

  push(@{ $self->{childs} }, $child);
}

sub getChilds {
  my ($self) = @_;
  return $self->{childs};
}

sub getSubNodes {
  my ($self, $name) = @_;
  $name = $name || "";
  my $ret = [];
  foreach my $child (@{ $self->{childs} }) {
    if((ref $child) && 
       (($name eq "") || ($name eq $child->getTagName()))) {
      push(@$ret, $child);
    }
  }
  return $ret;
}

sub getTagName {
  my ($self) = @_;
  return $self->{tagName};
}

sub escape {
  my ($self, $s) = @_;
  my $ret = "";
  for(my $i=0; $i<length($s); ++$i) {
    my $char = substr($s, $i, 1);
    if(ord($char) <= 6 || ord($char) > 255) {
      $ret .= "&#" . ord($char) . ";";
    }
    elsif($char eq "&") {
      $ret .= "&amp;";
    }
    elsif($char eq "<") {
      $ret .= "&lt;";
    }
    elsif($char eq ">") {
      $ret .= "&gt;";
    }
    elsif($char eq "\"") {
      $ret .= "&quot;";
    }
    else {
      $ret .= $char;
    }
  }
  return $ret;
}

sub toString {
  my ($self, $indent) = @_;
  $indent = $indent || "";
  my $s = "<" . $self->getTagName();
  foreach my $attr (keys %{ $self->{attributes} }) {
    $s .= ' ' . $attr . '="' .
      $self->escape($self->{attributes}->{$attr}) . '"';
  }

  if(scalar(@{ $self->{childs} }) > 0) {
    $s .= ">";
    foreach my $child(@{ $self->{childs} }) {
      if(not ref $child) {
	$s .= $self->escape($child);
      } else {
	$s .= $child->toString($indent . " ");
      }
    }
    $s .= "</" . $self->getTagName() . ">";
  } else {
    $s .= "/>";
  }

  return $s;
}

package XMXmlDTDTag;
use strict;
use warnings;
use diagnostics;

sub new {
  my ($this, $name) = @_;
  my $class = ref($this) || $this;

  my $object = {
		attributes => {},
		name => $name
	       };
  my $self = bless $object, $class;
  return $self;
}

sub addAttribute {
  my ($self, $name, $value) = @_;
  if(not grep(/$name/, keys %{ $self->{attributes} })) {
    $self->{attributes}->{$name} = [];
  }
  push(@{ $self->{attributes}->{$name} }, $value);
}

sub getDefault {
  my ($self, $name) = @_;
  if(not grep(/$name/, keys %{ $self->{attributes} })) {
    return "";
  }
  return $self->{attributes}->{$name}->[0];
}

sub getDefaultAttributes {
  my ($self) = @_;
  return [ keys %{ $self->{attributes} } ];
}

sub getName {
  my ($self) = @_;
  return $self->{name};
}

package XMXmlDTD;
use strict;
use warnings;
use diagnostics;

sub new {
  my ($this) = @_;
  my $class = ref($this) || $this;

  my $object = {
		tags => {}
	       };
  my $self = bless $object, $class;
  return $self;
}

sub addTag {
  my ($self, $tag) = @_;
  $self->{tags}->{$tag->getName()} = $tag;
}

sub getTag {
  my ($self, $name) = @_;
  if(not grep(/$name/, keys %{ $self->{tags} })) {
    my $ret = new XMXmlDTDTag($name);
    return $ret;
  }
  return $self->{tags}->{$name};
}

# pseudo xml parser
package XMXmlParser;
use strict;
use warnings;
use diagnostics;

use vars qw/$inWordExtra $startWord/;

sub new {
  $inWordExtra = q/[0-9]/;
  $startWord = q/[a-zA-Z]/;
  my ($this, $dtd) = @_;
  my $class = ref($this) || $this;

  my $object = {
		state => "parsechild",
		wasspace => 0,
		stack => [ new XMXmlNode("document") ],
		lineno => 1,
		column => 1,
		attr => "",
		buffer => "",
		closing => 0,
		dtd => $dtd
	       };
  my $self = bless $object, $class;
  return $self;
}

sub entity {
  my ($self, $name) = @_;
  if($name eq "amp") {
    return "&";
  }
  elsif($name eq "lt") {
    return "<";
  }
  elsif($name eq "gt") {
    return ">";
  }
  elsif($name eq "quot") {
    return '"';
  }
  elsif(length($name) > 0 && substr($name, 0, 1) eq "#") {
    return chr(substr($name, 1));
  }
  return "";
}

sub isSpace {
  my ($self, $char) = @_;
  return ($char eq " " || $char eq "\t" ||
	  $char eq "\n" || $char eq "\r");
}

sub isSlash {
  my ($self, $char) = @_;
  return $char eq "/";
}

sub isLeft {
  my ($self, $char) = @_;
  return $char eq "<";
}

sub isRight {
  my ($self, $char) = @_;
  return $char eq ">";
}

sub isEq {
  my ($self, $char) = @_;
  return $char eq "=";
}

sub isQuote {
  my ($self, $char) = @_;
  return $char eq "\"";
}

sub isAmp {
  my ($self, $char) = @_;
  return $char eq "&";
}

sub isSemicolon {
  my ($self, $char) = @_;
  return $char eq ";";
}

sub allowedName {
  my ($self, $char, $position) = @_;
  return $char =~ /$startWord/ ||
    ($position > 0 && $char =~ /$inWordExtra/);
}

sub error {
  my ($self, $description) = @_;
  my $s = "";
  if(defined $self->{file} && $self->{file} ne "") {
    $s .= $self->{file} . ": ";
  }
  $s .= "(".$self->{lineno}.", ".$self->{column}.") - ".$description;
  die $s;
}

sub parseChild {
  my ($self, $char) = @_;
  if($self->isLeft($char)) {
    if($self->{wasspace}) {
      $self->{buffer} .= ' ';
    }
    $self->{stack}->[scalar(@{ $self->{stack} })-1]->addChild($self->{buffer});
    $self->{buffer} = "";
    $self->{closing} = 0;
    $self->{state} = "tagname";
  } elsif($self->isAmp($char)) {
    $self->{entityname} = "";
    $self->{laststate} = "parsechild";
    $self->{state} = "entityname";
  } else {
    my $curnode = $self->{stack}->[scalar(@{ $self->{stack} })-1];
    if($curnode->hasAttribute('xml:space') &&
       $curnode->getAttribute('xml:space') eq "preserve") {
      $self->{buffer} .= $char;
    } else {
      if($self->isSpace($char)) {
	if(length($self->{buffer}) > 0 ||
	   scalar(@{ $curnode->getChilds() }) > 0) {
	  $self->{wasspace} = 1;
	}
      } else {
	if($self->{wasspace}) {
	  $self->{buffer} .= " ";
	  $self->{wasspace} = 0;
	}
	$self->{buffer} .= $char;
      }
    }
  }
}

sub tagName {
  my ($self, $char) = @_;
  if((!$self->{closing} &&
      $self->isSpace($char)) || $self->isRight($char)) {
    if(length($self->{buffer}) > 0) {
      if($self->{closing}) {
	if($self->{stack}->[scalar(@{ $self->{stack} })-1]->getTagName() ne
	   $self->{buffer}) {
	  $self->error("Closing tag mismatch");
	}
	if(scalar($self->{stack}) == 0) {
	  $self->error("You mustn't close the document tag");
	} else {
	  pop(@{ $self->{stack} });
	}
      } else {
	my $node = new XMXmlNode($self->{buffer}, $self->{dtd});
	$self->{stack}->[scalar(@{ $self->{stack} })-1]->addChild($node);
	push(@{ $self->{stack} }, $node);
      }
      $self->{buffer} = "";
      $self->{wasspace} = 0;
      if($self->isSpace($char)) {
	$self->{state} = "waitforattr";
      } else {
	$self->{state} = "parsechild";
      }
    } else {
      $self->error("Need tag name after <");
    }
  } elsif($self->isSlash($char) and length($self->{buffer}) == 0) {
    $self->{closing} = 1;
  } elsif($self->allowedName($char, length($self->{buffer}))) {
    $self->{buffer} .= $char;
  } else {
    $self->error("Character not allowed in tag name");
  }
}

sub waitForAttr {
  my ($self, $char) = @_;
  if($self->allowedName($char, 0)) {
    $self->{buffer} .= $char;
    $self->{state} = "attrname";
  } elsif($self->isRight($char)) {
    $self->{state} = "parsechild";
  } elsif($self->isSlash($char)) {
    $self->{state} = "nochild";
  } elsif(!$self->isSpace($char)) {
    $self->error("Character not allowed at beginning of attribute name");
  }
}

sub noChild {
  my ($self, $char) = @_;
  if($self->isRight($char)) {
    pop(@{ $self->{stack} });
    $self->{state} = "parsechild";
  } else {
    $self->error("> expected after / in tag");
  }
}

sub attrName {
  my ($self, $char) = @_;
  if($self->allowedName($char, 1)) {
    $self->{buffer} .= $char;
  } elsif($self->isSpace($char)) {
    $self->{attr} = $self->{buffer};
    $self->{buffer} = "";
    $self->{state} = "waitforeq";
  } elsif($self->isEq($char)) {
    $self->{attr} = $self->{buffer};
    $self->{buffer} = "";
    $self->{state} = "waitforvalue";
  } else {
    $self->error("Character not allowed in attribute name");
  }
}

sub waitForEq {
  my ($self, $char) = @_;
  if($self->isEq($char)) {
    $self->{state} = "waitforvalue";
  } elsif(!$self->isSpace($char)) {
    $self->error("= expected");
  }
}

sub waitForValue {
  my ($self, $char) = @_;
  if($self->isQuote($char)) {
    $self->{state} = "attrvalue";
  } elsif(!$self->isSpace($char)) {
    $self->error("\" expected");
  }
}

sub attrValue {
  my ($self, $char) = @_;
  if($self->isQuote($char)) {
    $self->{stack}->[scalar(@{ $self->{stack} })-1]->setAttribute
      ($self->{attr}, $self->{buffer});
    $self->{buffer} = "";
    $self->{state} = "waitforattr";
  } elsif($self->isAmp($char)) {
    $self->{entityname} = "";
    $self->{laststate} = "attrvalue";
    $self->{state} = "entityname";
  } else {
    $self->{buffer} .= $char;
  }
}

sub entityName {
  my ($self, $char) = @_;
  if($self->isSemicolon($char)) {
    $self->{buffer} .= $self->entity($self->{entityname});
    $self->{state} = $self->{laststate};
  } else {
    $self->{entityname} .= $char;
  }
}

sub parse {
  my ($self, $chunk) = @_;
  for(my $i=0; $i<length($chunk); ++$i) {
    my $char = substr($chunk, $i, 1);
    if($self->{lineno} == 1) {
      ;
    } elsif($self->{state} eq "tagname") {
      $self->tagName($char);
    } elsif($self->{state} eq "waitforattr") {
      $self->waitForAttr($char);
    } elsif($self->{state} eq "attrname") {
      $self->attrName($char);
    } elsif($self->{state} eq "waitforeq") {
      $self->waitForEq($char);
    } elsif($self->{state} eq "waitforvalue") {
      $self->waitForValue($char);
    } elsif($self->{state} eq "attrvalue") {
      $self->attrValue($char);
    } elsif($self->{state} eq "parsechild") {
      $self->parseChild($char);
    } elsif($self->{state} eq "nochild") {
      $self->noChild($char);
    } elsif($self->{state} eq "entityname") {
      $self->entityName($char);
    } else {
      $self->error("Invliad state: \"".$self->{state}."\"");
    }

    $self->{column} += 1;
    if($char eq "\n") {
      $self->{lineno} += 1;
      $self->{column} = 1;
    }
  }
}

sub finish {
  my ($self) = @_;
  $self->{state} = "parsechild";
  $self->{wasspace} = 0;
  $self->{lineno} = 1;
  $self->{column} = 1;
  $self->{attr} = "";
  $self->{buffer} = "";
  $self->{closing} = 0;
}

sub getNodes {
  my ($self) = @_;
  return $self->{stack}->[0]->getSubNodes();
}

sub parseFile {
  my ($self, $file) = @_;
  open(PFILE, "<$file");
  while(my $line = <PFILE>) {
    $self->parse($line);
  }
}

1;
