/*
** ============================================================================= +++++++++++++++++++++++++++++++++++++++
** File         : xclopts.c
** Author       : Mohan Shah
** Organization	: ISE, IU Bloomington, IN
** Description  : functions required for processing of command line opts specs.
** Options Specifications (aka optstring) specify what are the valid options and
** if those options require additional optargs or not.
** ============================================================================= +++++++++++++++++++++++++++++++++++++++
*/

#include "xclopts.h"

char *_opttypenames[] = {"nonopt", "shortopt", "longopt"};
char *_optargtypenames[] = {"nooptarg", "required", "optional"};


cloption_t *_allowedShortOptions;
int _numShortOptSpecs;

cloption_t *_allowedLongOptions;
int _numLongOptSpecs;

// first process options specifications:
//		- check syntax
//		- process opt. spec.
//		- extract allowed cloptions
int checkOptSpecsSyntax(const char *optsspec, _opttype_t opttype);		// opts. spec.
int splitShortOptsSpec(char **dest, const char *optsspec);				// opts. spec. NOTE: *dest will be destroyed
int extractAllowedCLOptions(const char *optsspec, _opttype_t opttype);	// opts. spec.

int findInAllowedOptions(char *option, _opttype_t opttype);

const char * const _delim = "\t\n\v\f\r ";


// int duplicateiOptSpecs(const char *optsspec, _opttype_t opttype) {
// } // end of int duplicateiOptSpecs(const char *optsspec, _opttype_t opttype)


/*
** ============================================================================= +++++++++++++++++++++++++++++++++++++++
** function		: checkOptSpecsSyntax
** synopsis		: check the syntax of opts. spec.
** signature	: int checkOptSpecsSyntax(const char *optsspec, _opttype_t opttype)
** input params	:
**		optsspec - string that gives opt spedcification (for a single opt)
**		optype - option type (either SHOROPT, LONGOPT, NONOPT or UNKNOWN)
** return value	:
**		XCLO_SUCCESS or XCLO_ERROR
** calls		:
** ============================================================================= +++++++++++++++++++++++++++++++++++++++
*/

int checkOptSpecsSyntax(const char *optsspec, _opttype_t opttype) {
	int retValue = XCLO_SUCCESS;

	if (optsspec == NULL || strlen(optsspec) == 0) {
		return retValue;
	}

	// first let us left trim the optsspec and find its length when effectively right-trimmed also:
	int oslen = strlen(optsspec);
	while ( isspace(*optsspec) ) { optsspec++; oslen--; }
	int end_pos = strlen(optsspec) - 1;
	while ( isspace(optsspec[end_pos]) ) { end_pos--; oslen--; }

	// STEP 01 : make sure that it starts with an alpha
	if ( ! isalpha(*optsspec) ) {
		LOG2("optsspec [%s] starts with an illegal char", optsspec);
		return (retValue = XCLO_ERR_OPTSPEC_SNTX);
	}

	// STEP 02 : make sure that there are only allowed chars and prepare for duplicate specs. check
	// char *tempOS = (char *) malloc( sizeof(char) * oslen + 1);
	char *tempOS = (char *) getmem( sizeof(char) * oslen + 1);
	if (tempOS == NULL) {
		LOG2("could not allocate memory\n");
		return (retValue = XCLO_ERROR);
	}
	memset(tempOS, 0, oslen+1);

	char c;
	int k = 0;
	for (int i = 0; i < strlen(optsspec); i++) {
		c = optsspec[i];
		if ( isalnum(c) ) {
			// allowed chars for short as well long optsspec are: upper-case, lower-case, digits
			*(tempOS+k++) = c;
		}
		else if ( c == ':' || isspace(c) ) {
			// other allowed chars for short as well as long optsspec are:
			// ':' to change the optargtype and whitespace to delimit (necessary for long optsspec)
		}
		else {
			LOG2("[step 02] optsspec [%s] invalid char [%c] at index %d", optsspec , c, i);
			if (opttype == SHORTOPT) {
				return (retValue = XCLO_ERR_SSOPTSPEC_SNTX);
			}
			else {
				return (retValue = XCLO_ERR_OPTSPEC_SNTX);
			}
		}
	} // end of for (i = 0; i < strlen(optsspec); i++)
	*(tempOS+k++) = '\0';

	if (opttype == SHORTOPT) {
		for(int i = 0; i < strlen(tempOS) - 1; i++) {
			for(int k = i+1; k < strlen(tempOS); k++) {
				if (tempOS[k] == tempOS[i]) {
					LOG2("tempOS = [%s] has duplicate opts. specs.%c %c", tempOS, tempOS[k], tempOS[i]);
					return (retValue = XCLO_ERR_OPTSPEC_SNTX);
				}
			}
		}
	}
	else {
		LOG2("[step 02] optsspec [%s]. wrong opttype [%s]  error", optsspec, _opttypenames[opttype]);
		return (retValue = XCLO_ERR_UNKNOWN_OPTTYPE);
	}
	// free(tempOS);
	freemem(tempOS, sizeof(char) * oslen + 1);


	// STEP 03 : check for illegal tokens ":::{0,}" nor "--{0,}", nor " :"
	// if ( (strstr(optsspec, ":::")!=NULL) || (strstr(optsspec, "--")!=NULL) || (strstr(optsspec, " :")!=NULL) )
		// LONGOPTS : make sure that we do not have something like ":::{0,}" nor "--{0,}", nor " :"
	if (opttype == SHORTOPT) {
		// SHORTOPTS : make sure that we do not have something like "::{0,}", nor " :"
		char *t = strchr(optsspec, ':');
		if ( (strstr(optsspec, "::")!=NULL) || (t != NULL && isspace(*(t-1))) ) {
			LOG2("[step 03] optsspec [%s] syntax error: contains '::' or ' :'", optsspec);
			return (retValue = XCLO_ERR_SSOPTSPEC_SNTX);
		}
	}
	else {
		LOG2("[step 03] optsspec [%s]. wrong opttype [%s]  error", optsspec, _opttypenames[opttype]);
		return (retValue = XCLO_ERR_UNKNOWN_OPTTYPE);
	}


	// STEP 04 : then make sure that SPECS do not start nor end with '-' (applies to only long options specs?)
	// if ( (strstr(optsspec, " -") != NULL) || (strstr(optsspec, "- ") != NULL) )
	// 	LOG2("[step 03] optsspec [%s] syntax error\n", optsspec);

	return retValue;
} // end of int checkOptSpecsSyntax(const char *optsspec, _opttype_t opttype)


