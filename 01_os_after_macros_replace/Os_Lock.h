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
 * \addtogroup Os_Lock
 *
 * \{
 *
 * \file
 * \brief       Interface implementation of lock objects.
 * \details     --no details--
 *
 *
 **********************************************************************************************************************/
/***********************************************************************************************************************
 *  REVISION HISTORY
 *  --------------------------------------------------------------------------------------------------------------------
 *  Refer to Os.h.
 **********************************************************************************************************************/

#ifndef OS_LOCK_H
# define OS_LOCK_H
                                                                                                                         /* PRQA S 0388 EOF */ /* MD_MSR_Dir1.1 */
/***********************************************************************************************************************
 *  INCLUDES
 **********************************************************************************************************************/
/* AUTOSAR includes */
# include "Std_Types.h"

/* Os module declarations */
# include "Os_Lock_Types.h"
# include "Os_LockInt.h"

/* Os module dependencies */
# include "Os_Error.h"
# include "Os_Application.h"

/* Os Hal dependencies */
#include "Os_Hal_Compiler.h"



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
 *  Os_LockListInit()
 **********************************************************************************************************************/
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
OS_ALWAYS_INLINE static inline void   Os_LockListInit (
    Os_LockListType OS_CONST * LockList
)
{
  *LockList = OS_LOCK_LIST_END;                                                                                         /* SBSW_OS_PWA_PRECONDITION */
}


/***********************************************************************************************************************
 *  Os_LockListPush()
 **********************************************************************************************************************/
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
OS_ALWAYS_INLINE static inline void   Os_LockListPush (
    Os_LockListType OS_CONST * LockList,
  const   Os_LockConfigType OS_CONST * Lock
)
{
  Lock->Dyn->Next = *LockList;                                                                                          /* SBSW_OS_LOCK_LOCKGETDYN_001 */
  *LockList = Lock;                                                                                                     /* SBSW_OS_PWA_PRECONDITION */
}


/***********************************************************************************************************************
 *  Os_LockListPop()
 **********************************************************************************************************************/
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
OS_ALWAYS_INLINE static inline void   Os_LockListPop (
    Os_LockListType OS_VAR_NOINIT * LockList
)
{
  const   Os_LockConfigType OS_CONST * lock;

  lock = *LockList;
  *LockList = lock->Dyn->Next;                                                                                          /* SBSW_OS_PWA_PRECONDITION */
  lock->Dyn->Next = OS_LOCK_NOT_LINKED;                                                                                 /* PRQA S 0306 */ /* MD_Os_Rule11.4_0306_ListEnd */ /* SBSW_OS_LOCK_LOCKGETDYN_001 */
}


/***********************************************************************************************************************
 *  Os_LockListDelete()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 */
/* PRQA S 3219 static inline void   3673 */ /* MD_Os_Rule2.1_3219 MD_MSR_Rule8.13 */ /* COV_OS_HALPLATFORMMULTICOREUNSUPPORTED */
OS_ALWAYS_INLINE, Os_LockListDelete,
(
    Os_LockListType OS_VAR_NOINIT * LockList,
  const   Os_LockConfigType OS_CONST * Lock
)                                                                                                                      /* COV_OS_HALPLATFORMMULTICOREUNSUPPORTED */
{
  Os_LockListType  *it;

  it = LockList;

  while(Os_LockListIsEmpty(it) == 0u)                                                                                   /* SBSW_OS_LOCK_LOCKLISTISEMPTY_001 */ /* COV_OS_INVSTATE */
  {
    if(Lock == *it)
    {
      *it = Lock->Dyn->Next;                                                                                            /* SBSW_OS_PWA_PRECONDITION */
      Lock->Dyn->Next = OS_LOCK_NOT_LINKED;                                                                             /* PRQA S 0306 */ /* MD_Os_Rule11.4_0306_ListEnd */ /* SBSW_OS_PWA_PRECONDITION */
      break;
    }
    it = Os_LockListNextIterator(it);                                                                                   /* SBSW_OS_LOCK_LOCKLISTNEXTITERATOR_001 */
  }
}


/***********************************************************************************************************************
 *  Os_LockListNext()
 **********************************************************************************************************************/
/* COV_OS_HALPLATFORMMULTICOREUNSUPPORTED */
OS_ALWAYS_INLINE static inline const   Os_LockConfigType OS_CONST *   Os_LockListNext (
  const   Os_LockListType OS_CONST * LockList
)                                                                                                                      /* COV_OS_HALPLATFORMMULTICOREUNSUPPORTED */
{
  return (*LockList)->Dyn->Next;
}

/***********************************************************************************************************************
 *  Os_LockListNextIterator()
 **********************************************************************************************************************/
/* COV_OS_HALPLATFORMMULTICOREUNSUPPORTED */
OS_ALWAYS_INLINE static inline   Os_LockListType OS_VAR_NOINIT *   Os_LockListNextIterator (
  const   Os_LockListType OS_VAR_NOINIT * Iterator
)                                                                                                                      /* COV_OS_HALPLATFORMMULTICOREUNSUPPORTED */
{
  return &((*Iterator)->Dyn->Next);
}


/***********************************************************************************************************************
 *  Os_LockListIsEmpty()
 **********************************************************************************************************************/
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
OS_ALWAYS_INLINE static inline Os_StdReturnType   Os_LockListIsEmpty (
  const   Os_LockListType OS_CONST * LockList
)
{
  return ((Os_StdReturnType)(*LockList == OS_LOCK_LIST_END));                                                           /* PRQA S 4304 */ /* MD_Os_C90BooleanCompatibility */
}


/***********************************************************************************************************************
 *  Os_LockListIsTopRes()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
OS_ALWAYS_INLINE static inline Os_StdReturnType   Os_LockListIsTopRes (
  const   Os_LockListType OS_VAR_NOINIT * LockList
)
{
  Os_StdReturnType result;

  result = 0;

  /* #10 If the list is not empty */
  if(Os_LockListIsEmpty(LockList) == 0u)                                                                                /* SBSW_OS_FC_PRECONDITION */
  {
    /* #20 and if the top element is a resource: return !0  */
    result = (Os_StdReturnType)(Os_LockIsResource(*LockList) != 0u);                                                    /* PRQA S 4304 */ /* MD_Os_C90BooleanCompatibility */ /* SBSW_OS_LOCK_LOCKISRESOURCE_001 */
  }

  /* #30 otherwise return 0. */
  return result;
}


