﻿#include "LuaWrapper\LuaWrapper.hpp"
#include "AppFrame.h"
#include "LMathConstant.hpp"

#define l___l
#define l_____l

inline LuaSTG::Core::Renderer& LR2D() { return LuaSTGPlus::AppFrame::GetInstance().GetRenderer2D(); }
inline LuaSTGPlus::ResourceMgr& LRESMGR() { return LuaSTGPlus::AppFrame::GetInstance().GetResourceMgr(); }
inline f2dRenderDevice* LRDEV() { return LuaSTGPlus::AppFrame::GetInstance().GetRenderDev(); }

#ifdef _DEBUG
#define check_rendertarget_usage(PTEXTURE) assert(!LuaSTGPlus::AppFrame::GetInstance().CheckRenderTargetInUse(PTEXTURE));
#else
#define check_rendertarget_usage(PTEXTURE)
#endif // _DEBUG

inline void rotate_float2(float& x, float& y, const float r)
{
    float const sinv = std::sinf(r);
    float const cosv = std::cosf(r);
    float const tx = x * cosv - y * sinv;
    float const ty = x * sinv + y * cosv;
    x = tx;
    y = ty;
}
inline void rotate_float2x4(float& x1, float& y1, float& x2, float& y2, float& x3, float& y3, float& x4, float& y4, const float r)
{
    float const sinv = std::sinf(r);
    float const cosv = std::cosf(r);
    {
        float const tx = x1 * cosv - y1 * sinv;
        float const ty = x1 * sinv + y1 * cosv;
        x1 = tx;
        y1 = ty;
    }
    {
        float const tx = x2 * cosv - y2 * sinv;
        float const ty = x2 * sinv + y2 * cosv;
        x2 = tx;
        y2 = ty;
    }
    {
        float const tx = x3 * cosv - y3 * sinv;
        float const ty = x3 * sinv + y3 * cosv;
        x3 = tx;
        y3 = ty;
    }
    {
        float const tx = x4 * cosv - y4 * sinv;
        float const ty = x4 * sinv + y4 * cosv;
        x4 = tx;
        y4 = ty;
    }
}
// 要改这个记得也改 LuaSTGPlus::AppFrame::updateGraph2DBlendMode
static void translate_blend(LuaSTG::Core::Renderer& ctx, const LuaSTGPlus::BlendMode blend)
{
    switch (blend)
    {
    default:
    case LuaSTGPlus::BlendMode::MulAlpha:
        ctx.setVertexColorBlendState(LuaSTG::Core::VertexColorBlendState::Mul);
        ctx.setBlendState(LuaSTG::Core::BlendState::Alpha);
        break;
    case LuaSTGPlus::BlendMode::MulAdd:
        ctx.setVertexColorBlendState(LuaSTG::Core::VertexColorBlendState::Mul);
        ctx.setBlendState(LuaSTG::Core::BlendState::Add);
        break;
    case LuaSTGPlus::BlendMode::MulRev:
        ctx.setVertexColorBlendState(LuaSTG::Core::VertexColorBlendState::Mul);
        ctx.setBlendState(LuaSTG::Core::BlendState::RevSub);
        break;
    case LuaSTGPlus::BlendMode::MulSub:
        ctx.setVertexColorBlendState(LuaSTG::Core::VertexColorBlendState::Mul);
        ctx.setBlendState(LuaSTG::Core::BlendState::Sub);
        break;
    case LuaSTGPlus::BlendMode::AddAlpha:
        ctx.setVertexColorBlendState(LuaSTG::Core::VertexColorBlendState::Add);
        ctx.setBlendState(LuaSTG::Core::BlendState::Alpha);
        break;
    case LuaSTGPlus::BlendMode::AddAdd:
        ctx.setVertexColorBlendState(LuaSTG::Core::VertexColorBlendState::Add);
        ctx.setBlendState(LuaSTG::Core::BlendState::Add);
        break;
    case LuaSTGPlus::BlendMode::AddRev:
        ctx.setVertexColorBlendState(LuaSTG::Core::VertexColorBlendState::Add);
        ctx.setBlendState(LuaSTG::Core::BlendState::RevSub);
        break;
    case LuaSTGPlus::BlendMode::AddSub:
        ctx.setVertexColorBlendState(LuaSTG::Core::VertexColorBlendState::Add);
        ctx.setBlendState(LuaSTG::Core::BlendState::Sub);
        break;
    case LuaSTGPlus::BlendMode::AlphaBal:
        ctx.setVertexColorBlendState(LuaSTG::Core::VertexColorBlendState::Mul);
        ctx.setBlendState(LuaSTG::Core::BlendState::Inv);
        break;
    case LuaSTGPlus::BlendMode::MulMin:
        ctx.setVertexColorBlendState(LuaSTG::Core::VertexColorBlendState::Mul);
        ctx.setBlendState(LuaSTG::Core::BlendState::Min);
        break;
    case LuaSTGPlus::BlendMode::MulMax:
        ctx.setVertexColorBlendState(LuaSTG::Core::VertexColorBlendState::Mul);
        ctx.setBlendState(LuaSTG::Core::BlendState::Max);
        break;
    case LuaSTGPlus::BlendMode::MulMutiply:
        ctx.setVertexColorBlendState(LuaSTG::Core::VertexColorBlendState::Mul);
        ctx.setBlendState(LuaSTG::Core::BlendState::Mul);
        break;
    case LuaSTGPlus::BlendMode::MulScreen:
        ctx.setVertexColorBlendState(LuaSTG::Core::VertexColorBlendState::Mul);
        ctx.setBlendState(LuaSTG::Core::BlendState::Screen);
        break;
    case LuaSTGPlus::BlendMode::AddMin:
        ctx.setVertexColorBlendState(LuaSTG::Core::VertexColorBlendState::Add);
        ctx.setBlendState(LuaSTG::Core::BlendState::Min);
        break;
    case LuaSTGPlus::BlendMode::AddMax:
        ctx.setVertexColorBlendState(LuaSTG::Core::VertexColorBlendState::Add);
        ctx.setBlendState(LuaSTG::Core::BlendState::Max);
        break;
    case LuaSTGPlus::BlendMode::AddMutiply:
        ctx.setVertexColorBlendState(LuaSTG::Core::VertexColorBlendState::Add);
        ctx.setBlendState(LuaSTG::Core::BlendState::Mul);
        break;
    case LuaSTGPlus::BlendMode::AddScreen:
        ctx.setVertexColorBlendState(LuaSTG::Core::VertexColorBlendState::Add);
        ctx.setBlendState(LuaSTG::Core::BlendState::Screen);
        break;
    case LuaSTGPlus::BlendMode::One:
        ctx.setVertexColorBlendState(LuaSTG::Core::VertexColorBlendState::Mul);
        ctx.setBlendState(LuaSTG::Core::BlendState::One);
        break;
    }
}
static LuaSTG::Core::BlendState translate_blend_3d(const LuaSTGPlus::BlendMode blend)
{
    switch (blend)
    {
    default:
    case LuaSTGPlus::BlendMode::MulAlpha:
        return LuaSTG::Core::BlendState::Alpha;
    case LuaSTGPlus::BlendMode::MulAdd:
        return LuaSTG::Core::BlendState::Add;
    case LuaSTGPlus::BlendMode::MulRev:
        return LuaSTG::Core::BlendState::RevSub;
    case LuaSTGPlus::BlendMode::MulSub:
        return LuaSTG::Core::BlendState::Sub;
    case LuaSTGPlus::BlendMode::AddAlpha:
        return LuaSTG::Core::BlendState::Alpha;
    case LuaSTGPlus::BlendMode::AddAdd:
        return LuaSTG::Core::BlendState::Add;
    case LuaSTGPlus::BlendMode::AddRev:
        return LuaSTG::Core::BlendState::RevSub;
    case LuaSTGPlus::BlendMode::AddSub:
        return LuaSTG::Core::BlendState::Sub;
    case LuaSTGPlus::BlendMode::AlphaBal:
        return LuaSTG::Core::BlendState::Inv;
    case LuaSTGPlus::BlendMode::MulMin:
        return LuaSTG::Core::BlendState::Min;
    case LuaSTGPlus::BlendMode::MulMax:
        return LuaSTG::Core::BlendState::Max;
    case LuaSTGPlus::BlendMode::MulMutiply:
        return LuaSTG::Core::BlendState::Mul;
    case LuaSTGPlus::BlendMode::MulScreen:
        return LuaSTG::Core::BlendState::Screen;
    case LuaSTGPlus::BlendMode::AddMin:
        return LuaSTG::Core::BlendState::Min;
    case LuaSTGPlus::BlendMode::AddMax:
        return LuaSTG::Core::BlendState::Max;
    case LuaSTGPlus::BlendMode::AddMutiply:
        return LuaSTG::Core::BlendState::Mul;
    case LuaSTGPlus::BlendMode::AddScreen:
        return LuaSTG::Core::BlendState::Screen;
    case LuaSTGPlus::BlendMode::One:
        return LuaSTG::Core::BlendState::One;
    }
}