/*
** ============================================================================= +++++++++++++++++++++++++++++++++++++++
** function		: splitShortOptsSpec
** synopsis		: short opt specs could be conjoined - split them
** signature	: int splitShortOptsSpec(char **dest, const char *optsspec)
** input params	:
**		optsspec is string that gives opts spedcifications (aka optstring)
**		optype is option type (either SHOROPT, LONGOPT, NONOPT or UNKNOWN)
** return value	:
**		XCLO_SUCCESS or XCLO_ERROR
** calls		:
** ============================================================================= +++++++++++++++++++++++++++++++++++++++
*/

int splitShortOptsSpec(char **dest, const char *optsspec) {
	int retValue = XCLO_SUCCESS;

	*dest = NULL;
	if (optsspec == NULL) {
		// return (s);
		return retValue;
	}

	int start_pos = 0;
	while ( isspace(optsspec[start_pos]) ) { start_pos++; }
	int end_pos = strlen(optsspec) - 1;
	while ( isspace(optsspec[end_pos]) ) { end_pos--; }

	int len = (end_pos - start_pos + 1);
	if (len <= 0) { return retValue; }

	// at most, e.g., "abc" -> "a b c" (i.e. n->2n-1 and additional 1 for null termination)
	// s = (char *) malloc( sizeof(char)*(2*len - 1) + 1);
	char *s = NULL;
	// s = *dest = (char *) malloc( sizeof(char)*(2*len - 1) + 1);
	s = *dest = (char *) getmem( sizeof(char)*(2*len - 1) + 1);
	if (s == NULL) {
		LOG2("could not allocate memory\n");
		return (retValue = XCLO_ERROR);
	}
	memset(s, 0, 2*len);

	int i = start_pos, k = 0;
	s[k++] = optsspec[i++];
	while ( i < end_pos + 1 ) {
		if ( isspace(optsspec[i]) ) {
			// ignore;
		}
		else if ( isalnum(optsspec[i]) ) {
			s[k++] = ' ';
			s[k++] = optsspec[i];
		}
		else {
			s[k++] = optsspec[i];
		}
		i++;
	}
	s[k++] = '\0';

	return retValue;
} // end of char *splitShortOptsSpec(const char *optsspec)


