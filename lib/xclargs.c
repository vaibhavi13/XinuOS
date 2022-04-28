/*
** ============================================================================= +++++++++++++++++++++++++++++++++++++++
** File         : xclargs.c
** Author       : Mohan Shah
** Organization	: ISE, IU Bloomington, IN
** Description  : functions for processing xinu command line args as given by user
** ============================================================================= +++++++++++++++++++++++++++++++++++++++
*/

#include "xclopts.h"


// extern char *_opttypenames[];	// allocated in xclopts.c
// extern char *_optargtypenames[];	// allocated in xclopts.c


int _nNormCmdArgs;					// number of normalized arguments sans command name (i.e., agrv[0])
char **_normCmdArgs;				// array of normalized arguments sans command name (i.e., agrv[0])

int _numGivenOpts;
cloption_t *_givenOptions;			// pointer to the array of CLOptions as given by user on the cmd line

int _numGivenOtherArgs;
char **_otherArgs;					// pointer to the array of command args that are not options


// process given command line args:
//		- split conjoined short options
//		- normalize command args
//		- parse command args
int splitShortCmdArg(char **dest, const char *arg);						// cmd args.
int normalizeCmdArgs(int argc, char* argv[]);							// cmd args.
int parseCmdArgs(int argcN, char* argvN[]);								// cmd args.
int processCmdArgs(int argc, char* argv[]);								// cmd args.

int findInAllowedOptions(char *option, _opttype_t opttype);
int processOptsSpecs(const char *optsspec, _opttype_t opttype);


extern const char * const _delim;	// string of all only white-spaces. allocated in xclopts.c

/*
** ============================================================================= +++++++++++++++++++++++++++++++++++++++
** function		: splitShortCmdArg
** synopsis		: split conjoined short options.
** signature	: int splitShortCmdArg(char **dest, const char *arg)
** input params	:
**		dest is an array of strings that consists of split split short options.
**		arg is source token that will be looked for if any spliting is required.
** return value	:
**		XCLO_SUCCESS or XCLO_ERROR (on any erroneous condition)
** calls		:
** ============================================================================= +++++++++++++++++++++++++++++++++++++++
*/

int splitShortCmdArg(char **dest, const char *arg) {
	int retValue = XCLO_SUCCESS;

	*dest = NULL;

	if (arg == NULL || strlen(arg) == 0) {
		return retValue;
	}

	// worst case: -abcx, i.e., "abcx" (4) -> "-a -b -c -x" (11 = 4*3-1)
	char *head = NULL;
	// head = *dest = (char *) malloc(sizeof(char) * strlen(arg)*3 - 1 + 1 );
	head = *dest = (char *) getmem(sizeof(char) * strlen(arg)*3 - 1 + 1 );
	if (head == NULL) {
		LOG2("could not allocate memory for head=*dest");
		return (retValue = XCLO_ERROR);
	}
	memset(head, 0, strlen(arg)*3);
	int currHeadLength = 0;

	// char *tail = (char *) malloc(sizeof(char)*strlen(arg)+1);
	char *tail = (char *) getmem(sizeof(char)*strlen(arg)+1);
	if (tail == NULL) {
		LOG2("could not allocate memory for tail");
		return (retValue = XCLO_ERROR);
	}
	memset(tail, 0, sizeof(char)*strlen(arg)+1);
	strcpy(tail, arg);

	// if ( (*tail == '-') && (*(tail+1) == '-') ) // not necessary as we are dealing with only SHORTOPT
	if (*tail == '-') {
		tail = tail+1;
	}
	// LOG1("inside loop head [%s] tail [%s]", head, tail);

	int index;
	int chunkLen = 1; // we are dealing here with only SHOTROPT;
	char currOpt[2] = {'\0', '\0'};

	while (*tail != '\0') {
		strncpy(currOpt, tail, chunkLen);

		if ( (index = findInAllowedOptions(currOpt, SHORTOPT)) == INDEX_NOT_FOUND ) {
			LOG2("arg [%s] processed upto [%s]; unrecognized short option [%s]", arg, head, currOpt);
			return (retValue = XCLO_ERR_UNKNOWN_SSOPT);
		}

		// here means - currOpt is a valid option: append it to head
		*head = '-'; head++; currHeadLength++;
		*head = *currOpt; head++; currHeadLength++;

		if (_allowedShortOptions[index].optArgType == NOOPTARG) {
			// nothing to do: remaining taili, if any, will continue splitting
		}
		else if (_allowedShortOptions[index].optArgType == REQUIRED) {
			// required optarg could be coming from the next argv !!! so no check here !!! so no check here
			if (tail != NULL && strlen(tail+1) > 0) {
				*head = ' '; head++; currHeadLength++;
				strcpy(head, tail+1);
				currHeadLength = currHeadLength + strlen(tail+1);
			}
			break;
		}
		/*
		else if (_allowedShortOptions[index].optArgType == OPTIONAL) {
			*head = '='; head++; currHeadLength++;
			break;
		}
		*/
		else {
			LOG2("unrecognized OptArgType [%d]", _allowedShortOptions[index].optArgType);
			return (retValue = XCLO_ERR_UNKNOWN_OPTARGTYPE);
		}

		tail = tail + 1;	// chunkLen is 1.
		if(strlen(tail) > 0) {
			*head = ' '; head++; currHeadLength++;
		}
	} // end of while (*t != 0)

	// LOG1("orig arg [%s] head is [%s] currHeadLength [%d]", arg, head, currHeadLength);
	return retValue;
} // end of int splitShortCmdArg(char **dest, const char *arg)


