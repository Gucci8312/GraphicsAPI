#include "Product.h"

Product* Factory::Create(Dx11Device* _Device)
{
	Product* p = CreateProduct(_Device);
	//RegisterProduct(p);
	return p;
}

Product* Factory::Create(Dx12Device* _Device)
{
	Product* p = CreateProduct(_Device);
	//RegisterProduct(p);
	return p;
}
