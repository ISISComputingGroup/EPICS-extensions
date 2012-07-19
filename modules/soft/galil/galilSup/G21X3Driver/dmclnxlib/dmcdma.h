#ifndef INCLUDE_DATARECORDACCESS
#define INCLUDE_DATARECORDACCESS

#define DMA_1_STATUS_REGISTER                  0x08
#define DMA_1_MASK_REGISTER                    0x0A
#define DMA_1_MODE_REGISTER                    0x0B
#define DMA_1_CLEAR_REGISTER                   0x0C

#define DMA_1_CHANNEL_0_CLEAR_MASK             0x00
#define DMA_1_CHANNEL_1_CLEAR_MASK             0x01
#define DMA_1_CHANNEL_2_CLEAR_MASK             0x02
#define DMA_1_CHANNEL_3_CLEAR_MASK             0x03
#define DMA_1_CHANNEL_0_SET_MASK               0x04
#define DMA_1_CHANNEL_1_SET_MASK               0x05
#define DMA_1_CHANNEL_2_SET_MASK               0x06
#define DMA_1_CHANNEL_3_SET_MASK               0x07

/* Set mode to write (i/o read) auto-initialize increment demmand mode */
#define DMA_1_CHANNEL_0_DEMAND_AUTO_INC_WRITE  0x014
#define DMA_1_CHANNEL_1_DEMAND_AUTO_INC_WRITE  0x015
#define DMA_1_CHANNEL_2_DEMAND_AUTO_INC_WRITE  0x016
#define DMA_1_CHANNEL_3_DEMAND_AUTO_INC_WRITE  0x017

#define AT_DMA_1_CHANNEL_0_BASE_ADDRESS        0x00
#define AT_DMA_1_CHANNEL_0_BASE_COUNT          0x01
#define AT_DMA_1_CHANNEL_1_BASE_ADDRESS        0x02
#define AT_DMA_1_CHANNEL_1_BASE_COUNT          0x03
#define AT_DMA_1_CHANNEL_2_BASE_ADDRESS        0x04
#define AT_DMA_1_CHANNEL_2_BASE_COUNT          0x05
#define AT_DMA_1_CHANNEL_3_BASE_ADDRESS        0x06
#define AT_DMA_1_CHANNEL_3_BASE_COUNT          0x07

#define AT_DMA_PAGE_REGISTER_CHANNEL_0         0x087
#define AT_DMA_PAGE_REGISTER_CHANNEL_1         0x083
#define AT_DMA_PAGE_REGISTER_CHANNEL_2         0x081
#define AT_DMA_PAGE_REGISTER_CHANNEL_3         0x082

#define MASK_64K                               0x0FFFF
#define MASK_LOW_WORD                          0x0F
#define MASK_HIGH_WORD                         0x0FF0
#define MASK_LOW_BYTE                          0x0F
#define MASK_HIGH_BYTE                         0x0FF0

/* Buffer has to be divisible by 16 for allocmem call */
#define DMA_BUFFER_SIZE                        1024
#define DMA_IN_PROGRESS                        0x080

#define MAX_INPUT_SIZE                         80

#ifdef __DPMI16__
DWORD   FAR PASCAL GlobalDosAlloc(DWORD);
UINT    FAR PASCAL GlobalDosFree(UINT);
#define LOBYTE(w)           ((BYTE)(w))
#define HIBYTE(w)           ((BYTE)((UINT)(w) >> 8))

#define LOWORD(l)           ((WORD)(l))
#define HIWORD(l)           ((WORD)((DWORD)(l) >> 16))

#define MAKELONG(low, high) ((LONG)(((WORD)(low)) | (((DWORD)((WORD)(high))) << 16)))
#endif /* __DPMI16__ */

#ifdef DMC_DMA
#ifdef __BORLANDC__
#include <alloc.h>
#define Allocmem     _dos_allocmem
#define Freemem      _dos_freemem
#define Dma_MK_FP    MK_FP
#define Dma_FP_SEG   FP_SEG
#else
#include <malloc.h>
#define Allocmem     _dos_allocmem
#define Freemem      _dos_freemem
#define Dma_MK_FP    _MK_FP
#define Dma_FP_SEG   _FP_SEG
#endif
#endif /* DMC_DMA */

typedef struct _DMADATA
{
   WORD      buffer_size;      /* Buffer size of memory allocated */
   WORD      transfer_size;    /* Actual DMA transfer size */
   CHAR FAR* dma_record;       /* Pointer to dma record */
   WORD      seg_size;         /* Segment size in paragraphs */
   WORD      seg_address;      /* Allocated segment address */
   WORD      page_register;    /* Physical page register */
   WORD      low_byte_addr;    /* Low-byte address */
   WORD      high_byte_addr;   /* High-byte address */
   WORD      low_byte_count;   /* Low-byte count */
   WORD      high_byte_count;  /* High-byte count */
   WORD      enable;           /* Boolean enable DRQ flag */
   DWORD     global_memory;    /* For DPMI 16-bit applications */
} DMADATA, *PDMADATA;

extern long InitDMA(int iIndex);
extern long UninitDMA(int iIndex);

USHORT GetDataType(USHORT usGeneralOffset, USHORT usAxisInfoOffset, USHORT usRevision);
void Dma_enable(PDMADATA pdmadata, USHORT usDMAChannel);
void Dma_disable(PDMADATA pdmadata, USHORT usDMAChannel);
long Dma_reset_channel(PDMADATA pdmadata, USHORT usDMAChannel);
long Dma_initialize(PDMADATA pdmadata, USHORT usDMAChannel);
void Dma_uninitialize(PDMADATA pdmadata, USHORT usDMAChannel);

#endif /* INCLUDE_DATARECORDACCESS */
