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
 * \addtogroup Os_Thread
 *
 * \{
 *
 * \file
 * \brief      Internal functions implementation to work with threads.
 * \details     --no details--
 *
 **********************************************************************************************************************/
/***********************************************************************************************************************
 *  REVISION HISTORY
 *  --------------------------------------------------------------------------------------------------------------------
 *  Refer to Os.h.
 **********************************************************************************************************************/


#ifndef OS_THREAD_H
# define OS_THREAD_H
                                                                                                                        /* PRQA S 0388 EOF */ /* MD_MSR_Dir1.1 */
/***********************************************************************************************************************
 *  INCLUDES
 **********************************************************************************************************************/
/* AUTOSAR includes */
# include "Std_Types.h"

/* Os module declarations */
# include "Os_ThreadInt.h"
# include "Os_Thread_Types.h"

/* Os module dependencies */
# include "Os_Common.h"
# include "Os_Stack.h"
# include "Os_Interrupt.h"
# include "Os_Core.h"
# include "Os_MemoryProtection.h"
# include "Os_TimingProtection.h"
# include "Os_Application.h"
# include "Os_Error.h"
# include "Os_Resource.h"
# include "Os_ServiceFunction.h"
# include "Os_XSignal_Cfg.h"
# include "Os_Trace.h"
# include "Os_Spinlock.h"
# include "Os_Lock.h"
# include "Os_XSignal.h"
# include "Os_AccessCheck.h"

/* Os Hal dependencies */
# include "Os_Hal_Compiler.h"
# include "Os_Hal_Context.h"


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
 *  Os_ThreadGetLocks()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE                                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
static inline   Os_LockListType OS_VAR_NOINIT *   Os_ThreadGetLocks (
  const   Os_ThreadConfigType OS_CONST * Thread
)
{
  return &(Thread->Dyn->Locks);
}


/***********************************************************************************************************************
 *  Os_ThreadGetSpinlocks()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE                                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
static inline   Os_SpinlockListType OS_VAR_NOINIT *   Os_ThreadGetSpinlocks (
  const   Os_ThreadConfigType OS_CONST * Thread
)
{
  return &(Thread->Dyn->Spinlocks);
}


/***********************************************************************************************************************
 *  Os_ThreadGetServiceCalls()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE                                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
static inline   Os_ServiceCallType OS_VAR_NOINIT *   Os_ThreadGetServiceCalls (
  const   Os_ThreadConfigType OS_CONST * Thread
)
{
  return Thread->Dyn->ServiceCalls;
}


/***********************************************************************************************************************
 *  Os_ThreadSetServiceCalls()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE                                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
static inline void   Os_ThreadSetServiceCalls (
  const   Os_ThreadConfigType OS_CONST * Thread,
    Os_ServiceCallType OS_VAR_NOINIT * ServiceCalls
)
{
  Thread->Dyn->ServiceCalls = ServiceCalls;                                                                             /* SBSW_OS_TRD_THREADGETDYN_001 */
}


/***********************************************************************************************************************
 *  Os_ThreadGetTrace()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE                                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
static inline const   Os_TraceThreadConfigType OS_CONST *   Os_ThreadGetTrace (
  const   Os_ThreadConfigType OS_CONST * Thread
)
{
  return Thread->Trace;
}


/***********************************************************************************************************************
 *  Os_ThreadGetTpConfig()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE                                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
static inline const   Os_TpThreadConfigType OS_CONST *   Os_ThreadGetTpConfig (
  const   Os_ThreadConfigType OS_CONST * Thread
)
{
  return Thread->TimeProtConfig;
}


/***********************************************************************************************************************
 *  Os_ThreadGetStack()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE                                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
static inline const   Os_StackConfigType OS_CONST *   Os_ThreadGetStack (
  const   Os_ThreadConfigType OS_CONST * Thread
)
{
  return (Thread->Stack);
}


/***********************************************************************************************************************
 *  Os_ThreadGetContext()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE                                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
static inline const   Os_Hal_ContextConfigType OS_CONST *   Os_ThreadGetContext (
  const   Os_ThreadConfigType OS_CONST * Thread
)
{
  return Thread->ContextConfig;
}


/***********************************************************************************************************************
 *  Os_ThreadGetIntApiState()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE                                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
static inline   Os_IntApiStateType OS_VAR_NOINIT *   Os_ThreadGetInterruptApiState (
  const   Os_ThreadConfigType OS_CONST * Thread
)
{
  return Thread->IntApiState;
}


/***********************************************************************************************************************
 *  Os_ThreadGetCore()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE                                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
static inline const   Os_CoreAsrConfigType OS_CONST *   Os_ThreadGetCore (
  const   Os_ThreadConfigType OS_CONST * Thread
)
{
  return Thread->Core;
}


/***********************************************************************************************************************
 *  Os_ThreadGetCurrentType()
 **********************************************************************************************************************/
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
OS_ALWAYS_INLINE static inline Os_CallContextType   Os_ThreadGetCurrentType (
  const   Os_ThreadConfigType OS_CONST * Thread
)
{
  return Thread->Dyn->CallContext;
}


/***********************************************************************************************************************
 *  Os_ThreadGetInitialType()
 **********************************************************************************************************************/
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
OS_ALWAYS_INLINE static inline Os_CallContextType   Os_ThreadGetInitialType (
  const   Os_ThreadConfigType OS_CONST * Thread
)
{
  return Thread->InitialCallContext;
}


/***********************************************************************************************************************
 *  Os_ThreadSwitchType()
 **********************************************************************************************************************/
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
OS_ALWAYS_INLINE static inline Os_CallContextType   Os_ThreadSwitchType (
  const   Os_ThreadConfigType OS_CONST * Thread,
  Os_CallContextType NextCallContext
)
{
  Os_CallContextType prevCallContext;

  prevCallContext = Thread->Dyn->CallContext;
  Thread->Dyn->CallContext = NextCallContext;                                                                           /* SBSW_OS_TRD_THREADGETDYN_001 */

  return prevCallContext;
}


/***********************************************************************************************************************
 *  Os_ThreadIsTask()
 **********************************************************************************************************************/
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
OS_ALWAYS_INLINE static inline Os_StdReturnType   Os_ThreadIsTask (
  const   Os_ThreadConfigType OS_CONST * Thread
)
{
  return ((Thread->InitialCallContext == OS_CALLCONTEXT_TASK) ||                                                        /* PRQA S  4404 */ /* MD_Os_C90BooleanCompatibility */
          (Thread->InitialCallContext == OS_CALLCONTEXT_PRESTARTTASK));
}


/***********************************************************************************************************************
 *  Os_ThreadIsTpIsr()
 **********************************************************************************************************************/
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
OS_ALWAYS_INLINE static inline Os_StdReturnType   Os_ThreadIsTpIsr (
  const   Os_ThreadConfigType OS_CONST * Thread
)
{
  return (Thread->InitialCallContext == OS_CALLCONTEXT_TPISR);                                                          /* PRQA S  4404 */ /* MD_Os_C90BooleanCompatibility */
}


