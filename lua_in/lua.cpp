#include <stdio.h>
#include <string>

extern "C" {
    #include <lua.h>
    #include <lauxlib.h>
    #include <lualib.h>
}

/*
    lua_pushnil(L);

    while( lua_next( L, -2 ) ) {
        if( lua_isnumber( L, -1 ) ) {
            int i = (int)lua_tonumber( L, -1 );
            //use number
        }
        lua_pop( L, 1 );
    }
    lua_pop( L, 1 );
*/

void PrintTable(lua_State *L)
{
    lua_pushnil(L);

    while(lua_next(L, -2) != 0)
    {
        if(lua_isstring(L, -1))
          printf("%s = %s\n", lua_tostring(L, -2), lua_tostring(L, -1));
        else if(lua_isnumber(L, -1))
          printf("%s = %d\n", lua_tostring(L, -2), lua_tonumber(L, -1));
        else if ( lua_isboolean( L, -1 ) )
            printf("%s = %d\n", lua_tostring(L, -2), lua_toboolean( L, -1 ));
        else if(lua_istable(L, -1))
          PrintTable(L);

        lua_pop(L, 1);
    }
}

int main(int argc, char const *argv[])
{
    printf("main\n");
    
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);

    int ret = luaL_loadfile(L, "/cust/script.lua");
    printf("RETLUA: %d\n", ret);
    lua_pcall(L, 0, 0, 0);

    // lua_getglobal(L, "getPorts");
    
    // if (lua_pcall(L, 1, 1, 0) != 0)
    //     printf("error running function : %s\n",
    //              lua_tostring(L, -1));

    lua_getglobal(L, "sum");

    if(lua_isfunction(L, -1) )
    {
        printf("SUM IS FUNC\n");
    } else {
        printf("ZALUPA\n");
    }

    lua_pushnumber(L, 10);
    lua_pushnumber(L, 12);
 
    lua_pcall(L, 2, 1, 0);
 
    int result = (int)lua_tonumber(L, -1);
    printf("RESULT: %d\n", result);
    lua_pop(L, 1);

    lua_getglobal(L, "getPorts");

    if(lua_isfunction(L, -1) )
    {
        printf("getPorts IS FUNC\n");
    } else {
        printf("getPorts ZALUPA\n");
    }

    lua_pcall(L, 0, 1, 0);
    //lua_pop(L, 1);

    PrintTable(L);
    //result = (int)lua_tonumber(L, -1);
    //printf("getPorts RESULT: %d\n", result);
    

    //PrintTable(L);

    printf("END\n");

    return 0;
}