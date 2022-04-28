/*
** ============================================================================= +++++++++++++++++++++++++++++++++++++++
** File         : strspn.c
** Author       : Mohan Shah
** Organization	: ISE, IU Bloomington, IN
** Description  : string span of initial segment
** ============================================================================= +++++++++++++++++++++++++++++++++++++++
*/

/*
** ============================================================================= +++++++++++++++++++++++++++++++++++++++
** function		: strspn
** synopsis		: the length of the initial maximal string span
** signature	: int strspn(const char *s, const char *accept)
** input params	:
**		s is the pointer to the string to search for initial maximal span of acceptable chars.
**		accept is the pointer to the string of all accepted characters (in the span).
** return value	:
**		the length of the initial (i.e. starting at s) maxiaml span of only acceptable chars.
** calls		:
** ============================================================================= +++++++++++++++++++++++++++++++++++++++
*/

int strspn(const char *s, const char *accept) {
	int span = 0;

	const char *a, *p;
	for (p = s; *p != '\0'; p++) {
		for (a = accept; *a != '\0'; a++) {
			if (*p == *a)
				// current letter p is in accept, no need to test further in accept
				break;
		}
		// here either *p is found at a or a = '\0' (i.e. *p is NOT found)
		if (*a == '\0') {
			return span;
		}
		span++;
	} // end of for (p = s; *p != '\0'; ++p)

	return span;
} // end of int strspn(const char *s, const char *accept)

