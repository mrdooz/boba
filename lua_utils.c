#include "lua/lauxlib.h"
#include "lua_utils.h"

// metatable method for handling "array[index]"
static int array_index(lua_State* L) {
  int** parray = luaL_checkudata(L, 1, "array");
  int index = luaL_checkint(L, 2);
  lua_pushnumber(L, (*parray)[index - 1]);
  return 1;
}

// metatable method for handle "array[index] = value"
static int array_newindex(lua_State* L) {
  int** parray = luaL_checkudata(L, 1, "array");
  int index = luaL_checkint(L, 2);
  int value = luaL_checkint(L, 3);
  (*parray)[index - 1] = value;
  return 0;
}

// expose an array to lua, by storing it in a userdata with the array metatable
static int expose_array(lua_State* L, int array[]) {
  int** parray = lua_newuserdata(L, sizeof(int**));
  *parray = array;
  luaL_getmetatable(L, "array");
  lua_setmetatable(L, -2);
  return 1;
}

// test data
int mydata[] = { 1, 2, 3, 4 };

// test routine which exposes our test array to Lua 
static int getarray(lua_State* L) {
  return expose_array(L, mydata);
}

int __declspec(dllexport) __cdecl luaopen_array(lua_State* L) {

  static const struct luaL_Reg array[] = {
    { "__index", array_index },
    { "__newindex", array_newindex },
    NULL, NULL
  };

  // create a metatable for our array type
  luaL_newmetatable(L, "array");
  luaL_setfuncs(L, array, 0);

  // make our test routine available to Lua
  lua_register(L, "myarray", getarray);
  return 0;
}