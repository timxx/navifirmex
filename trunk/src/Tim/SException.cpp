/*
http://code.google.com/p/navifirmex/
Copyright (C) 2011  Just Fancy (Just_Fancy@live.com)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include <eh.h>

#include "SException.h"

using namespace _TIM;

SException::SException(EXCEPTION_POINTERS *pExp)
{
	_addr = pExp->ExceptionRecord->ExceptionAddress;
	_code = pExp->ExceptionRecord->ExceptionCode;

	exceptioninfo(_code);
}

void SException::install() throw()
{
	_set_se_translator(translator);
}

const char* SException::what() const throw()
{
	return _what;
}

const void* SException::where() const throw()
{
	return _addr;
}

unsigned int SException::code() const  throw()
{
	return _code;
}

void SException::translator(unsigned int code, EXCEPTION_POINTERS * pExp)
{
	throw SException(pExp);
}

void SException::exceptioninfo(unsigned int code)
{
	switch(code)
	{
	case EXCEPTION_ACCESS_VIOLATION:
		_what = "访问违规";
		break;

	case EXCEPTION_STACK_OVERFLOW:
		_what = "栈溢出";
		break;

	case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
		_what = "数组越界";
		break;

	case EXCEPTION_FLT_DIVIDE_BY_ZERO:
	case EXCEPTION_INT_DIVIDE_BY_ZERO:
		_what = "被除数为0";
		break;

	default:
		_what = "未列举";
	}
}