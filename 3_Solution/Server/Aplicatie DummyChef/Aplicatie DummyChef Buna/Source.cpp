#define _CRT_SECURE_NO_WARNINGS
#include <iostream>
#include "DummyChef.h"
#include "Administrator.h"



int main(array<System::String^>^ args)
{
    try 
    {

        Administrator* admin = Administrator::getInstance();
        DummyChef* aplicatie = DummyChef::getApp(admin);
        aplicatie->run();

    }
    catch (const std::exception& e)
    {
        std::cout << e.what() << "\n";
    }
    return 0;
}