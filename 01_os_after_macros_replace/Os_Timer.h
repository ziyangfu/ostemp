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
 * \addtogroup Os_Timer
 *
 * \{
 *
 * \file
 * \brief       Interface implementation to high level timer hardware handling.
 * \details     --no details--
 *
 *
 **********************************************************************************************************************/
/***********************************************************************************************************************
 *  REVISION HISTORY
 *  --------------------------------------------------------------------------------------------------------------------
 *  Refer to Os.h.
 **********************************************************************************************************************/

#ifndef OS_TIMER_H
# define OS_TIMER_H
                                                                                                                         /* PRQA S 0388 EOF */ /* MD_MSR_Dir1.1 */
/***********************************************************************************************************************
 *  INCLUDES
 **********************************************************************************************************************/
/* AUTOSAR includes */
# include "Std_Types.h"

/* Os module declarations */
# include "Os_TimerInt.h"

/* Os kernel module dependencies */
# include "Os_Common.h"
# include "Os_Counter.h"
# include "Os_Isr.h"

/* Os HAL dependencies */
# include "Os_Hal_Timer.h"



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
# define OS_START_SEC_CODE
# include "Os_MemMap_OsCode.h"                                                                                          /* PRQA S 5087 */ /* MD_MSR_MemMap */

/**********************************************************************************************************************
 *  GLOBAL FUNCTIONS
 **********************************************************************************************************************/
/***********************************************************************************************************************
 *  Os_TimerSwInit()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline void   /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
Os_TimerSwInit (
  const   Os_TimerSwConfigType OS_CONST * Timer
)
{
  Timer->Dyn->Value = 0;                                                                                                /* SBSW_OS_TIMER_TIMERGETDYN_001 */
  Timer->Dyn->Compare = 0;                                                                                              /* SBSW_OS_TIMER_TIMERGETDYN_001 */

  /* Check that macro and configuration are consistent. */
  Os_Assert((Os_StdReturnType)( Timer->Counter.Characteristics.MaxCountingValue                                         /* PRQA S 4304 */ /* MD_Os_C90BooleanCompatibility */
          == OS_TIMERSW_GETMAXCOUNTINGVALUE((Os_TickType)Timer->Counter.Characteristics.MaxAllowedValue)));

  Os_Assert((Os_StdReturnType)( Timer->Counter.Characteristics.MaxDifferentialValue                                     /* PRQA S 4304 */ /* MD_Os_C90BooleanCompatibility */
          == OS_TIMERSW_GETMAXDIFFERENTIALVALUE((Os_TickType)Timer->Counter.Characteristics.MaxAllowedValue)));
}


/***********************************************************************************************************************
 *  Os_TimerPitInit()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline void   /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */ /* COV_OS_HALPLATFORMPITUNSUPPORTED_XX */
Os_TimerPitInit (
  const   Os_TimerPitConfigType OS_CONST * Timer
)                                                                                                                      /* COV_OS_HALPLATFORMPITUNSUPPORTED_XX */
{
  Os_TimerSwInit(&(Timer->SwCounter));                                                                                  /* SBSW_OS_FC_PRECONDITION */

  /* Check that macro and configuration are consistent. */
  Os_Assert((Os_StdReturnType)( Timer->SwCounter.Counter.Characteristics.MaxCountingValue                               /* PRQA S 4304 */ /* MD_Os_C90BooleanCompatibility */
      == OS_TIMERPIT_GETMAXCOUNTINGVALUE((Os_TickType)Timer->SwCounter.Counter.Characteristics.MaxAllowedValue)));      /* PRQA S 2995 */ /* MD_Os_Rule2.2_2995 */

  Os_Assert((Os_StdReturnType)( Timer->SwCounter.Counter.Characteristics.MaxDifferentialValue                           /* PRQA S 4304 */ /* MD_Os_C90BooleanCompatibility */
      == OS_TIMERPIT_GETMAXDIFFERENTIALVALUE((Os_TickType)Timer->SwCounter.Counter.Characteristics.MaxAllowedValue)));  /* PRQA S 2995 */ /* MD_Os_Rule2.2_2995 */

}


/***********************************************************************************************************************
 *  Os_TimerHrtInit()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline void   /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */ /* COV_OS_HALPLATFORMFRTUNSUPPORTED_XX */
