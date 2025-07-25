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
 * \addtogroup Os_Fifo32
 *
 * \{
 *
 * \file          Os_Fifo32.h
 * \brief         Contains the FIFO interface implementation.
 *  \details      --no details--
 *
 *
 **********************************************************************************************************************/
/***********************************************************************************************************************
 *  REVISION HISTORY
 *  --------------------------------------------------------------------------------------------------------------------
 *  Refer to Os.h.
 **********************************************************************************************************************/

#ifndef OS_FIFO32_H
# define OS_FIFO32_H
                                                                                                                         /* PRQA S 0388 EOF */ /* MD_MSR_Dir1.1 */
/***********************************************************************************************************************
 *  INCLUDES
 **********************************************************************************************************************/
/* AUTOSAR includes */
# include "Std_Types.h"

/* Os module declarations */
# include "Os_Fifo.h"
# include "Os_Fifo32Int.h"

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
 *  Os_Fifo32WriteInit()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 */
OS_ALWAYS_INLINE static inline void   /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
Os_Fifo32WriteInit (
  const   Os_Fifo32WriteConfigType OS_APPL_DATA * Fifo
)
{
  /* #10  Initialize all indices to zero. */
  Fifo->Dyn->WriteIdx = 0;                                                                                              /* SBSW_OS_FF32_FIFOWRITE_DYN_001 */
}


/***********************************************************************************************************************
 *  Os_Fifo32IndicesInRangeOnWrite()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 */
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
OS_ALWAYS_INLINE static inline Os_StdReturnType   Os_Fifo32IndicesInRangeOnWrite (
  const   Os_Fifo32WriteConfigType OS_APPL_DATA * Fifo
)
{
  /* #10  Use base class method. */
  return Os_FifoIndicesInRange(Fifo->Dyn->WriteIdx, Fifo->Read->ReadIdx, Fifo->Size);
}


/***********************************************************************************************************************
 *  Os_Fifo32Enqueue()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
OS_ALWAYS_INLINE static inline Os_FifoWriteResultType   Os_Fifo32Enqueue (
  const   Os_Fifo32WriteConfigType OS_APPL_DATA * Fifo,
  uint32 Value
)
{
  Os_FifoWriteResultType result;

  result = OS_FIFOWRITERESULT_BUFFEROVERFLOW;

  /* #10 Perform checks. */
  if(OS_LIKELY(Os_Fifo32IsFull(Fifo) == 0u))                                                                            /* SBSW_OS_FC_PRECONDITION */
  {
    /* Hint: As a foreign core may read the new Value concurrently, perform the write access before the writeIdx is
     * updated. */
    /* #20 Write the value. */
    Fifo->Data[Fifo->Dyn->WriteIdx].Value = Value;                                                                      /* SBSW_OS_FF32_FIFOWRITE_DATA_001 */

    /* #30 Increment the write index counter. */
    Os_FifoIdxIncrement(&(Fifo->Dyn->WriteIdx), Fifo->Size);                                                            /* SBSW_OS_FF32_FIFOIDXINCREMENT_002 */
    result = OS_FIFOWRITERESULT_OK;
  }

  return result;
}


/***********************************************************************************************************************
 *  Os_Fifo32IsFull()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 */
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
OS_ALWAYS_INLINE static inline Os_StdReturnType OS_CONST Os_Fifo32IsFull (
  const   Os_Fifo32WriteConfigType OS_APPL_DATA * Fifo
)
{
  /* #10  Use base class method. */
  return Os_FifoIsFull(Fifo->Dyn->WriteIdx, Fifo->Read->ReadIdx, Fifo->Size);
}


/***********************************************************************************************************************
 *  FIFO Read Functions
 **********************************************************************************************************************/
/***********************************************************************************************************************
 *  Os_Fifo32ReadInit()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 */
OS_ALWAYS_INLINE static inline void   /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
Os_Fifo32ReadInit (
  const   Os_Fifo32ReadConfigType OS_APPL_DATA * Fifo
)
{
  /* #10  Initialize all indices to zero. */
  Fifo->Dyn->ReadIdx = 0;                                                                                               /* SBSW_OS_FF32_FIFOREAD_DYN_001 */
}


/***********************************************************************************************************************
 *  Os_Fifo32IndicesInRangeOnRead()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 */
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
OS_ALWAYS_INLINE static inline Os_StdReturnType   Os_Fifo32IndicesInRangeOnRead (
  const   Os_Fifo32ReadConfigType OS_APPL_DATA * Fifo
)
{
  /* #10  Use base class method. */
  return Os_FifoIndicesInRange(Fifo->Write->WriteIdx, Fifo->Dyn->ReadIdx, Fifo->Size);
}


/***********************************************************************************************************************
 *  Os_Fifo32Dequeue()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline uint32   /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
Os_Fifo32Dequeue (
  const   Os_Fifo32ReadConfigType OS_APPL_DATA * Fifo
)
{
  uint32 result;

  /* #10 Perform checks. */
  Os_Assert((Os_StdReturnType)(Os_Fifo32IsEmpty(Fifo) == 0u));                                                          /* PRQA S 4304*/ /* MD_Os_C90BooleanCompatibility */ /* SBSW_OS_FC_PRECONDITION */
  /* #20 Read the result value. */
  result = Fifo->Data[Fifo->Dyn->ReadIdx].Value;

  /* #30 Increment the front index counter. */
  Os_FifoIdxIncrement(&(Fifo->Dyn->ReadIdx), Fifo->Size);                                                               /* SBSW_OS_FF32_FIFOIDXINCREMENT_001 */

  return result;
}


/***********************************************************************************************************************
 *  Os_Fifo32IsEmpty()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 */
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
OS_ALWAYS_INLINE static inline Os_StdReturnType OS_CONST Os_Fifo32IsEmpty (
  const   Os_Fifo32ReadConfigType OS_APPL_DATA * Fifo
)
{
  /* #10  Use base class method. */
  return Os_FifoIsEmpty(Fifo->Write->WriteIdx, Fifo->Dyn->ReadIdx);
}


/***********************************************************************************************************************
 *  Os_Fifo32EmptyQueue()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 */
OS_ALWAYS_INLINE static inline void OS_CONST /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
Os_Fifo32EmptyQueue (
  const   Os_Fifo32ReadConfigType OS_APPL_DATA * Fifo
)
{
  /* #10  Use base class method. */
  Os_FifoEmptyQueue(&(Fifo->Write->WriteIdx), &(Fifo->Dyn->ReadIdx));                                                   /* SBSW_OS_FF32_FIFOEMPTYQUEUE_001 */
}


# define OS_STOP_SEC_CODE
# include "Os_MemMap_OsCode.h"                                                                                          /* PRQA S 5087 */ /* MD_MSR_MemMap */


#endif /* OS_FIFO32_H */

/* module specific MISRA deviations:
 */

/*!
 * \}
 */
/***********************************************************************************************************************
 *  END OF FILE: Os_Fifo32.h
 **********************************************************************************************************************/
