//
//    emacs.h
//
//    Copyright 1986-2010 Barry A. Scott
//
#if defined( _WIN32 ) && defined( _DEBUG )
#undef _DEBUG
#endif

#include <emacsutl.h>

//
//    classes that need forward declaration
//
class EmacsInitialisation;
class EmacsBuffer;
class SyntaxTable;
class EmacsWindowRing;
class EmacsArray;
class Marker;
class Binding;
class ProgramNode;
class BoundName;
class KeyMap;
class EmacsBuffer;
class AbbrevTable;
class EmacsBufferJournal;
class EmacsProcess;
class VariableName;
class EmacsString;

//
//    externs that need forward declaration
//
extern EmacsBuffer *bf_cur;

//
//
//
const int must_be_zero( 0 );

//
//    structure definitions
//
#include <emobject.h>
#include <emexcept.h>
#include <emstring.h>
#include <fileio.h>
#include <emstrtab.h>
#include <queue.h>
#include <mlispexp.h>
#include <search.h>
#include <syntax.h>
#include <buffer.h>
#include <mlisp.h>

#include <em_time.h>
#include <display.h>
#include <keyboard.h>
#include <key_names.h>
#include <abbrev.h>
#include <ndbm.h>
#include <tt.h>
#include <window.h>
#include <undo.h>
#include <journal.h>
#include <minibuf.h>

#include <fileserv.h>
#include <getfile.h>
#include <getdirectory.h>
#include <getdb.h>
#include <CommandLine.h>

//
//    declare routines as external
//
#include <em_rtn.h>

//
//    declare variables as external
//
#include <em_var.h>

//
//    macros and inline fucntions
//
#include <em_mac.h>


//
//    Save class definitions
//
#include <em_util.h>