Os_TimerHrtInit (
  const   Os_TimerHrtConfigType OS_CONST * Timer
)                                                                                                                      /* COV_OS_HALPLATFORMFRTUNSUPPORTED_XX */
{
  Timer->Dyn->Value = 0;                                                                                                /* SBSW_OS_TIMER_TIMERGETDYN_001 */
  Timer->Dyn->Compare = 0;                                                                                              /* SBSW_OS_TIMER_TIMERGETDYN_001 */
  Timer->Dyn->ValueMask = 0;                                                                                            /* SBSW_OS_TIMER_TIMERGETDYN_001 */
  Timer->Dyn->HwLastCounterValue = 0;                                                                                   /* SBSW_OS_TIMER_TIMERGETDYN_001 */
  Timer->Dyn->HwTimeAtLogicalZero = 0;                                                                                  /* SBSW_OS_TIMER_TIMERGETDYN_001 */

  /* Check that macro and configuration are consistent. */
  Os_Assert((Os_StdReturnType)( Timer->Counter.Characteristics.MaxCountingValue                                         /* PRQA S 4304 */ /* MD_Os_C90BooleanCompatibility */
          == OS_TIMERHRT_GETMAXCOUNTINGVALUE((Os_TickType)Timer->Counter.Characteristics.MaxAllowedValue)));

  Os_Assert((Os_StdReturnType)( Timer->Counter.Characteristics.MaxDifferentialValue                                     /* PRQA S 4304 */ /* MD_Os_C90BooleanCompatibility */
          == OS_TIMERHRT_GETMAXDIFFERENTIALVALUE((Os_TickType)Timer->Counter.Characteristics.MaxAllowedValue)));
}


/***********************************************************************************************************************
 *  Os_TimerPfrtInit()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline void   /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */ /* COV_OS_HALPLATFORMPFRTUNSUPPORTED_XX */
Os_TimerPfrtInit (
  const   Os_TimerPfrtConfigType OS_CONST * Timer
)                                                                                                                      /* COV_OS_HALPLATFORMPFRTUNSUPPORTED_XX */
{
  Os_TimerSwInit(&(Timer->SwCounter));                                                                                  /* SBSW_OS_FC_PRECONDITION */
}


/***********************************************************************************************************************
 *  Os_TimerSwStart()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline void   /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
Os_TimerSwStart (
  const   Os_TimerSwConfigType OS_CONST * Timer
)
{
  /* Nothing to be done for software counter, as there is no underlying hardware. */
  OS_IGNORE_UNREF_PARAM(Timer);                                                                                         /* PRQA S 3112 */ /* MD_Os_3112 */
}


/***********************************************************************************************************************
 *  Os_TimerPitStart()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline void   /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */ /* COV_OS_HALPLATFORMPITUNSUPPORTED_XX */
Os_TimerPitStart (
  const   Os_TimerPitConfigType OS_CONST * Timer
)                                                                                                                      /* COV_OS_HALPLATFORMPITUNSUPPORTED_XX */
{
  Os_Hal_TimerPitInit(Timer->HwConfig);                                                                                 /* SBSW_OS_TIMER_HAL_TIMERPITINIT_001 */
}


/***********************************************************************************************************************
 *  Os_TimerHrtStart()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline void   /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */ /* COV_OS_HALPLATFORMFRTUNSUPPORTED_XX */
Os_TimerHrtStart (
  const   Os_TimerHrtConfigType OS_CONST * Timer
)                                                                                                                      /* COV_OS_HALPLATFORMFRTUNSUPPORTED_XX */
{
  Os_TickType timeZero;

  /* #10 Initialize (start) the FRT timer. */
  Os_Hal_TimerFrtInit(Timer->HwConfig);                                                                                 /* SBSW_OS_TIMER_HAL_TIMERFRTINIT_001 */

  timeZero = Os_TimerFrtTickType2TickType(Os_Hal_TimerFrtGetCounterValue(Timer->HwConfig));                             /* SBSW_OS_TIMER_HAL_TIMERFRTGETCOUNTERVALUE_001 */
  Timer->Dyn->HwTimeAtLogicalZero = timeZero;                                                                           /* SBSW_OS_TIMER_TIMERGETDYN_001 */

  /* #20 Correct the timer data (to support the case of FRT start by reset). */
  Timer->Dyn->ValueMask = Timer->Counter.Characteristics.MaxCountingValue; /* Allow the timer value to increase. */     /* SBSW_OS_TIMER_TIMERGETDYN_001 */
  Timer->Dyn->HwLastCounterValue = timeZero;                               /* Store the last hardware counter value. */ /* SBSW_OS_TIMER_TIMERGETDYN_001 */
  Os_TimerHrtSetCompareValue(Timer, Timer->Dyn->Compare);                  /* Set the initial compare value. */         /* SBSW_OS_FC_PRECONDITION */
}


/***********************************************************************************************************************
 *  Os_TimerSwGetValue()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_TickType   /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
Os_TimerSwGetValue (
  const   Os_TimerSwConfigType OS_CONST * Timer
)
{
  return Timer->Dyn->Value;
}


/***********************************************************************************************************************
 *  Os_TimerSwSetCompareValue()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE                                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
static inline void   Os_TimerSwSetCompareValue (
  const   Os_TimerSwConfigType OS_CONST * Timer,
  Os_TickType ExpirationTime
)
{
  Timer->Dyn->Compare = ExpirationTime;                                                                                 /* SBSW_OS_TIMER_TIMERGETDYN_001 */
}


