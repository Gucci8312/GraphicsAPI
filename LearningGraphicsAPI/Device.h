#pragma once
#include <iostream>
#include "Product.h"




class Device :public Product
{
public:
	Device(Dx11Device* Device);
	Device(Dx12Device* Device);
	~Device();
	GraphicDevice* Use() final;
	//void* GetDevice();
private:
	//Dx11Device* m_Dx11Device;
	//Dx12Device* m_Dx12Device;
	GraphicDevice* m_Dev;
};

