#ifndef CRITICAL_SECTION_H_
#define CRITICAL_SECTION_H_

#define ENTER_CRITICAL() __asm volatile("cpsid i")
#define EXIT_CRITICAL()  __asm volatile("cpsie i")

#endif /* CRITICAL_SECTION_H_ */
