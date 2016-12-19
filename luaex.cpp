#include "luaex.h"
#include "util.h"


#ifdef MIPS
lua_State* lua_script(const char *script, const char *function) {
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);

    int ret = luaL_loadfile(L, script);
    printf("RETLUA: %d\n", ret);
    lua_pcall(L, 0, 0, 0);

    lua_getglobal(L, function);

    if( ! lua_isfunction(L, -1) )
    {
        printf("%s ZALUPA\n", function);
        lua_close(L);
        return NULL;
    }

    lua_pcall(L, 0, 1, 0);

    return L;

};


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
#endif


std::string getPortStatus() {
    LOG
    #ifdef MIPS
    lua_State* L = lua_script("/cust/script.lua", "getPorts");

    if ( ! L ) {
        printf("FAILED\n");
        return "invalid info";
    }

    lua_pushnil(L);

    std::string response;
    int fisrt_item = 1;
    

    response.append( "[ ");
    while(lua_next(L, -2) != 0)
    {

        if(lua_istable(L, -1)) {
            lua_pushnil(L);

            int fisrt_arg = 1;

            if (fisrt_item) {
                fisrt_item = 0;
            } else {
                response.append( ", ");
            }

            
            response.append( "{ ");


            while(lua_next(L, -2) != 0)
            {
                if(lua_isstring(L, -1)) {
                    const char *name = strdup(lua_tostring(L, -2));
                    const char *val = strdup(lua_tostring(L, -1));
                    //printf("NAME: %s SVAL: %s\n", name, val);
                    //printf("%s = %s\n", lua_tostring(L, -2), lua_tostring(L, -1));

                    std::string sname(name);
                    std::string sval(val);

                    if ( sname == "port" ) {
                        if ( ! fisrt_arg)  response.append( ", " );
                        response.append( "\"port\": ");
                        response.append( "\"" + sval + "\"" );
                        fisrt_arg = 0;

                        if (sval == "3") {
                            response.append( ", \"info\": \"wan1\"");
                        }

                        if (sval == "4") {
                            response.append( ", \"info\": \"wan2\"");
                        }
                    }

                }

                // if(lua_isnumber(L, -1))
                //     printf("%s = %d\n", lua_tostring(L, -2), lua_tonumber(L, -1));

                else if ( lua_isboolean( L, -1 ) ) {
                    const char *name = strdup(lua_tostring(L, -2));
                    int val = lua_toboolean( L, -1 );
                    //printf("NAME: %s INTVAL: %d\n", name, val);

                    std::string sname(name);

                    
                    
                    if ( sname == "link" ) {
                        if ( ! fisrt_arg)  response.append( ", " );
                        response.append( "\"link\": ");
                        response.append( "\"" +  intToString(val)  + "\""  );
                        fisrt_arg = 0;
                    }

                } else {

                }
                
               

                lua_pop(L, 1);
            }
            
        }
        response.append( " } ");
        lua_pop(L, 1);
    }

    response.append( " ] ");

    printf("RESPONSE: %s\n", response.c_str());

    return response;
    #endif
    END
};


/*
void lua_in() {
    #ifdef MIPS
    lua_State* L = luaL_newstate();
    luaL_openlibs(L);

    int ret = luaL_loadfile(L, "/cust/script.lua");
    printf("RETLUA: %d\n", ret);
    lua_pcall(L, 0, 0, 0);

    lua_getglobal(L, "getPorts");

    if(lua_isfunction(L, -1) )
    {
        printf("getPorts IS FUNC\n");
    } else {
        printf("getPorts ZALUPA\n");
    }

    lua_pcall(L, 0, 1, 0);
    PrintTable(L);
    #endif
}

*/

