[%char]
%CHAR (ctce, ...)						      [Bliss]
! Converts the expressions to a concatenated string
[%string]
%STRING (string-param, ...)					      [Bliss]
! Concatenates the strings
[%exactstring]
%EXACTSTRING (length, fill, string-param, ...)			      [Bliss]
! Concatenates the strings and make the result an exact size
[%charcount]
%CHARCOUNT (string-param, ...)					      [Bliss]
! Concatenates the strings and returns the number of characters in the result
[%explode]
%EXPLODE (string-param, ...)					      [Bliss]
! Concatenates the strings and returns each character as a separate string
[%remove]
%REMOVE (parameter)						      [Bliss]
! Removes an balanced brackets (either (), [] or {})
[%name]
%NAME (string-param, ...)					      [Bliss]
! Concatenates the strings and returns the result as a name
[%null]
%NULL (parameter, ...)						      [Bliss]
! Returns 1 if all the params are null
[%identical]
%IDENTICAL (parameter, parameter)				      [Bliss]
! Returns 1 if the two parameters are identical
[%isstring]
%ISSTRING (expression)						      [Bliss]
! Returns 1 if the expression is a string literal
[%ctce]
%CTCE (expression)						      [Bliss]
! Returns 1 if the expression is a compile time constant
[%ltce]
%LTCE (expression)						      [Bliss]
! Returns 1 if the expression is a link time constant
[%nbits]
%NBITS (ctce, ...)						      [Bliss]
! Returns the minimum number of bits needed to represent the arguments
[%nbitsu]
%NBITSU (ctce, ...)						      [Bliss]
! Returns the minimum number of bits needed to represent the arguments
[%allocation]
%ALLOCATION (data-segment-name)					      [Bliss]
! Returns the number of storage units allocated to the name data segment
[%size]
%SIZE (structure-attribute)					      [Bliss]
! Returns the number of storage units for the specified structure */
[%fieldexpand]
%FIELDEXPAND (field-name, ctce)					      [Bliss]
! Returns the specified component of the fiels named
[%assign]
%ASSIGN (compiletime-name, ctce)				      [Bliss]
! Assigns the ctce to the compile-time variable
[%number]
%NUMBER (number-param)						      [Bliss]
! Returns the numeric literal formed from the argument
[%declared]
%DECLARED (name)						      [Bliss]
! Returns 1 if the name is declared
[%switches]
%SWITCHES (on-off-switch-name, ...)				      [Bliss]
! Returns 1 if all switch names match the switches in effect
[%bliss]
%BLISS (language-name)						      [Bliss]
! Return 1 if the given dialect corresponds to the current compiler
[%variant]
%VARIANT							      [Bliss]
! Returns the numeric literal specified with the /VARIANT compiler switch
[%error]
%ERROR (string-param, ...)					      [Bliss]
! Reports and error diagnostic with the concatenated string aguments
[%errormacro]
%ERRORMACRO (string-param, ...)					      [Bliss]
! Reports an error, and abort all active macros
[%warn]
%WARN (string-param, ...)					      [Bliss]
! Reports a warning diagnostic 
[%inform]
%INFORM (string-param, ...)					      [Bliss]
! Reports an informational diagnostic
[%print]
%PRINT (string-param, ...)					      [Bliss]
! Displays a message in the listing
[%message]
%MESSAGE (string-param)						      [Bliss]
! Displays a message to the terminal
[%title]
%TITLE quoted-string						      [Bliss]
! Sets the title for the module
[%sbttl]
%SBTTL quoted-string						      [Bliss]
! Sets the sub-title
[%quote]
%QUOTE								      [Bliss]
! Inhibits the binding of lexeme following the function name
[%unquote]
%UNQUOTE							      [Bliss]
! Forces binding of the lexeme following the function name
[%expand]
%EXPAND								      [Bliss]
! Forces binding of the following lexeme, and expand it as a macro if required
[%remaining]
%REMAINING							      [Bliss]
! Returns a comma separated list of the remaining macro arguments
[%length]
%LENGTH								      [Bliss]
! Returns the number of actual argument for a macro
[%count]
%COUNT								      [Bliss]
! Returns the macro recursion depth
[%exititeration]
%EXITITERATION							      [Bliss]
! Terminates the expansion of the current macro iteration
[%exitacro]
%EXITMACRO							      [Bliss]
! Terminates the current macro expansion
[%require]
%REQUIRE quoted-string						      [Bliss]
! Insert the designated file at this point in a module
[actualcount]
ACTUALCOUNT ()							      [Bliss]
! Returns the number of actual paramaters passed to a routine
[actualparameter]
ACTUALPARAMETER (i)						      [Bliss]
! Returns the value of the ith parameter
[argptr]
ARGPTR ()							      [Bliss]
! Returns the address of the argument block
[nullparameter]
NULLPARAMETER (i)						      [Bliss]
! Returns 1 if the ith parameter does not exist
[ch$ptr]
CH$PTR (addr, i, chsize)					      [Bliss]
! Creates a character string pointer
[ch$plus]
CH$PLUS (ptr, i)						      [Bliss]
! Increments the character string pointer
[ch$diff]
CH$DIFF (ptr1, ptr2)						      [Bliss]
! Takes the difference of two character strings
[ch$rchar]
CH$RCHAR (ptr)							      [Bliss]
! Returns the character pointer at by the character string pointer
[ch$wchar]
CH$WCHAR (char, ptr)						      [Bliss]
! Stores the character in the character string
[ch$rchar_a]
CH$RCHAR_A (addr)						      [Bliss]
! Returns the character, and then advances the character string pointer
[ch$wchar_a]
CH$WCHAR_A (char, addr)						      [Bliss]
! Stores thhe character and then advances the character string pointer
[ch$a_rchar]
CH$A_RCHAR (addr)						      [Bliss]
! Advances the character pointer and returns the pointed at character
[ch$a_wchar]
CH$A_WCHAR (char, addr)						      [Bliss]
! Advances the character pointer and then stores the character
[ch$allocation]
CH$ALLOCATION (n, chsize)					      [Bliss]
! Returns the amount of storage needed for the specified number of characters
[ch$size]
CH$SIZE (ptr)							      [Bliss]
! Returns the number of bits per character
[ch$move]
CH$MOVE (n, sptr, dptr)						      [Bliss]
! Moves n characters from sptr into dptr
[ch$copy]
CH$COPY (sn1, sptr1, ... fill, dn, dptr)			      [Bliss]
! Copies all the strings to the designated area
[ch$fill]
CH$FILL (fill, dn, dptr)					      [Bliss]
! Fills the designated string with a fill character
[ch$lss]
CH$LSS (n1, ptr1, n2, ptr2, fill)				      [Bliss]
! Compares to character strings
[ch$leq]
CH$LEQ (n1, ptr1, n2, ptr2, fill)				      [Bliss]
! Compares to character strings
[ch$gtr]
CH$GTR (n1, ptr1, n2, ptr2, fill)				      [Bliss]
! Compares to character strings
[ch$geq]
CH$GEQ (n1, ptr1, n2, ptr2, fill)				      [Bliss]
! Compares to character strings
[ch$eql]
CH$EQL (n1, ptr1, n2, ptr2, fill)				      [Bliss]
! Compares to character strings
[ch$neq]
CH$NEQ (n1, ptr1, n2, ptr2, fill)				      [Bliss]
! Compares to character strings
[ch$compare]
CH$COMPARE (n1, ptr1, n2, ptr2, fill)				      [Bliss]
! Returns indication of a two string comparison
[ch$find_sub]
CH$FIND_SUB (cn, cptr, pn, pptr)				      [Bliss]
! Locates a substring and returns its address
[ch$find_ch]
CH$FIND_CH (n, ptr, char)					      [Bliss]
! Finds the address of the designated character
[ch$find_not_ch]
CH$FIND_NOT_CH (n, ptr, char)					      [Bliss]
! Finds the first character other than the character specified
[ch$translatable]
CH$TRANSLATABLE (trans-string)					      [Bliss]
! Creates a translation string
[ch$translate]
CH$TRANSLATE (tab, sn, sptr, fill, dn, dptr)			      [Bliss]
! Translates a character
[ch$fail]
CH$FAIL (ptr)							      [Bliss]
! Returns 1 if  a null pointer is passed
