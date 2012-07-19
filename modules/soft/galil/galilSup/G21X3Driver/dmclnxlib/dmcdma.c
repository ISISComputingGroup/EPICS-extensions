#include "dmclnx.h"
#include "../PCIDriver/pciioctl.h"

#ifdef DMC_DATARECORDACCESSQR

LONG FAR GALILCALL DMCGetDataRecordQR(HANDLEDMC hdmc,
   PDMCDATARECORDQR pdmcdatarecordqr, USHORT usRecordLength)
{
   long     rc = 0L;
   int      iIndex;
   char     szBuffer[sizeof(DMCDATARECORDQR) + 1];
   USHORT   usOffset = 0;
   ULONG    ulBytes = 0L;
   ULONG    ulCurrentTime;
   ULONG    ulTimeout;

#ifdef DMC_DEBUG
   DMCTrace("Entering DMCGetDataRecordQR.\n");
#endif

   iIndex = Handle2Index(hdmc);
   if (iIndex == -1)
   {
#ifdef DMC_DEBUG
      DMCTrace("   Invalid handle.\n");
      DMCTrace("Leaving DMCGetDataRecordQR.\n");
#endif
      return DMCERROR_HANDLE;
   }

   if (!pdmcdatarecordqr)
   {
#ifdef DMC_DEBUG
      DMCTrace("   DMCDATARECORDQR structure is NULL.\n");
      DMCTrace("Leaving DMCGetDataRecordQR.\n");
#endif
      return DMCERROR_DATARECORD;
   }

   if (usRecordLength != sizeof(DMCDATARECORDQR))
   {
#ifdef DMC_DEBUG
      DMCTrace("   DMCDATARECORDQR structure is the incorrect size.\n");
      DMCTrace("Leaving DMCGetDataRecordQR.\n");
#endif
      return DMCERROR_DATARECORD;
   }

   rc = DMCWriteData(hdmc, "QR\r", 3, &ulBytes);
   if (rc)
   {
#ifdef DMC_DEBUG
      DMCTrace("   DMCWriteData failed.\n");
      DMCTrace("Leaving DMCGetDataRecordQR.\n");
#endif
      return rc;
   }

   controller[iIndex].fBinaryCommand = 1;

   ulBytes = 0L;

   if (controller[iIndex].controllerinfo.ulTimeout)
   {
      ulCurrentTime = DMCGetTime();
      ulTimeout = ulCurrentTime + controller[iIndex].controllerinfo.ulTimeout;
   }

   do
   {
      rc = DMCReadData(hdmc, szBuffer + usOffset, sizeof(szBuffer) - usOffset, &ulBytes);
      if (rc)
      {
#ifdef DMC_DEBUG
         DMCTrace("   DMCReadData failed.\n");
         DMCTrace("Leaving DMCGetDataRecordQR.\n");
#endif
         controller[iIndex].fBinaryCommand = 0;
         return rc;
      }

      usOffset += (USHORT)ulBytes;

      /* The first 2 bytes are the record length */
      if (usOffset >= 2)
      {
         USHORT*  pusRecordLength = (USHORT*)(szBuffer + 2);
         if ((USHORT)((*pusRecordLength) + 1) == usOffset) /* Must account for ':' */
            break;
      }
      /* If time-out is 0, ignore time-out errors */
      if (controller[iIndex].controllerinfo.ulTimeout)
      {
         ulCurrentTime = DMCGetTime();
         if (ulCurrentTime > ulTimeout)
            rc = DMCERROR_TIMEOUT;
      }
   } while (!rc);

   controller[iIndex].fBinaryCommand = 0;

   memcpy(pdmcdatarecordqr, szBuffer, usRecordLength);

#ifdef DMC_DEBUG
   DMCTrace("Leaving DMCGetDataRecordQR.\n");
#endif
   return rc;
}
#endif /* DMC_DATARECORDACCESSQR */



#ifdef DMC_DATARECORDACCESS

LONG FAR GALILCALL DMCRefreshDataRecord(HANDLEDMC hdmc, ULONG ulLength)
{
   long  rc = 0L;
   int   iIndex;
   DMCDATARECORDQR dmcdatarecordqr;

#ifdef DMC_DEBUG
   DMCTrace("Entering DMCRefreshDataRecord.\n");
#endif

   iIndex = Handle2Index(hdmc);
   if (iIndex == -1)
   {
#ifdef DMC_DEBUG
      DMCTrace("   Invalid handle.\n");
      DMCTrace("Leaving DMCRefreshDataRecord.\n");
#endif
      return DMCERROR_HANDLE;
   }

   if (controller[iIndex].controllerinfo.hardwareinfo.businfo.fDataRecordAccess == DataRecordAccessNone)
   {
#ifdef DMC_DEBUG
      DMCTrace("   Data record access is not in use.\n");
      DMCTrace("Leaving DMCRefreshDataRecord.\n");
#endif
      return DMCERROR_DATARECORD;
   }

   if (!controller[iIndex].pbDataRecord)
   {
#ifdef DMC_DEBUG
      DMCTrace("   Data record is NULL.\n");
      DMCTrace("Leaving DMCRefreshDataRecord.\n");
#endif
      return DMCERROR_DATARECORD;
   }

   ulLength = ulLength;

   if (controller[iIndex].controllerinfo.hardwareinfo.businfo.fDataRecordAccess == DataRecordAccessDMA)
   {
#ifdef DMC_DMA
      int   i = 0;

      if (controller[iIndex].controllerinfo.hardwareinfo.businfo.usDMAChannel == 4)
      {
#ifdef DMC_DEBUG
         DMCTrace("   Invalid DMA channel.\n");
         DMCTrace("Leaving DMCRefreshDataRecord.\n");
#endif
         return DMCERROR_DATARECORD;
      }

      /* Compare the DMA buffers to insure that a complete record has been read */
      do
      {
         memcpy(controller[iIndex].pTemp1, controller[iIndex].dmadata.dma_record,
            controller[iIndex].usDataRecordSize);
         memcpy(controller[iIndex].pTemp2, controller[iIndex].dmadata.dma_record,
            controller[iIndex].usDataRecordSize);
         i++;
      } while (memicmp(controller[iIndex].pTemp1, controller[iIndex].pTemp2,
         controller[iIndex].usDataRecordSize) && i < 25);

      /* Copy the DMA buffer to working storage */
      memcpy(controller[iIndex].pbDataRecord, controller[iIndex].pTemp1,
         controller[iIndex].usDataRecordSize);
#endif /* DMC_DMA */
   }
	else if (controller[iIndex].controllerinfo.hardwareinfo.businfo.fDataRecordAccess == DataRecordAccessFIFO)
  {
		return ioctl( controller[ iIndex ].iDriver, GALIL_PCI_GETFIFODR, controller[ iIndex ].pbDataRecord );
	}		
	else if (controller[iIndex].controllerinfo.hardwareinfo.businfo.fDataRecordAccess == DataRecordAccessQR)
	{
		// STA: added 6/16/04
		rc = DMCGetDataRecordQR( hdmc, &dmcdatarecordqr, sizeof(dmcdatarecordqr) );
		if (!rc)
		{
			memcpy(controller[iIndex].pbDataRecord, &dmcdatarecordqr.DataRecord, controller[iIndex].usDataRecordSize);
		}

	}
#ifdef DMC_DEBUG
   DMCTrace("Leaving DMCRefreshDataRecord.\n");
#endif
   return rc;
}

LONG FAR GALILCALL DMCGetDataRecord(
HANDLEDMC hdmc, 
USHORT 		usGeneralOffset,
USHORT 		usAxisInfoOffset, 
PUSHORT 	pusDataType, 
PLONG 		plData)
{
   long  rc = 0;
   int   iIndex;
   char* pTempData;

   unsigned char  TempUCHAR;
   short          TempSHORT;
   unsigned short TempUSHORT;
   long           TempLONG;
   unsigned long  TempULONG;
   
#ifdef DMC_DEBUG
   DMCTrace("Entering DMCGetDataRecord.\n");
#endif

   iIndex = Handle2Index(hdmc);
   if (iIndex == -1)
   {
#ifdef DMC_DEBUG
      DMCTrace("   Invalid handle.\n");
      DMCTrace("Leaving DMCGetDataRecord.\n");
#endif
      return DMCERROR_HANDLE;
   }

   if (!controller[iIndex].pbDataRecord)
   {
#ifdef DMC_DEBUG
      DMCTrace("   Data record is NULL.\n");
      DMCTrace("Leaving DMCGetDataRecord.\n");
#endif   
      return DMCERROR_DATARECORD;
   }

   if (!plData)
   {
#ifdef DMC_DEBUG
      DMCTrace("   Return buffer is NULL.\n");
      DMCTrace("Leaving DMCGetDataRecord.\n");
#endif   
      return DMCERROR_ARGUMENT;
   }

   if (usGeneralOffset >= controller[iIndex].usMaxGeneralOffset ||
      usAxisInfoOffset >= controller[iIndex].usMaxAxisOffset)
   {
#ifdef DMC_DEBUG
      DMCTrace("   Invalid data record offset.\n");
      DMCTrace("Leaving DMCGetDataRecord.\n");
#endif   
      return(DMCERROR_DATARECORD);
   }

   pTempData = (char *)controller[iIndex].pbDataRecord;

   pTempData += usGeneralOffset;

   //the following switch and if are for when DMCGetDataRecord is used outside of DMCGetDataRecordByItemId
   switch (controller[iIndex].usRevision)
   {
      default:
         pTempData += usAxisInfoOffset;
         break;
      case REV01:
         if (usGeneralOffset >= DRREV1GenOffAxis1)
            pTempData += usAxisInfoOffset;
         break;
      case REV02:
         if (usGeneralOffset >= DRREV2GenOffAxis1)
            pTempData += usAxisInfoOffset;
         break;
      case REV03:
         if (usGeneralOffset >= DRREV3GenOffAxis1)
            pTempData += usAxisInfoOffset;
         break;
      case REV04:
         if (usGeneralOffset >= DRREV4GenOffAxis1)
            pTempData += usAxisInfoOffset;
         break;
      case REV07:
         if (usGeneralOffset >= DRREV7GenOffAxis1)
            pTempData += usAxisInfoOffset;
         break;
   }

   if (!*pusDataType)
      *pusDataType = GetDataType(usGeneralOffset, usAxisInfoOffset, controller[iIndex].usRevision);

   switch (*pusDataType)
   {
      default:
         DMCTrace("   Invalid or unknown data type.\n");
         DMCTrace("Leaving DMCGetDataRecord.\n");
         return(DMCERROR_DATARECORD);
      
      case DRTypeUCHAR:
         memcpy(&TempUCHAR, pTempData, sizeof(unsigned char));
         *plData = (long)TempUCHAR;
         break;
      
      case DRTypeSHORT:
         memcpy(&TempSHORT, pTempData, sizeof(short));      
         *plData = (long)TempSHORT;      
         break;
      
      case DRTypeUSHORT:
         memcpy(&TempUSHORT, pTempData, sizeof(unsigned short));      
         *plData = (long)TempUSHORT;      
         break;
      
      case DRTypeLONG:
         memcpy(&TempLONG, pTempData, sizeof(long));      
         *plData = (long)TempLONG;      
         break;

      case DRTypeULONG: //18x6 SegmentCountContour
         memcpy(&TempULONG, pTempData, sizeof(unsigned long));      
         *plData = (long)TempULONG;      
         break;
   }
   
#ifdef DMC_DEBUG
   DMCTrace("Leaving DMCGetDataRecord.\n");
#endif   
   return rc;
}

