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

int test_clOptsSpecs(int nargs, char *args[]);

int test_cmdArgs(int nargs, char *args[]);
int test_cmdArgs_02(int nargs, char *args[]);
int test_cmdArgs_03(int nargs, char *args[]);

int test_memory(int nargs, char *args[]);


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
	printf("invoking command {%s}\n\twith args {\n\t\t", args[0]);

	for (int i = 1; i < nargs-1; i++) {
		printf("[%d]=[%s] ", i, args[i]); 
		if (i%5 == 0) {printf("\n\t\t");}
	}
	if (nargs > 1) {
		printf("[%d]=[%s] ", nargs-1, args[nargs-1]);
	}
	printf("\b\n\t}\n\n");


	if (strcmp(args[0], "clo") == 0) {
		test_clOptsSpecs(nargs, args);
	}

	else if (strcmp(args[0], "cmd") == 0) {
		test_cmdArgs(nargs, args);
		// test_cmdArgs_02(--nargs, ++args);
		// test_cmdArgs(--nargs, ++args);
		// test_cmdArgs_02(--nargs, ++args);
	}

	else if (strcmp(args[0], "mem") == 0) {
		test_memory(nargs, args);
		// test_memory(nargs, args);
	}

	else {
		fprintf(stderr, "test command [%s] not found...\n", args[0]);
	}

	return OK;
} /* end of shellcmd xsh_test(int nargs, char *args[]) */

