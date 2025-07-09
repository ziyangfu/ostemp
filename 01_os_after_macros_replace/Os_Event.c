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
 * \addtogroup Os_Event
 * \{
 *
 *
 * \file
 * \brief       Implementation of \ref Os_Event.
 *
 *
 **********************************************************************************************************************/
/***********************************************************************************************************************
 *  REVISION HISTORY
 *  --------------------------------------------------------------------------------------------------------------------
 *  Refer to Os.h.
 **********************************************************************************************************************/

                                                                                                                        /* PRQA S 0777, 0779, 0828  EOF */ /* MD_MSR_Rule5.1, MD_MSR_Rule5.2, MD_MSR_Dir1.1 */


#define OS_EVENT_SOURCE

/***********************************************************************************************************************
 *  INCLUDES
 **********************************************************************************************************************/
/* AUTOSAR includes */
#include "Std_Types.h"

/* Os module declarations */
#include "Os_Event.h"

/* Os kernel module dependencies */
#include "Os_Task.h"
#include "Os_Task_Lcfg.h"

/* Os hal dependencies */


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

/***********************************************************************************************************************
 *  GLOBAL DATA
 **********************************************************************************************************************/

/***********************************************************************************************************************
 *  LOCAL FUNCTION PROTOTYPES
 **********************************************************************************************************************/
#define OS_START_SEC_CODE
#include "Os_MemMap_OsCode.h"                                                                                           /* PRQA S 5087 */ /* MD_MSR_MemMap */


/***********************************************************************************************************************
 *  Os_EventTriggerSet()
 **********************************************************************************************************************/
/*! \brief          Triggers events.
 *  \details        --no details--
 *
 *  \param[in,out]  Events  The event management object to modify. Parameter must not be NULL.
 *  \param[in]      Mask    The bits to set.
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre            Concurrent access to given object is prevented by caller.
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline void   Os_EventTriggerSet (
    Os_EventStateType OS_VAR_NOINIT * Events,
  EventMaskType Mask
);


/***********************************************************************************************************************
 *  Os_EventTriggerGet()
 **********************************************************************************************************************/
/*! \brief          Returns events which have been triggered.
 *  \details        --no details--
 *
 *  \param[out]     Events  The event management object to query. Parameter must not be NULL.
 *
 *  \return         Event mask containing triggered events.
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre            Concurrent access to given object shall be prevented on platforms with less than 64 bits or caller
 *                  has to ensure consistent read.
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline EventMaskType   Os_EventTriggerGet (
  const   Os_EventStateType OS_VAR_NOINIT * Events
);


/***********************************************************************************************************************
 *  Os_EventTriggerClear()
 **********************************************************************************************************************/
/*! \brief          Clear events from event trigger mask.
 *  \details        --no details--
 *
 *  \param[in,out]  Events  The event management object to modify. Parameter must not be NULL.
 *  \param[in]      Mask    The bits to clear.

 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre            Concurrent access to given object is prevented by caller.
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline void   Os_EventTriggerClear (
    Os_EventStateType OS_VAR_NOINIT * Events,
  EventMaskType Mask
);


/***********************************************************************************************************************
 *  Os_EventWaitEventTriggered()
 **********************************************************************************************************************/
/*! \brief          Returns whether waiting events are triggered (!0) or not (0).
 *  \details        --no details--
 *
 *  \param[in,out]  Events  The event management object to query. Parameter must not be NULL.
 *
 *  \retval         0       No wait event is triggered.
 *  \retval         !0      At least one wait event is triggered.
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre            Concurrent access to given object is prevented by caller.
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline EventMaskType   Os_EventWaitEventTriggered (
  const   Os_EventStateType OS_VAR_NOINIT * Events
);


/***********************************************************************************************************************
 *  Os_EventWaitSet()
 **********************************************************************************************************************/
/*! \brief          Sets the wait events.
 *  \details        --no details--
 *
 *  \param[in,out]  Events  The event management object to modify. Parameter must not be NULL.
 *  \param[in]      Mask    The events to wait for.
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre            Concurrent access to given object is prevented by caller.
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline void   Os_EventWaitSet (
    Os_EventStateType OS_VAR_NOINIT * Events,
  EventMaskType Mask
);


/***********************************************************************************************************************
 *  Os_EventIsEnabled()
 **********************************************************************************************************************/
/*! \brief          Returns whether event API is enabled (!0) or not (0).
 *  \details        --no details--
 *
 *  \retval         !0    Event API is enabled.
 *  \retval         0     Event API is disabled.
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre            -
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StdReturnType   Os_EventIsEnabled (
  void
);


/***********************************************************************************************************************
 *  LOCAL FUNCTIONS
 **********************************************************************************************************************/
/***********************************************************************************************************************
 *  Os_EventTriggerSet()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline void   Os_EventTriggerSet (
    Os_EventStateType OS_VAR_NOINIT * Events,
  EventMaskType Mask
)
{
  Events->Triggered |= Mask;                                                                                            /* SBSW_OS_PWA_PRECONDITION */
}


/***********************************************************************************************************************
 *  Os_EventTriggerGet()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE  static inline EventMaskType   Os_EventTriggerGet (
  const   Os_EventStateType OS_VAR_NOINIT * Events
)
{
  return Events->Triggered;
}


/***********************************************************************************************************************
 *  Os_EventTriggerClear()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline void   Os_EventTriggerClear (
    Os_EventStateType OS_VAR_NOINIT * Events,
  EventMaskType Mask
)
{
  Events->Triggered &= (~Mask);                                                                                         /* SBSW_OS_PWA_PRECONDITION */
}


