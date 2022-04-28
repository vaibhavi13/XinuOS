/*
** ============================================================================= +++++++++++++++++++++++++++++++++++++++
** File         : xclopts.h
** Author       : Mohan Shah
** Organization	: ISE, IU Bloomington, IN
** Description  : header file for exposing the interface of the xgetopts
** ============================================================================= +++++++++++++++++++++++++++++++++++++++
*/

#ifndef __XCLOPTS_H						// xinu command line options
#define __XCLOPTS_H

#include <xinu.h>
// #include <stdio.h>					// already included from xinu.h
// #include <string.h>					// already included from xinu.h
#include <stdlib.h>
#include <ctype.h>


// enum to determine if a cmd line argument is an option (short or long) or not
typedef enum {
	UNKNOWN_OPTTYPE = -1,
	NONOPT,
	SHORTOPT,							// single alnum char. Specified by -<alnum char>
	LONGOPT								// string of alnums and '-'. Specified by --<string>
} _opttype_t;


// enum to determine if an option takes as argument or not
typedef enum {
	UNKNOWN_OPTARGTYPE = -1,
	NOOPTARG,							// option does not take an optarg
	REQUIRED,							// option requires a mandatory optarg
	OPTIONAL							// option takes an optarg if given
} _optargtype_t;


// struct to capture the essentials of a "command line option"
typedef struct  {
	char*			option;				// name or identifier of the option
	_opttype_t		optType;			// type of the option - either SHORTOPT or LONGOPT
	_optargtype_t 	optArgType;			// type of the optionArg - either NOOPTARG or REQUIRED or OPTIONAL
	char*			optArg;				// option argument
} cloption_t;


// general for xclopts processing
#define XCLO_SUCCESS					  (0)	// functions returning success in execution
#define XCLO_ERROR						 (-1)	// functions indicating errors in execution
#define INDEX_NOT_FOUND					 (-1)	// for array searches

// for opts. specs. processing
#define XCLO_ERR_UNKNOWN_OPTTYPE		(-10)	// only allowed option types are SHORTOPT and LONGOPT

#define XCLO_ERR_UNKNOWN_OPTARGTYPE		(-20)	// only allowed optarg types are NOOPTARG, REQUIRED and OPTIONAL

#define XCLO_ERR_OPTSPEC_SNTX			(-30)	// syntax error in optspec (option specification)
#define XCLO_ERR_SSOPTSPEC_SNTX			(-31)	// syntax error in optspec of a short option
#define XCLO_ERR_LLOPTSPEC_SNTX			(-32)	// syntax error in optspec of a long option

// for user given cmd args: option supplied by user on command line
#define XCLO_ERR_UNKNOWN_OPT			(-50)	// currently only allowed options for GetOpt are "-s" and "-l"
#define XCLO_ERR_UNKNOWN_SSOPT			(-51)	// short opts SPECS contain unallowed char
#define XCLO_ERR_UNKNOWN_LLOPT			(-52)	// long opts SPECS contain unallowed char

// for user given cmd args: option requires a mandatory optarg, but user did not supply
#define XCLO_ERR_NO_OPTARG				(-60)	//
#define XCLO_ERR_NO_SSOPTARG			(-61)	//
#define XCLO_ERR_NO_LLOPTARG			(-62)	//

// for user given cmd args: option requires optional optarg, user intended to supply (by specifying '=') but did not
#define XCLO_ERR_NO_OPTIONALOPTARG		(-70)	//
#define XCLO_ERR_NO_SS_OPTIONALOPTARG	(-71)	//
#define XCLO_ERR_NO_LL_OPTIONALOPTARG	(-72)	//


extern cloption_t *_allowedShortOptions;
extern int _numShortOptSpecs;
// extern cloption_t *_allowedLongOptions;
// extern int _numLongOptSpecs;

// extern char **_otherArgs;					// used in xclargs.c only
// extern int _numGivenOpts;					// used in xclargs.c only
// extern int _numGivenOtherArgs;				// used in xclargs.c only
int init_xgetopts(int nargs, char *args[], const char *ss, const char *ll);
cloption_t *xgetopts(int nargs, char *args[], const char *ss, const char *ll);



#define DBG_CLO				1

#ifdef DBG_CLO
	#define LOG1(fmt, ...)		fprintf(stdout, "\033[1;32m INFO [%s:%d %s] " fmt "\033[0m \n", __FILE__, __LINE__, __func__, ##__VA_ARGS__);
	#define LOG2(fmt, ...)		fprintf(stderr, "\033[1;31mERROR [%s:%d %s] " fmt "\033[0m \n", __FILE__, __LINE__, __func__, ##__VA_ARGS__);
	#define LOGW(fmt, ...)		fprintf(stdout, "\033[1;35m WARN [%s:%d %s] " fmt "\033[0m \n", __FILE__, __LINE__, __func__, ##__VA_ARGS__);
#else
	#define LOG1(fmt, ...)
	#define LOG2(fmt, ...)
	#define LOGW(fmt, ...)
#endif


#endif	// __XCLOPTS_H					// xinu command line options

