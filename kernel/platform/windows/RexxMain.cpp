/*----------------------------------------------------------------------------*/
/*                                                                            */
/* Copyright (c) 1995, 2004 IBM Corporation. All rights reserved.             */
/* Copyright (c) 2005-2006 Rexx Language Association. All rights reserved.    */
/*                                                                            */
/* This program and the accompanying materials are made available under       */
/* the terms of the Common Public License v1.0 which accompanies this         */
/* distribution. A copy is also available at the following address:           */
/* http://www.oorexx.org/license.html                          */
/*                                                                            */
/* Redistribution and use in source and binary forms, with or                 */
/* without modification, are permitted provided that the following            */
/* conditions are met:                                                        */
/*                                                                            */
/* Redistributions of source code must retain the above copyright             */
/* notice, this list of conditions and the following disclaimer.              */
/* Redistributions in binary form must reproduce the above copyright          */
/* notice, this list of conditions and the following disclaimer in            */
/* the documentation and/or other materials provided with the distribution.   */
/*                                                                            */
/* Neither the name of Rexx Language Association nor the names                */
/* of its contributors may be used to endorse or promote products             */
/* derived from this software without specific prior written permission.      */
/*                                                                            */
/* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS        */
/* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT          */
/* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS          */
/* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT   */
/* OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,      */
/* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED   */
/* TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA,        */
/* OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY     */
/* OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING    */
/* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS         */
/* SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.               */
/*                                                                            */
/*----------------------------------------------------------------------------*/
/******************************************************************************/
/* REXX Win  Support                                            winmain.c     */
/*                                                                            */
/* Main interface to the REXX interpreter                                     */
/*                                                                            */
/******************************************************************************/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <signal.h>

#include "RexxCore.h"                    /* bring in global defines           */
#include "StringClass.hpp"
#include "BufferClass.hpp"
#include "MethodClass.hpp"
#include "RexxCode.hpp"
#include "ArrayClass.hpp"
#include "RexxNativeActivation.hpp"
#include "RexxActivation.hpp"
#include "RexxActivity.hpp"
#include "SourceFile.hpp"
#include "DirectoryClass.hpp"
#include "RexxVariableDictionary.hpp"
#include "IntegerClass.hpp"
#include "RexxNativeAPI.h"                      /* REXX interface/native method code */
#include "APIServiceTables.h"
#include "SubcommandAPI.h"
#include "RexxAPIManager.h"
#include "Interpreter.hpp"
#include "ProtectedObject.hpp"
#include "PointerClass.hpp"

#include <fcntl.h>
#include <io.h>

#ifdef TIMESLICE                       /* System Yielding function prototype*/
APIRET REXXENTRY RexxSetYield(process_id_t procid, thread_id_t threadid);
#endif /*timeslice*/


extern bool UseMessageLoop;  /* speciality for VAC++ */

const char * SysFileExtension(const char *);
RexxMethod *SysRestoreProgramBuffer(PRXSTRING, RexxString *);
RexxMethod *SysRestoreInstoreProgram(PRXSTRING, RexxString *);
void SysSaveProgramBuffer(PRXSTRING, RexxMethod *);
void SysSaveTranslatedProgram(const char *, RexxMethod *);
bool SearchFileName(const char *, char *);
extern bool RexxStartedByApplication;

extern "C" {
APIRET REXXENTRY RexxTranslateProgram(const char *, const char *, PRXSYSEXIT);
char *REXXENTRY RexxGetVersionInformation();
}

// SIGHANDLER * oldSigIntHandler, * oldSigBreakHandler;


#define TRANSLATE     0                /* performing a translation only     */
#define EXECUTE       1                /* translate and execute Rexx Proc.  */
#define CREATEMETHOD  2                /* translate and return method obj.  */
#define RUNMETHOD     3                /* run method object                 */
#define LOADMETHOD    4                /* unflatten data                    */
#define STOREMETHOD   5                /* flatten method object             */

typedef struct _RexxScriptInfo {       /* Control info used by various API's*/
  short runtype;
  const char *index;                   /* index to directory which contains */
                                       /* objects being kept for process.   */

  PRXSTRING ProgramBuffer;             /* Data Buffer                       */
  RexxMethod * *pmethod;               /* Method Object                     */
  RexxObject * *presult;               /* Result Object                     */
  // these changes add the capability to use RexxRunMethod with
  // a. parameters and b. exits
  // as fas as I could make out, this function has never(!) been used up to now...
  REXXOBJECT (__stdcall *func)(void*); // callback for converting arbitrary data into REXX data types (stored in a RexxArray)
  void *args;                          // arguments for callback. if func == NULL, this will be treated as a RexxArray
  PRXSYSEXIT exits;                    // Array of system exit names used analogous to RexxStart() exits...
} RexxScriptInfo;

typedef struct _ConditionData {
  wholenumber_t code;
  wholenumber_t rc;
  RXSTRING message;
  RXSTRING errortext;
  size_t  position;
  RXSTRING program;
} ConditionData;

typedef struct _RexxStartInfo {
  int   runtype;                       /* How source should be handled      */
  size_t     argcount;                 /* Number of args in arglist         */
  PCONSTRXSTRING  arglist;             /* Array of args                     */
  const char*programname;              /* REXX program to run               */
  PRXSTRING  instore;                  /* Instore array                     */
  const char*envname;                  /* Initial cmd environment           */
  int        calltype;                 /* How the program is called         */
  PRXSYSEXIT exits;                    /* Array of system exit names        */
  short *    retcode;                  /* Integer form of result            */
  PRXSTRING  result;                   /* Result returned from program      */
  const char*outputName;               /* compilation output file           */
} RexxStartInfo;

bool HandleException = true;           /* Global switch for Exception Handling */
extern CRITICAL_SECTION waitProtect;

void REXXENTRY RexxCreateDirectory(const char * dirname)
{

  RexxString  *index;
  RexxActivity *activity;              /* target activity                   */

                                       /* (will create one if necessary)    */
  activity = ActivityManager::getActivity();

                                       /* Create string object              */
  index = new_string(dirname);

                                       /* Create directory and hang of off  */
                                       /* local env. directory.             */
  ActivityManager::localEnvironment->put(new_directory(), index);
  ActivityManager::returnActivity(activity);       /* release the kernel semaphore      */

  return;
}
/*********************************************************************/
/* Function SearchPrecision:  Check the precision of the activity and*/
/* use it if possible                                                */
/*********************************************************************/
extern "C" {
void SearchPrecision(
  size_t   *precision)                 /* required precision         */
{
    *precision = Numerics::DEFAULT_DIGITS;   /* set default digit count    */

/* give me the numeric digits settings of the current actitity       */

    RexxActivity *activity = ActivityManager::findActivity();
    if (activity != OREF_NULL)
    {
        RexxActivation *activation = activity->getCurrentActivation();
        *precision = activation->digits();
    }
}
}

RexxActivity *RunActivity = NULL;

// this function can be used to retrieve the value of "top level" variables
// on exit (RXTER). for each variable name that is found a call to the passed-in
// function is made [a pair ((char*) varname,(RexxObject*) varvalue)].
// please note that this does not work for multi-threaded environments!
void WinGetVariables(void (__stdcall *f)(void*))
{
  void* args[2];
  RexxArray *result;

  if (RunActivity) {
    result = RunActivity->getCurrentActivation()->getAllLocalVariables();
    for (size_t i=result->size(); i>0; i--) {
        RexxVariable *variable = (RexxVariable *)result->get(i);
        args[0] = (void *) variable->getName()->getStringData();
        args[1] = (void*) variable->getResolvedValue();
        f(args);
    }
  }
  return;
}