/***********************************************************************************************************************
 *  Os_EventWaitEventTriggered()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline EventMaskType   Os_EventWaitEventTriggered (
  const   Os_EventStateType OS_VAR_NOINIT * Events
)
{
  /* Both event trigger and waiting are declared volatile. As the fetch order of the arguments of a bitwise 'and'
   * is undefined, a local snapshot of the mask has to be created. This snapshot is used to calculate the
   * 'and' (found by IAR compiler 1.20.01.445 for RH850). */
  EventMaskType triggerd;
  EventMaskType waiting;

  /* #10 Take a snapshot of the event. */
  triggerd = Events->Triggered;

  /* #20 Compute the bitwise 'and'. */
  waiting = Events->Waiting;

  return (triggerd & waiting);
}


/***********************************************************************************************************************
 *  Os_EventWaitSet()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline void   Os_EventWaitSet (
    Os_EventStateType OS_VAR_NOINIT * Events,
  EventMaskType Mask
)
{
  Events->Waiting = Mask;                                                                                               /* SBSW_OS_PWA_PRECONDITION */
}


/***********************************************************************************************************************
 *  Os_EventIsEnabled()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StdReturnType   Os_EventIsEnabled (
  void
)
{
  return (Os_StdReturnType)(OS_CFG_EVENT == STD_ON);                                                                    /* PRQA S 4304, 2995 */ /* MD_Os_C90BooleanCompatibility, MD_Os_Rule2.2_2995 */
}


/***********************************************************************************************************************
 *  GLOBAL FUNCTIONS
 **********************************************************************************************************************/
/***********************************************************************************************************************
 *  Os_Api_SetEvent()
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
Os_StatusType     Os_Api_SetEvent
(
  TaskType TaskID,
  EventMaskType Mask
)
{
  Os_StatusType status;

  /* #10 Perform error checks. */
  if(Os_EventIsEnabled() == 0u)                                                                                         /* PRQA S 2992, 2996 */ /* MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2996 */
  {
    status = Os_ErrSetOkInStdStatus(OS_STATUS_EVENT_DISABLED_BY_CONFIG);                                                /* PRQA S 2880 */ /* MD_Os_Rule2.1_2880 */
  }
  else if(OS_UNLIKELY(Os_TaskCheckId(TaskID) == OS_CHECK_FAILED))
  {
    status = OS_STATUS_ID_1;
  }
  else
  {
    const   Os_ThreadConfigType OS_CONST * currentThread;
    Os_AppAccessMaskType permittedApplications;
    const   Os_AppConfigType OS_CONST * currentApplication;
    const   Os_TaskConfigType OS_CONST * task;

    task = Os_TaskId2Task(TaskID);
    currentThread = Os_CoreGetThread();
    permittedApplications = Os_TaskGetAccessingApplications(task);                                                      /* SBSW_OS_EV_TASKGETACCESSINGAPPLICATIONS_001 */
    currentApplication = Os_ThreadGetCurrentApplication(currentThread);                                                 /* SBSW_OS_EV_THREADGETCURRENTAPPLICATION_001 */

    if(OS_UNLIKELY(Os_ThreadCheckCallContext(currentThread, OS_APICONTEXT_SETEVENT) == OS_CHECK_FAILED))                /* SBSW_OS_EV_THREADCHECKCALLCONTEXT_001 */
    {
      status = OS_STATUS_CALLEVEL;
    }
    else if(OS_UNLIKELY(Os_AppCheckAccess(currentApplication, permittedApplications) == OS_CHECK_FAILED))               /* SBSW_OS_EV_APPCHECKACCESS_001 */
    {
      status = OS_STATUS_ACCESSRIGHTS_1;
    }
    else if(OS_UNLIKELY(Os_ThreadCheckAreInterruptsEnabled(currentThread) == OS_CHECK_FAILED))                          /* SBSW_OS_EV_THREADCHECKAREINTERRUPTSENABLED_001 */
    {
      status = OS_STATUS_DISABLEDINT;
    }
    else if(OS_UNLIKELY(Os_TaskCheckIsExtendedTask(task) == OS_CHECK_FAILED))                                           /* SBSW_OS_EV_TASKCHECKISEXTENDEDTASK_001 */
    {
      status = OS_STATUS_NOEXTENDEDTASK_1;
    }
    else
    {
      /* #20 If the given task belongs to the local core: */
      if(OS_LIKELY(Os_TaskIsCoreLocal(task) != 0u))                                                                     /* SBSW_OS_EV_TASKISCORELOCAL_001 */ /* COV_OS_HALPLATFORMMULTICOREUNSUPPORTED */ /* PRQA S 2991, 2995 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule2.2_2995 */
      {
        const   Os_CoreAsrConfigType OS_CONST * core;
        Os_IntStateType interruptState;

        core = Os_ThreadGetCore(currentThread);                                                                         /* SBSW_OS_EV_THREADGETCORE_001 */

        /* #30 Suspend interrupts. */
        Os_IntSuspend(&interruptState);                                                                                 /* SBSW_OS_FC_POINTER2LOCAL */

        /* #40 Call internal function to set events. */
        status = Os_EventSetLocal(task, Mask);                                                                          /* SBSW_OS_EV_EVENTSETLOCAL_001 */

        /* #50 If the caller is a task and set event leads to a task switch: */
        if(OS_LIKELY(status == OS_STATUS_OK))
        {
          if(Os_ThreadIsTask(currentThread) != 0u)                                                                      /* SBSW_OS_EV_THREADISTASK_001 */
          {
            const   Os_SchedulerConfigType OS_CONST * scheduler;

            scheduler = Os_CoreGetScheduler(core);                                                                      /* SBSW_OS_EV_COREGETSCHEDULER_001 */

            if(Os_SchedulerTaskSwitchIsNeeded(scheduler) != 0u)                                                         /* SBSW_OS_EV_SCHEDULERTASKSWITCHISNEEDED_001 */
            {
              /* #60 Perform a task switch. */
              Os_TaskSwitch(scheduler, currentThread);                                                                  /* SBSW_OS_EV_TASKSWITCH_001 */
            }
          }
        }

        /* #70 Resume interrupts. */
        Os_IntResume(&interruptState);                                                                                  /* SBSW_OS_FC_POINTER2LOCAL */
      }
      /* #80 If the task belongs to a foreign core: */
      else
      {
        const   Os_CoreAsrConfigType OS_CONST * core;
        core = Os_TaskGetCore(task);                                                                                    /* SBSW_OS_EV_TASKGETCORE_001 */

        /* #90 Signal the responsible core to set events. */
        status = Os_XSigSend_SetEvent(core, currentThread, TaskID, Mask);                                               /* SBSW_OS_EV_XSIGSEND_001 */
      }
    }
  }

  return status;
}                                                                                                                       /* PRQA S 6030, 6050, 6080 */ /* MD_MSR_STCYC, MD_MSR_STCAL, MD_MSR_STMIF */


