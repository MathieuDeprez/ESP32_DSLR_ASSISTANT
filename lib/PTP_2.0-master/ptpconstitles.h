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
#ifndef __PTPCONSTITLES_H__
#define __PTPCONSTITLES_H__

#include "ptpconst.h"
#include "mtpconst.h"
#include "ptp.h"

const char msgVendorDefined			[] PROGMEM = "Vendor defined";
const char msgTab					[] PROGMEM = "\t";
const char msgCRLF					[] PROGMEM = "\r\n";

// PTP Operations
const char msgUndefined				[] PROGMEM = "Undefined";
const char msgGetDeviceInfo			[] PROGMEM = "GetDeviceInfo";
const char msgOpenSession			[] PROGMEM = "OpenSession";
const char msgCloseSession			[] PROGMEM = "CloseSession";
const char msgGetStorageIDs			[] PROGMEM = "GetStorageIDs";
const char msgGetStorageInfo		[] PROGMEM = "GetStorageInfo";
const char msgGetNumObjects			[] PROGMEM = "GetNumObjects";
const char msgGetObjectHandles		[] PROGMEM = "GetObjectHandles";
const char msgGetObjectInfo			[] PROGMEM = "GetObjectInfo";
const char msgGetObject				[] PROGMEM = "GetObject";
const char msgGetThumb				[] PROGMEM = "GetThumb";
const char msgDeleteObject			[] PROGMEM = "DeleteObject";
const char msgSendObjectInfo		[] PROGMEM = "SendObjectInfo";
const char msgSendObject			[] PROGMEM = "SendObject";
const char msgInitiateCapture		[] PROGMEM = "InitiateCapture";
const char msgFormatStore			[] PROGMEM = "FormatStore";
const char msgResetDevice			[] PROGMEM = "ResetDevice";
const char msgSelfTest				[] PROGMEM = "SelfTest";
const char msgSetObjectProtection	[] PROGMEM = "SetObjectProtection";
const char msgPowerDown				[] PROGMEM = "PowerDown";
const char msgGetDevicePropDesc		[] PROGMEM = "GetDevicePropDesc";
const char msgGetDevicePropValue	[] PROGMEM = "GetDevicePropValue";
const char msgSetDevicePropValue	[] PROGMEM = "SetDevicePropValue";
const char msgResetDevicePropValue	[] PROGMEM = "ResetDevicePropValue";
const char msgTerminateOpenCapture	[] PROGMEM = "TerminateOpenCapture";
const char msgMoveObject			[] PROGMEM = "MoveObject";
const char msgCopyObject			[] PROGMEM = "CopyObject";
const char msgGetPartialObject		[] PROGMEM = "GetPartialObject";
const char msgInitiateOpenCapture	[] PROGMEM = "InitiateOpenCapture";

// MTP Operations
const char msgGetObjectPropsSupported		[] PROGMEM = "GetObjectPropsSupported";
const char msgGetObjectPropDesc				[] PROGMEM = "GetObjectPropDesc";
const char msgGetObjectPropValue			[] PROGMEM = "GetObjectPropValue";
const char msgSetObjectPropValue			[] PROGMEM = "SetObjectPropValue";
const char msgGetObjectPropList				[] PROGMEM = "GetObjectPropList";
const char msgSetObjectPropList				[] PROGMEM = "SetObjectPropList";
const char msgGetInterdependentPropDesc		[] PROGMEM = "GetInterdependentPropDesc";
const char msgSendObjectPropList			[] PROGMEM = "SendObjectPropList";
const char msgGetObjectReferences			[] PROGMEM = "GetObjectReferences";
const char msgSetObjectReferences			[] PROGMEM = "SetObjectReferences";
const char msgSkip							[] PROGMEM = "Skip";

// Canon EOS Operations
const char msgEOS_GetStorageIDs				[] PROGMEM = "EOS_GetStorageIDs";
const char msgEOS_GetStorageInfo			[] PROGMEM = "EOS_GetStorageInfo";
const char msgEOS_GetObject					[] PROGMEM = "EOS_GetObject";
const char msgEOS_GetDeviceInfo				[] PROGMEM = "EOS_GetDeviceInfo";
const char msgEOS_GetObjectIDs				[] PROGMEM = "EOS_GetObjectIDs";
const char msgEOS_Capture					[] PROGMEM = "EOS_Capture";
const char msgEOS_SetDevicePropValue		[] PROGMEM = "EOS_SetDevicePropValue";
const char msgEOS_SetPCConnectMode			[] PROGMEM = "EOS_SetPCConnectMode";
const char msgEOS_SetExtendedEventInfo		[] PROGMEM = "EOS_SetExtendedEventInfo";
const char msgEOS_GetEvent					[] PROGMEM = "EOS_GetEvent";
const char msgEOS_GetLiveViewPicture		[] PROGMEM = "EOS_GetLiveViewPicture";
const char msgEOS_MoveFocus					[] PROGMEM = "EOS_MoveFocus";

