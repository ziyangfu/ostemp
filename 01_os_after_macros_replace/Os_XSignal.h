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
 * \addtogroup Os_XSignal
 * \{
 *
 * \file
 * \brief       Implementation of cross core signal services.
 *
 *
 **********************************************************************************************************************/
/***********************************************************************************************************************
 *  REVISION HISTORY
 *  --------------------------------------------------------------------------------------------------------------------
 *  Refer to Os.h.
 **********************************************************************************************************************/

#ifndef OS_XSIGNAL_H
# define OS_XSIGNAL_H
                                                                                                                         /* PRQA S 0388 EOF */ /* MD_MSR_Dir1.1 */
/***********************************************************************************************************************
 *  INCLUDES
 **********************************************************************************************************************/
/* AUTOSAR includes */
# include "Std_Types.h"

/* Os module declarations */
# include "Os_XSignalInt.h"
# include "Os_XSignal_Lcfg.h"


/* Os kernel module dependencies */
# include "Os_Core.h"
# include "Os_Counter.h"
# include "Os_Task.h"
# include "Os_Event.h"
# include "Os_Alarm.h"
# include "Os_ScheduleTable.h"
# include "Os_Isr.h"
# include "Os_Thread.h"
# include "Os_XSignalChannel.h"
# include "Os_Error.h"
# include "Os_Interrupt.h"
# include "Os_Common.h"
# include "Os_Hook.h"

/* Os hal dependencies */
# include "Os_Hal_Core.h"
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
 *  Os_XSigIsEnabled()
 **********************************************************************************************************************/
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
OS_ALWAYS_INLINE static inline Os_StdReturnType   Os_XSigIsEnabled (
  void
)
{
  return (OS_CFG_XSIGNAL == STD_ON);                                                                                    /* PRQA S 4304, 4404, 2995, 2996 */ /* MD_Os_C90BooleanCompatibility, MD_Os_C90BooleanCompatibility, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
}


/***********************************************************************************************************************
 *  Os_XSigUseAsynchronous()
 **********************************************************************************************************************/
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */ /* COV_OS_HALPLATFORMMULTICOREUNSUPPORTED */
OS_ALWAYS_INLINE static inline Os_StdReturnType   Os_XSigUseAsynchronous (
  void
)                                                                                                                      /* COV_OS_HALPLATFORMMULTICOREUNSUPPORTED */
{
  return (OS_CFG_XSIG_USEASYNC == STD_ON);                                                                              /* PRQA S 4304, 4404, 2995, 2996 */ /* MD_Os_C90BooleanCompatibility, MD_Os_C90BooleanCompatibility, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
}


/***********************************************************************************************************************
 *  Os_XSigSend()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 */
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */ /* COV_OS_HALPLATFORMMULTICOREUNSUPPORTED */
OS_ALWAYS_INLINE static inline Os_StatusType   Os_XSigSend (
  const   Os_CoreAsrConfigType OS_CONST * RecvCore,
  const   Os_ThreadConfigType OS_CONST * SendThread,
  Os_XSigFunctionIdx FunctionIdx,
  const   Os_XSigParamsType   * Parameters
)                                                                                                                      /* COV_OS_HALPLATFORMMULTICOREUNSUPPORTED */
{
  Os_StatusType status;

  /* #10 Select the configured send function. */
  if(Os_XSigUseAsynchronous() != 0u)                                                                                    /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
  {
    status = Os_XSigSendAsync(RecvCore, SendThread, FunctionIdx, Parameters);                                           /* SBSW_OS_FC_PRECONDITION */
  }
  else
  {
    volatile Os_XSigOutputParamsType OutParameters;
    status = Os_XSigSendSync(RecvCore, SendThread, FunctionIdx, Parameters, &OutParameters);                            /* SBSW_OS_XSIG_XSIGSEND_001 */
  }

  return status;
}


/***********************************************************************************************************************
 *  Os_XSigSend_ActivateTask()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 */
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */ /* COV_OS_HALPLATFORMMULTICOREUNSUPPORTED */
OS_ALWAYS_INLINE static inline Os_StatusType   Os_XSigSend_ActivateTask (
  const   Os_CoreAsrConfigType OS_CONST * RecvCore,
  const   Os_ThreadConfigType OS_CONST * SendThread,
  TaskType TaskID
)                                                                                                                      /* COV_OS_HALPLATFORMMULTICOREUNSUPPORTED */
{
  Os_XSigParamsType parameters;

  /* #10 Initialize the call parameters. */
  parameters.ParameterData[0].ParamTaskType = TaskID;                                                                   /* SBSW_OS_PWA_LOCAL */

  /* #20 Send the request. */
  return Os_XSigSend(RecvCore, SendThread, XSigFunctionIdx_ActivateTask, &parameters);                                  /* SBSW_OS_XSIG_XSIGSEND_001 */
}


/***********************************************************************************************************************
 *  Os_XSigSend_ActivateTaskAsync()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 */
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */ /* COV_OS_HALPLATFORMMULTICOREUNSUPPORTED */
OS_ALWAYS_INLINE static inline Os_StatusType   Os_XSigSend_ActivateTaskAsync (
  const   Os_CoreAsrConfigType OS_CONST * RecvCore,
  const   Os_ThreadConfigType OS_CONST * SendThread,
  TaskType TaskID
)                                                                                                                      /* COV_OS_HALPLATFORMMULTICOREUNSUPPORTED */
{
  Os_XSigParamsType parameters;

  /* #10 Initialize the call parameters. */
  parameters.ParameterData[0].ParamTaskType = TaskID;                                                                   /* SBSW_OS_PWA_LOCAL */

  /* #20 Send the request. */
  return Os_XSigSendAsync(RecvCore, SendThread, XSigFunctionIdx_ActivateTask, &parameters);                             /* SBSW_OS_XSIG_XSIGSEND_001 */
}


/***********************************************************************************************************************
 *  Os_XSigSend_GetTaskState()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 */
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */ /* COV_OS_HALPLATFORMMULTICOREUNSUPPORTED */
OS_ALWAYS_INLINE static inline Os_StatusType   Os_XSigSend_GetTaskState (
  const   Os_CoreAsrConfigType OS_CONST * RecvCore,
  const   Os_ThreadConfigType OS_CONST * SendThread,
  TaskType TaskID,
  TaskStateRefType State
)                                                                                                                      /* COV_OS_HALPLATFORMMULTICOREUNSUPPORTED */
{
  Os_StatusType status;
  Os_XSigParamsType parameters;
  volatile Os_XSigOutputParamsType outParameters;

  /* #10 Initialize the call parameters. */
  parameters.ParameterData[0].ParamTaskType = TaskID;                                                                   /* SBSW_OS_PWA_LOCAL */

  /* #20 Send the request. */
  status = Os_XSigSendSync(RecvCore, SendThread, XSigFunctionIdx_GetTaskState, &parameters, &outParameters);            /* SBSW_OS_XSIG_XSIGSEND_001 */
  (*State) = outParameters.ParameterData.ParamTaskStateType;                                                            /* SBSW_OS_PWA_PRECONDITION */

  return status;
}


/***********************************************************************************************************************
 *  Os_XSigSend_GetCounterValue()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 */
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */ /* COV_OS_HALPLATFORMMULTICOREUNSUPPORTED */
OS_ALWAYS_INLINE static inline Os_StatusType   Os_XSigSend_GetCounterValue (
  const   Os_CoreAsrConfigType OS_CONST * RecvCore,
  const   Os_ThreadConfigType OS_CONST * SendThread,
  CounterType CounterID,
  TickRefType Value
)                                                                                                                      /* COV_OS_HALPLATFORMMULTICOREUNSUPPORTED */
{
  Os_StatusType status;
  Os_XSigParamsType parameters;
  volatile Os_XSigOutputParamsType outParameters;

  /* #10 Initialize the call parameters. */
  parameters.ParameterData[0].ParamCounterType = CounterID;                                                             /* SBSW_OS_PWA_LOCAL */

  /* #20 Send the request. */
  status = Os_XSigSendSync(RecvCore, SendThread, XSigFunctionIdx_GetCounterValue, &parameters, &outParameters);         /* SBSW_OS_XSIG_XSIGSEND_001 */
  (*Value) = outParameters.ParameterData.ParamTickType;                                                                 /* SBSW_OS_PWA_PRECONDITION */

  return status;
}


/***********************************************************************************************************************
 *  Os_XSigSend_SetEvent()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 */
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */ /* COV_OS_HALPLATFORMMULTICOREUNSUPPORTED */
OS_ALWAYS_INLINE static inline Os_StatusType   Os_XSigSend_SetEvent (
  const   Os_CoreAsrConfigType OS_CONST * RecvCore,
  const   Os_ThreadConfigType OS_CONST * SendThread,
  TaskType TaskID,
  EventMaskType Mask
)                                                                                                                      /* COV_OS_HALPLATFORMMULTICOREUNSUPPORTED */
{
  Os_XSigParamsType parameters;

  /* #10 Initialize the call parameters. */
  parameters.ParameterData[0].ParamTaskType = TaskID;                                                                   /* SBSW_OS_PWA_LOCAL */
  parameters.ParameterData[1].ParamEventMaskType = Mask;                                                                /* SBSW_OS_PWA_LOCAL */

  /* #20 Send the request. */
  return Os_XSigSend(RecvCore, SendThread, XSigFunctionIdx_SetEvent, &parameters);                                      /* SBSW_OS_XSIG_XSIGSEND_001 */
}


/***********************************************************************************************************************
 *  Os_XSigSend_SetEventAsync()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 */
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */ /* COV_OS_HALPLATFORMMULTICOREUNSUPPORTED */
OS_ALWAYS_INLINE static inline Os_StatusType   Os_XSigSend_SetEventAsync (
  const   Os_CoreAsrConfigType OS_CONST * RecvCore,
  const   Os_ThreadConfigType OS_CONST * SendThread,
  TaskType TaskID,
  EventMaskType Mask
)                                                                                                                      /* COV_OS_HALPLATFORMMULTICOREUNSUPPORTED */
{
  Os_XSigParamsType parameters;

  /* #10 Initialize the call parameters. */
  parameters.ParameterData[0].ParamTaskType = TaskID;                                                                   /* SBSW_OS_PWA_LOCAL */
  parameters.ParameterData[1].ParamEventMaskType = Mask;                                                                /* SBSW_OS_PWA_LOCAL */

  /* #20 Send the request. */
  return Os_XSigSendAsync(RecvCore, SendThread, XSigFunctionIdx_SetEvent, &parameters);                                 /* SBSW_OS_XSIG_XSIGSEND_001 */
}


/***********************************************************************************************************************
 *  Os_XSigSend_GetEvent()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 */
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */ /* COV_OS_HALPLATFORMMULTICOREUNSUPPORTED */
OS_ALWAYS_INLINE static inline Os_StatusType   Os_XSigSend_GetEvent (
  const   Os_CoreAsrConfigType OS_CONST * RecvCore,
  const   Os_ThreadConfigType OS_CONST * SendThread,
  TaskType TaskID,
  EventMaskRefType Mask
)                                                                                                                      /* COV_OS_HALPLATFORMMULTICOREUNSUPPORTED */
{
  Os_StatusType status;
  Os_XSigParamsType parameters;
  volatile Os_XSigOutputParamsType outParameters;

  /* #10 Initialize the call parameters. */
  parameters.ParameterData[0].ParamTaskType = TaskID;                                                                   /* SBSW_OS_PWA_LOCAL */

  /* #20 Send the request. */
  status = Os_XSigSendSync(RecvCore, SendThread, XSigFunctionIdx_GetEvent, &parameters, &outParameters);                /* SBSW_OS_XSIG_XSIGSEND_001 */

  (*Mask) = outParameters.ParameterData.ParamEventMaskType;                                                             /* SBSW_OS_PWA_PRECONDITION */

  return status;
}


/***********************************************************************************************************************
 *  Os_XSigSend_GetAlarm()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */ /* COV_OS_HALPLATFORMMULTICOREUNSUPPORTED */
OS_ALWAYS_INLINE static inline Os_StatusType   Os_XSigSend_GetAlarm (
  const   Os_CoreAsrConfigType OS_CONST * RecvCore,
  const   Os_ThreadConfigType OS_CONST * SendThread,
  AlarmType AlarmID,
  TickRefType Tick
)                                                                                                                      /* COV_OS_HALPLATFORMMULTICOREUNSUPPORTED */
{
  Os_StatusType status;
  Os_XSigParamsType parameters;
  volatile Os_XSigOutputParamsType outParameters;

  /* #10 Initialize the call parameters. */
  parameters.ParameterData[0].ParamAlarmType = AlarmID;                                                                 /* SBSW_OS_PWA_LOCAL */

  /* #20 Send the request synchronous. */
  status = Os_XSigSendSync(RecvCore, SendThread, XSigFunctionIdx_GetAlarm, &parameters, &outParameters);                /* SBSW_OS_XSIG_XSIGSEND_001 */

  /* #30 Get the result. */
  (*Tick) = outParameters.ParameterData.ParamTickType;                                                                  /* SBSW_OS_PWA_PRECONDITION */

  return status;
}


/***********************************************************************************************************************
 *  Os_XSigSend_SetRelAlarm()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 */
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */ /* COV_OS_HALPLATFORMMULTICOREUNSUPPORTED */
OS_ALWAYS_INLINE static inline Os_StatusType   Os_XSigSend_SetRelAlarm (
  const   Os_CoreAsrConfigType OS_CONST * RecvCore,
  const   Os_ThreadConfigType OS_CONST * SendThread,
  AlarmType AlarmID,
  TickType Increment,
  TickType Cycle
)                                                                                                                      /* COV_OS_HALPLATFORMMULTICOREUNSUPPORTED */
{
  Os_XSigParamsType parameters;

  /* #10 Initialize the call parameters. */
  parameters.ParameterData[0].ParamAlarmType = AlarmID;                                                                 /* SBSW_OS_PWA_LOCAL */
  parameters.ParameterData[1].ParamTickType = Increment;                                                                /* SBSW_OS_PWA_LOCAL */
  parameters.ParameterData[2].ParamTickType = Cycle;                                                                    /* SBSW_OS_PWA_LOCAL */

  /* #20 Send the request. */
  return Os_XSigSend(RecvCore, SendThread, XSigFunctionIdx_SetRelAlarm, &parameters);                                   /* SBSW_OS_XSIG_XSIGSEND_001 */
}


/***********************************************************************************************************************
 *  Os_XSigSend_SetAbsAlarm()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 */
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */ /* COV_OS_HALPLATFORMMULTICOREUNSUPPORTED */
OS_ALWAYS_INLINE static inline Os_StatusType   Os_XSigSend_SetAbsAlarm (
  const   Os_CoreAsrConfigType OS_CONST * RecvCore,
  const   Os_ThreadConfigType OS_CONST * SendThread,
  AlarmType AlarmID,
  TickType Start,
  TickType Cycle
)                                                                                                                      /* COV_OS_HALPLATFORMMULTICOREUNSUPPORTED */
{
  Os_XSigParamsType parameters;

  /* #10 Initialize the call parameters. */
  parameters.ParameterData[0].ParamAlarmType = AlarmID;                                                                 /* SBSW_OS_PWA_LOCAL */
  parameters.ParameterData[1].ParamTickType = Start;                                                                    /* SBSW_OS_PWA_LOCAL */
  parameters.ParameterData[2].ParamTickType = Cycle;                                                                    /* SBSW_OS_PWA_LOCAL */

  /* #20 Send the request. */
  return Os_XSigSend(RecvCore, SendThread, XSigFunctionIdx_SetAbsAlarm, &parameters);                                   /* SBSW_OS_XSIG_XSIGSEND_001 */
}


/***********************************************************************************************************************
 *  Os_XSigSend_CancelAlarm()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 */
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */ /* COV_OS_HALPLATFORMMULTICOREUNSUPPORTED */
OS_ALWAYS_INLINE static inline Os_StatusType   Os_XSigSend_CancelAlarm (
  const   Os_CoreAsrConfigType OS_CONST * RecvCore,
  const   Os_ThreadConfigType OS_CONST * SendThread,
  AlarmType AlarmID
)                                                                                                                      /* COV_OS_HALPLATFORMMULTICOREUNSUPPORTED */
{
  Os_XSigParamsType parameters;

  /* #10 Initialize the call parameters. */
  parameters.ParameterData[0].ParamAlarmType = AlarmID;                                                                 /* SBSW_OS_PWA_LOCAL */

  /* #20 Send the request. */
  return Os_XSigSend(RecvCore, SendThread, XSigFunctionIdx_CancelAlarm, &parameters);                                   /* SBSW_OS_XSIG_XSIGSEND_001 */
}


/***********************************************************************************************************************
 *  Os_XSigSend_StartScheduleTableRel()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 */
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */ /* COV_OS_HALPLATFORMMULTICOREUNSUPPORTED */
OS_ALWAYS_INLINE static inline Os_StatusType   Os_XSigSend_StartScheduleTableRel (
  const   Os_CoreAsrConfigType OS_CONST * RecvCore,
  const   Os_ThreadConfigType OS_CONST * SendThread,
  ScheduleTableType ScheduleTableID,
  TickType Offset
)                                                                                                                      /* COV_OS_HALPLATFORMMULTICOREUNSUPPORTED */
{
  Os_XSigParamsType parameters;

  /* #10 Initialize the call parameters. */
  parameters.ParameterData[0].ParamScheduleTableType = ScheduleTableID;                                                 /* SBSW_OS_PWA_LOCAL */
  parameters.ParameterData[1].ParamTickType = Offset;                                                                   /* SBSW_OS_PWA_LOCAL */

  /* #20 Send the request. */
  return Os_XSigSend(RecvCore, SendThread, XSigFunctionIdx_StartScheduleTableRel, &parameters);                         /* SBSW_OS_XSIG_XSIGSEND_001 */
}


/***********************************************************************************************************************
 *  Os_XSigSend_StartScheduleTableAbs()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 */
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */ /* COV_OS_HALPLATFORMMULTICOREUNSUPPORTED */
OS_ALWAYS_INLINE static inline Os_StatusType   Os_XSigSend_StartScheduleTableAbs (
  const   Os_CoreAsrConfigType OS_CONST * RecvCore,
  const   Os_ThreadConfigType OS_CONST * SendThread,
  ScheduleTableType ScheduleTableID,
  TickType Start
)                                                                                                                      /* COV_OS_HALPLATFORMMULTICOREUNSUPPORTED */
{
  Os_XSigParamsType parameters;

  /* #10 Initialize the call parameters. */
  parameters.ParameterData[0].ParamScheduleTableType = ScheduleTableID;                                                 /* SBSW_OS_PWA_LOCAL */
  parameters.ParameterData[1].ParamTickType = Start;                                                                    /* SBSW_OS_PWA_LOCAL */

  /* #20 Send the request. */
  return Os_XSigSend(RecvCore, SendThread, XSigFunctionIdx_StartScheduleTableAbs, &parameters);                         /* SBSW_OS_XSIG_XSIGSEND_001 */
}


/***********************************************************************************************************************
 *  Os_XSigSend_StopScheduleTable()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 */
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */ /* COV_OS_HALPLATFORMMULTICOREUNSUPPORTED */
OS_ALWAYS_INLINE static inline Os_StatusType   Os_XSigSend_StopScheduleTable (
  const   Os_CoreAsrConfigType OS_CONST * RecvCore,
  const   Os_ThreadConfigType OS_CONST * SendThread,
  ScheduleTableType ScheduleTableID
)                                                                                                                      /* COV_OS_HALPLATFORMMULTICOREUNSUPPORTED */
{
  Os_XSigParamsType parameters;

  /* #10 Initialize the call parameters. */
  parameters.ParameterData[0].ParamScheduleTableType = ScheduleTableID;                                                 /* SBSW_OS_PWA_LOCAL */

  /* #20 Send the request. */
  return Os_XSigSend(RecvCore, SendThread, XSigFunctionIdx_StopScheduleTable, &parameters);                             /* SBSW_OS_XSIG_XSIGSEND_001 */
}


/***********************************************************************************************************************
 *  Os_XSigSend_NextScheduleTable()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 */
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */ /* COV_OS_HALPLATFORMMULTICOREUNSUPPORTED */
OS_ALWAYS_INLINE static inline Os_StatusType   Os_XSigSend_NextScheduleTable (
  const   Os_CoreAsrConfigType OS_CONST * RecvCore,
  const   Os_ThreadConfigType OS_CONST * SendThread,
  ScheduleTableType ScheduleTableID_From,
  ScheduleTableType ScheduleTableID_To
)                                                                                                                      /* COV_OS_HALPLATFORMMULTICOREUNSUPPORTED */
{
  Os_XSigParamsType parameters;

  /* #10 Initialize the call parameters. */
  parameters.ParameterData[0].ParamScheduleTableType = ScheduleTableID_From;                                            /* SBSW_OS_PWA_LOCAL */
  parameters.ParameterData[1].ParamScheduleTableType = ScheduleTableID_To;                                              /* SBSW_OS_PWA_LOCAL */

  /* #20 Send the request. */
  return Os_XSigSend(RecvCore, SendThread, XSigFunctionIdx_NextScheduleTable, &parameters);                             /* SBSW_OS_XSIG_XSIGSEND_001 */
}


/***********************************************************************************************************************
 *  Os_XSigSend_GetScheduleTableStatus()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 */
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */ /* COV_OS_HALPLATFORMMULTICOREUNSUPPORTED */
OS_ALWAYS_INLINE static inline Os_StatusType   Os_XSigSend_GetScheduleTableStatus (
  const   Os_CoreAsrConfigType OS_CONST * RecvCore,
  const   Os_ThreadConfigType OS_CONST * SendThread,
  ScheduleTableType ScheduleTableID,
  ScheduleTableStatusRefType ScheduleStatus
)                                                                                                                      /* COV_OS_HALPLATFORMMULTICOREUNSUPPORTED */
{
  Os_StatusType status;
  Os_XSigParamsType parameters;
  volatile Os_XSigOutputParamsType outParameters;

  /* #10 Initialize the call parameters. */
  parameters.ParameterData[0].ParamScheduleTableType = ScheduleTableID;                                                 /* SBSW_OS_PWA_LOCAL */

  /* #20 Send the request. */
  status = Os_XSigSendSync(RecvCore, SendThread, XSigFunctionIdx_GetScheduleTableStatus, &parameters, &outParameters);  /* SBSW_OS_XSIG_XSIGSEND_001 */
  (*ScheduleStatus) = outParameters.ParameterData.ParamScheduleTableStatusType;                                         /* SBSW_OS_PWA_PRECONDITION */

  return status;
}


/***********************************************************************************************************************
 *  Os_XSigSend_GetTaskStackUsage()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 */
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */ /* COV_OS_HALPLATFORMMULTICOREUNSUPPORTED */
OS_ALWAYS_INLINE static inline Os_StatusType   Os_XSigSend_GetTaskStackUsage (
  const   Os_CoreAsrConfigType OS_CONST * RecvCore,
  const   Os_ThreadConfigType OS_CONST * SendThread,
  TaskType TaskID,
    uint32 OS_CONST * Result
)                                                                                                                      /* COV_OS_HALPLATFORMMULTICOREUNSUPPORTED */
{
  Os_StatusType status;
  Os_XSigParamsType parameters;
  volatile Os_XSigOutputParamsType outParameters;

  /* #10 Initialize the call parameters. */
  parameters.ParameterData[0].ParamTaskType = TaskID;                                                                   /* SBSW_OS_PWA_LOCAL */

  /* #20 Send the request. */
  status = Os_XSigSendSync(RecvCore, SendThread, XSigFunctionIdx_GetTaskStackUsage, &parameters, &outParameters);       /* SBSW_OS_XSIG_XSIGSEND_001 */
  (*Result) = outParameters.ParameterData.Paramuint32;                                                                  /* SBSW_OS_PWA_PRECONDITION */

  return status;
}


/***********************************************************************************************************************
 *  Os_XSigSend_GetISRStackUsage()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 */
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */ /* COV_OS_HALPLATFORMMULTICOREUNSUPPORTED */
OS_ALWAYS_INLINE static inline Os_StatusType   Os_XSigSend_GetISRStackUsage (
  const   Os_CoreAsrConfigType OS_CONST * RecvCore,
  const   Os_ThreadConfigType OS_CONST * SendThread,
  ISRType IsrID,
    uint32 OS_CONST * Result
)                                                                                                                      /* COV_OS_HALPLATFORMMULTICOREUNSUPPORTED */
{
  Os_StatusType status;
  Os_XSigParamsType parameters;
  volatile Os_XSigOutputParamsType outParameters;

  /* #10 Initialize the call parameters. */
  parameters.ParameterData[0].ParamISRType = IsrID;                                                                     /* SBSW_OS_PWA_LOCAL */

  /* #20 Send the request. */
  status = Os_XSigSendSync(RecvCore, SendThread, XSigFunctionIdx_GetISRStackUsage, &parameters, &outParameters);        /* SBSW_OS_XSIG_XSIGSEND_001 */
  (*Result) = outParameters.ParameterData.Paramuint32;                                                                  /* SBSW_OS_PWA_PRECONDITION */

  return status;
}


/***********************************************************************************************************************
 *  Os_XSigSend_ControlIdle()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 */
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */ /* COV_OS_HALPLATFORMMULTICOREUNSUPPORTED */
OS_ALWAYS_INLINE static inline Os_StatusType   Os_XSigSend_ControlIdle (
  const   Os_CoreAsrConfigType OS_CONST * RecvCore,
  const   Os_ThreadConfigType OS_CONST * SendThread,
  CoreIdType CoreID,
  IdleModeType IdleMode
)                                                                                                                      /* COV_OS_HALPLATFORMMULTICOREUNSUPPORTED */
{
  Os_StatusType status;
  Os_XSigParamsType parameters;

  /* #10 Initialize the call parameters. */
  parameters.ParameterData[0].ParamCoreIdType = CoreID;                                                                 /* SBSW_OS_PWA_LOCAL */
  parameters.ParameterData[1].ParamIdleModeType = IdleMode;                                                             /* SBSW_OS_PWA_LOCAL */

  /* #20 Send the request. */
  status = Os_XSigSend(RecvCore, SendThread, XSigFunctionIdx_ControlIdle, &parameters);                                 /* SBSW_OS_XSIG_XSIGSEND_001 */

  return status;
}


/***********************************************************************************************************************
 *  Os_XSigSend_GetKernelStackUsage()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 */
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */ /* COV_OS_HALPLATFORMMULTICOREUNSUPPORTED */
OS_ALWAYS_INLINE static inline Os_StatusType   Os_XSigSend_GetKernelStackUsage (
  const   Os_CoreAsrConfigType OS_CONST * RecvCore,
  const   Os_ThreadConfigType OS_CONST * SendThread,
  CoreIdType CoreID,
    uint32 OS_CONST * Result
)                                                                                                                      /* COV_OS_HALPLATFORMMULTICOREUNSUPPORTED */
{
  Os_StatusType status;
  Os_XSigParamsType parameters;
  volatile Os_XSigOutputParamsType outParameters;

  /* #10 Initialize the call parameters. */
  parameters.ParameterData[0].ParamCoreIdType = CoreID;                                                                 /* SBSW_OS_PWA_LOCAL */

  /* #20 Send the request. */
  status = Os_XSigSendSync(RecvCore, SendThread, XSigFunctionIdx_GetKernelStackUsage, &parameters, &outParameters);     /* SBSW_OS_XSIG_XSIGSEND_001 */
  (*Result) = outParameters.ParameterData.Paramuint32;                                                                  /* SBSW_OS_PWA_PRECONDITION */

  return status;
}


/***********************************************************************************************************************
 *  Os_XSigSend_GetStartupHookStackUsage()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 */
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */ /* COV_OS_HALPLATFORMMULTICOREUNSUPPORTED */
OS_ALWAYS_INLINE static inline Os_StatusType   Os_XSigSend_GetStartupHookStackUsage (
  const   Os_CoreAsrConfigType OS_CONST * RecvCore,
  const   Os_ThreadConfigType OS_CONST * SendThread,
  CoreIdType CoreID,
    uint32 OS_CONST * Result
)                                                                                                                      /* COV_OS_HALPLATFORMMULTICOREUNSUPPORTED */
{
  Os_StatusType status;
  Os_XSigParamsType parameters;
  volatile Os_XSigOutputParamsType outParameters;

  /* #10 Initialize the call parameters. */
  parameters.ParameterData[0].ParamCoreIdType = CoreID;                                                                 /* SBSW_OS_PWA_LOCAL */

  /* #20 Send the request. */
  status = Os_XSigSendSync(RecvCore, SendThread, XSigFunctionIdx_GetStartupHookStackUsage, &parameters, &outParameters);/* SBSW_OS_XSIG_XSIGSEND_001 */
  (*Result) = outParameters.ParameterData.Paramuint32;                                                                  /* SBSW_OS_PWA_PRECONDITION */

  return status;
}


/***********************************************************************************************************************
 *  Os_XSigSend_GetErrorHookStackUsage()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 */
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */ /* COV_OS_HALPLATFORMMULTICOREUNSUPPORTED */
OS_ALWAYS_INLINE static inline Os_StatusType   Os_XSigSend_GetErrorHookStackUsage (
  const   Os_CoreAsrConfigType OS_CONST * RecvCore,
  const   Os_ThreadConfigType OS_CONST * SendThread,
  CoreIdType CoreID,
    uint32 OS_CONST * Result
)                                                                                                                      /* COV_OS_HALPLATFORMMULTICOREUNSUPPORTED */
{
  Os_StatusType status;
  Os_XSigParamsType parameters;
  volatile Os_XSigOutputParamsType outParameters;

  /* #10 Initialize the call parameters. */
  parameters.ParameterData[0].ParamCoreIdType = CoreID;                                                                 /* SBSW_OS_PWA_LOCAL */

  /* #20 Send the request. */
  status = Os_XSigSendSync(RecvCore, SendThread, XSigFunctionIdx_GetErrorHookStackUsage, &parameters, &outParameters);  /* SBSW_OS_XSIG_XSIGSEND_001 */
  (*Result) = outParameters.ParameterData.Paramuint32;                                                                  /* SBSW_OS_PWA_PRECONDITION */

  return status;
}


/***********************************************************************************************************************
 *  Os_XSigSend_GetShutdownHookStackUsage()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 */
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */ /* COV_OS_HALPLATFORMMULTICOREUNSUPPORTED */
OS_ALWAYS_INLINE static inline Os_StatusType   Os_XSigSend_GetShutdownHookStackUsage (
  const   Os_CoreAsrConfigType OS_CONST * RecvCore,
  const   Os_ThreadConfigType OS_CONST * SendThread,
  CoreIdType CoreID,
    uint32 OS_CONST * Result
)                                                                                                                      /* COV_OS_HALPLATFORMMULTICOREUNSUPPORTED */
{
  Os_StatusType status;
  Os_XSigParamsType parameters;
  volatile Os_XSigOutputParamsType outParameters;

  /* #10 Initialize the call parameters. */
  parameters.ParameterData[0].ParamCoreIdType = CoreID;                                                                 /* SBSW_OS_PWA_LOCAL */

  /* #20 Send the request. */
  status = Os_XSigSendSync(                                                                                             /* SBSW_OS_XSIG_XSIGSEND_001 */
                RecvCore, SendThread, XSigFunctionIdx_GetShutdownHookStackUsage, &parameters, &outParameters);

  (*Result) = outParameters.ParameterData.Paramuint32;                                                                  /* SBSW_OS_PWA_PRECONDITION */

  return status;
}


/***********************************************************************************************************************
 *  Os_XSigSend_GetProtectionHookStackUsage()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 */
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */ /* COV_OS_HALPLATFORMMULTICOREUNSUPPORTED */
OS_ALWAYS_INLINE static inline Os_StatusType   Os_XSigSend_GetProtectionHookStackUsage (
  const   Os_CoreAsrConfigType OS_CONST * RecvCore,
  const   Os_ThreadConfigType OS_CONST * SendThread,
  CoreIdType CoreID,
    uint32 OS_CONST * Result
)                                                                                                                      /* COV_OS_HALPLATFORMMULTICOREUNSUPPORTED */
{
  Os_StatusType status;
  Os_XSigParamsType parameters;
  volatile Os_XSigOutputParamsType outParameters;

  /* #10 Initialize the call parameters. */
  parameters.ParameterData[0].ParamCoreIdType = CoreID;                                                                 /* SBSW_OS_PWA_LOCAL */

  /* #20 Send the request. */
  status = Os_XSigSendSync( RecvCore, SendThread, XSigFunctionIdx_GetProtectionHookStackUsage, &parameters,             /* SBSW_OS_XSIG_XSIGSEND_001 */
                            &outParameters);

  (*Result) = outParameters.ParameterData.Paramuint32;                                                                  /* SBSW_OS_PWA_PRECONDITION */

  return status;
}


/***********************************************************************************************************************
 *  Os_XSigSend_GetNonTrustedFunctionStackUsage()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 */
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */ /* COV_OS_HALPLATFORMMULTICOREUNSUPPORTED */
OS_ALWAYS_INLINE static inline Os_StatusType   Os_XSigSend_GetNonTrustedFunctionStackUsage (
  const   Os_CoreAsrConfigType OS_CONST * RecvCore,
  const   Os_ThreadConfigType OS_CONST * SendThread,
  Os_NonTrustedFunctionIndexType FunctionIndex,
    uint32 OS_CONST * Result
)                                                                                                                      /* COV_OS_HALPLATFORMMULTICOREUNSUPPORTED */
{
  Os_StatusType status;
  Os_XSigParamsType parameters;
  volatile Os_XSigOutputParamsType outParameters;

  /* #10 Initialize the call parameters. */
  parameters.ParameterData[0].ParamOs_NonTrustedFunctionIndexType = FunctionIndex;                                      /* SBSW_OS_PWA_LOCAL */

  /* #20 Send the request. */
  status = Os_XSigSendSync( RecvCore, SendThread, XSigFunctionIdx_GetNonTrustedFunctionStackUsage, &parameters,         /* SBSW_OS_XSIG_XSIGSEND_001 */
                            &outParameters);
  (*Result) = outParameters.ParameterData.Paramuint32;                                                                  /* SBSW_OS_PWA_PRECONDITION */

  return status;
}


/***********************************************************************************************************************
 *  Os_XSigSend_TerminateApplication()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 */
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */ /* COV_OS_HALPLATFORMMULTICOREUNSUPPORTED */
OS_ALWAYS_INLINE static inline Os_StatusType   Os_XSigSend_TerminateApplication (
  const   Os_CoreAsrConfigType OS_CONST * RecvCore,
  const   Os_ThreadConfigType OS_CONST * SendThread,
  ApplicationType Application,
  RestartType RestartOption,
  ApplicationType CurrentCallerApplicationId
)                                                                                                                      /* COV_OS_HALPLATFORMMULTICOREUNSUPPORTED */
{
  Os_StatusType status;
  Os_XSigParamsType parameters;
  volatile Os_XSigOutputParamsType outParameters;

  /* #10 Initialize the call parameters. */
  parameters.ParameterData[0].ParamApplicationType = Application;                                                       /* SBSW_OS_PWA_LOCAL */
  parameters.ParameterData[1].ParamRestartType = RestartOption;                                                         /* SBSW_OS_PWA_LOCAL */
  parameters.ParameterData[2].ParamApplicationType = CurrentCallerApplicationId;                                        /* SBSW_OS_PWA_LOCAL */

  /* #20 Send the request. */
  status = Os_XSigSendSync(RecvCore, SendThread, XSigFunctionIdx_TerminateApplication, &parameters, &outParameters);    /* SBSW_OS_XSIG_XSIGSEND_001 */

  return status;
}


/***********************************************************************************************************************
 *  Os_XSigSend_GetApplicationState()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 */
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */ /* COV_OS_HALPLATFORMMULTICOREUNSUPPORTED */
OS_ALWAYS_INLINE static inline Os_StatusType   Os_XSigSend_GetApplicationState (
  const   Os_CoreAsrConfigType OS_CONST * RecvCore,
  const   Os_ThreadConfigType OS_CONST * SendThread,
  ApplicationType Application,
  ApplicationStateRefType Value
)                                                                                                                      /* COV_OS_HALPLATFORMMULTICOREUNSUPPORTED */
{
  Os_StatusType status;
  Os_XSigParamsType parameters;
  volatile Os_XSigOutputParamsType outParameters;

  /* #10 Initialize the call parameters. */
  parameters.ParameterData[0].ParamApplicationType = Application;                                                       /* SBSW_OS_PWA_LOCAL */

  /* #20 Send the request. */
  status = Os_XSigSendSync(RecvCore, SendThread, XSigFunctionIdx_GetApplicationState, &parameters, &outParameters);     /* SBSW_OS_XSIG_XSIGSEND_001 */

  (*Value) = outParameters.ParameterData.ParamApplicationStateType;                                                     /* SBSW_OS_PWA_PRECONDITION */

  return status;
}


/***********************************************************************************************************************
 *  Os_XSigSend_ShutdownAllCores()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 */
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */ /* COV_OS_HALPLATFORMMULTICOREUNSUPPORTED */
OS_ALWAYS_INLINE static inline Os_StatusType   Os_XSigSend_ShutdownAllCores (
  const   Os_CoreAsrConfigType OS_CONST * RecvCore,
  const   Os_ThreadConfigType OS_CONST * SendThread,
  StatusType Error
)                                                                                                                      /* COV_OS_HALPLATFORMMULTICOREUNSUPPORTED */
{
  Os_StatusType status;
  Os_XSigParamsType parameters;

  /* #10 Initialize the call parameters. */
  parameters.ParameterData[0].ParamStatusType = Error;                                                                  /* SBSW_OS_PWA_LOCAL */

  /* #20 Send the request. */
  status = Os_XSigSendAsync(RecvCore, SendThread, XSigFunctionIdx_ShutdownAllCores, &parameters);                       /* SBSW_OS_XSIG_XSIGSEND_001 */

  return status;
}


/***********************************************************************************************************************
 *  Os_XSigSend_HookCallCallbackAsync()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline Os_StatusType   /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */ /* COV_OS_HALPLATFORMMULTICOREUNSUPPORTED */
Os_XSigSend_HookCallCallbackAsync (
  const   Os_CoreAsrConfigType OS_CONST * RecvCore,
  const   Os_ThreadConfigType OS_CONST * SendThread,
  Os_HookIdType Id
)                                                                                                                      /* COV_OS_HALPLATFORMMULTICOREUNSUPPORTED */
{
  Os_XSigParamsType parameters;

  /* #10 Initialize the call parameters. */
  parameters.ParameterData[0].ParamHookIdType = Id;                                                                     /* SBSW_OS_PWA_LOCAL */

  /* #20 Send the request. */
  return Os_XSigSendAsync(RecvCore, SendThread, XSigFunctionIdx_HookCallCallback, &parameters);                         /* SBSW_OS_XSIG_XSIGSEND_001 */
}


# define OS_STOP_SEC_CODE
# include "Os_MemMap_OsCode.h"                                                                                          /* PRQA S 5087 */ /* MD_MSR_MemMap */


#endif /* OS_XSIGNAL_H */

/* module specific MISRA deviations:
 */

/*!
 * \}
 */
/***********************************************************************************************************************
 *  END OF FILE: Os_XSignal.h
 **********************************************************************************************************************/
