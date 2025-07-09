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
 * \addtogroup Os_Counter Counter
 * \{
 *
 * \file
 * \brief       OS services for counters.
 * \details     --no details--
 *
 *
 **********************************************************************************************************************/
/***********************************************************************************************************************
 *  REVISION HISTORY
 *  --------------------------------------------------------------------------------------------------------------------
 *  Refer to Os.h.
 **********************************************************************************************************************/

#ifndef OS_COUNTER_H
# define OS_COUNTER_H
                                                                                                                         /* PRQA S 0388 EOF */ /* MD_MSR_Dir1.1 */
/***********************************************************************************************************************
 *  INCLUDES
 **********************************************************************************************************************/
/* AUTOSAR includes */
# include "Std_Types.h"

/* Os module declarations */
# include "Os_CounterInt.h"
# include "Os_Counter_Cfg.h"
# include "Os_Counter_Lcfg.h"

/* Os kernel module dependencies */
# include "Os_Common.h"
# include "Os_PriorityQueue.h"
# include "OsInt.h"
# include "Os_Lcfg.h"
# include "Os_Error.h"
# include "Os_Application.h"
# include "Os_Core.h"
# include "Os_Timer.h"
# include "Os_Interrupt.h"

/* Os hal dependencies */
# include "Os_Hal_Compiler.h"



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

/***********************************************************************************************************************
 *  GLOBAL FUNCTIONS
 **********************************************************************************************************************/

/***********************************************************************************************************************
 *  Os_CounterHasPfrtCounter()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StdReturnType   Os_CounterHasPfrtCounter /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */ /* COV_OS_HALPLATFORMPFRTUNSUPPORTED_XX */
(
  void
)                                                                                                                      /* COV_OS_HALPLATFORMPFRTUNSUPPORTED_XX */
{
  return (Os_StdReturnType)(OS_CFG_COUNTER_PFRT_USED == STD_ON);                                                        /* PRQA S 4304, 2995, 2996 */ /* MD_Os_C90BooleanCompatibility, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
}


/***********************************************************************************************************************
 *  Os_CounterHasHrtCounter()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StdReturnType   Os_CounterHasHrtCounter /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */ /* COV_OS_HALPLATFORMFRTUNSUPPORTED_XX */
(
  void
)                                                                                                                      /* COV_OS_HALPLATFORMFRTUNSUPPORTED_XX */
{
  return (Os_StdReturnType)(OS_CFG_COUNTER_HRT_USED == STD_ON);                                                         /* PRQA S 2995, 2996, 4304 */ /* MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996, MD_Os_C90BooleanCompatibility */
}


/***********************************************************************************************************************
 *  Os_CounterHasPitCounter()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StdReturnType   Os_CounterHasPitCounter /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */ /* COV_OS_HALPLATFORMPITUNSUPPORTED_XX */
(
  void
)                                                                                                                      /* COV_OS_HALPLATFORMPITUNSUPPORTED_XX */
{
  return (Os_StdReturnType)(OS_CFG_COUNTER_PIT_USED == STD_ON);                                                         /* PRQA S 4304, 2995, 2996 */ /* MD_Os_C90BooleanCompatibility, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
}


/***********************************************************************************************************************
 *  Os_CounterCheckId()
 **********************************************************************************************************************/
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
OS_ALWAYS_INLINE static inline Os_StdReturnType   Os_CounterCheckId (
  CounterType CounterId
)
{
  return Os_ErrExtendedCheck(Os_ErrIsValueLo((uint32)CounterId, (uint32)OS_COUNTERID_COUNT));
}


/***********************************************************************************************************************
 *  Os_CounterId2Counter()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
const   Os_CounterConfigType OS_CONST *   Os_CounterId2Counter (
  CounterType CounterId
)
{
  Os_Assert(Os_ErrIsValueLo((uint32)CounterId, (uint32)OS_COUNTERID_COUNT));
  return OsCfg_CounterRefs[CounterId];
}


/***********************************************************************************************************************
 *  Os_CounterGetApplication()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE                                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
static inline const   Os_AppConfigType OS_CONST *   Os_CounterGetApplication (
  const   Os_CounterConfigType OS_CONST * Counter
)
{
  return Counter->OwnerApplication;
}


/***********************************************************************************************************************
 *  Os_CounterGetAccessingApplications()
 **********************************************************************************************************************/
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
OS_ALWAYS_INLINE static inline Os_AppAccessMaskType   Os_CounterGetAccessingApplications (
  const   Os_CounterConfigType OS_CONST * Counter
)
{
  return Counter->AccessingApplications;
}


/***********************************************************************************************************************
 *  Os_CounterGetCore()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
const   Os_CoreAsrConfigType OS_CONST *   Os_CounterGetCore (
  const   Os_CounterConfigType OS_CONST * Counter
)
{
  return Counter->Core;
}


/***********************************************************************************************************************
 *  Os_CounterIsFutureValue()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 */
OS_ALWAYS_INLINE static inline Os_StdReturnType   Os_CounterIsFutureValue (
  const   Os_CounterConfigType OS_CONST * Counter,
  Os_TickType Value,
  Os_TickType ReferenceValue
)
{
  /* #10 Fetch result from internal implementation. */
  return Os_CounterIsFutureValueInternal( Counter->Characteristics.MaxCountingValue,
                                          Counter->Characteristics.MaxDifferentialValue,
                                          Value,
                                          ReferenceValue);
}


/***********************************************************************************************************************
 *  Os_CounterIsFutureValueInternal()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline Os_StdReturnType   Os_CounterIsFutureValueInternal (
  Os_TickType MaxCountingValue,
  Os_TickType MaxDifferentialValue,
  Os_TickType Value,
  Os_TickType ReferenceValue
)
{
  Os_StdReturnType result;
  Os_TickType diffTime;

  Os_Assert((Os_StdReturnType)(Value <= MaxCountingValue));                                                             /* PRQA S 4304 */ /* MD_Os_C90BooleanCompatibility */
  Os_Assert((Os_StdReturnType)(ReferenceValue <= MaxCountingValue));                                                    /* PRQA S 4304 */ /* MD_Os_C90BooleanCompatibility */

  /* - #10 Subtract the higher from the lower value and return whether the difference is greater
   *       than the MaxDifferentialValue or not. */
  if(Value > ReferenceValue)
  {
    diffTime = Value - ReferenceValue;

    /* If the left job is less than or equal to MaxDifferentialValue ticks ahead of the right job,
     * then the left job lies in the future of the right job.
     */
    result = (Os_StdReturnType)(diffTime <= MaxDifferentialValue);                                                      /* PRQA S 4304 */ /* MD_Os_C90BooleanCompatibility */
  }
  else if(Value < ReferenceValue)
  {
    diffTime = ReferenceValue - Value;

    /* If the right job is less than or equal to MaxDifferentialValue ticks ahead of the left job,
     * then the left job lies in the past of the right job.
     */
    result = (Os_StdReturnType)(diffTime > MaxDifferentialValue);                                                       /* PRQA S 4304 */ /* MD_Os_C90BooleanCompatibility */
  }
  else
  {
    /* Value == ReferenceValue => Value is not in the future of ReferenceValue. */
    result = 0;
  }

  return result;
}


/***********************************************************************************************************************
 *  Os_CounterCheckValueLeMaxAllowed()
 **********************************************************************************************************************/
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
OS_ALWAYS_INLINE static inline Os_StdReturnType   Os_CounterCheckValueLeMaxAllowed (
  const   Os_CounterConfigType OS_CONST * Counter,
  TickType Value
)
{
  return Os_ErrExtendedCheck((Os_StdReturnType)(Value <= Counter->Characteristics.MaxAllowedValue));                    /* PRQA S 4304 */ /* MD_Os_C90BooleanCompatibility */
}


/***********************************************************************************************************************
 *  Os_CounterCheckValueLeReducedMaxAllowed()
 **********************************************************************************************************************/
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
OS_ALWAYS_INLINE static inline Os_StdReturnType   Os_CounterCheckValueLeReducedMaxAllowed (
  const   Os_CounterConfigType OS_CONST * Counter,
  TickType Reduce,
  TickType Value
)
{
  Os_Assert((Os_StdReturnType)(Reduce <= Counter->Characteristics.MaxAllowedValue));                                    /* PRQA S 4304 */ /* MD_Os_C90BooleanCompatibility */
  return Os_ErrExtendedCheck((Os_StdReturnType)(Value <= (Counter->Characteristics.MaxAllowedValue - Reduce)));         /* PRQA S 4304 */ /* MD_Os_C90BooleanCompatibility */
}


/***********************************************************************************************************************
 *  Os_CounterCheckValueGeMinCycleOrZero()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE                                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
static inline Os_StdReturnType   Os_CounterCheckValueGeMinCycleOrZero (
  const   Os_CounterConfigType OS_CONST * Counter,
  TickType Cycle
)
{
  return Os_ErrExtendedCheck((Os_StdReturnType)((Cycle >= Counter->Characteristics.MinCycle) || (Cycle == 0u)));        /* PRQA S 4304 */ /* MD_Os_C90BooleanCompatibility */
}


/***********************************************************************************************************************
 *  Os_CounterGetCharacteristics()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE                                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
static inline const   Os_CounterCharacteristicsConfigType OS_CONST *   Os_CounterGetCharacteristics (
  const   Os_CounterConfigType OS_CONST * Counter
)
{
  return &(Counter->Characteristics);
}


/***********************************************************************************************************************
 *  Os_CounterGetMaxAllowedValue()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE                                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */ /* COV_OS_HALPLATFORMTPUNSUPPORTED_XX */
static inline Os_TickType   Os_CounterGetMaxAllowedValue (
  const   Os_CounterConfigType OS_CONST * Counter
)                                                                                                                      /* COV_OS_HALPLATFORMTPUNSUPPORTED_XX */
{
  return Counter->Characteristics.MaxAllowedValue;
}


/***********************************************************************************************************************
 *  Os_CounterGetMaxCountingValue()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE                                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */ /* COV_OS_HALPLATFORMTPUNSUPPORTED_XX */
static inline Os_TickType   Os_CounterGetMaxCountingValue (
  const   Os_CounterConfigType OS_CONST * Counter
)                                                                                                                      /* COV_OS_HALPLATFORMTPUNSUPPORTED_XX */
{
  return Counter->Characteristics.MaxCountingValue;
}


# define OS_STOP_SEC_CODE
# include "Os_MemMap_OsCode.h"                                                                                          /* PRQA S 5087 */ /* MD_MSR_MemMap */


#endif /* OS_COUNTER_H */

/* module specific MISRA deviations:
 */

/*!
 * \}
 */
/***********************************************************************************************************************
 *  END OF FILE: Os_Counter.h
 **********************************************************************************************************************/
