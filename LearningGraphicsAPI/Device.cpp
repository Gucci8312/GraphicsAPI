#include "Device.h"

Device::Device(Dx11Device* Device) 
{
	m_Dev = new Dx11Device;
	//m_Device = Device;

	std::cout << m_Dev->GetDevice() << "のデバイスを作ります。" << std::endl;
}

Device::Device(Dx12Device* Device)
{
	m_Dev = new Dx12Device;
	std::cout << m_Dev->GetDevice() << "のデバイスを作ります。" << std::endl;
}

Device::~Device() {}

GraphicDevice* Device::Use()
{
	return m_Dev;
}
