/***********************************************************************************************************************
 *  COPYRIGHT
 *  --------------------------------------------------------------------------------------------------------------------
 *  \verbatim
 *  Copyright (c) 2017 by Vector Informatik GmbH.                                              All rights reserved.
 *
 *                This software is copyright protected and proprietary to Vector Informatik GmbH.
 *                Vector Informatik GmbH grants to you only those rights as set out in the license conditions.
 *                All other rights remain with Vector Informatik GmbH.
 *  \endverbatim
 *  --------------------------------------------------------------------------------------------------------------------
 *  FILE DESCRIPTION
 *  ------------------------------------------------------------------------------------------------------------------*/
/**
 * \addtogroup  Os_Hal_Core
 * \{
 *
 * \file    Os_Hal_Core.h
 * \brief   Core and multi-core related primitives.
 *
 **********************************************************************************************************************/
/***********************************************************************************************************************
 *  REVISION HISTORY
 *  --------------------------------------------------------------------------------------------------------------------
 *  Refer to Os_Hal_Os.h.
 **********************************************************************************************************************/
                                                                                                                        /* PRQA S 0388  EOF */ /* MD_MSR_Dir1.1 */


#ifndef OS_HAL_CORE_H
# define OS_HAL_CORE_H

/***********************************************************************************************************************
 *  INCLUDES
 **********************************************************************************************************************/
/* AUTOSAR includes */
# include "Std_Types.h"

/* Os module declarations */
# include "Os_Hal_CoreInt.h"
# include "Os_Hal_Core_Lcfg.h"

/* Os kernel module declarations */
# include "Os_Common.h"
# include "Os_Thread.h"

/* Os HAL dependencies */
# include "Os_Hal_Interrupt.h"
# include "Os_Hal_Compiler.h"
# include "Os_Hal_Derivative.h"


# if defined (OS_HAL_CORE_161_EP)
#  include "Os_Hal_Core_161_EP.h"
# elif defined (OS_HAL_CORE_162_P)                                                                                      /* COV_OS_HAL_CORE_BETA */
#  include "Os_Hal_Core_162_P.h"
# elif defined (OS_HAL_CORE_18)
#  include "Os_Hal_Core_18.h"                                                                                           /* COV_OS_HAL_CORE_BETA */
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
 *  Os_Hal_CoreIsThreadPointerUserReadable()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StdReturnType   /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
Os_Hal_CoreIsThreadPointerUserReadable (
  void
)
{
  return !0u;                                                                                                           /* PRQA S 4404, 4558 */ /* MD_Os_C90BooleanCompatibility, MD_Os_Rule10.1_4558 */
}

/***********************************************************************************************************************
 *  Os_Hal_CoreIsCoreIdUserReadable()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StdReturnType   /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
Os_Hal_CoreIsCoreIdUserReadable (
  void
)
{
  return !0u;                                                                                                           /* PRQA S 4404, 4558 */ /* MD_Os_C90BooleanCompatibility, MD_Os_Rule10.1_4558 */
}


/***********************************************************************************************************************
 *  Os_Hal_CoreSetThread()
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
OS_ALWAYS_INLINE                                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
static inline void   Os_Hal_CoreSetThread (
    const   Os_Hal_Core2ThreadConfigType OS_CONST * Core2Thread,
    const   Os_ThreadConfigType OS_CONST * Thread
)
{
  /*!
   * Internal comment removed.
 *
 *
 *
   */

  Os_Hal_IntGlobalStateType previousState;

  /* #10 Store the current interrupt global state and disable all interrupts. */
  Os_Hal_IntGetGlobalState(&previousState);                                                                             /* SBSW_OS_HAL_FC_LOCAL_VAR */

  /* #20 Disable all interrupts. */
  Os_Hal_Disable();

  /* #30 Set the GW bit in the PSW register. */
  Os_Hal_Mtcr(OS_HAL_PSW_OFFSET, Os_Hal_Mfcr(OS_HAL_PSW_OFFSET) | OS_HAL_PSW_GW_MASK);
  Os_Hal_Isync();

  /* #40 Set the thread pointer to A8. */
  Os_Hal_Mta8((uint32)Thread);                                                                                          /* PRQA S 0306 */ /* MD_Os_Hal_Rule11.4_0306 */

  /* #50 Restore all interrupts to the previous state. */
  Os_Hal_IntSetGlobalState(&previousState);                                                                             /* SBSW_OS_HAL_FC_LOCAL_VAR */

  OS_IGNORE_UNREF_PARAM(Core2Thread);                                                                                   /* PRQA S 3112 */ /* MD_MSR_DummyStmt */
}