static void make_sprite_vertex(f2dSprite* pimg2d, LuaSTG::Core::DrawVertex2D* vertex, float const x, float const y, float const rot, float const hscale, float const vscale, float const z)
{
    fcyRect const img_rect = pimg2d->GetTexRect();
    // XY
    fcyVec2 const center_p = pimg2d->GetHotSpot() - img_rect.a;
    fFloat const img_w = img_rect.GetWidth();
    fFloat const img_h = img_rect.GetHeight();
    vertex[0].x = hscale * (l___l - center_p.x); vertex[0].y = vscale * (center_p.y l_____l);
    vertex[1].x = hscale * (img_w - center_p.x); vertex[1].y = vscale * (center_p.y l_____l);
    vertex[2].x = hscale * (img_w - center_p.x); vertex[2].y = vscale * (center_p.y - img_h);
    vertex[3].x = hscale * (l___l - center_p.x); vertex[3].y = vscale * (center_p.y - img_h);
    rotate_float2x4(
        vertex[0].x, vertex[0].y,
        vertex[1].x, vertex[1].y,
        vertex[2].x, vertex[2].y,
        vertex[3].x, vertex[3].y,
        rot);
    vertex[0].x += x; vertex[0].y += y;
    vertex[1].x += x; vertex[1].y += y;
    vertex[2].x += x; vertex[2].y += y;
    vertex[3].x += x; vertex[3].y += y;
    // Z
    vertex[0].z = z;
    vertex[1].z = z;
    vertex[2].z = z;
    vertex[3].z = z;
    // UV
    f2dTexture2D* const ptex2d = pimg2d->GetTexture();
    fFloat const tex_w = (fFloat)ptex2d->GetWidth();
    fFloat const tex_h = (fFloat)ptex2d->GetHeight();
    vertex[0].u = img_rect.a.x / tex_w; vertex[0].v = img_rect.a.y / tex_h; 
    vertex[1].u = img_rect.b.x / tex_w; vertex[1].v = img_rect.a.y / tex_h; 
    vertex[2].u = img_rect.b.x / tex_w; vertex[2].v = img_rect.b.y / tex_h; 
    vertex[3].u = img_rect.a.x / tex_w; vertex[3].v = img_rect.b.y / tex_h; 
    // color
    fcyColor color_ls[4];
    pimg2d->GetColor(color_ls);
    vertex[0].color = color_ls[0].argb;
    vertex[1].color = color_ls[1].argb;
    vertex[2].color = color_ls[2].argb;
    vertex[3].color = color_ls[3].argb;
}

