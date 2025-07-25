/***********************************************************************************************************************
 *  COPYRIGHT
 *  --------------------------------------------------------------------------------------------------------------------
 *  \verbatim
 *  Copyright (c) 2021 by Vector Informatik GmbH.                                                  All rights reserved.
 *
 *                This software is copyright protected and proprietary to Vector Informatik GmbH.
 *                Vector Informatik GmbH grants to you only those rights as set out in the license conditions.
 *                All other rights remain with Vector Informatik GmbH.
 *  \endverbatim
 *  --------------------------------------------------------------------------------------------------------------------
 *  FILE DESCRIPTION
 *  ------------------------------------------------------------------------------------------------------------------*/
/**
 * \addtogroup Os_TimingProtection
 * \{
 *
 *  \file
 *
 **********************************************************************************************************************/
/***********************************************************************************************************************
 *  REVISION HISTORY
 *  --------------------------------------------------------------------------------------------------------------------
 *  Refer to Os.h.
 **********************************************************************************************************************/

                                                                                                                        /* PRQA S 0777 EOF */ /* MD_MSR_Rule5.1 */


#define OS_TIMINGPROTECTION_SOURCE

/***********************************************************************************************************************
 *  INCLUDES
 **********************************************************************************************************************/
/* AUTOSAR includes */
#include "Std_Types.h"

/* Os module declarations */
#include "Os_TimingProtection.h"

/* Os kernel module dependencies */
#include "Os_Thread.h"

/* Os hal dependencies */
#include "Os_Isr.h"


/***********************************************************************************************************************
 *  LOCAL CONSTANT MACROS
 **********************************************************************************************************************/

/***********************************************************************************************************************
 *  LOCAL FUNCTION MACROS
 **********************************************************************************************************************/

/***********************************************************************************************************************
 *  LOCAL DATA TYPES AND STRUCTURES
 **********************************************************************************************************************/

/***********************************************************************************************************************
 *  LOCAL DATA PROTOTYPES
 **********************************************************************************************************************/

/**********************************************************************************************************************
 *  GLOBAL DATA
 **********************************************************************************************************************/

/**********************************************************************************************************************
 *  LOCAL FUNCTION PROTOTYPES
 **********************************************************************************************************************/

#define OS_START_SEC_CODE
#include "Os_MemMap_OsCode.h"                                                                                           /* PRQA S 5087 */ /* MD_MSR_MemMap */

/***********************************************************************************************************************
 *  Os_TpThreadReInitInternal()
 **********************************************************************************************************************/
/*! \brief          Initializes timing protection data of a thread, except time frame.
 *  \details        --no details--
 *
 *  \param[in]      Owner Pointer to the owner thread timing protection configuration.
 *                  Parameter must not be NULL.
 *  \param[in]      Dyn   Pointer to the dynamic timing protection data.
 *                  Parameter must not be NULL.
 *
 *  \context        ANY
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre            Concurrent access to the given object is prevented by caller.
 *
 *  Internal comment removed.
 *
 *
 *
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline void   Os_TpThreadReInitInternal (
  const   Os_TpOwnerThreadConfigType OS_CONST * Owner,
      Os_TpOwnerThreadType OS_CONST * Dyn
);



/***********************************************************************************************************************
 *  Os_TpThreadInitInternal()
 **********************************************************************************************************************/
/*! \brief        Initialize given timing protection settings.
 *  \details      Reset thread budgets to configured ones.
 *
 *  \param[in,out]  Config   The Timing protection configuration.
 *                           Parameter must not be NULL.
 *  \param[in,out]  Current  Timing protection budgets.
 *                           Parameter must not be NULL.
 *
 *  \context      OS_INTERNAL
 *
 *  \reentrant    TRUE for different objects.
 *  \synchronous  TRUE
 *
 *  \pre          Concurrent access to given object is prevented by caller.
 *  \pre          The timing protection of the owner core is initialized.
 **********************************************************************************************************************/
void     Os_TpThreadInitInternal                                                                             /* PRQA S 3449, 3451 */ /* MD_Os_Rule8.5_3449, MD_Os_Rule8.5_3451 */
(
  const   Os_TpConfigType OS_CONST * Config,
  const   Os_TpThreadConfigType OS_CONST * Current
);


/***********************************************************************************************************************
 *  Os_TpThreadKillInternal()
 **********************************************************************************************************************/
/*! \brief        Reset timing protection settings.
 *  \details      Cancel all budget monitoring and reset budgets to configured values.
 *                The EarliestArrival is not modified.
 *
 *  \param[in,out]  Thread  The thread to be killed. Parameter must not be NULL.
 *
 *  \context      OS_INTERNAL
 *
 *  \reentrant    TRUE for different objects.
 *  \synchronous  TRUE
 *
 *  \pre          Concurrent access to given object is prevented by caller.
 *  \pre          The timing protection of the owner core is initialized.
 **********************************************************************************************************************/
