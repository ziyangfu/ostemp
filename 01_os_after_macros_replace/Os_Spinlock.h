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
 * \addtogroup Os_Spinlock
 *
 * \{
 *
 * \file
 * \brief       User API for Spinlocks.
 * \details     --no details--
 *
 *
 **********************************************************************************************************************/
/***********************************************************************************************************************
 *  REVISION HISTORY
 *  --------------------------------------------------------------------------------------------------------------------
 *  Refer to Os.h.
 **********************************************************************************************************************/

#ifndef OS_SPINLOCK_H
# define OS_SPINLOCK_H
                                                                                                                         /* PRQA S 0388 EOF */ /* MD_MSR_Dir1.1 */
/***********************************************************************************************************************
 *  INCLUDES
 **********************************************************************************************************************/
/* AUTOSAR includes */
# include "Std_Types.h"

/* Os module declarations */
# include "Os_SpinlockInt.h"


/* Os kernel module dependencies */
# include "OsInt.h"
# include "Os_Lcfg.h"
# include "Os_Common.h"
# include "Os_Lock.h"
# include "Os_Error.h"
# include "Os_Core.h"
# include "Os_Interrupt.h"
# include "Os_Trace.h"
# include "Os_Thread.h"
# include "Os_Scheduler.h"

/* Os hal dependencies */
# include "Os_Hal_Compiler.h"
# include "Os_Hal_Core.h"



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
 *  Os_SpinlockIsEnabled()
 **********************************************************************************************************************/
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
OS_ALWAYS_INLINE static inline Os_StdReturnType   Os_SpinlockIsEnabled (void)
{
  return (Os_StdReturnType)((uint32)OS_SPINLOCKID_COUNT > 0u);                                                          /* PRQA S 4304, 2995 */ /* MD_Os_C90BooleanCompatibility, MD_Os_Rule2.2_2995*/
}


/***********************************************************************************************************************
 *  Os_SpinlockListInit()
 **********************************************************************************************************************/
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
OS_ALWAYS_INLINE static inline void   Os_SpinlockListInit (
    Os_SpinlockListType OS_CONST * LockList
)
{
  *LockList = OS_SPINLOCK_LIST_END;                                                                                     /* SBSW_OS_PWA_PRECONDITION */
}


/***********************************************************************************************************************
 *  Os_SpinlockInit()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline void   /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
Os_SpinlockInit (
  const   Os_SpinlockConfigType OS_CONST * Spinlock
)
{
    Os_SpinlockType OS_VAR_NOINIT * spinlockDyn;

  spinlockDyn = Os_SpinlockGetDyn(Spinlock);                                                                            /* SBSW_OS_FC_PRECONDITION */

  /* #10 Initialize the underlying lock. */
  Os_LockInit(&(Spinlock->Lock));                                                                                       /* SBSW_OS_FC_PRECONDITION */

  /* #20 Set owner thread to "no owner". */
  spinlockDyn->OwnerThread = ((void *)0);                                                                                  /* SBSW_OS_SL_SPINLOCKGETDYN_001 */

  /* #30 Initialize the HAL spinlock. */
  Os_Hal_SpinInit(&(spinlockDyn->Spinlock));                                                                            /* SBSW_OS_SL_HAL_SPININIT_001 */
}


/***********************************************************************************************************************
 *  Os_SpinlockGetDyn()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
  Os_SpinlockType OS_VAR_NOINIT *   Os_SpinlockGetDyn (
  const   Os_SpinlockConfigType OS_CONST * Spinlock
)
{
  return (  Os_SpinlockType OS_VAR_NOINIT *)Os_LockGetDyn(&(Spinlock->Lock));                            /* PRQA S 0310 */ /* MD_Os_Rule11.3_0310 */ /* SBSW_OS_FC_PRECONDITION */

}


/***********************************************************************************************************************
 *  Os_SpinlockCheckId()
 **********************************************************************************************************************/
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
OS_ALWAYS_INLINE static inline Os_StdReturnType   Os_SpinlockCheckId (
  SpinlockIdType SpinlockId
)
{
  return Os_ErrExtendedCheck(Os_ErrIsValueLo((uint32)SpinlockId, (uint32)OS_SPINLOCKID_COUNT));
}