LONG FAR GALILCALL DMCGetDataRecordByItemId(HANDLEDMC hdmc, USHORT usItemId,
   USHORT usAxisId, PUSHORT pusDataType, PLONG plData)
{
   long     rc = 0;
   int      iIndex;
   int      fDoAxis = FALSE;
   USHORT   usGeneralOffset = 0;
   USHORT   usAxisInfoOffset = 0;
   USHORT   usDataType = DRTypeUCHAR;
   
#ifdef DMC_DEBUG
   DMCTrace("Entering DMCGetDataRecordByItemId.\n");
#endif   

   iIndex = Handle2Index(hdmc);
   if (iIndex == -1)
   {
#ifdef DMC_DEBUG
      DMCTrace("   Invalid handle.\n");
      DMCTrace("Leaving DMCGetDataRecordByItemId.\n");
#endif   
      return DMCERROR_HANDLE;
   }

   if (usItemId > DRIdUserVariable || usAxisId > DRIdAxis8)
   {
#ifdef DMC_DEBUG
      DMCTrace("   Invalid data record item Id.\n");
      DMCTrace("Leaving DMCGetDataRecordByItemId.\n");
#endif   
      return(DMCERROR_DATARECORD);
   }

   switch (controller[iIndex].usRevision)
   {
      case REV01:
      {
         switch (usItemId)
         {
            case DRIdSampleNumber:
               usDataType = DRTypeUSHORT;
               break;
            case DRIdGeneralInput0:
               usGeneralOffset = DRREV1GenOffGeneralInput1;
               break;
            case DRIdGeneralInput1:
               usGeneralOffset = DRREV1GenOffGeneralInput1;
               break;
            case DRIdGeneralInput2:
               usGeneralOffset = DRREV1GenOffGeneralInput2;
               break;
            case DRIdGeneralInput3:
               usGeneralOffset = DRREV1GenOffGeneralInput3;
               break;
            case DRIdGeneralInput4:
               rc = DMCERROR_DATARECORD;
               break;
            case DRIdGeneralInput5:
               rc = DMCERROR_DATARECORD;
               break;
            case DRIdGeneralInput6:
               rc = DMCERROR_DATARECORD;
               break;
            case DRIdGeneralInput7:
               rc = DMCERROR_DATARECORD;
               break;
            case DRIdGeneralInput8:
               rc = DMCERROR_DATARECORD;
               break;
            case DRIdGeneralInput9:
               rc = DMCERROR_DATARECORD;
               break;
            case DRIdGeneralOutput0:
               usGeneralOffset = DRREV1GenOffGeneralOutput1;
               break;
            case DRIdGeneralOutput1:
               usGeneralOffset = DRREV1GenOffGeneralOutput1;
               break;
            case DRIdGeneralOutput2:
               usGeneralOffset = DRREV1GenOffGeneralOutput2;
               break;
            case DRIdGeneralOutput3:
               rc = DMCERROR_DATARECORD;
               break;
            case DRIdGeneralOutput4:
               rc = DMCERROR_DATARECORD;
               break;
            case DRIdGeneralOutput5:
               rc = DMCERROR_DATARECORD;
               break;
            case DRIdGeneralOutput6:
               rc = DMCERROR_DATARECORD;
               break;
            case DRIdGeneralOutput7:
               rc = DMCERROR_DATARECORD;
               break;
            case DRIdGeneralOutput8:
               rc = DMCERROR_DATARECORD;
               break;
            case DRIdGeneralOutput9:
               rc = DMCERROR_DATARECORD;
               break;
            case DRIdErrorCode:
               usGeneralOffset = DRREV1GenOffErrorCode;
               break;
            case DRIdGeneralStatus:
               usGeneralOffset = DRREV1GenOffGeneralStatus;
               break;
            case DRIdSegmentCountS:
               usGeneralOffset = DRREV1GenOffSegmentCount;
               usDataType = DRTypeUSHORT;
               break;
            case DRIdCoordinatedMoveStatusS:
               usGeneralOffset = DRREV1GenOffCoordinatedMoveStatus;
               usDataType = DRTypeUSHORT;
               break;
            case DRIdCoordinatedMoveDistanceS:
               usGeneralOffset = DRREV1GenOffCoordinatedMoveDistance;
               usDataType = DRTypeLONG;
               break;
            case DRIdSegmentCountT:
               rc = DMCERROR_DATARECORD;
               break;
            case DRIdCoordinatedMoveStatusT:
               rc = DMCERROR_DATARECORD;
               break;
            case DRIdCoordinatedMoveDistanceT:
               rc = DMCERROR_DATARECORD;
               break;
            case DRIdAnalogInput1:
               rc = DMCERROR_DATARECORD;
               break;
            case DRIdAnalogInput2:
               rc = DMCERROR_DATARECORD;
               break;
            case DRIdAnalogInput3:
               rc = DMCERROR_DATARECORD;
               break;
            case DRIdAnalogInput4:
               rc = DMCERROR_DATARECORD;
               break;
            case DRIdAnalogInput5:
               rc = DMCERROR_DATARECORD;
               break;
            case DRIdAnalogInput6:
               rc = DMCERROR_DATARECORD;
               break;
            case DRIdAnalogInput7:
               rc = DMCERROR_DATARECORD;
               break;
            case DRIdAnalogInput8:
               rc = DMCERROR_DATARECORD;
               break;
            case DRIdAxisStatus:
               fDoAxis = TRUE;
               usAxisInfoOffset = DRREV1AxisOffAxisStatus;
               usDataType = DRTypeUSHORT;
               break;
            case DRIdAxisSwitches:
               fDoAxis = TRUE;
               usAxisInfoOffset = DRREV1AxisOffAxisSwitches;
               break;
            case DRIdAxisStopCode:
               fDoAxis = TRUE;
               usAxisInfoOffset = DRREV1AxisOffAxisStopCode;
               break;
            case DRIdAxisReferencePosition:
               fDoAxis = TRUE;
               usAxisInfoOffset = DRREV1AxisOffAxisReferencePosition;
               usDataType = DRTypeLONG;
               break;
            case DRIdAxisMotorPosition:
               fDoAxis = TRUE;
               usAxisInfoOffset = DRREV1AxisOffAxisMotorPosition;
               usDataType = DRTypeLONG;
               break;
            case DRIdAxisPositionError:
               fDoAxis = TRUE;
               usAxisInfoOffset = DRREV1AxisOffAxisPositionError;
               usDataType = DRTypeLONG;
               break;
            case DRIdAxisAuxillaryPosition:
               fDoAxis = TRUE;
               usAxisInfoOffset = DRREV1AxisOffAxisAuxillaryPosition;
               usDataType = DRTypeLONG;
               break;
            case DRIdAxisVelocity:
               fDoAxis = TRUE;
               usAxisInfoOffset = DRREV1AxisOffAxisVelocity;
               usDataType = DRTypeLONG;
               break;
            case DRIdAxisTorque:
               fDoAxis = TRUE;
               usAxisInfoOffset = DRREV1AxisOffAxisTorque;
               usDataType = DRTypeSHORT;
               break;
         }
         if (fDoAxis)
         {
            switch (usAxisId)
            {
               case DRIdAxis1:
                  usGeneralOffset = DRREV1GenOffAxis1;
                  break;
               case DRIdAxis2:
                  usGeneralOffset = DRREV1GenOffAxis2;
                  break;
               case DRIdAxis3:
                  usGeneralOffset = DRREV1GenOffAxis3;
                  break;
               case DRIdAxis4:
                  usGeneralOffset = DRREV1GenOffAxis4;
                  break;
               case DRIdAxis5:
                  usGeneralOffset = DRREV1GenOffAxis5;
                  break;
               case DRIdAxis6:
                  usGeneralOffset = DRREV1GenOffAxis6;
                  break;
               case DRIdAxis7:
                  usGeneralOffset = DRREV1GenOffAxis7;
                  break;
               case DRIdAxis8:
                  usGeneralOffset = DRREV1GenOffAxis8;
                  break;
            }
         }
         break;
      }
      case REV02:
      {
         switch (usItemId)
         {
            case DRIdSampleNumber:
               usDataType = DRTypeUSHORT;
               break;
            case DRIdGeneralInput0:
               usGeneralOffset = DRREV2GenOffGeneralInput0;
               break;
            case DRIdGeneralInput1:
               usGeneralOffset = DRREV2GenOffGeneralInput1;
               break;
            case DRIdGeneralInput2:
               usGeneralOffset = DRREV2GenOffGeneralInput2;
               break;
            case DRIdGeneralInput3:
               usGeneralOffset = DRREV2GenOffGeneralInput3;
               break;
            case DRIdGeneralInput4:
               usGeneralOffset = DRREV2GenOffGeneralInput4;
               break;
            case DRIdGeneralInput5:
               usGeneralOffset = DRREV2GenOffGeneralInput5;
               break;
            case DRIdGeneralInput6:
               usGeneralOffset = DRREV2GenOffGeneralInput6;
               break;
            case DRIdGeneralInput7:
               usGeneralOffset = DRREV2GenOffGeneralInput7;
               break;
            case DRIdGeneralInput8:
               usGeneralOffset = DRREV2GenOffGeneralInput8;
               break;
            case DRIdGeneralInput9:
               usGeneralOffset = DRREV2GenOffGeneralInput9;
               break;
            case DRIdGeneralOutput0:
               usGeneralOffset = DRREV2GenOffGeneralOutput0;
               break;
            case DRIdGeneralOutput1:
               usGeneralOffset = DRREV2GenOffGeneralOutput1;
               break;
            case DRIdGeneralOutput2:
               usGeneralOffset = DRREV2GenOffGeneralOutput2;
               break;
            case DRIdGeneralOutput3:
               usGeneralOffset = DRREV2GenOffGeneralOutput3;
               break;
            case DRIdGeneralOutput4:
               usGeneralOffset = DRREV2GenOffGeneralOutput4;
               break;
            case DRIdGeneralOutput5:
               usGeneralOffset = DRREV2GenOffGeneralOutput5;
               break;
            case DRIdGeneralOutput6:
               usGeneralOffset = DRREV2GenOffGeneralOutput6;
               break;
            case DRIdGeneralOutput7:
               usGeneralOffset = DRREV2GenOffGeneralOutput7;
               break;
            case DRIdGeneralOutput8:
               usGeneralOffset = DRREV2GenOffGeneralOutput8;
               break;
            case DRIdGeneralOutput9:
               usGeneralOffset = DRREV2GenOffGeneralOutput9;
               break;
            case DRIdErrorCode:
               usGeneralOffset = DRREV2GenOffErrorCode;
               break;
            case DRIdGeneralStatus:
               usGeneralOffset = DRREV2GenOffGeneralStatus;
               break;
            case DRIdSegmentCountS:
               usGeneralOffset = DRREV2GenOffSegmentCount;
               usDataType = DRTypeUSHORT;
               break;
            case DRIdCoordinatedMoveStatusS:
               usGeneralOffset = DRREV2GenOffCoordinatedMoveStatus;
               usDataType = DRTypeUSHORT;
               break;
            case DRIdCoordinatedMoveDistanceS:
               usGeneralOffset = DRREV2GenOffCoordinatedMoveDistance;
               usDataType = DRTypeLONG;
               break;
            case DRIdSegmentCountT:
               rc = DMCERROR_DATARECORD;
               break;
            case DRIdCoordinatedMoveStatusT:
               rc = DMCERROR_DATARECORD;
               break;
            case DRIdCoordinatedMoveDistanceT:
               rc = DMCERROR_DATARECORD;
               break;
            case DRIdAnalogInput1:
               rc = DMCERROR_DATARECORD;
               break;
            case DRIdAnalogInput2:
               rc = DMCERROR_DATARECORD;
               break;
            case DRIdAnalogInput3:
               rc = DMCERROR_DATARECORD;
               break;
            case DRIdAnalogInput4:
               rc = DMCERROR_DATARECORD;
               break;
            case DRIdAnalogInput5:
               rc = DMCERROR_DATARECORD;
               break;
            case DRIdAnalogInput6:
               rc = DMCERROR_DATARECORD;
               break;
            case DRIdAnalogInput7:
               rc = DMCERROR_DATARECORD;
               break;
            case DRIdAnalogInput8:
               rc = DMCERROR_DATARECORD;
               break;
            case DRIdAxisStatus:
               fDoAxis = TRUE;
               usAxisInfoOffset = DRREV2AxisOffAxisStatus;
               usDataType = DRTypeUSHORT;
               break;
            case DRIdAxisSwitches:
               fDoAxis = TRUE;
               usAxisInfoOffset = DRREV2AxisOffAxisSwitches;
               break;
            case DRIdAxisStopCode:
               fDoAxis = TRUE;
               usAxisInfoOffset = DRREV2AxisOffAxisStopCode;
               break;
            case DRIdAxisReferencePosition:
               fDoAxis = TRUE;
               usAxisInfoOffset = DRREV2AxisOffAxisReferencePosition;
               usDataType = DRTypeLONG;
               break;
            case DRIdAxisMotorPosition:
               fDoAxis = TRUE;
               usAxisInfoOffset = DRREV2AxisOffAxisMotorPosition;
               usDataType = DRTypeLONG;
               break;
            case DRIdAxisPositionError:
               fDoAxis = TRUE;
               usAxisInfoOffset = DRREV2AxisOffAxisPositionError;
               usDataType = DRTypeLONG;
               break;
            case DRIdAxisAuxillaryPosition:
               fDoAxis = TRUE;
               usAxisInfoOffset = DRREV2AxisOffAxisAuxillaryPosition;
               usDataType = DRTypeLONG;
               break;
            case DRIdAxisVelocity:
               fDoAxis = TRUE;
               usAxisInfoOffset = DRREV2AxisOffAxisVelocity;
               usDataType = DRTypeLONG;
               break;
            case DRIdAxisTorque:
               fDoAxis = TRUE;
               usAxisInfoOffset = DRREV2AxisOffAxisTorque;
               usDataType = DRTypeSHORT;
               break;
         }
         if (fDoAxis)
         {
            switch (usAxisId)
            {
               case DRIdAxis1:
                  usGeneralOffset = DRREV2GenOffAxis1;
                  break;
               case DRIdAxis2:
                  usGeneralOffset = DRREV2GenOffAxis2;
                  break;
               case DRIdAxis3:
                  usGeneralOffset = DRREV2GenOffAxis3;
                  break;
               case DRIdAxis4:
                  usGeneralOffset = DRREV2GenOffAxis4;
                  break;
               case DRIdAxis5:
                  usGeneralOffset = DRREV2GenOffAxis5;
                  break;
               case DRIdAxis6:
                  usGeneralOffset = DRREV2GenOffAxis6;
                  break;
               case DRIdAxis7:
                  usGeneralOffset = DRREV2GenOffAxis7;
                  break;
               case DRIdAxis8:
                  usGeneralOffset = DRREV2GenOffAxis8;
                  break;
            }
         }
         break;
      }
      case REV03:
      {
         switch (usItemId)
         {
            case DRIdSampleNumber:
               usDataType = DRTypeUSHORT;
               break;
            case DRIdGeneralInput0:
               usGeneralOffset = DRREV3GenOffGeneralInput0;
               break;
            case DRIdGeneralInput1:
               usGeneralOffset = DRREV3GenOffGeneralInput1;
               break;
            case DRIdGeneralInput2:
               usGeneralOffset = DRREV3GenOffGeneralInput2;
               break;
            case DRIdGeneralInput3:
               usGeneralOffset = DRREV3GenOffGeneralInput3;
               break;
            case DRIdGeneralInput4:
               usGeneralOffset = DRREV3GenOffGeneralInput4;
               break;
            case DRIdGeneralInput5:
               usGeneralOffset = DRREV3GenOffGeneralInput5;
               break;
            case DRIdGeneralInput6:
               usGeneralOffset = DRREV3GenOffGeneralInput6;
               break;
            case DRIdGeneralInput7:
               usGeneralOffset = DRREV3GenOffGeneralInput7;
               break;
            case DRIdGeneralInput8:
               usGeneralOffset = DRREV3GenOffGeneralInput8;
               break;
            case DRIdGeneralInput9:
               usGeneralOffset = DRREV3GenOffGeneralInput9;
               break;
            case DRIdGeneralOutput0:
               usGeneralOffset = DRREV3GenOffGeneralOutput0;
               break;
            case DRIdGeneralOutput1:
               usGeneralOffset = DRREV3GenOffGeneralOutput1;
               break;
            case DRIdGeneralOutput2:
               usGeneralOffset = DRREV3GenOffGeneralOutput2;
               break;
            case DRIdGeneralOutput3:
               usGeneralOffset = DRREV3GenOffGeneralOutput3;
               break;
            case DRIdGeneralOutput4:
               usGeneralOffset = DRREV3GenOffGeneralOutput4;
               break;
            case DRIdGeneralOutput5:
               usGeneralOffset = DRREV3GenOffGeneralOutput5;
               break;
            case DRIdGeneralOutput6:
               usGeneralOffset = DRREV3GenOffGeneralOutput6;
               break;
            case DRIdGeneralOutput7:
               usGeneralOffset = DRREV3GenOffGeneralOutput7;
               break;
            case DRIdGeneralOutput8:
               usGeneralOffset = DRREV3GenOffGeneralOutput8;
               break;
            case DRIdGeneralOutput9:
               usGeneralOffset = DRREV3GenOffGeneralOutput9;
               break;
            case DRIdErrorCode:
               usGeneralOffset = DRREV3GenOffErrorCode;
               break;
            case DRIdGeneralStatus:
               usGeneralOffset = DRREV3GenOffGeneralStatus;
               break;
            case DRIdSegmentCountS:
               usGeneralOffset = DRREV3GenOffSegmentCountS;
               usDataType = DRTypeUSHORT;
               break;
            case DRIdCoordinatedMoveStatusS:
               usGeneralOffset = DRREV3GenOffCoordinatedMoveStatusS;
               usDataType = DRTypeUSHORT;
               break;
            case DRIdCoordinatedMoveDistanceS:
               usGeneralOffset = DRREV3GenOffCoordinatedMoveDistanceS;
               usDataType = DRTypeLONG;
               break;
            case DRIdSegmentCountT:
               usGeneralOffset = DRREV3GenOffSegmentCountT;
               usDataType = DRTypeUSHORT;
               break;
            case DRIdCoordinatedMoveStatusT:
               usGeneralOffset = DRREV3GenOffCoordinatedMoveStatusT;
               usDataType = DRTypeUSHORT;
               break;
            case DRIdCoordinatedMoveDistanceT:
               usGeneralOffset = DRREV3GenOffCoordinatedMoveDistanceT;
               usDataType = DRTypeLONG;
               break;
            case DRIdAnalogInput1:
               rc = DMCERROR_DATARECORD;
               break;
            case DRIdAnalogInput2:
               rc = DMCERROR_DATARECORD;
               break;
            case DRIdAnalogInput3:
               rc = DMCERROR_DATARECORD;
               break;
            case DRIdAnalogInput4:
               rc = DMCERROR_DATARECORD;
               break;
            case DRIdAnalogInput5:
               rc = DMCERROR_DATARECORD;
               break;
            case DRIdAnalogInput6:
               rc = DMCERROR_DATARECORD;
               break;
            case DRIdAnalogInput7:
               rc = DMCERROR_DATARECORD;
               break;
            case DRIdAnalogInput8:
               rc = DMCERROR_DATARECORD;
               break;
            case DRIdAxisStatus:
               fDoAxis = TRUE;
               usAxisInfoOffset = DRREV3AxisOffAxisStatus;
               usDataType = DRTypeUSHORT;
               break;
            case DRIdAxisSwitches:
               fDoAxis = TRUE;
               usAxisInfoOffset = DRREV3AxisOffAxisSwitches;
               break;
            case DRIdAxisStopCode:
               fDoAxis = TRUE;
               usAxisInfoOffset = DRREV3AxisOffAxisStopCode;
               break;
            case DRIdAxisReferencePosition:
               fDoAxis = TRUE;
               usAxisInfoOffset = DRREV3AxisOffAxisReferencePosition;
               usDataType = DRTypeLONG;
               break;
            case DRIdAxisMotorPosition:
               fDoAxis = TRUE;
               usAxisInfoOffset = DRREV3AxisOffAxisMotorPosition;
               usDataType = DRTypeLONG;
               break;
            case DRIdAxisPositionError:
               fDoAxis = TRUE;
               usAxisInfoOffset = DRREV3AxisOffAxisPositionError;
               usDataType = DRTypeLONG;
               break;
            case DRIdAxisAuxillaryPosition:
               fDoAxis = TRUE;
               usAxisInfoOffset = DRREV3AxisOffAxisAuxillaryPosition;
               usDataType = DRTypeLONG;
               break;
            case DRIdAxisVelocity:
               fDoAxis = TRUE;
               usAxisInfoOffset = DRREV3AxisOffAxisVelocity;
               usDataType = DRTypeLONG;
               break;
            case DRIdAxisTorque:
               fDoAxis = TRUE;
               usAxisInfoOffset = DRREV3AxisOffAxisTorque;
               usDataType = DRTypeSHORT;
               break;
         }
         if (fDoAxis)
         {
            switch (usAxisId)
            {
               case DRIdAxis1:
                  usGeneralOffset = DRREV3GenOffAxis1;
                  break;
               case DRIdAxis2:
                  usGeneralOffset = DRREV3GenOffAxis2;
                  break;
               case DRIdAxis3:
                  usGeneralOffset = DRREV3GenOffAxis3;
                  break;
               case DRIdAxis4:
                  usGeneralOffset = DRREV3GenOffAxis4;
                  break;
               case DRIdAxis5:
                  usGeneralOffset = DRREV3GenOffAxis5;
                  break;
               case DRIdAxis6:
                  usGeneralOffset = DRREV3GenOffAxis6;
                  break;
               case DRIdAxis7:
                  usGeneralOffset = DRREV3GenOffAxis7;
                  break;
               case DRIdAxis8:
                  usGeneralOffset = DRREV3GenOffAxis8;
                  break;
            }
         }
         break;
      }
      case REV04:
      {
         switch (usItemId)
         {
            case DRIdSampleNumber:
               usDataType = DRTypeUSHORT;
               break;
            case DRIdGeneralInput0:
               usGeneralOffset = DRREV4GenOffGeneralInput0;
               break;
            case DRIdGeneralInput1:
               usGeneralOffset = DRREV4GenOffGeneralInput1;
               break;
            case DRIdGeneralInput2:
               usGeneralOffset = DRREV4GenOffGeneralInput2;
               break;
            case DRIdGeneralInput3:
               usGeneralOffset = DRREV4GenOffGeneralInput3;
               break;
            case DRIdGeneralInput4:
               usGeneralOffset = DRREV4GenOffGeneralInput4;
               break;
            case DRIdGeneralInput5:
               usGeneralOffset = DRREV4GenOffGeneralInput5;
               break;
            case DRIdGeneralInput6:
               usGeneralOffset = DRREV4GenOffGeneralInput6;
               break;
            case DRIdGeneralInput7:
               usGeneralOffset = DRREV4GenOffGeneralInput7;
               break;
            case DRIdGeneralInput8:
               usGeneralOffset = DRREV4GenOffGeneralInput8;
               break;
            case DRIdGeneralInput9:
               usGeneralOffset = DRREV4GenOffGeneralInput9;
               break;
            case DRIdGeneralOutput0:
               usGeneralOffset = DRREV4GenOffGeneralOutput0;
               break;
            case DRIdGeneralOutput1:
               usGeneralOffset = DRREV4GenOffGeneralOutput1;
               break;
            case DRIdGeneralOutput2:
               usGeneralOffset = DRREV4GenOffGeneralOutput2;
               break;
            case DRIdGeneralOutput3:
               usGeneralOffset = DRREV4GenOffGeneralOutput3;
               break;
            case DRIdGeneralOutput4:
               usGeneralOffset = DRREV4GenOffGeneralOutput4;
               break;
            case DRIdGeneralOutput5:
               usGeneralOffset = DRREV4GenOffGeneralOutput5;
               break;
            case DRIdGeneralOutput6:
               usGeneralOffset = DRREV4GenOffGeneralOutput6;
               break;
            case DRIdGeneralOutput7:
               usGeneralOffset = DRREV4GenOffGeneralOutput7;
               break;
            case DRIdGeneralOutput8:
               usGeneralOffset = DRREV4GenOffGeneralOutput8;
               break;
            case DRIdGeneralOutput9:
               usGeneralOffset = DRREV4GenOffGeneralOutput9;
               break;
            case DRIdErrorCode:
               usGeneralOffset = DRREV4GenOffErrorCode;
               break;
            case DRIdGeneralStatus:
               usGeneralOffset = DRREV4GenOffGeneralStatus;
               break;
            case DRIdSegmentCountS:
               usGeneralOffset = DRREV4GenOffSegmentCountS;
               usDataType = DRTypeUSHORT;
               break;
            case DRIdCoordinatedMoveStatusS:
               usGeneralOffset = DRREV4GenOffCoordinatedMoveStatusS;
               usDataType = DRTypeUSHORT;
               break;
            case DRIdCoordinatedMoveDistanceS:
               usGeneralOffset = DRREV4GenOffCoordinatedMoveDistanceS;
               usDataType = DRTypeLONG;
               break;
            case DRIdSegmentCountT:
               usGeneralOffset = DRREV4GenOffSegmentCountT;
               usDataType = DRTypeUSHORT;
               break;
            case DRIdCoordinatedMoveStatusT:
               usGeneralOffset = DRREV4GenOffCoordinatedMoveStatusT;
               usDataType = DRTypeUSHORT;
               break;
            case DRIdCoordinatedMoveDistanceT:
               usGeneralOffset = DRREV4GenOffCoordinatedMoveDistanceT;
               usDataType = DRTypeLONG;
               break;
            case DRIdAnalogInput1:
               usGeneralOffset = DRREV4GenOffAxis1;
               usAxisInfoOffset = DRREV4AxisOffAnalogInput;
               usDataType = DRTypeSHORT;
               break;
            case DRIdAnalogInput2:
               usGeneralOffset = DRREV4GenOffAxis2;
               usAxisInfoOffset = DRREV4AxisOffAnalogInput;
               usDataType = DRTypeSHORT;
               break;
            case DRIdAnalogInput3:
               usGeneralOffset = DRREV4GenOffAxis3;
               usAxisInfoOffset = DRREV4AxisOffAnalogInput;
               usDataType = DRTypeSHORT;
               break;
            case DRIdAnalogInput4:
               usGeneralOffset = DRREV4GenOffAxis4;
               usAxisInfoOffset = DRREV4AxisOffAnalogInput;
               usDataType = DRTypeSHORT;
               break;
            case DRIdAnalogInput5:
               usGeneralOffset = DRREV4GenOffAxis5;
               usAxisInfoOffset = DRREV4AxisOffAnalogInput;
               usDataType = DRTypeSHORT;
               break;
            case DRIdAnalogInput6:
               usGeneralOffset = DRREV4GenOffAxis6;
               usAxisInfoOffset = DRREV4AxisOffAnalogInput;
               usDataType = DRTypeSHORT;
               break;
            case DRIdAnalogInput7:
               usGeneralOffset = DRREV4GenOffAxis7;
               usAxisInfoOffset = DRREV4AxisOffAnalogInput;
               usDataType = DRTypeSHORT;
               break;
            case DRIdAnalogInput8:
               usGeneralOffset = DRREV4GenOffAxis8;
               usAxisInfoOffset = DRREV4AxisOffAnalogInput;
               usDataType = DRTypeSHORT;
               break;
            case DRIdAxisStatus:
               fDoAxis = TRUE;
               usAxisInfoOffset = DRREV4AxisOffAxisStatus;
               usDataType = DRTypeUSHORT;
               break;
            case DRIdAxisSwitches:
               fDoAxis = TRUE;
               usAxisInfoOffset = DRREV4AxisOffAxisSwitches;
               break;
            case DRIdAxisStopCode:
               fDoAxis = TRUE;
               usAxisInfoOffset = DRREV4AxisOffAxisStopCode;
               break;
            case DRIdAxisReferencePosition:
               fDoAxis = TRUE;
               usAxisInfoOffset = DRREV4AxisOffAxisReferencePosition;
               usDataType = DRTypeLONG;
               break;
            case DRIdAxisMotorPosition:
               fDoAxis = TRUE;
               usAxisInfoOffset = DRREV4AxisOffAxisMotorPosition;
               usDataType = DRTypeLONG;
               break;
            case DRIdAxisPositionError:
               fDoAxis = TRUE;
               usAxisInfoOffset = DRREV4AxisOffAxisPositionError;
               usDataType = DRTypeLONG;
               break;
            case DRIdAxisAuxillaryPosition:
               fDoAxis = TRUE;
               usAxisInfoOffset = DRREV4AxisOffAxisAuxillaryPosition;
               usDataType = DRTypeLONG;
               break;
            case DRIdAxisVelocity:
               fDoAxis = TRUE;
               usAxisInfoOffset = DRREV4AxisOffAxisVelocity;
               usDataType = DRTypeLONG;
               break;
            case DRIdAxisTorque:
               fDoAxis = TRUE;
               usAxisInfoOffset = DRREV4AxisOffAxisTorque;
               usDataType = DRTypeSHORT;
               break;
         }
         if (fDoAxis)
         {
            switch (usAxisId)
            {
               case DRIdAxis1:
                  usGeneralOffset = DRREV4GenOffAxis1;
                  break;
               case DRIdAxis2:
                  usGeneralOffset = DRREV4GenOffAxis2;
                  break;
               case DRIdAxis3:
                  usGeneralOffset = DRREV4GenOffAxis3;
                  break;
               case DRIdAxis4:
                  usGeneralOffset = DRREV4GenOffAxis4;
                  break;
               case DRIdAxis5:
                  usGeneralOffset = DRREV4GenOffAxis5;
                  break;
               case DRIdAxis6:
                  usGeneralOffset = DRREV4GenOffAxis6;
                  break;
               case DRIdAxis7:
                  usGeneralOffset = DRREV4GenOffAxis7;
                  break;
               case DRIdAxis8:
                  usGeneralOffset = DRREV4GenOffAxis8;
                  break;
            }
         }
         break;
      }
      case REV07: //DMC-18x6
      {
         switch (usItemId)
         {
            case DRIdSampleNumber:   usDataType = DRTypeUSHORT;                    break;
            case DRIdGeneralInput0:  usGeneralOffset = DRREV7GenOffGeneralInput0;  break;
            case DRIdGeneralInput1:  usGeneralOffset = DRREV7GenOffGeneralInput1;  break;
            case DRIdGeneralInput2:  usGeneralOffset = DRREV7GenOffGeneralInput2;  break;
            case DRIdGeneralInput3:  usGeneralOffset = DRREV7GenOffGeneralInput3;  break;
            case DRIdGeneralInput4:  usGeneralOffset = DRREV7GenOffGeneralInput4;  break;
            case DRIdGeneralInput5:  usGeneralOffset = DRREV7GenOffGeneralInput5;  break;
            case DRIdGeneralInput6:  usGeneralOffset = DRREV7GenOffGeneralInput6;  break;
            case DRIdGeneralInput7:  usGeneralOffset = DRREV7GenOffGeneralInput7;  break;
            case DRIdGeneralInput8:  usGeneralOffset = DRREV7GenOffGeneralInput8;  break;
            case DRIdGeneralInput9:  usGeneralOffset = DRREV7GenOffGeneralInput9;  break;
            case DRIdGeneralOutput0: usGeneralOffset = DRREV7GenOffGeneralOutput0; break;
            case DRIdGeneralOutput1: usGeneralOffset = DRREV7GenOffGeneralOutput1; break;
            case DRIdGeneralOutput2: usGeneralOffset = DRREV7GenOffGeneralOutput2; break;
            case DRIdGeneralOutput3: usGeneralOffset = DRREV7GenOffGeneralOutput3; break;
            case DRIdGeneralOutput4: usGeneralOffset = DRREV7GenOffGeneralOutput4; break;
            case DRIdGeneralOutput5: usGeneralOffset = DRREV7GenOffGeneralOutput5; break;
            case DRIdGeneralOutput6: usGeneralOffset = DRREV7GenOffGeneralOutput6; break;
            case DRIdGeneralOutput7: usGeneralOffset = DRREV7GenOffGeneralOutput7; break;
            case DRIdGeneralOutput8: usGeneralOffset = DRREV7GenOffGeneralOutput8; break;
            case DRIdGeneralOutput9: usGeneralOffset = DRREV7GenOffGeneralOutput9; break;

            case DRIdErrorCode:      usGeneralOffset = DRREV7GenOffErrorCode;      break;
            case DRIdGeneralStatus:  usGeneralOffset = DRREV7GenOffThreadStatus;   break; //either DRIdGeneralStatus or DRIdThreadStatus will get the same thing (which threads are running _XQ) on 18x6
            case DRIdThreadStatus:   usGeneralOffset = DRREV7GenOffThreadStatus;   break; //18x6

            case DRIdSegmentCountContour:      usGeneralOffset = DRREV7GenOffSegmentCountContour;      usDataType = DRTypeULONG;  break; //18x6
            case DRIdBufferSpaceContour:       usGeneralOffset = DRREV7GenOffBufferSpaceContour;       usDataType = DRTypeUSHORT; break; //18x6

            case DRIdSegmentCountS:            usGeneralOffset = DRREV7GenOffSegmentCountS;            usDataType = DRTypeUSHORT; break;
            case DRIdCoordinatedMoveStatusS:   usGeneralOffset = DRREV7GenOffCoordinatedMoveStatusS;   usDataType = DRTypeUSHORT; break;
            case DRIdCoordinatedMoveDistanceS: usGeneralOffset = DRREV7GenOffCoordinatedMoveDistanceS; usDataType = DRTypeLONG;   break;
            case DRIdBufferSpaceS:             usGeneralOffset = DRREV7GenOffBufferSpaceS;             usDataType = DRTypeUSHORT; break; //18x6

            case DRIdSegmentCountT:            usGeneralOffset = DRREV7GenOffSegmentCountT;            usDataType = DRTypeUSHORT; break;
            case DRIdCoordinatedMoveStatusT:   usGeneralOffset = DRREV7GenOffCoordinatedMoveStatusT;   usDataType = DRTypeUSHORT; break;
            case DRIdCoordinatedMoveDistanceT: usGeneralOffset = DRREV7GenOffCoordinatedMoveDistanceT; usDataType = DRTypeLONG;   break;
            case DRIdBufferSpaceT:             usGeneralOffset = DRREV7GenOffBufferSpaceT;             usDataType = DRTypeUSHORT; break; //18x6

            case DRIdAnalogInput1:             usGeneralOffset = DRREV7GenOffAxis1; usAxisInfoOffset = DRREV7AxisOffAnalogInput; usDataType = DRTypeSHORT; break;
            case DRIdAnalogInput2:             usGeneralOffset = DRREV7GenOffAxis2; usAxisInfoOffset = DRREV7AxisOffAnalogInput; usDataType = DRTypeSHORT; break;
            case DRIdAnalogInput3:             usGeneralOffset = DRREV7GenOffAxis3; usAxisInfoOffset = DRREV7AxisOffAnalogInput; usDataType = DRTypeSHORT; break;
            case DRIdAnalogInput4:             usGeneralOffset = DRREV7GenOffAxis4; usAxisInfoOffset = DRREV7AxisOffAnalogInput; usDataType = DRTypeSHORT; break;
            case DRIdAnalogInput5:             usGeneralOffset = DRREV7GenOffAxis5; usAxisInfoOffset = DRREV7AxisOffAnalogInput; usDataType = DRTypeSHORT; break;
            case DRIdAnalogInput6:             usGeneralOffset = DRREV7GenOffAxis6; usAxisInfoOffset = DRREV7AxisOffAnalogInput; usDataType = DRTypeSHORT; break;
            case DRIdAnalogInput7:             usGeneralOffset = DRREV7GenOffAxis7; usAxisInfoOffset = DRREV7AxisOffAnalogInput; usDataType = DRTypeSHORT; break;
            case DRIdAnalogInput8:             usGeneralOffset = DRREV7GenOffAxis8; usAxisInfoOffset = DRREV7AxisOffAnalogInput; usDataType = DRTypeSHORT; break;
            
            case DRIdAxisStatus:            fDoAxis = TRUE; usAxisInfoOffset = DRREV7AxisOffAxisStatus;            usDataType = DRTypeUSHORT; break;
            case DRIdAxisSwitches:          fDoAxis = TRUE; usAxisInfoOffset = DRREV7AxisOffAxisSwitches;                                     break;
            case DRIdAxisStopCode:          fDoAxis = TRUE; usAxisInfoOffset = DRREV7AxisOffAxisStopCode;                                     break;
            case DRIdAxisReferencePosition: fDoAxis = TRUE; usAxisInfoOffset = DRREV7AxisOffAxisReferencePosition; usDataType = DRTypeLONG;   break;
            case DRIdAxisMotorPosition:     fDoAxis = TRUE; usAxisInfoOffset = DRREV7AxisOffAxisMotorPosition;     usDataType = DRTypeLONG;   break;
            case DRIdAxisPositionError:     fDoAxis = TRUE; usAxisInfoOffset = DRREV7AxisOffAxisPositionError;     usDataType = DRTypeLONG;   break;
            case DRIdAxisAuxillaryPosition: fDoAxis = TRUE; usAxisInfoOffset = DRREV7AxisOffAxisAuxillaryPosition; usDataType = DRTypeLONG;   break;
            case DRIdAxisVelocity:          fDoAxis = TRUE; usAxisInfoOffset = DRREV7AxisOffAxisVelocity;          usDataType = DRTypeLONG;   break;
            case DRIdAxisTorque:            fDoAxis = TRUE; usAxisInfoOffset = DRREV7AxisOffAxisTorque;            usDataType = DRTypeLONG;   break; //DMC-18x6 changed from DRTypeSHORT to DRTypeLONG
            case DRIdUserVariable:          fDoAxis = TRUE; usAxisInfoOffset = DRREV7AxisOffUserVar;               usDataType = DRTypeLONG;   break; //DMC-18x6
         } //switch (usItemId)
         
         if (fDoAxis)
         {
            switch (usAxisId)
            {
               case DRIdAxis1: usGeneralOffset = DRREV7GenOffAxis1; break;
               case DRIdAxis2: usGeneralOffset = DRREV7GenOffAxis2; break;
               case DRIdAxis3: usGeneralOffset = DRREV7GenOffAxis3; break;
               case DRIdAxis4: usGeneralOffset = DRREV7GenOffAxis4; break;
               case DRIdAxis5: usGeneralOffset = DRREV7GenOffAxis5; break;
               case DRIdAxis6: usGeneralOffset = DRREV7GenOffAxis6; break;
               case DRIdAxis7: usGeneralOffset = DRREV7GenOffAxis7; break;
               case DRIdAxis8: usGeneralOffset = DRREV7GenOffAxis8; break;
            }
         }
         break;
      }  //case REV07:
   }    //switch (controller[iIndex].usRevision)

   if (!rc)
      rc = DMCGetDataRecord(hdmc, usGeneralOffset, usAxisInfoOffset, &usDataType, plData);
   else
      DMCTrace("   Item is not available in this revision of the controller's firmware.\n");

   *pusDataType = usDataType;

#ifdef DMC_DEBUG
   DMCTrace("Leaving DMCGetDataRecordByItemId.\n");
#endif   
   return rc;
}

