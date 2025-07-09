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
 * \addtogroup Os_Job
 *
 * \{
 *
 * \file
 * \brief       Job implementation header.
 * \details     --no details--
 *
 *
 **********************************************************************************************************************/
/***********************************************************************************************************************
 *  REVISION HISTORY
 *  --------------------------------------------------------------------------------------------------------------------
 *  Refer to Os.h.
 **********************************************************************************************************************/

#ifndef OS_JOB_H
# define OS_JOB_H
                                                                                                                         /* PRQA S 0388 EOF */ /* MD_MSR_Dir1.1 */
/***********************************************************************************************************************
 *  INCLUDES
 **********************************************************************************************************************/
/* AUTOSAR includes */
# include "Std_Types.h"

/* Os module declarations */
# include "Os_Job_Types.h"
# include "Os_JobInt.h"

/* Os kernel module dependencies */
# include "Os_Counter.h"
# include "Os_Common.h"
# include "Os_Error.h"

/* Os HAL dependencies */
# include "Os_Hal_Compiler.h"




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
 *  Os_JobPriorityCompare()
 **********************************************************************************************************************/
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
OS_ALWAYS_INLINE static inline Os_StdReturnType   Os_JobPriorityCompare (
  const   Os_JobConfigType OS_CONST * LeftJob,
  const   Os_JobConfigType OS_CONST * RightJob
)
{
  Os_Assert((Os_StdReturnType)(LeftJob->Counter == RightJob->Counter));                                                 /* PRQA S 4304 */ /* MD_Os_C90BooleanCompatibility */

  /* If the right job is in the future of the left job, the left job has a higher priority. */
  return  Os_CounterIsFutureValue(                                                                                      /* SBSW_OS_JOB_COUNTERISFUTUREVALUE_001 */
            LeftJob->Counter,
            RightJob->Dyn->ExpirationTimestamp,
            LeftJob->Dyn->ExpirationTimestamp);
}


/***********************************************************************************************************************
 *  Os_JobGetExpirationTimestamp()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline Os_TickType   /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
Os_JobGetExpirationTimestamp (
  const   Os_JobConfigType OS_CONST * Job
)
{
  return Job->Dyn->ExpirationTimestamp;
}


/***********************************************************************************************************************
 *  Os_JobCopyExpirationTimestamp()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline void   /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
Os_JobCopyExpirationTimestamp (
  const   Os_JobConfigType OS_CONST * TargetJob,
  const   Os_JobConfigType OS_CONST * SourceJob
)
{
  TargetJob->Dyn->ExpirationTimestamp = SourceJob->Dyn->ExpirationTimestamp;                                            /* SBSW_OS_JOB_JOBGETDYN_001 */
}


/***********************************************************************************************************************
 *  Os_JobGetDyn()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
  Os_JobType OS_VAR_NOINIT *   Os_JobGetDyn (
  const   Os_JobConfigType OS_CONST * Job
)
{
  return Job->Dyn;
}


/***********************************************************************************************************************
 *  Os_JobSetExpirationTimestamp()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline void   /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
Os_JobSetExpirationTimestamp (
  const   Os_JobConfigType OS_CONST * Job,
  Os_TickType TimeStamp
)
{
  Job->Dyn->ExpirationTimestamp = TimeStamp;                                                                            /* SBSW_OS_JOB_JOBGETDYN_001 */
}


/***********************************************************************************************************************
 *  Os_JobAddRel()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline void   /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
Os_JobAddRel (
  const   Os_JobConfigType OS_CONST * Job,
  Os_TickType Offset
)
{
  Os_CounterAddRelJob(Job->Counter, Job, Offset);                                                                       /* SBSW_OS_JOB_COUNTERADDRELJOB_001 */
}


/***********************************************************************************************************************
 *  Os_JobAddAbs()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline void   /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
Os_JobAddAbs (
  const   Os_JobConfigType OS_CONST * Job,
  Os_TickType Start
)
{
  Os_CounterAddAbsJob(Job->Counter, Job, Start);                                                                        /* SBSW_OS_JOB_COUNTERADDABSJOB_001 */
}


/***********************************************************************************************************************
 *  Os_JobReload()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline void   /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
Os_JobReload (
  const   Os_JobConfigType OS_CONST * Job,
  Os_TickType Offset
)
{
  Os_CounterReloadJob(Job->Counter, Job, Offset);                                                                       /* SBSW_OS_JOB_COUNTERRELOADJOB_001 */
}


/***********************************************************************************************************************
 *  Os_JobDelete()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline void   /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
Os_JobDelete (
  const   Os_JobConfigType OS_CONST * Job
)
{
  Os_CounterDeleteJob(Job->Counter, Job);                                                                               /* SBSW_OS_JOB_COUNTERDELETEJOB_001 */
}


/***********************************************************************************************************************
 *  Os_JobGetCharacteristics()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
const   Os_CounterCharacteristicsConfigType OS_CONST *   Os_JobGetCharacteristics (
  const   Os_JobConfigType OS_CONST * Job
)
{
  return Os_CounterGetCharacteristics(Job->Counter);                                                                    /* SBSW_OS_JOB_COUNTERGETCHARACTERISTICS_001 */
}


/***********************************************************************************************************************
 *  Os_JobGetExpiryTime()
 **********************************************************************************************************************/
/*!
 *  Internal comment removed.
 *
 *
 *
 *
 *
 *
 *
 */
