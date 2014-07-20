#pragma once

#define WITH_ANT_TWEAK_BAR 1
#define WITH_FONT_RENDERING 0
#define WITH_MEMORY_TRACKING 0
#define WITH_GPU_PERF 1

#define WITH_DXGI_DEBUG 0

#define WITH_CONFIG_DLG 0
#define WITH_UNPACKED_RESOUCES 1

#define WITH_PROTOBUF_TEXTFORMAT 1

#define WITH_DEBUG_SHADERS 0

#ifdef _DEBUG
  #ifndef WITH_UNPACKED_RESOUCES 
    #define WITH_UNPACKED_RESOUCES 1
  #endif
  #ifndef WITH_ANT_TWEAK_BAR
    #define WITH_ANT_TWEAK_BAR 1
  #endif
  #ifndef WITH_PROTOBUF_TEXTFORMAT
    #define WITH_PROTOBUF_TEXTFORMAT 1
  #endif
  #ifndef WITH_DEBUG_SHADERS
    #define WITH_DEBUG_SHADERS 1
  #endif
  #ifndef WITH_DXGI_DEBUG
    #define WITH_DXGI_DEBUG 1
  #endif
#else
  #ifndef WITH_UNPACKED_RESOUCES 
    #define WITH_UNPACKED_RESOUCES 0
  #endif
  #ifndef WITH_ANT_TWEAK_BAR
    #define WITH_ANT_TWEAK_BAR 0
  #endif
  #ifndef WITH_PROTOBUF_TEXTFORMAT
    #define WITH_PROTOBUF_TEXTFORMAT 0
  #endif
  #ifndef WITH_DEBUG_SHADERS
    #define WITH_DEBUG_SHADERS 0
  #endif
  #ifndef WITH_DXGI_DEBUG
    #define WITH_DXGI_DEBUG 0
  #endif
#endif

#if !defined(NOMINMAX)
#define NOMINMAX
#endif


#include <stdint.h>
#include <assert.h>
#include <time.h>
#include <direct.h>
#include <sys/stat.h>
#include <io.h>

#include <dxgi.h>
#include <dxgidebug.h>
#include <d3d11.h>
#include <D3DX11tex.h>
#include <DirectXMath.h>

#if WITH_GPU_PERF
#include <d3d9.h>
#define GPU_BeginEvent(col, wszName) D3DPERF_BeginEvent(col, wszName)
#define GPU_EndEvent() D3DPERF_EndEvent()
#define GPU_SetMarker(col, wszName) D3DPERF_SetMarker(col, wszName)
#define GPU_SetRegion(col, wszName) D3DPERF_SetRegion(col, wszName)
#else
#define GPU_BeginEvent(col, wszName)
#define GPU_EndEvent()
#define GPU_SetMarker(col, wszName)
#define GPU_SetRegion(col, wszName)
#endif

#include <atlbase.h>
#include <windows.h>
#include <windowsx.h>

#include <vector>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>
#include <string>
#include <queue>
#include <functional>
#include <memory>

#include "lua/lua.hpp"

#include <google/protobuf/text_format.h>
#include <google/protobuf/repeated_field.h>

#if WITH_ANT_TWEAK_BAR
#include <AntTweakBar.h>
#endif

#if WITH_FONT_RENDERING
#include <FW1FontWrapper.h>
#pragma comment(lib, "FW1FontWrapper.lib")
#endif

#include <bristol/string_utils.hpp>
#include <bristol/flags.hpp>
#include <bristol/utils.hpp>
#include <bristol/error.hpp>
#include <bristol/file_utils.hpp>
#include <bristol/time_utils.hpp>
#include <bristol/file_watcher.hpp>
#include <bristol/dx/graphics_utils.hpp>
#include <bristol/dx/vertex_types.hpp>


typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;

namespace boba
{
  using std::vector;
  using std::string;
  using std::wstring;
  using std::set;
  using std::unordered_set;
  using std::map;
  using std::unordered_map;
  using std::deque;
  using std::swap;
  using std::min;
  using std::max;

  using std::pair;
  using std::make_pair;

  using std::function;

  using std::unique_ptr;
  using std::make_unique;

  using DirectX::XMFLOAT3;
  using DirectX::BoundingSphere;
  using DirectX::SimpleMath::Color;
  using DirectX::SimpleMath::Vector2;
  using DirectX::SimpleMath::Vector3;
  using DirectX::SimpleMath::Vector4;
  using DirectX::SimpleMath::Matrix;

  using namespace std::tr1::placeholders;

  using bristol::Flags;
  using bristol::FileWatcher;

  using bristol::TimeStamp;
  using bristol::TimeDuration;
}

#pragma comment(lib, "DXGI.lib")
#pragma comment(lib, "DXGUID.lib")
#pragma comment(lib, "D3D11.lib")
#pragma comment(lib, "D3DX11.lib")
#pragma comment(lib, "psapi.lib")

#if WITH_GPU_PERF
#pragma comment(lib, "D3D9.lib")
#endif

#if WITH_ANT_TWEAK_BAR
#pragma comment(lib, "AntTweakBar.lib")
#endif