/***********************************************************************************************************************
 *  Os_Api_ClearEvent()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 *
 */
Os_StatusType     Os_Api_ClearEvent
(
  EventMaskType Mask
)
{
  Os_StatusType status;
  const   Os_ThreadConfigType OS_CONST * currentThread;
  const   Os_TaskConfigType OS_CONST * task;

  currentThread = Os_CoreGetThread();

  /* #10 Perform error checks. */
  if(Os_EventIsEnabled() == 0u)                                                                                         /* PRQA S 2992, 2996 */ /* MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2996 */
  {
    status = Os_ErrSetOkInStdStatus(OS_STATUS_EVENT_DISABLED_BY_CONFIG);                                                /* PRQA S 2880 */ /* MD_Os_Rule2.1_2880 */
  }
  else if(OS_UNLIKELY(Os_ThreadCheckCallContext(currentThread, OS_APICONTEXT_CLEAREVENT) == OS_CHECK_FAILED))           /* SBSW_OS_EV_THREADCHECKCALLCONTEXT_001 */
  {
    status = OS_STATUS_CALLEVEL;
  }
  else
  {
    task = Os_TaskThread2Task(currentThread);                                                                           /* SBSW_OS_EV_TASKTHREAD2TASK_001 */

    if(OS_UNLIKELY(Os_ThreadCheckAreInterruptsEnabled(currentThread) == OS_CHECK_FAILED))                               /* SBSW_OS_EV_THREADCHECKAREINTERRUPTSENABLED_001 */
    {
      status = OS_STATUS_DISABLEDINT;
    }
    else if(OS_UNLIKELY(Os_TaskCheckIsExtendedTask(task) == OS_CHECK_FAILED))                                           /* SBSW_OS_EV_TASKCHECKISEXTENDEDTASK_002 */
    {
      status = OS_STATUS_NOEXTENDEDTASK_CALLER;
    }
    else
    {
        Os_EventStateType OS_CONST * event;
      Os_IntStateType interruptState;

      event = Os_TaskGetEvent(task);                                                                                    /* SBSW_OS_EV_TASKGETEVENT_001 */

      /* #20 Suspend interrupts. */
      Os_IntSuspend(&interruptState);                                                                                   /* SBSW_OS_FC_POINTER2LOCAL */

      /* #30 In caller's set event mask, clear all bits given in Mask. */
      Os_EventTriggerClear(event, Mask);                                                                                /* SBSW_OS_EV_EVENTTRIGGERCLEAR_001 */

      /* #40 Resume interrupts. */
      Os_IntResume(&interruptState);                                                                                    /* SBSW_OS_FC_POINTER2LOCAL */

      status = OS_STATUS_OK;
    }
  }

  return status;
}                                                                                                                       /* PRQA S 6050 */ /* MD_MSR_STCAL */


