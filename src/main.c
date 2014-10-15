#include <argp.h> // for argparse

#include "argparse_setup.h" // for argparse processing
#include "vcsfmt.h"         // for file processing and I/O
#include "vcscmp.h"         // for producing diff-compatible output

// example function used for g_slist_foreach
static inline void print_list_string(char * input_str) {
    printf("%s\n", input_str);
}

int main(int argc, char ** argv) {

    dwndiff_arguments args;
    args = initialize_dwndiff_arguments(args);
    argp_parse(&argp, argc, argv, 0, 0, &args);

    if (args.has_no_args) {
        argp_help(&argp, stderr, ARGP_HELP_USAGE, "standard");
    } else {
        if (args.is_write) {
            printf("HAHA\n");
        }
        if (args.is_compare) {
            printf("YO THIS WORKS!\n");
        }
        if (args.is_verbose) {
            printf("YOOO THIS TOO!\n");
        }
        if (strcmp(args.preformat_loc_dir, "") != 0) {
            printf("%s\n", args.preformat_loc_dir);
        }
        if (args.files != NULL) {
            g_slist_foreach(args.files, (GFunc) print_list_string, NULL);
        }
    }

    // vcsfmt("500_lines_of_dna_minus_lines.fasta");

    // de_vcsfmt("500_lines_of_dna_minus_lines.fasta.vcsfmt");

    vcscmp("500_lines_of_dna_minus_lines.fasta.vcsfmt1",
           "500_lines_of_dna_minus_lines.fasta.vcsfmt2",
           "WEIRD_OUTPUT_FILE.vcscmp");

    // mpz_t begin_line;
    // mpz_init_set_ui(begin_line, 1);
    // mpz_t mid_line;
    // mpz_init_set_ui(mid_line, 50);
    // mpz_t end_line;
    // mpz_init_set_ui(end_line, 232);
    // mpz_t end2_line;
    // mpz_init_set_ui(end2_line, 233);
    // FILE * infile = open_file_read("500_lines_of_dna_minus_lines.fasta.vcsfmt");
    // write_line_number_from_file_to_file(&begin_line, &mid_line, infile, stdout);
    // write_line_number_from_file_to_file(&mid_line, &end_line, infile, stdout);
    // write_line_number_from_file_to_file(&end_line, &end2_line, infile, stdout);
    // mpz_clear(begin_line);
    // mpz_clear(mid_line);
    // mpz_clear(end_line);
    // mpz_clear(end2_line);
}
