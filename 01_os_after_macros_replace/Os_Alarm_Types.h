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
 * \addtogroup Os_Alarm
 * \{
 *
 * \file
 * \brief       Type definitions for alarm management.
 * \details     --no details--
 *
 *
 **********************************************************************************************************************/
/***********************************************************************************************************************
 *  REVISION HISTORY
 *  --------------------------------------------------------------------------------------------------------------------
 *  Refer to Os.h.
 **********************************************************************************************************************/

#ifndef OS_ALARM_TYPES_H
# define OS_ALARM_TYPES_H
                                                                                                                         /* PRQA S 0388 EOF */ /* MD_MSR_Dir1.1 */
/***********************************************************************************************************************
 *  INCLUDES
 **********************************************************************************************************************/
/* AUTOSAR includes */
# include "Std_Types.h"


/***********************************************************************************************************************
 *  GLOBAL CONSTANT MACROS
 **********************************************************************************************************************/


/***********************************************************************************************************************
 *  GLOBAL FUNCTION MACROS
 **********************************************************************************************************************/


/***********************************************************************************************************************
 *  GLOBAL DATA TYPES AND STRUCTURES
 **********************************************************************************************************************/
struct Os_AlarmConfigType_Tag;
typedef struct Os_AlarmConfigType_Tag Os_AlarmConfigType;


struct Os_AlarmSetEventConfigType_Tag;
typedef struct Os_AlarmSetEventConfigType_Tag Os_AlarmSetEventConfigType;


struct Os_AlarmActivateTaskConfigType_Tag;
typedef struct Os_AlarmActivateTaskConfigType_Tag Os_AlarmActivateTaskConfigType;


struct Os_AlarmIncrementCounterConfigType_Tag;
typedef struct Os_AlarmIncrementCounterConfigType_Tag Os_AlarmIncrementCounterConfigType;


struct Os_AlarmCallbackConfigType_Tag;
typedef struct Os_AlarmCallbackConfigType_Tag Os_AlarmCallbackConfigType;



/***********************************************************************************************************************
 *  GLOBAL DATA PROTOTYPES
 **********************************************************************************************************************/


/***********************************************************************************************************************
 *  GLOBAL FUNCTION PROTOTYPES
 **********************************************************************************************************************/


/***********************************************************************************************************************
 *  GLOBAL FUNCTIONS
 **********************************************************************************************************************/





#endif /* OS_ALARM_TYPES_H */

/* module specific MISRA deviations:
 */

/*!
 * \}
 */
/***********************************************************************************************************************
 *  END OF FILE: Os_Alarm_Types.h
 **********************************************************************************************************************/
