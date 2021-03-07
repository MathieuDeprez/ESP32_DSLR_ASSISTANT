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
#ifndef __PTPMSGSTR_H__
#define __PTPMSGSTR_H__

const char MsgErrDeviceConf[]        = "Error configuring device. Returned:";
const char MsgErrProtoConf[]         = "Error configuring boot protocol. Returned:";
const char MsgDeviceInitialized[]    = "Device initialized";

#ifdef PTP_HANDLE_RESPONSES
//const char msgUndefined[] 					= "Undefined";
const char msgOK[] 							= "OK";
const char msgGeneralError[] 				= "GeneralError";
const char msgSessionNotOpen[] 				= "SessionNotOpen";
const char msgInvalidTransactionID[] 		= "InvalidTransactionID";
const char msgOperationNotSupported[] 		= "OperationNotSupported";
const char msgParameterNotSupported[] 		= "ParameterNotSupported";
const char msgIncompleteTransfer[] 			= "IncompleteTransfer";
const char msgInvalidStorageId[] 			= "InvalidStorageId";
const char msgInvalidObjectHandle[] 			= "InvalidObjectHandle";
const char msgDevicePropNotSupported[] 		= "DevicePropNotSupported";
const char msgInvalidObjectFormatCode[] 		= "InvalidObjectFormatCode";
const char msgStoreFull[] 					= "StoreFull";
const char msgObjectWriteProtected[] 		= "ObjectWriteProtected";
const char msgStoreReadOnly[] 				= "StoreReadOnly";
const char msgAccessDenied[] 				= "AccessDenied";
const char msgNoThumbnailPresent[] 			= "NoThumbnailPresent";
const char msgSelfTestFailed[] 				= "SelfTestFailed";
const char msgPartialDeletion[] 				= "PartialDeletion";
const char msgStoreNotAvailable[] 			= "StoreNotAvailable";
const char msgSpecificationByFormatUnsupported [] 	= "SpecificationByFormatUnsupported";
const char msgNoValidObjectInfo[] 			= "NoValidObjectInfo";
const char msgInvalidCodeFormat[] 			= "InvalidCodeFormat";
const char msgUnknownVendorCode[] 			= "UnknownVendorCode";
const char msgCaptureAlreadyTerminated[] 	= "CaptureAlreadyTerminated";
const char msgDeviceBusy[] 					= "DeviceBusy";
const char msgInvalidParentObject[] 			= "InvalidParentObject";
const char msgInvalidDevicePropFormat[] 		= "InvalidDevicePropFormat";
const char msgInvalidDevicePropValue[] 		= "InvalidDevicePropValue";
const char msgInvalidParameter[] 			= "InvalidParameter";
const char msgSessionAlreadyOpened[] 		= "SessionAlreadyOpened";
const char msgTransactionCanceled[] 			= "TransactionCanceled";
const char msgSpecificationOfDestinationUnsupported[] 	= "SpecificationOfDestinationUnsupported";
#endif

#if 0
const char msgUndefined[] 					= "Undefined";
const char msgBatteryLevel[] 				= "BatteryLevel";				
const char msgFunctionalMode[] 				= "FunctionalMode";
const char msgImageSize[] 					= "ImageSize";				
const char msgCompressionSetting[] 			= "CompressionSetting";
const char msgWhiteBalance[] 				= "WhiteBalance";			
const char msgRGBGain[] 						= "RGBGain";
const char msgFNumber[] 						= "FNumber";
const char msgFocalLength[] 					= "FocalLength";
const char msgFocusDistance[] 				= "FocusDistance";
const char msgFocusMode[] 					= "FocusMode";				
const char msgExposureMeteringMode[] 		= "ExposureMeteringMode";
const char msgFlashMode[] 					= "FlashMode";			
const char msgExposureTime[] 				= "ExposureTime";
const char msgExposureProgramMode[] 			= "ExposureProgramMode";
const char msgExposureIndex[] 				= "ExposureIndex";			
const char msgExposureBiasCompensation[] 	= "ExposureBiasCompensation";
const char msgDateTime[] 					= "DateTime";		
const char msgCaptureDelay[] 				= "CaptureDelay";
const char msgStillCaptureMode[] 			= "StillCaptureMode";
const char msgContrast[] 					= "Contrast";
const char msgSharpness[] 					= "Sharpness";
const char msgDigitalZoom[] 					= "DigitalZoom";
const char msgEffectMode[] 					= "EffectMode";
const char msgBurstNumber[] 					= "BurstNumber";
const char msgBurstInterval[] 				= "BurstInterval";
const char msgTimelapseNumber[] 				= "TimelapseNumber";
const char msgTimelapseInterval[] 			= "TimelapseInterval";
const char msgFocusMeteringMode[] 			= "FocusMeteringMode";
const char msgUploadURL[] 					= "UploadURL";
const char msgArtist[] 						= "Artist";
const char msgCopyrightInfo[] 				= "CopyrightInfo";
#endif

#endif //__PTPMSGSTR_H__