/***********************************************************************************************************************
 *  Os_TimerSwUserModulo()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 */
OS_ALWAYS_INLINE static inline Os_TickType   /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
Os_TimerSwUserModulo (
  const   Os_TimerSwConfigType OS_CONST * Timer,
  Os_TickType Value
)
{
  Os_TickType result = Value;

  /* #10 Subtract one user cycle if Value exceeds the MaxAllowedValue. */
  if(Value > Timer->Counter.Characteristics.MaxAllowedValue)
  {
    result -= (Timer->Counter.Characteristics.MaxAllowedValue + (Os_TickType)1);
  }

  /* Check that the calculated value is in a valid range (0 <= Value <= MaxAllowedValue). */
  Os_Assert((Os_StdReturnType)(result <= Timer->Counter.Characteristics.MaxAllowedValue));                              /* PRQA S 4304 */ /* MD_Os_C90BooleanCompatibility */

  return result;
}



/***********************************************************************************************************************
 *  Os_TimerPitGetValue()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_TickType   /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */ /* COV_OS_HALPLATFORMPITUNSUPPORTED_XX */
Os_TimerPitGetValue (
  const   Os_TimerPitConfigType OS_CONST * Timer
)                                                                                                                      /* COV_OS_HALPLATFORMPITUNSUPPORTED_XX */
{
  return Os_TimerSwGetValue(&(Timer->SwCounter));                                                                       /* SBSW_OS_FC_PRECONDITION */
}


/***********************************************************************************************************************
 *  Os_TimerPitSetCompareValue()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE                                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */ /* COV_OS_HALPLATFORMPITUNSUPPORTED_XX */
static inline void   Os_TimerPitSetCompareValue (
  const   Os_TimerPitConfigType OS_CONST * Timer,
  Os_TickType ExpirationTime
)                                                                                                                      /* COV_OS_HALPLATFORMPITUNSUPPORTED_XX */
{
  Os_TimerSwSetCompareValue(&(Timer->SwCounter), ExpirationTime);                                                       /* SBSW_OS_FC_PRECONDITION */
}


/***********************************************************************************************************************
 *  Os_TimerPitUserModulo()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_TickType   /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */ /* COV_OS_HALPLATFORMPITUNSUPPORTED_XX */
Os_TimerPitUserModulo (
  const   Os_TimerPitConfigType OS_CONST * Timer,
  Os_TickType Value
)                                                                                                                      /* COV_OS_HALPLATFORMPITUNSUPPORTED_XX */
{
  return Os_TimerSwUserModulo(&(Timer->SwCounter), Value);                                                              /* SBSW_OS_FC_PRECONDITION */
}


/***********************************************************************************************************************
 *  Os_TimerPfrtUserModulo()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_TickType   /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */ /* COV_OS_HALPLATFORMPFRTUNSUPPORTED_XX */
Os_TimerPfrtUserModulo (
  const   Os_TimerPfrtConfigType OS_CONST * Timer,
  Os_TickType Value
)                                                                                                                      /* COV_OS_HALPLATFORMPFRTUNSUPPORTED_XX */
{
  return Os_TimerSwUserModulo(&(Timer->SwCounter), Value);                                                              /* SBSW_OS_FC_PRECONDITION */
}


/***********************************************************************************************************************
 *  Os_TimerHrtGetCounter()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */ /* COV_OS_HALPLATFORMFRTUNSUPPORTED_XX */ /* COV_OS_HALPLATFORMTPUNSUPPORTED_XX */
const   Os_CounterConfigType OS_CONST *   Os_TimerHrtGetCounter (
  const   Os_TimerHrtConfigType OS_CONST * Timer
)                                                                                                                      /* COV_OS_HALPLATFORMFRTUNSUPPORTED_XX */ /* COV_OS_HALPLATFORMTPUNSUPPORTED_XX */
{
  return &(Timer->Counter);
}


/***********************************************************************************************************************
 *  Os_TimerHrtComputeHardwareCompareValue()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE                                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */ /* COV_OS_HALPLATFORMFRTUNSUPPORTED_XX */
