
#include "png.h"

#ifdef __cplusplus    // If used by C++ code,   
extern "C" {          // we need to export the C interface  
#endif  


static int l_dir (lua_State *L) {

    lua_newtable(L);
    int i = 1;
    while (i<5) {
        lua_pushnumber(L, i++); /* push key */
        lua_pushfstring(L, "%s-array(%d)", "11", i);
        lua_settable(L, -3);
    }
    return 1; /* table is already on top */
}


static const struct luaL_Reg pnglib [] = {
    {"dir", l_dir},
    {NULL, NULL} /* sentinel */
};


PNG_API int luaopen_png (lua_State *L) {
    luaL_newlib(L, pnglib);
    return 1;
}



#ifdef __cplusplus    // If used by C++ code,   
}          // we need to export the C interface  
#endif  