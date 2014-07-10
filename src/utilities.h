#ifndef ___UTILITIES_H___
#define ___UTILITIES_H___

#define PRINT_ERROR(str) fprintf(stderr,"%s",str)

#define PRINT_ERROR_AND_RETURN_NULL_IF_NULL(ptr,str)			\
	if (NULL == ptr){																				\
	fprintf(stderr,"%s",str);																\
	return NULL;																						\
	}

#define PRINT_ERROR_AND_RETURN_NEG_ONE_IF_NULL(ptr,str)	\
	if (NULL == ptr){																			\
	fprintf(stderr,"%s",str);															\
	return -1;																						\
	}

#define PRINT_ERROR_AND_RETURN_NEG_ONE_IF_NEG_ONE(val,str)	\
	if (-1 == val){																						\
	fprintf(stderr,"%s",str);																	\
	return -1;																								\
	}

#define PRINT_ERROR_AND_RETURN_NULL_IF_NEG_ONE(val,str)	\
	if (-1 == val){																				\
	fprintf(stderr,"%s",str);															\
	return NULL;																					\
	}

#endif /*___UTILITIES_H___*/
