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
 * \addtogroup Os_Trap
 * \{
 *
 * \file
 * \brief       Handles OS API calls
 *
 *
 **********************************************************************************************************************/
/***********************************************************************************************************************
 *  REVISION HISTORY
 *  --------------------------------------------------------------------------------------------------------------------
 *  Refer to Os.h.
 **********************************************************************************************************************/

                                                                                                                        /* PRQA S 0777, 0779, 0828  EOF */ /* MD_MSR_Rule5.1, MD_MSR_Rule5.2, MD_MSR_Dir1.1 */

#define OS_TRAP_SOURCE

/***********************************************************************************************************************
 *  INCLUDES
 **********************************************************************************************************************/
/* AUTOSAR includes */
#include "Std_Types.h"

/* Os module declarations */
#include "Os_Trap.h"

/* Os kernel module dependencies */
#include "Os_Trace.h"
#include "Os_Kernel.h"
#include "Os_Common_Types.h"
#include "Os_Cfg.h"
#include "Os.h"

/* Os hal dependencies */
#include "Os_Hal_Compiler.h"
#include "Os_Hal_Trap.h"


/***********************************************************************************************************************
 *  LOCAL CONSTANT MACROS
 **********************************************************************************************************************/

/***********************************************************************************************************************
 *  LOCAL FUNCTION MACROS
 **********************************************************************************************************************/

/***********************************************************************************************************************
 *  LOCAL DATA TYPES AND STRUCTURES
 **********************************************************************************************************************/
/*! Constant converter for legacy implementation of peripheral API. */
typedef union
{                                                                                                                       /* PRQA S 0750 */ /* MD_MSR_Union */
  /* Address type of legacy API. */
  uint32 AddressInt;
  /* Uint8 address pointer of type of current API. */
  const   uint8 OS_APPL_DATA * AddressPtr2Uint8;
  /* Uint16 address pointer of type of current API. */
  const   uint16 OS_APPL_DATA * AddressPtr2Uint16;
  /* Uint32 address pointer of type of current API. */
  const   uint32 OS_APPL_DATA * AddressPtr2Uint32;
}Os_PeripheralConstInt2AddressType;


/*! Converter for legacy implementation of peripheral API. */
typedef union
{                                                                                                                       /* PRQA S 0750 */ /* MD_MSR_Union */
  /* Address type of legacy API. */
  uint32 AddressInt;
  /* Uint8 address pointer of type of current API. */
    uint8 OS_APPL_DATA * AddressPtr2Uint8;
  /* Uint16 address pointer of type of current API. */
    uint16 OS_APPL_DATA * AddressPtr2Uint16;
  /* Uint16 address pointer of type of current API. */
    uint32 OS_APPL_DATA * AddressPtr2Uint32;
}Os_PeripheralInt2AddressType;

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
 *  Os_TrapIsTrapForWriteAccessRequired()
 **********************************************************************************************************************/
/*! \brief          Returns whether caller of an API has to trap for write access.
 *  \details        Performing a trap is typically very time consuming. For this reason the OS tries to prevent it.
 *                  If the current thread is already privileged, the OS does not have to trap. On some platforms this
 *                  optimization is not possible, because the hardware does not allow to efficiently check whether the
 *                  current thread is privileged. In this case, UserMode is assumed as soon as MemoryProtection is
 *                  enabled.
 *
 *  \retval         !0  Trap is required, caller thread does not have write privileges.
 *  \retval         0   Trap is not required, caller thread has write privileges.
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre            -
 **********************************************************************************************************************/
OS_ALWAYS_INLINE 
static inline Os_StdReturnType   Os_TrapIsTrapForWriteAccessRequired (
 void
);


/***********************************************************************************************************************
 *  Os_TrapIsTrapForReadAccessRequired()
 **********************************************************************************************************************/
/*! \brief          Returns whether caller of an API has to trap for read access.
 *  \details        Performing a trap is typically very time consuming. For this reason the OS tries to prevent it.
 *                  If the current thread has read privileges, the OS does not have to trap. On some platforms this
 *                  optimization is not possible, because the hardware does not allow to efficiently check whether the
 *                  current thread has read privileges. In this case, UserMode is assumed as soon as MemoryProtection
 *                  is enabled.
 *
 *  \retval         !0  Trap is required, caller thread does not have read privileges.
 *  \retval         0   Trap is not required, caller thread has read privileges.
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre            -
 **********************************************************************************************************************/
OS_ALWAYS_INLINE 
static inline Os_StdReturnType   Os_TrapIsTrapForReadAccessRequired (
 void
);


/***********************************************************************************************************************
 *  Os_TrapIsCurrentThreadPrivileged()
 **********************************************************************************************************************/
/*! \brief          Returns whether the current thread is privileged (!0) or not (0).
 *  \details        Reads the current thread pointer and check the corresponding App's privileges
 *
 *  \retval         !0  The current thread is privileged.
 *  \retval         0   The current thread is not privileged.
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre            Supervisor mode active, or thread pointer is readable in user mode
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StdReturnType   Os_TrapIsCurrentThreadPrivileged (void);


/***********************************************************************************************************************
 *  Os_TrapIsTrapRequired()
 **********************************************************************************************************************/
/*! \brief          Returns whether access to certain privileges requires a trap.
 *  \details        Used when it is partly known whether certain privileges are required.
 *                  - If the ThreadPointer is readable in user mode, known information can be used
 *                  - Otherwise, decision is made based on current privileges if possible
 *
 *  \param[in]      IsTrapRequired  Defines whether supervisor mode is required to access required privileges:
 *                                   - TRUE:  Trap shall be forced even if the thread pointer can be read in user mode
 *                                            (unless supervisor mode is already active).
 *                                   - FALSE: Trap shall be performed, if system state (i.e. ThreadPointer) can only be
 *                                            read after a trap.
 *
 *  \retval         !0  A trap is required to access the ThreadPointer or other privileges.
 *  \retval         0   No trap is required, the ThreadPointer can be accessed directly and no additional privileges
 *                      are needed.
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre            -
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StdReturnType   Os_TrapIsTrapRequired (
  boolean IsTrapRequired
);


/***********************************************************************************************************************
 *  Os_TrapCallCheckTaskMemoryAccess()
 **********************************************************************************************************************/
/*! \brief        OS service \ref CheckTaskMemoryAccess().
 *  \details      This function informs the trace module on entry and exit of the API CheckTaskMemoryAccess().
 *                For further details see \ref Os_Api_CheckTaskMemoryAccess().
 *
 *  \param[in]    TaskID    See \ref Os_Api_CheckTaskMemoryAccess().
 *  \param[in]    Address   See \ref Os_Api_CheckTaskMemoryAccess().
 *  \param[in]    Size      See \ref Os_Api_CheckTaskMemoryAccess().
 *
 *  \return       See \ref Os_Api_CheckTaskMemoryAccess().
 *
 *  \context      OS_INTERNAL
 *
 *  \reentrant    TRUE
 *  \synchronous  TRUE
 *
 *  \pre          See \ref Os_Api_CheckTaskMemoryAccess().
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline AccessType   Os_TrapCallCheckTaskMemoryAccess (
  TaskType TaskID,
  MemoryStartAddressType Address,
  MemorySizeType Size
);


/***********************************************************************************************************************
 *  Os_TrapCallCheckISRMemoryAccess()
 **********************************************************************************************************************/
/*! \brief          OS service \ref CheckISRMemoryAccess().
 *  \details        This function informs the trace module on entry and exit of the API CheckISRMemoryAccess().
 *                  For further details see \ref Os_Api_CheckISRMemoryAccess().
 *
 *  \param[in]      ISRID    See \ref Os_Api_CheckISRMemoryAccess().
 *  \param[in]      Address  See \ref Os_Api_CheckISRMemoryAccess().
 *  \param[in]      Size     See \ref Os_Api_CheckISRMemoryAccess().
 *
 *  \return         See \ref Os_Api_CheckISRMemoryAccess().
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre             See \ref Os_Api_CheckISRMemoryAccess().
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline AccessType   Os_TrapCallCheckISRMemoryAccess (
  ISRType ISRID,
  MemoryStartAddressType Address,
  MemorySizeType Size
);


/***********************************************************************************************************************
 *  Os_TrapCallGetAlarmBase()
 **********************************************************************************************************************/
/*! \brief          OS service \ref GetAlarmBase().
 *  \details        This function informs the trace module on entry and exit of the API GetAlarmBase().
 *                  For further details see \ref Os_Api_GetAlarmBase().
 *
 *  \param[in]      AlarmID See \ref Os_Api_GetAlarmBase().
 *  \param[in]      Info    See \ref Os_Api_GetAlarmBase().
 *
 *  \return         See \ref Os_Api_GetAlarmBase().
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre             See \ref Os_Api_GetAlarmBase().
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallGetAlarmBase (
  AlarmType AlarmID,
  AlarmBaseRefType Info
);


/***********************************************************************************************************************
 *  Os_TrapCallGetAlarm()
 **********************************************************************************************************************/
/*! \brief          OS service \ref GetAlarm().
 *  \details        This function informs the trace module on entry and exit of the API GetAlarm().
 *                  For further details see \ref Os_Api_GetAlarm().
 *
 *  \param[in]      AlarmID See \ref Os_Api_GetAlarm().
 *  \param[in]      Tick    See \ref Os_Api_GetAlarm().
 *
 *  \return         See \ref Os_Api_GetAlarm().
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre             See \ref Os_Api_GetAlarm().
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallGetAlarm (
  AlarmType AlarmID,
  TickRefType Tick
);


/***********************************************************************************************************************
 *  Os_TrapCallSetRelAlarm()
 **********************************************************************************************************************/
/*! \brief          OS service \ref SetRelAlarm().
 *  \details        This function informs the trace module on entry and exit of the API SetRelAlarm().
 *                  For further details see \ref Os_Api_SetRelAlarm().
 *
 *  \param[in]      AlarmID   See \ref Os_Api_SetRelAlarm().
 *  \param[in]      Increment See \ref Os_Api_SetRelAlarm().
 *  \param[in]      Cycle     See \ref Os_Api_SetRelAlarm().
 *
 *  \return         See \ref Os_Api_SetRelAlarm().
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre             See \ref Os_Api_SetRelAlarm().
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallSetRelAlarm (
  AlarmType AlarmID,
  TickType Increment,
  TickType Cycle
);


/***********************************************************************************************************************
 *  Os_TrapCallSetAbsAlarm()
 **********************************************************************************************************************/
/*! \brief          OS service \ref SetAbsAlarm().
 *  \details        This function informs the trace module on entry and exit of the API SetAbsAlarm().
 *                  For further details see \ref Os_Api_SetAbsAlarm().
 *
 *  \param[in]      AlarmID  See \ref Os_Api_SetAbsAlarm().
 *  \param[in]      Start    See \ref Os_Api_SetAbsAlarm().
 *  \param[in]      Cycle    See \ref Os_Api_SetAbsAlarm().
 *
 *  \return         See \ref Os_Api_SetAbsAlarm().
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre             See \ref Os_Api_SetAbsAlarm().
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallSetAbsAlarm (
  AlarmType AlarmID,
  TickType Start,
  TickType Cycle
);


/***********************************************************************************************************************
 *  Os_TrapCallCancelAlarm()
 **********************************************************************************************************************/
/*! \brief          OS service \ref CancelAlarm().
 *  \details        This function informs the trace module on entry and exit of the API CancelAlarm().
 *                  For further details see \ref Os_Api_CancelAlarm().
 *
 *  \param[in]      AlarmID    See \ref Os_Api_CancelAlarm().
 *
 *  \return         See \ref Os_Api_CancelAlarm().
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre             See \ref Os_Api_CancelAlarm().
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallCancelAlarm (
  AlarmType AlarmID
);


/***********************************************************************************************************************
 *  Os_TrapCallGetApplicationID()
 **********************************************************************************************************************/
/*! \brief          OS service \ref GetApplicationID().
 *  \details        This function informs the trace module on entry and exit of the API GetApplicationID().
 *                  For further details see \ref Os_Api_GetApplicationID().
 *
 *  \return         See \ref Os_Api_GetApplicationID().
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre             See \ref Os_Api_GetApplicationID().
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline ApplicationType   Os_TrapCallGetApplicationID (
  void
);


/***********************************************************************************************************************
 *  Os_TrapCallGetCurrentApplicationID()
 **********************************************************************************************************************/
/*! \brief          OS service \ref GetCurrentApplicationID().
 *  \details        This function informs the trace module on entry and exit of the API GetCurrentApplicationID().
 *                  For further details see \ref Os_Api_GetCurrentApplicationID().
 *
 *  \return         See \ref Os_Api_GetCurrentApplicationID().
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre             See \ref Os_Api_GetCurrentApplicationID().
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline ApplicationType   Os_TrapCallGetCurrentApplicationID (
  void
);


/***********************************************************************************************************************
 *  Os_TrapCallGetApplicationState()
 **********************************************************************************************************************/
/*! \brief          OS service \ref GetApplicationState().
 *  \details        This function informs the trace module on entry and exit of the API GetApplicationState().
 *                  For further details see \ref Os_Api_GetApplicationState().
 *
 *  \param[in]      Application See \ref Os_Api_GetApplicationState().
 *  \param[out]     Value       See \ref Os_Api_GetApplicationState().
 *
 *  \return         See \ref Os_Api_GetApplicationState().
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre             See \ref Os_Api_GetApplicationState().
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallGetApplicationState (
  ApplicationType Application,
  ApplicationStateRefType Value
);


/***********************************************************************************************************************
 *  Os_TrapCallCheckObjectAccess()
 **********************************************************************************************************************/
/*! \brief          OS service \ref CheckObjectAccess().
 *  \details        This function informs the trace module on entry and exit of the API CheckObjectAccess().
 *                  For further details see \ref Os_Api_CheckObjectAccess().
 *
 *  \param[in]      ApplID      See \ref Os_Api_CheckObjectAccess().
 *  \param[in]      ObjectType  See \ref Os_Api_CheckObjectAccess().
 *  \param[in]      ObjectID    See \ref Os_Api_CheckObjectAccess().
 *
 *  \return         See \ref Os_Api_CheckObjectAccess().
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre             See \ref Os_Api_CheckObjectAccess().
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline ObjectAccessType   Os_TrapCallCheckObjectAccess (
  ApplicationType ApplID,
  ObjectTypeType ObjectType,
  Os_ObjectIdType ObjectID
);


/***********************************************************************************************************************
 *  Os_TrapCallCheckObjectOwnership()
 **********************************************************************************************************************/
/*! \brief          OS service \ref CheckObjectOwnership().
 *  \details        This function informs the trace module on entry and exit of the API CheckObjectOwnership().
 *                  For further details see \ref Os_Api_CheckObjectOwnership().
 *
 *  \param[in]      ObjectType  See \ref Os_Api_CheckObjectOwnership().
 *  \param[in]      ObjectID    See \ref Os_Api_CheckObjectOwnership().
 *
 *  \return         See \ref Os_Api_CheckObjectOwnership().
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre             See \ref Os_Api_CheckObjectOwnership().
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline ApplicationType   Os_TrapCallCheckObjectOwnership (
  ObjectTypeType ObjectType,
  Os_ObjectIdType ObjectID
);


/***********************************************************************************************************************
 *  Os_TrapCallAllowAccess()
 **********************************************************************************************************************/
/*! \brief          OS service \ref AllowAccess().
 *  \details        This function informs the trace module on entry and exit of the API AllowAccess().
 *                  For further details see \ref Os_Api_AllowAccess().
 *
 *  \return         See \ref Os_Api_AllowAccess().
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre             See \ref Os_Api_AllowAccess().
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallAllowAccess (
  void
);


/***********************************************************************************************************************
 *  Os_TrapCallTerminateApplication()
 **********************************************************************************************************************/
/*! \brief          OS service \ref TerminateApplication().
 *  \details        This function informs the trace module on entry and exit of the API TerminateApplication().
 *                  For further details see \ref Os_Api_TerminateApplication().
 *
 *  \param[in]      Application    See \ref Os_Api_TerminateApplication().
 *  \param[in]      RestartOption  See \ref Os_Api_TerminateApplication().
 *
 *  \return         See \ref Os_Api_TerminateApplication().
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre             See \ref Os_Api_TerminateApplication().
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallTerminateApplication (
  ApplicationType Application,
  RestartType RestartOption
);


/***********************************************************************************************************************
 *  Os_TrapCallGetVersionInfo()
 **********************************************************************************************************************/
/*! \brief          OS service \ref Os_GetVersionInfo().
 *  \details        This function informs the trace module on entry and exit of the API Os_GetVersionInfo().
 *                  For further details see \ref Os_Api_GetVersionInfo().
 *
 *  \param[out]     versioninfo    See \ref Os_Api_GetVersionInfo().
 *
 *  \return         See \ref Os_Api_GetVersionInfo().
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre             See \ref Os_Api_GetVersionInfo().
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline void   Os_TrapCallGetVersionInfo (
  Std_VersionInfoType *versioninfo
);


/***********************************************************************************************************************
 *  Os_TrapCallGetExceptionContext()
 **********************************************************************************************************************/
/*! \brief          OS service \ref Os_GetExceptionContext().
 *  \details        This function informs the trace module on entry and exit of the API Os_GetExceptionContext().
 *                  For further details see \ref Os_Api_GetExceptionContext().
 *
 *  \param[out]     Context    See \ref Os_Api_GetExceptionContext().
 *
 *  \return         See \ref Os_Api_GetExceptionContext().
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre             See \ref Os_Api_GetExceptionContext().
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallGetExceptionContext (
  Os_ExceptionContextRefType Context
);


/***********************************************************************************************************************
 *  Os_TrapCallSetExceptionContext()
 **********************************************************************************************************************/
/*! \brief          OS service \ref Os_SetExceptionContext().
 *  \details        This function informs the trace module on entry and exit of the API Os_SetExceptionContext().
 *                  For further details see \ref Os_Api_SetExceptionContext().
 *
 *  \param[in]      Context    See \ref Os_Api_SetExceptionContext().
 *
 *  \return         See \ref Os_Api_SetExceptionContext().
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre             See \ref Os_Api_SetExceptionContext().
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallSetExceptionContext (
  const   Os_ExceptionContextType OS_CONST * Context
);


/***********************************************************************************************************************
 *  Os_TrapCallGetExceptionAddress()
 **********************************************************************************************************************/
/*! \brief          OS service \ref Os_GetExceptionAddress().
 *  \details        This function informs the trace module on entry and exit of the API Os_GetExceptionAddress().
 *                  For further details see \ref Os_Api_GetExceptionAddress().

 *  \return         See \ref Os_Api_GetExceptionAddress().
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre             See \ref Os_Api_GetExceptionAddress().
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_AddressOfConstType   Os_TrapCallGetExceptionAddress (
  void
);


/***********************************************************************************************************************
 *  Os_TrapCallInitMemory()
 **********************************************************************************************************************/
/*! \brief          OS service \ref Os_InitMemory().
 *  \details        For further details see \ref Os_Api_InitMemory().
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      FALSE
 *  \synchronous    TRUE
 *
 *  \pre             See \ref Os_Api_InitMemory().
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline void   Os_TrapCallInitMemory (
  void
);


/***********************************************************************************************************************
 *  Os_TrapCallInit()
 **********************************************************************************************************************/
/*! \brief          OS service \ref Os_Init().
 *  \details        For further details see \ref Os_Api_Init().
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      FALSE
 *  \synchronous    TRUE
 *
 *  \pre             See \ref Os_Api_Init().
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline void   Os_TrapCallInit (
  void
);


/***********************************************************************************************************************
 *  Os_TrapCallInitInterruptOnly()
 **********************************************************************************************************************/
/*! \brief          OS service \ref Os_InitInterruptOnly().
 *  \details        For further details see \ref Os_Api_InitInterruptOnly().
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      FALSE
 *  \synchronous    TRUE
 *
 *  \pre             See \ref Os_Api_InitInterruptOnly().
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline void   Os_TrapCallInitInterruptOnly (
  void
);


/***********************************************************************************************************************
 *  Os_TrapCallEnterPreStartTask()
 **********************************************************************************************************************/
/*! \brief          OS service \ref Os_EnterPreStartTask().
 *  \details        This function informs the trace module on entry and exit of the API Os_EnterPreStartTask().
 *                  For further details see \ref Os_Api_EnterPreStartTask().
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      FALSE
 *  \synchronous    TRUE
 *
 *  \pre             See \ref Os_Api_EnterPreStartTask().
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline void   Os_TrapCallEnterPreStartTask (
  void
);


/***********************************************************************************************************************
 *  Os_TrapCallStartCore()
 **********************************************************************************************************************/
/*! \brief          OS service \ref StartCore().
 *  \details        This function informs the trace module on entry and exit of the API StartCore().
 *                  For further details see \ref Os_Api_StartCore().
 *
 *  \param[in]      CoreID    See \ref Os_Api_StartCore().
 *  \param[in]      Status    See \ref Os_Api_StartCore().
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      FALSE
 *  \synchronous    TRUE
 *
 *  \pre             See \ref Os_Api_StartCore().
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline void   Os_TrapCallStartCore (
  CoreIdType CoreID,
  StatusType *Status
);


/***********************************************************************************************************************
 *  Os_TrapCallStartNonAutosarCore()
 **********************************************************************************************************************/
/*! \brief          OS service \ref StartNonAutosarCore().
 *  \details        This function informs the trace module on entry and exit of the API StartNonAutosarCore().
 *                  For further details see \ref Os_Api_StartNonAutosarCore().
 *
 *  \param[in]      CoreID    See \ref Os_Api_StartNonAutosarCore().
 *  \param[in]      Status    See \ref Os_Api_StartNonAutosarCore().
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      FALSE
 *  \synchronous    TRUE
 *
 *  \pre             See \ref Os_Api_StartNonAutosarCore().
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline void   Os_TrapCallStartNonAutosarCore (
  CoreIdType CoreID,
  StatusType *Status
);


/***********************************************************************************************************************
 *  Os_TrapCallGetCoreID()
 **********************************************************************************************************************/
/*! \brief          OS service \ref GetCoreID().
 *  \details        This function informs the trace module on entry and exit of the API GetCoreID().
 *                  For further details see \ref Os_Api_GetCoreID().
 *
 *  \return         See \ref Os_Api_GetCoreID().
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre             See \ref Os_Api_GetCoreID().
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline CoreIdType   Os_TrapCallGetCoreID (
  void
);


/***********************************************************************************************************************
 *  Os_TrapCallGetNumberOfActivatedCores()
 **********************************************************************************************************************/
/*! \brief          OS service \ref GetNumberOfActivatedCores().
 *  \details        This function informs the trace module on entry and exit of the API GetNumberOfActivatedCores().
 *                  For further details see \ref Os_Api_GetNumberOfActivatedCores().
 *
 *  \return         See \ref Os_Api_GetNumberOfActivatedCores().
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre             See \ref Os_Api_GetNumberOfActivatedCores().
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline uint32   Os_TrapCallGetNumberOfActivatedCores (
  void
);


/***********************************************************************************************************************
 *  Os_TrapCallGetActiveApplicationMode()
 **********************************************************************************************************************/
/*! \brief          OS service \ref GetActiveApplicationMode().
 *  \details        This function informs the trace module on entry and exit of the API GetActiveApplicationMode().
 *                  For further details see \ref Os_Api_GetActiveApplicationMode().
 *
 *  \return         See \ref Os_Api_GetActiveApplicationMode().
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre             See \ref Os_Api_GetActiveApplicationMode().
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline AppModeType   Os_TrapCallGetActiveApplicationMode (
  void
);


/***********************************************************************************************************************
 *  Os_TrapCallStartOS()
 **********************************************************************************************************************/
/*! \brief          OS service \ref StartOS().
 *  \details        This function informs the trace module on entry and exit of the API StartOS().
 *                  For further details see \ref Os_Api_StartOS().
 *
 *  \param[in]      see \ref Os_Api_StartOS().
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre             See \ref Os_Api_StartOS().
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline void   Os_TrapCallStartOS (
  AppModeType Mode
);


/***********************************************************************************************************************
 *  Os_TrapCallShutdownOS()
 **********************************************************************************************************************/
/*! \brief          OS service \ref ShutdownOS().
 *  \details        This function informs the trace module on entry and exit of the API ShutdownOS().
 *                  For further details see \ref Os_Api_ShutdownOS().
 *
 *  \param[in]      Error    See \ref Os_Api_ShutdownOS().
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre             See \ref Os_Api_ShutdownOS().
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline void   Os_TrapCallShutdownOS (
  StatusType Error
);


/***********************************************************************************************************************
 *  Os_TrapCallShutdownAllCores()
 **********************************************************************************************************************/
/*! \brief          OS service \ref ShutdownAllCores().
 *  \details        This function informs the trace module on entry and exit of the API ShutdownAllCores().
 *                  For further details see \ref Os_Api_ShutdownAllCores().
 *
 *  \param[in]      Error    See \ref Os_Api_ShutdownAllCores().
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre             See \ref Os_Api_ShutdownAllCores().
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline void   Os_TrapCallShutdownAllCores (
  StatusType Error
);


/***********************************************************************************************************************
 *  Os_TrapCallControlIdle()
 **********************************************************************************************************************/
/*! \brief          OS service \ref ControlIdle().
 *  \details        This function informs the trace module on entry and exit of the API ControlIdle().
 *                  For further details see \ref Os_Api_ControlIdle().
 *
 *  \param[in]      CoreID    See \ref Os_Api_ControlIdle().
 *  \param[in]      IdleMode  See \ref Os_Api_ControlIdle().
 *
 *  \return         See \ref Os_Api_ControlIdle().
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre             See \ref Os_Api_ControlIdle().
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallControlIdle (
  CoreIdType CoreID,
  IdleModeType IdleMode
);


/***********************************************************************************************************************
 *  Os_TrapCallGetUnhandledIrq()
 **********************************************************************************************************************/
/*! \brief          OS service \ref Os_GetUnhandledIrq().
 *  \details        This function informs the trace module on entry and exit of the API Os_GetUnhandledIrq().
 *                  For further details see \ref Os_Api_GetUnhandledIrq().
 *
 *  \param[out]     InterruptSource    See \ref Os_Api_GetUnhandledIrq().
 *
 *  \return         See \ref Os_Api_GetUnhandledIrq().
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre             See \ref Os_Api_GetUnhandledIrq().
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallGetUnhandledIrq (
  Os_InterruptSourceIdRefType InterruptSource
);


/***********************************************************************************************************************
 *  Os_TrapCallGetUnhandledExc()
 **********************************************************************************************************************/
/*! \brief          OS service \ref Os_GetUnhandledExc().
 *  \details        This function informs the trace module on entry and exit of the API Os_GetUnhandledExc().
 *                  For further details see \ref Os_Api_GetUnhandledExc().
 *
 *  \param[out]     ExceptionSource    See \ref Os_Api_GetUnhandledExc().
 *
 *  \return         See \ref Os_Api_GetUnhandledExc().
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre             See \ref Os_Api_GetUnhandledExc().
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallGetUnhandledExc (
  Os_ExceptionSourceIdRefType ExceptionSource
);


/***********************************************************************************************************************
 *  Os_TrapCallIncrementCounter()
 **********************************************************************************************************************/
/*! \brief          OS service \ref IncrementCounter().
 *  \details        This function informs the trace module on entry and exit of the API IncrementCounter().
 *                  For further details see \ref Os_Api_IncrementCounter().
 *
 *  \param[in]      CounterID    See \ref Os_Api_IncrementCounter().
 *
 *  \return         See \ref Os_Api_IncrementCounter().
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre             See \ref Os_Api_IncrementCounter().
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallIncrementCounter (
  CounterType CounterID
);


/***********************************************************************************************************************
 *  Os_TrapCallGetCounterValue()
 **********************************************************************************************************************/
/*! \brief          OS service \ref GetCounterValue().
 *  \details        This function informs the trace module on entry and exit of the API GetCounterValue().
 *                  For further details see \ref Os_Api_GetCounterValue().
 *
 *  \param[in]      CounterID See \ref Os_Api_GetCounterValue().
 *  \param[out]     Value     See \ref Os_Api_GetCounterValue().
 *
 *  \return         See \ref Os_Api_GetCounterValue().
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre             See \ref Os_Api_GetCounterValue().
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallGetCounterValue (
  CounterType CounterID,
  TickRefType Value
);


/***********************************************************************************************************************
 *  Os_TrapCallGetElapsedValue()
 **********************************************************************************************************************/
/*! \brief          OS service \ref GetElapsedValue().
 *  \details        This function informs the trace module on entry and exit of the API GetElapsedValue().
 *                  For further details see \ref Os_Api_GetElapsedValue().
 *
 *  \param[in]      CounterID    See \ref Os_Api_GetElapsedValue().
 *  \param[in,out]  Value        See \ref Os_Api_GetElapsedValue().
 *  \param[out]     ElapsedValue See \ref Os_Api_GetElapsedValue().
 *
 *  \return         See \ref Os_Api_GetElapsedValue().
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre             See \ref Os_Api_GetElapsedValue().
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallGetElapsedValue (
  CounterType CounterID,
  TickRefType Value,
  TickRefType ElapsedValue
);


/***********************************************************************************************************************
 *  Os_TrapCallGetDetailedError()
 **********************************************************************************************************************/
/*! \brief          OS service \ref Os_GetDetailedError().
 *  \details        This function informs the trace module on entry and exit of the API Os_GetDetailedError().
 *                  For further details see \ref Os_Api_GetDetailedError().
 *
 *  \param[out]     ErrorRef    See \ref Os_Api_GetDetailedError().
 *
 *  \return         See \ref Os_Api_GetDetailedError().
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre             See \ref Os_Api_GetDetailedError().
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallGetDetailedError (
  Os_ErrorInformationRefType ErrorRef
);


/***********************************************************************************************************************
 *  Os_TrapCallOSErrorGetServiceId()
 **********************************************************************************************************************/
/*! \brief          OS service \ref OSErrorGetServiceId().
 *  \details        This function informs the trace module on entry and exit of the API OSErrorGetServiceId().
 *                  For further details see \ref Os_Api_OSErrorGetServiceId().
 *
 *  \return         See \ref Os_Api_OSErrorGetServiceId().
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre             See \ref Os_Api_OSErrorGetServiceId().
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline OSServiceIdType   Os_TrapCallOSErrorGetServiceId (
  void
);


/***********************************************************************************************************************
 *  Os_TrapCallSetEvent()
 **********************************************************************************************************************/
/*! \brief          OS service \ref SetEvent().
 *  \details        This function informs the trace module on entry and exit of the API SetEvent().
 *                  For further details see \ref Os_Api_SetEvent().
 *
 *  \param[in]      TaskID See \ref Os_Api_SetEvent().
 *  \param[in]      Mask   See \ref Os_Api_SetEvent().
 *
 *  \return         See \ref Os_Api_SetEvent().
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre             See \ref Os_Api_SetEvent().
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallSetEvent (
  TaskType TaskID,
  EventMaskType Mask
);


/***********************************************************************************************************************
 *  Os_TrapCallClearEvent()
 **********************************************************************************************************************/
/*! \brief          OS service \ref ClearEvent().
 *  \details        This function informs the trace module on entry and exit of the API ClearEvent().
 *                  For further details see \ref Os_Api_ClearEvent().
 *
 *  \param[in]      Mask    See \ref Os_Api_ClearEvent().
 *
 *  \return         See \ref Os_Api_ClearEvent().
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre             See \ref Os_Api_ClearEvent().
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallClearEvent (
  EventMaskType Mask
);


/***********************************************************************************************************************
 *  Os_TrapCallGetEvent()
 **********************************************************************************************************************/
/*! \brief          OS service \ref GetEvent().
 *  \details        This function informs the trace module on entry and exit of the API GetEvent().
 *                  For further details see \ref Os_Api_GetEvent().
 *
 *  \param[in]      TaskID    See \ref Os_Api_GetEvent().
 *  \param[out]     Mask      See \ref Os_Api_GetEvent().
 *
 *  \return         See \ref Os_Api_GetEvent().
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre             See \ref Os_Api_GetEvent().
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallGetEvent (
  TaskType TaskID,
  EventMaskRefType Mask
);


/***********************************************************************************************************************
 *  Os_TrapCallWaitEvent()
 **********************************************************************************************************************/
/*! \brief          OS service \ref WaitEvent().
 *  \details        This function informs the trace module on entry and exit of the API WaitEvent().
 *                  For further details see \ref Os_Api_WaitEvent().
 *
 *  \param[in]      Mask    See \ref Os_Api_WaitEvent().
 *
 *  \return         See \ref Os_Api_WaitEvent().
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre             See \ref Os_Api_WaitEvent().
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallWaitEvent (
  EventMaskType Mask
);


/***********************************************************************************************************************
 *  Os_TrapCallDisableAllInterrupts()
 **********************************************************************************************************************/
/*! \brief          OS service \ref DisableAllInterrupts().
 *  \details        This function informs the trace module on entry and exit of the API DisableAllInterrupts().
 *                  For further details see \ref Os_Api_DisableAllInterrupts().
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre             See \ref Os_Api_DisableAllInterrupts().
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline void   Os_TrapCallDisableAllInterrupts (
  void
);


/***********************************************************************************************************************
 *  Os_TrapCallEnableAllInterrupts()
 **********************************************************************************************************************/
/*! \brief          OS service \ref EnableAllInterrupts().
 *  \details        This function informs the trace module on entry and exit of the API EnableAllInterrupts().
 *                  For further details see \ref Os_Api_EnableAllInterrupts().
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre             See \ref Os_Api_EnableAllInterrupts().
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline void   Os_TrapCallEnableAllInterrupts (
  void
);


/***********************************************************************************************************************
 *  Os_TrapCallSuspendAllInterrupts()
 **********************************************************************************************************************/
/*! \brief          OS service \ref SuspendAllInterrupts().
 *  \details        This function informs the trace module on entry and exit of the API SuspendAllInterrupts().
 *                  For further details see \ref Os_Api_SuspendAllInterrupts().
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre             See \ref Os_Api_SuspendAllInterrupts().
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline void   Os_TrapCallSuspendAllInterrupts (
  void
);


/***********************************************************************************************************************
 *  Os_TrapCallResumeAllInterrupts()
 **********************************************************************************************************************/
/*! \brief          OS service \ref ResumeAllInterrupts().
 *  \details        This function informs the trace module on entry and exit of the API ResumeAllInterrupts().
 *                  For further details see \ref Os_Api_ResumeAllInterrupts().
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre             See \ref Os_Api_ResumeAllInterrupts().
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline void   Os_TrapCallResumeAllInterrupts (
  void
);


/***********************************************************************************************************************
 *  Os_TrapCallSuspendOSInterrupts()
 **********************************************************************************************************************/
/*! \brief          OS service \ref SuspendOSInterrupts().
 *  \details        This function informs the trace module on entry and exit of the API SuspendOSInterrupts().
 *                  For further details see \ref Os_Api_SuspendOSInterrupts().
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre            -
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline void   Os_TrapCallSuspendOSInterrupts (
  void
);


/***********************************************************************************************************************
 *  Os_TrapCallResumeOSInterrupts()
 **********************************************************************************************************************/
/*! \brief          OS service \ref ResumeOSInterrupts().
 *  \details        This function informs the trace module on entry and exit of the API ResumeOSInterrupts().
 *                  For further details see \ref Os_Api_ResumeOSInterrupts().
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre            -
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline void   Os_TrapCallResumeOSInterrupts (
  void
);


/***********************************************************************************************************************
 *  Os_TrapCallDisableGlobal()
 **********************************************************************************************************************/
/*! \brief          OS service \ref DisableGlobal().
 *  \details        This function informs the trace module on entry and exit of the API DisableGlobal().
 *                  For further details see \ref Os_Api_DisableGlobal().
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre             See \ref Os_Api_DisableGlobal().
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline void   Os_TrapCallDisableGlobal (
  void
);


/***********************************************************************************************************************
 *  Os_TrapCallEnableGlobal()
 **********************************************************************************************************************/
/*! \brief          OS service \ref EnableGlobal().
 *  \details        This function informs the trace module on entry and exit of the API EnableGlobal().
 *                  For further details see \ref Os_Api_EnableGlobal().
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre             See \ref Os_Api_EnableGlobal().
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline void   Os_TrapCallEnableGlobal (
  void
);


/***********************************************************************************************************************
 *  Os_TrapCallDisableLevel()
 **********************************************************************************************************************/
/*! \brief          OS service \ref DisableLevel().
 *  \details        This function informs the trace module on entry and exit of the API DisableLevel().
 *                  For further details see \ref Os_Api_DisableLevel().
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre             See \ref Os_Api_DisableLevel().
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline void   Os_TrapCallDisableLevel (
  void
);


/***********************************************************************************************************************
 *  Os_TrapCallEnableLevel()
 **********************************************************************************************************************/
/*! \brief          OS service \ref EnableLevel().
 *  \details        This function informs the trace module on entry and exit of the API EnableLevel().
 *                  For further details see \ref Os_Api_EnableLevel().
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre             See \ref Os_Api_EnableLevel().
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline void   Os_TrapCallEnableLevel (
  void
);


/***********************************************************************************************************************
 *  Os_TrapCallGetISRID()
 **********************************************************************************************************************/
/*! \brief          OS service \ref GetISRID().
 *  \details        This function informs the trace module on entry and exit of the API GetISRID().
 *                  For further details see \ref Os_Api_GetISRID().
 *
 *  \return         See \ref Os_Api_GetISRID().
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre             See \ref Os_Api_GetISRID().
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline ISRType   Os_TrapCallGetISRID (
  void
);


/***********************************************************************************************************************
 *  Os_TrapCallDisableInterruptSource()
 **********************************************************************************************************************/
/*! \brief          OS service \ref Os_DisableInterruptSource().
 *  \details        This function informs the trace module on entry and exit of the API Os_DisableInterruptSource().
 *                  For further details see \ref Os_Api_DisableInterruptSource().
 *
 *  \param[in]      ISRID    See \ref Os_Api_DisableInterruptSource().
 *
 *  \return         See \ref Os_Api_DisableInterruptSource().
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre             See \ref Os_Api_DisableInterruptSource().
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallDisableInterruptSource (
  ISRType ISRID
);


/***********************************************************************************************************************
 *  Os_TrapCallEnableInterruptSource()
 **********************************************************************************************************************/
/*! \brief          OS service \ref Os_EnableInterruptSource().
 *  \details        This function informs the trace module on entry and exit of the API Os_EnableInterruptSource().
 *                  For further details see \ref Os_Api_EnableInterruptSource().
 *
 *  \param[in]      ISRID        See \ref Os_Api_EnableInterruptSource().
 *  \param[in]      ClearPending See \ref Os_Api_EnableInterruptSource().
 *
 *  \return         See \ref Os_Api_EnableInterruptSource().
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre             See \ref Os_Api_EnableInterruptSource().
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallEnableInterruptSource (
  ISRType ISRID,
  boolean ClearPending
);

/***********************************************************************************************************************
 *  Os_TrapCallInitialEnableInterruptSources()
 **********************************************************************************************************************/
/*! \brief          OS service \ref Os_InitialEnableInterruptSources().
 *  \details        This function informs the trace module on entry and exit of the API
 *                  Os_InitialEnableInterruptSources().
 *                  For further details see \ref Os_Api_InitialEnableInterruptSources().
 *
 *  \param[in]      ClearPending See \ref Os_Api_InitialEnableInterruptSources().
 *
 *  \return         See \ref Os_Api_InitialEnableInterruptSources().
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre            See \ref Os_Api_InitialEnableInterruptSources().
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallInitialEnableInterruptSources (
  boolean ClearPending
);


/***********************************************************************************************************************
 *  Os_TrapCallClearPendingInterrupt()
 **********************************************************************************************************************/
/*! \brief          OS service \ref Os_ClearPendingInterrupt().
 *  \details        This function informs the trace module on entry and exit of the API Os_ClearPendingInterrupt().
 *                  For further details see \ref Os_Api_ClearPendingInterrupt().
 *
 *  \param[in]      ISRID    See \ref Os_Api_ClearPendingInterrupt().
 *
 *  \return         See \ref Os_Api_ClearPendingInterrupt().
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre             See \ref Os_Api_ClearPendingInterrupt().
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallClearPendingInterrupt (
  ISRType ISRID
);


/***********************************************************************************************************************
 *  Os_TrapCallIsInterruptSourceEnabled()
 **********************************************************************************************************************/
/*! \brief          OS service \ref Os_IsInterruptSourceEnabled().
 *  \details        This function informs the trace module on entry and exit of the API Os_IsInterruptSourceEnabled().
 *                  For further details see \ref Os_Api_IsInterruptSourceEnabled().
 *
 *  \param[in]      ISRID     See \ref Os_Api_IsInterruptSourceEnabled().
 *  \param[out]     IsEnabled See \ref Os_Api_IsInterruptSourceEnabled().
 *
 *  \return         See \ref Os_Api_IsInterruptSourceEnabled().
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre             See \ref Os_Api_IsInterruptSourceEnabled().
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallIsInterruptSourceEnabled (
  ISRType ISRID,
    boolean OS_VAR_NOINIT * IsEnabled
);


/***********************************************************************************************************************
 *  Os_TrapCallIsInterruptPending()
 **********************************************************************************************************************/
/*! \brief          OS service \ref Os_IsInterruptPending().
 *  \details        This function informs the trace module on entry and exit of the API Os_IsInterruptPending().
 *                  For further details see \ref Os_Api_IsInterruptPending().
 *
 *  \param[in]      ISRID     See \ref Os_Api_IsInterruptPending().
 *  \param[out]     IsPending See \ref Os_Api_IsInterruptPending().
 *
 *  \return         See \ref Os_Api_IsInterruptPending().
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre             See \ref Os_Api_IsInterruptPending().
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallIsInterruptPending (
  ISRType ISRID,
    boolean OS_VAR_NOINIT * IsPending
);


/***********************************************************************************************************************
 *  Os_TrapCallReadPeripheral8()
 **********************************************************************************************************************/
/*! \brief          OS service \ref Os_ReadPeripheral8().
 *  \details        This function informs the trace module on entry and exit of the API Os_ReadPeripheral8().
 *                  For further details see \ref Os_Api_ReadPeripheral8().
 *
 *  \param[in]      PeripheralID See \ref Os_Api_ReadPeripheral8().
 *  \param[in]      Address      See \ref Os_Api_ReadPeripheral8().
 *
 *  \return         See \ref Os_Api_ReadPeripheral8().
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre             See \ref Os_Api_ReadPeripheral8().
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline uint8   Os_TrapCallReadPeripheral8 (
  Os_PeripheralIdType PeripheralID,
  const   uint8 OS_APPL_DATA * Address
);


/***********************************************************************************************************************
 *  Os_TrapCallReadPeripheral16()
 **********************************************************************************************************************/
/*! \brief          OS service \ref Os_ReadPeripheral16().
 *  \details        This function informs the trace module on entry and exit of the API Os_ReadPeripheral16().
 *                  For further details see \ref Os_Api_ReadPeripheral16().
 *
 *  \param[in]      PeripheralID See \ref Os_Api_ReadPeripheral16().
 *  \param[in]      Address      See \ref Os_Api_ReadPeripheral16().
 *
 *  \return         See \ref Os_Api_ReadPeripheral16().
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre             See \ref Os_Api_ReadPeripheral16().
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline uint16   Os_TrapCallReadPeripheral16 (
  Os_PeripheralIdType PeripheralID,
  const   uint16 OS_APPL_DATA * Address
);


/***********************************************************************************************************************
 *  Os_TrapCallReadPeripheral32()
 **********************************************************************************************************************/
/*! \brief          OS service \ref Os_ReadPeripheral32().
 *  \details        This function informs the trace module on entry and exit of the API Os_ReadPeripheral32().
 *                  For further details see \ref Os_Api_ReadPeripheral32().
 *
 *  \param[in]      PeripheralID See \ref Os_Api_ReadPeripheral32().
 *  \param[in]      Address      See \ref Os_Api_ReadPeripheral32().
 *
 *  \return         See \ref Os_Api_ReadPeripheral32().
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre             See \ref Os_Api_ReadPeripheral32().
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline uint32   Os_TrapCallReadPeripheral32 (
  Os_PeripheralIdType PeripheralID,
  const   uint32 OS_APPL_DATA * Address
);


/***********************************************************************************************************************
 *  Os_TrapCallWritePeripheral8()
 **********************************************************************************************************************/
/*! \brief          OS service \ref Os_WritePeripheral8().
 *  \details        This function informs the trace module on entry and exit of the API Os_WritePeripheral8().
 *                  For further details see \ref Os_Api_WritePeripheral8().
 *
 *  \param[in]      PeripheralID See \ref Os_Api_WritePeripheral8().
 *  \param[in,out]  Address      See \ref Os_Api_WritePeripheral8().
 *  \param[in]      Value        See \ref Os_Api_WritePeripheral8().
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre             See \ref Os_Api_WritePeripheral8().
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline void   Os_TrapCallWritePeripheral8 (
  Os_PeripheralIdType PeripheralID,
    uint8 OS_APPL_DATA * Address,
  uint8 Value
);


/***********************************************************************************************************************
 *  Os_TrapCallWritePeripheral16()
 **********************************************************************************************************************/
/*! \brief          OS service \ref Os_WritePeripheral16().
 *  \details        This function informs the trace module on entry and exit of the API Os_WritePeripheral16().
 *                  For further details see \ref Os_Api_WritePeripheral16().
 *
 *  \param[in]      PeripheralID See \ref Os_Api_WritePeripheral16().
 *  \param[in,out]  Address      See \ref Os_Api_WritePeripheral16().
 *  \param[in]      Value        See \ref Os_Api_WritePeripheral16().
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre             See \ref Os_Api_WritePeripheral16().
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline void   Os_TrapCallWritePeripheral16 (
  Os_PeripheralIdType PeripheralID,
    uint16 OS_APPL_DATA * Address,
  uint16 Value
);


/***********************************************************************************************************************
 *  Os_TrapCallWritePeripheral32()
 **********************************************************************************************************************/
/*! \brief          OS service \ref Os_WritePeripheral32().
 *  \details        This function informs the trace module on entry and exit of the API Os_WritePeripheral32().
 *                  For further details see \ref Os_Api_WritePeripheral32().
 *
 *  \param[in]      PeripheralID See \ref Os_Api_WritePeripheral32().
 *  \param[in,out]  Address      See \ref Os_Api_WritePeripheral32().
 *  \param[in]      Value        See \ref Os_Api_WritePeripheral32().
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre             See \ref Os_Api_WritePeripheral32().
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline void   Os_TrapCallWritePeripheral32 (
  Os_PeripheralIdType PeripheralID,
    uint32 OS_APPL_DATA * Address,
  uint32 Value
);


/***********************************************************************************************************************
 *  Os_TrapCallModifyPeripheral8()
 **********************************************************************************************************************/
/*! \brief          OS service \ref Os_ModifyPeripheral8().
 *  \details        This function informs the trace module on entry and exit of the API Os_ModifyPeripheral8().
 *                  For further details see \ref Os_Api_ModifyPeripheral8().
 *
 *  \param[in]      PeripheralID See \ref Os_Api_ModifyPeripheral8().
 *  \param[in,out]  Address      See \ref Os_Api_ModifyPeripheral8().
 *  \param[in]      ClearMask    See \ref Os_Api_ModifyPeripheral8().
 *  \param[in]      SetMask      See \ref Os_Api_ModifyPeripheral8().
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre             See \ref Os_Api_ModifyPeripheral8().
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline void   Os_TrapCallModifyPeripheral8 (
  Os_PeripheralIdType PeripheralID,
    uint8 OS_APPL_DATA * Address,
  uint8 ClearMask,
  uint8 SetMask
);


/***********************************************************************************************************************
 *  Os_TrapCallModifyPeripheral16()
 **********************************************************************************************************************/
/*! \brief          OS service \ref Os_ModifyPeripheral16().
 *  \details        This function informs the trace module on entry and exit of the API Os_ModifyPeripheral16().
 *                  For further details see \ref Os_Api_ModifyPeripheral16().
 *
 *  \param[in]      PeripheralID See \ref Os_Api_ModifyPeripheral16().
 *  \param[in,out]  Address      See \ref Os_Api_ModifyPeripheral16().
 *  \param[in]      ClearMask    See \ref Os_Api_ModifyPeripheral16().
 *  \param[in]      SetMask      See \ref Os_Api_ModifyPeripheral16().
 *
 *  \return         See \ref Os_Api_ModifyPeripheral16().
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre             See \ref Os_Api_ModifyPeripheral16().
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline void   Os_TrapCallModifyPeripheral16 (
  Os_PeripheralIdType PeripheralID,
    uint16 OS_APPL_DATA * Address,
  uint16 ClearMask,
  uint16 SetMask
);


/***********************************************************************************************************************
 *  Os_TrapCallModifyPeripheral32()
 **********************************************************************************************************************/
/*! \brief          OS service \ref Os_ModifyPeripheral32().
 *  \details        This function informs the trace module on entry and exit of the API Os_ModifyPeripheral32().
 *                  For further details see \ref Os_Api_ModifyPeripheral32().
 *
 *  \param[in]      PeripheralID See \ref Os_Api_ModifyPeripheral32().
 *  \param[in,out]  Address      See \ref Os_Api_ModifyPeripheral32().
 *  \param[in]      ClearMask    See \ref Os_Api_ModifyPeripheral32().
 *  \param[in]      SetMask      See \ref Os_Api_ModifyPeripheral32().
 *
 *  \return         See \ref Os_Api_ModifyPeripheral32().
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre             See \ref Os_Api_ModifyPeripheral32().
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline void   Os_TrapCallModifyPeripheral32 (
  Os_PeripheralIdType PeripheralID,
    uint32 OS_APPL_DATA * Address,
  uint32 ClearMask,
  uint32 SetMask
);


/***********************************************************************************************************************
 *  Os_TrapCallGetResource()
 **********************************************************************************************************************/
/*! \brief          OS service \ref GetResource().
 *  \details        This function informs the trace module on entry and exit of the API GetResource().
 *                  For further details see \ref Os_Api_GetResource().
 *
 *  \param[in]      ResID    See \ref Os_Api_GetResource().
 *
 *  \return         See \ref Os_Api_GetResource().
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre             See \ref Os_Api_GetResource().
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallGetResource (
  ResourceType ResID
);


/***********************************************************************************************************************
 *  Os_TrapCallReleaseResource()
 **********************************************************************************************************************/
/*! \brief          OS service \ref ReleaseResource().
 *  \details        This function informs the trace module on entry and exit of the API ReleaseResource().
 *                  For further details see \ref Os_Api_ReleaseResource().
 *
 *  \param[in]      ResID    See \ref Os_Api_ReleaseResource().
 *
 *  \return         See \ref Os_Api_ReleaseResource().
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre             See \ref Os_Api_ReleaseResource().
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallReleaseResource (
  ResourceType ResID
);


/***********************************************************************************************************************
 *  Os_TrapCallStartScheduleTableRel()
 **********************************************************************************************************************/
/*! \brief          OS service \ref StartScheduleTableRel().
 *  \details        This function informs the trace module on entry and exit of the API StartScheduleTableRel().
 *                  For further details see \ref Os_Api_StartScheduleTableRel().
 *
 *  \param[in]      ScheduleTableID See \ref Os_Api_StartScheduleTableRel().
 *  \param[in]      Offset          See \ref Os_Api_StartScheduleTableRel().
 *
 *  \return         See \ref Os_Api_StartScheduleTableRel().
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre             See \ref Os_Api_StartScheduleTableRel().
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallStartScheduleTableRel (
  ScheduleTableType ScheduleTableID,
  TickType Offset
);


/***********************************************************************************************************************
 *  Os_TrapCallStartScheduleTableAbs()
 **********************************************************************************************************************/
/*! \brief          OS service \ref StartScheduleTableAbs().
 *  \details        This function informs the trace module on entry and exit of the API StartScheduleTableAbs().
 *                  For further details see \ref Os_Api_StartScheduleTableAbs().
 *
 *  \param[in]      ScheduleTableID See \ref Os_Api_StartScheduleTableAbs().
 *  \param[in]      Start           See \ref Os_Api_StartScheduleTableAbs().
 *
 *  \return         See \ref Os_Api_StartScheduleTableAbs().
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre             See \ref Os_Api_StartScheduleTableAbs().
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallStartScheduleTableAbs (
  ScheduleTableType ScheduleTableID,
  TickType Start
);


/***********************************************************************************************************************
 *  Os_TrapCallStopScheduleTable()
 **********************************************************************************************************************/
/*! \brief          OS service \ref StopScheduleTable().
 *  \details        This function informs the trace module on entry and exit of the API StopScheduleTable().
 *                  For further details see \ref Os_Api_StopScheduleTable().
 *
 *  \param[in]      ScheduleTableID    See \ref Os_Api_StopScheduleTable().
 *
 *  \return         See \ref Os_Api_StopScheduleTable().
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre             See \ref Os_Api_StopScheduleTable().
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallStopScheduleTable (
  ScheduleTableType ScheduleTableID
);


/***********************************************************************************************************************
 *  Os_TrapCallNextScheduleTable()
 **********************************************************************************************************************/
/*! \brief          OS service \ref NextScheduleTable().
 *  \details        This function informs the trace module on entry and exit of the API NextScheduleTable().
 *                  For further details see \ref Os_Api_NextScheduleTable().
 *
 *  \param[in]      ScheduleTableID_From See \ref Os_Api_NextScheduleTable().
 *  \param[in]      ScheduleTableID_To   See \ref Os_Api_NextScheduleTable().
 *
 *  \return         See \ref Os_Api_NextScheduleTable().
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre             See \ref Os_Api_NextScheduleTable().
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallNextScheduleTable (
  ScheduleTableType ScheduleTableID_From,
  ScheduleTableType ScheduleTableID_To
);


/***********************************************************************************************************************
 *  Os_TrapCallStartScheduleTableSynchron()
 **********************************************************************************************************************/
/*! \brief          OS service \ref StartScheduleTableSynchron().
 *  \details        This function informs the trace module on entry and exit of the API StartScheduleTableSynchron().
 *                  For further details see \ref Os_Api_StartScheduleTableSynchron().
 *
 *  \param[in]      ScheduleTableID    See \ref Os_Api_StartScheduleTableSynchron().
 *
 *  \return         See \ref Os_Api_StartScheduleTableSynchron().
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre             See \ref Os_Api_StartScheduleTableSynchron().
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallStartScheduleTableSynchron (
  ScheduleTableType ScheduleTableID
);


/***********************************************************************************************************************
 *  Os_TrapCallSyncScheduleTable()
 **********************************************************************************************************************/
/*! \brief          OS service \ref SyncScheduleTable().
 *  \details        This function informs the trace module on entry and exit of the API SyncScheduleTable().
 *                  For further details see \ref Os_Api_SyncScheduleTable().
 *
 *  \param[in]      ScheduleTableID See \ref Os_Api_SyncScheduleTable().
 *  \param[in]      Value           See \ref Os_Api_SyncScheduleTable().
  *
 *  \return         See \ref Os_Api_SyncScheduleTable().
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre             See \ref Os_Api_SyncScheduleTable().
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallSyncScheduleTable (
  ScheduleTableType ScheduleTableID,
  TickType Value
);


/***********************************************************************************************************************
 *  Os_TrapCallSetScheduleTableAsync()
 **********************************************************************************************************************/
/*! \brief          OS service \ref SetScheduleTableAsync().
 *  \details        This function informs the trace module on entry and exit of the API SetScheduleTableAsync().
 *                  For further details see \ref Os_Api_SetScheduleTableAsync().
 *
 *  \param[in]      ScheduleTableID    See \ref Os_Api_SetScheduleTableAsync().
 *
 *  \return         See \ref Os_Api_SetScheduleTableAsync().
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre             See \ref Os_Api_SetScheduleTableAsync().
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallSetScheduleTableAsync (
  ScheduleTableType ScheduleTableID
);


/***********************************************************************************************************************
 *  Os_TrapCallGetScheduleTableStatus()
 **********************************************************************************************************************/
/*! \brief          OS service \ref GetScheduleTableStatus().
 *  \details        This function informs the trace module on entry and exit of the API GetScheduleTableStatus().
 *                  For further details see \ref Os_Api_GetScheduleTableStatus().
 *
 *  \param[in]      ScheduleTableID    See \ref Os_Api_GetScheduleTableStatus().
 *  \param[out]     ScheduleStatus    See \ref Os_Api_GetScheduleTableStatus().
 *
 *  \return         See \ref Os_Api_GetScheduleTableStatus().
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre             See \ref Os_Api_GetScheduleTableStatus().
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallGetScheduleTableStatus (
  ScheduleTableType ScheduleTableID,
  ScheduleTableStatusRefType ScheduleStatus
);


/***********************************************************************************************************************
 *  Os_TrapCallCallTrustedFunction()
 **********************************************************************************************************************/
/*! \brief          OS service \ref CallTrustedFunction().
 *  \details        This function informs the trace module on entry and exit of the API CallTrustedFunction().
 *                  For further details see \ref Os_Api_CallTrustedFunction().
 *
 *  \param[in]      FunctionIndex    See \ref Os_Api_CallTrustedFunction().
 *  \param[in]      FunctionParams   See \ref Os_Api_CallTrustedFunction().
 *
 *  \return         See \ref Os_Api_CallTrustedFunction().
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre             See \ref Os_Api_CallTrustedFunction().
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallCallTrustedFunction (
  TrustedFunctionIndexType FunctionIndex,
  TrustedFunctionParameterRefType FunctionParams
);


/***********************************************************************************************************************
 *  Os_TrapCallCallNonTrustedFunction()
 **********************************************************************************************************************/
/*! \brief          OS service \ref Os_CallNonTrustedFunction().
 *  \details        This function informs the trace module on entry and exit of the API Os_CallNonTrustedFunction().
 *                  For further details see \ref Os_Api_CallNonTrustedFunction().
 *
 *  \param[in]      FunctionIndex    See \ref Os_Api_CallNonTrustedFunction().
 *  \param[in]      FunctionParams    See \ref Os_Api_CallNonTrustedFunction().
 *
 *  \return         See \ref Os_Api_CallNonTrustedFunction().
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre             See \ref Os_Api_CallNonTrustedFunction().
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallCallNonTrustedFunction (
  Os_NonTrustedFunctionIndexType FunctionIndex,
  Os_NonTrustedFunctionParameterRefType FunctionParams
);


/***********************************************************************************************************************
 *  Os_TrapCallCallFastTrustedFunction()
 **********************************************************************************************************************/
/*! \brief          OS service \ref Os_CallFastTrustedFunction().
 *  \details        This function informs the trace module on entry and exit of the API Os_CallFastTrustedFunction().
 *                  For further details see \ref Os_Api_CallFastTrustedFunction().
 *
 *  \param[in]      FunctionIndex    See \ref Os_Api_CallFastTrustedFunction().
 *  \param[in]      FunctionParams    See \ref Os_Api_CallFastTrustedFunction().
 *
 *  \return         See \ref Os_Api_CallFastTrustedFunction().
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre             See \ref Os_Api_CallFastTrustedFunction().
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallCallFastTrustedFunction (
  Os_FastTrustedFunctionIndexType FunctionIndex,
  Os_FastTrustedFunctionParameterRefType FunctionParams
);


/***********************************************************************************************************************
 *  Os_TrapCallGetSpinlock()
 **********************************************************************************************************************/
/*! \brief          OS service \ref GetSpinlock().
 *  \details        This function informs the trace module on entry and exit of the API GetSpinlock().
 *                  For further details see \ref Os_Api_GetSpinlock().
 *
 *  \param[in]      Spinlock    See \ref Os_Api_GetSpinlock().
 *
 *  \return         See \ref Os_Api_GetSpinlock().
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre             See \ref Os_Api_GetSpinlock().
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallGetSpinlock (
  const   Os_SpinlockConfigType OS_CONST * Spinlock
);


/***********************************************************************************************************************
 *  Os_TrapCallReleaseSpinlock()
 **********************************************************************************************************************/
/*! \brief          OS service \ref ReleaseSpinlock().
 *  \details        This function informs the trace module on entry and exit of the API ReleaseSpinlock().
 *                  For further details see \ref Os_Api_ReleaseSpinlock().
 *
 *  \param[in]      Spinlock    See \ref Os_Api_ReleaseSpinlock().
 *
 *  \return         See \ref Os_Api_ReleaseSpinlock().
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre             See \ref Os_Api_ReleaseSpinlock().
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallReleaseSpinlock (
  const   Os_SpinlockConfigType OS_CONST * Spinlock
);


/***********************************************************************************************************************
 *  Os_TrapCallTryToGetSpinlock()
 **********************************************************************************************************************/
/*! \brief          OS service \ref TryToGetSpinlock().
 *  \details        This function informs the trace module on entry and exit of the API TryToGetSpinlock().
 *                  For further details see \ref Os_Api_TryToGetSpinlock().
 *
 *  \param[in]      Spinlock   See \ref Os_Api_TryToGetSpinlock().
 *  \param[out]     Success    See \ref Os_Api_TryToGetSpinlock().
 *
 *  \return         See \ref Os_Api_TryToGetSpinlock().
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre             See \ref Os_Api_TryToGetSpinlock().
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallTryToGetSpinlock (
  const   Os_SpinlockConfigType OS_CONST * Spinlock,
  TryToGetSpinlockType* Success
);


/***********************************************************************************************************************
 *  Os_TrapCallGetTaskStackUsage()
 **********************************************************************************************************************/
/*! \brief          OS service \ref Os_GetTaskStackUsage().
 *  \details        This function informs the trace module on entry and exit of the API Os_GetTaskStackUsage().
 *                  For further details see \ref Os_Api_GetTaskStackUsage().
 *
 *  \param[in]      TaskID   See \ref Os_Api_GetTaskStackUsage().
 *
 *  \return         See \ref Os_Api_GetTaskStackUsage().
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre             See \ref Os_Api_GetTaskStackUsage().
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline uint32   Os_TrapCallGetTaskStackUsage (
  TaskType TaskID
);


/***********************************************************************************************************************
 *  Os_TrapCallGetISRStackUsage()
 **********************************************************************************************************************/
/*! \brief          OS service \ref Os_GetISRStackUsage().
 *  \details        This function informs the trace module on entry and exit of the API Os_GetISRStackUsage().
 *                  For further details see \ref Os_Api_GetISRStackUsage().
 *
 *  \param[in]      IsrID    See \ref Os_Api_GetISRStackUsage().
 *
 *  \return         See \ref Os_Api_GetISRStackUsage().
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre             See \ref Os_Api_GetISRStackUsage().
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline uint32   Os_TrapCallGetISRStackUsage (
  ISRType IsrID
);


/***********************************************************************************************************************
 *  Os_TrapCallGetKernelStackUsage()
 **********************************************************************************************************************/
/*! \brief          OS service \ref Os_GetKernelStackUsage().
 *  \details        This function informs the trace module on entry and exit of the API Os_GetKernelStackUsage().
 *                  For further details see \ref Os_Api_GetKernelStackUsage().
 *
 *  \param[in]      CoreID   See \ref Os_Api_GetKernelStackUsage().
 *
 *  \return         See \ref Os_Api_GetKernelStackUsage().
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre             See \ref Os_Api_GetKernelStackUsage().
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline uint32   Os_TrapCallGetKernelStackUsage (
  CoreIdType CoreID
);


/***********************************************************************************************************************
 *  Os_TrapCallGetStartupHookStackUsage()
 **********************************************************************************************************************/
/*! \brief          OS service \ref Os_GetStartupHookStackUsage().
 *  \details        This function informs the trace module on entry and exit of the API Os_GetStartupHookStackUsage().
 *                  For further details see \ref Os_Api_GetStartupHookStackUsage().
 *
 *  \param[in]      CoreID   See \ref Os_Api_GetStartupHookStackUsage().
 *
 *  \return         See \ref Os_Api_GetStartupHookStackUsage().
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre             See \ref Os_Api_GetStartupHookStackUsage().
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline uint32   Os_TrapCallGetStartupHookStackUsage (
  CoreIdType CoreID
);


/***********************************************************************************************************************
 *  Os_TrapCallGetErrorHookStackUsage()
 **********************************************************************************************************************/
/*! \brief          OS service \ref Os_GetErrorHookStackUsage().
 *  \details        This function informs the trace module on entry and exit of the API Os_GetErrorHookStackUsage().
 *                  For further details see \ref Os_Api_GetErrorHookStackUsage().
 *
 *  \param[in]      CoreID   See \ref Os_Api_GetErrorHookStackUsage().
 *
 *  \return         See \ref Os_Api_GetErrorHookStackUsage().
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre             See \ref Os_Api_GetErrorHookStackUsage().
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline uint32   Os_TrapCallGetErrorHookStackUsage (
  CoreIdType CoreID
);


/***********************************************************************************************************************
 *  Os_TrapCallGetShutdownHookStackUsage()
 **********************************************************************************************************************/
/*! \brief          OS service \ref Os_GetShutdownHookStackUsage().
 *  \details        This function informs the trace module on entry and exit of the API Os_GetShutdownHookStackUsage().
 *                  For further details see \ref Os_Api_GetShutdownHookStackUsage().
 *
 *  \param[in]      CoreID   See \ref Os_Api_GetShutdownHookStackUsage().
 *
 *  \return         See \ref Os_Api_GetShutdownHookStackUsage().
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre             See \ref Os_Api_GetShutdownHookStackUsage().
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline uint32   Os_TrapCallGetShutdownHookStackUsage (
  CoreIdType CoreID
);


/***********************************************************************************************************************
 *  Os_TrapCallGetProtectionHookStackUsage()
 **********************************************************************************************************************/
/*! \brief          OS service \ref Os_GetProtectionHookStackUsage().
 *  \details        This function informs the trace module on entry and exit of the API
 *                  Os_GetProtectionHookStackUsage().
 *                  For further details see \ref Os_Api_GetProtectionHookStackUsage().
 *
 *  \param[in]      CoreID   See \ref Os_Api_GetProtectionHookStackUsage().
 *
 *  \return         See \ref Os_Api_GetProtectionHookStackUsage().
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre             See \ref Os_Api_GetProtectionHookStackUsage().
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline uint32   Os_TrapCallGetProtectionHookStackUsage (
  CoreIdType CoreID
);


/***********************************************************************************************************************
 *  Os_TrapCallGetNonTrustedFunctionStackUsage()
 **********************************************************************************************************************/
/*! \brief          OS service \ref Os_GetNonTrustedFunctionStackUsage().
 *  \details        This function informs the trace module on entry and exit of the API
 *                  Os_GetNonTrustedFunctionStackUsage().
 *                  For further details see \ref Os_Api_GetNonTrustedFunctionStackUsage().
 *
 *  \param[in]      CoreID   See \ref Os_Api_GetNonTrustedFunctionStackUsage().
 *
 *  \return         See \ref Os_Api_GetNonTrustedFunctionStackUsage().
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre             See \ref Os_Api_GetNonTrustedFunctionStackUsage().
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline uint32   Os_TrapCallGetNonTrustedFunctionStackUsage (
  Os_NonTrustedFunctionIndexType FunctionIndex
);


/***********************************************************************************************************************
 *  Os_TrapCallBarrierSynchronize()
 **********************************************************************************************************************/
/*! \brief        OS service \ref Os_BarrierSynchronize().
 *  \details      This function informs the trace module on entry and exit of the API Os_BarrierSynchronize().
 *                For further details see \ref Os_BarrierSynchronize().
 *
 *  \param[in]    BarrierID   See \ref Os_BarrierSynchronize().
 *
 *  \return       See \ref Os_BarrierSynchronize().
 *
 *  \context      OS_INTERNAL
 *
 *  \reentrant    TRUE
 *  \synchronous  TRUE
 *
 *  \pre          See \ref Os_BarrierSynchronize().
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallBarrierSynchronize (
  Os_BarrierIdType BarrierID
);


/***********************************************************************************************************************
 *  Os_TrapCallActivateTask()
 **********************************************************************************************************************/
/*! \brief          OS service \ref ActivateTask().
 *  \details        This function informs the trace module on entry and exit of the API ActivateTask().
 *                  For further details see \ref Os_Api_ActivateTask().
 *
 *  \param[in]      TaskID   See \ref Os_Api_ActivateTask().
 *
 *  \return         See \ref Os_Api_ActivateTask().
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre             See \ref Os_Api_ActivateTask().
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallActivateTask (
  TaskType TaskID
);


/***********************************************************************************************************************
 *  Os_TrapCallTerminateTask()
 **********************************************************************************************************************/
/*! \brief          OS service \ref TerminateTask().
 *  \details        This function informs the trace module on entry and exit of the API TerminateTask().
 *                  For further details see \ref Os_Api_TerminateTask().
 *
 *  \return         See \ref Os_Api_TerminateTask().
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre             See \ref Os_Api_TerminateTask().
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallTerminateTask (
  void
);


/***********************************************************************************************************************
 *  Os_TrapCallChainTask()
 **********************************************************************************************************************/
/*! \brief          OS service \ref ChainTask().
 *  \details        This function informs the trace module on entry and exit of the API ChainTask().
 *                  For further details see \ref Os_Api_ChainTask().
 *
 *  \param[in]      TaskID   See \ref Os_Api_ChainTask().
 *
 *  \return         See \ref Os_Api_ChainTask().
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre             See \ref Os_Api_ChainTask().
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallChainTask (
  TaskType TaskID
);


/***********************************************************************************************************************
 *  Os_TrapCallSchedule()
 **********************************************************************************************************************/
/*! \brief          OS service \ref Schedule().
 *  \details        This function informs the trace module on entry and exit of the API Schedule().
 *                  For further details see \ref Os_Api_Schedule().
 *
 *  \return         See \ref Os_Api_Schedule().
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre             See \ref Os_Api_Schedule().
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallSchedule (
  void
);


/***********************************************************************************************************************
 *  Os_TrapCallGetTaskID()
 **********************************************************************************************************************/
/*! \brief          OS service \ref GetTaskID().
 *  \details        This function informs the trace module on entry and exit of the API GetTaskID().
 *                  For further details see \ref Os_Api_GetTaskID().
 *
 *  \param[out]     TaskID   See \ref Os_Api_GetTaskID().
 *
 *  \return         See \ref Os_Api_GetTaskID().
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre             See \ref Os_Api_GetTaskID().
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallGetTaskID (
  TaskRefType TaskID
);


/***********************************************************************************************************************
 *  Os_TrapCallGetTaskState()
 **********************************************************************************************************************/
/*! \brief          OS service \ref GetTaskState().
 *  \details        This function informs the trace module on entry and exit of the API GetTaskState().
 *                  For further details see \ref Os_Api_GetTaskState().
 *
 *  \param[in]      TaskID   See \ref Os_Api_GetTaskState().
 *  \param[out]     State    See \ref Os_Api_GetTaskState().
 *
 *  \return         See \ref Os_Api_GetTaskState().
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre            See \ref Os_Api_GetTaskState().
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallGetTaskState (
  TaskType TaskID,
  TaskStateRefType State
);


/***********************************************************************************************************************
 *  Os_TrapCallGetCoreStartState()
 **********************************************************************************************************************/
/*! \brief          OS service \ref Os_GetCoreStartState().
 *  \details        This function informs the trace module on entry and exit of the API Os_GetCoreStartState().
 *                  For further details see \ref Os_Api_GetCoreStartState().
 *
 *  \param[in]      CoreID    See \ref Os_Api_GetCoreStartState().
 *  \param[out]     CoreState See \ref Os_Api_GetCoreStartState().
 *  \param[out]     Status    See \ref Os_Api_GetCoreStartState().
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre            See \ref Os_Api_GetCoreStartState().
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline void   Os_TrapCallGetCoreStartState (
  CoreIdType CoreID,
  Os_CoreStartStateType *CoreState,
  StatusType *Status
);


/***********************************************************************************************************************
 *  LOCAL FUNCTIONS
 **********************************************************************************************************************/

/***********************************************************************************************************************
 *  Os_TrapIsTrapForWriteAccessRequired()
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
 */
OS_ALWAYS_INLINE 
static inline Os_StdReturnType   Os_TrapIsTrapForWriteAccessRequired (
  void
)
{
  Os_StdReturnType result;

  /* #10 If memory protection is enabled: */
  if(Os_MpIsEnabled() != 0u)                                                                                            /* PRQA S 2991, 2995 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule2.2_2995 */
  {
    /* #20 If the privileged mode is readable in user mode: */
    if(Os_Hal_CoreIsPrivilegedModeUserReadable() != 0u)                                                                 /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */ /* COV_OS_HALPLATFORMPRIVILEGEMODEUSERREADABILITY */
    {
      /* #30 Read privileged mode from hardware. */
      result = !Os_Hal_CoreGetPrivilegedMode();                                                                         /* PRQA S 2995, 2996, 4116, 4404, 4558 */ /* MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996, MD_Os_Rule10.1_4116, MD_Os_C90BooleanCompatibility, MD_Os_Rule10.1_4558 */
    }
    /* #40 Else if the thread pointer is readable in user mode: */
    else if(Os_Hal_CoreIsThreadPointerUserReadable() != 0u)                                                             /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */ /* COV_OS_HALPLATFORMTHREADPOINTERUSERREADABILITY_TX_XF */
    {
      /* #50 Read privileged mode from thread. */
      result = !Os_TrapIsCurrentThreadPrivileged();                                                                     /* PRQA S 4116, 4404, 4558 */ /* MD_Os_Rule10.1_4116, MD_Os_C90BooleanCompatibility, MD_Os_Rule10.1_4558 */
    }
    /* #60 Otherwise return trap is needed. */
    else
    {
      result = !0u;                                                                                                     /* PRQA S 4404, 4558 */ /* MD_Os_C90BooleanCompatibility, MD_Os_Rule10.1_4558 */
    }
  }
  /* #70 Otherwise return trap is not needed. */
  else
  {
    result = 0;
  }

  return result;
}


/***********************************************************************************************************************
 *  Os_TrapIsTrapForReadAccessRequired()
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
 */
OS_ALWAYS_INLINE 
static inline Os_StdReturnType   Os_TrapIsTrapForReadAccessRequired (
 void
)
{
  Os_StdReturnType result;

  /* #10 If memory protection is enabled: */
  if(Os_MpIsEnabled() != 0u)                                                                                            /* PRQA S 2991, 2995 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule2.2_2995 */
  {
    /* #20 If tracing is enabled: */
    if(Os_TraceIsOrtiServiceTracingEnabled() != 0u)                                                                     /* PRQA S 2992, 2996 */ /* MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2996 */
    {
      /* #30 Check if trap is needed for write access. */
      result = Os_TrapIsTrapForWriteAccessRequired();
    }
    /* #40 Otherwise check whether CoreId and Threadpointer is readable in user mode. */
    else
    {
      /* #50 If thread pointer is readable in user mode: */
      if(Os_Hal_CoreIsThreadPointerUserReadable() != 0u)                                                                /* COV_OS_HALPLATFORMTHREADPOINTERUSERREADABILITY_TX_XF */ /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
      {
        /* #60 If CoreId is readable in user mode: */
        if(Os_Hal_CoreIsCoreIdUserReadable() != 0u)                                                                     /* COV_OS_HALPLATFORMCOREIDUSERREADABILITY_TX_XF */ /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
        {
          /* #70 Return trap is not needed. */
          result = 0;
        }
        /* #80 Otherwise check if trap is needed for write access. */
        else
        {
          result = Os_TrapIsTrapForWriteAccessRequired();
        }
      }
      /* #90 Otherwise check if trap is needed for write access. */
      else
      {
        result = Os_TrapIsTrapForWriteAccessRequired();
      }
    }
  }

  /* #100 Otherwise return trap is not needed. */
  else
  {
    result = 0;
  }

  return result;
}


/***********************************************************************************************************************
 *  Os_TrapIsCurrentThreadPrivileged()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 */
OS_ALWAYS_INLINE                                                                                            /* COV_OS_HALPLATFORMTHREADPOINTERUSERREADABILITY_XX */
static inline Os_StdReturnType   Os_TrapIsCurrentThreadPrivileged (
  void
)                                                                                                                      /* COV_OS_HALPLATFORMTHREADPOINTERUSERREADABILITY_XX */
{
  const   Os_ThreadConfigType OS_CONST * thread;
  const   Os_AppConfigType OS_CONST * app;

  /* #10 Get the current application. */
  thread = Os_CoreGetThread();
  app = Os_ThreadGetCurrentApplication(thread);                                                                         /* SBSW_OS_THREADGETCURRENTAPPLICATION_001 */

  /* #20 Return whether the current application is privileged. */
  return Os_AppIsPrivileged(app);                                                                                       /* SBSW_OS_APPISPRIVILEGED_001 */
}


/***********************************************************************************************************************
 *  Os_TrapIsTrapRequired()
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
 */
OS_ALWAYS_INLINE static inline Os_StdReturnType   Os_TrapIsTrapRequired (
  boolean IsTrapRequired
)
{
  Os_StdReturnType result;

  /* #10 If memory protection is enabled: */
  if(Os_MpIsEnabled() != 0u)                                                                                            /* PRQA S 2991, 2995 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule2.2_2995 */ /* COV_OS_HALPLATFORMTESTEDASMULTICORE */
  {
    /* #20 If the thread pointer is readable in user mode: */
    if(Os_Hal_CoreIsThreadPointerUserReadable() != 0u)                                                                  /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */ /* COV_OS_HALPLATFORMTHREADPOINTERUSERREADABILITY_TX_XF */
    {
      /* #30 If caller does not have write access, return IsTrapRequired. */
      if(Os_TrapIsTrapForWriteAccessRequired() != 0u) {                                                                 /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
        result = IsTrapRequired;
      }
      /* #40 Otherwise, return 0 */
      else
      {
        result = 0;
      }
    }
    /* #50 If the privilege mode is readable in user mode: */
    else if(Os_Hal_CoreIsPrivilegedModeUserReadable() != 0u)                                                            /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */ /* COV_OS_HALPLATFORMPRIVILEGEMODEUSERREADABILITY */
    {
      /* #60 Read privileged mode from hardware. */
      result = !Os_Hal_CoreGetPrivilegedMode();                                                                         /* PRQA S 4116, 4404, 4558 */ /* MD_Os_Rule10.1_4116, MD_Os_C90BooleanCompatibility, MD_Os_Rule10.1_4558 */
    }
    else
    {
      /* #70 Otherwise return !0. */
      result = !0u;                                                                                                     /* PRQA S 4404, 4558 */ /* MD_Os_C90BooleanCompatibility, MD_Os_Rule10.1_4558 */
    }
  }
  /* #80 Otherwise return 0. */
  else
  {
    result = 0;
  }

  return result;
}


/***********************************************************************************************************************
 *  Os_TrapCallCheckTaskMemoryAccess()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline AccessType   Os_TrapCallCheckTaskMemoryAccess (
  TaskType TaskID,
  MemoryStartAddressType Address,
  MemorySizeType Size
)
{
  AccessType result;

  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdCheckTaskMemoryAccess);

  /* #20 Call the API. */
  result = Os_Api_CheckTaskMemoryAccess(TaskID, Address, Size);                                                         /* SBSW_OS_FC_PRECONDITION */

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdCheckTaskMemoryAccess);

  return result;
}


/***********************************************************************************************************************
 *  Os_TrapCallCheckISRMemoryAccess()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline AccessType   Os_TrapCallCheckISRMemoryAccess (
  ISRType ISRID,
  MemoryStartAddressType Address,
  MemorySizeType Size
)
{
  AccessType result;

  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdCheckISRMemoryAccess);

  /* #20 Call the API. */
  result =  Os_Api_CheckISRMemoryAccess(ISRID, Address, Size);                                                          /* SBSW_OS_FC_PRECONDITION */

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdCheckISRMemoryAccess);

  return result;
}


/***********************************************************************************************************************
 *  Os_TrapCallGetAlarmBase()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallGetAlarmBase (
  AlarmType AlarmID,
  AlarmBaseRefType Info
)
{
  Os_StatusType status;

  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdGetAlarmBase);

  /* #20 Call the API. */
  status = Os_Api_GetAlarmBase(AlarmID, Info);                                                                          /* SBSW_OS_FC_PRECONDITION */

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdGetAlarmBase);

  return status;
}


/***********************************************************************************************************************
 *  Os_TrapCallGetAlarm()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallGetAlarm (
  AlarmType AlarmID,
  TickRefType Tick
)
{
  Os_StatusType status;

  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdGetAlarm);

  /* #20 Call the API. */
  status = Os_Api_GetAlarm(AlarmID, Tick);                                                                              /* SBSW_OS_FC_PRECONDITION */

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdGetAlarm);

  return status;
}


/***********************************************************************************************************************
 *  Os_TrapCallSetRelAlarm()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallSetRelAlarm (
  AlarmType AlarmID,
  TickType Increment,
  TickType Cycle
)
{
  Os_StatusType status;

  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdSetRelAlarm);

  /* #20 Call the API. */
  status = Os_Api_SetRelAlarm(AlarmID, Increment, Cycle);

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdSetRelAlarm);

  return status;
}


/***********************************************************************************************************************
 *  Os_TrapCallSetAbsAlarm()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallSetAbsAlarm (
  AlarmType AlarmID,
  TickType Start,
  TickType Cycle
)
{
  Os_StatusType status;

  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdSetAbsAlarm);

  /* #20 Call the API. */
  status = Os_Api_SetAbsAlarm(AlarmID, Start, Cycle);

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdSetAbsAlarm);

  return status;
}


/***********************************************************************************************************************
 *  Os_TrapCallCancelAlarm()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallCancelAlarm (
  AlarmType AlarmID
)
{
  Os_StatusType status;

  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdCancelAlarm);

  /* #20 Call the API. */
  status = Os_Api_CancelAlarm(AlarmID);

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdCancelAlarm);

  return status;
}


/***********************************************************************************************************************
 *  Os_TrapCallGetApplicationID()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline ApplicationType   Os_TrapCallGetApplicationID (
  void
)
{
  ApplicationType result;

  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdGetApplicationID);

  /* #20 Call the API. */
  result = Os_Api_GetApplicationID();

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdGetApplicationID);

  return result;
}


/***********************************************************************************************************************
 *  Os_TrapCallGetCurrentApplicationID()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline ApplicationType   Os_TrapCallGetCurrentApplicationID (
  void
)
{
  ApplicationType result;

  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdGetCurrentApplicationID);

  /* #20 Call the API. */
  result = Os_Api_GetCurrentApplicationID();

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdGetCurrentApplicationID);

  return result;
}


/***********************************************************************************************************************
 *  Os_TrapCallGetApplicationState()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallGetApplicationState (
  ApplicationType Application,
  ApplicationStateRefType Value
)
{
  Os_StatusType status;

  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdGetApplicationState);

  /* #20 Call the API. */
  status = Os_Api_GetApplicationState(Application, Value);                                                              /* SBSW_OS_FC_PRECONDITION */

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdGetApplicationState);

  return status;
}


/***********************************************************************************************************************
 *  Os_TrapCallCheckObjectAccess()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline ObjectAccessType   Os_TrapCallCheckObjectAccess (
  ApplicationType ApplID,
  ObjectTypeType ObjectType,
  Os_ObjectIdType ObjectID
)
{
  ObjectAccessType result;

  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdCheckObjectAccess);

  /* #20 Call the API. */
  result = Os_Api_CheckObjectAccess(ApplID, ObjectType, ObjectID);

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdCheckObjectAccess);

  return result;
}


/***********************************************************************************************************************
 *  Os_TrapCallCheckObjectOwnership()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline ApplicationType   Os_TrapCallCheckObjectOwnership (
  ObjectTypeType ObjectType,
  Os_ObjectIdType ObjectID
)
{
  ApplicationType result;

  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdCheckObjectOwnership);

  /* #20 Call the API. */
  result = Os_Api_CheckObjectOwnership(ObjectType, ObjectID);

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdCheckObjectOwnership);

  return result;
}


/***********************************************************************************************************************
 *  Os_TrapCallAllowAccess()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallAllowAccess (
  void
)
{
  Os_StatusType status;

  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdAllowAccess);

  /* #20 Call the API. */
  status = Os_Api_AllowAccess();

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdAllowAccess);

  return status;
}


/***********************************************************************************************************************
 *  Os_TrapCallTerminateApplication()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallTerminateApplication (
  ApplicationType Application,
  RestartType RestartOption
)
{
  Os_StatusType status;

  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdTerminateApplication);

  /* #20 Call the API. */
  status = Os_Api_TerminateApplication(Application, RestartOption);

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdTerminateApplication);

  return status;
}


/***********************************************************************************************************************
 *  Os_TrapCallGetVersionInfo()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 */
OS_ALWAYS_INLINE static inline void   Os_TrapCallGetVersionInfo (
  Std_VersionInfoType *versioninfo
)
{
  /* #10 Call the API. */
  Os_Api_GetVersionInfo(versioninfo);                                                                                   /* SBSW_OS_FC_PRECONDITION */
}


/***********************************************************************************************************************
 *  Os_TrapCallGetExceptionContext()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallGetExceptionContext (
  Os_ExceptionContextRefType Context
)
{
  Os_StatusType status;

  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdGetExceptionContext);

  /* #20 Call the API. */
  status = Os_Api_GetExceptionContext(Context);                                                                         /* SBSW_OS_FC_PRECONDITION */

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdGetExceptionContext);

  return status;
}


/***********************************************************************************************************************
 *  Os_TrapCallSetExceptionContext()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallSetExceptionContext (
  const   Os_ExceptionContextType OS_CONST * Context
)
{
  Os_StatusType status;

  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdSetExceptionContext);

  /* #20 Call the API. */
  status = Os_Api_SetExceptionContext(Context);                                                                         /* SBSW_OS_FC_PRECONDITION */

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdSetExceptionContext);

  return status;
}


/***********************************************************************************************************************
 *  Os_TrapCallGetExceptionAddress()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline Os_AddressOfConstType   Os_TrapCallGetExceptionAddress (
  void
)
{
  Os_AddressOfConstType address;

  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdGetExceptionAddress);

  /* #20 Call the API. */
  address = Os_Api_GetExceptionAddress();

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdGetExceptionAddress);

  return address;
}


/***********************************************************************************************************************
 *  Os_TrapCallInitMemory()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 */
OS_ALWAYS_INLINE static inline void   Os_TrapCallInitMemory (
  void
)
{
  /* #10 Call the API. */
  Os_Api_InitMemory();
}


/***********************************************************************************************************************
 *  Os_TrapCallInit()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 */
OS_ALWAYS_INLINE static inline void   Os_TrapCallInit (
  void
)
{
  /* #10 Call the API. */
  Os_Api_Init();
}


/***********************************************************************************************************************
 *  Os_TrapCallInitInterruptOnly()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 */
OS_ALWAYS_INLINE static inline void   Os_TrapCallInitInterruptOnly (
  void
)
{
  /* #10 Call the API. */
  Os_Api_InitInterruptOnly();
}


/***********************************************************************************************************************
 *  Os_TrapCallEnterPreStartTask()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline void   Os_TrapCallEnterPreStartTask (
  void
)
{
  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdEnterPreStartTask);

  /* #20 Call the API. */
  Os_Api_EnterPreStartTask();

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdEnterPreStartTask);
}


/***********************************************************************************************************************
 *  Os_TrapCallStartCore()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
/* COV_OS_HALPLATFORMTESTEDASMULTICORE */
OS_ALWAYS_INLINE static inline void   Os_TrapCallStartCore (
  CoreIdType CoreID,
  StatusType *Status
)                                                                                                                      /* COV_OS_HALPLATFORMTESTEDASMULTICORE */
{
  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdStartCore);

  /* #20 Call the API. */
  Os_Api_StartCore(CoreID, Status);                                                                                     /* SBSW_OS_FC_PRECONDITION */

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdStartCore);
}


/***********************************************************************************************************************
 *  Os_TrapCallStartNonAutosarCore()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
/* COV_OS_HALPLATFORMTESTEDASMULTICORE */
OS_ALWAYS_INLINE static inline void   Os_TrapCallStartNonAutosarCore (
  CoreIdType CoreID,
  StatusType *Status
)                                                                                                                      /* COV_OS_HALPLATFORMTESTEDASMULTICORE */
{
  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdStartNonAutosarCore);

  /* #20 Call the API. */
  Os_Api_StartNonAutosarCore(CoreID, Status);                                                                           /* SBSW_OS_FC_PRECONDITION */

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdStartNonAutosarCore);
}


/***********************************************************************************************************************
 *  Os_TrapCallGetCoreID()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline CoreIdType   Os_TrapCallGetCoreID (
  void
)
{
  CoreIdType result;

  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdGetCoreID);

  /* #20 Call the API. */
  result = Os_Api_GetCoreID();

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdGetCoreID);

  return result;
}


/***********************************************************************************************************************
 *  Os_TrapCallGetNumberOfActivatedCores()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
/* COV_OS_HALPLATFORMTESTEDASMULTICORE */
OS_ALWAYS_INLINE static inline uint32   Os_TrapCallGetNumberOfActivatedCores (
  void
)                                                                                                                      /* COV_OS_HALPLATFORMTESTEDASMULTICORE */
{
  uint32 result;

  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdGetNumberOfActivatedCores);

  /* #20 Call the API. */
  result = Os_Api_GetNumberOfActivatedCores();

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdGetNumberOfActivatedCores);

  return result;
}


/***********************************************************************************************************************
 *  Os_TrapCallGetActiveApplicationMode()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline AppModeType   Os_TrapCallGetActiveApplicationMode (
  void
)
{
  AppModeType result;

  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdGetActiveApplicationMode);

  /* #20 Call the API. */
  result = Os_Api_GetActiveApplicationMode();

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdGetActiveApplicationMode);

  return result;
}


/***********************************************************************************************************************
 *  Os_TrapCallStartOS()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline void   Os_TrapCallStartOS (
  AppModeType Mode
)
{
  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdStartOS);

  /* #20 Call the API. */
  Os_Api_StartOS(Mode);

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdStartOS);
}


/***********************************************************************************************************************
 *  Os_TrapCallShutdownOS()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline void   Os_TrapCallShutdownOS (
  StatusType Error
)
{
  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdShutdownOS);

  /* #20 Call the API. */
  Os_Api_ShutdownOS(Error);

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdShutdownOS);
}


/***********************************************************************************************************************
 *  Os_TrapCallShutdownAllCores()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
/* COV_OS_HALPLATFORMTESTEDASMULTICORE */
OS_ALWAYS_INLINE static inline void   Os_TrapCallShutdownAllCores (
  StatusType Error
)                                                                                                                      /* COV_OS_HALPLATFORMTESTEDASMULTICORE */
{
  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdShutdownAllCores);

  /* #20 Call the API. */
  Os_Api_ShutdownAllCores(Error);

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdShutdownAllCores);
}


/***********************************************************************************************************************
 *  Os_TrapCallControlIdle()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallControlIdle (
  CoreIdType CoreID,
  IdleModeType IdleMode
)
{
  Os_StatusType status;

  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdControlIdle);

  /* #20 Call the API. */
  status = Os_Api_ControlIdle(CoreID, IdleMode);

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdControlIdle);

  return status;
}


/***********************************************************************************************************************
 *  Os_TrapCallGetUnhandledIrq()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallGetUnhandledIrq (
  Os_InterruptSourceIdRefType InterruptSource
)
{
  Os_StatusType status;

  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdGetUnhandledIrq);

  /* #20 Call the API. */
  status = Os_Api_GetUnhandledIrq(InterruptSource);                                                                     /* SBSW_OS_FC_PRECONDITION */

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdGetUnhandledIrq);

  return status;
}


/***********************************************************************************************************************
 *  Os_TrapCallGetUnhandledExc()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallGetUnhandledExc (
  Os_ExceptionSourceIdRefType ExceptionSource
)
{
  Os_StatusType status;

  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdGetUnhandledExc);

  /* #20 Call the API. */
  status = Os_Api_GetUnhandledExc(ExceptionSource);                                                                     /* SBSW_OS_FC_PRECONDITION */

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdGetUnhandledExc);

  return status;
}


/***********************************************************************************************************************
 *  Os_TrapCallIncrementCounter()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallIncrementCounter (
  CounterType CounterID
)
{
  Os_StatusType status;

  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdIncrementCounter);

  /* #20 Call the API. */
  status = Os_Api_IncrementCounter(CounterID);

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdIncrementCounter);

  return status;
}


/***********************************************************************************************************************
 *  Os_TrapCallGetCounterValue()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallGetCounterValue (
  CounterType CounterID,
  TickRefType Value
)
{
  Os_StatusType status;

  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdGetCounterValue);

  /* #20 Call the API. */
  status = Os_Api_GetCounterValue(CounterID, Value);                                                                    /* SBSW_OS_FC_PRECONDITION */

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdGetCounterValue);

  return status;
}


/***********************************************************************************************************************
 *  Os_TrapCallGetElapsedValue()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallGetElapsedValue (
  CounterType CounterID,
  TickRefType Value,
  TickRefType ElapsedValue
)
{
  Os_StatusType status;

  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdGetElapsedValue);

  /* #20 Call the API. */
  status = Os_Api_GetElapsedValue(CounterID, Value, ElapsedValue);                                                      /* SBSW_OS_FC_PRECONDITION */

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdGetElapsedValue);

  return status;
}


/***********************************************************************************************************************
 *  Os_TrapCallGetDetailedError
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallGetDetailedError (
  Os_ErrorInformationRefType ErrorRef
)
{
  Os_StatusType status;

  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdGetDetailedError);

  /* #20 Call the API. */
  status = Os_Api_GetDetailedError(ErrorRef);                                                                           /* SBSW_OS_FC_PRECONDITION */

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdGetDetailedError);

  return status;
}


/***********************************************************************************************************************
 *  Os_TrapCallOSErrorGetServiceId()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline OSServiceIdType   Os_TrapCallOSErrorGetServiceId (
  void
)
{
  OSServiceIdType result;

  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdOSErrorGetServiceId);

  /* #20 Call the API. */
  result = Os_Api_ErrorGetServiceId();

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdOSErrorGetServiceId);

  return result;
}


/***********************************************************************************************************************
 *  Os_TrapCallSetEvent()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallSetEvent (
  TaskType TaskID,
  EventMaskType Mask
)
{
  Os_StatusType status;

  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdSetEvent);

  /* #20 Call the API. */
  status = Os_Api_SetEvent(TaskID, Mask);

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdSetEvent);

  return status;
}


/***********************************************************************************************************************
 *  Os_TrapCallClearEvent()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallClearEvent (
  EventMaskType Mask
)
{
  Os_StatusType status;

  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdClearEvent);

  /* #20 Call the API. */
  status = Os_Api_ClearEvent(Mask);

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdClearEvent);

  return status;
}


/***********************************************************************************************************************
 *  Os_TrapCallGetEvent()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallGetEvent (
  TaskType TaskID,
  EventMaskRefType Mask
)
{
  Os_StatusType status;

  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdGetEvent);

  /* #20 Call the API. */
  status = Os_Api_GetEvent(TaskID, Mask);                                                                               /* SBSW_OS_FC_PRECONDITION */

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdGetEvent);

  return status;
}


/***********************************************************************************************************************
 *  Os_TrapCallWaitEvent()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallWaitEvent (
  EventMaskType Mask
)
{
  Os_StatusType status;

  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdWaitEvent);

  /* #20 Call the API. */
  status = Os_Api_WaitEvent(Mask);

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdWaitEvent);

  return status;
}


/***********************************************************************************************************************
 *  Os_TrapCallDisableAllInterrupts()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline void   Os_TrapCallDisableAllInterrupts (
  void
)
{
  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdDisableAllInterrupts);

  /* #20 Call the API. */
  Os_Api_DisableAllInterrupts();

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdDisableAllInterrupts);
}


/***********************************************************************************************************************
 *  Os_TrapCallEnableAllInterrupts()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline void   Os_TrapCallEnableAllInterrupts (
  void
)
{
  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdEnableAllInterrupts);

  /* #20 Call the API. */
  Os_Api_EnableAllInterrupts();

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdEnableAllInterrupts);
}


/***********************************************************************************************************************
 *  Os_TrapCallSuspendAllInterrupts()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline void   Os_TrapCallSuspendAllInterrupts (
  void
)
{
  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdSuspendAllInterrupts);

  /* #20 Call the API. */
  Os_Api_SuspendAllInterrupts();

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdSuspendAllInterrupts);
}


/***********************************************************************************************************************
 *  Os_TrapCallResumeAllInterrupts()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline void   Os_TrapCallResumeAllInterrupts (
  void
)
{
  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdResumeAllInterrupts);

  /* #20 Call the API. */
  Os_Api_ResumeAllInterrupts();

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdResumeAllInterrupts);
}


/***********************************************************************************************************************
 *  Os_TrapCallSuspendOSInterrupts()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline void   Os_TrapCallSuspendOSInterrupts (
  void
)
{
  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdSuspendOSInterrupts);

  /* #20 Call the API. */
  Os_Api_SuspendOSInterrupts();

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdSuspendOSInterrupts);
}


/***********************************************************************************************************************
 *  Os_TrapCallResumeOSInterrupts()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline void   Os_TrapCallResumeOSInterrupts (
  void
)
{
  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdResumeOSInterrupts);

  /* #20 Call the API. */
  Os_Api_ResumeOSInterrupts();

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdResumeOSInterrupts);
}


/***********************************************************************************************************************
 *  Os_TrapCallDisableGlobal()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline void   Os_TrapCallDisableGlobal (
  void
)
{
  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdDisableAllInterrupts);

  /* #20 Call the API. */
  Os_Api_DisableGlobal();

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdDisableAllInterrupts);
}


/***********************************************************************************************************************
 *  Os_TrapCallEnableGlobal()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline void   Os_TrapCallEnableGlobal (
  void
)
{
  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdEnableAllInterrupts);

  /* #20 Call the API. */
  Os_Api_EnableGlobal();

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdEnableAllInterrupts);
}


/***********************************************************************************************************************
 *  Os_TrapCallDisableLevel()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline void   Os_TrapCallDisableLevel (
  void
)
{
  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdDisableOSInterrupts);

  /* #20 Call the API. */
  Os_Api_DisableLevel();

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdDisableOSInterrupts);
}


/***********************************************************************************************************************
 *  Os_TrapCallEnableLevel()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline void   Os_TrapCallEnableLevel (
  void
)
{
  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdEnableOSInterrupts);

  /* #20 Call the API. */
  Os_Api_EnableLevel();

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdEnableOSInterrupts);
}


/***********************************************************************************************************************
 *  Os_TrapCallGetISRID()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline ISRType   Os_TrapCallGetISRID (
  void
)
{
  ISRType result;

  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdGetISRID);

  /* #20 Call the API. */
  result = Os_Api_GetISRID();

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdGetISRID);

  return result;
}


/***********************************************************************************************************************
 *  Os_TrapCallDisableInterruptSource()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallDisableInterruptSource (
  ISRType ISRID
)
{
  Os_StatusType status;

  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdDisableInterruptSource);

  /* #20 Call the API. */
  status = Os_Api_DisableInterruptSource(ISRID);

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdDisableInterruptSource);

  return status;
}


/***********************************************************************************************************************
 *  Os_TrapCallEnableInterruptSource()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallEnableInterruptSource (
  ISRType ISRID,
  boolean ClearPending
)
{
  Os_StatusType status;

  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdEnableInterruptSource);

  /* #20 Call the API. */
  status = Os_Api_EnableInterruptSource(ISRID, ClearPending);

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdEnableInterruptSource);

  return status;
}


/***********************************************************************************************************************
 *  Os_TrapCallInitialEnableInterruptSources()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallInitialEnableInterruptSources (
  boolean ClearPending
)
{
  Os_StatusType status;

  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdInitialEnableInterruptSources);

  /* #20 Call the API. */
  status = Os_Api_InitialEnableInterruptSources(ClearPending);

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdInitialEnableInterruptSources);

  return status;
}


/***********************************************************************************************************************
 *  Os_TrapCallClearPendingInterrupt()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallClearPendingInterrupt (
  ISRType ISRID
)
{
  Os_StatusType status;

  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdClearPendingInterrupt);

  /* #20 Call the API. */
  status = Os_Api_ClearPendingInterrupt(ISRID);

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdClearPendingInterrupt);

  return status;
}


/***********************************************************************************************************************
 *  Os_TrapCallIsInterruptSourceEnabled()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallIsInterruptSourceEnabled (
  ISRType ISRID,
    boolean OS_VAR_NOINIT * IsEnabled
)
{
  Os_StatusType status;

  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdIsInterruptSourceEnabled);

  /* #20 Call the API. */
  status = Os_Api_IsInterruptSourceEnabled(ISRID, IsEnabled);                                                           /* SBSW_OS_FC_PRECONDITION */

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdIsInterruptSourceEnabled);

  return status;
}


/***********************************************************************************************************************
 *  Os_TrapCallIsInterruptPending()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallIsInterruptPending (
  ISRType ISRID,
    boolean OS_VAR_NOINIT * IsPending
)
{
  Os_StatusType status;

  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdIsInterruptPending);

  /* #20 Call the API. */
  status = Os_Api_IsInterruptPending(ISRID, IsPending);                                                                 /* SBSW_OS_FC_PRECONDITION */

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdIsInterruptPending);

  return status;
}


/***********************************************************************************************************************
 *  Os_TrapCallReadPeripheral8()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline uint8   Os_TrapCallReadPeripheral8 (
  Os_PeripheralIdType PeripheralID,
  const   uint8 OS_APPL_DATA * Address
)
{
  uint8 result;

  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdReadPeripheral8);

  /* #20 Call the API. */
  result = Os_Api_ReadPeripheral8(PeripheralID, Address);                                                               /* SBSW_OS_TRAP_API_USERPOINTER_003 */

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdReadPeripheral8);

  return result;
}


/***********************************************************************************************************************
 *  Os_TrapCallReadPeripheral16()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline uint16   Os_TrapCallReadPeripheral16 (
  Os_PeripheralIdType PeripheralID,
  const   uint16 OS_APPL_DATA * Address
)
{
  uint16 result;

  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdReadPeripheral16);

  /* #20 Call the API. */
  result = Os_Api_ReadPeripheral16(PeripheralID, Address);                                                              /* SBSW_OS_TRAP_API_USERPOINTER_003 */

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdReadPeripheral16);

  return result;
}


/***********************************************************************************************************************
 *  Os_TrapCallReadPeripheral32()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline uint32   Os_TrapCallReadPeripheral32 (
  Os_PeripheralIdType PeripheralID,
  const   uint32 OS_APPL_DATA * Address
)
{
  uint32 result;

  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdReadPeripheral32);

  /* #20 Call the API. */
  result = Os_Api_ReadPeripheral32(PeripheralID, Address);                                                              /* SBSW_OS_TRAP_API_USERPOINTER_003 */

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdReadPeripheral32);

  return result;
}


/***********************************************************************************************************************
 *  Os_TrapCallWritePeripheral8()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline void   Os_TrapCallWritePeripheral8 (
  Os_PeripheralIdType PeripheralID,
    uint8 OS_APPL_DATA * Address,
  uint8 Value
)
{
  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdWritePeripheral8);

  /* #20 Call the API. */
  Os_Api_WritePeripheral8(PeripheralID, Address, Value);                                                                /* SBSW_OS_TRAP_API_USERPOINTER_003 */

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdWritePeripheral8);

}


/***********************************************************************************************************************
 *  Os_TrapCallWritePeripheral16()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline void   Os_TrapCallWritePeripheral16 (
  Os_PeripheralIdType PeripheralID,
    uint16 OS_APPL_DATA * Address,
  uint16 Value
)
{
  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdWritePeripheral16);

  /* #20 Call the API. */
  Os_Api_WritePeripheral16(PeripheralID, Address, Value);                                                               /* SBSW_OS_TRAP_API_USERPOINTER_003 */

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdWritePeripheral16);
}


/***********************************************************************************************************************
 *  Os_TrapCallWritePeripheral32()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline void   Os_TrapCallWritePeripheral32 (
  Os_PeripheralIdType PeripheralID,
    uint32 OS_APPL_DATA * Address,
  uint32 Value
)
{
  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdWritePeripheral32);

  /* #20 Call the API. */
  Os_Api_WritePeripheral32(PeripheralID, Address, Value);                                                               /* SBSW_OS_TRAP_API_USERPOINTER_003 */

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdWritePeripheral32);
}


/***********************************************************************************************************************
 *  Os_TrapCallModifyPeripheral8()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline void   Os_TrapCallModifyPeripheral8 (
  Os_PeripheralIdType PeripheralID,
    uint8 OS_APPL_DATA * Address,
  uint8 ClearMask,
  uint8 SetMask
)
{
  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdModifyPeripheral8);

  /* #20 Call the API. */
  Os_Api_ModifyPeripheral8(PeripheralID, Address, ClearMask, SetMask);                                                  /* SBSW_OS_TRAP_API_USERPOINTER_003 */

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdModifyPeripheral8);
}


/***********************************************************************************************************************
 *  Os_TrapCallModifyPeripheral16()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline void   Os_TrapCallModifyPeripheral16 (
  Os_PeripheralIdType PeripheralID,
    uint16 OS_APPL_DATA * Address,
  uint16 ClearMask,
  uint16 SetMask
)
{
  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdModifyPeripheral16);

  /* #20 Call the API. */
  Os_Api_ModifyPeripheral16(PeripheralID, Address, ClearMask, SetMask);                                                 /* SBSW_OS_TRAP_API_USERPOINTER_003 */

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdModifyPeripheral16);
}


/***********************************************************************************************************************
 *  Os_TrapCallModifyPeripheral32()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline void   Os_TrapCallModifyPeripheral32 (
  Os_PeripheralIdType PeripheralID,
    uint32 OS_APPL_DATA * Address,
  uint32 ClearMask,
  uint32 SetMask
)
{
  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdModifyPeripheral32);

  /* #20 Call the API. */
  Os_Api_ModifyPeripheral32(PeripheralID, Address, ClearMask, SetMask);                                                 /* SBSW_OS_TRAP_API_USERPOINTER_003 */

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdModifyPeripheral32);
}


/***********************************************************************************************************************
 *  Os_TrapCallGetResource()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallGetResource (
  ResourceType ResID
)
{
  Os_StatusType status;

  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdGetResource);

  /* #20 Call the API. */
  status = Os_Api_GetResource(ResID);

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdGetResource);

  return status;
}


/***********************************************************************************************************************
 *  Os_TrapCallReleaseResource()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallReleaseResource (
  ResourceType ResID
)
{
  Os_StatusType status;

  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdReleaseResource);

  /* #20 Call the API. */
  status = Os_Api_ReleaseResource(ResID);

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdReleaseResource);

  return status;
}


/***********************************************************************************************************************
 *  Os_TrapCallStartScheduleTableRel()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallStartScheduleTableRel (
  ScheduleTableType ScheduleTableID,
  TickType Offset
)
{
  Os_StatusType status;

  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdStartScheduleTableRel);

  /* #20 Call the API. */
  status = Os_Api_StartScheduleTableRel(ScheduleTableID, Offset);

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdStartScheduleTableRel);

  return status;
}


/***********************************************************************************************************************
 *  Os_TrapCallStartScheduleTableAbs()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallStartScheduleTableAbs (
  ScheduleTableType ScheduleTableID,
  TickType Start
)
{
  Os_StatusType status;

  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdStartScheduleTableAbs);

  /* #20 Call the API. */
  status = Os_Api_StartScheduleTableAbs(ScheduleTableID, Start);

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdStartScheduleTableAbs);

  return status;
}


/***********************************************************************************************************************
 *  Os_TrapCallStopScheduleTable()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallStopScheduleTable (
  ScheduleTableType ScheduleTableID
)
{
  Os_StatusType status;

  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdStopScheduleTable);

  /* #20 Call the API. */
  status = Os_Api_StopScheduleTable(ScheduleTableID);

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdStopScheduleTable);

  return status;
}


/***********************************************************************************************************************
 *  Os_TrapCallNextScheduleTable()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallNextScheduleTable (
  ScheduleTableType ScheduleTableID_From,
  ScheduleTableType ScheduleTableID_To
)
{
  Os_StatusType status;

  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdNextScheduleTable);

  /* #20 Call the API. */
  status = Os_Api_NextScheduleTable(ScheduleTableID_From, ScheduleTableID_To);

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdNextScheduleTable);

  return status;
}


/***********************************************************************************************************************
 *  Os_TrapCallStartScheduleTableSynchron()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallStartScheduleTableSynchron (
  ScheduleTableType ScheduleTableID
)
{
  Os_StatusType status;

  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdStartScheduleTableSynchron);

  /* #20 Call the API. */
  status = Os_Api_StartScheduleTableSynchron(ScheduleTableID);

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdStartScheduleTableSynchron);

  return status;
}


/***********************************************************************************************************************
 *  Os_TrapCallSyncScheduleTable()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallSyncScheduleTable (
  ScheduleTableType ScheduleTableID,
  TickType Value
)
{
  Os_StatusType status;

  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdSyncScheduleTable);

  /* #20 Call the API. */
  status = Os_Api_SyncScheduleTable(ScheduleTableID, Value);

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdSyncScheduleTable);

  return status;
}


/***********************************************************************************************************************
 *  Os_TrapCallSetScheduleTableAsync()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallSetScheduleTableAsync (
  ScheduleTableType ScheduleTableID
)
{
  Os_StatusType status;

  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdSetScheduleTableAsync);

  /* #20 Call the API. */
  status = Os_Api_SetScheduleTableAsync(ScheduleTableID);

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdSetScheduleTableAsync);

  return status;
}


/***********************************************************************************************************************
 *  Os_TrapCallGetScheduleTableStatus()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallGetScheduleTableStatus (
  ScheduleTableType ScheduleTableID,
  ScheduleTableStatusRefType ScheduleStatus
)
{
  Os_StatusType status;

  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdGetScheduleTableStatus);

  /* #20 Call the API. */
  status = Os_Api_GetScheduleTableStatus(ScheduleTableID, ScheduleStatus);                                              /* SBSW_OS_FC_PRECONDITION */

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdGetScheduleTableStatus);

  return status;
}


/***********************************************************************************************************************
 *  Os_TrapCallCallTrustedFunction()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallCallTrustedFunction (
  TrustedFunctionIndexType FunctionIndex,
  TrustedFunctionParameterRefType FunctionParams
)
{
  Os_StatusType status;

  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdCallTrustedFunction);

  /* #20 Call the API. */
  status = Os_Api_CallTrustedFunction(FunctionIndex, FunctionParams);                                                   /* SBSW_OS_FC_PRECONDITION */

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdCallTrustedFunction);

  return status;
}


/***********************************************************************************************************************
 *  Os_TrapCallCallNonTrustedFunction()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallCallNonTrustedFunction (
  Os_NonTrustedFunctionIndexType FunctionIndex,
  Os_NonTrustedFunctionParameterRefType FunctionParams
)
{
  Os_StatusType status;

  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdCallNonTrustedFunction);

  /* #20 Call the API. */
  status = Os_Api_CallNonTrustedFunction(FunctionIndex, FunctionParams);                                                /* SBSW_OS_FC_PRECONDITION */

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdCallNonTrustedFunction);

  return status;
}


/***********************************************************************************************************************
 *  Os_TrapCallCallFastTrustedFunction()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallCallFastTrustedFunction (
  Os_FastTrustedFunctionIndexType FunctionIndex,
  Os_FastTrustedFunctionParameterRefType FunctionParams
)
{
  Os_StatusType status;

  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdCallFastTrustedFunction);

  /* #20 Call the API. */
  status = Os_Api_CallFastTrustedFunction(FunctionIndex, FunctionParams);                                               /* SBSW_OS_FC_PRECONDITION */

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdCallFastTrustedFunction);

  return status;
}


/***********************************************************************************************************************
 *  Os_TrapCallGetSpinlock()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallGetSpinlock (
  const   Os_SpinlockConfigType OS_CONST * Spinlock
)
{
  Os_StatusType status;

  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdGetSpinlock);

  /* #20 Call the API. */
  status = Os_SpinlockGet(Spinlock);                                                                                    /* SBSW_OS_FC_PRECONDITION */

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdGetSpinlock);

  return status;
}


/***********************************************************************************************************************
 *  Os_TrapCallReleaseSpinlock()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallReleaseSpinlock (
  const   Os_SpinlockConfigType OS_CONST * Spinlock
)
{
  Os_StatusType status;

  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdReleaseSpinlock);

  /* #20 Call the API. */
  status = Os_SpinlockRelease(Spinlock);                                                                                /* SBSW_OS_FC_PRECONDITION */

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdReleaseSpinlock);

  return status;
}


/***********************************************************************************************************************
 *  Os_TrapCallTryToGetSpinlock()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallTryToGetSpinlock (
  const   Os_SpinlockConfigType OS_CONST * Spinlock,
  TryToGetSpinlockType* Success
)
{
  Os_StatusType status;

  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdTryToGetSpinlock);

  /* #20 Call the API. */
  status = Os_SpinlockTryGet(Spinlock, Success);                                                                        /* SBSW_OS_FC_PRECONDITION */

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdTryToGetSpinlock);

  return status;
}


/***********************************************************************************************************************
 *  Os_TrapCallGetTaskStackUsage()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline uint32   Os_TrapCallGetTaskStackUsage (
  TaskType TaskID
)
{
  uint32 result;

  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdGetTaskStackUsage);

  /* #20 Call the API. */
  result = Os_Api_GetTaskStackUsage(TaskID);

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdGetTaskStackUsage);

  return result;
}


/***********************************************************************************************************************
 *  Os_TrapCallGetISRStackUsage()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline uint32   Os_TrapCallGetISRStackUsage (
  ISRType IsrID
)
{
  uint32 result;

  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdGetISRStackUsage);

  /* #20 Call the API. */
  result = Os_Api_GetISRStackUsage(IsrID);

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdGetISRStackUsage);

  return result;
}


/***********************************************************************************************************************
 *  Os_TrapCallGetKernelStackUsage()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline uint32   Os_TrapCallGetKernelStackUsage (
  CoreIdType CoreID
)
{
  uint32 result;

  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdGetKernelStackUsage);

  /* #20 Call the API. */
  result = Os_Api_GetKernelStackUsage(CoreID);

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdGetKernelStackUsage);

  return result;
}


/***********************************************************************************************************************
 *  Os_TrapCallGetStartupHookStackUsage()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline uint32   Os_TrapCallGetStartupHookStackUsage (
  CoreIdType CoreID
)
{
  uint32 result;

  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdGetStartupHookStackUsage);

  /* #20 Call the API. */
  result = Os_Api_GetStartupHookStackUsage(CoreID);

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdGetStartupHookStackUsage);

  return result;
}


/***********************************************************************************************************************
 *  Os_TrapCallGetErrorHookStackUsage()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline uint32   Os_TrapCallGetErrorHookStackUsage (
  CoreIdType CoreID
)
{
  uint32 result;

  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdGetErrorHookStackUsage);

  /* #20 Call the API. */
  result = Os_Api_GetErrorHookStackUsage(CoreID);

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdGetErrorHookStackUsage);

  return result;
}


/***********************************************************************************************************************
 *  Os_TrapCallGetShutdownHookStackUsage()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline uint32   Os_TrapCallGetShutdownHookStackUsage (
  CoreIdType CoreID
)
{
  uint32 result;

  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdGetShutdownHookStackUsage);

  /* #20 Call the API. */
  result = Os_Api_GetShutdownHookStackUsage(CoreID);

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdGetShutdownHookStackUsage);

  return result;
}


/***********************************************************************************************************************
 *  Os_TrapCallGetProtectionHookStackUsage()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline uint32   Os_TrapCallGetProtectionHookStackUsage (
  CoreIdType CoreID
)
{
  uint32 result;

  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdGetProtectionHookStackUsage);

  /* #20 Call the API. */
  result = Os_Api_GetProtectionHookStackUsage(CoreID);

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdGetProtectionHookStackUsage);

  return result;
}


/***********************************************************************************************************************
 *  Os_TrapCallGetNonTrustedFunctionStackUsage()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline uint32   Os_TrapCallGetNonTrustedFunctionStackUsage (
  Os_NonTrustedFunctionIndexType FunctionIndex
)
{
  uint32 result;

  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdGetNonTrustedFunctionStackUsage);

  /* #20 Call the API. */
  result = Os_Api_GetNonTrustedFunctionStackUsage(FunctionIndex);

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdGetNonTrustedFunctionStackUsage);

  return result;
}


/***********************************************************************************************************************
 *  Os_TrapCallBarrierSynchronize()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallBarrierSynchronize (
  Os_BarrierIdType BarrierID
)
{
  Os_StatusType status;

  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdBarrierSynchronize);

  /* #20 Call the API. */
  status = Os_Api_BarrierSynchronize(BarrierID);

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdBarrierSynchronize);

  return status;
}


/***********************************************************************************************************************
 *  Os_TrapCallActivateTask()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallActivateTask (
  TaskType TaskID
)
{
  Os_StatusType status;

  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdActivateTask);

  /* #20 Call the API. */
  status = Os_Api_ActivateTask(TaskID);

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdActivateTask);

  return status;
}


/***********************************************************************************************************************
 *  Os_TrapCallTerminateTask()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallTerminateTask (
  void
)
{
  Os_StatusType status;

  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdTerminateTask);

  /* #20 Call the API. */
  status = Os_Api_TerminateTask();

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdTerminateTask);

  return status;
}


/***********************************************************************************************************************
 *  Os_TrapCallChainTask()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallChainTask (
  TaskType TaskID
)
{
  Os_StatusType status;

  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdChainTask);

  /* #20 Call the API. */
  status = Os_Api_ChainTask(TaskID);

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdChainTask);

  return status;
}


/***********************************************************************************************************************
 *  Os_TrapCallSchedule()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallSchedule (
  void
)
{
  Os_StatusType status;

  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdSchedule);

  /* #20 Call the API. */
  status = Os_Api_Schedule();

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdSchedule);

  return status;
}


/***********************************************************************************************************************
 *  Os_TrapCallGetTaskID()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallGetTaskID (
  TaskRefType TaskID
)
{
  Os_StatusType status;

  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdGetTaskID);

  /* #20 Call the API. */
  status = Os_Api_GetTaskID(TaskID);                                                                                    /* SBSW_OS_FC_PRECONDITION */

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdGetTaskID);

  return status;
}


/***********************************************************************************************************************
 *  Os_TrapCallGetTaskState()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline Os_StatusType   Os_TrapCallGetTaskState (
  TaskType TaskID,
  TaskStateRefType State
)
{
  Os_StatusType status;

  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdGetTaskState);

  /* #20 Call the API. */
  status = Os_Api_GetTaskState(TaskID, State);                                                                          /* SBSW_OS_FC_PRECONDITION */

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdGetTaskState);

  return status;
}


/***********************************************************************************************************************
 *  Os_TrapCallGetCoreStartState()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline void   Os_TrapCallGetCoreStartState (
  CoreIdType CoreID,
  Os_CoreStartStateType *CoreState,
  StatusType *Status
)
{
  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdGetCoreStartState);

  /* #20 Call the API. */
  Os_Api_GetCoreStartState(CoreID, CoreState, Status);                                                                  /* SBSW_OS_FC_PRECONDITION */

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdGetCoreStartState);
}


/***********************************************************************************************************************
 *  GLOBAL FUNCTIONS
 **********************************************************************************************************************/

/***********************************************************************************************************************
 *  Os_TrapSpinlockGet()
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
 */
Os_StatusType     Os_TrapSpinlockGet
(
  const   Os_SpinlockConfigType OS_CONST * Spinlock
)
{
  Os_StatusType status;

  /* #10 If a trap is needed: */
  if(Os_TrapIsTrapForWriteAccessRequired() != 0u)                                                                       /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_SPINLOCKGET;
    packet.Parameters[OS_TRAPPARAMINDEX_1].ParamSpinlockConfigRefType = Spinlock;                                       /* SBSW_OS_PWA_LOCAL */

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */

    /* #40 Unmarshal return value. */
    status = packet.ReturnValue.ReturnOs_StatusType;
  }
  /* #50 Otherwise: */
  else
  {
    /* #60 Call the API directly. */
    status = Os_TrapCallGetSpinlock(Spinlock);                                                                          /* SBSW_OS_FC_PRECONDITION */
  }

  return status;
}


/***********************************************************************************************************************
 *  Os_TrapSpinlockRelease()
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
 */
Os_StatusType     Os_TrapSpinlockRelease
(
  const   Os_SpinlockConfigType OS_CONST * Spinlock
)
{
  Os_StatusType status;

  /* #10 If a trap is needed: */
  if(Os_TrapIsTrapForWriteAccessRequired() != 0u)                                                                       /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */ /* COV_OS_HALPLATFORMTESTEDASMULTICORE */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_SPINLOCKRELEASE;
    packet.Parameters[OS_TRAPPARAMINDEX_1].ParamSpinlockConfigRefType = Spinlock;                                       /* SBSW_OS_PWA_LOCAL */

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */

    /* #40 Unmarshal return value. */
    status = packet.ReturnValue.ReturnOs_StatusType;
  }
  /* #50 Otherwise: */
  else
  {
    /* #60 Call the API directly. */
    status = Os_TrapCallReleaseSpinlock(Spinlock);                                                                      /* SBSW_OS_FC_PRECONDITION */
  }

  return status;
}


/***********************************************************************************************************************
 *  Os_TrapSpinlockTryGet()
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
 */
Os_StatusType     Os_TrapSpinlockTryGet
(
  const   Os_SpinlockConfigType OS_CONST * Spinlock,
  TryToGetSpinlockType* Success
)
{
  Os_StatusType status;

  /* #10 If a trap is needed: */
  if(Os_TrapIsTrapForWriteAccessRequired() != 0u)                                                                       /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
  {
    Os_TrapPacketType packet;
    TryToGetSpinlockType userspaceSuccess;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_SPINLOCKTRYGET;
    packet.Parameters[OS_TRAPPARAMINDEX_1].ParamSpinlockConfigRefType = Spinlock;                                       /* SBSW_OS_PWA_LOCAL */
    packet.Parameters[OS_TRAPPARAMINDEX_2].ParamApiType.ParamTryToGetSpinlockRefType =
                                                                (TryToGetSpinlockType*)&userspaceSuccess;               /* SBSW_OS_PWA_LOCAL */

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */

    /* #40 Unmarshal return value. */
    (*Success) = *((volatile TryToGetSpinlockType*)&userspaceSuccess);                                                  /* SBSW_OS_TRAP_API_USERPOINTER_002 */
    status = packet.ReturnValue.ReturnOs_StatusType;
  }
  /* #50 Otherwise: */
  else
  {
    /* #60 Call the API directly. */
    status = Os_TrapCallTryToGetSpinlock(Spinlock, Success);                                                            /* SBSW_OS_FC_PRECONDITION */
  }

  return status;
}


/***********************************************************************************************************************
 *  Os_TrapIoc08Receive()
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
 */
Std_ReturnType     Os_TrapIoc08Receive
(
  boolean IsTrapRequired,
  const   Os_Ioc08ReceiveConfigType OS_CONST * Ioc,
    uint8   * Data
)
{
  Std_ReturnType result;

  /* #10 If trap is required: */
  if(Os_TrapIsTrapRequired(IsTrapRequired) != 0u)                                                                       /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_IOC08RECEIVE;
    packet.Parameters[OS_TRAPPARAMINDEX_1].ParamIoc08ReceiveConfigRefType = Ioc;                                        /* SBSW_OS_PWA_LOCAL */
    packet.Parameters[OS_TRAPPARAMINDEX_2].ParamUint8RefType = Data;                                                    /* SBSW_OS_PWA_LOCAL */

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */

    /* #40 Unmarshal return value. */
    result = packet.ReturnValue.ReturnStd_ReturnType;
  }
  /* #50 Otherwise: */
  else
  {
    /* #60 Call the API directly. */
    result = Os_Ioc08Receive(Ioc, Data);                                                                                /* SBSW_OS_FC_PRECONDITION */
  }

  return result;
}


/***********************************************************************************************************************
 *  Os_TrapIoc8EmptyQueue()
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
 */
Std_ReturnType     Os_TrapIoc08EmptyQueue
(
  boolean IsTrapRequired,
  const   Os_Ioc08ReceiveConfigType OS_CONST * Ioc
)
{
  Std_ReturnType result;

  /* #10 If trap is required: */
  if(Os_TrapIsTrapRequired(IsTrapRequired) != 0u)                                                                       /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_IOC08EMPTYQUEUE;
    packet.Parameters[OS_TRAPPARAMINDEX_1].ParamIoc08ReceiveConfigRefType = Ioc;                                        /* SBSW_OS_PWA_LOCAL */

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */

    /* #40 Unmarshal return value. */
    result = packet.ReturnValue.ReturnStd_ReturnType;
  }
  /* #50 Otherwise: */
  else
  {
    /* #60 Call the API directly. */
    result = Os_Ioc08EmptyQueue(Ioc);                                                                                   /* SBSW_OS_FC_PRECONDITION */
  }

  return result;
}


/***********************************************************************************************************************
 *  Os_TrapIoc8Send()
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
 */
Std_ReturnType     Os_TrapIoc08Send
(
  boolean IsTrapRequired,
  const   Os_Ioc08SendConfigType OS_CONST * Ioc,
  uint8 Data
)
{
  Std_ReturnType result;

  /* #10 If trap is required: */
  if(Os_TrapIsTrapRequired(IsTrapRequired) != 0u)                                                                       /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_IOC08SEND;
    packet.Parameters[OS_TRAPPARAMINDEX_1].ParamIoc08SendConfigRefType = Ioc;                                           /* SBSW_OS_PWA_LOCAL */
    packet.Parameters[OS_TRAPPARAMINDEX_2].ParamUint8Type = Data;                                                       /* SBSW_OS_PWA_LOCAL */

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */

    /* #40 Unmarshal return value. */
    result = packet.ReturnValue.ReturnStd_ReturnType;
  }
  /* #50 Otherwise: */
  else
  {
    /* #60 Call the API directly. */
    result = Os_Ioc08Send(Ioc, Data);                                                                                   /* SBSW_OS_FC_PRECONDITION */
  }

  return result;
}


/***********************************************************************************************************************
 *  Os_TrapIoc16Receive()
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
 */
Std_ReturnType     Os_TrapIoc16Receive
(
  boolean IsTrapRequired,
  const   Os_Ioc16ReceiveConfigType OS_CONST * Ioc,
    uint16   * Data
)
{
  Std_ReturnType result;

  /* #10 If trap is required: */
  if(Os_TrapIsTrapRequired(IsTrapRequired) != 0u)                                                                       /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_IOC16RECEIVE;
    packet.Parameters[OS_TRAPPARAMINDEX_1].ParamIoc16ReceiveConfigRefType = Ioc;                                        /* SBSW_OS_PWA_LOCAL */
    packet.Parameters[OS_TRAPPARAMINDEX_2].ParamUint16RefType = Data;                                                   /* SBSW_OS_PWA_LOCAL */

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */

    /* #40 Unmarshal return value. */
    result = packet.ReturnValue.ReturnStd_ReturnType;
  }
  /* #50 Otherwise: */
  else
  {
    /* #60 Call the API directly. */
    result = Os_Ioc16Receive(Ioc, Data);                                                                                /* SBSW_OS_FC_PRECONDITION */
  }

  return result;
}


/***********************************************************************************************************************
 *  Os_TrapIoc16EmptyQueue()
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
 */
Std_ReturnType     Os_TrapIoc16EmptyQueue
(
  boolean IsTrapRequired,
  const   Os_Ioc16ReceiveConfigType OS_CONST * Ioc
)
{
  Std_ReturnType result;

  /* #10 If trap is required: */
  if(Os_TrapIsTrapRequired(IsTrapRequired) != 0u)                                                                       /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_IOC16EMPTYQUEUE;
    packet.Parameters[OS_TRAPPARAMINDEX_1].ParamIoc16ReceiveConfigRefType = Ioc;                                        /* SBSW_OS_PWA_LOCAL */

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */

    /* #40 Unmarshal return value. */
    result = packet.ReturnValue.ReturnStd_ReturnType;
  }
  /* #50 Otherwise: */
  else
  {
    /* #60 Call the API directly. */
    result = Os_Ioc16EmptyQueue(Ioc);                                                                                   /* SBSW_OS_FC_PRECONDITION */
  }

  return result;
}


/***********************************************************************************************************************
 *  Os_TrapIoc16Send()
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
 */
Std_ReturnType     Os_TrapIoc16Send
(
  boolean IsTrapRequired,
  const   Os_Ioc16SendConfigType OS_CONST * Ioc,
  uint16 Data
)
{
  Std_ReturnType result;

  /* #10 If trap is required: */
  if(Os_TrapIsTrapRequired(IsTrapRequired) != 0u)                                                                       /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */ /* COV_OS_HALPLATFORMTESTEDASMULTICORE */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_IOC16SEND;
    packet.Parameters[OS_TRAPPARAMINDEX_1].ParamIoc16SendConfigRefType = Ioc;                                           /* SBSW_OS_PWA_LOCAL */
    packet.Parameters[OS_TRAPPARAMINDEX_2].ParamUint16Type = Data;                                                      /* SBSW_OS_PWA_LOCAL */

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */

    /* #40 Unmarshal return value. */
    result = packet.ReturnValue.ReturnStd_ReturnType;
  }
  /* #50 Otherwise: */
  else
  {
    /* #60 Call the API directly. */
    result = Os_Ioc16Send(Ioc, Data);                                                                                   /* SBSW_OS_FC_PRECONDITION */
  }

  return result;
}


/***********************************************************************************************************************
 *  Os_TrapIoc32Receive()
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
 */
Std_ReturnType     Os_TrapIoc32Receive
(
  boolean IsTrapRequired,
  const   Os_Ioc32ReceiveConfigType OS_CONST * Ioc,
    uint32   * Data
)
{
  Std_ReturnType result;

  /* #10 If trap is required: */
  if(Os_TrapIsTrapRequired(IsTrapRequired) != 0u)                                                                       /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_IOC32RECEIVE;
    packet.Parameters[OS_TRAPPARAMINDEX_1].ParamIoc32ReceiveConfigRefType = Ioc;                                        /* SBSW_OS_PWA_LOCAL */
    packet.Parameters[OS_TRAPPARAMINDEX_2].ParamUint32RefType = Data;                                                   /* SBSW_OS_PWA_LOCAL */

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */

    /* #40 Unmarshal return value. */
    result = packet.ReturnValue.ReturnStd_ReturnType;
  }
  /* #50 Otherwise: */
  else
  {
    /* #60 Call the API directly. */
    result = Os_Ioc32Receive(Ioc, Data);                                                                                /* SBSW_OS_FC_PRECONDITION */
  }

  return result;
}


/***********************************************************************************************************************
 *  Os_TrapIoc32EmptyQueue()
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
 */
Std_ReturnType     Os_TrapIoc32EmptyQueue
(
  boolean IsTrapRequired,
  const   Os_Ioc32ReceiveConfigType OS_CONST * Ioc
)
{
  Std_ReturnType result;

  /* #10 If trap is required: */
  if(Os_TrapIsTrapRequired(IsTrapRequired) != 0u)                                                                       /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_IOC32EMPTYQUEUE;
    packet.Parameters[OS_TRAPPARAMINDEX_1].ParamIoc32ReceiveConfigRefType = Ioc;                                        /* SBSW_OS_PWA_LOCAL */

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */

    /* #40 Unmarshal return value. */
    result = packet.ReturnValue.ReturnStd_ReturnType;
  }
  /* #50 Otherwise: */
  else
  {
    /* #60 Call the API directly. */
    result = Os_Ioc32EmptyQueue(Ioc);                                                                                   /* SBSW_OS_FC_PRECONDITION */
  }

  return result;
}


/***********************************************************************************************************************
 *  Os_TrapIoc32Send()
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
 */
Std_ReturnType     Os_TrapIoc32Send
(
  boolean IsTrapRequired,
  const   Os_Ioc32SendConfigType OS_CONST * Ioc,
  uint32 Data
)
{
  Std_ReturnType result;

  /* #10 If trap is required: */
  if(Os_TrapIsTrapRequired(IsTrapRequired) != 0u)                                                                       /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_IOC32SEND;
    packet.Parameters[OS_TRAPPARAMINDEX_1].ParamIoc32SendConfigRefType = Ioc;                                           /* SBSW_OS_PWA_LOCAL */
    packet.Parameters[OS_TRAPPARAMINDEX_2].ParamUint32Type = Data;                                                      /* SBSW_OS_PWA_LOCAL */

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */

    /* #40 Unmarshal return value. */
    result =  packet.ReturnValue.ReturnStd_ReturnType;
  }
  /* #50 Otherwise: */
  else
  {
    /* #60 Call the API directly. */
    result = Os_Ioc32Send(Ioc, Data);                                                                                   /* SBSW_OS_FC_PRECONDITION */
  }

  return result;
}


/***********************************************************************************************************************
 *  Os_TrapIocRefReceive()
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
 */
Std_ReturnType     Os_TrapIocRefReceive
(
  boolean IsTrapRequired,
  const   Os_IocRefReceiveConfigType OS_CONST * Ioc,
    void   * Data
)
{
  Std_ReturnType result;

  /* #10 If trap is required: */
  if(Os_TrapIsTrapRequired(IsTrapRequired) != 0u)                                                                       /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */ /* COV_OS_HALPLATFORMTESTEDASMULTICORE */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_IOCREFRECEIVE;
    packet.Parameters[OS_TRAPPARAMINDEX_1].ParamIocRefReceiveConfigRefType = Ioc;                                       /* SBSW_OS_PWA_LOCAL */
    packet.Parameters[OS_TRAPPARAMINDEX_2].ParamVoidRefType = Data;                                                     /* SBSW_OS_PWA_LOCAL */

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */

    /* #40 Unmarshal return value. */
    result = packet.ReturnValue.ReturnStd_ReturnType;
  }
  /* #50 Otherwise: */
  else
  {
    /* #60 Call the API directly. */
    result = Os_IocRefReceive(Ioc, Data);                                                                               /* SBSW_OS_FC_PRECONDITION */
  }

  return result;
}


/***********************************************************************************************************************
 *  Os_TrapIocRefEmptyQueue()
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
 */
Std_ReturnType     Os_TrapIocRefEmptyQueue
(
  boolean IsTrapRequired,
  const   Os_IocRefReceiveConfigType OS_CONST * Ioc
)
{
  Std_ReturnType result;

  /* #10 If trap is required: */
  if(Os_TrapIsTrapRequired(IsTrapRequired) != 0u)                                                                       /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */ /* COV_OS_HALPLATFORMTESTEDASMULTICORE */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_IOCREFEMPTYQUEUE;
    packet.Parameters[OS_TRAPPARAMINDEX_1].ParamIocRefReceiveConfigRefType = Ioc;                                       /* SBSW_OS_PWA_LOCAL */

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */

    /* #40 Unmarshal return value. */
    result = packet.ReturnValue.ReturnStd_ReturnType;
  }
  /* #50 Otherwise: */
  else
  {
    /* #60 Call the API directly. */
    result = Os_IocRefEmptyQueue(Ioc);                                                                                  /* SBSW_OS_FC_PRECONDITION */
  }

  return result;
}


/***********************************************************************************************************************
 *  Os_TrapIocRefSend()
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
 */
Std_ReturnType     Os_TrapIocRefSend
(
  boolean IsTrapRequired,
  const   Os_IocRefSendConfigType OS_CONST * Ioc,
  const   void   * Data
)
{
  Std_ReturnType result;

  /* #10 If trap is required: */
  if(Os_TrapIsTrapRequired(IsTrapRequired) != 0u)                                                                       /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */ /* COV_OS_HALPLATFORMTESTEDASMULTICORE */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_IOCREFSEND;
    packet.Parameters[OS_TRAPPARAMINDEX_1].ParamIocRefSendConfigRefType = Ioc;                                          /* SBSW_OS_PWA_LOCAL */
    packet.Parameters[OS_TRAPPARAMINDEX_2].ParamVoidConfigRefType = Data;                                               /* SBSW_OS_PWA_LOCAL */

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */

    /* #40 Unmarshal return value. */
    result = packet.ReturnValue.ReturnStd_ReturnType;
  }
  /* #50 Otherwise: */
  else
  {
    /* #60 Call the API directly. */
    result = Os_IocRefSend(Ioc, Data);                                                                                  /* SBSW_OS_FC_PRECONDITION */
  }

  return result;
}


/***********************************************************************************************************************
 *  Os_TrapIocRead()
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
 */
Std_ReturnType     Os_TrapIocRead
(
  boolean IsTrapRequired,
  const   Os_IocReadConfigType OS_CONST * Ioc,
    void   * Data
)
{
  Std_ReturnType result;

  /* #10 If trap is required: */
  if(Os_TrapIsTrapRequired(IsTrapRequired) != 0u)                                                                       /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */ /* COV_OS_HALPLATFORMTESTEDASMULTICORE */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_IOCREAD;
    packet.Parameters[OS_TRAPPARAMINDEX_1].ParamIocReadConfigRefType = Ioc;                                             /* SBSW_OS_PWA_LOCAL */
    packet.Parameters[OS_TRAPPARAMINDEX_2].ParamVoidRefType = Data;                                                     /* SBSW_OS_PWA_LOCAL */

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */

    /* #40 Unmarshal return value. */
    result = packet.ReturnValue.ReturnStd_ReturnType;
  }
  /* #50 Otherwise: */
  else
  {
    /* #60 Call the API directly. */
    result = Os_IocRead(Ioc, Data);                                                                                     /* SBSW_OS_FC_PRECONDITION */
  }

  return result;
}


/***********************************************************************************************************************
 *  Os_TrapIocWrite()
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
 */
Std_ReturnType     Os_TrapIocWrite
(
  boolean IsTrapRequired,
  const   Os_IocWriteConfigType OS_CONST * Ioc,
  const   void   * Data
)
{
  Std_ReturnType result;

  /* #10 If trap is required: */
  if(Os_TrapIsTrapRequired(IsTrapRequired) != 0u)                                                                       /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */ /* COV_OS_HALPLATFORMTESTEDASMULTICORE */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_IOCWRITE;
    packet.Parameters[OS_TRAPPARAMINDEX_1].ParamIocWriteConfigRefType = Ioc;                                            /* SBSW_OS_PWA_LOCAL */
    packet.Parameters[OS_TRAPPARAMINDEX_2].ParamVoidConfigRefType = Data;                                               /* SBSW_OS_PWA_LOCAL */

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */

    /* #40 Unmarshal return value. */
    result = packet.ReturnValue.ReturnStd_ReturnType;
  }
  /* #50 Otherwise: */
  else
  {
    /* #60 Call the API directly. */
    result = Os_IocWrite(Ioc, Data);                                                                                    /* SBSW_OS_FC_PRECONDITION */
  }

  return result;
}


/***********************************************************************************************************************
 *  Os_TrapIoc08MultiReceive()
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
 */
Std_ReturnType     Os_TrapIoc08MultiReceive                                                                  /* COV_OS_HALPLATFORMTESTEDASMULTICORE */
(
  boolean IsTrapRequired,
  const   Os_Ioc08MultiReceiveConfigType OS_CONST * Ioc,
    uint8   * Data
)
{
  Std_ReturnType result;

  /* #10 If trap is required: */
  if(Os_TrapIsTrapRequired(IsTrapRequired) != 0u)                                                                       /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */ /* COV_OS_HALPLATFORM_NEED_NO_TRAP_IOC_MULTIQUEUE */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_IOC08MULTIRECEIVE;
    packet.Parameters[OS_TRAPPARAMINDEX_1].ParamIoc08MultiReceiveConfigRefType = Ioc;                                   /* SBSW_OS_PWA_LOCAL */
    packet.Parameters[OS_TRAPPARAMINDEX_2].ParamVoidConfigRefType = Data;                                               /* PRQA S 0315 */ /* MD_Os_Dir1.1_0315 */ /* SBSW_OS_PWA_LOCAL */

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */

    /* #40 Unmarshal return value. */
    result = packet.ReturnValue.ReturnStd_ReturnType;
  }
  /* #50 Otherwise: */
  else
  {
    /* #60 Call the API directly. */
    result = Os_Ioc08MultiReceive(Ioc, Data);                                                                           /* SBSW_OS_FC_PRECONDITION */
  }

  return result;
}


/***********************************************************************************************************************
 *  Os_TrapIoc08EmptyMultiQueue()
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
 */
Std_ReturnType     Os_TrapIoc08EmptyMultiQueue                                                               /* COV_OS_HALPLATFORMTESTEDASMULTICORE */
(
  boolean IsTrapRequired,
  const   Os_Ioc08MultiReceiveConfigType OS_CONST * Ioc
)
{
  Std_ReturnType result;

  /* #10 If trap is required: */
  if(Os_TrapIsTrapRequired(IsTrapRequired) != 0u)                                                                       /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */ /* COV_OS_HALPLATFORM_NEED_NO_TRAP_IOC_MULTIQUEUE */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_IOC08EMPTYMULTIQUEUE;
    packet.Parameters[OS_TRAPPARAMINDEX_1].ParamIoc08MultiReceiveConfigRefType = Ioc;                                   /* SBSW_OS_PWA_LOCAL */

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */

    /* #40 Unmarshal return value. */
    result = packet.ReturnValue.ReturnStd_ReturnType;
  }
  /* #50 Otherwise: */
  else
  {
    /* #60 Call the API directly. */
    result = Os_Ioc08EmptyMultiQueue(Ioc);                                                                              /* SBSW_OS_FC_PRECONDITION */
  }

  return result;
}


/***********************************************************************************************************************
 *  Os_TrapIoc16MultiReceive()
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
 */
Std_ReturnType     Os_TrapIoc16MultiReceive                                                                  /* COV_OS_HALPLATFORMTESTEDASMULTICORE */
(
  boolean IsTrapRequired,
  const   Os_Ioc16MultiReceiveConfigType OS_CONST * Ioc,
    uint16   * Data
)
{
  Std_ReturnType result;

  /* #10 If trap is required: */
  if(Os_TrapIsTrapRequired(IsTrapRequired) != 0u)                                                                       /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */ /* COV_OS_HALPLATFORM_NEED_NO_TRAP_IOC_MULTIQUEUE */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_IOC16MULTIRECEIVE;
    packet.Parameters[OS_TRAPPARAMINDEX_1].ParamIoc16MultiReceiveConfigRefType = Ioc;                                   /* SBSW_OS_PWA_LOCAL */
    packet.Parameters[OS_TRAPPARAMINDEX_2].ParamVoidConfigRefType = Data;                                               /* PRQA S 0315 */ /* MD_Os_Dir1.1_0315 */ /* SBSW_OS_PWA_LOCAL */

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */

    /* #40 Unmarshal return value. */
    result = packet.ReturnValue.ReturnStd_ReturnType;
  }
  /* #50 Otherwise: */
  else
  {
    /* #60 Call the API directly. */
    result = Os_Ioc16MultiReceive(Ioc, Data);                                                                           /* SBSW_OS_FC_PRECONDITION */
  }

  return result;
}


/***********************************************************************************************************************
 *  Os_TrapIoc16EmptyMultiQueue()
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
 */
Std_ReturnType     Os_TrapIoc16EmptyMultiQueue                                                               /* COV_OS_HALPLATFORMTESTEDASMULTICORE */
(
  boolean IsTrapRequired,
  const   Os_Ioc16MultiReceiveConfigType OS_CONST * Ioc
)
{
  Std_ReturnType result;

  /* #10 If trap is required: */
  if(Os_TrapIsTrapRequired(IsTrapRequired) != 0u)                                                                       /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */ /* COV_OS_HALPLATFORM_NEED_NO_TRAP_IOC_MULTIQUEUE */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_IOC16EMPTYMULTIQUEUE;
    packet.Parameters[OS_TRAPPARAMINDEX_1].ParamIoc16MultiReceiveConfigRefType = Ioc;                                   /* SBSW_OS_PWA_LOCAL */

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */

    /* #40 Unmarshal return value. */
    result = packet.ReturnValue.ReturnStd_ReturnType;
  }
  /* #50 Otherwise: */
  else
  {
    /* #60 Call the API directly. */
    result = Os_Ioc16EmptyMultiQueue(Ioc);                                                                              /* SBSW_OS_FC_PRECONDITION */
  }

  return result;
}


/***********************************************************************************************************************
 *  Os_TrapIoc32MultiReceive()
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
 */
Std_ReturnType     Os_TrapIoc32MultiReceive                                                                  /* COV_OS_HALPLATFORMTESTEDASMULTICORE */
(
  boolean IsTrapRequired,
  const   Os_Ioc32MultiReceiveConfigType OS_CONST * Ioc,
    uint32   * Data
)
{
  Std_ReturnType result;

  /* #10 If trap is required: */
  if(Os_TrapIsTrapRequired(IsTrapRequired) != 0u)                                                                       /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */ /* COV_OS_HALPLATFORM_NEED_NO_TRAP_IOC_MULTIQUEUE */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_IOC32MULTIRECEIVE;
    packet.Parameters[OS_TRAPPARAMINDEX_1].ParamIoc32MultiReceiveConfigRefType = Ioc;                                   /* SBSW_OS_PWA_LOCAL */
    packet.Parameters[OS_TRAPPARAMINDEX_2].ParamVoidConfigRefType = Data;                                               /* PRQA S 0315 */ /* MD_Os_Dir1.1_0315 */ /* SBSW_OS_PWA_LOCAL */

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */

    /* #40 Unmarshal return value. */
    result = packet.ReturnValue.ReturnStd_ReturnType;
  }
  /* #50 Otherwise: */
  else
  {
    /* #60 Call the API directly. */
    result = Os_Ioc32MultiReceive(Ioc, Data);                                                                           /* SBSW_OS_FC_PRECONDITION */
  }

  return result;
}

/***********************************************************************************************************************
 *  Os_TrapIoc32EmptyMultiQueue()
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
 */
Std_ReturnType     Os_TrapIoc32EmptyMultiQueue                                                               /* COV_OS_HALPLATFORMTESTEDASMULTICORE */
(
  boolean IsTrapRequired,
  const   Os_Ioc32MultiReceiveConfigType OS_CONST * Ioc
)
{
  Std_ReturnType result;

  /* #10 If trap is required: */
  if(Os_TrapIsTrapRequired(IsTrapRequired) != 0u)                                                                       /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */ /* COV_OS_HALPLATFORM_NEED_NO_TRAP_IOC_MULTIQUEUE */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_IOC32EMPTYMULTIQUEUE;
    packet.Parameters[OS_TRAPPARAMINDEX_1].ParamIoc32MultiReceiveConfigRefType = Ioc;                                   /* SBSW_OS_PWA_LOCAL */

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */

    /* #40 Unmarshal return value. */
    result = packet.ReturnValue.ReturnStd_ReturnType;
  }
  /* #50 Otherwise: */
  else
  {
    /* #60 Call the API directly. */
    result = Os_Ioc32EmptyMultiQueue(Ioc);                                                                              /* SBSW_OS_FC_PRECONDITION */
  }

  return result;
}

/***********************************************************************************************************************
 *  Os_TrapIocRefMultiReceive()
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
 */
Std_ReturnType     Os_TrapIocRefMultiReceive                                                                 /* COV_OS_HALPLATFORMTESTEDASMULTICORE */
(
  boolean IsTrapRequired,
  const   Os_IocRefMultiReceiveConfigType OS_CONST * Ioc,
    void   * Data
)
{
  Std_ReturnType result;

  /* #10 If trap is required: */
  if(Os_TrapIsTrapRequired(IsTrapRequired) != 0u)                                                                       /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */ /* COV_OS_HALPLATFORM_NEED_NO_TRAP_IOC_MULTIQUEUE */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_IOCREFMULTIRECEIVE;
    packet.Parameters[OS_TRAPPARAMINDEX_1].ParamIocRefMultiReceiveConfigRefType = Ioc;                                  /* SBSW_OS_PWA_LOCAL */
    packet.Parameters[OS_TRAPPARAMINDEX_2].ParamVoidConfigRefType = Data;                                               /* SBSW_OS_PWA_LOCAL */

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */

    /* #40 Unmarshal return value. */
    result = packet.ReturnValue.ReturnStd_ReturnType;
  }
  /* #50 Otherwise: */
  else
  {
    /* #60 Call the API directly. */
    result = Os_IocRefMultiReceive(Ioc, Data);                                                                          /* SBSW_OS_FC_PRECONDITION */
  }

  return result;
}


/***********************************************************************************************************************
 *  Os_TrapIocRefEmptyMultiQueue()
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
 */
Std_ReturnType     Os_TrapIocRefEmptyMultiQueue                                                              /* COV_OS_HALPLATFORMTESTEDASMULTICORE */
(
  boolean IsTrapRequired,
  const   Os_IocRefMultiReceiveConfigType OS_CONST * Ioc
)
{
  Std_ReturnType result;

  /* #10 If trap is required: */
  if(Os_TrapIsTrapRequired(IsTrapRequired) != 0u)                                                                       /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */ /* COV_OS_HALPLATFORM_NEED_NO_TRAP_IOC_MULTIQUEUE */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_IOCREFEMPTYMULTIQUEUE;
    packet.Parameters[OS_TRAPPARAMINDEX_1].ParamIocRefMultiReceiveConfigRefType = Ioc;                                  /* SBSW_OS_PWA_LOCAL */

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */

    /* #40 Unmarshal return value. */
    result = packet.ReturnValue.ReturnStd_ReturnType;
  }
  /* #50 Otherwise: */
  else
  {
    /* #60 Call the API directly. */
    result = Os_IocRefEmptyMultiQueue(Ioc);                                                                             /* SBSW_OS_FC_PRECONDITION */
  }

  return result;
}


/***********************************************************************************************************************
 *  Os_TrapErrApplicationError()
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
void     Os_TrapErrApplicationError
(
  OSServiceIdType ServiceId,
  Os_StatusType Status,
    Os_ErrorParamsType OS_VAR_NOINIT * Parameters
)
{
  /* #10 If a trap is needed: */
  if(Os_TrapIsTrapForWriteAccessRequired() != 0u)                                                                       /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_ERRAPPLICATIONERROR;
    packet.Parameters[OS_TRAPPARAMINDEX_1].ParamOSServiceIdType = ServiceId;                                            /* SBSW_OS_PWA_LOCAL */
    packet.Parameters[OS_TRAPPARAMINDEX_2].ParamOs_StatusType = Status;                                                 /* SBSW_OS_PWA_LOCAL */
    packet.Parameters[OS_TRAPPARAMINDEX_3].ParamOs_ErrorParamsRefType = Parameters;                                     /* SBSW_OS_PWA_LOCAL */

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */
  }
  /* #40 Otherwise: */
  else
  {
    /* #50 Call the API directly. */
    Os_ErrApplicationError(ServiceId, Status, Parameters);                                                              /* SBSW_OS_FC_PRECONDITION */
  }
}


/***********************************************************************************************************************
 *  Os_TrapServiceReturn()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 */
OS_NORETURN void   Os_TrapServiceReturn (void)
{
  Os_TrapPacketType packet;

  /*! Internal comment removed.
 *
 * */

  /* #10 Marshal API data. */
  packet.TrapId = OS_TRAPID_SERVICERETURN;

  /* #20 Trigger trap. */
  Os_Hal_Trap(&packet);                                                                                                 /* SBSW_OS_FC_POINTER2LOCAL */

  Os_ErrKernelPanic();
}


/***********************************************************************************************************************
 *  Os_TrapIsrEpilogue()
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
OS_NORETURN void   Os_TrapIsrEpilogue (void)
{
  /* #10 If a trap is needed: */
  if(Os_TrapIsTrapForWriteAccessRequired() != 0u)                                                                       /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_ISREPILOGUE;

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */

  }
  /* #40 Otherwise: */
  else
  {
    /* #50 Call the API directly. */
    Os_IsrEpilogue();
  }

  Os_ErrKernelPanic();
}


/***********************************************************************************************************************
 *  Os_TrapTaskMissingTerminateTask()
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
OS_NORETURN void   Os_TrapTaskMissingTerminateTask (void)
{
  /* #10 If a trap is needed: */
  if(Os_TrapIsTrapForWriteAccessRequired() != 0u)                                                                       /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_TASKMISSINGTERMINATETASK;

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */
  }
  /* #40 Otherwise: */
  else
  {
    /* #50 Call the API directly. */
    Os_TaskMissingTerminateTask();
  }

  Os_ErrKernelPanic();
}


/***********************************************************************************************************************
 *  Os_TrapHookReturn()
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
OS_NORETURN void   Os_TrapHookReturn (void)
{
  /* #10 If a trap is needed: */
  if(Os_TrapIsTrapForWriteAccessRequired() != 0u)                                                                       /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_HOOKRETURN;

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */
  }
  /* #40 Otherwise: */
  else
  {
    /* #50 Call the API directly. */
    Os_HookReturn();
  }

  Os_ErrKernelPanic();
}


/***********************************************************************************************************************
 *  Os_TrapCoreGetThread()
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
 */
const OS_CONST Os_ThreadConfigType       * Os_TrapCoreGetThread
(
  void
)
{
  const   Os_ThreadConfigType OS_CONST * result;

  /* #10 If a trap is needed: */
  if(Os_TrapIsTrapForWriteAccessRequired() != 0u)                                                                       /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_COREGETTHREAD;

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */

    /* #40 Unmarshal return value. */
    result = packet.ReturnValue.ReturnThreadConfigRefType;
  }
  /* #50 Otherwise: */
  else
  {
    /* #60 Call the API directly. */
    result = Os_CoreGetThread();
  }

  return result;
}



/*
 * \}
 * \addtogroup Os_UserInterface
 * \{
 */
/***********************************************************************************************************************
 *  Os_GetVersionInfo()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 */
void     Os_GetVersionInfo
(
  Std_VersionInfoType *versioninfo
)
{
  /* #10 Getter API. Call API directly. */
  Os_TrapCallGetVersionInfo(versioninfo);                                                                               /* SBSW_OS_TRAP_API_USERPOINTER_001 */
}


/***********************************************************************************************************************
 *  Os_GetExceptionContext()
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
 */
StatusType     Os_GetExceptionContext
(
  Os_ExceptionContextRefType Context
)
{
  Os_StatusType status;

  /* #10 Check that the given pointer is valid. */
  if(OS_UNLIKELY(Os_ErrCheckPointerIsNotNull(Context) == OS_CHECK_FAILED))                                              /* PRQA S 0315 */ /* MD_Os_Dir1.1_0315 */ /* SBSW_OS_TRAP_ERRCHECKPOINTERISNOTNULL_001 */
  {
    status = OS_STATUS_PARAM_POINTER_1;
  }
  else
  {
    /* #20 If a trap is needed: */
    if(Os_TrapIsTrapForReadAccessRequired() != 0u)                                                                      /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
    {
      Os_TrapPacketType packet;
      Os_ExceptionContextType userspaceContext;

      /* #30 Marshal API data. */
      packet.TrapId = OS_TRAPID_GETEXCEPTIONCONTEXT;
      packet.Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamExceptionContextRefType =
                                                               (Os_ExceptionContextType*)&userspaceContext;             /* SBSW_OS_PWA_LOCAL */

      /* #40 Trigger trap. */
      Os_Hal_Trap(&packet);                                                                                             /* SBSW_OS_FC_POINTER2LOCAL */

      /* #50 Unmarshal return value. */
      (*Context) = *((volatile Os_ExceptionContextType*)&userspaceContext);                                             /* SBSW_OS_TRAP_API_USERPOINTER_002 */
      status = packet.ReturnValue.ReturnOs_StatusType;
    }
    /* #60 Otherwise: */
    else
    {
      /* #70 Call the API directly. */
      status = Os_TrapCallGetExceptionContext(Context);                                                                 /* SBSW_OS_TRAP_API_USERPOINTER_001 */
    }
  }

  return Os_ErrReportGetExceptionContext(status, Context);                                                              /* SBSW_OS_TRAP_API_USERPOINTER_001 */
}


/***********************************************************************************************************************
 *  Os_SetExceptionContext()
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
 */
StatusType     Os_SetExceptionContext
(
  Os_ExceptionContextRefType Context
)
{
  Os_StatusType status;

  /* #10 Check that the given pointer is valid. */
  if(OS_UNLIKELY(Os_ErrCheckPointerIsNotNull(Context) == OS_CHECK_FAILED))                                              /* PRQA S 0315 */ /* MD_Os_Dir1.1_0315 */ /* SBSW_OS_TRAP_ERRCHECKPOINTERISNOTNULL_001 */
  {
    status = OS_STATUS_PARAM_POINTER_1;
  }
  else
  {
    /* #20 If a trap is needed: */
    if(Os_TrapIsTrapForWriteAccessRequired() != 0u)                                                                     /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
    {
      Os_TrapPacketType packet;
      Os_ExceptionContextType userspaceContext;
      userspaceContext = (*Context);

      /* #30 Marshal API data. */
      packet.TrapId = OS_TRAPID_SETEXCEPTIONCONTEXT;
      packet.Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamExceptionContextRefType =
                                                               (Os_ExceptionContextType*)&userspaceContext;             /* SBSW_OS_PWA_LOCAL */

      /* #40 Trigger trap. */
      Os_Hal_Trap(&packet);                                                                                             /* SBSW_OS_FC_POINTER2LOCAL */

      /* #50 Unmarshal return value. */
      status = packet.ReturnValue.ReturnOs_StatusType;
    }
    /* #60 Otherwise: */
    else
    {
      /* #70 Call the API directly. */
      status = Os_TrapCallSetExceptionContext(Context);                                                                 /* SBSW_OS_TRAP_API_USERPOINTER_001 */

    }
  }

  return Os_ErrReportSetExceptionContext(status, Context);                                                              /* SBSW_OS_TRAP_API_USERPOINTER_001 */
}


/***********************************************************************************************************************
 *  Os_GetExceptionAddress()
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
 */
Os_AddressOfConstType     Os_GetExceptionAddress
(
  void
)
{
  Os_AddressOfConstType address;

  /* #10 If a trap is needed: */
  if(Os_TrapIsTrapForReadAccessRequired() != 0u)                                                                        /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_GETEXCEPTIONADDRESS;

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */

    /* #40 Unmarshal return value. */
    address = packet.ReturnValue.ReturnOs_AddressOfConstType;
  }
  /* #50 Otherwise: */
  else
  {
    /* #60 Call the API directly. */
    address = Os_TrapCallGetExceptionAddress();                                                                         /* SBSW_OS_TRAP_API_USERPOINTER_001 */
  }

  return address;
}


/***********************************************************************************************************************
 *  Os_InitMemory()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 */
void     Os_InitMemory
(
  void
)
{
  /* #10 Caller (startup code) is privileged. Call API directly. */
  Os_TrapCallInitMemory();
}


/***********************************************************************************************************************
 *  Os_Init()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 */
void     Os_Init
(
  void
)
{
  /* #10 Caller (startup code) is privileged exception vectors are not yet initialized. Call API directly. */
  Os_TrapCallInit();
}


/***********************************************************************************************************************
 *  Os_InitInterruptOnly()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 */
void     Os_InitInterruptOnly
(
  void
)
{
  /* #10 Caller (startup code) is privileged. Call API directly. */
  Os_TrapCallInitInterruptOnly();
}


/***********************************************************************************************************************
 *  Os_EnterPreStartTask()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 */
void     Os_EnterPreStartTask
(
  void
)
{
  /* #10 Caller (startup code) is privileged. Call API directly. */
  Os_TrapCallEnterPreStartTask();
}


/***********************************************************************************************************************
 *  StartCore()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 */
void     StartCore                                                                                           /* COV_OS_HALPLATFORMTESTEDASMULTICORE */
(
  CoreIdType CoreID,
  StatusType *Status
)
{
  /* #10 If the given status pointer is valid: */
  if(OS_UNLIKELY(Os_ErrCheckPointerIsNotNull(Status) != OS_CHECK_FAILED))                                               /* PRQA S 0315 */ /* MD_Os_Dir1.1_0315 */ /* SBSW_OS_TRAP_ERRCHECKPOINTERISNOTNULL_001 */
  {
    /* #20 Caller (startup code) is privileged. Call API directly. */
    Os_TrapCallStartCore(CoreID, Status);                                                                               /* SBSW_OS_TRAP_API_USERPOINTER_001 */
  }
  /* else
   *   Do NOT write the error code to the Status pointer, as it is a ((void *)0)!  */
}


/***********************************************************************************************************************
 *  StartNonAutosarCore()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 */
void     StartNonAutosarCore                                                                                 /* COV_OS_HALPLATFORMTESTEDASMULTICORE */
(
  CoreIdType CoreID,
  StatusType *Status
)
{
  /* #10 If the given status pointer is valid: */
  if(OS_UNLIKELY(Os_ErrCheckPointerIsNotNull(Status) != OS_CHECK_FAILED))                                               /* PRQA S 0315 */ /* MD_Os_Dir1.1_0315 */ /* SBSW_OS_TRAP_ERRCHECKPOINTERISNOTNULL_001 */
  {
    /* #20 Caller (startup code) is privileged. Call API directly. */
    Os_TrapCallStartNonAutosarCore(CoreID, Status);                                                                     /* SBSW_OS_TRAP_API_USERPOINTER_001 */
  }
  /* else
   *   Do NOT write the error code to the Status pointer, as it is a ((void *)0)!  */
}


/***********************************************************************************************************************
 *  GetCoreID()
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
 */
CoreIdType     GetCoreID
(
  void
)
{
  CoreIdType result;

  /* #10 If a trap is needed: */
  if(Os_TrapIsTrapForReadAccessRequired() != 0u)                                                                        /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_GETCOREID;

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */

    /* #40 Unmarshal return value. */
    result = packet.ReturnValue.ReturnCoreIdType;
  }
  /* #50 Otherwise: */
  else
  {
    /* #60 Call the API directly. */
    result = Os_TrapCallGetCoreID();
  }

  return result;
}


/***********************************************************************************************************************
 *  GetNumberOfActivatedCores()
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
 */
uint32     GetNumberOfActivatedCores
(
  void
)
{
  uint32 result;

  /* #10 If a trap is needed: */
  if(Os_TrapIsTrapForReadAccessRequired() != 0u)                                                                        /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */ /* COV_OS_HALPLATFORMTESTEDASMULTICORE */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_GETNUMBEROFACTIVATEDCORES;

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */

    /* #40 Unmarshal return value. */
    result = packet.ReturnValue.ReturnUint32Type;
  }
  /* #50 Otherwise: */
  else
  {
    /* #60 Call the API directly. */
    result = Os_TrapCallGetNumberOfActivatedCores();
  }

  return result;
}


/***********************************************************************************************************************
 *  Os_GetCoreStartState()
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
 */
void     Os_GetCoreStartState
(
  CoreIdType CoreID,
    Os_CoreStartStateType   * CoreState,
  StatusType *Status
)
{
  /* #10 If the given status pointer is valid: */
  if(OS_UNLIKELY(Os_ErrCheckPointerIsNotNull(Status) != OS_CHECK_FAILED))                                               /* PRQA S 0315 */ /* MD_Os_Dir1.1_0315 */ /* SBSW_OS_TRAP_ERRCHECKPOINTERISNOTNULL_001 */
  {
    /* #20 If the given state pointer is valid: */
    if(OS_UNLIKELY(Os_ErrCheckPointerIsNotNull(CoreState) != OS_CHECK_FAILED))                                          /* PRQA S 0315 */ /* MD_Os_Dir1.1_0315 */ /* SBSW_OS_TRAP_ERRCHECKPOINTERISNOTNULL_001 */
    {
      /* #30 If a trap is needed: */
      if(Os_TrapIsTrapForReadAccessRequired() != 0u)                                                                    /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
      {
        Os_TrapPacketType packet;
        Os_CoreStartStateType userspaceCoreState;
        StatusType userspaceStatus;

        /* #40 Marshal API data. */
        packet.TrapId = OS_TRAPID_GETCORESTARTSTATE;
        packet.Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamCoreIdType = CoreID;                                   /* SBSW_OS_PWA_LOCAL */
        packet.Parameters[OS_TRAPPARAMINDEX_2].ParamApiType.ParamCoreStartStateRefType =
                                                                (Os_CoreStartStateType*)&userspaceCoreState;            /* SBSW_OS_PWA_LOCAL */
        packet.Parameters[OS_TRAPPARAMINDEX_3].ParamApiType.ParamStatusRefType = (StatusType*)&userspaceStatus;         /* SBSW_OS_PWA_LOCAL */

        /* #50 Trigger trap. */
        Os_Hal_Trap(&packet);                                                                                           /* SBSW_OS_FC_POINTER2LOCAL */

        /* #60 Unmarshal return values. */
        (*Status) = *((volatile StatusType*)&userspaceStatus);                                                          /* SBSW_OS_TRAP_API_USERPOINTER_002 */
        (*CoreState) = *((volatile Os_CoreStartStateType*)&userspaceCoreState);                                         /* SBSW_OS_TRAP_API_USERPOINTER_002 */
      }
      /* #70 Otherwise: */
      else
      {
        /* #80 Call the API directly. */
        Os_TrapCallGetCoreStartState(CoreID, CoreState, Status);                                                        /* SBSW_OS_TRAP_API_USERPOINTER_001 */
      }
    }
    /* #90 Otherwise: */
    else
    {
      /* #100 Set the Status parameter to E_OS_PARAM_POINTER. */
      *Status = E_OS_PARAM_POINTER;                                                                                     /* SBSW_OS_PWA_PRECONDITION */
    }
  }
  /* else
   *   Do NOT write the error code to the Status pointer, as it is a ((void *)0)!  */
}


/***********************************************************************************************************************
 *  GetActiveApplicationMode()
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
 */
AppModeType     GetActiveApplicationMode
(
  void
)
{
  AppModeType result;

  /* #10 If a trap is needed: */
  if(Os_TrapIsTrapForReadAccessRequired() != 0u)                                                                        /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_GETACTIVEAPPLICATIONMODE;

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */

    /* #40 Unmarshal return value. */
    result = packet.ReturnValue.ReturnAppModeType;
  }
  /* #50 Otherwise: */
  else
  {
    /* #60 Call the API directly. */
    result = Os_TrapCallGetActiveApplicationMode();
  }

  return result;
}


/***********************************************************************************************************************
 *  StartOS()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 */
void     StartOS
(
  AppModeType Mode
)
{
  /* #10 Caller (startup code) is privileged. Call API directly. */
  Os_TrapCallStartOS(Mode);
}


/***********************************************************************************************************************
 *  ShutdownOS()
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
void     ShutdownOS
(
  StatusType Error
)
{
  /* #10 If a trap is needed: */
  if(Os_TrapIsTrapForWriteAccessRequired() != 0u)                                                                       /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_SHUTDOWNOS;
    packet.Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamStatusType = Error;                                        /* SBSW_OS_PWA_LOCAL */

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */
  }
  /* #40 Otherwise: */
  else
  {
    /* #50 Call the API directly. */
    Os_TrapCallShutdownOS(Error);
  }
}


/***********************************************************************************************************************
 *  ShutdownAllCores()
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
void     ShutdownAllCores                                                                                    /* COV_OS_HALPLATFORMTESTEDASMULTICORE */
(
  StatusType Error
)
{
  /* #10 If a trap is needed: */
  if(Os_TrapIsTrapForWriteAccessRequired() != 0u)                                                                       /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_SHUTDOWNALLCORES;
    packet.Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamStatusType = Error;                                        /* SBSW_OS_PWA_LOCAL */

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */
  }
  /* #40 Otherwise: */
  else
  {
    /* #50 Call the API directly. */
    Os_TrapCallShutdownAllCores(Error);
  }
}


/***********************************************************************************************************************
 *  ControlIdle()
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
 */
StatusType     ControlIdle
(
  CoreIdType CoreID,
  IdleModeType IdleMode
)
{
  Os_StatusType status;

  /* #10 If a trap is needed: */
  if(Os_TrapIsTrapForWriteAccessRequired() != 0u)                                                                       /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */ /* COV_OS_HALPLATFORMTESTEDASMULTICORE */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_CONTROLIDLE;
    packet.Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamCoreIdType = CoreID;                                       /* SBSW_OS_PWA_LOCAL */
    packet.Parameters[OS_TRAPPARAMINDEX_2].ParamApiType.ParamIdleModeType = IdleMode;                                   /* SBSW_OS_PWA_LOCAL */

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */

    /* #40 Unmarshal return value. */
    status = packet.ReturnValue.ReturnOs_StatusType;
  }
  /* #50 Otherwise: */
  else
  {
    /* #60 Call the API directly. */
    status = Os_TrapCallControlIdle(CoreID, IdleMode);
  }

  return Os_ErrReportControlIdle(status, CoreID, IdleMode);
}                                                                                                                       /* PRQA S 6050 */ /* MD_MSR_STCAL */


/***********************************************************************************************************************
 *  Os_GetUnhandledIrq()
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
 */
StatusType     Os_GetUnhandledIrq
(
  Os_InterruptSourceIdRefType InterruptSource
)
{
  Os_StatusType status;

  /* #10 Check that the given pointer is valid. */
  if(OS_UNLIKELY(Os_ErrCheckPointerIsNotNull(InterruptSource) == OS_CHECK_FAILED))                                      /* PRQA S 0315 */ /* MD_Os_Dir1.1_0315 */ /* SBSW_OS_TRAP_ERRCHECKPOINTERISNOTNULL_001 */ /* COV_OS_HALPLATFORMTESTEDASMULTICORE */
  {
    status = OS_STATUS_PARAM_POINTER_1;
  }
  else
  {
    /* #20 If a trap is needed: */
    if(Os_TrapIsTrapForWriteAccessRequired() != 0u)                                                                     /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */ /* COV_OS_HALPLATFORMTESTEDASMULTICORE */
    {
      Os_TrapPacketType packet;
      Os_InterruptSourceIdType userspaceInterruptSource;

      /* #30 Marshal API data. */
      packet.TrapId = OS_TRAPID_GETUNHANDLEDIRQ;
      packet.Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamInterruptSourceIdRefType =
                                                               (Os_InterruptSourceIdType*)&userspaceInterruptSource;    /* SBSW_OS_PWA_LOCAL */

      /* #40 Trigger trap. */
      Os_Hal_Trap(&packet);                                                                                             /* SBSW_OS_FC_POINTER2LOCAL */

      /* #50 Unmarshal return value. */
      (*InterruptSource) = *((volatile Os_InterruptSourceIdType*)&userspaceInterruptSource);                            /* SBSW_OS_TRAP_API_USERPOINTER_002 */
      status = packet.ReturnValue.ReturnOs_StatusType;
    }
    /* #60 Otherwise: */
    else
    {
      /* #70 Call the API directly. */
      status = Os_TrapCallGetUnhandledIrq(InterruptSource);                                                             /* SBSW_OS_TRAP_API_USERPOINTER_001 */

    }
  }

  return Os_ErrReportGetUnhandledIrq(status, InterruptSource);                                                          /* SBSW_OS_TRAP_API_USERPOINTER_001 */
}                                                                                                                       /* PRQA S 6050 */ /* MD_MSR_STCAL */


/***********************************************************************************************************************
 *  Os_GetUnhandledExc()
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
 */
StatusType     Os_GetUnhandledExc
(
  Os_ExceptionSourceIdRefType ExceptionSource
)
{
  Os_StatusType status;

  /* #10 Check that the given pointer is valid. */
  if(OS_UNLIKELY(Os_ErrCheckPointerIsNotNull(ExceptionSource) == OS_CHECK_FAILED))                                      /* PRQA S 0315 */ /* MD_Os_Dir1.1_0315 */ /* SBSW_OS_TRAP_ERRCHECKPOINTERISNOTNULL_001 */ /* COV_OS_HALPLATFORMTESTEDASMULTICORE */
  {
    status = OS_STATUS_PARAM_POINTER_1;
  }
  else
  {
    /* #20 If a trap is needed: */
    if(Os_TrapIsTrapForWriteAccessRequired() != 0u)                                                                     /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */ /* COV_OS_HALPLATFORMTESTEDASMULTICORE */
    {
      Os_TrapPacketType packet;
      Os_ExceptionSourceIdType userspaceExceptionSource;

      /* #30 Marshal API data. */
      packet.TrapId = OS_TRAPID_GETUNHANDLEDEXC;
      packet.Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamExceptionSourceIdRefType =
                                                               (Os_ExceptionSourceIdType*)&userspaceExceptionSource;    /* SBSW_OS_PWA_LOCAL */

      /* #40 Trigger trap. */
      Os_Hal_Trap(&packet);                                                                                             /* SBSW_OS_FC_POINTER2LOCAL */

      /* #50 Unmarshal return value. */
      (*ExceptionSource) = *((volatile Os_ExceptionSourceIdType*)&userspaceExceptionSource);                            /* SBSW_OS_TRAP_API_USERPOINTER_002 */
      status = packet.ReturnValue.ReturnOs_StatusType;
    }
    /* #60 Otherwise: */
    else
    {
      /* #70 Call the API directly. */
      status = Os_TrapCallGetUnhandledExc(ExceptionSource);                                                             /* SBSW_OS_TRAP_API_USERPOINTER_001 */

    }
  }

  return Os_ErrReportGetUnhandledExc(status, ExceptionSource);                                                          /* SBSW_OS_TRAP_API_USERPOINTER_001 */
}                                                                                                                       /* PRQA S 6050 */ /* MD_MSR_STCAL */


/***********************************************************************************************************************
 *  Os_GetTaskStackUsage()
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
 */
uint32     Os_GetTaskStackUsage
(
  TaskType TaskID
)
{
  uint32 result;

  /* #10 If a trap is needed: */
  if(Os_TrapIsTrapForWriteAccessRequired() != 0u)                                                                       /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_GETTASKSTACKUSAGE;
    packet.Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamTaskType = TaskID;                                         /* SBSW_OS_PWA_LOCAL */

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */

    /* #40 Unmarshal return value. */
    result = packet.ReturnValue.ReturnUint32Type;
  }
  /* #50 Otherwise: */
  else
  {
    /* #60 Call the API directly. */
    result = Os_TrapCallGetTaskStackUsage(TaskID);
  }

  return result;
}                                                                                                                       /* PRQA S 6050 */ /* MD_MSR_STCAL */



/***********************************************************************************************************************
 *  Os_GetISRStackUsage()
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
 */
uint32     Os_GetISRStackUsage
(
  ISRType IsrID
)
{
  uint32 result;

  /* #10 If a trap is needed: */
  if(Os_TrapIsTrapForWriteAccessRequired() != 0u)                                                                       /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_GETISRSTACKUSAGE;
    packet.Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamISRType = IsrID;                                           /* SBSW_OS_PWA_LOCAL */

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */

    /* #40 Unmarshal return value. */
    result = packet.ReturnValue.ReturnUint32Type;
  }
  /* #50 Otherwise: */
  else
  {
    /* #60 Call the API directly. */
    result = Os_TrapCallGetISRStackUsage(IsrID);
  }

  return result;
}                                                                                                                       /* PRQA S 6050 */ /* MD_MSR_STCAL */


/***********************************************************************************************************************
 *  Os_GetKernelStackUsage()
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
 */
uint32     Os_GetKernelStackUsage
(
  CoreIdType CoreID
)
{
  uint32 result;

  /* #10 If a trap is needed: */
  if(Os_TrapIsTrapForWriteAccessRequired() != 0u)                                                                       /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_GETKERNELSTACKUSAGE;
    packet.Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamCoreIdType = CoreID;                                       /* SBSW_OS_PWA_LOCAL */

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */

    /* #40 Unmarshal return value. */
    result = packet.ReturnValue.ReturnUint32Type;
  }
  /* #50 Otherwise: */
  else
  {
    /* #60 Call the API directly. */
    result = Os_TrapCallGetKernelStackUsage(CoreID);
  }

  return result;
}                                                                                                                       /* PRQA S 6050 */ /* MD_MSR_STCAL */


/***********************************************************************************************************************
 *  Os_GetStartupHookStackUsage()
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
 */
uint32     Os_GetStartupHookStackUsage
(
  CoreIdType CoreID
)
{
  uint32 result;

  /* #10 If a trap is needed: */
  if(Os_TrapIsTrapForWriteAccessRequired() != 0u)                                                                       /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_GETSTARTUPHOOKSTACKUSAGE;
    packet.Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamCoreIdType = CoreID;                                       /* SBSW_OS_PWA_LOCAL */

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */

    /* #40 Unmarshal return value. */
    result = packet.ReturnValue.ReturnUint32Type;
  }
  /* #50 Otherwise: */
  else
  {
    /* #60 Call the API directly. */
    result = Os_TrapCallGetStartupHookStackUsage(CoreID);
  }

  return result;
}                                                                                                                       /* PRQA S 6050 */ /* MD_MSR_STCAL */


/***********************************************************************************************************************
 *  Os_GetErrorHookStackUsage()
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
 */
uint32     Os_GetErrorHookStackUsage
(
  CoreIdType CoreID
)
{
  uint32 result;

  /* #10 If a trap is needed: */
  if(Os_TrapIsTrapForWriteAccessRequired() != 0u)                                                                       /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_GETERRORHOOKSTACKUSAGE;
    packet.Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamCoreIdType = CoreID;                                       /* SBSW_OS_PWA_LOCAL */

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */

    /* #40 Unmarshal return value. */
    result = packet.ReturnValue.ReturnUint32Type;
  }
  /* #50 Otherwise: */
  else
  {
    /* #60 Call the API directly. */
    result = Os_TrapCallGetErrorHookStackUsage(CoreID);
  }

  return result;
}                                                                                                                       /* PRQA S 6050 */ /* MD_MSR_STCAL */


/***********************************************************************************************************************
 *  Os_GetShutdownHookStackUsage()
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
 */
uint32     Os_GetShutdownHookStackUsage
(
  CoreIdType CoreID
)
{
  uint32 result;

  /* #10 If a trap is needed: */
  if(Os_TrapIsTrapForWriteAccessRequired() != 0u)                                                                       /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_GETSHUTDOWNHOOKSTACKUSAGE;
    packet.Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamCoreIdType = CoreID;                                       /* SBSW_OS_PWA_LOCAL */

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */

    /* #40 Unmarshal return value. */
    result = packet.ReturnValue.ReturnUint32Type;
  }
  /* #50 Otherwise: */
  else
  {
    /* #60 Call the API directly. */
    result = Os_TrapCallGetShutdownHookStackUsage(CoreID);
  }

  return result;
}                                                                                                                       /* PRQA S 6050 */ /* MD_MSR_STCAL */


/***********************************************************************************************************************
 *  Os_GetProtectionHookStackUsage()
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
 */
uint32     Os_GetProtectionHookStackUsage
(
  CoreIdType CoreID
)
{
  uint32 result;

  /* #10 If a trap is needed: */
  if(Os_TrapIsTrapForWriteAccessRequired() != 0u)                                                                       /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_GETPROTECTIONHOOKSTACKUSAGE;
    packet.Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamCoreIdType = CoreID;                                       /* SBSW_OS_PWA_LOCAL */

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */

    /* #40 Unmarshal return value. */
    result = packet.ReturnValue.ReturnUint32Type;
  }
  /* #50 Otherwise: */
  else
  {
    /* #60 Call the API directly. */
    result = Os_TrapCallGetProtectionHookStackUsage(CoreID);
  }

  return result;
}                                                                                                                       /* PRQA S 6050 */ /* MD_MSR_STCAL */


/***********************************************************************************************************************
 *  Os_GetNonTrustedFunctionStackUsage()
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
 */
uint32     Os_GetNonTrustedFunctionStackUsage
(
  Os_NonTrustedFunctionIndexType FunctionIndex
)
{
  uint32 result;

  /* #10 If a trap is needed: */
  if(Os_TrapIsTrapForWriteAccessRequired() != 0u)                                                                       /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_GETNONTRUSTEDFUNCTIONSTACKUSAGE;
    packet.Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamOs_NonTrustedFunctionIndexType = FunctionIndex;            /* SBSW_OS_PWA_LOCAL */

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */

    /* #40 Unmarshal return value. */
    result = packet.ReturnValue.ReturnUint32Type;
  }
  /* #50 Otherwise: */
  else
  {
    /* #60 Call the API directly. */
    result = Os_TrapCallGetNonTrustedFunctionStackUsage(FunctionIndex);
  }

  return result;
}                                                                                                                       /* PRQA S 6050 */ /* MD_MSR_STCAL */


/***********************************************************************************************************************
 *  GetSpinlock()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 */
StatusType     GetSpinlock(SpinlockIdType SpinlockId)
{
  Os_StatusType status;

  /* #10 Spinlock API call is configurable. Spinlock module handles whether trap is needed or not. */
  status = Os_Api_GetSpinlock(SpinlockId);

  return Os_ErrReportGetSpinlock(status, SpinlockId);
}


/***********************************************************************************************************************
 *  ReleaseSpinlock()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 */
StatusType     ReleaseSpinlock(SpinlockIdType SpinlockId)
{
  Os_StatusType status;

  /* #10 Spinlock API call is configurable. Spinlock module handles whether trap is needed or not. */
  status = Os_Api_ReleaseSpinlock(SpinlockId);

  return Os_ErrReportReleaseSpinlock(status, SpinlockId);
}


/***********************************************************************************************************************
 *  TryToGetSpinlock()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 */
StatusType     TryToGetSpinlock
(
  SpinlockIdType SpinlockId,
  TryToGetSpinlockType* Success
)
{
  Os_StatusType status;

  /* #10 Check that the given pointer is valid. */
  if(OS_UNLIKELY(Os_ErrCheckPointerIsNotNull(Success) == OS_CHECK_FAILED))                                              /* PRQA S 0315 */ /* MD_Os_Dir1.1_0315 */ /* SBSW_OS_TRAP_ERRCHECKPOINTERISNOTNULL_001 */
  {
    status = OS_STATUS_PARAM_POINTER_2;
  }
  else
  {
    /* #20 Otherwise call the API. */
    status = Os_Api_TryToGetSpinlock(SpinlockId, Success);                                                              /* SBSW_OS_TRAP_API_USERPOINTER_001 */
  }

  return Os_ErrReportTryToGetSpinlock(status, SpinlockId, Success);                                                     /* SBSW_OS_TRAP_ERRREPORT_001 */
}


/***********************************************************************************************************************
 *  DisableAllInterrupts()
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
void     DisableAllInterrupts
(
  void
)
{
  /* #10 If a trap is needed: */
  if(Os_TrapIsTrapForWriteAccessRequired() != 0u)                                                                       /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_DISABLEALLINTERRUPTS;

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */
  }
  /* #40 Otherwise: */
  else
  {
    /* #50 Call the API directly. */
    Os_TrapCallDisableAllInterrupts();
  }
}                                                                                                                       /* PRQA S 6050 */ /* MD_MSR_STCAL */


/***********************************************************************************************************************
 *  EnableAllInterrupts()
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
void     EnableAllInterrupts
(
  void
)
{
  /* #10 If a trap is needed: */
  if(Os_TrapIsTrapForWriteAccessRequired() != 0u)                                                                       /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_ENABLEALLINTERRUPTS;

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */
  }
  /* #40 Otherwise: */
  else
  {
    /* #50 Call the API directly. */
    Os_TrapCallEnableAllInterrupts();
  }
}                                                                                                                       /* PRQA S 6050 */ /* MD_MSR_STCAL */


/***********************************************************************************************************************
 *  SuspendAllInterrupts()
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
void     SuspendAllInterrupts
(
  void
)
{
  /* #10 If a trap is needed: */
  if(Os_TrapIsTrapForWriteAccessRequired() != 0u)                                                                       /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_SUSPENDALLINTERRUPTS;

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */
  }
  /* #40 Otherwise: */
  else
  {
    /* #50 Call the API directly. */
    Os_TrapCallSuspendAllInterrupts();
  }
}                                                                                                                       /* PRQA S 6050 */ /* MD_MSR_STCAL */


/***********************************************************************************************************************
 *  ResumeAllInterrupts()
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
void     ResumeAllInterrupts
(
  void
)
{
  /* #10 If a trap is needed: */
  if(Os_TrapIsTrapForWriteAccessRequired() != 0u)                                                                       /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_RESUMEALLINTERRUPTS;

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */
  }
  /* #40 Otherwise: */
  else
  {
    /* #50 Call the API directly. */
    Os_TrapCallResumeAllInterrupts();
  }
}                                                                                                                       /* PRQA S 6050 */ /* MD_MSR_STCAL */


/***********************************************************************************************************************
 *  SuspendOSInterrupts()
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
void     SuspendOSInterrupts
(
  void
)
{
  /* #10 If a trap is needed: */
  if(Os_TrapIsTrapForWriteAccessRequired() != 0u)                                                                       /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_SUSPENDOSINTERRUPTS;

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */
  }
  /* #40 Otherwise: */
  else
  {
    /* #50 Call the API directly. */
    Os_TrapCallSuspendOSInterrupts();
  }
}                                                                                                                       /* PRQA S 6050 */ /* MD_MSR_STCAL */


/***********************************************************************************************************************
 *  ResumeOSInterrupts()
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
void     ResumeOSInterrupts
(
  void
)
{
  /* #10 If a trap is needed: */
  if(Os_TrapIsTrapForWriteAccessRequired() != 0u)                                                                       /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_RESUMEOSINTERRUPTS;

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */
  }
  /* #40 Otherwise: */
  else
  {
    /* #50 Call the API directly. */
    Os_TrapCallResumeOSInterrupts();
  }
}                                                                                                                       /* PRQA S 6050 */ /* MD_MSR_STCAL */


/***********************************************************************************************************************
 *  Os_DisableLevelAM()
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
 */
void     Os_DisableLevelAM
(
  void
)
{
  /* #10 If a trap is needed: */
  if(Os_TrapIsTrapForWriteAccessRequired() != 0u)                                                                       /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_DISABLELEVEL;

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */
  }
  /* #40 Otherwise: */
  else
  {
    /* #50 Inform the trace module on service function entry. */
    Os_TraceOrtiApiEntry(OsOrtiApiIdDisableOSInterrupts);

    /* #60 Call the API. */
    Os_InterruptDisableLevel();

    /* #70 Inform the trace module on service function exit. */
    Os_TraceOrtiApiExit(OsOrtiApiIdDisableOSInterrupts);
  }
}                                                                                                                       /* PRQA S 6050 */ /* MD_MSR_STCAL */


/***********************************************************************************************************************
 *  Os_DisableLevelKM()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
void     Os_DisableLevelKM
(
  void
)
{
  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdDisableOSInterrupts);

  /* #20 Call the API. */
  Os_InterruptDisableLevel();

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdDisableOSInterrupts);
}


/***********************************************************************************************************************
 *  Os_DisableLevelUM()
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
 */
void     Os_DisableLevelUM
(
  void
)
{
  /* #10 If memory protection is enabled call the API via trap. */
  if(Os_MpIsEnabled() != 0u)                                                                                            /* PRQA S 2991, 2995 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule2.2_2995 */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_DISABLELEVEL;

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */
  }
  /* #40 Otherwise call the API directly. */
  else
  {
    /* #50 Inform the trace module on service function entry. */
    Os_TraceOrtiApiEntry(OsOrtiApiIdDisableOSInterrupts);

    /* #60 Call the API. */
    Os_InterruptDisableLevel();

    /* #70 Inform the trace module on service function exit. */
    Os_TraceOrtiApiExit(OsOrtiApiIdDisableOSInterrupts);
  }
}


/***********************************************************************************************************************
 *  Os_EnableLevelAM()
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
 */
void     Os_EnableLevelAM
(
  void
)
{
  /* #10 If a trap is needed: */
  if(Os_TrapIsTrapForWriteAccessRequired() != 0u)                                                                       /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_ENABLELEVEL;

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */
  }
  /* #40 Otherwise: */
  else
  {
    /* #50 Inform the trace module on service function entry. */
    Os_TraceOrtiApiEntry(OsOrtiApiIdEnableOSInterrupts);

    /* #60 Call the API. */
    Os_InterruptEnableLevel();

    /* #70 Inform the trace module on service function exit. */
    Os_TraceOrtiApiExit(OsOrtiApiIdEnableOSInterrupts);
  }
}                                                                                                                       /* PRQA S 6050 */ /* MD_MSR_STCAL */


/***********************************************************************************************************************
 *  Os_EnableLevelKM()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
void     Os_EnableLevelKM
(
  void
)
{
  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdEnableOSInterrupts);

  /* #20 Call the API. */
  Os_InterruptEnableLevel();

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdEnableOSInterrupts);
}


/***********************************************************************************************************************
 *  Os_EnableLevelUM()
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
 */
void     Os_EnableLevelUM
(
  void
)
{
  /* #10 If memory protection is enabled call the API via trap. */
  if(Os_MpIsEnabled() != 0u)                                                                                            /* PRQA S 2991, 2995 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule2.2_2995 */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_ENABLELEVEL;

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */
  }
  /* #40 Otherwise call the API directly. */
  else
  {
    /* #50 Inform the trace module on service function entry. */
    Os_TraceOrtiApiEntry(OsOrtiApiIdEnableOSInterrupts);

    /* #60 Call the API. */
    Os_InterruptEnableLevel();

    /* #70 Inform the trace module on service function exit. */
    Os_TraceOrtiApiExit(OsOrtiApiIdEnableOSInterrupts);
  }
}


/***********************************************************************************************************************
 *  Os_DisableGlobalAM()
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
 */
void     Os_DisableGlobalAM
(
  void
)
{
  /* #10 If a trap is needed: */
  if(Os_TrapIsTrapForWriteAccessRequired() != 0u)                                                                       /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_DISABLEGLOBAL;

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */
  }
  /* #40 Otherwise: */
  else
  {
    /* #50 Inform the trace module on service function entry. */
    Os_TraceOrtiApiEntry(OsOrtiApiIdDisableAllInterrupts);

    /* #60 Call the API. */
    Os_InterruptDisableGlobal();

    /* #70 Inform the trace module on service function exit. */
    Os_TraceOrtiApiExit(OsOrtiApiIdDisableAllInterrupts);
  }
}                                                                                                                       /* PRQA S 6050 */ /* MD_MSR_STCAL */


/***********************************************************************************************************************
 *  Os_DisableGlobalKM()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
void     Os_DisableGlobalKM
(
  void
)
{

  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdDisableAllInterrupts);

  /* #20 Call the API. */
  Os_InterruptDisableGlobal();

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdDisableAllInterrupts);
}


/***********************************************************************************************************************
 *  Os_DisableGlobalUM()
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
 */
void     Os_DisableGlobalUM
(
  void
)
{
  /* #10 If memory protection is enabled call the API via trap. */
  if(Os_MpIsEnabled() != 0u)                                                                                            /* PRQA S 2991, 2995 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule2.2_2995 */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_DISABLEGLOBAL;

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */
  }
  /* #40 Otherwise call the API directly. */
  else
  {
    /* #50 Inform the trace module on service function entry. */
    Os_TraceOrtiApiEntry(OsOrtiApiIdDisableAllInterrupts);

    /* #60 Call the API. */
    Os_InterruptDisableGlobal();

    /* #70 Inform the trace module on service function exit. */
    Os_TraceOrtiApiExit(OsOrtiApiIdDisableAllInterrupts);
  }
}


/***********************************************************************************************************************
 *  Os_EnableGlobalAM()
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
 */
void     Os_EnableGlobalAM
(
  void
)
{
  /* #10 If a trap is needed: */
  if(Os_TrapIsTrapForWriteAccessRequired() != 0u)                                                                       /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_ENABLEGLOBAL;

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */
  }
  /* #40 Otherwise: */
  else
  {
    /* #60 Inform the trace module on service function entry. */
    Os_TraceOrtiApiEntry(OsOrtiApiIdEnableAllInterrupts);

    /* #70 Call the API. */
    Os_InterruptEnableGlobal();

    /* #80 Inform the trace module on service function exit. */
    Os_TraceOrtiApiExit(OsOrtiApiIdEnableAllInterrupts);
  }
}                                                                                                                       /* PRQA S 6050 */ /* MD_MSR_STCAL */


/***********************************************************************************************************************
 *  Os_EnableGlobalKM()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
void     Os_EnableGlobalKM
(
  void
)
{

  /* #10 Inform the trace module on service function entry. */
  Os_TraceOrtiApiEntry(OsOrtiApiIdEnableAllInterrupts);

  /* #20 Call the API. */
  Os_InterruptEnableGlobal();

  /* #30 Inform the trace module on service function exit. */
  Os_TraceOrtiApiExit(OsOrtiApiIdEnableAllInterrupts);
}


/***********************************************************************************************************************
 *  Os_EnableGlobalUM()
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
 */
void     Os_EnableGlobalUM
(
  void
)
{
  /* #10 If memory protection is enabled call the API via trap. */
  if(Os_MpIsEnabled() != 0u)                                                                                            /* PRQA S 2991, 2995 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule2.2_2995 */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_ENABLEGLOBAL;

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */
  }
  /* #40 Otherwise call the API directly. */
  else
  {
    /* #50 Inform the trace module on service function entry. */
    Os_TraceOrtiApiEntry(OsOrtiApiIdEnableAllInterrupts);

    /* #60 Call the API. */
    Os_InterruptEnableGlobal();

    /* #70 Inform the trace module on service function exit. */
    Os_TraceOrtiApiExit(OsOrtiApiIdEnableAllInterrupts);
  }
}


/***********************************************************************************************************************
 *  Os_DisableInterruptSource()
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
 */
StatusType     Os_DisableInterruptSource
(
  ISRType ISRID
)
{
  Os_StatusType status;

  /* #10 If a trap is needed: */
  if(Os_TrapIsTrapForWriteAccessRequired() != 0u)                                                                       /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_DISABLEINTERRUPTSOURCE;
    packet.Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamISRType = ISRID;                                           /* SBSW_OS_PWA_LOCAL */

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */

    /* #40 Unmarshal return value. */
    status = packet.ReturnValue.ReturnOs_StatusType;
  }
  /* #50 Otherwise: */
  else
  {
    /* #60 Call the API directly. */
    status = Os_TrapCallDisableInterruptSource(ISRID);
  }

  return Os_ErrReportDisableInterruptSource(status, ISRID);
}                                                                                                                       /* PRQA S 6050 */ /* MD_MSR_STCAL */


/***********************************************************************************************************************
 *  Os_EnableInterruptSource()
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
 */
StatusType     Os_EnableInterruptSource
(
  ISRType ISRID,
  boolean ClearPending
)
{
  Os_StatusType status;

  /* #10 If a trap is needed: */
  if(Os_TrapIsTrapForWriteAccessRequired() != 0u)                                                                       /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_ENABLEINTERRUPTSOURCE;
    packet.Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamISRType = ISRID;                                           /* SBSW_OS_PWA_LOCAL */
    packet.Parameters[OS_TRAPPARAMINDEX_2].ParamApiType.Paramboolean = ClearPending;                                    /* SBSW_OS_PWA_LOCAL */

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */

    /* #40 Unmarshal return value. */
    status = packet.ReturnValue.ReturnOs_StatusType;
  }
  /* #50 Otherwise: */
  else
  {
    /* #60 Call the API directly. */
    status = Os_TrapCallEnableInterruptSource(ISRID, ClearPending);
  }

  return Os_ErrReportEnableInterruptSource(status, ISRID, ClearPending);
}                                                                                                                       /* PRQA S 6050 */ /* MD_MSR_STCAL */


/***********************************************************************************************************************
 *  Os_InitialEnableInterruptSources()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 */
StatusType     Os_InitialEnableInterruptSources
(
  boolean           ClearPending
)
{
  Os_StatusType status;

  /* #10 Call the API directly. */
  status = Os_TrapCallInitialEnableInterruptSources(ClearPending);

  return Os_ErrReportInitialEnableInterruptSources(status, ClearPending);
}


/***********************************************************************************************************************
 *  Os_ClearPendingInterrupt()
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
 */
StatusType     Os_ClearPendingInterrupt
(
  ISRType ISRID
)
{
  Os_StatusType status;

  /* #10 If a trap is needed: */
  if(Os_TrapIsTrapForWriteAccessRequired() != 0u)                                                                       /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_CLEARPENDINGINTERRUPT;
    packet.Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamISRType = ISRID;                                           /* SBSW_OS_PWA_LOCAL */

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */

    /* #40 Unmarshal return value. */
    status = packet.ReturnValue.ReturnOs_StatusType;
  }
  /* #50 Otherwise: */
  else
  {
    /* #60 Call the API directly. */
    status = Os_TrapCallClearPendingInterrupt(ISRID);
  }

  return Os_ErrReportClearPendingInterrupt(status, ISRID);
}                                                                                                                       /* PRQA S 6050 */ /* MD_MSR_STCAL */


/***********************************************************************************************************************
 *  Os_IsInterruptSourceEnabled()
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
 */
StatusType     Os_IsInterruptSourceEnabled
(
  ISRType ISRID,
    boolean OS_VAR_NOINIT * IsEnabled
)
{
  Os_StatusType status;

  /* #10 Check that the given pointer is valid. */
  if(OS_UNLIKELY(Os_ErrCheckPointerIsNotNull(IsEnabled) == OS_CHECK_FAILED))                                            /* PRQA S 0315 */ /* MD_Os_Dir1.1_0315 */ /* SBSW_OS_TRAP_ERRCHECKPOINTERISNOTNULL_001 */
  {
    status = OS_STATUS_PARAM_POINTER_2;
  }
  else
  {
    /* #20 If a trap is needed: */
    if(Os_TrapIsTrapForWriteAccessRequired() != 0u)                                                                     /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
    {
      Os_TrapPacketType packet;
      boolean userspaceIsEnabled;

      /* #30 Marshal API data. */
      packet.TrapId = OS_TRAPID_ISINTERRUPTSOURCEENABLED;
      packet.Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamISRType = ISRID;                                         /* SBSW_OS_PWA_LOCAL */
      packet.Parameters[OS_TRAPPARAMINDEX_2].ParamApiType.ParambooleanRefType = (boolean*)&userspaceIsEnabled;          /* SBSW_OS_PWA_LOCAL */

      /* #40 Trigger trap. */
      Os_Hal_Trap(&packet);                                                                                             /* SBSW_OS_FC_POINTER2LOCAL */

      /* #50 Unmarshal return value and output parameter. */
      (*IsEnabled) = *((volatile boolean*)&userspaceIsEnabled);                                                         /* SBSW_OS_TRAP_API_USERPOINTER_002 */
      status = packet.ReturnValue.ReturnOs_StatusType;
    }
    /* #60 Otherwise: */
    else
    {
      /* #70 Call the API directly. */
      status = Os_TrapCallIsInterruptSourceEnabled(ISRID, IsEnabled);                                                   /* SBSW_OS_TRAP_API_USERPOINTER_001 */

    }
  }

  return Os_ErrReportIsInterruptSourceEnabled(status, ISRID, IsEnabled);                                                /* SBSW_OS_TRAP_ERRREPORT_001 */
}                                                                                                                       /* PRQA S 6050 */ /* MD_MSR_STCAL */


/***********************************************************************************************************************
 *  Os_IsInterruptPending()
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
 */
StatusType     Os_IsInterruptPending
(
  ISRType ISRID,
    boolean OS_VAR_NOINIT * IsPending
)
{
  Os_StatusType status;

  /* #10 Check that the given pointer is valid. */
  if(OS_UNLIKELY(Os_ErrCheckPointerIsNotNull(IsPending) == OS_CHECK_FAILED))                                            /* PRQA S 0315 */ /* MD_Os_Dir1.1_0315 */ /* SBSW_OS_TRAP_ERRCHECKPOINTERISNOTNULL_001 */
  {
    status = OS_STATUS_PARAM_POINTER_2;
  }
  else
  {
    /* #20 If a trap is needed: */
    if(Os_TrapIsTrapForWriteAccessRequired() != 0u)                                                                     /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
    {
      Os_TrapPacketType packet;
      boolean userspaceIsPending;

      /* #30 Marshal API data. */
      packet.TrapId = OS_TRAPID_ISINTERRUPTPENDING;
      packet.Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamISRType = ISRID;                                         /* SBSW_OS_PWA_LOCAL */
      packet.Parameters[OS_TRAPPARAMINDEX_2].ParamApiType.ParambooleanRefType = (boolean*)&userspaceIsPending;          /* SBSW_OS_PWA_LOCAL */

      /* #40 Trigger trap. */
      Os_Hal_Trap(&packet);                                                                                             /* SBSW_OS_FC_POINTER2LOCAL */

      /* #50 Unmarshal return value. */
      (*IsPending) = *((volatile boolean*)&userspaceIsPending);                                                         /* SBSW_OS_TRAP_API_USERPOINTER_002 */
      status = packet.ReturnValue.ReturnOs_StatusType;
    }
    /* #60 Otherwise: */
    else
    {
      /* #70 Call the API directly. */
      status = Os_TrapCallIsInterruptPending(ISRID, IsPending);                                                         /* SBSW_OS_TRAP_API_USERPOINTER_001 */

    }
  }

  return Os_ErrReportIsInterruptPending(status, ISRID, IsPending);                                                      /* SBSW_OS_TRAP_ERRREPORT_001 */
}                                                                                                                       /* PRQA S 6050 */ /* MD_MSR_STCAL */


/***********************************************************************************************************************
 *  Os_BarrierSynchronize()
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
 */
StatusType     Os_BarrierSynchronize
(
  Os_BarrierIdType BarrierID
)
{
  Os_StatusType status;

  /* #10 If a trap is needed: */
  if(Os_TrapIsTrapForWriteAccessRequired() != 0u)                                                                       /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */ /* COV_OS_TESTSUITEOSC4998 */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_BARRIERSYNCHRONIZE;
    packet.Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamBarrierIdType = BarrierID;                                 /* SBSW_OS_PWA_LOCAL */

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */

    /* #40 Unmarshal return value. */
    status = packet.ReturnValue.ReturnOs_StatusType;
  }
  /* #50 Otherwise: */
  else
  {
    /* #60 Call the API directly. */
    status = Os_TrapCallBarrierSynchronize(BarrierID);
  }

  return Os_ErrReportBarrierSynchronize(status, BarrierID);
}                                                                                                                       /* PRQA S 6050 */ /* MD_MSR_STCAL */


/***********************************************************************************************************************
 *  ActivateTask()
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
 */
StatusType     ActivateTask
(
  TaskType TaskID
)
{
  Os_StatusType status;

  /* #10 If a trap is needed: */
  if(Os_TrapIsTrapForWriteAccessRequired() != 0u)                                                                       /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_ACTIVATETASK;
    packet.Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamTaskType = TaskID;                                         /* SBSW_OS_PWA_LOCAL */

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */

    /* #40 Unmarshal return value. */
    status = packet.ReturnValue.ReturnOs_StatusType;
  }
  /* #50 Otherwise: */
  else
  {
    /* #60 Call the API directly. */
    status = Os_TrapCallActivateTask(TaskID);
  }

  return Os_ErrReportActivateTask(status, TaskID);
}                                                                                                                       /* PRQA S 6050 */ /* MD_MSR_STCAL */


/***********************************************************************************************************************
 *  TerminateTask()
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
 */
StatusType     TerminateTask
(
  void
)
{
  Os_StatusType status;

  /* #10 If a trap is needed: */
  if(Os_TrapIsTrapForWriteAccessRequired() != 0u)                                                                       /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_TERMINATETASK;

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */

    /* #40 Unmarshal return value. */
    status = packet.ReturnValue.ReturnOs_StatusType;
  }
  /* #50 Otherwise: */
  else
  {
    /* #60 Call the API directly. */
    status = Os_TrapCallTerminateTask();
  }

  return Os_ErrReportTerminateTask(status);
}                                                                                                                       /* PRQA S 6050 */ /* MD_MSR_STCAL */


/***********************************************************************************************************************
 *  ChainTask()
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
 */
StatusType     ChainTask
(
  TaskType TaskID
)
{
  Os_StatusType status;

  /* #10 If a trap is needed: */
  if(Os_TrapIsTrapForWriteAccessRequired() != 0u)                                                                       /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_CHAINTASK;
    packet.Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamTaskType = TaskID;                                         /* SBSW_OS_PWA_LOCAL */

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */

    /* #40 Unmarshal return value. */
    status = packet.ReturnValue.ReturnOs_StatusType;
  }
  /* #50 Otherwise: */
  else
  {
    /* #60 Call the API directly. */
    status = Os_TrapCallChainTask(TaskID);
  }

  return Os_ErrReportChainTask(status, TaskID);
}                                                                                                                       /* PRQA S 6050 */ /* MD_MSR_STCAL */


/***********************************************************************************************************************
 *  Schedule()
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
 */
StatusType     Schedule(void)
{
  Os_StatusType status;

  /* #10 If a trap is needed: */
  if(Os_TrapIsTrapForWriteAccessRequired() != 0u)                                                                       /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_SCHEDULE;

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */

    /* #40 Unmarshal return value. */
    status = packet.ReturnValue.ReturnOs_StatusType;
  }
  /* #50 Otherwise: */
  else
  {
    /* #60 Call the API directly. */
    status = Os_TrapCallSchedule();
  }

  return Os_ErrReportSchedule(status);
}                                                                                                                       /* PRQA S 6050 */ /* MD_MSR_STCAL */


/***********************************************************************************************************************
 *  GetTaskID()
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
 */
StatusType     GetTaskID(TaskRefType TaskID)
{
  Os_StatusType status;

  /* #10 Check that the given pointer is valid. */
  if(OS_UNLIKELY(Os_ErrCheckPointerIsNotNull(TaskID) == OS_CHECK_FAILED))                                               /* PRQA S 0315 */ /* MD_Os_Dir1.1_0315 */ /* SBSW_OS_TRAP_ERRCHECKPOINTERISNOTNULL_001 */
  {
    status = OS_STATUS_PARAM_POINTER_1;
  }
  else
  {
    /* #20 If a trap is needed: */
    if(Os_TrapIsTrapForReadAccessRequired() != 0u)                                                                      /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
    {
      Os_TrapPacketType packet;
      TaskType userspaceTaskId;

      /* #30 Marshal API data. */
      packet.TrapId = OS_TRAPID_GETTASKID;
      packet.Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamTaskRefType = (TaskType*)&userspaceTaskId;               /* SBSW_OS_PWA_LOCAL */

      /* #40 Trigger trap. */
      Os_Hal_Trap(&packet);                                                                                             /* SBSW_OS_FC_POINTER2LOCAL */

      /* #50 Unmarshal return value. */
      (*TaskID) = *((volatile TaskType*)&userspaceTaskId);                                                              /* SBSW_OS_TRAP_API_USERPOINTER_002 */
      status = packet.ReturnValue.ReturnOs_StatusType;
    }
    /* #60 Otherwise: */
    else
    {
      /* #70 Call the API directly. */
      status = Os_TrapCallGetTaskID(TaskID);                                                                            /* SBSW_OS_TRAP_API_USERPOINTER_001 */
    }
  }

  return Os_ErrReportGetTaskID(status, TaskID);                                                                         /* SBSW_OS_TRAP_ERRREPORT_001 */
}


/***********************************************************************************************************************
 *  GetTaskState()
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
 */
StatusType     GetTaskState(TaskType TaskID, TaskStateRefType State)
{
  Os_StatusType status;

  /* #10 Check that the given pointer is valid. */
  if(OS_UNLIKELY(Os_ErrCheckPointerIsNotNull(State) == OS_CHECK_FAILED))                                                /* PRQA S 0315 */ /* MD_Os_Dir1.1_0315 */ /* SBSW_OS_TRAP_ERRCHECKPOINTERISNOTNULL_001 */
  {
    status = OS_STATUS_PARAM_POINTER_2;
  }
  else
  {
    /* #20 If a trap is needed: */
    if(Os_TrapIsTrapForWriteAccessRequired() != 0u)                                                                     /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
    {
      Os_TrapPacketType packet;
      TaskStateType userspaceState;

      /* #30 Marshal API data. */
      packet.TrapId = OS_TRAPID_GETTASKSTATE;
      packet.Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamTaskType = TaskID;                                       /* SBSW_OS_PWA_LOCAL */
      packet.Parameters[OS_TRAPPARAMINDEX_2].ParamApiType.ParamTaskStateRefType = (TaskStateType*)&userspaceState;      /* SBSW_OS_PWA_LOCAL */

      /* #40 Trigger trap. */
      Os_Hal_Trap(&packet);                                                                                             /* SBSW_OS_FC_POINTER2LOCAL */

      /* #50 Unmarshal return value. */
      (*State) = *((volatile TaskStateType*)&userspaceState);                                                           /* SBSW_OS_TRAP_API_USERPOINTER_002 */
      status = packet.ReturnValue.ReturnOs_StatusType;
    }
    /* #60 Otherwise: */
    else
    {
      /* #70 Call the API directly. */
      status = Os_TrapCallGetTaskState(TaskID, State);                                                                  /* SBSW_OS_TRAP_API_USERPOINTER_001 */

    }
  }

  return Os_ErrReportGetTaskState(status, TaskID, State);                                                               /* SBSW_OS_TRAP_ERRREPORT_001 */
}                                                                                                                       /* PRQA S 6050 */ /* MD_MSR_STCAL */


/***********************************************************************************************************************
 *  GetISRID()
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
 */
ISRType     GetISRID(void)
{
  ISRType result;

  /* #10 If a trap is needed: */
  if(Os_TrapIsTrapForReadAccessRequired() != 0u)                                                                        /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_GETISRID;

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */

    /* #40 Unmarshal return value. */
    result = packet.ReturnValue.ReturnISRType;
  }
  /* #50 Otherwise: */
  else
  {
    /* #60 Call the API directly. */
    result = Os_TrapCallGetISRID();
  }

  return result;
}


/***********************************************************************************************************************
 *  SetEvent()
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
 */
StatusType     SetEvent
(
  TaskType TaskID,
  EventMaskType Mask
)
{
  Os_StatusType status;

  /* #10 If a trap is needed: */
  if(Os_TrapIsTrapForWriteAccessRequired() != 0u)                                                                       /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_SETEVENT;
    packet.Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamTaskType = TaskID;                                         /* SBSW_OS_PWA_LOCAL */
    packet.Parameters[OS_TRAPPARAMINDEX_2].ParamApiType.ParamEventMaskType = Mask;                                      /* SBSW_OS_PWA_LOCAL */

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */

    /* #40 Unmarshal return value. */
    status = packet.ReturnValue.ReturnOs_StatusType;
  }
  /* #50 Otherwise: */
  else
  {
    /* #60 Call the API directly. */
    status = Os_TrapCallSetEvent(TaskID, Mask);
  }

  return Os_ErrReportSetEvent(status, TaskID, Mask);
}                                                                                                                       /* PRQA S 6050 */ /* MD_MSR_STCAL */


/***********************************************************************************************************************
 *  ClearEvent()
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
 */
StatusType     ClearEvent
(
  EventMaskType Mask
)
{
  Os_StatusType status;

  /* #10 If a trap is needed: */
  if(Os_TrapIsTrapForWriteAccessRequired() != 0u)                                                                       /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_CLEAREVENT;
    packet.Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamEventMaskType = Mask;                                      /* SBSW_OS_PWA_LOCAL */

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */

    /* #40 Unmarshal return value. */
    status = packet.ReturnValue.ReturnOs_StatusType;
  }
  /* #50 Otherwise: */
  else
  {
    /* #60 Call the API directly. */
    status = Os_TrapCallClearEvent(Mask);
  }

  return Os_ErrReportClearEvent(status, Mask);
}                                                                                                                       /* PRQA S 6050 */ /* MD_MSR_STCAL */


/***********************************************************************************************************************
 *  GetEvent()
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
 */
StatusType     GetEvent
(
  TaskType TaskID,
  EventMaskRefType Mask
)
{
  Os_StatusType status;

  /* #10 Check that the given pointer is valid. */
  if(OS_UNLIKELY(Os_ErrCheckPointerIsNotNull(Mask) == OS_CHECK_FAILED))                                                 /* PRQA S 0315 */ /* MD_Os_Dir1.1_0315 */ /* SBSW_OS_TRAP_ERRCHECKPOINTERISNOTNULL_001 */
  {
    status = OS_STATUS_PARAM_POINTER_2;
  }
  else
  {
    /* #20 If a trap is needed: */
    if(Os_TrapIsTrapForWriteAccessRequired() != 0u)                                                                     /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
    {
      Os_TrapPacketType packet;
      EventMaskType userspaceMask;

      /* #30 Marshal API data. */
      packet.TrapId = OS_TRAPID_GETEVENT;
      packet.Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamTaskType = TaskID;                                       /* SBSW_OS_PWA_LOCAL */
      packet.Parameters[OS_TRAPPARAMINDEX_2].ParamApiType.ParamEventMaskRefType = (EventMaskType*)&userspaceMask;       /* SBSW_OS_PWA_LOCAL */

      /* #40 Trigger trap. */
      Os_Hal_Trap(&packet);                                                                                             /* SBSW_OS_FC_POINTER2LOCAL */

      /* #50 Unmarshal return value. */
      (*Mask) = *((volatile EventMaskType*)&userspaceMask);                                                             /* SBSW_OS_TRAP_API_USERPOINTER_002 */
      status = packet.ReturnValue.ReturnOs_StatusType;
    }
    /* #60 Otherwise: */
    else
    {
      /* #70 Call the API directly. */
      status = Os_TrapCallGetEvent(TaskID, Mask);                                                                       /* SBSW_OS_TRAP_API_USERPOINTER_001 */

    }
  }

  return Os_ErrReportGetEvent(status, TaskID, Mask);                                                                    /* SBSW_OS_TRAP_ERRREPORT_001 */
}                                                                                                                       /* PRQA S 6050 */ /* MD_MSR_STCAL */


/***********************************************************************************************************************
 *  WaitEvent()
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
 */
StatusType     WaitEvent
(
  EventMaskType Mask
)
{
  Os_StatusType status;

  /* #10 If a trap is needed: */
  if(Os_TrapIsTrapForWriteAccessRequired() != 0u)                                                                       /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_WAITEVENT;
    packet.Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamEventMaskType = Mask;                                      /* SBSW_OS_PWA_LOCAL */

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */

    /* #40 Unmarshal return value. */
    status = packet.ReturnValue.ReturnOs_StatusType;
  }
  /* #50 Otherwise: */
  else
  {
    /* #60 Call the API directly. */
    status = Os_TrapCallWaitEvent(Mask);
  }

  return Os_ErrReportWaitEvent(status, Mask);
}                                                                                                                       /* PRQA S 6050 */ /* MD_MSR_STCAL */


/***********************************************************************************************************************
 *  IncrementCounter()
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
 */
StatusType     IncrementCounter
(
  CounterType CounterID
)
{
  Os_StatusType status;

  /* #10 If a trap is needed: */
  if(Os_TrapIsTrapForWriteAccessRequired() != 0u)                                                                       /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_INCREMENTCOUNTER;
    packet.Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamCounterType = CounterID;                                   /* SBSW_OS_PWA_LOCAL */

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */

    /* #40 Unmarshal return value. */
    status = packet.ReturnValue.ReturnOs_StatusType;
  }
  /* #50 Otherwise: */
  else
  {
    /* #60 Call the API directly. */
    status = Os_TrapCallIncrementCounter(CounterID);
  }

  return Os_ErrReportIncrementCounter(status, CounterID);
}                                                                                                                       /* PRQA S 6050 */ /* MD_MSR_STCAL */


/***********************************************************************************************************************
 *  GetCounterValue()
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
 */
StatusType     GetCounterValue
(
  CounterType CounterID,
  TickRefType Value
)
{
  Os_StatusType status;

  /* #10 Check that the given pointer is valid. */
  if(OS_UNLIKELY(Os_ErrCheckPointerIsNotNull(Value) == OS_CHECK_FAILED))                                                /* PRQA S 0315 */ /* MD_Os_Dir1.1_0315 */ /* SBSW_OS_TRAP_ERRCHECKPOINTERISNOTNULL_001 */
  {
    status = OS_STATUS_PARAM_POINTER_2;
  }
  else
  {
    /* #20 If a trap is needed: */
    if(Os_TrapIsTrapForWriteAccessRequired() != 0u)                                                                     /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
    {
      Os_TrapPacketType packet;
      TickType userspaceValue;

      /* #30 Marshal API data. */
      packet.TrapId = OS_TRAPID_GETCOUNTERVALUE;
      packet.Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamCounterType = CounterID;                                 /* SBSW_OS_PWA_LOCAL */
      packet.Parameters[OS_TRAPPARAMINDEX_2].ParamApiType.ParamTickRefType = (TickType*)&userspaceValue;                /* SBSW_OS_PWA_LOCAL */

      /* #40 Trigger trap. */
      Os_Hal_Trap(&packet);                                                                                             /* SBSW_OS_FC_POINTER2LOCAL */

      /* #50 Unmarshal return value. */
      (*Value) = *((volatile TickType*)&userspaceValue);                                                                /* SBSW_OS_TRAP_API_USERPOINTER_002 */
      status = packet.ReturnValue.ReturnOs_StatusType;
    }
    /* #60 Otherwise: */
    else
    {
      /* #70 Call the API directly. */
      status = Os_TrapCallGetCounterValue(CounterID, Value);                                                            /* SBSW_OS_TRAP_API_USERPOINTER_001 */

    }
  }

  return Os_ErrReportGetCounterValue(status, CounterID, Value);                                                         /* SBSW_OS_TRAP_ERRREPORT_001 */
}                                                                                                                       /* PRQA S 6050 */ /* MD_MSR_STCAL */


/***********************************************************************************************************************
 *  GetElapsedValue()
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
 */
StatusType     GetElapsedValue
(
  CounterType CounterID,
  TickRefType Value,
  TickRefType ElapsedValue
)
{
  Os_StatusType status;

  /* #10 Check that the given pointer is valid. */
  if(OS_UNLIKELY(Os_ErrCheckPointerIsNotNull(Value) == OS_CHECK_FAILED))                                                /* PRQA S 0315 */ /* MD_Os_Dir1.1_0315 */ /* SBSW_OS_TRAP_ERRCHECKPOINTERISNOTNULL_001 */
  {
    status = OS_STATUS_PARAM_POINTER_2;
  }
  else if(OS_UNLIKELY(Os_ErrCheckPointerIsNotNull(ElapsedValue) == OS_CHECK_FAILED))                                    /* PRQA S 0315 */ /* MD_Os_Dir1.1_0315 */ /* SBSW_OS_TRAP_ERRCHECKPOINTERISNOTNULL_001 */
  {
    status = OS_STATUS_PARAM_POINTER_3;
  }
  else
  {
    /* #20 If a trap is needed: */
    if(Os_TrapIsTrapForWriteAccessRequired() != 0u)                                                                     /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
    {
      Os_TrapPacketType packet;
      TickType userspaceValue;
      TickType userspaceElapsedValue;

      /* #30 Marshal API data. */
      userspaceValue = (*Value);
      packet.TrapId = OS_TRAPID_GETELAPSEDVALUE;
      packet.Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamCounterType = CounterID;                                 /* SBSW_OS_PWA_LOCAL */
      packet.Parameters[OS_TRAPPARAMINDEX_2].ParamApiType.ParamTickRefType = (TickType*)&userspaceValue;                /* SBSW_OS_PWA_LOCAL */
      packet.Parameters[OS_TRAPPARAMINDEX_3].ParamApiType.ParamTickRefType = (TickType*)&userspaceElapsedValue;         /* SBSW_OS_PWA_LOCAL */

      /* #40 Trigger trap. */
      Os_Hal_Trap(&packet);                                                                                             /* SBSW_OS_FC_POINTER2LOCAL */

      /* #50 Unmarshal return value. */
      (*Value) = *((volatile TickType*)&userspaceValue);                                                                /* SBSW_OS_TRAP_API_USERPOINTER_002 */
      (*ElapsedValue) = *((volatile TickType*)&userspaceElapsedValue);                                                  /* SBSW_OS_TRAP_API_USERPOINTER_002 */
      status = packet.ReturnValue.ReturnOs_StatusType;
    }
    /* #60 Otherwise: */
    else
    {
      /* #70 Call the API directly. */
      status = Os_TrapCallGetElapsedValue(CounterID, Value, ElapsedValue);                                              /* SBSW_OS_TRAP_API_USERPOINTER_001 */

    }
  }

  return Os_ErrReportGetElapsedValue(status, CounterID, Value, ElapsedValue);                                           /* SBSW_OS_TRAP_ERRREPORT_001 */
}                                                                                                                       /* PRQA S 6050 */ /* MD_MSR_STCAL */


/***********************************************************************************************************************
 *  GetAlarmBase()
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
 */
StatusType     GetAlarmBase
(
  AlarmType AlarmID,
  AlarmBaseRefType Info
)
{
  Os_StatusType status;

  /* #10 Check that the given pointer is valid. */
  if(OS_UNLIKELY(Os_ErrCheckPointerIsNotNull(Info) == OS_CHECK_FAILED))                                                 /* PRQA S 0315 */ /* MD_Os_Dir1.1_0315 */ /* SBSW_OS_TRAP_ERRCHECKPOINTERISNOTNULL_001 */
  {
    status = OS_STATUS_PARAM_POINTER_2;
  }
  else
  {
    /* #20 If a trap is needed: */
    if(Os_TrapIsTrapForReadAccessRequired() != 0u)                                                                      /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
    {
      Os_TrapPacketType packet;
      AlarmBaseType userspaceInfo;

      /* #30 Marshal API data. */
      packet.TrapId = OS_TRAPID_GETALARMBASE;
      packet.Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamAlarmType = AlarmID;                                     /* SBSW_OS_PWA_LOCAL */
      packet.Parameters[OS_TRAPPARAMINDEX_2].ParamApiType.ParamAlarmBaseRefType = (AlarmBaseType*)&userspaceInfo;       /* SBSW_OS_PWA_LOCAL */

      /* #40 Trigger trap. */
      Os_Hal_Trap(&packet);                                                                                             /* SBSW_OS_FC_POINTER2LOCAL */

      /* #50 Unmarshal return value. */
      (*Info) = *((volatile AlarmBaseType*)&userspaceInfo);                                                             /* SBSW_OS_TRAP_API_USERPOINTER_002 */
      status = packet.ReturnValue.ReturnOs_StatusType;
    }
    /* #60 Otherwise: */
    else
    {
      /* #70 Call the API directly. */
      status = Os_TrapCallGetAlarmBase(AlarmID, Info);                                                                  /* SBSW_OS_TRAP_API_USERPOINTER_001 */
    }
  }

  return Os_ErrReportGetAlarmBase(status, AlarmID, Info);                                                               /* SBSW_OS_TRAP_ERRREPORT_001 */
}


/***********************************************************************************************************************
 *  GetAlarm()
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
 */
StatusType     GetAlarm
(
  AlarmType AlarmID,
  TickRefType Tick
)
{
  Os_StatusType status;

  /* #10 Check that the given pointer is valid. */
  if(OS_UNLIKELY(Os_ErrCheckPointerIsNotNull(Tick) == OS_CHECK_FAILED))                                                 /* PRQA S 0315 */ /* MD_Os_Dir1.1_0315 */ /* SBSW_OS_TRAP_ERRCHECKPOINTERISNOTNULL_001 */
  {
    status = OS_STATUS_PARAM_POINTER_2;
  }
  else
  {
    /* #20 If a trap is needed: */
    if(Os_TrapIsTrapForWriteAccessRequired() != 0u)                                                                     /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
    {
      Os_TrapPacketType packet;
      TickType userspaceTick;

      /* #30 Marshal API data. */
      packet.TrapId = OS_TRAPID_GETALARM;
      packet.Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamAlarmType = AlarmID;                                     /* SBSW_OS_PWA_LOCAL */
      packet.Parameters[OS_TRAPPARAMINDEX_2].ParamApiType.ParamTickRefType = (TickType*)&userspaceTick;                 /* SBSW_OS_PWA_LOCAL */

      /* #40 Trigger trap. */
      Os_Hal_Trap(&packet);                                                                                             /* SBSW_OS_FC_POINTER2LOCAL */

      /* #50 Unmarshal return value. */
      (*Tick) = *((volatile TickType*)&userspaceTick);                                                                  /* SBSW_OS_TRAP_API_USERPOINTER_002 */
      status = packet.ReturnValue.ReturnOs_StatusType;
    }
    /* #60 Otherwise: */
    else
    {
      /* #70 Call the API directly. */
      status = Os_TrapCallGetAlarm(AlarmID, Tick);                                                                      /* SBSW_OS_TRAP_API_USERPOINTER_001 */

    }
  }

  return Os_ErrReportGetAlarm(status, AlarmID, Tick);                                                                   /* SBSW_OS_TRAP_ERRREPORT_001 */
}                                                                                                                       /* PRQA S 6050 */ /* MD_MSR_STCAL */


/***********************************************************************************************************************
 *  SetRelAlarm()
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
 */
StatusType     SetRelAlarm
(
  AlarmType AlarmID,
  TickType Increment,
  TickType Cycle
)
{
  Os_StatusType status;

  /* #10 If a trap is needed: */
  if(Os_TrapIsTrapForWriteAccessRequired() != 0u)                                                                       /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_SETRELALARM;
    packet.Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamAlarmType = AlarmID;                                       /* SBSW_OS_PWA_LOCAL */
    packet.Parameters[OS_TRAPPARAMINDEX_2].ParamApiType.ParamTickType = Increment;                                      /* SBSW_OS_PWA_LOCAL */
    packet.Parameters[OS_TRAPPARAMINDEX_3].ParamApiType.ParamTickType = Cycle;                                          /* SBSW_OS_PWA_LOCAL */

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */

    /* #40 Unmarshal return value. */
    status = packet.ReturnValue.ReturnOs_StatusType;
  }
  /* #50 Otherwise: */
  else
  {
    /* #60 Call the API directly. */
    status = Os_TrapCallSetRelAlarm(AlarmID, Increment, Cycle);
  }

  return Os_ErrReportSetRelAlarm(status, AlarmID, Increment, Cycle);
}                                                                                                                       /* PRQA S 6050 */ /* MD_MSR_STCAL */


/***********************************************************************************************************************
 *  SetAbsAlarm()
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
 */
StatusType     SetAbsAlarm
(
  AlarmType AlarmID,
  TickType Start,
  TickType Cycle
)
{
  Os_StatusType status;

  /* #10 If a trap is needed: */
  if(Os_TrapIsTrapForWriteAccessRequired() != 0u)                                                                       /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_SETABSALARM;
    packet.Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamAlarmType = AlarmID;                                       /* SBSW_OS_PWA_LOCAL */
    packet.Parameters[OS_TRAPPARAMINDEX_2].ParamApiType.ParamTickType = Start;                                          /* SBSW_OS_PWA_LOCAL */
    packet.Parameters[OS_TRAPPARAMINDEX_3].ParamApiType.ParamTickType = Cycle;                                          /* SBSW_OS_PWA_LOCAL */

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */

    /* #40 Unmarshal return value. */
    status = packet.ReturnValue.ReturnOs_StatusType;
  }
  /* #50 Otherwise: */
  else
  {
    /* #60 Call the API directly. */
    status = Os_TrapCallSetAbsAlarm(AlarmID, Start, Cycle);
  }

  return Os_ErrReportSetAbsAlarm(status, AlarmID, Start, Cycle);
}                                                                                                                       /* PRQA S 6050 */ /* MD_MSR_STCAL */


/***********************************************************************************************************************
 *  CancelAlarm()
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
 */
StatusType     CancelAlarm
(
  AlarmType AlarmID
)
{
  Os_StatusType status;

  /* #10 If a trap is needed: */
  if(Os_TrapIsTrapForWriteAccessRequired() != 0u)                                                                       /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_CANCELALARM;
    packet.Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamAlarmType = AlarmID;                                       /* SBSW_OS_PWA_LOCAL */

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */

    /* #40 Unmarshal return value. */
    status = packet.ReturnValue.ReturnOs_StatusType;
  }
  /* #50 Otherwise: */
  else
  {
    /* #60 Call the API directly. */
    status = Os_TrapCallCancelAlarm(AlarmID);
  }

  return Os_ErrReportCancelAlarm(status, AlarmID);
}                                                                                                                       /* PRQA S 6050 */ /* MD_MSR_STCAL */


/***********************************************************************************************************************
 *  GetResource()
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
 */
StatusType     GetResource
(
  ResourceType ResID
)
{
  Os_StatusType status;

  /* #10 If a trap is needed: */
  if(Os_TrapIsTrapForWriteAccessRequired() != 0u)                                                                       /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_GETRESOURCE;
    packet.Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamResourceType = ResID;                                      /* SBSW_OS_PWA_LOCAL */

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */

    /* #40 Unmarshal return value. */
    status = packet.ReturnValue.ReturnOs_StatusType;
  }
  /* #50 Otherwise: */
  else
  {
    /* #60 Call the API directly. */
    status = Os_TrapCallGetResource(ResID);
  }

  return Os_ErrReportGetResource(status, ResID);
}                                                                                                                       /* PRQA S 6050 */ /* MD_MSR_STCAL */


/***********************************************************************************************************************
 *  ReleaseResource()
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
 */
StatusType     ReleaseResource
(
  ResourceType ResID
)
{
  Os_StatusType status;

  /* #10 If a trap is needed: */
  if(Os_TrapIsTrapForWriteAccessRequired() != 0u)                                                                       /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_RELEASERESOURCE;
    packet.Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamResourceType = ResID;                                      /* SBSW_OS_PWA_LOCAL */

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */

    /* #40 Unmarshal return value. */
    status = packet.ReturnValue.ReturnOs_StatusType;
  }
  /* #50 Otherwise: */
  else
  {
    /* #60 Call the API directly. */
    status = Os_TrapCallReleaseResource(ResID);
  }

  return Os_ErrReportReleaseResource(status, ResID);
}                                                                                                                       /* PRQA S 6050 */ /* MD_MSR_STCAL */


/***********************************************************************************************************************
 *  StartScheduleTableRel()
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
 */
StatusType     StartScheduleTableRel
(
  ScheduleTableType ScheduleTableID,
  TickType Offset
)
{
  Os_StatusType status;

  /* #10 If a trap is needed: */
  if(Os_TrapIsTrapForWriteAccessRequired() != 0u)                                                                       /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_STARTSCHEDULETABLEREL;
    packet.Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamScheduleTableType = ScheduleTableID;                       /* SBSW_OS_PWA_LOCAL */
    packet.Parameters[OS_TRAPPARAMINDEX_2].ParamApiType.ParamTickType = Offset;                                         /* SBSW_OS_PWA_LOCAL */

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */

    /* #40 Unmarshal return value. */
    status = packet.ReturnValue.ReturnOs_StatusType;
  }
  /* #50 Otherwise: */
  else
  {
    /* #60 Call the API directly. */
    status = Os_TrapCallStartScheduleTableRel(ScheduleTableID, Offset);
  }

  return Os_ErrReportStartScheduleTableRel(status, ScheduleTableID, Offset);
}                                                                                                                       /* PRQA S 6050 */ /* MD_MSR_STCAL */


/***********************************************************************************************************************
 *  StartScheduleTableAbs()
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
 */
StatusType     StartScheduleTableAbs
(
  ScheduleTableType ScheduleTableID,
  TickType Start
)
{
  Os_StatusType status;

  /* #10 If a trap is needed: */
  if(Os_TrapIsTrapForWriteAccessRequired() != 0u)                                                                       /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_STARTSCHEDULETABLEABS;
    packet.Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamScheduleTableType = ScheduleTableID;                       /* SBSW_OS_PWA_LOCAL */
    packet.Parameters[OS_TRAPPARAMINDEX_2].ParamApiType.ParamTickType = Start;                                          /* SBSW_OS_PWA_LOCAL */

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */

    /* #40 Unmarshal return value. */
    status = packet.ReturnValue.ReturnOs_StatusType;
  }
  /* #50 Otherwise: */
  else
  {
    /* #60 Call the API directly. */
    status = Os_TrapCallStartScheduleTableAbs(ScheduleTableID, Start);
  }

  return Os_ErrReportStartScheduleTableAbs(status, ScheduleTableID, Start);
}                                                                                                                       /* PRQA S 6050 */ /* MD_MSR_STCAL */


/***********************************************************************************************************************
 *  StopScheduleTable()
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
 */
StatusType     StopScheduleTable
(
  ScheduleTableType ScheduleTableID
)
{
  Os_StatusType status;

  /* #10 If a trap is needed: */
  if(Os_TrapIsTrapForWriteAccessRequired() != 0u)                                                                       /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_STOPSCHEDULETABLE;
    packet.Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamScheduleTableType = ScheduleTableID;                       /* SBSW_OS_PWA_LOCAL */

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */

    /* #40 Unmarshal return value. */
    status = packet.ReturnValue.ReturnOs_StatusType;
  }
  /* #50 Otherwise: */
  else
  {
    /* #60 Call the API directly. */
    status = Os_TrapCallStopScheduleTable(ScheduleTableID);
  }

  return Os_ErrReportStopScheduleTable(status, ScheduleTableID);
}                                                                                                                       /* PRQA S 6050 */ /* MD_MSR_STCAL */


/***********************************************************************************************************************
 *  NextScheduleTable()
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
 */
StatusType     NextScheduleTable
(
  ScheduleTableType ScheduleTableID_From,
  ScheduleTableType ScheduleTableID_To
)
{
  Os_StatusType status;

  /* #10 If a trap is needed: */
  if(Os_TrapIsTrapForWriteAccessRequired() != 0u)                                                                       /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_NEXTSCHEDULETABLE;
    packet.Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamScheduleTableType = ScheduleTableID_From;                  /* SBSW_OS_PWA_LOCAL */
    packet.Parameters[OS_TRAPPARAMINDEX_2].ParamApiType.ParamScheduleTableType = ScheduleTableID_To;                    /* SBSW_OS_PWA_LOCAL */

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */

    /* #40 Unmarshal return value. */
    status = packet.ReturnValue.ReturnOs_StatusType;
  }
  /* #50 Otherwise: */
  else
  {
    /* #60 Call the API directly. */
    status = Os_TrapCallNextScheduleTable(ScheduleTableID_From, ScheduleTableID_To);
  }

  return Os_ErrReportNextScheduleTable(status, ScheduleTableID_From, ScheduleTableID_To);
}                                                                                                                       /* PRQA S 6050 */ /* MD_MSR_STCAL */


/***********************************************************************************************************************
 *  GetScheduleTableStatus()
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
 */
StatusType     GetScheduleTableStatus
(
  ScheduleTableType ScheduleTableID,
  ScheduleTableStatusRefType ScheduleStatus
)
{
  Os_StatusType status;

  /* #10 Check that the given pointer is valid. */
  if(OS_UNLIKELY(Os_ErrCheckPointerIsNotNull(ScheduleStatus) == OS_CHECK_FAILED))                                       /* PRQA S 0315 */ /* MD_Os_Dir1.1_0315 */ /* SBSW_OS_TRAP_ERRCHECKPOINTERISNOTNULL_001 */
  {
    status = OS_STATUS_PARAM_POINTER_2;
  }
  else
  {
    /* #20 If a trap is needed: */
    if(Os_TrapIsTrapForWriteAccessRequired() != 0u)                                                                     /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
    {
      Os_TrapPacketType packet;
      ScheduleTableStatusType userspaceScheduleStatus;

      /* #30 Marshal API data. */
      userspaceScheduleStatus = (*ScheduleStatus);
      packet.TrapId = OS_TRAPID_GETSCHEDULETABLESTATUS;
      packet.Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamScheduleTableType = ScheduleTableID;                     /* SBSW_OS_PWA_LOCAL */
      packet.Parameters[OS_TRAPPARAMINDEX_2].ParamApiType.ParamScheduleTableStatusRefType =
                                                                  (ScheduleTableStatusType*)&userspaceScheduleStatus;   /* SBSW_OS_PWA_LOCAL */

      /* #40 Trigger trap. */
      Os_Hal_Trap(&packet);                                                                                             /* SBSW_OS_FC_POINTER2LOCAL */

      /* #50 Unmarshal return value. */
      (*ScheduleStatus) = *((volatile ScheduleTableStatusType*)&userspaceScheduleStatus);                               /* SBSW_OS_TRAP_API_USERPOINTER_002 */
      status = packet.ReturnValue.ReturnOs_StatusType;
    }
    /* #60 Otherwise: */
    else
    {
      /* #70 Call the API directly. */
      status = Os_TrapCallGetScheduleTableStatus(ScheduleTableID, ScheduleStatus);                                      /* SBSW_OS_TRAP_API_USERPOINTER_001 */

    }
  }

  return Os_ErrReportGetScheduleTableStatus(status, ScheduleTableID, ScheduleStatus);                                   /* SBSW_OS_TRAP_ERRREPORT_001 */
}                                                                                                                       /* PRQA S 6050 */ /* MD_MSR_STCAL */


/***********************************************************************************************************************
 *  StartScheduleTableSynchron()
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
 */
StatusType     StartScheduleTableSynchron
(
  ScheduleTableType ScheduleTableID
)
{
  Os_StatusType status;

  /* #10 If a trap is needed: */
  if(Os_TrapIsTrapForWriteAccessRequired() != 0u)                                                                       /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_STARTSCHEDULETABLESYNCHRON;
    packet.Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamScheduleTableType = ScheduleTableID;                       /* SBSW_OS_PWA_LOCAL */

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */

    /* #40 Unmarshal return value. */
    status = packet.ReturnValue.ReturnOs_StatusType;
  }
  /* #50 Otherwise: */
  else
  {
    /* #60 Call the API directly. */
    status = Os_TrapCallStartScheduleTableSynchron(ScheduleTableID);
  }

  return Os_ErrReportStartScheduleTableSynchron(status, ScheduleTableID);
}                                                                                                                       /* PRQA S 6050 */ /* MD_MSR_STCAL */


/***********************************************************************************************************************
 *  SyncScheduleTable()
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
 */
StatusType     SyncScheduleTable
(
  ScheduleTableType ScheduleTableID,
  TickType Value
)
{
  Os_StatusType status;

  /* #10 If a trap is needed: */
  if(Os_TrapIsTrapForWriteAccessRequired() != 0u)                                                                       /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_SYNCSCHEDULETABLE;
    packet.Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamScheduleTableType = ScheduleTableID;                       /* SBSW_OS_PWA_LOCAL */
    packet.Parameters[OS_TRAPPARAMINDEX_2].ParamApiType.ParamTickType = Value;                                          /* SBSW_OS_PWA_LOCAL */

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */

    /* #40 Unmarshal return value. */
    status = packet.ReturnValue.ReturnOs_StatusType;
  }
  /* #50 Otherwise: */
  else
  {
    /* #60 Call the API directly. */
    status = Os_TrapCallSyncScheduleTable(ScheduleTableID, Value);
  }

  return Os_ErrReportSyncScheduleTable(status, ScheduleTableID, Value);
}                                                                                                                       /* PRQA S 6050 */ /* MD_MSR_STCAL */


/***********************************************************************************************************************
 *  SetScheduleTableAsync()
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
 */
StatusType     SetScheduleTableAsync
(
  ScheduleTableType ScheduleTableID
)
{
  Os_StatusType status;

  /* #10 If a trap is needed: */
  if(Os_TrapIsTrapForWriteAccessRequired() != 0u)                                                                       /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_SETSCHEDULETABLEASYNC;
    packet.Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamScheduleTableType = ScheduleTableID;                       /* SBSW_OS_PWA_LOCAL */

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */

    /* #40 Unmarshal return value. */
    status = packet.ReturnValue.ReturnOs_StatusType;
  }
  /* #50 Otherwise: */
  else
  {
    /* #60 Call the API directly. */
    status = Os_TrapCallSetScheduleTableAsync(ScheduleTableID);
  }

  return Os_ErrReportSetScheduleTableAsync(status, ScheduleTableID);
}                                                                                                                       /* PRQA S 6050 */ /* MD_MSR_STCAL */


/***********************************************************************************************************************
 *  GetApplicationID()
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
 */
ApplicationType     GetApplicationID
(
  void
)
{
  ApplicationType result;

  /* #10 If a trap is needed: */
  if(Os_TrapIsTrapForReadAccessRequired() != 0u)                                                                        /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */ /* COV_OS_HALPLATFORMTRAPALWAYSNEEDED */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_GETAPPLICATIONID;

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */

    /* #40 Unmarshal return value. */
    result = packet.ReturnValue.ReturnApplicationType;
  }
  /* #50 Otherwise: */
  else
  {
    /* #60 Call the API directly. */
    result = Os_TrapCallGetApplicationID();
  }

  return result;
}


/***********************************************************************************************************************
 *  GetCurrentApplicationID()
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
 */
ApplicationType     GetCurrentApplicationID
(
  void
)
{
  ApplicationType result;

  /* #10 If a trap is needed: */
  if(Os_TrapIsTrapForReadAccessRequired() != 0u)                                                                        /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */ /* COV_OS_HALPLATFORMTRAPALWAYSNEEDED */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_GETCURRENTAPPLICATIONID;

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */

    /* #40 Unmarshal return value. */
    result = packet.ReturnValue.ReturnApplicationType;
  }
  /* #50 Otherwise: */
  else
  {
    /* #60 Call the API directly. */
    result = Os_TrapCallGetCurrentApplicationID();
  }

  return result;
}


/***********************************************************************************************************************
 *  GetApplicationState()
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
 */
StatusType     GetApplicationState
(
  ApplicationType Application,
  ApplicationStateRefType Value
)
{
  Os_StatusType status;

  /* #10 Check that the given pointer is valid. */
  if(OS_UNLIKELY(Os_ErrCheckPointerIsNotNull(Value) == OS_CHECK_FAILED))                                                /* PRQA S 0315 */ /* MD_Os_Dir1.1_0315 */ /* SBSW_OS_TRAP_ERRCHECKPOINTERISNOTNULL_001 */
  {
    status = OS_STATUS_PARAM_POINTER_2;
  }
  else
  {
    /* #20 If a trap is needed: */
    if(Os_TrapIsTrapForWriteAccessRequired() != 0u)                                                                     /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
    {
      Os_TrapPacketType packet;
      ApplicationStateType userspaceValue;

      /* #30 Marshal API data. */
      packet.TrapId = OS_TRAPID_GETAPPLICATIONSTATE;
      packet.Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamApplicationType = Application;                           /* SBSW_OS_PWA_LOCAL */
      packet.Parameters[OS_TRAPPARAMINDEX_2].ParamApiType.ParamApplicationStateRefType =
                                                                  (ApplicationStateType*)&userspaceValue;               /* SBSW_OS_PWA_LOCAL */

      /* #40 Trigger trap. */
      Os_Hal_Trap(&packet);                                                                                             /* SBSW_OS_FC_POINTER2LOCAL */

      /* #50 Unmarshal return value. */
      (*Value) = *((volatile ApplicationStateType*)&userspaceValue);                                                    /* SBSW_OS_TRAP_API_USERPOINTER_002 */
      status = packet.ReturnValue.ReturnOs_StatusType;
    }
    /* #60 Otherwise: */
    else
    {
      /* #70 Call the API directly. */
      status = Os_TrapCallGetApplicationState(Application, Value);                                                      /* SBSW_OS_TRAP_API_USERPOINTER_001 */

    }
  }

  return Os_ErrReportGetApplicationState(status, Application, Value);                                                   /* SBSW_OS_TRAP_ERRREPORT_001 */
}                                                                                                                       /* PRQA S 6050 */ /* MD_MSR_STCAL */


/***********************************************************************************************************************
 *  CheckObjectAccess()
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
 */
ObjectAccessType     CheckObjectAccess
(
  ApplicationType ApplID,
  ObjectTypeType ObjectType,
  Os_ObjectIdType ObjectID
)
{
  ObjectAccessType result;

  /* #10 If a trap is needed: */
  if(Os_TrapIsTrapForWriteAccessRequired() != 0u)                                                                       /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_CHECKOBJECTACCESS;
    packet.Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamApplicationType = ApplID;                                  /* SBSW_OS_PWA_LOCAL */
    packet.Parameters[OS_TRAPPARAMINDEX_2].ParamApiType.ParamObjectTypeType  = ObjectType;                              /* SBSW_OS_PWA_LOCAL */
    packet.Parameters[OS_TRAPPARAMINDEX_3].ParamApiType.ParamObjectIdType    = ObjectID;                                /* SBSW_OS_PWA_LOCAL */

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */

    /* #40 Unmarshal return value. */
    result = packet.ReturnValue.ReturnObjectAccessType;
  }
  /* #50 Otherwise: */
  else
  {
    /* #60 Call the API directly. */
    result = Os_TrapCallCheckObjectAccess(ApplID, ObjectType, ObjectID);
  }

  return result;
}


/***********************************************************************************************************************
 *  CheckObjectOwnership()
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
 */
ApplicationType     CheckObjectOwnership
(
  ObjectTypeType ObjectType,
  Os_ObjectIdType ObjectID
)
{
  ApplicationType result;

  /* #10 If a trap is needed: */
  if(Os_TrapIsTrapForWriteAccessRequired() != 0u)                                                                       /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_CHECKOBJECTOWNERSHIP;
    packet.Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamObjectTypeType = ObjectType;                               /* SBSW_OS_PWA_LOCAL */
    packet.Parameters[OS_TRAPPARAMINDEX_2].ParamApiType.ParamObjectIdType   = ObjectID;                                 /* SBSW_OS_PWA_LOCAL */

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */

    /* #40 Unmarshal return value. */
    result = packet.ReturnValue.ReturnApplicationType;
  }
  /* #50 Otherwise: */
  else
  {
    /* #60 Call the API directly. */
    result = Os_TrapCallCheckObjectOwnership(ObjectType, ObjectID);
  }

  return result;
}


/***********************************************************************************************************************
 *  AllowAccess()
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
 */
StatusType     AllowAccess
(
  void
)
{
  Os_StatusType status;

  /* #10 If a trap is needed: */
  if(Os_TrapIsTrapForWriteAccessRequired() != 0u)                                                                       /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_ALLOWACCESS;

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */

    /* #40 Unmarshal return value. */
    status = packet.ReturnValue.ReturnOs_StatusType;
  }
  /* #50 Otherwise: */
  else
  {
    /* #60 Call the API directly. */
    status = Os_TrapCallAllowAccess();
  }

  return Os_ErrReportAllowAccess(status);
}                                                                                                                       /* PRQA S 6050 */ /* MD_MSR_STCAL */


/***********************************************************************************************************************
 *  TerminateApplication()
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
 */
StatusType     TerminateApplication
(
  ApplicationType Application,
  RestartType RestartOption
)
{
  Os_StatusType status;

  /* #10 If a trap is needed: */
  if(Os_TrapIsTrapForWriteAccessRequired() != 0u)                                                                       /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_TERMINATEAPPLICATION;
    packet.Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamApplicationType = Application;                             /* SBSW_OS_PWA_LOCAL */
    packet.Parameters[OS_TRAPPARAMINDEX_2].ParamApiType.ParamRestartType = RestartOption;                               /* SBSW_OS_PWA_LOCAL */

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */

    /* #40 Unmarshal return value. */
    status = packet.ReturnValue.ReturnOs_StatusType;
  }
  /* #50 Otherwise: */
  else
  {
    /* #60 Call the API directly. */
    status = Os_TrapCallTerminateApplication(Application, RestartOption);
  }

  return Os_ErrReportTerminateApplication(status, Application, RestartOption);
}                                                                                                                       /* PRQA S 6050 */ /* MD_MSR_STCAL */


/***********************************************************************************************************************
 *  Os_ReadPeripheral8Legacy()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 */
uint8     Os_ReadPeripheral8Legacy                                                                           /* COV_OS_PERIPHERAL_DEPRECATED */
(
  Os_PeripheralIdType PeripheralID,
  uint32 Address
)
{
  Os_PeripheralConstInt2AddressType converter;                                                                          /* PRQA S 0759 */ /* MD_MSR_Union */

  /* #10 Call the peripheral API. */
  converter.AddressInt = Address;
  return Os_ReadPeripheral8(PeripheralID, converter.AddressPtr2Uint8);                                                  /* SBSW_OS_FC_PRECONDITION */
}



/***********************************************************************************************************************
 *  Os_ReadPeripheral16Legacy()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 */
uint16     Os_ReadPeripheral16Legacy                                                                         /* COV_OS_PERIPHERAL_DEPRECATED */
(
  Os_PeripheralIdType PeripheralID,
  uint32 Address
)
{
  Os_PeripheralConstInt2AddressType converter;                                                                          /* PRQA S 0759 */ /* MD_MSR_Union */

  /* #10 Call the peripheral API. */
  converter.AddressInt = Address;
  return Os_ReadPeripheral16(PeripheralID, converter.AddressPtr2Uint16);                                                /* SBSW_OS_FC_PRECONDITION */
}


/***********************************************************************************************************************
 *  Os_ReadPeripheral32Legacy()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 */
uint32     Os_ReadPeripheral32Legacy                                                                         /* COV_OS_PERIPHERAL_DEPRECATED */
(
  Os_PeripheralIdType PeripheralID,
  uint32 Address
)
{
  Os_PeripheralConstInt2AddressType converter;                                                                          /* PRQA S 0759 */ /* MD_MSR_Union */

  /* #10 Call the peripheral API. */
  converter.AddressInt = Address;
  return Os_ReadPeripheral32(PeripheralID, converter.AddressPtr2Uint32);                                                /* SBSW_OS_FC_PRECONDITION */
}


/***********************************************************************************************************************
 *  Os_WritePeripheral8Legacy()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 */
void     Os_WritePeripheral8Legacy                                                                           /* COV_OS_PERIPHERAL_DEPRECATED */
(
  Os_PeripheralIdType PeripheralID,
  uint32 Address,
  uint8 Value
)
{
  Os_PeripheralInt2AddressType converter;                                                                               /* PRQA S 0759 */ /* MD_MSR_Union */

  /* #10 Call the peripheral API. */
  converter.AddressInt = Address;
  Os_WritePeripheral8(PeripheralID, converter.AddressPtr2Uint8, Value);                                                 /* SBSW_OS_FC_PRECONDITION */
}


/***********************************************************************************************************************
 *  Os_WritePeripheral16Legacy()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 */
void     Os_WritePeripheral16Legacy                                                                          /* COV_OS_PERIPHERAL_DEPRECATED */
(
  Os_PeripheralIdType PeripheralID,
  uint32 Address,
  uint16 Value
)
{
  Os_PeripheralInt2AddressType converter;                                                                               /* PRQA S 0759 */ /* MD_MSR_Union */

  /* #10 Call the peripheral API. */
  converter.AddressInt = Address;
  Os_WritePeripheral16(PeripheralID, converter.AddressPtr2Uint16, Value);                                               /* SBSW_OS_FC_PRECONDITION */
}


/***********************************************************************************************************************
 *  Os_WritePeripheral32Legacy()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 */
void     Os_WritePeripheral32Legacy                                                                          /* COV_OS_PERIPHERAL_DEPRECATED */
(
  Os_PeripheralIdType PeripheralID,
  uint32 Address,
  uint32 Value
)
{
  Os_PeripheralInt2AddressType converter;                                                                               /* PRQA S 0759 */ /* MD_MSR_Union */

  /* #10 Call the peripheral API. */
  converter.AddressInt = Address;
  Os_WritePeripheral32(PeripheralID, converter.AddressPtr2Uint32, Value);                                               /* SBSW_OS_FC_PRECONDITION */
}


/***********************************************************************************************************************
 *  Os_ModifyPeripheral8Legacy()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 */
void     Os_ModifyPeripheral8Legacy                                                                          /* COV_OS_PERIPHERAL_DEPRECATED */
(
  Os_PeripheralIdType PeripheralID,
  uint32 Address,
  uint8 ClearMask,
  uint8 SetMask
)
{
  Os_PeripheralInt2AddressType converter;                                                                               /* PRQA S 0759 */ /* MD_MSR_Union */

  /* #10 Call the peripheral API. */
  converter.AddressInt = Address;
  Os_ModifyPeripheral8(PeripheralID, converter.AddressPtr2Uint8, ClearMask, SetMask);                                   /* SBSW_OS_FC_PRECONDITION */
}


/***********************************************************************************************************************
 *  Os_ModifyPeripheral16Legacy()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 */
void     Os_ModifyPeripheral16Legacy                                                                         /* COV_OS_PERIPHERAL_DEPRECATED */
(
  Os_PeripheralIdType PeripheralID,
  uint32 Address,
  uint16 ClearMask,
  uint16 SetMask
)
{
  Os_PeripheralInt2AddressType converter;                                                                               /* PRQA S 0759 */ /* MD_MSR_Union */

  /* #10 Call the peripheral API. */
  converter.AddressInt = Address;
  Os_ModifyPeripheral16(PeripheralID, converter.AddressPtr2Uint16, ClearMask, SetMask);                                 /* SBSW_OS_FC_PRECONDITION */
}


/***********************************************************************************************************************
 *  Os_ModifyPeripheral32Legacy()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 */
void     Os_ModifyPeripheral32Legacy                                                                         /* COV_OS_PERIPHERAL_DEPRECATED */
(
  Os_PeripheralIdType PeripheralID,
  uint32 Address,
  uint32 ClearMask,
  uint32 SetMask
)
{
  Os_PeripheralInt2AddressType converter;                                                                               /* PRQA S 0759 */ /* MD_MSR_Union */

  /* #10 Call the peripheral API. */
  converter.AddressInt = Address;
  Os_ModifyPeripheral32(PeripheralID, converter.AddressPtr2Uint32, ClearMask, SetMask);                                 /* SBSW_OS_FC_PRECONDITION */
}


/***********************************************************************************************************************
 *  Os_ReadPeripheral8()
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
 */
uint8     Os_ReadPeripheral8
(
  Os_PeripheralIdType PeripheralID,
  const   uint8 OS_APPL_DATA * Address
)
{
  uint8 result;

  /* #10 If a trap is needed: */
  if(Os_TrapIsTrapForWriteAccessRequired() != 0u)                                                                       /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */ /* COV_OS_HALPLATFORMTRAPALWAYSNEEDED */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_READPERIPHERAL8;
    packet.Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamPeripheralIdType = PeripheralID;                           /* SBSW_OS_PWA_LOCAL */
    packet.Parameters[OS_TRAPPARAMINDEX_2].ParamConstUint8RefType = Address;                                            /* SBSW_OS_PWA_LOCAL */

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */

    /* #40 Unmarshal return value. */
    result = packet.ReturnValue.ReturnUint8Type;
  }
  /* #50 Otherwise: */
  else
  {
    /* #60 Call the API directly. */
    result = Os_TrapCallReadPeripheral8(PeripheralID, Address);                                                         /* SBSW_OS_TRAP_API_USERPOINTER_003 */
  }

  return result;
}


/***********************************************************************************************************************
 *  Os_ReadPeripheral16()
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
 */
uint16     Os_ReadPeripheral16
(
  Os_PeripheralIdType PeripheralID,
  const   uint16 OS_APPL_DATA * Address
)
{
  uint16 result;

  /* #10 If a trap is needed: */
  if(Os_TrapIsTrapForWriteAccessRequired() != 0u)                                                                       /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */ /* COV_OS_HALPLATFORMTRAPALWAYSNEEDED */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_READPERIPHERAL16;
    packet.Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamPeripheralIdType = PeripheralID;                           /* SBSW_OS_PWA_LOCAL */
    packet.Parameters[OS_TRAPPARAMINDEX_2].ParamConstUint16RefType = Address;                                           /* SBSW_OS_PWA_LOCAL */

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */

    /* #40 Unmarshal return value. */
    result = packet.ReturnValue.ReturnUint16Type;
  }
  /* #50 Otherwise: */
  else
  {
    /* #60 Call the API directly. */
    result = Os_TrapCallReadPeripheral16(PeripheralID, Address);                                                        /* SBSW_OS_TRAP_API_USERPOINTER_003 */
  }

  return result;
}


/***********************************************************************************************************************
 *  Os_ReadPeripheral32()
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
 */
uint32     Os_ReadPeripheral32
(
  Os_PeripheralIdType PeripheralID,
  const   uint32 OS_APPL_DATA * Address
)
{
  uint32 result;

  /* #10 If a trap is needed: */
  if(Os_TrapIsTrapForWriteAccessRequired() != 0u)                                                                       /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */ /* COV_OS_HALPLATFORMTRAPALWAYSNEEDED */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_READPERIPHERAL32;
    packet.Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamPeripheralIdType = PeripheralID;                           /* SBSW_OS_PWA_LOCAL */
    packet.Parameters[OS_TRAPPARAMINDEX_2].ParamConstUint32RefType = Address;                                           /* SBSW_OS_PWA_LOCAL */

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */

    /* #40 Unmarshal return value. */
    result = packet.ReturnValue.ReturnUint32Type;
  }
  /* #50 Otherwise: */
  else
  {
    /* #60 Call the API directly. */
    result = Os_TrapCallReadPeripheral32(PeripheralID, Address);                                                        /* SBSW_OS_TRAP_API_USERPOINTER_003 */
  }

  return result;
}


/***********************************************************************************************************************
 *  Os_WritePeripheral8()
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
void     Os_WritePeripheral8
(
  Os_PeripheralIdType PeripheralID,
    uint8 OS_APPL_DATA * Address,
  uint8 Value
)
{
  /* #10 If a trap is needed: */
  if(Os_TrapIsTrapForWriteAccessRequired() != 0u)                                                                       /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */ /* COV_OS_HALPLATFORMTRAPALWAYSNEEDED */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_WRITEPERIPHERAL8;
    packet.Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamPeripheralIdType = PeripheralID;                           /* SBSW_OS_PWA_LOCAL */
    packet.Parameters[OS_TRAPPARAMINDEX_2].ParamUint8RefType = Address;                                                 /* SBSW_OS_PWA_LOCAL */
    packet.Parameters[OS_TRAPPARAMINDEX_3].ParamUint8Type = Value;                                                      /* SBSW_OS_PWA_LOCAL */

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */
  }
  /* #40 Otherwise: */
  else
  {
    /* #50 Call the API directly. */
    Os_TrapCallWritePeripheral8(PeripheralID, Address, Value);                                                          /* SBSW_OS_TRAP_API_USERPOINTER_003 */
  }
}


/***********************************************************************************************************************
 *  Os_WritePeripheral16()
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
void     Os_WritePeripheral16
(
  Os_PeripheralIdType PeripheralID,
    uint16 OS_APPL_DATA * Address,
  uint16 Value
)
{
  /* #10 If a trap is needed: */
  if(Os_TrapIsTrapForWriteAccessRequired() != 0u)                                                                       /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */ /* COV_OS_HALPLATFORMTRAPALWAYSNEEDED */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_WRITEPERIPHERAL16;
    packet.Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamPeripheralIdType = PeripheralID;                           /* SBSW_OS_PWA_LOCAL */
    packet.Parameters[OS_TRAPPARAMINDEX_2].ParamUint16RefType = Address;                                                /* SBSW_OS_PWA_LOCAL */
    packet.Parameters[OS_TRAPPARAMINDEX_3].ParamUint16Type = Value;                                                     /* SBSW_OS_PWA_LOCAL */

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */
  }
  /* #40 Otherwise: */
  else
  {
    /* #50 Call the API directly. */
    Os_TrapCallWritePeripheral16(PeripheralID, Address, Value);                                                         /* SBSW_OS_TRAP_API_USERPOINTER_003 */
  }
}                                                                                                                       /* PRQA S 6050 */ /* MD_MSR_STCAL */


/***********************************************************************************************************************
 *  Os_WritePeripheral32()
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
void     Os_WritePeripheral32
(
  Os_PeripheralIdType PeripheralID,
    uint32 OS_APPL_DATA * Address,
  uint32 Value
)
{
  /* #10 If a trap is needed: */
  if(Os_TrapIsTrapForWriteAccessRequired() != 0u)                                                                       /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */ /* COV_OS_HALPLATFORMTRAPALWAYSNEEDED */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_WRITEPERIPHERAL32;
    packet.Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamPeripheralIdType = PeripheralID;                           /* SBSW_OS_PWA_LOCAL */
    packet.Parameters[OS_TRAPPARAMINDEX_2].ParamUint32RefType = Address;                                                /* SBSW_OS_PWA_LOCAL */
    packet.Parameters[OS_TRAPPARAMINDEX_3].ParamUint32Type = Value;                                                     /* SBSW_OS_PWA_LOCAL */

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */
  }
  /* #40 Otherwise: */
  else
  {
    /* #50 Call the API directly. */
    Os_TrapCallWritePeripheral32(PeripheralID, Address, Value);                                                         /* SBSW_OS_TRAP_API_USERPOINTER_003 */
  }
}                                                                                                                       /* PRQA S 6050 */ /* MD_MSR_STCAL */


/***********************************************************************************************************************
 *  Os_ModifyPeripheral8()
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
void     Os_ModifyPeripheral8
(
  Os_PeripheralIdType PeripheralID,
    uint8 OS_APPL_DATA * Address,
  uint8 ClearMask,
  uint8 SetMask
)
{
  /* #10 If a trap is needed: */
  if(Os_TrapIsTrapForWriteAccessRequired() != 0u)                                                                       /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */ /* COV_OS_HALPLATFORMTRAPALWAYSNEEDED */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_MODIFYPERIPHERAL8;
    packet.Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamPeripheralIdType = PeripheralID;                           /* SBSW_OS_PWA_LOCAL */
    packet.Parameters[OS_TRAPPARAMINDEX_2].ParamUint8RefType = Address;                                                 /* SBSW_OS_PWA_LOCAL */
    packet.Parameters[OS_TRAPPARAMINDEX_3].ParamUint8Type = ClearMask;                                                  /* SBSW_OS_PWA_LOCAL */
    packet.Parameters[OS_TRAPPARAMINDEX_4].ParamUint8Type = SetMask;                                                    /* SBSW_OS_PWA_LOCAL */

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */
  }
  /* #40 Otherwise: */
  else
  {
    /* #50 Call the API directly. */
    Os_TrapCallModifyPeripheral8(PeripheralID, Address, ClearMask, SetMask);                                            /* SBSW_OS_TRAP_API_USERPOINTER_003 */
  }
}                                                                                                                       /* PRQA S 6050 */ /* MD_MSR_STCAL */


/***********************************************************************************************************************
 *  Os_ModifyPeripheral16()
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
void     Os_ModifyPeripheral16
(
  Os_PeripheralIdType PeripheralID,
    uint16 OS_APPL_DATA * Address,
  uint16 ClearMask,
  uint16 SetMask
)
{
  /* #10 If a trap is needed: */
  if(Os_TrapIsTrapForWriteAccessRequired() != 0u)                                                                       /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */ /* COV_OS_HALPLATFORMTRAPALWAYSNEEDED */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_MODIFYPERIPHERAL16;
    packet.Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamPeripheralIdType = PeripheralID;                           /* SBSW_OS_PWA_LOCAL */
    packet.Parameters[OS_TRAPPARAMINDEX_2].ParamUint16RefType = Address;                                                /* SBSW_OS_PWA_LOCAL */
    packet.Parameters[OS_TRAPPARAMINDEX_3].ParamUint16Type = ClearMask;                                                 /* SBSW_OS_PWA_LOCAL */
    packet.Parameters[OS_TRAPPARAMINDEX_4].ParamUint16Type = SetMask;                                                   /* SBSW_OS_PWA_LOCAL */

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */
  }
  /* #40 Otherwise: */
  else
  {
    /* #50 Call the API directly. */
    Os_TrapCallModifyPeripheral16(PeripheralID, Address, ClearMask, SetMask);                                           /* SBSW_OS_TRAP_API_USERPOINTER_003 */
  }
}


/***********************************************************************************************************************
 *  Os_ModifyPeripheral32()
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
void     Os_ModifyPeripheral32
(
  Os_PeripheralIdType PeripheralID,
    uint32 OS_APPL_DATA * Address,
  uint32 ClearMask,
  uint32 SetMask
)
{
  /* #10 If a trap is needed: */
  if(Os_TrapIsTrapForWriteAccessRequired() != 0u)                                                                       /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */ /* COV_OS_HALPLATFORMTRAPALWAYSNEEDED */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_MODIFYPERIPHERAL32;
    packet.Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamPeripheralIdType = PeripheralID;                           /* SBSW_OS_PWA_LOCAL */
    packet.Parameters[OS_TRAPPARAMINDEX_2].ParamUint32RefType = Address;                                                /* SBSW_OS_PWA_LOCAL */
    packet.Parameters[OS_TRAPPARAMINDEX_3].ParamUint32Type = ClearMask;                                                 /* SBSW_OS_PWA_LOCAL */
    packet.Parameters[OS_TRAPPARAMINDEX_4].ParamUint32Type = SetMask;                                                   /* SBSW_OS_PWA_LOCAL */

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */
  }
  /* #40 Otherwise: */
  else
  {
    /* #50 Call the API directly. */
    Os_TrapCallModifyPeripheral32(PeripheralID, Address, ClearMask, SetMask);                                           /* SBSW_OS_TRAP_API_USERPOINTER_003 */
  }
}                                                                                                                       /* PRQA S 6050 */ /* MD_MSR_STCAL */


/***********************************************************************************************************************
 *  CallTrustedFunction()
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
 */
StatusType     CallTrustedFunction
(
  TrustedFunctionIndexType FunctionIndex,
  TrustedFunctionParameterRefType FunctionParams
)
{
  Os_StatusType status;

  /* #10 If a trap is needed: */
  if(Os_TrapIsTrapForWriteAccessRequired() != 0u)                                                                       /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */ /* COV_OS_HALPLATFORMTRAPALWAYSNEEDED */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_CALLTRUSTEDFUNCTION;
    packet.Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamTrustedFunctionIndexType = FunctionIndex;                  /* SBSW_OS_PWA_LOCAL */
    packet.Parameters[OS_TRAPPARAMINDEX_2].ParamApiType.ParamTrustedFunctionParameterRefType = FunctionParams;          /* SBSW_OS_PWA_LOCAL */

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */

    /* #40 Unmarshal return value. */
    status = packet.ReturnValue.ReturnOs_StatusType;
  }
  /* #50 Otherwise: */
  else
  {
    /* #50 Call the API directly. */
    status = Os_TrapCallCallTrustedFunction(FunctionIndex, FunctionParams);                                             /* SBSW_OS_TRAP_API_USERPOINTER_001 */
  }

  return Os_ErrReportCallTrustedFunction(status, FunctionIndex, FunctionParams);                                        /* SBSW_OS_TRAP_ERRREPORT_001 */
}                                                                                                                       /* PRQA S 6050 */ /* MD_MSR_STCAL */


/***********************************************************************************************************************
 *  Os_CallNonTrustedFunction()
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
 */
StatusType     Os_CallNonTrustedFunction
(
  Os_NonTrustedFunctionIndexType FunctionIndex,
  Os_NonTrustedFunctionParameterRefType FunctionParams
)
{
  Os_StatusType status;

  /* #10 If a trap is needed: */
  if(Os_TrapIsTrapForWriteAccessRequired() != 0u)                                                                       /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */ /* COV_OS_HALPLATFORMTRAPALWAYSNEEDED */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_CALLNONTRUSTEDFUNCTION;
    packet.Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamOs_NonTrustedFunctionIndexType = FunctionIndex;            /* SBSW_OS_PWA_LOCAL */
    packet.Parameters[OS_TRAPPARAMINDEX_2].ParamApiType.ParamOs_NonTrustedFunctionParameterRefType = FunctionParams;    /* SBSW_OS_PWA_LOCAL */

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */

    /* #40 Unmarshal return value. */
    status = packet.ReturnValue.ReturnOs_StatusType;
  }
  /* #50 Otherwise: */
  else
  {
    /* #70 Call the API directly. */
    status = Os_TrapCallCallNonTrustedFunction(FunctionIndex, FunctionParams);                                          /* SBSW_OS_TRAP_API_USERPOINTER_001 */
  }

  return Os_ErrReportCallNonTrustedFunction(status, FunctionIndex, FunctionParams);                                     /* SBSW_OS_TRAP_ERRREPORT_001 */
}                                                                                                                       /* PRQA S 6050 */ /* MD_MSR_STCAL */


/***********************************************************************************************************************
 *  Os_CallFastTrustedFunction()
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
 */
StatusType     Os_CallFastTrustedFunction
(
  Os_FastTrustedFunctionIndexType FunctionIndex,
  Os_FastTrustedFunctionParameterRefType FunctionParams
)
{
  Os_StatusType status;

  /* #10 If a trap is needed: */
  if(Os_TrapIsTrapForWriteAccessRequired() != 0u)                                                                       /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_CALLFASTTRUSTEDFUNCTION;
    packet.Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamOs_FastTrustedFunctionIndexType = FunctionIndex;           /* SBSW_OS_PWA_LOCAL */
    packet.Parameters[OS_TRAPPARAMINDEX_2].ParamApiType.ParamOs_FastTrustedFunctionParameterRefType = FunctionParams;   /* SBSW_OS_PWA_LOCAL */

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */

    /* #40 Unmarshal return value. */
    status = packet.ReturnValue.ReturnOs_StatusType;
  }
  /* #50 Otherwise: */
  else
  {
    /* #60 Call the API directly. */
    status = Os_TrapCallCallFastTrustedFunction(FunctionIndex, FunctionParams);                                         /* SBSW_OS_TRAP_API_USERPOINTER_001 */
  }

  return Os_ErrReportCallFastTrustedFunction(status, FunctionIndex, FunctionParams);                                    /* SBSW_OS_TRAP_ERRREPORT_001 */
}                                                                                                                       /* PRQA S 6050 */ /* MD_MSR_STCAL */


/***********************************************************************************************************************
 *  CheckTaskMemoryAccess()
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
 */
AccessType     CheckTaskMemoryAccess
(
  TaskType TaskID,
  MemoryStartAddressType Address,
  MemorySizeType Size
)
{
  AccessType result;

  /* #10 If a trap is needed: */
  if(Os_TrapIsTrapForReadAccessRequired() != 0u)                                                                        /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */ /* COV_OS_HALPLATFORMTRAPALWAYSNEEDED */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_CHECKTASKMEMORYACCESS;
    packet.Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamTaskType = TaskID;                                         /* SBSW_OS_PWA_LOCAL */
    packet.Parameters[OS_TRAPPARAMINDEX_2].ParamApiType.ParamMemoryStartAddressType = Address;                          /* SBSW_OS_PWA_LOCAL */
    packet.Parameters[OS_TRAPPARAMINDEX_3].ParamApiType.ParamMemorySizeType = Size;                                     /* SBSW_OS_PWA_LOCAL */

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */

    /* #40 Unmarshal return value. */
    result = packet.ReturnValue.ReturnAccessType;
  }
  /* #50 Otherwise: */
  else
  {
    /* #60 Call the API directly. */
    result = Os_TrapCallCheckTaskMemoryAccess(TaskID, Address, Size);                                                   /* SBSW_OS_TRAP_API_USERPOINTER_001 */
  }

  return result;
}


/***********************************************************************************************************************
 *  CheckISRMemoryAccess
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
 */
AccessType     CheckISRMemoryAccess
(
  ISRType ISRID,
  MemoryStartAddressType Address,
  MemorySizeType Size
)
{
  AccessType result;

  /* #10 If a trap is needed: */
  if(Os_TrapIsTrapForReadAccessRequired() != 0u)                                                                        /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */ /* COV_OS_HALPLATFORMTRAPALWAYSNEEDED */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_CHECKISRMEMORYACCESS;
    packet.Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamISRType = ISRID;                                           /* SBSW_OS_PWA_LOCAL */
    packet.Parameters[OS_TRAPPARAMINDEX_2].ParamApiType.ParamMemoryStartAddressType = Address;                          /* SBSW_OS_PWA_LOCAL */
    packet.Parameters[OS_TRAPPARAMINDEX_3].ParamApiType.ParamMemorySizeType = Size;                                     /* SBSW_OS_PWA_LOCAL */

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */

    /* #40 Unmarshal return value. */
    result = packet.ReturnValue.ReturnAccessType;
  }
  /* #50 Otherwise: */
  else
  {
    /* #60 Call the API directly. */
    result = Os_TrapCallCheckISRMemoryAccess(ISRID, Address, Size);                                                     /* SBSW_OS_TRAP_API_USERPOINTER_001 */
  }

  return result;
}


/***********************************************************************************************************************
 *  Os_GetDetailedError()
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
 */
StatusType     Os_GetDetailedError
(
  Os_ErrorInformationRefType ErrorRef
)
{
  Os_StatusType status;

  /* #10 Check that the given pointer is valid. */
  if(OS_UNLIKELY(Os_ErrCheckPointerIsNotNull(ErrorRef) == OS_CHECK_FAILED))                                             /* PRQA S 0315 */ /* MD_Os_Dir1.1_0315 */ /* SBSW_OS_TRAP_ERRCHECKPOINTERISNOTNULL_001 */
  {
    status = OS_STATUS_PARAM_POINTER_1;
  }
  else
  {
    /* #20 If a trap is needed: */
    if(Os_TrapIsTrapForWriteAccessRequired() != 0u)                                                                     /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
    {
      Os_TrapPacketType packet;

      /* #30 Marshal API data. */
      packet.TrapId = OS_TRAPID_GETDETAILEDERROR;
      packet.Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamErrorInformationRefType = ErrorRef;                      /* SBSW_OS_PWA_LOCAL */

      /* #40 Trigger trap. */
      Os_Hal_Trap(&packet);                                                                                             /* SBSW_OS_FC_POINTER2LOCAL */

      /* #50 Unmarshal return value. */
      status = packet.ReturnValue.ReturnOs_StatusType;
    }
    /* #60 Otherwise: */
    else
    {
      /* #70 Call the API directly. */
      status = Os_TrapCallGetDetailedError(ErrorRef);                                                                   /* SBSW_OS_TRAP_API_USERPOINTER_001 */

    }
  }

  /* No error reporting for this API, because the ErrorHook is the only callcontext. */
  return Os_ErrOsStatus2UserStatus(status);
}                                                                                                                       /* PRQA S 6050 */ /* MD_MSR_STCAL */


/***********************************************************************************************************************
 *  OSErrorGetServiceId()
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
 */
OSServiceIdType     OSErrorGetServiceId(void)
{
  OSServiceIdType result;

  /* #10 If a trap is needed: */
  if(Os_TrapIsTrapForWriteAccessRequired() != 0u)                                                                       /* PRQA S 2991, 2992, 2995, 2996 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2995, MD_Os_Rule2.2_2996 */
  {
    Os_TrapPacketType packet;

    /* #20 Marshal API data. */
    packet.TrapId = OS_TRAPID_GETSERVICEID;

    /* #30 Trigger trap. */
    Os_Hal_Trap(&packet);                                                                                               /* SBSW_OS_FC_POINTER2LOCAL */

    /* #40 Unmarshal return value. */
    result = packet.ReturnValue.ReturnOSServiceIdType;
  }
  /* #50 Otherwise: */
  else
  {
    /* #60 Call the API directly. */
    result = Os_TrapCallOSErrorGetServiceId();
  }

  return result;
}


/***********************************************************************************************************************
*  OSError_Os_DisableInterruptSource_ISRID()
**********************************************************************************************************************/
ISRType     OSError_Os_DisableInterruptSource_ISRID(void)
{
  return Os_ErrorGetParameter_ISRType(OS_ERRORPARAMINDEX_1);
}


/***********************************************************************************************************************
*  OSError_Os_EnableInterruptSource_ISRID()
**********************************************************************************************************************/
ISRType     OSError_Os_EnableInterruptSource_ISRID(void)
{
  return Os_ErrorGetParameter_ISRType(OS_ERRORPARAMINDEX_1);
}


/***********************************************************************************************************************
*  OSError_Os_EnableInterruptSource_ClearPending()
**********************************************************************************************************************/
boolean     OSError_Os_EnableInterruptSource_ClearPending(void)
{
  return Os_ErrorGetParameter_boolean(OS_ERRORPARAMINDEX_2);
}


/***********************************************************************************************************************
*  OSError_Os_ClearPendingInterrupt_ISRID()
**********************************************************************************************************************/
ISRType     OSError_Os_ClearPendingInterrupt_ISRID(void)                                                     /* COV_OS_HALPLATFORMCLEARPENDINGAVAILABILITY */
{
  return Os_ErrorGetParameter_ISRType(OS_ERRORPARAMINDEX_1);
}


/***********************************************************************************************************************
*  OSError_Os_IsInterruptSourceEnabled_ISRID()
**********************************************************************************************************************/
ISRType     OSError_Os_IsInterruptSourceEnabled_ISRID(void)
{
  return Os_ErrorGetParameter_ISRType(OS_ERRORPARAMINDEX_1);
}


/***********************************************************************************************************************
*  OSError_Os_IsInterruptSourceEnabled_IsEnabled()
**********************************************************************************************************************/
OS_VAR_NOINIT boolean       * OSError_Os_IsInterruptSourceEnabled_IsEnabled(void)
{
  return Os_ErrorGetParameter_booleanRefType(OS_ERRORPARAMINDEX_2);
}

/***********************************************************************************************************************
*  OSError_Os_IsInterruptPending_ISRID()
**********************************************************************************************************************/
ISRType     OSError_Os_IsInterruptPending_ISRID (void)                                                       /* COV_OS_HALPLATFORMCLEARPENDINGAVAILABILITY */
{
  return Os_ErrorGetParameter_ISRType(OS_ERRORPARAMINDEX_1);
}


/***********************************************************************************************************************
*  OSError_Os_IsInterruptPending_IsPending()
**********************************************************************************************************************/
OS_VAR_NOINIT boolean       * OSError_Os_IsInterruptPending_IsPending (void)                  /* COV_OS_HALPLATFORMCLEARPENDINGAVAILABILITY */
{
  return Os_ErrorGetParameter_booleanRefType(OS_ERRORPARAMINDEX_2);
}

/***********************************************************************************************************************
*  OSError_CallTrustedFunction_FunctionIndex()
**********************************************************************************************************************/
TrustedFunctionIndexType     OSError_CallTrustedFunction_FunctionIndex(void)                                 /* COV_OS_HALPLATFORMTESTEDASMULTICORE */
{
  return Os_ErrorGetParameter_TrustedFunctionIndexType(OS_ERRORPARAMINDEX_1);
}


/***********************************************************************************************************************
 *  OSError_CallTrustedFunction_FunctionParams()
 **********************************************************************************************************************/
TrustedFunctionParameterRefType     OSError_CallTrustedFunction_FunctionParams(void)                         /* COV_OS_HALPLATFORMTESTEDASMULTICORE */
{
  return Os_ErrorGetParameter_TrustedFunctionParameterRefType(OS_ERRORPARAMINDEX_2);
}


/***********************************************************************************************************************
*  OSError_CallFastTrustedFunction_FunctionIndex()
**********************************************************************************************************************/
Os_FastTrustedFunctionIndexType     OSError_CallFastTrustedFunction_FunctionIndex(void)
{
  return Os_ErrorGetParameter_FastTrustedFunctionIndexType(OS_ERRORPARAMINDEX_1);
}


/***********************************************************************************************************************
 *  OSError_CallFastTrustedFunction_FunctionParams()
 **********************************************************************************************************************/
Os_FastTrustedFunctionParameterRefType     OSError_CallFastTrustedFunction_FunctionParams(void)
{
  return Os_ErrorGetParameter_FastTrustedFunctionParameterRefType(OS_ERRORPARAMINDEX_2);
}


/***********************************************************************************************************************
*  OSError_CallNonTrustedFunction_FunctionIndex()
**********************************************************************************************************************/
Os_NonTrustedFunctionIndexType     OSError_CallNonTrustedFunction_FunctionIndex(void)                        /* COV_OS_HALPLATFORMTESTEDASMULTICORE */
{
  return Os_ErrorGetParameter_Os_NonTrustedFunctionIndexType(OS_ERRORPARAMINDEX_1);
}


/***********************************************************************************************************************
 *  OSError_CallNonTrustedFunction_FunctionParams()
 **********************************************************************************************************************/
Os_NonTrustedFunctionParameterRefType     OSError_CallNonTrustedFunction_FunctionParams(void)                /* COV_OS_HALPLATFORMTESTEDASMULTICORE */
{
  return Os_ErrorGetParameter_Os_NonTrustedFunctionParameterRefType(OS_ERRORPARAMINDEX_2);
}


/***********************************************************************************************************************
 *  OSError_StartScheduleTableRel_ScheduleTableID()
 **********************************************************************************************************************/
ScheduleTableType     OSError_StartScheduleTableRel_ScheduleTableID(void)
{
  return Os_ErrorGetParameter_ScheduleTableType(OS_ERRORPARAMINDEX_1);
}


/***********************************************************************************************************************
 *  OSError_StartScheduleTableRel_Offset()
 **********************************************************************************************************************/
TickType     OSError_StartScheduleTableRel_Offset(void)
{
  return Os_ErrorGetParameter_TickType(OS_ERRORPARAMINDEX_2);
}


/***********************************************************************************************************************
 *  OSError_StartScheduleTableAbs_ScheduleTableID()
 **********************************************************************************************************************/
ScheduleTableType     OSError_StartScheduleTableAbs_ScheduleTableID(void)
{
  return Os_ErrorGetParameter_ScheduleTableType(OS_ERRORPARAMINDEX_1);
}


/***********************************************************************************************************************
 *  OSError_StartScheduleTableAbs_Start()
 **********************************************************************************************************************/
TickType     OSError_StartScheduleTableAbs_Start(void)
{
  return Os_ErrorGetParameter_TickType(OS_ERRORPARAMINDEX_2);
}


/***********************************************************************************************************************
 *  OSError_StopScheduleTable_ScheduleTableID()
 **********************************************************************************************************************/
ScheduleTableType     OSError_StopScheduleTable_ScheduleTableID(void)
{
  return Os_ErrorGetParameter_ScheduleTableType(OS_ERRORPARAMINDEX_1);
}


/***********************************************************************************************************************
 *  OSError_NextScheduleTable_ScheduleTableID_From()
 **********************************************************************************************************************/
ScheduleTableType     OSError_NextScheduleTable_ScheduleTableID_From(void)
{
  return Os_ErrorGetParameter_ScheduleTableType(OS_ERRORPARAMINDEX_1);
}


/***********************************************************************************************************************
 *  OSError_NextScheduleTable_ScheduleTableID_To()
 **********************************************************************************************************************/
ScheduleTableType     OSError_NextScheduleTable_ScheduleTableID_To(void)
{
  return Os_ErrorGetParameter_ScheduleTableType(OS_ERRORPARAMINDEX_2);
}


/***********************************************************************************************************************
 *  OSError_StartScheduleTableSynchron_ScheduleTableID()
 **********************************************************************************************************************/
ScheduleTableType     OSError_StartScheduleTableSynchron_ScheduleTableID(void)
{
  return Os_ErrorGetParameter_ScheduleTableType(OS_ERRORPARAMINDEX_1);
}


/***********************************************************************************************************************
 *  OSError_SyncScheduleTable_ScheduleTableID()
 **********************************************************************************************************************/
ScheduleTableType     OSError_SyncScheduleTable_ScheduleTableID(void)
{
  return Os_ErrorGetParameter_ScheduleTableType(OS_ERRORPARAMINDEX_1);
}


/***********************************************************************************************************************
 *  OSError_SyncScheduleTable_Value()
 **********************************************************************************************************************/
TickType     OSError_SyncScheduleTable_Value(void)
{
  return Os_ErrorGetParameter_TickType(OS_ERRORPARAMINDEX_2);
}


/***********************************************************************************************************************
 *  OSError_SetScheduleTableAsync_ScheduleTableID()
 **********************************************************************************************************************/
ScheduleTableType     OSError_SetScheduleTableAsync_ScheduleTableID(void)
{
  return Os_ErrorGetParameter_ScheduleTableType(OS_ERRORPARAMINDEX_1);
}


/***********************************************************************************************************************
 *  OSError_GetScheduleTableStatus_ScheduleTableID()
 **********************************************************************************************************************/
ScheduleTableType     OSError_GetScheduleTableStatus_ScheduleTableID(void)
{
  return Os_ErrorGetParameter_ScheduleTableType(OS_ERRORPARAMINDEX_1);
}


/***********************************************************************************************************************
 *  OSError_GetScheduleTableStatus_ScheduleStatus()
 **********************************************************************************************************************/
ScheduleTableStatusRefType     OSError_GetScheduleTableStatus_ScheduleStatus(void)
{
  return Os_ErrorGetParameter_ScheduleTableStatusRefType(OS_ERRORPARAMINDEX_2);
}


/***********************************************************************************************************************
 *  OSError_IncrementCounter_CounterID()
 **********************************************************************************************************************/
CounterType     OSError_IncrementCounter_CounterID(void)
{
  return Os_ErrorGetParameter_CounterType(OS_ERRORPARAMINDEX_1);
}


/***********************************************************************************************************************
 *  OSError_GetCounterValue_CounterID()
 **********************************************************************************************************************/
CounterType     OSError_GetCounterValue_CounterID(void)
{
  return Os_ErrorGetParameter_CounterType(OS_ERRORPARAMINDEX_1);
}


/***********************************************************************************************************************
 *  OSError_GetCounterValue_Value()
 **********************************************************************************************************************/
TickRefType     OSError_GetCounterValue_Value(void)
{
  return Os_ErrorGetParameter_TickRefType(OS_ERRORPARAMINDEX_2);
}


/***********************************************************************************************************************
 *  OSError_GetElapsedValue_CounterID()
 **********************************************************************************************************************/
CounterType     OSError_GetElapsedValue_CounterID(void)
{
  return Os_ErrorGetParameter_CounterType(OS_ERRORPARAMINDEX_1);
}


/***********************************************************************************************************************
 *  OSError_GetElapsedValue_Value()
 **********************************************************************************************************************/
TickRefType     OSError_GetElapsedValue_Value(void)
{
  return Os_ErrorGetParameter_TickRefType(OS_ERRORPARAMINDEX_2);
}


/***********************************************************************************************************************
 *  OSError_GetElapsedValue_ElapsedValue()
 **********************************************************************************************************************/
TickRefType     OSError_GetElapsedValue_ElapsedValue(void)
{
  return Os_ErrorGetParameter_TickRefType(OS_ERRORPARAMINDEX_3);
}


/***********************************************************************************************************************
 *  OSError_TerminateApplication_Application()
 **********************************************************************************************************************/
ApplicationType     OSError_TerminateApplication_Application(void)
{
  return Os_ErrorGetParameter_ApplicationType(OS_ERRORPARAMINDEX_1);
}


/***********************************************************************************************************************
 *  OSError_TerminateApplication_RestartOption()
 **********************************************************************************************************************/
RestartType     OSError_TerminateApplication_RestartOption(void)
{
  return Os_ErrorGetParameter_RestartType(OS_ERRORPARAMINDEX_2);
}


/***********************************************************************************************************************
 *  OSError_GetApplicationState_Application()
 **********************************************************************************************************************/
ApplicationType     OSError_GetApplicationState_Application(void)
{
  return Os_ErrorGetParameter_ApplicationType(OS_ERRORPARAMINDEX_1);
}


/***********************************************************************************************************************
 *  OSError_GetApplicationState_Value()
 **********************************************************************************************************************/
ApplicationStateRefType     OSError_GetApplicationState_Value(void)
{
  return Os_ErrorGetParameter_ApplicationStateRefType(OS_ERRORPARAMINDEX_2);
}


/***********************************************************************************************************************
 *  OSError_GetSpinlock_SpinlockId()
 **********************************************************************************************************************/
SpinlockIdType     OSError_GetSpinlock_SpinlockId(void)
{
  return Os_ErrorGetParameter_SpinlockIdType(OS_ERRORPARAMINDEX_1);
}


/***********************************************************************************************************************
 *  OSError_ReleaseSpinlock_SpinlockId()
 **********************************************************************************************************************/
SpinlockIdType     OSError_ReleaseSpinlock_SpinlockId(void)
{
  return Os_ErrorGetParameter_SpinlockIdType(OS_ERRORPARAMINDEX_1);
}


/***********************************************************************************************************************
 *  OSError_TryToGetSpinlock_SpinlockId()
 **********************************************************************************************************************/
SpinlockIdType     OSError_TryToGetSpinlock_SpinlockId(void)
{
  return Os_ErrorGetParameter_SpinlockIdType(OS_ERRORPARAMINDEX_1);
}


/***********************************************************************************************************************
 *  OSError_TryToGetSpinlock_Success()
 **********************************************************************************************************************/
TryToGetSpinlockType const *     OSError_TryToGetSpinlock_Success(void)
{
  return Os_ErrorGetParameter_TryToGetSpinlockRefType(OS_ERRORPARAMINDEX_2);
}


/***********************************************************************************************************************
 *  OSError_Os_GetExceptionContext_Context()
 **********************************************************************************************************************/
Os_ExceptionContextRefType     OSError_Os_GetExceptionContext_Context(void)
{
  return Os_ErrorGetParameter_ExceptionContextRefType(OS_ERRORPARAMINDEX_1);
}


/***********************************************************************************************************************
 *  OSError_Os_SetExceptionContext_Context()
 **********************************************************************************************************************/
Os_ExceptionContextRefType     OSError_Os_SetExceptionContext_Context(void)
{
  return Os_ErrorGetParameter_ExceptionContextRefType(OS_ERRORPARAMINDEX_1);
}


/***********************************************************************************************************************
 *  OSError_ControlIdle_CoreID()
 **********************************************************************************************************************/
CoreIdType     OSError_ControlIdle_CoreID(void)                                                              /* COV_OS_HALPLATFORMTESTEDASMULTICORE */
{
  return Os_ErrorGetParameter_CoreIdType(OS_ERRORPARAMINDEX_1);
}


/***********************************************************************************************************************
 *  OSError_ControlIdle_IdleMode()
 **********************************************************************************************************************/
IdleModeType     OSError_ControlIdle_IdleMode(void)                                                          /* COV_OS_HALPLATFORMTESTEDASMULTICORE */
{
  return Os_ErrorGetParameter_IdleModeType(OS_ERRORPARAMINDEX_2);
}


/***********************************************************************************************************************
 *  OSError_BarrierSynchronize_BarrierID()
 **********************************************************************************************************************/
Os_BarrierIdType     OSError_BarrierSynchronize_BarrierID(void)                                              /* COV_OS_TESTSUITEOSC4998 */
{
  return Os_ErrorGetParameter_BarrierIdType(OS_ERRORPARAMINDEX_1);
}


/***********************************************************************************************************************
 *  OSError_ActivateTask_TaskID()
 **********************************************************************************************************************/
TaskType     OSError_ActivateTask_TaskID(void)
{
  return Os_ErrorGetParameter_TaskType(OS_ERRORPARAMINDEX_1);
}


/***********************************************************************************************************************
 *  OSError_ChainTask_TaskID()
 **********************************************************************************************************************/
TaskType     OSError_ChainTask_TaskID(void)
{
  return Os_ErrorGetParameter_TaskType(OS_ERRORPARAMINDEX_1);
}


/***********************************************************************************************************************
 *  OSError_GetTaskID_TaskID()
 **********************************************************************************************************************/
TaskRefType     OSError_GetTaskID_TaskID(void)
{
  return Os_ErrorGetParameter_TaskRefType(OS_ERRORPARAMINDEX_1);
}


/***********************************************************************************************************************
 *  OSError_GetTaskState_TaskID()
 **********************************************************************************************************************/
TaskType     OSError_GetTaskState_TaskID(void)
{
  return Os_ErrorGetParameter_TaskType(OS_ERRORPARAMINDEX_1);
}


/***********************************************************************************************************************
 *  OSError_GetTaskState_State()
 **********************************************************************************************************************/
TaskStateRefType     OSError_GetTaskState_State(void)
{
  return Os_ErrorGetParameter_TaskStateRefType(OS_ERRORPARAMINDEX_2);
}


/***********************************************************************************************************************
 *  OSError_SetEvent_TaskID()
 **********************************************************************************************************************/
TaskType     OSError_SetEvent_TaskID(void)
{
  return Os_ErrorGetParameter_TaskType(OS_ERRORPARAMINDEX_1);
}


/***********************************************************************************************************************
 *  OSError_SetEvent_Mask()
 **********************************************************************************************************************/
EventMaskType     OSError_SetEvent_Mask(void)
{
  return Os_ErrorGetParameter_EventMaskType(OS_ERRORPARAMINDEX_2);
}


/***********************************************************************************************************************
 *  OSError_ClearEvent_Mask()
 **********************************************************************************************************************/
EventMaskType     OSError_ClearEvent_Mask(void)
{
  return Os_ErrorGetParameter_EventMaskType(OS_ERRORPARAMINDEX_1);
}


/***********************************************************************************************************************
 *  OSError_GetEvent_TaskID()
 **********************************************************************************************************************/
TaskType     OSError_GetEvent_TaskID(void)
{
  return Os_ErrorGetParameter_TaskType(OS_ERRORPARAMINDEX_1);
}


/***********************************************************************************************************************
 *  OSError_GetEvent_Mask()
 **********************************************************************************************************************/
EventMaskRefType     OSError_GetEvent_Mask(void)
{
  return Os_ErrorGetParameter_EventMaskRefType(OS_ERRORPARAMINDEX_2);
}


/***********************************************************************************************************************
 *  OSError_WaitEvent_Mask()
 **********************************************************************************************************************/
EventMaskType     OSError_WaitEvent_Mask(void)
{
  return Os_ErrorGetParameter_EventMaskType(OS_ERRORPARAMINDEX_1);
}


/***********************************************************************************************************************
 *  OSError_GetAlarmBase_AlarmID()
 **********************************************************************************************************************/
AlarmType     OSError_GetAlarmBase_AlarmID(void)
{
  return Os_ErrorGetParameter_AlarmType(OS_ERRORPARAMINDEX_1);
}


/***********************************************************************************************************************
 *  OSError_GetAlarmBase_Info()
 **********************************************************************************************************************/
AlarmBaseRefType     OSError_GetAlarmBase_Info(void)
{
  return Os_ErrorGetParameter_AlarmBaseRefType(OS_ERRORPARAMINDEX_2);
}


/***********************************************************************************************************************
 *  OSError_GetAlarm_AlarmID()
 **********************************************************************************************************************/
AlarmType     OSError_GetAlarm_AlarmID(void)
{
  return Os_ErrorGetParameter_AlarmType(OS_ERRORPARAMINDEX_1);
}


/***********************************************************************************************************************
 *  OSError_GetAlarm_Tick()
 **********************************************************************************************************************/
TickRefType     OSError_GetAlarm_Tick(void)
{
  return Os_ErrorGetParameter_TickRefType(OS_ERRORPARAMINDEX_2);
}


/***********************************************************************************************************************
 *  OSError_SetRelAlarm_AlarmID()
 **********************************************************************************************************************/
AlarmType     OSError_SetRelAlarm_AlarmID(void)
{
  return Os_ErrorGetParameter_AlarmType(OS_ERRORPARAMINDEX_1);
}


/***********************************************************************************************************************
 *  OSError_SetRelAlarm_increment()
 **********************************************************************************************************************/
TickType     OSError_SetRelAlarm_increment(void)
{
  return Os_ErrorGetParameter_TickType(OS_ERRORPARAMINDEX_2);
}


/***********************************************************************************************************************
 *  OSError_SetRelAlarm_cycle()
 **********************************************************************************************************************/
TickType     OSError_SetRelAlarm_cycle(void)
{
  return Os_ErrorGetParameter_TickType(OS_ERRORPARAMINDEX_3);
}


/***********************************************************************************************************************
 *  OSError_SetAbsAlarm_AlarmID()
 **********************************************************************************************************************/
AlarmType     OSError_SetAbsAlarm_AlarmID(void)
{
  return Os_ErrorGetParameter_AlarmType(OS_ERRORPARAMINDEX_1);
}


/***********************************************************************************************************************
 *  OSError_SetAbsAlarm_start()
 **********************************************************************************************************************/
TickType     OSError_SetAbsAlarm_start(void)
{
  return Os_ErrorGetParameter_TickType(OS_ERRORPARAMINDEX_2);
}


/***********************************************************************************************************************
 *  OSError_SetAbsAlarm_cycle()
 **********************************************************************************************************************/
TickType     OSError_SetAbsAlarm_cycle(void)
{
  return Os_ErrorGetParameter_TickType(OS_ERRORPARAMINDEX_3);
}


/***********************************************************************************************************************
 *  OSError_CancelAlarm_AlarmID()
 **********************************************************************************************************************/
AlarmType     OSError_CancelAlarm_AlarmID(void)
{
  return Os_ErrorGetParameter_AlarmType(OS_ERRORPARAMINDEX_1);
}


/***********************************************************************************************************************
 *  OSError_GetResource_ResID()
 **********************************************************************************************************************/
ResourceType     OSError_GetResource_ResID(void)
{
  return Os_ErrorGetParameter_ResourceType(OS_ERRORPARAMINDEX_1);
}


/***********************************************************************************************************************
 *  OSError_ReleaseResource_ResID()
 **********************************************************************************************************************/
ResourceType     OSError_ReleaseResource_ResID(void)
{
  return Os_ErrorGetParameter_ResourceType(OS_ERRORPARAMINDEX_1);
}


/***********************************************************************************************************************
 *  OSError_Os_GetUnhandledIrq_InterruptSource()
 **********************************************************************************************************************/
Os_InterruptSourceIdRefType     OSError_Os_GetUnhandledIrq_InterruptSource(void)                             /* COV_OS_HALPLATFORMTESTEDASMULTICORE */
{
  return Os_ErrorGetParameter_InterruptSourceIdRefType(OS_ERRORPARAMINDEX_1);
}


/***********************************************************************************************************************
 *  OSError_Os_GetUnhandledExc_ExceptionSource()
 **********************************************************************************************************************/
Os_ExceptionSourceIdRefType     OSError_Os_GetUnhandledExc_ExceptionSource(void)                             /* COV_OS_HALPLATFORMTESTEDASMULTICORE */
{
  return Os_ErrorGetParameter_ExceptionSourceIdRefType(OS_ERRORPARAMINDEX_1);
}


/***********************************************************************************************************************
 *  Os_TrapHandler()
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
 */
void     Os_TrapHandler
(
    Os_TrapPacketType OS_VAR_NOINIT * Packet,
    Os_Hal_IntTrapStateType OS_VAR_NOINIT * CallerInterruptState
)
{
  /* #10 If MemoryProtection is disabled: KernelPanic. */
  if(Os_MpIsEnabled() == 0u)                                                                                            /* COV_OS_INVSTATE */ /* PRQA S 2992, 2996 */ /* MD_Os_Rule14.3_2992, MD_Os_Rule2.2_2996 */
  {
    Os_ErrKernelPanic();
  }
  /* #20 Otherwise: */
  else
  {
    /* #30 Restore interrupt state from caller. */
    Os_Hal_IntTrapRestore(CallerInterruptState);                                                                        /* SBSW_OS_FC_PRECONDITION */

    /* #40 Call appropriate API.
     *     In case of APIs which may modify the interrupt state (e.g. interrupt API, service functions, spinlocks)
     *     update the interrupt state of the caller. */
    switch(Packet->TrapId)
    {
      case OS_TRAPID_GETCOREID:
        Packet->ReturnValue.ReturnCoreIdType = Os_TrapCallGetCoreID();                                                  /* SBSW_OS_PWA_PRECONDITION */
        break;

      case OS_TRAPID_GETNUMBEROFACTIVATEDCORES:
        Packet->ReturnValue.ReturnUint32Type = Os_TrapCallGetNumberOfActivatedCores();                                  /* SBSW_OS_PWA_PRECONDITION */
        break;

      case OS_TRAPID_GETACTIVEAPPLICATIONMODE:
        Packet->ReturnValue.ReturnAppModeType = Os_TrapCallGetActiveApplicationMode();                                  /* SBSW_OS_PWA_PRECONDITION */
        break;


      case OS_TRAPID_GETTASKID:
        Packet->ReturnValue.ReturnOs_StatusType =                                                                       /* SBSW_OS_PWA_PRECONDITION */
            Os_TrapCallGetTaskID(                                                                                       /* SBSW_OS_FC_PRECONDITION */
                Packet->Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamTaskRefType
            );
        break;

      case OS_TRAPID_GETISRID:
        Packet->ReturnValue.ReturnISRType = Os_TrapCallGetISRID();                                                      /* SBSW_OS_PWA_PRECONDITION */
        break;

      case OS_TRAPID_SHUTDOWNOS:
        Os_TrapCallShutdownOS(
            Packet->Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamStatusType
        );
        break;

      case OS_TRAPID_SHUTDOWNALLCORES:                                                                                  /* COV_OS_HALPLATFORMTESTEDASMULTICORE */
        Os_TrapCallShutdownAllCores(
            Packet->Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamStatusType
        );
        break;

      case OS_TRAPID_DISABLELEVEL:
        Os_TrapCallDisableLevel();
        Os_Hal_IntTrapUpdate(CallerInterruptState);                                                                     /* SBSW_OS_FC_PRECONDITION */
        break;

      case OS_TRAPID_ENABLELEVEL:
        Os_TrapCallEnableLevel();
        Os_Hal_IntTrapUpdate(CallerInterruptState);                                                                     /* SBSW_OS_FC_PRECONDITION */
        break;

      case OS_TRAPID_DISABLEGLOBAL:
        Os_TrapCallDisableGlobal();
        Os_Hal_IntTrapUpdate(CallerInterruptState);                                                                     /* SBSW_OS_FC_PRECONDITION */
        break;

      case OS_TRAPID_ENABLEGLOBAL:
        Os_TrapCallEnableGlobal();
        Os_Hal_IntTrapUpdate(CallerInterruptState);                                                                     /* SBSW_OS_FC_PRECONDITION */
        break;

      case OS_TRAPID_DISABLEALLINTERRUPTS:
        Os_TrapCallDisableAllInterrupts();
        Os_Hal_IntTrapUpdate(CallerInterruptState);                                                                     /* SBSW_OS_FC_PRECONDITION */
        break;

      case OS_TRAPID_ENABLEALLINTERRUPTS:
        Os_TrapCallEnableAllInterrupts();
        Os_Hal_IntTrapUpdate(CallerInterruptState);                                                                     /* SBSW_OS_FC_PRECONDITION */
        break;

      case OS_TRAPID_SUSPENDALLINTERRUPTS:
        Os_TrapCallSuspendAllInterrupts();
        Os_Hal_IntTrapUpdate(CallerInterruptState);                                                                     /* SBSW_OS_FC_PRECONDITION */
        break;

      case OS_TRAPID_RESUMEALLINTERRUPTS:
        Os_TrapCallResumeAllInterrupts();
        Os_Hal_IntTrapUpdate(CallerInterruptState);                                                                     /* SBSW_OS_FC_PRECONDITION */
        break;

      case OS_TRAPID_SUSPENDOSINTERRUPTS:
        Os_TrapCallSuspendOSInterrupts();
        Os_Hal_IntTrapUpdate(CallerInterruptState);                                                                     /* SBSW_OS_FC_PRECONDITION */
        break;

      case OS_TRAPID_RESUMEOSINTERRUPTS:
        Os_TrapCallResumeOSInterrupts();
        Os_Hal_IntTrapUpdate(CallerInterruptState);                                                                     /* SBSW_OS_FC_PRECONDITION */
        break;

      case OS_TRAPID_DISABLEINTERRUPTSOURCE:
        Packet->ReturnValue.ReturnOs_StatusType =                                                                       /* SBSW_OS_PWA_PRECONDITION */
            Os_TrapCallDisableInterruptSource(
                Packet->Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamISRType
            );
        break;

      case OS_TRAPID_ENABLEINTERRUPTSOURCE:
        Packet->ReturnValue.ReturnOs_StatusType =                                                                       /* SBSW_OS_PWA_PRECONDITION */
            Os_TrapCallEnableInterruptSource(
                Packet->Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamISRType,
                Packet->Parameters[OS_TRAPPARAMINDEX_2].ParamApiType.Paramboolean
            );
        break;

      case OS_TRAPID_CLEARPENDINGINTERRUPT:
        Packet->ReturnValue.ReturnOs_StatusType =                                                                       /* SBSW_OS_PWA_PRECONDITION */
            Os_TrapCallClearPendingInterrupt(
                Packet->Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamISRType
            );
        break;

      case OS_TRAPID_ISINTERRUPTSOURCEENABLED:
        Packet->ReturnValue.ReturnOs_StatusType =                                                                       /* SBSW_OS_PWA_PRECONDITION */
            Os_TrapCallIsInterruptSourceEnabled(
                Packet->Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamISRType,
                Packet->Parameters[OS_TRAPPARAMINDEX_2].ParamApiType.ParambooleanRefType                                /* SBSW_OS_FC_PRECONDITION */
            );
        break;

      case OS_TRAPID_ISINTERRUPTPENDING:
        Packet->ReturnValue.ReturnOs_StatusType =                                                                       /* SBSW_OS_PWA_PRECONDITION */
            Os_TrapCallIsInterruptPending(
                Packet->Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamISRType,
                Packet->Parameters[OS_TRAPPARAMINDEX_2].ParamApiType.ParambooleanRefType                                /* SBSW_OS_FC_PRECONDITION */
            );
        break;

      case OS_TRAPID_ALLOWACCESS:
        Packet->ReturnValue.ReturnOs_StatusType = Os_TrapCallAllowAccess();                                             /* SBSW_OS_PWA_PRECONDITION */
        break;

      case OS_TRAPID_GETAPPLICATIONID:
        Packet->ReturnValue.ReturnApplicationType = Os_TrapCallGetApplicationID();                                      /* SBSW_OS_PWA_PRECONDITION */
        break;

      case OS_TRAPID_GETCURRENTAPPLICATIONID:
        Packet->ReturnValue.ReturnApplicationType = Os_TrapCallGetCurrentApplicationID();                               /* SBSW_OS_PWA_PRECONDITION */
        break;

      case OS_TRAPID_GETAPPLICATIONSTATE:
        Packet->ReturnValue.ReturnOs_StatusType =                                                                       /* SBSW_OS_PWA_PRECONDITION */
            Os_TrapCallGetApplicationState(                                                                             /* SBSW_OS_FC_PRECONDITION */
                Packet->Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamApplicationType,
                Packet->Parameters[OS_TRAPPARAMINDEX_2].ParamApiType.ParamApplicationStateRefType
            );
        break;

      case OS_TRAPID_CHECKOBJECTACCESS:
        Packet->ReturnValue.ReturnObjectAccessType =                                                                    /* SBSW_OS_PWA_PRECONDITION */
            Os_TrapCallCheckObjectAccess(
                Packet->Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamApplicationType,
                Packet->Parameters[OS_TRAPPARAMINDEX_2].ParamApiType.ParamObjectTypeType,
                Packet->Parameters[OS_TRAPPARAMINDEX_3].ParamApiType.ParamObjectIdType
            );
        break;

      case OS_TRAPID_CHECKOBJECTOWNERSHIP:
        Packet->ReturnValue.ReturnApplicationType =                                                                     /* SBSW_OS_PWA_PRECONDITION */
            Os_TrapCallCheckObjectOwnership(
                Packet->Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamObjectTypeType,
                Packet->Parameters[OS_TRAPPARAMINDEX_2].ParamApiType.ParamObjectIdType
             );
        break;

      case OS_TRAPID_TERMINATEAPPLICATION:
        Packet->ReturnValue.ReturnOs_StatusType =                                                                       /* SBSW_OS_PWA_PRECONDITION */
            Os_TrapCallTerminateApplication(
                Packet->Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamApplicationType,
                Packet->Parameters[OS_TRAPPARAMINDEX_2].ParamApiType.ParamRestartType
            );
        break;

      case OS_TRAPID_ACTIVATETASK:
        Packet->ReturnValue.ReturnOs_StatusType =                                                                       /* SBSW_OS_PWA_PRECONDITION */
            Os_TrapCallActivateTask(
                Packet->Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamTaskType
            );
        break;

      case OS_TRAPID_TERMINATETASK:
        Packet->ReturnValue.ReturnOs_StatusType = Os_TrapCallTerminateTask();                                           /* SBSW_OS_PWA_PRECONDITION */
        break;

      case OS_TRAPID_CHAINTASK:
        Packet->ReturnValue.ReturnOs_StatusType =                                                                       /* SBSW_OS_PWA_PRECONDITION */
            Os_TrapCallChainTask(
                Packet->Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamTaskType
            );
        break;

      case OS_TRAPID_GETTASKSTATE:
        Packet->ReturnValue.ReturnOs_StatusType =                                                                       /* SBSW_OS_PWA_PRECONDITION */
            Os_TrapCallGetTaskState(                                                                                    /* SBSW_OS_FC_PRECONDITION */
                Packet->Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamTaskType,
                Packet->Parameters[OS_TRAPPARAMINDEX_2].ParamApiType.ParamTaskStateRefType
            );
        break;

      case OS_TRAPID_SCHEDULE:
        Packet->ReturnValue.ReturnOs_StatusType = Os_TrapCallSchedule();                                                /* SBSW_OS_PWA_PRECONDITION */
        break;

      case OS_TRAPID_INCREMENTCOUNTER:
        Packet->ReturnValue.ReturnOs_StatusType =                                                                       /* SBSW_OS_PWA_PRECONDITION */
            Os_TrapCallIncrementCounter(
                Packet->Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamCounterType
            );
        break;

      case OS_TRAPID_GETCOUNTERVALUE:
        Packet->ReturnValue.ReturnOs_StatusType =                                                                       /* SBSW_OS_PWA_PRECONDITION */
            Os_TrapCallGetCounterValue(                                                                                 /* SBSW_OS_FC_PRECONDITION */
                Packet->Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamCounterType,
                Packet->Parameters[OS_TRAPPARAMINDEX_2].ParamApiType.ParamTickRefType
            );
        break;

      case OS_TRAPID_GETELAPSEDVALUE:
        Packet->ReturnValue.ReturnOs_StatusType =                                                                       /* SBSW_OS_PWA_PRECONDITION */
            Os_TrapCallGetElapsedValue(                                                                                 /* SBSW_OS_FC_PRECONDITION */
                Packet->Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamCounterType,
                Packet->Parameters[OS_TRAPPARAMINDEX_2].ParamApiType.ParamTickRefType,
                Packet->Parameters[OS_TRAPPARAMINDEX_3].ParamApiType.ParamTickRefType
            );
        break;

      case OS_TRAPID_GETALARM:
        Packet->ReturnValue.ReturnOs_StatusType =                                                                       /* SBSW_OS_PWA_PRECONDITION */
            Os_TrapCallGetAlarm(                                                                                        /* SBSW_OS_FC_PRECONDITION */
                Packet->Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamAlarmType,
                Packet->Parameters[OS_TRAPPARAMINDEX_2].ParamApiType.ParamTickRefType
            );
        break;

      case OS_TRAPID_GETALARMBASE:
        Packet->ReturnValue.ReturnOs_StatusType =                                                                       /* SBSW_OS_PWA_PRECONDITION */
            Os_TrapCallGetAlarmBase(                                                                                    /* SBSW_OS_FC_PRECONDITION */
                Packet->Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamAlarmType,
                Packet->Parameters[OS_TRAPPARAMINDEX_2].ParamApiType.ParamAlarmBaseRefType
            );
        break;

      case OS_TRAPID_SETRELALARM:
        Packet->ReturnValue.ReturnOs_StatusType =                                                                       /* SBSW_OS_PWA_PRECONDITION */
            Os_TrapCallSetRelAlarm(
                Packet->Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamAlarmType,
                Packet->Parameters[OS_TRAPPARAMINDEX_2].ParamApiType.ParamTickType,
                Packet->Parameters[OS_TRAPPARAMINDEX_3].ParamApiType.ParamTickType
            );
        break;

      case OS_TRAPID_SETABSALARM:
        Packet->ReturnValue.ReturnOs_StatusType =                                                                       /* SBSW_OS_PWA_PRECONDITION */
            Os_TrapCallSetAbsAlarm(
                Packet->Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamAlarmType,
                Packet->Parameters[OS_TRAPPARAMINDEX_2].ParamApiType.ParamTickType,
                Packet->Parameters[OS_TRAPPARAMINDEX_3].ParamApiType.ParamTickType
            );
        break;

      case OS_TRAPID_CANCELALARM:
        Packet->ReturnValue.ReturnOs_StatusType =                                                                       /* SBSW_OS_PWA_PRECONDITION */
            Os_TrapCallCancelAlarm(
                Packet->Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamAlarmType
            );
        break;

      case OS_TRAPID_GETRESOURCE:
        Packet->ReturnValue.ReturnOs_StatusType =                                                                       /* SBSW_OS_PWA_PRECONDITION */
            Os_TrapCallGetResource(
                Packet->Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamResourceType
            );
        Os_Hal_IntTrapUpdate(CallerInterruptState);                                                                     /* SBSW_OS_FC_PRECONDITION */
        break;

      case OS_TRAPID_RELEASERESOURCE:
        Packet->ReturnValue.ReturnOs_StatusType =                                                                       /* SBSW_OS_PWA_PRECONDITION */
            Os_TrapCallReleaseResource(
                Packet->Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamResourceType
            );
        Os_Hal_IntTrapUpdate(CallerInterruptState);                                                                     /* SBSW_OS_FC_PRECONDITION */
        break;

      case OS_TRAPID_SETEVENT:
        Packet->ReturnValue.ReturnOs_StatusType =                                                                       /* SBSW_OS_PWA_PRECONDITION */
            Os_TrapCallSetEvent(
                Packet->Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamTaskType,
                Packet->Parameters[OS_TRAPPARAMINDEX_2].ParamApiType.ParamEventMaskType
            );
        break;

      case OS_TRAPID_CLEAREVENT:
        Packet->ReturnValue.ReturnOs_StatusType =                                                                       /* SBSW_OS_PWA_PRECONDITION */
            Os_TrapCallClearEvent(
                Packet->Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamEventMaskType
            );
        break;

      case OS_TRAPID_WAITEVENT:
        Packet->ReturnValue.ReturnOs_StatusType =                                                                       /* SBSW_OS_PWA_PRECONDITION */
            Os_TrapCallWaitEvent(
                Packet->Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamEventMaskType
            );
        break;

      case OS_TRAPID_GETEVENT:
        Packet->ReturnValue.ReturnOs_StatusType =                                                                       /* SBSW_OS_PWA_PRECONDITION */
            Os_TrapCallGetEvent(                                                                                        /* SBSW_OS_FC_PRECONDITION */
                Packet->Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamTaskType,
                Packet->Parameters[OS_TRAPPARAMINDEX_2].ParamApiType.ParamEventMaskRefType
            );
        break;

      case OS_TRAPID_STARTSCHEDULETABLEREL:
        Packet->ReturnValue.ReturnOs_StatusType =                                                                       /* SBSW_OS_PWA_PRECONDITION */
            Os_TrapCallStartScheduleTableRel(
                Packet->Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamScheduleTableType,
                Packet->Parameters[OS_TRAPPARAMINDEX_2].ParamApiType.ParamTickType
            );
        break;

      case OS_TRAPID_STARTSCHEDULETABLEABS:
        Packet->ReturnValue.ReturnOs_StatusType =                                                                       /* SBSW_OS_PWA_PRECONDITION */
            Os_TrapCallStartScheduleTableAbs(
                Packet->Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamScheduleTableType,
                Packet->Parameters[OS_TRAPPARAMINDEX_2].ParamApiType.ParamTickType
            );
        break;

      case OS_TRAPID_STOPSCHEDULETABLE:
        Packet->ReturnValue.ReturnOs_StatusType =                                                                       /* SBSW_OS_PWA_PRECONDITION */
            Os_TrapCallStopScheduleTable(
                Packet->Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamScheduleTableType
            );
        break;

      case OS_TRAPID_NEXTSCHEDULETABLE:
        Packet->ReturnValue.ReturnOs_StatusType =                                                                       /* SBSW_OS_PWA_PRECONDITION */
            Os_TrapCallNextScheduleTable(
                Packet->Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamScheduleTableType,
                Packet->Parameters[OS_TRAPPARAMINDEX_2].ParamApiType.ParamScheduleTableType
            );
        break;

      case OS_TRAPID_STARTSCHEDULETABLESYNCHRON:
        Packet->ReturnValue.ReturnOs_StatusType =                                                                       /* SBSW_OS_PWA_PRECONDITION */
            Os_TrapCallStartScheduleTableSynchron(
                Packet->Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamScheduleTableType
            );
        break;

      case OS_TRAPID_SETSCHEDULETABLEASYNC:
        Packet->ReturnValue.ReturnOs_StatusType =                                                                       /* SBSW_OS_PWA_PRECONDITION */
            Os_TrapCallSetScheduleTableAsync(
                Packet->Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamScheduleTableType
            );
        break;

      case OS_TRAPID_SYNCSCHEDULETABLE:
        Packet->ReturnValue.ReturnOs_StatusType =                                                                       /* SBSW_OS_PWA_PRECONDITION */
            Os_TrapCallSyncScheduleTable(
                Packet->Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamScheduleTableType,
                Packet->Parameters[OS_TRAPPARAMINDEX_2].ParamApiType.ParamTickType
            );
        break;

      case OS_TRAPID_GETSCHEDULETABLESTATUS:
        Packet->ReturnValue.ReturnOs_StatusType =                                                                       /* SBSW_OS_PWA_PRECONDITION */
            Os_TrapCallGetScheduleTableStatus(                                                                          /* SBSW_OS_FC_PRECONDITION */
                Packet->Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamScheduleTableType,
                Packet->Parameters[OS_TRAPPARAMINDEX_2].ParamApiType.ParamScheduleTableStatusRefType
            );
        break;

      case OS_TRAPID_SPINLOCKGET:
        Packet->ReturnValue.ReturnOs_StatusType =                                                                       /* SBSW_OS_PWA_PRECONDITION */
            Os_TrapCallGetSpinlock(                                                                                     /* SBSW_OS_FC_PRECONDITION */
                Packet->Parameters[OS_TRAPPARAMINDEX_1].ParamSpinlockConfigRefType
            );
        Os_Hal_IntTrapUpdate(CallerInterruptState);                                                                     /* SBSW_OS_FC_PRECONDITION */
        break;

      case OS_TRAPID_SPINLOCKRELEASE:
        Packet->ReturnValue.ReturnOs_StatusType =                                                                       /* SBSW_OS_PWA_PRECONDITION */
            Os_TrapCallReleaseSpinlock(                                                                                 /* SBSW_OS_FC_PRECONDITION */
                Packet->Parameters[OS_TRAPPARAMINDEX_1].ParamSpinlockConfigRefType
            );
        Os_Hal_IntTrapUpdate(CallerInterruptState);                                                                     /* SBSW_OS_FC_PRECONDITION */
        break;

      case OS_TRAPID_SPINLOCKTRYGET:
        Packet->ReturnValue.ReturnOs_StatusType =                                                                       /* SBSW_OS_PWA_PRECONDITION */
            Os_TrapCallTryToGetSpinlock(                                                                                /* SBSW_OS_FC_PRECONDITION */
                Packet->Parameters[OS_TRAPPARAMINDEX_1].ParamSpinlockConfigRefType,
                Packet->Parameters[OS_TRAPPARAMINDEX_2].ParamApiType.ParamTryToGetSpinlockRefType
            );
        Os_Hal_IntTrapUpdate(CallerInterruptState);                                                                     /* SBSW_OS_FC_PRECONDITION */
        break;

      case OS_TRAPID_IOC08RECEIVE:
        Packet->ReturnValue.ReturnStd_ReturnType =                                                                      /* SBSW_OS_PWA_PRECONDITION */
            Os_Ioc08Receive(                                                                                            /* SBSW_OS_FC_PRECONDITION */
                Packet->Parameters[OS_TRAPPARAMINDEX_1].ParamIoc08ReceiveConfigRefType,
                Packet->Parameters[OS_TRAPPARAMINDEX_2].ParamUint8RefType
            );
        break;

      case OS_TRAPID_IOC08EMPTYQUEUE:
        Packet->ReturnValue.ReturnStd_ReturnType =                                                                      /* SBSW_OS_PWA_PRECONDITION */
            Os_Ioc08EmptyQueue(                                                                                         /* SBSW_OS_FC_PRECONDITION */
                Packet->Parameters[OS_TRAPPARAMINDEX_1].ParamIoc08ReceiveConfigRefType
            );
        break;

      case OS_TRAPID_IOC08SEND:
        Packet->ReturnValue.ReturnStd_ReturnType =                                                                      /* SBSW_OS_PWA_PRECONDITION */
            Os_Ioc08Send(                                                                                               /* SBSW_OS_FC_PRECONDITION */
                Packet->Parameters[OS_TRAPPARAMINDEX_1].ParamIoc08SendConfigRefType,
                Packet->Parameters[OS_TRAPPARAMINDEX_2].ParamUint8Type
            );
        break;

      case OS_TRAPID_IOC16RECEIVE:
        Packet->ReturnValue.ReturnStd_ReturnType =                                                                      /* SBSW_OS_PWA_PRECONDITION */
            Os_Ioc16Receive(                                                                                            /* SBSW_OS_FC_PRECONDITION */
                Packet->Parameters[OS_TRAPPARAMINDEX_1].ParamIoc16ReceiveConfigRefType,
                Packet->Parameters[OS_TRAPPARAMINDEX_2].ParamUint16RefType
            );
        break;

      case OS_TRAPID_IOC16EMPTYQUEUE:
        Packet->ReturnValue.ReturnStd_ReturnType =                                                                      /* SBSW_OS_PWA_PRECONDITION */
            Os_Ioc16EmptyQueue(                                                                                         /* SBSW_OS_FC_PRECONDITION */
                Packet->Parameters[OS_TRAPPARAMINDEX_1].ParamIoc16ReceiveConfigRefType
            );
        break;

      case OS_TRAPID_IOC16SEND:
        Packet->ReturnValue.ReturnStd_ReturnType =                                                                      /* SBSW_OS_PWA_PRECONDITION */
            Os_Ioc16Send(                                                                                               /* SBSW_OS_FC_PRECONDITION */
                Packet->Parameters[OS_TRAPPARAMINDEX_1].ParamIoc16SendConfigRefType,
                Packet->Parameters[OS_TRAPPARAMINDEX_2].ParamUint16Type
            );
        break;

      case OS_TRAPID_IOC32RECEIVE:
        Packet->ReturnValue.ReturnStd_ReturnType =                                                                      /* SBSW_OS_PWA_PRECONDITION */
            Os_Ioc32Receive(                                                                                            /* SBSW_OS_FC_PRECONDITION */
                Packet->Parameters[OS_TRAPPARAMINDEX_1].ParamIoc32ReceiveConfigRefType,
                Packet->Parameters[OS_TRAPPARAMINDEX_2].ParamUint32RefType
            );
        break;

      case OS_TRAPID_IOC32EMPTYQUEUE:
        Packet->ReturnValue.ReturnStd_ReturnType =                                                                      /* SBSW_OS_PWA_PRECONDITION */
            Os_Ioc32EmptyQueue(                                                                                         /* SBSW_OS_FC_PRECONDITION */
                Packet->Parameters[OS_TRAPPARAMINDEX_1].ParamIoc32ReceiveConfigRefType
            );
        break;

      case OS_TRAPID_IOC32SEND:
        Packet->ReturnValue.ReturnStd_ReturnType =                                                                      /* SBSW_OS_PWA_PRECONDITION */
            Os_Ioc32Send(                                                                                               /* SBSW_OS_FC_PRECONDITION */
                Packet->Parameters[OS_TRAPPARAMINDEX_1].ParamIoc32SendConfigRefType,
                Packet->Parameters[OS_TRAPPARAMINDEX_2].ParamUint32Type
            );
        break;

      case OS_TRAPID_IOCREFRECEIVE:                                                                                     /* COV_OS_HALPLATFORMTESTEDASMULTICORE */
        Packet->ReturnValue.ReturnStd_ReturnType =                                                                      /* SBSW_OS_PWA_PRECONDITION */
            Os_IocRefReceive(                                                                                           /* SBSW_OS_FC_PRECONDITION */
                Packet->Parameters[OS_TRAPPARAMINDEX_1].ParamIocRefReceiveConfigRefType,
                Packet->Parameters[OS_TRAPPARAMINDEX_2].ParamVoidRefType
            );
        break;

      case OS_TRAPID_IOCREFEMPTYQUEUE:                                                                                  /* COV_OS_HALPLATFORMTESTEDASMULTICORE */
        Packet->ReturnValue.ReturnStd_ReturnType =                                                                      /* SBSW_OS_PWA_PRECONDITION */
            Os_IocRefEmptyQueue(                                                                                        /* SBSW_OS_FC_PRECONDITION */
                Packet->Parameters[OS_TRAPPARAMINDEX_1].ParamIocRefReceiveConfigRefType
            );
        break;

      case OS_TRAPID_IOCREFSEND:                                                                                        /* COV_OS_HALPLATFORMTESTEDASMULTICORE */
        Packet->ReturnValue.ReturnStd_ReturnType =                                                                      /* SBSW_OS_PWA_PRECONDITION */
            Os_IocRefSend(                                                                                              /* SBSW_OS_FC_PRECONDITION */
                Packet->Parameters[OS_TRAPPARAMINDEX_1].ParamIocRefSendConfigRefType,
                Packet->Parameters[OS_TRAPPARAMINDEX_2].ParamVoidConfigRefType
            );
        break;

      case OS_TRAPID_IOCREAD:
        Packet->ReturnValue.ReturnStd_ReturnType =                                                                      /* SBSW_OS_PWA_PRECONDITION */
            Os_IocRead(                                                                                                 /* SBSW_OS_FC_PRECONDITION */
                Packet->Parameters[OS_TRAPPARAMINDEX_1].ParamIocReadConfigRefType,
                Packet->Parameters[OS_TRAPPARAMINDEX_2].ParamVoidRefType
            );
        break;

      case OS_TRAPID_IOCWRITE:
        Packet->ReturnValue.ReturnStd_ReturnType =                                                                      /* SBSW_OS_PWA_PRECONDITION */
            Os_IocWrite(                                                                                                /* SBSW_OS_FC_PRECONDITION */
                Packet->Parameters[OS_TRAPPARAMINDEX_1].ParamIocWriteConfigRefType,
                Packet->Parameters[OS_TRAPPARAMINDEX_2].ParamVoidConfigRefType
            );
        break;

      case OS_TRAPID_IOC08MULTIRECEIVE:                                                                                 /* COV_OS_HALPLATFORM_NEED_NO_TRAP_IOC_MULTIQUEUE */
        Packet->ReturnValue.ReturnStd_ReturnType =                                                                      /* SBSW_OS_PWA_PRECONDITION */
            Os_Ioc08MultiReceive(                                                                                       /* SBSW_OS_FC_PRECONDITION */
                Packet->Parameters[OS_TRAPPARAMINDEX_1].ParamIoc08MultiReceiveConfigRefType,
                Packet->Parameters[OS_TRAPPARAMINDEX_2].ParamUint8RefType
            );
        break;

      case OS_TRAPID_IOC08EMPTYMULTIQUEUE:                                                                              /* COV_OS_HALPLATFORM_NEED_NO_TRAP_IOC_MULTIQUEUE */
        Packet->ReturnValue.ReturnStd_ReturnType =                                                                      /* SBSW_OS_PWA_PRECONDITION */
            Os_Ioc08EmptyMultiQueue(                                                                                    /* SBSW_OS_FC_PRECONDITION */
                Packet->Parameters[OS_TRAPPARAMINDEX_1].ParamIoc08MultiReceiveConfigRefType
            );
        break;

      case OS_TRAPID_IOC16MULTIRECEIVE:                                                                                 /* COV_OS_HALPLATFORM_NEED_NO_TRAP_IOC_MULTIQUEUE */
        Packet->ReturnValue.ReturnStd_ReturnType =                                                                      /* SBSW_OS_PWA_PRECONDITION */
            Os_Ioc16MultiReceive(                                                                                       /* SBSW_OS_FC_PRECONDITION */
                Packet->Parameters[OS_TRAPPARAMINDEX_1].ParamIoc16MultiReceiveConfigRefType,
                Packet->Parameters[OS_TRAPPARAMINDEX_2].ParamUint16RefType
            );
        break;

      case OS_TRAPID_IOC16EMPTYMULTIQUEUE:                                                                              /* COV_OS_HALPLATFORM_NEED_NO_TRAP_IOC_MULTIQUEUE */
        Packet->ReturnValue.ReturnStd_ReturnType =                                                                      /* SBSW_OS_PWA_PRECONDITION */
            Os_Ioc16EmptyMultiQueue(                                                                                    /* SBSW_OS_FC_PRECONDITION */
                Packet->Parameters[OS_TRAPPARAMINDEX_1].ParamIoc16MultiReceiveConfigRefType
            );
        break;

      case OS_TRAPID_IOC32MULTIRECEIVE:                                                                                 /* COV_OS_HALPLATFORM_NEED_NO_TRAP_IOC_MULTIQUEUE */
        Packet->ReturnValue.ReturnStd_ReturnType =                                                                      /* SBSW_OS_PWA_PRECONDITION */
            Os_Ioc32MultiReceive(                                                                                       /* SBSW_OS_FC_PRECONDITION */
                Packet->Parameters[OS_TRAPPARAMINDEX_1].ParamIoc32MultiReceiveConfigRefType,
                Packet->Parameters[OS_TRAPPARAMINDEX_2].ParamUint32RefType
            );
        break;

      case OS_TRAPID_IOC32EMPTYMULTIQUEUE:                                                                              /* COV_OS_HALPLATFORM_NEED_NO_TRAP_IOC_MULTIQUEUE */
        Packet->ReturnValue.ReturnStd_ReturnType =                                                                      /* SBSW_OS_PWA_PRECONDITION */
            Os_Ioc32EmptyMultiQueue(                                                                                    /* SBSW_OS_FC_PRECONDITION */
                Packet->Parameters[OS_TRAPPARAMINDEX_1].ParamIoc32MultiReceiveConfigRefType
            );
        break;

      case OS_TRAPID_IOCREFMULTIRECEIVE:                                                                                /* COV_OS_HALPLATFORM_NEED_NO_TRAP_IOC_MULTIQUEUE */
        Packet->ReturnValue.ReturnStd_ReturnType =                                                                      /* SBSW_OS_PWA_PRECONDITION */
            Os_IocRefMultiReceive(                                                                                      /* SBSW_OS_FC_PRECONDITION */
                Packet->Parameters[OS_TRAPPARAMINDEX_1].ParamIocRefMultiReceiveConfigRefType,
                Packet->Parameters[OS_TRAPPARAMINDEX_2].ParamVoidRefType
            );
        break;

      case OS_TRAPID_IOCREFEMPTYMULTIQUEUE:                                                                             /* COV_OS_HALPLATFORM_NEED_NO_TRAP_IOC_MULTIQUEUE */
        Packet->ReturnValue.ReturnStd_ReturnType =                                                                      /* SBSW_OS_PWA_PRECONDITION */
            Os_IocRefEmptyMultiQueue(                                                                                   /* SBSW_OS_FC_PRECONDITION */
                Packet->Parameters[OS_TRAPPARAMINDEX_1].ParamIocRefMultiReceiveConfigRefType
            );
        break;

      case OS_TRAPID_READPERIPHERAL8:
        Packet->ReturnValue.ReturnUint8Type =                                                                           /* SBSW_OS_PWA_PRECONDITION */
            Os_TrapCallReadPeripheral8(                                                                                 /* SBSW_OS_TRAP_API_USERPOINTER_003 */
                Packet->Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamPeripheralIdType,
                Packet->Parameters[OS_TRAPPARAMINDEX_2].ParamConstUint8RefType
            );
        break;

      case OS_TRAPID_READPERIPHERAL16:
        Packet->ReturnValue.ReturnUint16Type =                                                                          /* SBSW_OS_PWA_PRECONDITION */
            Os_TrapCallReadPeripheral16(                                                                                /* SBSW_OS_TRAP_API_USERPOINTER_003 */
                Packet->Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamPeripheralIdType,
                Packet->Parameters[OS_TRAPPARAMINDEX_2].ParamConstUint16RefType
            );
        break;

      case OS_TRAPID_READPERIPHERAL32:
        Packet->ReturnValue.ReturnUint32Type =                                                                          /* SBSW_OS_PWA_PRECONDITION */
            Os_TrapCallReadPeripheral32(                                                                                /* SBSW_OS_TRAP_API_USERPOINTER_003 */
                Packet->Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamPeripheralIdType,
                Packet->Parameters[OS_TRAPPARAMINDEX_2].ParamConstUint32RefType
            );
        break;

      case OS_TRAPID_WRITEPERIPHERAL8:
        Os_TrapCallWritePeripheral8(                                                                                    /* SBSW_OS_TRAP_API_USERPOINTER_003 */
            Packet->Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamPeripheralIdType,
            Packet->Parameters[OS_TRAPPARAMINDEX_2].ParamUint8RefType,
            Packet->Parameters[OS_TRAPPARAMINDEX_3].ParamUint8Type
        );
        break;

      case OS_TRAPID_WRITEPERIPHERAL16:
        Os_TrapCallWritePeripheral16(                                                                                   /* SBSW_OS_TRAP_API_USERPOINTER_003 */
            Packet->Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamPeripheralIdType,
            Packet->Parameters[OS_TRAPPARAMINDEX_2].ParamUint16RefType,
            Packet->Parameters[OS_TRAPPARAMINDEX_3].ParamUint16Type
        );
        break;

      case OS_TRAPID_WRITEPERIPHERAL32:
        Os_TrapCallWritePeripheral32(                                                                                   /* SBSW_OS_TRAP_API_USERPOINTER_003 */
            Packet->Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamPeripheralIdType,
            Packet->Parameters[OS_TRAPPARAMINDEX_2].ParamUint32RefType,
            Packet->Parameters[OS_TRAPPARAMINDEX_3].ParamUint32Type
        );
        break;

      case OS_TRAPID_MODIFYPERIPHERAL8:
        Os_TrapCallModifyPeripheral8(                                                                                   /* SBSW_OS_TRAP_API_USERPOINTER_003 */
            Packet->Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamPeripheralIdType,
            Packet->Parameters[OS_TRAPPARAMINDEX_2].ParamUint8RefType,
            Packet->Parameters[OS_TRAPPARAMINDEX_3].ParamUint8Type,
            Packet->Parameters[OS_TRAPPARAMINDEX_4].ParamUint8Type
        );
        break;

      case OS_TRAPID_MODIFYPERIPHERAL16:
        Os_TrapCallModifyPeripheral16(                                                                                  /* SBSW_OS_TRAP_API_USERPOINTER_003 */
            Packet->Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamPeripheralIdType,
            Packet->Parameters[OS_TRAPPARAMINDEX_2].ParamUint16RefType,
            Packet->Parameters[OS_TRAPPARAMINDEX_3].ParamUint16Type,
            Packet->Parameters[OS_TRAPPARAMINDEX_4].ParamUint16Type
        );
        break;

      case OS_TRAPID_MODIFYPERIPHERAL32:
        Os_TrapCallModifyPeripheral32(                                                                                  /* SBSW_OS_TRAP_API_USERPOINTER_003 */
            Packet->Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamPeripheralIdType,
            Packet->Parameters[OS_TRAPPARAMINDEX_2].ParamUint32RefType,
            Packet->Parameters[OS_TRAPPARAMINDEX_3].ParamUint32Type,
            Packet->Parameters[OS_TRAPPARAMINDEX_4].ParamUint32Type
        );
        break;

      case OS_TRAPID_CALLTRUSTEDFUNCTION:
        Packet->ReturnValue.ReturnOs_StatusType =                                                                       /* SBSW_OS_PWA_PRECONDITION */
            Os_TrapCallCallTrustedFunction(                                                                             /* SBSW_OS_FC_PRECONDITION */
                Packet->Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamTrustedFunctionIndexType,
                Packet->Parameters[OS_TRAPPARAMINDEX_2].ParamApiType.ParamTrustedFunctionParameterRefType
            );
        Os_Hal_IntTrapUpdate(CallerInterruptState);                                                                     /* SBSW_OS_FC_PRECONDITION */
        break;

      case OS_TRAPID_CALLNONTRUSTEDFUNCTION:
        Packet->ReturnValue.ReturnOs_StatusType =                                                                       /* SBSW_OS_PWA_PRECONDITION */
            Os_TrapCallCallNonTrustedFunction(                                                                          /* SBSW_OS_FC_PRECONDITION */
                Packet->Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamOs_NonTrustedFunctionIndexType,
                Packet->Parameters[OS_TRAPPARAMINDEX_2].ParamApiType.ParamOs_NonTrustedFunctionParameterRefType
            );
        Os_Hal_IntTrapUpdate(CallerInterruptState);                                                                     /* SBSW_OS_FC_PRECONDITION */
        break;

      case OS_TRAPID_CALLFASTTRUSTEDFUNCTION:
        Packet->ReturnValue.ReturnOs_StatusType =                                                                       /* SBSW_OS_PWA_PRECONDITION */
            Os_TrapCallCallFastTrustedFunction(                                                                         /* SBSW_OS_FC_PRECONDITION */
                Packet->Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamOs_FastTrustedFunctionIndexType,
                Packet->Parameters[OS_TRAPPARAMINDEX_2].ParamApiType.ParamOs_FastTrustedFunctionParameterRefType
            );
        Os_Hal_IntTrapUpdate(CallerInterruptState);                                                                     /* SBSW_OS_FC_PRECONDITION */
        break;

      case OS_TRAPID_ERRAPPLICATIONERROR:
        Os_ErrApplicationError(                                                                                         /* SBSW_OS_FC_PRECONDITION */
            Packet->Parameters[OS_TRAPPARAMINDEX_1].ParamOSServiceIdType,
            Packet->Parameters[OS_TRAPPARAMINDEX_2].ParamOs_StatusType,
            Packet->Parameters[OS_TRAPPARAMINDEX_3].ParamOs_ErrorParamsRefType
        );
        break;

      case OS_TRAPID_TASKMISSINGTERMINATETASK:
        Os_TaskMissingTerminateTask();
        break;

      case OS_TRAPID_ISREPILOGUE:
        Os_IsrEpilogue();
        break;

      case OS_TRAPID_SERVICERETURN:
        Os_ServiceReturn();
        break;

      case OS_TRAPID_HOOKRETURN:
        Os_HookReturn();
        break;

      case OS_TRAPID_CONTROLIDLE:                                                                                       /* COV_OS_HALPLATFORMTESTEDASMULTICORE */
        Packet->ReturnValue.ReturnOs_StatusType =                                                                       /* SBSW_OS_PWA_PRECONDITION */
            Os_TrapCallControlIdle(
                Packet->Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamCoreIdType,
                Packet->Parameters[OS_TRAPPARAMINDEX_2].ParamApiType.ParamIdleModeType
            );
        break;

      case OS_TRAPID_GETTASKSTACKUSAGE:
        Packet->ReturnValue.ReturnUint32Type =                                                                          /* SBSW_OS_PWA_PRECONDITION */
            Os_TrapCallGetTaskStackUsage(
                Packet->Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamTaskType
            );
        break;

      case OS_TRAPID_GETISRSTACKUSAGE:
        Packet->ReturnValue.ReturnUint32Type =                                                                          /* SBSW_OS_PWA_PRECONDITION */
            Os_TrapCallGetISRStackUsage(
                Packet->Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamISRType
            );
        break;

      case OS_TRAPID_GETKERNELSTACKUSAGE:
        Packet->ReturnValue.ReturnUint32Type =                                                                          /* SBSW_OS_PWA_PRECONDITION */
            Os_TrapCallGetKernelStackUsage(
                Packet->Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamCoreIdType
            );
        break;

      case OS_TRAPID_GETSTARTUPHOOKSTACKUSAGE:
        Packet->ReturnValue.ReturnUint32Type =                                                                          /* SBSW_OS_PWA_PRECONDITION */
            Os_TrapCallGetStartupHookStackUsage(
                Packet->Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamCoreIdType
            );
        break;

      case OS_TRAPID_GETERRORHOOKSTACKUSAGE:
        Packet->ReturnValue.ReturnUint32Type =                                                                          /* SBSW_OS_PWA_PRECONDITION */
            Os_TrapCallGetErrorHookStackUsage(
                Packet->Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamCoreIdType
            );
        break;

      case OS_TRAPID_GETSHUTDOWNHOOKSTACKUSAGE:
        Packet->ReturnValue.ReturnUint32Type =                                                                          /* SBSW_OS_PWA_PRECONDITION */
            Os_TrapCallGetShutdownHookStackUsage(
                Packet->Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamCoreIdType
            );
        break;

      case OS_TRAPID_GETPROTECTIONHOOKSTACKUSAGE:
        Packet->ReturnValue.ReturnUint32Type =                                                                          /* SBSW_OS_PWA_PRECONDITION */
            Os_TrapCallGetProtectionHookStackUsage(
                Packet->Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamCoreIdType
            );
        break;

      case OS_TRAPID_GETNONTRUSTEDFUNCTIONSTACKUSAGE:
        Packet->ReturnValue.ReturnUint32Type =                                                                          /* SBSW_OS_PWA_PRECONDITION */
            Os_TrapCallGetNonTrustedFunctionStackUsage(
                Packet->Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamOs_NonTrustedFunctionIndexType
            );
        break;

      case OS_TRAPID_GETUNHANDLEDIRQ:                                                                                   /* COV_OS_HALPLATFORMTESTEDASMULTICORE */
        Packet->ReturnValue.ReturnOs_StatusType =                                                                       /* SBSW_OS_PWA_PRECONDITION */
            Os_TrapCallGetUnhandledIrq(                                                                                 /* SBSW_OS_FC_PRECONDITION */
                Packet->Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamInterruptSourceIdRefType
            );
        break;

      case OS_TRAPID_GETUNHANDLEDEXC:                                                                                   /* COV_OS_HALPLATFORMTESTEDASMULTICORE */
        Packet->ReturnValue.ReturnOs_StatusType =                                                                       /* SBSW_OS_PWA_PRECONDITION */
            Os_TrapCallGetUnhandledExc(                                                                                 /* SBSW_OS_FC_PRECONDITION */
                Packet->Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamExceptionSourceIdRefType
            );
        break;

      case OS_TRAPID_GETDETAILEDERROR:
        Packet->ReturnValue.ReturnOs_StatusType =                                                                       /* SBSW_OS_PWA_PRECONDITION */
            Os_TrapCallGetDetailedError(                                                                                /* SBSW_OS_FC_PRECONDITION */
                Packet->Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamErrorInformationRefType
            );
        break;

      case OS_TRAPID_GETSERVICEID:
        Packet->ReturnValue.ReturnOSServiceIdType = Os_TrapCallOSErrorGetServiceId();                                   /* SBSW_OS_PWA_PRECONDITION */
        break;

      case OS_TRAPID_COREGETTHREAD:
        Packet->ReturnValue.ReturnThreadConfigRefType = Os_CoreGetThread();                                             /* SBSW_OS_PWA_PRECONDITION */
        break;

      case OS_TRAPID_CHECKTASKMEMORYACCESS:
        Packet->ReturnValue.ReturnAccessType =                                                                          /* SBSW_OS_PWA_PRECONDITION */
            Os_TrapCallCheckTaskMemoryAccess(                                                                           /* SBSW_OS_FC_PRECONDITION */
                Packet->Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamTaskType,
                Packet->Parameters[OS_TRAPPARAMINDEX_2].ParamApiType.ParamMemoryStartAddressType,
                Packet->Parameters[OS_TRAPPARAMINDEX_3].ParamApiType.ParamMemorySizeType
            );
        break;

      case OS_TRAPID_CHECKISRMEMORYACCESS:
        Packet->ReturnValue.ReturnAccessType =                                                                          /* SBSW_OS_PWA_PRECONDITION */
            Os_TrapCallCheckISRMemoryAccess(                                                                            /* SBSW_OS_FC_PRECONDITION */
                Packet->Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamISRType,
                Packet->Parameters[OS_TRAPPARAMINDEX_2].ParamApiType.ParamMemoryStartAddressType,
                Packet->Parameters[OS_TRAPPARAMINDEX_3].ParamApiType.ParamMemorySizeType
            );
        break;

      case OS_TRAPID_BARRIERSYNCHRONIZE:                                                                                /* COV_OS_TESTSUITEOSC4998 */
        Packet->ReturnValue.ReturnOs_StatusType =                                                                       /* SBSW_OS_PWA_PRECONDITION */
            Os_TrapCallBarrierSynchronize(                                                                              /* SBSW_OS_FC_PRECONDITION */
                Packet->Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamBarrierIdType
            );
        break;

      case OS_TRAPID_GETEXCEPTIONCONTEXT:
        Packet->ReturnValue.ReturnOs_StatusType =                                                                       /* SBSW_OS_PWA_PRECONDITION */
            Os_TrapCallGetExceptionContext(                                                                             /* SBSW_OS_FC_PRECONDITION */
                Packet->Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamExceptionContextRefType
            );
        break;

      case OS_TRAPID_SETEXCEPTIONCONTEXT:
        Packet->ReturnValue.ReturnOs_StatusType =                                                                       /* SBSW_OS_PWA_PRECONDITION */
            Os_TrapCallSetExceptionContext(                                                                             /* SBSW_OS_FC_PRECONDITION */
                Packet->Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamExceptionContextRefType
            );
        break;

      case OS_TRAPID_GETCORESTARTSTATE:
            Os_TrapCallGetCoreStartState(                                                                               /* SBSW_OS_FC_PRECONDITION */
                Packet->Parameters[OS_TRAPPARAMINDEX_1].ParamApiType.ParamCoreIdType,
                Packet->Parameters[OS_TRAPPARAMINDEX_2].ParamApiType.ParamCoreStartStateRefType,
                Packet->Parameters[OS_TRAPPARAMINDEX_3].ParamApiType.ParamStatusRefType
            );
        break;

      case OS_TRAPID_GETEXCEPTIONADDRESS:
        Packet->ReturnValue.ReturnOs_AddressOfConstType = Os_TrapCallGetExceptionAddress();                             /* SBSW_OS_PWA_PRECONDITION */
        break;

      default:                                                                                                          /* COV_OS_INVSTATE */
        Os_ErrKernelPanic();
        break;                                                                                                          /* PRQA S 2880 */ /* MD_Os_Rule2.1_2880_NonReturning */
    }
  }
}                                                                                                                       /* PRQA S 6010, 6030, 6050 */ /* MD_MSR_STPTH, MD_MSR_STCYC, MD_MSR_STCAL */



#define OS_STOP_SEC_CODE
#include "Os_MemMap_OsCode.h"                                                                                           /* PRQA S 5087 */ /* MD_MSR_MemMap */


/* SBSW_JUSTIFICATION_BEGIN

\ID SBSW_OS_CALLSERVICEFUNCTION_001
 \DESCRIPTION    Os_Api_Call*TrustedFunction is called with a pointer passed to the caller. Pointer may be a ((void *)0).
 \COUNTERMEASURE \N Os_Api_Call*TrustedFunction is able to handle ((void *)0).

\ID SBSW_OS_THREADGETCURRENTAPPLICATION_001
 \DESCRIPTION    Os_ThreadGetCurrentApplication is called with the return value of Os_CoreGetThread.
 \COUNTERMEASURE \N [CM_OS_COREGETTHREAD_N]

\ID SBSW_OS_THREADGETMPACCESSRIGHTS_001
 \DESCRIPTION    Os_ThreadGetMpAccessRights is called with the return value of Os_CoreGetThread.
 \COUNTERMEASURE \N [CM_OS_COREGETTHREAD_N]

\ID SBSW_OS_THREADGETCORE_001
 \DESCRIPTION    Os_ThreadGetCore is called with the return value of Os_CoreGetThread.
 \COUNTERMEASURE \N [CM_OS_COREGETTHREAD_N]

\ID SBSW_OS_APPISPRIVILEGED_001
 \DESCRIPTION    Os_AppIsPrivileged is called with the return value of Os_ThreadGetCurrentApplication.
 \COUNTERMEASURE \N [CM_OS_THREADGETCURRENTAPPLICATION_N]

\ID SBSW_OS_TRAP_API_USERPOINTER_001
 \DESCRIPTION    A pointer passed as OS API parameter is passed to the API implementation.
 \COUNTERMEASURE \S User has to pass valid pointer or use memory protection.

\ID SBSW_OS_TRAP_API_USERPOINTER_002
 \DESCRIPTION    Pointer write access to a pointer passed as OS API parameter.
 \COUNTERMEASURE \S User has to pass valid pointer or use memory protection.

\ID SBSW_OS_TRAP_API_USERPOINTER_003
 \DESCRIPTION    A pointer passed as OS API parameter is passed to the API implementation.
 \COUNTERMEASURE \N No precondition for given pointer. Pointer is checked by the called API.

\ID SBSW_OS_TRAP_ERRCHECKPOINTERISNOTNULL_001
 \DESCRIPTION    Os_ErrCheckPointerIsNotNull is called with a value passed as argument to the caller.
 \COUNTERMEASURE \N [CM_OS_ERRCHECKPOINTERISNOTNULL_N]

\ID SBSW_OS_TRAP_ERRREPORT_001
 \DESCRIPTION    An Os_ErrReport function is called with a value passed as argument to the caller.
 \COUNTERMEASURE \N [CM_OS_ERRREPORT_N]

SBSW_JUSTIFICATION_END */

/* START_COVERAGE_JUSTIFICATION

\ID COV_OS_TRAPALWAYSNEEDED
   \ACCEPT TX
   \REASON The function is called only in the case the trap has to be used. Therefore the false branch is not covered.
           The risk of keeping the code is considered to be low as it is unit tested and reviewed.

\ID COV_OS_PERIPHERAL_DEPRECATED
   \ACCEPT XX
   \REASON The API is a wrapper which supports backward compatibility and not intended for new designs.
           Functionality is ensured by review.

\ID COV_OS_TRAPMISSINGTEST
   \ACCEPT XX
   \ACCEPT TX
   \ACCEPT XF
   \REASON The condition could be evaluated to true and false with additional tests. However, as very similar functions
           are tested with different results of this condition, the risk of the missing tests is considered to be small.
           A work item exists to remind the developers about the missing tests: WORKITEM15088: #Test: Enhance Coverage
           by creation of additional tests

END_COVERAGE_JUSTIFICATION */



/*!
 * \}
 */

/***********************************************************************************************************************
 *  END OF FILE: Os_Trap.c
 **********************************************************************************************************************/