void     Os_TpThreadKillInternal                                                                             /* PRQA S 3449, 3451 */ /* MD_Os_Rule8.5_3449, MD_Os_Rule8.5_3451 */
(
  const   Os_TpThreadConfigType OS_CONST * Thread
);


/**********************************************************************************************************************
 *  GLOBAL FUNCTIONS
 **********************************************************************************************************************/


/***********************************************************************************************************************
 *  Os_TpThreadInitInternal()
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
void     Os_TpThreadInitInternal                                                                             /* COV_OS_HALPLATFORMTPUNSUPPORTED_XX */
(
  const   Os_TpConfigType OS_CONST * Config,
  const   Os_TpThreadConfigType OS_CONST * Current
)
{
  /* #10 Optimization switch: Enter kernel panic if timing protection is disabled. */
  if(Os_TpIsEnabled() == 0u)                                                                                            /*  COV_OS_INVSTATE*/ /* PRQA S 2991, 2995 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule2.2_2995 */
  {
    Os_ErrKernelPanic();
  }

  /* #15 If the thread is a timing protection owner: */
  if(Current->ThreadKind == OS_TPTHREADKIND_OWNER)
  {
      Os_TpOwnerThreadType OS_CONST * currentDyn;
    const   Os_TpOwnerThreadConfigType OS_CONST * currentOwner;

    currentOwner = Os_TpThread2TpOwnerThread(Current);                                                                  /* SBSW_OS_FC_PRECONDITION */

    currentDyn = currentOwner->Dyn;

    /* #20 Re-initialize the thread's timing protection data. */
    Os_TpThreadReInitInternal(currentOwner, currentDyn);                                                                /* SBSW_OS_TP_TPTHREADREINITINTERNAL_001 */

    /* #30 If there is a time frame configured: */
    if(currentOwner->TimeFrame > 0u)
    {
      /* #40 Set the next arrival to now, so the next arrival is allowed. */
      currentDyn->EarliestArrival = Os_TpGetArrivalTimeStamp(Config);                                                   /* SBSW_OS_TP_TPTHREADGETDYN_001 */ /* SBSW_OS_FC_PRECONDITION */
    }
  }
}


/***********************************************************************************************************************
 *  Os_TpThreadKillInternal()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 */
void     Os_TpThreadKillInternal                                                                             /* COV_OS_HALPLATFORMTPUNSUPPORTED_XX */
(
  const   Os_TpThreadConfigType OS_CONST * Thread
)
{
  /* #10 If the thread is a timing protection owner: */
  if(Thread->ThreadKind == OS_TPTHREADKIND_OWNER)
  {
      Os_TpOwnerThreadType OS_CONST * currentDyn;
    const   Os_TpOwnerThreadConfigType OS_CONST * currentOwner;

    currentOwner = Os_TpThread2TpOwnerThread(Thread);                                                                   /* SBSW_OS_FC_PRECONDITION */

    currentDyn = currentOwner->Dyn;

    /* #20 Re-initialize the threads timing protection data. */
    Os_TpThreadReInitInternal(currentOwner, currentDyn);                                                                /* SBSW_OS_TP_TPTHREADREINITINTERNAL_001 */
  }
}

/***********************************************************************************************************************
 *  Os_TpThreadReInitInternal()
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
OS_ALWAYS_INLINE static inline void   Os_TpThreadReInitInternal /* COV_OS_HALPLATFORMTPUNSUPPORTED_XX */
(
  const   Os_TpOwnerThreadConfigType OS_CONST * Owner,
      Os_TpOwnerThreadType OS_CONST * Dyn
)                                                                                                                      /* COV_OS_HALPLATFORMTPUNSUPPORTED_XX */
{
  /* #10 Set delay level to zero. */
  Dyn->DelayLevel = 0;                                                                                                  /* SBSW_OS_PWA_PRECONDITION */

  /* #20 Set current violation to none. */
  Dyn->Violation = OS_TPMONITORINGTYPE_NONE;                                                                            /* SBSW_OS_PWA_PRECONDITION */

  /* #30 Empty list of monitored budgets. */
  Dyn->MonitoredBudgets = ((void *)0);                                                                                     /* SBSW_OS_PWA_PRECONDITION */

  /* #40 If there is an execution budget configured: */
  if(Owner->ExecutionBudget.BudgetKind == OS_TPMONITORINGTYPE_EXECUTION)
  {
    /* #50 Add the execution budget to the list of monitored budgets. */
    Owner->ExecutionBudget.Dyn->Previous = ((void *)0);                                                                    /* SBSW_OS_TP_TP_OWNERTHREAD_EXECUTIONBUDGET_DYN_001 */
    Dyn->MonitoredBudgets = &(Owner->ExecutionBudget);                                                                  /* SBSW_OS_PWA_PRECONDITION */

    /* #60 Reset monitored budget. */
    Dyn->MonitoredBudgets->Dyn->Remaining = Owner->Dyn->MonitoredBudgets->Budget;                                       /* SBSW_OS_PWA_PRECONDITION */
  }
}


