#include "lua_particle.hpp"
#include "Particle/Particle2D.h"
#include "Core/Graphics/Sprite.hpp"
#include "AppFrame.h"
#include "LuaWrapper.hpp"
#include "LuaWrapperMisc.hpp"
#include "lua_luastg_hash.hpp"

using namespace LuaSTGPlus;
using namespace LuaSTGPlus::Particle;

std::string_view const ID_Pool2D("particle.ParticlePool2D");
std::string_view const ID_Particle2D("particle.Particle2D");
std::string_view const ID_Vector2("particle.Vector2");

struct Particle2DUserData
{
    ParticlePool2D::Particle* p;
};

int lua_NewPool2D(lua_State* L)
{
    int const argc = lua_gettop(L);
    int32_t poolsize = 512;
    BlendMode blend = BlendMode::MulAlpha;
    Core::ScopeObject<Core::Graphics::ISprite> img;
    
    if (argc < 1)
        return luaL_error(L, "must provide image resource for particle system");
    
    img = LAPP.GetResourceMgr().FindSprite(luaL_checkstring(L, 1))->GetSprite();
    
    if (argc >= 2)
        blend = TranslateBlendMode(L, 2);
    if (argc >= 3)
        poolsize = luaL_checkint(L, 3);

    ParticlePool2D** pool = static_cast<ParticlePool2D**>(lua_newuserdata(L, sizeof(*pool)));
    luaL_setmetatable(L, ID_Pool2D.data());
    *pool = new ParticlePool2D(poolsize, img, blend);

    return 1;
}

int lua_pool2d_AddParticle(lua_State* L)
{
    ParticlePool2D** self = static_cast<ParticlePool2D**>(luaL_checkudata(L, 1, ID_Pool2D.data()));
    int const argc = lua_gettop(L);

    ParticlePool2D::Particle p{
        .pos = Core::Vector2F(luaL_checknumber(L, 2), luaL_checknumber(L, 3)),
        .vel = Core::Vector2F((float)luaL_optnumber(L, 5, 0), (float)luaL_optnumber(L, 6, 0)),
        .scale = Core::Vector2F(1, 1),
        .rot = (float)luaL_optnumber(L, 4, 0),
        .color = Core::Color4B(0xFFFFFFFF),
    };

    if (argc == 7)
    {
        float s = luaL_checknumber(L, 7);
        p.scale = Core::Vector2F(s, s);
    }
    else if (argc > 7)
    {
        p.scale = Core::Vector2F(luaL_checknumber(L, 7), luaL_checknumber(L, 8));
    }

    ParticlePool2D::Particle** ud = static_cast<ParticlePool2D::Particle**>(lua_newuserdata(L, sizeof(*ud)));
    luaL_setmetatable(L, ID_Particle2D.data());
    new (ud) ParticlePool2D::Particle* { (*self)->AddParticle(std::move(p)) };
    
    return 1;
}

int lua_pool2d_Update(lua_State* L)
{
    ParticlePool2D** self = static_cast<ParticlePool2D**>(luaL_checkudata(L, 1, ID_Pool2D.data()));
    (*self)->Update();
    return 0;
}

int lua_pool2d_Render(lua_State* L)
{
    ParticlePool2D** self = static_cast<ParticlePool2D**>(luaL_checkudata(L, 1, ID_Pool2D.data()));
    (*self)->Render();
    return 0;
}

int lua_pool2d_Apply(lua_State* L)
{
    ParticlePool2D** self = static_cast<ParticlePool2D**>(luaL_checkudata(L, 1, ID_Pool2D.data()));

    int r_fn = luaL_ref(L, LUA_REGISTRYINDEX);

    (*self)->Apply([&](auto p) {
        lua_rawgeti(L, LUA_REGISTRYINDEX, r_fn);

        ParticlePool2D::Particle** ud = static_cast<ParticlePool2D::Particle**>(lua_newuserdata(L, sizeof(*ud)));
        *ud = p;
        luaL_getmetatable(L, ID_Particle2D.data());
        lua_setmetatable(L, -2);


        lua_call(L, 1, 1);
        bool ret = lua_toboolean(L, -1);
        lua_pop(L, 1);

        return ret;
    });

    luaL_unref(L, LUA_REGISTRYINDEX, r_fn);
    return 0;
}