LONG FAR GALILCALL DMCGetDataRecordSize(HANDLEDMC hdmc, PUSHORT pusRecordSize)
{
   long  rc = 0;
   int   iIndex;
   
#ifdef DMC_DEBUG
   DMCTrace("Entering DMCGetDataRecordSize.\n");
#endif   

   iIndex = Handle2Index(hdmc);
   if (iIndex == -1)
   {
#ifdef DMC_DEBUG
      DMCTrace("   Invalid handle.\n");
      DMCTrace("Leaving DMCGetDataRecordSize.\n");
#endif   
      return DMCERROR_HANDLE;
   }

   if (!controller[iIndex].pbDataRecord)
   {
#ifdef DMC_DEBUG
      DMCTrace("   Data record is NULL.\n");
      DMCTrace("Leaving DMCGetDataRecordSize.\n");
#endif   
      return DMCERROR_DATARECORD;
   }

   if (!pusRecordSize)
   {
#ifdef DMC_DEBUG
      DMCTrace("   Return buffer is NULL.\n");
      DMCTrace("Leaving DMCGetDataRecordSize.\n");
#endif   
      return DMCERROR_ARGUMENT;
   }

   *pusRecordSize = controller[iIndex].usDataRecordSize;

#ifdef DMC_DEBUG
   DMCTrace("Leaving DMCGetDataRecordSize.\n");
#endif     
   return rc;
}