/*
** ============================================================================= +++++++++++++++++++++++++++++++++++++++
** function		: normalizeCmdArgs
** synopsis		: normalize argv[] to normCmdArgs[] - no two opts or an opt & optarg conjoined
** signature	: int normalizeCmdArgs(int argc, char* argv[])
** input params	:
**		argc - the arguments count
**		argv[] is the array of command line arguments
** return value	:
**		XCLO_SUCCESS or XCLO_ERROR (on any erroneous condition)
**		sets the global vars: int _nNormCmdArgs and char **_normCmdArgs
** calls		: splitShortCmdArg
** ============================================================================= +++++++++++++++++++++++++++++++++++++++
*/

int normalizeCmdArgs(int argc, char* argv[]) {
	int retValue = XCLO_SUCCESS;

	// first we convert (int argc, char **argv) to (int _nNormCmdArgs, char **_normCmdArgs)
	// where _normCmdArgs is an array of "normalized" arguments (i.e. de-clubbed and
	// with a single space as delimiter between two arguments as well as an option
	// and its option-argument) sans-command (i.e., argv[0]).

	_nNormCmdArgs = 0;
	_numGivenOpts = 0;
	_numGivenOtherArgs = 0;

	// first estimate (a lower bound) of _nNormCmdArgs : only short options could be clubbed together
	int i;
	// since argv[0] is the command, we start from index 1.
	for (i = 1; i < argc; i++) {
		if (strncmp(argv[i], "--", 2) == 0) {
			// LONGOPT // these are not clubbed
			_nNormCmdArgs++;
		}
		else if (strncmp(argv[i], "-", 1) == 0) {
			// SHORTOPT // short opts could be clubbed together and -1 for the "-"
			_nNormCmdArgs = _nNormCmdArgs + strlen(argv[i]) - 1;
		}
		else {
			// other args (or non-option cmd args)
			_nNormCmdArgs++;
		}
	} // end of for (i = 1; i < argc; i++)


	// _normCmdArgs = (char **) malloc(sizeof(char *)*_nNormCmdArgs);
	_normCmdArgs = (char **) getmem(sizeof(char *)*_nNormCmdArgs);
	if (_normCmdArgs == NULL) {
		LOG2("could not allocate memory for _normCmdArgs (normalized argv)");
		return (retValue = XCLO_ERROR);
	}

	int len, nn = 0;
	// since argv[0] is the command, we start the loop from from index 1.
	for (i = 1; i < argc; i++) {

		if (strncmp(argv[i], "--", 2) == 0) {
			// it is still kept separate to tell apart from short option starting with single '-'
			len = strlen(argv[i]);
			// _normCmdArgs[nn] = (char *) malloc(sizeof(char)*len + 1);
			_normCmdArgs[nn] = (char *) getmem(sizeof(char)*len + 1);
			if (_normCmdArgs[nn] == NULL) {
				LOG2("no mem for _normCmdArgs[%d] for argv[%d]=%s", nn, i, argv[i]);
				return (retValue = XCLO_ERROR);
			}
			memset(_normCmdArgs[nn], 0, len+1);
			strcpy(_normCmdArgs[nn], argv[i]);
			_numGivenOpts++;
			nn++;
		} // end of if (strncmp(argv[i], "--", 2) == 0)

		else if (strncmp(argv[i], "-", 1) == 0) {
			// worst case "-abcx" : "abcx" (3) --> "-a -b -c -x" (11 = 4*3-1)
			char *splitArgs = NULL;
			retValue = splitShortCmdArg(&splitArgs, argv[i] + 1);
			char *temp = splitArgs;
			int templen = strlen(splitArgs);
			// TODO: check the retValue

			char *tkn = strtok(splitArgs, _delim);
			while (tkn != NULL) {
				len = strlen(tkn);
				// _normCmdArgs[nn] = (char *) malloc(sizeof(char)*len + 1);
				_normCmdArgs[nn] = (char *) getmem(sizeof(char)*len + 1);
				if (_normCmdArgs[nn] == NULL) {
					LOG2("no mem for _normCmdArgs[%d] argv[%d]=%s", nn, i, argv[i]);
					return (retValue = XCLO_ERROR);
				}
				memset(_normCmdArgs[nn], 0, len+1);
				strcpy(_normCmdArgs[nn], tkn);
				if( strncmp(tkn, "-", 1) == 0 ) {
					_numGivenOpts++;
				}
				else {
					_numGivenOtherArgs++;
				}
				nn++;
				tkn = strtok(NULL, _delim);
			} // end of while (tkn != NULL)
			// free (temp);
			freemem (temp, templen);
		} // end of else if (strncmp(argv[i], "-", 1) == 0)

		else {
			len = strlen(argv[i]);
			// _normCmdArgs[nn] = (char *) malloc(sizeof(char)*len + 1);
			_normCmdArgs[nn] = (char *) getmem(sizeof(char)*len + 1);
			if (_normCmdArgs[nn] == NULL) {
				LOG2("no mem for _normCmdArgs[%d] argv[%d]=%s", nn, i, argv[i]);
				return (retValue = XCLO_ERROR);
			}
			memset(_normCmdArgs[nn], 0, len+1);
			strcpy(_normCmdArgs[nn], argv[i]);
			_numGivenOtherArgs++;
			nn++;
		}

	} // end of for (i = 1; i < argc; i++)

	// current value of nn is the correct value of _nNormCmdArgs
	_nNormCmdArgs = nn;

	return (retValue);
} // end of int normalizeCmdArgs(int argc, char* argv[])


