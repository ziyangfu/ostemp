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
 * \ingroup Os_Kernel
 * \addtogroup Os_Core
 * \{
 *
 * \file
 * \brief       OS services for execution control of one or multiple cores.
 * \details     --no details--
 **********************************************************************************************************************/
/***********************************************************************************************************************
 *  REVISION HISTORY
 *  --------------------------------------------------------------------------------------------------------------------
 *  Refer to Os.h.
 **********************************************************************************************************************/

#ifndef OS_CORE_H
# define OS_CORE_H
                                                                                                                         /* PRQA S 0388 EOF */ /* MD_MSR_Dir1.1 */
/***********************************************************************************************************************
 *  INCLUDES
 **********************************************************************************************************************/
/* AUTOSAR includes */
# include "Std_Types.h"

/* Os module declarations */
# include "Os_CoreInt.h"
# include "Os_Core_Cfg.h"

/* Os module dependencies */
# include "Os_Lcfg.h"
# include "Os_Deque.h"
# include "Os_Hook.h"
# include "Os_Error.h"
# include "Os_Scheduler.h"
# include "Os_Interrupt.h"
# include "Os_Stack.h"
# include "Os_Spinlock.h"
# include "Os_Barrier.h"
# include "Os_Resource.h"
# include "Os_Application.h"
# include "Os_ServiceFunction.h"
# include "Os_Common.h"
# include "Os_XSignal.h"
# include "Os_TimingProtection.h"
# include "Os_Ioc.h"
# include "Os_MemoryProtection.h"
# include "Os_Trace.h"
# include "Os_Isr.h"
# include "Os_Task.h"
# include "Os_Thread.h"

/* Os HAL dependencies */
# include "Os_Hal_Os.h"
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
 *  Os_CoreIsSingleCoreOs()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StdReturnType   Os_CoreIsSingleCoreOs /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
(void)
{
  return (Os_StdReturnType)(OS_COREID_COUNT == 1u);                                                                     /* PRQA S 1881, 4304, 2995, 2996 */ /* MD_Os_Rule10.4_1881, MD_Os_C90BooleanCompatibility, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
}


/***********************************************************************************************************************
 *  Os_CoreIsSingleAsrCoreOs()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StdReturnType   Os_CoreIsSingleAsrCoreOs /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
(void)
{
  return (Os_StdReturnType)(OS_COREASRCOREIDX_COUNT == 1uL);                                                            /* PRQA S 1881, 4304, 2995, 2996 */ /* MD_Os_Rule10.4_1881, MD_Os_C90BooleanCompatibility, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
}


/***********************************************************************************************************************
 *  Os_CoreIsInterruptOnlyOs()
 **********************************************************************************************************************/
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
OS_ALWAYS_INLINE static inline Os_StdReturnType   Os_CoreIsInterruptOnlyOs (void)
{
  return (Os_StdReturnType)(OS_CFG_INTERRUPT_ONLY == STD_ON);                                                           /* PRQA S 4304, 2995, 2996 */ /* MD_Os_C90BooleanCompatibility, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
}


/***********************************************************************************************************************
 *  Os_CoreAsrIsCoreLocal()
 **********************************************************************************************************************/
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
OS_ALWAYS_INLINE static inline Os_StdReturnType   Os_CoreAsrIsCoreLocal (
  const   Os_CoreAsrConfigType OS_CONST *  Core
)
{
  const   Os_CoreAsrConfigType OS_CONST * localCore = Os_ThreadGetCore(Os_CoreGetThread());                  /* PRQA S 2983 */ /* MD_Os_Rule2.2_2983_Config */ /* SBSW_OS_CORE_THREADGETCORE_001 */
  return (Os_StdReturnType)((Os_CoreIsSingleAsrCoreOs()) || (Core == localCore));                                       /* PRQA S 4115, 4304, 4558 */ /* MD_Os_Rule10.1_4115, MD_Os_C90BooleanCompatibility, MD_Os_Rule10.1_4558 */ /* COV_OS_HALPLATFORMMULTICOREUNSUPPORTED */
}


/***********************************************************************************************************************
 *  Os_CoreAsrCheckIsCoreLocal()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StdReturnType   /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
Os_CoreAsrCheckIsCoreLocal (
  const   Os_CoreAsrConfigType OS_CONST *  Core
)
{
  return Os_ErrExtendedCheck(Os_CoreAsrIsCoreLocal(Core));                                                              /* SBSW_OS_FC_PRECONDITION */
}


/***********************************************************************************************************************
 *  Os_CoreAsrGetDyn()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
  Os_CoreAsrType OS_VAR_NOINIT *   Os_CoreAsrGetDyn (
  const   Os_CoreAsrConfigType OS_CONST * Core
)
{
  return Core->Dyn;
}


/***********************************************************************************************************************
 *  Os_CoreAsrGetTimingProtection()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
const   Os_TpConfigType OS_CONST *   Os_CoreAsrGetTimingProtection (
  const   Os_CoreAsrConfigType OS_CONST * Core
)
{
  return Core->TimingProtection;
}

/***********************************************************************************************************************
 *  Os_CoreGetTrace()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
  Os_TraceCoreType OS_VAR_NOINIT *   Os_CoreGetTrace (
  const   Os_CoreConfigType OS_CONST * Core
)
{
  return Core->Trace;
}


/***********************************************************************************************************************
 *  Os_CoreAsrGetBarriers()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */ /* COV_OS_TESTSUITEOSC4998 */
const   Os_BarrierBaseConfigRefType OS_VAR_NOINIT *   Os_CoreAsrGetBarriers (
  const   Os_CoreAsrConfigType OS_CONST * Core
)                                                                                                                      /* COV_OS_TESTSUITEOSC4998 */
{
  return Core->BarrierRefs;
}


/***********************************************************************************************************************
 *  Os_Core2AsrCore()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
const   Os_CoreAsrConfigType OS_CONST *   Os_Core2AsrCore (
  const   Os_CoreConfigType OS_CONST * Core
)
{
  Os_Assert((Os_StdReturnType)(Core->IsAsrCore == TRUE));                                                               /* PRQA S 4304 */ /* MD_Os_C90BooleanCompatibility */
  return (const   Os_CoreAsrConfigType OS_CONST *)Core;                                                      /* PRQA S 0310 */ /* MD_Os_Rule11.3_0310 */
}


/***********************************************************************************************************************
 *  Os_CoreAsr2Core()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */ /* COV_OS_HALPLATFORMMULTICOREUNSUPPORTED */
const   Os_CoreConfigType OS_CONST *   Os_CoreAsr2Core (
  const   Os_CoreAsrConfigType OS_CONST * Core
)                                                                                                                      /* COV_OS_HALPLATFORMMULTICOREUNSUPPORTED */
{
  return &(Core->Core);
}


/***********************************************************************************************************************
 *  Os_CoreId2Core()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
const   Os_CoreConfigType OS_CONST *   Os_CoreId2Core (
  CoreIdType CoreId
)
{
  Os_Assert(Os_ErrIsValueLo((uint32)CoreId, (uint32)OS_COREID_COUNT));
  return OsCfg_CoreRefs[CoreId];                                                                                        /* PRQA S 2842 */ /* MD_Os_Rule18.1_2842 */
}