RexxActivity *WinStore = NULL;

void WinEnterKernel(bool execute)
{
  if (execute) {
    RexxInitialize();
    ActivityManager::getActivity(); // create a "current" activity
  }

  RexxNativeActivation *newNativeAct = new RexxNativeActivation(ActivityManager::currentActivity, OREF_NULL);
  ActivityManager::currentActivity->push(newNativeAct); /* Push new nativeAct onto stack     */
  WinStore = ActivityManager::currentActivity;
}

void WinLeaveKernel(bool execute)
{
  if (ActivityManager::currentActivity == NULL)
    ActivityManager::currentActivity = WinStore;
  ActivityManager::currentActivity->pop(false);
  if (execute) {
    ActivityManager::returnActivity();
    RexxTerminate();
  }

}

/******************************************************************************/
/* Name:       RexxRemoveDirectory                                            */
/*                                                                            */
/* Arguments:  dirname - Name of directory object to be created.              */
/*                                                                            */
/* Notes: Remove directory object from local environment.                     */
/******************************************************************************/

void REXXENTRY RexxRemoveDirectory(const char *dirname)
{
                                       /* (will create one if necessary)    */
  ActivityManager::getActivity();

                                       /* Create string object              */
  RexxString *index = new_string(dirname);
                                       /* Remove directory from Local env.  */
  ActivityManager::localEnvironment->remove(index);
                                       /* release the kernel semaphore      */
  ActivityManager::returnActivity();
  return;
}

/******************************************************************************/
/* Name:       RexxDispose                                                    */
/*                                                                            */
/* Arguments:  dirname - Name of directory object to remove object from       */
/*             RexxObj - Object to be removed                                 */
/*                                                                            */
/* Notes: Remove object from the directory of saved objects.                  */
/*                                                                            */
/* Returned:   true - Object disposed ok                                      */
/*             false - Invalid REXX object                                    */
/******************************************************************************/

bool REXXENTRY RexxDispose(const char *dirname, RexxObject *RexxObj)
{
                                       /* Find an activity for this thread  */
                                       /* (will create one if necessary)    */
  ActivityManager::getActivity();

                                       /* Create string object              */
  RexxString *index = new_string(dirname);
                                       /* Get directory of locked objects   */
  RexxDirectory *locked_objects = (RexxDirectory *)ActivityManager::localEnvironment->at(index);
                                       /* Remove object                     */
  RexxObj = locked_objects->remove(new_string((const char *)&RexxObj, sizeof(RexxObject *)));
                                       /* release the kernel semaphore      */
  ActivityManager::returnActivity();
  if (RexxObj == TheNilObject)
    return false;
  else
    return true;
}

/******************************************************************************/
/* Name:       RexxResultString                                               */
/*                                                                            */
/* Arguments:  result - (Input) result object containing Result String.       */
/*             pResultBuffer - (Output) Result String.                        */
/*                                                                            */
/* Notes: Return resulting string from Result Object                          */
/*                                                                            */
/* Returned:   1 - General error                                              */
/*            -1 - Invalid Object                                             */
/******************************************************************************/

APIRET REXXENTRY RexxResultString(RexxObject * result, PRXSTRING pResultBuffer)
{
  APIRET rc = 0;

  ActivityManager::getActivity();
                                       /* force to a string value           */
  RexxString *string_result = result->stringValue();
  if (string_result != NULL) {                /* didn't convert?                   */
                                       /* get the result length             */
    stringsize_t length = string_result->getLength() +1;
                                       /* allocate a new RXSTRING buffer    */
    pResultBuffer->strptr = (char *)SysAllocateResultMemory(length);
    if (pResultBuffer->strptr) {       /* Got storage ok ?                  */

                                       /* yes, copy the data (including the */
                                       /* terminating null implied by the   */
                                       /* use of length + 1                 */
      memcpy(pResultBuffer->strptr, string_result->getStringData(), length);
                                       /* give the true data length         */
      pResultBuffer->strlength = length - 1;
    }
    else
    {
        rc = 1;
    }
  }
  else
  {
      rc = -1;
  }
  ActivityManager::returnActivity();       /* release the kernel semaphore      */
  return rc;
}

/******************************************************************************/
/* Name:       RexxCopyMethod                                                 */
/*                                                                            */
/* Arguments:  dirname - (Input) Name of directory object used for anchoring  */
/*                          objects that need to be kept around.              */
/*             method - (Input) method object to be copied.                   */
/*             pmethod - (Output) pointer to new method                       */
/*                                                                            */
/* Notes: Clone a method                                                      */
/*                                                                            */
/* Returned:   1 - General Error                                              */
/*            -1 - Invalid Object                                             */
/*                                                                            */
/******************************************************************************/

APIRET REXXENTRY RexxCopyMethod(const char *dirname, RexxObject * method, RexxObject * *pmethod)
{
  APIRET rc = 0;

                                       /* Find an activity for this thread  */
                                       /* (will create one if necessary)    */

  ActivityManager::getActivity();
  if (isOfClass(Method, method)) {      /* Make sure this is a method object */

    if ((*pmethod = (RexxMethod *)method->copy()) != OREF_NULL) {

                                       /* Need to keep around for process   */
                                       /* duration.                         */
      RexxDirectory *locked_objects = (RexxDirectory *)ActivityManager::localEnvironment->at(new_string(dirname));
      locked_objects->put(*pmethod, new_string((const char *)pmethod, sizeof(RexxObject *)));
    } else
      rc = 1;
  } else
    rc = -1;

  ActivityManager::returnActivity();       /* release the kernel semaphore      */
  return rc;
}

/******************************************************************************/
/* Name:       RexxValidObject                                                */
/*                                                                            */
/* Arguments:  dirname - (Input) Name of directory object used for anchoring  */
/*                          objects that need to be kept around.              */
/*             object - (Input) object to be validated                        */
/*                                                                            */
/* Notes: Make sure that the object specified is acnchored in the directory   */
/*        specified by dirname.                                               */
/*                                                                            */
/* Returned:   true - Valid living REXX OBJECT                                */
/*             false - Invalid REXX object                                    */
/*                                                                            */
/******************************************************************************/

bool REXXENTRY RexxValidObject(const char *dirname, RexxObject * object)
{
                                       /* Find an activity for this thread  */
                                       /* (will create one if necessary)    */
  ActivityManager::getActivity();

                                       /* Get directory of locked objects   */
  RexxDirectory *locked_objects = (RexxDirectory *)ActivityManager::localEnvironment->at(new_string(dirname));
                                       /* See if object exists              */
  object = locked_objects->at(new_string((const char *)&object, sizeof(RexxObject *)));
                                       /* release the kernel semaphore      */
  ActivityManager::returnActivity();
  if (object == OREF_NULL)             /* Was the object in the directory ? */
    return false;                      /* Invalid object                    */
  else
    return true;                       /* Valid object                      */
}



