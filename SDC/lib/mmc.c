#include <stm32f10x.h>
#include <stm32f10x_spi.h>
#include <stm32f10x_gpio.h>
#include <stm32f10x_rcc.h>
#include "spi.h"

#define GPIO_Pin_CS GPIO_Pin_3
#define SD_SPI SPI1

// MMC/SD Command (SPI mode)
#define CMD0  (0)    // GO_IDLE_STATE
#define CMD1  (1)    // SEND_OP_COND
#define ACMD41 (0x80+41) // SEND OP_COND (SDC)
#define CMD8  (8)    // SEND_IF_COND
#define CMD9  (9)    // SEND_CSD
#define CMD10  (10)  // SEND_CID
#define CMD12  (12)  // STOP_TRANSMISSION
#define CMD13  (13)  // SEND_STATUS
#define ACMD13 (0x80+13)  // SD_STATUS (SDC)
#define CMD16  (16)  // SET_BLOCKLEN
#define CMD17  (17)  // READ_SINGLE_BLOCK
#define CMD18  (18)  // READ_MULTIPLE_BLOCK
#define CMD23  (23)  // SET_BLOCK_COUNT
#define ACMD23 (0x80+23)  // SET_WR_BLK_ERASE_COUNT (SDC)
#define CMD24  (24)  // WRITE_SINGLE_BLOCK
#define CMD25  (25)  // WRITE_MULTIPLE_BLOCK
#define CMD41  (41)  // SEND_OP_COND (ACMD)
#define CMD55  (55)  // APP_CMD
#define CMD58  (58)  // READ_OCR

// Card type flags
#define CT_MMC   0x01
#define CT_SD1   0x02
#define CT_SD2   0x04
#define ST_SDC   (CT_SD1 | CT_SD2)
#define CT_BLOCK 0x08

static uint8_t CardType;

// Transmit bytes to card
static void xmit_mmc (const uint8_t* buff, int bc) {
  spi_readWrite(SD_SPI, 0, buff, bc);
}

// Receive bytes from the card
static void rcvr_mmc (uint8_t *buff, int bc) {
  spi_readWrite(SD_SPI, buff, 0, bc);
}

// wait for card ready
static int wait_ready (void) { // 1:OK, 0:Timeout
  uint8_t d;
  int tmr;

  for(tmr = 500; tmr; tmr--) {
    rcvr_mmc(&d, 1);
    if(d == 0xFF) break;
    Delay(1);
  }
  return tmr ? 1:0;
}

// deselect card
static void deselect (void) {
  uint8_t d;
  GPIO_SetBits(GPIOC, GPIO_Pin_CS);
  rcvr_mmc(&d, 1);
}

// select the card
static int select (void) { // 1:OK, 0:Timeout
  uint8_t d;

  GPIO_ResetBits(GPIOC, GPIO_Pin_CS);
  rcvr_mmc(&d, 1);

  if (wait_ready()) return 1; // OK
  deselect();
  return 0;                   // Failed
}

// init ports
void init_port(void) {
  GPIO_InitTypeDef GPIO_InitStructure;

  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_CS;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  deselect();
}

// receive a data packet from the card
static int rcvr_datablock(uint8_t *buff, int btr) {
  uint8_t d[2];
  int tmr;

  for (tmr = 100; tmr; tmr--) { // wait for data packet in timeout of 100ms
    rcvr_mmc(d , 1);
    if (d[0] != 0xFF) break;
    Delay(1);
  }
  if (d[0] != 0xFE) return 0; // if not valid data token, return 0
  rcvr_mmc(buff, btr);
  rcvr_mmc(d, 2); // discard crc
  return 1; // if success return success
}