/***********************************************************************************************************************
 *  Os_ThreadIsIsr()
 **********************************************************************************************************************/
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
OS_ALWAYS_INLINE static inline Os_StdReturnType   Os_ThreadIsIsr (
  const   Os_ThreadConfigType OS_CONST * Thread
)
{
  return ((Os_StdReturnType)Thread->InitialCallContext & OS_CALLCONTEXT_ISR);
}


/***********************************************************************************************************************
 *  Os_ThreadIsAnyInitialContext()
 **********************************************************************************************************************/
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
OS_ALWAYS_INLINE static inline Os_StdReturnType   Os_ThreadIsAnyInitialContext (
  const   Os_ThreadConfigType OS_CONST * Thread,
  Os_CallContextType Context
)
{
  return ((Os_StdReturnType)Thread->InitialCallContext & (Os_StdReturnType)Context);
}


/***********************************************************************************************************************
 *  Os_ThreadIsShutdownHook()
 **********************************************************************************************************************/
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
OS_ALWAYS_INLINE static inline Os_StdReturnType   Os_ThreadIsShutdownHook (
  const   Os_ThreadConfigType OS_CONST * Thread
)
{
  return (Thread->InitialCallContext == OS_CALLCONTEXT_SHUTDOWNHOOK);                                                   /* PRQA S  4404 */ /*  MD_Os_C90BooleanCompatibility */
}


/***********************************************************************************************************************
 *  Os_ThreadIsHook()
 **********************************************************************************************************************/
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
OS_ALWAYS_INLINE static inline Os_StdReturnType   Os_ThreadIsHook (
  const   Os_ThreadConfigType OS_CONST * Thread
)
{
  return ((Os_StdReturnType)Thread->InitialCallContext & OS_CALLCONTEXT_ANYHOOK);
}


/***********************************************************************************************************************
 *  Os_ThreadCheckCallContext()
 **********************************************************************************************************************/
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
OS_ALWAYS_INLINE static inline Os_StdReturnType   Os_ThreadCheckCallContext (
  const   Os_ThreadConfigType OS_CONST * Thread,
  Os_CallContextType PermittedCallContext
)
{
  return Os_ErrExtendedCheck((Os_StdReturnType)Os_ThreadGetCurrentType(Thread) & PermittedCallContext);                 /* SBSW_OS_FC_PRECONDITION */
}


/***********************************************************************************************************************
 *  Os_ThreadCheckIsCurrentApplication()
 **********************************************************************************************************************/
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
OS_ALWAYS_INLINE static inline Os_StdReturnType   Os_ThreadCheckIsCurrentApplication (
  const   Os_ThreadConfigType OS_CONST * Thread,
  const   Os_AppConfigType OS_CONST * App
)
{
  return Os_ErrServiceProtectionCheck((Os_StdReturnType)(Os_ThreadGetCurrentApplication(Thread) == App));               /* PRQA S 4304 */ /* MD_Os_C90BooleanCompatibility */ /* SBSW_OS_FC_PRECONDITION */
}


/***********************************************************************************************************************
 *  Os_ThreadGetOwnerApplication()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE                                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
static inline const   Os_AppConfigType OS_CONST *   Os_ThreadGetOwnerApplication (
  const   Os_ThreadConfigType OS_CONST * Thread
)
{
  return Thread->OwnerApplication;
}


/***********************************************************************************************************************
 *  Os_ThreadGetCurrentApplication()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE                                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
static inline const   Os_AppConfigType OS_CONST *   Os_ThreadGetCurrentApplication (
  const   Os_ThreadConfigType OS_CONST * Thread
)
{
  return Thread->Dyn->Application;
}


/***********************************************************************************************************************
 *  Os_ThreadSetCurrentApplication()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE                                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
static inline void   Os_ThreadSetCurrentApplication (
  const   Os_ThreadConfigType OS_CONST * Thread,
  const   Os_AppConfigType OS_CONST * App
)
{
  Thread->Dyn->Application = App;                                                                                       /* SBSW_OS_TRD_THREADGETDYN_001 */
}


/***********************************************************************************************************************
 *  Os_ThreadHasResourcesLocked()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 */
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
OS_ALWAYS_INLINE static inline Os_StdReturnType   Os_ThreadHasResourcesLocked (
  const   Os_ThreadConfigType OS_CONST * Thread
)
{
  Os_StdReturnType hasResources;
  Os_LockListType it;

  hasResources = 0;
  it = Thread->Dyn->Locks;

  /* #10 Search the complete lock list */
  while(OS_UNLIKELY(Os_LockListIsEmpty(&it) == 0u))                                                                     /* SBSW_OS_TRD_LOCKLISTISEMPTY_001 */
  {
    /* #20 If one resource is found, stop searching and return 1 (0 in any other case). */
    if(Os_LockListIsTopRes(&it) != 0u)                                                                                  /* SBSW_OS_TRD_LOCKLISTISTOPRES_001 */
    {
      hasResources = 1;
      break;
    }

    it = Os_LockListNext(&it);                                                                                          /* SBSW_OS_TRD_LOCKLISTNEXT_001 */
  }

  return hasResources;
}


/***********************************************************************************************************************
 *  Os_ThreadHasSpinlockLocked()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 */
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
OS_ALWAYS_INLINE static inline Os_StdReturnType   Os_ThreadHasSpinlockLocked (
  const   Os_ThreadConfigType OS_CONST * Thread
)
{
  Os_StdReturnType hasSpinlocks;
  Os_LockListType it;

  hasSpinlocks = 0;
  it = Thread->Dyn->Locks;


  /* #10 Search the complete lock list */
  while(OS_UNLIKELY(Os_LockListIsEmpty(&it) == 0u))                                                                     /* SBSW_OS_TRD_LOCKLISTISEMPTY_001 */ /* COV_OS_HALPLATFORMTESTEDASMULTICORE */
  {
    /* #20 If one spinlock is found, stop searching and return 1 (0 in any other case). */
    if(Os_LockListIsTopSpinlock(&it) != 0u)                                                                             /* SBSW_OS_TRD_LOCKLISTISTOPSPINLOCK_001 */
    {
      hasSpinlocks = 1;
      break;
    }

    it = Os_LockListNext(&it);                                                                                          /* SBSW_OS_TRD_LOCKLISTNEXT_001 */
  }

  return hasSpinlocks;
}


/***********************************************************************************************************************
 *  Os_ThreadCheckResourcesReleased()
 **********************************************************************************************************************/
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
OS_ALWAYS_INLINE static inline Os_StdReturnType   Os_ThreadCheckResourcesReleased (
  const   Os_ThreadConfigType OS_CONST * Thread
)
{
  return Os_ErrExtendedCheck(Os_ThreadHasResourcesLocked(Thread) == 0u);                                                /* PRQA S 4404 */ /* MD_Os_C90BooleanCompatibility */ /* SBSW_OS_FC_PRECONDITION */
}


/***********************************************************************************************************************
 *  Os_ThreadCheckSpinlocksReleased()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StdReturnType /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
    Os_ThreadCheckSpinlocksReleased (
  const   Os_ThreadConfigType OS_CONST * Thread
)
{
  return Os_ErrExtendedCheck(Os_ThreadHasSpinlockLocked(Thread) == 0u);                                                 /* PRQA S 4404 */ /* MD_Os_C90BooleanCompatibility */ /* SBSW_OS_FC_PRECONDITION */
}


