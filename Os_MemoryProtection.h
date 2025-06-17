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
 * \addtogroup Os_MemoryProtection
 *
 * \{
 *
 * \file
 * \brief       Internal functions implementation.
 * \details     --no details--
 *
 *
 **********************************************************************************************************************/
/***********************************************************************************************************************
 *  REVISION HISTORY
 *  --------------------------------------------------------------------------------------------------------------------
 *  Refer to Os.h.
 **********************************************************************************************************************/

#ifndef OS_MEMORYPROTECTION_H
# define OS_MEMORYPROTECTION_H
                                                                                                                         /* PRQA S 0388 EOF */ /* MD_MSR_Dir1.1 */
/***********************************************************************************************************************
 *  INCLUDES
 **********************************************************************************************************************/
/* AUTOSAR includes */
# include "Std_Types.h"

/* Os module declarations */
# include "Os_MemoryProtection_Types.h"
# include "Os_MemoryProtectionInt.h"
# include "Os_MemoryProtection_Cfg.h"
# include "Os_MemoryProtection_Lcfg.h"

/* Os kernel module dependencies */
# include "Os_Error.h"

/* Os Hal dependencies */
# include "Os_Hal_Compiler.h"
# include "Os_Hal_MemoryProtection.h"
# include "Os_Hal_Core.h"
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
 *  Os_MpSystemInit()
 **********************************************************************************************************************/
OS_FUNC_ATTRIBUTE_DEFINITION(OS_LOCAL_INLINE void, OS_CODE, OS_ALWAYS_INLINE,                                           /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
Os_MpSystemInit,
(
  P2CONST(Os_MpSystemConfigType, AUTOMATIC, OS_CONST) SystemConfig
))
{
  if(Os_MpIsEnabled() != 0u)                                                                                            /* PRQA S 2991, 2995 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule2.2_2995 */
  {
    Os_Hal_MpSystemInit(SystemConfig->HwConfig);                                                                        /* SBSW_OS_MP_Hal_MPSYSTEMINIT_001 */
  }
}


/***********************************************************************************************************************
 *  Os_MpCoreInit()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 */
OS_FUNC_ATTRIBUTE_DEFINITION(OS_LOCAL_INLINE void, OS_CODE, OS_ALWAYS_INLINE,                                           /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
Os_MpCoreInit,
(
  P2CONST(Os_MpCoreConfigType, AUTOMATIC, OS_CONST) CoreConfig,
  P2CONST(Os_Hal_ContextStackConfigType, AUTOMATIC, OS_VAR_NOINIT) InitialStackRegion
))
{
  if(Os_MpIsEnabled() != 0u)                                                                                            /* PRQA S 2991, 2995 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule2.2_2995 */
  {
    /* #10 Initialize the core memory protection and the initial MPU stack window. */
    Os_Hal_MpCoreInit(CoreConfig->HwConfig, InitialStackRegion);                                                        /* SBSW_OS_MP_Hal_MPCOREINIT_001 */
  }
}


/***********************************************************************************************************************
 *  Os_MpThreadInit()
 **********************************************************************************************************************/
OS_FUNC_ATTRIBUTE_DEFINITION(                                                                                           /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
OS_LOCAL_INLINE void, OS_CODE, OS_ALWAYS_INLINE, Os_MpThreadInit,
(
  P2CONST(Os_MpAccessRightsType, AUTOMATIC, OS_CONST) Config,
  P2VAR(Os_MpAccessRightsRefType, AUTOMATIC, OS_VAR_NOINIT_FAST) Settings
))
{
  if(Os_MpIsEnabled() != 0u)                                                                                            /* PRQA S 2991, 2995 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule2.2_2995 */
  {
    *Settings = Config;                                                                                                 /* SBSW_OS_PWA_PRECONDITION */
  }
}


/***********************************************************************************************************************
 *  Os_MpIsEnabled()
 **********************************************************************************************************************/
OS_FUNC_ATTRIBUTE_DEFINITION(OS_LOCAL_INLINE Os_StdReturnType, OS_CODE,                                                 /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
OS_ALWAYS_INLINE, Os_MpIsEnabled,
(
  void
))
{
  return (Os_StdReturnType)(OS_CFG_MEMORY_PROTECTION == STD_ON);                                                        /* PRQA S 4304, 2995 */ /* MD_Os_C90BooleanCompatibility, MD_Os_Rule2.2_2995 */
}


/***********************************************************************************************************************
 *  Os_MpIsEnabledForApplications()
 **********************************************************************************************************************/
OS_FUNC_ATTRIBUTE_DEFINITION(OS_LOCAL_INLINE Os_StdReturnType, OS_CODE,                                                 /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
OS_ALWAYS_INLINE, Os_MpIsEnabledForApplications,
(
  void
))
{
  return (Os_StdReturnType)(OS_CFG_MEMORY_PROTECTION_FOR_APPLICATIONS == STD_ON);                                       /* PRQA S 4304, 2995 */ /* MD_Os_C90BooleanCompatibility, MD_Os_Rule2.2_2995 */
}


/***********************************************************************************************************************
 *  Os_MpIsEnabledForThreads()
 **********************************************************************************************************************/
OS_FUNC_ATTRIBUTE_DEFINITION(OS_LOCAL_INLINE Os_StdReturnType, OS_CODE,                                                 /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
OS_ALWAYS_INLINE, Os_MpIsEnabledForThreads,
(
  void
))
{
  return (Os_StdReturnType)(OS_CFG_MEMORY_PROTECTION_FOR_THREADS == STD_ON);                                            /* PRQA S 4304, 2995 */ /* MD_Os_C90BooleanCompatibility, MD_Os_Rule2.2_2995 */
}


/***********************************************************************************************************************
 *  Os_MpSwitch()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_FUNC_ATTRIBUTE_DEFINITION(OS_LOCAL_INLINE void, OS_CODE, OS_ALWAYS_INLINE,                                           /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
Os_MpSwitch,
(
  P2CONST(Os_MpAccessRightsType, AUTOMATIC, OS_CONST) Current,
  P2CONST(Os_MpAccessRightsType, AUTOMATIC, OS_CONST) Next
))
{
  if(Os_MpIsEnabled() != 0u)                                                                                            /* PRQA S 2991, 2995 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule2.2_2995 */
  {
    if(Os_MpIsEnabledForApplications() != 0u)                                                                           /* PRQA S 2991, 2995 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule2.2_2995 */
    {
      /* #10 If current and next application access rights are different: */
      if(Current->AppAccessRights != Next->AppAccessRights)
      {
        /* #20 Switch application access rights. */
        Os_Hal_MpAppSwitch(Next->AppAccessRights);                                                                      /* SBSW_OS_MP_Hal_MPAPPSWITCH_001 */
      }
    }
    if(Os_MpIsEnabledForThreads() != 0u)                                                                                /* PRQA S 2991, 2995 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule2.2_2995 */
    {
      /* #30 Switch thread access rights. */
      Os_Hal_MpThreadSwitch(Next->ThreadAccessRights);                                                                  /* SBSW_OS_MP_Hal_MPATHREADSWITCH_001 */
    }
  }

}


