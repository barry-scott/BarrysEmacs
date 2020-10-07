//
//    unix_ext_func.cpp
//
//    Copyright (c) 1996-2003 Barry A. Scott
//
#include <emacs.h>

#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
static EmacsInitialisation emacs_initialisation( __DATE__ " " __TIME__, THIS_FILE );

class EmacsExternImageOsInfo_ForUnix: public EmacsExternImageOsInfo
{
public:
    EmacsExternImageOsInfo_ForUnix();
    ~EmacsExternImageOsInfo_ForUnix();

    bool isLoaded();
    bool load( const EmacsString &file );
    bool unload();
    void *symbolValue( const EmacsString &symbol );

    EmacsString    file;
};

extern EmacsExternImageOsInfo *make_EmacsExternImageOsInfo()
{
    return new EmacsExternImageOsInfo_ForUnix();
}

EmacsExternImageOsInfo_ForUnix::EmacsExternImageOsInfo_ForUnix()
{
}

EmacsExternImageOsInfo_ForUnix::~EmacsExternImageOsInfo_ForUnix()
{
}

bool EmacsExternImageOsInfo_ForUnix::isLoaded()
{
    return false;
}

bool EmacsExternImageOsInfo_ForUnix::load( const EmacsString &_file )
{
    file = _file;
    return false;
}

bool EmacsExternImageOsInfo_ForUnix::unload()
{
    return false;
}

void *EmacsExternImageOsInfo_ForUnix::symbolValue( const EmacsString &symbol )
{
    return NULL;
}