static inline Os_TickType   Os_TimerHrtComputeHardwareCompareValue (
  const   Os_TimerHrtConfigType OS_CONST * Timer,
  Os_TickType ExpirationTime
)                                                                                                                      /* COV_OS_HALPLATFORMFRTUNSUPPORTED_XX */
{
  /* #10 Initialize the return value with the requested expiration time.*/
  Os_TickType hwCompareValue = ExpirationTime;

  /* #20 If the size of the physical counter value type is smaller then the OS TickType: */
  if(Os_TimerIsFrtShortCorrectionNeeded() != 0u)                                                                        /* COV_OS_HALPLATFORMFRT16BITAVAILABILITY */ /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
  {
    Os_TickType now = Os_TimerHrtGetValue(Timer);                                                                       /* SBSW_OS_FC_PRECONDITION */

    /* #30 If the expiration time is a future value: */
    if(Os_CounterIsFutureValue(&(Timer->Counter), ExpirationTime, now) != 0u)                                           /* SBSW_OS_FC_PRECONDITION */
    {
      /* #40 Calculate the difference in ticks. */
      Os_TickType diff = ExpirationTime - now;

      /* #50 If the difference is larger than a half hardware timer cycle: */
      if(diff > ((Os_TickType)OS_FRT_TICKTYPE_HALF))                                                                    /* COMP_WARN_OS_CW_005 */ /* PRQA S 2992, 2996 */ /* MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2996 */
      {
        /* #60 Set the return value to the current time added by a half hardware timer cycle. */
        hwCompareValue = now + ((Os_TickType)OS_FRT_TICKTYPE_HALF);                                                     /* PRQA S 2880 */ /* MD_Os_Rule2.1_2880 */
      }
    }
  }

  /* #70 Return the hardware compare value. */
  return hwCompareValue;
}


/***********************************************************************************************************************
 *  Os_TimerHrtSetCompareValue()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE                                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */ /* COV_OS_HALPLATFORMFRTUNSUPPORTED_XX */
static inline void   Os_TimerHrtSetCompareValue (
  const   Os_TimerHrtConfigType OS_CONST * Timer,
  Os_TickType ExpirationTime
)                                                                                                                      /* COV_OS_HALPLATFORMFRTUNSUPPORTED_XX */
{
  Os_TickType now;
  Os_TickType correctedExpirationTime;

  /* #10 Store the ExpirationTime for the case that the compare difference does not fit into the hardware part
   *     of the counter. */
  Timer->Dyn->Compare = ExpirationTime;                                                                                 /* SBSW_OS_TIMER_TIMERGETDYN_001 */

  /* #13 Get the compare value that needs to be written to the hardware register. */
  correctedExpirationTime = Os_TimerHrtComputeHardwareCompareValue(Timer, ExpirationTime);                              /* SBSW_OS_FC_PRECONDITION */

  /* #15 Calculate the corrected expiration time in accordance to the hardware time at logical zero. */
  correctedExpirationTime = Timer->Dyn->HwTimeAtLogicalZero + correctedExpirationTime;                                  /* SBSW_OS_FC_PRECONDITION */

  /* #20 Set the compare value. */
  Os_TimerFrtSetCompareValue(Timer->HwConfig, correctedExpirationTime);                                                 /* SBSW_OS_TIMER_TIMERFRTSETCOMPAREVALUE_001 */

  /* #30 Check whether the ExpirationTime is already reached. */
  now = Os_TimerHrtGetValue(Timer);                                                                                     /* SBSW_OS_FC_PRECONDITION */

  /* #40 If new compare value is already in the past or now: */
  if(OS_UNLIKELY(Os_CounterIsFutureValue(&(Timer->Counter), ExpirationTime, now) == 0u))                                /* SBSW_OS_FC_PRECONDITION */
  {
    /* #50 If the platform supports peripheral interrupt trigger: */
    if (Os_Hal_IntIsPeripheralInterruptTriggerSupported() != 0u)                                                        /* COV_OS_HALPLATFORMPERIPHERALINTTRIGGERSUPPORTED */ /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
    {
      /* #60 Trigger interrupt, so the violation is handled after the thread switch is completed. */
      Os_Hal_TimerFrtTriggerIrq(Timer->HwConfig);                                                                       /* SBSW_OS_TIMER_HAL_TIMERFRTTRIGGERIRQ_001 */
    }
    else
    {
      Os_TickType iterationId;
      Os_TickType delta;
      Os_TickType compareValue;
      Os_TickType timeBeforeSetCompare;
      Os_TickType timeAfterSetCompare;
      delta = 0;
      iterationId = 1;
      do
      {
        /* #70 Else read the current counter value from hardware timer. */
        timeBeforeSetCompare = Os_TimerFrtTickType2TickType(Os_Hal_TimerFrtGetCounterValue(Timer->HwConfig));           /* SBSW_OS_TIMER_HAL_TIMERFRTGETCOUNTERVALUE_001 */

        /* #80 Set the compare value to the previously read counter value plus delay. */
        compareValue = timeBeforeSetCompare + delta + iterationId;                                                      /* PRQA S 2986 */ /* MD_Os_Rule2.2_2986 */
        Os_TimerFrtSetCompareValue(Timer->HwConfig, compareValue);                                                      /* SBSW_OS_TIMER_TIMERFRTSETCOMPAREVALUE_001 */

        /* #90 Read again the current hardware counter value and update the delay, taking into
               account the counter values difference and the loop iteration count. */
        timeAfterSetCompare = Os_TimerFrtTickType2TickType(Os_Hal_TimerFrtGetCounterValue(Timer->HwConfig));            /* SBSW_OS_TIMER_HAL_TIMERFRTGETCOUNTERVALUE_001 */
        delta = timeAfterSetCompare - timeBeforeSetCompare;
        iterationId++;

      /* #100 Loop while compare value is in the past or now with respect to the last read counter value. */
      }while (OS_UNLIKELY(Os_CounterIsFutureValue(&(Timer->Counter), compareValue, timeAfterSetCompare) == 0u));        /* SBSW_OS_FC_PRECONDITION */

    }
  }
}                                                                                                                       /* PRQA S 6050 */ /* MD_MSR_STCAL */


