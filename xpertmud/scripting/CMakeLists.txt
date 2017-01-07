cmake_minimum_required ( VERSION 3.0.2 )

if(PYTHON_SUPPORT)
    message(STATUS "Enable Python support")
    add_subdirectory(python)
endif(PYTHON_SUPPORT)

if(PERL_SUPPORT)
    message(STATUS "Enable Perl support")
    add_subdirectory(perl)
endif(PERL_SUPPORT)

if(RUBY_SUPPORT)
    message(STATUS "Enable Ruby support")
    add_subdirectory(ruby)
endif(RUBY_SUPPORT)