/***********************************************************************************************************************
 *  Os_ThreadCheckAreInterruptsEnabled()
 **********************************************************************************************************************/
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
OS_ALWAYS_INLINE static inline Os_StdReturnType   Os_ThreadCheckAreInterruptsEnabled (
  const   Os_ThreadConfigType OS_CONST * Thread
)
{
  return Os_InterruptCheckEnabled(Os_ThreadGetInterruptApiState(Thread));                                               /* SBSW_OS_TRD_INTERRUPTCHECKENABLED_001 */ /* SBSW_OS_FC_PRECONDITION */
}


/***********************************************************************************************************************
 *  Os_ThreadSpinlockForceRelease()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline void   /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
Os_ThreadSpinlockForceRelease (
  const   Os_ThreadConfigType OS_CONST * Thread
)
{
  Os_SpinlockForciblyReleaseThreadLocks(Thread);                                                                        /* SBSW_OS_FC_PRECONDITION */
}


/***********************************************************************************************************************
 *  Os_ThreadResourceForceRelease()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline void   /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
Os_ThreadResourceForceRelease (
  const   Os_ThreadConfigType OS_CONST * Thread
)
{
  Os_ResourceForciblyReleaseLocks(Thread);                                                                              /* SBSW_OS_FC_PRECONDITION */
}


/***********************************************************************************************************************
 *  Os_ThreadLocksForceRelease()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline void   /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
Os_ThreadLocksForceRelease (
  const   Os_ThreadConfigType OS_CONST * Thread
)
{
  /* #10 Clean up the interrupt state. */
  Os_InterruptCleanup(Thread->IntApiState);                                                                             /* SBSW_OS_TRD_INTERRUPTCLEANUP_001 */

  /* #20 Release all locks. */
  Os_ThreadResourceForceRelease(Thread);                                                                                /* SBSW_OS_FC_PRECONDITION */
  Os_ThreadSpinlockForceRelease(Thread);                                                                                /* SBSW_OS_FC_PRECONDITION */
}


/***********************************************************************************************************************
 *  Os_ThreadClassInit()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 */
OS_ALWAYS_INLINE static inline void   /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
Os_ThreadClassInit (
    Os_ThreadClassType OS_CONST * ThreadClass
)
{
  /* #10 Fetch initial FPU context from HAL if FPU context is enabled. */
  if(Os_ThreadIsFpuContextEnabled() != 0u)                                                                              /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
  {
    Os_Hal_ContextFetchInitialFpuContext(&(ThreadClass->InitialFpuContext));                                            /* SBSW_OS_FC_PRECONDITION */
  }
}


/***********************************************************************************************************************
 *  Os_ThreadInit()
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
 */
OS_ALWAYS_INLINE static inline void   /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
Os_ThreadInit (
  const   Os_ThreadConfigType OS_CONST * Thread
)
{
  /* #10 Initialize current application. */
  Thread->Dyn->Application = Thread->OwnerApplication;                                                                  /* SBSW_OS_TRD_THREADGETDYN_001 */

  /* #20 Initialize current context. */
  Thread->Dyn->CallContext = Thread->InitialCallContext;                                                                /* SBSW_OS_TRD_THREADGETDYN_001 */

  /* #30 Initialize service call list. */
  Os_ServiceCallsInit(&(Thread->Dyn->ServiceCalls));                                                                    /* SBSW_OS_TRD_SERVICECALLSINIT_001 */

  /* #40 Initialize the lock list. */
  Os_LockListInit(&(Thread->Dyn->Locks));                                                                               /* SBSW_OS_TRD_LOCKLISTINIT_001 */

  /* #50 Initialize spinlock order list. */
  Os_SpinlockListInit(Os_ThreadGetSpinlocks(Thread));                                                                   /* SBSW_OS_TRD_SPINLOCKLISTINIT_001 */ /* SBSW_OS_FC_PRECONDITION */

  /* #60 Initialize the timing protection. */
  {
    const   Os_TpThreadConfigType OS_CONST * currentTp;
    const   Os_CoreAsrConfigType OS_CONST * core;
    const   Os_TpConfigType OS_CONST * coreTp;

    core = Os_ThreadGetCore(Thread);                                                                                    /* SBSW_OS_FC_PRECONDITION */
    coreTp = Os_CoreAsrGetTimingProtection(core);                                                                       /* SBSW_OS_TRD_COREASRGETTIMINGPROTECTION_001 */
    currentTp = Os_ThreadGetTpConfig(Thread);                                                                           /* SBSW_OS_FC_PRECONDITION */

    Os_TpThreadInit(coreTp, currentTp);                                                                                 /* SBSW_OS_TRD_TPTHREADINIT_001 */
  }

  /* #70 Initialize the memory protection. */
  Os_MpThreadInit(Thread->MpAccessRightsInitial, &(Thread->Dyn->MpAccessRights));                                       /* SBSW_OS_TRD_MPTHREADINIT_001 */


  /* #80 If the given thread shall be initialized during startup: */
  if(Thread->InitDuringStartUp != FALSE)
  {
    /* #90 Initialize its hardware context, so that it will start with a fresh stack and at function entry,
     *     if resumed. */
    Os_Hal_ContextInit(Thread->ContextConfig, Thread->Context);                                                         /* SBSW_OS_TRD_HAL_CONTEXTINIT_001 */

    /* #95 Initialize the FPU context of all threads which use FPU at startup. */
    Os_ThreadInitFpuContext(Thread);                                                                                    /* SBSW_OS_FC_PRECONDITION */
  }


  /* #100 Initialize the tracing data of this thread. */
  Os_TraceThreadInit(Thread->Trace);                                                                                    /* SBSW_OS_TRD_TRACETHREADINIT_001 */
}                                                                                                                       /* PRQA S 6050 */ /* MD_MSR_STCAL */


/***********************************************************************************************************************
 *  Os_ThreadInit2()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline void   /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
Os_ThreadInit2 (
  const   Os_ThreadConfigType OS_CONST * Thread
)
{
  /* #10 Initialize current application. */
  Thread->Dyn->Application = Thread->OwnerApplication;                                                                  /* SBSW_OS_TRD_THREADGETDYN_001 */

  /* #20 Initialize the memory protection. */
  Os_MpThreadInit(Thread->MpAccessRightsInitial, &(Thread->Dyn->MpAccessRights));                                       /* SBSW_OS_TRD_MPTHREADINIT_001 */
}


/***********************************************************************************************************************
 *  Os_ThreadResetAndResume()
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
 */
