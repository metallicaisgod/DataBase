///////////////////////////////////////////////////////////////////////////////
// syserror.h
// ~~~~~~~~~~
// Contains the system error wrapper definitions.
//
// Author:      Alexey Zhukov (alexz)
// Created:     23-Aug-2000   (alexz)
// Modified:    24-Aug-2000   (alexz)
//
// Copyright (C) 2000 Maris Multimedia Ltd. All Rights Reserved.
///////////////////////////////////////////////////////////////////////////////
#ifndef __syserror_h__
#define __syserror_h__

#include "cdicommon.h"

namespace CDICommon
{
    //
    // Base class for system errors.
    //
    class CDICOMMON_API Error
    {
    public:
        // This method must be called to install exception translator.
        static void install() throw();

        // Constructor.
        Error(const void* in_info, const char* in_what = 0) throw();

        // Destructor.
        virtual ~Error() throw();

        // Returns the error code.
        unsigned int code() const throw() { return (m_code); }

        // Returns a pointer to error description string.
        const char* what() const throw() { return (m_what); }

        // Returns a pointer to an address where the exception occured.
        const void* where() const throw() { return (m_where); }

    protected:
        const char*  m_what;

    private:
        const void*   m_where;
        unsigned int  m_code;
    };

    //
    // Several subclasses to allow the error type filtering within try/catch
    // block.
    //

    // Access violation.
    class ErrAccessViolation : public Error
    {
    public:
        ErrAccessViolation(const void* in_info) throw();

        // Returns a type of the exception (read/write).
        bool isWrite() const throw() { return (m_isWrite); }

        // Returns an address of the inaccessible data.
        const void* ptr() const throw() { return (m_ptr); }

    private:
        const void* m_ptr;
        bool        m_isWrite;
    };

    // Page fault.
    class ErrPageFault : public Error
    {
    public:
        ErrPageFault(const void* in_info) throw();
    };

    // Array bounds exceeded.
    class ErrArrayOutOfBounds : public Error
    {
    public:
        ErrArrayOutOfBounds(const void* in_info) throw();
    };

    // Divide by zero.
    class ErrDivideByZero : public Error
    {
    public:
        ErrDivideByZero(const void* in_info) throw();
    };

        class ErrDivideByZeroFlt : public ErrDivideByZero     // Floating-point.
        {
        public:
            ErrDivideByZeroFlt(const void* in_info) throw();
        };

        class ErrDivideByZeroInt : public ErrDivideByZero     // Integer.
        {
        public:
            ErrDivideByZeroInt(const void* in_info) throw();
        };

    // Stack overflow.
    class ErrStackOverflow : public Error
    {
    public:
        ErrStackOverflow(const void* in_info) throw();
    };
}

#endif // !__syserror_h__