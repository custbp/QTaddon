#include "inc.h"

void PrintTable(lua_State *L);
lua_State* lua_script(const char *script, const char *function) ;
std::string getPortStatus();