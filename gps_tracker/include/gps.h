#define GPS_STRING_BUFFER_LEN 100

uint8_t gps_validate_sentence_checksum(char *buf);
uint8_t gps_get_sentence(USART_TypeDef *usart, char *nmea_code, char *buf);
