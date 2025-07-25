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
 * \addtogroup Os_PriorityQueue
 *
 * \{
 * \file
 * \brief   Provides the API declaration.
 * \details --no details--
 *
 **********************************************************************************************************************/
/***********************************************************************************************************************
 *  REVISION HISTORY
 *  --------------------------------------------------------------------------------------------------------------------
 *  Refer to Os.h.
 **********************************************************************************************************************/


#ifndef OS_PRIORITYQUEUE_H
# define OS_PRIORITYQUEUE_H
                                                                                                                         /* PRQA S 0388 EOF */ /* MD_MSR_Dir1.1 */
/***********************************************************************************************************************
 *  INCLUDES
 **********************************************************************************************************************/
/* AUTOSAR includes */
# include "Std_Types.h"

/* Os module declarations */
# include "Os_PriorityQueueInt.h"

/* Os kernel module dependencies */
# include "Os_Counter.h"
# include "Os_Job.h"

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
 *  Os_PriorityQueueTopGet()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE                                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
static inline const   Os_JobConfigType     *   Os_PriorityQueueTopGet (
  const   Os_PriorityQueueConfigType OS_CONST * PriorityQueue
)
{
  const   Os_JobConfigType OS_APPL_DATA * result;

  /* #10   If the queue is empty:*/
  if(OS_UNLIKELY(PriorityQueue->Dyn->QueueUsage == 0u))
  {
    /* #20   Set the result to ((void *)0). */
    result = ((void *)0);
  }
  /* #30   Otherwise there is at least one element available. */
  else
  {
    /* #40   Set result to the root data pointer. */
    result = PriorityQueue->Queue[OS_PRIORITYQUEUE_ROOT_IDX].Data;
  }
  return result;
}



# define OS_STOP_SEC_CODE
# include "Os_MemMap_OsCode.h"                                                                                          /* PRQA S 5087 */ /* MD_MSR_MemMap */


#endif /* OS_PRIORITYQUEUEINT_H */

/* module specific MISRA deviations:
 *
 */

/*!
 * \}
 */

/***********************************************************************************************************************
 *  END OF FILE: Os_PriorityQueue.h
 **********************************************************************************************************************/