static void api_drawSprite(LuaSTGPlus::ResSprite* pimg2dres, float const x, float const y, float const rot, float const hscale, float const vscale, float const z)
{
    f2dSprite* pimg2d = pimg2dres->GetSprite();
    auto& ctx = LR2D();

    float const scale = pimg2d->GetScale();
    LuaSTGPlus::BlendMode blend = pimg2dres->GetBlendMode();
    translate_blend(ctx, blend);

    f2dTexture2D* const ptex2d = pimg2d->GetTexture();
    check_rendertarget_usage(ptex2d);
    ctx.setTextureAlphaType(ptex2d->IsPremultipliedAlpha() ? LuaSTG::Core::TextureAlphaType::PremulAlpha : LuaSTG::Core::TextureAlphaType::Normal);
    ctx.setTexture(LuaSTG::Core::TextureID(ptex2d->GetHandle()));

    LuaSTG::Core::DrawVertex2D vertex[4];
    make_sprite_vertex(pimg2d, vertex, x, y, rot, hscale * scale, vscale * scale, z);
    ctx.drawQuad(vertex[0], vertex[1], vertex[2], vertex[3]);
}
static void api_drawSprite(char const* name, float const x, float const y, float const rot, float const hscale, float const vscale, float const z)
{
    fcyRefPointer<LuaSTGPlus::ResSprite> pimg2dres = LRESMGR().FindSprite(name);
    if (!pimg2dres)
    {
        spdlog::error("[luastg] lstg.Renderer.drawSprite failed, can't find sprite '{}'", name);
        return;
    }
    api_drawSprite(*pimg2dres, x, y, rot, hscale, vscale, z);
}
static void api_drawSpriteRect(LuaSTGPlus::ResSprite* pimg2dres, float const l, float const r, float const b, float const t, float const z)
{
    f2dSprite* pimg2d = pimg2dres->GetSprite();
    f2dTexture2D* ptex2d = pimg2d->GetTexture();

    fFloat tex_w = (fFloat)ptex2d->GetWidth();
    fFloat tex_h = (fFloat)ptex2d->GetHeight();

    fcyRect img_rect = pimg2d->GetTexRect();
    LuaSTGPlus::BlendMode blend = pimg2dres->GetBlendMode();
    fcyColor color_ls[4];
    pimg2d->GetColor(color_ls);

    LuaSTG::Core::DrawVertex2D vertex[4] = {
        {.x = l, .y = t, .z = z, .color = color_ls[0].argb, .u = img_rect.a.x / tex_w, .v = img_rect.a.y / tex_h },
        {.x = r, .y = t, .z = z, .color = color_ls[1].argb, .u = img_rect.b.x / tex_w, .v = img_rect.a.y / tex_h },
        {.x = r, .y = b, .z = z, .color = color_ls[2].argb, .u = img_rect.b.x / tex_w, .v = img_rect.b.y / tex_h },
        {.x = l, .y = b, .z = z, .color = color_ls[3].argb, .u = img_rect.a.x / tex_w, .v = img_rect.b.y / tex_h },
    };

    auto& ctx = LR2D();
    translate_blend(ctx, blend);
    check_rendertarget_usage(ptex2d);
    ctx.setTextureAlphaType(ptex2d->IsPremultipliedAlpha() ? LuaSTG::Core::TextureAlphaType::PremulAlpha : LuaSTG::Core::TextureAlphaType::Normal);
    ctx.setTexture(LuaSTG::Core::TextureID(ptex2d->GetHandle()));
    ctx.drawQuad(vertex[0], vertex[1], vertex[2], vertex[3]);
}
static void api_drawSpriteRect(char const* name, float const l, float const r, float const b, float const t, float const z)
{
    fcyRefPointer<LuaSTGPlus::ResSprite> pimg2dres = LRESMGR().FindSprite(name);
    if (!pimg2dres)
    {
        spdlog::error("[luastg] lstg.Renderer.drawSpriteRect failed, can't find sprite '{}'", name);
        return;
    }
    api_drawSpriteRect(*pimg2dres, l, r, b, t, z);
}
static void api_drawSprite4V(LuaSTGPlus::ResSprite* pimg2dres, float const x1, float const y1, float const z1, float const x2, float const y2, float const z2, float const x3, float const y3, float const z3, float const x4, float const y4, float const z4)
{
    f2dSprite* pimg2d = pimg2dres->GetSprite();
    f2dTexture2D* ptex2d = pimg2d->GetTexture();

    fFloat tex_w = (fFloat)ptex2d->GetWidth();
    fFloat tex_h = (fFloat)ptex2d->GetHeight();

    fcyRect img_rect = pimg2d->GetTexRect();
    LuaSTGPlus::BlendMode blend = pimg2dres->GetBlendMode();
    fcyColor color_ls[4];
    pimg2d->GetColor(color_ls);

    LuaSTG::Core::DrawVertex2D vertex[4] = {
        {.x = x1, .y = y1, .z = z1, .color = color_ls[0].argb, .u = img_rect.a.x / tex_w, .v = img_rect.a.y / tex_h },
        {.x = x2, .y = y2, .z = z2, .color = color_ls[1].argb, .u = img_rect.b.x / tex_w, .v = img_rect.a.y / tex_h },
        {.x = x3, .y = y3, .z = z3, .color = color_ls[2].argb, .u = img_rect.b.x / tex_w, .v = img_rect.b.y / tex_h },
        {.x = x4, .y = y4, .z = z4, .color = color_ls[3].argb, .u = img_rect.a.x / tex_w, .v = img_rect.b.y / tex_h },
    };

    auto& ctx = LR2D();
    translate_blend(ctx, blend);
    check_rendertarget_usage(ptex2d);
    ctx.setTextureAlphaType(ptex2d->IsPremultipliedAlpha() ? LuaSTG::Core::TextureAlphaType::PremulAlpha : LuaSTG::Core::TextureAlphaType::Normal);
    ctx.setTexture(LuaSTG::Core::TextureID(ptex2d->GetHandle()));
    ctx.drawQuad(vertex[0], vertex[1], vertex[2], vertex[3]);
}
static void api_drawSprite4V(char const* name, float const x1, float const y1, float const z1, float const x2, float const y2, float const z2, float const x3, float const y3, float const z3, float const x4, float const y4, float const z4)
{
    fcyRefPointer<LuaSTGPlus::ResSprite> pimg2dres = LRESMGR().FindSprite(name);
    if (!pimg2dres)
    {
        spdlog::error("[luastg] lstg.Renderer.drawSprite4V failed, can't find sprite '{}'", name);
        return;
    }
    api_drawSprite4V(*pimg2dres, x1, y1, z1, x2, y2, z2, x3, y3, z3, x4, y4, z4);
}

static void api_drawSpriteSequence(LuaSTGPlus::ResAnimation* pani2dres, int const ani_timer, float const x, float const y, float const rot, float const hscale, float const vscale, float const z)
{
    f2dSprite* pimg2d = pani2dres->GetSprite(((fuInt)ani_timer / pani2dres->GetInterval()) % (fuInt)pani2dres->GetCount());
    auto& ctx = LR2D();

    float const scale = pimg2d->GetScale();
    LuaSTGPlus::BlendMode blend = pani2dres->GetBlendMode();
    translate_blend(ctx, blend);

    f2dTexture2D* const ptex2d = pimg2d->GetTexture();
    check_rendertarget_usage(ptex2d);
    ctx.setTextureAlphaType(ptex2d->IsPremultipliedAlpha() ? LuaSTG::Core::TextureAlphaType::PremulAlpha : LuaSTG::Core::TextureAlphaType::Normal);
    ctx.setTexture(LuaSTG::Core::TextureID(ptex2d->GetHandle()));

    LuaSTG::Core::DrawVertex2D vertex[4];
    make_sprite_vertex(pimg2d, vertex, x, y, rot, hscale * scale, vscale * scale, z);
    ctx.drawQuad(vertex[0], vertex[1], vertex[2], vertex[3]);
}
static void api_drawSpriteSequence(char const* name, int const ani_timer, float const x, float const y, float const rot, float const hscale, float const vscale, float const z)
{
    fcyRefPointer<LuaSTGPlus::ResAnimation> pani2dres = LRESMGR().FindAnimation(name);
    if (!pani2dres)
    {
        spdlog::error("[luastg] lstg.Renderer.drawSpriteSequence failed, can't find sprite sequence '{}'", name);
        return;
    }
    api_drawSpriteSequence(*pani2dres, ani_timer, x, y, rot, hscale, vscale, z);
}