OS_ALWAYS_INLINE static inline Os_TickType   /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
Os_JobGetExpiryTime (
  const   Os_JobConfigType OS_CONST * Job,
  Os_TickType TimeStamp
)
{
  Os_TickType now;
  Os_TickType timeStampMod;
  Os_TickType result;

  /* #10 Get current time from counter. */
  now = Os_CounterGetValue(Job->Counter);                                                                               /* SBSW_OS_JOB_COUNTERGETVALUE_001 */

  /* #20 Perform TimeStamp % MaxAllowedValue. */
  timeStampMod = Os_CounterGetModulo(Job->Counter, TimeStamp);                                                          /* SBSW_OS_JOB_COUNTERGETMODULO_001 */

  /* #30 If the TimeStamp is a future value: */
  if(OS_LIKELY(Os_CounterIsFutureValue(Job->Counter, timeStampMod, now) != 0u))                                         /* SBSW_OS_JOB_COUNTERISFUTUREVALUE_001 */ /* COV_OS_NOFUNCCALL */
  {
    /* #40 Subtract current time from TimeStamp % MaxAllowedValue. */
    result = Os_CounterSub(Job->Counter, timeStampMod, now);                                                            /* SBSW_OS_JOB_COUNTERSUB_001 */
  }
  /* #50 Otherwise: */
  else
  {
    /* #60 Job expiration time already passed, job will be handled with the next timer interrupt. */
    result = 0u;
  }

  return result;
}



/***********************************************************************************************************************
 *  Os_JobCheckValueLeMaxAllowed()
 **********************************************************************************************************************/
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
OS_ALWAYS_INLINE static inline Os_StdReturnType   Os_JobCheckValueLeMaxAllowed (
  const   Os_JobConfigType OS_CONST * Job,
  TickType Value
)
{
  return Os_CounterCheckValueLeMaxAllowed(Job->Counter, Value);                                                         /* SBSW_OS_JOB_COUNTERCHECKVALUELEMAXALLOWED_001 */
}


/***********************************************************************************************************************
 *  Os_JobCheckValueLeReducedMaxAllowed()
 **********************************************************************************************************************/
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
OS_ALWAYS_INLINE static inline Os_StdReturnType   Os_JobCheckValueLeReducedMaxAllowed (
  const   Os_JobConfigType OS_CONST * Job,
  TickType Reduce,
  TickType Value
)
{
  return Os_CounterCheckValueLeReducedMaxAllowed(Job->Counter, Reduce, Value);                                          /* SBSW_OS_JOB_COUNTERCHECKVALUELEREDUCEDMAXALLOWED_001 */
}


/***********************************************************************************************************************
 *  Os_JobCheckValueGeMinCycleOrZero()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE                                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
static inline Os_StdReturnType   Os_JobCheckValueGeMinCycleOrZero (
  const   Os_JobConfigType OS_CONST * Job,
  TickType Cycle
)
{
  return Os_CounterCheckValueGeMinCycleOrZero(Job->Counter, Cycle);                                                     /* SBSW_OS_JOB_COUNTERCHECKVALUEGEMINCYCLEORZERO_001 */
}


/***********************************************************************************************************************
 *  Os_JobCheckAssignedToSameCounter()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE                                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
static inline Os_StdReturnType   Os_JobCheckAssignedToSameCounter (
  const   Os_JobConfigType OS_CONST * Job_1,
  const   Os_JobConfigType OS_CONST * Job_2
)
{
  return Os_ErrExtendedCheck((Os_StdReturnType)(Job_1->Counter == Job_2->Counter));                                     /* PRQA S 4304 */ /* MD_Os_C90BooleanCompatibility */
}


/***********************************************************************************************************************
 *  Os_JobGetCore()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline                                                                            /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
const   Os_CoreAsrConfigType OS_CONST *   Os_JobGetCore (
  const   Os_JobConfigType OS_CONST * Job
)
{
  return Os_CounterGetCore(Job->Counter);                                                                               /* SBSW_OS_JOB_COUNTERGETCORE_001 */
}


/***********************************************************************************************************************
 *  Os_JobGetValue()
 **********************************************************************************************************************/
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
OS_ALWAYS_INLINE static inline Os_TickType   Os_JobGetValue (
  const   Os_JobConfigType OS_CONST * Job
)
{
  return Os_CounterGetValue(Job->Counter);                                                                              /* SBSW_OS_JOB_COUNTERGETVALUE_001 */
}


/***********************************************************************************************************************
 *  Os_JobSub()
 **********************************************************************************************************************/
/* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
OS_ALWAYS_INLINE static inline Os_TickType   Os_JobSub (
  const   Os_JobConfigType OS_CONST * Job,
  Os_TickType Value,
  Os_TickType Sub
)
{
  return Os_CounterSub(Job->Counter, Value, Sub);                                                                       /* SBSW_OS_JOB_COUNTERSUB_001 */
}


/***********************************************************************************************************************
 *  Os_JobDo()
 **********************************************************************************************************************/
OS_ALWAYS_INLINE static inline void   /* PRQA S 3219 */ /* MD_Os_Rule2.1_3219 */
Os_JobDo (
  const   Os_JobConfigType OS_CONST * Job
)
{
  Job->Callback(Job);                                                                                                   /* SBSW_OS_JOB_JOB_CALLBACK_001 */
}


# define OS_STOP_SEC_CODE
# include "Os_MemMap_OsCode.h"                                                                                          /* PRQA S 5087 */ /* MD_MSR_MemMap */





#endif /* OS_JOB_H */

/* module specific MISRA deviations:
 */

/*!
 * \}
 */
/***********************************************************************************************************************
 *  END OF FILE: Os_JobInt.h
 **********************************************************************************************************************/
