/**********************************************************************************************************************
 *  COPYRIGHT
 *  -------------------------------------------------------------------------------------------------------------------
 *  \verbatim
 *  Copyright (c) 2021 by Vector Informatik GmbH.                                              All rights reserved.
 *
 *                This software is copyright protected and proprietary to Vector Informatik GmbH.
 *                Vector Informatik GmbH grants to you only those rights as set out in the license conditions.
 *                All other rights remain with Vector Informatik GmbH.
 *  \endverbatim
 *  -------------------------------------------------------------------------------------------------------------------
 *  FILE DESCRIPTION
 *  -----------------------------------------------------------------------------------------------------------------*/
/**
 * \addtogroup  Os_Hal_Derivative
 * \{
 *
 * \file
 * \brief  This file contains derivative specific information.
 *
 * Internal comment removed.
 *
 *
 *
 *
 *
 *********************************************************************************************************************/
/**********************************************************************************************************************
 *  REVISION HISTORY
 *  -------------------------------------------------------------------------------------------------------------------
 *  Refer to Os_Hal_Os.h.
 *********************************************************************************************************************/

#ifndef OS_HAL_DERIVATIVE_TC24XINT_H
# define OS_HAL_DERIVATIVE_TC24XINT_H

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/

/**********************************************************************************************************************
 *  GLOBAL CONSTANT MACROS
 *********************************************************************************************************************/

/** TC24x **/

/* STM module */
# define OS_HAL_TIMER_STM                            STD_ON
# define OS_HAL_TIMER_CoreSTM                        STD_OFF

# define OS_HAL_TIMER_STM0_BASE                      (0xF0000000uL)
# define OS_HAL_INT_SRC_STM0_CH0_OFFSET              (0x490uL)
# define OS_HAL_INT_SRC_STM0_CH1_OFFSET              (0x494uL)


/* GPT module */
# define OS_HAL_TIMER_GPT                            STD_ON
# define OS_HAL_TIMER_GPT_BASE                       (0xF0002E00uL)
# define OS_HAL_INT_SRC_GPT_CH0_OFFSET               (0x468uL)
# define OS_HAL_INT_SRC_GPT_CH1_OFFSET               (0x474uL)


/* TriCore core module */
# define OS_HAL_CORE_161_EP                          STD_ON

/* Interrupt router (IR) module */
# define OS_HAL_INTERRUPT_AURIXTC2XX_IR              STD_ON
# define OS_HAL_INTERRUPT_IR_BASE                    (0xF0037000uL)



#endif /* OS_HAL_DERIVATIVE_TC24XINT_H */

/*!
 * \}
 */
/**********************************************************************************************************************
 *  END OF FILE: Os_Hal_Derivative_TC24xInt.h
 *********************************************************************************************************************/
