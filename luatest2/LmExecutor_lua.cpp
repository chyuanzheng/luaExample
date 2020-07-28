#include "stdafx.h"
#include "LmExecutor.h"

//示例代码，从dll 启动main.lua 从main.lua 再调用dll。


LmExecutor::LmExecutor(MarkInterface *markInterface)
{
    {
        m_luaState =luaL_newstate();
        luaL_openlibs(m_luaState);
        if (m_luaState==NULL)
        {
            LOG_C << "lua state wrong";
            return;
        }

        wstring path =  BslFileHelp::GetModuleDirectory();
        BslStringHelp help;
        string inPath  =  help.UCSToANSI(path.c_str());
        inPath += "\\main.lua";

        if (luaL_loadfile(m_luaState, inPath.c_str()) )
        {
            LOG_E <<  lua_tostring(m_luaState, -1);
            lua_close(m_luaState);
            m_luaState = NULL;
            LOG_E <<  "cannot call luaL_loadfile";
            return;
        }
        lua_pushinteger(m_luaState, (int)this);
        if(lua_pcall(m_luaState, 1, 0, 0))
        {
            LOG_E <<  lua_tostring(m_luaState, -1);
            lua_close(m_luaState);
            m_luaState = NULL;
            LOG_E <<  "cannot call lua_pcall";
            return;
        }
    }
}

LmExecutor::~LmExecutor(void)
{
}

void LmExecutor::DoCMDData(LmCMD * ptstate)
{
    if (m_luaState==NULL)
    {
        return;
    }

    if (ptstate->CMD_ID != 0x0201)
    {
        return;
    }
    /* push functions and arguments */
    if (lua_getglobal(m_luaState, "DoCMDData"))
    {
        return;
    }
    
    lua_pushlstring(m_luaState, (const char *)ptstate, sizeof(LmCMD));
    /* do the call (2 arguments, 1 result) */
    if (lua_pcall(m_luaState, 1, 1, 0) != LUA_OK)
    {
        LOG_E <<  lua_tostring(m_luaState, -1);
        BSLLOG << "DoCMDData lua_pcall wrong";
    }
    /* retrieve result */
    size_t len;
    const char *p = lua_tolstring(m_luaState,-1,&len);
    if (p==NULL && len != sizeof(LmCMD))
    {
        BSLLOG << "DoCMDData lua_tolstring wrong";
        return;
    }
    memcpy(ptstate, p, sizeof(LmCMD));
}

class LuaCMD :public LmCMD
{
public:
    LmExecutor *m_executor;
};


#define checkarray(L) \
    (LuaCMD *)luaL_checkudata(L, 1, "LuaBook.array")


static int l_newcmd (lua_State *L) {
    LmExecutor *p = (LmExecutor *)luaL_checkinteger(L, 1); /* number of bits */

    LuaCMD * a = (LuaCMD *)lua_newuserdata(L, sizeof(LuaCMD));
    memset(a,0, sizeof(LuaCMD));
    a->m_executor = p;
    a->m_executor->m_luaState = L;

    luaL_getmetatable(L, "LuaBook.array");
    lua_setmetatable(L, -2);

    return 1; /* new userdata is already on the stack */
}


static int l_addcmd (lua_State *L) {
    LuaCMD *a = checkarray(L);
    uint16 index = (uint16)luaL_checkinteger(L, 2);
    LOG_E << "l_addcmd " << index;
    return 0;
}

static const struct luaL_Reg marklib [] = {
    {"newcmd", l_newcmd},
    {NULL, NULL} /* sentinel */
};

static const struct luaL_Reg marklib_m [] = {
    {"addcmd", l_addcmd},
    {NULL, NULL}
};


extern "C" LMCCFG_API int luaopen_mark (lua_State *L) {
    luaL_newmetatable(L, "LuaBook.array"); /* create metatable */
    lua_pushvalue(L, -1); /* duplicate the metatable */
    lua_setfield(L, -2, "__index"); /* mt.__index = mt */
    luaL_setfuncs(L, marklib_m, 0); /* register metamethods */
    luaL_newlib(L, marklib);
    return 1;
}