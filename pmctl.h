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

#include <IOKit/ps/IOPowerSources.h>
#include <IOKit/ps/IOPSKeys.h>
#include <IOKit/pwr_mgt/IOPMLib.h>
#include <IOKit/IOTypes.h>
#include "private.h"

#define smcBatteryMax			'BCLM'
#define smcBatteryChargingIntel		'CH0B'
#define smcBatteryChargingAppleLegacy	'CH0C'
#define smcDisableInflowLegacy		'CH0I'
#define smcBatteryChargingApple		'CHTE'
#define smcDisableInflow		'CHIE'

#define smcDisableInflowValueLegacy	1
#define smcDisableInflowValueLatest	8


struct options
{       
        int		read;
        uint8_t		batteryMax;
        uint8_t		batteryCharging;
        uint8_t		inflowDisable;
	cpu_type_t	cputype;
};

struct osversion
{
	int	majorVersion;
	int	minorVersion;
	int	patchVersion;
};

/* smc.c */
int		smc_write(uint32_t, uint8_t);
int		smc_read(uint32_t);
IOReturn	callSMCFunction(int, SMCParamStruct *, SMCParamStruct *);

/* assert.c */
IOPMAssertionID		assert_create(CFStringRef, const char *);
int			assert_inflow_disable(uint8_t);

/* os.m */
struct osversion	get_os_version();

/* power.c */
int			pwr_battery_soc(int);

/* util.c */
cpu_type_t		get_cpu_arch();