/***********************************************************************************************************************
 *  Os_Api_GetEvent()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
Os_StatusType     Os_Api_GetEvent
(
  TaskType TaskID,
  EventMaskRefType Mask
)
{
  Os_StatusType status;

  /* #10 Perform error checks. */
  if(Os_EventIsEnabled() == 0u)                                                                                         /* PRQA S 2992, 2996 */ /* MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2996 */
  {
    status = Os_ErrSetOkInStdStatus(OS_STATUS_EVENT_DISABLED_BY_CONFIG);                                                /* PRQA S 2880 */ /* MD_Os_Rule2.1_2880 */
  }
  else if(OS_UNLIKELY(Os_TaskCheckId(TaskID) == OS_CHECK_FAILED))
  {
    status = OS_STATUS_ID_1;
  }
  else
  {
    const   Os_ThreadConfigType OS_CONST * currentThread;
    Os_AppAccessMaskType permittedApplications;
    const   Os_AppConfigType OS_CONST * currentApplication;
    const   Os_TaskConfigType OS_CONST * task;

    task = Os_TaskId2Task(TaskID);
    currentThread = Os_CoreGetThread();
    permittedApplications = Os_TaskGetAccessingApplications(task);                                                      /* SBSW_OS_EV_TASKGETACCESSINGAPPLICATIONS_001 */
    currentApplication = Os_ThreadGetCurrentApplication(currentThread);                                                 /* SBSW_OS_EV_THREADGETCURRENTAPPLICATION_001 */

    if(OS_UNLIKELY(Os_ThreadCheckCallContext(currentThread, OS_APICONTEXT_GETEVENT) == OS_CHECK_FAILED))                /* SBSW_OS_EV_THREADCHECKCALLCONTEXT_001 */
    {
      status = OS_STATUS_CALLEVEL;
    }
    else if(OS_UNLIKELY(Os_AppCheckAccess(currentApplication, permittedApplications) == OS_CHECK_FAILED))               /* SBSW_OS_EV_APPCHECKACCESS_001 */
    {
      status = OS_STATUS_ACCESSRIGHTS_1;
    }
    else if(OS_UNLIKELY(Os_ThreadCheckAreInterruptsEnabled(currentThread) == OS_CHECK_FAILED))                          /* SBSW_OS_EV_THREADCHECKAREINTERRUPTSENABLED_001 */
    {
      status = OS_STATUS_DISABLEDINT;
    }
    else if(OS_UNLIKELY(Os_TaskCheckIsExtendedTask(task) == OS_CHECK_FAILED))                                           /* SBSW_OS_EV_TASKCHECKISEXTENDEDTASK_001 */
    {
      status = OS_STATUS_NOEXTENDEDTASK_1;
    }
    else
    {
      /* #20 If the given task belongs to the local core get the event locally. */
      if(OS_LIKELY(Os_TaskIsCoreLocal(task) != 0u))                                                                     /* SBSW_OS_EV_TASKISCORELOCAL_001 */ /* COV_OS_HALPLATFORMMULTICOREUNSUPPORTED */ /* PRQA S 2991, 2995 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule2.2_2995 */
      {
        status = Os_EventGetLocal(task, Mask);                                                                          /* SBSW_OS_EV_EVENTGETLOCAL_001 */
      }
      /* #30 Otherwise signal the responsible core to get events. */
      else
      {
        const   Os_CoreAsrConfigType OS_CONST * core;
        core = Os_TaskGetCore(task);                                                                                    /* SBSW_OS_EV_TASKGETCORE_001 */

        status = Os_XSigSend_GetEvent(core, currentThread, TaskID, Mask);                                               /* SBSW_OS_EV_XSIGSEND_001 */
      }
    }
  }

  return status;
}                                                                                                                       /* PRQA S 6050, 6080 */ /* MD_MSR_STCAL, MD_MSR_STMIF */