// 下面 api_GameObject_ 系列是要废弃的
static void api_GameObject_updateBlendMode(LuaSTGPlus::BlendMode blend)
{
    translate_blend(LR2D(), blend);
}
static void api_GameObject_drawSprite(LuaSTGPlus::ResSprite* pimg2dres, float const x, float const y, float const rot, float const hscale, float const vscale, float const z)
{
    api_drawSprite(pimg2dres, x, y, rot, hscale, vscale, z);
}
static void api_GameObject_drawSpriteSequence(LuaSTGPlus::ResAnimation* pani2dres, int const ani_timer, float const x, float const y, float const rot, float const hscale, float const vscale, float const z)
{
    api_drawSpriteSequence(pani2dres, ani_timer, x, y, rot, hscale, vscale, z);
}
static void api_GameObject_drawSprite(f2dSprite* pimg2d, float const x, float const y, float const rot, float const hscale, float const vscale, float const z)
{
    auto& ctx = LR2D();

    float const scale = pimg2d->GetScale();

    f2dTexture2D* const ptex2d = pimg2d->GetTexture();
    check_rendertarget_usage(ptex2d);
    ctx.setTextureAlphaType(ptex2d->IsPremultipliedAlpha() ? LuaSTG::Core::TextureAlphaType::PremulAlpha : LuaSTG::Core::TextureAlphaType::Normal);
    ctx.setTexture(LuaSTG::Core::TextureID(ptex2d->GetHandle()));

    LuaSTG::Core::DrawVertex2D vertex[4];
    make_sprite_vertex(pimg2d, vertex, x, y, rot, hscale * scale, vscale * scale, z);
    ctx.drawQuad(vertex[0], vertex[1], vertex[2], vertex[3]);
}
static void api_GameObject_drawParticle(LuaSTGPlus::ResParticle::ParticlePool* p, float hscale, float vscale)
{
    p->Render(LAPP.GetGraphics2D(), hscale, vscale);
}

static void api_setFogState(float start, float end, fcyColor color)
{
    auto& ctx = LR2D();
    if (start != end)
    {
        if (start == -1.0f)
        {
            ctx.setFogState(LuaSTG::Core::FogState::Exp, LuaSTG::Core::Color4B(color.argb), end, 0.0f);
        }
        else if (start == -2.0f)
        {
            ctx.setFogState(LuaSTG::Core::FogState::Exp2, LuaSTG::Core::Color4B(color.argb), end, 0.0f);
        }
        else
        {
            ctx.setFogState(LuaSTG::Core::FogState::Linear, LuaSTG::Core::Color4B(color.argb), start, end);
        }
    }
    else
    {
        ctx.setFogState(LuaSTG::Core::FogState::Disable, LuaSTG::Core::Color4B(), 0.0f, 0.0f);
    }
}

static int lib_beginScene(lua_State* L)LNOEXCEPT
{
    if (!LR2D().beginScene())
        return luaL_error(L, "[luastg] lstg.Renderer.BeginScene failed");
    return 0;
}
static int lib_endScene(lua_State* L)LNOEXCEPT
{
    if (!LR2D().endScene())
        return luaL_error(L, "[luastg] lstg.Renderer.endScene failed");
    return 0;
}

static int lib_clearRenderTarget(lua_State* L)LNOEXCEPT
{
    LuaSTG::Core::Color4B color;
    if (lua_isnumber(L, 1))
    {
        color.u.color = (uint32_t)lua_tonumber(L, 1);
    }
    else
    {
        color.u.color = static_cast<fcyColor*>(luaL_checkudata(L, 1, LUASTG_LUA_TYPENAME_COLOR))->argb;
    }
    LR2D().clearRenderTarget(color);
    return 0;
}
static int lib_clearDepthBuffer(lua_State* L)LNOEXCEPT
{
    LR2D().clearDepthBuffer(luaL_checknumber(L, 1));
    return 0;
}

static int lib_setOrtho(lua_State* L)LNOEXCEPT
{
    LuaSTG::Core::Box box;
    if (lua_gettop(L) < 6)
    {
        box.left = (float)luaL_checknumber(L, 1);
        box.right = (float)luaL_checknumber(L, 2);
        box.bottom = (float)luaL_checknumber(L, 3);
        box.top = (float)luaL_checknumber(L, 4);
        box.front = 0.0f;
        box.back = 1.0f;
    }
    else
    {
        box.left = (float)luaL_checknumber(L, 1);
        box.right = (float)luaL_checknumber(L, 2);
        box.bottom = (float)luaL_checknumber(L, 3);
        box.top = (float)luaL_checknumber(L, 4);
        box.front = (float)luaL_checknumber(L, 5);
        box.back = (float)luaL_checknumber(L, 6);
    }
    LR2D().setOrtho(box);
    return 0;
}
static int lib_setPerspective(lua_State* L)LNOEXCEPT
{
    LuaSTG::Core::Vector3 eye;
    eye.x = (float)luaL_checknumber(L, 1);
    eye.y = (float)luaL_checknumber(L, 2);
    eye.z = (float)luaL_checknumber(L, 3);
    LuaSTG::Core::Vector3 lookat;
    lookat.x = (float)luaL_checknumber(L, 4);
    lookat.y = (float)luaL_checknumber(L, 5);
    lookat.z = (float)luaL_checknumber(L, 6);
    LuaSTG::Core::Vector3 headup;
    headup.x = (float)luaL_checknumber(L, 7);
    headup.y = (float)luaL_checknumber(L, 8);
    headup.z = (float)luaL_checknumber(L, 9);
    LR2D().setPerspective(eye, lookat, headup,
        (float)luaL_checknumber(L, 10),
        (float)luaL_checknumber(L, 11),
        (float)luaL_checknumber(L, 12),
        (float)luaL_checknumber(L, 13));
    return 0;
}

static int lib_setViewport(lua_State* L)LNOEXCEPT
{
    LuaSTG::Core::Box box;
    if (lua_gettop(L) < 6)
    {
        box.left = (float)luaL_checknumber(L, 1);
        box.top = (float)luaL_checknumber(L, 2);
        box.right = (float)luaL_checknumber(L, 3);
        box.bottom = (float)luaL_checknumber(L, 4);
        box.front = 0.0f;
        box.back = 1.0f;
    }
    else
    {
        box.left = (float)luaL_checknumber(L, 1);
        box.top = (float)luaL_checknumber(L, 2);
        box.right = (float)luaL_checknumber(L, 3);
        box.bottom = (float)luaL_checknumber(L, 4);
        box.front = (float)luaL_checknumber(L, 5);
        box.back = (float)luaL_checknumber(L, 6);
    }
    LR2D().setViewport(box);
    return 0;
}
static int lib_setScissorRect(lua_State* L)LNOEXCEPT
{
    LuaSTG::Core::Rect rect;
    rect.left = (float)luaL_checknumber(L, 1);
    rect.top = (float)luaL_checknumber(L, 2);
    rect.right = (float)luaL_checknumber(L, 3);
    rect.bottom = (float)luaL_checknumber(L, 4);
    LR2D().setScissorRect(rect);
    return 0;
}