/***********************************************************************************************************************
 *  Os_TimerPfrtSetCompareValue()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 */
OS_ALWAYS_INLINE static inline void   /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */ /* COV_OS_HALPLATFORMPFRTUNSUPPORTED_XX */
Os_TimerPfrtSetCompareValue (
  const   Os_TimerPfrtConfigType OS_CONST * Timer,
  Os_TickType ExpirationTime
)                                                                                                                      /* COV_OS_HALPLATFORMPFRTUNSUPPORTED_XX */
{
  /* #10 Use function of software timer. */
  Os_TimerSwSetCompareValue(&(Timer->SwCounter), ExpirationTime);                                                       /* SBSW_OS_FC_PRECONDITION */
}


/***********************************************************************************************************************
 *  Os_TimerHrtGetCompareValue()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_TickType   /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */ /* COV_OS_HALPLATFORMFRTUNSUPPORTED_XX */ /* COV_OS_HALPLATFORMTPUNSUPPORTED_XX */
Os_TimerHrtGetCompareValue (
  const   Os_TimerHrtConfigType OS_CONST * Timer
)                                                                                                                      /* COV_OS_HALPLATFORMFRTUNSUPPORTED_XX */ /* COV_OS_HALPLATFORMTPUNSUPPORTED_XX */
{
  return Timer->Dyn->Compare;
}


/***********************************************************************************************************************
 *  Os_TimerHrtAcknowledge()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline void   /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */ /* COV_OS_HALPLATFORMFRTUNSUPPORTED_XX */
Os_TimerHrtAcknowledge (
  const   Os_TimerHrtConfigType OS_CONST * Timer
)                                                                                                                      /* COV_OS_HALPLATFORMFRTUNSUPPORTED_XX */
{
  Os_Hal_TimerFrtAcknowledge(Timer->HwConfig);                                                                          /* SBSW_OS_TIMER_HAL_TIMERFRTACKNOWLEDGE_001 */
}


/***********************************************************************************************************************
 *  Os_TimerHrtIsLogicalCompareValueReached()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline Os_StdReturnType   /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */ /* COV_OS_HALPLATFORMFRTUNSUPPORTED_XX */
Os_TimerHrtIsLogicalCompareValueReached (
  const   Os_TimerHrtConfigType OS_CONST * Timer
)                                                                                                                      /* COV_OS_HALPLATFORMFRTUNSUPPORTED_XX */
{
  /* #10 Initialize the result to FALSE. */
  Os_StdReturnType result = 0u;                                                                                         /* PRQA S 2981 */ /* MD_Os_Rule2.2_2981 */

  /* #20 If the size of the physical counter value type is smaller then the OS TickType: */
  if(Os_TimerIsFrtShortCorrectionNeeded() != 0u)                                                                        /* COV_OS_HALPLATFORMFRT16BITAVAILABILITY */ /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
  {
    Os_TickType now = Os_TimerHrtGetValue(Timer);                                                                       /* SBSW_OS_FC_PRECONDITION */

    /* #30 If the current time is after or equal to the logical compare value: */
    if(Os_CounterIsFutureValue(&(Timer->Counter), Timer->Dyn->Compare, now) == 0u)                                      /* SBSW_OS_FC_PRECONDITION */
    {
      /* #40 The result is TRUE. */
      result = !0u;                                                                                                     /* PRQA S 4404, 4558 */ /* MD_Os_C90BooleanCompatibility, MD_Os_Rule10.1_4558 */
    }
  }
  /* #50 Otherwise: */
  else
  {
    /* #60 The hardware compare value is equal to the OS internal compare value, so the result is TRUE. */
    result = !0u;                                                                                                       /* PRQA S 4404, 4558 */ /* MD_Os_C90BooleanCompatibility, MD_Os_Rule10.1_4558 */
  }

  return result;
}


/***********************************************************************************************************************
 *  Os_TimerHrtUserModulo()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_TickType   /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */ /* COV_OS_HALPLATFORMFRTUNSUPPORTED_XX */
Os_TimerHrtUserModulo (
  const   Os_TimerHrtConfigType OS_CONST * Timer,
  Os_TickType Value
)                                                                                                                      /* COV_OS_HALPLATFORMFRTUNSUPPORTED_XX */
{
  return (Value & Timer->Counter.Characteristics.MaxAllowedValue);
}


