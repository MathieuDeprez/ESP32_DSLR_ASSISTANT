#ifndef __DEVPROPPARSER_H__
#define __DEVPROPPARSER_H__

#include <ptpconst.h>
#include <ptp.h>
#include <Usb.h>

// Device properties
const char msgUndefined					[]  = "Undefined";
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

// Data Types
const char msgUNDEF		[]  = "UNDEF";
const char msgINT8		[]  = "INT8";
const char msgUINT8		[]  = "UINT8";
const char msgINT16		[]  = "INT16";
const char msgUINT16	[]  = "UINT16";
const char msgINT32		[]  = "INT32";
const char msgUINT32	[]  = "UINT32";
const char msgINT64		[]  = "INT64";
const char msgUINT64	[]  = "UINT64";
const char msgINT128	[]  = "INT128";
const char msgUINT128	[]  = "UINT128";
const char msgAINT8		[]  = "AINT8";
const char msgAUINT8	[]  = "AUINT8";
const char msgAINT16	[]  = "AINT16";
const char msgAUINT16	[]  = "AUINT16";
const char msgAINT32	[]  = "AINT32";
const char msgAUINT32	[]  = "AUINT32";
const char msgAINT64	[]  = "AINT64";
const char msgAUINT64	[]  = "AUINT64";
const char msgAINT128	[]  = "AINT128";
const char msgAUINT128	[]  = "AUINT128";
const char msgSTR	[]  = "STR";


class DevPropParser : public PTPReadParser
{
	static const char* const dtNames1[];
	static const char* const dtNames2[];
	static const char* const prNames[];

	uint8_t				nStage;
        uint8_t                         enStage;
	uint16_t			dataType;
        uint8_t				formFlag;
        uint8_t				bytesSize;
        MultiValueBuffer	        theBuffer;
        uint8_t				varBuffer[16];
        uint16_t                        enLen;
        uint16_t                        enLenCntdn;

	MultiByteValueParser		valParser;

	void PrintDevProp	(uint8_t **pp, uint16_t *pcntdn);
	void PrintDataType	(uint8_t **pp, uint16_t *pcntdn);
	void PrintGetSet	(uint8_t **pp, uint16_t *pcntdn);
	bool PrintValue		(uint8_t **pp, uint16_t *pcntdn);
	bool PrintEnum		(uint8_t **pp, uint16_t *pcntdn);
	bool ParseEnum		(uint8_t **pp, uint16_t *pcntdn);
	bool ParseEnumSingle	(uint8_t **pp, uint16_t *pcntdn);
	bool ParseEnumArray	(uint8_t **pp, uint16_t *pcntdn);

	static void PrintChar(const MultiValueBuffer * const p,
            uint32_t count __attribute__((unused)),
            const void *me __attribute__((unused)))
	{
		if (((unsigned char*)p->pValue)[0])
			Serial.print(((unsigned char*)p->pValue)[0]);
	};
	static void PrintByte(const MultiValueBuffer * const p, uint32_t count,
            const void *me __attribute__((unused)))
	{
		if (count)
			E_Notify(PSTR(","), 0x80);
		PrintHex<uint8_t>(((uint8_t*)p->pValue)[0], 0x80);
	};
	static void PrintTwoBytes(const MultiValueBuffer * const p,
            uint32_t count,
            const void *me __attribute__((unused)))
	{
		if (count)
			E_Notify(PSTR(","), 0x80);
		PrintHex<uint16_t>(((uint16_t*)p->pValue)[0], 0x80);
	};
	static void PrintFourBytes(const MultiValueBuffer * const p,
            uint32_t count, const void *me __attribute__((unused)))
	{
		if (count)
			E_Notify(PSTR(","), 0x80);
		PrintHex<uint32_t>(((uint32_t*)p->pValue)[0], 0x80);
	};
	static void PrintEightBytes(const MultiValueBuffer * const p,
            uint32_t count, const void *me __attribute__((unused)))
	{
		if (count)
			E_Notify(PSTR(","), 0x80);
    		for (uint8_t i=p->valueSize; i; i--)
    	  	    PrintHex<uint8_t>(((uint8_t*)p->pValue)[i-1], 0x80);
	};
	static void PrintEnumValue(const MultiValueBuffer * const p,
            uint32_t count, const void *me __attribute__((unused)))
        {
            if (count)
                Serial.print(", ");

            switch (((MultiValueBuffer*)p)->valueSize)
            {
            case 1:
    		PrintHex<uint8_t>(*((uint8_t*)p->pValue), 0x80);
                break;
            case 2:
    		PrintHex<uint16_t>(*((uint16_t*)p->pValue), 0x80);
                break;
            case 4:
    		PrintHex<uint32_t>(*((uint32_t*)p->pValue), 0x80);
                break;
    	    default:
    		for (uint8_t i=p->valueSize; i; i--)
    	  	    PrintHex<uint8_t>(((uint8_t*)p->pValue)[i-1], 0x80);
            }
            count++;
        };

        PTPListParser      enumParser;

        uint8_t GetDataSize();

public:
	DevPropParser() :
		nStage(0),
		enStage(0),
		dataType(0),
		formFlag(0),
		bytesSize(0),
                enLen(0),
                enLenCntdn(0)
	{
		theBuffer.pValue = varBuffer;
	}
	virtual void Parse(const uint16_t len, const uint8_t * const pbuf, const uint32_t &offset);
};

#endif // __DEVPROPPARSER_H__