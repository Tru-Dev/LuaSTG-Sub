﻿#include "GameObjectBentLaser.hpp"
#include "AppFrame.h"

using namespace std;
using namespace LuaSTGPlus;

//======================================

static fcyMemPool<sizeof(GameObjectBentLaser)> s_GameObjectBentLaserPool(1024);

GameObjectBentLaser* GameObjectBentLaser::AllocInstance()
{
	//潜在bad_alloc已在luawrapper中处理
	GameObjectBentLaser* pRet = new(s_GameObjectBentLaserPool.Alloc()) GameObjectBentLaser();
	return pRet;
}

void GameObjectBentLaser::FreeInstance(GameObjectBentLaser* p)
{
	p->~GameObjectBentLaser();
	s_GameObjectBentLaserPool.Free(p);
}

GameObjectBentLaser::GameObjectBentLaser() noexcept
{
}

GameObjectBentLaser::~GameObjectBentLaser() noexcept
{
}

//======================================

void GameObjectBentLaser::_UpdateNode(size_t i) noexcept
{
	// 当前节点，先去掉大转角标记
	LaserNode& node = m_Queue[i];
	node.sharp = false;

	// 只有一个节点，也就是自己的情况
	if (m_Queue.Size() <= 1)
	{
		// 不渲染
		node.x_dir = 0.0f;
		node.y_dir = 0.0f;
		return;
	}

	// 对首尾的情况特殊处理
	if (i == 0)
	{
		// 是首节点（老节点），利用下一个节点的位置来计算节点朝向
		LaserNode& next = m_Queue[i + 1]; // 下一个节点（相对更新的节点）
		fcyVec2 const vec = next.pos - node.pos;

		// 逆时针旋转 90 度
		fcyVec2 const vecr(vec.y, -vec.x);

		// 归一化向量，保存
		fcyVec2 const vecn = vecr.GetNormalize();
		node.x_dir = vecn.x;
		node.y_dir = vecn.y;
		return;
	}
	else if (i == (m_Queue.Size() - 1))
	{
		// 是尾节点（新节点），利用上一个节点的位置来计算节点朝向
		LaserNode& last = m_Queue[i - 1]; // 上一个节点（相对更老的节点）
		fcyVec2 const vec = node.pos - last.pos;

		// 逆时针旋转 90 度
		fcyVec2 const vecr(vec.y, -vec.x);

		// 归一化向量，保存
		fcyVec2 const vecn = vecr.GetNormalize();
		node.x_dir = vecn.x;
		node.y_dir = vecn.y;
		return;
	}

	// 节点在中间，两侧均有节点
	LaserNode& last = m_Queue[i - 1]; // 上一个节点（相对更老的节点）
	LaserNode& next = m_Queue[i + 1]; // 下一个节点（相对更新的节点）

	// 计算向量以及点积
	fcyVec2 const vec1 = node.pos - last.pos;
	fcyVec2 const vec2 = next.pos - node.pos;
	fFloat const dotv = vec1 * vec2;

	// 转角小于 90 度的情况
	if (dotv > 0.0f)
	{
		// 将两个向量都逆时针旋转 90 度
		// A = -pi / 2
		// cosA = 0
		// sinA = -1
		// x = X * cosA - Y * sinA = Y
		// y = X * sinA + Y * cosA = -X
		fcyVec2 const vec1r(vec1.y, -vec1.x);
		fcyVec2 const vec2r(vec2.y, -vec2.x);
		
		// 向量求和，得到延展向量
		fcyVec2 const vec = vec1r + vec2r;

		// 归一化向量，保存
		fcyVec2 const vecn = vec.GetNormalize();
		node.x_dir = vecn.x;
		node.y_dir = vecn.y;
		return;
	}

	// 转角大于等于 90 度的情况
	else
	{
		// 向量求和，得到延展向量
		fcyVec2 const vec = -vec1 + vec2;

		// 归一化向量，保存
		fcyVec2 const vecn = vec.GetNormalize();
		node.x_dir = vecn.x;
		node.y_dir = vecn.y;
		return;
	}

	// EX+ 的写法
	/*
	LaserNode& cur = m_Queue[i];
	cur.sharp = false;
	int sz = m_Queue.Size() - 1;
	if (sz > 0)
	{
		float curcos = 0.0f, cursin = 0.0f;
		float nextcos = 0.0f, nextsin = 0.0f;
		cursin = sin(cur.rot);
		curcos = cos(cur.rot);
		if (i == 0 || i == sz)
		{
			//头节点或为尾节点，顶点朝向为节点朝向的垂直方向
			cur.x_dir = cursin;
			cur.y_dir = -curcos;
			return; // 不需要继续计算
		}
		else
		{
			//中间节点，计算出下一个节点的朝向
			LaserNode& next = m_Queue[i + 1];
			nextcos = cos(next.rot);
			nextsin = sin(next.rot);
		}
		float d1 = curcos * nextsin - nextcos * cursin; // 行列式，平行四边形的面积
		if (d1 < 0.3 && d1 > -0.3) {
			//处于较小的锐角或较大的钝角时
			float d2 = curcos * nextcos + cursin * nextsin; // 点积
			if (d2 > 0.0f) {
				//锐角，曲线激光比较平直
				if (d1<0.01 && d1>-0.01)
				{
					//平行四边形法则
					cur.x_dir = (cursin + nextsin) / 2;
					cur.y_dir = (-curcos - nextcos) / 2;
					return;//不需要继续计算
				}
			}
			else {
				//钝角，曲线激光弯曲程度较大
				cur.sharp = true;
				//翻转向量
				nextcos = -nextcos;
				nextsin = -nextsin;
				d1 = -d1;
			}
		}
		//处于接近90度的锐角或钝角时
		cur.x_dir = (curcos - nextcos) / d1;
		cur.y_dir = (cursin - nextsin) / d1;
	}
	//*/
}