/***********************************************************************************************************************
 *  Os_TpReducedInit()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 */
void     Os_TpReducedInit
(
  const   Os_TpConfigType OS_CONST * Config,
  const   Os_ThreadConfigType OS_CONST * InitialThread
)
{
  /* #10 If timing protection is configured */
  if(Os_TpIsEnabledByCoreTpConfig(Config) != 0u)                                                                        /* PRQA S 2992, 2996 */ /* MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2996 */ /* SBSW_OS_FC_PRECONDITION */ /* COV_OS_HALPLATFORMTPUNSUPPORTED_XF */
  {
    Os_Assert((Os_StdReturnType)(Os_ThreadGetTpConfig(InitialThread)->ThreadKind == OS_TPTHREADKIND_OWNER));            /* PRQA S 4304 */ /* MD_Os_C90BooleanCompatibility */ /* SBSW_OS_FC_PRECONDITION */

    /* #20 Initialize the monitored thread. */
    Config->Dyn->MonitoredThread = InitialThread;                                                                       /* SBSW_OS_TP_TPGETDYN_001 */
  }
}


/***********************************************************************************************************************
 *  Os_TpInit()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 *
 */
void     Os_TpInit
(
  const   Os_TpConfigType OS_CONST * Config
)
{
  const   Os_CounterConfigType OS_CONST * counter;
  Os_TickType maxAllowedValue;
  Os_TickType compareValue;
  Os_TickType now;

  /* #10 If timing protection is configured */
  if(Os_TpIsEnabledByCoreTpConfig(Config) != 0u)                                                                        /* PRQA S 2992, 2996 */ /* MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2996 */ /* SBSW_OS_FC_PRECONDITION */ /* COV_OS_HALPLATFORMTPUNSUPPORTED_XF */
  {
    /* #20 Initialize timer hardware. */
    counter = Os_TimerHrtGetCounter(Config->Timer);                                                                     /* SBSW_OS_TP_TIMERHRTGETCOUNTER_001 */
    Os_CounterInit(counter);                                                                                            /* SBSW_OS_TP_COUNTERINIT_001 */

    /* #30 Initialize data so that arrival is allowed from now on. */
    now = Os_TimerHrtGetValue(Config->Timer);                                                                           /* SBSW_OS_TP_TIMERHRTGETVALUE_001 */

    Config->Dyn->LastCounterValue = now;                                                                                /* SBSW_OS_TP_TPGETDYN_001 */
    Config->Dyn->Snapshot = (Os_InterArrivalTimeType)now;                                                               /* SBSW_OS_TP_TPGETDYN_001 */

    /* #40 Set compare value into far future (changes with initialization of threads TP). */
    maxAllowedValue = Os_CounterGetMaxAllowedValue(counter);                                                            /* SBSW_OS_TP_COUNTERGETMAXALLOWEDVALUE_001 */
    compareValue = Os_TpAdd(Config, now, maxAllowedValue);                                                              /* SBSW_OS_FC_PRECONDITION_AND_CHECK_IF_NEEDED */

    Os_TimerHrtSetCompareValue(Config->Timer, compareValue);                                                            /* SBSW_OS_TP_TIMERHRTSETCOMPAREVALUE_001 */
  }
}                                                                                                                       /* PRQA S 6050 */ /* MD_MSR_STCAL */

/***********************************************************************************************************************
 *  Os_TpStart()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
void     Os_TpStart(void)
{
  const   Os_TpConfigType OS_CONST * config;
  const   Os_CounterConfigType OS_CONST * counter;
  const   Os_CoreAsrConfigType OS_CONST * core;

  /* #10 If timing protection is configured */
  if(Os_TpIsEnabled() != 0u)                                                                                            /* COV_OS_HALPLATFORMTPUNSUPPORTED_XF */ /* PRQA S 2992, 2996 */ /* MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2996 */
  {
    core = Os_Core2AsrCore(Os_CoreGetCurrentCore());                                                                    /* SBSW_OS_CORE_CORE2ASRCORE_002 */
    config = Os_CoreAsrGetTimingProtection(core);                                                                       /* SBSW_OS_CORE_COREASRGETTIMINGPROTECTION_001 */
    /* #20 If timing protection is configured for the current core */
    if(Os_TpIsEnabledByCoreTpConfig(config) != 0u)                                                                      /* SBSW_OS_TP_TPISENABLEDBYCORETPCONFIG_001 */
    {
      counter = Os_TimerHrtGetCounter(config->Timer);                                                                   /* SBSW_OS_TP_TIMERHRTGETCOUNTER_002 */

      /* #30 Start the timer. */
      Os_CounterStart(counter);                                                                                         /* SBSW_OS_TP_COUNTERSTART_001 */
    }
  }
}                                                                                                                       /* PRQA S 6050 */ /* MD_MSR_STCAL */


