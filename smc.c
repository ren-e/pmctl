/*
 * Copyright (c) 2022 René Ammerlaan <>
 *
 * Permission to use, copy, modify, and distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <CoreFoundation/CoreFoundation.h>
#include <IOKit/IOKitLib.h>
#include <IOKit/pwr_mgt/IOPM.h>
#include <err.h>

#include "pmctl.h"

static int 
smc_key_info(uint32_t key, SMCParamStruct *src, SMCParamStruct *dst)
{
	IOReturn	r;

	src->key	= key;
	src->data8	= kSMCGetKeyInfo;

	r = callSMCFunction(kSMCHandleYPCEvent, src, dst);
	if (r != kSMCSuccess)
		errx(1, "Caught exception in read call");

	if (dst->result == kSMCKeyNotFound) {
		fprintf(stderr, "Cannot find key\n");
		return (-1);
	}
	else if (dst->result != kSMCSuccess)
		errx(1, "Internal error");

	return (0);
}

int
smc_write(uint32_t key, uint8_t value)
{
	IOReturn	r;
	SMCParamStruct	smc[2];

	bzero(&smc, sizeof(SMCParamStruct) * 2);

	if (smc_key_info(key, &smc[0], &smc[1]) != 0)
		errx(1, "Cannot retrieve SMC key info");

	smc[0].key		= key;
	smc[0].data8		= kSMCWriteKey;
	smc[0].keyInfo.dataSize	= smc[1].keyInfo.dataSize;
	smc[0].bytes[0]		= value;
	bzero(&smc[1], sizeof(SMCParamStruct));

	r = callSMCFunction(kSMCHandleYPCEvent, &smc[0], &smc[1]);
	if (r != kSMCSuccess)
		err(1, "Caught exception in write call");

	return (0);

}

int
smc_read(uint32_t key) {
	IOReturn	r;
	SMCParamStruct	smc[2];

	bzero(&smc, sizeof(SMCParamStruct) * 2);
	if (smc_key_info(key, &smc[0], &smc[1]) != 0)
		return (-1);

	smc[0].key		= key;
	smc[0].data8		= kSMCReadKey;
	smc[0].keyInfo.dataSize	= smc[1].keyInfo.dataSize;
	bzero(&smc[1], sizeof(SMCParamStruct));

	r = callSMCFunction(kSMCHandleYPCEvent, &smc[0], &smc[1]);
	if (r != kSMCSuccess || smc[1].result != 0)
		errx(1, "Caught exception in call\n");

	switch (smc[0].key) {
		case smcBatteryMax:	
			printf("smcBatteryMax		%d%%\n",
			    smc[1].bytes[0]);
			break;
		case smcBatteryChargingApple:
		case smcBatteryChargingIntel:
			printf("smcBatteryCharging	%s\n",
			    smc[1].bytes[0] == 0 ? "enabled" : "disabled");
			break;
		case smcDisableInflow:
			printf("smcDisableInflow	%s\n",
			    smc[1].bytes[0] == 0 ? "disabled" : "enabled");
			break;
		default:
			printf("Unknown key type\n");
			break;
	}
	return (0);
}

IOReturn
callSMCFunction(int dsc, SMCParamStruct *src, SMCParamStruct *dst)
{
	IOReturn	r = kIOReturnError;
	io_connect_t	_SMCConnect = IO_OBJECT_NULL;
	io_service_t	smc = IO_OBJECT_NULL;
	size_t		sz = sizeof(SMCParamStruct);

	smc = IOServiceGetMatchingService(kIOMainPortDefault,
	    IOServiceMatching("AppleSMC"));

	if (smc == IO_OBJECT_NULL)
		return (kIOReturnNotFound);


	r = IOServiceOpen(smc, mach_task_self(), 1, &_SMCConnect);        
	if (r != kIOReturnSuccess || _SMCConnect == IO_OBJECT_NULL) {
		_SMCConnect = IO_OBJECT_NULL;
		goto done;
	}

	r = IOConnectCallStructMethod(_SMCConnect, dsc,
	    src, sizeof(SMCParamStruct), dst, &sz);

done:
	if (_SMCConnect != IO_OBJECT_NULL) {
		IOConnectCallMethod(_SMCConnect, kSMCUserClientClose, NULL, 0,
		   NULL, 0, NULL, NULL, NULL, NULL);
		IOServiceClose(_SMCConnect);    
	}

	return (r);
}
