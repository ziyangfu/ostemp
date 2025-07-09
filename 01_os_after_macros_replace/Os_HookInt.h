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
 *  \ingroup     Os_Kernel
 *  \defgroup    Os_Hook Hook
 *  \brief       Hook function calling.
 *  \details
 *
 *  A Hook function is implemented by the user and invoked by the operating system in the case of certain incidents.
 *
 *  In MICROSAR OS hooks are encapsulated in a thread.
 *
 *  Hooks don't have a timing budget. They are executed within the current budget with disabled interrupts.
 *
 *  The following scenarios are exemplary shown for the ErrorHook.
 *
 *  ![Hook Call](Hook_Call.png)
 *
 *  ![Hook Caller Task Killed](Hook_TaskKilled.png)
 *
 *  \{
 *
 *  \file
 *  \brief       Inlined Hook routines and type definitions.
 *  \details     --no details--
 *
 *
 **********************************************************************************************************************/
/***********************************************************************************************************************
 *  REVISION HISTORY
 *  --------------------------------------------------------------------------------------------------------------------
 *  Refer to Os.h.
 **********************************************************************************************************************/

#ifndef OS_HOOKINT_H
# define OS_HOOKINT_H
                                                                                                                         /* PRQA S 0388 EOF */ /* MD_MSR_Dir1.1 */
/***********************************************************************************************************************
 *  INCLUDES
 **********************************************************************************************************************/
/* AUTOSAR includes */
# include "Std_Types.h"

/* Os module declarations */
# include "Os_Hook_Types.h"

/* Os kernel module dependencies */
# include "Os_ThreadInt.h"

/* Os Hal dependencies */
# include "Os_Hal_CompilerInt.h"



/***********************************************************************************************************************
 *  GLOBAL CONSTANT MACROS
 **********************************************************************************************************************/


/***********************************************************************************************************************
 *  GLOBAL FUNCTION MACROS
 **********************************************************************************************************************/


/*! Type cast from Os_HookType to Os_ThreadType by use of base element addressing. */
#define OS_HOOK_CASTDYN_HOOK_2_THREAD(hook)               (&((hook).Thread))


/*! Type cast from Os_HookStatusHookConfigType to Os_HookConfigType by use of base element addressing. */
#define OS_HOOK_CASTCONFIG_STATUSHOOK_2_HOOK(hook)        (&((hook).Hook))


/*! Type cast from Os_HookCallbackConfigType to Os_HookConfigType by use of base element addressing. */
#define OS_HOOK_CASTCONFIG_CALLBACK_2_HOOK(hook)          (&((hook).Hook))


/*! Type cast from Os_HookProtectionHookConfigType to Os_HookConfigType by use of base element addressing. */
#define OS_HOOK_CASTCONFIG_PROTECTIONHOOK_2_HOOK(hook)    (&((hook).Hook))



/***********************************************************************************************************************
 *  GLOBAL DATA TYPES AND STRUCTURES
 **********************************************************************************************************************/
/*! Specifies the hook callback return behavior if the caller thread is a task. */
typedef enum
{
  /*! Hook returns to caller tasks. */
  Os_HookTaskReturn_ToCallerTask,   /* Startup, ShutdownHook */
  /*! Hook returns into tasks with highest priority. */
  Os_HookTaskReturn_WithSchedule,   /* ErrorHook, ProtectionHook, Callback */
  /*! Hook switches to task with highest priority or to idle task. */
  Os_HookTaskReturn_BeginScheduling /* Os_CoreInitHook */
}Os_HookTaskReturnType;


/*! Function pointer data type for hook user functions with status parameter. */
typedef P2void    , Os_HookStatusHookCbkType(StatusType);                                                    /* PRQA S 1336 */ /* MD_Os_Rule8.2_1336 */

/*! Function pointer data type for startup hook user functions. */
typedef P2void    , Os_HookVoidHookCbkType(void);

/*! Function pointer data type for hook user functions with status parameter. */
typedef P2ProtectionReturnType    , Os_HookProtectionHookCbkType(StatusType);                                /* PRQA S 1336 */ /* MD_Os_Rule8.2_1336 */

/*! Function pointer data type for init hook functions with mode parameter. */
typedef P2void    , Os_HookInitHookCbkType(AppModeType);                                                     /* PRQA S 1336 */ /* MD_Os_Rule8.2_1336 */

/*! Defines the parameter types which may be passed to a hook.*/
typedef union
{                                                                                                                       /* PRQA S 0750 */ /* MD_MSR_Union */
  StatusType Status;
  AppModeType Mode;
}Os_HookParameterType;

/*! Defines the parameter types which may be returned from a hook.*/
typedef union
{                                                                                                                       /* PRQA S 0750 */ /* MD_MSR_Union */
  ProtectionReturnType ProtectionReaction;
}Os_HookReturnType;


/*! Dynamic management information of a hook.
 * \extends Os_ThreadType_Tag
 */
struct Os_HookType_Tag
{
  /*! Dynamic thread attributes.
   * This attribute must come first! */
  Os_ThreadType Thread;

  /*! A copy of the interrupt state of the caller. */
  Os_IntApiStateType IntApiStateCopy;

  /*! The parameter which is passed to the hook. */
  Os_HookParameterType Parameter;

  /*! The return value of a hook */
  Os_HookReturnType ReturnValue;

  /*! Contains a snapshot of the hook flags. */
  Os_HookFlagType HookTypeFlag;
};


/*! \brief    Configuration information of an abstract hook.
 *  \details
 *  The dynamic data pointer of a hook can be fetched with Os_HookGetDyn().
 *  \extends Os_ThreadConfigType_Tag
 */
struct Os_HookConfigType_Tag
{
  /*! Thread attributes.
   * This attribute must come first! */
  Os_ThreadConfigType Thread;

  /*! Specifies the type of a Hook. */
  Os_HookFlagType HookTypeFlag;

  /*! Specifies which hooks may be called within this hook. */
  Os_HookFlagType NestingMask;

  /*! Unique hook identifier. */
  Os_HookIdType Id;

  /*! Specifies how to return to task level */
  Os_HookTaskReturnType TaskReturn;
};


/*! Status callbacks. */
struct Os_HookStatusHookConfigType_Tag
{
  /*! The base class.
   *  This argument must come first. */
  Os_HookConfigType Hook;

  /*! The callback function which shall be executed. */
  Os_HookStatusHookCbkType Callback;
};


/*! Container for void callbacks. */
struct Os_HookCallbackConfigType_Tag
{
  /*! The base class.
   *  This argument must come first. */
  Os_HookConfigType Hook;

  /*! The callback function which shall be executed. */
  Os_HookVoidHookCbkType Callback;
};


/*! Container for protection callbacks. */
struct Os_HookProtectionHookConfigType_Tag
{
  /*! The base class.
   *  This argument must come first. */
  Os_HookConfigType Hook;

  /*! The callback function which shall be executed. */
  Os_HookProtectionHookCbkType Callback;
};

/*! Container for init callbacks. */
struct Os_HookInitHookConfigType_Tag
{
  /*! The base class.
   *  This argument must come first. */
  Os_HookConfigType Hook;

  /*! The callback function which shall be executed. */
  Os_HookInitHookCbkType Callback;
};


/***********************************************************************************************************************
 *  GLOBAL DATA PROTOTYPES
 **********************************************************************************************************************/


/***********************************************************************************************************************
 *  GLOBAL FUNCTION PROTOTYPES
 **********************************************************************************************************************/

# define OS_START_SEC_CODE
# include "Os_MemMap_OsCode.h"                                                                                          /* PRQA S 5087 */ /* MD_MSR_MemMap */

/***********************************************************************************************************************
 *  Os_HookIsErrorHookEnabled()
 **********************************************************************************************************************/
/*! \brief        Returns whether error hooks are enabled or not.
 *  \details      --no details--
 *
 *  \retval       !0    Error hooks are enabled.
 *  \retval       0     Error hooks are disabled.
 *
 *  \context      OS_INTERNAL
 *
 *  \reentrant    TRUE
 *  \synchronous  TRUE
 *
 *  \pre          -
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StdReturnType   Os_HookIsErrorHookEnabled ( void );


/***********************************************************************************************************************
 *  Os_HookIsProtectionHookEnabled()
 **********************************************************************************************************************/
/*! \brief        Returns whether ProtectionHook is enabled or not.
 *  \details      --no details--
 *
 *  \retval       !0    ProtectionHook is enabled.
 *  \retval       0     ProtectionHook is disabled.
 *
 *  \context      OS_INTERNAL
 *
 *  \reentrant    TRUE
 *  \synchronous  TRUE
 *
 *  \pre          -
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StdReturnType   Os_HookIsProtectionHookEnabled ( void );


/***********************************************************************************************************************
 *  Os_HookGetThread()
 **********************************************************************************************************************/
/*! \brief          Returns the thread of the hook.
 *  \details        --no details--
 *
 *  \param[in]      Hook  Hook to query. Parameter must not be NULL.
 *
 *  \return         The thread of the hook.
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre            -
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline const   Os_ThreadConfigType OS_CONST *   Os_HookGetThread (
  const   Os_HookConfigType OS_CONST * Hook
);


/***********************************************************************************************************************
 *  Os_HookThread2Hook()
 **********************************************************************************************************************/
/*! \brief          Translate thread reference to hook.
 *  \details        The caller has to ensure, that the thread actually belongs to a hook.
 *
 *  \param[in]      Thread     Thread to translate. Parameter must not be NULL.
 *
 *  \context        ANY
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre            -
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline const   Os_HookConfigType OS_CONST *   Os_HookThread2Hook (
  const   Os_ThreadConfigType OS_CONST * Thread
);


/***********************************************************************************************************************
 *  Os_HookInfoInit()
 **********************************************************************************************************************/
/*! \brief        Initialize hooks state object.
 *  \details      This function is called in Init-Step3 and Init-Step4 in order to handle the situation that
 *                Init-Step3 is optional.
 *
 *  \param[out]   HookInfo     Hooks state. Parameter must not be NULL.
 *
 *  \context      OS_INTERNAL
 *
 *  \reentrant    TRUE
 *  \synchronous  TRUE
 *
 *  \pre          Concurrent access to given object is prevented by caller.
 **********************************************************************************************************************/
void     Os_HookInfoInit
(
    Os_HookInfoType OS_VAR_NOINIT * HookInfo
);


/***********************************************************************************************************************
 *  Os_HookCallStatusHook()
 **********************************************************************************************************************/
/*! \brief        Calls given status hook in a separate thread.
 *  \details      If given pointer is NULL, this function performs no action.
 *
 *  \param[in]    Hook    Hook to call. Parameter must not be NULL.
 *  \param[in]    Error   Status code to pass.
 *
 *  \context      OS_INTERNAL
 *
 *  \reentrant    TRUE
 *  \synchronous  TRUE
 *
 *  \pre          Concurrent access to given object is prevented by caller.
 *
 *  \trace        SPEC-64016
 **********************************************************************************************************************/
void     Os_HookCallStatusHook
(
  const   Os_HookStatusHookConfigType OS_CONST * Hook,
  StatusType Error
);


/***********************************************************************************************************************
 *  Os_HookStatusHookGetThread()
 **********************************************************************************************************************/
/*! \brief          Returns callbacks's thread.
 *  \details        --no details--
 *
 *  \param[in]      Hook  Hook to query. Parameter must not be NULL.
 *
 *  \return         Callback hook's thread.
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre            -
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline const   Os_ThreadConfigType OS_CONST *   Os_HookStatusHookGetThread (
  const   Os_HookStatusHookConfigType OS_CONST * Hook
);


/***********************************************************************************************************************
 *  Os_HookProtectionHookGetThread()
 **********************************************************************************************************************/
/*! \brief          Returns protection hook's thread.
 *  \details        --no details--
 *
 *  \param[in]      Hook  Hook to query. Parameter must not be NULL.
 *
 *  \return         Protection hook's thread.
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre            -
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline const   Os_ThreadConfigType OS_CONST *   Os_HookProtectionHookGetThread (
  const   Os_HookProtectionHookConfigType OS_CONST * Hook
);


/***********************************************************************************************************************
 *  Os_HookCallProtectionHook()
 **********************************************************************************************************************/
/*! \brief        Calls given protection hook in a separate thread.
 *  \details      If given pointer is NULL, this function performs no action.
 *
 *  \param[in]    Hook        Hook to call. Parameter must not be NULL.
 *  \param[in]    Fatalerror  Status code to pass.
 *
 *  \return       Protection reaction returned by the protection hook.
 *
 *  \context      OS_INTERNAL
 *
 *  \reentrant    TRUE
 *  \synchronous  TRUE
 *
 *  \trace        SPEC-63832
 *
 *  \pre          Concurrent access to given object is prevented by caller.
 **********************************************************************************************************************/
ProtectionReturnType     Os_HookCallProtectionHook
(
  const   Os_HookProtectionHookConfigType OS_CONST * Hook,
  StatusType Fatalerror
);


/***********************************************************************************************************************
 *  Os_HookWrapperProtectionHook()
 **********************************************************************************************************************/
/*! \brief          Calls user shutdown hook function, passes status.
 *  \details        --no details--
 *
 *  \param[in]      Thread    Thread of Hook to be called. Parameter must not be NULL.
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre            Concurrent access is prevented by caller.
 *  \pre            Never called from static source code (Callback routine).
 **********************************************************************************************************************/
void     Os_HookWrapperProtectionHook
(
  const   Os_ThreadConfigType OS_CONST * Thread
);


/***********************************************************************************************************************
 *  Os_HookWrapperStatusHook()
 **********************************************************************************************************************/
/*! \brief        Calls user hook function, passes status.
 *  \details      --no details--
 *
 *  \param[in]    Thread    Thread of Hook to be called. Parameter must not be NULL.
 *
 *  \context      OS_INTERNAL
 *
 *  \reentrant    TRUE
 *  \synchronous  TRUE
 *
 *  \pre          Concurrent access is prevented by caller.
 *  \pre          Never called from static source code (Callback routine).
 **********************************************************************************************************************/
void     Os_HookWrapperStatusHook
(
  const   Os_ThreadConfigType OS_CONST * Thread
);


/***********************************************************************************************************************
 *  Os_HookWrapperCallback()
 **********************************************************************************************************************/
/*! \brief        Calls callback function without a parameter.
 *  \details      --no details--
 *
 *  \param[in]    Thread    Thread of Hook to be called. Parameter must not be NULL.
 *
 *  \context      OS_INTERNAL
 *
 *  \reentrant    TRUE
 *  \synchronous  TRUE
 *
 *  \pre          Concurrent access is prevented by caller.
 *  \pre          Never called from static source code (Callback routine).
 **********************************************************************************************************************/
void     Os_HookWrapperCallback
(
  const   Os_ThreadConfigType OS_CONST * Thread
);


/***********************************************************************************************************************
 *  Os_HookCallOs_CoreInitHook()
 **********************************************************************************************************************/
/*! \brief        Calls given init hook in a separate thread.
 *  \details      If given pointer is NULL, this function calls the panic hook.
 *
 *  \param[in]    Hook        Hook to call. Parameter must not be NULL.
 *  \param[in]    Mode        application mode to pass.
 *
 *  \context      OS_INTERNAL
 *
 *  \reentrant    FALSE
 *  \synchronous  TRUE
 *
 *  \pre          Concurrent access to given object is prevented by caller.
 **********************************************************************************************************************/
void     Os_HookCallOs_CoreInitHook
(
  const   Os_HookInitHookConfigType OS_CONST * Hook,
  AppModeType Mode
);


/***********************************************************************************************************************
 *  Os_HookWrapperOs_CoreInitHook()
 **********************************************************************************************************************/
/*! \brief        Calls init hook function, passes application mode.
 *  \details      --no details--
 *
 *  \context      OS_INTERNAL
 *
 *  \reentrant    FALSE
 *  \synchronous  TRUE
 *
 *  \pre          Never called from static source code (Callback routine).
 **********************************************************************************************************************/
void     Os_HookWrapperOs_CoreInitHook
(
  void
);


/***********************************************************************************************************************
 *  Os_HookInit()
 **********************************************************************************************************************/
/*! \brief        Initialize the given hook.
 *  \details      --no details--
 *
 *  \param[in,out]    Hook     Hook to initialize. Parameter must not be NULL.
 *
 *  \context      OS_INTERNAL
 *
 *  \reentrant    TRUE
 *  \synchronous  TRUE
 *
 *  \pre          Concurrent access to given object is prevented by caller.
 **********************************************************************************************************************/
void     Os_HookInit
(
  const   Os_HookConfigType OS_CONST * Hook
);


/***********************************************************************************************************************
 *  Os_HookCallCallback()
 **********************************************************************************************************************/
/*! \brief        Calls given callback in a separate thread.
 *  \details      If the given callback is not assigned to the local core, a cross core request is triggered.
 *
 *  \param[in]    Hook     Hook to call. Parameter must not be NULL.
 *
 *  \context      OS_INTERNAL
 *
 *  \reentrant    TRUE
 *  \synchronous  TRUE
 *
 *  \pre          -
 *
 *  \trace        SPEC-63577
 **********************************************************************************************************************/
Os_StatusType     Os_HookCallCallback
(
  const   Os_HookCallbackConfigType OS_CONST * Hook
);


/***********************************************************************************************************************
 *  Os_HookCallCallbackLocal()
 **********************************************************************************************************************/
/*! \brief        Calls given callback in a separate thread.
 *  \details      --no details--
 *
 *  \param[in]    Hook     Hook to call. Parameter must not be NULL.
 *
 *  \retval       OS_STATUS_OK                    If no error occurs.
 *  \retval       OS_STATUS_NOTACCESSIBLE_1       If the owner application of the given hook is not accessible.
 *
 *  \context      OS_INTERNAL
 *
 *  \reentrant    TRUE
 *  \synchronous  TRUE
 *
 *  \pre          Concurrent access to given object is prevented by caller.
 *  \pre          Callback is assigned to local core.
 **********************************************************************************************************************/
Os_StatusType     Os_HookCallCallbackLocal
(
  const   Os_HookCallbackConfigType OS_CONST * Hook
);


/***********************************************************************************************************************
 *  Os_HookCallbackGetThread()
 **********************************************************************************************************************/
/*! \brief          Returns callbacks's thread.
 *  \details        --no details--
 *
 *  \param[in]      Hook  Hook to query. Parameter must not be NULL.
 *
 *  \return         Callback hook's thread.
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre            -
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline const   Os_ThreadConfigType OS_CONST *   Os_HookCallbackGetThread (
  const   Os_HookCallbackConfigType OS_CONST * Hook
);


/***********************************************************************************************************************
 *  Os_HookKill()
 **********************************************************************************************************************/
/*! \brief          Kill the given hook.
 *  \details        The given hook is killed and the previous thread, where the hook has been triggered is resumed.
 *
 *  \param[in,out]  Hook    Hook to kill. Parameter must not be NULL.
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre            Concurrent access to given object is prevented by caller.
 **********************************************************************************************************************/
void     Os_HookKill
(
  const   Os_HookConfigType OS_CONST * Hook
);


/***********************************************************************************************************************
 *  Os_HookSuicide()
 **********************************************************************************************************************/
/*! \brief          Self termination of current hook.
 *  \details        Execute the hook return path.
 *
 *  \context        STARTHOOK|ERRHOOK|SHUTHOOK
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre            Current thread is a hook.
 *  \pre            Os_HookKill() has already been called.
 *  \pre            Concurrent access to given object is prevented by caller.
 **********************************************************************************************************************/
OS_NORETURN OS_ALWAYS_INLINE static inline void   Os_HookSuicide (void);


/***********************************************************************************************************************
 *  Os_HookReturn()
 **********************************************************************************************************************/
/*! \brief          Resume previous thread where the hook was called.
 *  \details        --no details--
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre            The current thread is a hook thread.
 **********************************************************************************************************************/
OS_NORETURN void   Os_HookReturn (void);


/***********************************************************************************************************************
 *  Os_HookCheckId()
 **********************************************************************************************************************/
/*! \brief          Returns whether the given ID is valid (Not OS_CHECK_FAILED) or not (OS_CHECK_FAILED).
 *  \details        In case that extended checks are disabled, the default result of \ref Os_ErrExtendedCheck()
 *                  is returned.
 *
 *  \param[in]      Id        The ID to check.
 *
 *  \retval         Not OS_CHECK_FAILED   If the given ID is valid.
 *  \retval         OS_CHECK_FAILED       If the given ID is not valid.
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre            -
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StdReturnType   Os_HookCheckId (
  Os_HookIdType Id
);


/***********************************************************************************************************************
 *  Os_HookId2Hook()
 **********************************************************************************************************************/
/*! \brief          Returns the hook corresponding to the given ID.
 *  \details        --no details--
 *
 *  \param[in]     Id     The hook Id to convert. The value must be smaller than OS_CFG_NUM_HOOKS.
 *
 *  \return         Hook with the given Id.
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre            -
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline const   Os_HookConfigType OS_CONST *   Os_HookId2Hook (
  Os_HookIdType Id
);


/***********************************************************************************************************************
 *  Os_Hook2CallbackHook()
 **********************************************************************************************************************/
/*! \brief          Returns the hook as a callback.
 *  \details        --no details--
 *
 *  \param[in]      Hook     The hook to convert. Parameter must not be NULL and must be a callback.
 *
 *  \return         Callback.
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre            -
 **********************************************************************************************************************/
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
OS_ALWAYS_INLINE static inline const   Os_HookCallbackConfigType OS_CONST *   Os_Hook2CallbackHook (
  const   Os_HookConfigType OS_CONST * Hook
);


/***********************************************************************************************************************
 *  Os_HookCallPanicHook()
 **********************************************************************************************************************/
/*! \brief        Calls Panic Hook, if configured.
 *  \details      --no details--
 *
 *  \context      OS_INTERNAL
 *
 *  \reentrant    TRUE
 *  \synchronous  TRUE
 *
 *  \pre          -
 **********************************************************************************************************************/
void     Os_HookCallPanicHook(void);


# define OS_STOP_SEC_CODE
# include "Os_MemMap_OsCode.h"                                                                                          /* PRQA S 5087 */ /* MD_MSR_MemMap */




#endif /* OS_HOOKINT_H */

/* module specific MISRA deviations:
 */

/*!
 * \}
 */
/***********************************************************************************************************************
 *  END OF FILE: Os_HookInt.h
 **********************************************************************************************************************/
