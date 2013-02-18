#ifndef INCLUDE_FIRMWARE
#define INCLUDE_FIRMWARE

#define CONTROL_B     0x02
#define CONTROL_M     0x0D
#define CONTROL_O     0x0F
#define CONTROL_R     0x12
#define CONTROL_S     0x13

#define DOWNLOAD_BUFFER_SIZE 64

LONG TurnOnMonitor(HANDLEDMC hdmc);
LONG CheckResult(HANDLEDMC hdmc);
VOID Delay(ULONG ulDelay);

#endif /* INCLUDE_FIRMWARE */