OS_NORETURN OS_ALWAYS_INLINE static inline void   /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
Os_ThreadResetAndResume (
  const   Os_ThreadConfigType OS_CONST * Next,
  boolean CallPreThreadHook
)
{
  const   Os_ThreadConfigType OS_CONST * current;

  current = Os_CoreGetThread();

  /* Check that the function is called from a task. */
  Os_Assert((Os_StdReturnType)(Os_ThreadIsTask(current) != 0u));                                                        /* PRQA S 4304 */ /* MD_Os_C90BooleanCompatibility */ /* SBSW_OS_TRD_THREADISTASK_001 */

  /* #10 Release all services called by the thread. */
  Os_ServiceCallForceRelease(&(current->Dyn->ServiceCalls));                                                            /* SBSW_OS_TRD_SERVICECALLFORCERELEASE_001 */

  /* #20 Perform stack overflow check. */
  Os_StackOverflowCheck();

  /* #25 Initialize the FPU context of all threads which use FPU at startup. */
  Os_ThreadInitFpuContext(current);                                                                                     /* SBSW_OS_TRD_THREADINITFPUCONTEXT_001 */

  /* #30 Tell timing protection module to resume timing protection for Next. */
  {
    const   Os_CoreAsrConfigType OS_CONST * core;
    const   Os_TpConfigType OS_CONST * configTp;
    const   Os_TpThreadConfigType OS_CONST * nextTp;
    const   Os_TpThreadConfigType OS_CONST * currentTp;

    core = Os_ThreadGetCore(Next);                                                                                      /* SBSW_OS_FC_PRECONDITION */
    configTp = Os_CoreAsrGetTimingProtection(core);                                                                     /* SBSW_OS_TRD_COREASRGETTIMINGPROTECTION_001 */
    nextTp = Os_ThreadGetTpConfig(Next);                                                                                /* SBSW_OS_FC_PRECONDITION */
    currentTp = Os_ThreadGetTpConfig(current);                                                                          /* SBSW_OS_TRD_THREADGETTPCONFIG_001 */
    Os_TpResetAndResume(configTp, currentTp, nextTp, Next);                                                             /* SBSW_OS_TRD_TPRESETANDRESUME_001 */
  }

  /* #40 Resume memory protection access rights of Next. */
  Os_MpSwitch(current->Dyn->MpAccessRights, Next->Dyn->MpAccessRights);                                                 /* SBSW_OS_TRD_MPSWITCH_001 */

  /* #45 Set the current thread pointer. */
  Os_CoreSetThread(Next);                                                                                               /* SBSW_OS_FC_PRECONDITION */

  /* #50 Call PreThreadHook. */
  Os_ThreadCallPreThreadHook(Next, CallPreThreadHook);                                                                  /* SBSW_OS_FC_PRECONDITION */

  /* #55 Restore FPU context of next thread if needed. */
  Os_ThreadRestoreFpuContext(Next);                                                                                     /* SBSW_OS_FC_PRECONDITION */

  /* #60 Set the current stack pointer. */
  Os_CoreSetCurrentStack(Next->Stack, Next->Core);                                                                      /* SBSW_OS_TRD_CORESETCURRENTSTACK_001 */

  /* #70 Update the Msr of the next thread. */
  Os_Hal_ContextSetUserMsrBits(Next->Context);                                                                          /* SBSW_OS_TRD_HAL_CONTEXTSETUSERMSRBITS_001 */

  /* #80 Resume the hardware context of the next thread. */
  Os_Hal_ContextResetAndResume(current->ContextConfig, current->Context, Next->Context); /* function never returns! */  /* SBSW_OS_TRD_HAL_CONTEXTRESETANDRESUME_001 */

  /* Never reached */
  Os_ErrKernelPanic(); /* function never returns! */                                                                    /* PRQA S 2880 */ /* MD_Os_Rule2.1_2880_NonReturning */
}                                                                                                                       /* PRQA S 6050 */ /* MD_MSR_STCAL */


/***********************************************************************************************************************
 *  Os_ThreadStartFirst()
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
OS_ALWAYS_INLINE static inline void   /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
Os_ThreadStartFirst (
  const   Os_ThreadConfigType OS_CONST * Next,
  boolean CallPreThreadHook
)
{
  const   Os_TpConfigType OS_CONST * configTp;
  const   Os_TpThreadConfigType OS_CONST * nextTp;
  const   Os_TpThreadConfigType OS_CONST * currentTp;

  configTp = Os_CoreAsrGetTimingProtection(Os_ThreadGetCore(Next));                                                     /* SBSW_OS_TRD_COREASRGETTIMINGPROTECTION_001 */ /* SBSW_OS_FC_PRECONDITION */
  nextTp = Os_ThreadGetTpConfig(Next);                                                                                  /* SBSW_OS_FC_PRECONDITION */
  currentTp = Os_ThreadGetTpConfig(Os_CoreGetThread());                                                                 /* SBSW_OS_TRD_THREADGETTPCONFIG_001 */

  /* #10 Tell timing protection module to resume timing protection for Next. */
  Os_TpResetAndResume(configTp, currentTp, nextTp, Next);                                                               /* SBSW_OS_TRD_TPRESETANDRESUME_001 */

  /* #20 Set memory protection access rights of Next. */
  Os_MpSwitchTo(Next->Dyn->MpAccessRights);                                                                             /* SBSW_OS_TRD_MPSWITCHTO_001 */

  /* #25 Set the current thread pointer. */
  Os_CoreSetThread(Next);                                                                                               /* SBSW_OS_FC_PRECONDITION */

  /* #30 Call PreThreadHook. */
  Os_ThreadCallPreThreadHook(Next, CallPreThreadHook);                                                                  /* SBSW_OS_FC_PRECONDITION */

  /* #40 Set the current stack pointer. */
  Os_CoreSetCurrentStack(Next->Stack, Next->Core);                                                                      /* SBSW_OS_TRD_CORESETCURRENTSTACK_001 */

  /* #50 Update the Msr of the nest thread. */
  Os_Hal_ContextSetUserMsrBits(Next->Context);                                                                          /* SBSW_OS_TRD_HAL_CONTEXTSETUSERMSRBITS_001 */

  /* #60 Resume the hardware context of the first thread. */
  Os_Hal_ContextFirstResume(Next->Context); /* function never returns! */                                               /* SBSW_OS_TRD_HAL_CONTEXTFIRSTRESUME_001 */

  /* Never reached. */
  Os_ErrKernelPanic(); /* function never returns! */                                                                    /* PRQA S 2880 */ /* MD_Os_Rule2.1_2880_NonReturning */
}                                                                                                                       /* PRQA S 6050 */ /* MD_MSR_STCAL */


/***********************************************************************************************************************
 *  Os_ThreadCleanupAndResume()
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
 */
