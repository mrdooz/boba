#include "resource_manager.hpp"
#include "graphics.hpp"

using namespace boba;
using namespace bristol;

#if WITH_UNPACKED_RESOUCES

//------------------------------------------------------------------------------
static bool FileExists(const char *filename)
{
  if (_access(filename, 0) != 0)
    return false;

  struct _stat status;
  if (_stat(filename, &status) != 0)
    return false;

  return !!(status.st_mode & _S_IFREG);
}

//------------------------------------------------------------------------------
static string NormalizePath(const string &path, bool addTrailingSlash)
{
  string res(path);
  boost::replace_all(res, "\\", "/");
  if (addTrailingSlash) {
    if (!res.empty() && res.back() != '/')
      res.push_back('/');
  }
  return res;
}
//------------------------------------------------------------------------------

static ResourceManager *g_instance;

//------------------------------------------------------------------------------
ResourceManager &ResourceManager::Instance()
{
  return *g_instance;
}

//------------------------------------------------------------------------------
bool ResourceManager::Create(const char *outputFilename)
{
  g_instance = new ResourceManager(outputFilename);
  return true;
}

//------------------------------------------------------------------------------
bool ResourceManager::Destroy()
{
  delete exch_null(g_instance);
  return true;
}

//------------------------------------------------------------------------------
ResourceManager::ResourceManager(const char *outputFilename)
  : _outputFilename(outputFilename)
{
  _paths.push_back("./");
}

//------------------------------------------------------------------------------
ResourceManager::~ResourceManager()
{
  if (!_outputFilename.empty())
  {
    FILE *f = fopen(_outputFilename.c_str(), "wt");
    for (auto it = begin(_readFiles); it != end(_readFiles); ++it)
    {
      fprintf(f, "%s\t%s\n", it->orgName.c_str(), it->resolvedName.c_str());
    }
    fclose(f);
  }
}

//------------------------------------------------------------------------------
void ResourceManager::AddPath(const string& path)
{
  _paths.push_back(NormalizePath(path, true));
}

//------------------------------------------------------------------------------
bool ResourceManager::LoadFile(const char *filename, vector<char> *buf)
{
  const string& fullPath = ResolveFilename(filename, true);
  if (fullPath.empty())
    return false;
  _readFiles.insert(FileInfo(filename, fullPath));

  return bristol::LoadFile(fullPath.c_str(), buf);
}

//------------------------------------------------------------------------------
bool ResourceManager::LoadPartial(
    const char* filename,
    size_t ofs,
    size_t len,
    vector<char>* buf)
{
  buf->resize(len);
  return LoadInplace(filename, ofs, len, buf->data());
}

//------------------------------------------------------------------------------
bool ResourceManager::LoadInplace(
    const char* filename,
    size_t ofs,
    size_t len,
    void* buf)
{
  const string& fullPath = ResolveFilename(filename, true);
  _readFiles.insert(FileInfo(filename, fullPath));

  ScopedHandle h(CreateFileA(fullPath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 
      FILE_ATTRIBUTE_NORMAL, NULL));
  if (h.handle() == INVALID_HANDLE_VALUE)
    return false;

  DWORD size = GetFileSize(h, NULL);
  if (ofs + len > size)
    return false;

  DWORD res;
  if (SetFilePointer(h, ofs, 0, FILE_BEGIN) == INVALID_SET_FILE_POINTER) {
    return false;
  }

  if (!ReadFile(h, buf, len, &res, NULL)) {
    DWORD err = GetLastError();
    return false;
  }
  return true;
}

//------------------------------------------------------------------------------
bool ResourceManager::FileExists(const char *filename)
{
  return !ResolveFilename(filename, false).empty();
}

//------------------------------------------------------------------------------
__time64_t ResourceManager::ModifiedDate(const char *filename)
{
  struct _stat s;
  _stat(filename, &s);
  return s.st_mtime;
}

//------------------------------------------------------------------------------
string ResourceManager::ResolveFilename(const char* filename, bool fullPath)
{

  if (bristol::FileExists(filename))
  {
    if (fullPath)
    {
      char buf[MAX_PATH];
      GetFullPathNameA(filename, MAX_PATH, buf, NULL);
      return NormalizePath(buf, false);
    }
    else
    {
      return NormalizePath(filename, false);
    }
  }

  auto it = _resolvedPaths.find(filename);
  if (it != _resolvedPaths.end())
    return it->second;
  string res;
#if _DEBUG
  // warn about duplicates
  int count = 0;
  for (size_t i = 0; i < _paths.size(); ++i)
  {
    string cand(_paths[i] + filename);
    if (bristol::FileExists(cand.c_str()))
    {
      count++;
      if (res.empty())
        res = cand.c_str();
    }
  }
  if (count > 1)
  {
//    LOG_WARNING_LN("Multiple paths resolved for file: %s", filename);
  }
#else
  for (size_t i = 0; i < _paths.size(); ++i)
  {
    string cand(_paths[i] + filename);
    if (boba::FileExists(cand.c_str()))
    {
      res = cand.c_str();
      break;
    }
  }
#endif
  if (!res.empty())
  {
    res = NormalizePath(res.c_str(), false);
    _resolvedPaths[filename] = res;
  }
  return res;
}

//------------------------------------------------------------------------------
void ResourceManager::AddFileWatch(
    const string& filename,
    void* token,
    bool initialCallback,
    bool* initialResult,
    const cbFileChanged &cb)
{
  _fileWatcher.AddFileWatch(filename, token, initialCallback, initialResult, cb);
}

//------------------------------------------------------------------------------
void ResourceManager::RemoveFileWatch(const cbFileChanged &cb)
{
  _fileWatcher.RemoveFileWatch(cb);
}

