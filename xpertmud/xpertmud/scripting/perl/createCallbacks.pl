#!/usr/bin/perl -w

use strict;

if(! defined $ARGV[0]) {
  die "Give an argument!";
}

my $functions = "";
my $xsses = "";
my $wrapperH = "#undef bool\n";
my $wrapperCC = "\#include \"AutoGuiScriptingWrapper.h\"\n\#include \"../GuiScriptingBindings.h\"\n\#include <qcstring.h>\n\n";

while(my $line = <STDIN>) {
  if($line =~ s/\s+virtual\s+([^\s]+)\s+([^\(]+)\(//) {
    my $functionType = $1;
    my $functionName = $2;
    #    my $functionArgs = $3;
    my $perlName = "";
    if($functionName =~ /XM_([^_]+)_?(.*)/) {
      if($2) { $perlName = "XM$1::$2"; }
      else   { $perlName = "XM::$1"; }
    }
    $functions .= "\n  XS($functionName) {\n    dXSARGS;\n    FIXPERLWARN;\n\n";
    $xsses .= "   newXS((char*)\"$perlName\",\n        $functionName, (char*)__FILE__);\n";

    my $wrapperType = "$functionType";
    $wrapperType =~ s/QString/LenChar/;
    my $wrapperName = "$functionName(";
    my $wrapperCallback = "";
    if($functionType ne "void") {
      $wrapperCallback = "$functionType ret =\n    ";
    }
    $wrapperCallback .= "callBack->$functionName(";
    my $wrapperCommands = "";

    my $numArgs = 0;
    my $minArgs = 0;
    my $callBack = "    ";
    if($functionType ne "void") {
      $callBack .= "$functionType ret = ";
      $callBack =~ s/QString/LenChar/;
    }
    $callBack .= "currentCallBack->\n        $functionName(";
    my $errMsg = "Usage: $perlName(";
    my $readVariables = "";
    while($line =~ s/^\s*([^,\)]+),?//) {
      $1 =~ /^(.*)\s+([^\s]+)$/;
      my $type = $1;
      my $name = $2;
      my $oldType = $type;
      $type =~ s/QString&/char*/;

      my $defaultValue = undef;
      if($name =~ /^([^=]+)=(.*)$/) {
	$name = $1;
	$defaultValue = $2;
      }
      $callBack .= "$name,";
      $wrapperName .= "$type $name,";
      if($type eq "const char*") {
	$callBack .= $name."Len,";
	$wrapperName .= "int ".$name."Len,";
	$wrapperCallback .= "q$name,";
	$wrapperCommands .= "  QString q$name = QString::fromUtf8($name, ${name}Len);\n";
      } else {
	$wrapperCallback .= "$name,";
      }
	
      if($name eq "id") {
	$readVariables .= "    LOAD_ID;\n\n";
	++$numArgs;
	next;
      }

      if(defined $defaultValue) { $errMsg .= "\$$name = $defaultValue, "; }
      else                      { $errMsg .= "\$$name, "; ++$minArgs; }

      $readVariables .= "    $type $name";
      if(defined $defaultValue) {
	$readVariables .= " = $defaultValue;\n    ";
	if($type eq "const char*") {
	  $readVariables .= "STRLEN ${name}Len = strlen($name);\n    ";
	}
	$readVariables .= "if(items > $numArgs)\n      ";
      } else {
	$readVariables .= ";\n    ";
	if($type eq "const char*") {
	  $readVariables .= "STRLEN ${name}Len = 0;\n    ";
	}
      }
      if($type eq "int") {
	$readVariables .= "$name = SvIV(ST($numArgs));\n\n";
      } elsif($type eq "unsigned int") {
	$readVariables .= "$name = SvUV(ST($numArgs));\n\n";
      } elsif($type eq "bool") {
	$readVariables .= "$name = (SvIV(ST($numArgs)) != 0);\n\n";
      } elsif($type eq "const char*") {
	$readVariables .= "$name = SvPV(ST($numArgs), ${name}Len);\n\n";
      }

      ++$numArgs;
    }
    $errMsg =~ s/,\s+$//; $errMsg .= ")";
    $callBack =~ s/,\s*$//; $callBack .= ")";
    $wrapperName =~ s/,\s*$//; $wrapperName .= ")";
    $wrapperCallback =~ s/,\s*$//; $wrapperCallback .= ")";

    $wrapperH .= "  $wrapperType $wrapperName;\n";
    $wrapperCC .= "$wrapperType GuiScriptingWrapper::${wrapperName} {\n$wrapperCommands  $wrapperCallback;\n";

    if($minArgs != $numArgs) {
      $functions .= "    if(items < $minArgs || items > $numArgs)\n      croak(\"$errMsg\");\n\n";
    } else {
      $functions .= "    if(items != $numArgs)\n      croak(\"$errMsg\");\n\n";
    }

    $functions .= $readVariables;
    $functions .= "    dXSTARG;\n    FIXPERLWARN2;\n\n";

    if($functionType ne "void") {
      if($functionType eq "QString") {
	$wrapperCC .= "  QCString locallyEncoded=ret.utf8();\n  LenChar retc; retc.pointer = qstrdup(locallyEncoded.data());\n  retc.length = locallyEncoded.length();\n  return retc;\n";
      } else {
	$wrapperCC .= "  return ret;\n";
      }
    }
    $wrapperCC .= "}\n\n";

    $functions .= "$callBack;\n";
    if($functionType eq "void") {
      $functions .= "    XSRETURN_UNDEF;\n  }\n";
    } elsif($functionType eq "int" || $functionType eq "char") {
      $functions .= "    XSRETURN_IV(ret);\n  }\n";
    } elsif($functionType eq "bool") {
      $functions .= "    if(ret)\n      XSRETURN_YES;\n    else\n      XSRETURN_NO;\n  }\n";
    } elsif($functionType eq "QString") {
      $functions .= "    if(ret.pointer) {\n      ST(0)=sv_2mortal(newSVpv(ret.pointer,ret.length));\n      SvUTF8_on(ST(0));     delete[] ret.pointer;\n      XSRETURN(1);\n    } else {\n      XSRETURN_UNDEF;\n    }\n  }\n";
    }
  }
}

if($ARGV[0] eq "interpret_cc") {
  open(PERLIN, "<$ARGV[1]");
  while(<PERLIN>) {
    if(/___IMPLEMENTATIONS___/) {
      print $functions;
    } elsif(/___XSSES___/) {
      print $xsses;
    } else {
      print;
    }
  }
  close(PERLIN);
} elsif($ARGV[0] eq "wrapper_h") {
  open(PERLIN, "<$ARGV[1]");
  while(<PERLIN>) {
    if(/___DEFINITIONS___/) {
      print $wrapperH;
    } else {
      print;
    }
  }
  close(PERLIN);
} elsif($ARGV[0] eq "wrapper_cc") {
  print $wrapperCC;
}

#print $functions;
#print $xsses;
#print $wrapperCC;