/***********************************************************************************************************************
 *  ISR(Os_TpIsr)
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
ISR(Os_TpIsr)                                                                                                           /* COV_OS_HALPLATFORMTPUNSUPPORTED_XX */
{
  /*! Internal comment removed.
 *
 *
 *
 *
   */

  const   Os_ThreadConfigType OS_CONST * currentThread;
  const   Os_CoreAsrConfigType OS_CONST * core;
  const   Os_TpConfigType OS_CONST * config;

  /* #10 If TimingProtection is not enabled: KernelPanic. */
  if(Os_TpIsEnabled() == 0u)                                                                                            /* COV_OS_INVSTATE */ /* PRQA S 2991, 2995 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule2.2_2995 */
  {
    Os_ErrKernelPanic();
  }

  currentThread = Os_CoreGetThread();
  core = Os_ThreadGetCore(currentThread);                                                                                /* SBSW_OS_TP_THREADGETCORE_001 */
  config = Os_CoreAsrGetTimingProtection(core);                                                                         /* SBSW_OS_TP_COREASRGETTIMINGPROTECTION_001 */

  /* #20 Acknowledge the timer hardware, so that new timer interrupts can be triggered. */
  Os_TimerHrtAcknowledge(config->Timer);                                                                                /* SBSW_OS_TP_TIMERHRTACKNOWLEDGE_001 */

  /* #30 If the requested time span has elapsed: */
  if(Os_TimerHrtIsLogicalCompareValueReached(config->Timer) != 0u)                                                      /* SBSW_OS_TP_TIMERISLOGICALCOMPAREVALUEREACHED_001 */ /* COV_OS_HALPLATFORMFRT16BITAVAILABILITY_TX */ /* PRQA S 2991, 2995 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule2.2_2995 */
  {
    const   Os_TpOwnerThreadConfigType OS_CONST * current;
    Os_TickType deadline;

    current = Os_TpGetCurrentTpOwnerThread(config);                                                                     /* SBSW_OS_TP_TPGETCURRENTTPOWNERTHREAD_001 */
    deadline = Os_TimerHrtGetCompareValue(config->Timer);                                                               /* SBSW_OS_TP_TIMERHRTGETCOMPAREVALUE_001 */

    /* #40 Add the passed time to the inter-arrival time snapshot. */
    (void)Os_TpGetArrivalTimeStamp(config);                                                                             /* SBSW_OS_TP_TPGETARRIVALTIMESTAMP_001 */

    /* #50 If there is a budget owner: */
    if(current->Dyn->MonitoredBudgets != ((void *)0))
    {
      const   Os_CounterConfigType OS_CONST * counter;
      Os_TickType now;

      now = Os_TimerHrtGetValue(config->Timer);                                                                         /* SBSW_OS_TP_TIMERHRTGETVALUE_002 */
      counter = Os_TimerHrtGetCounter(config->Timer);                                                                   /* SBSW_OS_TP_TIMERHRTGETCOUNTER_002 */

      /* #60 If there is a thread which has exhausted its budget: */
      if(OS_UNLIKELY(Os_CounterIsFutureValue(counter, deadline, now) == 0u))                                            /* SBSW_OS_TP_COUNTERISFUTUREVALUE_001 */
      {
        Os_TpMonitoringType violation;

        /* #70 Determine the type of budget that has been exhausted. */
        violation = current->Dyn->MonitoredBudgets->BudgetKind;

        Os_Assert((Os_StdReturnType)                                                                                    /* PRQA S 4304 */ /* MD_Os_C90BooleanCompatibility */
            ((violation == OS_TPMONITORINGTYPE_LOCK) || (violation == OS_TPMONITORINGTYPE_EXECUTION)));                 /* COV_OS_INVSTATE */

        /* #80 If the thread is inside a trusted function with "delayed-timing-violation-call". */
        if(current->Dyn->DelayLevel > 0u)
        {
          /* #90 Delay protection error until return from trusted function. */
          Os_TpUpdateViolation(current, violation);                                                                     /* SBSW_OS_TP_TPUPDATEVIOLATION_001 */

          /* #100 Update the remaining time to avoid frequent activation of TP ISR. */
          current->Dyn->MonitoredBudgets->Dyn->Remaining = Os_CounterGetMaxAllowedValue(counter);                       /* SBSW_OS_TP_TPBUDGET_GETDYN_003 */ /* SBSW_OS_TP_COUNTERGETMAXALLOWEDVALUE_001 */
        }
        /* #110 Otherwise, call protection error. */
        else if(violation == OS_TPMONITORINGTYPE_LOCK)
        {
          Os_ErrProtectionError(OS_STATUS_PROTECTION_LOCKED);
        }
        else if(violation == OS_TPMONITORINGTYPE_EXECUTION)
        {
          Os_ErrProtectionError(OS_STATUS_PROTECTION_TIME);
        }
        else
        {
          /* This case may not occur. MISRA 14.10 */
        }
      }
    }
    /* #120 Ensure that the next interrupt is in time. */
    else
    {
      const   Os_CounterConfigType OS_CONST * counter;
      Os_TickType maxAllowedValue;
      Os_TickType compareValue;

      counter = Os_TimerHrtGetCounter(config->Timer);                                                                   /* SBSW_OS_TP_TIMERHRTGETCOUNTER_002 */
      maxAllowedValue = Os_CounterGetMaxAllowedValue(counter);                                                          /* SBSW_OS_TP_COUNTERGETMAXALLOWEDVALUE_001 */

      compareValue = Os_TpAdd(config, deadline, maxAllowedValue);                                                       /* SBSW_OS_TP_TPAPP_001 */

      Os_TimerHrtSetCompareValue(config->Timer, compareValue);                                                          /* SBSW_OS_TP_TIMERHRTSETCOMPAREVALUE_002 */
    }
  }else
  {
    /* #80 Update the hardware compare value in accordance to the logical compare value. */
    Os_TimerHrtSetCompareValue(config->Timer, config->Timer->Dyn->Compare);                                             /* SBSW_OS_TP_TIMERHRTSETCOMPAREVALUE_002 */
  }
}                                                                                                                       /* PRQA S 6050, 6080 */ /* MD_MSR_STCAL, MD_MSR_STMIF */




