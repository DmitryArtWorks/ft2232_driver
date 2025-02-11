#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <dos.h>
#include <ftd2xx.h>
#include <time.h>
// #include <minwindef.h>


#define PacketSize 256
#define NumSamples 1000000000
#define PacketCoef 1 // (пока 1, но должно быть 2) потому что один отсчет будет преобразован в два 8-битных отсчета
#define rxTotal NumSamples*PacketCoef

FT_HANDLE Handle1;
FT_STATUS myftStatus;
UCHAR MASK = 0xFF; // Я В ДУШЕ НЕ ЕБУ, ДОЛЖНЫ БЫТЬ ЗДЕСЬ ВСЕ НУЛИ ИЛИ ВСЕ ЕДИНИЦЫ (0xFF)
UCHAR Mode;
UCHAR LatTimer = 16;
    DWORD sig1 = 0x00000000;
    DWORD sig2 = 0xFFFFFFFF;

// char *rxBuffer;

DWORD EventWord;
DWORD rxBytes;
DWORD txBytes;
LPDWORD BytesReceived;
static FT_PROGRAM_DATA datastruct;
PUCHAR gotBitMode;

FILE *fp;
void printEEPdata(FT_HANDLE Handle);
void delay(int milliseconds);
volatile sig_atomic_t stop = 0;
size_t total_size = (size_t)1024 * (size_t)1024 * (size_t)1024 * 2;

// Обработчик сигнала SIGINT
void handle_sigint(int sig) {
    stop = 1; // Устанавливаем флаг для выхода из цикла
}

int main(){
    BytesReceived = (LPDWORD)malloc(sizeof(LPDWORD));
    gotBitMode = (PUCHAR)malloc(sizeof(PUCHAR));
    size_t NumWrite = 0;
    unsigned long int numBytes = 0;
    // Установка обработчика сигнала SIGINT
    signal(SIGINT, handle_sigint);
    LPVOID rxBuffer   = (LPVOID ) _aligned_malloc(65536 * sizeof(unsigned char)*PacketCoef + 1024, 64);
    memset(rxBuffer, (unsigned char)0, 65536 * sizeof(unsigned char)*PacketCoef + 1024);
    LPVOID dataBuffer = (LPVOID) _aligned_malloc(total_size * sizeof(unsigned char)*PacketCoef + 65536, 64); // буфер
    memset(dataBuffer, (unsigned char)0, total_size * sizeof(unsigned char)*PacketCoef + 65536);
    
    if (rxBuffer == NULL) exit(1);
    
    if ((fp = fopen("test.bin", "wb"))==NULL) {
    printf("Cannot open file.\n");
    goto endProg;
    }
    

    myftStatus = FT_Open(0, &Handle1);
    if (!FT_SUCCESS(myftStatus)){
        printf("err no %ld while opening device\n", myftStatus);
        goto endProg;
    }
    else printf("Device opened successful, code %ld\n", myftStatus);
    

    // // // // // // // //
    // CODE STARTS HERE  // 
    // // // // // // // //

    printEEPdata(Handle1);
    
    Mode = 0x00; // reset mode
    myftStatus = FT_SetBitMode(Handle1, MASK, FT_BITMODE_RESET);
    if (!FT_SUCCESS(myftStatus)){
        printf("error #%i while resetting\n", myftStatus);
        goto endProg;
    }    

    delay(250);
    Mode = 0x40; // sync FIFO mode
    myftStatus = FT_SetBitMode(Handle1, MASK, FT_BITMODE_SYNC_FIFO);
    if (!FT_SUCCESS(myftStatus)){
        printf("error #%i while setting sync FIFO mode\n", myftStatus);
        goto endProg;
    }
    else{
        myftStatus = FT_GetBitMode(Handle1, gotBitMode);
        // if (!FT_SUCCESS(myftStatus)){
        //     printf("error #%i while trying to get bitmode\n", myftStatus);
        // goto endProg;
        // }
        printf("received bitmode: %p\n", gotBitMode);
        printf("setting up some options after status %i\n", myftStatus);
        myftStatus = FT_SetLatencyTimer(Handle1, LatTimer);
        if (!FT_SUCCESS(myftStatus)){
        printf("error #%i while setting latency timer\n", myftStatus);
        goto endProg;
        }

        myftStatus = FT_SetTimeouts(Handle1, 1000, 1000);
        if (!FT_SUCCESS(myftStatus)){
        printf("error #%i while setting timeouts\n", myftStatus);
        goto endProg;
        }

        myftStatus = FT_SetUSBParameters(Handle1, 0x10000, 0x10000); //TODO: ОПРЕДЕЛИТЬСЯ С РАЗМЕРАМИ БУФЕРОВ, МБ ОНИ СЛИШКОМ БОЛЬШИЕ
        if (!FT_SUCCESS(myftStatus)){
        printf("error #%i while setting USB parameters\n", myftStatus);
        goto endProg;
        }
        // myftStatus = FT_SetFlowControl(Handle1, FT_FLOW_RTS_CTS, 0, 0);
        // if (!FT_SUCCESS(myftStatus)){
        // printf("error #%i while setting flow control\n", myftStatus);
        // goto endProg;
        // }
    }
    // printEEPdata(Handle1);
    
    // printf("");
    printf("Trying to receive bytes\n");
    
    while(numBytes < total_size){
            if (stop) {
                printf("Reception interrupted by user.\n");
                break; // Выход из цикла при установке флага
            }
            // myftStatus = FT_GetStatus(Handle1, &rxBytes, &txBytes, &EventWord);
            myftStatus = FT_GetQueueStatus(Handle1, &rxBytes);
            // printf("available %lu bytes\n", rxBytes);
            if ( (FT_SUCCESS(myftStatus)) && (rxBytes >= PacketSize) ){
                
                myftStatus = FT_Read(Handle1, rxBuffer, rxBytes, BytesReceived);
                    // NumWrite = fwrite(rxBuffer, sizeof(char), *BytesReceived, fp);
                    // printf("copied %lu bytes\n", *BytesReceived);
                    memcpy(dataBuffer + numBytes, rxBuffer, *BytesReceived);
                    numBytes += rxBytes;
            }
        
    }
    printf("left receiving loop. Saving data...\n");
    NumWrite = fwrite(dataBuffer, sizeof(char), numBytes, fp);
    printf("Successfully written %lu bytes\n", NumWrite);
    endProg:
    // CLOSING DEVICE
    myftStatus = FT_Close(Handle1);
    if (!FT_SUCCESS(myftStatus))
        printf("err no %ld while closing device\n", myftStatus);
    else printf("Device closed successful, code %ld\n", myftStatus);


    fclose(fp);
    _aligned_free(rxBuffer);
    _aligned_free(dataBuffer);
    
    return 0;
}

