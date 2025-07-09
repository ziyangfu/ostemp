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
 * \addtogroup Os_Isr
 * \{
 *
 * \file
 * \brief       OS services concerning category 2 ISRs.
 * \details     --no details--
 *
 **********************************************************************************************************************/
/***********************************************************************************************************************
 *  REVISION HISTORY
 *  --------------------------------------------------------------------------------------------------------------------
 *  Refer to Os.h.
 **********************************************************************************************************************/

#ifndef OS_ISR_H
# define OS_ISR_H
                                                                                                                        /* PRQA S 0388 EOF */ /* MD_MSR_Dir1.1 */

/***********************************************************************************************************************
 *  INCLUDES
 **********************************************************************************************************************/
/* AUTOSAR includes */
# include "Std_Types.h"

/* Os module dependencies */
# include "Os_IsrInt.h"
# include "Os_Isr_Lcfg.h"

/* Os kernel module dependencies */
# include "Os_Common.h"
# include "Os_Thread.h"
# include "Os_Lcfg.h"
# include "Os_AccessCheck.h"
# include "Os_XSignal.h"
# include "Os_Timer.h"

/* Os Hal dependencies */
# include "Os_Hal_Compiler.h"
# include "Os_Hal_Interrupt.h"


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
 *  Os_IsrThread2Isr()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
const   Os_IsrConfigType OS_CONST *   Os_IsrThread2Isr (
  const   Os_ThreadConfigType OS_CONST * Thread
)
{
  Os_Assert((Os_StdReturnType)(Os_ThreadIsIsr(Thread) != 0u));                                                          /* PRQA S 4304 */ /* MD_Os_C90BooleanCompatibility */ /* SBSW_OS_FC_PRECONDITION */
  return (const   Os_IsrConfigType OS_CONST *)Thread;                                                        /* PRQA S 0310 */ /* MD_Os_Rule11.3_0310 */
}


/***********************************************************************************************************************
 *  Os_IsrId2Isr()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
const   Os_IsrConfigType OS_CONST *   Os_IsrId2Isr (
  ISRType IsrId
)
{
  Os_Assert(Os_ErrIsValueLo((uint32)IsrId, (uint32)OS_ISRID_COUNT));
  return OsCfg_IsrRefs[IsrId];                                                                                          /* PRQA S 2842 */ /* MD_Os_Rule18.1_2842 */
}


/***********************************************************************************************************************
 *  Os_IsrCheckId()
 **********************************************************************************************************************/
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
OS_ALWAYS_INLINE static inline Os_StdReturnType   Os_IsrCheckId (
  ISRType IsrId
)
{
  return Os_ErrExtendedCheck(Os_ErrIsValueLo((uint32)IsrId, (uint32)OS_ISRID_COUNT));
}


/***********************************************************************************************************************
 *  Os_IsrHalIntLevel2CoreIntLevel()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline void   /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
Os_IsrHalIntLevel2CoreIntLevel (
    Os_IntLevelType   * Level,
  const   Os_Hal_IntLevelType OS_CONST * HalLevel
)
{
  Level->State = *HalLevel;                                                                                             /* SBSW_OS_PWA_PRECONDITION */
}


/***********************************************************************************************************************
 *  Os_IsrLevelIsLe()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE                                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
static inline Os_StdReturnType   Os_IsrLevelIsLe (
  const   Os_IsrConfigType OS_CONST * Isr,
  const   Os_IntLevelType OS_CONST * Level
)
{
  return Os_Hal_IntIsrLevelIsLe(Isr->SourceConfig->HwConfig, &(Level->State));                                          /* SBSW_OS_ISR_HAL_INTISRLEVELISLE_001 */
}


/***********************************************************************************************************************
 *  Os_IsrGetThread()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
const   Os_ThreadConfigType OS_CONST *   Os_IsrGetThread (
  const   Os_IsrConfigType OS_CONST * Isr
)
{
  return &(Isr->Thread);
}


/***********************************************************************************************************************
 *  Os_IsrGetAccessRights()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
const   Os_AccessCheckConfigType OS_CONST *   Os_IsrGetAccessRights (
  const   Os_IsrConfigType OS_CONST * Isr
)
{
  const   Os_ThreadConfigType OS_CONST * thread;

  thread = Os_IsrGetThread(Isr);                                                                                        /* SBSW_OS_FC_PRECONDITION */
  return Os_ThreadGetAccessRights(thread);                                                                              /* SBSW_OS_ISR_THREADGETACCESSRIGHTS_001 */
}


/***********************************************************************************************************************
 *  Os_IsrSuicide()
 **********************************************************************************************************************/
OS_NORETURN OS_ALWAYS_INLINE                                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
static inline void   Os_IsrSuicide (void)
{
  Os_IsrEpilogue();
}


/***********************************************************************************************************************
 *  Os_IsrDisable()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE                                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
static inline void   Os_IsrDisable (
  const   Os_IsrConfigType OS_CONST * Isr
)
{
  Os_IsrDisableSource(Isr->SourceConfig);                                                                               /* SBSW_OS_ISR_DISABLESOURCE_001 */
}


/***********************************************************************************************************************
 *  Os_IsrCat1Disable()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE                                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
static inline void   Os_IsrCat1Disable (
  const   Os_IsrCat1ConfigType OS_CONST * Isr
)
{
  Os_IsrDisableSource(Isr->SourceConfig);                                                                               /* SBSW_OS_ISR_DISABLESOURCE_001 */
}


