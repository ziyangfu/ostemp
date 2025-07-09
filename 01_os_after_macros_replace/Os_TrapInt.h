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
 *  \defgroup    Os_Trap Trap
 *  \brief       Handles supervisor mode entries.
 *  \details
 *  This module provides the OS API to the user. It is also responsible to get access to data and peripherals
 *  needed to execute OS services.
 *
 *  The following cases are differentiated by the trap module:
 *
 *   - Memory protection disabled: All APIs are called directly from the trap module.
 *   - Memory protection enabled:
 *     - Regular APIs which need write access to OS data check whether the caller is privileged:
 *       - Privileged caller: Direct call to API.
 *       - Non-Privileged caller: Call API via trap.
 *     - APIs which do not need write access to OS data call the API directly.
 *     - APIs which have out-parameters use local variables to protect the OS from invalid user-pointers.
 *       The write operation to the original pointer is performed in the trap function with caller's privilege level.
 *     - APIs which are always called in privileged mode (e.g. Os_DisableLevelKM()) directly call the API.
 *     - APIs which are expected to be called in non-privileged mode (e.g. Os_WritePeripheral8())
 *       call the API via trap.
 *
 *  ![Service Call - No Memory Protection](Trap_NoMemoryProtection.png)
 *
 *  ![Service Call - Memory Protection](Trap_MemoryProtection.png)
 *
 *  ![Service Call - Memory Protection For Trusted](Trap_MemoryProtectionForTrusted.png)
 *
 *
 *
 *
 *
 *  \{
 *
 *  \file
 *  \brief       Internal Trap API declaration.
 *
 *
 **********************************************************************************************************************/
/***********************************************************************************************************************
 *  REVISION HISTORY
 *  --------------------------------------------------------------------------------------------------------------------
 *  Refer to Os.h.
 **********************************************************************************************************************/

#ifndef OS_TRAPINT_H
# define OS_TRAPINT_H
                                                                                                                         /* PRQA S 0388 EOF */ /* MD_MSR_Dir1.1 */
/***********************************************************************************************************************
 *  INCLUDES
 **********************************************************************************************************************/
/* AUTOSAR includes */
# include "Std_Types.h"

/* Os module declarations */
# include "Os_Trap_Types.h"

/* Os module dependencies */
# include "Os_Spinlock_Types.h"
# include "Os_Ioc_Types.h"
# include "Os_ErrorInt.h"
# include "Os_KernelInt.h"

/* Os Hal dependencies */
# include "Os_Hal_CompilerInt.h"



/***********************************************************************************************************************
 *  GLOBAL CONSTANT MACROS
 **********************************************************************************************************************/


/***********************************************************************************************************************
 *  GLOBAL FUNCTION MACROS
 **********************************************************************************************************************/


/***********************************************************************************************************************
 *  GLOBAL DATA TYPES AND STRUCTURES
 **********************************************************************************************************************/
/*! Trap vector service ID. */
typedef enum
{
  OS_TRAPID_GETCOREID,
  OS_TRAPID_GETACTIVEAPPLICATIONMODE,
  OS_TRAPID_SHUTDOWNOS,
  OS_TRAPID_SHUTDOWNALLCORES,
  OS_TRAPID_DISABLELEVEL,
  OS_TRAPID_ENABLELEVEL,
  OS_TRAPID_DISABLEGLOBAL,
  OS_TRAPID_ENABLEGLOBAL,
  OS_TRAPID_DISABLEALLINTERRUPTS,
  OS_TRAPID_ENABLEALLINTERRUPTS,
  OS_TRAPID_SUSPENDALLINTERRUPTS,
  OS_TRAPID_RESUMEALLINTERRUPTS,
  OS_TRAPID_SUSPENDOSINTERRUPTS,
  OS_TRAPID_RESUMEOSINTERRUPTS,
  OS_TRAPID_DISABLEINTERRUPTSOURCE,
  OS_TRAPID_ENABLEINTERRUPTSOURCE,
  OS_TRAPID_CLEARPENDINGINTERRUPT,
  OS_TRAPID_ISINTERRUPTSOURCEENABLED,
  OS_TRAPID_ISINTERRUPTPENDING,
  OS_TRAPID_ALLOWACCESS,
  OS_TRAPID_GETAPPLICATIONID,
  OS_TRAPID_GETCURRENTAPPLICATIONID,
  OS_TRAPID_GETAPPLICATIONSTATE,
  OS_TRAPID_CHECKOBJECTACCESS,
  OS_TRAPID_CHECKOBJECTOWNERSHIP,
  OS_TRAPID_TERMINATEAPPLICATION,
  OS_TRAPID_ACTIVATETASK,
  OS_TRAPID_TERMINATETASK,
  OS_TRAPID_CHAINTASK,
  OS_TRAPID_GETTASKSTATE,
  OS_TRAPID_GETTASKID,
  OS_TRAPID_GETISRID,
  OS_TRAPID_SCHEDULE,
  OS_TRAPID_INCREMENTCOUNTER,
  OS_TRAPID_GETCOUNTERVALUE,
  OS_TRAPID_GETELAPSEDVALUE,
  OS_TRAPID_GETALARM,
  OS_TRAPID_GETALARMBASE,
  OS_TRAPID_SETRELALARM,
  OS_TRAPID_SETABSALARM,
  OS_TRAPID_CANCELALARM,
  OS_TRAPID_GETRESOURCE,
  OS_TRAPID_RELEASERESOURCE,
  OS_TRAPID_SETEVENT,
  OS_TRAPID_CLEAREVENT,
  OS_TRAPID_WAITEVENT,
  OS_TRAPID_GETEVENT,
  OS_TRAPID_STARTSCHEDULETABLEREL,
  OS_TRAPID_STARTSCHEDULETABLEABS,
  OS_TRAPID_STOPSCHEDULETABLE,
  OS_TRAPID_NEXTSCHEDULETABLE,
  OS_TRAPID_STARTSCHEDULETABLESYNCHRON,
  OS_TRAPID_SETSCHEDULETABLEASYNC,
  OS_TRAPID_SYNCSCHEDULETABLE,
  OS_TRAPID_GETSCHEDULETABLESTATUS,
  OS_TRAPID_SPINLOCKGET,
  OS_TRAPID_SPINLOCKRELEASE,
  OS_TRAPID_SPINLOCKTRYGET,
  OS_TRAPID_IOC08RECEIVE,
  OS_TRAPID_IOC08EMPTYQUEUE,
  OS_TRAPID_IOC08SEND,
  OS_TRAPID_IOC16RECEIVE,
  OS_TRAPID_IOC16EMPTYQUEUE,
  OS_TRAPID_IOC16SEND,
  OS_TRAPID_IOC32RECEIVE,
  OS_TRAPID_IOC32EMPTYQUEUE,
  OS_TRAPID_IOC32SEND,
  OS_TRAPID_IOCREFRECEIVE,
  OS_TRAPID_IOCREFEMPTYQUEUE,
  OS_TRAPID_IOCREFSEND,
  OS_TRAPID_IOCREAD,
  OS_TRAPID_IOCWRITE,
  OS_TRAPID_IOC08MULTIRECEIVE,
  OS_TRAPID_IOC08EMPTYMULTIQUEUE,
  OS_TRAPID_IOC16MULTIRECEIVE,
  OS_TRAPID_IOC16EMPTYMULTIQUEUE,
  OS_TRAPID_IOC32MULTIRECEIVE,
  OS_TRAPID_IOC32EMPTYMULTIQUEUE,
  OS_TRAPID_IOCREFMULTIRECEIVE,
  OS_TRAPID_IOCREFEMPTYMULTIQUEUE,
  OS_TRAPID_READPERIPHERAL8,
  OS_TRAPID_READPERIPHERAL16,
  OS_TRAPID_READPERIPHERAL32,
  OS_TRAPID_WRITEPERIPHERAL8,
  OS_TRAPID_WRITEPERIPHERAL16,
  OS_TRAPID_WRITEPERIPHERAL32,
  OS_TRAPID_MODIFYPERIPHERAL8,
  OS_TRAPID_MODIFYPERIPHERAL16,
  OS_TRAPID_MODIFYPERIPHERAL32,
  OS_TRAPID_CALLTRUSTEDFUNCTION,
  OS_TRAPID_CALLNONTRUSTEDFUNCTION,
  OS_TRAPID_CALLFASTTRUSTEDFUNCTION,
  OS_TRAPID_ERRAPPLICATIONERROR,
  OS_TRAPID_TASKMISSINGTERMINATETASK,
  OS_TRAPID_ISREPILOGUE,
  OS_TRAPID_SERVICERETURN,
  OS_TRAPID_HOOKRETURN,
  OS_TRAPID_CONTROLIDLE,
  OS_TRAPID_GETTASKSTACKUSAGE,
  OS_TRAPID_GETISRSTACKUSAGE,
  OS_TRAPID_GETKERNELSTACKUSAGE,
  OS_TRAPID_GETSTARTUPHOOKSTACKUSAGE,
  OS_TRAPID_GETERRORHOOKSTACKUSAGE,
  OS_TRAPID_GETSHUTDOWNHOOKSTACKUSAGE,
  OS_TRAPID_GETPROTECTIONHOOKSTACKUSAGE,
  OS_TRAPID_GETUNHANDLEDIRQ,
  OS_TRAPID_GETUNHANDLEDEXC,
  OS_TRAPID_GETDETAILEDERROR,
  OS_TRAPID_GETSERVICEID,
  OS_TRAPID_COREGETTHREAD,
  OS_TRAPID_GETNUMBEROFACTIVATEDCORES,
  OS_TRAPID_CHECKTASKMEMORYACCESS,
  OS_TRAPID_CHECKISRMEMORYACCESS,
  OS_TRAPID_BARRIERSYNCHRONIZE,
  OS_TRAPID_GETEXCEPTIONCONTEXT,
  OS_TRAPID_SETEXCEPTIONCONTEXT,
  OS_TRAPID_GETCORESTARTSTATE,
  OS_TRAPID_GETEXCEPTIONADDRESS,
  OS_TRAPID_GETNONTRUSTEDFUNCTIONSTACKUSAGE,
  OS_TRAPID_COUNT
} Os_TrapIdType;


/*! Identifies parameters within a API prototype. */
typedef enum
{
  OS_TRAPPARAMINDEX_1 = 0,
  OS_TRAPPARAMINDEX_2,
  OS_TRAPPARAMINDEX_3,
  OS_TRAPPARAMINDEX_4,
  OS_TRAPPARAMINDEX_COUNT
} Os_TrapParamIndexType;


/*! Union of all OS API parameter types. */
typedef union
{                                                                                                                       /* PRQA S 0750 */ /* MD_MSR_Union */
  Os_ParamType                                                  ParamApiType;
  const   Os_SpinlockConfigType OS_CONST *             ParamSpinlockConfigRefType;
  const   Os_Ioc08SendConfigType OS_CONST *            ParamIoc08SendConfigRefType;
  const   Os_Ioc08ReceiveConfigType OS_CONST *         ParamIoc08ReceiveConfigRefType;
    uint8   *                              ParamUint8RefType;
  const   uint8   *                            ParamConstUint8RefType;
  uint8                                                         ParamUint8Type;
  const   Os_Ioc16SendConfigType OS_CONST *            ParamIoc16SendConfigRefType;
  const   Os_Ioc16ReceiveConfigType OS_CONST *         ParamIoc16ReceiveConfigRefType;
    uint16   *                             ParamUint16RefType;
  const   uint16   *                           ParamConstUint16RefType;
  uint16                                                        ParamUint16Type;
  const   Os_Ioc32SendConfigType OS_CONST *            ParamIoc32SendConfigRefType;
  const   Os_Ioc32ReceiveConfigType OS_CONST *         ParamIoc32ReceiveConfigRefType;
    uint32   *                             ParamUint32RefType;
  const   uint32   *                           ParamConstUint32RefType;
  uint32                                                        ParamUint32Type;
  const   Os_IocRefSendConfigType OS_CONST *           ParamIocRefSendConfigRefType;
  const   Os_IocRefReceiveConfigType OS_CONST *        ParamIocRefReceiveConfigRefType;
    void   *                               ParamVoidRefType;
  const   void   *                             ParamVoidConfigRefType;
  const   Os_IocReadConfigType OS_CONST *              ParamIocReadConfigRefType;
  const   Os_IocWriteConfigType OS_CONST *             ParamIocWriteConfigRefType;
  const   Os_Ioc08MultiReceiveConfigType OS_CONST *    ParamIoc08MultiReceiveConfigRefType;
  const   Os_Ioc16MultiReceiveConfigType OS_CONST *    ParamIoc16MultiReceiveConfigRefType;
  const   Os_Ioc32MultiReceiveConfigType OS_CONST *    ParamIoc32MultiReceiveConfigRefType;
  const   Os_IocRefMultiReceiveConfigType OS_CONST *   ParamIocRefMultiReceiveConfigRefType;
  OSServiceIdType                                               ParamOSServiceIdType;
  Os_StatusType                                                 ParamOs_StatusType;
    Os_ErrorParamsType OS_VAR_NOINIT *             ParamOs_ErrorParamsRefType;
} Os_TrapParameterType;


/*! Union of all OS API return types. */
typedef union
{                                                                                                                       /* PRQA S 0750 */ /* MD_MSR_Union */
  Os_StatusType         ReturnOs_StatusType;
  Std_ReturnType        ReturnStd_ReturnType;
  uint8                 ReturnUint8Type;
  uint16                ReturnUint16Type;
  uint32                ReturnUint32Type;
  CoreIdType            ReturnCoreIdType;
  AppModeType           ReturnAppModeType;
  ISRType               ReturnISRType;
  ApplicationType       ReturnApplicationType;
  ObjectAccessType      ReturnObjectAccessType;
  AccessType            ReturnAccessType;
  OSServiceIdType       ReturnOSServiceIdType;
  Os_AddressOfConstType ReturnOs_AddressOfConstType;
  const   Os_ThreadConfigType OS_CONST *   ReturnThreadConfigRefType;
} Os_TrapReturnType;



/*! Parameter packet to pass service data from user space to kernel's trap handler. */
struct Os_TrapPacketType_Tag
{
  /*! Service to be called. */
  Os_TrapIdType TrapId;

  /*! Parameters to be passed. */
  Os_TrapParameterType Parameters[OS_TRAPPARAMINDEX_COUNT];

  /*! Return value. */
  volatile Os_TrapReturnType ReturnValue;
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
 *  Os_TrapSpinlockGet()
 **********************************************************************************************************************/
/*! \brief        OS service GetSpinlock().
 *  \details      For further details see Os_SpinlockGet().
 *
 *  \param[in]    Spinlock        See Os_SpinlockGet().
 *
 *  \return       See Os_SpinlockGet().
 *
 *  \context      TASK|ISR2
 *
 *  \reentrant    TRUE
 *  \synchronous  TRUE
 *
 *  \pre          See Os_SpinlockGet().
 **********************************************************************************************************************/
Os_StatusType     Os_TrapSpinlockGet
(
  const   Os_SpinlockConfigType OS_CONST * Spinlock
);


/***********************************************************************************************************************
 *  Os_TrapSpinlockRelease()
 **********************************************************************************************************************/
/*! \brief        OS service ReleaseSpinlock().
 *  \details      For further details see Os_SpinlockRelease().
 *
 *  \param[in]    Spinlock        See Os_SpinlockRelease().
 *
 *  \return       See Os_SpinlockRelease().
 *
 *  \context      TASK|ISR2
 *
 *  \reentrant    TRUE
 *  \synchronous  TRUE
 *
 *  \pre          See Os_SpinlockRelease().
 **********************************************************************************************************************/
Os_StatusType     Os_TrapSpinlockRelease
(
  const   Os_SpinlockConfigType OS_CONST * Spinlock
);


/***********************************************************************************************************************
 *  Os_TrapSpinlockTryGet()
 **********************************************************************************************************************/
/*! \brief        OS service TryGetSpinlock().
 *  \details      For further details see Os_SpinlockTryGet().
 *
 *  \param[in]    Spinlock        See Os_SpinlockTryGet().
 *  \param[out]   Success         See Os_SpinlockTryGet().
 *
 *  \return       See Os_SpinlockTryGet().
 *
 *  \context      TASK|ISR2
 *
 *  \reentrant    TRUE
 *  \synchronous  TRUE
 *
 *  \pre          See Os_SpinlockTryGet().
 **********************************************************************************************************************/
Os_StatusType     Os_TrapSpinlockTryGet
(
  const   Os_SpinlockConfigType OS_CONST * Spinlock,
  TryToGetSpinlockType* Success
);


/***********************************************************************************************************************
 *  Os_TrapIoc08Receive()
 **********************************************************************************************************************/
/*! \brief        Reads one item from the IOC.
 *  \details      For further details see Os_Ioc08Receive().
 *
 *  \param[in]      IsTrapRequired  Defines whether a trap is required:
 *                                   - TRUE:  Trap is always performed.
 *                                   - FALSE: Trap is performed, if supervisor mode is required by platform.
 *  \param[in,out]  Ioc             See Os_Ioc08Receive().
 *  \param[out]     Data            See Os_Ioc08Receive().
 *
 *  \return       See Os_Ioc08Receive().
 *
 *  \context      ANY
 *
 *  \reentrant    TRUE for different objects.
 *  \synchronous  TRUE
 *
 *  \pre          See Os_Ioc08Receive().
 **********************************************************************************************************************/
Std_ReturnType     Os_TrapIoc08Receive
(
  boolean IsTrapRequired,
  const   Os_Ioc08ReceiveConfigType OS_CONST * Ioc,
    uint8   * Data
);


/***********************************************************************************************************************
 *  Os_TrapIoc08EmptyQueue()
 **********************************************************************************************************************/
/*! \brief        Empties the queue from the receiver side.
 *  \details      --no details--
 *
 *  \param[in]      IsTrapRequired  Defines whether a trap is required:
 *                                   - TRUE:  Trap is always performed.
 *                                   - FALSE: Trap is performed, if supervisor mode is required by platform.
 *  \param[in,out]  Ioc             See Os_Ioc08EmptyQueue().
 *
 *  \return       See Os_Ioc08EmptyQueue().
 *
 *  \context      ANY
 *
 *  \reentrant    TRUE for different objects.
 *  \synchronous  TRUE
 *
 *  \pre          See Os_Ioc08EmptyQueue().
 **********************************************************************************************************************/
Std_ReturnType     Os_TrapIoc08EmptyQueue
(
  boolean IsTrapRequired,
  const   Os_Ioc08ReceiveConfigType OS_CONST * Ioc
);


/***********************************************************************************************************************
 *  Os_TrapIoc08Send()
 **********************************************************************************************************************/
/*! \brief        Writes one item to the IOC.
 *  \details      For further details see Os_Ioc08Send().
 *
 *  \param[in]      IsTrapRequired  Defines whether a trap is required:
 *                                   - TRUE:  Trap is always performed.
 *                                   - FALSE: Trap is performed, if supervisor mode is required by platform.
 *  \param[in,out]  Ioc             See Os_Ioc08Send().
 *  \param[in]      Data            See Os_Ioc08Send().
 *
 *  \return       See Os_Ioc08Send().
 *
 *  \context      ANY
 *
 *  \reentrant    TRUE for different objects.
 *  \synchronous  TRUE
 *
 *  \pre          See Os_Ioc08Send().
 **********************************************************************************************************************/
Std_ReturnType     Os_TrapIoc08Send
(
  boolean IsTrapRequired,
  const   Os_Ioc08SendConfigType OS_CONST * Ioc,
  uint8 Data
);


/***********************************************************************************************************************
 *  Os_TrapIoc16Receive()
 **********************************************************************************************************************/
/*! \brief        Reads one item from the IOC.
 *  \details      For further details see Os_Ioc16Receive().
 *
 *  \param[in]      IsTrapRequired  Defines whether a trap is required:
 *                                   - TRUE:  Trap is always performed.
 *                                   - FALSE: Trap is performed, if supervisor mode is required by platform.
 *  \param[in,out]  Ioc             See Os_Ioc16Receive().
 *  \param[out]     Data            See Os_Ioc16Receive().
 *
 *  \return       See Os_Ioc16Receive().
 *
 *  \context      ANY
 *
 *  \reentrant    TRUE for different objects.
 *  \synchronous  TRUE
 *
 *  \pre          See Os_Ioc16Receive().
 **********************************************************************************************************************/
Std_ReturnType     Os_TrapIoc16Receive
(
  boolean IsTrapRequired,
  const   Os_Ioc16ReceiveConfigType OS_CONST * Ioc,
    uint16   * Data
);


/***********************************************************************************************************************
 *  Os_TrapIoc16EmptyQueue()
 **********************************************************************************************************************/
/*! \brief        Empties the queue from the receiver side.
 *  \details      --no details--
 *
 *  \param[in]      IsTrapRequired  Defines whether a trap is required:
 *                                   - TRUE:  Trap is always performed.
 *                                   - FALSE: Trap is performed, if supervisor mode is required by platform.
 *  \param[in,out]  Ioc             See Os_Ioc16EmptyQueue().
 *
 *  \return       See Os_Ioc16EmptyQueue().
 *
 *  \context      ANY
 *
 *  \reentrant    TRUE for different objects.
 *  \synchronous  TRUE
 *
 *  \pre          See Os_Ioc16EmptyQueue().
 **********************************************************************************************************************/
Std_ReturnType     Os_TrapIoc16EmptyQueue
(
  boolean IsTrapRequired,
  const   Os_Ioc16ReceiveConfigType OS_CONST * Ioc
);


/***********************************************************************************************************************
 *  Os_TrapIoc16Send()
 **********************************************************************************************************************/
/*! \brief        Writes one item to the IOC.
 *  \details      For further details see Os_Ioc16Send().
 *
 *  \param[in]      IsTrapRequired  Defines whether a trap is required:
 *                                   - TRUE:  Trap is always performed.
 *                                   - FALSE: Trap is performed, if supervisor mode is required by platform.
 *  \param[in,out]  Ioc             See Os_Ioc16Send().
 *  \param[in]      Data            See Os_Ioc16Send().
 *
 *  \return       See Os_Ioc16Send().
 *
 *  \context      ANY
 *
 *  \reentrant    TRUE for different objects.
 *  \synchronous  TRUE
 *
 *  \pre          See Os_Ioc16Send().
 **********************************************************************************************************************/
Std_ReturnType     Os_TrapIoc16Send
(
  boolean IsTrapRequired,
  const   Os_Ioc16SendConfigType OS_CONST * Ioc,
  uint16 Data
);


/***********************************************************************************************************************
 *  Os_TrapIoc32Receive()
 **********************************************************************************************************************/
/*! \brief        Reads one item from the IOC.
 *  \details      For further details see Os_Ioc32Receive().
 *
 *  \param[in]      IsTrapRequired  Defines whether a trap is required:
 *                                   - TRUE:  Trap is always performed.
 *                                   - FALSE: Trap is performed, if supervisor mode is required by platform.
 *  \param[in,out]  Ioc             See Os_Ioc32Receive().
 *  \param[out]     Data            See Os_Ioc32Receive().
 *
 *  \return       See Os_Ioc32Receive().
 *
 *  \context      ANY
 *
 *  \reentrant    TRUE for different objects.
 *  \synchronous  TRUE
 *
 *  \pre          See Os_Ioc32Receive().
 **********************************************************************************************************************/
Std_ReturnType     Os_TrapIoc32Receive
(
  boolean IsTrapRequired,
  const   Os_Ioc32ReceiveConfigType OS_CONST * Ioc,
    uint32   * Data
);


/***********************************************************************************************************************
 *  Os_TrapIoc32EmptyQueue()
 **********************************************************************************************************************/
/*! \brief        Empties the queue from the receiver side.
 *  \details      --no details--
 *
 *  \param[in]      IsTrapRequired  Defines whether a trap is required:
 *                                   - TRUE:  Trap is always performed.
 *                                   - FALSE: Trap is performed, if supervisor mode is required by platform.
 *  \param[in,out]  Ioc             See Os_Ioc32EmptyQueue().
 *
 *  \return       See Os_Ioc32EmptyQueue().
 *
 *  \context      ANY
 *
 *  \reentrant    TRUE for different objects.
 *  \synchronous  TRUE
 *
 *  \pre          See Os_Ioc32EmptyQueue().
 **********************************************************************************************************************/
Std_ReturnType     Os_TrapIoc32EmptyQueue
(
  boolean IsTrapRequired,
  const   Os_Ioc32ReceiveConfigType OS_CONST * Ioc
);


/***********************************************************************************************************************
 *  Os_TrapIoc32Send()
 **********************************************************************************************************************/
/*! \brief        Writes one item to the IOC.
 *  \details      For further details see Os_Ioc32Send().
 *
 *  \param[in]      IsTrapRequired  Defines whether a trap is required:
 *                                   - TRUE:  Trap is always performed.
 *                                   - FALSE: Trap is performed, if supervisor mode is required by platform.
 *  \param[in,out]  Ioc             See Os_Ioc32Send().
 *  \param[in]      Data            See Os_Ioc32Send().
 *
 *  \return       See Os_Ioc32Send().
 *
 *  \context      ANY
 *
 *  \reentrant    TRUE for different objects.
 *  \synchronous  TRUE
 *
 *  \pre          See Os_Ioc32Send().
 **********************************************************************************************************************/
Std_ReturnType     Os_TrapIoc32Send
(
  boolean IsTrapRequired,
  const   Os_Ioc32SendConfigType OS_CONST * Ioc,
  uint32 Data
);


/***********************************************************************************************************************
 *  Os_TrapIocRefReceive()
 **********************************************************************************************************************/
/*! \brief        Reads one item from the IOC.
 *  \details      For further details see Os_IocRefReceive().
 *
 *  \param[in]      IsTrapRequired  Defines whether a trap is required:
 *                                   - TRUE:  Trap is always performed.
 *                                   - FALSE: Trap is performed, if supervisor mode is required by platform.
 *  \param[in,out]  Ioc             See Os_IocRefReceive().
 *  \param[out]     Data            See Os_IocRefReceive().
 *
 *  \return       See Os_IocRefReceive().
 *
 *  \context      ANY
 *
 *  \reentrant    TRUE for different objects.
 *  \synchronous  TRUE
 *
 *  \pre          See Os_IocRefReceive().
 **********************************************************************************************************************/
Std_ReturnType     Os_TrapIocRefReceive
(
  boolean IsTrapRequired,
  const   Os_IocRefReceiveConfigType OS_CONST * Ioc,
    void   * Data
);


/***********************************************************************************************************************
 *  Os_TrapIocRefEmptyQueue()
 **********************************************************************************************************************/
/*! \brief        Empties the queue from the receiver side.
 *  \details      --no details--
 *
 *  \param[in]      IsTrapRequired  Defines whether a trap is required:
 *                                   - TRUE:  Trap is always performed.
 *                                   - FALSE: Trap is performed, if supervisor mode is required by platform.
 *  \param[in,out]  Ioc             See Os_IocRefEmptyQueue().
 *
 *  \return       See Os_IocRefEmptyQueue().
 *
 *  \context      ANY
 *
 *  \reentrant    TRUE for different objects.
 *  \synchronous  TRUE
 *
 *  \pre          See Os_IocRefEmptyQueue().
 **********************************************************************************************************************/
Std_ReturnType     Os_TrapIocRefEmptyQueue
(
  boolean IsTrapRequired,
  const   Os_IocRefReceiveConfigType OS_CONST * Ioc
);


/***********************************************************************************************************************
 *  Os_TrapIocRefSend()
 **********************************************************************************************************************/
/*! \brief        Writes one item to the IOC.
 *  \details      For further details see Os_IocRefSend().
 *
 *  \param[in]      IsTrapRequired  Defines whether a trap is required:
 *                                   - TRUE:  Trap is always performed.
 *                                   - FALSE: Trap is performed, if supervisor mode is required by platform.
 *  \param[in,out]  Ioc             See Os_IocRefSend().
 *  \param[in]      Data            See Os_IocRefSend().
 *
 *  \return       See Os_IocRefSend().
 *
 *  \context      ANY
 *
 *  \reentrant    TRUE for different objects.
 *  \synchronous  TRUE
 *
 *  \pre          See Os_IocRefSend().
 **********************************************************************************************************************/
Std_ReturnType     Os_TrapIocRefSend
(
  boolean IsTrapRequired,
  const   Os_IocRefSendConfigType OS_CONST * Ioc,
  const   void   * Data
);


/***********************************************************************************************************************
 *  Os_TrapIocRead()
 **********************************************************************************************************************/
/*! \brief        Reads one item from the IOC.
 *  \details      For further details see Os_IocRead().
 *
 *  \param[in]      IsTrapRequired  Defines whether a trap is required:
 *                                   - TRUE:  Trap is always performed.
 *                                   - FALSE: Trap is performed, if supervisor mode is required by platform.
 *  \param[in,out]  Ioc             See Os_IocRead().
 *  \param[out]     Data            See Os_IocRead().
 *
 *  \return       See Os_IocRead().
 *
 *  \context      ANY
 *
 *  \reentrant    TRUE for different objects.
 *  \synchronous  TRUE
 *
 *  \pre          See Os_IocRead().
 **********************************************************************************************************************/
Std_ReturnType     Os_TrapIocRead
(
  boolean IsTrapRequired,
  const   Os_IocReadConfigType OS_CONST * Ioc,
    void   * Data
);


/***********************************************************************************************************************
 *  Os_TrapIocWrite()
 **********************************************************************************************************************/
/*! \brief        Writes one item to the IOC.
 *  \details      For further details see Os_IocWrite().
 *
 *  \param[in]      IsTrapRequired  Defines whether a trap is required:
 *                                   - TRUE:  Trap is always performed.
 *                                   - FALSE: Trap is performed, if supervisor mode is required by platform.
 *  \param[in,out]  Ioc             See Os_IocWrite().
 *  \param[in]      Data            See Os_IocWrite().
 *
 *  \return       See Os_IocWrite().
 *
 *  \context      ANY
 *
 *  \reentrant    TRUE for different objects.
 *  \synchronous  TRUE
 *
 *  \pre          See Os_IocWrite().
 **********************************************************************************************************************/
Std_ReturnType     Os_TrapIocWrite
(
  boolean IsTrapRequired,
  const   Os_IocWriteConfigType OS_CONST * Ioc,
  const   void   * Data
);

/***********************************************************************************************************************
 *  Os_TrapIoc08MultiReceive()
 **********************************************************************************************************************/
/*! \brief        Reads one item from the next IOC channel.
 *  \details      For further details see Os_Ioc08MultiReceive().
 *
 *  \param[in]      IsTrapRequired  Defines whether a trap is required:
 *                                   - TRUE:  Trap is always performed.
 *                                   - FALSE: Trap is performed, if supervisor mode is required by platform.
 *  \param[in,out]  Ioc             See Os_Ioc08MultiReceive().
 *  \param[in]      Data            See Os_Ioc08MultiReceive().
 *
 *  \return       See Os_Ioc08MultiReceive().
 *
 *  \context      ANY
 *
 *  \reentrant    TRUE for different objects.
 *  \synchronous  TRUE
 *
 *  \pre          See Os_Ioc08MultiReceive().
 **********************************************************************************************************************/
Std_ReturnType     Os_TrapIoc08MultiReceive
(
  boolean IsTrapRequired,
  const   Os_Ioc08MultiReceiveConfigType OS_CONST * Ioc,
    uint8   * Data
);


/***********************************************************************************************************************
 *  Os_TrapIoc08EmptyMultiQueue()
 **********************************************************************************************************************/
/*! \brief        Empties the queues from the receiver side.
 *  \details      For further details see Os_Ioc08EmptyMultiQueue().
 *
 *  \param[in]      IsTrapRequired  Defines whether a trap is required:
 *                                   - TRUE:  Trap is always performed.
 *                                   - FALSE: Trap is performed, if supervisor mode is required by platform.
 *  \param[in,out]  Ioc             See Os_Ioc08EmptyMultiQueue().
 *  \param[in]      Data            See Os_Ioc08EmptyMultiQueue().
 *
 *  \return       See Os_Ioc08EmptyMultiQueue().
 *
 *  \context      ANY
 *
 *  \reentrant    TRUE for different objects.
 *  \synchronous  TRUE
 *
 *  \pre          See Os_Ioc08EmptyMultiQueue().
 **********************************************************************************************************************/
Std_ReturnType     Os_TrapIoc08EmptyMultiQueue
(
    boolean IsTrapRequired,
  const   Os_Ioc08MultiReceiveConfigType OS_CONST * Ioc
);


/***********************************************************************************************************************
 *  Os_TrapIoc16MultiReceive()
 **********************************************************************************************************************/
/*! \brief        Reads one item from the next IOC channel.
 *  \details      For further details see Os_Ioc16MultiReceive().
 *
 *  \param[in]      IsTrapRequired  Defines whether a trap is required:
 *                                   - TRUE:  Trap is always performed.
 *                                   - FALSE: Trap is performed, if supervisor mode is required by platform.
 *  \param[in,out]  Ioc             See Os_Ioc16MultiReceive().
 *  \param[in]      Data            See Os_Ioc16MultiReceive().
 *
 *  \return       See Os_Ioc16MultiReceive().
 *
 *  \context      ANY
 *
 *  \reentrant    TRUE for different objects.
 *  \synchronous  TRUE
 *
 *  \pre          See Os_Ioc16MultiReceive().
 **********************************************************************************************************************/
Std_ReturnType     Os_TrapIoc16MultiReceive
(
  boolean IsTrapRequired,
  const   Os_Ioc16MultiReceiveConfigType OS_CONST * Ioc,
    uint16   * Data
);


/***********************************************************************************************************************
 *  Os_TrapIoc16EmptyMultiQueue()
 **********************************************************************************************************************/
/*! \brief        Empties the queues from the receiver side.
 *  \details      For further details see Os_Ioc16EmptyMultiQueue().
 *
 *  \param[in]      IsTrapRequired  Defines whether a trap is required:
 *                                   - TRUE:  Trap is always performed.
 *                                   - FALSE: Trap is performed, if supervisor mode is required by platform.
 *  \param[in,out]  Ioc             See Os_Ioc16EmptyMultiQueue().
 *  \param[in]      Data            See Os_Ioc16EmptyMultiQueue().
 *
 *  \return       See Os_Ioc16EmptyMultiQueue().
 *
 *  \context      ANY
 *
 *  \reentrant    TRUE for different objects.
 *  \synchronous  TRUE
 *
 *  \pre          See Os_Ioc16EmptyMultiQueue().
 **********************************************************************************************************************/
Std_ReturnType     Os_TrapIoc16EmptyMultiQueue
(
  boolean IsTrapRequired,
  const   Os_Ioc16MultiReceiveConfigType OS_CONST * Ioc
);


/***********************************************************************************************************************
 *  Os_TrapIoc32MultiReceive()
 **********************************************************************************************************************/
/*! \brief        Reads one item from the next IOC channel.
 *  \details      For further details see Os_Ioc32MultiReceive().
 *
 *  \param[in]      IsTrapRequired  Defines whether a trap is required:
 *                                   - TRUE:  Trap is always performed.
 *                                   - FALSE: Trap is performed, if supervisor mode is required by platform.
 *  \param[in,out]  Ioc             See Os_Ioc32MultiReceive().
 *  \param[in]      Data            See Os_Ioc32MultiReceive().
 *
 *  \return       See Os_Ioc32MultiReceive().
 *
 *  \context      ANY
 *
 *  \reentrant    TRUE for different objects.
 *  \synchronous  TRUE
 *
 *  \pre          See Os_Ioc32MultiReceive().
 **********************************************************************************************************************/
Std_ReturnType     Os_TrapIoc32MultiReceive
(
  boolean IsTrapRequired,
  const   Os_Ioc32MultiReceiveConfigType OS_CONST * Ioc,
    uint32   * Data
);

/***********************************************************************************************************************
 *  Os_TrapIoc32EmptyMultiQueue()
 **********************************************************************************************************************/
/*! \brief        Empties the queues from the receiver side.
 *  \details      For further details see Os_Ioc32EmptyMultiQueue().
 *
 *  \param[in]      IsTrapRequired  Defines whether a trap is required:
 *                                   - TRUE:  Trap is always performed.
 *                                   - FALSE: Trap is performed, if supervisor mode is required by platform.
 *  \param[in,out]  Ioc             See Os_Ioc32EmptyMultiQueue().
 *  \param[in]      Data            See Os_Ioc32EmptyMultiQueue().
 *
 *  \return       See Os_Ioc32EmptyMultiQueue().
 *
 *  \context      ANY
 *
 *  \reentrant    TRUE for different objects.
 *  \synchronous  TRUE
 *
 *  \pre          See Os_Ioc32EmptyMultiQueue().
 **********************************************************************************************************************/
Std_ReturnType     Os_TrapIoc32EmptyMultiQueue
(
  boolean IsTrapRequired,
  const   Os_Ioc32MultiReceiveConfigType OS_CONST * Ioc
);

/***********************************************************************************************************************
 *  Os_TrapIocRefMultiReceive()
 **********************************************************************************************************************/
/*! \brief        Reads one item from the next IOC channel.
 *  \details      For further details see Os_IocWrite().
 *
 *  \param[in]      IsTrapRequired  Defines whether a trap is required:
 *                                   - TRUE:  Trap is always performed.
 *                                   - FALSE: Trap is performed, if supervisor mode is required by platform.
 *  \param[in,out]  Ioc             See Os_IocRefMultiReceive().
 *  \param[in]      Data            See Os_IocRefMultiReceive().
 *
 *  \return       See Os_IocRefMultiReceive().
 *
 *  \context      ANY
 *
 *  \reentrant    TRUE for different objects.
 *  \synchronous  TRUE
 *
 *  \pre          See Os_IocRefMultiReceive().
 **********************************************************************************************************************/
Std_ReturnType     Os_TrapIocRefMultiReceive
(
  boolean IsTrapRequired,
  const   Os_IocRefMultiReceiveConfigType OS_CONST * Ioc,
    void   * Data
);


/***********************************************************************************************************************
 *  Os_TrapIocRefEmptyMultiQueue()
 **********************************************************************************************************************/
/*! \brief        Empties the queues from the receiver side.
 *  \details      For further details see Os_IocRefEmptyMultiQueue().
 *
 *  \param[in]      IsTrapRequired  Defines whether a trap is required:
 *                                   - TRUE:  Trap is always performed.
 *                                   - FALSE: Trap is performed, if supervisor mode is required by platform.
 *  \param[in,out]  Ioc             See Os_IocRefEmptyMultiQueue().
 *
 *  \return       See Os_IocRefEmptyMultiQueue().
 *
 *  \context      ANY
 *
 *  \reentrant    TRUE for different objects.
 *  \synchronous  TRUE
 *
 *  \pre          See Os_IocRefEmptyMultiQueue().
 **********************************************************************************************************************/
Std_ReturnType     Os_TrapIocRefEmptyMultiQueue
(
  boolean IsTrapRequired,
  const   Os_IocRefMultiReceiveConfigType OS_CONST * Ioc
);


/***********************************************************************************************************************
 *  Os_TrapErrApplicationError()
 **********************************************************************************************************************/
/*! \brief        Behavior for application errors.
 *  \details      For further details see Os_ErrApplicationError().
 *
 *  \param[in]    ServiceId   See Os_ErrApplicationError().
 *  \param[in]    Status      See Os_ErrApplicationError().
 *  \param[in]    Parameters  See Os_ErrApplicationError().
 *
 *  \context      ANY
 *
 *  \reentrant    TRUE
 *  \synchronous  TRUE
 *
 *  \pre          See Os_ErrApplicationError().
 **********************************************************************************************************************/
void     Os_TrapErrApplicationError
(
  OSServiceIdType ServiceId,
  Os_StatusType Status,
    Os_ErrorParamsType OS_VAR_NOINIT * Parameters
);


/***********************************************************************************************************************
 *  Os_TrapServiceReturn()
 **********************************************************************************************************************/
/*! \brief        Returns to caller state before service call.
 *  \details      For further details see Os_ServiceReturn().
 *
 *  \context      OS_INTERNAL
 *
 *  \reentrant    TRUE
 *  \synchronous  TRUE
 *
 *  \pre          Calling function is Os_ServiceTfWrapper or Os_ServiceNtfWrapper.
 **********************************************************************************************************************/
OS_NORETURN void   Os_TrapServiceReturn (void);


/***********************************************************************************************************************
 *  Os_TrapTaskMissingTerminateTask()
 **********************************************************************************************************************/
/*! \brief        Behavior in case a task did forget to terminate and simply returns.
 *  \details      For further details see Os_TaskMissingTerminateTask().
 *
 *  \context      OS_INTERNAL
 *
 *  \reentrant    TRUE
 *  \synchronous  TRUE
 *
 *  \pre          See Os_TaskMissingTerminateTask().
 **********************************************************************************************************************/
OS_NORETURN void   Os_TrapTaskMissingTerminateTask (void);


/***********************************************************************************************************************
 *  Os_TrapHookReturn()
 **********************************************************************************************************************/
/*! \brief          Resume previous thread where the hook was called.
 *  \details        For further details see Os_HookReturn().
 *
 *  \context        OS_INTERNAL
 *
 *  \reentrant      TRUE
 *  \synchronous    TRUE
 *
 *  \pre            See Os_HookReturnFromStartupHook().
 **********************************************************************************************************************/
OS_NORETURN void   Os_TrapHookReturn (void);


/***********************************************************************************************************************
 *  Os_TrapCoreGetThread()
 **********************************************************************************************************************/
/*! \brief        Returns the current thread pointer of the local core.
 *  \details      For further details see Os_CoreGetThread().
 *
 *  \context      OS_INTERNAL
 *
 *  \reentrant    TRUE
 *  \synchronous  TRUE
 *
 *  \pre          See Os_CoreGetThread().
 **********************************************************************************************************************/
const OS_CONST Os_ThreadConfigType       * Os_TrapCoreGetThread
(
  void
);


/***********************************************************************************************************************
 *  Os_TrapIsrEpilogue()
 **********************************************************************************************************************/
/*! \brief        Epilogue for ISR user functions.
 *  \details      For further details see Os_IsrEpilogue().
 *
 *  \context      ISR2|OS_INTERNAL
 *
 *  \reentrant    TRUE
 *  \synchronous  TRUE
 *
 *  \pre            See Os_IsrEpilogue().
 **********************************************************************************************************************/
OS_NORETURN void   Os_TrapIsrEpilogue (void);


/***********************************************************************************************************************
 *  Os_TrapHandler()
 **********************************************************************************************************************/
/*! \brief        Handles trap requests and calls the appropriate APIs with supervisor access.
 *  \details      --no details--
 *
 *  \param[in,out]  Packet                The data packet which contains parameters for API.
 *                                        Parameter must not be NULL.
 *  \param[in,out]  CallerInterruptState  Interrupt state of the caller which shall be restored/modified.
 *                                        Parameter must not be NULL.
 *
 *  \context      ANY
 *
 *  \reentrant    TRUE for different objects.
 *  \synchronous  TRUE
 *
 *  \pre          Concurrent access is prevented by caller.
 *  \pre          Reference API parameters of the TrapPacket are valid.
 **********************************************************************************************************************/
void     Os_TrapHandler
(
    Os_TrapPacketType OS_VAR_NOINIT * Packet,
    Os_Hal_IntTrapStateType OS_VAR_NOINIT * CallerInterruptState
);


/***********************************************************************************************************************
 *  GLOBAL FUNCTIONS
 **********************************************************************************************************************/



# define OS_STOP_SEC_CODE
# include "Os_MemMap_OsCode.h"                                                                                          /* PRQA S 5087 */ /* MD_MSR_MemMap */

#endif /* OS_TRAPINT_H */

/* module specific MISRA deviations:
 */

/*!
 * \}
 */
/***********************************************************************************************************************
 *  END OF FILE: Os_TrapInt.h
 **********************************************************************************************************************/