/*
** ============================================================================= +++++++++++++++++++++++++++++++++++++++
** function		: parseCmdArgs
** synopsis		: parse given comnmand line arguments - classify them and detect errors
** signature	: int parseCmdArgs(int argcN, char* argvN[])
** input params	:
**		argcN - the normalized-arguments count
**		argvN[] is the array of normalized-arguments
** return value	:
**		XCLO_SUCCESS or XCLO_ERROR (on any erroneous condition)
**		sets the global vars: int _numGivenOpts & cloption_t *_givenOpts
**		sets the global vars: int _numGivenOtherArgs & char **_otherArgs
** calls		:
** ============================================================================= +++++++++++++++++++++++++++++++++++++++
*/

int parseCmdArgs(int argcN, char* argvN[]) {
	int retValue = XCLO_SUCCESS;
	// here argvN is the normalized array of cmd args sans command (i.e., argv[0])
	// _numGivenOpts and _numGivenOtherArgs are already best-estimated in the call
	// to the function int normalizeCmdArgs(int argc, char* argv[])

	// _givenOptions = (cloption_t *) malloc(sizeof(cloption_t)*_numGivenOpts);
	_givenOptions = (cloption_t *) getmem(sizeof(cloption_t)*_numGivenOpts);
	if (_givenOptions == NULL) {
		LOG2("could not allocate memory for givenShortOptions");
		return (retValue = XCLO_ERROR);
	}
	memset(_givenOptions, 0, sizeof(cloption_t)*_numGivenOpts);

	_otherArgs = (char **) getmem(sizeof(char *)*(_numGivenOtherArgs+1));
	if (_otherArgs == NULL) {
		LOG2("could not allocate memory for _otherArgs");
		return (retValue = XCLO_ERROR);
	}
	memset(_otherArgs, 0, sizeof(char *)*(_numGivenOtherArgs+1));

	char *currOption;
	_opttype_t currOptType;
	char *currOptarg = NULL;
	_optargtype_t currOptargType;

	char *currArg;
	int index, pp = 0, rr = 0;	// pp for inserting into givenOption and rr for inserting into _otherArgs

	for (int i = 0; i < argcN; i++) {
		currArg = argvN[i];

		currOptType = UNKNOWN_OPTTYPE;
		currOptarg = NULL;
		currOptargType = UNKNOWN_OPTARGTYPE;

#ifdef XCLO_LONGOPTS
		// currArg is an option of type LONGOPT
		if (strncmp(temp, "--", 2) == 0) {
		} // end of if (strncmp(temp, "--", 2) == 0)
		else
#endif

		// currArg is an option of type LONGOPT
		if (strncmp(argvN[i], "-", 1) == 0) {
			currOption = currArg+1;
			currOptType = SHORTOPT;

			if ( (index = findInAllowedOptions(currOption, currOptType)) == INDEX_NOT_FOUND) {
				LOG2("unrecognized short [%d] option [%s]", currOptType, currOption);
				return (XCLO_ERR_UNKNOWN_SSOPT);
			}

			currOptargType = _allowedShortOptions[index].optArgType;

			if (currOptargType == NOOPTARG) {
				// nothing to gobble up
			}
			else if (currOptargType == REQUIRED) {
				// gobble up the next argvN
				if ( i < (argcN -1) ) {
					if (currOptarg == NULL) {
						// we can gobble up the next one
						// currOptarg = (char *) malloc(sizeof(char)*strlen(argvN[i+1]) + 1);
						currOptarg = (char *) getmem(sizeof(char)*strlen(argvN[i+1]) + 1);
						if (currOptarg == NULL) {
							LOG2("could not allocate memory for currOptarg to gobble up argvN[%d] %s", i+1, argvN[i+1]);
							return (retValue = XCLO_ERROR);
						}
						memset(currOptarg, 0, strlen(argvN[i+1]) + 1);
						strcpy(currOptarg, argvN[i+1]);
						i = i + 1;
					}
					else {
						// nothing left to gobble up
						LOG2("currOptarg missing for argvN[%d] %s", i, argvN[i]);
						return (retValue = XCLO_ERR_NO_OPTARG);
					}
				}
			}
			/*
			else if (currOptargType == OPTIONAL) {
			}
			*/
			else {
				LOG2("unrecognized short currOptargType [%d]", currOptargType);
				return (XCLO_ERR_UNKNOWN_OPTARGTYPE);
			}

		} // end of else if (strncmp(temp, "-", 1) == 0)


		// currArg is NOT an option, but an ordinary command argument
		else {
			currOption = currArg;
			currOptType = NONOPT;
			currOptarg = NULL;
			// currOptargType = ???; we do not care
		}

		// Now insert into either _givenOptions or _otherArgs
		if (currOptType == SHORTOPT
#ifdef XCLO_LONGOPTS
			currOptType == LONGOPT ||
#endif
		) {
			// we insert into _givenOptions
			// _givenOptions[pp].option = (char *) malloc(sizeof(char)*strlen(currOption)+1);
			_givenOptions[pp].option = (char *) getmem(sizeof(char)*strlen(currOption)+1);
			if (_givenOptions[pp].option == NULL) {
				LOG2("could not allocate memory _givenOptions[%d].option %s", pp, currOption);
				return (retValue = XCLO_ERROR);
			}
			memset(_givenOptions[pp].option, 0, sizeof(char)*strlen(currOption) + 1);
			strcpy(_givenOptions[pp].option, currOption);

			_givenOptions[pp].optType = currOptType;

			if (currOptarg != NULL) {
				// _givenOptions[pp].optArg = (char *) malloc(sizeof(char)*strlen(currOptarg)+1);
				_givenOptions[pp].optArg = (char *) getmem(sizeof(char)*strlen(currOptarg)+1);
				if (_givenOptions[pp].optArg == NULL) {
					LOG2("no mem _givenOptions[%d].optionArg %s", pp, currOptarg);
					return (retValue = XCLO_ERROR);
				}
				memset(_givenOptions[pp].optArg, 0, sizeof(char)*strlen(currOptarg) + 1);
				strcpy(_givenOptions[pp].optArg, currOptarg);
			}
			else {
				_givenOptions[pp].optArg = NULL;
			}

			_givenOptions[pp].optArgType = currOptargType;

			pp++;
		}

		else if (currOptType == NONOPT) {
			// we insert into _otherArgs
			_otherArgs[rr] = (char *) getmem(sizeof(char)*(strlen(currOption)+1));
			if (_otherArgs[rr] == NULL) {
				LOG2("could not allocate memory _otherArgs[%d] %s", rr, currOption);
				return (retValue = XCLO_ERROR);
			}
			memset(_otherArgs[rr], 0, sizeof(char)*(strlen(currOption)+1));
			strcpy(_otherArgs[rr], currOption);

			rr++;
		}

		// free(currOption);
		freemem(currOptarg, sizeof(char)*strlen(argvN[i+1]) + 1);
	} // end of for (i = 0; i < argcN; i++)

	if (_numGivenOpts != pp) {
		_numGivenOpts = pp;
	}
	if (_numGivenOtherArgs != rr) {
		_numGivenOtherArgs = rr;
	}

#ifdef DEBUG
	printf("DEBUG:: exiting %s\n", __func__);
#endif
	return (retValue);
} // end of int parseCmdArgs(int argcN, char* argvN[])