/*
** ============================================================================= +++++++++++++++++++++++++++++++++++++++
** function		: extractAllowedCLOptions
** synopsis		: short opt specs could be conjoined - split them
** signature	: int extractAllowedCLOptions(const char *optsspec, _opttype_t opttype)
** input params	:
**		optsspec is syntax-checked and in compact normalized format (single space delimited
**			and eparate opt-specs consisting of only alnums (and alos '-' for longopts)
**		optype is option type (either SHOROPT, LONGOPT, NONOPT or UNKNOWN)
** return value	:
**		XCLO_SUCCESS or XCLO_ERROR
** calls		:
** ============================================================================= +++++++++++++++++++++++++++++++++++++++
*/

int extractAllowedCLOptions(const char *optsspec, _opttype_t opttype) {
	int retValue = XCLO_SUCCESS;

	// here we assume that optsspec is already syntax-checked and in compact normalized format
	// (single space delimited opt-specs and option consists of only alnums (or '-' for LONGOPT types)
	// and opt. specs. neither start nor end with '-'.

	int nOptSpecs = 0;
	// char *tmp = (char *) malloc(sizeof(char)*strlen(optsspec)+1);
	char *tmp = (char *) getmem(sizeof(char)*strlen(optsspec)+1);
	char *s = tmp;
	if (tmp == NULL) {
		LOG2("could not allocate memory for tmp");
		return (retValue = XCLO_ERROR);
	}
	memset(tmp, 0, strlen(optsspec)+1);
	strcpy(tmp, optsspec);
	char *tkn = strtok(tmp, _delim);	// splitShortOptsSpec has already done its job.
	while (tkn != NULL) {
		nOptSpecs++;
		tkn = strtok(NULL, _delim);
	}
	// free(s);	// as tmp is changed by strtok, we use s.
	freemem(s, sizeof(char)*strlen(optsspec)+1);

	cloption_t *allowedOptions;
	if (opttype == SHORTOPT) {
		// _allowedShortOptions = (cloption_t*) malloc(sizeof(cloption_t)*nOptSpecs);
		_allowedShortOptions = (cloption_t*) getmem(sizeof(cloption_t)*nOptSpecs);
		if (_allowedShortOptions == NULL) {
			LOG2("could not allocate memory for allowedShortOptions");
			return (retValue = XCLO_ERROR);
		}
		memset(_allowedShortOptions, 0, sizeof(cloption_t)*nOptSpecs);
		allowedOptions = _allowedShortOptions;
		_numShortOptSpecs = nOptSpecs;
	}
	else {
		fprintf(stderr, "extractAllowedCLOptions:: UNKNOWN opttype %d\n", opttype);
		return (retValue = XCLO_ERR_UNKNOWN_OPTTYPE);
	}

	int len;
	// char *temp = (char *) malloc(sizeof(char)*strlen(optsspec)+1);
	char *temp = (char *) getmem(sizeof(char)*strlen(optsspec)+1);
	if (temp == NULL) {
		LOG2("could not allocate memory for temp");
		return(retValue = XCLO_ERROR);
	}
	memset(temp, 0, strlen(optsspec)+1);

	strcpy(temp, optsspec);
	// char delim[] = {'', '', '', '', '', ''};
	// char *delim = "\t\n\v\f\r ";
	char *token = strtok(temp, _delim);

	int i = 0;
	while (token != NULL) {
		// LOG1("token is [%s]", token);
		if ( (strlen(token) == 2) && (strstr(token, ":") != NULL) ) {
			len = strlen(token) - 1;
			// allowedOptions[i].option = (char *) malloc(sizeof(char) * len + 1);
			allowedOptions[i].option = (char *) getmem(sizeof(char) * len + 1);
			if (allowedOptions[i].option == NULL) {
				LOG2("no mem for REQUIRED allowedOptions[%d].option", i);
				return (retValue = XCLO_ERROR);
			}
			memset(allowedOptions[i].option, 0, len + 1);
			strncpy(allowedOptions[i].option, token, len);
			// LOG1("copied option is [%s]", allowedOptions[i]);
			allowedOptions[i].optArgType = REQUIRED;
		}
		else if ( strlen(token) == 1 ) {
			len = strlen(token);
			// allowedOptions[i].option = (char *) malloc(sizeof(char) * len + 1);
			allowedOptions[i].option = (char *) getmem(sizeof(char) * len + 1);
			if (allowedOptions[i].option == NULL) {
				LOG2("no mem for NOOPTARG allowedOptions[%d].option", i);
				return (retValue = XCLO_ERROR);
			}
			memset(allowedOptions[i].option, 0, len + 1);
			strncpy(allowedOptions[i].option, token, len);
			allowedOptions[i].optArgType = NOOPTARG;
		}
		else {
			// you should never be here
			LOG2("UNKNOWN token[%d] %s\n", i, token);
			return (retValue = XCLO_ERR_OPTSPEC_SNTX);
		}

		if (opttype == SHORTOPT) {
			allowedOptions[i].optType = SHORTOPT;
		}
		else {
			LOG2("UNKNOWN OptionType %d\n", opttype);
			return (retValue = XCLO_ERR_UNKNOWN_OPTTYPE);
		}

		allowedOptions[i].optArg = NULL;

		token = strtok(NULL, _delim);
		i++;
	}  // end of while (token != NULL)
	// free(temp);
	freemem(temp, sizeof(char)*strlen(optsspec)+1);

	return retValue;
} // end of int extractAllowedCLOptions(const char *optsspec, _opttype_t opttype)