int lua_pool2d_GetSize(lua_State* L)
{
    ParticlePool2D** self = static_cast<ParticlePool2D**>(luaL_checkudata(L, 1, ID_Pool2D.data()));

    lua_pushnumber(L, (*self)->GetSize());

    return 1;
}

int lua_pool2d_mtToString(lua_State* L)
{
    ParticlePool2D** self = static_cast<ParticlePool2D**>(luaL_checkudata(L, 1, ID_Pool2D.data()));

    lua_pushfstring(L, "ParticlePool2D(size = %d)", (*self)->GetSize());

    return 1;
}

int lua_pool2d_mtGC(lua_State* L)
{
    ParticlePool2D** self = static_cast<ParticlePool2D**>(luaL_checkudata(L, 1, ID_Pool2D.data()));

    delete* self;

    return 0;
}

int lua_particle2d_mtIndex(lua_State* L)
{
    ParticlePool2D::Particle** self = static_cast<ParticlePool2D::Particle**>(luaL_checkudata(L, 1, ID_Particle2D.data()));
    const char* k = luaL_checkstring(L, 2);

    switch (k[0]) {
    case 'a':
        switch (k[1]) {
        case 'x':
            switch (k[2]) {
            case '\0':
                lua_pushnumber(L, (*self)->accel.x);
                return 1;
            }
            break;
        case 'y':
            switch (k[2]) {
            case '\0':
                lua_pushnumber(L, (*self)->accel.y);
                return 1;
            }
            break;
        case '\0':
            lua_pushinteger(L, (*self)->color.a);
            return 1;
        }
        break;
    case 'b':
        switch (k[2]) {
        case '\0':
            lua_pushinteger(L, (*self)->color.b);
            return 1;
        }
        break;
    case 'c':
        switch (k[1]) {
        case 'o':
            switch (k[2]) {
            case 'l':
                switch (k[3]) {
                case 'o':
                    switch (k[4]) {
                    case 'r':
                        switch (k[5]) {
                        case '\0':
                            LuaWrapper::ColorWrapper::CreateAndPush(L, (*self)->color);
                            return 1;
                        }
                        break;
                    }
                    break;
                }
                break;
            }
            break;
        }
        break;
    case 'e':
        switch (k[1]) {
        case 'x':
            switch (k[2]) {
            case 't':
                switch (k[3]) {
                case 'r':
                    switch (k[4]) {
                    case 'a':
                        switch (k[5]) {
                        case '1':
                            switch (k[6]) {
                            case '\0':
                                lua_pushnumber(L, (*self)->extra1);
                                return 1;
                            }
                            break;
                        case '2':
                            switch (k[6]) {
                            case '\0':
                                lua_pushnumber(L, (*self)->extra2);
                                return 1;
                            }
                            break;
                        case '3':
                            switch (k[6]) {
                            case '\0':
                                lua_pushnumber(L, (*self)->extra3);
                                return 1;
                            }
                            break;
                        }
                        break;
                    }
                    break;
                }
                break;
            }
            break;
        }
        break;
    case 'o':
        switch (k[1]) {
        case 'm':
            switch (k[2]) {
            case 'i':
                switch (k[3]) {
                case 'g':
                    switch (k[4]) {
                    case 'a':
                        switch (k[5]) {
                        case '\0':
                            lua_pushnumber(L, (*self)->omiga);
                            return 1;
                        }
                        break;
                    }
                    break;
                }
                break;
            }
            break;
        }
        break;
    case 'g':
        switch (k[2]) {
        case '\0':
            lua_pushinteger(L, (*self)->color.g);
            return 1;
        }
        break;
    case 'r':
        switch (k[1]) {
        case 'o':
            switch (k[2]) {
            case 't':
                switch (k[3]) {
                case '\0':
                    lua_pushnumber(L, (*self)->rot);
                    return 1;
                }
                break;
            }
            break;
        case '\0':
            lua_pushinteger(L, (*self)->color.r);
            return 1;
        }
        break;
    case 's':
        switch (k[1]) {
        case 'x':
            switch (k[2]) {
            case '\0':
                lua_pushnumber(L, (*self)->scale.x);
                return 1;
            }
            break;
        case 'y':
            switch (k[2]) {
            case '\0':
                lua_pushnumber(L, (*self)->scale.y);
                return 1;
            }
            break;
        }
        break;
    case 't':
        switch (k[1]) {
        case 'i':
            switch (k[2]) {
            case 'm':
                switch (k[3]) {
                case 'e':
                    switch (k[4]) {
                    case 'r':
                        switch (k[5]) {
                        case '\0':
                            lua_pushinteger(L, (*self)->timer);
                            return 1;
                        }
                        break;
                    }
                    break;
                }
                break;
            }
            break;
        }
        break;
    case 'v':
        switch (k[1]) {
        case 'x':
            switch (k[2]) {
            case '\0':
                lua_pushnumber(L, (*self)->vel.x);
                return 1;
            }
            break;
        case 'y':
            switch (k[2]) {
            case '\0':
                lua_pushnumber(L, (*self)->vel.y);
                return 1;
            }
            break;
            break;
        }
    case 'x':
        switch (k[2]) {
        case '\0':
            lua_pushnumber(L, (*self)->pos.x);
            return 1;
        }
        break;
    case 'y':
        switch (k[2]) {
        case '\0':
            lua_pushnumber(L, (*self)->pos.y);
            return 1;
        }
        break;
    }

    lua_pushnil(L);
    return 1;
}

