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
 * \addtogroup Os_FifoRef
 *
 * \{
 *
 * \file
 * \brief       Contains the FIFO interface implementation.
 * \details     --no details--
 *
 *
 **********************************************************************************************************************/
/***********************************************************************************************************************
 *  REVISION HISTORY
 *  --------------------------------------------------------------------------------------------------------------------
 *  Refer to Os.h.
 **********************************************************************************************************************/

#ifndef OS_FIFOREF_H
# define OS_FIFOREF_H
                                                                                                                         /* PRQA S 0388 EOF */ /* MD_MSR_Dir1.1 */
/***********************************************************************************************************************
 *  INCLUDES
 **********************************************************************************************************************/
/* AUTOSAR includes */
# include "Std_Types.h"

/* Os module declarations */
# include "Os_Fifo.h"
# include "Os_FifoRefInt.h"

/* Os kernel module dependencies */
# include "Os_Error.h"
# include "Os_Common.h"

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
 *  FIFO Write Functions
 **********************************************************************************************************************/
/***********************************************************************************************************************
 *  Os_FifoRefWriteInit()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 */
OS_ALWAYS_INLINE static inline void   /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
Os_FifoRefWriteInit (
  const   Os_FifoRefWriteConfigType OS_APPL_DATA * Fifo
)
{
  /* #10  Initialize all indices to zero. */
  Fifo->Dyn->WriteIdx = 0;                                                                                              /* SBSW_OS_FFREF_FIFOWRITE_DYN_001 */
}


/***********************************************************************************************************************
 *  Os_FifoRefIndicesInRangeOnWrite()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 */
OS_ALWAYS_INLINE static inline Os_StdReturnType   /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
Os_FifoRefIndicesInRangeOnWrite (
  const   Os_FifoRefWriteConfigType OS_APPL_DATA * Fifo
)
{
  /* #10  Use base class method. */
  return Os_FifoIndicesInRange(Fifo->Dyn->WriteIdx, Fifo->Read->ReadIdx, Fifo->Size);
}


/***********************************************************************************************************************
 *  Os_FifoRefEnqueue()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline Os_FifoWriteResultType   /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
Os_FifoRefEnqueue (
  const   Os_FifoRefWriteConfigType OS_APPL_DATA * Fifo,
  const   void   * Value
)
{
  Os_FifoWriteResultType result = OS_FIFOWRITERESULT_BUFFEROVERFLOW;

  /* #10 Perform checks. */
  if(OS_LIKELY(Os_FifoRefIsFull(Fifo) == 0u))                                                                           /* SBSW_OS_FC_PRECONDITION */
  {
    /* Hint: As a foreign core may read the new Value concurrently, perform the write access before the writeIdx is
     * updated. */
    /* #20 Write the value. */
    Fifo->WriteCbk(Fifo->Data, Fifo->Dyn->WriteIdx, Value);                                                             /* SBSW_OS_FFREF_FIFO_WRITECBK_001 */

    /* #30 Increment the write index counter. */
    Os_FifoIdxIncrement(&(Fifo->Dyn->WriteIdx), Fifo->Size);                                                            /* SBSW_OS_FFREF_FIFOIDXINCREMENT_002 */
    result = OS_FIFOWRITERESULT_OK;
  }

  return result;
}


/***********************************************************************************************************************
 *  Os_FifoRefIsFull()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 */
OS_ALWAYS_INLINE static inline Os_StdReturnType OS_CONST /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
Os_FifoRefIsFull (
  const   Os_FifoRefWriteConfigType OS_APPL_DATA * Fifo
)
{
  /* #10  Use base class method. */
  return Os_FifoIsFull(Fifo->Dyn->WriteIdx, Fifo->Read->ReadIdx, Fifo->Size);
}


/***********************************************************************************************************************
 *  FIFO Read Functions
 **********************************************************************************************************************/
/***********************************************************************************************************************
 *  Os_FifoRefReadInit()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 */
OS_ALWAYS_INLINE static inline void   /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
Os_FifoRefReadInit (
  const   Os_FifoRefReadConfigType OS_APPL_DATA * Fifo
)
{
  /* #10  Initialize all indices to zero. */
  Fifo->Dyn->ReadIdx = 0;                                                                                               /* SBSW_OS_FFREF_FIFOREAD_DYN_001 */
}


/***********************************************************************************************************************
 *  Os_FifoRefIndicesInRangeOnRead()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 */
OS_ALWAYS_INLINE static inline Os_StdReturnType   /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
Os_FifoRefIndicesInRangeOnRead (
  const   Os_FifoRefReadConfigType OS_APPL_DATA * Fifo
)
{
  /* #10  Use base class method. */
  return Os_FifoIndicesInRange(Fifo->Write->WriteIdx, Fifo->Dyn->ReadIdx, Fifo->Size);
}


/***********************************************************************************************************************
 *  Os_FifoRefDequeue()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline void   /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
Os_FifoRefDequeue (
  const   Os_FifoRefReadConfigType OS_APPL_DATA * Fifo,
    void   * DataRef
)
{
  /* #10 Perform checks. */
  Os_Assert((Os_StdReturnType)(Os_FifoRefIsEmpty(Fifo) == 0u));                                                         /* PRQA S 4304 */ /* MD_Os_C90BooleanCompatibility */ /* SBSW_OS_FC_PRECONDITION */

  /* #20 Read the result value. */
  Fifo->ReadCbk(Fifo->Data, Fifo->Dyn->ReadIdx, DataRef);                                                               /* SBSW_OS_FFREF_FIFO_READCBK_001 */

  /* #30 Increment the front index counter. */
  Os_FifoIdxIncrement(&(Fifo->Dyn->ReadIdx), Fifo->Size);                                                               /* SBSW_OS_FFREF_FIFOIDXINCREMENT_001 */
}


/***********************************************************************************************************************
 *  Os_FifoRefIsEmpty()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 */
OS_ALWAYS_INLINE static inline Os_StdReturnType OS_CONST /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
Os_FifoRefIsEmpty (
  const   Os_FifoRefReadConfigType OS_APPL_DATA * Fifo
)
{
  /* #10  Use base class method. */
  return Os_FifoIsEmpty(Fifo->Write->WriteIdx, Fifo->Dyn->ReadIdx);
}


/***********************************************************************************************************************
 *  Os_FifoRefEmptyQueue()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 */
OS_ALWAYS_INLINE static inline void OS_CONST /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
Os_FifoRefEmptyQueue (
  const   Os_FifoRefReadConfigType OS_APPL_DATA * Fifo
)
{
  /* #10  Use base class method. */
  Os_FifoEmptyQueue(&(Fifo->Write->WriteIdx), &(Fifo->Dyn->ReadIdx));                                                   /* SBSW_OS_FFREF_FIFOEMPTYQUEUE_001 */
}


# define OS_STOP_SEC_CODE
# include "Os_MemMap_OsCode.h"                                                                                          /* PRQA S 5087 */ /* MD_MSR_MemMap */


#endif /* OS_FIFOREF_H */

/* module specific MISRA deviations:
 */

/*!
 * \}
 */
/***********************************************************************************************************************
 *  END OF FILE: Os_FifoRef.h
 **********************************************************************************************************************/