/******************************************************************************/
/* Name:       RexxMain                                                       */
/*                                                                            */
/* Arguments:  argcount - Number of args in arglist                           */
/*             arglist - Array of args (array of RXSTRINGs)                   */
/*             programname - REXX program to run                              */
/*             instore - Instore array (array of 2 RXSTRINGs)                 */
/*             envname - Initial cmd environment                              */
/*             calltype - How the program is called                           */
/*             exits - Array of system exit names (array of RXSTRINGs)        */
/*                                                                            */
/* Returned:   result - Result returned from program                          */
/*             rc - Return code from program                                  */
/*                                                                            */
/* Notes:  Primary path into Object REXX.  Makes sure Object REXX is up       */
/*   and runs the requested program.                                          */
/*                                                                            */
/*   Mainline (32-bit) path looks like this:                                  */
/*     RexxStart => RexxMain => server_RexxStart                              */
/*   Mainline (16-bit) path looks like this:                                  */
/*     REXXSAA => RexxStart32 => RexxStart => RexxMain => server_RexxStart    */
/******************************************************************************/
int APIENTRY RexxStart(
  size_t argcount,                     /* Number of args in arglist         */
  PCONSTRXSTRING arglist,              /* Array of args                     */
  const char *programname,             /* REXX program to run               */
  PRXSTRING instore,                   /* Instore array                     */
  const char *envname,                 /* Initial cmd environment           */
  int   calltype,                      /* How the program is called         */
  PRXSYSEXIT exits,                    /* Array of system exit names        */
  short * retcode,                     /* Integer form of result            */
  PRXSTRING result)                    /* Result returned from program      */

{

  int      rc;                         /* RexxStart return code             */
//  WinBeginExceptions                 /* Begin of exception handling       */

  RexxStartInfo RexxStartArguments;    /* entry argument information        */
  RexxObject *  tempArgument;          /* temporary argument item           */

                                       /* copy all of the arguments into    */
                                       /* the info control block, which is  */
                                       /* passed across the kernel boundary */
                                       /* into the real RexxStart method    */
                                       /* this is a real execution          */
  RexxStartArguments.runtype = EXECUTE;
  RexxStartArguments.argcount = argcount;
  RexxStartArguments.arglist = arglist;
  RexxStartArguments.programname = (char *) programname;
  RexxStartArguments.instore = instore;
  RexxStartArguments.envname = (char *) envname;
  RexxStartArguments.calltype = (short)calltype;
  RexxStartArguments.exits = exits;
  RexxStartArguments.retcode = retcode;
  RexxStartArguments.result = result;
  RexxStartArguments.outputName = NULL;


  /* Because of using the stand-alone runtime library or when using different compilers,
     the std-streams of the calling program and the REXX.DLL might be located at different
     addresses and therefore _file might be -1. If so, std-streams are reassigned to the
     file standard handles returned by the system */
  if ((stdin->_file == -1) && (GetFileType(GetStdHandle(STD_INPUT_HANDLE)) != FILE_TYPE_UNKNOWN))
      *stdin = *_fdopen(_open_osfhandle((intptr_t)GetStdHandle(STD_INPUT_HANDLE),_O_RDONLY), "r");
  if ((stdout->_file == -1) && (GetFileType(GetStdHandle(STD_OUTPUT_HANDLE)) != FILE_TYPE_UNKNOWN))
      *stdout = *_fdopen(_open_osfhandle((intptr_t)GetStdHandle(STD_OUTPUT_HANDLE),_O_APPEND), "a");
  if ((stderr->_file == -1) && (GetFileType(GetStdHandle(STD_ERROR_HANDLE)) != FILE_TYPE_UNKNOWN))
      *stderr = *_fdopen(_open_osfhandle((intptr_t)GetStdHandle(STD_ERROR_HANDLE),_O_APPEND), "a");

  RexxInitialize();                    /* Perform any needed inits          */

  ActivityManager::getActivity();     /* get a base activity under us      */
  {
      ProtectedObject resultObject;        /* dummy returned result             */
                                           /* wrap up the argument              */
      tempArgument = (RexxObject *)new_pointer(&RexxStartArguments);
                                           /* pass along to the real method     */
      rc = (int)ActivityManager::currentActivity->messageSend(ActivityManager::localServer, OREF_RUN_PROGRAM, 1, &tempArgument, resultObject);
  }
  ActivityManager::returnActivity();
  RexxTerminate();                     /* perform needed termination        */

//  WinEndExceptions                   /* End of Exception handling         */

  return -rc;                          /* return the error code (negated)   */
}

void CreateRexxCondData(
  RexxDirectory *conditionobj,         /* condition object                  */
  ConditionData *pRexxCondData)        /* returned condition data           */

{
  size_t length;
  RexxString *message;
  RexxString *errortext;
  RexxString *program;

  memset(pRexxCondData,0,sizeof(ConditionData));
  pRexxCondData->code = message_number((RexxString *)conditionobj->at(OREF_CODE));

  pRexxCondData->rc = message_number((RexxString *)conditionobj->at(OREF_RC));

  message = (RexxString *)conditionobj->at(OREF_NAME_MESSAGE);
  if ( (RexxObject*) message != ooRexxNil) {
                                       /* get the length                    */
    length = message->getLength() + 1;
                                       /* allocate a new RXSTRING buffer    */
    pRexxCondData->message.strptr = (char *)SysAllocateResultMemory(length);
                                       /* yes, copy the data (including the */
                                       /* terminating null implied by the   */
                                       /* use of length + 1                 */
    memcpy(pRexxCondData->message.strptr, message->getStringData(), length);
                                       /* give the true data length         */
    pRexxCondData->message.strlength = length - 1;
  }

  errortext = (RexxString *)conditionobj->at(OREF_ERRORTEXT);
                                       /* get the result length             */
  length = errortext->getLength() +1;
                                       /* allocate a new RXSTRING buffer    */
  pRexxCondData->errortext.strptr = (char *)SysAllocateResultMemory(length);
                                       /* yes, copy the data (including the */
                                       /* terminating null implied by the   */
                                       /* use of length + 1                 */
  memcpy(pRexxCondData->errortext.strptr, errortext->getStringData(), length);
                                       /* give the true data length         */
  pRexxCondData->errortext.strlength = length - 1;

  pRexxCondData->position = ((RexxInteger *)(conditionobj->at(OREF_POSITION)))->getValue();

  program = (RexxString *)conditionobj->at(OREF_PROGRAM);
                                       /* get the result length             */
  length = program->getLength() +1;
                                       /* allocate a new RXSTRING buffer    */
  pRexxCondData->program.strptr = (char *)SysAllocateResultMemory(length);
                                       /* yes, copy the data (including the */
                                       /* terminating null implied by the   */
                                       /* use of length + 1                 */
  memcpy(pRexxCondData->program.strptr, program->getStringData(), length);
                                       /* give the true data length         */
  pRexxCondData->program.strlength = length - 1;
}


/* functions for concurrency synchronization/termination */

void APIENTRY RexxWaitForTermination(void)
{
    // the Interpreter class does the heavy lifting here
    Interpreter::terminate();
}


APIRET APIENTRY RexxDidRexxTerminate(void)
{
   return Interpreter::isTerminated();
}


BOOL APIENTRY RexxSetProcessMessages(BOOL onoff)
{
   bool old;
   old = UseMessageLoop;
   UseMessageLoop = (onoff != 0);
   return old;
}


