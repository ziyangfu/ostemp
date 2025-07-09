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
 * \addtogroup Os_Scheduler
 *
 * \{
 * \file
 * \brief       Interface implementation of \ref Os_Scheduler.
 * \details     --no details--
 *
 **********************************************************************************************************************/
/***********************************************************************************************************************
 *  REVISION HISTORY
 *  --------------------------------------------------------------------------------------------------------------------
 *  Refer to Os.h.
 **********************************************************************************************************************/


#ifndef OS_SCHEDULER_H
# define OS_SCHEDULER_H
                                                                                                                         /* PRQA S 0388 EOF */ /* MD_MSR_Dir1.1 */
/***********************************************************************************************************************
 *  INCLUDES
 **********************************************************************************************************************/
/* AUTOSAR includes */
# include "Std_Types.h"

/* Os module declarations */
# include "Os_Scheduler_Types.h"
# include "Os_SchedulerInt.h"
# include "Os_Scheduler_Lcfg.h"

/* Os module dependencies */
# include "Os_Task.h"
# include "Os_BitArray.h"
# include "Os_Deque.h"
# include "Os_Common.h"
# include "Os_PriorityQueue.h"

/* Os HAL dependencies */
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
 *  Os_SchedulerIsRoundRobinEnabled()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StdReturnType   Os_SchedulerIsRoundRobinEnabled ( void )
{
  return (Os_StdReturnType)(OS_CFG_SCHEDULE_ROUND_ROB_ENABLED == STD_ON);                                               /* PRQA S 4304, 2995 */ /* MD_Os_C90BooleanCompatibility, MD_Os_Rule2.2_2995 */
}


/***********************************************************************************************************************
 *  Os_SchedulerPriorityIsHigher()
 **********************************************************************************************************************/
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
OS_ALWAYS_INLINE static inline Os_StdReturnType   Os_SchedulerPriorityIsHigher (
  Os_TaskPrioType PrioX,
  Os_TaskPrioType PrioY
)
{
   return ((PrioX) < (PrioY));                                                                                          /* PRQA S 4404 */ /*  MD_Os_C90BooleanCompatibility */
}


/***********************************************************************************************************************
 *  Os_SchedulerPriorityIsHigherOrEqual()
 **********************************************************************************************************************/
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
OS_ALWAYS_INLINE static inline Os_StdReturnType   Os_SchedulerPriorityIsHigherOrEqual (
  Os_TaskPrioType PrioX,
  Os_TaskPrioType PrioY
)
{
  return (Os_StdReturnType)((PrioX) <= (PrioY));                                                                        /* PRQA S 4304 */ /* MD_Os_C90BooleanCompatibility */
}


/***********************************************************************************************************************
 *  Os_SchedulerPriorityIsLower()
 **********************************************************************************************************************/
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */ /* COV_OS_HALPLATFORMTESTEDASMULTICORE */
OS_ALWAYS_INLINE static inline Os_StdReturnType   Os_SchedulerPriorityIsLower (
  Os_TaskPrioType PrioX,
  Os_TaskPrioType PrioY
)                                                                                                                      /* COV_OS_HALPLATFORMTESTEDASMULTICORE */
{
  return (!Os_SchedulerPriorityIsHigherOrEqual(PrioX, PrioY));                                                          /* PRQA S 4116, 4404, 4558 */ /* MD_Os_Rule10.1_4116, MD_Os_C90BooleanCompatibility, MD_Os_Rule10.1_4558 */
}


/***********************************************************************************************************************
 *  Os_SchedulerGetCurrentTask()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
const   Os_TaskConfigType OS_CONST *   Os_SchedulerGetCurrentTask (
  const   Os_SchedulerConfigType OS_CONST * Scheduler
)
{
  return Scheduler->Dyn->CurrentTask;
}


/***********************************************************************************************************************
 *  Os_SchedulerTaskSwitchIsNeeded()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StdReturnType /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
    Os_SchedulerTaskSwitchIsNeeded (
  const   Os_SchedulerConfigType OS_CONST * Scheduler
)
{
  return (Os_StdReturnType)(Scheduler->Dyn->CurrentTask != Scheduler->Dyn->NextTask);                                   /* PRQA S 4304 */ /* MD_Os_C90BooleanCompatibility */
}


# define OS_STOP_SEC_CODE
# include "Os_MemMap_OsCode.h"                                                                                          /* PRQA S 5087 */ /* MD_MSR_MemMap */


#endif /* OS_SCHEDULERINT_H */

/* module specific MISRA deviations:
 */

/*!
 * \}
 */

/***********************************************************************************************************************
 *  END OF FILE: Os_SchedulerInt.h
 **********************************************************************************************************************/
