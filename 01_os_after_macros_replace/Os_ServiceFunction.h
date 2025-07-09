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
 * \addtogroup Os_ServiceFunction
 * \{
 *
 * \file
 * \brief       Service Function API.
 * \details     --no details--
 *
 *
 **********************************************************************************************************************/
/***********************************************************************************************************************
 *  REVISION HISTORY
 *  --------------------------------------------------------------------------------------------------------------------
 *  Refer to Os.h.
 **********************************************************************************************************************/

#ifndef OS_SERVICEFUNCTION_H
# define OS_SERVICEFUNCTION_H
                                                                                                                         /* PRQA S 0388 EOF */ /* MD_MSR_Dir1.1 */
/***********************************************************************************************************************
 *  INCLUDES
 **********************************************************************************************************************/
/* AUTOSAR includes */
# include "Std_Types.h"

/* Os module declarations */
# include "Os_ServiceFunctionInt.h"
# include "Os_ServiceFunction_Lcfg.h"

/* Os kernel module dependencies */
# include "Os_Lcfg.h"
# include "Os_BitArray.h"
# include "Os_Application.h"
# include "Os_Error.h"
# include "Os_MemoryProtection.h"
# include "Os_Stack.h"

/* Os hal dependencies */
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
 *  Os_ServiceNtfIsEnabled()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE                                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
static inline Os_StdReturnType   Os_ServiceNtfIsEnabled (
  void
)
{
   return (Os_StdReturnType)((uint8)OS_NONTRUSTEDFUNCTIONID_COUNT > 0u);                                                /* PRQA S 4304, 2995 */ /* MD_Os_C90BooleanCompatibility, MD_Os_Rule2.2_2995*/
}


/***********************************************************************************************************************
 *  Os_ServiceTfIsEnabled()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE                                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
static inline Os_StdReturnType   Os_ServiceTfIsEnabled (
  void
)
{
   return (Os_StdReturnType)((uint8)OS_TRUSTEDFUNCTIONID_COUNT > 0u);                                                   /* PRQA S 4304, 2995 */ /* MD_Os_C90BooleanCompatibility, MD_Os_Rule2.2_2995*/
}


/***********************************************************************************************************************
 *  Os_ServiceFtfIsEnabled()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE                                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
static inline Os_StdReturnType   Os_ServiceFtfIsEnabled (
  void
)
{
   return (Os_StdReturnType)((uint8)OS_FASTTRUSTEDFUNCTIONID_COUNT > 0u);                                               /* PRQA S 4304, 2995, 2996 */ /* MD_Os_C90BooleanCompatibility, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
}


/***********************************************************************************************************************
 *  Os_ServiceCheckNtfId()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE                                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
static inline Os_StdReturnType   Os_ServiceCheckNtfId (
  Os_NonTrustedFunctionIndexType Id
)
{
  return Os_ErrIsValueLo((uint32)Id, (uint32)OS_NONTRUSTEDFUNCTIONID_COUNT);
}


/***********************************************************************************************************************
 *  Os_ServiceNtfId2Ntf()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE                                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
static inline const   Os_ServiceNtfConfigType OS_CONST *   Os_ServiceNtfId2Ntf (
  Os_NonTrustedFunctionIndexType Id
)
{
  Os_Assert(Os_ErrIsValueLo((uint32)Id, (uint32)OS_NONTRUSTEDFUNCTIONID_COUNT));
  return OsCfg_NonTrustedFunctionRefs[Id];
}


/***********************************************************************************************************************
 *  Os_ServiceCallsInit()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 */
OS_ALWAYS_INLINE                                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
static inline void   Os_ServiceCallsInit (
    Os_ServiceCallListType OS_VAR_NOINIT * ServiceCalls
)
{
  /* #10 Write list end marker to start of list. */
  (*ServiceCalls) = OS_SERVICE_LIST_END;                                                                                /* SBSW_OS_PWA_PRECONDITION */
}


/***********************************************************************************************************************
*  Os_ServiceInit()
***********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 */
OS_ALWAYS_INLINE                                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
static inline void   Os_ServiceInit (
  const   Os_ServiceConfigType OS_CONST * Service
)
{
  Os_StdReturnType tfEnabled = Os_ServiceTfIsEnabled();
  Os_StdReturnType ntfEnabled = Os_ServiceNtfIsEnabled();

  /* #10 If Service is enabled: */
  /* A logical OR instead of a bitwise OR would be more meaningful here.
   * But some compilers would complain about known decision outcome.
   * Moving the calls directly into the decision would solve this, but MISRA will complain about possible sideeffects
   * of the second call. The bitwise OR leads to correct functionality and overcomes both issues. */
  if((ntfEnabled | tfEnabled) != 0u)                                                                                    /* PRQA S 2991, 2995 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule2.2_2995 */
  {
    void     Os_ServiceInitInternal(const   Os_ServiceConfigType OS_CONST * Service);             /* PRQA S 3449, 3451 */ /* MD_Os_Rule8.5_3449, MD_Os_Rule8.5_3451 */

    /* #20 Redirect to the internal initialization. */
    Os_ServiceInitInternal(Service);                                                                                    /* SBSW_OS_FC_PRECONDITION */
  }
}



# define OS_STOP_SEC_CODE
# include "Os_MemMap_OsCode.h"                                                                                          /* PRQA S 5087 */ /* MD_MSR_MemMap */

#endif /* OS_SERVICEFUNCTION_H */

/* module specific MISRA deviations:
 */

/*!
 * \}
 */
/***********************************************************************************************************************
 *  END OF FILE: Os_ServiceFunction.h
 **********************************************************************************************************************/
