#ifndef _MMC_H
#define _MMC_H

uint8_t disk_initialize ( void );
uint8_t disk_write ( const uint8_t *buff, uint32_t sector );
uint8_t disk_read ( uint8_t *buff, uint32_t sector );

#endif
