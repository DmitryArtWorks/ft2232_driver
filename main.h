#include <ftd2xx.h>
#include <stdio.h>
#include <time.h>
#include <signal.h>

FT_HANDLE Handle1;
FT_STATUS myftStatus;
UCHAR MASK = 0xFF; // Я В ДУШЕ НЕ ЕБУ, ДОЛЖНЫ БЫТЬ ЗДЕСЬ ВСЕ НУЛИ ИЛИ ВСЕ ЕДИНИЦЫ (0xFF)
UCHAR Mode;
UCHAR LatTimer = 16;
DWORD sig1 = 0x00000000;
DWORD sig2 = 0xFFFFFFFF;

int retCode;

DWORD EventWord;
DWORD rxBytes;
DWORD txBytes;
LPDWORD BytesReceived;
static FT_PROGRAM_DATA datastruct;
PUCHAR gotBitMode;

FILE *fp;
void printEEPdata(FT_HANDLE Handle);
int setupFTdevice();
void delay(int milliseconds);
volatile sig_atomic_t stop = 0;
