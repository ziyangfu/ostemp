/***********************************************************************************************************************
 *  COPYRIGHT
 *  --------------------------------------------------------------------------------------------------------------------
 *  \verbatim
 *  Copyright (c) 2019 by Vector Informatik GmbH.                                              All rights reserved.
 *
 *                This software is copyright protected and proprietary to Vector Informatik GmbH.
 *                Vector Informatik GmbH grants to you only those rights as set out in the license conditions.
 *                All other rights remain with Vector Informatik GmbH.
 *  \endverbatim
 *  --------------------------------------------------------------------------------------------------------------------
 *  FILE DESCRIPTION
 *  ------------------------------------------------------------------------------------------------------------------*/
/**
 *  \ingroup    Os_Hal
 *  \defgroup   Os_Hal_Interrupt  HAL Interrupt
 *  \brief      Primitives for interrupt state manipulation
 *  \details
 *  This component provides the following interrupt primitives:
 *  - Enter/leave critical sections
 *  - Disable interrupt sources
 *
 *  ### Interrupt Priority Levels ###
 *  The kernel distinguishes between the following logical groups of interrupt priority levels:
 *
 *  Interrupt Priority Level    | Description
 *  :-------------------------- | :-------------------------------------------------------------------------------------
 *  Timing Protection Interrupt | The timing protection interrupt is handled on this level.
 *  Category 1 ISRs             | AUTOSAR category 1 ISRs are handled on this level.
 *  Category 2 ISRs             | AUTOSAR category 2 ISRs are handled on this level.
 *
 *  The kernel needs some primitive interrupt functions, to enter/leave critical sections on these interrupt priority
 *  levels.  Os_Hal_IntSuspend() and Os_Hal_IntResume() work either up to timing protection level or up to category 1
 *  level, depending whether timing protection is level or NMI based.
 *
 *  ![Interrupt Priorities and Relevant HAL Functions](InterruptPriorities.png)
 *
 *
 *  ### Implementation Hint ###
 *  During implementation of this module you may have to work with the following hardware properties:
 *
 *  - Core interrupt flag
 *  - Level register in interrupt controller
 *  - Timing protection interrupt flag
 *  \{
 *
 *  \file
 *
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
 **********************************************************************************************************************/
/***********************************************************************************************************************
 *  REVISION HISTORY
 *  --------------------------------------------------------------------------------------------------------------------
 *  Refer to Os_Hal_Os.h.
 **********************************************************************************************************************/

#ifndef OS_HAL_INTERRUPTINT_H
# define OS_HAL_INTERRUPTINT_H

/***********************************************************************************************************************
 *  INCLUDES
 **********************************************************************************************************************/
/* AUTOSAR includes */
# include "Std_Types.h"

/* Os kernel module dependencies */
# include "Os_Hal_Interrupt_Types.h"

/* Os module declarations */
# include "Os_CommonInt.h"

/* Os HAL dependencies */
# include "Os_Hal_CompilerInt.h"
# include "Os_Hal_CoreInt.h"
# include "Os_Hal_DerivativeInt.h"

# if defined(OS_HAL_INTERRUPT_AURIXTC2XX_IR)
#  include "Os_Hal_InterruptController_AurixTC2xx_IRInt.h"
# elif defined(OS_HAL_INTERRUPT_AURIXTC3XX_IR)                                                                          /* COV_OS_HAL_INTERRUPTCONTROLLER_BETA */
#  include "Os_Hal_InterruptController_AurixTC3xx_IRInt.h"
# elif defined(OS_HAL_INTERRUPT_AURIXTC4XX_IR)                                                                          /* COV_OS_HAL_INTERRUPTCONTROLLER_BETA */
#  include "Os_Hal_InterruptController_AurixTC4xx_IRInt.h"
# else
#  error "The selected core module is not supported!"
# endif

/***********************************************************************************************************************
 *  GLOBAL CONSTANT MACROS
 **********************************************************************************************************************/


