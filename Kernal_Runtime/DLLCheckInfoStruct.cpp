#include "pch.h"
#include "DLLCheckInfoStruct.h"

bool DCIS_Equal(const DCIS* val1, const DCIS* val2)
{
	if (val1 == val2) {
		return true;
	}
	if (!strcmp(val1->file,val2->file)) {
		if (!memcmp(val1->md5Value, val2->md5Value, MD5Size) || !memcmp(val1->sha256Value, val2->sha256Value, SHA256Size)) {
			return true;
		}
		else {
			return false;
		}
	}
	else {
		return false;
	}
}