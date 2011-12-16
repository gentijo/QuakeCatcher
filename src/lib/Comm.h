#pragma once

#include <stdbool.h>

void commInit2(int baud, bool fRx, bool fTx);
void commPrint(const char *psz);
void commPrintLn(const char *psz);
void commTxCh(char ch);
bool commIsCharWaiting();
char commInKey();




