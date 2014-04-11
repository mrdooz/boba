#include "scene_test.hpp"
#include "graphics.hpp"
#include "deferred_context.hpp"
#include "app.hpp"
#include "demo_engine.hpp"
#include "proto_helpers.hpp"
#include "resource_manager.hpp"
#include "scene.hpp"

#include "boba_io.hpp"

using namespace boba;

namespace
{
  struct CBufferPerFrame
  {
    Matrix world;
    Matrix viewProj;
  };
}


Mesh mesh;

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
        mesh->_numVerts = e->numVerts;
        mesh->_numIndices = e->numIndices;

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
            e->numVerts * vertexSize * sizeof(float),
            false,
            buf.data(),
            vertexSize * sizeof(float));

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
bool LoadShadersFromFile(
    const string& filenameBase,
    GraphicsObjectHandle* vs,
    GraphicsObjectHandle* ps,
    GraphicsObjectHandle* inputLayout,
    u32 vertexFlags)
{
  vector<char> buf;
  if (vs)
  {
    if (!RESOURCE_MANAGER.LoadFile((filenameBase + ".vso").c_str(), &buf))
      return false;

    *vs = GRAPHICS.CreateVertexShader(buf, "VsMain");
    if (!vs->IsValid())
      return false;

    if (inputLayout)
    {
      vector<D3D11_INPUT_ELEMENT_DESC> desc;
      if (vertexFlags & Mesh::VF_POS)
      {
        D3D11_INPUT_ELEMENT_DESC element =
            {"POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0};
        desc.push_back(element);
      }

      if (vertexFlags & Mesh::VF_NORMAL)
      {
        D3D11_INPUT_ELEMENT_DESC element =
            { "NORMAL", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 };
        desc.push_back(element);
      }

      *inputLayout  = GRAPHICS.CreateInputLayout(desc, buf);
      if (!inputLayout->IsValid())
        return false;
    }
  }

  if (ps)
  {
    if (!RESOURCE_MANAGER.LoadFile((filenameBase + ".pso").c_str(), &buf))
      return false;

    *ps = GRAPHICS.CreatePixelShader(buf, "PsMain");
    if (!ps->IsValid())
      return false;
  }

  return true;
}


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
  u32 vertexFlags = Mesh::VF_POS | Mesh::VF_NORMAL;
  if (!loader.LoadMesh("mesh1", vertexFlags, &mesh))
    return false;

  if (!LoadShadersFromFile("shaders/simple_mesh", &_vs, &_ps, &mesh._layout, vertexFlags))
    return false;

  if (!GRAPHICS.LoadShadersFromFile("shaders/simple_mesh", &_vs, &_ps))
    return false;

  _configName = config;

  if (!LoadProto(config, &_config))
    return false;

//  BindConfig(&_config);

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
  Matrix view, proj;
  proj = Matrix::CreatePerspectiveFieldOfView(45.0f/180*3.1415f, 16.0f/10, 1, 1000);
  view = Matrix::CreateLookAt(
    Vector3(0, 0, -50),
    Vector3(0, 0, 0),
    Vector3(0, 1, 0));

  cb.world = Matrix::Identity();
  cb.viewProj = view * proj;

  _ctx->SetCBuffer(_cbuffer, &cb, sizeof(cb), ShaderType::VertexShader);

  _ctx->SetVS(_vs);
  _ctx->SetPS(_ps);
  _ctx->SetLayout(mesh._layout);
  _ctx->SetVB(mesh._vb);
  _ctx->SetIB(mesh._ib);
  _ctx->SetTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
  _ctx->DrawIndexed(mesh._numIndices, 0, 0);

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