/***********************************************************************************************************************
 *  Os_CoreGetCurrentCore()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
const   Os_CoreConfigType OS_CONST *   Os_CoreGetCurrentCore (
  void
)
{
  return OsCfg_CorePhysicalRefs[Os_Hal_CoreGetId()];
}


/***********************************************************************************************************************
 *  Os_CoreGetId()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE                                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
static inline CoreIdType   Os_CoreGetId (
  const   Os_CoreConfigType OS_CONST * Core
)
{
  CoreIdType coreId;

  if(Os_CoreIsSingleCoreOs() != 0u)                                                                                     /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */ /* COV_OS_TESTSUITERESTRICTIONCORECOUNT */
  {
    coreId = OS_CORE_ID_MASTER;
  }
  else
  {
    coreId = (CoreIdType)Core->Id;
  }

  return coreId;
}


/***********************************************************************************************************************
 *  Os_CoreIsAsrCore()
 **********************************************************************************************************************/
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
OS_ALWAYS_INLINE static inline Os_StdReturnType   Os_CoreIsAsrCore (
  const   Os_CoreConfigType OS_CONST * Core
)
{
  return (Os_StdReturnType)(Core->IsAsrCore == TRUE);                                                                   /* PRQA S 4304 */ /* MD_Os_C90BooleanCompatibility */
}