static int lib_setVertexColorBlendState(lua_State* L)LNOEXCEPT
{
    LR2D().setVertexColorBlendState((LuaSTG::Core::VertexColorBlendState)luaL_checkinteger(L, 1));
    return 0;
}
static int lib_setSamplerState(lua_State* L)LNOEXCEPT
{
    LR2D().setSamplerState((LuaSTG::Core::SamplerState)luaL_checkinteger(L, 1));
    return 0;
}
static int lib_setFogState(lua_State* L)LNOEXCEPT
{
    LuaSTG::Core::Color4B color;
    if (lua_isnumber(L, 2))
    {
        color.u.color = (uint32_t)lua_tonumber(L, 2);
    }
    else
    {
        color.u.color = static_cast<fcyColor*>(luaL_checkudata(L, 2, LUASTG_LUA_TYPENAME_COLOR))->argb;
    }
    LR2D().setFogState(
        (LuaSTG::Core::FogState)luaL_checkinteger(L, 1),
        color,
        (float)luaL_checknumber(L, 3),
        (float)luaL_optnumber(L, 4, 0.0));
    return 0;
}
static int lib_setDepthState(lua_State* L)LNOEXCEPT
{
    LR2D().setDepthState((LuaSTG::Core::DepthState)luaL_checkinteger(L, 1));
    return 0;
}
static int lib_setBlendState(lua_State* L)LNOEXCEPT
{
    LR2D().setBlendState((LuaSTG::Core::BlendState)luaL_checkinteger(L, 1));
    return 0;
}
static int lib_setTexture(lua_State* L)LNOEXCEPT
{
    char const* name = luaL_checkstring(L, 1);
    fcyRefPointer<LuaSTGPlus::ResTexture> p = LRESMGR().FindTexture(name);
    if (!p)
    {
        spdlog::error("[luastg] lstg.Renderer.setTexture failed: can't find texture '{}'", name);
        return false;
    }
    LuaSTG::Core::TextureID tex;
    tex.handle = p->GetTexture()->GetHandle();
    check_rendertarget_usage(p->GetTexture());
    LR2D().setTextureAlphaType(p->GetTexture()->IsPremultipliedAlpha() ? LuaSTG::Core::TextureAlphaType::PremulAlpha : LuaSTG::Core::TextureAlphaType::Normal);
    LR2D().setTexture(tex);
    return 0;
}

static int lib_drawTriangle(lua_State* L)
{
    LuaSTG::Core::DrawVertex2D vertex[3];

    lua_rawgeti(L, 1, 1);
    lua_rawgeti(L, 1, 2);
    lua_rawgeti(L, 1, 3);
    lua_rawgeti(L, 1, 4);
    lua_rawgeti(L, 1, 5);
    lua_rawgeti(L, 1, 6);
    vertex[0].x = (float)luaL_checknumber(L, 4);
    vertex[0].y = (float)luaL_checknumber(L, 5);
    vertex[0].z = (float)luaL_checknumber(L, 6);
    vertex[0].u = (float)luaL_checknumber(L, 7);
    vertex[0].v = (float)luaL_checknumber(L, 8);
    vertex[0].color = (uint32_t)luaL_checknumber(L, 9);
    lua_pop(L, 6);

    lua_rawgeti(L, 2, 1);
    lua_rawgeti(L, 2, 2);
    lua_rawgeti(L, 2, 3);
    lua_rawgeti(L, 2, 4);
    lua_rawgeti(L, 2, 5);
    lua_rawgeti(L, 2, 6);
    vertex[1].x = (float)luaL_checknumber(L, 4);
    vertex[1].y = (float)luaL_checknumber(L, 5);
    vertex[1].z = (float)luaL_checknumber(L, 6);
    vertex[1].u = (float)luaL_checknumber(L, 7);
    vertex[1].v = (float)luaL_checknumber(L, 8);
    vertex[1].color = (uint32_t)luaL_checknumber(L, 9);
    lua_pop(L, 6);

    lua_rawgeti(L, 3, 1);
    lua_rawgeti(L, 3, 2);
    lua_rawgeti(L, 3, 3);
    lua_rawgeti(L, 3, 4);
    lua_rawgeti(L, 3, 5);
    lua_rawgeti(L, 3, 6);
    vertex[2].x = (float)luaL_checknumber(L, 4);
    vertex[2].y = (float)luaL_checknumber(L, 5);
    vertex[2].z = (float)luaL_checknumber(L, 6);
    vertex[2].u = (float)luaL_checknumber(L, 7);
    vertex[2].v = (float)luaL_checknumber(L, 8);
    vertex[2].color = (uint32_t)luaL_checknumber(L, 9);
    lua_pop(L, 6);

    LR2D().drawTriangle(vertex[0], vertex[1], vertex[2]);
    return 0;
}
static int lib_drawQuad(lua_State* L)
{
    LuaSTG::Core::DrawVertex2D vertex[4];

    lua_rawgeti(L, 1, 1);
    lua_rawgeti(L, 1, 2);
    lua_rawgeti(L, 1, 3);
    lua_rawgeti(L, 1, 4);
    lua_rawgeti(L, 1, 5);
    lua_rawgeti(L, 1, 6);
    vertex[0].x = (float)luaL_checknumber(L, 5);
    vertex[0].y = (float)luaL_checknumber(L, 6);
    vertex[0].z = (float)luaL_checknumber(L, 7);
    vertex[0].u = (float)luaL_checknumber(L, 8);
    vertex[0].v = (float)luaL_checknumber(L, 9);
    vertex[0].color = (uint32_t)luaL_checknumber(L, 10);
    lua_pop(L, 6);

    lua_rawgeti(L, 2, 1);
    lua_rawgeti(L, 2, 2);
    lua_rawgeti(L, 2, 3);
    lua_rawgeti(L, 2, 4);
    lua_rawgeti(L, 2, 5);
    lua_rawgeti(L, 2, 6);
    vertex[1].x = (float)luaL_checknumber(L, 5);
    vertex[1].y = (float)luaL_checknumber(L, 6);
    vertex[1].z = (float)luaL_checknumber(L, 7);
    vertex[1].u = (float)luaL_checknumber(L, 8);
    vertex[1].v = (float)luaL_checknumber(L, 9);
    vertex[1].color = (uint32_t)luaL_checknumber(L, 10);
    lua_pop(L, 6);

    lua_rawgeti(L, 3, 1);
    lua_rawgeti(L, 3, 2);
    lua_rawgeti(L, 3, 3);
    lua_rawgeti(L, 3, 4);
    lua_rawgeti(L, 3, 5);
    lua_rawgeti(L, 3, 6);
    vertex[2].x = (float)luaL_checknumber(L, 5);
    vertex[2].y = (float)luaL_checknumber(L, 6);
    vertex[2].z = (float)luaL_checknumber(L, 7);
    vertex[2].u = (float)luaL_checknumber(L, 8);
    vertex[2].v = (float)luaL_checknumber(L, 9);
    vertex[2].color = (uint32_t)luaL_checknumber(L, 10);
    lua_pop(L, 6);

    lua_rawgeti(L, 4, 1);
    lua_rawgeti(L, 4, 2);
    lua_rawgeti(L, 4, 3);
    lua_rawgeti(L, 4, 4);
    lua_rawgeti(L, 4, 5);
    lua_rawgeti(L, 4, 6);
    vertex[3].x = (float)luaL_checknumber(L, 5);
    vertex[3].y = (float)luaL_checknumber(L, 6);
    vertex[3].z = (float)luaL_checknumber(L, 7);
    vertex[3].u = (float)luaL_checknumber(L, 8);
    vertex[3].v = (float)luaL_checknumber(L, 9);
    vertex[3].color = (uint32_t)luaL_checknumber(L, 10);
    lua_pop(L, 6);

    LR2D().drawQuad(vertex[0], vertex[1], vertex[2], vertex[3]);
    return 0;
}

