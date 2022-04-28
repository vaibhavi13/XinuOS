/*
** ============================================================================= +++++++++++++++++++++++++++++++++++++++
** File         : strpbrk.c
** Author       : Mohan Shah
** Organization	: ISE, IU Bloomington, IN
** Description  : string span of initial segment
** ============================================================================= +++++++++++++++++++++++++++++++++++++++
*/

/*
** ============================================================================= +++++++++++++++++++++++++++++++++++++++
** function		: strpbrk
** synopsis		: find the first acceptable delimiting char in a string
** signature	: char *strpbrk(const char *s, const char *accept)
** input params	:
**		s is the pointer to the string to search for acceptable delimiter.
**		accept is the pointer to the string of all acceptable delimiter characters.
** return value	:
**		pointer to the first acceptable delimiter char, if found; else NULL.
** calls		:
** ============================================================================= +++++++++++++++++++++++++++++++++++++++
*/

#include <xinu.h>

char *strpbrk(const char *s, const char *accept) {
	while (*s != '\0') {
		const char *a = accept;
		while (*a != '\0')
		if (*a++ == *s) {
			return (char *) s;
		}
		s++;
	}
	return NULL;
} // end of char *strpbrk(const char *s, const char *accept)

