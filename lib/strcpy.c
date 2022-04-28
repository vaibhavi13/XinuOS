/* strcpy.c  -  strcpy */

/*------------------------------------------------------------------------
 * strcpy - copy the string given my the second argument into the first
 *------------------------------------------------------------------------
 */
char *strcpy(
	  char *tar,			/* target string		*/
	  const char *src			/* source string		*/
	)
{
	char *ot = tar;
	while ( (*tar++ = *src++) != '\0') {
		;
	}
	return ot;
}
