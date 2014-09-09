///////////////////////////////////////////////////////////////////////////////
// syserror.cpp
// ~~~~~~~~~~~~
// Contains the system error wrapper implementation.
//
// Author:      Alexey Zhukov (alexz)
// Created:     23-Aug-2000   (alexz)
// Modified:    24-Aug-2000   (alexz)
//
// Copyright (C) 2000 Maris Multimedia Ltd. All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////
#include "stdafx.h"
#include <eh.h>                     // Win32: for _set_se_translator.
#include <cstdio>                   // for sprintf().

#include "syserror.h"

namespace
{
    //
    // Win32 structured to C++ exception translator.
    //
    void __cdecl translate(unsigned int in_code, EXCEPTION_POINTERS* in_ptr)
    {
        switch(in_code)
        {
            case EXCEPTION_ACCESS_VIOLATION:
                throw CDICommon::ErrAccessViolation(in_ptr);

            case EXCEPTION_IN_PAGE_ERROR:
                throw CDICommon::ErrPageFault(in_ptr);

            case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
                throw CDICommon::ErrArrayOutOfBounds(in_ptr);

            case EXCEPTION_FLT_DIVIDE_BY_ZERO:
                throw CDICommon::ErrDivideByZeroFlt(in_ptr);

            case EXCEPTION_INT_DIVIDE_BY_ZERO:
                throw CDICommon::ErrDivideByZeroInt(in_ptr);

            case EXCEPTION_STACK_OVERFLOW:
                throw CDICommon::ErrStackOverflow(in_ptr);

            case EXCEPTION_DATATYPE_MISALIGNMENT:
                throw CDICommon::Error(in_ptr, "Data is misaligned");

            case EXCEPTION_FLT_DENORMAL_OPERAND:
                throw CDICommon::Error(in_ptr, "Floating-point denormal operand");

            case EXCEPTION_FLT_INEXACT_RESULT:
                throw CDICommon::Error(in_ptr, "Floating-point inexact result");

            case EXCEPTION_FLT_INVALID_OPERATION:
                throw CDICommon::Error(in_ptr, "Floating-point invalid operation");

            case EXCEPTION_FLT_OVERFLOW:
                throw CDICommon::Error(in_ptr, "Floating-point overflow");

            case EXCEPTION_FLT_STACK_CHECK:
                throw CDICommon::Error(in_ptr, "Stack corruption as the result of a floating-point operation");

            case EXCEPTION_FLT_UNDERFLOW:
                throw CDICommon::Error(in_ptr, "Floating-point underflow");

            case EXCEPTION_ILLEGAL_INSTRUCTION:
                throw CDICommon::Error(in_ptr, "Illegal instruction");

            case EXCEPTION_INT_OVERFLOW:
                throw CDICommon::Error(in_ptr, "Integer overflow");

            case EXCEPTION_INVALID_DISPOSITION:
                throw CDICommon::Error(in_ptr, "Invalid disposition");

            case EXCEPTION_NONCONTINUABLE_EXCEPTION:
                throw CDICommon::Error(in_ptr, "Noncontinuable exception");

            case EXCEPTION_PRIV_INSTRUCTION:
                throw CDICommon::Error(in_ptr, "Operation is not allowed in the current machine mode");

            default:
                throw CDICommon::Error(in_ptr, "Unknown error");
        }
    }
}

namespace CDICommon
{
    //
    // Error implementation.
    //
    Error::Error(const void* in_info, const char* in_what) throw() :
                                                           m_what(0),
                                                           m_where(0),
                                                           m_code(0)
    {
        static char msg[256];

        const EXCEPTION_POINTERS* exPtr = static_cast<const EXCEPTION_POINTERS*>(in_info);
        const EXCEPTION_RECORD*   exRec = exPtr->ExceptionRecord;

        m_where = exRec->ExceptionAddress;
        m_code  = exRec->ExceptionCode;

        if (in_what)
        {
            sprintf(msg, "%s (0x%x).", in_what, m_code);
            m_what = msg;
        }
    }

    Error::~Error() throw()
    {
    }

    void Error::install() throw()
    {
        _set_se_translator(translate);
    }

    //
    // ErrAccessViolation implementation.
    //
    ErrAccessViolation::ErrAccessViolation(const void* in_info) throw() :
                                                                Error(in_info),
                                                                m_ptr(0),
                                                                m_isWrite(false)
    {
        static char msg[64];

        const EXCEPTION_POINTERS* exPtr = static_cast<const EXCEPTION_POINTERS*>(in_info);
        const EXCEPTION_RECORD*   exRec = exPtr->ExceptionRecord;

        if (exRec->NumberParameters > 0)
        {
            m_isWrite = exRec->ExceptionInformation[0] != 0;
            m_ptr     = reinterpret_cast<const void*>(exRec->ExceptionInformation[1]);
			DWORD addr = (DWORD)exRec->ExceptionInformation[1];
            sprintf(msg,
                    "Access violation (0x%x): attempt to %s 0x%x.",
                    code(),
                    m_isWrite ? "write to" : "read from",
                    addr);
        }
        else
            sprintf(msg, "Access violation (0x%x).", code());

        m_what = msg;
    }


    //
    // ErrPageFault implementation.
    //
    ErrPageFault::ErrPageFault(const void* in_info) throw() : Error(in_info)
    {
        static char msg[64];

        sprintf(msg, "Page fault (0x%x).", code());

        m_what = msg;
    }

    //
    // ErrArrayOutOfBounds implementation.
    //
    ErrArrayOutOfBounds::ErrArrayOutOfBounds(const void* in_info) throw() :
                                                                 Error(in_info)
    {
        static char msg[64];

        sprintf(msg, "Array bounds exceeded (0x%x).", code());

        m_what = msg;
    }

    //
    // ErrDivideByZero implementation.
    //
    ErrDivideByZero::ErrDivideByZero(const void* in_info) throw() :
                                                          Error(in_info)
    {
        static char msg[64];

        sprintf(msg, "Divide by zero (0x%x).", code());

        m_what = msg;
    }

    //
    // ErrDivideByZeroFlt implementation.
    //
    ErrDivideByZeroFlt::ErrDivideByZeroFlt(const void* in_info) throw() :
                                                       ErrDivideByZero(in_info)
    {
        static char msg[64];

        sprintf(msg, "Divide a floating-point value by zero (0x%x).", code());

        m_what = msg;
    }

    //
    // ErrDivideByZeroInt implementation.
    //
    ErrDivideByZeroInt::ErrDivideByZeroInt(const void* in_info) throw() :
                                                       ErrDivideByZero(in_info)
    {
        static char msg[64];

        sprintf(msg, "Divide an integer value by zero (0x%x).", code());

        m_what = msg;
    }

    //
    // ErrStackOverflow implementation.
    //
    ErrStackOverflow::ErrStackOverflow(const void* in_info) throw() :
                                                            Error(in_info)
    {
        static char msg[64];

        sprintf(msg, "Stack overflow (0x%x).", code());

        m_what = msg;
    }
}