/***********************************************************************************************************************
 *  Os_IsrDisableSource()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline void   /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
Os_IsrDisableSource (
  const   Os_IsrHwConfigType OS_CONST * Isr
)
{
  /* #10 If the given interrupt is mapped. */
  if((Os_Hal_IntIsInterruptMappingSupported() != 0u) && (Isr->IsMapped == TRUE))                                        /* PRQA S 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */ /* COV_OS_HALPLATFORMISRMAPPINGAVAILABILITY */
  {
    Os_Hal_IntDisableSourceMapped(Isr->HwConfig, Isr->MapConfig);                                                       /* SBSW_OS_ISR_HAL_INTDISABLESOURCEMAPPED_001 */
  }
  /* #20 Otherwise. */
  else
  {
    Os_Hal_IntDisableSource(Isr->HwConfig);                                                                             /* SBSW_OS_ISR_HAL_INTDISABLESOURCE_001 */
  }
}


/***********************************************************************************************************************
 *  Os_IsrInterruptSourceInit()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline void   /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
Os_IsrInterruptSourceInit (
  const   Os_IsrHwConfigType OS_CONST * Isr
)
{
  /* #10 If the given interrupt is mapped. */
  if((Os_Hal_IntIsInterruptMappingSupported() != 0u) && (Isr->IsMapped == TRUE))                                        /* PRQA S 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */ /* COV_OS_HALPLATFORMISRMAPPINGAVAILABILITY */
  {
    Os_Hal_IntInterruptSourceInitMapped(Isr->HwConfig, Isr->MapConfig);                                                 /* SBSW_OS_ISR_HAL_INTINTERRUPTSOURCEINITMAPPED_001 */
  }
  /* #20 Otherwise. */
  else
  {
    Os_Hal_IntInterruptSourceInit(Isr->HwConfig);                                                                       /* SBSW_OS_ISR_HAL_INTINTERRUPTSOURCEINIT_001 */
  }
}


/***********************************************************************************************************************
 *  Os_IsrInitialEnable()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline void   Os_IsrInitialEnable (
  const   Os_IsrConfigType OS_CONST * Isr,
  boolean ClearPending
)
{
  /*#10 If ISR shall be enabled on initialization  */
  if(Isr->IsEnabledOnInitialization != FALSE)
  {
    /* #20 Clear the pending flag if requested. */
    if(ClearPending == TRUE)                                                                                            /* COV_OS_HALPLATFORMCLEARPENDINGAVAILABILITY */
    {
      Os_IsrClearPending(Isr->SourceConfig);                                                                            /* SBSW_OS_ISR_CLEARPENDING_002 */
    }

    /* #30 Enable the ISR source. */
    Os_IsrEnable(Isr);                                                                                                  /* SBSW_OS_FC_PRECONDITION */
  }/* if IsEnabledOnInitialization */

}


/***********************************************************************************************************************
 *  Os_IsrEnable()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline void   Os_IsrEnable (
  const   Os_IsrConfigType OS_CONST * Isr
)
{
  /* #10 If the given interrupt is mapped. */
  if((Os_Hal_IntIsInterruptMappingSupported() != 0u) && (Isr->SourceConfig->IsMapped == TRUE))                          /* PRQA S 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */ /* COV_OS_HALPLATFORMISRMAPPINGAVAILABILITY */
  {
    Os_Hal_IntEnableSourceMapped(Isr->SourceConfig->HwConfig, Isr->SourceConfig->MapConfig);                            /* SBSW_OS_ISR_HAL_INTENABLESOURCEMAPPED_001 */
  }
  /* #20 Otherwise. */
  else
  {
    Os_Hal_IntEnableSource(Isr->SourceConfig->HwConfig);                                                                /* SBSW_OS_ISR_HAL_INTENABLESOURCE_001 */
  }
}


/***********************************************************************************************************************
 *  Os_IsrClearPending()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline void   /* COV_OS_HALPLATFORMCLEARPENDINGAVAILABILITY */
Os_IsrClearPending (
  const   Os_IsrHwConfigType OS_CONST * Isr
)                                                                                                                      /* COV_OS_HALPLATFORMCLEARPENDINGAVAILABILITY */
{
  /* #10 If the given interrupt is mapped. */
  if((Os_Hal_IntIsInterruptMappingSupported() != 0u) && (Isr->IsMapped == TRUE))                                        /* PRQA S 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */ /* COV_OS_HALPLATFORMISRMAPPINGAVAILABILITY */
  {
    Os_Hal_IntClearPendingMapped(Isr->HwConfig, Isr->MapConfig);                                                        /* SBSW_OS_ISR_HAL_INTCLEARPENDINGMAPPED_001 */
  }
  /* #20 Otherwise. */
  else
  {
    Os_Hal_IntClearPending(Isr->HwConfig);                                                                              /* SBSW_OS_ISR_HAL_INTCLEARPENDING_001 */
  }
}


# define OS_STOP_SEC_CODE
# include "Os_MemMap_OsCode.h"                                                                                          /* PRQA S 5087 */ /* MD_MSR_MemMap */


#endif /* OS_ISR_H */

/* module specific MISRA deviations:
 */

/*!
 * \}
 */
/***********************************************************************************************************************
 *  END OF FILE: Os_Isr.h
 **********************************************************************************************************************/
