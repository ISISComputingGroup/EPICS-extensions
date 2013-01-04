#ifndef INCLUDE_PCIBUS
#define INCLUDE_PCIBUS

//#include <sys/pci.h>
//#include <sys/osinfo.h>

extern long PCIBUSReadData(int iIndex, PCHAR pchResponse, ULONG cbResponse, PULONG pulBytesRead);
extern long PCIBUSWriteData(int iIndex, PCHAR pchCommand, ULONG cbCommand, PULONG pulBytesWritten);
extern long PCIBUSClear(int iIndex);
extern int PCIBUSCharAvailableInput(int iIndex);

#define MAX_PCI_DEVICES         32
#define BIOS_INT                0x1A   /* BIOS interrupt number */
#define FAILED                  0x01
#define PLX_NOT_GALIL           0x03   /* PLX chip found, but it is not A galil controller */
#define VENDOR_ID               0x10B5 /* PLX vendor ID */
#define DEVICE_ID               0x9050 /* PLX device ID */
#define SUB_VENDOR              0x1079 /* Galil subsytem vendor id */
#define SUB_DEVICE_1600         0x1640 /* Galil subsystem device id for DMC-1600 */
#define SUB_DEVICE_1800         0x1800 /* Galil subsystem device id for DMC-1800 */
#define SUB_DEVICE_1802         0x1842 /* Galil subsystem device id for DMC-1802 */
#define CLEAR_LOW_BIT           0xFFFE
#define CLEAR_LOW_NIBBLE        0xFFF0

/* PCI return code list per specification 2.1 */
#define SUCCESSFUL              0x00
#define FUNC_NOT_SUPPORTED      0x81
#define BAD_VENDOR_ID           0x83
#define DEVICE_NOT_FOUND        0x86
#define BAD_REGISTER_NUMBER     0x87
#define SET_FAILED              0x88
#define BUFFER_TOO_SMALL        0x89

//long FindPCIController(int Index, PCONTROLLERINFO pcontrollerinfo);
//ULONG GetSerialNumber(USHORT fIOStyle, USHORT usAddress);

#endif /* INCLUDE_PCIBUS */