/******************************************************************************/
/* Name:       RexxCreateMethod                                               */
/*                                                                            */
/* Arguments:  dirname - (Input) Name of directory object used for anchoring  */
/*                          method object.                                    */
/*             sourceData - (Input) Buffer with Rexx source code              */
/*             pmethod - (Output) pointer to new method object                */
/*             pRexxCondData (Output) Pointer to condition data               */
/*                                                                            */
/* Notes: Create a Rexx Method Object                                         */
/*                                                                            */
/* Returned:   rc - Return code from program                                  */
/*                                                                            */
/******************************************************************************/

APIRET REXXENTRY RexxCreateMethod(
  const char *dirname,                 /* directory name to save new method */
  PRXSTRING sourceData,                /* Buffer with Rexx source code      */
  REXXOBJECT   *pmethod,               /* returned method object            */
  ConditionData *pRexxCondData)        /* returned condition data           */
{
  APIRET   rc;                         /* RexxStart return code             */
  RexxScriptInfo RexxScriptArgs;
  RexxObject *  tempArgument;          /* temporary argument item           */


                                       /* clear the RexxScriptArgs block    */
  memset((void *)&RexxScriptArgs, 0, sizeof(RexxScriptInfo));
  RexxScriptArgs.runtype = CREATEMETHOD;
                                       /* Create string object              */
  RexxScriptArgs.index = dirname;
  RexxScriptArgs.ProgramBuffer = sourceData;
  RexxScriptArgs.pmethod = (RexxMethod * *)pmethod;

  RexxInitialize();                    /* Perform any needed inits          */

  ActivityManager::getActivity();     /* get a base activity under us      */
                                       /* wrap up the argument              */
  tempArgument = (RexxObject *)new_pointer(&RexxScriptArgs);
  {
      ProtectedObject  resultObject;       /* dummy returned result             */
                                           /* pass along to the real method     */
      rc = (APIRET)ActivityManager::currentActivity->messageSend(ActivityManager::localServer, OREF_RUN_PROGRAM, 1, &tempArgument, resultObject);
  }

                                       /* if error, get condition data from */
                                       /* condition object.                 */
  if (rc && ActivityManager::currentActivity->getCurrentCondition() != OREF_NULL)
    CreateRexxCondData(ActivityManager::currentActivity->getCurrentCondition(), pRexxCondData);
  ActivityManager::returnActivity();
  RexxTerminate();                     /* perform needed termination        */
  return rc;                           /* return the error code             */
}

/******************************************************************************/
/* Name:       RexxRunMethod                                                  */
/*                                                                            */
/* Arguments:  dirname - (Input) Name of directory object used for anchoring  */
/*                          method object.                                    */
/*             method - (Input) Method object to run                          */
/*             args - (input) argument array (can be null)                    */
/*             f - (input) callback function (can be null)                    */
/*             exitlist - (input) exit list (can be null)                     */
/*             presult - (Output) Result object returned                      */
/*             pRexxCondData (Output) Pointer to condition data               */
/*                                                                            */
/* Notes: Run a REXX Method Object and return result object                   */
/*                                                                            */
/* Returned:   rc - Return code from program                                  */
/*                                                                            */
/******************************************************************************/
extern "C" {
APIRET REXXENTRY RexxRunMethod(
  const char * dirname,
  REXXOBJECT method,
  void * args,
  REXXOBJECT (__stdcall *f)(void*),
  PRXSYSEXIT exit_list,
  REXXOBJECT *presult,
  REXXOBJECT *securityManager,
  ConditionData *pRexxCondData)        /* returned condition data           */
{
  APIRET   rc;                         /* RexxStart return code             */
  RexxScriptInfo RexxScriptArgs;
  RexxObject *  tempArgument;          /* temporary argument item           */
  RexxActivity *tempActivity;
  RexxActivity *store;

                                       /* clear the RexxScriptArgs block    */
  memset((void *)&RexxScriptArgs, 0, sizeof(RexxScriptInfo));

  RexxScriptArgs.runtype = RUNMETHOD;
                                       /* Create string object              */
  RexxScriptArgs.index = dirname;
  RexxScriptArgs.pmethod = (RexxMethod * *)&method;
  RexxScriptArgs.presult = (RexxObject **)presult;
  RexxScriptArgs.args = args;
  RexxScriptArgs.func = f;
  RexxScriptArgs.exits = exit_list;

  RexxInitialize();                    /* Perform any needed inits          */

  if (securityManager) ((RexxMethod*) method)->setSecurityManager((RexxObject *)securityManager);
  tempActivity = ActivityManager::getActivity();     /* get a base activity under us      */
  store = RunActivity; // store old one
  RunActivity = tempActivity; // set to current
                                       /* wrap up the argument              */
  tempArgument = (RexxObject *)new_pointer(&RexxScriptArgs);
  {
      ProtectedObject  resultObject;       /* dummy returned result             */
                                           /* pass along to the real method     */
      rc = (APIRET)tempActivity->messageSend(ActivityManager::localServer, OREF_RUN_PROGRAM, 1, &tempArgument, resultObject);
  }

                                       /* if error, get condition data from */
                                       /* condition object.                 */
  if (rc && tempActivity->getCurrentCondition() != OREF_NULL)
    CreateRexxCondData(tempActivity->getCurrentCondition(), pRexxCondData);
  ActivityManager::returnActivity(tempActivity);
  RexxTerminate();                     /* perform needed termination        */
  RunActivity = store; // restore old RunActivity (NULL or from a nested call)
  return rc;                           /* return the error code             */
}
} // end extern "C"

/******************************************************************************/
/* Name:       RexxStoreMethod                                                */
/*                                                                            */
/* Arguments:  method - (Input) Method object to flatten                      */
/*             scriptData - (Output) RXSTRING containing flattened data       */
/*                                                                            */
/* Notes: Flatten a REXX method object                                        */
/*                                                                            */
/* Returned:   rc - Return code from program                                  */
/*                                                                            */
/******************************************************************************/

APIRET REXXENTRY RexxStoreMethod(REXXOBJECT method, PRXSTRING scriptData)

{
  APIRET   rc;                         /* RexxStart return code             */
  RexxScriptInfo RexxScriptArgs;
  RexxObject *  tempArgument;          /* temporary argument item           */

                                       /* clear the RexxScriptArgs block    */
  memset((void *)&RexxScriptArgs, 0, sizeof(RexxScriptInfo));
  RexxScriptArgs.runtype = STOREMETHOD;
  RexxScriptArgs.pmethod = (RexxMethod * *)&method;
  RexxScriptArgs.ProgramBuffer = scriptData;

  RexxInitialize();                    /* Perform any needed inits          */

  ActivityManager::getActivity();     /* get a base activity under us      */
                                       /* wrap up the argument              */
  tempArgument = (RexxObject *)new_pointer(&RexxScriptArgs);
  {
      ProtectedObject resultObject;        /* dummy returned result             */
                                           /* pass along to the real method     */
      rc = (APIRET)ActivityManager::currentActivity->messageSend(ActivityManager::localServer, OREF_RUN_PROGRAM, 1, &tempArgument, resultObject);
  }
  ActivityManager::returnActivity();
  RexxTerminate();                     /* perform needed termination        */
  return rc;                           /* return the error code             */
}

