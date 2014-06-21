/*----------------------------------------------------------------------------*/
/*                                                                            */
/* Copyright (c) 1995, 2004 IBM Corporation. All rights reserved.             */
/* Copyright (c) 2005-2014 Rexx Language Association. All rights reserved.    */
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
/* REXX Translator                                                            */
/*                                                                            */
/* Primitive Say Parse Class                                                  */
/*                                                                            */
/******************************************************************************/
#include <stdlib.h>
#include "RexxCore.h"
#include "StringClass.hpp"
#include "RexxActivity.hpp"
#include "RexxActivation.hpp"
#include "SayInstruction.hpp"

RexxInstructionSay::RexxInstructionSay(
    RexxObject *_expression)            /* assciated expression              */
/******************************************************************************/
/* Function:  Complete initialization of a SAY instruction                    */
/******************************************************************************/
{
   OrefSet(this, this->expression, _expression);
}

void  RexxInstructionSay::execute(
    RexxActivation      *context,      /* current activation context        */
    RexxExpressionStack *stack)        /* evaluation stack                  */
/******************************************************************************/
/* Function:  Execute a REXX SAY instruction                                  */
/******************************************************************************/
{
    RexxString *value;                   /* string version of output value    */

    context->traceInstruction(this);     /* trace if necessary                */
    if (this->expression != OREF_NULL) /* have an expression value?         */
    {
        /* get the expression value          */
        RexxObject *result = this->expression->evaluate(context, stack);
        value = REQUEST_STRING(result);    /* get the string version            */
    }
    else
    {
        /* use a NULL string                 */
        value = OREF_NULLSTRING;
    }
    context->traceResult(value);         /* trace the output value            */
                                         /* write out the line                */
    context->getActivity()->sayOutput(context, value);
    context->pauseInstruction();         /* do debug pause if necessary       */
}