/*
** ============================================================================= +++++++++++++++++++++++++++++++++++++++
** function		: findInAllowedOptions
** synopsis		: find the option in the array of allowed options
** signature	: int findInAllowedOptions(char *option, _opttype_t opttype)
** input params	:
**		option - the option to be found
**		optype - option type (either SHOROPT, LONGOPT, NONOPT or UNKNOWN)
** return value	:
**		index in the array of allowed options where the option is found (or INDEX_NOT_FOUND)
** calls		:
** ============================================================================= +++++++++++++++++++++++++++++++++++++++
*/

int findInAllowedOptions(char *option, _opttype_t opttype) {
	int retValue = INDEX_NOT_FOUND;

	cloption_t* allowedOptions;
	int numOpts;

	if (opttype == SHORTOPT) {
		allowedOptions = _allowedShortOptions;
		numOpts = _numShortOptSpecs;
	}
#ifdef XCLO_LONGOPTS
	else if (opttype == LONGOPT) {
		allowedOptions = _allowedLongOptions;
		numOpts = _numLongOptSpecs;
	}
#endif
	else {
		fprintf(stderr, "findInAllowedOptions:: UNKNOWN opttype %d\n", opttype);
		return (retValue = XCLO_ERR_UNKNOWN_OPTTYPE);
	}

	int i;
	for (i = 0; i < numOpts; i++) {
		// LOG1("comparing option [%s] with allowedOptions[%d]=[%s]", option, i, allowedOptions[i]);
		if (strcmp(option, allowedOptions[i].option) == 0) {
			retValue = i;
			break;
		}
	}

	return (retValue);
} // end of int findInAllowedOptions(char *option, _opttype_t opttype)


/*
** ============================================================================= +++++++++++++++++++++++++++++++++++++++
** function		: processOptsSpecs
** synopsis		: the main function to process given opts. spec and its opttype
** signature	: int processOptsSpecs(const char *optsspec, _opttype_t opttype)
** input params	:
**		optsspec - the opts. spec that will sytax-checked and normalized
**		optype - option type (either SHOROPT, LONGOPT, NONOPT or UNKNOWN)
** return value	:
**		XCLO_SUCCESS or XCLO_ERROR
** calls		: checkOptSpecsSyntax, splitShortOptsSpec and extractAllowedCLOptions
** ============================================================================= +++++++++++++++++++++++++++++++++++++++
*/

int processOptsSpecs(const char *optsspec, _opttype_t opttype) {
	int retValue = XCLO_SUCCESS;

	retValue = checkOptSpecsSyntax(optsspec, opttype);
	if (retValue != XCLO_SUCCESS) {
		LOG2("synatx error [%d] in short Options Spec [%s]", retValue, optsspec);
		return (retValue);
	}

	char *tempOSpec = NULL;
	retValue = splitShortOptsSpec(&tempOSpec, optsspec);
	if (retValue != XCLO_SUCCESS) {
		LOG2("could not split/normalize short Options Spec [%s] error [%d]", optsspec, retValue);
		return (retValue);
	}


	retValue = extractAllowedCLOptions(tempOSpec, opttype);
	if (retValue != XCLO_SUCCESS) {
		LOG1("tempOSpec [%s]", tempOSpec);
		LOG2("could not extract information for Options Spec [%s] error [%d]", optsspec, retValue);

	}
	// free(tempOSpec);
	freemem(tempOSpec, strlen(tempOSpec));

	return retValue;
} // end of int processOptsSpecs(const char *optsspec, _opttype_t opttype)