/******************************************************************************/
/* Name:       RexxLoadMethod                                                 */
/*                                                                            */
/* Arguments:  dirname - (Input) Name of directory object used for anchoring  */
/*                          method object.                                    */
/*             scriptData - (Input) Buffer containing flattened code          */
/*             pmethod - (Output) pointer to new method object                */
/*                                                                            */
/* Notes: Unflatten a REXX method from a RXSTRING into a method object.       */
/*                                                                            */
/* Returned:   rc - Return code from program                                  */
/*                                                                            */
/******************************************************************************/

APIRET REXXENTRY RexxLoadMethod(const char *dirname, PRXSTRING scriptData, REXXOBJECT *pmethod)

{
  APIRET   rc;                         /* RexxStart return code             */
  RexxScriptInfo RexxScriptArgs;
  RexxObject *  tempArgument;          /* temporary argument item           */

                                       /* clear the RexxScriptArgs block    */
  memset((void *)&RexxScriptArgs, 0, sizeof(RexxScriptInfo));
  RexxScriptArgs.runtype = LOADMETHOD;
                                       /* Create string object              */
  RexxScriptArgs.index = dirname;
  RexxScriptArgs.ProgramBuffer = scriptData;
  RexxScriptArgs.pmethod = (RexxMethod * *)pmethod;

  RexxInitialize();                    /* Perform any needed inits          */

  ActivityManager::getActivity();     /* get a base activity under us      */
                                       /* wrap up the argument              */
  tempArgument = (RexxObject *)new_pointer(&RexxScriptArgs);
  {
      ProtectedObject  resultObject;       /* dummy returned result             */
                                           /* pass along to the real method     */
      rc = (APIRET)ActivityManager::currentActivity->messageSend(ActivityManager::localServer, OREF_RUN_PROGRAM, 1, &tempArgument, resultObject);
  }
  ActivityManager::returnActivity();

  RexxTerminate();                     /* perform needed termination        */

  if (*RexxScriptArgs.pmethod == OREF_NULL)
    rc = 1;                            /* scriptData must have been corrupt */

  return rc;                           /* return the error code             */

}

/******************************************************************************/
/* Name:       RexxTranslateProgram                                           */
/*                                                                            */
/* Arguments:  inFile   - Input source program                                */
/*             outFile  - Output file name                                    */
/*             exits    - Pointer to system exits                             */
/*                                                                            */
/******************************************************************************/
APIRET REXXENTRY RexxTranslateProgram(
   const char     *inFile,             /* input source program              */
   const char     *outFile,            /* output file name                  */
   PRXSYSEXIT   exits)                 /* system exits                      */
{
  APIRET   rc;                         /* RexxStart return code             */
  RexxStartInfo RexxStartArguments;    /* entry argument information        */
  RexxObject *  tempArgument;          /* temporary argument item           */

                                       /* clear the RexxStart block         */
  memset((void *)&RexxStartArguments, 0, sizeof(RexxStartInfo));
                                       /* program name is the input file    */
  RexxStartArguments.programname = inFile;
                                       /* and pass along the output name    */
  RexxStartArguments.outputName = outFile;
                                       /* this is a translation step        */
  RexxStartArguments.runtype = TRANSLATE;

  RexxStartArguments.exits = exits;

  RexxInitialize();                    /* Perform any needed inits          */

  ActivityManager::getActivity();     /* get a base activity under us      */
                                       /* wrap up the argument              */
  tempArgument = (RexxObject *)new_pointer(&RexxStartArguments);
  {
      ProtectedObject resultObject;        /* dummy returned result             */
                                           /* pass along to the real method     */
      rc = (APIRET)ActivityManager::currentActivity->messageSend(ActivityManager::localServer, OREF_RUN_PROGRAM, 1, &tempArgument, resultObject);
  }
  ActivityManager::returnActivity();
  RexxTerminate();                     /* perform needed termination        */
  return rc;                           /* return the error code             */
}


#ifdef TIMESLICE
/******************************************************************************/
/* Name:       RexxSetYield                                                   */
/*                                                                            */
/* Arguments:  procid - Process id of target REXX procedure                   */
/*             threadid - Thread id of target REXX procedure                  */
/*                                                                            */
/* Returned:   rc - RXARI_OK (halt condition set)                             */
/*                  RXARI_NOT_FOUND (couldn't find threadid)                  */
/*                                                                            */
/* Notes:  activity_setyield -> activation_yield ->..->activity_relinquish    */
/*         Causes bits in top activation to be flipped which will cause       */
/*         a system yield via activity_relinquish.                            */
/*                                                                            */
/******************************************************************************/
APIRET REXXENTRY RexxSetYield(process_id_t procid, thread_id_t threadid)
{
  if (RexxQuery()) {                        /* Are we up?                     */
    if (ActivityManager::yieldActivity(threadid))    /* Set yield condition?           */
      return (RXARI_OK);                    /* Yes, return okay               */
    else
      return (RXARI_NOT_FOUND);             /* Couldn't find threadid         */
    }
  else
    return (RXARI_NOT_FOUND);               /* REXX not running, error...     */
}

#endif /* TIMESLICE*/

/******************************************************************************/
/* Name:       RexxSetHalt                                                    */
/*                                                                            */
/* Arguments:  procid - Process id of target REXX procedure                   */
/*             threadid - Thread id of target REXX procedure                  */
/*                                                                            */
/* Returned:   rc - RXARI_OK (halt condition set)                             */
/*                  RXARI_NOT_FOUND (couldn't find threadid)                  */
/*                                                                            */
/* Notes:      Sends request to the activity to flip on the halt flag in the  */
/*             target activation.                                             */
/******************************************************************************/
APIRET REXXENTRY RexxSetHalt(process_id_t procid, thread_id_t threadid)
{
  if (RexxQuery())
  {                        /* Are we up?                     */
     if (threadid == 0)
     {
         ActivityManager::haltAllActivities();
     }
     else
     {
         if (!ActivityManager::haltActivity(threadid, OREF_NULL))
         {
             return (RXARI_NOT_FOUND);             /* Couldn't find threadid         */
         }
     }
     return (RXARI_OK);
  }
  return RXARI_NOT_FOUND;     /* REXX not running, error...     */
}




/******************************************************************************/
/* Name:       InternSetResetTrace                                            */
/*                                                                            */
/* Arguments:  procid - Process id of target REXX procedure                   */
/*             threadid - Thread id of target REXX procedure                  */
/*             flag - set trace on (1) or off (0) ?                           */
/*                                                                            */
/* Returned:   rc - RXARI_OK (halt condition set)                             */
/*                  RXARI_NOT_FOUND (couldn't find threadid)                  */
/*                                                                            */
/* Note: by IH to reuse code in RexxSetTrace and RexxResetTrace              */
/*                                                                            */
/******************************************************************************/

APIRET InternSetResetTrace(process_id_t procid, thread_id_t threadid, bool flag)
{
    if (RexxQuery())
    {                        /* Are we up?                     */
       if (threadid == 0)
       {
           ActivityManager::traceAllActivities(flag);
       }
       else
       {
           if (!ActivityManager::setActivityTrace(threadid, flag))
           {
               return (RXARI_NOT_FOUND);             /* Couldn't find threadid         */
           }
       }
       return (RXARI_OK);
    }
    return RXARI_NOT_FOUND;     /* REXX not running, error...     */
}