// Canon PowerShot Operations
const char msgPS_GetObjectSize				[] PROGMEM = "PS_GetObjectSize";
const char msgPS_StartShootingMode			[] PROGMEM = "PS_StartShootingMode";
const char msgPS_EndShootingMode			[] PROGMEM = "PS_EndShootingMode";
const char msgPS_ViewfinderOn				[] PROGMEM = "PS_ViewfinderOn";
const char msgPS_ViewfinderOff				[] PROGMEM = "PS_ViewfinderOff";
const char msgPS_ReflectChanges				[] PROGMEM = "PS_ReflectChanges";
const char msgPS_CheckEvent					[] PROGMEM = "PS_CheckEvent";
const char msgPS_FocusLock					[] PROGMEM = "PS_FocusLock";
const char msgPS_FocusUnlock				[] PROGMEM = "PS_FocusUnlock";
const char msgPS_InitiateCaptureInMemory	[] PROGMEM = "PS_InitiateCaptureInMemory";
const char msgPS_GetPartialObject			[] PROGMEM = "PS_GetPartialObject";
const char msgPS_GetViewfinderImage			[] PROGMEM = "PS_GetViewfinderImage";
const char msgPS_GetChanges					[] PROGMEM = "PS_GetChanges";
const char msgPS_GetFolderEntries			[] PROGMEM = "PS_GetFolderEntries";


// PTP Events
const char msgCancelTransaction		[] PROGMEM = "CancelTransaction";
const char msgObjectAdded			[] PROGMEM = "ObjectAdded";
const char msgObjectRemoved			[] PROGMEM = "ObjectRemoved";
const char msgStoreAdded			[] PROGMEM = "StoreAdded";
const char msgStoreRemoved			[] PROGMEM = "StoreRemoved";
const char msgDevicePropChanged		[] PROGMEM = "DevicePropChanged";
const char msgObjectInfoChanged		[] PROGMEM = "ObjectInfoChanged";
const char msgDeviceInfoChanged		[] PROGMEM = "DeviceInfoChanged";
const char msgRequestObjectTransfer	[] PROGMEM = "RequestObjectTransfer";
const char msgStoreFull				[] PROGMEM = "StoreFull";
const char msgDeviceReset			[] PROGMEM = "DeviceReset";
const char msgStorageInfoChanged	[] PROGMEM = "StorageInfoChanged";
const char msgCaptureComplete		[] PROGMEM = "CaptureComplete";
const char msgUnreportedStatus		[] PROGMEM = "UnreportedStatus";

// MTP Events
const char msgObjectPropChanged			[] PROGMEM = "ObjectPropChanged";
const char msgObjectPropDescChanged		[] PROGMEM = "ObjectPropDescChanged";
const char msgObjectReferencesChanged	[] PROGMEM = "ObjectReferencesChanged";

// PTP Device properties
const char msgBatteryLevel				[] PROGMEM = "BatteryLevel";
const char msgFunctionalMode			[] PROGMEM = "FunctionalMode";
const char msgImageSize					[] PROGMEM = "ImageSize";
const char msgCompressionSetting		[] PROGMEM = "CompressionSetting";
const char msgWhiteBalance				[] PROGMEM = "WhiteBalance";
const char msgRGBGain					[] PROGMEM = "RGBGain";
const char msgFNumber					[] PROGMEM = "FNumber";
const char msgFocalLength				[] PROGMEM = "FocalLength";
const char msgFocusDistance				[] PROGMEM = "FocusDistance";
const char msgFocusMode					[] PROGMEM = "FocusMode";
const char msgExposureMeteringMode		[] PROGMEM = "ExposureMeteringMode";
const char msgFlashMode					[] PROGMEM = "FlashMode";
const char msgExposureTime				[] PROGMEM = "ExposureTime";
const char msgExposureProgramMode		[] PROGMEM = "ExposureProgramMode";
const char msgExposureIndex				[] PROGMEM = "ExposureIndex";
const char msgExposureBiasCompensation	[] PROGMEM = "ExposureBiasCompensation";
const char msgDateTime					[] PROGMEM = "DateTime";
const char msgCaptureDelay				[] PROGMEM = "CaptureDelay";
const char msgStillCaptureMode			[] PROGMEM = "StillCaptureMode";
const char msgContrast					[] PROGMEM = "Contrast";
const char msgSharpness					[] PROGMEM = "Sharpness";
const char msgDigitalZoom				[] PROGMEM = "DigitalZoom";
const char msgEffectMode				[] PROGMEM = "EffectMode";
const char msgBurstNumber				[] PROGMEM = "BurstNumber";
const char msgBurstInterval				[] PROGMEM = "BurstInterval";
const char msgTimelapseNumber			[] PROGMEM = "TimelapseNumber";
const char msgTimelapseInterval			[] PROGMEM = "TimelapseInterval";
const char msgFocusMeteringMode			[] PROGMEM = "FocusMeteringMode";
const char msgUploadURL					[] PROGMEM = "UploadURL";
const char msgArtist					[] PROGMEM = "Artist";
const char msgCopyrightInfo				[] PROGMEM = "CopyrightInfo";