/***********************************************************************************************************************
 *  Os_TimerHrtGetValue()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline Os_TickType   /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */ /* COV_OS_HALPLATFORMFRTUNSUPPORTED_XX */
Os_TimerHrtGetValue (
  const   Os_TimerHrtConfigType OS_CONST * Timer
)                                                                                                                      /* COV_OS_HALPLATFORMFRTUNSUPPORTED_XX */
{
  Os_TickType now;
  Os_Hal_TimerFrtTickType timePassed;

  /* #10 Get the current hardware counter value. */
  now = Os_TimerFrtTickType2TickType(Os_Hal_TimerFrtGetCounterValue(Timer->HwConfig)) & (Timer->Dyn->ValueMask);        /* SBSW_OS_TIMER_HAL_TIMERFRTGETCOUNTERVALUE_001 */

  /* #20 Calculate how much time has passed since we have read from the hardware the last time. */
  timePassed = Os_TimerTickType2FrtTickType(now - Timer->Dyn->HwLastCounterValue);                                      /* PRQA S 2986 */ /* MD_Os_Rule2.2_2986 */
  Timer->Dyn->HwLastCounterValue = now;                                                                                 /* SBSW_OS_TIMER_TIMERGETDYN_001 */

  /* #30 Add this time to the current counter value and return it. */
  Timer->Dyn->Value += Os_TimerFrtTickType2TickType(timePassed);                                                        /* SBSW_OS_TIMER_TIMERGETDYN_001 */

  return Timer->Dyn->Value;
}


/***********************************************************************************************************************
 *  Os_TimerPfrtGetValue()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 */
OS_ALWAYS_INLINE static inline Os_TickType   /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */ /* COV_OS_HALPLATFORMPFRTUNSUPPORTED_XX */
Os_TimerPfrtGetValue (
  const   Os_TimerPfrtConfigType OS_CONST * Timer
)                                                                                                                      /* COV_OS_HALPLATFORMPFRTUNSUPPORTED_XX */
{
  return Os_TimerSwGetValue(&(Timer->SwCounter));                                                                       /* SBSW_OS_FC_PRECONDITION */
}


/***********************************************************************************************************************
 *  Os_TimerSwAbsCounter2AbsTimerValue()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline Os_TickType   /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
Os_TimerSwAbsCounter2AbsTimerValue (
  Os_TickType MaxAllowedValue,
  Os_TickType Now,
  Os_TickType CounterValue
)
{
  Os_TickType timerValue;
  Os_TickType timerFirstCycle;
  Os_TickType timerSecondCycle;

  timerFirstCycle = CounterValue;
  timerSecondCycle = CounterValue + MaxAllowedValue + (uint32)1;

  /* #10 If CounterValue is bigger than Now (implies that Now is in the first cycle). */
  if(CounterValue > Now)
  {
    /* #20 Set return value to CounterValue, the expiration time will be reached within the first cycle. */
    timerValue = timerFirstCycle;
  }
  /* #30 Else if CounterValue+MaxallowedValue+1 is bigger than Now (Now may be in first or second cycle). */
  else if(timerSecondCycle > Now)
  {
    /*#40 Set return value to CounterValue+MaxallowedValue+1, the expiration time will be reached in the second cycle.*/
    timerValue = timerSecondCycle;
  }
  /* #50 Else (Now is in the second cycle) */
  else
  {
    /* #60 Set return value to CounterValue, the expiration time will be reached in the first cycle. */
    timerValue = timerFirstCycle;
  }

  return timerValue;
}


/***********************************************************************************************************************
 *  Os_TimerPitAbsCounter2AbsTimerValue()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_TickType   /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */ /* COV_OS_HALPLATFORMPITUNSUPPORTED_XX */
Os_TimerPitAbsCounter2AbsTimerValue (
  Os_TickType MaxAllowedValue,
  Os_TickType Now,
  Os_TickType CounterValue
)                                                                                                                      /* COV_OS_HALPLATFORMPITUNSUPPORTED_XX */
{
  return Os_TimerSwAbsCounter2AbsTimerValue(MaxAllowedValue, Now, CounterValue);
}


/***********************************************************************************************************************
 *  Os_TimerHrtAbsCounter2AbsTimerValue()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline Os_TickType   /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */ /* COV_OS_HALPLATFORMFRTUNSUPPORTED_XX */
Os_TimerHrtAbsCounter2AbsTimerValue (
  Os_TickType MaxAllowedValue,
  Os_TickType MaxCountingValue,
  Os_TickType Now,
  Os_TickType CounterValue
)                                                                                                                      /* COV_OS_HALPLATFORMFRTUNSUPPORTED_XX */
{
  Os_TickType timerValue;

  /* #10 Take the upper bits from the timer (Now) and the lower bits from the CounterValue. */
  timerValue = (Now & (~(MaxAllowedValue))) + CounterValue;

  /* #20 If this time stamp is already in the past or now, take the next cycle. */
  if(timerValue <= Now)
  {
    timerValue += (MaxAllowedValue + (uint32)1);
  }

  return (timerValue & MaxCountingValue);
}


