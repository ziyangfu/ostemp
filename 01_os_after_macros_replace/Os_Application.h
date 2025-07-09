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
 *  \addtogroup Os_Application
 *  \{
 *  \file
 *  \brief       Public application services.
 *  \details     --no details--
 *
 **********************************************************************************************************************/
/***********************************************************************************************************************
 *  REVISION HISTORY
 *  --------------------------------------------------------------------------------------------------------------------
 *  Refer to Os.h.
 **********************************************************************************************************************/

#ifndef OS_APPLICATION_H
# define OS_APPLICATION_H
                                                                                                                         /* PRQA S 0388 EOF */ /* MD_MSR_Dir1.1 */
/***********************************************************************************************************************
 *  INCLUDES
 **********************************************************************************************************************/
/* AUTOSAR includes */
# include "Std_Types.h"

/* Os module declarations */
# include "Os_ApplicationInt.h"

/* Os kernel module dependencies */
# include "Os_Error.h"
# include "Os_Core.h"
# include "Os_Isr.h"
# include "Os_TimingProtection.h"
# include "Os_Task.h"
# include "Os_Hook.h"
# include "Os_Counter.h"
# include "Os_Alarm.h"
# include "Os_Application.h"
# include "Os_ScheduleTable.h"
# include "Os_Common.h"
# include "Os_ServiceFunction.h"

/* Os Hal dependencies */
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
 *  Os_AppSetNotAccessible()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline void   /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
Os_AppSetNotAccessible (
  const   Os_AppConfigType OS_CONST * App
)
{
  App->Dyn->State = APPLICATION_TERMINATED;                                                                             /* SBSW_OS_APP_DYN_001 */
}


/***********************************************************************************************************************
 *  Os_AppStart()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 */
OS_ALWAYS_INLINE static inline void   Os_AppStart /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
(
  const   Os_AppConfigType OS_CONST * App
)
{
  Os_ObjIdx_IteratorType index;

  /* #10 Start Counters. */
  for(index = 0; index < (Os_ObjIdx_IteratorType) App->CounterCount; index++)
  {
    Os_CounterStart(App->CounterRefs[index]);                                                                           /* SBSW_OS_APP_COUNTERSTART_001 */
  }
}


/***********************************************************************************************************************
 *  Os_AppGetState()
 **********************************************************************************************************************/
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
OS_ALWAYS_INLINE static inline ApplicationStateType   Os_AppGetState (
  const   Os_AppConfigType OS_CONST * App
)
{
  return App->Dyn->State;
}


/***********************************************************************************************************************
 *  Os_AppIsTrusted()
 **********************************************************************************************************************/
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
OS_ALWAYS_INLINE static inline Os_StdReturnType   Os_AppIsTrusted (
  const   Os_AppConfigType OS_CONST * App
)
{
  return (Os_StdReturnType)(App->IsTrusted != FALSE);                                                                   /* PRQA S 4304 */ /* MD_Os_C90BooleanCompatibility */
}


/***********************************************************************************************************************
 *  Os_AppIsPrivileged()
 **********************************************************************************************************************/
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */ /* COV_OS_HALPLATFORMAPPISPRIVILEGED */
OS_ALWAYS_INLINE static inline Os_StdReturnType   Os_AppIsPrivileged (
  const   Os_AppConfigType OS_CONST * App
)                                                                                                                      /* COV_OS_HALPLATFORMAPPISPRIVILEGED */
{
  return (Os_StdReturnType)(App->IsPrivileged != FALSE);                                                                /* PRQA S 4304 */ /* MD_Os_C90BooleanCompatibility */
}


/***********************************************************************************************************************
 *  Os_AppHasTimingProtectionDelay()
 **********************************************************************************************************************/
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
OS_ALWAYS_INLINE static inline Os_StdReturnType   Os_AppHasTimingProtectionDelay (
  const   Os_AppConfigType OS_CONST * App
)
{
  return (Os_StdReturnType)((Os_TpIsEnabled()) && (App->HasTimingProtectionDelay != FALSE));                            /* PRQA S 4115, 4304, 4558 */ /* MD_Os_Rule10.1_4115, MD_Os_C90BooleanCompatibility, MD_Os_Rule10.1_4558 */ /* COV_OS_HALPLATFORMTPUNSUPPORTED_XF */
}


/***********************************************************************************************************************
 *  Os_AppAccessArrayNeeded()
 **********************************************************************************************************************/
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
OS_ALWAYS_INLINE static inline Os_StdReturnType   Os_AppAccessArrayNeeded (void)
{
  return (Os_StdReturnType)((Os_AppMaskType)OS_APPID_COUNT > OS_APPMASK_BITWIDTH);                                      /* PRQA S 2996, 4304 */ /* MD_Os_Rule2.2_2996, MD_Os_C90BooleanCompatibility */
}


/***********************************************************************************************************************
 *  Os_AppIsAccessAllowed()
 **********************************************************************************************************************/
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
OS_ALWAYS_INLINE static inline Os_StdReturnType   Os_AppIsAccessAllowed (
  ApplicationType AppId,
  Os_AppMaskType AppMask,
  Os_AppAccessMaskType AccessMask
)
{
  Os_AppMaskType accessMsk;

  if(Os_AppAccessArrayNeeded() != 0u)                                                                                   /* PRQA S 2992, 2996 */ /* MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2996 */
  {
    accessMsk = ((Os_AppMaskType*)AccessMask)[(Os_AppMaskType)AppId >> OS_APPMASK_INDEXBITWIDTH];                       /* PRQA S 0306 */ /* MD_Os_Rule11.4_0306 */
  }
  else
  {
    accessMsk = (Os_AppMaskType)AccessMask;
  }

  return (Os_StdReturnType)(AppMask & accessMsk);
}


/***********************************************************************************************************************
 *  Os_AppCheckAccess()
 **********************************************************************************************************************/
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
OS_ALWAYS_INLINE static inline Os_StdReturnType   Os_AppCheckAccess (
  const   Os_AppConfigType OS_CONST * App,
  Os_AppAccessMaskType AccessMask
)
{
  return Os_ErrServiceProtectionCheck(Os_AppIsAccessAllowed(App->Id, App->AccessRightId, AccessMask));
}


/***********************************************************************************************************************
 *  Os_AppCheckIsAccessible()
 **********************************************************************************************************************/
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
OS_ALWAYS_INLINE static inline Os_StdReturnType   Os_AppCheckIsAccessible (
  const   Os_AppConfigType OS_CONST * App
)
{
  return Os_ErrServiceProtectionCheck((Os_StdReturnType)(                                                               /* PRQA S 4304 */ /* MD_Os_C90BooleanCompatibility */
      Os_AppGetState(App) == APPLICATION_ACCESSIBLE));                                                                  /* SBSW_OS_FC_PRECONDITION */
}


/***********************************************************************************************************************
 *  Os_AppIsCoreLocal()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StdReturnType /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
    Os_AppIsCoreLocal (
  const   Os_AppConfigType OS_CONST * App
)
{
  return (Os_StdReturnType)(Os_CoreAsrIsCoreLocal(Os_AppGetCore(App)));                                                 /* SBSW_OS_APP_COREASRISCORELOCAL_001 */ /* SBSW_OS_FC_PRECONDITION */
}


