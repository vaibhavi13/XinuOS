/*
** ============================================================================= +++++++++++++++++++++++++++++++++++++++
** File         : test_cmdArgs.c
** Author       : Mohan Shah
** Organization	: ISE, IU Bloomington, IN
** Description  : functions to be tested in xinu: test <invocation name> <args>
** ============================================================================= +++++++++++++++++++++++++++++++++++++++
*/

/*
** ============================================================================= +++++++++++++++++++++++++++++++++++++++
** function		: test_cmdArgs
** synopsis		: invocation-name to function-call paitring is done in xsh_test.c
** signature	: int test_cmdArgs(int nargs, char *args[])
** input params	:
**		argc - just like in a typical main function - int main(int argc, char *argv[]).
**		argv - just like in a typical main function - int main(int argc, char *argv[]).
** return value	:
**		typically OK or SYSERR.
** calls		:
** ============================================================================= +++++++++++++++++++++++++++++++++++++++
*/

#include <xinu.h>
#include <stdlib.h>
#include <xclopts.h>

extern int _numGivenOtherArgs;
extern char **_otherArgs;          // pointer to the array of command args that are not options


// xinu cmd:
// test cmd the -r RRRR quick -q QQQQ brown fox -pPPPP jumps -abc over -e EE the -f FF lazy -d DD dog

int test_cmdArgs(int nargs, char *args[]) {
	int retValue = OK;

	// char *ssOptsSpec = "abcd:e:f:p::q::r::";
	char *ssOptsSpec = "abcd:e:f:p:q:r:";
	char *llOptsSpec = "a-long b-long c-long d-long: e-long: f-log: p-long:: q-long:: r-long::";
	printf("\t%s:: ssOptsSpec [%s] llOptsSpec [%s]\n", __func__, ssOptsSpec, llOptsSpec);
	printf("\t%s:: number of arguments, not counting command, is [%d]\n", __func__, nargs-1);


	cloption_t *pclo = NULL;
	while( (pclo = xgetopts (nargs,args, ssOptsSpec, llOptsSpec)) != NULL ) {
		// pclo->option, _opttypenames[pclo->optType], _optargtypenames[pclo->optArgType], pclo->optArg
		switch ( *(pclo->option) ) {
			case 'a':
				printf("\t%s:: option a selected\n", __func__);
				break;
			case 'b':
				printf("\t%s:: option b selected\n", __func__);
				break;
			case 'c':
				printf("\t%s:: option c selected\n", __func__);
				break;
			case 'd':
				printf("\t%s:: option d selected with optarg [%s]\n", __func__, pclo->optArg);
				break;
			case 'e':
				printf("\t%s:: option e selected with optarg [%s]\n", __func__, pclo->optArg);
				break;
			case 'f':
				printf("\t%s:: option f selected with optarg [%s]\n", __func__, pclo->optArg);
				break;
			case 'p':
				printf("\t%s:: option p selected with optarg [%s]\n", __func__, pclo->optArg);
				break;
			case 'q':
				printf("\t%s:: option q selected with optarg [%s]\n", __func__, pclo->optArg);
				break;
			case 'r':
				printf("\t%s:: option r selected with optarg [%s]\n", __func__, pclo->optArg);
				break;
			default:
				break;
		} // end of switch ( *(pclo->option) )
	} // end of while( (pclo = xgetopts (argc,argv, ssOptsSpec, llOptsSpec)) != NULL )

	printf("\n");
	for (int i = 0; i < _numGivenOtherArgs; i++) {
		printf("\t%s:: non-opt cmd args are [%2d]: [%s]\n", __func__, i, _otherArgs[i]);
	}
	printf("\n");

	return (retValue);
} // end of int test_cmdArgs(int nargs, char *args[])



