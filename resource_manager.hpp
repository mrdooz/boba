#pragma once

#include "graphics_object_handle.hpp"

namespace boba
{
#if WITH_UNPACKED_RESOUCES

  typedef function<bool (const string&, void *)> cbFileChanged;

  class ResourceManager
  {
  public:
    ResourceManager(const char *outputFilename);
    ~ResourceManager();

    static ResourceManager &Instance();
    static bool Create(const char *outputFilename);
    static bool Destroy();

    bool FileExists(const char *filename);
    __time64_t ModifiedDate(const char *filename);
    bool LoadFile(const char *filename, vector<char> *buf);
    bool LoadPartial(const char *filename, size_t ofs, size_t len, vector<char> *buf);
    bool LoadInplace(const char *filename, size_t ofs, size_t len, void *buf);
    GraphicsObjectHandle LoadTexture(
        const char* filename,
        const char* friendlyName = nullptr,
        bool srgb = false,
        D3DX11_IMAGE_INFO *info = nullptr);
    GraphicsObjectHandle LoadTextureFromMemory(
        const char* buf,
        size_t len,
        const char* friendlyName,
        bool srgb,
        D3DX11_IMAGE_INFO *info);

    void AddPath(const string &path);

    void AddFileWatch(
        const string& filename, 
        void* token,
        const cbFileChanged& cb,
        bool initial_callback,
        bool* initial_result);

    void RemoveFileWatch(const cbFileChanged &cb);

    void Tick();

  private:
    string ResolveFilename(const char *filename, bool fullPath);

    struct WatchedFile
    {
      string filename;
      time_t lastModification;
      vector<pair<void*, cbFileChanged>> callbacks;
    };

    ptime _lastTickTime;
    vector<string> _paths;
    unordered_map<string, string> _resolvedPaths;
    unordered_map<string, WatchedFile> _watchedFiles;

    string _outputFilename;

    struct FileInfo
    {
      FileInfo(const string &orgName, const string &resolvedName) 
        : orgName(orgName), resolvedName(resolvedName) {}

      bool operator<(const FileInfo &rhs) const
      {
        return make_pair(orgName, resolvedName) < make_pair(rhs.orgName, rhs.resolvedName);
      }
      string orgName;
      string resolvedName;
    };

    set<FileInfo> _readFiles;

  };
#define RESOURCE_MANAGER ResourceManager::Instance()

#else

  class PackedResourceManager
  {
  public:
    PackedResourceManager(const char *resourceFile);
    ~PackedResourceManager();

    static PackedResourceManager &instance();
    static bool create(const char *resourceFile);
    static bool close();

    bool load_file(const char *filename, vector<char> *buf);
    bool load_partial(const char *filename, size_t ofs, size_t len, vector<char> *buf);
    bool load_inplace(const char *filename, size_t ofs, size_t len, void *buf);
    GraphicsObjectHandle LoadTexture(const char *filename, const char *friendly_name, bool srgb, D3DX11_IMAGE_INFO *info);
    GraphicsObjectHandle LoadTextureFromMemory(const char *buf, size_t len, const char *friendly_name, bool srgb, D3DX11_IMAGE_INFO *info);

  private:

    bool loadPackedFile(const char *filename, vector<char> *buf);
    bool loadPackedInplace(const char *filename, size_t ofs, size_t len, void *buf);
    int hashLookup(const char *key);

    struct PackedFileInfo {
      int offset;
      int compressedSize;
      int finalSize;
    };

    vector<char> _fileBuffer;
    vector<int> _intermediateHash;
    vector<int> _finalHash;
    vector<PackedFileInfo> _fileInfo;

    static PackedResourceManager *_instance;
    string _resourceFile;
  };

#define RESOURCE_MANAGER PackedResourceManager::Instance()

#endif
}