/******************************************************************************/
/* Name:       RexxSetTrace                                                   */
/*                                                                            */
/* Arguments:  procid - Process id of target REXX procedure                   */
/*             threadid - Thread id of target REXX procedure                  */
/*                                                                            */
/* Returned:   rc - RXARI_OK (halt condition set)                             */
/*                  RXARI_NOT_FOUND (couldn't find threadid)                  */
/*                                                                            */
/* Notes:      Sends request to the activity to turn on interactive trace in  */
/*             the target activation.                                         */
/******************************************************************************/
APIRET REXXENTRY RexxSetTrace(process_id_t procid, thread_id_t threadid)
{
  return (InternSetResetTrace(procid, threadid, true));     /* 1 to set trace on */
}


/******************************************************************************/
/* Name:       RexxResetTrace                                                 */
/*                                                                            */
/* Arguments:  procid - Process id of target REXX procedure                   */
/*             threadid - Thread id of target REXX procedure                  */
/*                                                                            */
/* Returned:   rc - RXARI_OK (halt condition set)                             */
/*                  RXARI_NOT_FOUND (couldn't find threadid)                  */
/*                                                                            */
/* Notes:      Sends request to the activity to turn off interactive trace in */
/*             the target activation.                                         */
/******************************************************************************/
APIRET REXXENTRY RexxResetTrace(process_id_t procid, thread_id_t threadid)
{
  return (InternSetResetTrace(procid, threadid, false));  /* 0 to set trace off */
}

void translateSource(
   RexxString           * inputName,   /* input program name                */
   RexxNativeActivation * newNativeAct,/* base activation                   */
   const char           * outputName ) /* output file name                  */
/******************************************************************************/
/* Function:  Process instorage execution arguments                           */
/******************************************************************************/
{
  RexxString * fullName;               /* fully resolved input name         */
  RexxMethod * method;                 /* created method                    */
  char         name[CCHMAXPATH + 2];   /* temporary name buffer             */
  bool            fileFound;

  {
      UnsafeBlock releaser;
                                           /* go resolve the name               */
      fileFound = SearchFileName(inputName->getStringData(), name);
  }

  if (!fileFound)
                                       /* got an error here                 */
    reportException(Error_Program_unreadable_notfound, inputName);

  fullName = new_string(name);        /* get as a string value             */
  newNativeAct->saveObject(fullName);  /* protect from garbage collect      */
                                       /* go translate the image            */
  method = TheMethodClass->newFile(fullName);
  if (outputName != NULL) {            /* want to save this to a file?      */
    newNativeAct->saveObject(method);  /* protect from garbage collect      */
                                       /* go save this method               */
    SysSaveTranslatedProgram(outputName, method);
  }
}

RexxMethod * process_instore(
   PRXSTRING    instore,               /* instore arguments                 */
   RexxString * name )                 /* program name                      */
/******************************************************************************/
/* Function:  Process instorage execution arguments                           */
/******************************************************************************/
{
  RexxMethod * method;                 /* returned method                   */
  RexxBuffer * source_buffer;          /* buffered source                   */
  unsigned short temp;                 /* unused position info              */
  RXSTRING buffer;                     /* instorage buffer                  */

  if (instore[0].strptr == NULL && instore[1].strptr == NULL) {
                                       /* see if this exists                */
    if (!RexxQueryMacro(name->getStringData(), (unsigned short *)&temp)) {
                                       /* get image of function             */
      RexxExecuteMacroFunction(name->getStringData(), &buffer);
                                       /* go convert into a method          */
      method = SysRestoreProgramBuffer(&buffer, name);

      /* On Windows we need to free the allocated buffer for the macro */
      if (buffer.strptr) GlobalFree(buffer.strptr);
      return method;
    }
    return OREF_NULL;                  /* temp scaffolding                  */
  }
  if (instore[1].strptr != NULL) {     /* have an image                     */
                                       /* go convert into a method          */
    method = SysRestoreProgramBuffer(&instore[1], name);
    if (method != OREF_NULL) {         /* did it unflatten successfully?    */
      if (instore[0].strptr != NULL) { /* have source also?                 */
                                       /* get a buffer object               */
        source_buffer = new_buffer(instore[0].strlength);
                                       /* copy source into the buffer       */
        memcpy(source_buffer->address(), instore[0].strptr, instore[0].strlength);
                                       /* reconnect this with the source    */
        ((RexxCode *)method)->getSourceObject()->setBufferedSource(source_buffer);
      }
      return method;                   /* go return it                      */
    }
  }
  if (instore[0].strptr != NULL) {     /* have instorage source             */
                                       /* check for a compiled program      */
    method = SysRestoreInstoreProgram(&instore[0], name);
    if (method == OREF_NULL) {         /* not precompiled?                  */
                                       /* get a buffer object               */
      source_buffer = new_buffer(instore[0].strlength);
                                       /* copy source into the buffer       */
      memcpy(source_buffer->address(), instore[0].strptr, instore[0].strlength);
                                       /* translate this source             */
      method = TheMethodClass->newRexxBuffer(name, source_buffer, (RexxClass *)TheNilObject);
                                       /* return this back in instore[1]    */
      SysSaveProgramBuffer(&instore[1], method);
    }
    return method;                     /* return translated source          */
  }
  return OREF_NULL;                    /* processing failed                 */
}

void CreateMethod(
  RexxScriptInfo *pRexxScriptArgs,
  RexxNativeActivation *newNativeAct)
{

 RexxBuffer *source_buffer;            /* buffered source                   */
 RexxString *name;                     /* input program name                */
 RexxDirectory *locked_objects;        /* directory used to keep objects    */
                                       /* around for process duration.      */

 name = OREF_NULLSTRING;               /* use an "unlocatable" name         */
 newNativeAct->saveObject(name);      /* protect from garbage collect      */
                                       /* get a buffer object               */
 source_buffer = new_buffer(pRexxScriptArgs->ProgramBuffer->strlength);
                                       /* copy source into the buffer       */
 memcpy(source_buffer->address(),
   pRexxScriptArgs->ProgramBuffer->strptr, pRexxScriptArgs->ProgramBuffer->strlength);
                                       /* translate this source             */
 *pRexxScriptArgs->pmethod = TheMethodClass->newRexxBuffer(name, source_buffer, (RexxClass *)TheNilObject);

 if (pRexxScriptArgs->index == OREF_NULL)
                                       /* protect from garbage collect      */
   newNativeAct->saveObject(*pRexxScriptArgs->pmethod);     /* protect from garbage collect      */
 else {
                                       /* Need to keep around for process   */
                                       /* duration.                         */
   locked_objects = (RexxDirectory *)ActivityManager::localEnvironment->at(new_string(pRexxScriptArgs->index));
   locked_objects->put(*pRexxScriptArgs->pmethod, new_string((const char *)pRexxScriptArgs->pmethod, sizeof(RexxObject *)));
 }
                                       /* finally, discard our activation   */
 ActivityManager::currentActivity->pop(false);
 return;
}

