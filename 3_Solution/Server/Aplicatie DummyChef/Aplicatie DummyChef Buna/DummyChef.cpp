#include "DummyChef.h"


DummyChef* DummyChef::app = nullptr;


DummyChef* DummyChef::getApp(Administrator* administrator)
{
	if (app == nullptr)
	{
		app = new DummyChef(administrator);
	}
	return app;
}

void DummyChef::destroyApp()
{
	if (app != nullptr)
	{
		delete app;
		app = nullptr;
	}
}

void DummyChef::menu()
{


}



