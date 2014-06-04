#pragma once

#ifdef _DEBUG
#define WITH_UNPACKED_RESOUCES 1
#define WITH_ANT_TWEAK_BAR 1
#define WITH_PROTOBUF_TEXTFORMAT 1
#else
#define WITH_UNPACKED_RESOUCES 0
#define WITH_ANT_TWEAK_BAR 0
#define WITH_PROTOBUF_TEXTFORMAT 0
#endif

#if !defined(NOMINMAX)
#define NOMINMAX
#endif

#define WITH_FONT_RENDERING 1

#define WITH_MEMORY_TRACKING 0

#define WITH_GPU_PERF 1

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

#include <d3d9.h>

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

#define BOOST_ALL_NO_LIB
#include <boost/date_time.hpp>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/algorithm/string/replace.hpp>

#include "DirectXTK/SimpleMath.h"

#if WITH_ANT_TWEAK_BAR
#include <AntTweakBar.h>
#endif

#if WITH_FONT_RENDERING
#include <FW1FontWrapper.h>
#pragma comment(lib, "FW1FontWrapper.lib")
#endif

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

  using boost::posix_time::ptime;
  using boost::posix_time::time_duration;
  using boost::posix_time::seconds;
  using boost::posix_time::milliseconds;
  using boost::posix_time::microseconds;
  using boost::posix_time::microsec_clock;

  using boost::algorithm::replace_all;

  using namespace std::tr1::placeholders;
}

#pragma comment(lib, "DXGI.lib")
#pragma comment(lib, "DXGUID.lib")
#pragma comment(lib, "D3D11.lib")
#pragma comment(lib, "D3DX11.lib")
#pragma comment(lib, "psapi.lib")

#if WITH_GPU_PERF
#pragma comment(lib, "D3D9.lib")
#endif

#ifdef _DEBUG
#pragma comment(lib, "libboost_date_time-vc120-mt-sgd-1_55.lib")
#else
#pragma comment(lib, "libboost_date_time-vc120-mt-sg-1_55.lib")
#endif

#if WITH_ANT_TWEAK_BAR
#pragma comment(lib, "AntTweakBar.lib")
#endif