//------------------------------------------------------------------------------
void ResourceManager::Tick()
{
  _fileWatcher.Tick();
}

//------------------------------------------------------------------------------
GraphicsObjectHandle ResourceManager::LoadTexture(
    const char* filename,
    const char* friendlyName,
    bool srgb,
    D3DX11_IMAGE_INFO* info)
{
  string fullPath = ResolveFilename(filename, true);
  _readFiles.insert(FileInfo(filename, fullPath));

  return GRAPHICS.LoadTexture(fullPath.c_str(), friendlyName, srgb, info);
}

//------------------------------------------------------------------------------
GraphicsObjectHandle ResourceManager::LoadTextureFromMemory(
    const char* buf,
    size_t len,
    const char* friendlyName,
    bool srgb,
    D3DX11_IMAGE_INFO* info)
{
  return GRAPHICS.LoadTextureFromMemory(buf, len, friendlyName, srgb, info);
}

#else

#include "lz4/lz4.h"

using namespace std::tr1::placeholders;
using namespace std;

static PackedResourceManager *g_instance;
static const int cMaxFileBufferSize = 16 * 1024 * 1024;

static uint32 FnvHash(uint32 d, const char *str) {
  if (d == 0)
    d = 0x01000193;

  while (true) {
    char c = *str++;
    if (!c)
      return d;
    d = ((d * 0x01000193) ^ c) & 0xffffffff;
  }
}

struct PackedHeader {
  int headerSize;
  int numFiles;
};

PackedResourceManager &PackedResourceManager::instance() {
  KASSERT(g_instance);
  return *g_instance;
}

bool PackedResourceManager::create(const char *outputFilename) {
  KASSERT(!g_instance);
  g_instance = new PackedResourceManager(outputFilename);
  return true;
}

bool PackedResourceManager::close() {
  delete exch_null(g_instance);
  return true;
}

PackedResourceManager::PackedResourceManager(const char *resourceFile) 
  : _resourceFile(resourceFile)
{
  FILE *f = fopen(resourceFile, "rb");
  LOG_ERROR_COND_LN(f, "Unable to open resource file: %s", resourceFile);
  DEFER([&]{ fclose(f); });

  PackedHeader header;
  LOG_ERROR_COND_LN(fread(&header, sizeof(header), 1, f) == 1, "Error reading packed header");

  // read the perfect hash tables
  _intermediateHash.resize(header.numFiles);
  _finalHash.resize(header.numFiles);

  LOG_ERROR_COND_LN(fread(&_intermediateHash[0], sizeof(int), header.numFiles, f) == header.numFiles, "Error reading hash");
  LOG_ERROR_COND_LN(fread(&_finalHash[0], sizeof(int), header.numFiles, f) == header.numFiles, "Error reading hash");

  _fileInfo.resize(header.numFiles);
  LOG_ERROR_COND_LN(fread(&_fileInfo[0], sizeof(PackedFileInfo), header.numFiles, f) == header.numFiles, "Error reading packed file info");

  int pos = ftell(f);
  fseek(f, 0, SEEK_END);
  int endPos = ftell(f);
  int dataSize = endPos - pos;
  fseek(f, pos, SEEK_SET);
  _fileBuffer.resize(dataSize);
  LOG_ERROR_COND_LN(fread(&_fileBuffer[0], 1, dataSize, f) == dataSize, "Error reading packed file data");
}

PackedResourceManager::~PackedResourceManager() {
}

int PackedResourceManager::hashLookup(const char *key) {
  int d = _intermediateHash[FnvHash(0, key) % _intermediateHash.size()];
  return d < 0 ? _finalHash[-d-1] : _finalHash[FnvHash(d, key) % _finalHash.size()];
}

bool PackedResourceManager::loadPackedFile(const char *filename, vector<char> *buf) {
  PackedFileInfo *p = &_fileInfo[hashLookup(filename)];
  buf->resize(p->finalSize);
  int res = LZ4_uncompress(&_fileBuffer[p->offset], buf->data(), p->finalSize);
  return res == p->compressedSize;
}

bool PackedResourceManager::loadPackedInplace(const char *filename, size_t ofs, size_t len, void *buf) {
  vector<char> tmp;
  if (!loadPackedFile(filename, &tmp))
    return false;

  memcpy(buf, tmp.data() + ofs, len);
  return true;
}

bool PackedResourceManager::load_file(const char *filename, vector<char> *buf) {
  return loadPackedFile(filename, buf);
}


bool PackedResourceManager::load_partial(const char *filename, size_t ofs, size_t len, vector<char> *buf) {
  // this is kinda cheesy..
  vector<char> tmp;
  if (!loadPackedFile(filename, &tmp))
    return false;
  buf->resize(len);
  copy(begin(tmp) + ofs, begin(tmp) + ofs + len, begin(*buf));
  return true;
}

bool PackedResourceManager::load_inplace(const char *filename, size_t ofs, size_t len, void *buf) {
  return loadPackedInplace(filename, ofs, len, buf);
}

GraphicsObjectHandle PackedResourceManager::LoadTexture(const char *filename, const char *friendly_name, bool srgb, D3DX11_IMAGE_INFO *info) {
  vector<char> tmp;
  loadPackedFile(filename, &tmp);
  return GRAPHICS.LoadTextureFromMemory(tmp.data(), tmp.size(), friendly_name, srgb, info);
}

GraphicsObjectHandle PackedResourceManager::LoadTextureFromMemory(
  const char *buf, size_t len, const char *friendly_name, bool srgb, D3DX11_IMAGE_INFO *info)
{
  return GRAPHICS.LoadTextureFromMemory(buf, len, friendly_name, srgb, info);
}

#endif
