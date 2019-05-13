#ifndef _RUBIK_CUBE_H_
#define _RUBIC_CUBE_H_

#include <D3DX11.h>
#include "D3DGraphics.h"
#include "D3DInput.h"
#include "D3DGeometry.h"
#include "RubikSolver.h"

#define PI 3.1415926535

struct CubeVertexData
{
	D3DXVECTOR3  position;
	D3DXVECTOR2  uv;
};

struct MatrixBufferType
{
	D3DXMATRIX world_mat;
	D3DXMATRIX view_mat;
	D3DXMATRIX proj_mat;

};

enum Direction
{
	X_Positive,
	X_Negative,
	Y_Positive,
	Y_Negative,
	Z_Positive,
	Z_Negative
};

enum RotationType
{
	RotationXFace ,
	RotationYFace ,
	RotationZFace 
};

class RubikCube
{
public:
	RubikCube(D3DXMATRIX proj_mat );
	~RubikCube();

public:
	void																InitResource(D3DGraphics * graphics, HWND hwnd);
	void																Render(ID3D11DeviceContext * device_context );
	bool																CanRotate( RotationType rotation_type , int ind  );
	bool																PreRotate( RotationType rotation_type , int ind , float val );
	void																ColorRotate(RotationType rotation_type, int ind);
	void																RotationUpdate( int i , int j , int k );
	void																ColorUpdate();
	void																InputLogicUpdate( D3DInput * input );
	bool																Intersection(int & x , int & y , int & z , Direction & face , int screen_x , int screen_y );
	void																MouseInputUpdate(int mouse_x, int mouse_y);
	bool																BoxHit(Direction & face, int x, int y, int z, const Ray & ray , float &  t );
	void																MouseRotationUpdate( int mouse_x , int mouse_y );
	void																RotateFromSolverPath();

	void																SolverRotate( RotationType rotation_type , int ind , int count );
	void																SolveRubikCube();

private:
	D3DXMATRIX												GetRotationMatrix(int x, int y, int z);

private:
	ID3D11ShaderResourceView*						m_CubicTextureArray[7];
	ID3D11Buffer*												m_VertexBuffer;
	ID3D11Buffer*												m_IndexBuffer;
	ID3D11Buffer*												m_MatrixConstantBuffer;
	ID3D11InputLayout*									m_InputLayout;
	ID3D11VertexShader*									m_VertexShader;
	ID3D11PixelShader*									m_PixelShader;
	ID3D11SamplerState*									m_SamplerState;
	D3DXVECTOR3											m_CubicPosition[3][3][3];
	D3DXVECTOR3											m_CubicRotation[3][3][3];
	D3DXVECTOR3											m_CubicPreRotation[3][3][3];
	UINT															m_CubicFaceColor[3][3][3][6];

	D3DXVECTOR3											m_CameraPosition;
	D3DXMATRIX												m_ViewMatrix;
	D3DXMATRIX												m_ProjMatrix;
	RubikSolver													m_RubikSolver;
	queue<pair<int, int>>								m_SolverQueue;

	bool																m_ButtonDown;
	int																m_RecordTime;
	std::pair<int,int>											m_StartPoint;
	int																m_Prev_i, m_Prev_j, m_Prev_k, m_Prev_face;
};

#endif