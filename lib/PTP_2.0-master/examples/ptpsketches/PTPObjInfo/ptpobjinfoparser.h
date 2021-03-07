#ifndef __PTPOBJINFOPARSER_H__
#define __PTPOBJINFOPARSER_H__

#include <ptp.h>
#include <mtpconst.h>
#include <ptpcallback.h>

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


/**
 * This structure is used to store ObjectInfo dataset used in ObjectInfo parser
 * as well as SendObjectInfo operation.
 */
typedef struct {

        union {
                uint8_t raw[64];

                struct {
                    uint32_t StorageID;
                    uint16_t ObjectFormat;
                    uint16_t ProtectionStatus;
                    uint32_t ObjectCompressedSize;
                    uint16_t ThumbFormat;
                    uint32_t ThumbCompressedSize;
                    uint32_t ThumbPixWidth;
                    uint32_t ThumbPixHeight;
                    uint32_t ImagePixWidth;
                    uint32_t ImagePixHeight;
                    uint32_t ImageBitDepth;
                    uint32_t ParentObject;
                    uint16_t AssociationType;
                    uint32_t AssociationDescription;
                    uint32_t SequenceNumber;
                    uint8_t FilenameLength;
                    uint16_t* Filename;
                    uint8_t DateCreatedLength;
                    uint16_t* DateCreated;
                    uint8_t DateModifiedLength;
                    uint16_t* DateModified;
                    uint8_t KeywordsLength;
                    uint16_t* Keywords;
                };
        };
} ObjectInfo;

class PTPObjInfoParser : public PTPReadParser
{
	static const char* const acNames[];
	static const char* const imNames[];

	MultiValueBuffer theBuffer;
	uint32_t varBuffer;
	uint8_t	nStage;
        ObjectInfo objectinfo_ds;  // TODO: init

	MultiByteValueParser valueParser;
	PTPListParser arrayParser;

	static void PrintChar(MultiValueBuffer *p)
	{
		if (((unsigned char*)p->pValue)[0])
			Serial.print(((unsigned char*)p->pValue)[0]);
	};
	void PrintFormat(uint16_t op);

public:
	PTPObjInfoParser() : nStage(0) { theBuffer.pValue = (uint8_t*)&varBuffer; };
	virtual void Parse(const uint16_t len, const uint8_t *pbuf, const uint32_t &offset);
        uint8_t CopyObjectInfo(uint8_t* buf);    // copies ObjectInfo dataset
                                                    // into buffer provided
};

#endif // __PTPOBJINFOPARSER_H__