#define OS_STOP_SEC_CODE
#include "Os_MemMap_OsCode.h"                                                                                           /* PRQA S 5087 */ /* MD_MSR_MemMap */

/* module specific MISRA deviations:
 */

/* SBSW_JUSTIFICATION_BEGIN

\ID SBSW_OS_TP_TPTHREADREINITINTERNAL_001
 \DESCRIPTION    Os_TpThreadReInitInternal is called with the return value of Os_TpThread2TpOwnerThread and the
                 Dyn pointer derived from the same object.
 \COUNTERMEASURE \R [CM_OS_TPTHREAD2TPOWNERTHREAD_R]
                 \M [CM_OS_TPOWNERTHREAD_DYN_M]

\ID SBSW_OS_TP_TPGETRESOURCEBUDGET_001
 \DESCRIPTION    Os_TpGetResourceBudget is called with the return value of Os_TpThread2TpOwnerThread.
 \COUNTERMEASURE \R [CM_OS_TPTHREAD2TPOWNERTHREAD_R]

\ID SBSW_OS_TP_TP_OWNERTHREAD_EXECUTIONBUDGET_DYN_001
 \DESCRIPTION    Write access to the Dyn pointer of the ExecutionBudget of an Owner object. The Owner object is
                 passed as argument to the caller function. Precondition ensures that the pointer is valid.
                 The compliance of the precondition is checked during review.
 \COUNTERMEASURE \M [CM_OS_TP_TPBUDGET_DYN_M]

\ID SBSW_OS_TP_TPTHREADGETDYN_001
 \DESCRIPTION    Write access to the Dyn pointer of a TpOwnerThread. The TpOwnerThread is the return value of
                 Os_TpThread2TpOwnerThread.
 \COUNTERMEASURE \R [CM_OS_TPTHREAD2TPOWNERTHREAD_R]
                 \M [CM_OS_TPOWNERTHREAD_DYN_M]

\ID SBSW_OS_TP_TPTHREADGETDYN_002
 \DESCRIPTION    Write access to the Dyn pointer derived from the return value of Os_TpGetCurrentTpOwnerThread.
                 Precondition ensures, that the current owner is set to an owner thread. The compliance of the
                 precondition is checked during review.
 \COUNTERMEASURE \M [CM_OS_TPOWNERTHREAD_DYN_M]

 \ID SBSW_OS_TP_TPTHREADGETDYN_003
 \DESCRIPTION    Write access to the Dyn pointer of a Os_TpOwnerThreadConfigType object. The Current object is passed
                 as argument to the caller function. Precondition ensures that the pointer is valid
                 The compliance of the precondition is checked during review.
 \COUNTERMEASURE \M [CM_OS_TPOWNERTHREAD_DYN_M]

\ID SBSW_OS_TP_TIMERHRTGETCOUNTER_001
 \DESCRIPTION    Os_TimerHrtGetCounter is called with a Timer derived from a Config object. The Config object is
                 passed as argument to the caller function. Precondition ensures that the pointer is valid.
                 The compliance of the precondition is checked during review.
 \COUNTERMEASURE \M [CM_OS_TP_TIMER_M]

\ID SBSW_OS_TP_TIMERHRTGETCOUNTER_002
 \DESCRIPTION    Os_TimerHrtGetCounter is called with a Timer derived from the return value of
                 Os_CoreAsrGetTimingProtection.
 \COUNTERMEASURE \M [CM_OS_TP_TIMER_M]
                 \M [CM_OS_COREASRGETTIMINGPROTECTION1_M]

\ID SBSW_OS_TP_TIMERHRTGETVALUE_001
 \DESCRIPTION    Os_TimerHrtGetValue is called with a Timer derived from a Config object. The Config object is
                 passed as argument to the caller function. Precondition ensures that the pointer is valid.
                 The compliance of the precondition is checked during review.
 \COUNTERMEASURE \M [CM_OS_TP_TIMER_M]

\ID SBSW_OS_TP_TIMERHRTGETVALUE_002
 \DESCRIPTION    Os_TimerHrtGetValue is called with a Timer derived from the return value of
                 Os_CoreAsrGetTimingProtection.
 \COUNTERMEASURE \M [CM_OS_TP_TIMER_M]
                 \M [CM_OS_COREASRGETTIMINGPROTECTION1_M]

\ID SBSW_OS_TP_COUNTERISFUTUREVALUE_001
 \DESCRIPTION    Os_CounterIsFutureValue is called with the return value of Os_TimerHrtGetCounter.
 \COUNTERMEASURE \N [CM_OS_TIMERHRTGETCOUNTER_N]

\ID SBSW_OS_TP_TPGETDYN_001
 \DESCRIPTION    Write access to the Dyn pointer of a Tp object. The Config object is
                 passed as argument to the caller function. Precondition ensures that the pointer is valid.
                 The compliance of the precondition is checked during review.
 \COUNTERMEASURE \M [CM_OS_TP_DYN_M]

\ID SBSW_OS_TP_COUNTERGETMAXALLOWEDVALUE_001
 \DESCRIPTION    Os_CounterGetMaxAllowedValue is called with the return value of Os_TimerHrtGetCounter.
 \COUNTERMEASURE \N [CM_OS_TIMERHRTGETCOUNTER_N]

\ID SBSW_OS_TP_COUNTERGETMAXCOUNTINGVALUE_001
 \DESCRIPTION    Os_CounterGetMaxCountingValue is called with the return value of Os_TimerHrtGetCounter.
 \COUNTERMEASURE \N [CM_OS_TIMERHRTGETCOUNTER_N]

\ID SBSW_OS_TP_TIMERHRTSETCOMPAREVALUE_001
 \DESCRIPTION    Os_TimerHrtSetCompareValue is called with a Timer derived from a Config object. The Config object is
                 passed as argument to the caller function. Precondition ensures that the pointer is valid.
                 The compliance of the precondition is checked during review.
 \COUNTERMEASURE \M [CM_OS_TP_TIMER_M]

\ID SBSW_OS_TP_TIMERHRTSETCOMPAREVALUE_002
 \DESCRIPTION    Os_TimerHrtSetCompareValue is called with a Timer derived from the return value of
                 Os_CoreAsrGetTimingProtection.
 \COUNTERMEASURE \M [CM_OS_TP_TIMER_M]
                 \M [CM_OS_COREASRGETTIMINGPROTECTION1_M]

\ID SBSW_OS_TP_COUNTERSTART_001
 \DESCRIPTION    Os_CounterStart is called with the return value of Os_TimerHrtGetCounter.
 \COUNTERMEASURE \N [CM_OS_TIMERHRTGETCOUNTER_N]

\ID SBSW_OS_TP_COUNTERINIT_001
 \DESCRIPTION    Os_CounterInit is called with the return value of Os_TimerHrtGetCounter.
 \COUNTERMEASURE \N [CM_OS_TIMERHRTGETCOUNTER_N]

\ID SBSW_OS_TP_THREADGETCORE_001
 \DESCRIPTION    Os_ThreadGetCore is called with the return value of Os_CoreGetThread.
 \COUNTERMEASURE \N [CM_OS_COREGETTHREAD_N]

\ID SBSW_OS_TP_TPUPDATEVIOLATION_001
 \DESCRIPTION    Os_TpUpdateViolation is called with the return value of Os_TpGetCurrentTpOwnerThread.
 \COUNTERMEASURE \R [CM_OS_TPGETCURRENTTPOWNERTHREAD_R]

\ID SBSW_OS_TP_TPGETCURRENTTPOWNERTHREAD_001
 \DESCRIPTION    Os_TpGetCurrentTpOwnerThread is called with the return value of Os_CoreAsrGetTimingProtection.
 \COUNTERMEASURE \M [CM_OS_COREASRGETTIMINGPROTECTION1_M]

\ID SBSW_OS_TP_TPISENABLEDBYCORETPCONFIG_001
 \DESCRIPTION    Os_TpIsEnabledByCoreTpConfig is called with the return value of Os_CoreAsrGetTimingProtection.
 \COUNTERMEASURE \M [CM_OS_COREASRGETTIMINGPROTECTION1_M]

\ID SBSW_OS_TP_TPAPP_001
 \DESCRIPTION    Os_TpAdd is called with the return value of Os_CoreAsrGetTimingProtection.
 \COUNTERMEASURE \M [CM_OS_COREASRGETTIMINGPROTECTION1_M]

\ID SBSW_OS_TP_COREASRGETTIMINGPROTECTION_001
 \DESCRIPTION    Os_CoreAsrGetTimingProtection is called with the return value of Os_ThreadGetCore.
 \COUNTERMEASURE \M [CM_OS_THREADGETCORE_M]

\ID SBSW_OS_TP_TIMERHRTGETCOMPAREVALUE_001
 \DESCRIPTION    Os_TimerHrtGetCompareValue is called with a Timer derived from the return value of
                 Os_CoreAsrGetTimingProtection.
 \COUNTERMEASURE \M [CM_OS_TP_TIMER_M]
                 \M [CM_OS_COREASRGETTIMINGPROTECTION1_M]

\ID SBSW_OS_TP_TIMERHRTGETCOMPAREVALUE_002
 \DESCRIPTION    Os_TimerHrtGetCompareValue is called with a Timer derived from a Config object. The Config object is
                 passed as argument to the caller function. Precondition ensures that the pointer is valid.
                 The compliance of the precondition is checked during review.
 \COUNTERMEASURE \M [CM_OS_TP_TIMER_M]

\ID SBSW_OS_TP_TIMERHRTACKNOWLEDGE_001
 \DESCRIPTION    Os_TimerHrtAcknowledge is called with a Timer derived from the return value of
                 Os_CoreAsrGetTimingProtection.
 \COUNTERMEASURE \M [CM_OS_TP_TIMER_M]
                 \M [CM_OS_COREASRGETTIMINGPROTECTION1_M]

\ID SBSW_OS_TP_TIMERISLOGICALCOMPAREVALUEREACHED_001
 \DESCRIPTION    Os_TimerHrtIsLogicalCompareValueReached is called with a Timer derived from the return value of
                 Os_CoreAsrGetTimingProtection.
 \COUNTERMEASURE \M [CM_OS_TP_TIMER_M]
                 \M [CM_OS_COREASRGETTIMINGPROTECTION1_M]

\ID SBSW_OS_TP_TPGETARRIVALTIMESTAMP_001
 \DESCRIPTION    Os_TpGetArrivalTimeStamp is called with a the return value of Os_CoreAsrGetTimingProtection.
 \COUNTERMEASURE \M [CM_OS_COREASRGETTIMINGPROTECTION1_M]

\ID SBSW_OS_TP_THREADGETTPCONFIG_001
 \DESCRIPTION    Os_ThreadGetTpConfig is called with the MonitoredThread derived from the Dyn object of a
                 Config object passed as pointer to the caller function. Precondition ensures that the pointer is valid.
                 The compliance of the precondition is checked during review.
 \COUNTERMEASURE \M [CM_OS_TP_DYN_M]
                 \T [CM_OS_TP_MONITOREDTHREAD_T]

\ID SBSW_OS_TP_TPTHREAD2TPOWNERTHREAD_001
 \DESCRIPTION    Os_TpThread2TpOwnerThread is called with a the return value of Os_ThreadGetTpConfig.
 \COUNTERMEASURE \M [CM_OS_THREADGETTPCONFIG1_M]

\ID SBSW_OS_TP_TPBUDGET_GETDYN_001
 \DESCRIPTION    Write access to the Dyn object of MonitoredBudgets, which is derived from the Dyn object of a
                 TpOwnerThread 'Current'. Current is passed as argument to the caller function. Precondition
                 ensures that Current and MonitoredBudgets, derived from the Current pointer, are valid. The
                 compliance of the precondition is checked during review.
 \COUNTERMEASURE \M [CM_OS_TPOWNERTHREAD_DYN_M]
                 \M [CM_OS_TP_TPBUDGET_DYN_M]

\ID SBSW_OS_TP_TPBUDGET_GETDYN_002
 \DESCRIPTION    Write access to the Dyn object of a budget, which is passed as argument to the caller function.
                 Precondition ensures that Current and MonitoredBudgets, derived from the Current pointer, are valid.
                 Precondition also ensures that the BudgetKind of Budget is not OS_TPMONITORINGTYPE_NONE.
                 The compliance of the precondition is checked during review.
 \COUNTERMEASURE \M [CM_OS_TP_TPBUDGET_DYN_M]

\ID SBSW_OS_TP_TPBUDGET_GETDYN_003
 \DESCRIPTION    Write access to the Dyn object of MonitoredBudgets which is derived from the Dyn object of
                 the return value of Os_TpGetCurrentTpOwnerThread.
 \COUNTERMEASURE \M [CM_OS_TPOWNERTHREAD_DYN_M]
                 \M [CM_OS_TP_TPBUDGET_DYN_M]
                 \R [CM_OS_TPGETCURRENTTPOWNERTHREAD_R]

\ID SBSW_OS_TP_TPSUSPEND_001
 \DESCRIPTION    Os_TpSuspend is called with a Config object and the return value of Os_TpThread2TpOwnerThread.
                 The Config object is passed as argument to the caller function. Precondition ensures that the pointer
                 is valid. The compliance of the precondition is checked during review.
 \COUNTERMEASURE \R [CM_OS_TPTHREAD2TPOWNERTHREAD_R]

\ID SBSW_OS_TP_TPBUDGETSTART_001
 \DESCRIPTION    Os_TpBudgetStart is called with a Config object, the return value of
                 Os_TpGetCurrentTpOwnerThread and the return value of Os_TpGetResourceBudget. The Config object is
                 passed as argument to the caller function. Precondition ensures that the pointer is valid. Precondition
                 of Os_TpGetCurrentTpOwnerThread ensures that the return value is valid. The compliance of the
                 precondition is checked during review.
 \COUNTERMEASURE \R [CM_OS_TPGETRESOURCEBUDGET_01_02_R]
                 \M [CM_OS_TPGETRESOURCEBUDGET_02_02_M]
                 \M [CM_OS_RESOURCEID2RESOURCE_M]

\ID SBSW_OS_TP_TPBUDGETSTART_002
 \DESCRIPTION    Os_TpBudgetStart is called with a Config object and two pointers derived from the return value of
                 Os_TpGetCurrentTpOwnerThread. The Config object is passed as argument to the caller function.
                 Precondition ensures that the pointer is valid. The compliance of the precondition is checked during
                 review.
 \COUNTERMEASURE \N [CM_OS_TPGETCURRENTTPOWNERTHREAD_N]

\ID SBSW_OS_TP_TPBUDGETSTOP_001
 \DESCRIPTION    Os_TpBudgetStop is called with a Config object, the return value of
                 Os_TpGetCurrentTpOwnerThread and the return value of Os_TpGetResourceBudget. The Config object is
                 passed as argument to the caller function. Precondition ensures that the pointer is valid. Precondition
                 of Os_TpGetCurrentTpOwnerThread ensures that the return value is valid. The compliance of the
                 precondition is checked during review.
 \COUNTERMEASURE \R [CM_OS_TPGETRESOURCEBUDGET_01_02_R]
                 \M [CM_OS_TPGETRESOURCEBUDGET_02_02_M]
                 \M [CM_OS_RESOURCEID2RESOURCE_M]

\ID SBSW_OS_TP_TPBUDGETSTOP_002
 \DESCRIPTION    Os_TpBudgetStop is called with a Config object and two pointers derived from the return value of
                 Os_TpGetCurrentTpOwnerThread. The Config object is passed as argument to the caller function.
                 Precondition ensures that the pointer is valid. The compliance of the precondition is checked during
                 review.
 \COUNTERMEASURE \N [CM_OS_TPGETCURRENTTPOWNERTHREAD_N]


SBSW_JUSTIFICATION_END */