static int lib_drawSprite(lua_State* L)
{
    float const hscale = (float)luaL_optnumber(L, 5, 1.0);
    api_drawSprite(
        luaL_checkstring(L, 1),
        (float)luaL_checknumber(L, 2), (float)luaL_checknumber(L, 3),
        (float)luaL_optnumber(L, 4, 0.0) * LDEGREE2RAD,
        hscale * LRESMGR().GetGlobalImageScaleFactor(), (float)luaL_optnumber(L, 6, hscale) * LRESMGR().GetGlobalImageScaleFactor(),
        (float)luaL_optnumber(L, 7, 0.5));
    return 0;
}
static int lib_drawSpriteRect(lua_State* L)
{
    api_drawSpriteRect(
        luaL_checkstring(L, 1),
        (float)luaL_checknumber(L, 2), (float)luaL_checknumber(L, 3),
        (float)luaL_checknumber(L, 4), (float)luaL_checknumber(L, 5),
        (float)luaL_optnumber(L, 6, 0.5));
    return 0;
}
static int lib_drawSprite4V(lua_State* L)
{
    api_drawSprite4V(
        luaL_checkstring(L, 1),
        (float)luaL_checknumber(L, 2), (float)luaL_checknumber(L, 3), (float)luaL_checknumber(L, 4),
        (float)luaL_checknumber(L, 5), (float)luaL_checknumber(L, 6), (float)luaL_checknumber(L, 7),
        (float)luaL_checknumber(L, 8), (float)luaL_checknumber(L, 9), (float)luaL_checknumber(L, 10),
        (float)luaL_checknumber(L, 11), (float)luaL_checknumber(L, 12), (float)luaL_checknumber(L, 13));
    return 0;
}

static int lib_drawSpriteSequence(lua_State* L)
{
    float const hscale = (float)luaL_optnumber(L, 6, 1.0);
    api_drawSpriteSequence(
        luaL_checkstring(L, 1),
        (int)luaL_checkinteger(L, 2),
        (float)luaL_checknumber(L, 3), (float)luaL_checknumber(L, 4),
        (float)luaL_optnumber(L, 5, 0.0) * LDEGREE2RAD,
        hscale * LRESMGR().GetGlobalImageScaleFactor(), (float)luaL_optnumber(L, 7, hscale) * LRESMGR().GetGlobalImageScaleFactor(),
        (float)luaL_optnumber(L, 8, 0.5));
    return 0;
}

static int lib_drawTexture(lua_State* L)LNOEXCEPT
{
    const char* name = luaL_checkstring(L, 1);
    LuaSTGPlus::BlendMode blend = LuaSTGPlus::TranslateBlendMode(L, 2);
    LuaSTG::Core::DrawVertex2D vertex[4];

    for (int i = 0; i < 4; ++i)
    {
        lua_pushinteger(L, 1);
        lua_gettable(L, 3 + i);
        vertex[i].x = (float)lua_tonumber(L, -1);

        lua_pushinteger(L, 2);
        lua_gettable(L, 3 + i);
        vertex[i].y = (float)lua_tonumber(L, -1);

        lua_pushinteger(L, 3);
        lua_gettable(L, 3 + i);
        vertex[i].z = (float)lua_tonumber(L, -1);

        lua_pushinteger(L, 4);
        lua_gettable(L, 3 + i);
        vertex[i].u = (float)lua_tonumber(L, -1);

        lua_pushinteger(L, 5);
        lua_gettable(L, 3 + i);
        vertex[i].v = (float)lua_tonumber(L, -1);

        lua_pushinteger(L, 6);
        lua_gettable(L, 3 + i);
        if (lua_isnumber(L, -1))
        {
            vertex[i].color = (uint32_t)lua_tonumber(L, -1);
        }
        else
        {
            vertex[i].color = static_cast<fcyColor*>(luaL_checkudata(L, -1, LUASTG_LUA_TYPENAME_COLOR))->argb;
        }

        lua_pop(L, 6);
    }

    auto& ctx = LR2D();

    translate_blend(ctx, blend);

    fcyRefPointer<LuaSTGPlus::ResTexture> ptex2dres = LRESMGR().FindTexture(name);
    if (!ptex2dres)
    {
        spdlog::error("[luastg] lstg.Renderer.drawTexture failed: can't find texture '{}'", name);
        return false;
    }
    f2dTexture2D* const ptex2d = ptex2dres->GetTexture();
    float const tex_w = 1.0f / (float)ptex2d->GetWidth();
    float const tex_h = 1.0f / (float)ptex2d->GetHeight();
    for (int i = 0; i < 4; ++i)
    {
        vertex[i].u *= tex_w;
        vertex[i].v *= tex_h;
    }
    check_rendertarget_usage(ptex2d);
    ctx.setTextureAlphaType(ptex2d->IsPremultipliedAlpha() ? LuaSTG::Core::TextureAlphaType::PremulAlpha : LuaSTG::Core::TextureAlphaType::Normal);
    ctx.setTexture(LuaSTG::Core::TextureID(ptex2d->GetHandle()));

    ctx.drawQuad(vertex[0], vertex[1], vertex[2], vertex[3]);

    return 0;
}

