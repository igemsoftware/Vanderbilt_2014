#include <argp.h> // for argparse

#include "argparse_setup.h" // for argparse processing
#include "vcsfmt.h"         // for file processing and I/O
#include "vcscmp.h"         // for producing diff-compatible output

#ifdef DEBUG
// example function used for g_slist_foreach
void print_list_string(char * input_str) {
  printf("%s\n", input_str);
}
#endif

int main(int argc, char ** argv) {

  dwndiff_arguments args;
  args = initialize_dwndiff_arguments(args);
  argp_parse(&argp, argc, argv, 0, 0, &args);

  if (args.has_no_args) {
    argp_help(&argp, stderr, ARGP_HELP_USAGE, "standard");
  } else {
    if (args.is_verbose) {
      printf("verbose\n");
    }
    if (args.unzip_or_zip) {
      printf("unzip\n");
    } else {
      printf("zip\n");
    }
    if (args.output_file_location) {
      printf("%s\n", args.output_file_location);
    }
    if (args.input_file_location) {
      printf("%s\n", args.input_file_location);
    }
    if (args.files != NULL) {
      g_slist_foreach(args.files, (GFunc) print_list_string, NULL);
    }
  }

  vcsfmt("ape_1");
  vcsfmt("ape_2");

  // de_vcsfmt("500_lines_of_dna_minus_lines.fasta.vcsfmt");

  vcscmp("ape_1.vcsfmt", "ape_2.vcsfmt", "WEIRD_OUTPUT_FILE.vcscmp");
}
