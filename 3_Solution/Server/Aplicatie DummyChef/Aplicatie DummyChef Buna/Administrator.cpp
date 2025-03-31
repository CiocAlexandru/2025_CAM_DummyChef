#include "Administrator.h"

Administrator* Administrator::getInstance()
{
    if (instance == nullptr)
        instance = new Administrator();
    return instance;
}