/***********************************************************************************************************************
 *  Os_MpSwitchTo()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 */
OS_FUNC_ATTRIBUTE_DEFINITION(OS_LOCAL_INLINE void, OS_CODE, OS_ALWAYS_INLINE,                                           /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
Os_MpSwitchTo,
(
  P2CONST(Os_MpAccessRightsType, AUTOMATIC, OS_CONST) AccessRights
))
{
  if(Os_MpIsEnabled() != 0u)                                                                                            /* PRQA S 2991, 2995 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule2.2_2995 */
  {
    if(Os_MpIsEnabledForApplications() != 0u)                                                                           /* PRQA S 2991, 2995 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule2.2_2995 */
    {
      /* #10 Switch application access rights. */
      Os_Hal_MpAppSwitch(AccessRights->AppAccessRights);                                                                /* SBSW_OS_MP_Hal_MPAPPSWITCH_001 */
    }

    if(Os_MpIsEnabledForThreads() != 0u)                                                                                /* PRQA S 2991, 2995 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule2.2_2995 */
    {
      /* #20 Switch thread access rights. */
      Os_Hal_MpThreadSwitch(AccessRights->ThreadAccessRights);                                                          /* SBSW_OS_MP_Hal_MPATHREADSWITCH_001 */
    }
  }
}


# define OS_STOP_SEC_CODE
# include "Os_MemMap_OsCode.h"                                                                                          /* PRQA S 5087 */ /* MD_MSR_MemMap */




#endif /* OS_MEMORYPROTECTION_H */

/* module specific MISRA deviations:
 */

/*!
 * \}
 */
/***********************************************************************************************************************
 *  END OF FILE: Os_MemoryProtectionInt.h
 **********************************************************************************************************************/
