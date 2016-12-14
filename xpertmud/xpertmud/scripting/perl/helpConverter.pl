use strict;
use warnings;
use diagnostics;

use xmlparser;
use helplayout;

my $dtd = new helpdtd();
my $allHelp = new XMXmlNode('help');
$allHelp->setAttribute('name', 'help');

foreach my $file (@ARGV) {
  my $p = new XMXmlParser($dtd);
  $p->parseFile($file);
  foreach my $node (@{ $p->getNodes() }) {
    foreach my $inner (@{ $node->getSubNodes() }) {
      $allHelp->addChild($inner);
    }
  }
}

my $mode = "docbook";

my $layout = new helplayout($mode);
print $layout->generate($allHelp);