int lua_particle2d_mtNewIndex(lua_State* L)
{
    ParticlePool2D::Particle** self = static_cast<ParticlePool2D::Particle**>(luaL_checkudata(L, 1, ID_Particle2D.data()));
    const char* k = luaL_checkstring(L, 2);

    switch (k[0]) {
    case 'a':
        switch (k[1]) {
        case 'x':
            switch (k[2]) {
            case '\0':
                (*self)->accel.x = luaL_checknumber(L, 3);
                return 0;
            }
            break;
        case 'y':
            switch (k[2]) {
            case '\0':
                (*self)->accel.y = luaL_checknumber(L, 3);
                return 0;
            }
            break;
        case '\0':
            (*self)->color.a = luaL_checknumber(L, 3);
            return 0;
        }
        break;
    case 'b':
        switch (k[1]) {
        case '\0':
            (*self)->color.b = luaL_checknumber(L, 3);
            return 0;
        }
        break;
    case 'c':
        switch (k[1]) {
        case 'o':
            switch (k[2]) {
            case 'l':
                switch (k[3]) {
                case 'o':
                    switch (k[4]) {
                    case 'r':
                        switch (k[5]) {
                        case '\0':
                            (*self)->color = *LuaWrapper::ColorWrapper::Cast(L, 3);
                            return 0;
                        }
                        break;
                    }
                    break;
                }
                break;
            }
            break;
        }
        break;
    case 'e':
        switch (k[1]) {
        case 'x':
            switch (k[2]) {
            case 't':
                switch (k[3]) {
                case 'r':
                    switch (k[4]) {
                    case 'a':
                        switch (k[5]) {
                        case '1':
                            switch (k[6]) {
                            case '\0':
                                (*self)->extra1 = luaL_checknumber(L, 3);
                                return 0;
                            }
                            break;
                        case '2':
                            switch (k[6]) {
                            case '\0':
                                (*self)->extra2 = luaL_checknumber(L, 3);
                                return 0;
                            }
                            break;
                        case '3':
                            switch (k[6]) {
                            case '\0':
                                (*self)->extra3 = luaL_checknumber(L, 3);
                                return 0;
                            }
                            break;
                        }
                        break;
                    }
                    break;
                }
                break;
            }
            break;
        }
        break;
    case 'o':
        switch (k[1]) {
        case 'm':
            switch (k[2]) {
            case 'i':
                switch (k[3]) {
                case 'g':
                    switch (k[4]) {
                    case 'a':
                        switch (k[5]) {
                        case '\0':
                            (*self)->omiga = luaL_checknumber(L, 3);
                            return 0;
                        }
                        break;
                    }
                    break;
                }
                break;
            }
            break;
        }
        break;
    case 'g':
        switch (k[1]) {
        case '\0':
            (*self)->color.g = luaL_checknumber(L, 3);
            return 0;
        }
        break;
    case 'r':
        switch (k[1]) {
        case 'o':
            switch (k[2]) {
            case 't':
                switch (k[3]) {
                case '\0':
                    (*self)->rot = luaL_checknumber(L, 3);
                    return 0;
                }
                break;
            }
            break;
        case '\0':
            (*self)->color.r = luaL_checknumber(L, 3);
            return 0;
        }
        break;
    case 's':
        switch (k[1]) {
        case 'x':
            switch (k[2]) {
            case '\0':
                (*self)->scale.x = luaL_checknumber(L, 3);
                return 0;
            }
            break;
        case 'y':
            switch (k[2]) {
            case '\0':
                (*self)->scale.y = luaL_checknumber(L, 3);
                return 0;
            }
            break;
        }
        break;
    case 't':
        switch (k[1]) {
        case 'i':
            switch (k[2]) {
            case 'm':
                switch (k[3]) {
                case 'e':
                    switch (k[4]) {
                    case 'r':
                        switch (k[5]) {
                        case '\0':
                            (*self)->timer = luaL_checknumber(L, 3);
                            return 0;
                        }
                        break;
                    }
                    break;
                }
                break;
            }
            break;
        }
        break;
    case 'v':
        switch (k[1]) {
        case 'x':
            switch (k[2]) {
            case '\0':
                (*self)->vel.x = luaL_checknumber(L, 3);
                return 0;
            }
            break;
        case 'y':
            switch (k[2]) {
            case '\0':
                (*self)->vel.y = luaL_checknumber(L, 3);
                return 0;
            }
            break;
            break;
        }
    case 'x':
        switch (k[1]) {
        case '\0':
            (*self)->pos.x = luaL_checknumber(L, 3);
            return 0;
        }
        break;
    case 'y':
        switch (k[1]) {
        case '\0':
            (*self)->pos.y = luaL_checknumber(L, 3);
            return 0;
        }
        break;
    }

    return luaL_error(L, "Attempted to set an invalid particle index.");
}