/***********************************************************************************************************************
 *  Os_Api_WaitEvent()
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
Os_StatusType     Os_Api_WaitEvent
(
  EventMaskType Mask
)
{
  Os_StatusType status;
  const   Os_ThreadConfigType OS_CONST * currentThread;

  currentThread = Os_CoreGetThread();

  /* #10 Perform error checks. */
  if(Os_EventIsEnabled() == 0u)                                                                                         /* PRQA S 2992, 2996 */ /* MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2996 */
  {
    status = Os_ErrSetOkInStdStatus(OS_STATUS_EVENT_DISABLED_BY_CONFIG);                                                /* PRQA S 2880 */ /* MD_Os_Rule2.1_2880 */
  }
  else if(OS_UNLIKELY(Os_ThreadCheckCallContext(currentThread, OS_APICONTEXT_WAITEVENT) == OS_CHECK_FAILED))            /* SBSW_OS_EV_THREADCHECKCALLCONTEXT_001 */
  {
    status = OS_STATUS_CALLEVEL;
  }
  else
  {
    const   Os_TaskConfigType OS_CONST * currentTask;

    currentTask = Os_TaskThread2Task(currentThread);                                                                    /* SBSW_OS_EV_TASKTHREAD2TASK_001 */

    if(OS_UNLIKELY(Os_TaskCheckIsExtendedTask(currentTask) == OS_CHECK_FAILED))                                         /* SBSW_OS_EV_TASKCHECKISEXTENDEDTASK_002 */
    {
      status = OS_STATUS_NOEXTENDEDTASK_CALLER;
    }
    else if(OS_UNLIKELY(Os_ThreadCheckResourcesReleased(currentThread) == OS_CHECK_FAILED))                             /* SBSW_OS_EV_THREADCHECKRESOURCESRELEASED_001 */
    {
      status = OS_STATUS_RESOURCE;
    }
    else if(OS_UNLIKELY(Os_ThreadCheckSpinlocksReleased(currentThread) == OS_CHECK_FAILED))                             /* SBSW_OS_EV_THREADCHECKSPINLOCKSRELEASED_001 */ /* COV_OS_HALPLATFORMTESTEDASMULTICORE */
    {
      status = OS_STATUS_SPINLOCK;
    }
    else if(OS_UNLIKELY(Os_ThreadCheckAreInterruptsEnabled(currentThread) == OS_CHECK_FAILED))                          /* SBSW_OS_EV_THREADCHECKAREINTERRUPTSENABLED_001 */
    {
      status = OS_STATUS_DISABLEDINT;
    }
    else
    {
      const   Os_TpConfigType OS_CONST * configTp =
          Os_CoreAsrGetTimingProtection(Os_ThreadGetCore(currentThread));                                               /* SBSW_OS_TASK_COREASRGETTIMINGPROTECTION_001 */ /* SBSW_OS_TASK_THREADGETCORE_002 */
      const   Os_TpThreadConfigType OS_CONST * threadTp = Os_ThreadGetTpConfig(currentThread);                 /* SBSW_OS_TASK_THREADGETTPCONFIG_001 */
        Os_EventStateType OS_CONST * event;
      Os_IntStateType interruptState;
      boolean setWaitState = TRUE;

      event = Os_TaskGetEvent(currentTask);                                                                             /* SBSW_OS_EV_TASKGETEVENT_001 */

      /* #20 Suspend interrupts. */
      Os_IntSuspend(&interruptState);                                                                                   /* SBSW_OS_FC_POINTER2LOCAL */

      /* #30 Set task's wait event mask to given Mask. */
      Os_EventWaitSet(event, Mask);                                                                                     /* SBSW_OS_EV_EVENTWAITSET_001 */

      /* #40 If the events, the task wants to wait for, are already set: */
      if((Os_EventWaitEventTriggered(event) != 0u))                                                                     /* SBSW_OS_EV_EVENTWAITEVENTTRIGGERED_001 */
      {
        /* #50 If no inter arrival time violation exists,
         * tell timing protection to start task time frame (inter-arrival time). */
        if(OS_LIKELY(Os_TpCheckInterArrival(configTp, threadTp) != OS_CHECK_FAILED))                                    /* SBSW_OS_TASK_TPCHECKINTERARRIVAL_001 */ /* PRQA S 2991, 2995 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule2.2_2995 */
        {
          /* #60 Tell timing protection to reset execution budget. */
          /* current and next TP config are the same here */
          Os_TpResetAndResume(configTp, threadTp, threadTp, currentThread);                                             /* SBSW_OS_TASK_TPRESETANDRESUME_001 */

          /* #70 Task should not be set into wait state */
          setWaitState = FALSE;

          /* #80 Inform the trace object about this. */
          Os_TraceTaskWaitEventNoWait(Os_ThreadGetTrace(currentThread), Mask);                                          /* SBSW_OS_EV_THREADGETTRACE_001 */ /* SBSW_OS_EV_THREADSETTRACE_002 */
        }
      }

      /* #90 If the task should be set into wait state */
      if (setWaitState == TRUE)
      {
        /* #100 Put the task into the waiting state. */
        Os_TaskWait(currentTask);                                                                                       /* SBSW_OS_EV_TASKWAIT_001 */
      }

      /* #110 Resume interrupts. */
      Os_IntResume(&interruptState);                                                                                    /* SBSW_OS_FC_POINTER2LOCAL */

      status = OS_STATUS_OK;
    }
  }

  return status;
}                                                                                                                       /* PRQA S 6050, 6080 */ /* MD_MSR_STCAL, MD_MSR_STMIF */


/***********************************************************************************************************************
 *  Os_EventSetLocal()
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
Os_StatusType     Os_EventSetLocal
(
  const   Os_TaskConfigType OS_CONST * Task,
  EventMaskType Mask
)
{
  Os_IntStateType intState;
  Os_StatusType status;
    Os_EventStateType OS_CONST * event;
  const   Os_ThreadConfigType OS_CONST * thread;
  const   Os_AppConfigType OS_CONST * ownerApplication;

  /* #10 If the module is not enabled: KernelPanic. */
  if(Os_EventIsEnabled() == 0u)                                                                                         /* COV_OS_INVSTATE */ /* PRQA S 2992, 2996 */ /* MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2996 */
  {
    Os_ErrKernelPanic();                                                                                                /* PRQA S 2880 */ /* MD_Os_Rule2.1_2880 */
  }

  thread = Os_TaskGetThread(Task);                                                                                      /* SBSW_OS_FC_PRECONDITION */
  ownerApplication = Os_ThreadGetOwnerApplication(thread);                                                              /* SBSW_OS_EV_THREADGETOWNERAPPLICATION_002 */

  Os_IntSuspend(&intState);                                                                                             /* SBSW_OS_FC_POINTER2LOCAL */

  /* #20 Check whether the owner application of the given task is accessible. */
  if(OS_UNLIKELY(Os_AppCheckIsAccessible(ownerApplication) == OS_CHECK_FAILED))                                         /* SBSW_OS_EV_APPCHECKISACCESSIBLE_001 */
  {
    status = OS_STATUS_NOTACCESSIBLE_1;
  }
  else if(OS_UNLIKELY(Os_TaskCheckIsNotSuspended(Task) == OS_CHECK_FAILED))                                             /* SBSW_OS_FC_PRECONDITION */
  {
    status = OS_STATUS_STATE_1;
  }
  else
  {
    EventMaskType waitEventsTriggerd;

    event = Os_TaskGetEvent(Task);                                                                                      /* SBSW_OS_FC_PRECONDITION */

    /* #30 Set task's trigger event mask according to given mask. */
    Os_EventTriggerSet(event, Mask);                                                                                    /* SBSW_OS_EV_EVENTTRIGGERSET_001 */

    waitEventsTriggerd = Os_EventWaitEventTriggered(event);                                                             /* SBSW_OS_EV_EVENTWAITEVENTTRIGGERED_001 */

    /* #40 If the task has been WAITING for one of the events: resume the task from waiting state. */
    if( (Os_TaskIsWaiting(Task) != 0u) && (waitEventsTriggerd != 0u) )                                                  /* SBSW_OS_FC_PRECONDITION */
    {
      Os_TaskLeaveWaitingState(Task);                                                                                   /* SBSW_OS_FC_PRECONDITION */

      /* #50 Inform the trace object about this */
      Os_TraceTaskSetEvent(Os_ThreadGetTrace(thread), Mask, TRUE);                                                      /* SBSW_OS_EV_THREADGETTRACE_001 */ /* SBSW_OS_EV_THREADSETTRACE_001 */
    }
    else
    {
      Os_TraceTaskSetEvent(Os_ThreadGetTrace(thread), Mask, FALSE);                                                     /* SBSW_OS_EV_THREADGETTRACE_001 */ /* SBSW_OS_EV_THREADSETTRACE_001 */
    }

    status = OS_STATUS_OK;
  }

  Os_IntResume(&intState);                                                                                              /* SBSW_OS_FC_POINTER2LOCAL */

  return status;
}                                                                                                                       /* PRQA S 6050 */ /* MD_MSR_STCAL */