static int lib_drawModel(lua_State* L)
{
    const char* name = luaL_checkstring(L, 1);

    float const x = (float)luaL_checknumber(L, 2);
    float const y = (float)luaL_checknumber(L, 3);
    float const z = (float)luaL_checknumber(L, 4);

    float const roll  = (float)(L_DEG_TO_RAD * luaL_optnumber(L, 5, 0.0));
    float const pitch = (float)(L_DEG_TO_RAD * luaL_optnumber(L, 6, 0.0));
    float const yaw   = (float)(L_DEG_TO_RAD * luaL_optnumber(L, 7, 0.0));

    float const sx = (float)luaL_optnumber(L, 8, 1.0);
    float const sy = (float)luaL_optnumber(L, 9, 1.0);
    float const sz = (float)luaL_optnumber(L, 10, 1.0);

    fcyRefPointer<LuaSTGPlus::ResModel> pmodres = LRESMGR().FindModel(name);
    if (!pmodres)
    {
        spdlog::error("[luastg] lstg.Renderer.drawModel failed: can't find model '{}'", name);
        return false;
    }

    auto& ctx = LR2D();

    pmodres->GetModel()->setScaling(LuaSTG::Core::Vector3(sx, sy, sz));
    pmodres->GetModel()->setRotationRollPitchYaw(roll, pitch, yaw);
    pmodres->GetModel()->setPosition(LuaSTG::Core::Vector3(x, y, z));
    ctx.drawModel(pmodres->GetModel());

    return 0;
}

#define MKFUNC(X) {#X, &lib_##X}

static luaL_Reg const lib_func[] = {
    MKFUNC(beginScene),
    MKFUNC(endScene),

    MKFUNC(clearRenderTarget),
    MKFUNC(clearDepthBuffer),

    MKFUNC(setOrtho),
    MKFUNC(setPerspective),

    MKFUNC(setViewport),
    MKFUNC(setScissorRect),

    MKFUNC(setVertexColorBlendState),
    MKFUNC(setSamplerState),
    MKFUNC(setFogState),
    MKFUNC(setDepthState),
    MKFUNC(setBlendState),
    MKFUNC(setTexture),

    MKFUNC(drawTriangle),
    MKFUNC(drawQuad),

    MKFUNC(drawSprite),
    MKFUNC(drawSpriteRect),
    MKFUNC(drawSprite4V),

    MKFUNC(drawSpriteSequence),

    MKFUNC(drawTexture),

    { NULL, NULL },
};

static int compat_SetViewport(lua_State* L)LNOEXCEPT
{
    LuaSTG::Core::Box box;
    box.left = (float)luaL_checknumber(L, 1);
    box.top = (float)luaL_checknumber(L, 4);
    box.right = (float)luaL_checknumber(L, 2);
    box.bottom = (float)luaL_checknumber(L, 3);
    if (lua_gettop(L) < 6)
    {
        box.front = 0.0f;
        box.back = 1.0f;
    }
    else
    {
        box.front = (float)luaL_checknumber(L, 5);
        box.back = (float)luaL_checknumber(L, 6);
    }
    if (auto* p = LRDEV()->GetRenderTarget())
    {
        box.top = (float)p->GetHeight() - box.top;
        box.bottom = (float)p->GetHeight() - box.bottom;
    }
    else
    {
        box.top = (float)LRDEV()->GetBufferHeight() - box.top;
        box.bottom = (float)LRDEV()->GetBufferHeight() - box.bottom;
    }
    LR2D().setViewport(box);
    return 0;
}
static int compat_SetScissorRect(lua_State* L)LNOEXCEPT
{
    LuaSTG::Core::Rect rect;
    rect.left = (float)luaL_checknumber(L, 1);
    rect.top = (float)luaL_checknumber(L, 4);
    rect.right = (float)luaL_checknumber(L, 2);
    rect.bottom = (float)luaL_checknumber(L, 3);
    if (auto* p = LRDEV()->GetRenderTarget())
    {
        rect.top = (float)p->GetHeight() - rect.top;
        rect.bottom = (float)p->GetHeight() - rect.bottom;
    }
    else
    {
        rect.top = (float)LRDEV()->GetBufferHeight() - rect.top;
        rect.bottom = (float)LRDEV()->GetBufferHeight() - rect.bottom;
    }
    LR2D().setScissorRect(rect);
    return 0;
}
static int compat_SetFog(lua_State* L)LNOEXCEPT
{
    int const argc = lua_gettop(L);
    if (argc >= 3)
    {
        api_setFogState(
            static_cast<float>(luaL_checknumber(L, 1)),
            static_cast<float>(luaL_checknumber(L, 2)),
            *(static_cast<fcyColor*>(luaL_checkudata(L, 3, LUASTG_LUA_TYPENAME_COLOR)))
        );
    }
    else if (argc == 2)
    {
        api_setFogState(
            static_cast<float>(luaL_checknumber(L, 1)),
            static_cast<float>(luaL_checknumber(L, 2)),
            0xFF000000
        );
    }
    else
    {
        api_setFogState(0.0f, 0.0f, 0x00000000);
    }
    return 0;
}
static int compat_SetZBufferEnable(lua_State* L)LNOEXCEPT
{
    LR2D().setDepthState((LuaSTG::Core::DepthState)luaL_checkinteger(L, 1));
    return 0;
}
static int compat_ClearZBuffer(lua_State* L)LNOEXCEPT
{
    LR2D().clearDepthBuffer(luaL_optnumber(L, 1, 1.0));
    return 0;
}
static int compat_PushRenderTarget(lua_State* L)LNOEXCEPT
{
    LR2D().flush();
    LuaSTGPlus::ResTexture* p = LRES.FindTexture(luaL_checkstring(L, 1));
    if (!p)
        return luaL_error(L, "rendertarget '%s' not found.", luaL_checkstring(L, 1));
    if (!p->IsRenderTarget())
        return luaL_error(L, "'%s' is a texture.", luaL_checkstring(L, 1));

    if (!LAPP.PushRenderTarget(p))
        return luaL_error(L, "push rendertarget '%s' failed.", luaL_checkstring(L, 1));
    LR2D().setViewportAndScissorRect();
    return 0;
}
static int compat_PopRenderTarget(lua_State* L)LNOEXCEPT
{
    LR2D().flush();
    if (!LAPP.PopRenderTarget())
        return luaL_error(L, "pop rendertarget failed.");
    LR2D().setViewportAndScissorRect();
    return 0;
}
static int compat_PostEffect(lua_State* L)
{
    const char* ps_name = luaL_checkstring(L, 1);
    const char* rt_name = luaL_checkstring(L, 2);
    const LuaSTG::Core::SamplerState rtsv = (LuaSTG::Core::SamplerState)luaL_checkinteger(L, 3);
    const LuaSTG::Core::BlendState blend = translate_blend_3d(LuaSTGPlus::TranslateBlendMode(L, 4));

    LuaSTGPlus::ResFX* pfx = LRES.FindFX(ps_name);
    if (!pfx)
        return luaL_error(L, "effect '%s' not found.", ps_name);
    LuaSTG::Core::ShaderID ps(pfx->GetPixelShader());
    
    LuaSTGPlus::ResTexture* prt = LRES.FindTexture(rt_name);
    if (!prt)
        return luaL_error(L, "texture '%s' not found.", rt_name);
    check_rendertarget_usage(prt->GetTexture());
    LuaSTG::Core::TextureID rt(prt->GetTexture()->GetHandle());

    LuaSTG::Core::Vector4 cbdata[8] = {};
    LuaSTG::Core::TextureID tdata[4] = {};
    LuaSTG::Core::SamplerState tsdata[4] = {};
    
    size_t cbdata_n = lua_objlen(L, 5);
    cbdata_n = (cbdata_n <= 8) ? cbdata_n : 8;
    for (int i = 1; i <= cbdata_n; i += 1)
    {
        lua_rawgeti(L, 5, i);  // ??? t
        lua_rawgeti(L, -1, 1); // ??? t f1
        lua_rawgeti(L, -2, 2); // ??? t f1 f2
        lua_rawgeti(L, -3, 3); // ??? t f1 f2 f3
        lua_rawgeti(L, -4, 4); // ??? t f1 f2 f3 f4
        cbdata[i - 1].x = (float)luaL_checknumber(L, -4);
        cbdata[i - 1].y = (float)luaL_checknumber(L, -3);
        cbdata[i - 1].z = (float)luaL_checknumber(L, -2);
        cbdata[i - 1].w = (float)luaL_checknumber(L, -1);
        lua_pop(L, 5);
    }
    size_t tdata_n = lua_objlen(L, 6);
    tdata_n = (tdata_n <= 8) ? tdata_n : 4;
    for (int i = 1; i <= tdata_n; i += 1)
    {
        lua_rawgeti(L, 6, i);  // ??? t
        lua_rawgeti(L, -1, 1); // ??? t tex
        lua_rawgeti(L, -2, 2); // ??? t tex sampler
        const char* tx_name = luaL_checkstring(L, -2);
        LuaSTGPlus::ResTexture* ptex = LRES.FindTexture(tx_name);
        if (!ptex)
            return luaL_error(L, "texture '%s' not found.", tx_name);
        check_rendertarget_usage(ptex->GetTexture());
        tdata[i - 1] = LuaSTG::Core::TextureID(ptex->GetTexture()->GetHandle());
        tsdata[i - 1] = (LuaSTG::Core::SamplerState)luaL_checkinteger(L, -1);
    }

    LR2D().postEffect(ps, rt, rtsv, cbdata, cbdata_n, tdata, tsdata, tdata_n, blend);

    return 0;
}

