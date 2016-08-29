//
//  emunicode.h
//
//  Copyright (c) 2010 Barry A. Scott
//
#ifndef __EMUNICODE_H__
#define __EMUNICODE_H__

#include <map>
#include <set>

typedef std::map<EmacsChar_t, EmacsChar_t> EmacsCharToCharMap_t;
typedef std::set<EmacsChar_t> EmacsCharCategorySet_t;

extern int length_utf8_to_unicode( int utf8_length, const unsigned char *utf8_data );
extern int length_utf8_to_unicode( int utf8_length, const unsigned char *utf8_data, int unicode_limit, int &utf8_usable_length );
extern void convert_utf8_to_unicode( const unsigned char *utf8_data, int unicode_length, EmacsChar_t *unicode_data );

extern int length_unicode_to_utf8( int unicode_length, const EmacsChar_t *unicode_data );
extern int length_unicode_to_utf8( int unicode_length, const EmacsChar_t *unicode_data, int utf8_limit, int &unicode_usable_length );
extern void convert_unicode_to_utf8( int unicode_length, const EmacsChar_t *unicode_data, unsigned char *utf8_data );

extern int length_utf16_to_unicode( int data_length, const unsigned char *data );
extern int length_utf16_to_unicode( int data_length, const unsigned char *data, int unicode_limit, int &utf16_usable_length );
extern void convert_utf16_to_unicode( const unsigned char *data, int unicode_length, EmacsChar_t *unicode_data );

extern int length_unicode_to_utf16( int unicode_length, const EmacsChar_t *unicode_data );
extern int length_unicode_to_utf16( int unicode_length, const EmacsChar_t *unicode_data, int utf16_limit, int &unicode_usable_length );
extern void convert_unicode_to_utf16( int unicode_length, const EmacsChar_t *unicode_data, unsigned short *utf16_data );

extern int unicode_strcmp(  int len1, const EmacsChar_t *str1, int len2, const EmacsChar_t *str2 );
extern int unicode_stricmp( int len1, const EmacsChar_t *str1, int len2, const EmacsChar_t *str2 );

bool unicode_is_space( EmacsChar_t code_point );
bool unicode_is_digit( EmacsChar_t code_point );
bool unicode_is_numeric( EmacsChar_t code_point );
EmacsCharCategorySet_t::const_iterator getNumericBegin();
EmacsCharCategorySet_t::const_iterator getNumericEnd();

bool unicode_is_alphabetic( EmacsChar_t code_point );
EmacsCharCategorySet_t::const_iterator getAlphabeticBegin();
EmacsCharCategorySet_t::const_iterator getAlphabeticEnd();

inline bool unicode_is_alphabetic_numeric( EmacsChar_t code_point )
{
    if( unicode_is_alphabetic( code_point ) )
        return true;
    else
        return unicode_is_numeric( code_point );
}

extern bool unicode_is_upper( EmacsChar_t code_point );
extern bool unicode_has_upper_translation( EmacsChar_t code_point );
extern EmacsChar_t unicode_to_upper( EmacsChar_t code_point );

extern bool unicode_is_lower( EmacsChar_t code_point );
extern bool unicode_has_lower_translation( EmacsChar_t code_point );
extern EmacsChar_t unicode_to_lower( EmacsChar_t code_point );

extern bool unicode_is_title( EmacsChar_t code_point );
extern bool unicode_has_title_translation( EmacsChar_t code_point );
extern EmacsChar_t unicode_to_title( EmacsChar_t code_point );

extern bool unicode_has_casefold_translation( EmacsChar_t code_point );
extern EmacsChar_t unicode_casefold( EmacsChar_t code_point );

#endif // __EMUNICODE_H__