LONG FAR GALILCALL DMCCopyDataRecord(HANDLEDMC hdmc, PVOID pDataRecord)
{
   long  rc = 0;
   int   iIndex;
   
#ifdef DMC_DEBUG
   DMCTrace("Entering DMCCopyDataRecord.\n");
#endif     

   iIndex = Handle2Index(hdmc);
   if (iIndex == -1)
   {
#ifdef DMC_DEBUG
      DMCTrace("   Invalid handle.\n");
      DMCTrace("Leaving DMCCopyDataRecord.\n");
#endif     
      return DMCERROR_HANDLE;
   }

   if (!controller[iIndex].pbDataRecord)
   {
#ifdef DMC_DEBUG
      DMCTrace("   Data record is NULL.\n");
      DMCTrace("Leaving DMCCopyDataRecord.\n");
#endif     
      return DMCERROR_DATARECORD;
   }

   if (!pDataRecord)
   {
#ifdef DMC_DEBUG
      DMCTrace("   Return buffer is NULL.\n");
      DMCTrace("Leaving DMCCopyDataRecord.\n");
#endif     
      return DMCERROR_ARGUMENT;
   }

   memcpy((PVOID)pDataRecord, (PVOID)controller[iIndex].pbDataRecord, (size_t)controller[iIndex].usDataRecordSize);

#ifdef DMC_DEBUG
   DMCTrace("Leaving DMCCopyDataRecord.\n");
#endif       
   return rc;
}