/***********************************************************************************************************************
 *  Os_EventSetInternal()
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
Os_StatusType     Os_EventSetInternal
(
  const   Os_TaskConfigType OS_CONST * Task,
  EventMaskType Mask
)
{
  Os_StatusType status;

  /* #10 If the module is not enabled: KernelPanic. */
  if(Os_EventIsEnabled() == 0u)                                                                                         /* COV_OS_INVSTATE */ /* PRQA S 2992, 2996 */ /* MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2996 */
  {
    Os_ErrKernelPanic();                                                                                                /* PRQA S 2880 */ /* MD_Os_Rule2.1_2880 */
  }

  /* #20 If the given task belongs to the local core, */
  if(OS_LIKELY(Os_TaskIsCoreLocal(Task) != 0u))                                                                         /* SBSW_OS_FC_PRECONDITION */ /* COV_OS_HALPLATFORMMULTICOREUNSUPPORTED */ /* PRQA S 2991, 2995 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule2.2_2995 */
  {
    /* #30 Call local function to set the event. */
    status = Os_EventSetLocal(Task, Mask);                                                                              /* SBSW_OS_EV_EVENTSETLOCAL_001 */
  }
  /* #40 Otherwise the task belongs to a foreign core, */
  else
  {
    /* #50 Signal the responsible core to activate the task. */
    status = Os_XSigSend_SetEventAsync(Os_TaskGetCore(Task), Os_CoreGetThread(), Os_TaskGetId(Task), Mask);             /* SBSW_OS_FC_PRECONDITION */ /* SBSW_OS_EV_XSIGSEND_001 */ /* SBSW_OS_FC_PRECONDITION */
  }

  return status;
}                                                                                                                       /* PRQA S 6050 */ /* MD_MSR_STCAL */


/***********************************************************************************************************************
 *  Os_EventGetLocal()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 *
 */
Os_StatusType     Os_EventGetLocal
(
  const   Os_TaskConfigType OS_CONST * Task,
  EventMaskRefType Mask
)
{
  Os_StatusType status;
  const   Os_ThreadConfigType OS_CONST * thread;
  const   Os_AppConfigType OS_CONST * ownerApplication;

  /* #10 If the module is not enabled: KernelPanic. */
  if(Os_EventIsEnabled() == 0u)                                                                                         /* COV_OS_INVSTATE */ /* PRQA S 2992, 2996 */ /* MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2996 */
  {
    Os_ErrKernelPanic();                                                                                                /* PRQA S 2880 */ /* MD_Os_Rule2.1_2880 */
  }

  thread = Os_TaskGetThread(Task);                                                                                      /* SBSW_OS_FC_PRECONDITION */
  ownerApplication = Os_ThreadGetOwnerApplication(thread);                                                              /* SBSW_OS_EV_THREADGETOWNERAPPLICATION_002 */

  /* #20 Perform runtime data checks. */
  if(OS_UNLIKELY(Os_TaskCheckIsNotSuspended(Task) == OS_CHECK_FAILED))                                                  /* SBSW_OS_FC_PRECONDITION */
  {
    status = OS_STATUS_STATE_1;
  }
  else if(OS_UNLIKELY(Os_AppCheckIsAccessible(ownerApplication) == OS_CHECK_FAILED))                                    /* SBSW_OS_EV_APPCHECKISACCESSIBLE_001 */
  {
    status = OS_STATUS_NOTACCESSIBLE_1;
  }
  else
  {
    const   Os_EventStateType OS_CONST * event;
    event = Os_TaskGetEvent(Task);                                                                                      /* SBSW_OS_FC_PRECONDITION */

    do
    {
      /* #30 Read task's trigger event mask from given pointer. */
      (*Mask) = Os_EventTriggerGet(event);                                                                              /* SBSW_OS_EV_EVENTTRIGGERGET_001 */ /* SBSW_OS_PWA_PRECONDITION */

    /* #40 Ensure that the read value was not changed partly during write. */
    /* This is relevant for architectures where the word width is smaller then the event mask width. */
    }while((*Mask) != Os_EventTriggerGet(event));                                                                       /* SBSW_OS_EV_EVENTTRIGGERGET_001 */ /* COV_OS_INVSTATE */

    status = OS_STATUS_OK;
  }

  return status;
}                                                                                                                       /* PRQA S 6050 */ /* MD_MSR_STCAL */


