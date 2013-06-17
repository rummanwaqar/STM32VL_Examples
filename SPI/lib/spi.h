#ifndef _SPI_H
#define _SPI_H

void spi_init(SPI_TypeDef* SPIx);
int spi_readWrite(SPI_TypeDef* SPIx, uint8_t *rbuf, const uint8_t *tbuf, int cnt);

#endif