LONG FAR GALILCALL DMCGetDataRecordRevision(HANDLEDMC hdmc, PUSHORT pusRevision)
{
   long  rc = 0;
   int   iIndex;
   
#ifdef DMC_DEBUG
   DMCTrace("Entering DMCGetDataRecordRevision.\n");
#endif     

   iIndex = Handle2Index(hdmc);
   if (iIndex == -1)
   {
#ifdef DMC_DEBUG
      DMCTrace("   Invalid handle.\n");
      DMCTrace("Leaving DMCGetDataRecordRevision.\n");
#endif     
      return DMCERROR_HANDLE;
   }

   if (!controller[iIndex].pbDataRecord)
   {
#ifdef DMC_DEBUG
      DMCTrace("   Data record is NULL.\n");
      DMCTrace("Leaving DMCGetDataRecordRevision.\n");
#endif     
      return DMCERROR_DATARECORD;
   }

   if (!pusRevision)
   {
#ifdef DMC_DEBUG
      DMCTrace("   Return buffer is NULL.\n");
      DMCTrace("Leaving DMCGetDataRecordRevision.\n");
#endif   
      return DMCERROR_ARGUMENT;
   }

   switch (controller[iIndex].usRevision)
   {
      default:
         *pusRevision = 0;
         rc = DMCERROR_DATARECORD;
         break;
      case REV01:
         *pusRevision = 1;
         break;
      case REV02:
         *pusRevision = 2;
         break;
      case REV03:
         *pusRevision = 3;
         break;
      case REV04:
         *pusRevision = 4;
         break;
      case REV07:
         *pusRevision = 7;
         break;
   }  

#ifdef DMC_DEBUG
   DMCTrace("Leaving DMCGetDataRecordRevision.\n");
#endif   
   return rc;
}

