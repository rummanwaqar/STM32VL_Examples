#ifndef _SPI_H
#define _SPI_H

void spi_init(void);
int spi_readWrite(uint8_t *rbuf, const uint8_t *tbuf, int cnt);

#endif
