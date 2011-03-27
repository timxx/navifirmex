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

#pragma once

#ifndef __SEH_EXCEPTION_H__
#define __SEH_EXCEPTION_H__

#include <Windows.h>
#include <exception>

#include "tim.h"

//注意编译选项
//Enable C++ Exception 选/EHa

_TIM_BEGIN

class SException : public std::exception
{
// public:
protected:
	SException(EXCEPTION_POINTERS *pExp) throw();

public:
	//每个线程调用一次即可
	static void install() throw();

	virtual const char*	what()  const throw();
	const void*			where() const throw();
	unsigned int		code()  const throw();

protected:
	static void translator(unsigned int code, EXCEPTION_POINTERS * pExp);

	void exceptioninfo(unsigned int code);
protected:
	const char *_what;
	const void	*_addr;
	unsigned int _code;
};

_TIM_END

#endif