/***********************************************************************************************************************
 *  Os_CoreIsStartedAsAsr()
 **********************************************************************************************************************/
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
OS_ALWAYS_INLINE static inline Os_StdReturnType   Os_CoreIsStartedAsAsr (
  const   Os_CoreConfigType OS_CONST * Core
)
{
  return (Os_StdReturnType)(Core->Status->ActivationState == OS_CORESTATE_ACTIVATED_ASR);                               /* PRQA S 4304 */ /* MD_Os_C90BooleanCompatibility */
}


/***********************************************************************************************************************
 *  Os_CoreCheckIsAsrCore()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE                                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
static inline Os_StdReturnType   Os_CoreCheckIsAsrCore (
  const   Os_CoreConfigType OS_CONST * Core
)
{
  return Os_ErrExtendedCheck(Os_CoreIsAsrCore(Core));                                                                   /* SBSW_OS_FC_PRECONDITION */
}


/***********************************************************************************************************************
 *  Os_CoreCheckStartedAsAsr()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE                                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
static inline Os_StdReturnType   Os_CoreCheckStartedAsAsr (
  const   Os_CoreConfigType OS_CONST * Core
)
{
  return Os_ErrExtendedCheck(Os_CoreIsStartedAsAsr(Core));                                                              /* SBSW_OS_FC_PRECONDITION */
}


/***********************************************************************************************************************
 *  Os_CoreOsIsStarted()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE                                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
static inline Os_StdReturnType   Os_CoreOsIsStarted (
  const   Os_CoreAsrConfigType OS_CONST * Core
)
{
  return (Os_StdReturnType)(Os_CoreAsrGetAsrStatus(Core)->OsState == OS_COREOSSTATE_STARTED);                           /* PRQA S 4304 */ /* MD_Os_C90BooleanCompatibility */ /* SBSW_OS_FC_PRECONDITION */
}


/***********************************************************************************************************************
 *  Os_CoreOsIsPrestarted()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE                                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
static inline Os_StdReturnType   Os_CoreOsIsPrestarted (
  const   Os_CoreAsrConfigType OS_CONST * Core
)
{
  return (Os_StdReturnType)(Os_CoreAsrGetAsrStatus(Core)->OsState == OS_COREOSSTATE_PRESTARTTASK);                      /* PRQA S 4304 */ /* MD_Os_C90BooleanCompatibility */ /* SBSW_OS_FC_PRECONDITION */
}


/***********************************************************************************************************************
 *  Os_CoreAsrGetAsrStatus()
 **********************************************************************************************************************/
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
OS_ALWAYS_INLINE static inline   Os_CoreAsrStatusType volatile OS_VAR_NOINIT *   Os_CoreAsrGetAsrStatus (
  const   Os_CoreAsrConfigType OS_CONST * Core
)
{
  return ((  Os_CoreAsrStatusType volatile OS_VAR_NOINIT *)Core->Core.Status);                           /* PRQA S 0310 */ /* MD_Os_Rule11.3_0310 */
}


/***********************************************************************************************************************
 *  Os_CoreCheckId()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE                                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
static inline Os_StdReturnType   Os_CoreCheckId (
  CoreIdType CoreId
)
{
  return Os_ErrExtendedCheck(Os_ErrIsValueLo((uint32)CoreId, (uint32)OS_COREID_COUNT));
}


/***********************************************************************************************************************
 *  Os_CoreGetKernelStack()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
const   Os_StackConfigType OS_CONST *   Os_CoreGetKernelStack (
  const   Os_CoreAsrConfigType OS_CONST * Core
)
{
  return Core->KernelStack;
}


/***********************************************************************************************************************
 *  Os_CoreGetAsrCoreIdx()
 **********************************************************************************************************************/
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */ /* COV_OS_HALPLATFORMMULTICOREUNSUPPORTED */
OS_ALWAYS_INLINE static inline Os_CoreAsrCoreIdx   Os_CoreGetAsrCoreIdx (
  const   Os_CoreAsrConfigType OS_CONST * Core
)                                                                                                                      /* COV_OS_HALPLATFORMMULTICOREUNSUPPORTED */
{
  return Core->CoreIdx;
}


/***********************************************************************************************************************
 *  Os_CoreGetXSig()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */ /* COV_OS_HALPLATFORMMULTICOREUNSUPPORTED */