/***********************************************************************************************************************
 *  Os_LockListCheckIsListTop()
 **********************************************************************************************************************/
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
OS_ALWAYS_INLINE static inline Os_StdReturnType   Os_LockListCheckIsListTop (
  const   Os_LockListType OS_VAR_NOINIT * LockList,
  const   Os_LockConfigType OS_CONST * Lock
)
{
  return Os_ErrExtendedCheck((Os_StdReturnType)(*LockList == Lock));                                                    /* PRQA S 4304 */ /* MD_Os_C90BooleanCompatibility */
}


/***********************************************************************************************************************
 *  Os_LockListIsTopSpinlock()
 **********************************************************************************************************************/
/*!
 * Internal comment removed.
 *
 *
 *
 *
 */
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
OS_ALWAYS_INLINE static inline Os_StdReturnType   Os_LockListIsTopSpinlock (
  const   Os_LockListType OS_VAR_NOINIT * LockList
)
{
  Os_StdReturnType result;

  result = 0;

  /* #10 If the list is not empty. */
  if(Os_LockListIsEmpty(LockList) == 0u)                                                                                /* SBSW_OS_FC_PRECONDITION */
  {
    /* #20 And if the top element is a spinlock: return !0. */
    result = (Os_StdReturnType)(((*LockList)->Type == OS_LOCKTYPE_SPINLOCK));                                           /* PRQA S 4304 */ /* MD_Os_C90BooleanCompatibility */
  }

  /* #30 Otherwise return 0. */
  return result;
}