OS_NORETURN OS_ALWAYS_INLINE static inline void   /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
Os_ThreadCleanupAndResume (
  const   Os_ThreadConfigType OS_CONST * Next,
  boolean CallPreThreadHook
)
{
  const   Os_ThreadConfigType OS_CONST * current;

  current = Os_CoreGetThread();

  /* Check that the function is not called from a task. */
  Os_Assert((Os_StdReturnType)(Os_ThreadIsTask(current) == 0u));                                                        /* PRQA S 4304 */ /* MD_Os_C90BooleanCompatibility */ /* SBSW_OS_TRD_THREADISTASK_001 */

  /* #10 Release all services called by the thread. */
  Os_ServiceCallForceRelease(&(current->Dyn->ServiceCalls));                                                            /* SBSW_OS_TRD_SERVICECALLFORCERELEASE_001 */

  /* #20 Perform stack overflow check. */
  Os_StackOverflowCheck();

  /* #30 Tell timing protection module to resume timing protection for Next. */
  {
    const   Os_CoreAsrConfigType OS_CONST * core;
    const   Os_TpConfigType OS_CONST * configTp;
    const   Os_TpThreadConfigType OS_CONST * nextTp;
    const   Os_TpThreadConfigType OS_CONST * currentTp;

    core = Os_ThreadGetCore(Next);                                                                                      /* SBSW_OS_FC_PRECONDITION */
    configTp = Os_CoreAsrGetTimingProtection(core);                                                                     /* SBSW_OS_TRD_COREASRGETTIMINGPROTECTION_001 */
    nextTp = Os_ThreadGetTpConfig(Next);                                                                                /* SBSW_OS_FC_PRECONDITION */
    currentTp = Os_ThreadGetTpConfig(current);                                                                          /* SBSW_OS_TRD_THREADGETTPCONFIG_001 */
    Os_TpResetAndResume(configTp, currentTp, nextTp, Next);                                                             /* SBSW_OS_TRD_TPRESETANDRESUME_001 */
  }

  /* #40 Resume memory protection access rights of Next. */
  Os_MpSwitch(current->Dyn->MpAccessRights, Next->Dyn->MpAccessRights);                                                 /* SBSW_OS_TRD_MPSWITCH_001 */

  /* #45 Set the current thread pointer. */
  Os_CoreSetThread(Next);                                                                                               /* SBSW_OS_FC_PRECONDITION */

  /* #50 Call PreThreadHook. */
  Os_ThreadCallPreThreadHook(Next, CallPreThreadHook);                                                                  /* SBSW_OS_FC_PRECONDITION */

  /* #55 Restore FPU context of next thread if needed. */
  Os_ThreadRestoreFpuContext(Next);                                                                                     /* SBSW_OS_FC_PRECONDITION */

  /* #60 Set the current stack pointer. */
  Os_CoreSetCurrentStack(Next->Stack, Next->Core);                                                                      /* SBSW_OS_TRD_CORESETCURRENTSTACK_001 */

  /* #70 Update the Msr of the next thread. */
  Os_Hal_ContextSetUserMsrBits(Next->Context);                                                                          /* SBSW_OS_TRD_HAL_CONTEXTSETUSERMSRBITS_001 */

  /* #80 Cleanup the current hardware context and resume the hardware context of the next thread. */
  Os_Hal_ContextReturn(current->Context, Next->Context);                                                                /* SBSW_OS_TRD_HAL_CONTEXTRETURN_001 */

  /* Never reached */
  Os_ErrKernelPanic(); /* function never returns! */                                                                    /* PRQA S 2880 */ /* MD_Os_Rule2.1_2880_NonReturning */
}                                                                                                                       /* PRQA S 6050 */ /* MD_MSR_STCAL */


/***********************************************************************************************************************
 *  Os_ThreadKill()
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
 */
OS_ALWAYS_INLINE static inline void   /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
Os_ThreadKill (
  const   Os_ThreadConfigType OS_CONST * Thread
)
{
  /* #10 Initialize current application. */
  Thread->Dyn->Application = Thread->OwnerApplication;                                                                  /* SBSW_OS_TRD_THREADGETDYN_001 */

  /* #20 Initialize current context. */
  Thread->Dyn->CallContext = Thread->InitialCallContext;                                                                /* SBSW_OS_TRD_THREADGETDYN_001 */

  /* #30 Release all services called by the thread. */
  Os_ServiceCallForceRelease(&(Thread->Dyn->ServiceCalls));                                                             /* SBSW_OS_TRD_SERVICECALLFORCERELEASE_001 */

  /* #40 Release all locks and resources, which are hold by the thread. */
  Os_ThreadLocksForceRelease(Thread);                                                                                   /* SBSW_OS_FC_PRECONDITION */

  /* #50 Reset timing protection. */
  {
    const   Os_TpThreadConfigType OS_CONST * currentTp;

    currentTp = Os_ThreadGetTpConfig(Thread);                                                                           /* SBSW_OS_FC_PRECONDITION */

    Os_TpThreadKill(currentTp);                                                                                         /* SBSW_OS_TRD_TPTHREADKILL_001 */
  }

  /* #60 Initialize the memory protection. */
  Os_MpThreadInit(Thread->MpAccessRightsInitial, &(Thread->Dyn->MpAccessRights));                                       /* SBSW_OS_TRD_MPTHREADINIT_001 */


  /* #70 If the given thread is not the current thread: */
  {
    const   Os_ThreadConfigType OS_CONST * current;

    current = Os_CoreGetThread();

    if(OS_LIKELY(Thread != current))
    {
      /* #80 If the given thread is a task: */
      if(Os_ThreadIsTask(Thread) != 0u)                                                                                 /* SBSW_OS_FC_PRECONDITION */
      {
        /* #90 Reset its hardware context. */
        Os_Hal_ContextReset(Thread->ContextConfig, Thread->Context);                                                    /* SBSW_OS_TRD_HAL_CONTEXTRESET_001 */
        Os_ThreadInitFpuContext(Thread);                                                                                /* SBSW_OS_FC_PRECONDITION */
      }
      /* #100 Otherwise: */
      else
      {
        Os_Hal_IntStateType currentIntState;

        /* #110 Abort the threads context. Execute the return function the next time it will be resumed. */
        Os_Hal_ContextAbort(Thread->ContextConfig, Thread->Context);                                                    /* SBSW_OS_TRD_HAL_CONTEXTABORT_001 */

        /* #120 Set the interrupt state for the context used after the abort. */
        Os_Hal_IntGetInterruptState(&currentIntState);                                                                  /* SBSW_OS_FC_POINTER2LOCAL */
        Os_Hal_ContextSetInterruptState(Thread->Context, &currentIntState);                                             /* SBSW_OS_TRD_HAL_CONTEXTSETINTERRUPTSTATE_001 */
      }
    }
  }
}                                                                                                                       /* PRQA S 6050 */ /* MD_MSR_STCAL */


/***********************************************************************************************************************
 *  Os_ThreadSwitch()
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
 */
