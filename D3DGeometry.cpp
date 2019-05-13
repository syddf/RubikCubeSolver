#include "D3DGeometry.h"

Ray::Ray(const D3DXVECTOR3 & origin, const D3DXVECTOR3 & direction)
{
	D3DXVec3Normalize(&m_Direction, &direction);
	m_OriginPosition = origin;
}

Ray Ray::Transform(const D3DXMATRIX & transMat)
{
	D3DXVECTOR4 dirv(m_Direction, 0.0f);
	D3DXVECTOR4 posv(m_OriginPosition, 1.0f);
	D3DXVECTOR4 tmp;

	D3DXVECTOR3 res_dir, res_pos;
	D3DXVec4Transform(&tmp, &dirv, &transMat);
	res_dir.x = tmp.x;
	res_dir.y = tmp.y;
	res_dir.z = tmp.z;

	D3DXVec4Transform(&tmp, &posv, &transMat);
	res_pos.x = tmp.x;
	res_pos.y = tmp.y;
	res_pos.z = tmp.z;

	return Ray(res_pos, res_dir);
}