const   Os_XSigConfigType OS_CONST *   Os_CoreGetXSig (
  const   Os_CoreAsrConfigType OS_CONST * Core
)                                                                                                                      /* COV_OS_HALPLATFORMMULTICOREUNSUPPORTED */
{
  return Core->XSignal;
}


/***********************************************************************************************************************
 *  Os_CoreGetHookInfo()
 **********************************************************************************************************************/
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
OS_ALWAYS_INLINE static inline   Os_HookInfoType OS_VAR_NOINIT *   Os_CoreGetHookInfo (
  const   Os_CoreAsrConfigType OS_CONST * Core
)
{
  return &(Os_CoreAsrGetDyn(Core)->HookInfo);                                                                           /* SBSW_OS_FC_PRECONDITION */
}


/***********************************************************************************************************************
 *  Os_CoreGetThreadClass()
 **********************************************************************************************************************/
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
OS_ALWAYS_INLINE static inline   Os_ThreadClassType OS_VAR_NOINIT *   Os_CoreGetThreadClass (
  const   Os_CoreAsrConfigType OS_CONST * Core
)
{
  return Core->ThreadClass;
}


/***********************************************************************************************************************
 *  Os_CoreGetStartupHook()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
const   Os_HookCallbackConfigType OS_CONST *   Os_CoreGetStartupHook (
  const   Os_CoreAsrConfigType OS_CONST * Core
)
{
  return Core->StartupHookRef;
}


/***********************************************************************************************************************
 *  Os_CoreGetShutdownHook()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
const   Os_HookStatusHookConfigType OS_CONST *   Os_CoreGetShutdownHook (
  const   Os_CoreAsrConfigType OS_CONST * Core
)
{
  return Core->ShutdownHookRef;
}


/***********************************************************************************************************************
 *  Os_CoreGetProtectionHook()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
const   Os_HookProtectionHookConfigType OS_CONST *   Os_CoreGetProtectionHook (
  const   Os_CoreAsrConfigType OS_CONST * Core
)
{
  return Core->ProtectionHookRef;
}


/***********************************************************************************************************************
 *  Os_CoreGetErrorHook()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
const   Os_HookStatusHookConfigType OS_CONST *   Os_CoreGetErrorHook (
  const   Os_CoreAsrConfigType OS_CONST * Core
)
{
  return Core->ErrorHookRef;
}

/***********************************************************************************************************************
 *  Os_CoreGetIdleTask()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE                                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
static inline const   Os_TaskConfigType OS_CONST *   Os_CoreGetIdleTask (
  const   Os_CoreAsrConfigType OS_CONST * Core
)
{
  return Core->IdleTask;
}


/***********************************************************************************************************************
 *  Os_CoreGetKernelApp()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
const   Os_AppConfigType OS_CONST *   Os_CoreGetKernelApp (
  const   Os_CoreAsrConfigType OS_CONST * Core
)
{
  return Core->KernelApp;
}


/***********************************************************************************************************************
 *  Os_CoreGetScheduler()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE                                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
static inline const   Os_SchedulerConfigType OS_CONST *   Os_CoreGetScheduler (
  const   Os_CoreAsrConfigType OS_CONST * Core
)
{
  return Core->Scheduler;
}


/***********************************************************************************************************************
 *  Os_CoreGetLastError()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE                                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
static inline   Os_ErrorType OS_VAR_NOINIT *   Os_CoreGetLastError (
  const   Os_CoreAsrConfigType OS_CONST * Core
)
{
  return &(Os_CoreAsrGetDyn(Core)->LastError);                                                                          /* SBSW_OS_FC_PRECONDITION */
}


/***********************************************************************************************************************
 *  Os_CoreInterruptedThreadsPush()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE                                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
static inline void   Os_CoreInterruptedThreadsPush (
  const   Os_CoreAsrConfigType OS_CONST * Core,
  const   Os_ThreadConfigType OS_CONST * InterruptedThread
)
{
    Os_CoreAsrType OS_VAR_NOINIT * coreDyn;

  coreDyn = Os_CoreAsrGetDyn(Core);                                                                                     /* SBSW_OS_FC_PRECONDITION */

  if(OS_UNLIKELY(coreDyn->InterruptedThreadsCounter >= OS_MAX_INTERRUPTEDTHREADS))                                      /* COV_OS_INVSTATE */
  {
    Os_ErrKernelPanic();
  }

  coreDyn->InterruptedThreads[coreDyn->InterruptedThreadsCounter] = InterruptedThread;                                  /* PRQA S 2842 */ /* MD_Os_Rule18.1_2842_Check */ /* SBSW_OS_CORE_COREASR_INTERRUPTEDTHREADS_001 */
  coreDyn->InterruptedThreadsCounter++;                                                                                 /* SBSW_OS_CORE_COREASR_DYN_001 */
}


