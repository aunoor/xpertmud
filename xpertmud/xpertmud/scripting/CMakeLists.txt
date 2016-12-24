cmake_minimum_required ( VERSION 2.8.12 )

if(PYTHON_SUPPORT)
    add_subdirectory(python)
endif(PYTHON_SUPPORT)

if(PERL_SUPPORT)
    add_subdirectory(perl)
endif(PERL_SUPPORT)