/*
** ============================================================================= +++++++++++++++++++++++++++++++++++++++
** function		: processCmdArgs
** synopsis		: normalize and parse given command line args
** signature	: int processCmdArgs(int argc, char* argv[])
** input params	:
**		argc - the arguments count
**		argv[] is the array of arguments
** return value	:
**		XCLO_SUCCESS or XCLO_ERROR (on any erroneous condition in called functions)
** calls		: normalizeCmdArgs and parseCmdArgs
** ============================================================================= +++++++++++++++++++++++++++++++++++++++
*/

int processCmdArgs(int argc, char* argv[]) {
	int retValue = XCLO_SUCCESS;

	if ( (retValue = normalizeCmdArgs(argc, argv)) != XCLO_SUCCESS) {
		return retValue;
	}

	retValue = parseCmdArgs(_nNormCmdArgs, _normCmdArgs);

	return (retValue);
} // end of int processCmdArgs(int argc, char* argv[])


/*
** ============================================================================= +++++++++++++++++++++++++++++++++++++++
** function		: xgetopts
** synopsis		: the main API for the programmer for handling CLArgs
** signature	: cloption_t *xgetopts(int nargs, char *args[], const char *ss, const char *ll)
** input params	:
**		nargs - the number of arguments
**		args[] - the array of arguments
**		ss - the opts specifications for short options
**		ll - the opts specifications for long options
** return value	:
**		a pointer to cloption_t or NULL (on exhausting all cl-options)
** calls		: processOptsSpecs and processCmdArgs
** ============================================================================= +++++++++++++++++++++++++++++++++++++++
*/