USHORT GetDataType(USHORT usGeneralOffset, USHORT usAxisInfoOffset, USHORT usRevision)
{
   switch (usRevision)
   {
      case REV01:
      {
         switch (usGeneralOffset)
         {
            case DRREV1GenOffSampleNumber:
            case DRREV1GenOffSegmentCount:
            case DRREV1GenOffCoordinatedMoveStatus:
               return(DRTypeUSHORT);
      
            case DRREV1GenOffGeneralInput1:
            case DRREV1GenOffGeneralInput2:
            case DRREV1GenOffGeneralInput3:
            case DRREV1GenOffSpare:
            case DRREV1GenOffGeneralOutput1:
            case DRREV1GenOffGeneralOutput2:
            case DRREV1GenOffErrorCode:
            case DRREV1GenOffGeneralStatus:
               return(DRTypeUCHAR);
      
            case DRREV1GenOffCoordinatedMoveDistance:
               return(DRTypeLONG);
         }
         if (usGeneralOffset < DRREV1GenOffAxis1)
            return(DRTypeUnknown);

         switch (usAxisInfoOffset)
         {
            case DRREV1AxisOffAxisStatus:
               return(DRTypeUSHORT);
      
            case DRREV1AxisOffAxisSwitches:
            case DRREV1AxisOffAxisStopCode:
               return(DRTypeUCHAR);
      
            case DRREV1AxisOffAxisReferencePosition:
            case DRREV1AxisOffAxisMotorPosition:
            case DRREV1AxisOffAxisPositionError:
            case DRREV1AxisOffAxisAuxillaryPosition:
            case DRREV1AxisOffAxisVelocity:
               return(DRTypeLONG);

            case DRREV1AxisOffAxisTorque:
               return(DRTypeSHORT);
         }
         break;
      }

      case REV02:
      {
         switch (usGeneralOffset)
         {
            case DRREV2GenOffSampleNumber:
            case DRREV2GenOffSegmentCount:
            case DRREV2GenOffCoordinatedMoveStatus:
               return(DRTypeUSHORT);
      
            case DRREV2GenOffGeneralInput0:
            case DRREV2GenOffGeneralInput1:
            case DRREV2GenOffGeneralInput2:
            case DRREV2GenOffGeneralInput3:
            case DRREV2GenOffGeneralInput4:
            case DRREV2GenOffGeneralInput5:
            case DRREV2GenOffGeneralInput6:
            case DRREV2GenOffGeneralInput7:
            case DRREV2GenOffGeneralInput8:
            case DRREV2GenOffGeneralInput9:
            case DRREV2GenOffGeneralOutput0:
            case DRREV2GenOffGeneralOutput1:
            case DRREV2GenOffGeneralOutput2:
            case DRREV2GenOffGeneralOutput3:
            case DRREV2GenOffGeneralOutput4:
            case DRREV2GenOffGeneralOutput5:
            case DRREV2GenOffGeneralOutput6:
            case DRREV2GenOffGeneralOutput7:
            case DRREV2GenOffGeneralOutput8:
            case DRREV2GenOffGeneralOutput9:
            case DRREV2GenOffErrorCode:
            case DRREV2GenOffGeneralStatus:
               return(DRTypeUCHAR);
      
            case DRREV2GenOffCoordinatedMoveDistance:
               return(DRTypeLONG);
         }
         if (usGeneralOffset < DRREV2GenOffAxis1)
            return(DRTypeUnknown);

         switch (usAxisInfoOffset)
         {
            case DRREV2AxisOffAxisStatus:
               return(DRTypeUSHORT);
      
            case DRREV2AxisOffAxisSwitches:
            case DRREV2AxisOffAxisStopCode:
               return(DRTypeUCHAR);
      
            case DRREV2AxisOffAxisReferencePosition:
            case DRREV2AxisOffAxisMotorPosition:
            case DRREV2AxisOffAxisPositionError:
            case DRREV2AxisOffAxisAuxillaryPosition:
            case DRREV2AxisOffAxisVelocity:
               return(DRTypeLONG);

            case DRREV2AxisOffAxisTorque:
               return(DRTypeSHORT);
         }
         break;
      }

      case REV03:
      {
         switch (usGeneralOffset)
         {
            case DRREV3GenOffSampleNumber:
            case DRREV3GenOffSegmentCountS:
            case DRREV3GenOffSegmentCountT:
            case DRREV3GenOffCoordinatedMoveStatusS:
            case DRREV3GenOffCoordinatedMoveStatusT:
               return(DRTypeUSHORT);
      
            case DRREV3GenOffGeneralInput0:
            case DRREV3GenOffGeneralInput1:
            case DRREV3GenOffGeneralInput2:
            case DRREV3GenOffGeneralInput3:
            case DRREV3GenOffGeneralInput4:
            case DRREV3GenOffGeneralInput5:
            case DRREV3GenOffGeneralInput6:
            case DRREV3GenOffGeneralInput7:
            case DRREV3GenOffGeneralInput8:
            case DRREV3GenOffGeneralInput9:
            case DRREV3GenOffGeneralOutput0:
            case DRREV3GenOffGeneralOutput1:
            case DRREV3GenOffGeneralOutput2:
            case DRREV3GenOffGeneralOutput3:
            case DRREV3GenOffGeneralOutput4:
            case DRREV3GenOffGeneralOutput5:
            case DRREV3GenOffGeneralOutput6:
            case DRREV3GenOffGeneralOutput7:
            case DRREV3GenOffGeneralOutput8:
            case DRREV3GenOffGeneralOutput9:
            case DRREV3GenOffErrorCode:
            case DRREV3GenOffGeneralStatus:
               return(DRTypeUCHAR);
      
            case DRREV3GenOffCoordinatedMoveDistanceS:
            case DRREV3GenOffCoordinatedMoveDistanceT:
               return(DRTypeLONG);
         }
         if (usGeneralOffset < DRREV3GenOffAxis1)
            return(DRTypeUnknown);

         switch (usAxisInfoOffset)
         {
            case DRREV3AxisOffAxisStatus:
               return(DRTypeUSHORT);
      
            case DRREV3AxisOffAxisSwitches:
            case DRREV3AxisOffAxisStopCode:
               return(DRTypeUCHAR);
      
            case DRREV3AxisOffAxisReferencePosition:
            case DRREV3AxisOffAxisMotorPosition:
            case DRREV3AxisOffAxisPositionError:
            case DRREV3AxisOffAxisAuxillaryPosition:
            case DRREV3AxisOffAxisVelocity:
               return(DRTypeLONG);

            case DRREV3AxisOffAxisTorque:
               return(DRTypeSHORT);
         }
         break;
      }

      case REV04:
      {
         switch (usGeneralOffset)
         {
            case DRREV4GenOffSampleNumber:
            case DRREV4GenOffSegmentCountS:
            case DRREV4GenOffSegmentCountT:
            case DRREV4GenOffCoordinatedMoveStatusS:
            case DRREV4GenOffCoordinatedMoveStatusT:
               return(DRTypeUSHORT);
      
            case DRREV4GenOffGeneralInput0:
            case DRREV4GenOffGeneralInput1:
            case DRREV4GenOffGeneralInput2:
            case DRREV4GenOffGeneralInput3:
            case DRREV4GenOffGeneralInput4:
            case DRREV4GenOffGeneralInput5:
            case DRREV4GenOffGeneralInput6:
            case DRREV4GenOffGeneralInput7:
            case DRREV4GenOffGeneralInput8:
            case DRREV4GenOffGeneralInput9:
            case DRREV4GenOffGeneralOutput0:
            case DRREV4GenOffGeneralOutput1:
            case DRREV4GenOffGeneralOutput2:
            case DRREV4GenOffGeneralOutput3:
            case DRREV4GenOffGeneralOutput4:
            case DRREV4GenOffGeneralOutput5:
            case DRREV4GenOffGeneralOutput6:
            case DRREV4GenOffGeneralOutput7:
            case DRREV4GenOffGeneralOutput8:
            case DRREV4GenOffGeneralOutput9:
            case DRREV4GenOffErrorCode:
            case DRREV4GenOffGeneralStatus:
               return(DRTypeUCHAR);
      
            case DRREV4GenOffCoordinatedMoveDistanceS:
            case DRREV4GenOffCoordinatedMoveDistanceT:
               return(DRTypeLONG);
         }
         if (usGeneralOffset < DRREV4GenOffAxis1)
            return(DRTypeUnknown);

         switch (usAxisInfoOffset)
         {
            case DRREV4AxisOffAxisStatus:
               return(DRTypeUSHORT);
      
            case DRREV4AxisOffAxisSwitches:
            case DRREV4AxisOffAxisStopCode:
               return(DRTypeUCHAR);
      
            case DRREV4AxisOffAxisReferencePosition:
            case DRREV4AxisOffAxisMotorPosition:
            case DRREV4AxisOffAxisPositionError:
            case DRREV4AxisOffAxisAuxillaryPosition:
            case DRREV4AxisOffAxisVelocity:
               return(DRTypeLONG);

            case DRREV4AxisOffAxisTorque:
            case DRREV4AxisOffAnalogInput:
               return(DRTypeSHORT);
         }
         break;
      } //case REV04:

      case REV07:
      {
         switch (usGeneralOffset)
         {
            case DRREV7GenOffSampleNumber:
            case DRREV7GenOffSegmentCountS:
            case DRREV7GenOffSegmentCountT:
            case DRREV7GenOffCoordinatedMoveStatusS:
            case DRREV7GenOffCoordinatedMoveStatusT:
            case DRREV7GenOffBufferSpaceS: //18x6
            case DRREV7GenOffBufferSpaceT: //18x6
            case DRREV7GenOffBufferSpaceContour: //18X6
               return(DRTypeUSHORT);
      
            case DRREV7GenOffGeneralInput0:
            case DRREV7GenOffGeneralInput1:
            case DRREV7GenOffGeneralInput2:
            case DRREV7GenOffGeneralInput3:
            case DRREV7GenOffGeneralInput4:
            case DRREV7GenOffGeneralInput5:
            case DRREV7GenOffGeneralInput6:
            case DRREV7GenOffGeneralInput7:
            case DRREV7GenOffGeneralInput8:
            case DRREV7GenOffGeneralInput9:
            case DRREV7GenOffGeneralOutput0:
            case DRREV7GenOffGeneralOutput1:
            case DRREV7GenOffGeneralOutput2:
            case DRREV7GenOffGeneralOutput3:
            case DRREV7GenOffGeneralOutput4:
            case DRREV7GenOffGeneralOutput5:
            case DRREV7GenOffGeneralOutput6:
            case DRREV7GenOffGeneralOutput7:
            case DRREV7GenOffGeneralOutput8:
            case DRREV7GenOffGeneralOutput9:
            case DRREV7GenOffErrorCode:
            case DRREV7GenOffThreadStatus:
               return(DRTypeUCHAR);
      
            case DRREV7GenOffCoordinatedMoveDistanceS:
            case DRREV7GenOffCoordinatedMoveDistanceT:
               return(DRTypeLONG);

            case DRREV7GenOffSegmentCountContour: //18X6
               return(DRTypeULONG);
         }
         if (usGeneralOffset < DRREV7GenOffAxis1)
            return(DRTypeUnknown);

         switch (usAxisInfoOffset)
         {
            case DRREV7AxisOffAxisStatus:
               return(DRTypeUSHORT);
      
            case DRREV7AxisOffAxisSwitches:
            case DRREV7AxisOffAxisStopCode:
               return(DRTypeUCHAR);
      
            case DRREV7AxisOffAxisReferencePosition:
            case DRREV7AxisOffAxisMotorPosition:
            case DRREV7AxisOffAxisPositionError:
            case DRREV7AxisOffAxisAuxillaryPosition:
            case DRREV7AxisOffAxisVelocity:
            case DRREV7AxisOffAxisTorque: //18x6 new size
            case DRREV7AxisOffUserVar: //18x6
               return(DRTypeLONG);

            case DRREV7AxisOffAnalogInput:
               return(DRTypeSHORT);     
         }
         break;
      } //case REV07:
      
   } //switch (usRevision)

   return DRTypeUnknown;
} //USHORT GetDataType(USHORT usGeneralOffset, USHORT usAxisInfoOffset, USHORT usRevision)


