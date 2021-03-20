#ifndef __STINFOPARSER_H__
#define __STINFOPARSER_H__

#include <ptpconst.h>
#include <ptp.h>

// Storage Types
const char msgUndefined	[]  = "Undefined";
const char msgFixedROM	[]  = "FixedROM";
const char msgRemovableROM []  = "RemovableROM";
const char msgFixedRAM	[]  = "FixedRAM";
const char msgRemovableRAM []  = "RemovableRAM";

// File System Types
const char msgGenericFlat []  = "GenericFlat";
const char msgGenericHierarchical []  = "GenericHierarchical";
const char msgDCF []  = "DCF";

// Access Capability
const char msgReadWrite []  = "ReadWrite";
const char msgReadOnly	[]  = "ReadOnly";
const char msgReadOnly_with_Object_Deletion []  = "ReadOnly with Object Deletion";


class PTPStorageInfoParser : public PTPReadParser
{
	uint8_t nStage;
	uint8_t	strByteCountDown;

	static const char* const stNames[];
	static const char* const fstNames[];
	static const char* const acNames[];

	void PrintStorageType(uint8_t **pp, uint16_t *pcntdn);
	void PrintFileSystemType(uint8_t **pp, uint16_t *pcntdn);
	void PrintAccessCapability(uint8_t **pp, uint16_t *pcntdn);
	void PrintMaxCapacity(uint8_t **pp, uint16_t *pcntdn);
	void PrintFreeSpaceInBytes(uint8_t **pp, uint16_t *pcntdn);
	void PrintFreeSpaceInImages(uint8_t **pp, uint16_t *pcntdn);
	bool PrintString(uint8_t **pp, uint16_t *pcntdn);

public:
	PTPStorageInfoParser() : nStage(0),	strByteCountDown(0) {};

	virtual void Parse(const uint16_t len, const uint8_t *pbuf, const uint32_t &offset);
};

#endif // __STINFOPARSER_H__
