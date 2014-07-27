#include <argp.h>								// for argparse

#include "argparse_setup.h"
#include "vcsfmt.h"             // for file processing and I/O
#include "vcscmp.h"							// for producing diff-compatible output

// example function used for g_slist_foreach
void print_list_string(char * input_str){
	printf("%s\n",input_str);
}

int main(int argc, char ** argv){

	arguments args;
	args.files = NULL;
	args.preformat_loc_dir = "";
	args.is_write = false;
	args.is_compare = false;
	args.is_verbose = false;

	argp_parse(&argp, argc, argv, 0, 0, &args);

	if (args.is_write){
		printf("HAHA\n");
	}
	if (args.is_compare){
		printf("YO THIS WORKS!\n");
	}
	if (args.is_verbose){
		printf("YOOO THIS TOO!\n");
	}
	if (strcmp(args.preformat_loc_dir,"") != 0){
		printf("%s\n",args.preformat_loc_dir);
	}
	if (args.files != NULL){
		g_slist_foreach(args.files,(GFunc)print_list_string,NULL);
	}
	else{
		argp_help(&argp,stderr,ARGP_HELP_USAGE,"standard");
	}
	
  if (vcsfmt("500_lines_of_dna_minus_lines.fasta") == -1){
    PRINT_ERROR("vcsfmt failed.\n");
  }
  else{
    printf("Succeeded!\n");
  }
  if (de_vcsfmt("500_lines_of_dna_minus_lines.fasta.vcsfmt") == -1){
    PRINT_ERROR("de_vcsfmt failed.\n");
  }
  else{
    printf("Succeeded!\n");
  }
}
