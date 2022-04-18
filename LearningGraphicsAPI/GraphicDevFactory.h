#pragma once
#include <iostream>
#include <list>
#include "Product.h"
#include "Device.h"

class GraphicDevFactory :public Factory
{
private:
	//std::list<std::string> Devices;

public:
	GraphicDevFactory();
	~GraphicDevFactory();
	Product* CreateProduct(Dx11Device* Dev);
	Product* CreateProduct(Dx12Device* Dev);
	//void RegisterProduct(Product* Product);
};

