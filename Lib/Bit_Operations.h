/**
 * @file    Bit_Operations.h
 * @brief   Portable bit-manipulation macros for register access.
 */

#ifndef BIT_OPERATIONS_H_
#define BIT_OPERATIONS_H_

#define SET_BIT(REG, BIT)       ((REG) |=  (1U << (BIT)))
#define CLR_BIT(REG, BIT)       ((REG) &= ~(1U << (BIT)))
#define TOG_BIT(REG, BIT)       ((REG) ^=  (1U << (BIT)))
#define GET_BIT(REG, BIT)       (((REG) >> (BIT)) & 1U)

#define SET_BITS(REG, MSK)      ((REG) |=  (MSK))
#define CLR_BITS(REG, MSK)      ((REG) &= ~(MSK))

#define WRITE_REG(REG, VAL)     ((REG) = (VAL))
#define READ_REG(REG)           ((REG))

#define MODIFY_REG(REG, CLR_MSK, SET_MSK) \
    WRITE_REG((REG), (((REG) & ~(CLR_MSK)) | (SET_MSK)))

#endif /* BIT_OPERATIONS_H_ */
