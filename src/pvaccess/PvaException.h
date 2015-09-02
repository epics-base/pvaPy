// Copyright information and license terms for this software can be
// found in the file LICENSE that is included with the distribution

#ifndef PVA_EXCEPTION_H
#define PVA_EXCEPTION_H

#include <exception>
#include <iostream>
#include <string>
#include <cstdarg>

/**
 * Base exception class.
 */
class PvaException : public std::exception
{
public:
    static const int MaxMessageLength;
    static const int GenericErrorCode;
    static const char* PyExceptionClassName;

    PvaException(int errorCode, const std::string& message=""); 
    PvaException(const std::string& message=""); 
    PvaException(const char* message, ...); 
    PvaException(const char* message, va_list messageArgs);
    virtual ~PvaException() throw();
    virtual std::string getMessage() const;
    virtual void setMessage(const std::string& message);
    virtual const char* what() const throw();
    virtual int getErrorCode() const;
    virtual void setErrorCode(int errorCode);
    virtual const char* getPyExceptionClassName() const;
    friend std::ostream& operator<<(std::ostream& out, const PvaException& ex);

private:
    std::string error;
    int errorCode;
};

inline std::string PvaException::getMessage() const
{
    return error;
}

inline void PvaException::setMessage(const std::string& message)
{
    error = message;
}

inline const char* PvaException::what() const throw()
{
    return error.c_str();
}

inline int PvaException::getErrorCode() const
{
    return errorCode;
}

inline void PvaException::setErrorCode(int errorCode)
{
    this->errorCode = errorCode;
}

inline const char* PvaException::getPyExceptionClassName() const 
{
    return PvaException::PyExceptionClassName;
}


#endif 
