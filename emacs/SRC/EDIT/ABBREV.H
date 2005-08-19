/*	Copyright (c) 1982, 1983, 1984, 1985 */
/*		Barry A. Scott and nick Emery */
/* Definitions for Unix Emacs Abbrev mode */
/* An abbrev table contains an array of pointers to abbrev entries.  When a */
/* word is to be looked up in a abbrev table it is hashed to a long value and */
/* that value is taken mod the array size to get the head of the appropriate */
/* chain.  The chain is scanned for an entry whose hash matches (comparing */
/* hash values is faster than comparins strings) and whose string matches. */

#define ABBREVSIZE 87
struct abbrevent {                      /* a phrase-abbreve pair in an abbrev table */
    struct abbrevent *abbrev_next;      /* the next pair in this chain */
    unsigned char *abbrev_abbrev;       /* the abbreviation */
    unsigned char *abbrev_phrase;       /* the expanded phrase */
    int abbrev_hash;               /* abbrev_abbrev hashed */
    struct bound_name *abbrev_expansion_hook;     /* BoundName */
/* the command that will be executed when this abbrev is expanded */
    } ;
/* a table of abbreviations and their expansions */
struct abbrevtable {
    unsigned char *abbrev_name;         /* the name of this abbrev table */
    int abbrev_number_defined;      /* the number of abbrevs defined in this */
/* abbrev table */
    struct abbrevent *abbrev_table [ABBREVSIZE];
/* the array of pointers to chains of name pairs */
    } ;
