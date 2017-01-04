#!/usr/bin/ruby

$functions = "";
$definitions = "";

STDIN.each { |line|
  if line =~ /\s+virtual\s+([^\s]+)\s+([^\(]+)\(/
    line = $~.post_match;
    functionType = $1;
    functionName = $2;
    spacedName = "RB" + functionName;
    initCode = "";
    rubyName = functionName.gsub(/^XM_/,'');
    
    #TODO adjust
#    $definitions += "    {const_cast<char *>(\"%s\"), \t%s, \tMETH_VARARGS, const_cast<char *>(\"Not documented\")},\n" % [ rubyName, spacedName ];
    
    callBack = "  ";
    if functionType != "void"
      callBack += "%s ret = " % functionType;
    end
    callBack += "currentCallBack->\n      %s(" % functionName;
    argString = " ";
    paramString = "";
    hasDefault = 0;
    hasCodec = 0;
    paramCount = 0;
    while line =~ /^\s*([^,\)]+),?/;
      paramCount+=1;
      arg = $1;
      line = $~.post_match;
      m = /^(.*)\s+([^\s]+)$/.match(arg);
      type = m[1];
      type.sub!(/const QString&/, 'char*');
      name = m[2];
      defaultValue = nil;
      if type == "char*"
	initCode += "  QString #{name};\n";
      else
	initCode +="  #{type} #{name};\n";
      end
      m = /^([^=]+)=(.*)$/.match(name);
      if m != nil
	name = m[1];
	defaultValue = m[2];
	initCode += "  if(p#{name} != Qnil) {\n  ";
      end
      if argString != ""
	argString += ", ";
      end
      argString += "VALUE p#{name}";
      if paramString != ""
	paramString += ", ";
      end
      paramString += "#{name}";
      if type == "bool"
	initCode += "  if(TYPE(p#{name}) != T_TRUE and TYPE(p#{name}) != T_FALSE and TYPE(p#{name}) != T_NIL) {\n    std::cout << TYPE(p#{name});\n    rb_raise(rb_eTypeError, \"#{name} has no valid value\");\n    return Qnil;\n  }\n";
	initCode += "  #{name} = (p#{name} == Qtrue ? true:false);\n";
      elsif type == 'unsigned int'
	initCode += "  if(TYPE(p#{name}) != T_FIXNUM and TYPE(p#{name}) != T_BIGNUM) {\n    std::cout << TYPE(p#{name});\n    rb_raise(rb_eTypeError, \"#{name} has no valid value\");\n    return Qnil;\n  }\n";
	initCode += "  #{name} = NUM2UINT(p#{name});\n";
      elsif type == 'int'
	initCode += "  if(TYPE(p#{name}) != T_FIXNUM and TYPE(p#{name}) != T_BIGNUM) {\n    std::cout << TYPE(p#{name});\n    rb_raise(rb_eTypeError, \"#{name} has no valid value\");\n    return Qnil;\n  }\n";
	initCode += "  #{name} = NUM2INT(p#{name});\n";
      elsif type == 'char*'
	initCode += "  if(TYPE(p#{name}) != T_STRING) {\n    std::cout << TYPE(p#{name});\n    rb_raise(rb_eTypeError, \"#{name} has no valid value\");\n    return Qnil;\n  }\n";
	initCode += "  #{name} = QString::fromUtf8(STR2CSTR(p#{name}));\n";
      end
      if(m != nil)
	initCode += "  }\n";
      end
    end
    callBack+=paramString + ");\n";
    $definitions += "    RbMethodDef(const_cast<char*>(\"#{rubyName}\"), #{paramCount},  reinterpret_cast<VALUE (&)(...)>(#{spacedName})),\n";
    $functions += "VALUE %s(VALUE /* unused */" % spacedName;
    $functions += argString + ") {\n";
    $functions += initCode;
    $functions += callBack;
    if functionType == "void"
      $functions += "  return Qtrue;\n}\n";
    elsif functionType == "bool"
      $functions += "  return (ret ? Qtrue:Qfalse);\n}\n";
    elsif functionType == "int" or functionType == "unsigned int"
      $functions += "  return INT2NUM(ret);\n}\n";
    elsif functionType == "char"
      $functions += "  return rb_str_new(&ret,1);\n}\n";
    elsif functionType == "QString"
      $functions += "  QCString ctmp = ret.utf8();\n  return rb_str_new(ctmp,ctmp.length());\n}\n";      
    end
  end
}

#print $functions
#print $definitions

File.open(ARGV[0], "r") { |f|
  f.each { |line|
    if line =~ /___IMPLEMENTATIONS___/
      puts $functions;
    elsif line =~ /___DEFINITIONS___/
      puts $definitions;
    else
      puts line;
    end
  }
}
