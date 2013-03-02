/* Glue for FatFS accessing the DSO Quad internal disc.
 */
#include <stdint.h>

#include "diskio.h"
#include "gpio.h"

#define always_read(x) asm(""::"r"(x))
static inline void __enable_irq()               { asm volatile ("cpsie i"); }
static inline void __disable_irq()              { asm volatile ("cpsid i"); }

/* SPI access routines */
DECLARE_GPIO(cs, GPIOB, 7);

static void spi_send(uint8_t byte)
{
    while (!(SPI3->SR & SPI_SR_TXE));
    SPI3->DR = byte;
}

static uint8_t spi_recv()
{
    while (!(SPI3->SR & SPI_SR_TXE));
    while (SPI3->SR & SPI_SR_BSY); // Wait for previous byte transmission
    always_read(SPI3->DR); // Clear RXNE & overflow flags
    always_read(SPI3->SR);
    SPI3->DR = 0xFF;
    while (!(SPI3->SR & SPI_SR_RXNE));
    return SPI3->DR;
}

static void  __attribute__((optimize("O2")))
spi_recv_block(uint8_t *buffer, unsigned count)
{
    while (!(SPI3->SR & SPI_SR_TXE));
    while (SPI3->SR & SPI_SR_BSY); // Wait for previous byte transmission
    always_read(SPI3->DR); // Clear RXNE & overflow flags
    always_read(SPI3->SR);
    
    SPI3->DR = 0xFF;
    while (count--)
    {
        while (!(SPI3->SR & SPI_SR_TXE));
        
        __disable_irq(); // Interrupt in here could cause RX overflow
        SPI3->DR = 0xFF;
        while (!(SPI3->SR & SPI_SR_RXNE));
        *buffer++ = SPI3->DR ^ 0xFF; // DSO Quad stores the inverse of bytes
        __enable_irq();
    }
}

/* Flash (M25PE16) access routines (faster than the BIOS versions) */

#define WREN       0x06  // Write enable instruction 
#define READ       0x03  // Read from Memory instruction 
#define RDSR       0x05  // Read Status Register instruction  
#define PP         0x02  // Write to Memory instruction 
#define PE         0xDB  // Page Erase instruction 
#define PW         0x0A  // Page write instruction 
#define DP         0xB9  // Deep power-down instruction 
#define RDP        0xAB  // Release from deep power-down instruction 
#define WIP_Flag   0x01  // Write In Progress (WIP) flag 

static void wait_for_write_end()
{
    gpio_cs_set(0);
    spi_send(RDSR);
    while (spi_recv() & WIP_Flag);
    gpio_cs_set(1);
}

static void read_flash(uint8_t *buffer, unsigned addr, unsigned count)
{
    // USB accesses may disturb our reads. We detect this by checking
    // CS after transfer.
    do {
        gpio_cs_set(1);
        wait_for_write_end();
        
        gpio_cs_set(0);
        
        spi_send(READ);
        spi_send((addr >> 16) & 0xFF);
        spi_send((addr >> 8) & 0xFF);
        spi_send(addr & 0xFF);
    
        spi_recv_block(buffer, count);
    
        spi_recv();
    } while (gpio_cs_get());
    
    gpio_cs_set(1);
}

/* FatFS hook implementations */

DSTATUS disk_initialize(BYTE drv)
{
    // Supports only one drive, no initialization necessary.
    return (drv == 0) ? 0 : STA_NOINIT;
}

DSTATUS disk_status (BYTE drv)
{
    return (drv == 0) ? 0 : STA_NOINIT;
}

DRESULT disk_read(BYTE drv, BYTE *buff, DWORD sector, BYTE count)
{
    if (drv != 0 || count == 0) return RES_PARERR;
    
    read_flash(buff, sector * 512, count * 512);
    
    return RES_OK;
}

DRESULT disk_write(BYTE drv, const BYTE *buff, DWORD sector, BYTE count)
{
    if (drv != 0 || count == 0) return RES_PARERR;
    
    while (count--)
    {
        if (__ProgDiskPage((u8*)buff, sector * 512) != 0)
            return RES_ERROR;
        
        if (__ProgDiskPage((u8*)buff + 256, sector * 512 + 256) != 0)
            return RES_ERROR;
        
        sector++;
        buff += 512;
    }
    
    return RES_OK;
}

DRESULT disk_ioctl(BYTE drv, BYTE ctrl, void *buff)
{
    if (drv != 0) return RES_PARERR;
    
    if (ctrl == CTRL_SYNC)
    {
        return RES_OK;
    }
    else if (ctrl == GET_SECTOR_COUNT)
    {
        *(DWORD*)buff = 4096;
        return RES_OK;
    }
    else if (ctrl == GET_SECTOR_SIZE || ctrl == GET_BLOCK_SIZE)
    {
        *(DWORD*)buff = 512;
        return RES_OK;
    }
    else
    {
        return RES_PARERR;
    }
}

DWORD get_fattime (void)
{
        /* Pack date and time into a DWORD variable */
        return    ((DWORD)(2000 - 1980) << 25)
                        | ((DWORD)1 << 21)
                        | ((DWORD)1 << 16)
                        | ((DWORD)13 << 11)
                        | ((DWORD)37 << 5)
                        | ((DWORD)0 >> 1);
}
