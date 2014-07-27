#include "dynamic_mesh.hpp"
#include "graphics.hpp"
#include "app.hpp"

using namespace boba;

DynamicMesh::DynamicMesh()
{
  Reset();
}

void DynamicMesh::Init()
{
  CD3D11_RASTERIZER_DESC desc = CD3D11_RASTERIZER_DESC(CD3D11_DEFAULT());
  desc.FillMode = D3D11_FILL_SOLID;
  _solid = GRAPHICS.CreateRasterizerState(desc);
  desc.FillMode = D3D11_FILL_WIREFRAME;
  desc.CullMode = D3D11_CULL_NONE;
  _wireframe = GRAPHICS.CreateRasterizerState(desc);
}

void DynamicMesh::Reset()
{
  rotation = Matrix::Identity();
  translation = Vector3(0, 0, 0);

  indices.clear();
  verts.clear();
  faces.clear();
}

Matrix DynamicMesh::World()
{
  return rotation * Matrix::CreateTranslation(translation);
}

void DynamicMesh::CalcBoundingSphere()
{
  if (verts.empty())
    return;

  // find center
  size_t numVerts = verts.size() / 6;
  translation = Vector3(verts[0], verts[1], verts[2]);
  for (size_t i = 1*6; i < numVerts; ++i)
  {
    translation.x += verts[i * 6 + 0];
    translation.y += verts[i * 6 + 1];
    translation.z += verts[i * 6 + 2];
  }

  translation/= (float)numVerts;

  // center the verts
  for (size_t i = 0; i < numVerts; ++i)
  {
    verts[i * 6 + 0] -= translation.x;
    verts[i * 6 + 1] -= translation.y;
    verts[i * 6 + 2] -= translation.z;
  }

  // find max radius to center
  float r = Vector3::DistanceSquared(Vector3(0, 0, 0), Vector3(verts[0], verts[1], verts[2]));
  for (size_t i = 1*6; i < numVerts; ++i)
  {
    float cand = Vector3::DistanceSquared(Vector3(0, 0, 0), Vector3(verts[i * 6 + 0], verts[i * 6 + 1], verts[i * 6 + 2]));
    r = max(r, cand);
  }

  center = Vector3(0, 0, 0);
  radius = sqrt(r);
}

namespace boba
{
  // vertex[index]
  int VertexIndex(lua_State* L)
  {
    // note, indices are 0 bases
    DynamicMesh* mesh = *(DynamicMesh**)luaL_checkudata(L, 1, "vertices");
    int index = luaL_checkint(L, 2);
    luaL_argcheck(L, index >= 0 && index < (int)mesh->verts.size(), 2, "index out of range");
    lua_pushnumber(L, mesh->verts[index]);
    return 1;
  }

  // metatable method for handle "vertex[index] = value"
  int VertexNewIndex(lua_State* L)
  {
    DynamicMesh* mesh = *(DynamicMesh**)luaL_checkudata(L, 1, "vertices");
    int index = luaL_checkint(L, 2);
    float value = luaL_checknumber(L, 3);
    if (index >= (int)mesh->verts.size())
      mesh->verts.resize(index+1);
    mesh->verts[index] = value;
    return 0;
  }

  // face[index]
  int FaceIndex(lua_State* L)
  {
    // note, indices are 0 bases
    DynamicMesh* mesh = *(DynamicMesh**)luaL_checkudata(L, 1, "indices");
    int index = luaL_checkint(L, 2);
    luaL_argcheck(L, index >= 0 && index < (int)mesh->indices.size(), 2, "index out of range");
    lua_pushinteger(L, mesh->indices[index]);
    return 1;
  }

  // metatable method for handle "face[index] = value"
  int FaceNewIndex(lua_State* L)
  {
    DynamicMesh* mesh = *(DynamicMesh**)luaL_checkudata(L, 1, "indices");
    int index = luaL_checkint(L, 2);
    int value = luaL_checkint(L, 3);
    if (index >= (int)mesh->indices.size())
      mesh->indices.resize(index+1);
    mesh->indices[index] = value;
    return 0;
  }

  DynamicMesh* g_mesh = nullptr;

  void SetDynamicMesh(DynamicMesh* mesh)
  {
    g_mesh = mesh;
  }

  int GetMeshVertices(lua_State* L)
  {
    DynamicMesh** mesh = (DynamicMesh**)lua_newuserdata(L, sizeof(DynamicMesh**));
    *mesh = g_mesh;
    luaL_setmetatable(L, "vertices");
    return 1;
  }

  int GetMeshIndices(lua_State* L)
  {
    DynamicMesh** mesh = (DynamicMesh**)lua_newuserdata(L, sizeof(DynamicMesh**));
    *mesh = g_mesh;
    luaL_setmetatable(L, "indices");
    return 1;
  }

  int LuaPrint(lua_State* L)
  {
    int nargs = lua_gettop(L);

    for (int i=1; i <= nargs; i++)
    {
      if (lua_isstring(L, i))
      {
        const char* str = lua_tostring(L, i);
        APP.AddMessage(MessageType::Info, str);
      }
      else {

      }
    }

    return 0;
  }

  int __cdecl luaopen_meshlib(lua_State* L)
  {
    string filename("scripts/meshlib.lua");
    if (luaL_loadfile(L, filename.c_str()))
    {
      APP.AddMessage(MessageType::Error, "Error loading " + filename);
      return false;
    }
    else
    {
      APP.AddMessage(MessageType::Info, "Loaded " + filename);
    }

    lua_pcall(L, 0, 0, 0);

    return 1;
  }

  int __cdecl luaopen_mesh(lua_State* L)
  {
    static const struct luaL_Reg vertexFunctions[] ={
        { "__index", VertexIndex },
        { "__newindex", VertexNewIndex },
        NULL, NULL
    };

    static const struct luaL_Reg faceFunctions[] ={
        { "__index", FaceIndex },
        { "__newindex", FaceNewIndex },
        NULL, NULL
    };

    static const struct luaL_Reg printlib[] ={
        { "print", LuaPrint },
        { NULL, NULL } /* end of array */
    };

    // create a metatable for our mesh type
    luaL_newmetatable(L, "vertices");
    luaL_setfuncs(L, vertexFunctions, 0);

    luaL_newmetatable(L, "indices");
    luaL_setfuncs(L, faceFunctions, 0);

    // make our test routine available to Lua
    lua_register(L, "get_vertices", GetMeshVertices);
    lua_register(L, "get_indices", GetMeshIndices);

    // hook up our own print function
    lua_getglobal(L, "_G");
    luaL_setfuncs(L, printlib, 0);
    lua_pop(L, 1);

    return 1;
  }
}
