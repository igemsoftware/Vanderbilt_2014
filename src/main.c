#include <stdio.h>							// for printf

#include "utilities.h"
#include "vcsfmt.h"							// for cat_file

int main(){
	int result_of_vcsfmt = vcsfmt("vcsfmt.h",8192);
	if (result_of_vcsfmt == -1){
	  PRINT_ERROR("vcsfmt failed.\n");
	}
	else{
		printf("Succeeded!\n");
	}
}
