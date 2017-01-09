use XMMapper;

use strict;
use warnings;
use diagnostics;


use vars qw($Mapper);

statusWindow()->setWordWrapColumn(80);
statusWindow->print(ansi("Xpertmud Mapper started %r%r%r"));

$Mapper=new XMMapper("XMMapper");

