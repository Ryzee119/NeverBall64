// SPDX-License-Identifier: GPL-2.0

#include <libdragon.h>
#include <string.h>
#include <malloc.h>
#include <fcntl.h>
#include <system.h>

/**
 * @file regsinternal.h
 * @brief Register definitions for various hardware in the N64
 * @ingroup lowlevel
 */
#ifndef __LIBDRAGON_REGSINTERNAL_H
#define __LIBDRAGON_REGSINTERNAL_H

/**
 * @defgroup lowlevel Low Level Hardware Interfaces
 * @ingroup libdragon
 * @brief Low level hardware interface descriptions and functionality
 *
 * The low level hardware interfaces handle several functions in the N64 that
 * would otherwise be handled by a kernel or RTOS.  This includes the @ref dma,
 * the @ref exceptions, the @ref interrupt and the @ref n64sys.  The DMA controller
 * handles DMA requests between the cartridge and the N64 RDRAM.  Other systems
 * in the N64 have their own DMA controllers that are handled in the relevant
 * subsystems.  The exception handler traps any exceptions raised by the N64,
 * including the reset exception.  The interrupt handler sets up the MIPS
 * interface (MI) which handles low level interrupt functionality for all other
 * systems in the N64.  The N64 system interface provides the ability for code to
 * manipulate cache and boot options.
 */

/**
 * @brief Register definition for the AI interface
 * @ingroup lowlevel
 */
typedef struct AI_regs_s {
    /** @brief Pointer to uncached memory buffer of samples to play */
    volatile void * address;
    /** @brief Size in bytes of the buffer to be played.  Should be
     *         number of stereo samples * 2 * sizeof( uint16_t ) 
     */
    uint32_t length;
    /** @brief DMA start register.  Write a 1 to this register to start
     *         playing back an audio sample. */
    uint32_t control;
    /** @brief AI status register.  Bit 31 is the full bit, bit 30 is the busy bit. */
    uint32_t status;
    /** @brief Rate at which the buffer should be played.
     *
     * Use the following formula to calculate the value: ((2 * clockrate / frequency) + 1) / 2 - 1
     */
    uint32_t dacrate;
    /** @brief The size of a single sample in bits. */
    uint32_t samplesize;
} AI_regs_t;

/**
 * @brief Register definition for the MI interface
 * @ingroup lowlevel
 */
typedef struct MI_regs_s {
    /** @brief Mode register */
    uint32_t mode;
    /** @brief Version register */
    uint32_t version;
    /** @brief Current interrupts on the system */
    uint32_t intr;
    /** @brief Interrupt mask */
    uint32_t mask;
} MI_regs_t;

/**
 * @brief Register definition for the VI interface
 * @ingroup lowlevel
 */
typedef struct VI_regs_s {
    /** @brief VI control register.  Sets up various rasterization modes */
    uint32_t control;
    /** @brief Pointer to uncached buffer in memory to rasterize */
    void * framebuffer;
    /** @brief Width of the buffer in pixels */
    uint32_t width;
    /** @brief Vertical interrupt control register.  Controls which horizontal
     *         line must be hit to generate a VI interrupt
     */
    uint32_t v_int;
    /** @brief Current vertical line counter. */
    uint32_t cur_line;
    /** @brief Timing generation register for PAL/NTSC signals */
    uint32_t timing;
    /** @brief Number of lines per frame */
    uint32_t v_sync;
    /** @brief Number of pixels in line and leap pattern */
    uint32_t h_sync;
    /** @brief Number of pixels in line, set identically to h_sync */
    uint32_t h_sync2;
    /** @brief Beginning and end of video horizontally */
    uint32_t h_limits;
    /** @brief Beginning and end of video vertically */
    uint32_t v_limits;
    /** @brief Beginning and end of color burst in vertical lines */
    uint32_t color_burst;
    /** @brief Horizontal scaling factor from buffer to screen */
    uint32_t h_scale;
    /** @brief Vertical scaling factor from buffer to screen */
    uint32_t v_scale;
} VI_regs_t;

/**
 * @brief Register definition for the PI interface
 * @ingroup lowlevel
 */
typedef struct PI_regs_s {
    /** @brief Uncached address in RAM where data should be found */
    volatile void * ram_address;
    /** @brief Address of data on peripheral */
    uint32_t pi_address;
    /** @brief How much data to read from RAM into the peripheral */
    uint32_t read_length;
    /** @brief How much data to write to RAM from the peripheral */
    uint32_t write_length;
    /** @brief Status of the PI, including DMA busy */
    uint32_t status;
} PI_regs_t;

/** 
 * @brief Register definition for the SI interface
 * @ingroup lowlevel
 */
typedef struct SI_regs_s {
    /** @brief Uncached address in RAM where data should be found */
    volatile void * DRAM_addr;
    /** @brief Address to read when copying from PIF RAM */
    volatile void * PIF_addr_read;
    /** @brief Reserved word */
    uint32_t reserved1;
    /** @brief Reserved word */
    uint32_t reserved2;
    /** @brief Address to write when copying to PIF RAM */
    volatile void * PIF_addr_write;
    /** @brief Reserved word */
    uint32_t reserved3;
    /** @brief SI status, including DMA busy and IO busy */
    uint32_t status;
} SI_regs_t;