/***********************************************************************************************************************
 *  Os_Hal_CoreGetThread()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
const   Os_ThreadConfigType OS_CONST *   Os_Hal_CoreGetThread (
    const   Os_Hal_Core2ThreadConfigType OS_CONST * Core2Thread
)
{
  uint32 ptr;

  OS_IGNORE_UNREF_PARAM(Core2Thread);                                                                                   /* PRQA S 3112 */ /* MD_MSR_DummyStmt */

  ptr = Os_Hal_Mfa8();
  return (const   Os_ThreadConfigType OS_CONST *)ptr;                                                        /* PRQA S 0306 */ /* MD_Os_Hal_Rule11.4_0306 */
}


/***********************************************************************************************************************
 *  Os_Hal_CoreIsPrivilegedModeUserReadable()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StdReturnType   /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
Os_Hal_CoreIsPrivilegedModeUserReadable (
  void
)
{
  /*!
   * Internal comment removed.
 *
 *
   */
  return !0u;                                                                                                           /* PRQA S 4404, 4558 */ /* MD_Os_C90BooleanCompatibility, MD_Os_Rule10.1_4558 */
}


/***********************************************************************************************************************
 *  Os_Hal_CoreGetPrivilegedMode()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_StdReturnType   /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
Os_Hal_CoreGetPrivilegedMode (
  void
)
{
  return ((Os_StdReturnType)(                                                                                           /* PRQA S 4304 */ /* MD_Os_C90BooleanCompatibility */
                              (((uint32)Os_Hal_Mfcr(OS_HAL_PSW_OFFSET)) & OS_HAL_PSW_IO_MASK) ==
                              OS_HAL_PSW_IO_SUPERVISOR
                            ));
}

/***********************************************************************************************************************
 *  Os_Hal_CoreGetPrivilegedModeFromPcxi()
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
OS_ALWAYS_INLINE static inline boolean   /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
Os_Hal_CoreGetPrivilegedModeFromPcxi (
  uint32 Param
)
{
  uint32 lowerCsaAddr;
  uint32 pcxi;
  uint32 upperCsaAddr;
  uint32 psw;
  /* #10 Get CSA address of lower context. */
  lowerCsaAddr = Os_Hal_GetCsaAddress(Param);
  /* #20 Get value of PCXI from lower context. */
  pcxi = *(uint32*)(lowerCsaAddr + OS_HAL_CSA_PCXI_OFFSET);                                                             /* PRQA S 0306, 2985 */ /* MD_Os_Hal_Rule11.4_0306, MD_Os_Hal_Rule2.2_2985 */
  /* #30 Get CSA address of upper context. */
  upperCsaAddr = Os_Hal_GetCsaAddress(pcxi);
  /* #40 Get value of PSW from upper context. */
  psw = *(uint32*)(upperCsaAddr + OS_HAL_CSA_PSW_OFFSET);                                                               /* PRQA S 0306 */ /* MD_Os_Hal_Rule11.4_0306 */
  /* #50 Return privileged mode flag. 0 = user mode, !0 = supervisor mode */
  return (boolean)((psw & OS_HAL_PSW_IO_MASK) == OS_HAL_PSW_IO_SUPERVISOR);                                             /* PRQA S 4304 */ /* MD_Os_C90BooleanCompatibility */
}


