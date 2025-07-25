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
 *  \defgroup    Os_Core     Core
 *  \brief       OS execution control of one or multiple cores.
 *  \details
 *  The core module is responsible for managing the information and functionality which are dedicated to one or
 *  multiple cores.
 *
 *  Configuration
 *  ------------------
 *
 *  In AUTOSAR the user configures OS-Applications, and uses a core reference to assigns them to a core.
 *  AUTOSAR does not request any core configuration inside of the OS container.
 *  But the OS has to provide logical IDs for all physically available cores (CoreIdType).
 *
 *  The OS generator needs to know which core is an AUTOSAR core, to be able to prepare
 *  the idle task, system application and core stacks (kernel stack, system hook stacks).
 *
 *  AUTOSAR OS defines the number of cores (OsNumberOfCores). Which defines the maximum number of cores
 *  that are controlled by the AUTOSAR OS. This is redundant information.
 *
 *  MICROSAR OS forces the user to configure cores, to illustrate this behavior a bit more.
 *  This is redundant information.
 *
 *
 *  Startup
 *  ---------------------
 *
 *  ### Multi-Core Startup ###
 *  Cores can be started using \ref StartCore() and \ref StartNonAutosarCore().
 *
 *  AUTOSAR does not request to start all cores which are referenced in OS applications.
 *  How many AUTOSAR cores are started can be decided during runtime in the startup sequence.
 *  The API GetNumberOfActivatedCores() tells how many cores have been started as AUTOSAR cores.
 *
 *  AUTOSAR does not prohibit from starting a core as a Non-AUTOSAR core, although it is referenced in OS applications.
 *
 *  This means, the number of the participants, which are synchronized at the synchronization barriers have to
 *  be determined dynamically during startup.
 *
 *  ![Multi-Core Startup](Core_StartupMultiCore.png)
 *
 *
 *  ### Startup with PreStartTask ###
 *  ![Startup with PreStartTask](Core_StartupPreStartTask.png)
 *
 *
 *  ### OS Initialization ###
 *  ![OS Initialization](Core_OsInitialization.png)
 *
 *  ### Start OS ###
 *
 *  ![Enter the IdleTask](Core_StartOSEnterTheIdleTask.png)
 *
 *  ![Inside the IdleTask](Core_StartOSInsideTheIdleTask.png)
 *
 *  \see \ref Os_Application
 *
 *  Shutdown
 *  ---------------------
 *
 *  ### Single-Core Shutdown ###
 *  ![Single-Core Shutdown](Core_ShutdownSingleCore.png)
 *
 *
 *  ### Multi-Core Shutdown ###
 *  ![Multi-Core Shutdown](Core_ShutdownMultiCore.png)
 *
 *
 *
 *  Thread Management
 *  ---------------------
 *
 *  Threads are managed in three ways:
 *    - The current thread pointer is stored in the HAL in a efficient way (e.g. special purpose register).
 *    - The list of interrupted threads is managed in a stack:
 *      - On each ISR/Hook call, the previous thread is pushed on the stack.
 *      - On return from ISR/Hook, the thread is popped from the stack.
 *    - Tasks a managed by the Scheduler.
 *
 *  ![Thread Management](Core_ThreadManagement.png)
 *
 *
 *  ### Unhandled Exception/Interrupt Handling ###
 *  ![Unhandled Exception Handling](Core_UnhandledException.png)
 *
 *  ![Unhandled Interrupt Handling](Core_UnhandledInterrupt.png)
 *
 *
 *  Internal comment removed.
 *
 *
 *
 *
 *
 *  \trace      CREQ-1201
 *  \trace      SPEC-63925
 *
 * \{
 * \file
 * \brief       OS internal functions for execution control of one or multiple cores.
 * \details     --no details--
 *
 **********************************************************************************************************************/
/***********************************************************************************************************************
 *  REVISION HISTORY
 *  --------------------------------------------------------------------------------------------------------------------
 *  Refer to Os.h.
 **********************************************************************************************************************/

#ifndef OS_COREINT_H
# define OS_COREINT_H
                                                                                                                         /* PRQA S 0388 EOF */ /* MD_MSR_Dir1.1 */
/***********************************************************************************************************************
 *  INCLUDES
 **********************************************************************************************************************/
/* AUTOSAR includes */
# include "Std_Types.h"

/* Os module dependencies */
# include "Os_Core_Cfg.h"
# include "Os_Core_Lcfg.h"
# include "Os_Core_Types.h"

/* Os kernel module dependencies */
# include "Os_Scheduler_Types.h"
# include "Os_Resource_Types.h"
# include "Os_Application_Types.h"
# include "Os_ServiceFunction_Types.h"
# include "Os_Common_Types.h"
# include "Os_Hook_Types.h"
# include "Os_Lcfg.h"
# include "Os_XSignal_Types.h"
# include "Os_TimingProtection_Types.h"
# include "Os_Ioc_Types.h"
# include "Os_Isr_Types.h"
# include "Os_ErrorInt.h"
# include "Os_SchedulerInt.h"
# include "Os_InterruptInt.h"
# include "Os_StackInt.h"
# include "Os_SpinlockInt.h"
# include "Os_BarrierInt.h"
# include "Os_DequeInt.h"
# include "Os_MemoryProtection_Types.h"
# include "Os_Trace_Types.h"
# include "Os_Thread_Types.h"
# include "Os_Task_Types.h"

/* Os Hal dependencies */
# include "Os_Hal_OsInt.h"
# include "Os_Hal_CoreInt.h"
# include "Os_Hal_CompilerInt.h"


/***********************************************************************************************************************
 *  GLOBAL CONSTANT MACROS
 **********************************************************************************************************************/
/*! Maximum hook nesting depth.
 *  (StartupHook -> ErrorHook -> ProtectionHook -> ShutdownHook.
 *  Each Hook is may use IOC, which may result in an IOC-Callback call.) */
#define OS_MAX_HOOKNESTINGDEPTH                 (8u)

/*! Maximum thread nesting depth. (ISR2 nesting depth + hook nesting depth) */
#define OS_MAX_INTERRUPTEDTHREADS               ((Os_CoreThreadIdx)(OS_CFG_NUM_ISRLEVELS + OS_MAX_HOOKNESTINGDEPTH))

/*! The pattern for the first echo procedure by synchronizing multi-core booting process. */
#define OS_CORE_BOOTBARRIER_ECHOPATTERN1                    (0xABCD5678UL)

/*! The pattern for the second echo procedure by synchronizing multi-core booting process. */
#define OS_CORE_BOOTBARRIER_ECHOPATTERN2                    (0x5678ABCDUL)

/*! The pattern indicating that the core should wait. */
#define OS_CORE_BOOTBARRIER_WAITINGSIGN                     (0x1234ABCDUL)

/*! The pattern indicating that the core should continue with hardware initialization. */
#define OS_CORE_BOOTBARRIER_INITHARDWARE                    (0xABCD1234UL)

/*! The pattern indicating that the core is finished with hardware initialization. */
#define OS_CORE_BOOTBARRIER_INITHARDWARE_COMPLETED          (0x4321DCBAUL)

/*! The pattern indicating that the core is started via StartCore. */
#define OS_CORE_BOOTBARRIER_STARTCORE                       (0xDCBA4321UL)

/***********************************************************************************************************************
 *  GLOBAL FUNCTION MACROS
 **********************************************************************************************************************/

/*! Dummy cast to make generator's life easier. */
#define OS_CORE_CASTCONFIG_CORE_2_CORE(core)                 (&(core))

/*! Type cast from Os_CoreAsrConfigType to Os_CoreConfigType by use of base element addressing. */
#define OS_CORE_CASTCONFIG_COREASR_2_CORE(core)              (&((core).Core))

/*! Dummy cast to make generator's life easier. */
#define OS_CORE_CASTSTATUS_CORESTATUS_2_CORESTATUS(core)     (&(core))

/*! Type cast from Os_CoreAsrStatusType to Os_CoreStatusType by use of base element addressing. */
#define OS_CORE_CASTSTATUS_COREASRSTATUS_2_CORESTATUS(core)  (&((core).Core))


/***********************************************************************************************************************
 *  GLOBAL DATA TYPES AND STRUCTURES
 **********************************************************************************************************************/


/*! \brief Defines the allowed activation states of cores.
 *  \details
 *  ![Core State Machine](Core_StateMachine.png)
 *
 */
typedef enum
{
  /*! The core is not started. */
  OS_CORESTATE_INACTIVE = 0,
  /*! The core is started as AUTOSAR core. */
  OS_CORESTATE_ACTIVATED_ASR,
  /*! The core is started as non-AUTOSAR core. */
  OS_CORESTATE_ACTIVATED_NONASR
}Os_CoreStateType;


/*! \brief Defines the status of the OS on a core.
 *  \details
 *  ![OS State Machine](OS_StateMachine.png)
 *
 */
typedef enum
{
  /*! The core is not started. */
  OS_COREOSSTATE_INITIALZE = 0,
  /*! In Pre-Start Task. */
  OS_COREOSSTATE_PRESTARTTASK,
  /*! The core is started as AUTOSAR core. */
  OS_COREOSSTATE_STARTED
}Os_CoreOsStateType;


/*! Dynamic management information of a processor core. */
struct Os_CoreStatusType_Tag
{
  /*! The activation state of the core. */
  Os_CoreStateType ActivationState;

  /*! Output mail box for core start request. */
  Os_CoreStateType CoreStartRequests[OS_COREID_COUNT];
};


/*! Dynamic management information of an AUTOSAR core.
 *  \extends Os_CoreStatusType */
struct Os_CoreAsrStatusType_Tag
{
  /*! General core attributes. */
  Os_CoreStatusType Core;

  /*! The OS start state of the local core */
  Os_CoreOsStateType OsState;

  /*! The application mode in which the OS is started. */
  AppModeType ApplicationMode;

  /*! Tells whether the system is running in critical user section. */
  uint32 CriticalUserSection;
};


/*! Pointer to spinlock configuration */
typedef const   Os_SpinlockConfigType OS_CONST * Os_SpinlockConfigRefType;


/*! Pointer to stack configuration */
typedef const   Os_StackConfigType OS_CONST * Os_StackConfigRefType;


/*! Pointer to IOC configuration */
typedef const   Os_IocConfigType OS_CONST * Os_IocConfigRefType;


/*! Entry type of the interrupted threads stack. */
typedef const   Os_ThreadConfigType OS_CONST * Os_CoreInterruptedThreadsType;


/*! Index of a thread within a list of threads. */
typedef sint16 Os_CoreThreadIdx;


/*! Pointer to interrupt source configuration. */
typedef const   Os_IsrHwConfigType OS_CONST * Os_InterruptSourceConfigRefType;


/*! Configuration information of a processor core. */
struct Os_CoreConfigType_Tag
{
  /*! Dynamic status data (not cached). */
    volatile Os_CoreStatusType OS_VAR_ZERO_INIT * Status;

  /*! HAL specific core configuration. */
  const   Os_Hal_CoreConfigType OS_CONST * HwConfig;

  /*! List of core specific interrupt source objects. */
  const   Os_InterruptSourceConfigRefType OS_CONST * InterruptSourceRefs;

  /*! Number of interrupt source objects. */
  Os_ObjIdxType InterruptSourceCount;

  /*! Logical core ID (tells also whether the core is the master core). */
  CoreIdType Id;

  /*! Trace information for this core */
    Os_TraceCoreType OS_VAR_NOINIT * Trace;

  /*! Tells whether the core can be started as an AUTOSAR core. */
  boolean IsAsrCore;

  /*! Tells whether the core is auto-started after reset. */
  boolean IsAutostart;

  /*! Tells whether the core can start a master core. */
  boolean IsMasterStartAllowed;

  /*! Tells whether the core shall be used for system wide hardware initialization. */
  boolean IsHardwareInitCore;
};


/*! \brief    Dynamic management information of an AUTOSAR core.
 *  \details  --no details--
 */
struct Os_CoreAsrType_Tag
{
  /*! Interrupt API state. */
  Os_IntApiStateType IntApiState;

  /*! Counter for interrupted threads. */
  Os_CoreThreadIdx InterruptedThreadsCounter;

  /*! Stack of currently interrupted threads. */
  Os_CoreInterruptedThreadsType InterruptedThreads[OS_MAX_INTERRUPTEDTHREADS];

  /*! Detailed information about the last error which has been detected. */
  Os_ErrorType LastError;

  /*! Interrupt source of the last interrupt, which occurred but is not handled by the OS. */
  Os_InterruptSourceIdType LastUnhandledIrq;

  /*! Unhandled interrupt reported since start up. */
  Os_UnhandledEventReportType UnhandledIrqReported;

  /*! Exception source of the last exception, which occurred but is not handled by the OS. */
  Os_ExceptionSourceIdType LastUnhandledExc;

  /*! Unhandled exception reported since start up. */
  Os_UnhandledEventReportType UnhandledExcReported;

  /*! The mode of the idle behavior. */
  IdleModeType IdleMode;

  /*! Tells which hook is currently executing. */
  Os_HookInfoType HookInfo;

  /*! The currently active stack configuration. */
  Os_StackConfigRefType CurrentStack;
};



/*! Pointer to Resource configuration */
typedef const   Os_ResourceConfigType OS_CONST * Os_ResourceConfigRefType;


/*! Pointer to Application configuration */
typedef const   Os_AppConfigType OS_CONST * Os_AppConfigRefType;


/*! Configuration information of an AUTOSAR core.
 * \extends Os_CoreConfigType */
struct Os_CoreAsrConfigType_Tag
{
  /*! General core attributes. */
  Os_CoreConfigType Core;

  /*! The dynamic data of the core (may be cached). */
    Os_CoreAsrType OS_VAR_NOINIT * Dyn;

  /*! Scheduler. */
  const   Os_SchedulerConfigType OS_CONST * Scheduler;

  /*! Idle task. */
  const   Os_TaskConfigType OS_CONST * IdleTask;

  /*! OS application of this core. */
  const   Os_AppConfigType OS_CONST * KernelApp;

  /*! Kernel stack. */
  const   Os_StackConfigType OS_CONST * KernelStack;

  /*! Pre-start task. */
  const   Os_TaskConfigType OS_CONST * PreStartTask;

  /*! Pre-start task callback function . */
  Os_TaskVoidTaskCbkType PreStartTaskCallback;

  /*! System StartupHook. */
  const   Os_HookCallbackConfigType OS_CONST * StartupHookRef;

  /*! System ShutdownHook. */
  const   Os_HookStatusHookConfigType OS_CONST * ShutdownHookRef;

  /*! System ErrorHook. */
  const   Os_HookStatusHookConfigType OS_CONST * ErrorHookRef;

  /*! System ProtectionHook. */
  const   Os_HookProtectionHookConfigType OS_CONST * ProtectionHookRef;

  /*! System InitHook. */
  const   Os_HookInitHookConfigType OS_CONST * InitHookRef;

  /*! List of hooks. */
  const   Os_HookConfigRefType OS_CONST * HookRefs;

  /*! Number of hooks. */
  Os_ObjIdxType HookCount;

  /*! List of applications. */
  const   Os_AppConfigRefType OS_CONST * AppRefs;

  /*! Number of applications. */
  Os_ObjIdxType AppCount;

  /*! List of resources. */
  const   Os_ResourceConfigRefType OS_CONST * ResourceRefs;

  /*! Number of resources. */
  Os_ObjIdxType ResourceCount;

  /*! HAL specific core configuration. */
  const   Os_Hal_CoreAsrConfigType OS_CONST * HwConfig;

  /*! Core memory protection configuration. */
  const   Os_MpCoreConfigType OS_CONST * MemoryProtection;

  /*! Core timing protection configuration. */
  const   Os_TpConfigType OS_CONST * TimingProtection;

  /*! List of stacks. */
  const   Os_StackConfigRefType OS_CONST * StackRefs;

  /*! Number of stacks. */
  Os_ObjIdxType StackCount;

  /*! The unique index of each autosar core in range 0 to (NumAutosarCores-1). */
  Os_CoreAsrCoreIdx CoreIdx;

  /*! Cross Core communication. */
  const   Os_XSigConfigType OS_VAR_NOINIT * XSignal;

  /*! List of core IOC configurations */
  const   Os_IocConfigRefType OS_CONST * IocRefs;

  /*! Number of core IOC configurations */
  Os_ObjIdxType IocCount;

  /*! Synchronization of AUTOSAR cores. */
  const   Os_BarrierBaseConfigType OS_CONST * Barrier;

  /*! List of configured barriers */
  const   Os_BarrierBaseConfigRefType OS_CONST * BarrierRefs;

  /*! Number of configured barriers */
  Os_ObjIdxType BarrierCount;

  /*! Holds thread data shared between all threads of this core. */
    Os_ThreadClassType OS_VAR_NOINIT * ThreadClass;

  /*! List of core ISR configurations */
  const   Os_IsrConfigRefType OS_CONST * IsrRefs;

  /*! Number of core ISR configurations */
  Os_ObjIdxType IsrCount;

};


/*! Configuration information of an OS system. */
struct Os_SystemConfigType_Tag
{
  /*! OS version information. */
  Std_VersionInfoType VersionInfo;

  /*! Number of configured AUTOSAR cores. */
  Os_CoreAsrCoreIdx NumberOfAsrCores;

  /*! List of spinlocks. */
  const   Os_SpinlockConfigRefType OS_CONST * SpinlockRefs;

  /*! Number of spinlocks. */
  Os_ObjIdxType SpinlockCount;

  /*! HAL specific system configuration. */
  const   Os_Hal_SystemConfigType OS_CONST * HwConfig;

  /*! System-wide memory protection configuration. */
  const   Os_MpSystemConfigType OS_CONST * MemoryProtection;

  /*! List of core specific interrupt source objects. */
  const   Os_InterruptSourceConfigRefType OS_CONST * InterruptSourceRefs;

  /*! Number of interrupt source objects. */
  Os_ObjIdxType InterruptSourceCount;

  /*! Hardware dependent data structure to get the current thread for a given core (NULL if not needed). */
  const   Os_Hal_Core2ThreadConfigType OS_CONST * Core2Thread;
};


/*! Defines the runtime data to synchronize the multi-core booting process. */
struct Os_CoreBootBarrierType_Tag
{
  /*! The "In" variable, which is written by master. */
  volatile Os_CoreBootBarrierContentType EchoIn;

  /*! The "Out" variable, which is written by slave. */
  volatile Os_CoreBootBarrierContentType EchoOut;

  /*! The variable used to decide, whether a core should wait or continue. */
  volatile Os_CoreBootBarrierContentType WaitingSign;
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
 *  Os_CoreIsSingleCoreOs()
 **********************************************************************************************************************/
/*! \brief          Returns whether the operating system is configured as single core system.
 *  \details        --no details--
 *
 *  \retval         !0   There is only one Core configured.
 *  \retval         0    Multiple cores are configured.
 *
 *  \context        ANY
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre            -
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StdReturnType   Os_CoreIsSingleCoreOs (
  void
);


/***********************************************************************************************************************
 *  Os_CoreIsSingleAsrCoreOs()
 **********************************************************************************************************************/
/*! \brief          Returns whether the operating system is configured as single AUTOSAR core system.
 *  \details        --no details--
 *
 *  \retval         !0   There is only one AUTOSAR Core configured.
 *  \retval         0    Multiple AUTOSAR cores are configured.
 *
 *  \context        ANY
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre            -
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StdReturnType   Os_CoreIsSingleAsrCoreOs (
  void
);


/***********************************************************************************************************************
 *  Os_CoreIsInterruptOnlyOs()
 **********************************************************************************************************************/
/*! \brief        Returns whether the Operating system is configured for the INTERRUPT_ONLY use case.
 *  \details      --no details--
 *
 *  \retval       !0    INTERRUPT_ONLY use case active.
 *  \retval       0     INTERRUPT_ONLY use case inactive.
 *
 *  \context      OS_INTERNAL
 *
 *  \reentrant    TRUE
 *  \synchronous  TRUE
 *
 *  \pre          -
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StdReturnType   Os_CoreIsInterruptOnlyOs (void);


/***********************************************************************************************************************
 *  Os_CoreAsrIsCoreLocal()
 **********************************************************************************************************************/
/*! \brief          Returns whether the given core is the local core.
 *  \details        --no details--
 *
 *  \param[in]      Core  The core to check.
 *
 *  \retval         !0   The given core is the local core.
 *  \retval         0    The given core is a remote core.
 *
 *  \context        ANY
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre            -
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StdReturnType   Os_CoreAsrIsCoreLocal (
  const   Os_CoreAsrConfigType OS_CONST *  Core
);


/***********************************************************************************************************************
 *  Os_CoreAsrCheckIsCoreLocal()
 **********************************************************************************************************************/
/*! \brief          Returns whether the given core is the local core (Not OS_CHECK_FAILED) or not (OS_CHECK_FAILED).
 *  \details        In case that extended checks are disabled, the default result of \ref Os_ErrExtendedCheck() is
 *                  returned.
 *
 *  \param[in]      Core  The core to check.
 *
 *  \retval         Not OS_CHECK_FAILED   The given core is the local core.
 *  \retval         OS_CHECK_FAILED       The given core is not the local core.
 *
 *  \context        ANY
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre            -
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StdReturnType   Os_CoreAsrCheckIsCoreLocal (
  const   Os_CoreAsrConfigType OS_CONST *  Core
);


/***********************************************************************************************************************
 *  Os_CoreId2Core()
 **********************************************************************************************************************/
/*! \brief          Returns the core belonging to the given ID.
 *  \details        --no details--
 *
 *  \param[in]      CoreId        The logical ID of the searched core. The ID must be smaller than OS_COREID_COUNT.
 *
 *  \return         The core belonging to the given ID.
 *
 *  \context        ANY
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre            -
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline const   Os_CoreConfigType OS_CONST *   Os_CoreId2Core (
  CoreIdType CoreId
);


/***********************************************************************************************************************
 *  Os_CoreGetCurrentCore()
 **********************************************************************************************************************/
/*! \brief          Returns the current core.
 *  \details        --no details--
 *
 *  \return         The core currently executing this code.
 *
 *  \context        ANY
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre            -
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline const   Os_CoreConfigType OS_CONST *   Os_CoreGetCurrentCore (
  void
);


/***********************************************************************************************************************
 *  Os_CoreGetId()
 **********************************************************************************************************************/
/*! \brief          Returns the logical identifier of the given core.
 *  \details        --no details--
 *
 *  \param[in]      Core        The core to query. Parameter must not be NULL.
 *
 *  \return         The logical identifier of the given core.
 *
 *  \context        ANY
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre            -
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline CoreIdType   Os_CoreGetId (
  const   Os_CoreConfigType OS_CONST * Core
);


/***********************************************************************************************************************
 *  Os_Core2AsrCore()
 **********************************************************************************************************************/
/*! \brief          Casts core to AUTOSAR core.
 *  \details        --no details--
 *
 *  \param[in]      Core        The core to transform. Parameter must not be NULL.
 *
 *  \return         The given core as an AUTOSAR core.
 *
 *  \context        ANY
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre            -
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline const   Os_CoreAsrConfigType OS_CONST *   Os_Core2AsrCore (
  const   Os_CoreConfigType OS_CONST * Core
);


/***********************************************************************************************************************
 *  Os_CoreAsr2Core()
 **********************************************************************************************************************/
/*! \brief          Casts AUTOSAR core to core.
 *  \details        --no details--
 *
 *  \param[in]      Core        The core to transform. Parameter must not be NULL.
 *
 *  \return         The given AUTOSAR core as a core.
 *
 *  \context        ANY
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre            -
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline const   Os_CoreConfigType OS_CONST *   Os_CoreAsr2Core (
  const   Os_CoreAsrConfigType OS_CONST * Core
);


/***********************************************************************************************************************
 *  Os_CoreAsrGetDyn()
 **********************************************************************************************************************/
/*! \brief          Returns dynamic data of an AUTOSAR core.
 *  \details        --no details--
 *
 *  \param[in]      Core        The core to query. Parameter must not be NULL.
 *
 *  \return         The pointer to the dynamic information of the core.
 *
 *  \context        ANY
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre            -
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline   Os_CoreAsrType OS_VAR_NOINIT *   Os_CoreAsrGetDyn (
  const   Os_CoreAsrConfigType OS_CONST * Core
);


/***********************************************************************************************************************
 *  Os_CoreAsrGetTimingProtection()
 **********************************************************************************************************************/
/*! \brief          Returns the timing protection configuration of an AUTOSAR core.
 *  \details        --no details--
 *
 *  \param[in]      Core        The core to query. Parameter must not be NULL.
 *
 *  \return         The pointer to the timing protection configuration of the core. The pointer is always valid,
 *                  if timing protection is enabled.
 *
 *  \context        ANY
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre            -
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline const   Os_TpConfigType OS_CONST *   Os_CoreAsrGetTimingProtection (
  const   Os_CoreAsrConfigType OS_CONST * Core
);


/***********************************************************************************************************************
 *  Os_CoreGetTrace()
 **********************************************************************************************************************/
/*! \brief          Returns a pointer to the dynamic trace information of a core.
 *  \details        --no details--
 *
 *  \param[in]      Core        The core to query. Parameter must not be NULL.
 *
 *  \return         The trace pointer of the given core. The pointer is always valid, if ORTI tracing is enabled.
 *
 *  \context        ANY
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre            -
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline
  Os_TraceCoreType OS_VAR_NOINIT *   Os_CoreGetTrace (
  const   Os_CoreConfigType OS_CONST * Core
);


/***********************************************************************************************************************
 *  Os_CoreAsrGetBarriers()
 **********************************************************************************************************************/
/*! \brief          Returns a pointer to the barrier reference list of an AUTOSAR core.
 *  \details        --no details--
 *
 *  \param[in]      Core        The core to query. Parameter must not be NULL.
 *
 *  \return         The barrier reference list pointer of the given core.
 *
 *  \context        ANY
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre            -
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline                                                                           /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
const   Os_BarrierBaseConfigRefType OS_VAR_NOINIT *   Os_CoreAsrGetBarriers (
  const   Os_CoreAsrConfigType OS_CONST * Core
);


/***********************************************************************************************************************
 *  Os_CoreIsAsrCore()
 **********************************************************************************************************************/
/*! \brief          Returns whether the given core is an AUTOSAR core (!0) or not (0).
 *  \details        --no details--
 *
 *  \param[in]      Core        The core to check. Parameter must not be NULL.
 *
 *  \retval         !0   If the given core is an AUTOSAR core.
 *  \retval         0    If the given core is not an AUTOSAR core.
 *
 *  \context        ANY
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre            -
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StdReturnType   Os_CoreIsAsrCore (
  const   Os_CoreConfigType OS_CONST * Core
);


/***********************************************************************************************************************
 *  Os_CoreIsStartedAsAsr()
 **********************************************************************************************************************/
/*! \brief          Returns whether the given core has been started as an AUTOSAR core (!0) or not (0).
 *  \details        --no details--
 *
 *  \param[in]      Core        The core to check. Parameter must not be NULL.
 *
 *  \retval         !0   If the given core has been started as an AUTOSAR core.
 *  \retval         0    If the given core has not been started as an AUTOSAR core.
 *
 *  \context        ANY
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre            -
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StdReturnType   Os_CoreIsStartedAsAsr (
  const   Os_CoreConfigType OS_CONST * Core
);


/***********************************************************************************************************************
 *  Os_CoreCheckIsAsrCore()
 **********************************************************************************************************************/
/*! \brief          Returns whether the given core is an AUTOSAR core (Not OS_CHECK_FAILED) or not (OS_CHECK_FAILED).
 *  \details        In case that extended checks are disabled, the default result of \ref Os_ErrExtendedCheck() is
 *                  returned.
 *
 *  \param[in]      Core        The core to check. Parameter must not be NULL.
 *
 *  \retval         Not OS_CHECK_FAILED   If the given core is an AUTOSAR core.
 *  \retval         OS_CHECK_FAILED       If the given core is not an AUTOSAR core.
 *
 *  \context        ANY
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre            -
 **********************************************************************************************************************/
OS_ALWAYS_INLINE 
static inline Os_StdReturnType   Os_CoreCheckIsAsrCore (
  const   Os_CoreConfigType OS_CONST * Core
);


/***********************************************************************************************************************
 *  Os_CoreCheckStartedAsAsr()
 **********************************************************************************************************************/
/*! \brief          Returns whether the given core has been started as an AUTOSAR core (Not OS_CHECK_FAILED)
 *                  or not (OS_CHECK_FAILED).
 *  \details        In case that extended checks are disabled, the default result of \ref Os_ErrExtendedCheck() is
 *                  returned.
 *
 *  \param[in]      Core        The core to check. Parameter must not be NULL.
 *
 *  \retval         Not OS_CHECK_FAILED   If the given core has been started as an AUTOSAR core.
 *  \retval         OS_CHECK_FAILED       If the given core has not been started as an AUTOSAR core.
 *
 *  \context        ANY
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre            -
 **********************************************************************************************************************/
OS_ALWAYS_INLINE 
static inline Os_StdReturnType   Os_CoreCheckStartedAsAsr (
  const   Os_CoreConfigType OS_CONST * Core
);


/***********************************************************************************************************************
 *  Os_CoreAsrGetAsrStatus()
 **********************************************************************************************************************/
/*! \brief          Returns dynamic status of an AUTOSAR core.
 *  \details        --no details--
 *
 *  \param[in]      Core        The core to query. Parameter must not be NULL.
 *
 *  \return         The pointer to the dynamic status information of the core.
 *
 *  \context        ANY
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre            -
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline   Os_CoreAsrStatusType volatile OS_VAR_NOINIT *   Os_CoreAsrGetAsrStatus (
  const   Os_CoreAsrConfigType OS_CONST * Core
);


/***********************************************************************************************************************
 *  Os_CoreOsIsStarted()
 **********************************************************************************************************************/
/*! \brief          Returns whether the OS is started on given core (!0) or not (0).
 *  \details        --no details--
 *
 *  \param[in]      Core        The core to check. Parameter must not be NULL.
 *
 *  \retval         !0  The OS is started.
 *  \retval         0   The OS is not started.
 *
 *  \context        ANY
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre            -
 **********************************************************************************************************************/
OS_ALWAYS_INLINE 
static inline Os_StdReturnType   Os_CoreOsIsStarted (
  const   Os_CoreAsrConfigType OS_CONST * Core
);


/***********************************************************************************************************************
 *  Os_CoreOsIsPrestarted()
 **********************************************************************************************************************/
/*! \brief          Returns whether the OS is in pre start phase on given core (!0) or not (0).
 *  \details        --no details--
 *
 *  \param[in]      Core        The core to check. Parameter must not be NULL.
 *
 *  \retval         !0  The OS is in pre start phase.
 *  \retval         0   The OS is not in pre start phase.
 *
 *  \context        ANY
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre            -
 **********************************************************************************************************************/
OS_ALWAYS_INLINE 
static inline Os_StdReturnType   Os_CoreOsIsPrestarted (
  const   Os_CoreAsrConfigType OS_CONST * Core
);


/***********************************************************************************************************************
 *  Os_CoreCheckId()
 **********************************************************************************************************************/
/*! \brief          Returns whether the given ID is valid (Not OS_CHECK_FAILED) or not (OS_CHECK_FAILED).
 *  \details        In case that extended checks are disabled, the default result of \ref Os_ErrExtendedCheck() is
 *                  returned.
 *
 *  \param[in]      CoreId        The ID to check.
 *
 *  \retval         Not OS_CHECK_FAILED   If the given ID is valid.
 *  \retval         OS_CHECK_FAILED       If the given ID is not valid.
 *
 *  \context        ANY
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre            -
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StdReturnType   Os_CoreCheckId (
  CoreIdType CoreId
);


/***********************************************************************************************************************
 *  Os_CoreGetKernelStack()
 **********************************************************************************************************************/
/*! \brief          Returns the kernel stack of the given core.
 *  \details        --no details--
 *
 *  \param[in]      Core          Pointer to a core. Parameter must not be NULL.
 *
 *  \return         The kernel stack belonging to the given core.
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre            -
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline const   Os_StackConfigType OS_CONST *   Os_CoreGetKernelStack (
  const   Os_CoreAsrConfigType OS_CONST * Core
);


/***********************************************************************************************************************
 *  Os_CoreGetAsrCoreIdx()
 **********************************************************************************************************************/
/*! \brief          Returns the unique autosar core index.
 *  \details        --no details--
 *
 *  \param[in]      Core          Pointer to a core. Parameter must not be NULL.
 *
 *  \return         The core index.
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre            -
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_CoreAsrCoreIdx   Os_CoreGetAsrCoreIdx (
  const   Os_CoreAsrConfigType OS_CONST * Core
);


/***********************************************************************************************************************
 *  Os_CoreGetXSig()
 **********************************************************************************************************************/
/*! \brief          Returns the XSignal ISR configuration of the core.
 *  \details        --no details--
 *
 *  \param[in]      Core      Pointer to the receiver core. Parameter must not be NULL.
 *
 *  \return         The XSignal configuration of the core or a ((void *)0) if XSIGNAL is disabled.
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre            -
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline const   Os_XSigConfigType OS_CONST *   Os_CoreGetXSig (
  const   Os_CoreAsrConfigType OS_CONST * Core
);


/***********************************************************************************************************************
 *  Os_CoreGetHookInfo()
 **********************************************************************************************************************/
/*! \brief          Returns the hook information object of the given core.
 *  \details        --no details--
 *
 *  \param[in]      Core    The core of interest. Parameter must not be NULL.
 *
 *  \return         Hook information object.
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre            -
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline   Os_HookInfoType OS_VAR_NOINIT *   Os_CoreGetHookInfo (
  const   Os_CoreAsrConfigType OS_CONST * Core
);


/***********************************************************************************************************************
 *  Os_CoreGetThreadClass()
 **********************************************************************************************************************/
/*! \brief          Returns the thread class object of the caller core.
 *  \details        --no details---
 *
 *  \param[in]      Core    The core of interest. Parameter must not be NULL.
 *
 *  \return         The thread class object
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre            The object referred by the returned pointer contains only valid data if FPU context is enabled.
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline   Os_ThreadClassType OS_VAR_NOINIT *   Os_CoreGetThreadClass (
  const   Os_CoreAsrConfigType OS_CONST * Core
);


/***********************************************************************************************************************
 *  Os_CoreGetStartupHook()
 **********************************************************************************************************************/
/*! \brief          Returns the Startup Hook of the core.
 *  \details        --no details--
 *
 *  \param[in]      Core    The core to query. Parameter must not be NULL.
 *
 *  \return         Startup Hook of the core.
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre            -
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline const   Os_HookCallbackConfigType OS_CONST *   Os_CoreGetStartupHook (
  const   Os_CoreAsrConfigType OS_CONST * Core
);


/***********************************************************************************************************************
 *  Os_CoreGetShutdownHook()
 **********************************************************************************************************************/
/*! \brief          Returns the Shutdown Hook of the core.
 *  \details        --no details--
 *
 *  \param[in]      Core    The core to query. Parameter must not be NULL.
 *
 *  \return         Shutdown Hook of the core.
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre            -
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline const   Os_HookStatusHookConfigType OS_CONST *   Os_CoreGetShutdownHook (
  const   Os_CoreAsrConfigType OS_CONST * Core
);


/***********************************************************************************************************************
 *  Os_CoreGetProtectionHook()
 **********************************************************************************************************************/
/*! \brief          Returns the Protection Hook of the core.
 *  \details        --no details--
 *
 *  \param[in]      Core    The core to query. Parameter must not be NULL.
 *
 *  \return         Protection Hook of the core.
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre            -
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline const   Os_HookProtectionHookConfigType OS_CONST *   Os_CoreGetProtectionHook (
  const   Os_CoreAsrConfigType OS_CONST * Core
);


/***********************************************************************************************************************
 *  Os_CoreGetErrorHook()
 **********************************************************************************************************************/
/*! \brief          Returns the Error Hook of the core.
 *  \details        --no details--
 *
 *  \param[in]      Core    The core to query. Parameter must not be NULL.
 *
 *  \return         Error Hook of the core.
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre            -
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline const   Os_HookStatusHookConfigType OS_CONST *   Os_CoreGetErrorHook (
  const   Os_CoreAsrConfigType OS_CONST * Core
);


/***********************************************************************************************************************
 *  Os_CoreGetIdleTask()
 **********************************************************************************************************************/
/*! \brief          Returns the idle task of the core.
 *  \details        --no details--
 *
 *  \param[in]      Core    The core to query. Parameter must not be NULL.
 *
 *  \return         Idle task of the core.
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre            -
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline const   Os_TaskConfigType OS_CONST *   Os_CoreGetIdleTask (
  const   Os_CoreAsrConfigType OS_CONST * Core
);


/***********************************************************************************************************************
 *  Os_CoreGetKernelApp()
 **********************************************************************************************************************/
/*! \brief          Returns the kernel application of the given core.
 *  \details        --no details--
 *
 *  \param[in]      Core    The core to query. Parameter must not be NULL.
 *
 *  \return         Kernel application of the core.
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre            -
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline const   Os_AppConfigType OS_CONST *   Os_CoreGetKernelApp (
  const   Os_CoreAsrConfigType OS_CONST * Core
);


/***********************************************************************************************************************
 *  Os_CoreGetScheduler()
 **********************************************************************************************************************/
/*! \brief          Returns the scheduler of the core.
 *  \details        --no details--
 *
 *  \param[in]      Core    The core to query. Parameter must not be NULL.
 *
 *  \return         Scheduler of the core.
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre            -
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline const   Os_SchedulerConfigType OS_CONST *   Os_CoreGetScheduler (
  const   Os_CoreAsrConfigType OS_CONST * Core
);


/***********************************************************************************************************************
 *  Os_CoreGetLastError()
 **********************************************************************************************************************/
/*! \brief          Returns pointer to detailed information about the last occurred error.
 *  \details        --no details--
 *
 *  \param[in]      Core    The core to query. Parameter must not be NULL.
 *
 *  \return         Error information of the core.
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre            -
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline   Os_ErrorType OS_VAR_NOINIT *   Os_CoreGetLastError (
  const   Os_CoreAsrConfigType OS_CONST * Core
);


/***********************************************************************************************************************
 *  Os_CoreInterruptedThreadsPush()
 **********************************************************************************************************************/
/*! \brief          Push the given thread on the interrupted thread stack.
 *  \details        --no details--
 *
 *  \param[in,out]  Core              The affected core. Parameter must not be NULL.
 *  \param[in]      InterruptedThread The thread which shall be pushed. Parameter must not be NULL.
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre            Concurrent access to given object is prevented by caller.
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline void   Os_CoreInterruptedThreadsPush (
  const   Os_CoreAsrConfigType OS_CONST * Core,
  const   Os_ThreadConfigType OS_CONST * InterruptedThread
);


/***********************************************************************************************************************
 *  Os_CoreInterruptedThreadsPop()
 **********************************************************************************************************************/
/*! \brief          Pop the given thread from the interrupted thread stack.
 *  \details        --no details--
 *
 *  \param[in,out]  Core              The affected core. Parameter must not be NULL.
 *
 *  \return         Pointer to interrupted thread.
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre            Concurrent access to given object is prevented by caller.
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline const   Os_ThreadConfigType OS_CONST *   Os_CoreInterruptedThreadsPop (
  const   Os_CoreAsrConfigType OS_CONST * Core
);


/***********************************************************************************************************************
 *  Os_CoreThreadGetByContext()
 **********************************************************************************************************************/
/*! \brief          Returns the first thread with any of the given contexts.
 *  \details        Checks the list of interrupted threads and the current thread, whether it belongs to any of the
 *                  given contexts. If there are multiple matches, the thread with the highest priority is returned.
 *
 *  \param[in]      Core              The core query. Parameter must not be NULL.
 *  \param[in]      Context           The context to search for.
 *
 *  \return         A thread with any of the given contexts or ((void *)0).
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre            Concurrent access to given object is prevented by caller.
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline const   Os_ThreadConfigType OS_CONST *   Os_CoreThreadGetByContext (
  const   Os_CoreAsrConfigType OS_CONST * Core,
  Os_CallContextType Context
);


/***********************************************************************************************************************
 *  Os_CoreInterruptedThreadsGetTopIdx()
 **********************************************************************************************************************/
/*! \brief          Returns the index of the thread that was interrupted last.
 *  \details        --no details--
 *
 *  \param[in,out]  Core              The affected core. Parameter must not be NULL.
 *
 *  \return         Thread that was interrupted last.
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre            Concurrent access to given object is prevented by caller.
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_CoreThreadIdx   Os_CoreInterruptedThreadsGetTopIdx (
  const   Os_CoreAsrConfigType OS_CONST * Core
);


/***********************************************************************************************************************
 *  Os_CoreInterruptedThreadsGet()
 **********************************************************************************************************************/
/*! \brief          Returns the thread at the given index.
 *  \details        --no details--
 *
 *  \param[in,out]  Core    The affected core. Parameter must not be NULL.
 *  \param[in]      Idx     Index of interest. Idx must be in range [1 .. Core->Dyn->InterruptedThreadsCounter].
 *
 *  \return         Thread that was interrupted last.
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre            Concurrent access to given object is prevented by caller.
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline const   Os_ThreadConfigType OS_CONST *   Os_CoreInterruptedThreadsGet (
  const   Os_CoreAsrConfigType OS_CONST * Core,
  Os_CoreThreadIdx Idx
);


/***********************************************************************************************************************
 *  Os_CoreInterruptedThreadsGetBottomIdx()
 **********************************************************************************************************************/
/*! \brief          Returns the index of the first thread in the list of interrupted threads.
 *  \details        --no details--
 *
 *  \return         Thread that was interrupted first.
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre            Concurrent access to given object is prevented by caller.
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_CoreThreadIdx   Os_CoreInterruptedThreadsGetBottomIdx (void);


/***********************************************************************************************************************
 *  Os_CoreInterruptedThreadsMoveDown()
 **********************************************************************************************************************/
/*! \brief          Moves the given iterator one position downwards.
 *  \details        --no details--
 *
 *  \param[in]      Iterator  The iterator which shall be moved downwards.
 *                            The iterator must be greater than 0.
 *
 *  \return         New iterator.
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre            Concurrent access to given object is prevented by caller.
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_CoreThreadIdx   Os_CoreInterruptedThreadsMoveDown (
  Os_CoreThreadIdx Iterator
);


/***********************************************************************************************************************
 *  Os_CoreShutdownAllLocal()
 **********************************************************************************************************************/
/*! \brief          Perform core shutdown as part of a \ref ShutdownAllCores() call.
 *  \details        Shuts down the current core and waits at the shutdown synchronization barrier before calling the
 *                  global shutdown hook.
 *
 *  \param[in]      Error         Status code to report to the shutdown hooks.
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre            Supervisor mode.
 *
 *  \trace          CREQ-95, CREQ-161
 **********************************************************************************************************************/
OS_NORETURN void   Os_CoreShutdownAllLocal (
  StatusType Error
);


/***********************************************************************************************************************
 *  Os_CoreSetThread()
 **********************************************************************************************************************/
/*! \brief        Set the current thread pointer on the local core.
 *  \details        --no details--
 *
 *  \param[in]    Thread  Pointer to the current thread of the local core. Parameter must not be NULL.
 *
 *  \context      OS_INTERNAL
 *
 *  \reentrant    TRUE
 *  \synchronous  TRUE
 *
 *  \pre          Supervisor mode
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline void   Os_CoreSetThread (
  const   Os_ThreadConfigType OS_CONST * Thread
);


/***********************************************************************************************************************
 *  Os_CoreGetThread()
 **********************************************************************************************************************/
/*! \brief        Returns the current thread pointer of the local core.
 *  \details        --no details--
 *
 *  \return       Pointer to the current thread of the local core.
 *
 *  \context      OS_INTERNAL
 *
 *  \reentrant    TRUE
 *  \synchronous  TRUE
 *
 *  \pre          Current thread pointer has been set before.
 *
 *  Internal comment removed.
 *
 *
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline const   Os_ThreadConfigType OS_CONST *   Os_CoreGetThread (
  void
);


/***********************************************************************************************************************
 *  Os_CoreSetCurrentStack()
 **********************************************************************************************************************/
/*! \brief        Set the current stack configuration pointer on the local core.
 *  \details      This function is used for the feature StackMonitoring.
 *
 *  \param[in]    Stack  Pointer to the current stack configuration of the local core. Parameter must not be NULL.
 *  \param[in]    Core   Pointer to the local AUTOSAR core configuration. Parameter must not be NULL.
 *
 *  \context      OS_INTERNAL
 *
 *  \reentrant    TRUE
 *  \synchronous  TRUE
 *
 *  \pre          Supervisor mode
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline void   Os_CoreSetCurrentStack (
  const   Os_StackConfigType OS_CONST * Stack,
  const   Os_CoreAsrConfigType OS_CONST * Core
);


/***********************************************************************************************************************
 *  Os_CoreGetCurrentStack()
 **********************************************************************************************************************/
/*! \brief        Returns the current stack configuration pointer of the local core.
 *  \details        --no details--
 *
 *  \return       Pointer to the current stack configuration of the local core.
 *
 *  \context      OS_INTERNAL
 *
 *  \reentrant    TRUE
 *  \synchronous  TRUE
 *
 *  \pre          Current stack configuration pointer has been set before.
 *
 *  Internal comment removed.
 *
 *
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline const   Os_StackConfigType OS_CONST *   Os_CoreGetCurrentStack (
  void
);


/***********************************************************************************************************************
 *  Os_SystemShutdown()
 **********************************************************************************************************************/
/*! \brief          System internal shutdown of all cores.
 *  \details        This function does not perform access right checks.
 *
 *  \param[in]      Error    Status code to report to the shutdown hooks.
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre            Supervisor mode.
 **********************************************************************************************************************/
OS_NORETURN void   Os_SystemShutdown (
  StatusType Error
);


/***********************************************************************************************************************
 *  Os_SystemGet()
 **********************************************************************************************************************/
/*! \brief          Returns the system configuration.
 *  \details        --no details--
 *
 *  \return         The system configuration object.
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre            -
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline const   Os_SystemConfigType OS_CONST *   Os_SystemGet (
 void
);


/***********************************************************************************************************************
 *  Os_SystemGetCore2Thread()
 **********************************************************************************************************************/
/*! \brief          Returns the current thread information for all cores.
 *  \details        --no details--
 *
 *  \return         The current thread information for all cores. Parameter must not be NULL.
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre            -
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline const   Os_Hal_Core2ThreadConfigType OS_CONST *   Os_SystemGetCore2Thread (
  const   Os_SystemConfigType OS_CONST * System
);


/***********************************************************************************************************************
 *  Os_UnhandledIrq()
 **********************************************************************************************************************/
/*! \brief        Kernel behavior for unhandled interrupts.
 *  \details      This function never returns.
 *
 *  \param[in]    InterruptSource               The ID of the unhandled interrupt source.
 *  \param[in]    ExceptionInstructionAddress   The address of the instruction where the unhandled interrupt occurred.
 *  \param[in]    ExceptionCauserPrivileged     Defines whether the system was in privileged mode during occurence of
 *                                              the interrupt:
 *                                                - TRUE:  Privileged mode was active.
 *                                                - FALSE: Non-Privileged mode was active.
 *
 *  \context      OS_INTERNAL
 *
 *  \reentrant    TRUE
 *  \synchronous  TRUE
 *
 *  \pre          Supervisor mode.
 *
 *  \trace        CREQ-24
 **********************************************************************************************************************/
OS_NORETURN void   Os_UnhandledIrq (
  Os_InterruptSourceIdType InterruptSource,
  Os_AddressOfConstType ExceptionInstructionAddress,
  boolean ExceptionCauserPrivileged
);


/***********************************************************************************************************************
 *  Os_UnhandledExc()
 **********************************************************************************************************************/
/*! \brief        Kernel behavior for unhandled exceptions.
 *  \details      This function never returns.
 *
 *  \param[in]    ExceptionSource               The ID of the unhandled exception source.
 *  \param[in]    ExceptionInstructionAddress   The address of the instruction where the unhandled exception occurred.
 *  \param[in]    ExceptionCauserPrivileged     Defines whether the system was in privileged mode during occurence of
 *                                              the exception:
 *                                                - TRUE:  Privileged mode was active.
 *                                                - FALSE: Non-Privileged mode was active.
 *
 *  \context      OS_INTERNAL
 *
 *  \reentrant    TRUE
 *  \synchronous  TRUE
 *
 *  \pre          Supervisor mode.
 **********************************************************************************************************************/
void     Os_UnhandledExc
(
  Os_ExceptionSourceIdType ExceptionSource,
  Os_AddressOfConstType ExceptionInstructionAddress,
  boolean ExceptionCauserPrivileged
);


/***********************************************************************************************************************
 *  Os_UnhandledSysCall()
 **********************************************************************************************************************/
/*! \brief        Kernel behavior for unhandled system calls.
 *  \details      This function never returns.
 *
 *  \param[in]    ExceptionInstructionAddress   The address of the instruction where the unhandled system call occurred.
 *  \param[in]    ExceptionCauserPrivileged     Defines whether the system was in privileged mode during occurence of
 *                                              the system call:
 *                                                - TRUE:  Privileged mode was active.
 *                                                - FALSE: Non-Privileged mode was active.
 *
 *  \context      OS_INTERNAL
 *
 *  \reentrant    TRUE
 *  \synchronous  TRUE
 *
 *  \pre          Supervisor mode.
 **********************************************************************************************************************/
OS_NORETURN void   Os_UnhandledSysCall (
  Os_AddressOfConstType ExceptionInstructionAddress,
  boolean ExceptionCauserPrivileged
);


/***********************************************************************************************************************
 *  Os_CoreIsExceptionContextReadingSupported()
 **********************************************************************************************************************/
/*! \brief          Returns whether the HAL supports exception context reading ( !0 ) or not ( 0 ).
 *  \details        --no details--
 *
 *  \retval         !0      If the HAL supports exception context reading.
 *  \retval         0       If the HAL does not support exception context reading.
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre            -
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StdReturnType   Os_CoreIsExceptionContextReadingSupported (
  void
);


/***********************************************************************************************************************
 *  Os_CoreIsExceptionContextModificationSupported()
 **********************************************************************************************************************/
/*! \brief          Returns whether the HAL supports exception context modification ( !0 ) or not ( 0 ).
 *  \details        --no details--
 *
 *  \retval         !0      If the HAL supports exception context modification.
 *  \retval         0       If the HAL does not support exception context modification.
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre            -
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StdReturnType   Os_CoreIsExceptionContextModificationSupported (
  void
);


/***********************************************************************************************************************
 *  Os_CoreControlIdle()
 **********************************************************************************************************************/
/*! \brief        Core local implementation ControlIdle().
 *  \details        --no details--
 *
 *  \param[in]    Core     Selects the core which idle mode is set. Parameter must not be NULL.
 *  \param[in]    IdleMode The mode which shall be performed during idle time.
 *                         The value must be smaller than OS_IDLEMODE_COUNT.
 *
 *  \retval       OS_STATUS_OK            No error.
 *
 *  \context      OS_INTERNAL
 *
 *  \reentrant    TRUE
 *  \synchronous  TRUE
 *
 *  \pre          The given core is the local core
 **********************************************************************************************************************/
Os_StatusType     Os_CoreControlIdle
(
  const   Os_CoreAsrConfigType OS_CONST * Core,
  IdleModeType IdleMode
);


/***********************************************************************************************************************
 *  Os_CoreInitialEnableInterruptSources()
 **********************************************************************************************************************/
/*! \brief          Initially initialize the Isrs of the given core.
 *  \details        Simply a loop to call the respective Isr function for a specific Isr.
 *
 *  \param[in]      Core          The affected core. Parameter must not be NULL.
 *  \param[in]      ClearPending  Defines whether the pending flag shall be cleared (TRUE) or not (FALSE).
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline void   Os_CoreInitialEnableInterruptSources (
  const   Os_CoreAsrConfigType OS_CONST * Core,
  boolean ClearPending
);


/***********************************************************************************************************************
 *  Os_Api_GetVersionInfo()
 **********************************************************************************************************************/
/*! \brief        AUTOSAR Get Version Information API.
 *  \details      For further details see Os_GetVersionInfo().
 *
 *  \param[out]   versioninfo      See Os_GetVersionInfo().
 *
 *  \context      ANY
 *
 *  \reentrant    TRUE
 *  \synchronous  TRUE
 *
 *  \pre          See Os_GetVersionInfo().
 *  \trace        CREQ-144484
 **********************************************************************************************************************/
void     Os_Api_GetVersionInfo
(
  Std_VersionInfoType *versioninfo
);


/***********************************************************************************************************************
 *  Os_Api_GetExceptionContext()
 **********************************************************************************************************************/
/*! \brief        Getter function for the exception context.
 *  \details      For further details see Os_GetExceptionContext().
 *
 *  \param[out]   Context      See Os_GetExceptionContext().
 *
 *  \context      ANY
 *
 *  \reentrant    TRUE
 *  \synchronous  TRUE
 *
 *  \pre          See Os_GetExceptionContext().
 *
 *  \trace        CREQ-129664
 **********************************************************************************************************************/
Os_StatusType     Os_Api_GetExceptionContext
(
  Os_ExceptionContextRefType Context
);


/***********************************************************************************************************************
 *  Os_Api_SetExceptionContext()
 **********************************************************************************************************************/
/*! \brief        Setter function for the exception context.
 *  \details      For further details see Os_SetExceptionContext().
 *
 *  \param[in]    Context      See Os_SetExceptionContext().
 *
 *  \context      ANY
 *
 *  \reentrant    TRUE
 *  \synchronous  TRUE
 *
 *  \pre          See Os_GetExceptionContext().
 *
 *  \trace        CREQ-129663
 **********************************************************************************************************************/
Os_StatusType     Os_Api_SetExceptionContext
(
  const   Os_ExceptionContextType OS_CONST * Context
);


/***********************************************************************************************************************
 *  Os_Api_GetExceptionAddress()
 **********************************************************************************************************************/
/*! \brief        Getter function for the latest exception address.
 *  \details      For further details see Os_GetExceptionAddress().
 *
 *  \context      ANY
 *
 *  \reentrant    TRUE
 *  \synchronous  TRUE
 *
 *  \pre          See Os_GetExceptionAddress().
 *
 *  \trace        CREQ-247263
 **********************************************************************************************************************/
Os_AddressOfConstType     Os_Api_GetExceptionAddress
(
  void
);


/***********************************************************************************************************************
 *  Os_Api_InitMemory()
 **********************************************************************************************************************/
/*! \brief        Proprietary initialization function to optimize startup code.
 *  \details      For further details see Os_InitMemory().
 *
 *  \context      -
 *
 *  \reentrant    FALSE
 *  \synchronous  TRUE
 *
 *  \pre          See Os_InitMemory().
 **********************************************************************************************************************/
void     Os_Api_InitMemory
(
  void
);


/***********************************************************************************************************************
 *  Os_Api_Init()
 **********************************************************************************************************************/
/*! \brief        AUTOSAR Initialization Interface (Init-Step2).
 *  \details      For further details see Os_Init().
 *
 *                See (\ref ApiUsageBeforeStartOsAnchor) for more details.
 *
 *  \context      -
 *
 *  \reentrant    FALSE
 *  \synchronous  TRUE
 *
 *  \pre          See Os_Init().
 *
 *  \trace        CREQ-162, CREQ-1200, CREQ-115996
 *  \trace        SPEC-63985, SPEC-64064, SPEC-63755, SPEC-63720
 **********************************************************************************************************************/
void     Os_Api_Init
(
  void
);


/***********************************************************************************************************************
 *  Os_Api_InitInterruptOnly()
 **********************************************************************************************************************/
/*! \brief        Initialization Interface for INTERRUPT_ONLY use case.
 *  \details      For further details see Os_InitInterruptOnly().
 *
 *  \context      -
 *
 *  \reentrant    FALSE
 *  \synchronous  TRUE
 *
 *  \pre          See Os_InitInterruptOnly().
 **********************************************************************************************************************/
void     Os_Api_InitInterruptOnly
(
  void
);


/***********************************************************************************************************************
 *  Os_Api_EnterPreStartTask()
 **********************************************************************************************************************/
/*! \brief        OS service Os_EnterPreStartTask().
 *  \details      For further details see Os_EnterPreStartTask().
 *
 *  \context      -
 *
 *  \reentrant    FALSE
 *  \synchronous  TRUE
 *
 *  \pre          See Os_EnterPreStartTask().
 *
 *  \trace        CREQ-105587, CREQ-115372, CREQ-115373
 **********************************************************************************************************************/
void     Os_Api_EnterPreStartTask
(
  void
);


/***********************************************************************************************************************
 *  Os_Api_StartCore()
 **********************************************************************************************************************/
/*! \brief        OS service StartCore().
 *  \details      For further details see StartCore().
 *
 *  \param[in]    CoreID  See StartCore().
 *  \param[out]   Status  See StartCore().
 *
 *  \return       See StartCore().
 *
 *  \context      -
 *
 *  \reentrant    FALSE
 *  \synchronous  TRUE
 *
 *  \pre          See StartCore().
 *
 *  \trace        CREQ-37, CREQ-40
 *  \trace        SPEC-63631, SPEC-63679, SPEC-63627, SPEC-63596, SPEC-63652, SPEC-63628
 *  \trace        SPEC-64041, SPEC-63742, SPEC-63850, SPEC-63755, SPEC-64064, SPEC-63753
 **********************************************************************************************************************/
void     Os_Api_StartCore
(
  CoreIdType CoreID,
  StatusType *Status
);


/***********************************************************************************************************************
 *  Os_Api_StartNonAutosarCore()
 **********************************************************************************************************************/
/*! \brief        OS service StartNonAutosarCore().
 *  \details      For further details see StartNonAutosarCore().
 *
 *  \param[in]    CoreID  See StartNonAutosarCore().
 *  \param[out]   Status  See StartNonAutosarCore().
 *
 *  \return       See StartNonAutosarCore().
 *
 *  \context      INIT
 *
 *  \reentrant    FALSE
 *  \synchronous  TRUE
 *
 *  \pre          See StartNonAutosarCore().
 *
 *  \trace        CREQ-120, CREQ-40
 *  \trace        SPEC-63650, SPEC-63653, SPEC-63612, SPEC-63599, SPEC-63769, SPEC-64064, SPEC-63753, SPEC-63755
 *  \trace        SPEC-63850, SPEC-64041
 **********************************************************************************************************************/
void     Os_Api_StartNonAutosarCore
(
  CoreIdType CoreID,
  StatusType *Status
);


/***********************************************************************************************************************
 *  Os_Api_GetCoreID()
 **********************************************************************************************************************/
/*! \brief        OS service GetCoreID().
 *  \details      For further details see GetCoreID().
 *
 *  \return       See GetCoreID().
 *
 *  \context      ANY
 *
 *  \reentrant    TRUE
 *  \synchronous  TRUE
 *
 *  \pre          See GetCoreID().
 *
 *  \trace        CREQ-149
 *  \trace        SPEC-63664, SPEC-63616, SPEC-63770
 **********************************************************************************************************************/
CoreIdType     Os_Api_GetCoreID
(
  void
);


/***********************************************************************************************************************
 *  Os_Api_GetNumberOfActivatedCores()
 **********************************************************************************************************************/
/*! \brief        OS service GetNumberOfActivatedCores().
 *  \details      For further details see GetNumberOfActivatedCores().
 *
 *  \return       See GetNumberOfActivatedCores().
 *
 *  \context      TASK|ISR2
 *
 *  \reentrant    TRUE
 *  \synchronous  TRUE
 *
 *  \pre          See GetNumberOfActivatedCores().
 *
 *  \trace        CREQ-148
 *  \trace        SPEC-63675, SPEC-63589, SPEC-63967
 **********************************************************************************************************************/
uint32     Os_Api_GetNumberOfActivatedCores
(
  void
);


/***********************************************************************************************************************
 *  Os_Api_GetActiveApplicationMode()
 **********************************************************************************************************************/
/*! \brief        OS service GetActiveApplicationMode().
 *  \details      For further details see GetActiveApplicationMode().
 *
 *  \return       See GetActiveApplicationMode().
 *
 *  \context      TASK|ISR2|ERRHOOK|PRETHOOK|POSTTHOOK|STARTHOOK|SHUTHOOK
 *
 *  \reentrant    TRUE
 *  \synchronous  TRUE
 *
 *  \pre          See GetActiveApplicationMode().
 *
 *  \trace        CREQ-117, CREQ-118
 *  \trace        SPEC-67569, SPEC-67570
 **********************************************************************************************************************/
AppModeType     Os_Api_GetActiveApplicationMode
(
  void
);


/***********************************************************************************************************************
 *  Os_Api_StartOS()
 **********************************************************************************************************************/
/*! \brief        OS service StartOS().
 *  \details      For further details see StartOS().
 *
 *  \param[in]    Mode    See StartOS().
 *
 *  \context      INIT
 *
 *  \reentrant    FALSE
 *  \synchronous  TRUE
 *
 *  \pre          See StartOS().
 *
 *  \trace        CREQ-63, CREQ-45, CREQ-51, CREQ-72, CREQ-146, CREQ-162, CREQ-299, CREQ-48
 *  \trace        CREQ-118
 *  \trace        SPEC-67565, SPEC-67568, SPEC-67587, SPEC-67588
 *  \trace        SPEC-63607, SPEC-63688, SPEC-63644, SPEC-67566, SPEC-64027
 *  \trace        SPEC-64047, SPEC-63860, SPEC-63998, SPEC-63837, SPEC-63828, SPEC-64012, SPEC-64066, SPEC-63753
 *  \trace        SPEC-63742, SPEC-63970, SPEC-63699
 *  \trace        SPEC-63811, SPEC-63987, SPEC-63725
 *
 *  Internal comment removed.
 *
 *
 *
 **********************************************************************************************************************/
void     Os_Api_StartOS
(
  AppModeType Mode
);


/***********************************************************************************************************************
 *  Os_CoreInitHook()
 **********************************************************************************************************************/
/*! \brief        The init hook which performs the core initialization.
 *  \details      --no details--
 *
 *  \param[in]    Mode    The application mode in which the core shall start.
 *
 *  \context      OS_INTERNAL
 *
 *  \reentrant    FALSE
 *  \synchronous  TRUE
 *
 *  \pre          -
 **********************************************************************************************************************/
void     Os_CoreInitHook
(
  AppModeType Mode
);


/***********************************************************************************************************************
 *  Os_Api_ShutdownOS()
 **********************************************************************************************************************/
/*! \brief        OS service ShutdownOS().
 *  \details      For further details see ShutdownOS().
 *
 *  \param[in]    Error   See ShutdownOS().
 *
 *  \context      TASK|ISR2|ERRHOOK|STARTHOOK
 *
 *  \reentrant    TRUE
 *  \synchronous  TRUE
 *
 *  \pre          See ShutdownOS().
 *
 *  \trace        CREQ-153, CREQ-161, CREQ-76, CREQ-40
 *  \trace        SPEC-63849, SPEC-63971, SPEC-63862, SPEC-63825, SPEC-63946, SPEC-63787, SPEC-63861, SPEC-63803
 *  \trace        SPEC-63940, SPEC-63747, SPEC-63975
 *  \trace        SPEC-67571, SPEC-67572, SPEC-67573, SPEC-67574
 *  \trace        SPEC-67589, SPEC-67590, SPEC-63859
 **********************************************************************************************************************/
void     Os_Api_ShutdownOS
(
  StatusType Error
);


/***********************************************************************************************************************
 *  Os_Api_ShutdownAllCores()
 **********************************************************************************************************************/
/*! \brief        OS service ShutdownAllCores().
 *  \details      For further details see ShutdownAllCores().
 *
 *  \param[in]    Error    See ShutdownAllCores().
 *
 *  \context      TASK|ISR2|ERRHOOK|STARTHOOK
 *
 *  \reentrant    TRUE
 *  \synchronous  TRUE
 *
 *  \pre          See ShutdownAllCores().
 *
 *  \trace        CREQ-95, CREQ-161, CREQ-76
 *  \trace        SPEC-64058, SPEC-63803, SPEC-63757, SPEC-64013, SPEC-64058, SPEC-63940, SPEC-63747, SPEC-63975
 *  \trace        SPEC-63610, SPEC-63656, SPEC-63668, SPEC-63594, SPEC-63859
 **********************************************************************************************************************/
void     Os_Api_ShutdownAllCores
(
  StatusType Error
);


/***********************************************************************************************************************
 *  Os_Api_ControlIdle()
 **********************************************************************************************************************/
/*! \brief        OS service ControlIdle().
 *  \details      For further details see ControlIdle().
 *
 *  \param[in]    CoreID    See ControlIdle().
 *  \param[in]    IdleMode  See ControlIdle().
 *
 *  \retval       OS_STATUS_OK            No error.
 *  \retval       OS_STATUS_ID_1          (EXTENDED status): Invalid core.
 *  \retval       OS_STATUS_ID_2          (EXTENDED status): Invalid invalid IdleMode.
 *  \retval       OS_STATUS_CALLEVEL      (EXTENDED status:) Called from invalid context.
 *  \retval       OS_STATUS_DISABLEDINT   (Service Protection:) Caller is in interrupt API sequence.
 *  \retval       OS_STATUS_CORE          (EXTENDED status): Called for non AUTOSAR core.
 *
 *  \context      TASK|ISR2
 *
 *  \reentrant    TRUE
 *  \synchronous  TRUE
 *
 *  \pre          See ControlIdle().
 *
 *  \trace        CREQ-168, CREQ-40
 *  \trace        SPEC-63572, SPEC-63569, SPEC-63570, SPEC-2458876
 *
 *  Internal comment removed.
 *
 *
 **********************************************************************************************************************/
Os_StatusType     Os_Api_ControlIdle
(
  CoreIdType CoreID,
  IdleModeType IdleMode
);


/***********************************************************************************************************************
 *  Os_Api_GetUnhandledIrq()
 **********************************************************************************************************************/
/*! \brief        Proprietary OS service Os_GetUnhandledIrq().
 *  \details      For further details see Os_GetUnhandledIrq().
 *
 *  \param[out]   InterruptSource   See Os_GetUnhandledIrq().
 *
 *  \return       See Os_GetUnhandledIrq().
 *
 *  \context      ANY
 *
 *  \reentrant    TRUE
 *  \synchronous  TRUE
 *
 *  \pre          See Os_GetUnhandledIrq().
 *
 *  \trace        CREQ-78
 **********************************************************************************************************************/
Os_StatusType     Os_Api_GetUnhandledIrq
(
  Os_InterruptSourceIdRefType InterruptSource
);


/***********************************************************************************************************************
 *  Os_Api_GetUnhandledExc()
 **********************************************************************************************************************/
/*! \brief        Proprietary OS service Os_GetUnhandledExc().
 *  \details      For further details see Os_GetUnhandledExc().
 *
 *  \param[out]   ExceptionSource   Exception source number.
 *
 *  \return       See Os_GetUnhandledExc().
 *
 *  \context      ANY
 *
 *  \reentrant    TRUE
 *  \synchronous  TRUE
 *
 *  \pre          See Os_GetUnhandledExc().
 *
 *  \trace        CREQ-78
 *  \trace        SPEC-63884
 **********************************************************************************************************************/
Os_StatusType     Os_Api_GetUnhandledExc
(
  Os_ExceptionSourceIdRefType ExceptionSource
);


/***********************************************************************************************************************
 *  Os_Api_GetCoreStartState()
 **********************************************************************************************************************/
/*! \brief        OS service Os_GetCoreStartState().
 *  \details      For further details see Os_GetCoreStartState().
 *
 *  \return       See Os_GetCoreStartState().
 *
 *  \context      ANY
 *
 *  \reentrant    TRUE
 *  \synchronous  TRUE
 *
 *  \pre          See Os_GetCoreStartState().
 *
 *  \trace        CREQ-213720
 **********************************************************************************************************************/
void     Os_Api_GetCoreStartState
(
  CoreIdType CoreID,
  Os_CoreStartStateType *CoreState,
  StatusType *Status
);


/***********************************************************************************************************************
 *  Os_Task_Os_IdleTask()
 **********************************************************************************************************************/
/*! \brief          The idle task which loops forever.
 *  \details        --no details--
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre            Called via task switch (not directly called from source code).
 **********************************************************************************************************************/
TASK(Os_IdleTask);


/***********************************************************************************************************************
 *  Os_CorePreStartTaskWrapper()
 **********************************************************************************************************************/
/*! \brief        Entry function for Pre-Start Tasks.
 *  \details      Wrapper function for each Pre-Start Task which performs Pre-Start initialization before calling the
 *                user function.
 *
 *  \context      OS_INTERNAL
 *
 *  \reentrant    TRUE
 *  \synchronous  TRUE
 *
 *  \pre            Never called from static source code (Callback routine).
 **********************************************************************************************************************/
OS_NORETURN void   Os_CorePreStartTaskWrapper (
  void
);


/***********************************************************************************************************************
 *  Os_CoreCriticalUserSectionEnter()
 **********************************************************************************************************************/
/*! \brief        Enter the critical user section.
 *  \details      OS enters the critical user section. Nesting is allowed.
 *
 *  \param[in]    CoreAsr  The core to query. Parameter must not be NULL.
 *
 *  \context      OS_INTERNAL
 *
 *  \reentrant    TRUE
 *  \synchronous  TRUE
 *
 *  \pre          -
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline void   Os_CoreCriticalUserSectionEnter (
  const   Os_CoreAsrConfigType OS_CONST * CoreAsr
);


/***********************************************************************************************************************
 *  Os_CoreCriticalUserSectionLeave()
 **********************************************************************************************************************/
/*! \brief        Leave the critical user section.
 *  \details      OS leaves the critical user section. Nesting is allowed.
 *
 *  \param[in]    CoreAsr  The core to query. Parameter must not be NULL.
 *
 *  \context      OS_INTERNAL
 *
 *  \reentrant    TRUE
 *  \synchronous  TRUE
 *
 *  \pre          Os_CoreCriticalUserSectionEnter() must be called before.
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline void   Os_CoreCriticalUserSectionLeave (
  const   Os_CoreAsrConfigType OS_CONST * CoreAsr
);


/***********************************************************************************************************************
 *  Os_CoreCriticalUserSectionIsActive()
 **********************************************************************************************************************/
/*! \brief        Return the core status of critical user section.
 *  \details      Return the value of core status CriticalUserSection.
 *
 *  \param[in]    CoreAsr  The core to query. Parameter must not be NULL.
 *
 *  \context      ANY
 *
 *  \reentrant    TRUE
 *  \synchronous  TRUE
 *
 *  \pre          -
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline boolean   Os_CoreCriticalUserSectionIsActive (
  const   Os_CoreAsrConfigType OS_CONST * CoreAsr
);


# define OS_STOP_SEC_CODE
# include "Os_MemMap_OsCode.h"                                                                                          /* PRQA S 5087 */ /* MD_MSR_MemMap */


#endif /* OS_COREINT_H */

/* module specific MISRA deviations:
 */

/*!
 * \}
 */

/***********************************************************************************************************************
 *  END OF FILE: Os_CoreInt.h
 **********************************************************************************************************************/