/**
 * @brief Register definition for the SP interface
 * @ingroup lowlevel
 */
typedef struct SP_regs_s {
    /** @brief RSP memory address (IMEM/DMEM) */
    volatile void * RSP_addr;
    /** @brief RDRAM memory address */
    volatile void * DRAM_addr;
    /** @brief RDRAM->RSP DMA length */
    uint32_t rsp_read_length;
    /** @brief RDP->RDRAM DMA length */
    uint32_t rsp_write_length;
    /** @brief RSP status */
    uint32_t status;
    /** @brief RSP DMA full */
    uint32_t rsp_dma_full;
    /** @brief RSP DMA busy */
    uint32_t rsp_dma_busy;
    /** @brief RSP Semaphore */
    uint32_t rsp_semaphore;
} SP_regs_t;

#endif

static const uint32_t SRAM_MAGIC = 0x64646464;
#define SRAMFS_MIN(a,b) (((a)<(b))?(a):(b))
#define SRAMFS_MAX(a,b) (((a)>(b))?(a):(b))

typedef struct sram_files_t
{
    const char *name;
    uint32_t size;
    uint32_t offset;
} sram_files_t;

sram_files_t *sram_files = NULL;
int sram_num_files = 0;

static int sram_get_handle_by_name(const char *name)
{
    for (int i = 1; i <= sram_num_files; i++)
    {
        if (strcasecmp(sram_files[i].name, name) == 0)
        {
            return i;
        }
    }
    return -1;
}

static int sram_get_file_start_by_handle(int handle)
{
    int offset = 0;
    for (int i = 1; i < handle; i++)
    {
        offset += sram_files[i].size;
    }
    return offset;
}

uint8_t __attribute__((aligned(16))) sector_cache[16];
static volatile struct PI_regs_s * const PI_regs = (struct PI_regs_s *)0xa4600000;
static void _dma_read(void * ram_address, unsigned long pi_address, unsigned long len) 
{
    disable_interrupts();
    dma_wait();
    MEMORY_BARRIER();
    PI_regs->ram_address = ram_address;
    MEMORY_BARRIER();
    PI_regs->pi_address = pi_address;
    MEMORY_BARRIER();
    PI_regs->write_length = len-1;
    MEMORY_BARRIER();
    dma_wait();
    enable_interrupts();
}

static void _dma_write(void * ram_address, unsigned long pi_address, unsigned long len) 
{
    disable_interrupts();
    dma_wait();
    MEMORY_BARRIER();
    PI_regs->ram_address = (void*)ram_address;
    MEMORY_BARRIER();
    PI_regs->pi_address = pi_address;
    MEMORY_BARRIER();
    PI_regs->read_length = len-1;
    MEMORY_BARRIER();
    dma_wait();
    enable_interrupts();
}

//Reading and writing to SRAM via DMA must occur from a 16 bytes aligned buffer. For Omnispeak, the save file
//can be greater than 1 SRAM bank (32kB), so I need to be able to manage access multiple banks contiguously from non aligned buffers at random lengths.
//I read and write by 16 byte 'sectors' and manage the banking automatically. If a read crosses to a different bank this is handled here.
static void read_sram(uint8_t *dst, uint32_t offset, int len)
{
    assert(offset + len < 0x18000);

    uint32_t aligned_offset;
    uint32_t banked_offset;
    while (len > 0)
    {
        //Make sure we're on a 32bit boundary in a 16 byte sector
        aligned_offset = offset - (offset % 4);
        aligned_offset = aligned_offset - (aligned_offset % 16);

        //Select the right bank
        if (aligned_offset > 0xFFFF)
        {
            banked_offset = aligned_offset - (0xFFFF + 1);
            banked_offset |= (2 << 18);
        }
        else if (aligned_offset > 0x7FFF)
        {
            banked_offset = aligned_offset - (0x7FFF + 1);
            banked_offset |= (1 << 18);
        }
        else
        {
            banked_offset = aligned_offset;
        }

        //Read a sector of data from SRAM
        _dma_read(sector_cache, 0x08000000 + (banked_offset & 0x07FFFFFF), 16);
        data_cache_hit_writeback_invalidate(sector_cache, 16);

        //Copy only the required bytes into the dst buffer
        int br = SRAMFS_MIN(len, 16 - (offset - aligned_offset));
        memcpy(dst, sector_cache + (offset - aligned_offset), br);

        dst += br;
        offset += br;
        len -= br;
    }
}