OS_ALWAYS_INLINE static inline void   /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
Os_ThreadSwitch (
  const   Os_ThreadConfigType OS_CONST * Current,
  const   Os_ThreadConfigType OS_CONST * Next,
  boolean CallPreThreadHook
)
{
  /* #10 Perform stack overflow check. */
  Os_StackOverflowCheck();

  /* #15 Store FPU context of current thread if needed. */
  Os_ThreadStoreFpuContext(Current);                                                                                    /* SBSW_OS_FC_PRECONDITION */

  /* #20 Tell timing protection module to switch. */
  {
    const   Os_CoreAsrConfigType OS_CONST * core;
    const   Os_TpConfigType OS_CONST * configTp;
    const   Os_TpThreadConfigType OS_CONST * currentTp;
    const   Os_TpThreadConfigType OS_CONST * nextTp;

    core = Os_ThreadGetCore(Next);                                                                                      /* SBSW_OS_FC_PRECONDITION */
    configTp = Os_CoreAsrGetTimingProtection(core);                                                                     /* SBSW_OS_TRD_COREASRGETTIMINGPROTECTION_001 */
    currentTp = Os_ThreadGetTpConfig(Current);                                                                          /* SBSW_OS_FC_PRECONDITION */
    nextTp = Os_ThreadGetTpConfig(Next);                                                                                /* SBSW_OS_FC_PRECONDITION */
    Os_TpSwitch(configTp, currentTp, nextTp, Next);                                                                     /* SBSW_OS_TRD_TPSWITCH_001 */
  }

  /* #30 Resume memory protection access rights of Next. */
  Os_MpSwitch(Current->Dyn->MpAccessRights, Next->Dyn->MpAccessRights);                                                 /* SBSW_OS_TRD_MPSWITCH_002 */

  /* #35 Switch current thread pointer. */
  Os_CoreSetThread(Next);                                                                                               /* SBSW_OS_FC_PRECONDITION */

  /* #40 Call PreThreadHook. */
  Os_ThreadCallPreThreadHook(Next, CallPreThreadHook);                                                                  /* SBSW_OS_FC_PRECONDITION */

  /* #45 Restore FPU context of next thread if needed. */
  Os_ThreadRestoreFpuContext(Next);                                                                                     /* SBSW_OS_FC_PRECONDITION */

  /* #50 Set the current stack pointer. */
  Os_CoreSetCurrentStack(Next->Stack, Next->Core);                                                                      /* SBSW_OS_TRD_CORESETCURRENTSTACK_001 */

  /* #60 Update the Msr of the next thread. */
  Os_Hal_ContextSetUserMsrBits(Next->Context);                                                                          /* SBSW_OS_TRD_HAL_CONTEXTSETUSERMSRBITS_001 */

  /* #70 Perform a hardware context switch. */
  Os_Hal_ContextSwitch(Current->Context, Next->Context);                                                                /* SBSW_OS_TRD_HAL_CONTEXTSWITCH_001 */
}                                                                                                                       /* PRQA S 6050 */ /* MD_MSR_STCAL */


/***********************************************************************************************************************
 *  Os_ThreadSuspendAndStart()
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
 */
OS_ALWAYS_INLINE static inline void   /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
Os_ThreadSuspendAndStart (
  const   Os_ThreadConfigType OS_CONST * Current,
  const   Os_ThreadConfigType OS_CONST * Next,
  boolean CallPreThreadHook
)
{
  Os_StackOverflowCheck();

  /* #10 Tell timing protection module to switch. */
  {
    const   Os_CoreAsrConfigType OS_CONST * core;
    const   Os_TpConfigType OS_CONST * configTp;
    const   Os_TpThreadConfigType OS_CONST * currentTp;
    const   Os_TpThreadConfigType OS_CONST * nextTp;

    core = Os_ThreadGetCore(Next);                                                                                      /* SBSW_OS_FC_PRECONDITION */
    configTp = Os_CoreAsrGetTimingProtection(core);                                                                     /* SBSW_OS_TRD_COREASRGETTIMINGPROTECTION_001 */
    currentTp = Os_ThreadGetTpConfig(Current);                                                                          /* SBSW_OS_FC_PRECONDITION */
    nextTp = Os_ThreadGetTpConfig(Next);                                                                                /* SBSW_OS_FC_PRECONDITION */
    Os_TpSwitch(configTp, currentTp, nextTp, Next);                                                                     /* SBSW_OS_TRD_TPSWITCH_001 */
  }

  /* #15 Store FPU context if needed */
  Os_ThreadStoreFpuContext(Current);                                                                                    /* SBSW_OS_FC_PRECONDITION */

  /* #20 Resume memory protection access rights of Next. */
  Os_MpSwitch(Current->Dyn->MpAccessRights, Next->Dyn->MpAccessRights);                                                 /* SBSW_OS_TRD_MPSWITCH_002 */

  /* #25 Switch current thread pointer. */
  Os_CoreSetThread(Next);                                                                                               /* SBSW_OS_FC_PRECONDITION */

  /* #30 Call PreThreadHook. */
  Os_ThreadCallPreThreadHook(Next, CallPreThreadHook);                                                                  /* SBSW_OS_FC_PRECONDITION */

  /* #40 Set the current stack pointer. */
  Os_CoreSetCurrentStack(Next->Stack, Next->Core);                                                                      /* SBSW_OS_TRD_CORESETCURRENTSTACK_001 */

  /* #50 Prepare context of next thread. */
  Os_Hal_ContextInit(Next->ContextConfig, Next->Context);                                                               /* SBSW_OS_TRD_HAL_CONTEXTINIT_001 */

  /* #60 Setup FPU for next thread. */
  Os_ThreadInitFpu(Next);                                                                                               /* SBSW_OS_FC_PRECONDITION */

  /* #70 Set parameter for the entry function, currently only used for hooks (hook thread pointer). */
  Os_Hal_ContextSetParameter(Next->Context, Next);                                                                      /* PRQA S 0315 */ /* MD_Os_Dir1.1_0315 */ /* SBSW_OS_TRD_HAL_CONTEXTSETPARAMETER_001 */

  /* #80 Update the Msr of the next thread. */
  Os_Hal_ContextSetUserMsrBits(Next->Context);                                                                          /* SBSW_OS_TRD_HAL_CONTEXTSETUSERMSRBITS_001 */

  /* #90 Perform context switch. */
  Os_Hal_ContextCallOnStack(Current->Context, Next->Context);                                                           /* SBSW_OS_TRD_HAL_CONTEXTCALLONSTACK_001 */

  /* #100 Restore FPU context if needed */
  Os_ThreadRestoreFpuContext(Current);                                                                                  /* SBSW_OS_FC_PRECONDITION */
}                                                                                                                       /* PRQA S 6050 */ /* MD_MSR_STCAL */


/***********************************************************************************************************************
 *  Os_ThreadGetAccessRights()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
const   Os_AccessCheckConfigType OS_CONST *   Os_ThreadGetAccessRights (
  const   Os_ThreadConfigType OS_CONST * Thread
)
{
  return Thread->AccessRights;
}


/***********************************************************************************************************************
 *  Os_ThreadGetMpAccessRights()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
const   Os_MpAccessRightsType OS_CONST *   Os_ThreadGetMpAccessRights (
  const   Os_ThreadConfigType OS_CONST * Thread
)
{
  return Thread->Dyn->MpAccessRights;
}


/***********************************************************************************************************************
 *  Os_ThreadSetMpAccessRights()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE                                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
static inline void   Os_ThreadSetMpAccessRights (
  const   Os_ThreadConfigType OS_CONST * Thread,
  const   Os_MpAccessRightsType OS_CONST * AccessRights
)
{
  Thread->Dyn->MpAccessRights = AccessRights;                                                                           /* SBSW_OS_TRD_THREADGETDYN_001 */
}


/***********************************************************************************************************************
 *  Os_ThreadSetInterruptState()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE                                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
static inline void   Os_ThreadSetInterruptState (
  const   Os_ThreadConfigType OS_CONST * Thread,
  const   Os_Hal_IntStateType OS_CONST * InterruptState
)
{
  Os_Hal_ContextSetInterruptState(Thread->Context, InterruptState);                                                     /* SBSW_OS_FC_PRECONDITION */
}