void GameObjectBentLaser::_UpdateAllNode() noexcept
{
	// 无论如何都重置长度
	m_fLength = 0.0f;

	// 检查节点数量
	size_t const node_count = m_Queue.Size();
	if (node_count == 0)
	{
		return;
	}
	else if (node_count == 1)
	{
		LaserNode& node = m_Queue[0];
		//node.rot = 0.0f; // 保留不变
		node.dis = 0.0f;
		node.x_dir = 0.0f;
		node.y_dir = 0.0f;
		node.sharp = false;
		return;
	}

	// 更新所有节点间的距离和节点的朝向以及总长度，注意，尾部才是新节点，头部是最早的老节点
	m_Queue[0].dis = 0.0f; // 已经没有上一个节点了，它就是最老的那个
	for (size_t i = 0; i < (node_count - 1); i += 1)
	{
		LaserNode& last = m_Queue[i];     // 靠近头部（老节点）
		LaserNode& next = m_Queue[i + 1]; // 靠近尾部（新节点）
		fcyVec2 const vec = next.pos - last.pos;
		fFloat const len = vec.Length();
		if (len > std::numeric_limits<float>::min())
		{
			next.rot = vec.CalcuAngle(); // 相对上一个节点的角度就是自身的朝向
		}
		else
		{
			next.rot = last.rot; // 特殊情况，距离太小计算出来的朝向无意义，直接用上一个节点的朝向
		}
		// 节点间距离，与上一个节点的距离
		next.dis = len;
		// 总长度
		if (last.active && next.active)
		{
			m_fLength += len; // TODO: 可能会影响渲染
		}
	}
	m_Queue[0].rot = m_Queue[1].rot; // 让最老的节点的朝向也与下一个节点的一致，这里这么做的原因是，所有的点的位置都被修改了，因此它的朝向可能已经过时

	// 更新所有节点的延展向量
	for (size_t i = 0u; i < node_count; i += 1)
	{
		_UpdateNode(i);
	}
}

void GameObjectBentLaser::_PopHead() noexcept
{
	if (m_Queue.Size() > 1)
	{
		LaserNode last; // 最老的节点
		m_Queue.Pop(last);
		if (m_Queue.Size() > 1)
		{
			LaserNode& next = m_Queue.Front(); // 下一个节点
			// 如果最后两个节点都是激活的，根据节点间的距离减少曲线激光总长度
			if (last.active && next.active)
			{
				m_fLength -= next.dis; // 到上一个节点的距离
			}
			// 让 next 成为最老的节点
			next.dis = 0.0f; // 已经没有上一个节点
		}
	}
}

