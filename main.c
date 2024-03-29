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
#include <err.h>

#include "pmctl.h"


__dead void
usage(void)
{
	extern char	*__progname;

	fprintf(stderr, "usage:\t%s [-r] [-c charge] [-b socmax] "
	    "[-d discharge]\n",
	    __progname);
	exit(1);
}

static int
pmctl_intel(struct options *opt)
{
	if (opt->inflowDisable < 10)
		errx(1, "kIOPMInflowDisableAssertion only supports battery"
		    " level, acceptable range is 10 to 99.");

	if (opt->batteryMax > 0)
		smc_write(smcBatteryMax, opt->batteryMax);

	if (opt->batteryCharging != 0xff)
		smc_write(smcBatteryChargingIntel, opt->batteryCharging);

	if (opt->inflowDisable != 0xff)
		assert_inflow_disable(opt->inflowDisable);

	if (opt->read) {
		pwr_battery_soc(1);
		smc_read(smcBatteryMax);
		smc_read(smcBatteryChargingIntel);
	}

	return (0);
}

static int
pmctl_apple(struct options *opt)
{
	if (opt->batteryMax > 0)
		errx(1, "smcBatteryMax unsupported on Apple Silicon");
	if (opt->inflowDisable > 1 && opt->inflowDisable != 0xff)
		errx(1, "smcDisableInflow only supports enable or disable"
		    ", acceptable range is 0 or 1.");

	if (opt->batteryCharging != 0xff)
		smc_write(smcBatteryChargingApple,
		    opt->batteryCharging == 2 ? 1 : 0);

	if (opt->inflowDisable == 1) {
		smc_write(smcDisableInflow, 0);
		smc_write(smcDisableInflow, 1);
	} else if (opt->inflowDisable == 0) {
		smc_write(smcDisableInflow, 0);
	}

	if (opt->read) {
		pwr_battery_soc(1);
		smc_read(smcBatteryChargingApple);
		smc_read(smcDisableInflow);
	}

	return (0);
}

int
main(int argc, char *argv[])
{       
	int		 ch;
	uint8_t		 t;
	const char	*errstr;

	struct options	 opt = {
		.read			= 0,
		.batteryMax		= 0,
		.batteryCharging	= 0xff,
		.inflowDisable		= 0xff,
	};
        
	while ((ch = getopt(argc, argv, "b:c:d:r")) != -1) {
		switch (ch) {
		case 'b':
			opt.batteryMax = strtonum(optarg, 20, 100, &errstr);
			if (errstr != NULL)
				errx(1, "smcBatteryMax is %s: %s",
				    errstr, optarg);
			break;
		case 'c':
			t = strtonum(optarg, 0, 1, &errstr);
			if (errstr != NULL)
				errx(1, "smcBatteryCharging is %s: %s",
				    errstr, optarg);
			opt.batteryCharging = t == 0 ? 2 : 0;
			break;
		case 'd':
			opt.inflowDisable = strtonum(optarg, 0, 99,
			    &errstr);
			if (errstr != NULL)
				errx(1, "inflowDisable is %s: %s",
				    errstr, optarg);
			break;
		case 'r':
			opt.read = 1;
			break;
		default:
			usage();
		}
	}

	argc -= optind;
	argv += optind;
	optreset = 1;
	optind = 1;

	if ((opt.cputype = get_cpu_arch()) == -1)
		errx(1, "Unsupported architecture");

	if ((argc > 0) || (opt.read == 0 && opt.batteryMax == 0 &&
	    opt.batteryCharging == 0xff && opt.inflowDisable == 0xff))
		usage();

	switch (opt.cputype) {
	case CPU_TYPE_X86:
		pmctl_intel(&opt);
		break;
	case CPU_TYPE_ARM64:
		pmctl_apple(&opt);
		break;
	}

	return (0);
}