void delay(int milliseconds)
{
    long pause;
    clock_t now,then;

    pause = milliseconds*(CLOCKS_PER_SEC/1000);
    now = then = clock();
    while( (now-then) < pause )
        now = clock();
}


void printEEPdata(FT_HANDLE Handle)
{
    datastruct.Signature1 = sig1;
    datastruct.Signature2 = sig2;
    datastruct.Manufacturer = (char *)malloc(256); /* E.g "deponce" */
    datastruct.ManufacturerId = (char *)malloc(256); /* E.g. "FT" */
    datastruct.Description = (char *)malloc(256); /* E.g. "USB HS Serial Converter" */
    datastruct.SerialNumber = (char *)malloc(256); /* E.g. "FT000001" if fixed, or NULL */
    if (datastruct.Manufacturer == NULL ||
        datastruct.ManufacturerId == NULL ||
        datastruct.Description == NULL ||
        datastruct.SerialNumber == NULL)
    {
        printf("Failed to allocate memory.\n");
        exit(1);
    }
    
    myftStatus = FT_EE_Read(Handle, &datastruct);
    if (!FT_SUCCESS(myftStatus)){
        printf("error #%i while reading EEPROM\n", myftStatus);
        exit(1);
    }

    printf("Program Data Version: %x \n", datastruct.Version);
    printf("FT_EE_Read succeeded.\n\n");
    printf("Signature1(must be 00000000) = %d\n", (int)datastruct.Signature1);          
    printf("Signature2(must be ffffffff)= %d\n", (int)datastruct.Signature2);
    printf("Version = %d\n", (int)datastruct.Version);
    printf("VendorId = 0x%04X\n", datastruct.VendorId);               
    printf("ProductId = 0x%04X\n", datastruct.ProductId);
    printf("Manufacturer = %s\n", datastruct.Manufacturer);           
    printf("ManufacturerId = %s\n\n", datastruct.ManufacturerId);
    printf("this 2 values is nonzero if mode is 245 FIFO and 245 FIFO CPU target respectievly: %x  , %x \n", (int)datastruct.IFBIsFifo7, (int)datastruct.IFBIsFifoTar7);
    printf("this 2 values is nonzero if A and B ports is to use VCP drivers: %x  , %x \n", datastruct.AIsVCP7, datastruct.BIsVCP7);

    free(datastruct.Manufacturer);
    free(datastruct.ManufacturerId);
    free(datastruct.Description);
    free(datastruct.SerialNumber);
}