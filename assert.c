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

#include <CoreFoundation/CFArray.h>
#include <IOKit/IOKitLib.h>
#include <IOKit/pwr_mgt/IOPMLib.h>
#include <IOKit/pwr_mgt/IOPM.h>

#include <signal.h>
#include <stdio.h>

#include "pmctl.h"

static int	abortsignal = 0;

static void                                                                     
abort_trap()                                                                    
{
	printf("Caught SIGINT signal...\n");
	abortsignal = 1;
}

int
assert_inflow_disable(uint8_t percentage)
{
	int			r = -1;
	IOPMAssertionID		id = 0;

	if (pwr_battery_soc(1) <= percentage) {
		printf("Battery SoC is equal or lower than configured...\n");
		goto done;
	}

	/* Trap CTRL+C */
	signal(SIGINT, abort_trap);
	printf("Disabled charging until battery SoC reaches %d%%, "
	    "press CTRL+C to cancel...\n", percentage);

	if (!(id = assert_create(kIOPMInflowDisableAssertion, "DisableInflow")))
		goto done;

	while (abortsignal == 0 && pwr_battery_soc(1) > percentage) {
		sleep(60);
	}
	r = 0;
done:
	if (id) {
		IOPMAssertionRelease(id);
		printf("\n");
	}

	return (r);
}


IOPMAssertionID
assert_create(CFStringRef type, const char *str)
{

	IOReturn		r;
	IOPMAssertionID		id = 0;
	CFStringRef		name = CFStringCreateWithCString(NULL, str,
	    kCFStringEncodingUTF8);

	if ((r = IOPMAssertionCreateWithName(type, kIOPMAssertionLevelOn, name,
	    &id)) != kIOReturnSuccess)
		goto done;

	if (id)
		printf("IOPMAssertionID: %d (%s)\n", id, str);
done:
	return (id);
}
