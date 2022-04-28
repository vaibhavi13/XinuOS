/*
** ============================================================================= +++++++++++++++++++++++++++++++++++++++
** File         : test_clOptsSpecs.c
** Author       : Mohan Shah
** Organization	: ISE, IU Bloomington, IN
** Description  : functions to be tested in xinu: test <invocation name> <args>
** ============================================================================= +++++++++++++++++++++++++++++++++++++++
*/

/*
** ============================================================================= +++++++++++++++++++++++++++++++++++++++
** function		: test_clOptsSpecs
** synopsis		: invocation-name to function-call paitring is done in xsh_test.c
** signature	: int test_clOptsSpecs(int nargs, char *args[])
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

extern char *_opttypenames[];
extern char *_optargtypenames[];

extern cloption_t *_allowedShortOptions;
extern int _numShortOptSpecs;

extern cloption_t *_allowedLongOptions;
extern int _numLongOptSpecs;

int processOptsSpecs(const char *optsspec, _opttype_t opttype);


// xinu cmd:
// test clo "ss opts. spec" "ll opts. spec"
// test clo "ab c d: e:f:p: q:r:" "a-long b-long c-long d-long: e-long: f-log: p-long:: q-long:: r-long::"

int test_clOptsSpecs(int nargs, char *args[]) {
	int retValue = OK;

	char *ssOptsSpec = NULL;
	char *llOptsSpec = NULL;
	if (nargs == 2) {
		ssOptsSpec = args[1];
	}
	else if (nargs > 2) {
		ssOptsSpec = args[1];
		llOptsSpec = args[2];
	}

	printf("\t%s:: number of arguments, not counting command, is [%d]\n", __func__, nargs-1);
	printf("\t%s:: ssOptsSpecs [%s] llOptsSpecs [%s]\n", __func__, ssOptsSpec, llOptsSpec);

	// test optstring for short options
	retValue = processOptsSpecs(ssOptsSpec, SHORTOPT);
	if (retValue != XCLO_SUCCESS) {
		printf("\t%s:: ssOptsSpec [%s] cannot be processed\n", __func__, ssOptsSpec);
		return retValue;
	}
	printf("\tCLO:: [%-7s] [%-15s] [%-10s] [%s]\n", "opttype", "option", "optargtype", "optarg");
	printf("\t---:: [%-7s] [%-15s] [%-10s] [%s]\n", "-------", "---------------", "----------", "----------------");
	cloption_t *pclo;
	for (int i = 0; i < _numShortOptSpecs; i++) {
		pclo = _allowedShortOptions + i;
		printf("\tCLO: [%-7s] [%-15s] [%-10s] [%s]\n",
			_opttypenames[pclo->optType], pclo->option, _optargtypenames[pclo->optArgType], pclo->optArg);
	}
	printf("\n");

	printf("\t%s:: llOptsSpec [%s] LONGOPTS not implemented yet\n", __func__, llOptsSpec);
	/*
	// test optstring for long options
	retValue = processOptsSpecs(llOptsSpec, LONGOPT);
	if (retValue != XCLO_SUCCESS) {
		printf("%s:: llOptsSpec [%s] cannot be processed\n", __func__, llOptsSpec);
		return retValue;
	}
	printf("CLO:: [%-7s] [%-15s] [%-10s] [%s]\n", "opttype", "option", "optargtype", "optarg");
	printf("---:: [%-7s] [%-15s] [%-10s] [%s]\n", "-------", "---------------", "----------", "----------------");
	option_t *pclo;
	for (int i = 0; i < _numLongOptSpecs; i++) {
		pclo = _allowedLongOptions + i;
		printf("CLO: [%-7s] [%-15s] [%-10s] [%s]\n",
			_opttypenames[pclo->optType], pclo->option, _optargtypenames[pclo->optArgType], pclo->optarg);
	}
	*/
	printf("\n");

	return (retValue);
} // end of int test_clOptsSpecs(int nargs, char *args[])

