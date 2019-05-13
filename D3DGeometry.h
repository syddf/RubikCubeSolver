#ifndef D3D_GEOMETRY_H
#define D3D_GEOMETRY_H

#include <D3DX11.h>
#include <D3DX10math.h>

class Ray
{
public:
	Ray(const D3DXVECTOR3 & origin, const D3DXVECTOR3 & direction);
	Ray												Transform(const D3DXMATRIX & transMat);

public:
	D3DXVECTOR3							m_OriginPosition;
	D3DXVECTOR3							m_Direction;

};


#endif 