int lua_particle2d_mtToString(lua_State* L)
{
    ParticlePool2D::Particle** self = static_cast<ParticlePool2D::Particle**>(luaL_checkudata(L, 1, ID_Particle2D.data()));

    lua_pushfstring(L, "Particle2D(pos = %d, %d)", (*self)->pos.x, (*self)->pos.y);

    return 1;
}

luaL_Reg const pool2d_methods[] = {
    { "AddParticle", &lua_pool2d_AddParticle },
    { "Update", &lua_pool2d_Update },
    { "Render", &lua_pool2d_Render },
    { "Apply", &lua_pool2d_Apply },
    { "GetSize", &lua_pool2d_GetSize },

    { NULL, NULL }
};

luaL_Reg const pool2d_mt[] = {
    { "__tostring", &lua_pool2d_mtToString },
    { "__gc", &lua_pool2d_mtGC },

    { NULL, NULL }
};

luaL_Reg particle2d_mt[] = {
    { "__index", &lua_particle2d_mtIndex },
    { "__newindex", &lua_particle2d_mtNewIndex },
    { "__tostring", &lua_particle2d_mtToString },

    { NULL, NULL }
};

// 3D particles go here

luaL_Reg const funcs[] = {
    { "NewPool2D", &lua_NewPool2D },

    { NULL, NULL }
};

luaL_Reg nofuncs[] = {
    { NULL, NULL }
};

int luaopen_particle(lua_State* L)
{
    luaL_register(L, "particle", funcs);
    RegisterClassIntoTable2(L, ".Particle2D", nofuncs, ID_Particle2D.data(), particle2d_mt);
    RegisterClassIntoTable(L, ".ParticlePool2D", pool2d_methods, ID_Pool2D.data(), pool2d_mt);
    //RegisterClassIntoTable2(L, ".Particle3D", nofuncs, ID_Particle3D.data(), particle3d_mt);
    //RegisterClassIntoTable(L, ".ParticlePool3D", pool3d_methods, ID_Pool3D.data(), pool3d_mt);
    lua_pop(L, 1);

    return 1;
}