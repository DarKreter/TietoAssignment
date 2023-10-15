#include "test.h"
#include "cpu.h"
#include <assert.h>

void RunTests(void)
{
	assert(2 + 2 == 4);

	CPUStats prev, curr;
	prev.idle		= 5000;
	prev.iowait		= 0;
	prev.user		= 2500;
	prev.nice		= 0;
	prev.system		= 2500;
	prev.irq		= 0;
	prev.softirq	= 0;
	prev.steal		= 0;
	prev.guest		= 0;
	prev.guest_nice = 0;
	curr.idle		= 10000;
	curr.iowait		= 0;
	curr.user		= 5000;
	curr.nice		= 0;
	curr.system		= 5000;
	curr.irq		= 0;
	curr.softirq	= 0;
	curr.steal		= 0;
	curr.guest		= 0;
	curr.guest_nice = 0;

	assert(50.f == CalculateCpuUsage(prev, curr));
}