cloption_t *xgetopts(int nargs, char *args[], const char *ss, const char *ll) {
	cloption_t *pclo = NULL;

	static int freeingRequired = 0;
	if (freeingRequired) {
		freemem((char *)_givenOptions, sizeof(cloption_t)*_numGivenOpts);


		// the allocation was like:
		//	_otherArgs = (char **) getmem(sizeof(char *)*(_numGivenOtherArgs+1));
		//			_otherArgs[rr] = (char *) getmem(sizeof(char)*(strlen(currOption)+1));

		for (int i = 0; i < _numGivenOtherArgs; i++) {
			freemem(_otherArgs[i], sizeof(char)*(strlen(_otherArgs[i]+1)));
		}
		freemem((char *)_otherArgs, sizeof(char *)*(_numGivenOtherArgs+1));
	} // end of if (freeingRequired)


	static int initDone = 0;
	if ( ! initDone ) {
		int retValue;
		retValue = processOptsSpecs(ss, SHORTOPT);
		if (retValue != XCLO_SUCCESS) {
			fprintf(stderr, "processing error [%d] in short Options Spec [%s]\n", retValue, ss);
			return (pclo = NULL);
		}
		retValue = processCmdArgs(nargs, args);
		if (retValue != XCLO_SUCCESS) {
			fprintf(stderr, "errors in processing command arguments\n");
			return (pclo = NULL);
		}
		initDone = 1;
	} // end of if ( ! initDone )


	static int currIndx = 0;
	if (currIndx < _numGivenOpts) {
		pclo = _givenOptions + currIndx;
	}
	else {
		pclo = NULL;
	}
	currIndx++;

	if (pclo == NULL) {
		// this is the terminating call - next call requires freeing followed by init
		freeingRequired = 1;
		initDone = 0;
		currIndx = 0;
	}


	return pclo;
} // end of cloption_t *xgetopts(int nargs, char *args[], const char *ss, const char *ll)

