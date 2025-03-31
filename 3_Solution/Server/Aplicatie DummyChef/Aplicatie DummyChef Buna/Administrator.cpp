#include "Administrator.h"

Administrator* Administrator::instance = nullptr;

Administrator* Administrator::getInstance()
{
    if (instance == nullptr)
        instance = new Administrator();
    return instance;
}