/***********************************************************************************************************************
 *  Os_ThreadCallPreThreadHook()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 */
OS_ALWAYS_INLINE                                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
static inline void   Os_ThreadCallPreThreadHook (
  const   Os_ThreadConfigType OS_CONST * Thread,
  boolean CallPreThreadHook
)
{
  /* #10 Call the PreThreadHook if requested and configured. */
  if((CallPreThreadHook == TRUE) && (Thread->PreThreadHook != ((void *)0)))
  {
    Thread->PreThreadHook();                                                                                            /* SBSW_OS_TRD_PRETHREAD_CALLBACK_001 */
  }
}


/***********************************************************************************************************************
 *  Os_ThreadCallContextOnStack(()
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
OS_FUNC_ATTRIBUTE_DEFINITION(static inline void,    , OS_ALWAYS_INLINE,                                           /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
Os_ThreadCallContextOnStack,
(
  const   Os_ThreadConfigType OS_CONST * Thread,
  const   Os_Hal_ContextConfigType OS_CONST * HalContextCfg,
  const   Os_Hal_ContextStackConfigType OS_CONST * HalStackCfg,
  const   void OS_VAR_NOINIT * Parameter,
  const   Os_Hal_IntStateType OS_VAR_NOINIT * InterruptState,
    Os_Hal_ContextType OS_VAR_NOINIT * CallerContext,
    Os_ThreadContextRefType OS_VAR_NOINIT * CalleeContext
))
{
  OS_JUSTIFY_NOINIT(Os_Hal_ContextType calleeContext);

  *CalleeContext = &calleeContext;                                                                                      /* PRQA S 2916, 2982, 3225 */ /* MD_Os_Rule18.6_2916, MD_Os_Rule2.2_2982_ContextSwitch, MD_Os_Rule18.6_3225 */ /* SBSW_OS_PWA_PRECONDITION */

  /* #10 Prepare non-trusted function context. Set its parameter, stack, interrupt and machine state. */
  Os_Hal_ContextInit(HalContextCfg, &calleeContext);                                                                    /* SBSW_OS_TRD_HAL_CONTEXTINIT_002 */
  Os_Hal_ContextSetParameter(&calleeContext, Parameter);                                                                /* SBSW_OS_TRD_HAL_CONTEXTSETPARAMETER_002 */
  Os_Hal_ContextSetStack(&calleeContext, HalStackCfg);                                                                  /* SBSW_OS_TRD_HAL_CONTEXTSETSTACK_001 */
  Os_Hal_ContextSetInterruptState(&calleeContext, InterruptState);                                                      /* SBSW_OS_TRD_HAL_CONTEXTSETINTERRUPTSTATE_002 */
  Os_Hal_ContextSetUserMsrBits(&calleeContext);                                                                         /* SBSW_OS_FC_POINTER2LOCAL */

  /* #20 If the calling thread needs saving of the FPU context: */
  if(Os_ThreadIsFpuContextEnabledForThread(Thread) != 0u)                                                               /* SBSW_OS_FC_PRECONDITION */ /* PRQA S 2992, 2996 */ /* MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2996 */
  {
    /*!
     * Internal comment removed.
 *
 *
 *
     */
    Os_Hal_ContextFpuContextType fpuContext;

    /* #30 Store and reinitialize FPU context. */
    Os_Hal_ContextStoreFpuContext(&fpuContext);                                                                         /* SBSW_OS_FC_POINTER2LOCAL */
    Os_ThreadInitFpu(Thread);                                                                                           /* SBSW_OS_FC_PRECONDITION */

    /* #40 Switch to prepared context. */
    Os_Hal_ContextCallOnStack(CallerContext, &calleeContext);                                                           /* SBSW_OS_TRD_HAL_CONTEXTCALLONSTACK_001 */

    /* #50 Restore the FPU context. */
    Os_Hal_ContextRestoreFpuContext(&fpuContext);                                                                       /* SBSW_OS_FC_POINTER2LOCAL */
  }
  /* #60 Otherwise */
  else
  {
    /* #70 Switch to prepared context. */
    Os_Hal_ContextCallOnStack(CallerContext, &calleeContext);                                                           /* SBSW_OS_TRD_HAL_CONTEXTCALLONSTACK_001 */
  }

  /* #80 Invalidate output pointer. */
  *CalleeContext = ((void *)0);                                                                                            /* PRQA S 3225 */ /* MD_Os_Rule18.6_3225 */ /* SBSW_OS_PWA_PRECONDITION */
}                                                                                                                       /* PRQA S 6050, 6060 */ /* MD_MSR_STCAL, MD_MSR_STPAR */


/***********************************************************************************************************************
 *  Os_ThreadCallContext(()
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
OS_FUNC_ATTRIBUTE_DEFINITION(static inline void,    , OS_ALWAYS_INLINE,                                           /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
Os_ThreadCallContext,
(
  const   Os_ThreadConfigType OS_CONST * Thread,
  const   Os_Hal_ContextConfigType OS_CONST * HalContextCfg,
  const   void OS_VAR_NOINIT * Parameter,
  const   Os_Hal_IntStateType OS_VAR_NOINIT * InterruptState,
    Os_Hal_ContextType OS_VAR_NOINIT * CallerContext,
    Os_ThreadContextRefType OS_VAR_NOINIT * CalleeContext
))
{
  OS_JUSTIFY_NOINIT(Os_Hal_ContextType calleeContext);

  *CalleeContext = &calleeContext;                                                                                      /* PRQA S 2916, 2982, 3225 */ /* MD_Os_Rule18.6_2916, MD_Os_Rule2.2_2982_ContextSwitch, MD_Os_Rule18.6_3225 */ /* SBSW_OS_PWA_PRECONDITION */

  /* #10 Prepare call context. Set its parameter, interrupt and machine state. */
  Os_Hal_ContextInit(HalContextCfg, &calleeContext);                                                                    /* SBSW_OS_TRD_HAL_CONTEXTINIT_002 */
  Os_Hal_ContextSetParameter(&calleeContext, Parameter);                                                                /* SBSW_OS_FC_POINTER2LOCAL */
  Os_Hal_ContextSetInterruptState(&calleeContext, InterruptState);                                                      /* SBSW_OS_FC_POINTER2LOCAL */
  Os_Hal_ContextSetUserMsrBits(&calleeContext);                                                                         /* SBSW_OS_FC_POINTER2LOCAL */

  /* #20 If the calling thread needs saving of the FPU context: */
  if(Os_ThreadIsFpuContextEnabledForThread(Thread) != 0u)                                                               /* SBSW_OS_FC_PRECONDITION */ /* COV_OS_OSC7103_XF */ /* PRQA S 2992, 2996 */ /* MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2996 */
  {
    /*!
     * Internal comment removed.
 *
 *
 *
     */
    Os_Hal_ContextFpuContextType fpuContext;

    /* #30 Store and reinitialize FPU context. */
    Os_Hal_ContextStoreFpuContext(&fpuContext);                                                                         /* SBSW_OS_FC_POINTER2LOCAL */
    Os_ThreadInitFpu(Thread);                                                                                           /* SBSW_OS_FC_PRECONDITION */

    /* #40 Switch to prepared context. */
    Os_Hal_ContextCall(CallerContext, &calleeContext);                                                                  /* SBSW_OS_TRD_HAL_CONTEXTCALL_001 */


    /* #50 Restore the FPU context. */
    Os_Hal_ContextRestoreFpuContext(&fpuContext);                                                                       /* SBSW_OS_FC_POINTER2LOCAL */
  }
  /* #60 Otherwise */
  else
  {
    /* #70 Switch to prepared context. */
    Os_Hal_ContextCall(CallerContext, &calleeContext);                                                                  /* SBSW_OS_TRD_HAL_CONTEXTCALL_001 */
  }

  /* #80 Invalidate output pointer. */
  *CalleeContext = ((void *)0);                                                                                            /* PRQA S 3225 */ /* MD_Os_Rule18.6_3225 */ /* SBSW_OS_PWA_PRECONDITION */
}                                                                                                                       /* PRQA S 6050, 6060 */ /* MD_MSR_STCAL, MD_MSR_STPAR */