// send a data packet to the card
static int xmit_datablock(const uint8_t *buff, uint8_t token) {
  // 512 byte data block to be transmitted ; Data/Stop token
  uint8_t d[2];

  if(!wait_ready()) return 0;

  d[0] = token;
  xmit_mmc(d, 1); // xmit token
  if (token != 0xFD) {  // is it a data token?
    xmit_mmc(buff, 512);
    rcvr_mmc(d, 2); // dummy crc
    rcvr_mmc(d, 1); // receive data response
    if ((d[0] & 0x1F) != 0x05) return 0; // if not accepted, return 0
  }
  return 1;
}

// send a command packet to the card
static int send_cmd( uint8_t cmd, uint32_t arg ) {
  // returns command response (bit7==1:Send failed)
  uint8_t n, d, buf[6];

  if (cmd & 0x80) {
    cmd &= 0x7F;
    n = send_cmd(CMD55, 0);
    if (n > 1) return n;
  }

  // select the card
  deselect();
  if (!select()) return 0xFF;

  // send a command packet
  buf[0] = 0x40 | cmd;
  buf[1] = (uint8_t) (arg >> 24); 
  buf[2] = (uint8_t) (arg >> 16);
  buf[3] = (uint8_t) (arg >> 8);
  buf[4] = (uint8_t) arg;
  n = 0x01;

  if (cmd == CMD0) n = 0x95;
  if (cmd == CMD8) n = 0x87;
  buf[5] = n;
  xmit_mmc(buf, 6);

  // receive command response
  if (cmd == CMD12) rcvr_mmc(&d, 1); // skip a stuff byte when stop reading 
  n = 10;
  do
    rcvr_mmc(&d, 1);
  while ((d & 0x80) && --n);

  return d;
}

// initialize disk drive
uint8_t disk_initialize ( void ) {
  uint8_t ty, cmd, buf[4], status;
  int tmr, n;

  spi_init(SD_SPI);

  spi_setPrescaler(SD_SPI, SPI_BaudRatePrescaler_64);

  init_port();
  for (n = 10; n; n--) rcvr_mmc(buf, 1); // 80 dummy clocks

  ty = 0;
  if (send_cmd(CMD0, 0) == 1 ) { // enter idle state
    if (send_cmd(CMD8, 0x1AA) == 1) {  // SDv2
      rcvr_mmc(buf, 4);
      if (buf[2] == 0x01 && buf[3] == 0xAA) { // card can work at vdd range of 2.7-3.6V
        for(tmr=100; tmr; tmr--) {
          // wait for leaving idle state
          if (send_cmd(ACMD41, 1UL << 30) == 0) break;
          Delay(10);
        }
        if (tmr && send_cmd(CMD58, 0) == 0) { // check ccs bit in the OCR
          rcvr_mmc(buf, 4);
          ty = (buf[0] & 0x40) ? CT_SD2 | CT_BLOCK : CT_SD2;
        }
      }
    } else { // SDv1 or MMCv3
      if (send_cmd(ACMD41, 0) <= 1) {
        ty = CT_SD1; cmd = ACMD41;
      } else {
        ty = CT_MMC; cmd = CMD1;
      }
      for (tmr=100; tmr; tmr--) {
        if (send_cmd(ACMD41, 0) == 0) break;
        Delay(10);
      }
      if (!tmr || send_cmd(CMD16, 512) != 0) // set R/W block length to 512
        ty = 0;
    }
  }

  deselect();

  CardType = ty;
  
  spi_setPrescaler(SD_SPI, SPI_BaudRatePrescaler_8);

  return ty;
}

// write sector
uint8_t disk_write ( const uint8_t *buff, uint32_t sector) {
  // Single block write
  uint8_t status = 0;
  if ( ( send_cmd(CMD24, sector) == 0) && xmit_datablock(buff, 0xFE) )
    status = 1;
  deselect();
  return status;
}

// read sector
uint8_t disk_read ( uint8_t *buff, uint32_t sector ) {
  uint8_t status = 0;
  if ( ( send_cmd(CMD17, sector) == 0) && rcvr_datablock(buff, 512) )
    status = 1;
  deselect();
  return status;
}