int GameObjectBentLaser::GetSize() noexcept
{
	return (int)m_Queue.Size();
}

GameObjectBentLaser::LaserNode* GameObjectBentLaser::GetNode(size_t i) noexcept
{
	if (m_Queue.Size() > 0u)
	{
		if (i >= 0u && i < m_Queue.Size())
		{
			return &(m_Queue[i]);
		}
	}
	return nullptr;
}

void GameObjectBentLaser::GetEnvelope(float& height, float& base, float& rate, float& power) noexcept
{
	height = m_fEnvelopeHeight;
	base = m_fEnvelopeBase;
	rate = m_fEnvelopeRate;
	power = m_fEnvelopePower;
}

void GameObjectBentLaser::SetEnvelope(float height, float base, float rate, float power) noexcept
{
	m_fEnvelopeHeight = height;
	m_fEnvelopeBase = std::clamp(base, 0.0f, 1.0f);
	m_fEnvelopeRate = rate;
	m_fEnvelopePower = 0.4f * std::floorf(power / 0.4f); // 不要问，问就是魔法数字
}

bool GameObjectBentLaser::Update(size_t id, int length, float width, bool active) noexcept
{
	GameObject* p = LPOOL.GetPooledObject(id);
	if (!p)
	{
		spdlog::error("[luastg] [GameObjectBentLaser::Update] 无效的lstg.GameObject");
		return false;
	}
	if (length <= 1)
	{
		spdlog::error("[luastg] [GameObjectBentLaser::Update] 无效的参数length={}", length);
		return false;
	}
	
	// ！循环队列的头部是最早创建的，尾部才是最新放入的！
	
	// 准备插入的新节点
	LaserNode tNode;
	tNode.pos.Set((float)p->x, (float)p->y);
	tNode.half_width = width * 0.5f;
	tNode.active = active;
	
	// 检查是否有必要更新节点
	bool lvalid = false;
	fcyVec2 dpos;
	fFloat len = 0.0f;
	bool lactive = false;
	if (m_Queue.Size() > 0)
	{
		lvalid = true;
		LaserNode& tNodeLast = m_Queue.Back();
		dpos = tNode.pos - tNodeLast.pos;
		len = dpos.Length();
		lactive = tNodeLast.active;
		if (len <= std::numeric_limits<fFloat>::min())
		{
			// 仍然需要更新节点数量
			// 移除多余的节点，保证长度在 length 范围内
			while (m_Queue.Size() >= (size_t)length)
			{
				_PopHead();
			}
			return true; // 变化几乎可以忽略不计，不插入该节点
		}
	}
	
	// 移除多余的节点，保证长度在 length - 1 范围内
	while (m_Queue.IsFull() || m_Queue.Size() >= (size_t)length)
	{
		_PopHead();
	}
	
	// 计算
	if (lvalid && m_Queue.Size() > 0)
	{
		tNode.dis = len; // 距离
		if (active && lactive)
		{
			m_fLength += len;// 增加总长度
		}
		tNode.rot = dpos.CalcuAngle(); // 【即将废弃】计算节点朝向
	}
	else
	{
		tNode.dis = 0.0f;
		fcyVec2 speed((fFloat)p->vy, (fFloat)p->vx);
		if (speed.Length() > std::numeric_limits<float>::min())
		{
			tNode.rot = speed.CalcuAngle(); // 【即将废弃】使用速度方向作为节点朝向
		}
		else
		{
			tNode.rot = (float)p->rot; // 【即将废弃】使用游戏对象朝向作为节点朝向
		}
	}

	// 插入并更新最新的两个节点
	m_Queue.Push(tNode);
	_UpdateNode(m_Queue.Size() - 1);
	if (m_Queue.Size() >= 2)
	{
		_UpdateNode(m_Queue.Size() - 2);
	}
	return true;
}

void GameObjectBentLaser::SetAllWidth(float width)  noexcept
{
	for (size_t i = 0; i < m_Queue.Size(); i += 1)
	{
		m_Queue[i].half_width = width / 2.0f;
	}
}