#ifdef DMC_DMA
long InitDMA(int iIndex)
{
   long  rc = 0L;

   controller[iIndex].dmadata.buffer_size = DMA_BUFFER_SIZE;
   controller[iIndex].dmadata.transfer_size = controller[iIndex].usDataRecordSize;

   rc = Dma_initialize(&(controller[iIndex].dmadata), controller[iIndex].controllerinfo.hardwareinfo.businfo.usDMAChannel);

   return rc;
}

long UninitDMA(int iIndex)
{
   Dma_uninitialize(&(controller[iIndex].dmadata), controller[iIndex].controllerinfo.hardwareinfo.businfo.usDMAChannel);

   return 0L;
}

/* Set dma mask register to enable the DMA transfer */
void Dma_enable(PDMADATA pdmadata, USHORT usDMAChannel)
{
   if (!pdmadata || pdmadata->enable)
      return;

   if (usDMAChannel == 0)
      OUTP(DMA_1_MASK_REGISTER,DMA_1_CHANNEL_0_CLEAR_MASK);
   else if (usDMAChannel == 1)
      OUTP(DMA_1_MASK_REGISTER,DMA_1_CHANNEL_1_CLEAR_MASK);
   else if (usDMAChannel == 3)
      OUTP(DMA_1_MASK_REGISTER,DMA_1_CHANNEL_3_CLEAR_MASK);
   else
      return;

   pdmadata->enable = 1;

#ifdef DMC_DEBUG
   DMCTrace("   Turning on DMA channel %d\n",usDMAChannel);
#endif
}

