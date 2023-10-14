#ifndef CPU_TIETO_ASSIGNMENT
#define CPU_TIETO_ASSIGNMENT

typedef struct {
  int user;
  int nice;
  int system;
  int idle;
  int iowait;
  int irq;
  int softirq;
  int steal;
  int guest;
  int guest_nice;
} CPUStats;

float CalculateCpuUsage(CPUStats);
short CheckCoreCount();

#endif // CPU_TIETO_ASSIGNMENT