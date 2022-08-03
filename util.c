#include <stdio.h>
#include <mach-o/arch.h>

cpu_type_t
get_cpu_arch()
{
	
	cpu_type_t		 cputype = -1;
	const NXArchInfo	*arch = NXGetLocalArchInfo();
	if (arch == NULL)
		return (-1);

	cputype = arch->cputype;

	switch (arch->cputype) {
	case CPU_TYPE_X86:
	case CPU_TYPE_ARM64:
		break;
	default:
		cputype = -1;
		break;
	}

	if (arch)
		NXFreeArchInfo(arch);
	return (cputype);
}