// MTP Device properties
const char msgSynchronization_Partner				[] PROGMEM = "Synchronization_Partner";
const char msgDevice_Friendly_Name					[] PROGMEM = "Device_Friendly_Name";
const char msgVolume								[] PROGMEM = "Volume";
const char msgSupportedFormatsOrdered				[] PROGMEM = "SupportedFormatsOrdered";
const char msgDeviceIcon							[] PROGMEM = "DeviceIcon";
const char msgSession_Initiator_Version_Info		[] PROGMEM = "Session_Initiator_Version_Info";
const char msgPerceived_Device_Type					[] PROGMEM = "Perceived_Device_Type";
const char msgPlayback_Rate							[] PROGMEM = "Playback_Rate";
const char msgPlayback_Object						[] PROGMEM = "Playback_Object";
const char msgPlayback_Container					[] PROGMEM = "Playback_Container";

// Ancillary formats
const char msgAssociation	[] PROGMEM = "Association";
const char msgScript		[] PROGMEM = "Script";
const char msgExecutable	[] PROGMEM = "Executable";
const char msgText			[] PROGMEM = "Text";
const char msgHTML			[] PROGMEM = "HTML";
const char msgDPOF			[] PROGMEM = "DPOF";
const char msgAIFF	 		[] PROGMEM = "AIFF";
const char msgWAV			[] PROGMEM = "WAV";
const char msgMP3			[] PROGMEM = "MP3";
const char msgAVI			[] PROGMEM = "AVI";
const char msgMPEG			[] PROGMEM = "MPEG";
const char msgASF			[] PROGMEM = "ASF";
const char msgQT			[] PROGMEM = "QT";

// Image formats
const char msgEXIF_JPEG			[] PROGMEM = "EXIF_JPEG";
const char msgTIFF_EP			[] PROGMEM = "TIFF_EP";
const char msgFlashPix			[] PROGMEM = "FlashPix";
const char msgBMP				[] PROGMEM = "BMP";
const char msgCIFF				[] PROGMEM = "CIFF";
const char msgUndefined_0x3806	[] PROGMEM = "Undefined_0x3806";
const char msgGIF				[] PROGMEM = "GIF";
const char msgJFIF				[] PROGMEM = "JFIF";
const char msgPCD				[] PROGMEM = "PCD";
const char msgPICT				[] PROGMEM = "PICT";
const char msgPNG				[] PROGMEM = "PNG";
const char msgUndefined_0x380C	[] PROGMEM = "Undefined_0x380C";
const char msgTIFF				[] PROGMEM = "TIFF";
const char msgTIFF_IT			[] PROGMEM = "TIFF_IT";
const char msgJP2				[] PROGMEM = "JP2";
const char msgJPX				[] PROGMEM = "JPX";

// MTP Object Formats
const char msgUndefined_Firmware				[] PROGMEM = "Undefined_Firmware";
const char msgWindows_Image_Format				[] PROGMEM = "Windows_Image_Format";
const char msgUndefined_Audio					[] PROGMEM = "Undefined_Audio";
const char msgWMA								[] PROGMEM = "WMA";
const char msgOGG								[] PROGMEM = "OGG";
const char msgAAC								[] PROGMEM = "AAC";
const char msgAudible							[] PROGMEM = "Audible";
const char msgFLAC								[] PROGMEM = "FLAC";
const char msgUndefined_Video					[] PROGMEM = "Undefined_Video";
const char msgWMV								[] PROGMEM = "WMV";
const char msgMP4_Container						[] PROGMEM = "MP4_Container";
const char msgMP2								[] PROGMEM = "MP2";
const char msg3GP_Container						[] PROGMEM = "3GP_Container";


#endif // __PTPCONSTITLES_H__
