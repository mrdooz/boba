#include "scene_test.hpp"
#include "graphics.hpp"
#include "deferred_context.hpp"
#include "app.hpp"
#include "demo_engine.hpp"
#include "proto_helpers.hpp"
#include "resource_manager.hpp"

#include "boba_io.hpp"

using namespace boba;

namespace
{
  struct CBufferPerFrame
  {
    Vector4 col;
  };
}

struct Mesh
{
  enum VertexFlags
  {
    VF_POS    = 1 << 0,
    VF_NORMAL = 1 << 1,
    VF_UV     = 1 << 2,
  };

  string _name;
  GraphicsObjectHandle _vb;
  GraphicsObjectHandle _ib;
  GraphicsObjectHandle _layout;
};

struct Scene
{
  vector<Mesh> _meshes;
};

struct BobaLoader
{
  bool Load(const char* filename)
  {
    if (!LoadFile(filename, &buf))
      return false;

    const BobaScene* scene = (const BobaScene*)&buf[0];

    if (strncmp(scene->id, "boba", 4) != 0)
      return false;

    ProcessFixups(scene->fixupOffset);

    char* ptr;

    // add meshes
    ptr = &buf[scene->elementOffset[(u32)SceneElement::Mesh]];
    u32 numMeshes = *(u32*)ptr;
    MeshElement* element = (MeshElement*)(ptr + 4);
    for (u32 i = 0; i < numMeshes; ++i, ++element)
    {
      meshes.push_back(element);
    }
    return true;
  }

  bool LoadMesh(const string& name, u32 flags, Mesh* mesh)
  {
    for (const MeshElement* e : meshes)
    {
      if (e->name == name)
      {
        mesh->_name = name;

        // create a combined buffer for the vertex data
        bool hasPos = flags & Mesh::VF_POS && e->verts;
        bool hasNormal = flags & Mesh::VF_NORMAL && e->normals;
        bool hasUv = flags & Mesh::VF_UV && e->uv;

        u32 vertexSize = (hasPos ? 3 : 0) + (hasNormal ? 3 : 0) + (hasUv ? 2 : 0);

        vector<u8> buf(vertexSize * e->numVerts * sizeof(float));
        float* ptr = (float*)buf.data();

        // interleave the vertex data..
        u32 numVerts = e->numVerts;
        u32 ofs = 0;
        if (flags & Mesh::VF_POS)
        {
          for (u32 i = 0; i < numVerts; ++i)
            memcpy(&ptr[i*vertexSize], &e->verts[i*3], 3 * sizeof(float));
          ofs += 3;
        }

        if (flags & Mesh::VF_NORMAL)
        {
          for (u32 i = 0; i < numVerts; ++i)
            memcpy(&ptr[i*vertexSize + ofs], &e->normals[i*3], 3 * sizeof(float));
          ofs += 3;
        }

        if (flags & Mesh::VF_UV)
        {
          for (u32 i = 0; i < numVerts; ++i)
            memcpy(&ptr[i*vertexSize + ofs], &e->uv[i*2], 2 * sizeof(float));
          ofs += 2;
        }

        mesh->_vb = GRAPHICS.CreateBuffer(
            D3D11_BIND_VERTEX_BUFFER,
            e->numVerts * vertexSize,
            false,
            buf.data(),
            vertexSize);

        mesh->_ib = GRAPHICS.CreateBuffer(
            D3D11_BIND_INDEX_BUFFER,
            e->numIndices * sizeof(u32),
            false,
            e->indices,
            DXGI_FORMAT_R32_UINT);

        return true;
      }
    }

    return false;
  }

  void ProcessFixups(u32 fixupOffset)
  {
    // Process all the fixups. A list of locations that point to relative
    // data is stored (the fixup list), and for each of these locations, we
    // add the base of the file we loaded, converting the fixups to valid
    // memory locations
    u32* fixups = (u32*)&buf[fixupOffset];
    u32 numFixups = *fixups++;
    u32 base = (u32)&buf[0];
    for (u32 i = 0; i < numFixups; ++i)
    {
      u32 fixup = fixups[i];
      *(u32*)(base + fixup) += base;
    }
  }

  vector<MeshElement*> meshes;
  vector<char> buf;
};

//------------------------------------------------------------------------------
SceneTest::SceneTest(const string &name)
  : Effect(name)
{
}

//------------------------------------------------------------------------------
SceneTest::~SceneTest()
{
}

//------------------------------------------------------------------------------
bool SceneTest::Init(const char* config)
{
  BobaLoader loader;
  loader.Load("D:\\projects\\model\\thing1.lxo.boba");
  Mesh mesh;
  loader.LoadMesh("mesh1", Mesh::VF_POS | Mesh::VF_NORMAL, &mesh);
  _configName = config;

  if (!LoadProto(config, &_config))
    return false;

//  BindConfig(&_config);

  _texture = RESOURCE_MANAGER.LoadTexture("gfx/Abstract_BG_Texture2.jpg");
  if (!_texture.IsValid())
    return false;

  vector<char> buf;
  if (!RESOURCE_MANAGER.LoadFile("shaders/fullscreen.vso", &buf))
    return false;

  _vs = GRAPHICS.CreateVertexShader(buf, "VSQuad");
  if (!_vs.IsValid())
    return false;

  if (!RESOURCE_MANAGER.LoadFile("shaders/fullscreen.pso", &buf))
    return false;

  _ps = GRAPHICS.CreatePixelShader(buf, "PSQuad");
  if (!_ps.IsValid())
    return false;

  CD3D11_SAMPLER_DESC sampler = CD3D11_SAMPLER_DESC(CD3D11_DEFAULT());
  _samplerState = GRAPHICS.CreateSamplerState(sampler);
  if (!_samplerState.IsValid())
    return false;

  _cbuffer = GRAPHICS.CreateBuffer(D3D11_BIND_CONSTANT_BUFFER, sizeof(CBufferPerFrame), true);
  if (!_cbuffer.IsValid())
    return false;

  return true;
}

//------------------------------------------------------------------------------
bool SceneTest::Update(const UpdateState& state)
{
  return true;
}

//------------------------------------------------------------------------------
bool SceneTest::Render()
{
  _ctx->SetSwapChain(GRAPHICS.DefaultSwapChain(), true);
  _ctx->BeginFrame();

  CBufferPerFrame cb;
  FromProtocol(_config.bb_col4f(), &cb.col);
  _ctx->SetCBuffer(_cbuffer, &cb, sizeof(cb), ShaderType::PixelShader);

  _ctx->SetVS(_vs);
  _ctx->SetPS(_ps);
  _ctx->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
  _ctx->SetShaderResource(_texture, ShaderType::PixelShader);
  _ctx->SetSamplerState(_samplerState, ShaderType::PixelShader);
  _ctx->Draw(3,0);

  _ctx->EndFrame();

  return true;
}

//------------------------------------------------------------------------------
bool SceneTest::Close()
{
  return true;
}

//------------------------------------------------------------------------------
bool SceneTest::SaveSettings()
{
  if (FILE* f = fopen(_configName.c_str() ,"wt"))
  {
    fprintf(f, "%s", _config.DebugString().c_str());
    fclose(f);
  }
  return true;
}

//------------------------------------------------------------------------------
Effect* SceneTest::Create(const char* name)
{
  return new SceneTest(name);
}

//------------------------------------------------------------------------------
const char* SceneTest::Name()
{
  return "scene_test";
}
