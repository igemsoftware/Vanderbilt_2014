#ifndef ___ARGPARSE_SETUP_H___
#define ___ARGPARSE_SETUP_H___

/*
	hosts all required variables and functions to implement argparse
*/

#include "utilities.h"

// version
const char * argp_program_version = "darwin 0.0";
const char * argp_program_bug_address = "<danieldmcclanahan@gmail.com>";
// documentation

static struct argp_option options[] = {
	{"preformat_loc",	'p',	"DIR",	0,
	 "Location of pre-existing .vcsfmt files for given input DNA",	0},
	{"write",	'w',	0,	0,	"Write output to file(s) instead of stdout",	0},
	{"compare",	'c',	0,	0,
	 "If two files given, produce unix diff-compatible comparison .vcsdiff file",	1},
	// TODO: make this option produce more information (stderr)
	{"verbose",	'v',	0,	0,	"Produce verbose output",	2},
	{0,0,0,0,0,0}
};
	
// used to communicate with parse_opt
typedef struct{
	GSList * files;								// FILES arg to --files
	char * preformat_loc_dir;			// DIR arg to -preformat_loc
	bool is_write;								// --write option
	bool is_compare;							// --compare option
	bool is_verbose;							// --verbose option
	bool has_no_args;
} dardiff_arguments;
// CTOR
dardiff_arguments initialize_dardiff_arguments(dardiff_arguments args){
	args.files = NULL;
	args.preformat_loc_dir = "";
	args.is_write = false;
	args.is_compare = false;
	args.is_verbose = false;
	args.has_no_args = false;
	return args;
}

static error_t parse_opt (int key, char * arg, struct argp_state * state){
	dardiff_arguments * args_ptr = state->input;
	switch (key){
	case 'p':
		args_ptr->preformat_loc_dir = arg;
		break;
	case 'w':
		args_ptr->is_write = true;
		break;
	case 'c':
		args_ptr->is_compare = true;
		break;
	case 'v':
		args_ptr->is_verbose = true;
		break;
	case ARGP_KEY_ARG:						// file argument
		args_ptr->files = g_slist_append(args_ptr->files,arg);
		break;
	case ARGP_KEY_NO_ARGS:
		args_ptr->has_no_args = true;
	default:
		return ARGP_ERR_UNKNOWN;
	}
	return 0;
}

static char args_doc[] = "[FILES...]";

static char doc[] = "dardiff is a command-line tool to format DNA sequences \
for use in version control systems. It is intended to be used as a backend \
for version control tools which act upon biological data. It defaults to \
\'format\' mode, where it formats a given DNA sequence (through stdin, or \
the given files) to a special format, appending the \'.vcsfmt\' suffix. If \
given the \'-c\' option, however, it will take two files, convert them to \
.vcsfmt format if necessary, and produce a .vcsdiff file which can be used \
in version control software.";

static struct argp argp = { options, parse_opt, args_doc, doc, 0, 0, 0 };

#endif /*___ARGPARSE_SETUP_H___*/
