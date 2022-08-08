// Write by Kuanlan (Kuanlan@outlook.com)
// Minimum perfect hash function for Lua language binding.
// This source file is generated by code generator.
// Don't modify this file directly, please modify the generator configuration.

#pragma once

namespace LuaSTG {
    enum class GameObjectMember {
        __NOT_FOUND = -1,
        _A = 1,
        VANGLE = 2,
        _B = 3,
        _BLEND = 4,
        _COLOR = 5,
        _G = 6,
        _R = 7,
        VSPEED = 8,
        _SUBA = 9,
        _SUBB = 10,
        _SUBCOLOR = 11,
        _SUBG = 12,
        _SUBR = 13,
        A = 14,
        AG = 15,
        ANI = 16,
        AX = 17,
        AY = 18,
        B = 19,
        BOUND = 20,
        CLASS = 21,
        COLLI = 22,
        COLLIDER = 23,
        DX = 24,
        DY = 25,
        GROUP = 26,
        HIDE = 27,
        HSCALE = 28,
        IMG = 29,
        LAYER = 30,
        MAXV = 31,
        MAXVX = 32,
        MAXVY = 33,
        NAVI = 34,
        IGNORESUPERPAUSE = 35,
        OMEGA = 36,
        PAUSE = 37,
        RES_RC = 38,
        RECT = 39,
        RESOLVEMOVE = 40,
        ROT = 41,
        STATUS = 42,
        TIMER = 43,
        VSCALE = 44,
        VX = 45,
        VY = 46,
        WORLD = 47,
        X = 48,
        Y = 49,
    };
    GameObjectMember MapGameObjectMember(const char* key);
}

namespace LuaSTG {
    enum class BlendModeX {
        __NOT_FOUND = -1,
        AddAdd = 6,
        AddAlpha = 5,
        AddMax = 19,
        AddMin = 18,
        AddMutiply = 20,
        AddRev = 7,
        AddScreen = 21,
        AddSub = 8,
        AlphaBal = 13,
        GradAdd = 10,
        GradAlpha = 9,
        GradMax = 23,
        GradMin = 22,
        GradMutiply = 24,
        GradRev = 11,
        GradScreen = 25,
        GradSub = 12,
        MulAdd = 2,
        MulAlpha = 1,
        MulMax = 15,
        MulMin = 14,
        MulMutiply = 16,
        MulRev = 3,
        MulScreen = 17,
        MulSub = 4,
        One = 26,
    };
    BlendModeX MapBlendModeX(const char* key);
}

namespace LuaSTG {
    enum class ColorMember {
        __NOT_FOUND = -1,
        f_AHSV = 1,
        f_ARGB = 2,
        m_a = 3,
        m_argb = 4,
        m_b = 5,
        m_g = 6,
        m_h = 7,
        m_r = 8,
        m_s = 9,
        m_v = 10,
    };
    ColorMember MapColorMember(const char* key);
}

