/**
 * @file    Std_Types.h
 * @brief   Standard platform-independent type definitions.
 */

#ifndef STD_TYPES_H_
#define STD_TYPES_H_

#include <stdint.h>
#include <stddef.h>

/* ------------------------------------------------------------------ */
/*  Boolean                                                            */
/* ------------------------------------------------------------------ */
#ifndef TRUE
#define TRUE   ((uint8_t)1U)
#endif

#ifndef FALSE
#define FALSE  ((uint8_t)0U)
#endif

/* ------------------------------------------------------------------ */
/*  Return status                                                      */
/* ------------------------------------------------------------------ */
typedef uint8_t Std_ReturnType;

#define E_OK       ((Std_ReturnType)0U)
#define E_NOT_OK   ((Std_ReturnType)1U)

/* ------------------------------------------------------------------ */
/*  NULL pointer guard                                                 */
/* ------------------------------------------------------------------ */
#ifndef NULL_PTR
#define NULL_PTR   ((void *)0)
#endif

#endif /* STD_TYPES_H_ */