bool GameObjectBentLaser::Render(const char* tex_name, BlendMode blend, fcyColor c, float tex_left, float tex_top, float tex_width, float tex_height, float scale) noexcept
{
	using namespace LuaSTG::Core;
	using namespace LuaSTG::Core::Graphics;

	// 忽略只有一个节点的情况
	if (m_Queue.Size() <= 1)
		return true;

	// 首先拿到纹理
	fcyRefPointer<ResTexture> pTex = LRES.FindTexture(tex_name);
	if (!pTex)
	{
		spdlog::error("[luastg] [GameObjectBentLaser::Render] 找不到纹理'{}'", tex_name);
		return false;
	}

	// 设置纹理、混合模式等
	auto* p_renderer = LAPP.GetAppModel()->getRenderer();
	LAPP.updateGraph2DBlendMode(blend);
	p_renderer->setTexture(pTex->GetTexture());

	// 分配顶点和索引
	// 顶点总共需要：节点数 * 2
	// 索引总共需要：(节点数 - 1) * 3 * 2
	// 两个节点之间组成一个四边形
	uint16_t const node_count = (uint16_t)m_Queue.Size();
	IRenderer::DrawVertex* p_vertex = nullptr;
	IRenderer::DrawIndex* p_index = nullptr;
	uint16_t index_offset = 0;
	if (!p_renderer->drawRequest(
		node_count * 2,
		(node_count - 1) * 6,
		&p_vertex,
		&p_index,
		&index_offset)) return false; // 分配空间失败了

	// 归一化 uv 坐标
	float const u_scale = 1.0f / (float)pTex->GetTexture()->getSize().x;
	float const v_scale = 1.0f / (float)pTex->GetTexture()->getSize().y;
	float const v_top = tex_top * v_scale;
	float const v_bottom = (tex_top + tex_height) * v_scale;

	// TODO: 原 Ex Plus 的逻辑里还有跳过连续的非 active 的节点的优化
	// if (!cur.active || !next.active) continue;
	// 得思考一下如何加进去

	// 第一部分：填充顶点，从老节点到新节点
	// 0---2---4---6
	// |\  |\  |\  |
	// | \ | \ | \ |
	// |  \|  \|  \|
	// 1---3---5---7
	float total_length = 0.0f;
	bool flip = false;
	uint32_t const vertex_color = c.argb;
	c.a = 0;
	uint32_t const vertex_color_alpha = c.argb;
	IRenderer::DrawVertex* p_vert = p_vertex;
	for (size_t i = 0; i < node_count; i += 1)
	{
		LaserNode& node = m_Queue[i];

		// 拐成钝角，需要翻转一下延展方向
		if (node.sharp)
		{
			flip = !flip;
		}

		// 计算总长度，尾部节点到上一个节点的距离固定为 0
		total_length += node.dis;

		// 计算 u 坐标（像素坐标）
		float tex_u = tex_left + (total_length / m_fLength) * tex_width;

		// 计算延展向量，逆时针垂直于节点朝向
		float pos_x = node.x_dir * scale * node.half_width;
		float pos_y = node.y_dir * scale * node.half_width;
		if (flip)
		{
			pos_x = -pos_x;
			pos_y = -pos_y;
		}

		// 填充顶点，顶点沿着节点向两侧延展
		p_vert[0] = IRenderer::DrawVertex(
			node.pos.x - pos_x,
			node.pos.y - pos_y,
			0.5f,
			tex_u * u_scale,
			v_top,
			node.active ? vertex_color : vertex_color_alpha
		);
		p_vert[1] = IRenderer::DrawVertex(
			node.pos.x + pos_x,
			node.pos.y + pos_y,
			0.5f,
			tex_u * u_scale,
			v_bottom,
			node.active ? vertex_color : vertex_color_alpha
		);

		// 已使用 2 个顶点，接下来不要再修改这些顶点
		p_vert += 2;
	}

	// 第二部分：填充索引
	// 0 0-->2 2 2-->4 4 4-->6
	// |\ \  | |\ \  | |\ \  |
	// | \ \ | | \ \ | | \ \ |
	// |  \ \| |  \ \| |  \ \|
	// 1<--3 3 3<--5 5 5<--7 7
	IRenderer::DrawIndex* p_vidx = p_index;
	uint16_t quad_offset = 0;
	for (size_t i = 0; i < (node_count - 1); i += 1)
	{
		// 0-2-3
		p_vidx[0] = index_offset + quad_offset; // + 0
		p_vidx[1] = index_offset + quad_offset + 2;
		p_vidx[2] = index_offset + quad_offset + 3;

		// 3-1-0
		p_vidx[3] = index_offset + quad_offset + 3;
		p_vidx[4] = index_offset + quad_offset + 1;
		p_vidx[5] = index_offset + quad_offset; // + 0

		// 已使用 6 个索引，接下来不要再修改这些索引
		p_vidx += 6;
		quad_offset += 2;
	}

	return true;
}

