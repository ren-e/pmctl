/*
 * Copyright (c) 2022 René Ammerlaan <pnnv@icloud.com>
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

#include <IOKit/ps/IOPowerSources.h>
#include <IOKit/ps/IOPSKeys.h>
#include <IOKit/pwr_mgt/IOPMLib.h>
#include <IOKit/IOTypes.h>
#include "private.h"

#define CSTR2INT(s)	(uint32_t)(s[0] << 24 | s[1] << 16 | s[2] << 8 | s[3])

#define smcBatteryMax		CSTR2INT("BCLM")
#define smcBatteryCharging	CSTR2INT("CH0B")

struct options
{       
        int		read;
        uint8_t		batteryMax;
        uint8_t		batteryCharging;
        uint8_t		inflowDisable;
};

/* smc.c */
int		smc_write(uint32_t, uint8_t);
int		smc_read(uint32_t);
IOReturn	callSMCFunction(int, SMCParamStruct *, SMCParamStruct *);

/* assert.c */
IOPMAssertionID		assert_create(CFStringRef, const char *);
int			assert_inflow_disable(uint8_t);

/* power.c */
int			pwr_battery_soc(int);