/***********************************************************************************************************************
 *  Os_ThreadInitFpuContext()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline void   /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
Os_ThreadInitFpuContext (
  const   Os_ThreadConfigType OS_CONST * Thread
)
{
  /* #10 If the FPU context needs to be stored at context switches */
  if(Os_ThreadIsFpuContextEnabledForThread(Thread) != 0u)                                                               /* SBSW_OS_FC_PRECONDITION */ /* PRQA S 2992, 2996 */ /* MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2996 */
  {
    /* #20 Initialize the FPU context. */
    const   Os_ThreadClassType OS_VAR_NOINIT * threadClassObject;

    threadClassObject = Os_CoreGetThreadClass(Os_ThreadGetCore(Thread));                                                /* SBSW_OS_TRD_COREGETTHREADCLASS_001 */ /* SBSW_OS_FC_PRECONDITION */
    Os_Hal_ContextInitFpuContext(Thread->FpuContext, &(threadClassObject->InitialFpuContext));                          /* SBSW_OS_TRD_HAL_CONTEXTINITFPUCONTEXT_001 */
  }
}


/***********************************************************************************************************************
 *  Os_ThreadInitFpu()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline void   /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
Os_ThreadInitFpu (
  const   Os_ThreadConfigType OS_CONST * Thread
)
{
  /* #10 If the FPU context needs to be stored at context switches: */
  if(Os_ThreadIsFpuContextEnabledForThread(Thread) != 0u)                                                               /* SBSW_OS_FC_PRECONDITION */ /* PRQA S 2992, 2996 */ /* MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2996 */
  {
    /* #20 Initialize the FPU from stored initial FPU context. */
    const   Os_ThreadClassType OS_VAR_NOINIT * threadClassObject;

    threadClassObject = Os_CoreGetThreadClass(Os_ThreadGetCore(Thread));                                                /* SBSW_OS_TRD_COREGETTHREADCLASS_001 */ /* SBSW_OS_FC_PRECONDITION */
    Os_Hal_ContextInitFpu(&(threadClassObject->InitialFpuContext));                                                     /* SBSW_OS_TRD_HAL_CONTEXTINITFPU_001 */
  }
}


/***********************************************************************************************************************
 *  Os_ThreadStoreFpuContext()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline void   /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
Os_ThreadStoreFpuContext (
  const   Os_ThreadConfigType OS_CONST * Thread
)
{
  if(Os_ThreadIsFpuContextEnabledForThread(Thread) != 0u)                                                               /* SBSW_OS_FC_PRECONDITION */ /* PRQA S 2992, 2996 */ /* MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2996 */
  {
    Os_Hal_ContextStoreFpuContext(Thread->FpuContext);                                                                  /* SBSW_OS_TRD_HAL_CONTEXTSTOREFPUCONTEXT_001 */
  }
}


/***********************************************************************************************************************
 *  Os_ThreadRestoreFpuContext()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline void   /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
Os_ThreadRestoreFpuContext (
  const   Os_ThreadConfigType OS_CONST * Thread
)
{
  if(Os_ThreadIsFpuContextEnabledForThread(Thread) != 0u)                                                               /* SBSW_OS_FC_PRECONDITION */ /* PRQA S 2992, 2996 */ /* MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2996 */
  {
    Os_Hal_ContextRestoreFpuContext(Thread->FpuContext);                                                                /* SBSW_OS_TRD_HAL_CONTEXTRESTOREFPUCONTEXT_001 */
  }
}


/***********************************************************************************************************************
 *  Os_ThreadIsFpuContextEnabledForThread()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline Os_StdReturnType   /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
Os_ThreadIsFpuContextEnabledForThread (
  const   Os_ThreadConfigType OS_CONST * Thread
)
{
  Os_StdReturnType result;

  /* #10 If FPU context is enabled: */
  if(Os_ThreadIsFpuContextEnabled() != 0u)                                                                              /* PRQA S 2992, 2996 */ /* MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2996 */
  {
    /* #20 If FPU context is generally enabled for all threads return !0. */
    if(Os_ThreadAllThreadsUseFpu() != 0u)
    {
      result = !0u;                                                                                                     /* PRQA S 4404, 4558 */  /* MD_Os_C90BooleanCompatibility, MD_Os_Rule10.1_4558 */
    }
    /* #30 Otherwise return whether the given thread uses FPU context. */
    else
    {
      result = (Os_StdReturnType)Thread->UsesFpu;
    }
  }
  /* #40 Otherwise return 0. */
  else
  {
    result = 0;
  }

  return result;
}


/***********************************************************************************************************************
 *  Os_ThreadAllThreadsUseFpu()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 */
OS_ALWAYS_INLINE static inline Os_StdReturnType   /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
Os_ThreadAllThreadsUseFpu (
  void
)
{
  /* #10 Return whether all threads use FPU. */
  return (OS_CFG_THREAD_FPU_CONTEXT_FOR_ALL_THREADS_ENABLED == STD_ON);                                                 /* PRQA S 4404, 4548, 2996*/ /* MD_Os_C90BooleanCompatibility, MD_Os_Rule10.1_4548, MD_Os_Rule2.2_2996 */
}


/***********************************************************************************************************************
 *  Os_ThreadIsFpuContextEnabled()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 */
OS_ALWAYS_INLINE static inline Os_StdReturnType   /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
Os_ThreadIsFpuContextEnabled (
  void
)
{
  /* #10 Return whether FPU contexts are enabled. */
  return (OS_CFG_THREAD_FPU_CONTEXT_ENABLED == STD_ON);                                                                 /* PRQA S 4404, 4548, 2996 */ /* MD_Os_C90BooleanCompatibility, MD_Os_Rule10.1_4548, MD_Os_Rule2.2_2996 */
}


# define OS_STOP_SEC_CODE
# include "Os_MemMap_OsCode.h"                                                                                          /* PRQA S 5087 */ /* MD_MSR_MemMap */


#endif /* OS_THREAD_H */

/* module specific MISRA deviations:
 */

/*!
 * \}
 */

/***********************************************************************************************************************
 *  END OF FILE: Os_Thread.h
 **********************************************************************************************************************/
