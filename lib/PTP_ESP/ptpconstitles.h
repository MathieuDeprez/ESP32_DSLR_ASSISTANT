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
#include "ptp.h"

const char msgVendorDefined			[]  = "Vendor defined";
const char msgTab					[]  = "\t";
const char msgCRLF					[]  = "\r\n";

// PTP Operations
const char msgUndefined				[]  = "Undefined";
const char msgGetDeviceInfo			[]  = "GetDeviceInfo";
const char msgOpenSession			[]  = "OpenSession";
const char msgCloseSession			[]  = "CloseSession";
const char msgGetStorageIDs			[]  = "GetStorageIDs";
const char msgGetStorageInfo		[]  = "GetStorageInfo";
const char msgGetNumObjects			[]  = "GetNumObjects";
const char msgGetObjectHandles		[]  = "GetObjectHandles";
const char msgGetObjectInfo			[]  = "GetObjectInfo";
const char msgGetObject				[]  = "GetObject";
const char msgGetThumb				[]  = "GetThumb";
const char msgDeleteObject			[]  = "DeleteObject";
const char msgSendObjectInfo		[]  = "SendObjectInfo";
const char msgSendObject			[]  = "SendObject";
const char msgInitiateCapture		[]  = "InitiateCapture";
const char msgFormatStore			[]  = "FormatStore";
const char msgResetDevice			[]  = "ResetDevice";
const char msgSelfTest				[]  = "SelfTest";
const char msgSetObjectProtection	[]  = "SetObjectProtection";
const char msgPowerDown				[]  = "PowerDown";
const char msgGetDevicePropDesc		[]  = "GetDevicePropDesc";
const char msgGetDevicePropValue	[]  = "GetDevicePropValue";
const char msgSetDevicePropValue	[]  = "SetDevicePropValue";
const char msgResetDevicePropValue	[]  = "ResetDevicePropValue";
const char msgTerminateOpenCapture	[]  = "TerminateOpenCapture";
const char msgMoveObject			[]  = "MoveObject";
const char msgCopyObject			[]  = "CopyObject";
const char msgGetPartialObject		[]  = "GetPartialObject";
const char msgInitiateOpenCapture	[]  = "InitiateOpenCapture";

// MTP Operations
const char msgGetObjectPropsSupported		[]  = "GetObjectPropsSupported";
const char msgGetObjectPropDesc				[]  = "GetObjectPropDesc";
const char msgGetObjectPropValue			[]  = "GetObjectPropValue";
const char msgSetObjectPropValue			[]  = "SetObjectPropValue";
const char msgGetObjectPropList				[]  = "GetObjectPropList";
const char msgSetObjectPropList				[]  = "SetObjectPropList";
const char msgGetInterdependentPropDesc		[]  = "GetInterdependentPropDesc";
const char msgSendObjectPropList			[]  = "SendObjectPropList";
const char msgGetObjectReferences			[]  = "GetObjectReferences";
const char msgSetObjectReferences			[]  = "SetObjectReferences";
const char msgSkip							[]  = "Skip";

// Canon EOS Operations
const char msgEOS_GetStorageIDs				[]  = "EOS_GetStorageIDs";
const char msgEOS_GetStorageInfo			[]  = "EOS_GetStorageInfo";
const char msgEOS_GetObject					[]  = "EOS_GetObject";
const char msgEOS_GetDeviceInfo				[]  = "EOS_GetDeviceInfo";
const char msgEOS_GetObjectIDs				[]  = "EOS_GetObjectIDs";
const char msgEOS_Capture					[]  = "EOS_Capture";
const char msgEOS_SetDevicePropValue		[]  = "EOS_SetDevicePropValue";
const char msgEOS_SetPCConnectMode			[]  = "EOS_SetPCConnectMode";
const char msgEOS_SetExtendedEventInfo		[]  = "EOS_SetExtendedEventInfo";
const char msgEOS_GetEvent					[]  = "EOS_GetEvent";
const char msgEOS_GetLiveViewPicture		[]  = "EOS_GetLiveViewPicture";
const char msgEOS_MoveFocus					[]  = "EOS_MoveFocus";

// Canon PowerShot Operations
const char msgPS_GetObjectSize				[]  = "PS_GetObjectSize";
const char msgPS_StartShootingMode			[]  = "PS_StartShootingMode";
const char msgPS_EndShootingMode			[]  = "PS_EndShootingMode";
const char msgPS_ViewfinderOn				[]  = "PS_ViewfinderOn";
const char msgPS_ViewfinderOff				[]  = "PS_ViewfinderOff";
const char msgPS_ReflectChanges				[]  = "PS_ReflectChanges";
const char msgPS_CheckEvent					[]  = "PS_CheckEvent";
const char msgPS_FocusLock					[]  = "PS_FocusLock";
const char msgPS_FocusUnlock				[]  = "PS_FocusUnlock";
const char msgPS_InitiateCaptureInMemory	[]  = "PS_InitiateCaptureInMemory";
const char msgPS_GetPartialObject			[]  = "PS_GetPartialObject";
const char msgPS_GetViewfinderImage			[]  = "PS_GetViewfinderImage";
const char msgPS_GetChanges					[]  = "PS_GetChanges";
const char msgPS_GetFolderEntries			[]  = "PS_GetFolderEntries";