/***********************************************************************************************************************
 *  Os_TimerPfrtAbsCounter2AbsTimerValue()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 */
OS_ALWAYS_INLINE static inline Os_TickType   /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */ /* COV_OS_HALPLATFORMPFRTUNSUPPORTED_XX */
Os_TimerPfrtAbsCounter2AbsTimerValue (
  Os_TickType MaxAllowedValue,
  Os_TickType Now,
  Os_TickType CounterValue
)                                                                                                                      /* COV_OS_HALPLATFORMPFRTUNSUPPORTED_XX */
{
  /* #10 Convert values based on software timer implementation. */
  return Os_TimerSwAbsCounter2AbsTimerValue(MaxAllowedValue, Now, CounterValue);
}


/***********************************************************************************************************************
 *  Os_TimerAdd()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline Os_TickType   /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
Os_TimerAdd (
  Os_TickType MaxCountingValue,
  Os_TickType Value,
  Os_TickType Add
)
{
  Os_TickType result;

  /* #10 If the sum of Value and Add exceeds the MaxCountingValue, subtract MaxCountingValue+1 from the sum. */
  if(Add > (MaxCountingValue - Value))
  {
    Os_TickType mod;

    /* 1.             Add > MaxCountingValue - Value            - see if-statement
     * 2.  => Add + Value > MaxCountingValue
     * 3.  => There is a value overflow.
     */
    mod = MaxCountingValue + (uint32)1;
    result = (Value - mod) + Add;
  }
  /* #20 Else add Value and Add. */
  else
  {
    result = Value + Add;
  }

  /* Check that the calculated value is in a valid range (0 <= Value < mod). */
  Os_Assert((Os_StdReturnType)(result <= MaxCountingValue));                                                            /* PRQA S 4304 */ /* MD_Os_C90BooleanCompatibility */

  return result;
}


/***********************************************************************************************************************
 *  Os_TimerSub()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline Os_TickType   /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */ /* COV_OS_HALPLATFORMFRTUNSUPPORTED_XX */
Os_TimerSub (
  Os_TickType MaxCountingValue,
  Os_TickType Value,
  Os_TickType Sub
)                                                                                                                      /* COV_OS_HALPLATFORMFRTUNSUPPORTED_XX */
{
  Os_TickType result;
  Os_TickType mod;

  mod = MaxCountingValue + (uint32)1;

  Os_Assert((Os_StdReturnType)(Value <= MaxCountingValue));                                                             /* PRQA S 4304 */ /* MD_Os_C90BooleanCompatibility */
  Os_Assert((Os_StdReturnType)(Sub <=  MaxCountingValue));                                                              /* PRQA S 4304 */ /* MD_Os_C90BooleanCompatibility */

  /* #10 If the Sub is greater than Value, add MaxCountingValue+1 to Value and subtract Sub. */
  if(Sub > Value)
  {
    result = (Value + mod) - Sub;
  }
  /* #20 Else subtract Sub from Value. */
  else
  {
    result = Value - Sub;
  }

  /* Check that the calculated value is in a valid range (0 <= Value < mod). */
  Os_Assert((Os_StdReturnType)(result <= MaxCountingValue));                                                            /* PRQA S 4304 */ /* MD_Os_C90BooleanCompatibility */

  return result;
}


/***********************************************************************************************************************
 *  Os_TimerFrtSetCompareValue()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 *
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline void   /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */ /* COV_OS_HALPLATFORMFRTUNSUPPORTED_XX */
Os_TimerFrtSetCompareValue (
  const   Os_Hal_TimerFrtConfigType OS_CONST * TimerConfig,
  Os_TickType CompareValue
)                                                                                                                      /* COV_OS_HALPLATFORMFRTUNSUPPORTED_XX */
{
  /* #10 If correction of the compare value is needed: */
  if(Os_TimerIsFrtLongCorrectionNeeded() != 0u)                                                                         /* COV_OS_HALPLATFORMFRT64BITAVAILABILITY */ /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
  {
    Os_Hal_TimerFrtTickType now;      /* current time in (big) HAL type */
    Os_TickType             shortNow; /* current time in (smaller) Core type */
    Os_Hal_TimerFrtTickType hwCompareValue;

    /* #20 Get the current time value from hardware. */
    now = Os_Hal_TimerFrtGetCounterValue(TimerConfig);                                                                  /* SBSW_OS_FC_PRECONDITION */
    shortNow = Os_TimerFrtTickType2TickType(now);

    /* #30 Calculate the hardware compare value by transferring the upper bits of the now value to it. */
    hwCompareValue = (now & Os_TimerFrtGetLongValueMask()) + Os_TimerTickType2FrtTickType(CompareValue);

    /* #40 If an overflow has to occur between now and compare value: */
    if(Os_TimerIsCompareValueCorrectionNeeded(shortNow, CompareValue) != 0u)
    {
      /* #50 Handle the overflow by incrementing the upper bits of the hardware compare value. */
      hwCompareValue += Os_TimerTickType2FrtTickType(OS_OS_TICKTYPE_MAX) + 1u;
    }
    /* #60 Set the hardware compare value from corrected parameter value. */
    Os_Hal_TimerFrtSetCompareValue(TimerConfig, hwCompareValue);                                                        /* SBSW_OS_FC_PRECONDITION */
  /* #70 otherwise: */
  }else{
    /* #80 Set the hardware compare value directly from parameter value. */
    Os_Hal_TimerFrtSetCompareValue(TimerConfig, Os_TimerTickType2FrtTickType(CompareValue));                            /* SBSW_OS_FC_PRECONDITION */
  }
}


