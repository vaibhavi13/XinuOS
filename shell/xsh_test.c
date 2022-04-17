/*
** ============================================================================= +++++++++++++++++++++++++++++++++++++++
** File         : xsh_test.c
** Author       : Mohan Shah
** Organization	: ISE, IU Bloomington, IN
** Description  : xinu command for testing other functions.
** ============================================================================= +++++++++++++++++++++++++++++++++++++++
*/

/*
** ============================================================================= +++++++++++++++++++++++++++++++++++++++
** function		: xsh_test
** synopsis		: this function will be called as "test" from xinu shell (xsh)
** signature	: shellcmd xsh_hello(int nargs, char *args[])
** input params	:
**		nargs is the number of command line arguments (just like argc in a typical main function).
**		args is array of those arguments (as strings) (just like argv in a typical main function).
**		args[0] is always the invocation name of this function (as given in the shell.c)
** return value	:
**		OK or SYSERR
** calls		:
** ============================================================================= +++++++++++++++++++++++++++++++++++++++
*/

#include <xinu.h>


shellcmd xsh_test(int nargs, char *args[]) {

	/*
	for (int i = 0; i < nargs; i++) {
		printf("%s:: args[%d] = [%s]\n", __func__, i, args[i]);
	}
	*/

	if (nargs < 2) {
		// note that here args[0] is "test" (this function)
		printf("USAGE: %s <name> <args>\n", args[0]);
		return SYSERR;
	}

	printf("%s:: ", args[0]);
	// preparing to pass the args to the invocation of args[1]:
	args++;
	nargs--;
	// note that args has moved forward (and nargs has decreased by 1)
	printf("invoking command {%s}\n\twith %d args {\n", args[0], nargs-1);

	int i;
	for (i = 1; i < nargs; i++) {
		if ((i-1)%5==0) {printf("\t\t");}
		printf("[%d]=[%s] ", i, args[i]); 
		if (i%5 == 0) {printf("\n");}
	}
	if (i > 1) {
		if (((i-1)%5) != 0) { printf("\n\t"); }
		else { printf("\t"); }
	}
	else { printf("\t"); }
	printf("}\n\n");

	// call a function here that is to be tested

	return OK;
} /* end of shellcmd xsh_test(int nargs, char *args[]) */