// PTP Events
const char msgCancelTransaction		[]  = "CancelTransaction";
const char msgObjectAdded			[]  = "ObjectAdded";
const char msgObjectRemoved			[]  = "ObjectRemoved";
const char msgStoreAdded			[]  = "StoreAdded";
const char msgStoreRemoved			[]  = "StoreRemoved";
const char msgDevicePropChanged		[]  = "DevicePropChanged";
const char msgObjectInfoChanged		[]  = "ObjectInfoChanged";
const char msgDeviceInfoChanged		[]  = "DeviceInfoChanged";
const char msgRequestObjectTransfer	[]  = "RequestObjectTransfer";
const char msgStoreFull				[]  = "StoreFull";
const char msgDeviceReset			[]  = "DeviceReset";
const char msgStorageInfoChanged	[]  = "StorageInfoChanged";
const char msgCaptureComplete		[]  = "CaptureComplete";
const char msgUnreportedStatus		[]  = "UnreportedStatus";

// MTP Events
const char msgObjectPropChanged			[]  = "ObjectPropChanged";
const char msgObjectPropDescChanged		[]  = "ObjectPropDescChanged";
const char msgObjectReferencesChanged	[]  = "ObjectReferencesChanged";

// PTP Device properties
const char msgBatteryLevel				[]  = "BatteryLevel";
const char msgFunctionalMode			[]  = "FunctionalMode";
const char msgImageSize					[]  = "ImageSize";
const char msgCompressionSetting		[]  = "CompressionSetting";
const char msgWhiteBalance				[]  = "WhiteBalance";
const char msgRGBGain					[]  = "RGBGain";
const char msgFNumber					[]  = "FNumber";
const char msgFocalLength				[]  = "FocalLength";
const char msgFocusDistance				[]  = "FocusDistance";
const char msgFocusMode					[]  = "FocusMode";
const char msgExposureMeteringMode		[]  = "ExposureMeteringMode";
const char msgFlashMode					[]  = "FlashMode";
const char msgExposureTime				[]  = "ExposureTime";
const char msgExposureProgramMode		[]  = "ExposureProgramMode";
const char msgExposureIndex				[]  = "ExposureIndex";
const char msgExposureBiasCompensation	[]  = "ExposureBiasCompensation";
const char msgDateTime					[]  = "DateTime";
const char msgCaptureDelay				[]  = "CaptureDelay";
const char msgStillCaptureMode			[]  = "StillCaptureMode";
const char msgContrast					[]  = "Contrast";
const char msgSharpness					[]  = "Sharpness";
const char msgDigitalZoom				[]  = "DigitalZoom";
const char msgEffectMode				[]  = "EffectMode";
const char msgBurstNumber				[]  = "BurstNumber";
const char msgBurstInterval				[]  = "BurstInterval";
const char msgTimelapseNumber			[]  = "TimelapseNumber";
const char msgTimelapseInterval			[]  = "TimelapseInterval";
const char msgFocusMeteringMode			[]  = "FocusMeteringMode";
const char msgUploadURL					[]  = "UploadURL";
const char msgArtist					[]  = "Artist";
const char msgCopyrightInfo				[]  = "CopyrightInfo";

// MTP Device properties
const char msgSynchronization_Partner				[]  = "Synchronization_Partner";
const char msgDevice_Friendly_Name					[]  = "Device_Friendly_Name";
const char msgVolume								[]  = "Volume";
const char msgSupportedFormatsOrdered				[]  = "SupportedFormatsOrdered";
const char msgDeviceIcon							[]  = "DeviceIcon";
const char msgSession_Initiator_Version_Info		[]  = "Session_Initiator_Version_Info";
const char msgPerceived_Device_Type					[]  = "Perceived_Device_Type";
const char msgPlayback_Rate							[]  = "Playback_Rate";
const char msgPlayback_Object						[]  = "Playback_Object";
const char msgPlayback_Container					[]  = "Playback_Container";

// Ancillary formats
const char msgAssociation	[]  = "Association";
const char msgScript		[]  = "Script";
const char msgExecutable	[]  = "Executable";
const char msgText			[]  = "Text";
const char msgHTML			[]  = "HTML";
const char msgDPOF			[]  = "DPOF";
const char msgAIFF	 		[]  = "AIFF";
const char msgWAV			[]  = "WAV";
const char msgMP3			[]  = "MP3";
const char msgAVI			[]  = "AVI";
const char msgMPEG			[]  = "MPEG";
const char msgASF			[]  = "ASF";
const char msgQT			[]  = "QT";

// Image formats
const char msgEXIF_JPEG			[]  = "EXIF_JPEG";
const char msgTIFF_EP			[]  = "TIFF_EP";
const char msgFlashPix			[]  = "FlashPix";
const char msgBMP				[]  = "BMP";
const char msgCIFF				[]  = "CIFF";
const char msgUndefined_0x3806	[]  = "Undefined_0x3806";
const char msgGIF				[]  = "GIF";
const char msgJFIF				[]  = "JFIF";
const char msgPCD				[]  = "PCD";
const char msgPICT				[]  = "PICT";
const char msgPNG				[]  = "PNG";
const char msgUndefined_0x380C	[]  = "Undefined_0x380C";
const char msgTIFF				[]  = "TIFF";
const char msgTIFF_IT			[]  = "TIFF_IT";
const char msgJP2				[]  = "JP2";
const char msgJPX				[]  = "JPX";

// MTP Object Formats
const char msgUndefined_Firmware				[]  = "Undefined_Firmware";
const char msgWindows_Image_Format				[]  = "Windows_Image_Format";
const char msgUndefined_Audio					[]  = "Undefined_Audio";
const char msgWMA								[]  = "WMA";
const char msgOGG								[]  = "OGG";
const char msgAAC								[]  = "AAC";
const char msgAudible							[]  = "Audible";
const char msgFLAC								[]  = "FLAC";
const char msgUndefined_Video					[]  = "Undefined_Video";
const char msgWMV								[]  = "WMV";
const char msgMP4_Container						[]  = "MP4_Container";
const char msgMP2								[]  = "MP2";
const char msg3GP_Container						[]  = "3GP_Container";


#endif // __PTPCONSTITLES_H__
