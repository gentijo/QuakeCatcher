
#include <stdio.h>
#include <stdlib.h>

#include "global.h"

typedef struct WifiModemDriver {
  void  (*init)(FILE *Port);
  char* (*getTime)();
  int  (*sendHttpPost)(char *url, int len, u08 *data);
  int  (*sendHttpGet)(char *url);
} WifiModemDriver;