/***********************************************************************************************************************
 *  Os_SpinlockId2Spinlock()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
const   Os_SpinlockConfigType OS_CONST *   Os_SpinlockId2Spinlock (
  SpinlockIdType SpinlockId
)
{
  Os_Assert(Os_ErrIsValueLo((uint32)SpinlockId, (uint32)OS_SPINLOCKID_COUNT));
  return OsCfg_SpinlockRefs[SpinlockId];
}


/***********************************************************************************************************************
 *  Os_SpinlockGetAccessingApplications()
 **********************************************************************************************************************/
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
OS_ALWAYS_INLINE static inline Os_AppAccessMaskType   Os_SpinlockGetAccessingApplications (
  const   Os_SpinlockConfigType OS_CONST * Spinlock
)
{
  return Os_LockGetAccessingApplications(&Spinlock->Lock);                                                              /* SBSW_OS_FC_PRECONDITION */
}


/***********************************************************************************************************************
 *  Os_SpinlockInternalGet()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline void   /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */ /* COV_OS_HALPLATFORMMULTICOREUNSUPPORTED */
Os_SpinlockInternalGet (
  const   Os_SpinlockConfigType OS_CONST * Spinlock
)                                                                                                                      /* COV_OS_HALPLATFORMMULTICOREUNSUPPORTED */
{
  const   Os_SpinlockType OS_VAR_NOINIT * spinlockDyn;
  Os_Hal_SpinlockResultType result;

  spinlockDyn = Os_SpinlockGetDyn(Spinlock);                                                                            /* SBSW_OS_FC_PRECONDITION */

  /* #10 Inform the trace module */
  Os_TraceSpinlockRequested(Spinlock->TimingHookTrace);                                                                 /* SBSW_OS_SL_TRACESPINLOCKREQUESTED_001 */

  /* #20 Loop. */
  do
  {
    /* #30 Spin while the spinlock is locked. */
    while(Os_Hal_SpinIsLocked(&(spinlockDyn->Spinlock)) != 0u)                                                          /* PRQA S 2994, 2996 */ /* MD_Os_Rule14.3_2994, MD_Os_Rule2.2_2996 */ /* SBSW_OS_SL_HAL_SPINISLOCKED_001 */
    {
      Os_Hal_CoreNop();
    }

    /* #40 Try to get the lock. */
    result = Os_SpinlockInternalTryGet(Spinlock);                                                                       /* SBSW_OS_FC_PRECONDITION */

  /* #50 Loop while locking failed. */
  } while(result != OS_HAL_SPINLOCKSUCCEEDED);                                                                          /* PRQA S 2993, 2996 */ /* MD_Os_Rule14.3_2993, MD_Os_Rule2.2_2996 */ /* COV_OS_SPINLOCKRACECONDITION */
}


/***********************************************************************************************************************
 *  Os_SpinlockInternalTryGet()
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
 */
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */ /* COV_OS_HALPLATFORMMULTICOREUNSUPPORTED */
OS_ALWAYS_INLINE static inline Os_Hal_SpinlockResultType   Os_SpinlockInternalTryGet (
  const   Os_SpinlockConfigType OS_CONST * Spinlock
)                                                                                                                      /* COV_OS_HALPLATFORMMULTICOREUNSUPPORTED */
{
  Os_Hal_SpinlockResultType result;
    Os_SpinlockType OS_VAR_NOINIT * spinlockDyn;

  spinlockDyn = Os_SpinlockGetDyn(Spinlock);                                                                            /* SBSW_OS_FC_PRECONDITION */

  /* #10 If killing is enabled: */
  if(Os_ErrIsKillingEnabled() != 0u)                                                                                    /* COV_OS_HALPLATFORMTESTEDASMULTICORE */ /* PRQA S 2991, 2995 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule2.2_2995 */
  {
    Os_IntStateType interruptState;

    /* #20 Suspend all interrupts including timing protection. */
    Os_IntSuspend(&interruptState);                                                                                     /* SBSW_OS_FC_POINTER2LOCAL */

    /* #30 Try to get the lock. */
    result = Os_Hal_SpinTryLock(&(spinlockDyn->Spinlock));                                                              /* SBSW_OS_SL_HAL_SPINTRYLOCK_001 */

    /* #40 On success: */
    if(result == OS_HAL_SPINLOCKSUCCEEDED)                                                                              /* PRQA S 2991, 2995 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule2.2_2995 */
    {
      /* #50 Set the lock owner. */
      spinlockDyn->OwnerThread = Os_CoreGetThread();                                                                    /* SBSW_OS_SL_SPINLOCKGETDYN_001 */
    }

    /* #60 Resume interrupts. */
    Os_IntResume(&interruptState);                                                                                      /* SBSW_OS_FC_POINTER2LOCAL */
  }
  /* #70 Else try to get the lock. */
  else
  {
    result = Os_Hal_SpinTryLock(&(spinlockDyn->Spinlock));                                                              /* SBSW_OS_SL_HAL_SPINTRYLOCK_001 */
  }

  /* #80 On Success inform the trace module. */
  if(result == OS_HAL_SPINLOCKSUCCEEDED)                                                                                /* PRQA S 2991, 2995 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule2.2_2995 */
  {
    Os_TraceSpinlockTaken(Spinlock->TimingHookTrace);                                                                   /* SBSW_OS_SL_TRACESPINLOCKTAKEN_001 */
  }

  /* #90 Return result. */
  return result;
}