int test_cmdArgs_02(int nargs, char *args[]) {
	int retValue = OK;

	// char *ssOptsSpec = "abcd:e:f:p::q::r::";
	char *ssOptsSpec = "abcd:e:f:p:q:r:";
	char *llOptsSpec = "a-long b-long c-long d-long: e-long: f-log: p-long:: q-long:: r-long::";
	printf("\t%s:: ssOptsSpec [%s] llOptsSpec [%s]\n", __func__, ssOptsSpec, llOptsSpec);
	printf("\t%s:: number of arguments, not counting command, is [%d]\n", __func__, nargs-1);


	cloption_t *pclo = NULL;
	// while( (pclo = xgetopts (argc,argv, ssOptsSpec, llOptsSpec)) != NULL )
	// if( init_xgetopts(nargs, args, ssOptsSpec, llOptsSpec) != XCLO_SUCCESS ) {return SYSERR;}
	while( (pclo = xgetopts (nargs,args, ssOptsSpec, llOptsSpec)) != NULL ) {
		// pclo->option, _opttypenames[pclo->optType], _optargtypenames[pclo->optArgType], pclo->optArg
		switch ( *(pclo->option) ) {
			case 'a':
				printf("\t%s:: option a selected\n", __func__);
				break;
			case 'b':
				printf("\t%s:: option b selected\n", __func__);
				break;
			case 'c':
				printf("\t%s:: option c selected\n", __func__);
				break;
			case 'd':
				printf("\t%s:: option d selected with optarg [%s]\n", __func__, pclo->optArg);
				break;
			case 'e':
				printf("\t%s:: option e selected with optarg [%s]\n", __func__, pclo->optArg);
				break;
			case 'f':
				printf("\t%s:: option f selected with optarg [%s]\n", __func__, pclo->optArg);
				break;
			case 'p':
				printf("\t%s:: option p selected with optarg [%s]\n", __func__, pclo->optArg);
				break;
			case 'q':
				printf("\t%s:: option q selected with optarg [%s]\n", __func__, pclo->optArg);
				break;
			case 'r':
				printf("\t%s:: option r selected with optarg [%s]\n", __func__, pclo->optArg);
				break;
			default:
				break;
		} // end of switch ( *(pclo->option) )
	} // end of while( (pclo = xgetopts (argc,argv, ssOptsSpec, llOptsSpec)) != NULL )

	printf("\n");
	for (int i = 0; i < _numGivenOtherArgs; i++) {
		printf("\t%s:: non-opt cmd args are [%2d]: [%s]\n", __func__, i, _otherArgs[i]);
	}
	printf("\n");

	return (retValue);
} // end of int test_cmdArgs_02(int nargs, char *args[])



/*
int testDriver_cmdArgs_03(int nargs, char *args[]) {
	int retValue = OK;

	// for (int i = 0; i < nargs; i++) {
	// 	printf("%s:: args[%d] = [%s]\n", __func__, i, args[i]);
	// }

	printf("%s::\tcmd args {", __func__);
	for (int i = 0; i < nargs; i++) {
		if (i%5 == 0) { printf("\n\t"); }
		printf("[%s]    ", args[i]);
	}
	printf("\n}\n");

	char ss[] = "        abcd:e:f:p:q:r:   ";
	char ll[] = "aa-long     bb-long cc-long: dd-long: ee-long:: ff-long::";

	printf("%s:: ssOptsSpec [%s]\n", __func__, ss);
	printf("%s:: llOptsSpec [%s]\n", __func__, ll);
	printf("\n");

	printf("testDriver_xclopts_01:: [%-7s] [%-15s] [%-10s] [%s]\n", "opttype", "option", "optargtype", "optarg");
	printf("---------------------:: [%-7s] [%-15s] [%-10s] [%s]\n", "-------", "---------------", "----------", "----------------");

	cloption_t *pclo = NULL;
	if (init_xgetopts(nargs, args, ss, ll) != XCLO_SUCCESS) {return SYSERR;}
	while ( (pclo = xgetopts(nargs, args, ss, ll)) != NULL ) {
		printf("testDriver_xclopts_01:: ");

		if      (pclo->optType == SHORTOPT) { printf("[%-7s] [%-15s] ", "short", pclo->option); }
		else if (pclo->optType == LONGOPT)  { printf("[%-7s] [%-15s] ", "long", pclo->option); }
		else if (pclo->optType == NONOPT)   { printf("[%-7s] [%-15s] ", "noopt", pclo->option); }
		else                                { printf("[%-7s] [%-15s] ", "unknwn", pclo->option); }

		if      (pclo->optArgType == NOOPTARG) { printf("[%-10s] ", "nooptarg"); }
		else if (pclo->optArgType == REQUIRED) { printf("[%-10s] ", "required"); }
		else if (pclo->optArgType == OPTIONAL) { printf("[%-10s] ", "optional"); }
		else                                   { printf("[%-10s] ", "unknown"); }

		// printf("[%s]", enQuote( escapeQuotes(pclo->optionArg)) );
		printf("[%s]", pclo->optArg);

		// printf("CLO: [%s]", toString(pclo));
		printf("\n");
	} // end of while ( (pclo = xgetopts(nargs, args, ss, lla)) != NULLi )
	printf("\n");

	printf("%s:: [%-7s] [%s]\n", __func__, "non-opt", "other args");
	printf("---------------------:: [%-7s] [%s]\n", "-------", "----------------");
	for (int j = 0; j < _numGivenOtherArgs; j++) {
		printf("%s:: ", __func__);
		// printf("[%-6s] [%s] ", "ARG", enQuote(escapeQuotes(otherArgs[j])));
		printf("[%-7s] [%s] ", "ARG", _otherArgs[j]);
		printf("\n");
	}
	printf("\n");

	// extern int _nNormArgs;
	// extern char **_normArgs;
	// for (int i = 0; i < _nNormArgs; i++) {
	// 	fprintf(stdout, "NORMALIZED: args[%2d] = [%s]\n", i, _normArgs[i]);
	// }

	return (retValue);
} // end of int testDriver_cmdArgs_03(int nargs, char *args[])
*/

