/* Copyright (C) 2011 Circuits At Home, LTD. All rights reserved.

This software may be distributed and modified under the terms of the GNU
General Public License version 2 (GPL2) as published by the Free Software
Foundation and appearing in the file GPL2.TXT included in the packaging of
this file. Please note that GPL2 Section 2[b] requires that all works based
on this software must also be made publicly available under the terms of
the GPL2 ("Copyleft").

Contact information
-------------------

Circuits At Home, LTD
Web      :  http://www.circuitsathome.com
e-mail   :  support@circuitsathome.com
*/

#include "ptpconstitles.h"

const char* const ptpopNames[] =
    {
        msgUndefined,
        msgGetDeviceInfo,
        msgOpenSession,
        msgCloseSession,
        msgGetStorageIDs,
        msgGetStorageInfo,
        msgGetNumObjects,
        msgGetObjectHandles,
        msgGetObjectInfo,
        msgGetObject,
        msgGetThumb,
        msgDeleteObject,
        msgSendObjectInfo,
        msgSendObject,
        msgInitiateCapture,
        msgFormatStore,
        msgResetDevice,
        msgSelfTest,
        msgSetObjectProtection,
        msgPowerDown,
        msgGetDevicePropDesc,
        msgGetDevicePropValue,
        msgSetDevicePropValue,
        msgResetDevicePropValue,
        msgTerminateOpenCapture,
        msgMoveObject,
        msgCopyObject,
        msgGetPartialObject,
        msgInitiateOpenCapture};

const char* const mtpopNames[] =
    {
        msgUndefined,
        msgGetObjectPropsSupported,
        msgGetObjectPropDesc,
        msgGetObjectPropValue,
        msgSetObjectPropValue,
        msgGetObjectPropList,
        msgSetObjectPropList,
        msgGetInterdependentPropDesc,
        msgSendObjectPropList};

const char* const ptpevNames[] =
    {
        msgUndefined,
        msgCancelTransaction,
        msgObjectAdded,
        msgObjectRemoved,
        msgStoreAdded,
        msgStoreRemoved,
        msgDevicePropChanged,
        msgObjectInfoChanged,
        msgDeviceInfoChanged,
        msgRequestObjectTransfer,
        msgStoreFull,
        msgDeviceReset,
        msgStorageInfoChanged,
        msgCaptureComplete,
        msgUnreportedStatus};

const char* const mtpevNames[] =
    {
        msgUndefined,
        msgObjectPropChanged,
        msgObjectPropDescChanged,
        msgObjectReferencesChanged};

const char* const ptpprNames[] =
    {
        msgUndefined,
        msgBatteryLevel,
        msgFunctionalMode,
        msgImageSize,
        msgCompressionSetting,
        msgWhiteBalance,
        msgRGBGain,
        msgFNumber,
        msgFocalLength,
        msgFocusDistance,
        msgFocusMode,
        msgExposureMeteringMode,
        msgFlashMode,
        msgExposureTime,
        msgExposureProgramMode,
        msgExposureIndex,
        msgExposureBiasCompensation,
        msgDateTime,
        msgCaptureDelay,
        msgStillCaptureMode,
        msgContrast,
        msgSharpness,
        msgDigitalZoom,
        msgEffectMode,
        msgBurstNumber,
        msgBurstInterval,
        msgTimelapseNumber,
        msgTimelapseInterval,
        msgFocusMeteringMode,
        msgUploadURL,
        msgArtist,
        msgCopyrightInfo};

const char* const mtpprNames[] =
    {
        msgUndefined,
        msgSynchronization_Partner,
        msgDevice_Friendly_Name,
        msgVolume,
        msgSupportedFormatsOrdered,
        msgDeviceIcon,
        msgSession_Initiator_Version_Info,
        msgPerceived_Device_Type,
        msgPlayback_Rate,
        msgPlayback_Object,
        msgPlayback_Container};

const char* const acNames[] =
    {
        msgUndefined,
        msgAssociation,
        msgScript,
        msgExecutable,
        msgText,
        msgHTML,
        msgDPOF,
        msgAIFF,
        msgWAV,
        msgMP3,
        msgAVI,
        msgMPEG,
        msgASF,
        msgQT};

const char* const imNames[] =
    {
        msgUndefined,
        msgEXIF_JPEG,
        msgTIFF_EP,
        msgFlashPix,
        msgBMP,
        msgCIFF,
        msgUndefined_0x3806,
        msgGIF,
        msgJFIF,
        msgPCD,
        msgPICT,
        msgPNG,
        msgUndefined_0x380C,
        msgTIFF,
        msgTIFF_IT,
        msgJP2,
        msgJPX,
};

bool PrintPTPOperation(uint16_t op) {
    if ((op & 0xFF) <= (PTP_OC_InitiateOpenCapture & 0xFF)) {
        E_Notify(ptpopNames[(op & 0xFF)], 0x80);
        return true;
    }
    return false;
}