/*
\CM CM_OS_TPTHREAD2TPOWNERTHREAD_R
      Implementation ensures that the return value is valid.

\CM CM_OS_TPGETCURRENTTPOWNERTHREAD_R
      Implementation ensures that the value passed to Os_TpThread2TpOwnerThread is an owner thread. Therefore the
      return value is valid.

\CM CM_OS_TPGETCURRENTTPOWNERTHREAD_N
      Precondition of Os_TpGetCurrentTpOwnerThread ensures, that the return value is valid.

\CM CM_OS_TPGETRESOURCEBUDGET_01_02_R
      Implementation ensures that the returned pointer is not used for write accesses, if the pointer is ((void *)0).

\CM CM_OS_TPGETRESOURCEBUDGET_02_02_M
      Verify that:
        1. the ResourceLockBudgets of each TpOwnerThread is no ((void *)0),
        2. each TpOwnerThread's ResourceLockBudgets size is equal to the OS_RESOURCEID_COUNT + 1.

\CM CM_OS_TPOWNERTHREAD_DYN_M
      Verify that the Dyn object of each Os_TpOwnerThreadConfigType is a non ((void *)0).

\CM CM_OS_TP_DYN_M
      Verify that the Dyn object of each Os_TpConfigType is a non ((void *)0).

\CM CM_OS_TP_TIMER_M
      Verify that the Timer object of each Os_TpConfigType is a non ((void *)0).

\CM CM_OS_TP_MONITOREDTHREAD_T
      TCASE-345838 ensures, that MonitoredThread is initialized during initialization phase. Design ensures, that
      MonitoredThread is only modified within the timing protection subcomponent.

\CM CM_OS_TP_TPBUDGET_DYN_M
      Verify for each Os_TpBudgetConfigType, which is not of type OS_TPMONITORINGTYPE_NONE, that the Dyn
      object is a non ((void *)0).

 */
/*!
 * \}
 */

/**********************************************************************************************************************
 *  END OF FILE: Os_TimingProtection.c
 *********************************************************************************************************************/
