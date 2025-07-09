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
 * \addtogroup Os_Hook
 * \{
 *
 * \file
 * \brief       Public interface of Hook unit.
 * \details     --no details--
 *
 *
 **********************************************************************************************************************/
/***********************************************************************************************************************
 *  REVISION HISTORY
 *  --------------------------------------------------------------------------------------------------------------------
 *  Refer to Os.h.
 **********************************************************************************************************************/

#ifndef OS_HOOK_H
# define OS_HOOK_H
                                                                                                                         /* PRQA S 0388 EOF */ /* MD_MSR_Dir1.1 */
/***********************************************************************************************************************
 *  INCLUDES
 **********************************************************************************************************************/
/* AUTOSAR includes */
# include "Std_Types.h"

/* Os module declarations */
# include "Os_HookInt.h"
# include "Os_Hook_Cfg.h"
# include "Os_Hook_Lcfg.h"

/* Os module dependencies */
# include "Os_Common.h"
# include "Os_Thread.h"
# include "Os_Core.h"
# include "Os_Error.h"

/* Os hal dependencies */
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

# define OS_START_SEC_CODE
# include "Os_MemMap_OsCode.h"                                                                                          /* PRQA S 5087 */ /* MD_MSR_MemMap */

/***********************************************************************************************************************
 *  GLOBAL FUNCTIONS
 **********************************************************************************************************************/

/***********************************************************************************************************************
 *  Os_HookIsErrorHookEnabled()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StdReturnType   Os_HookIsErrorHookEnabled ( void )
{
  return (Os_StdReturnType)(OS_CFG_ERRORHOOKS  == STD_ON);                                                              /* PRQA S 4304, 2995 */ /* MD_Os_C90BooleanCompatibility, MD_Os_Rule2.2_2995 */
}


/***********************************************************************************************************************
 *  Os_HookIsProtectionHookEnabled()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StdReturnType   /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
Os_HookIsProtectionHookEnabled ( void )
{
  return (Os_StdReturnType)(OS_CFG_PROTECTIONHOOK == STD_ON);                                                           /* PRQA S 4304, 2995 */ /* MD_Os_C90BooleanCompatibility, MD_Os_Rule2.2_2995 */
}


/***********************************************************************************************************************
 *  Os_HookCallbackGetThread()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE                                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
static inline const   Os_ThreadConfigType OS_CONST *   Os_HookCallbackGetThread (
  const   Os_HookCallbackConfigType OS_CONST * Hook
)
{
  return &Hook->Hook.Thread;
}


/***********************************************************************************************************************
 *  Os_HookStatusHookGetThread()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE                                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
static inline const   Os_ThreadConfigType OS_CONST *   Os_HookStatusHookGetThread (
  const   Os_HookStatusHookConfigType OS_CONST * Hook
)
{
  return &Hook->Hook.Thread;
}


/***********************************************************************************************************************
 *  Os_HookProtectionHookGetThread()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE                                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
static inline const   Os_ThreadConfigType OS_CONST *   Os_HookProtectionHookGetThread (
  const   Os_HookProtectionHookConfigType OS_CONST * Hook
)
{
  return &Hook->Hook.Thread;
}


/***********************************************************************************************************************
 *  Os_HookThread2Hook()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline const   Os_HookConfigType OS_CONST *   /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
Os_HookThread2Hook (
  const   Os_ThreadConfigType OS_CONST * Thread
)
{
  Os_Assert((Os_StdReturnType)(Os_ThreadIsHook(Thread) != 0u));                                                         /* PRQA S 4304 */ /* MD_Os_C90BooleanCompatibility */ /* SBSW_OS_FC_PRECONDITION */
  return (const   Os_HookConfigType OS_CONST *)Thread;                                                       /* PRQA S 0310 */ /* MD_Os_Rule11.3_0310 */
}


/***********************************************************************************************************************
 *  Os_HookGetThread()
 **********************************************************************************************************************/
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
OS_ALWAYS_INLINE static inline const   Os_ThreadConfigType OS_CONST *   Os_HookGetThread (
  const   Os_HookConfigType OS_CONST * Hook
)
{
  return &(Hook->Thread);
}


/***********************************************************************************************************************
 *  Os_HookSuicide()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_NORETURN OS_ALWAYS_INLINE                                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
static inline void   Os_HookSuicide (void)
{
  const   Os_ThreadConfigType OS_CONST * currentThread = Os_CoreGetThread();

  /* #10 If the current thread is no hook: */
  if(OS_UNLIKELY(Os_ThreadIsHook(currentThread) == 0u))                                                                 /* SBSW_OS_HK_THREADISTASK_001 */ /* COV_OS_INVSTATE */
  {
    /* #20 Panic! */
    Os_ErrKernelPanic();
  }

  /* #30 Otherwise return from hook. */
  Os_HookReturn();
}


/***********************************************************************************************************************
 *  Os_HookCheckId()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StdReturnType   /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */ /* COV_OS_HALPLATFORMTESTEDASMULTICORE */
Os_HookCheckId (
  Os_HookIdType Id
)                                                                                                                      /* COV_OS_HALPLATFORMTESTEDASMULTICORE */
{
  return Os_ErrExtendedCheck(Os_ErrIsValueLo((uint32)Id, (uint32)OS_CFG_NUM_HOOKS));
}


/***********************************************************************************************************************
 *  Os_HookId2Hook()
 **********************************************************************************************************************/
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */ /* COV_OS_HALPLATFORMTESTEDASMULTICORE */
OS_ALWAYS_INLINE static inline const   Os_HookConfigType OS_CONST *   Os_HookId2Hook (
  Os_HookIdType Id
)                                                                                                                      /* COV_OS_HALPLATFORMTESTEDASMULTICORE */
{
  Os_Assert(Os_ErrIsValueLo((uint32)Id, (uint32)OS_CFG_NUM_HOOKS));
  return  OsCfg_HookRefs[Id];
}


/***********************************************************************************************************************
 *  Os_Hook2CallbackHook()
 **********************************************************************************************************************/
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
OS_ALWAYS_INLINE static inline const   Os_HookCallbackConfigType OS_CONST *   Os_Hook2CallbackHook (
  const   Os_HookConfigType OS_CONST * Hook
)
{
  return  (const   Os_HookCallbackConfigType OS_CONST *)(Hook);                                              /* PRQA S 0310 */ /* MD_Os_Rule11.3_0310 */
}



# define OS_STOP_SEC_CODE
# include "Os_MemMap_OsCode.h"                                                                                          /* PRQA S 5087 */ /* MD_MSR_MemMap */


#endif /* OS_HOOK_H */

/* module specific MISRA deviations:
 */

/*!
 * \}
 */
/***********************************************************************************************************************
 *  END OF FILE: Os_Hook.h
 **********************************************************************************************************************/