/* Set dma mask register to disable the DMA transfer */
void Dma_disable(PDMADATA pdmadata, USHORT usDMAChannel)
{
   if (!pdmadata || !pdmadata->enable)
      return;

   if (usDMAChannel == 0)
      OUTP(DMA_1_MASK_REGISTER,DMA_1_CHANNEL_0_SET_MASK);
   else if (usDMAChannel == 1)
      OUTP(DMA_1_MASK_REGISTER,DMA_1_CHANNEL_1_SET_MASK);
   else if (usDMAChannel == 3)
      OUTP(DMA_1_MASK_REGISTER,DMA_1_CHANNEL_3_SET_MASK);

#ifdef DMC_DEBUG
   DMCTrace("    Turning off DMA channel %d\n",usDMAChannel);
#endif
}

/* Free up the DMA transfer memory */
void Dma_uninitialize(PDMADATA pdmadata, USHORT usDMAChannel)
{
   if (!pdmadata)
      return;

#ifdef __DPMI16__
   GlobalDosFree(LOWORD(pdmadata->global_memory));
#else
   if (pdmadata->seg_address)
   {
      Freemem(pdmadata->seg_address);
      pdmadata->seg_address = 0;
   }
#endif

   Dma_disable(pdmadata, usDMAChannel);
}

/* Set up DMA transfer and enable the DMA channel

   The general procedure is listed in the following 7 steps:
     1. Block DMA channel
     2. Set transfer mode
     3. Clear flip-flop
     4. Write address of the data block
     5. Write page of the data block
     6. Write transfer length
     7. Release DMA channel again */
long Dma_reset_channel(PDMADATA pdmadata, USHORT usDMAChannel)
{
   if (!pdmadata)
   {
#ifdef DMC_DEBUG
      DMCTrace("   Invalid argument for DMCDATA\n");
#endif
      return DMCERROR_ARGUMENT;
   }

   if (usDMAChannel != 0 && usDMAChannel != 1 && usDMAChannel != 3)
   {
#ifdef DMC_DEBUG
      DMCTrace("   Invalid DMA Channel %d \n",usDMAChannel);
#endif
      return DMCERROR_DMA;
   }

   if (usDMAChannel == 0)
   {
      OUTP(DMA_1_MASK_REGISTER,DMA_1_CHANNEL_0_SET_MASK);
      OUTP(DMA_1_MODE_REGISTER,DMA_1_CHANNEL_0_DEMAND_AUTO_INC_WRITE);
      OUTP(DMA_1_CLEAR_REGISTER,0);
      OUTP(AT_DMA_1_CHANNEL_0_BASE_ADDRESS,pdmadata->low_byte_addr);
      OUTP(AT_DMA_1_CHANNEL_0_BASE_ADDRESS,pdmadata->high_byte_addr);
      OUTP(AT_DMA_PAGE_REGISTER_CHANNEL_0,pdmadata->page_register);
      OUTP(AT_DMA_1_CHANNEL_0_BASE_COUNT,pdmadata->low_byte_count);
      OUTP(AT_DMA_1_CHANNEL_0_BASE_COUNT,pdmadata->high_byte_count);
   }
   else if (usDMAChannel == 1)
   {
      OUTP(DMA_1_MASK_REGISTER,DMA_1_CHANNEL_1_SET_MASK);
      OUTP(DMA_1_MODE_REGISTER,DMA_1_CHANNEL_1_DEMAND_AUTO_INC_WRITE);
      OUTP(DMA_1_CLEAR_REGISTER,0);
      OUTP(AT_DMA_1_CHANNEL_1_BASE_ADDRESS,pdmadata->low_byte_addr);
      OUTP(AT_DMA_1_CHANNEL_1_BASE_ADDRESS,pdmadata->high_byte_addr);
      OUTP(AT_DMA_PAGE_REGISTER_CHANNEL_1,pdmadata->page_register);
      OUTP(AT_DMA_1_CHANNEL_1_BASE_COUNT,pdmadata->low_byte_count);
      OUTP(AT_DMA_1_CHANNEL_1_BASE_COUNT,pdmadata->high_byte_count);
   }
   else if (usDMAChannel == 3)
   {
      OUTP(DMA_1_MASK_REGISTER,DMA_1_CHANNEL_3_SET_MASK);
      OUTP(DMA_1_MODE_REGISTER,DMA_1_CHANNEL_3_DEMAND_AUTO_INC_WRITE);
      OUTP(DMA_1_CLEAR_REGISTER,0);
      OUTP(AT_DMA_1_CHANNEL_3_BASE_ADDRESS,pdmadata->low_byte_addr);
      OUTP(AT_DMA_1_CHANNEL_3_BASE_ADDRESS,pdmadata->high_byte_addr);
      OUTP(AT_DMA_PAGE_REGISTER_CHANNEL_3,pdmadata->page_register);
      OUTP(AT_DMA_1_CHANNEL_3_BASE_COUNT,pdmadata->low_byte_count);
      OUTP(AT_DMA_1_CHANNEL_3_BASE_COUNT,pdmadata->high_byte_count);
   }

  Dma_enable(pdmadata, usDMAChannel);

  return 0L;
}

/* Initialize pdmadata record and enable the DMA transfer */
long Dma_initialize(PDMADATA pdmadata, USHORT usDMAChannel)
{
   unsigned stat = 0;

   if (!pdmadata)
   {
#ifdef DMC_DEBUG
      DMCTrace("   Invalid argument for DMCDATA\n");
#endif
      return DMCERROR_ARGUMENT;
   }

   if (usDMAChannel != 0 && usDMAChannel != 1 && usDMAChannel != 3)
   {
#ifdef DMC_DEBUG
      DMCTrace("   Invalid DMA Channel %d \n",usDMAChannel);
#endif
      return DMCERROR_DMA;
   }

#ifdef __DPMI16__
   stat = stat;

   /* GlobalDosAlloc() allows you to allocate on the segment boundary */
   pdmadata->global_memory = GlobalDosAlloc(pdmadata->buffer_size);
   if (!pdmadata->global_memory)
   {
#ifdef DMC_DEBUG
      DMCTrace("   Could not allocate memory for DMA.\n");
#endif
      return DMCERROR_MEMORY;
   }

   pdmadata->dma_record = (CHAR FAR*)MAKELONG(0, LOWORD(pdmadata->global_memory));
   pdmadata->seg_address = HIWORD(pdmadata->global_memory);
   pdmadata->page_register = pdmadata->seg_address >> 12;
   pdmadata->low_byte_addr = ((pdmadata->seg_address & MASK_LOW_BYTE) << 4);
   pdmadata->high_byte_addr = ((pdmadata->seg_address & MASK_HIGH_BYTE) >> 4);
#else
   pdmadata->seg_size = DMA_BUFFER_SIZE >> 4;

   /* Allocmem() allows you to allocate on the segment boundary */
   stat = Allocmem(pdmadata->seg_size, (unsigned*)&pdmadata->seg_address);
   if (stat != 0)
   {
#ifdef DMC_DEBUG
      DMCTrace("   Could not allocate memory for DMA.\n");
#endif
      return DMCERROR_MEMORY;
   }

   pdmadata->dma_record = (CHAR FAR*)Dma_MK_FP(pdmadata->seg_address, 0);
   pdmadata->page_register = (unsigned short)(pdmadata->seg_address >> 12);
   pdmadata->low_byte_addr = (unsigned short)((pdmadata->seg_address & MASK_LOW_BYTE) << 4);
   pdmadata->high_byte_addr = (unsigned short)((pdmadata->seg_address & MASK_HIGH_BYTE) >> 4);
#endif /* __DPMI16 */

   pdmadata->low_byte_count = (unsigned short)((pdmadata->transfer_size - 1) & 0x0FF);
   pdmadata->high_byte_count = (unsigned short)((pdmadata->transfer_size - 1) >> 8);

#ifdef DMC_DEBUG
   DMCTrace("   Allocated memory at segment: %d\n   Buffer size = %d bytes\n   Page register =  %d\n   Low byte address =  %d\n   High byte address =  %d\n   Low byte count =  %d\n   High byte count =  %d\n",
      pdmadata->seg_address, pdmadata->buffer_size, pdmadata->page_register, pdmadata->low_byte_addr,
      pdmadata->high_byte_addr, pdmadata->low_byte_count, pdmadata->high_byte_count);
#endif

   return Dma_reset_channel(pdmadata, usDMAChannel);
}
#endif /* DMC_DMA */

#endif /* DMC_DATARECORDACCESS || DMC_DMA */
