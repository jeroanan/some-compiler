#include <ctype.h>
#include <string.h>

/*
 * Trim whitepsace from either side of the given string
 */
char* trim_string(char* s) {

  int i;

  for(;*s;s++) {
    if (!isspace((unsigned char)*s)) {
      break;
    }
  }

  for (i=strlen(s)-1;i>0;i--) {
    if (isspace((unsigned char)s[i])) {
      s[i] = '\0';
    } else {
      break;
    }
  }

  return s;
}
