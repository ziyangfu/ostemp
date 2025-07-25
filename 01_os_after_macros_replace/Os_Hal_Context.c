 /**********************************************************************************************************************
 *  COPYRIGHT
 *  -------------------------------------------------------------------------------------------------------------------
 *  \verbatim
 *  Copyright (c) 2017 by Vector Informatik GmbH.                                              All rights reserved.
 *
 *                This software is copyright protected and proprietary to Vector Informatik GmbH.
 *                Vector Informatik GmbH grants to you only those rights as set out in the license conditions.
 *                All other rights remain with Vector Informatik GmbH.
 *  \endverbatim
 *  -------------------------------------------------------------------------------------------------------------------
 *  FILE DESCRIPTION
 *  -----------------------------------------------------------------------------------------------------------------*/
/**
 * \addtogroup  Os_Hal_Context
 * \{
 *
 * \file        Os_Hal_Context.c
 * \brief       Context related primitives which must not be inlined.
 *
 *
 *********************************************************************************************************************/
/***********************************************************************************************************************
 *  REVISION HISTORY
 *  --------------------------------------------------------------------------------------------------------------------
 *  Refer to Os_Hal_Os.h.
 **********************************************************************************************************************/

                                                                                                                        /* PRQA S 0777 EOF */ /* MD_MSR_Rule5.1 */

#define OS_HAL_CONTEXT_SOURCE

/**********************************************************************************************************************
 *  INCLUDES
 *********************************************************************************************************************/
/* AUTOSAR includes */
#include "Std_Types.h"

/* Os kernel module dependencies */

/* Os hal dependencies */
#include "Os_Hal_Context.h"

/**********************************************************************************************************************
 *  LOCAL CONSTANT MACROS
 **********************************************************************************************************************/

/**********************************************************************************************************************
 *  LOCAL FUNCTION MACROS
 **********************************************************************************************************************/

/**********************************************************************************************************************
 *  LOCAL DATA TYPES AND STRUCTURES
 **********************************************************************************************************************/

/**********************************************************************************************************************
 *  LOCAL DATA PROTOTYPES
 **********************************************************************************************************************/

/**********************************************************************************************************************
 *  GLOBAL DATA
 **********************************************************************************************************************/

/**********************************************************************************************************************
 *  LOCAL FUNCTION PROTOTYPES
 **********************************************************************************************************************/
#define OS_START_SEC_CODE
#include "Os_MemMap_OsCode.h"                                                                                           /* PRQA S 5087 */ /* MD_MSR_MemMap */


/**********************************************************************************************************************
 *  LOCAL FUNCTIONS
 **********************************************************************************************************************/


/**********************************************************************************************************************
 *  GLOBAL FUNCTIONS
 **********************************************************************************************************************/

/***********************************************************************************************************************
 *  Os_Hal_ContextSwitch()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 */
OS_HAL_NOINLINE void   Os_Hal_ContextSwitch (
    Os_Hal_ContextType OS_VAR_NOINIT * Current,
  const   Os_Hal_ContextType OS_VAR_NOINIT * Next
)
{
  /* #10 Save the current PCXI and the link register */
  Os_Hal_ContextIntSave(Current);                                                                                       /* SBSW_OS_HAL_FC_CALLER */

  /* #20 Restore the next PCXI and the link register */
  Os_Hal_ContextIntRestore(Next);                                                                                       /* SBSW_OS_HAL_FC_CALLER */

  /*!
   * Internal comment removed.
 *
 *
 *
 *
 *
   */
}

/***********************************************************************************************************************
 *  Os_Hal_ContextCall()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 *
 */
/* COV_OS_UNSUPPORTED89701 */
OS_HAL_NOINLINE void   Os_Hal_ContextCall (
    Os_Hal_ContextType OS_VAR_NOINIT * Current,
    Os_Hal_ContextType OS_VAR_NOINIT * Next
)
{
  uint32 PCXIAddr;

  /* #10 Save the current PCXI and the link register */
  Os_Hal_ContextIntSave(Current);                                                                                       /* SBSW_OS_HAL_FC_CALLER */

  /* #20 Append the CSA list of the next context to the current CSA list */
  PCXIAddr = Os_Hal_GetCsaAddress(Next->InitPCXI);
  *(uint32*)PCXIAddr = Current->PreviousContextInfo;                                                                    /* SBSW_OS_HAL_PWA_PCXI_CONTEXTSWITCH */ /* PRQA S 0306 */ /* MD_Os_Hal_Rule11.4_0306 */

  /* #30 Modify stack information in the context, because it is initialized with 0 */
  if(Os_MpIsEnabled() != 0u)                                                                                            /* PRQA S 2991, 2995 */ /* MD_Os_Rule14.3_2991, MD_Os_Rule2.2_2995 */
  {
    Next->MpuRegionForStackLow = Os_Hal_Mfcr(OS_HAL_COREMPU_DPR_LOW0);                                                  /* SBSW_OS_HAL_PWA_CALLER */
    Next->MpuRegionForStackUpper = Os_Hal_Mfcr(OS_HAL_COREMPU_DPR_UPPER0);                                              /* SBSW_OS_HAL_PWA_CALLER */
  }
  PCXIAddr = Os_Hal_GetCsaAddress(Next->PreviousContextInfo);
  *(uint32*)(PCXIAddr + OS_HAL_CSA_SP_OFFSET) = Os_Hal_Mfa10();                                                         /* SBSW_OS_HAL_PWA_PCXI_CONTEXTSWITCH */ /* PRQA S 0306 */ /* MD_Os_Hal_Rule11.4_0306 */

  /* #40 Set the next thread entry */
  Os_Hal_ContextIntRestore(Next);                                                                                       /* SBSW_OS_HAL_FC_CALLER */
}




/***********************************************************************************************************************
 *  Os_Hal_ContextCallOnStack()
 **********************************************************************************************************************/
 /*!
  * Internal comment removed.
 *
 *
 *
 *
  */
OS_HAL_NOINLINE void   Os_Hal_ContextCallOnStack (
    Os_Hal_ContextType OS_VAR_NOINIT * Current,
  const   Os_Hal_ContextType OS_VAR_NOINIT * Next
)
{
  uint32 PCXIAddr;

  /* #10 Save the current PCXI and the link register */
  Os_Hal_ContextIntSave(Current);                                                                                       /* SBSW_OS_HAL_FC_CALLER */

  /* #20 Append the CSA list of the next context to the current CSA list */
  PCXIAddr = Os_Hal_GetCsaAddress(Next->InitPCXI);
  *(uint32*)PCXIAddr = Current->PreviousContextInfo;                                                                    /* SBSW_OS_HAL_PWA_PCXI_CONTEXTSWITCH */ /* PRQA S 0306 */ /* MD_Os_Hal_Rule11.4_0306 */

  /* #30 Set the next thread entry */
  Os_Hal_ContextIntRestore(Next);                                                                                       /* SBSW_OS_HAL_FC_CALLER */
}




#define OS_STOP_SEC_CODE
#include "Os_MemMap_OsCode.h"                                                                                           /* PRQA S 5087 */ /* MD_MSR_MemMap */

/*!
 * \}
 */

/**********************************************************************************************************************
 *  END OF FILE: Os_Hal_Context.c
 *********************************************************************************************************************/