static void write_sram(uint8_t *src, uint32_t offset, int len)
{
    assert(offset + len < 0x18000);

    uint32_t aligned_offset;
    uint32_t banked_offset;
    while (len > 0)
    {
        //Make sure we're on a 32bit boundary in a 16 byte sector
        aligned_offset = offset - (offset % 4);
        aligned_offset = aligned_offset - (aligned_offset % 16);

        //Select the right bank
        if (aligned_offset > 0xFFFF)
        {
            banked_offset = aligned_offset - (0xFFFF + 1);
            banked_offset |= (2 << 18);
        }
        else if (aligned_offset > 0x7FFF)
        {
            banked_offset = aligned_offset - (0x7FFF + 1);
            banked_offset |= (1 << 18);
        }
        else
        {
            banked_offset = aligned_offset;
        }

        //Read a sector of data from SRAM if writing to an unaligned sector
        //Can skip this read/modify/write if we're aligned and atleast 16 byte.
        if (len < 16 || (offset - aligned_offset) != 0)
        {
            _dma_read(sector_cache, 0x08000000 + (banked_offset & 0x07FFFFFF), 16);
            data_cache_hit_writeback_invalidate(sector_cache, 16);
        }

        int bw = SRAMFS_MIN(len, 16 - (offset - aligned_offset));
        memcpy(sector_cache + (offset - aligned_offset), src, bw);
        data_cache_hit_writeback_invalidate(sector_cache, 16);
        _dma_write(sector_cache, 0x08000000 + (banked_offset & 0x07FFFFFF), 16);

        src += bw;
        offset += bw;
        len -= bw;
    }
}

static void *__open(char *name, int flags)
{
    name++;
    int handle = sram_get_handle_by_name(name);
    if (handle <= 0)
    {
        return NULL;
    }

    int offset = sram_get_file_start_by_handle(handle);
    int magic;
    read_sram((uint8_t *)&magic, offset, 4);

    //File is meant to be ready only, see if it exists by reading the first few bytes
    //to see if the magic number is present.
    if (flags == O_RDONLY && magic != SRAM_MAGIC)
    {
        return NULL;
    }

    //We should 'create' the file
    if (magic != SRAM_MAGIC)
    {
        //Write the magic number then return handle to the file, then zero the remainder
        magic = SRAM_MAGIC;
        write_sram((uint8_t *)&magic, offset, sizeof(SRAM_MAGIC));
        uint8_t zero[16] = {0};
        data_cache_hit_writeback_invalidate(zero, 16);
        int remaining = sram_files[handle].size - sizeof(SRAM_MAGIC);
        int pos = 0;
        while(remaining)
        {
            int chunk = SRAMFS_MIN(sizeof(zero), remaining);
            write_sram(zero, sizeof(SRAM_MAGIC) + offset + pos, chunk);
            remaining -= chunk;
            pos += chunk;
        }
    }
    sram_files[handle].offset = 0;
    return (void *)handle;
}

static int __fstat( void *file, struct stat *st )
{
    int handle = (uint32_t)file;
    st->st_dev = 0;
    st->st_ino = 0;
    st->st_mode = S_IFREG;
    st->st_nlink = 1;
    st->st_uid = 0;
    st->st_gid = 0;
    st->st_rdev = 0;
    st->st_size = sram_files[handle].size;
    st->st_atime = 0;
    st->st_mtime = 0;
    st->st_ctime = 0;
    st->st_blksize = 0;
    st->st_blocks = 0;
    return 0;
}

static int __lseek(void *file, int ptr, int dir)
{
    int handle = (uint32_t)file;
    int new_offset = sram_files[handle].offset;

    if (dir == SEEK_SET)
    {
        new_offset = ptr;
    }
    else if (dir == SEEK_CUR)
    {
        new_offset += ptr;
    }
    else if (dir == SEEK_END)
    {
        new_offset = sram_files[handle].size;
    }

    if (new_offset < 0)
    {
        new_offset = 0;
    }
    else if (new_offset > sram_files[handle].size)
    {
        new_offset = sram_files[handle].size;
    }

    sram_files[handle].offset = new_offset;

    return new_offset;
}

static int __read( void *file, uint8_t *ptr, int len )
{
    int handle = (uint32_t)file;
    int offset = sram_get_file_start_by_handle(handle) + sram_files[handle].offset + sizeof(SRAM_MAGIC);
    int max_len = SRAMFS_MIN(len, sram_files[handle].size - offset);
    read_sram(ptr, offset, max_len);
    sram_files[handle].offset += max_len;
    return max_len;
}

static int __write( void *file, uint8_t *ptr, int len )
{
    int handle = (uint32_t)file;
    int offset = sram_get_file_start_by_handle(handle) + sram_files[handle].offset + sizeof(SRAM_MAGIC);
    write_sram(ptr, offset, len);
    sram_files[handle].offset += len;
    return len;
}

static int __close( void *file )
{
    return 0;
}

static filesystem_t sram_fs = {
    __open,
    __fstat,
    __lseek,
    __read,
    __write,
    __close,
    0,
    0,
    0
};

int sramfs_init(sram_files_t *files, int num_files)
{
    assert(files != NULL);
    assert(num_files > 0);

    sram_files = malloc(sizeof(sram_files_t) * (num_files + 1));
    assert(sram_files != NULL);
    memcpy(&sram_files[1], files, sizeof(sram_files_t) * num_files);
    sram_num_files = num_files;
    int res = attach_filesystem("sram:/", &sram_fs);
    return res;
}