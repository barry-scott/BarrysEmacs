//
//    Copyright (c) 2010 Barry A. Scott
//
extern int length_utf8_to_unicode( int utf8_length, const unsigned char *utf8_data );
extern int length_utf8_to_unicode( int utf8_length, const unsigned char *utf8_data, int unicode_limit, int &utf8_usable_length );
extern void convert_utf8_to_unicode( const unsigned char *utf8_data, int unicode_length, EmacsChar_t *unicode_data );
extern int length_unicode_to_utf8( int unicode_length, const EmacsChar_t *unicode_data );
extern int length_unicode_to_utf8( int unicode_length, const EmacsChar_t *unicode_data, int utf8_limit, int &unicode_usable_length );
extern void convert_unicode_to_utf8( int unicode_length, const EmacsChar_t *unicode_data, unsigned char *utf8_data );

extern int unicode_strcmp(  int len1, const EmacsChar_t *str1, int len2, const EmacsChar_t *str2 );
extern int unicode_stricmp( int len1, const EmacsChar_t *str1, int len2, const EmacsChar_t *str2 );