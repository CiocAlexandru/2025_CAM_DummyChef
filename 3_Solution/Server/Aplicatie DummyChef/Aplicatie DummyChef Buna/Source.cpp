#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include "DummyChef.h"
#include "Administrator.h"
#include "iExceptie.h"


int main(array<System::String^>^ args)
{
    try 
    {

        Administrator* admin = Administrator::getInstance();
        DummyChef* aplicatie = DummyChef::getApp(admin);
        aplicatie->run();

    }
    catch (iExceptie* exceptie)
    {
        exceptie->printInfo();
    }
    return 0;
}