/*
//перебор секций тест
int getCfgValue(const char *name,char *value, int valueBufLen)
{
    printf("getCfgValue: %s\n", name);
    if (NULL == name || NULL == value)
    {
        return -1;
    }

    struct uci_context * uciCtx = uci_alloc_context();
    
    struct uci_package * pkg = NULL;  
    struct uci_element * e;

    
    if (UCI_OK != uci_load(uciCtx, name, &pkg))  {
        printf("uci_load_pkg ERR: %s\n", name);
        goto cleanup;
    }
  
    uci_foreach_element(&pkg->sections, e)  
    {  
        
        struct uci_section *s = uci_to_section(e);  
        printf("uci_foreach_element: %s\n", s->e.name);

        if (uci_lookup_option_string(uciCtx, s, "wan2"))  
        {
            //strncpy(value, pValueData, valueBufLen);
            //printf("get wireless cfg ok, %s: %s\n", name, value);
            printf("XER WAN2\n");
        }
    }  

    printf("getCfgValue SUCC: %s\n", name);
    uci_unload(uciCtx, pkg);  
    
cleanup:  
    uci_free_context(uciCtx);  
    uciCtx = NULL; 

    return -1;
}

char *uci_get(struct uci_context *context, const char *str)
{
    printf("uci_get: %s\n", str);
    struct uci_ptr result = {};
    char str_copy[strlen(str) + 1];
    snprintf(str_copy, (strlen(str) + 1), "%s", str);

    if (uci_lookup_ptr(context, &result, (char *) str_copy, true) != UCI_OK) {
        return NULL;
    }

    if (UCI_TYPE_SECTION == result.target && result.s) {
        char *ret = strdup(result.s->type);
        if (!ret)
            printf("Error memory.\n");
        return ret;
    }

    if (!result.o)
        return NULL;

    char *ret = strdup(result.o->v.string);
    if (!ret)
        printf("Error uci get.\n");

    if (!ret)
        ret = strdup("");
    return ret;
}

//перебор секций тест
int getSection(const char *name)
{
    printf("getSection: %s\n", name);
    struct uci_context *c;
    struct uci_ptr ptr;

    struct uci_package * pkg = NULL;  

    char *copy_name = strdup (name);
    c = uci_alloc_context ();

    int ret;

    ret = uci_load(c, "network", &pkg);
    if (ret != UCI_OK){
        printf("FAILED UCI LOAD\n");
        return -1;
    }

    char* uci_get_ret = uci_get(c, "network.wan2");
    printf("RETUCIGET: %s\n", uci_get_ret);

    // if ((uci_lookup_ptr(c, &ptr, copy_name, true) != UCI_OK)) {
    //     printf("UCI_ERR lookup\n");
    //     uci_free_context(c);
    //     free(copy_name);
    //     return -1;
    // }

    

    // if(ptr.flags & UCI_LOOKUP_COMPLETE) {
    //     printf("UCI_LOOKUP_COMPLETE\n");
    // } else {
    //     printf("UCI_ERR incomplete\n");
    // }

    // uci_free_context(c);
    // free(copy_name);
    return 1;
}



void add_section(const char *root, const char *section) {
    #ifdef MIPS
    printf("add_section: %s %s\n", root, section);
    struct uci_package *p = NULL;
    struct uci_section *s = NULL;

    struct uci_context *c;
    c = uci_alloc_context ();

    int ret;
    ret = uci_load(c, root, &p);
    if (ret != UCI_OK){
        printf("FAILED UCI LOAD\n");
        return;
    }

    ret = uci_add_section(c, p, section, &s);
    if (ret != UCI_OK){
        printf("FAILED UCI ADD SECTION\n");
        return;
    }

    ret = uci_save(c, p);

    printf("SECTION ADDED\n");
    #endif
}

void set_value(const char *name, const char *value) {
    #ifdef MIPS
    printf("UCISETVAUE: %s => %s\n", name, value);
   struct uci_context *c;
   struct uci_ptr p;
   char *a = strdup (name);

   c = uci_alloc_context ();
   if (uci_lookup_ptr (c, &p, a, true) != UCI_OK)
   {
      uci_perror (c, "XXX");
      return;
   }

   //printf("%s: %s\n", name, p.o->v.string);
   p.value = value;
   uci_set(c,&p);
   uci_commit(c, &p.p, false);

   uci_free_context (c);
   #endif
};
*/