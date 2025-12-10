#include <Foundation/Foundation.h>
#include <stdio.h>
#include <pmctl.h>

struct osversion get_os_version()
{
	NSOperatingSystemVersion objv = [[NSProcessInfo processInfo] operatingSystemVersion];
	struct osversion os = {
		.majorVersion = objv.majorVersion,
		.minorVersion = objv.minorVersion,
		.patchVersion = objv.patchVersion,
	};
	return (os);
}