/***********************************************************************************************************************
 *  Os_TimerFrtTickType2TickType()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_TickType   /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */ /* COV_OS_HALPLATFORMFRTUNSUPPORTED_XX */
Os_TimerFrtTickType2TickType (
  Os_Hal_TimerFrtTickType Value
)                                                                                                                      /* COV_OS_HALPLATFORMFRTUNSUPPORTED_XX */
{
  return (Os_TickType)Value;
}


/***********************************************************************************************************************
 *  Os_TimerTickType2FrtTickType()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_Hal_TimerFrtTickType   /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */ /* COV_OS_HALPLATFORMFRTUNSUPPORTED_XX */
Os_TimerTickType2FrtTickType (
  Os_TickType Value
)                                                                                                                      /* COV_OS_HALPLATFORMFRTUNSUPPORTED_XX */
{
  return (Os_Hal_TimerFrtTickType)Value;
}


/***********************************************************************************************************************
 *  Os_TimerIsFrtLongCorrectionNeeded()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StdReturnType   /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */ /* COV_OS_HALPLATFORMFRTUNSUPPORTED_XX */
Os_TimerIsFrtLongCorrectionNeeded (void)                                                                               /* COV_OS_HALPLATFORMFRTUNSUPPORTED_XX */
{
  return (Os_StdReturnType) (OS_OS_TICKTYPE_MAX < OS_FRT_TICKTYPE_MAX);                                                 /* PRQA S 4304, 2995, 2996 */ /* MD_Os_C90BooleanCompatibility, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
}


/***********************************************************************************************************************
 *  Os_TimerIsFrtShortCorrectionNeeded()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StdReturnType   /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */ /* COV_OS_HALPLATFORMFRTUNSUPPORTED_XX */
Os_TimerIsFrtShortCorrectionNeeded (void)                                                                              /* COV_OS_HALPLATFORMFRTUNSUPPORTED_XX */
{
  return (Os_StdReturnType) (OS_OS_TICKTYPE_MAX > OS_FRT_TICKTYPE_MAX);                                                 /* PRQA S 4304, 2995, 2996 */ /* MD_Os_C90BooleanCompatibility, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
}


/***********************************************************************************************************************
 *  Os_TimerIsCompareValueCorrectionNeeded()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StdReturnType   /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */ /* COV_OS_HALPLATFORMFRT64BITAVAILABILITY */
Os_TimerIsCompareValueCorrectionNeeded (
  Os_TickType NowValue,
  Os_TickType CompareValue
)                                                                                                                      /* COV_OS_HALPLATFORMFRT64BITAVAILABILITY */
{
  return (Os_StdReturnType) ((OS_TIMER_GETTICKTYPEQUARTER(NowValue)     == OS_OS_TICKTYPE_QUARTER3) &&                  /* PRQA S 4304 */ /* MD_Os_C90BooleanCompatibility */
                             (OS_TIMER_GETTICKTYPEQUARTER(CompareValue) == OS_OS_TICKTYPE_QUARTER0)    );
}


/***********************************************************************************************************************
 *  Os_TimerFrtGetLongValueMask()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_Hal_TimerFrtTickType   /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */ /* COV_OS_HALPLATFORMFRT64BITAVAILABILITY */
Os_TimerFrtGetLongValueMask (void)                                                                                     /* COV_OS_HALPLATFORMFRT64BITAVAILABILITY */
{
  return (Os_Hal_TimerFrtTickType)(OS_FRT_TICKTYPE_MAX - OS_OS_TICKTYPE_MAX);                                           /* PRQA S 2910 */ /* MD_Os_Rule12.4_2910 */
}


# define OS_STOP_SEC_CODE
# include "Os_MemMap_OsCode.h"                                                                                          /* PRQA S 5087 */ /* MD_MSR_MemMap */


#endif /* OS_TIMER_H */

/* module specific MISRA deviations:
 */

/*!
 * \}
 */
/**********************************************************************************************************************
 *  END OF FILE: Os_TimerInt.h
 *********************************************************************************************************************/