/***********************************************************************************************************************
 *  Os_AppIsKernelApp()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE                                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
static inline Os_StdReturnType   Os_AppIsKernelApp (
  const   Os_AppConfigType OS_CONST * App
)
{
  return (Os_StdReturnType)(App == Os_CoreGetKernelApp(Os_AppGetCore(App)));                                            /* SBSW_OS_APP_COREGETKERNELAPP_001 */ /* SBSW_OS_FC_PRECONDITION */ /* PRQA S 4304 */ /* MD_Os_C90BooleanCompatibility */
}


/***********************************************************************************************************************
 *  Os_AppGetCore()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE                                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
static inline const   Os_CoreAsrConfigType OS_CONST *   Os_AppGetCore (
  const   Os_AppConfigType OS_CONST * App
)
{
  return App->Core;
}


/***********************************************************************************************************************
 *  Os_AppGetId()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE                                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
static inline ApplicationType   Os_AppGetId (
  const   Os_AppConfigType OS_CONST * App
)
{
  return App->Id;
}


/***********************************************************************************************************************
 *  Os_AppGetStartupHook()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
const   Os_HookCallbackConfigType OS_CONST *   Os_AppGetStartupHook (
  const   Os_AppConfigType OS_CONST * App
)
{
  return App->StartupHookRef;
}


/***********************************************************************************************************************
 *  Os_AppGetErrorHook()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
const   Os_HookStatusHookConfigType OS_CONST *   Os_AppGetErrorHook (
  const   Os_AppConfigType OS_CONST * App
)
{
  return App->ErrorHookRef;
}


/***********************************************************************************************************************
 *  Os_AppGetShutdownHook()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
const   Os_HookStatusHookConfigType OS_CONST *   Os_AppGetShutdownHook (
  const   Os_AppConfigType OS_CONST * App
)
{
  return App->ShutdownHookRef;
}


/***********************************************************************************************************************
 *  Os_AppCheckId()
 **********************************************************************************************************************/
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
OS_ALWAYS_INLINE static inline Os_StdReturnType   Os_AppCheckId (
  ApplicationType AppId
)
{
  return Os_ErrExtendedCheck(Os_ErrIsValueLo((uint32)AppId, (uint32)OS_APPID_COUNT));
}


/***********************************************************************************************************************
 *  Os_AppId2App()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE                                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
static inline const   Os_AppConfigType OS_CONST *   Os_AppId2App (
  ApplicationType AppId
)
{
  Os_Assert(Os_ErrIsValueLo((uint32)AppId, (uint32)OS_APPID_COUNT));
  return OsCfg_AppRefs[AppId];
}


/***********************************************************************************************************************
 *  Os_AppIsRestartTaskAvailable()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE                                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
static inline Os_StdReturnType   Os_AppIsRestartTaskAvailable (
    const   Os_AppConfigType OS_CONST * App
)
{
  return (Os_StdReturnType)(App->RestartTask != ((void *)0));                                                              /* PRQA S 4304 */ /* MD_Os_C90BooleanCompatibility */
}


# define OS_STOP_SEC_CODE
# include "Os_MemMap_OsCode.h"                                                                                          /* PRQA S 5087 */ /* MD_MSR_MemMap */



#endif /* OS_APPLICATION_H */

/* module specific MISRA deviations:
 */

/*!
 * \}
 */
/***********************************************************************************************************************
 *  END OF FILE: Os_Application.h
 **********************************************************************************************************************/