void RunMethod(
  RexxScriptInfo *pRexxScriptArgs,
  RexxNativeActivation *newNativeAct)
{
    RexxString *initial_address;         /* initial address setting           */
    RexxArray *new_arglist;              /* passed on argument list           */
    RexxActivity *savedAct;              /* store current activity */
    RexxDirectory *locked_objects;       /* directory used to keep objects    */
                                         /* around for process duration.      */
    size_t argcount=1;
    size_t arglength=0;
    const char *rexxargument="";
    const char *envname="CMD";           /* ADDRESS environment name          */
    int i;                               // for exit installation

    // callback activated?
    if (pRexxScriptArgs->func)
    {
        savedAct = ActivityManager::currentActivity; // save the activity...
        // note: imho this is "dirty". a process global variable (ActivityManager::currentActivity)
        // might (will) be overwritten, but to save the program flow (it will be
        // referenced again later on), i save it in a temporary variable...
        new_arglist = (RexxArray*) pRexxScriptArgs->func(pRexxScriptArgs->args);
        ActivityManager::currentActivity = savedAct; // restore activity
    }
    else if (pRexxScriptArgs->args)
    {
        // func == NULL && args != NULL => treat as RexxArray;
        new_arglist = (RexxArray*) pRexxScriptArgs->args;
        newNativeAct->saveObject(new_arglist);

    }
    // use dummy argument array
    else
    {
        /* get a new argument array          */
        new_arglist = new_array(argcount);   /* lock the argument list            */
        newNativeAct->saveObject(new_arglist);
        /* add to the argument array         */
        new_arglist->put(new_string(rexxargument, arglength), 1);
    }

    initial_address = new_string(envname);
    /* protect from garbage collect      */
    newNativeAct->saveObject(initial_address);

    /* install exits */
    if (pRexxScriptArgs->exits != NULL)  /* have exits to process             */
    {
        i = 0;                             /* start with first exit             */
                                           /* while not the list ender          */
        while (pRexxScriptArgs->exits[i].sysexit_code != RXENDLST)
        {
            /* enable this exit                  */
            ActivityManager::currentActivity->setExitHandler(pRexxScriptArgs->exits[i]);
            i++;                             /* step to the next exit             */
        }
    }

    /* Check to see if halt or trace sys */
    /* were set                          */
    ActivityManager::currentActivity->queryTrcHlt();
    /* run and get the result            */
    *pRexxScriptArgs->presult = (RexxString *)((RexxObject *)ActivityManager::currentActivity)->shriekRun(*pRexxScriptArgs->pmethod, OREF_COMMAND, initial_address, new_arglist->data(), new_arglist->size());
    if (pRexxScriptArgs->index != OREF_NULL && *pRexxScriptArgs->presult != OREF_NULL)
    {
        /* Need to keep around for process   */
        /* duration.                         */
        locked_objects = (RexxDirectory *)ActivityManager::localEnvironment->at(new_string(pRexxScriptArgs->index));
        locked_objects->put(*pRexxScriptArgs->presult, new_string((const char *)pRexxScriptArgs->presult, sizeof(RexxObject *)));
    }

    /* finally, discard our activation   */
    ActivityManager::currentActivity->pop(false);
    return;
}