/***********************************************************************************************************************
 *  GLOBAL FUNCTION MACROS
 **********************************************************************************************************************/


/***********************************************************************************************************************
 *  GLOBAL DATA TYPES AND STRUCTURES
 **********************************************************************************************************************/


/*! \brief    Hardware specific configuration for category 2 ISRs.
 *  \details  It typically contains the interrupt priority level and the interrupt source of the ISR.
 *            The kernel uses this data type to:
 *            - Disable an interrupt source as part of killing applications.
 *            - Implement interrupt resources.
 */
struct Os_Hal_IntIsrConfigType_Tag
{
  /*! Interrupt level */
  Os_Hal_IntLevelType Level;

  /*! Interrupt source register offset */
  Os_Hal_IntSourceType Source;

  /*! The core allocation of the ISR. */
  uint8 CoreAssignment;
};


/*! \brief Hardware specific mapping configuration of ISRs. */
struct Os_Hal_IntIsrMapConfigType_Tag
{
  uint32 dummy;
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
 *  GLOBAL FUNCTIONS
 **********************************************************************************************************************/

/***********************************************************************************************************************
 *  Os_Hal_IntEnableGlobal()
 **********************************************************************************************************************/
/*! \brief          Enables global core interrupts.
 *
 *  \context        ANY
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *  \pre            Supervisor mode
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline void   Os_Hal_IntEnableGlobal (void);


/***********************************************************************************************************************
 *  Os_Hal_IntDisableGlobal()
 **********************************************************************************************************************/
/*! \brief          Disables global core interrupts.
 *
 *  \context        ANY
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *  \pre            Supervisor mode
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline void   Os_Hal_IntDisableGlobal (void);


/***********************************************************************************************************************
 *  Os_Hal_IntDisableAndReturnGlobalState()
 **********************************************************************************************************************/
/*! \brief          Disables global core interrupts and returns earlier global state of interrupts.
 *
 *  \return         Current state of global interrupts.
 *
 *  \context        ANY
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *  \pre            Supervisor mode
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_Hal_IntGlobalStateType   Os_Hal_IntDisableAndReturnGlobalState (void);


/***********************************************************************************************************************
 *  Os_Hal_IntSetGlobalState()
 **********************************************************************************************************************/
/*! \brief          Set the global interrupt state to the given value.
 *  \details        This function is used to enable and disable interrupt handling globally.
 *
 *  \param[in]      State   In: Desired interrupt global flag.
 *                          Parameter must not be NULL.
 *  \context        ANY
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *  \pre            Supervisor mode
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline void   Os_Hal_IntSetGlobalState (
  const   Os_Hal_IntGlobalStateType OS_APPL_DATA * State
);


/***********************************************************************************************************************
 *  Os_Hal_IntGetLevel()
 **********************************************************************************************************************/
/*! \brief          Returns the current interrupt priority level.
 *  \details        -
 *
 *
 *  \param[out]     Level   Current interrupt priority level. Parameter must not be NULL.
 *
 *  \context        ANY
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre            Supervisor mode
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline void   Os_Hal_IntGetLevel (
    Os_Hal_IntLevelType OS_APPL_DATA * Level
);


/***********************************************************************************************************************
 *  Os_Hal_IntSetLevel()
 **********************************************************************************************************************/
/*! \brief          Set the interrupt priority level to the given value.
 *  \details        -
 *
 *  \param[in]      Level   Desired interrupt priority level.
 *                          Parameter must not be NULL.
 *  \context        ANY
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *  \pre            Supervisor mode
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline void   Os_Hal_IntSetLevel (
  const   Os_Hal_IntLevelType OS_APPL_DATA * Level
);


/***********************************************************************************************************************
 *  Os_Hal_IntGetGlobalEnable()
 **********************************************************************************************************************/
/*! \brief          Returns the global flag value which is used to enable all interrupts.
 *  \details        -
 *
 *  \param[out]     State   Global flag value to enable all interrupts.
 *                          Parameter must not be NULL.
 *  \context        ANY
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *  \pre            Supervisor mode
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline void   Os_Hal_IntGetGlobalEnable (
    Os_Hal_IntGlobalStateType OS_APPL_DATA * State
);


/***********************************************************************************************************************
 *  Os_Hal_IntGetCat2LockLevel()
 **********************************************************************************************************************/
/*! \brief          Returns the interrupt priority level which is used to lock all category 2 ISRs.
 *  \details        -
 *
 *  \param[out]     Level   Category 2 Lock Level. Parameter must not be NULL.
 *
 *  \context        ANY
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre            Supervisor mode
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline void   Os_Hal_IntGetCat2LockLevel (
    Os_Hal_IntLevelType OS_APPL_DATA * Level
);


/***********************************************************************************************************************
 *  Os_Hal_IntGetCat1LockLevel()
 **********************************************************************************************************************/
/*! \brief          Returns the interrupt priority level which is used to lock all category 1 ISRs.
 *  \details        -
 *
 *
 *  \param[out]     Level   Category 1 Lock Level. Parameter must not be NULL.
 *
 *  \context        ANY
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre            Supervisor mode
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline void   Os_Hal_IntGetCat1LockLevel (
    Os_Hal_IntLevelType OS_APPL_DATA * Level
);


/***********************************************************************************************************************
 *  Os_Hal_IntGetTPLockLevel()
 **********************************************************************************************************************/
/*! \brief          Returns the interrupt priority level which is used to lock the TP ISR.
 *  \details        If there is no timing protection interrupt it returns the level
 *                  which is used to lock all category 1 ISRs.
 *
 *  \param[out]     Level   TP Lock Level. Parameter must not be NULL.
 *
 *  \context        ANY
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre            Supervisor mode
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline void   Os_Hal_IntGetTPLockLevel (
    Os_Hal_IntLevelType OS_APPL_DATA * Level
);


/***********************************************************************************************************************
 *  Os_Hal_IntGetTaskLevel()
 **********************************************************************************************************************/
/*! \brief          Returns the interrupt level which is used to enable interrupt handling on task level.
 *  \details        -
 *
 *  \param[out]     Level   Task level to enable interrupt handling. Parameter must not be NULL.
 *
 *  \context        ANY
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre            Supervisor mode.
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline void   Os_Hal_IntGetTaskLevel (
    Os_Hal_IntLevelType OS_APPL_DATA * Level
);


/***********************************************************************************************************************
 *  Os_Hal_IntGetInterruptState()
 **********************************************************************************************************************/
/*! \brief          Returns the current interrupt state.
 *  \details        This function is used to transfer the current interrupt state between different context
 *                  (e.g. non-trusted functions).
 *
 *  \param[out]     InterruptState   Location where to store the current interrupt state. Parameter must not be NULL.
 *
 *  \context        ANY
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre            Supervisor mode
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline void   Os_Hal_IntGetInterruptState (
    Os_Hal_IntStateType OS_VAR_NOINIT * InterruptState
);


/***********************************************************************************************************************
 *  Os_Hal_IntDisableSource()
 **********************************************************************************************************************/
/*! \brief          Disable interrupt delivery for the given interrupt source.
 *  \details        -
 *
 *  \param[in]      IsrConfig   Interrupt configuration of the ISR which shall be disabled.
 *                              Parameter must not be NULL.
 *  \context        ANY
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *  \pre            Supervisor mode
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline void   Os_Hal_IntDisableSource (
  const   Os_Hal_IntIsrConfigType OS_APPL_DATA * IsrConfig
);


/***********************************************************************************************************************
 *  Os_Hal_IntDisableSourceMapped()
 **********************************************************************************************************************/
/*! \brief          Disable interrupt delivery for the given mapped interrupt source.
 *  \details        -
 *
 *  \param[in]      IsrConfig     Interrupt configuration of the ISR which shall be disabled.
 *                                Parameter must not be NULL.
 *  \param[in]      IsrMapConfig  Associated mapped interrupt configuration of the ISR.
 *                                Parameter must not be NULL.
 *
 *  \context        ANY
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *  \pre            Supervisor mode
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline void   Os_Hal_IntDisableSourceMapped (
  const   Os_Hal_IntIsrConfigType OS_APPL_DATA * IsrConfig,
  const   Os_Hal_IntIsrMapConfigType OS_APPL_DATA * IsrMapConfig
);


/***********************************************************************************************************************
 *  Os_Hal_IntEnableSource()
 **********************************************************************************************************************/
/*! \brief          Enable interrupt delivery for the given interrupt source.
 *  \details        -
 *
 *  \param[in]      IsrConfig   Interrupt configuration of the ISR which shall be enabled.
 *                              Parameter must not be NULL.
 *  \context        ANY
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *  \pre            Supervisor mode
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline void   Os_Hal_IntEnableSource (
  const   Os_Hal_IntIsrConfigType OS_APPL_DATA * IsrConfig
);


/***********************************************************************************************************************
 *  Os_Hal_IntEnableSourceMapped()
 **********************************************************************************************************************/
/*! \brief          Enable interrupt delivery for the given mapped interrupt source.
 *  \details        -
 *
 *  \param[in]      IsrConfig     Interrupt configuration of the ISR which shall be enabled.
 *                                Parameter must not be NULL.
 *  \param[in]      IsrMapConfig  Associated mapped interrupt configuration of the ISR.
 *                                Parameter must not be NULL.
 *
 *  \context        ANY
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *  \pre            Supervisor mode
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline void   Os_Hal_IntEnableSourceMapped (
  const   Os_Hal_IntIsrConfigType OS_APPL_DATA * IsrConfig,
  const   Os_Hal_IntIsrMapConfigType OS_APPL_DATA * IsrMapConfig
);


/***********************************************************************************************************************
 *  Os_Hal_IntIsClearPendingSupported()
 **********************************************************************************************************************/
/*! \brief        Returns whether the hardware supports clearing of a pending interrupt.
 *  \details      --no details--
 *
 *  \retval       !0    Pending interrupts can be cleared.
 *  \retval       0     Pending interrupts cannot be cleared.
 *
 *  \context      OS_INTERNAL
 *
 *  \reentrant    TRUE
 *  \synchronous  TRUE
 *
 *  \pre          -
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StdReturnType   Os_Hal_IntIsClearPendingSupported ( void );


/***********************************************************************************************************************
 *  Os_Hal_IntClearPending()
 **********************************************************************************************************************/
/*! \brief          Clears the pending flag of the given interrupt source.
 *  \details        --no details--
 *
 *  \param[in]      IsrConfig   Interrupt configuration of the ISR which shall be cleared.
 *                              Parameter must not be NULL.
 *  \context        ANY
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *  \pre            Supervisor mode
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline void   Os_Hal_IntClearPending (
  const   Os_Hal_IntIsrConfigType OS_APPL_DATA * IsrConfig
);


/***********************************************************************************************************************
 *  Os_Hal_IntClearPendingMapped()
 **********************************************************************************************************************/
/*! \brief          Clears the pending flag of the given mapped interrupt source.
 *  \details        --no details--
 *
 *  \param[in]      IsrConfig     Interrupt configuration of the ISR which shall be cleared.
 *                                Parameter must not be NULL.
 *  \param[in]      IsrMapConfig  Associated mapped interrupt configuration of the ISR.
 *                                Parameter must not be NULL.
 *
 *  \context        ANY
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *  \pre            Supervisor mode
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline void   Os_Hal_IntClearPendingMapped (
  const   Os_Hal_IntIsrConfigType OS_APPL_DATA * IsrConfig,
  const   Os_Hal_IntIsrMapConfigType OS_APPL_DATA * IsrMapConfig
);

/***********************************************************************************************************************
 *  Os_Hal_IntIsInterruptSourceEnabled()
 **********************************************************************************************************************/
/*! \brief          Checks if the interrupt source is enabled
 *  \details        --no details--
 *
 *  \param[in]      IsrConfig   Interrupt configuration of the ISR which shall be checked.
 *                              Parameter must not be NULL.
 *  \retval         0           not enabled
 *  \retval         !0          enabled
 *  \context        ANY
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *  \pre            -
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StdReturnType   Os_Hal_IntIsInterruptSourceEnabled (
  const   Os_Hal_IntIsrConfigType OS_APPL_DATA * IsrConfig
);


/***********************************************************************************************************************
 *  Os_Hal_IntIsInterruptSourceEnabledMapped()
 **********************************************************************************************************************/
/*! \brief          Checks if the interrupt source is enabled
 *  \details        --no details--
 *
 *  \param[in]      IsrConfig     Interrupt configuration of the ISR which shall be checked.
 *                                Parameter must not be NULL.
 *  \param[in]      IsrMapConfig  Associated mapped interrupt configuration of the ISR.
 *                                Parameter must not be NULL.
 *
 *  \retval         0           not enabled
 *  \retval         !0          enabled
 *  \context        ANY
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *  \pre            -
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StdReturnType   Os_Hal_IntIsInterruptSourceEnabledMapped (
  const   Os_Hal_IntIsrConfigType OS_APPL_DATA * IsrConfig,
  const   Os_Hal_IntIsrMapConfigType OS_APPL_DATA * IsrMapConfig
);


/***********************************************************************************************************************
 *  Os_Hal_IntIsInterruptPending()
 **********************************************************************************************************************/
/*! \brief          Checks if the specified ISR has a pending request
 *  \details        --no details--
 *
 *  \param[in]      IsrConfig   Interrupt configuration of the ISR which shall be cleared.
 *                              Parameter must not be NULL.
 *  \retval         0           no pending request.
 *  \retval         !0          pending request.
 *  \context        ANY
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *  \pre            -
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StdReturnType   Os_Hal_IntIsInterruptPending (
  const   Os_Hal_IntIsrConfigType OS_APPL_DATA * IsrConfig
);


/***********************************************************************************************************************
 *  Os_Hal_IntIsInterruptPendingMapped()
 **********************************************************************************************************************/
/*! \brief          Checks if the specified ISR has a pending request
 *  \details        --no details--
 *
 *  \param[in]      IsrConfig     Interrupt configuration of the ISR which shall be cleared.
 *                                Parameter must not be NULL.
 *  \param[in]      IsrMapConfig  Associated mapped interrupt configuration of the ISR.
 *                                Parameter must not be NULL.
 *
 *  \retval         0           no pending request.
 *  \retval         !0          pending request.
 *  \context        ANY
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *  \pre            -
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StdReturnType   Os_Hal_IntIsInterruptPendingMapped (
  const   Os_Hal_IntIsrConfigType OS_APPL_DATA * IsrConfig,
  const   Os_Hal_IntIsrMapConfigType OS_APPL_DATA * IsrMapConfig
);


/***********************************************************************************************************************
 *  Os_Hal_IntEndOfInterrupt()
 **********************************************************************************************************************/
/*! \brief          Signal hardware that interrupt processing has been completed.
 *  \details        -
 *
 *  \param[in]      IsrConfig  Interrupt configuration of the ISR which shall signal an end of interrupt.
 *                             Parameter must not be NULL.
 *  \context        ANY
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *  \pre            Supervisor mode
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline void   Os_Hal_IntEndOfInterrupt (
  const   Os_Hal_IntIsrConfigType OS_APPL_DATA * IsrConfig
);


/***********************************************************************************************************************
 *  Os_Hal_IntEndOfInterruptMapped()
 **********************************************************************************************************************/
/*! \brief          Signal hardware that interrupt processing has been completed.
 *  \details        -
 *
 *  \param[in]      IsrConfig     Interrupt configuration of the ISR which shall signal an end of interrupt.
 *                                Parameter must not be NULL.
 *  \param[in]      IsrMapConfig  Associated mapped interrupt configuration of the ISR.
 *                                Parameter must not be NULL.
 *
 *  \context        ANY
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *  \pre            Supervisor mode
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline void   Os_Hal_IntEndOfInterruptMapped (
  const   Os_Hal_IntIsrConfigType OS_APPL_DATA * IsrConfig,
  const   Os_Hal_IntIsrMapConfigType OS_APPL_DATA * IsrMapConfig
);


/***********************************************************************************************************************
 *  Os_Hal_IntIsrLevelIsLe()
 **********************************************************************************************************************/
/*! \brief          Returns whether the given ISR configuration has an interrupt priority level, which is lower than
 *                  or equal to the given interrupt priority level.
 *  \details        -
 *
 *  \param[in]      IsrConfig   The ISR configuration which shall be checked. Parameter must not be NULL.
 *  \param[in]      Level       The interrupt priority level to compare with. Parameter must not be NULL.
 *
 *   \retval        !0  The configured interrupt priority level of Isr is logically lower than or equal to Level.
 *  \retval         0   The configured interrupt priority level of Isr is logically greater than Level.
 *
 *  \context        ANY
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *  \pre            Supervisor mode
 *  \note           Candidate for compiler attributes OS_NOSIDEEFFECTS.
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StdReturnType   Os_Hal_IntIsrLevelIsLe (
  const   Os_Hal_IntIsrConfigType OS_APPL_DATA * IsrConfig,
  const   Os_Hal_IntLevelType OS_CONST * Level
);


/***********************************************************************************************************************
 *  Os_Hal_IntLevelIsGt()
 **********************************************************************************************************************/
/*! \brief          Returns whether given interrupt priority level LevelX is logically greater than LevelY.
 *  \details        -
 *
 *
 *  \param[in]      LevelX   Left interrupt priority level. Parameter must not be NULL.
 *  \param[in]      LevelY   Right interrupt priority level. Parameter must not be NULL.
 *
 *  \retval         !0  LevelX is logically greater than LevelY.
 *  \retval         0   LevelX is logically less than or equal to LevelY.
 *
 *  \context        ANY
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre            -
 *  \note           Candidate for compiler attributes OS_NOSIDEEFFECTS.
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StdReturnType   Os_Hal_IntLevelIsGt (
  const   Os_Hal_IntLevelType OS_CONST * LevelX,
  const   Os_Hal_IntLevelType OS_CONST * LevelY
);


/***********************************************************************************************************************
 *  Os_Hal_IntTrapRestore()
 **********************************************************************************************************************/
/*! \brief          Restore interrupt state which has been saved during trap.
 *  \details        This function shall restore the interrupt state which was active before the trap handler has been
 *                  triggered. The interrupt state given as parameter has been stored in \ref Os_Exc_SysCall().
 *
 *  \param[in]      InterruptState   Previous interrupt state to restore. Parameter must not be NULL.
 *
 *  \context        ANY
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre            Supervisor mode.
 *  \pre            Called after trap.
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline void   Os_Hal_IntTrapRestore (
  const   Os_Hal_IntTrapStateType OS_VAR_NOINIT * InterruptState
);


/***********************************************************************************************************************
 *  Os_Hal_IntTrapUpdate()
 **********************************************************************************************************************/
/*! \brief          Copies the current interrupt state to the interrupt state which has been saved during trap.
 *  \details        This function is called by the trap handler. It allows modification of the interrupt state of the
 *                  trap caller. The interrupt state given as parameter has been stored in \ref Os_Exc_SysCall().
 *
 *  \param[out]     InterruptState   User space interrupt information. Parameter must not be NULL.
 *
 *  \context        ANY
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre            Supervisor mode.
 *  \pre            Called after trap.
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline void   Os_Hal_IntTrapUpdate (
    Os_Hal_IntTrapStateType OS_VAR_NOINIT * InterruptState
);


/***********************************************************************************************************************
 *  Os_Hal_IntInterruptSourceInit()
 **********************************************************************************************************************/
/*! \brief          Initialize given interrupt source so interrupt handling is possible.
 *  \details
 *  Initialization includes:
 *    - Binding of interrupt sources to cores.
 *    - Setting the interrupt priority of each configured ISR.
 *
 *  The enabled-state interrupt sources shall not be touched. (This might not be possible on some interrupt
 *  controllers.)
 *
 *  \param[in,out]  InterruptSource   Interrupt source which shall be initialized. Parameter must not be NULL.
 *
 *  \context        ANY
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre            Supervisor mode
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline void   Os_Hal_IntInterruptSourceInit (
  const   Os_Hal_IntIsrConfigType OS_APPL_DATA * InterruptSource
);


/***********************************************************************************************************************
 *  Os_Hal_IntInterruptSourceInitMapped()
 **********************************************************************************************************************/
/*! \brief          Initialize given mapped interrupt source so interrupt handling is possible.
 *  \details
 *  Initialization includes:
 *    - Binding of interrupt sources to cores.
 *    - Setting the interrupt priority of each configured ISR.
 *
 *  The enabled-state interrupt sources shall not be touched. (This might not be possible on some interrupt
 *  controllers.)
 *
 *  \param[in,out]  InterruptSource   Interrupt source which shall be initialized. Parameter must not be NULL.
 *  \param[in,out]  IsrMapConfig      Associated mapped interrupt configuration of the ISR. Parameter must not be NULL.
 *
 *  \context        ANY
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre            Supervisor mode
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline void   Os_Hal_IntInterruptSourceInitMapped (
  const   Os_Hal_IntIsrConfigType OS_APPL_DATA * InterruptSource,
  const   Os_Hal_IntIsrMapConfigType OS_APPL_DATA * IsrMapConfig
);


/***********************************************************************************************************************
 *  Os_Hal_IntIsGlobal2LevelSwitchSupported()
 **********************************************************************************************************************/
/*! \brief        Returns whether the platform supports the global disable to level lock switch (!0) or not (0).
 *  \details      If the platform does not have global disable flag then the feature cannot be supported.
 *                Feature is supported if platform allows to disable global, switch priority level and resume global
 *                interrupt state and a pending interrupt with lower priority does not occur after this sequence.
 *
 *  \retval       !0    Supported.
 *  \retval       0     Not supported.
 *
 *  \context      OS_INTERNAL
 *
 *  \reentrant    TRUE
 *  \synchronous  TRUE
 *
 *  \pre          -
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StdReturnType   Os_Hal_IntIsGlobal2LevelSwitchSupported (void);


/***********************************************************************************************************************
 *  Os_Hal_IntIsPeripheralInterruptTriggerSupported()
 **********************************************************************************************************************/
/*! \brief        Returns whether the hardware supports peripheral interrupt software trigger.
 *  \details      --no details--
 *
 *  \retval       !0    Peripheral interrupts can be triggered.
 *  \retval       0     Peripheral interrupts cannot be triggered.
 *
 *  \context      OS_INTERNAL
 *
 *  \reentrant    TRUE
 *  \synchronous  TRUE
 *
 *  \pre          -
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StdReturnType   Os_Hal_IntIsPeripheralInterruptTriggerSupported ( void );


/***********************************************************************************************************************
 *  Os_Hal_IntIsLevelSupported()
 **********************************************************************************************************************/
/*! \brief        Returns whether the hardware supports interrupt priority levels.
 *  \details      --no details--
 *
 *  \retval       !0    The interrupt controller supports level switching.
 *  \retval       0     The interrupt controller does not support level switching.
 *
 *  \context      OS_INTERNAL
 *
 *  \reentrant    TRUE
 *  \synchronous  TRUE
 *
 *  \pre          -
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StdReturnType   Os_Hal_IntIsLevelSupported ( void );


/***********************************************************************************************************************
 *  Os_Hal_IntGetGlobalState()
 **********************************************************************************************************************/
/*! \brief          Returns current core interrupt global state.
 *  \details        -
 *
 *  \param[out]     State   Current interrupt global flag.
 *                          Parameter must not be NULL.
 *  \context        ANY
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *  \pre            Supervisor mode
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline void   Os_Hal_IntGetGlobalState (
    Os_Hal_IntGlobalStateType OS_APPL_DATA * State
);


/***********************************************************************************************************************
 *  Os_Hal_IntIsInterruptMappingSupported()
 **********************************************************************************************************************/
/*! \brief        Returns whether the hardware supports interrupt mapping.
 *  \details      --no details--
 *
 *  \retval       !0    Interrupt mapping is supported.
 *  \retval       0     Interrupt mapping is not supported.
 *
 *  \context      OS_INTERNAL
 *
 *  \reentrant    TRUE
 *  \synchronous  TRUE
 *
 *  \pre          -
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StdReturnType   Os_Hal_IntIsInterruptMappingSupported ( void );


/***********************************************************************************************************************
 *  Os_Hal_IntIsInterruptPostActionSupported()
 **********************************************************************************************************************/
/*! \brief        Returns whether interrupt post action handling is supported.
 *  \details      See \ref Os_Hal_IntPostAction and \ref Os_Hal_IntPostActionMapped.
 *
 *  \retval       !0    Interrupt post action handling is supported.
 *  \retval       0     Interrupt post action handling is not supported.
 *
 *  \context      OS_INTERNAL
 *
 *  \reentrant    TRUE
 *  \synchronous  TRUE
 *
 *  \pre          -
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StdReturnType   Os_Hal_IntIsInterruptPostActionSupported ( void );


/***********************************************************************************************************************
 *  Os_Hal_IntPostAction()
 **********************************************************************************************************************/
/*! \brief          Execute interrupt specific post action.
 *  \details        This function is called before \ref Os_Hal_IntEndOfInterrupt().
 *
 *  \param[in]      IsrConfig  Interrupt configuration of the ISR for which the post action needs to be executed.
 *                             Parameter must not be NULL.
 *  \context        ANY
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *  \pre            Supervisor mode
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline void   Os_Hal_IntPostAction (
  const   Os_Hal_IntIsrConfigType OS_APPL_DATA * IsrConfig
);


/***********************************************************************************************************************
 *  Os_Hal_IntPostActionMapped()
 **********************************************************************************************************************/
/*! \brief          Execute interrupt specific post action.
 *  \details        This function is called before \ref Os_Hal_IntEndOfInterruptMapped().
 *
 *  \param[in]      IsrConfig     Interrupt configuration of the ISR for which the post action needs to be executed.
 *                                Parameter must not be NULL.
 *  \param[in]      IsrMapConfig  Associated mapped interrupt configuration of the ISR.
 *                                Parameter must not be NULL.
 *
 *  \context        ANY
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *  \pre            Supervisor mode
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline void   Os_Hal_IntPostActionMapped (
  const   Os_Hal_IntIsrConfigType OS_APPL_DATA * IsrConfig,
  const   Os_Hal_IntIsrMapConfigType OS_APPL_DATA * IsrMapConfig
);


# define OS_STOP_SEC_CODE
# include "Os_MemMap_OsCode.h"                                                                                          /* PRQA S 5087 */ /* MD_MSR_MemMap */


#endif /* OS_HAL_INTERRUPTINT_H */

/* module specific MISRA deviations:
 */

/*!
 * \}
 */

/**********************************************************************************************************************
 *  END OF FILE: Os_Hal_InterruptInt.h
 *********************************************************************************************************************/

