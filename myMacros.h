#pragma once


#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <string.h>


#define CHECK1
#ifdef CHECK1
#define CHECK_RETURN_0(pointer) {\
 if(!(pointer))  return 0 ;\
}
#endif

#define CHECK2
#ifdef CHECK2
#define CHECK_MSG_RETURN_0(pointer) {\
	if (!(pointer)) { \
		printf("Error open supermarket file\n");\
		return 0; \
		}\
}
#endif
#define CHECK3
#ifdef CHECK3
#define FREE_CLOSE_FILE_RETURN_0(pointer,file) {\
free(pointer);\
fclose(file);\
return 0;\
}
#endif

#define CHECK4
#ifdef CHECK4
#define CLOSE_RETURN_0(file) {\
fclose(file);\
return 0;\
}
#endif