// 应该要废弃掉的方法
static int compat_SetTextureSamplerState(lua_State* L)LNOEXCEPT
{
    static F2DTEXFILTERTYPE last_filter = F2DTEXFILTER_LINEAR;
    static F2DTEXTUREADDRESS last_addr = F2DTEXTUREADDRESS_CLAMP;
    size_t arg1_l;
    const char* arg1_s = luaL_checklstring(L, 1, &arg1_l);
    std::string_view arg1(arg1_s, arg1_l);
    if (arg1 == "address")
    {
        F2DTEXTUREADDRESS arg2 = LuaSTGPlus::TranslateTextureSamplerAddress(L, 2);
        switch (arg2)
        {
        case F2DTEXTUREADDRESS_WRAP:
            if (last_filter == F2DTEXFILTER_LINEAR)
            {
                LR2D().setSamplerState(LuaSTG::Core::SamplerState::LinearWrap);
            }
            else
            {
                LR2D().setSamplerState(LuaSTG::Core::SamplerState::PointWrap);
            }
            break;
        case F2DTEXTUREADDRESS_CLAMP:
            if (last_filter == F2DTEXFILTER_LINEAR)
            {
                LR2D().setSamplerState(LuaSTG::Core::SamplerState::LinearClamp);
            }
            else
            {
                LR2D().setSamplerState(LuaSTG::Core::SamplerState::PointClamp);
            }
            break;
        default:
            return luaL_error(L, "invalid argument address mode.");
        }
        last_addr = arg2;
    }
    else if (arg1 == "filter")
    {
        F2DTEXFILTERTYPE arg2 = LuaSTGPlus::TranslateTextureSamplerFilter(L, 2);
        switch (arg2)
        {
        case F2DTEXFILTER_POINT:
            if (last_addr == F2DTEXTUREADDRESS_CLAMP)
            {
                LR2D().setSamplerState(LuaSTG::Core::SamplerState::PointClamp);
            }
            else
            {
                LR2D().setSamplerState(LuaSTG::Core::SamplerState::PointWrap);
            }
            break;
        case F2DTEXFILTER_LINEAR:
            if (last_addr == F2DTEXTUREADDRESS_CLAMP)
            {
                LR2D().setSamplerState(LuaSTG::Core::SamplerState::LinearClamp);
            }
            else
            {
                LR2D().setSamplerState(LuaSTG::Core::SamplerState::LinearWrap);
            }
            break;
        default:
            return luaL_error(L, "invalid argument address mode.");
        }
        last_filter = arg2;
    }
    else
    {
        return luaL_error(L, "invalid argument '%s'.", arg1.data());
    }
    return 0;
}

static int compat_Noop(lua_State* L)
{
    return 0;
}

static luaL_Reg const lib_compat[] = {
    { "BeginScene", &lib_beginScene },
    { "EndScene", &lib_endScene },
    { "RenderClear", &lib_clearRenderTarget },
    { "SetViewport", &compat_SetViewport },
    { "SetScissorRect", &compat_SetScissorRect },
    { "SetOrtho", &lib_setOrtho },
    { "SetPerspective", &lib_setPerspective },
    { "Render", &lib_drawSprite },
    { "RenderRect", &lib_drawSpriteRect },
    { "Render4V", &lib_drawSprite4V },
    { "RenderAnimation", &lib_drawSpriteSequence },
    { "RenderTexture", &lib_drawTexture },
    { "RenderModel", &lib_drawModel },
    { "SetFog", &compat_SetFog },
    { "SetZBufferEnable", &compat_SetZBufferEnable },
    { "ClearZBuffer", &compat_ClearZBuffer },
    { "PushRenderTarget", &compat_PushRenderTarget },
    { "PopRenderTarget", &compat_PopRenderTarget },
    { "PostEffect", &compat_PostEffect },
    // 应该要废弃掉的方法
    { "SetTextureSamplerState", &compat_SetTextureSamplerState },
    // 置为空方法
    { "DrawCollider", &compat_Noop },
    { "RenderGroupCollider", &compat_Noop },
    { "RenderTextureSector", &compat_Noop },
    { "RenderTextureAnnulus", &compat_Noop },
    { NULL, NULL },
};

void LuaSTGPlus::LuaWrapper::RendererWrapper::Register(lua_State* L) LNOEXCEPT
{
    static luaL_Reg const lib_empty[] = {
        { NULL, NULL },
    };
    luaL_register(L, LUASTG_LUA_LIBNAME, lib_compat);           // ??? lstg
    luaL_register(L, LUASTG_LUA_LIBNAME ".Renderer", lib_func); // ??? lstg lstg.Renderer
    lua_setfield(L, -1, "Renderer");                            // ??? lstg
    lua_pop(L, 1);                                              // ???
}
