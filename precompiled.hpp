#pragma once

#ifdef _DEBUG
#define WITH_UNPACKED_RESOUCES 1
#else
#endif

#include <stdint.h>
#include <assert.h>
#include <time.h>
#include <direct.h>
#include <sys/stat.h>
#include <io.h>

#include <atlbase.h>
#include <windows.h>
#include <windowsx.h>

#include <dxgi.h>
#include <dxgidebug.h>
#include <d3d11.h>
#include <D3DX11tex.h>

#include <vector>
#include <set>
#include <unordered_set>
#include <map>
#include <unordered_map>
#include <string>
#include <queue>
#include <functional>
#include <memory>

#include <google/protobuf/text_format.h>

#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/algorithm/string/replace.hpp>

#include "DirectXTK/SimpleMath.h"

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
  using std::set;
  using std::unordered_set;
  using std::map;
  using std::unordered_map;
  using std::deque;

  using std::pair;
  using std::make_pair;

  using std::function;

  using std::unique_ptr;

  using DirectX::SimpleMath::Vector2;
  using DirectX::SimpleMath::Vector3;
  using DirectX::SimpleMath::Vector4;
  using DirectX::SimpleMath::Matrix;

  using boost::posix_time::time_duration;
  using boost::posix_time::seconds;
  using boost::posix_time::milliseconds;
  using boost::posix_time::microseconds;

  using boost::algorithm::replace_all;

  using namespace std::tr1::placeholders;
}

#pragma comment(lib, "DXGI.lib")
#pragma comment(lib, "DXGUID.lib")
#pragma comment(lib, "D3D11.lib")
#pragma comment(lib, "D3DX11.lib")
#pragma comment(lib, "psapi.lib")