void LoadMethod(
  RexxScriptInfo *pRexxScriptArgs,
  RexxNativeActivation *newNativeAct)
{

 RexxString *name;                     /* input program name                */
 RexxDirectory *locked_objects;        /* directory used to keep objects    */
                                       /* around for process duration.      */

 name = OREF_NULLSTRING;               /* use an "unlocatable" name         */
                                       /* protect from garbage collect      */
 newNativeAct->saveObject(name);
 *pRexxScriptArgs->pmethod =
    SysRestoreProgramBuffer(pRexxScriptArgs->ProgramBuffer, name);

 if (pRexxScriptArgs->index != OREF_NULL && *pRexxScriptArgs->pmethod != OREF_NULL) {
                                       /* Need to keep around for process   */
                                       /* duration.                         */
   locked_objects = (RexxDirectory *)ActivityManager::localEnvironment->at(new_string(pRexxScriptArgs->index));
   locked_objects->put(*pRexxScriptArgs->pmethod, new_string((const char *)pRexxScriptArgs->pmethod, sizeof(RexxObject *)));
 }

                                       /* finally, discard our activation   */
 ActivityManager::currentActivity->pop(false);
 return;
}
/*********************************************************************/
/*                                                                   */
/*   Function:  Do initial startup on a REXX program                 */
/*                                                                   */
/*********************************************************************/
void  SysRunProgram(
  void   *ControlInfo )                /* flattened control information     */
{
    RexxStartInfo *self;                 /* Rexxstart argument info           */
    RexxArray   * new_arglist;           /* passed on argument list           */
    size_t        i;                     /* loop counter                      */
    RexxString  * fullname;              /* fully resolved program name       */
    RexxString  * name;                  /* input program name                */
    RexxMethod  * method;                /* translated file image             */
    RexxString  * source_calltype;       /* parse source call type            */
    bool          tokenize_only;         /* don't actually execute program    */
    RexxString  * initial_address;       /* initial address setting           */
    const char  * file_extension;        /* potential file extension          */
    RexxString  * program_result;        /* returned program result           */
    RexxNativeActivation * newNativeAct; /* Native Activation to run on       */
    size_t        length;                /* return result length              */
    wholenumber_t return_code;           /* converted return code info        */

    tokenize_only = false;               /* default is to run the program     */
                                         /* create the native method to be run*/
                                         /* on the activity                   */
    newNativeAct = new RexxNativeActivation(ActivityManager::currentActivity, OREF_NULL);
    ActivityManager::currentActivity->push(newNativeAct); /* Push new nativeAct onto stack     */
    switch (*((short *)ControlInfo))
    {
        case CREATEMETHOD:
            CreateMethod((RexxScriptInfo *)ControlInfo, newNativeAct);
            return;
        case RUNMETHOD:
            RunMethod((RexxScriptInfo *)ControlInfo, newNativeAct);
            return;
        case STOREMETHOD:
            SysSaveProgramBuffer(((RexxScriptInfo *)ControlInfo)->ProgramBuffer,
                                 *(((RexxScriptInfo *)ControlInfo)->pmethod));


            /* finally, discard our activation   */
            ActivityManager::currentActivity->pop(false);
            return;
        case LOADMETHOD:
            LoadMethod((RexxScriptInfo *)ControlInfo, newNativeAct);
            return;
        default:
            break;
    }
    self = (RexxStartInfo *)ControlInfo; /* address all of the arguments      */
    if (self->programname != NULL)       /* have an actual name?              */
    {
        /* get string version of the name    */
        name = new_string(self->programname);
    }
    else
    {
        name = OREF_NULLSTRING;            /* use an "unlocatable" name         */
    }

    newNativeAct->saveObject(name);      /* protect from garbage collect      */

    if (self->exits != NULL)             /* have exits to process             */
    {
        i = 0;                           /* start with first exit             */
                                         /* while not the list ender          */
        while (self->exits[i].sysexit_code != RXENDLST)
        {
            /* enable this exit                  */
            ActivityManager::currentActivity->setExitHandler(self->exits[i]);
            i++;                           /* step to the next exit             */
        }
    }

    /* get a new argument array          */
    if (self->runtype==TRANSLATE)        /* just translating this?            */
    {
        /* just do the translation step      */
        translateSource(name, newNativeAct, self->outputName);
        return;                            /* and finished                      */
    }

    new_arglist = new_array(self->argcount);
    /* lock the argument list            */
    newNativeAct->saveObject(new_arglist);
    /* loop through the argument list    */
    for (i = 0; i < self->argcount; i++)
    {
        /* have a real argument?             */
        if (self->arglist[i].strptr != NULL)
        {
            /* add to the argument array         */
            new_arglist->put(new_string(self->arglist[i].strptr, self->arglist[i].strlength), i + 1);
        }
    }
    if (self->calltype == RXCOMMAND)     /* need to process command arg?      */
    {
        /* is there an argument?             */
        /* also check self->argcount to be a not null number         */
        if (self->argcount != 0 && self->arglist != NULL &&
            self->arglist[0].strptr != NULL && self->arglist[0].strlength > 1)
        {
            /* is there a leading blank?         */
            if (*(self->arglist[0].strptr) == ' ')
            {
                /* replace the first argument        */
                new_arglist->put(new_string(self->arglist[0].strptr+1, self->arglist[0].strlength - 1), 1);
            }
            /* have a "//T" in the argument?     */
            if ( (((RexxString *)(new_arglist->get(1)))->caselessPos(OREF_TOKENIZE_ONLY, 0) != 0) && RexxStartedByApplication)
            {
                tokenize_only = true;          /* don't execute this                */
            }
        }
        RexxStartedByApplication = true;
    }
    switch (self->calltype)              /* turn calltype into a string       */
    {
        case  RXCOMMAND:                   /* command invocation                */
            source_calltype = OREF_COMMAND;  /* this is the 'COMMAND' string      */
            break;

        case  RXFUNCTION:                  /* function invocation               */
            /* 'FUNCTION' string                 */
            source_calltype = OREF_FUNCTIONNAME;
            break;

        case  RXSUBROUTINE:                /* subroutine invocation             */
            /* 'SUBROUTINE' string               */
            source_calltype = OREF_SUBROUTINE;
            break;

        default:
            source_calltype = OREF_COMMAND;  /* this is the 'COMMAND' string      */
            break;
    }
    if (self->instore == NULL)           /* no instore request?               */
    {
        /* go resolve the name               */
        fullname = SysResolveProgramName(name, OREF_NULL);
        if (fullname == OREF_NULL)         /* not found?                        */
        {
            /* got an error here                 */
            reportException(Error_Program_unreadable_notfound, name);
        }
        newNativeAct->saveObject(fullname);/* protect from garbage collect      */
                                           /* try to restore saved image        */
        method = SysRestoreProgram(fullname);
        if (method == OREF_NULL)           /* unable to restore?                */
        {
            /* go translate the image            */
            method = TheMethodClass->newFile(fullname);
            newNativeAct->saveObject(method);/* protect from garbage collect      */
            SysSaveProgram(fullname, method);/* go save this method               */
        }
    }
    else                                 /* have an instore program           */
    {
        /* go handle instore parms           */
        method = process_instore(self->instore, name);
        if (method == OREF_NULL)           /* couldn't get it?                  */
        {
            /* got an error here                 */
            reportException(Error_Program_unreadable_name, name);
        }
        fullname = name;                   /* copy the name                     */
    }
    if (self->envname != NULL)           /* have an address override?         */
    {
        /* use the provided one              */
        initial_address = new_string(self->envname);

    }
    else
    {
        /* check for a file extension        */
        file_extension = SysFileExtension(fullname->getStringData());
        if (file_extension != NULL)      /* have a real one?                  */
        {
            /* use extension as the environment  */
            initial_address = new_string(file_extension + 1);
        }
        else
        {
            /* use system defined default        */
            initial_address = OREF_INITIALADDRESS;
        }
    }
    /* protect from garbage collect      */
    newNativeAct->saveObject(initial_address);
    /* actually need to run this?        */
    if (method != OREF_NULL && !tokenize_only)
    {
        /* Check to see if halt or trace sys */
        ActivityManager::currentActivity->queryTrcHlt();    /* were set                          */
        /* run and get the result            */
        program_result = (RexxString *)((RexxObject *)ActivityManager::currentActivity)->shriekRun(method, source_calltype, initial_address, new_arglist->data(), new_arglist->size());
        if (self->result != NULL)          /* if return provided for            */
        {
            /* actually have a result to return? */
            if (program_result != OREF_NULL)
            {
                /* force to a string value           */
                program_result = program_result->stringValue();
                /* get the result length             */
                length = (LONG)program_result->getLength() + 1;
                /* buffer too short or no return?    */
                if (length > self->result->strlength || self->result->strptr == NULL)
                {
                    /* allocate a new RXSTRING buffer    */
                    self->result->strptr = (char *)SysAllocateResultMemory(length);
                }
                /* yes, copy the data (including the */
                /* terminating null implied by the   */
                /* use of length + 1                 */
                memcpy(self->result->strptr, program_result->getStringData(), length);
                /* give the true data length         */
                self->result->strlength = length - 1;
            }
            else                             /* make this an invalid string       */
            {
                MAKERXSTRING(*(self->result), NULL, 0);
            }
        }

        if (self->retcode)                 /* asking for the binary return code?*/
        {
            *(self->retcode) = 0;            /* set default rc value              */
                                             /* If there is a return val...       */
            if (program_result != OREF_NULL)
            {
                /* if a whole number...              */
                if (program_result->numberValue(return_code) && return_code <= SHRT_MAX && return_code >= SHRT_MIN)
                {
                    /* ...copy to return code.           */
                    *(self->retcode) = (short)return_code;
                }
            }
        }
    }
    ActivityManager::currentActivity->pop(false);         /* finally, discard our activation   */
}


char *REXXENTRY RexxGetVersionInformation()
{
    char ver[20];
    sprintf( ver, " %d.%d.%d", ORX_VER, ORX_REL, ORX_MOD );
    char vbuf0[] = "Open Object Rexx %s Version";
  #ifdef _DEBUG
    char vbuf1[] = " - Internal Test Version\nBuild date: ";
  #else
    char vbuf1[] = "\nBuild date: ";
  #endif
    char vbuf2[] = "\nCopyright (c) IBM Corporation 1995, 2004.\nCopyright (c) RexxLA 2005-2007.\nAll Rights Reserved.";
    char vbuf3[] = "\nThis program and the accompanying materials";
    char vbuf4[] = "\nare made available under the terms of the Common Public License v1.0";
    char vbuf5[] = "\nwhich accompanies this distribution.";
    char vbuf6[] = "\nhttp://www.oorexx.org/license.html";
    size_t s0 = strlen(vbuf0);
    size_t s1 = strlen(vbuf1);
    size_t s2 = strlen(vbuf2);
    size_t s3 = strlen(vbuf3);
    size_t s4 = strlen(vbuf4);
    size_t s5 = strlen(vbuf5);
    size_t s6 = strlen(vbuf6);
    size_t sd = strlen(__DATE__);
    size_t sv = strlen(ver);
    char *ptr = (char *)GlobalAlloc(GMEM_FIXED, sv+s0+s1+s2+s3+s4+s5+s6+sd+1);
    if (ptr)
    {
        memcpy(ptr, vbuf0, s0);
        memcpy(ptr+s0, ver, sv);
        memcpy(ptr+s0+sv, vbuf1, s1);
        memcpy(ptr+s0+sv+s1, __DATE__, sd);
        memcpy(ptr+s0+sv+s1+sd, vbuf2, s2);
        memcpy(ptr+s0+sv+s1+sd+s2, vbuf3, s3);
        memcpy(ptr+s0+sv+s1+sd+s2+s3, vbuf4, s4);
        memcpy(ptr+s0+sv+s1+sd+s2+s3+s4, vbuf5, s5);
        memcpy(ptr+s0+sv+s1+sd+s2+s3+s4+s5, vbuf6, s6+1);
    }
    return ptr;
}