/***********************************************************************************************************************
 *  Os_Hal_CountLeadingZeros()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline uint8   /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
Os_Hal_CountLeadingZeros (
    Os_Hal_BitFieldType BitField
)
{
  return (uint8)Os_Hal_Clz(BitField);
}


/***********************************************************************************************************************
 *  Os_Hal_StackEndGet()
 **********************************************************************************************************************/
MD_Os_Hal_Rule8.13_3673 */
static inline   Os_Hal_StackType OS_VAR_NOINIT *                                                                                            /* PRQA S 3219 3673 */ /* MD_Os_Rule2.1_3219   OS_ALWAYS_INLINE,
Os_Hal_StackEndGet,
(
    Os_Hal_StackType OS_VAR_NOINIT * LowAddress,
    Os_Hal_StackType OS_VAR_NOINIT * HighAddress
)
{
  OS_IGNORE_UNREF_PARAM(HighAddress);                                                                                   /* PRQA S 3112 */ /* MD_MSR_DummyStmt */

  return LowAddress;
}


/***********************************************************************************************************************
 *  Os_Hal_StackBottomGet()
 **********************************************************************************************************************/
MD_Os_Hal_Rule8.13_3673 */
static inline   Os_Hal_StackType OS_VAR_NOINIT *                                                                                            /* PRQA S 3219 3673 */ /* MD_Os_Rule2.1_3219   OS_ALWAYS_INLINE,
Os_Hal_StackBottomGet,
(
    Os_Hal_StackType OS_VAR_NOINIT * LowAddress,
    Os_Hal_StackType OS_VAR_NOINIT * HighAddress
)
{
  OS_IGNORE_UNREF_PARAM(LowAddress);                                                                                    /* PRQA S 3112 */ /* MD_MSR_DummyStmt */

  return HighAddress;
}


/***********************************************************************************************************************
 *  Os_Hal_StackTopGet()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE                                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
static inline   Os_Hal_StackType OS_VAR_NOINIT *   Os_Hal_StackTopGet (
  void
)
{
  return (Os_Hal_StackType*)Os_Hal_Mfa10();                                                                             /* PRQA S 0306 */ /* MD_Os_Hal_Rule11.4_0306 */
}


/***********************************************************************************************************************
 *  Os_Hal_StackPointerMoveDown()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
  Os_Hal_StackType OS_VAR_NOINIT *   Os_Hal_StackPointerMoveDown (
    Os_Hal_StackType OS_VAR_NOINIT * StackPointer
)
{
  return (Os_Hal_StackType*)(&(StackPointer[1]));
}


/***********************************************************************************************************************
 *  Os_Hal_StackCalculateUsage()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline uint32   /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
Os_Hal_StackCalculateUsage (
  const   Os_Hal_StackType OS_VAR_NOINIT * StackBottom,
  const   Os_Hal_StackType OS_VAR_NOINIT * StackTop
)
{
  return ((uint32)StackBottom - (uint32)StackTop);                                                                      /* PRQA S 0306 */ /* MD_Os_Hal_Rule11.4_0306 */
}


/***********************************************************************************************************************
 *  Os_Hal_SpinInit()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE                                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
static inline void   Os_Hal_SpinInit (
    volatile Os_Hal_SpinlockType OS_VAR_NOINIT * Spinlock
)
{
  *Spinlock = OS_HAL_UNLOCKED;                                                                                          /* SBSW_OS_HAL_PWA_CALLER */
  Os_Hal_Dsync();
}


/***********************************************************************************************************************
 *  Os_Hal_SpinIsLocked()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE                                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
static inline Os_StdReturnType   Os_Hal_SpinIsLocked (
  const   volatile Os_Hal_SpinlockType OS_VAR_NOINIT * Spinlock
)
{
  return (Os_StdReturnType)(*Spinlock != OS_HAL_UNLOCKED);                                                              /* PRQA S 4304 */ /* MD_Os_C90BooleanCompatibility */
}


