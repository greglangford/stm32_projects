#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "include/stm32f1xx.h"
#include "include/uart.h"
#include "include/gps.h"

USART_TypeDef *USART;

char gps_getchar() {
    while(!(USART->SR & USART_SR_RXNE));
    return USART->DR;
}

uint8_t gps_validate_sentence_checksum(char *buf) {
    int i;
    int sentence_checksum;
    int calculated_checksum;
    char tmp[GPS_STRING_BUFFER_LEN];
    char *p;

    sentence_checksum = 0;
    calculated_checksum = 0;

    strcpy(tmp, buf);       // Copy buf to tmp
    p = strtok(tmp, "*");   // p = before first *
    p = strtok(NULL, "*");  // p = after first *

    // Check p is not NULL
    if(p) {
        sscanf(p, "%02x", &sentence_checksum);  // sentence_checksum to integer of sentence checksum chars

        // Calculate checksum manually
        for(i = 0; (i < strlen(buf)) && (buf[i] != '*'); i++) {
            // Skip first char as it is a $ symbol
            if(i > 0) {
                calculated_checksum ^= buf[i];
            }
        }

        return 1;
    }

    return 0;
}

uint8_t gps_get_sentence(USART_TypeDef *ptrUSART, char *nmea_code, char *buf) {
    char *ptrbuf;
    uint8_t gps_ready;
    uint8_t gps_start;

    USART = ptrUSART;                           // Set USART pointer
    memset(buf, 0, GPS_STRING_BUFFER_LEN);      // Clear string
    ptrbuf = buf;                               // Reset pointer
    gps_ready = 0;                              // Reset ready condition
    gps_start = 0;                              // Reset start condition

    while(!gps_ready) {
        char c;
        c = gps_getchar();

        if(c == '$') {
            gps_start = 1;
        }

        if(gps_start) {
            *ptrbuf++ = c;
        }

        if(c == '\n') {
            gps_start = 0;
            gps_ready = 1;
        }
    }

    if(gps_ready) {
        gps_ready = 0;

        char *p;
        p = strstr(buf, nmea_code);

        if((p) && gps_validate_sentence_checksum(buf)) {
            return 1;
        }
    }

    return 0;
}
