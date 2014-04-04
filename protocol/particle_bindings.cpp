#include "particle.pb.h"
namespace particle {
void BindConfig(particle::Config* data)
{
	TwBar* bar = TwNewBar("particle.Config");
	// Add num_particles
	TwAddVarCB(bar, "num_particles", TW_TYPE_UINT32,
		[](const void* value, void* data) { ((particle::Config*)(data))->set_num_particles(*(uint32_t*)value); },
		[](void* value, void* data) { *(uint32_t*)value = ((particle::Config*)(data))->num_particles(); }, data, nullptr);

	// Add bb_col3f
	TwAddVarCB(bar, "bb_col3f", TW_TYPE_COLOR3F,
		[](const void* value, void* data) { memcpy(((particle::Config*)(data))->mutable_bb_col3f()->mutable_data(), value, 3 * sizeof(float)); },
		[](void* value, void* data) { memcpy(value, ((particle::Config*)(data))->bb_col3f().data(), 3 * sizeof(float)); }, data, nullptr);

	// Add bb_col4f
	TwAddVarCB(bar, "bb_col4f", TW_TYPE_COLOR4F,
		[](const void* value, void* data) { memcpy(((particle::Config*)(data))->mutable_bb_col4f()->mutable_data(), value, 4 * sizeof(float)); },
		[](void* value, void* data) { memcpy(value, ((particle::Config*)(data))->bb_col4f().data(), 4 * sizeof(float)); }, data, nullptr);

	// Add bb_dir3f
	TwAddVarCB(bar, "bb_dir3f", TW_TYPE_DIR3F,
		[](const void* value, void* data) { memcpy(((particle::Config*)(data))->mutable_bb_dir3f()->mutable_data(), value, 3 * sizeof(float)); },
		[](void* value, void* data) { memcpy(value, ((particle::Config*)(data))->bb_dir3f().data(), 3 * sizeof(float)); }, data, nullptr);

	// Add cc
	TwAddVarCB(bar, "cc", TW_TYPE_BOOLCPP,
		[](const void* value, void* data) { ((particle::Config*)(data))->set_cc(*(bool*)value); },
		[](void* value, void* data) { *(bool*)value = ((particle::Config*)(data))->cc(); }, data, nullptr);

}
}
