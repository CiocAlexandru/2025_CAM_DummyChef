#pragma once
#include "iExceptie.h"
#include<iostream>
#include<string>

class MyException
{
protected:
	int mCode;
	std::string mMessage;
public:
	MyException(int code=0,const char*message="") :mCode(code), mMessage(message) {}
	int getCode() { return mCode; }
	std::string getMessage()const { return mMessage; }
};