/***********************************************************************************************************************
 *  Os_CoreInterruptedThreadsPop()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE                                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
static inline const   Os_ThreadConfigType OS_CONST *   Os_CoreInterruptedThreadsPop (
  const   Os_CoreAsrConfigType OS_CONST * Core
)
{
    Os_CoreAsrType OS_VAR_NOINIT * coreDyn;

  coreDyn = Os_CoreAsrGetDyn(Core);                                                                                     /* SBSW_OS_FC_PRECONDITION */

  if(OS_UNLIKELY(coreDyn->InterruptedThreadsCounter == 0))                                                              /* COV_OS_INVSTATE */
  {
    Os_ErrKernelPanic();
  }

  coreDyn->InterruptedThreadsCounter--;                                                                                 /* SBSW_OS_CORE_COREASR_DYN_001 */

  return coreDyn->InterruptedThreads[coreDyn->InterruptedThreadsCounter];
}


/***********************************************************************************************************************
 *  Os_CoreThreadGetByContext()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 */
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
OS_ALWAYS_INLINE static inline const   Os_ThreadConfigType OS_CONST *   Os_CoreThreadGetByContext (
  const   Os_CoreAsrConfigType OS_CONST * Core,
  Os_CallContextType Context
)
{
  const   Os_ThreadConfigType OS_CONST * currentThread;
  const   Os_ThreadConfigType OS_CONST * result;

  currentThread = Os_CoreGetThread();

  /* #10 Check whether the current thread matches the context. */
  if(Os_ThreadIsAnyInitialContext(currentThread, Context) != 0u)                                                        /* SBSW_OS_CORE_THREADISANYINITIALCONTEXT_001 */
  {
    result = currentThread;
  }
  /* #20 Otherwise search the list of interrupted threads. */
  else
  {
    const   Os_ThreadConfigType OS_CONST * interruptedThread;
    Os_CoreThreadIdx iterator;
    Os_CoreThreadIdx iteratorEnd;

    result = ((void *)0);

    iterator = Os_CoreInterruptedThreadsGetTopIdx(Core);                                                                /* SBSW_OS_FC_PRECONDITION */
    iteratorEnd = Os_CoreInterruptedThreadsGetBottomIdx();

    while(iterator != iteratorEnd)                                                                                      /* COV_OS_HALPLATFORMTPUNSUPPORTED_XF */
    {
      interruptedThread = Os_CoreInterruptedThreadsGet(Core, iterator);                                                 /* SBSW_OS_FC_PRECONDITION */

      if(Os_ThreadIsAnyInitialContext(interruptedThread, Context) != 0u)                                                /* SBSW_OS_CORE_THREADISANYINITIALCONTEXT_002 */
      {
        result = interruptedThread;
        break;
      }

      iterator = Os_CoreInterruptedThreadsMoveDown(iterator);
    }
  }

  return result;
}


/***********************************************************************************************************************
 *  Os_CoreInterruptedThreadsGetTopIdx()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE                                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
static inline Os_CoreThreadIdx   Os_CoreInterruptedThreadsGetTopIdx (
  const   Os_CoreAsrConfigType OS_CONST * Core
)
{
  const   Os_CoreAsrType OS_VAR_NOINIT * coreDyn;

  coreDyn = Os_CoreAsrGetDyn(Core);                                                                                     /* SBSW_OS_FC_PRECONDITION */

  Os_Assert((Os_StdReturnType)(coreDyn->InterruptedThreadsCounter < OS_MAX_INTERRUPTEDTHREADS));                        /* PRQA S 4304 */ /* MD_Os_C90BooleanCompatibility */

  return coreDyn->InterruptedThreadsCounter;
}


