use IO::File;
my $dir="/tmp/htmlmap";

my $mapWidth=1000;
my $width=100;
my $mapHeight=1000;
my $height=100;
my $overviewwidth=600;
my $overviewheight=400;
my %offsets=(
	     "01"=>10,
	     "04"=>3,
#	     "08"=>2,
#	     "10"=>1
);
my %raster= (
	     "01"=>[ 250,250 ],
	     "04"=>[ 100,50 ],
#	     "08"=>[ 50,50 ]
	    );
my @zooms=sort keys %offsets;
$Core->parse("#HUD:xperthud:GS:R# XX,0,0,0,,,,,,,,,,,,");

my %cliffs=(
	    none=>"#HUD:xperthud:SGI:R# -,-,-,-,-,-,-,-,-,-",
	    mech=>"#HUD:xperthud:SGI:R# B,-,-,-,-,-,-,-,-,-",
	    hover=>"#HUD:xperthud:SGI:R# H,-,-,-,-,-,-,-,-,-"
	    );
# generate overview map
$Map->setZoom(1);
$Core->parse($cliffs{none});
$Map->saveImage($dir."/proto_overview.png",0,0,1000,1000);
system("/usr/bin/convert","-size","600x400",$dir."/proto_overview.png","-scale","600x400",$dir."/overview.png");
unlink($dir."/proto_overview.png");
foreach my $zoom (@zooms) {
  $width= $raster{$zoom}->[0];
  $height= $raster{$zoom}->[1];
  foreach my $cliff (sort keys %cliffs) {
$Core->parse($cliffs{$cliff});
    my $header = q[<html>
<head>
<title>Xpertmud - HTML Map</title>
</head>
<body>
<map name="map_map">
];
    for(my $yy=0; $yy < $mapHeight; $yy += $height) {
      for(my $xx=0; $xx < $mapWidth; $xx += $width) {
	my ($sx,$sy) = (int($xx*$overviewwidth/$mapWidth),int($yy*$overviewheight/$mapHeight));
	my ($ex,$ey) = (int(($xx+$width)*$overviewwidth/$mapWidth -1),int(($yy+$height)*$overviewheight/$mapHeight-1));
	$header .= qq[  <area shape="rect" coords="].$sx.",".$sy.",".$ex.",".$ey.qq[" href="map_${zoom}_${xx}_${yy}_${cliff}.html" alt="(].$xx.",".$yy.qq[)-(].($xx+$width).",".($yy+$height).qq[)" title="(].$xx.",".$yy.qq[)-(].($xx+$width).",".($yy+$height).qq[)" />\n];
      }
    }
    $header .= qq[</map>\n];
    my $offset=$offsets{$zoom};
    $Map->setZoom($zoom);
    for(my $x=0; $x < $mapWidth; $x += $width) {
      for(my $y=0; $y < $mapHeight; $y += $height) {
	my $fname="$dir/map_${zoom}_${x}_${y}_${cliff}";
	$Map->saveImage($fname.".png",$x-$offset,$y-$offset,
			$x+$width+$offset,$y+$height+$offset);
	my $html=new IO::File(">$fname.html");
	$html->print($header);
	$html->print(q[
      <table border="0">
	<tr><th>Zoom Level:</th>
       ]);
	foreach my $z (@zooms) {
	  if ($z == $zoom) {
	    $html->print(qq[<td>].($z+0).qq[</td>]);
	  } else {
	    my $nx = int($x/$raster{$z}->[0])*$raster{$z}->[0];
	    my $ny = int($y/$raster{$z}->[1])*$raster{$z}->[1];
	    $html->print(qq[<td><a href="map_${z}_${nx}_${ny}_${cliff}.html">].($z+0).q[</a></td>]);
	  }
	}
	$html->print(qq[<td width="30">&nbsp;</td><th>Cliffs:</th>]);
	foreach my $c (sort keys %cliffs) {
	  if ($c eq $cliff) {
	    $html->print(qq[<td>$c</td>]);
	  } else {
	    $html->print(qq[<td><a href="map_${zoom}_${x}_${y}_${c}.html">$c</a></td>]);
	  }
	}
	my $bwidth=int(600*100/$mapWidth);
	$html->print(qq[</tr>\n</table>\n<br />\n]);
	$html->print(qq[<h1>Map (${x},${y})-(].($x+$width).",".($y+$height).qq[)</h1><br />]);
	$html->print(q[<table border="0" cellspacing="0" cellpadding="0">
<tr><td width="30">(]);
	if($x == 0) {
	  $html->print("<b>0</b>,");
	} else {
	  $html->print("0,");
	}
	if($y == 0) {
	  $html->print("<b>0</b>");
	} else {
	  $html->print("0");
	}
	$html->print(qq[)</td><td width="].int($bwidth/2).q[">&nbsp;</td>]);
	my $k = 1;
	for(my $px=100; $px < $mapWidth; $px += 100) {
	  ++$k;
	  if($px == $x) {
	    $html->print(qq[<td align="center" width="${bwidth}"><b>${px}</b></td>]);
	  } else {
	    $html->print(qq[<td align="center" width="${bwidth}">${px}</td>]);
	  }
	}
	my $l = int($mapHeight/100);
	my $bheight=int(400*100/$mapHeight);
	$html->print(qq[<td>&nbsp;</td></tr><tr><td height="].($bheight/2).qq[">&nbsp;</td><td rowspan="].($l+1).qq[" colspan="].($k+1).qq["><img src="overview.png" border="0" usemap="#map_map" /></td></tr>]);
	for(my $py = 100;$py < $l*100; $py += 100) {
	  if($py == $y) {
	    $html->print(qq[<tr><td height="${bheight}" valign="middle"><b>${py}</b></td></tr>]);
	  } else {
	    $html->print(qq[<tr><td height="${bheight}" valign="middle">${py}</td></tr>]);
	  }
	}
	$html->print(qq[<tr><td>&nbsp;</td></tr></table><br />\n]);
	$html->print(qq[<table border="0">\n<tr><td>&nbsp;</td><td align="center">]);
	if($y > 0) {
	  $html->print(qq[<a href="map_${zoom}_${x}_].($y-$height).qq[_${cliff}.html">^</a>]);
	} else {
	  $html->print(qq[&nbsp;]);
	}
	$html->print(qq[<tr>\n<td>]);
	if($x > 0) {
	  $html->print(qq[<a href="map_${zoom}_].($x-$width).qq[_${y}_${cliff}.html">&lt;</a>]);
	} else {
	  $html->print(qq[&nbsp;]);
	}
	$html->print(qq[</td><td><img src="map_${zoom}_${x}_${y}_${cliff}.png" border="0"  /></td><td>]);
	if($x+$width < $mapWidth) {
	  $html->print(qq[<a href="map_${zoom}_].($x+$width).qq[_${y}_${cliff}.html">&gt;</a>]);
	} else {
	  $html->print(qq[&nbsp;]);
	}
	$html->print(qq[</td></tr>\n<tr><td>&nbsp;</td><td align="center">]);
	if($y+$height < $mapHeight) {
	  $html->print(qq[<a href="map_${zoom}_${x}_].($y+$height).qq[_${cliff}.html">v</a>]);
	} else {
	  $html->print(qq[&nbsp;]);
	}
	$html->print(qq[</td><td>&nbsp;</td></tr>\n</table>\n]);
	$html->print("</body></html>");
	$html->close();
      }
    }
  }
}
