#include <argp.h> // for argparse

#include "argparse_setup.h" // for argparse processing
#include "vcsfmt.h"         // for file processing and I/O
#include "vcscmp.h"         // for producing diff-compatible output

dwndiff_arguments args;

#ifdef DEBUG
// example function used for g_slist_foreach
void print_list_string(char * input_str) {
  printf("%s\n", input_str);
}
#endif

void format_file_arg(char * filename) {
  if (args.is_zip) {
    vcsfmt(filename, args.output_file_location);
  } else if (args.is_unzip) {
    de_vcsfmt(filename, args.output_file_location);
  } else {
    PRINT_ERROR_AND_RETURN("Error: Neither zip nor unzip specified for file.");
  }
}

int main(int argc, char ** argv) {

  args = initialize_dwndiff_arguments(args);
  argp_parse(&argp, argc, argv, 0, 0, &args);

  if (args.has_no_args) {
    argp_help(&argp, stderr, ARGP_HELP_USAGE, "standard");
  } else {
    if (0 != args.width) {
      FASTA_LINE_LENGTH = args.width;
    }
    if (args.output_file_location) {
      printf("%s\n", args.output_file_location);
      if (NULL != args.files) {
        g_slist_foreach(args.files, (GFunc) format_file_arg, NULL);
      } else {
        PRINT_ERROR("Error: No input files given.");
      }
    } else {
      PRINT_ERROR("Error: No output directory given.");
      return -1;
    }
  }
}