/***********************************************************************************************************************
 *  Os_CoreInterruptedThreadsGet()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE                                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
static inline const   Os_ThreadConfigType OS_CONST *   Os_CoreInterruptedThreadsGet (
  const   Os_CoreAsrConfigType OS_CONST * Core,
  Os_CoreThreadIdx Idx
)
{
  const   Os_CoreAsrType OS_VAR_NOINIT * coreDyn;

  coreDyn = Os_CoreAsrGetDyn(Core);                                                                                     /* SBSW_OS_FC_PRECONDITION */

  if(OS_UNLIKELY((Idx > coreDyn->InterruptedThreadsCounter) || (Idx == 0)))                                             /* COV_OS_INVSTATE */
  {
    Os_ErrKernelPanic();
  }

  return coreDyn->InterruptedThreads[Idx-1];                                                                            /* PRQA S 2842 */ /* MD_Os_Rule18.1_2842_Check */
}


/***********************************************************************************************************************
 *  Os_CoreInterruptedThreadsGetBottomIdx()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE                                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
static inline Os_CoreThreadIdx   Os_CoreInterruptedThreadsGetBottomIdx (void)
{
  return (Os_CoreThreadIdx)0;
}


/***********************************************************************************************************************
 *  Os_CoreInterruptedThreadsMoveDown()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 */
OS_ALWAYS_INLINE                                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
static inline Os_CoreThreadIdx   Os_CoreInterruptedThreadsMoveDown (
    Os_CoreThreadIdx Iterator
)
{
  /* #10 Return the decremented iterator. */
  return (Iterator - (Os_CoreThreadIdx)1);
}


/***********************************************************************************************************************
 *  Os_CoreSetThread()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE                                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
static inline void   Os_CoreSetThread (
  const   Os_ThreadConfigType OS_CONST * Thread
)
{
  Os_Hal_CoreSetThread(Os_SystemGetCore2Thread(Os_SystemGet()), Thread);                                                /* SBSW_OS_CORE_SYSTEMGETCORE2THREAD_001 */ /* SBSW_OS_CORE_HAL_CORESETTHREAD_001 */
}


/***********************************************************************************************************************
 *  Os_CoreGetThread()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE                                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
static inline const   Os_ThreadConfigType OS_CONST *   Os_CoreGetThread (
  void
)
{
  return Os_Hal_CoreGetThread(Os_SystemGetCore2Thread(Os_SystemGet()));                                                 /* SBSW_OS_CORE_SYSTEMGETCORE2THREAD_001 */ /* SBSW_OS_CORE_HAL_COREGETTHREAD_001 */
}


/***********************************************************************************************************************
 *  Os_CoreSetCurrentStack()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE                                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
static inline void   Os_CoreSetCurrentStack (
  const   Os_StackConfigType OS_CONST * Stack,
  const   Os_CoreAsrConfigType OS_CONST * Core
)
{
  if(Os_StackIsMonitoringEnabled() != 0u)                                                                               /* PRQA S 2991, 2995 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule2.2_2995 */
  {
    Os_CoreAsrGetDyn(Core)->CurrentStack = Stack;                                                                       /* SBSW_OS_FC_PRECONDITION */ /* SBSW_OS_CORE_COREASRGETDYN_003 */
  }
}


/***********************************************************************************************************************
 *  Os_CoreGetCurrentStack()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE                                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
static inline const   Os_StackConfigType OS_CONST *   Os_CoreGetCurrentStack (
  void
)
{
  if(Os_StackIsMonitoringEnabled() == 0u)                                                                               /* COV_OS_INVSTATE */ /* PRQA S 2992, 2996 */ /* MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2996 */
  {
    Os_ErrKernelPanic();
  }
  return Os_CoreAsrGetDyn(Os_Core2AsrCore(Os_CoreGetCurrentCore()))->CurrentStack;                                      /* SBSW_OS_CORE_CORE2ASRCORE_002 */ /* SBSW_OS_CORE_COREASRGETDYN_002 */
}


/***********************************************************************************************************************
 *  Os_SystemGet()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
const   Os_SystemConfigType OS_CONST *   Os_SystemGet (
  void
)
{
  return &OsCfg_System;
}


/***********************************************************************************************************************
 *  Os_SystemGetCore2Thread()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
const   Os_Hal_Core2ThreadConfigType OS_CONST *   Os_SystemGetCore2Thread (
  const   Os_SystemConfigType OS_CONST * System
)
{
  return System->Core2Thread;
}


/***********************************************************************************************************************
 *  Os_CoreCriticalUserSectionEnter()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 */