#define OS_STOP_SEC_CODE
#include "Os_MemMap_OsCode.h"                                                                                           /* PRQA S 5087 */ /* MD_MSR_MemMap */


/* module specific MISRA deviations:
 */


/* SBSW_JUSTIFICATION_BEGIN

\ID SBSW_OS_EV_THREADGETOWNERAPPLICATION_001
 \DESCRIPTION    Os_ThreadGetOwnerApplication is called with the return value of Os_CoreGetThread.
 \COUNTERMEASURE \N [CM_OS_COREGETTHREAD_N]

\ID SBSW_OS_EV_THREADGETOWNERAPPLICATION_002
 \DESCRIPTION    Os_ThreadGetOwnerApplication is called with the return value of Os_TaskGetThread.
 \COUNTERMEASURE \N [CM_OS_TASKGETTHREAD_N]

\ID SBSW_OS_EV_TASKGETACCESSINGAPPLICATIONS_001
 \DESCRIPTION    Os_TaskGetAccessingApplications is called with the return value of Os_TaskId2Task.
 \COUNTERMEASURE \M [CM_OS_TASKID2TASK_M]

\ID SBSW_OS_EV_THREADCHECKCALLCONTEXT_001
 \DESCRIPTION    Os_ThreadCheckCallContext is called with the return value of Os_CoreGetThread.
 \COUNTERMEASURE \N [CM_OS_COREGETTHREAD_N]

\ID SBSW_OS_EV_THREADCHECKAREINTERRUPTSENABLED_001
 \DESCRIPTION    Os_ThreadCheckAreInterruptsEnabled is called with the return value of Os_CoreGetThread.
 \COUNTERMEASURE \N [CM_OS_COREGETTHREAD_N]

\ID SBSW_OS_EV_TASKCHECKISEXTENDEDTASK_001
 \DESCRIPTION    Os_TaskCheckIsExtendedTask is called with the return value of Os_TaskId2Task.
 \COUNTERMEASURE \M [CM_OS_TASKID2TASK_M]

\ID SBSW_OS_EV_TASKCHECKISEXTENDEDTASK_002
 \DESCRIPTION    Os_TaskCheckIsExtendedTask is called with the return value of Os_TaskThread2Task.
 \COUNTERMEASURE \R [CM_OS_TASKTHREAD2TASK_R]

\ID SBSW_OS_EV_TASKISCORELOCAL_001
 \DESCRIPTION    Os_TaskIsCoreLocal is called with the return value of Os_TaskId2Task.
 \COUNTERMEASURE \M [CM_OS_TASKID2TASK_M]

\ID SBSW_OS_EV_APPCHECKISACCESSIBLE_001
 \DESCRIPTION    Os_AppCheckIsAccessible is called with the return value of Os_ThreadGetOwnerApplication.
 \COUNTERMEASURE \M [CM_OS_THREADGETOWNERAPPLICATION_M]

\ID SBSW_OS_EV_THREADGETCORE_001
 \DESCRIPTION    Os_ThreadGetCore is called with the return value of Os_CoreGetThread.
 \COUNTERMEASURE \N [CM_OS_COREGETTHREAD_N]

\ID SBSW_OS_EV_THREADGETCORE_002
 \DESCRIPTION    Os_ThreadGetCore is called with the return value of Os_TaskGetThread.
 \COUNTERMEASURE \N [CM_OS_TASKGETTHREAD_N]

\ID SBSW_OS_EV_THREADISTASK_001
 \DESCRIPTION    Os_ThreadIsTask is called with the return value of Os_CoreGetThread.
 \COUNTERMEASURE \N [CM_OS_COREGETTHREAD_N]

\ID SBSW_OS_EV_COREGETSCHEDULER_001
 \DESCRIPTION    Os_CoreGetScheduler is called with the return value of Os_ThreadGetCore.
 \COUNTERMEASURE \M [CM_OS_THREADGETCORE_M]

\ID SBSW_OS_EV_SCHEDULERTASKSWITCHISNEEDED_001
 \DESCRIPTION    Os_SchedulerTaskSwitchIsNeeded is called with the return value of Os_CoreGetScheduler.
 \COUNTERMEASURE \M [CM_OS_COREGETSCHEDULER_M]

\ID SBSW_OS_EV_TASKSWITCH_001
 \DESCRIPTION    Os_TaskSwitch is called with the return values of Os_CoreGetScheduler and Os_CoreGetThread.
                 The return value of Os_CoreGetThread is always valid (ensured by precondition).
 \COUNTERMEASURE \M [CM_OS_COREGETSCHEDULER_M]

\ID SBSW_OS_EV_TASKGETCORE_001
 \DESCRIPTION    Os_TaskGetCore is called with the return value of Os_TaskId2Task.
 \COUNTERMEASURE \M [CM_OS_TASKID2TASK_M]

\ID SBSW_OS_EV_XSIGSEND_001
 \DESCRIPTION    A cross core send function is called, with the return values of Os_TaskGetCore() and
                 Os_CoreGetThread(). Correctness of Os_CoreGetThread is ensured by preconditions.
 \COUNTERMEASURE \M [CM_OS_TASKGETCORE_M]

\ID SBSW_OS_EV_EVENTSETLOCAL_001
 \DESCRIPTION    Os_EventSetLocal is called with the return value of Os_TaskId2Task.
 \COUNTERMEASURE \M [CM_OS_TASKID2TASK_M]

\ID SBSW_OS_EV_TASKTHREAD2TASK_001
 \DESCRIPTION    Os_TaskThread2Task is called with the return value of Os_CoreGetThread.
 \COUNTERMEASURE \N [CM_OS_COREGETTHREAD_N]

\ID SBSW_OS_EV_TASKGETEVENT_001
 \DESCRIPTION    Os_TaskGetEvent is called with the return value of Os_TaskThread2Task.
 \COUNTERMEASURE \R [CM_OS_TASKTHREAD2TASK_R]

\ID SBSW_OS_EV_EVENTTRIGGERCLEAR_001
 \DESCRIPTION    Os_EventTriggerClear is called with the return value of Os_TaskGetEvent.
 \COUNTERMEASURE \M [CM_OS_TASKGETEVENT_M]

\ID SBSW_OS_EV_EVENTTRIGGERGET_001
 \DESCRIPTION    Os_EventTriggerGet is called with the return value of Os_TaskGetEvent.
 \COUNTERMEASURE \M [CM_OS_TASKGETEVENT_M]

\ID SBSW_OS_EV_THREADGETCURRENTAPPLICATION_001
 \DESCRIPTION    Os_ThreadGetCurrentApplication is called with the return value of Os_CoreGetThread.
 \COUNTERMEASURE \N [CM_OS_COREGETTHREAD_N]

\ID SBSW_OS_EV_THREADCHECKRESOURCESRELEASED_001
 \DESCRIPTION    Os_ThreadCheckResourcesReleased is called with the return value of Os_CoreGetThread.
 \COUNTERMEASURE \N [CM_OS_COREGETTHREAD_N]

\ID SBSW_OS_EV_THREADCHECKSPINLOCKSRELEASED_001
 \DESCRIPTION    Os_ThreadCheckSpinlocksReleased is called with the return value of Os_CoreGetThread.
 \COUNTERMEASURE \N [CM_OS_COREGETTHREAD_N]

\ID SBSW_OS_EV_EVENTWAITSET_001
 \DESCRIPTION    Os_EventWaitSet is called with the return value of Os_TaskGetEvent.
 \COUNTERMEASURE \M [CM_OS_TASKGETEVENT_M]

\ID SBSW_OS_EV_EVENTGETLOCAL_001
 \DESCRIPTION    Os_EventGetLocal is called with the return value of Os_TaskId2Task.
 \COUNTERMEASURE \M [CM_OS_TASKID2TASK_M]

\ID SBSW_OS_EV_EVENTWAITEVENTTRIGGERED_001
 \DESCRIPTION    Os_EventWaitEventTriggered is called with the return value of Os_TaskGetEvent.
 \COUNTERMEASURE \M [CM_OS_TASKGETEVENT_M]

\ID SBSW_OS_EV_TASKWAIT_001
 \DESCRIPTION    Os_TaskWait is called with the return value of Os_TaskThread2Task.
 \COUNTERMEASURE \R [CM_OS_TASKTHREAD2TASK_R]

\ID SBSW_OS_EV_EVENTTRIGGERSET_001
 \DESCRIPTION    Os_EventTriggerSet is called with the return value of Os_TaskGetEvent.
 \COUNTERMEASURE \M [CM_OS_TASKGETEVENT_M]

\ID SBSW_OS_EV_THREADGETTRACE_001
 \DESCRIPTION    Os_ThreadGetTrace is called with the return value of Os_TaskGetThread.
 \COUNTERMEASURE \N [CM_OS_TASKGETTHREAD_N]

\ID SBSW_OS_EV_THREADSETTRACE_001
 \DESCRIPTION    Os_TraceTaskSetEvent is called with the return value of Os_ThreadGetTrace.
 \COUNTERMEASURE \M [CM_OS_THREADGETTRACE_M]

\ID SBSW_OS_EV_THREADSETTRACE_002
 \DESCRIPTION    Os_TraceTaskWaitEventNoWait is called with the return value of Os_ThreadGetTrace.
 \COUNTERMEASURE \M [CM_OS_THREADGETTRACE_M]

\ID SBSW_OS_EV_APPCHECKACCESS_001
 \DESCRIPTION    Os_AppCheckAccess is called with the return value of Os_ThreadGetCurrentApplication.
 \COUNTERMEASURE \N [CM_OS_THREADGETCURRENTAPPLICATION_N]

SBSW_JUSTIFICATION_END */


/*!
 * \}
 */

/***********************************************************************************************************************
 *  END OF FILE: Os_Event.c
 **********************************************************************************************************************/