/***********************************************************************************************************************
 *  Os_LockListGetTop()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
const   Os_LockConfigType OS_CONST *   Os_LockListGetTop (
  const   Os_LockListType OS_VAR_NOINIT * LockList
)
{
  return (*LockList);
}


/***********************************************************************************************************************
 *  Os_LockInit()
 **********************************************************************************************************************/
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
OS_ALWAYS_INLINE static inline void   Os_LockInit (
  const   Os_LockConfigType OS_CONST * Lock
)
{
  Lock->Dyn->Next = OS_LOCK_NOT_LINKED;                                                                                 /* PRQA S 0306 */ /* MD_Os_Rule11.4_0306_ListEnd */ /* SBSW_OS_LOCK_LOCKGETDYN_001 */
}


/***********************************************************************************************************************
 *  Os_LockIsIntResource()
 **********************************************************************************************************************/
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
OS_ALWAYS_INLINE static inline Os_StdReturnType   Os_LockIsIntResource (
  const   Os_LockConfigType OS_CONST * Lock
)
{
  return (Os_StdReturnType)(Lock->Type == OS_LOCKTYPE_RESOURCE_INTERRUPT);                                              /* PRQA S 4304 */ /* MD_Os_C90BooleanCompatibility */
}


/***********************************************************************************************************************
 *  Os_LockIsResource()
 **********************************************************************************************************************/
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
OS_ALWAYS_INLINE static inline Os_StdReturnType   Os_LockIsResource (
  const   Os_LockConfigType OS_CONST * Lock
)
{
  return (Os_StdReturnType)(    (Lock->Type == OS_LOCKTYPE_RESOURCE_STANDARD)                                           /* PRQA S 4304 */ /* MD_Os_C90BooleanCompatibility */
                            ||  (Lock->Type == OS_LOCKTYPE_RESOURCE_INTERRUPT));                                        /* COV_OS_HALPLATFORMMULTICOREUNSUPPORTED */
}


/***********************************************************************************************************************
 *  Os_LockCheckIsAvailable()
 **********************************************************************************************************************/
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
OS_ALWAYS_INLINE static inline Os_StdReturnType   Os_LockCheckIsAvailable (
  const   Os_LockConfigType OS_CONST * Lock
)
{
  return Os_ErrExtendedCheck(
      (Os_StdReturnType)(Lock->Dyn->Next == OS_LOCK_NOT_LINKED));                                                       /* PRQA S 0306, 4304, 2996 */ /* MD_Os_Rule11.4_0306_ListEnd, MD_Os_C90BooleanCompatibility, MD_Os_Rule2.2_2996 */
}


/***********************************************************************************************************************
 *  Os_LockCheckIsOccupied()
 **********************************************************************************************************************/
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
OS_ALWAYS_INLINE static inline Os_StdReturnType   Os_LockCheckIsOccupied (
  const   Os_LockConfigType OS_CONST * Lock
)
{
  return Os_ErrExtendedCheck(
      (Os_StdReturnType)(Lock->Dyn->Next != OS_LOCK_NOT_LINKED));                                                       /* PRQA S 0306, 4304, 2995 */ /* MD_Os_Rule11.4_0306_ListEnd, MD_Os_C90BooleanCompatibility, MD_Os_Rule2.2_2995 */
}


/***********************************************************************************************************************
 *  Os_LockGetDyn()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
  Os_LockType OS_VAR_NOINIT *   Os_LockGetDyn (
  const   Os_LockConfigType OS_CONST * Lock
)
{
  return Lock->Dyn;
}


/***********************************************************************************************************************
 *  Os_LockGetAccessingApplications()
 **********************************************************************************************************************/
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
OS_ALWAYS_INLINE static inline Os_AppAccessMaskType   Os_LockGetAccessingApplications (
  const   Os_LockConfigType OS_CONST * Lock
)
{
  return Lock->AccessingApplications;
}


# define OS_STOP_SEC_CODE
# include "Os_MemMap_OsCode.h"                                                                                          /* PRQA S 5087 */ /* MD_MSR_MemMap */


#endif /* OS_LOCK_H */

/* module specific MISRA deviations:
 */

/*!
 * \}
 */
/***********************************************************************************************************************
 *  END OF FILE: Os_LockInt.h
 **********************************************************************************************************************/