void GameObjectBentLaser::RenderCollider(fcyColor fillColor) noexcept {
	// 忽略只有一个节点的情况
	int sn = m_Queue.Size();
	if (sn <= 1)
		return;
	
	LAPP.DebugSetGeometryRenderState();

	GameObject testObjA;
	testObjA.Reset();

	for (size_t i = 0; i < sn; ++i)
	{
		LaserNode& n = m_Queue[i];
		if (!n.active)
			continue;
		if (i > 0) {
			LaserNode& last = m_Queue[i - 1];
			if (!last.active) {
				float df = n.dis;
				if (df > n.half_width) {
					//计算部分
					fcyVec2 c = (last.pos + n.pos) * 0.5;
					testObjA.x = c.x;
					testObjA.y = c.y;
					testObjA.rect = true;
					testObjA.rot = n.rot;
					testObjA.a = df / 2;
					testObjA.b = n.half_width;
					//testObjA.UpdateCollisionCirclrRadius();
					LAPP.DebugDrawRect((float)testObjA.x, (float)testObjA.y, (float)testObjA.a, (float)testObjA.b, (float)testObjA.rot, fillColor);
				}
			}
		}
		
		//计算
		testObjA.x = n.pos.x;
		testObjA.y = n.pos.y;
		testObjA.rect = false;
		testObjA.a
			= testObjA.b
			= n.half_width * _GetEnvelope((float)i / (float)(sn - 1u));
		//testObjA.UpdateCollisionCirclrRadius();
		//渲染
		LAPP.DebugDrawCircle((float)testObjA.x, (float)testObjA.y, (float)testObjA.a, fillColor);
	}
}

bool GameObjectBentLaser::CollisionCheck(float x, float y, float rot, float a, float b, bool rect) noexcept
{
	// 忽略只有一个节点的情况
	if (m_Queue.Size() <= 1)
		return false;

	GameObject testObjA;
	testObjA.Reset();
	testObjA.rot = 0.;
	testObjA.rect = false;

	GameObject testObjB;
	testObjB.Reset();
	testObjB.x = x;
	testObjB.y = y;
	testObjB.rot = rot;
	testObjB.a = a;
	testObjB.b = b;
	testObjB.rect = rect;
	testObjB.UpdateCollisionCirclrRadius();
	int sn = m_Queue.Size();
	for (size_t i = 0; i < sn; ++i)
	{
		LaserNode& n = m_Queue[i];
		if (!n.active)continue;
		/*
		if (i > 0) {
			LaserNode& last = m_Queue[i - 1];
			if (!last.active) {
				float df = n.dis;
				if (df > n.half_width) {
					fcyVec2 c = (last.pos + n.pos) * 0.5;
					testObjA.x = c.x;
					testObjA.y = c.y;
					testObjA.rect = true;
					testObjA.rot = n.rot;
					testObjA.a = df / 2;
					testObjA.b = n.half_width;
					testObjA.UpdateCollisionCirclrRadius();
					if (LuaSTGPlus::CollisionCheck(&testObjA, &testObjB))
						return true;

				}
			}
		}
		//*/
		testObjA.x = n.pos.x;
		testObjA.y = n.pos.y;
		testObjA.a = testObjA.b = n.half_width * _GetEnvelope((float)i / (float)(sn - 1u)); //n.half_width;
		testObjA.rect = false;
		testObjA.UpdateCollisionCirclrRadius();
		if (LuaSTGPlus::CollisionCheck(&testObjA, &testObjB))
			return true;
	}
	return false;
}

