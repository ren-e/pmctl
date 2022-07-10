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

#include "pmctl.h"

int
pwr_battery_soc(int verbose)
{
	int		i;
	int32_t		max = 0;
	int32_t		cur = 0;
	int		soc = -1;
	CFTypeRef	inf = IOPSCopyPowerSourcesInfo();
	CFArrayRef	pwr = IOPSCopyPowerSourcesList(inf);
	CFDictionaryRef psrc;
	CFNumberRef	val;

	for (i=0; i < CFArrayGetCount(pwr); i++) {
		psrc = IOPSGetPowerSourceDescription(inf, CFArrayGetValueAtIndex(pwr, i));

		val = CFDictionaryGetValue(psrc, CFSTR(kIOPSCurrentCapacityKey));
		CFNumberGetValue(val, kCFNumberSInt32Type, &cur);
		val = CFDictionaryGetValue(psrc, CFSTR(kIOPSMaxCapacityKey));
		CFNumberGetValue(val, kCFNumberSInt32Type, &max);

		soc = (int)((double)cur/(double)max * 100);
		if (verbose)
			printf("Battery SoC: %d%%\n", soc);
		break;
	}

	if (pwr)
		CFRelease(pwr);
	if (inf)
		CFRelease(inf);

	return (soc);
}