/***********************************************************************************************************************
 *  Os_Hal_SpinTryLock()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE                                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
static inline Os_Hal_SpinlockResultType   Os_Hal_SpinTryLock (
    volatile Os_Hal_SpinlockType OS_VAR_NOINIT * Spinlock
)
{
  Os_Hal_SpinlockResultType retVal = Os_Hal_CmpSwap(Spinlock, Os_Hal_Mfcr(OS_HAL_CORE_ID_REGISTER), OS_HAL_UNLOCKED);   /* SBSW_OS_HAL_FC_CALLER */
  Os_Hal_Dsync();
  return retVal;
}


/***********************************************************************************************************************
 *  Os_Hal_SpinUnlock()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE                                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
static inline void   Os_Hal_SpinUnlock (
    volatile Os_Hal_SpinlockType OS_VAR_NOINIT * Spinlock
)
{
  Os_Hal_Dsync();
  *Spinlock = OS_HAL_UNLOCKED;                                                                                          /* SBSW_OS_HAL_PWA_CALLER */
  Os_Hal_Dsync();
}


/***********************************************************************************************************************
 *  Os_Hal_XSigInit()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 */
OS_ALWAYS_INLINE static inline void   /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
Os_Hal_XSigInit (
  const   Os_Hal_XSigInterruptConfigType OS_CONST * XSigConfig
)
{
  /* #10 Enable the cross core interrupt. */
  uint32 tempSRC = *(volatile uint32*)(OS_HAL_INT_SRC_BASE + XSigConfig->ConfigData->Source);                           /* PRQA S 0303 */ /* MD_Os_Hal_Rule11.4_0303 */
  tempSRC |= OS_HAL_INT_SRC_SRE_MASK;
  *(volatile uint32*) (OS_HAL_INT_SRC_BASE + XSigConfig->ConfigData->Source) = tempSRC;                                 /* PRQA S 0303 */ /* MD_Os_Hal_Rule11.4_0303 */ /* SBSW_OS_HAL_PWA_ISRCONFIG_XSIG */
}


/***********************************************************************************************************************
 *  Os_Hal_XSigTrigger()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline void   /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
Os_Hal_XSigTrigger (
  const   Os_Hal_XSigInterruptConfigType OS_CONST * XSigConfig
)
{
  uint32 tempSRC = *(volatile uint32*)(OS_HAL_INT_SRC_BASE + XSigConfig->ConfigData->Source);                           /* PRQA S 0303 */ /* MD_Os_Hal_Rule11.4_0303 */
  tempSRC |= OS_HAL_INT_SRC_SETR_MASK;
  *(volatile uint32*) (OS_HAL_INT_SRC_BASE + XSigConfig->ConfigData->Source) = tempSRC;                                 /* PRQA S 0303 */ /* MD_Os_Hal_Rule11.4_0303 */ /* SBSW_OS_HAL_PWA_ISRCONFIG_XSIG */
}


/***********************************************************************************************************************
 *  Os_Hal_XSigAcknowledge()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline void   /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
Os_Hal_XSigAcknowledge (
  const   Os_Hal_XSigInterruptConfigType OS_CONST * XSigConfig
)
{
  OS_IGNORE_UNREF_PARAM(XSigConfig);                                                                                    /* PRQA S 3112 */ /* MD_MSR_DummyStmt */
}


/***********************************************************************************************************************
 *  Os_Hal_CoreIdle()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline void   /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
Os_Hal_CoreIdle (
  void
)
{
  Os_Hal_Nop();
}


/***********************************************************************************************************************
 *  Os_Hal_CoreNop()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline void   /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
Os_Hal_CoreNop (
  void
)
{
  Os_Hal_Nop();
}


# define OS_STOP_SEC_CODE
# include "Os_MemMap_OsCode.h"                                                                                          /* PRQA S 5087 */ /* MD_MSR_MemMap */

#endif /* OS_HAL_CORE_H */

/*!
 * \}
 */

/***********************************************************************************************************************
 *  END OF FILE: Os_Hal_Core.h
 **********************************************************************************************************************/