bool GameObjectBentLaser::CollisionCheckW(float x, float y, float rot, float a, float b, bool rect, float width) noexcept
{
	// 忽略只有一个节点的情况
	if (m_Queue.Size() <= 1)
		return false;
	
	width = width / 2;
	GameObject testObjA;
	testObjA.Reset();
	testObjA.rot = 0.;
	testObjA.rect = false;

	GameObject testObjB;
	testObjB.Reset();
	testObjB.x = x;
	testObjB.y = y;
	testObjB.rot = rot;
	testObjB.a = a;
	testObjB.b = b;
	testObjB.rect = rect;
	testObjB.UpdateCollisionCirclrRadius();
	int sn = m_Queue.Size();
	for (size_t i = 0; i < sn; ++i)
	{
		LaserNode& n = m_Queue[i];
		if (!n.active)continue;
		/*
		if (i > 0) {
			LaserNode& last = m_Queue[i - 1];
			if (!last.active) {
				float df = n.dis;
				if (df > width) {
					fcyVec2 c = (last.pos + n.pos) * 0.5;
					testObjA.x = c.x;
					testObjA.y = c.y;
					testObjA.rect = true;
					testObjA.rot = n.rot;
					testObjA.a = df / 2;
					testObjA.b = width;
					testObjA.UpdateCollisionCirclrRadius();
					if (LuaSTGPlus::CollisionCheck(&testObjA, &testObjB))
						return true;

				}
			}
		}
		//*/
		testObjA.x = n.pos.x;
		testObjA.y = n.pos.y;
		testObjA.a = testObjA.b = width;
		testObjA.rect = false;
		testObjA.UpdateCollisionCirclrRadius();
		if (LuaSTGPlus::CollisionCheck(&testObjA, &testObjB))
			return true;
	}
	return false;
}

bool GameObjectBentLaser::BoundCheck() noexcept
{
	fcyRect tBound = LPOOL.GetBound();
	for (size_t i = 0u; i < m_Queue.Size(); i++)
	{
		LaserNode& n = m_Queue[i];
		if (n.pos.x >= tBound.a.x &&
			n.pos.x <= tBound.b.x &&
			n.pos.y <= tBound.a.y &&
			n.pos.y >= tBound.b.y)
			return true;
	}
	return false;
}

bool GameObjectBentLaser::UpdateByNode(size_t id, int node, int length, float width, bool active) noexcept
{
	GameObject* p = LPOOL.GetPooledObject(id);
	if (!p) {
		spdlog::error("[luastg] [GameObjectBentLaser::UpdateByNode] 无效的lstg.GameObject");
		return false;
	}
	if (length <= 1) {
		spdlog::error("[luastg] [GameObjectBentLaser::UpdateByNode] 无效的参数length={}", length);
		return false;
	}

	// 对索引取余
	if (node < 0) {
		node = m_Queue.Size() + node;
	}

	// 添加新节点
	if (node < m_Queue.Size() && node >= 0)
	{
		LaserNode& tNode = m_Queue[node];
		tNode.active = active;
		_UpdateAllNode();
	}

	return true;
}