/***********************************************************************************************************************
 *  Os_SpinlockInternalRelease()
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
OS_ALWAYS_INLINE static inline void   /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */ /* COV_OS_HALPLATFORMMULTICOREUNSUPPORTED */
Os_SpinlockInternalRelease (
  const   Os_SpinlockConfigType OS_CONST * Spinlock
)                                                                                                                      /* COV_OS_HALPLATFORMMULTICOREUNSUPPORTED */
{
    Os_SpinlockType OS_VAR_NOINIT * spinlockDyn;

  spinlockDyn = Os_SpinlockGetDyn(Spinlock);                                                                            /* SBSW_OS_FC_PRECONDITION */

  /* #10 If killing is enabled: */
  if(Os_ErrIsKillingEnabled() != 0u)                                                                                    /* COV_OS_HALPLATFORMTESTEDASMULTICORE */ /* PRQA S 2991, 2995 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule2.2_2995 */
  {
    Os_IntStateType interruptState;

    Os_Assert((Os_StdReturnType)(spinlockDyn->OwnerThread == Os_CoreGetThread()));                                      /* PRQA S 4304 */ /* MD_Os_C90BooleanCompatibility */

    /* #20 Suspend interrupts. */
    Os_IntSuspend(&interruptState);                                                                                     /* SBSW_OS_FC_POINTER2LOCAL */

    /* #30 Inform the trace module. */
    Os_TraceSpinlockReleased(Spinlock->TimingHookTrace);                                                                /* SBSW_OS_SL_TRACESPINLOCKRELEASED_001 */

    /* #40 Delegate to the HAL. */
    Os_Hal_SpinUnlock(&(spinlockDyn->Spinlock));                                                                        /* SBSW_OS_SL_HAL_SPINUNLOCKED_001 */

    /* #50 Clear the owner. */
    spinlockDyn->OwnerThread = ((void *)0);                                                                                /* SBSW_OS_SL_SPINLOCKGETDYN_001 */

    /* #60 Resume interrupts. */
    Os_IntResume(&interruptState);                                                                                      /* SBSW_OS_FC_POINTER2LOCAL */
  }
  else
  {
    /* #70 Else inform the trace module. */
    Os_TraceSpinlockReleased(Spinlock->TimingHookTrace);                                                                /* SBSW_OS_SL_TRACESPINLOCKRELEASED_001 */

    /* #80 Delegate to the HAL. */
    Os_Hal_SpinUnlock(&(spinlockDyn->Spinlock));                                                                        /* SBSW_OS_SL_HAL_SPINUNLOCKED_001 */
  }

}                                                                                                                       /* PRQA S 6050 */ /* MD_MSR_STCAL */


# define OS_STOP_SEC_CODE
# include "Os_MemMap_OsCode.h"                                                                                          /* PRQA S 5087 */ /* MD_MSR_MemMap */





#endif /* OS_SPINLOCK_H */

/* module specific MISRA deviations:
 */

/*!
 * \}
 */
/***********************************************************************************************************************
 *  END OF FILE: Os_Spinlock.h
 **********************************************************************************************************************/