OS_ALWAYS_INLINE static inline void   Os_CoreCriticalUserSectionEnter /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
(
  const   Os_CoreAsrConfigType OS_CONST * CoreAsr
)
{
  /* #10 Mark that OS is executed in critical user section. */
  Os_CoreAsrGetAsrStatus(CoreAsr)->CriticalUserSection++;                                                               /* PRQA S 3387 */ /* MD_Os_Rule13.3_3387 */ /* SBSW_OS_FC_PRECONDITION */ /* SBSW_OS_CORE_COREASR_STATUS_002 */
  Os_Assert((Os_StdReturnType)(Os_CoreAsrGetAsrStatus(CoreAsr)->CriticalUserSection != 0u));                            /* PRQA S 4304 */ /* MD_Os_C90BooleanCompatibility */ /* SBSW_OS_FC_PRECONDITION */
}


/***********************************************************************************************************************
 *  Os_CoreCriticalUserSectionLeave()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 */
OS_ALWAYS_INLINE static inline void   Os_CoreCriticalUserSectionLeave /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
(
  const   Os_CoreAsrConfigType OS_CONST * CoreAsr
)
{
  /* #10 Mark that OS has left critical user section. */
  Os_Assert((Os_StdReturnType)(Os_CoreAsrGetAsrStatus(CoreAsr)->CriticalUserSection != 0u));                            /* PRQA S 4304 */ /* MD_Os_C90BooleanCompatibility */ /* SBSW_OS_FC_PRECONDITION */
  Os_CoreAsrGetAsrStatus(CoreAsr)->CriticalUserSection--;                                                               /* PRQA S 3387 */ /* MD_Os_Rule13.3_3387 */ /* SBSW_OS_FC_PRECONDITION */ /* SBSW_OS_CORE_COREASR_STATUS_002 */
}


/***********************************************************************************************************************
 *  Os_CoreCriticalUserSectionIsActive()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 */
OS_ALWAYS_INLINE static inline boolean   Os_CoreCriticalUserSectionIsActive /* PRQA S 3219, 4404 */ /* MD_Os_Rule2.1_3219, MD_Os_C90BooleanCompatibility */
(
  const   Os_CoreAsrConfigType OS_CONST * CoreAsr
)
{
  /* #10 Return if OS is executed in critical user section. */
  return (boolean)(Os_CoreAsrGetAsrStatus(CoreAsr)->CriticalUserSection != 0u);                                         /* PRQA S 4304 */ /* MD_Os_C90BooleanCompatibility */ /* SBSW_OS_FC_PRECONDITION */
}


/***********************************************************************************************************************
 *  Os_CoreIsExceptionContextReadingSupported()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StdReturnType   /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
Os_CoreIsExceptionContextReadingSupported (
  void
)
{
  return Os_Hal_ContextIsExceptionContextReadingSupported();
}


/***********************************************************************************************************************
 *  Os_CoreIsExceptionContextModificationSupported()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StdReturnType   /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
Os_CoreIsExceptionContextModificationSupported (
  void
)
{
  return Os_Hal_ContextIsExceptionContextModificationSupported();
}


/***********************************************************************************************************************
 *  Os_CoreInitialEnableInterruptSources()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline void   /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
Os_CoreInitialEnableInterruptSources (
  const   Os_CoreAsrConfigType OS_CONST * Core,
  boolean ClearPending
)
{
  Os_ObjIdx_IteratorType idx;

  for(idx = 0; idx < (Os_ObjIdx_IteratorType) Core->IsrCount; idx++)
  {
     Os_IsrInitialEnable((Core->IsrRefs[idx]), ClearPending);                                                           /* SBSW_OS_FC_CORE_ISRINITIALENABLE_001 */
  }
}


# define OS_STOP_SEC_CODE
# include "Os_MemMap_OsCode.h"                                                                                          /* PRQA S 5087 */ /* MD_MSR_MemMap */


#endif /* OS_CORE_H */

/* module specific MISRA deviations:
 */

/*!
 * \}
 */

/***********************************************************************************************************************
 *  END OF FILE: Os_Core.h
 **********************************************************************************************************************/
