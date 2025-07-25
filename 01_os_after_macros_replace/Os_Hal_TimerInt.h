/***********************************************************************************************************************
 *  COPYRIGHT
 *  --------------------------------------------------------------------------------------------------------------------
 *  \verbatim
 *  Copyright (c) 2021 by Vector Informatik GmbH.                                              All rights reserved.
 *
 *                This software is copyright protected and proprietary to Vector Informatik GmbH.
 *                Vector Informatik GmbH grants to you only those rights as set out in the license conditions.
 *                All other rights remain with Vector Informatik GmbH.
 *  \endverbatim
 *  --------------------------------------------------------------------------------------------------------------------
 *  FILE DESCRIPTION
 *  ------------------------------------------------------------------------------------------------------------------*/
/**
 *  \ingroup     Os_Hal
 *  \defgroup    Os_Hal_Timer HAL Timer
 *  \{
 *
 *  \file        Os_Hal_TimerInt.h
 *  \brief       Primitives to work with timer hardware.
 *  \details
 *  MICROSAR OS supports the following timer hardware:
 *  - free running timer (FRT)
 *  - programmable interval timer (PIT)
 *
 *  Often hardware timers can be configured to behave either as a FRT or as a PIT.
 *
 *  If the platform does not support high resolution, check this by an error directive:
 *
 *  ~~~~~~~~~{.c}
 *  #  if OS_CFG_TIMER_HIGH_RESOLUTION == STD_ON
 *  #   error "High resolution is not supported!"
 *  #  endif
 *  ~~~~~~~~~
 *
 *
 *  Free Running Timer (FRT) {#Os_Hal_Timer_FRT}
 *  ========================
 *  This kind of timer keeps repeatedly running from end to end without being reloaded or stopped at intermediate states.
 *  The kernel uses this kind of timer to implement high resolution systems.
 *
 *  MICROSAR OS assumes that this kind of timer has:
 *  - Counter register
 *  - Compare registers
 *  - The ability to generate interrupts.
 *
 *  Counter Register
 *  ----------------
 *  The counter register (sometimes called timer register) is most typically a n-bit up or down counter, with the
 *  ability to read and write the current count value.
 *
 *  Compare Registers
 *  -----------------
 *  Compare registers (sometimes called match registers) hold a value against which the current counter value
 *  is automatically and continuously compared. A compare register is used to trigger an interrupt when the value in
 *  the counter register matches the value in the compare register.
 *
 *  FRT Interrupt
 *  -------------
 *  A FRT uses a compare match to generate an interrupt, but does not configure the timer to clear when
 *  the match occurs. Thus the interrupt is triggered but the timer continues to run. It will eventually overflow,
 *  going from 2<sup>n</sup>-1 to 0, but that overflow will not stop the timer either. So the timer register counts
 *  indefinitely, going from 0 to 2<sup>n</sup>-1 and repeating that cycle endlessly.
 *
 *
 *  Programmable Interval Timer (PIT) {#Os_Hal_Timer_PIT}
 *  =================================
 *  This kind of timer generates an output signal when it reaches a programmed count and performs a reset in hardware.
 *
 *  A PIT does not necessarily need a compare register. It may use the counter overflow to generate an interrupt.
 *  Therefore MICROSAR OS assumes that this kind of timer has:
 *  - Counter register
 *  - The ability to generate interrupts.
 *
 *  PIT Interrupt
 *  -------------
 *  The interrupt is generated by match between the counter register and a compare register (or overflow).
 *
 *  The compare event which triggers the interrupt also automatically reloads the counter register,
 *  so we have a configuration that can generate repetitive timing intervals.
 *
 *  Using a FRT as a PIT
 *  --------------------
 *  The FRT can be used to implement a PIT in software. The compare register must be modified on each interrupt.
 *  The modification just involves adding a constant (the timer period, in ticks) to the compare register
 *  (ignoring any overflow).
 *
 *
 *  \see [Timers/Counters](www.scriptoriumdesigns.com/embedded/timers.php)
 *  \see [Counter](en.wikipedia.org/wiki/Counter_%28digital%29)
 *  \see [PIT](en.wikipedia.org/wiki/Programmable_interval_timer)
 *  \see \ref GlossaryHighResolutionTimer
 *  \see \ref JobScheduling
 *  \see Os_Counter
 *
 *
 *
 **********************************************************************************************************************/
/***********************************************************************************************************************
 *  REVISION HISTORY
 *  --------------------------------------------------------------------------------------------------------------------
 *  Refer to Os_Hal_Os.h.
 **********************************************************************************************************************/


#ifndef OS_HAL_TIMERINT_H
# define OS_HAL_TIMERINT_H

/***********************************************************************************************************************
 *  INCLUDES
 **********************************************************************************************************************/
/* AUTOSAR includes */
# include "Std_Types.h"

/* Os module declarations */

/* Os kernel module dependencies */

/* Os Hal dependencies */
# include "Os_Hal_CompilerInt.h"
# include "Os_Hal_Derivative.h"

# include "Os_Hal_Timer_Types.h"

# if (OS_HAL_TIMER_STM == STD_ON)                                                                                       /* COV_OS_HAL_TIMER_BETA */
#  include "Os_Hal_Timer_STMInt.h"
# endif

# if (OS_HAL_TIMER_GPT == STD_ON)                                                                                       /* COV_OS_HAL_TIMER_BETA */
#  include "Os_Hal_Timer_GPTInt.h"
# else
#  include "Os_Hal_Timer_PIT_DummyInt.h"
# endif

# if (OS_HAL_TIMER_CoreSTM == STD_ON)                                                                                   /* COV_OS_HAL_TIMER_BETA */
#  include "Os_Hal_Timer_CoreSTMInt.h"
# endif

/***********************************************************************************************************************
 *  GLOBAL CONSTANT MACROS
 **********************************************************************************************************************/


/***********************************************************************************************************************
 *  GLOBAL FUNCTION MACROS
 **********************************************************************************************************************/


/***********************************************************************************************************************
 *  GLOBAL DATA TYPES AND STRUCTURES
 **********************************************************************************************************************/


/***********************************************************************************************************************
 *  GLOBAL DATA PROTOTYPES
 **********************************************************************************************************************/


/***********************************************************************************************************************
 *  GLOBAL FUNCTION PROTOTYPES
 **********************************************************************************************************************/


#endif /* OS_HAL_TIMERINT_H */

/*!
 * \}
 */
/***********************************************************************************************************************
 *  END OF FILE: Os_Hal_TimerInt.h
 **********************************************************************************************************************/
