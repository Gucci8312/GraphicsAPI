#include "GraphicDevFactory.h"

GraphicDevFactory::GraphicDevFactory()
{
}

GraphicDevFactory::~GraphicDevFactory()
{
}

Product* GraphicDevFactory::CreateProduct(Dx11Device* Dev)
{
	return new Device(Dev);
}

Product* GraphicDevFactory::CreateProduct(Dx12Device* Dev)
{
	return new Device(Dev);
}

//void GraphicDevFactory::RegisterProduct(Product* Product)
//{
//	Devices.push_back(dynamic_cast<Device*>(Product)->GetDevice());
//}
//
//std::list<std::string> GraphicDevFactory::GetDevices()
//{
//	return Devices;
//}