bool GameObjectBentLaser::UpdatePositionByList(lua_State * L, int length, float width, int index, bool revert) noexcept// ... t(list) //lua index从1开始
{
	// ... t(list)
	int push_count = 0;//以插入头的节点数量 

	for (int i = 0; i < length; i++)
	{
		//获得x,y
		lua_rawgeti(L, -1, i + 1);// ... t(list) t(object)
		lua_pushstring(L, "x");// ... t(list) t(object) 'x'
		lua_gettable(L, -2);// ... t(list) t(object) x
		float x = luaL_optnumber(L, -1, 0.0);
		lua_pop(L, 1);
		lua_pushstring(L, "y");// ... t(list) t(object) 'y'
		lua_gettable(L, -2);// ... t(list) t(object) y
		float y = luaL_optnumber(L, -1, 0.0);// ... t(list) t(object) y
		lua_pop(L, 2);// ... t(list)

		//得到index
		//顶点处在队列前边
		int cindex = push_count + index - 1 + (revert ? -i : i);
		if (cindex < 0) {
			int j = cindex;
			LaserNode np;
			np.active = false;
			while (j > 0) {
				m_Queue.Push(np);
				j--;
				push_count++;
			}
		}

		int size = m_Queue.Size();
		//顶点处在队列后边
		if (cindex >= size) {
			int j = cindex - size + 1;
			LaserNode np;
			np.active = false;
			while (j > 0) {
				m_Queue.PushBack(np);
				j--;
			}
		}
		size = m_Queue.Size();
		//设置顶点
		LaserNode* tNode = &m_Queue[size - cindex - 1];
		tNode->active = true;
		tNode->half_width = width / 2;
		tNode->pos.Set(x, y);
	}
	_UpdateAllNode();
	return true;
}

int GameObjectBentLaser::SampleL(lua_State * L, float length) noexcept
{
	//插入一个数组
	lua_newtable(L); //... t(list)
	// 忽略没有节点的情况
	if (m_Queue.Size() <= 1)
		return true;

	float fLeft = 0;// 剩余长度
	int count = 0;

	float tVecLength = 0;
	for (size_t i = m_Queue.Size() - 1; i > 0; --i)
	{
		LaserNode& cur = m_Queue[i];
		LaserNode& next = m_Queue[i - 1];

		fcyVec2 vn = cur.pos;
		fcyVec2 offsetA = next.pos - cur.pos;
		float lenOffsetA = offsetA.Length();
		fcyVec2 expandVec = offsetA.GetNormalize();
		float angle = expandVec.CalcuAngle() * LRAD2DEGREE + 180;
		while (fLeft - lenOffsetA <= 0) {
			vn = expandVec * fLeft + cur.pos;
			lua_newtable(L); //... t(list) t(object)
			lua_pushnumber(L, vn.x); //... t(list) t(object) <x>
			lua_setfield(L, -2, "x");//... t(list) t(object)
			lua_pushnumber(L, vn.y); //... t(list) t(object) <y>
			lua_setfield(L, -2, "y");//... t(list) t(object)
			lua_pushnumber(L, angle); //... t(list) t(object) <angle>
			lua_setfield(L, -2, "rot");//... t(list) t(object)
			count++;
			lua_rawseti(L, -2, count);//... t(list)
			fLeft = fLeft + length;
		}
		fLeft = fLeft - lenOffsetA;
	}
	return true;
}

int GameObjectBentLaser::SampleT(lua_State * L, float delay) noexcept
{
	//插入一个数组
	lua_newtable(L); //... t(list)
	// 忽略没有节点的情况
	if (m_Queue.Size() <= 1)
		return true;

	float fLeft = 0;// 剩余长度
	int count = 0;

	float tVecLength = 0;
	for (size_t i = m_Queue.Size() - 1; i > 0; --i)
	{
		LaserNode& cur = m_Queue[i];
		LaserNode& next = m_Queue[i - 1];


		fcyVec2 vn = cur.pos;
		fcyVec2 offsetA = next.pos - cur.pos;
		float lenOffsetA = offsetA.Length();
		float angle = offsetA.CalcuAngle() * LRAD2DEGREE + 180;
		while (fLeft - 1 <= 0) {
			vn = offsetA * fLeft + cur.pos;
			lua_newtable(L); //... t(list) t(object)
			lua_pushnumber(L, vn.x); //... t(list) t(object) <x>
			lua_setfield(L, -2, "x");//... t(list) t(object)
			lua_pushnumber(L, vn.y); //... t(list) t(object) <y>
			lua_setfield(L, -2, "y");//... t(list) t(object)
			lua_pushnumber(L, angle); //... t(list) t(object) <angle>
			lua_setfield(L, -2, "rot");//... t(list) t(object)
			count++;
			lua_rawseti(L, -2, count);//... t(list)
			fLeft = fLeft + delay;
		}
		fLeft = fLeft - 1;
	}
	return true;
}
