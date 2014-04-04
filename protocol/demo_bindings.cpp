#include "demo.pb.h"
namespace demo {
void BindPart(demo::Part* data)
{
	TwBar* bar = TwNewBar("demo.Part");
	// Add start
	TwAddVarCB(bar, "start", TW_TYPE_UINT32,
		[](const void* value, void* data) { ((demo::Part*)(data))->set_start(*(uint32_t*)value); },
		[](void* value, void* data) { *(uint32_t*)value = ((demo::Part*)(data))->start(); }, data, nullptr);

	// Add end
	TwAddVarCB(bar, "end", TW_TYPE_UINT32,
		[](const void* value, void* data) { ((demo::Part*)(data))->set_end(*(uint32_t*)value); },
		[](void* value, void* data) { *(uint32_t*)value = ((demo::Part*)(data))->end(); }, data, nullptr);

}
void BindConfig(demo::Config* data)
{
	TwBar* bar = TwNewBar("demo.Config");
}
}
