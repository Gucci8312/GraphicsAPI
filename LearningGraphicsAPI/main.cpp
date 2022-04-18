#include "main.h"

//int main()
//{
//	Application App(SCREEN_WIDTH,SCREEN_HEIGHT);
//
//	App.Run();
//
//	return 0;
//}

#include "Product.h"
#include "GraphicDevFactory.h"
#include "Device.h"

int main(void)
{
	Dx11Device Dev11;
	Dx12Device Dev12;
	Factory* Factory = new GraphicDevFactory();
	Product* GraphDev = Factory->Create(&Dev11);
	//GraphDev->Use()->GetDevice()

	getchar();
	return 0;
}