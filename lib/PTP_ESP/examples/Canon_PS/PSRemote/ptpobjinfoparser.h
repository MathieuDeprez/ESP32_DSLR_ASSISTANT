#ifndef __PTPOBJINFOPARSER_H__
#define __PTPOBJINFOPARSER_H__

#include <ptp.h>
#include <mtpconst.h>

const char msgUndefined		[]  = "Undefined";

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


class PTPObjInfoParser : public PTPReadParser
{
	static const char* acNames[];
	static const char* imNames[];

	MultiValueBuffer				theBuffer;
	uint32_t						varBuffer;
	uint8_t							nStage;

	MultiByteValueParser			valueParser;
	PTPListParser					arrayParser;

	static void PrintChar(MultiValueBuffer *p)
	{
		if (((unsigned char*)p->pValue)[0])
			Serial.print(((unsigned char*)p->pValue)[0]);
	};
	void PrintFormat(uint16_t op);

public:
	PTPObjInfoParser() : nStage(0) { theBuffer.pValue = (uint8_t*)&varBuffer; };
	virtual void Parse(const uint16_t len, const uint8_t *pbuf, const uint32_t &offset);
};

#endif // __PTPOBJINFOPARSER_H__
