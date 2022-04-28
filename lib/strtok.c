/*
** ============================================================================= +++++++++++++++++++++++++++++++++++++++
** File         : strtok.c
** Author       : Mohan Shah
** Organization	: ISE, IU Bloomington, IN
** Description  : string tokenizer function, strtok, for xinu
** ============================================================================= +++++++++++++++++++++++++++++++++++++++
*/

/*
** ============================================================================= +++++++++++++++++++++++++++++++++++++++
** function		: strtok
** synopsis		: the string tokenizer function
** signature	: char *strtok(char *s, const char *delim)
** input params	:
**		s is the pointer to the string that will be tokenized.
**		delim is the pointer to the string of all delimiters.
** return value	:
**		token is the pointer to the next token of s.
** calls		: strspn and strpbrk (so we need the string.h)
** ============================================================================= +++++++++++++++++++++++++++++++++++++++
*/

#include <xinu.h>
// #include <string.h>								// already included from xinu.h

char *strtok(char *s, const char *delim) {
	static char *remainingStr = NULL;

	char *token = NULL;

	if (s == NULL) {
		s = remainingStr;
	}

	s = s + strspn (s, delim);					// gobble up all leading delimiters
	if (*s == '\0') {
		remainingStr = s;
		return NULL;
	}

	token = s;									// next token begins at s; where does it end?
	s = strpbrk (token, delim);
	if (s == NULL) {
		remainingStr = strchr(token, '\0');		// should be using memchr
		if (remainingStr == NULL) {
			remainingStr = s + strlen(s);
		}
	}
	else {
		*s = '\0';								// replace delim with string terminator
		remainingStr = s + 1;
	}

	return token;
} // end of char *strtok(char *s, const char *delim)

