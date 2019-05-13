#include "RubikCube.h"
#include "D3DGeometry.h"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600
#define RECORD_MOUSE_MAX 2000

#define t_min -1e8
#define t_max 1e8

extern int posX;
extern int posY;

RubikCube::RubikCube(D3DXMATRIX proj_mat )
{
	memset(m_CubicPreRotation, 0, sizeof(m_CubicRotation));
	m_CameraPosition = D3DXVECTOR3(12.0f, 12.0f, -12.0f);
	m_RecordTime = 0;
	D3DXVECTOR3 v2(0.0f, 0.0f, 0.0f);
	D3DXVECTOR3 v3(0.0f, 1.0f, 0.0f);

	D3DXMatrixLookAtLH(&m_ViewMatrix, &m_CameraPosition, &v2, &v3);
	m_ProjMatrix = proj_mat; 


}

RubikCube::~RubikCube()
{

}

void RubikCube::InitResource(D3DGraphics * graphics , HWND hwnd )
{
	auto get_color = []( UINT r , UINT g , UINT b , UINT a ) ->UINT {
		return r | (g << 8) | (b << 16) | ( a << 24);
	};

	UINT color[7] = { 
		get_color(0 , 0 , 0 , 255),
		get_color( 255 , 0 , 0 , 255 ), 
		get_color(255 , 255 , 0 , 255),
		get_color(255 , 0 , 255 , 255),
		get_color(0 , 255 , 255 , 255),
		get_color(0 , 255 , 0 , 255),
		get_color(0, 0 , 255 , 255)
	};

	UINT color_data[7][128][128];
	for (int i = 0; i < 7; i++)
	{
		for (int j = 0; j < 128; j++)
		{
			for (int k = 0; k < 128; k++)
			{
				if (j <= 2 || k <= 2 || j >= 126 || k >= 126) 				color_data[i][j][k] = color[0];
				else 				color_data[i][j][k] = color[i];
			}
		}

		m_CubicTextureArray[i] = graphics->CreateTexture2D(&color_data[i][0][0], 128, 128);
	}


	
	CubeVertexData vertices[] = {
		// +X中
		{ D3DXVECTOR3(1.0f, -1.0f, -1.0f), D3DXVECTOR2(1.0f, 0.0f)},
		{ D3DXVECTOR3(1.0f, 1.0f, -1.0f), D3DXVECTOR2(0.0f, 0.0f)},
		{ D3DXVECTOR3(1.0f, 1.0f, 1.0f), D3DXVECTOR2(0.0f, 1.0f)},
		{ D3DXVECTOR3(1.0f, -1.0f, 1.0f), D3DXVECTOR2(1.0f, 1.0f) },
		// -X中
		{ D3DXVECTOR3(-1.0f, -1.0f, 1.0f), D3DXVECTOR2(1.0f, 0.0f) },
		{ D3DXVECTOR3(-1.0f, 1.0f, 1.0f), D3DXVECTOR2(0.0f, 0.0f) },
		{ D3DXVECTOR3(-1.0f, 1.0f, -1.0f), D3DXVECTOR2(0.0f, 1.0f) },
		{ D3DXVECTOR3(-1.0f, -1.0f, -1.0f), D3DXVECTOR2(1.0f, 1.0f) },
		// +Y中
		{ D3DXVECTOR3(-1.0f, 1.0f, -1.0f), D3DXVECTOR2(1.0f, 0.0f) },
		{ D3DXVECTOR3(-1.0f, 1.0f, 1.0f), D3DXVECTOR2(0.0f, 0.0f) },
		{ D3DXVECTOR3(1.0f, 1.0f, 1.0f), D3DXVECTOR2(0.0f, 1.0f) },
		{ D3DXVECTOR3(1.0f, 1.0f, -1.0f), D3DXVECTOR2(1.0f, 1.0f) },
		// -Y中
		{ D3DXVECTOR3(-1.0f, -1.0f, 1.0f), D3DXVECTOR2(1.0f, 0.0f) },
		{ D3DXVECTOR3(-1.0f, -1.0f, -1.0f), D3DXVECTOR2(0.0f, 0.0f) },
		{ D3DXVECTOR3(1.0f, -1.0f, -1.0f), D3DXVECTOR2(0.0f, 1.0f) },
		{ D3DXVECTOR3(1.0f, -1.0f, 1.0f), D3DXVECTOR2(1.0f, 1.0f) },
		// +Z中
		{ D3DXVECTOR3(1.0f, -1.0f, 1.0f), D3DXVECTOR2(1.0f, 0.0f)},
		{ D3DXVECTOR3(1.0f, 1.0f, 1.0f), D3DXVECTOR2(0.0f, 0.0f)},
		{ D3DXVECTOR3(-1.0f, 1.0f, 1.0f), D3DXVECTOR2(0.0f, 1.0f)},
		{ D3DXVECTOR3(-1.0f, -1.0f, 1.0f), D3DXVECTOR2(1.0f, 1.0f)},
		// -Z中
		{ D3DXVECTOR3(-1.0f, -1.0f, -1.0f), D3DXVECTOR2(1.0f, 0.0f)},
		{ D3DXVECTOR3(-1.0f, 1.0f, -1.0f), D3DXVECTOR2(0.0f, 0.0f)},
		{ D3DXVECTOR3(1.0f, 1.0f, -1.0f), D3DXVECTOR2(0.0f, 1.0f)},
		{ D3DXVECTOR3(1.0f, -1.0f, -1.0f), D3DXVECTOR2(1.0f, 1.0f)},
	};

	D3D11_SUBRESOURCE_DATA sub_data;
	sub_data.pSysMem = vertices;
	m_VertexBuffer = graphics->CreateVertexBuffer(sizeof(vertices), false, false, &sub_data);

	WORD indices[] = { 0, 1, 2, 2, 3, 0 };
	sub_data.pSysMem = indices;
	m_IndexBuffer = graphics->CreateIndexBuffer(sizeof(indices), false, &sub_data);

	std::vector<std::string> semantic_vec;
	std::vector<DXGI_FORMAT> format_vec;

	semantic_vec.push_back("POSITION");
	format_vec.push_back(DXGI_FORMAT_R32G32B32_FLOAT);
	semantic_vec.push_back("TEXCOORD");
	format_vec.push_back(DXGI_FORMAT_R32G32_FLOAT);

	graphics->CreateVertexShader(L"texture.vs", "main", m_VertexShader, m_InputLayout, semantic_vec, format_vec , hwnd  );
	m_PixelShader = graphics->CreatePixelShader(L"texture.ps", "main", hwnd);
	memset(m_CubicFaceColor, 0, sizeof(m_CubicFaceColor));
	m_MatrixConstantBuffer = graphics->CreateConstantBuffer(sizeof(MatrixBufferType) , true , true , NULL );
	m_SamplerState = graphics->CreateSamplerState();
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			m_CubicFaceColor[2][i][j][X_Positive] = 1;
			m_CubicFaceColor[0][i][j][X_Negative] = 2;

			m_CubicFaceColor[j][2][i][Y_Positive] = 3;
			m_CubicFaceColor[j][0][i][Y_Negative] = 4;

			m_CubicFaceColor[i][j][2][Z_Positive] = 5;
			m_CubicFaceColor[i][j][0][Z_Negative] = 6;

		}
	}

	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			for (int k = 0; k < 3; k++)
			{
				m_CubicRotation[i][j][k] = D3DXVECTOR3(0.0f, 0.0f, 0.0f);
				m_CubicPosition[i][j][k] = D3DXVECTOR3( (i - 1) * 2.0f ,( j - 1 ) * 2.0f , (k - 1 ) * 2.0f );
			}
		}
	}
	
}

void RubikCube::Render(ID3D11DeviceContext * device_context)
{
	UINT stride = sizeof(CubeVertexData);
	UINT offset = 0;

	device_context->IASetIndexBuffer(m_IndexBuffer, DXGI_FORMAT_R16_UINT, 0);
	device_context->IASetInputLayout(m_InputLayout);
	device_context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	device_context->IASetVertexBuffers(0, 1, &m_VertexBuffer, &stride, &offset);

	device_context->VSSetShader(m_VertexShader, NULL, 0);
	device_context->VSSetConstantBuffers(0, 1, &m_MatrixConstantBuffer);

	device_context->PSSetShader(m_PixelShader, NULL, 0);
	device_context->PSSetSamplers(0, 1, &m_SamplerState);

	ColorUpdate();
	RotateFromSolverPath();

	for (int i = 0; i < 3;  i++)
	{
		for (int j = 0; j < 3; j++)
		{
			for (int k = 0; k < 3; k++)
			{
				RotationUpdate(i, j, k);

				D3D11_MAPPED_SUBRESOURCE mapped_resource;
				MatrixBufferType* matrix_buffer;
				HRESULT hr = device_context->Map(m_MatrixConstantBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped_resource);
				if (FAILED(hr))
				{
					throw "Error.";
				}
				matrix_buffer = (MatrixBufferType*)mapped_resource.pData;
				D3DXMatrixTranslation(&matrix_buffer->world_mat, m_CubicPosition[i][j][k].x, m_CubicPosition[i][j][k].y, m_CubicPosition[i][j][k].z);
				D3DXMATRIX rotation_mat = GetRotationMatrix( i , j , k );
				matrix_buffer->world_mat *= rotation_mat;
				matrix_buffer->view_mat = m_ViewMatrix;
				matrix_buffer->proj_mat = m_ProjMatrix;

				D3DXMATRIX world = matrix_buffer->world_mat;
				D3DXMATRIX view = matrix_buffer->view_mat;
				D3DXMATRIX proj = matrix_buffer->proj_mat;

				D3DXMatrixTranspose(&matrix_buffer->world_mat, &world);
				D3DXMatrixTranspose(&matrix_buffer->view_mat, &view);
				D3DXMatrixTranspose(&matrix_buffer->proj_mat, &proj );

				device_context->Unmap(m_MatrixConstantBuffer, 0);
		
				for (int m = 0; m < 6; m++)
				{
					int voffset = m * 4;
					device_context->PSSetShaderResources(0, 1, &m_CubicTextureArray[m_CubicFaceColor[i][j][k][m]]);
					device_context->DrawIndexed(6, 0, voffset);
				}
				
			}
		}
	}
}

bool RubikCube::CanRotate(RotationType rotation_type, int ind)
{
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			D3DXVECTOR3 pre_rotation_vec;
			switch (rotation_type)
			{
			case RotationXFace:
				pre_rotation_vec = m_CubicPreRotation[ind][i][j];
				if (pre_rotation_vec.y != 0.0f || pre_rotation_vec.z != 0.0f) return false;
				break;
			case RotationYFace:
				pre_rotation_vec = m_CubicPreRotation[i][ind][j];
				if (pre_rotation_vec.x != 0.0f || pre_rotation_vec.z != 0.0f) return false;

				break;
			case RotationZFace:
				pre_rotation_vec = m_CubicPreRotation[i][j][ind];
				if (pre_rotation_vec.x != 0.0f || pre_rotation_vec.y != 0.0f) return false;

				break;
			}

		}
	}
	return true;
}

bool RubikCube::PreRotate(RotationType rotation_type , int ind, float val)
{
	if (!CanRotate(rotation_type, ind)) 
		return false;
	
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			D3DXVECTOR3 pre_rotation_vec;
			switch (rotation_type)
			{
			case RotationXFace:
				m_CubicPreRotation[ind][i][j].x += val ;
				break;
			case RotationYFace:
				m_CubicPreRotation[i][ind][j].y += val ;
				break;
			case RotationZFace:
				m_CubicPreRotation[i][j][ind].z += val ;
				break;
			}
		}
	}

	return true;
}

void RubikCube::ColorRotate(RotationType rotation_type, int ind)
{
	static std::vector<std::pair<int, int>> CubeSwapIndex1 {
		{ 0 , 0 }, { 0 , 1 }, { 0 , 2 }, { 1 , 2 } , { 2 , 2 } , { 2 , 1 } 
	};
	static std::vector<std::pair<int, int>> CubeSwapIndex2 {
		{ 0 , 2 },{ 1 , 2 },{ 2 , 2 },{ 2 , 1 } ,{ 2 , 0 } ,{ 1 , 0 }
	};

	int swap_count = CubeSwapIndex1.size();
	static Direction DirectionReplaceTable[3][6] = {
		{ X_Positive  , X_Negative , Z_Negative , Z_Positive , Y_Positive , Y_Negative  },
		{ Z_Positive , Z_Negative , Y_Positive , Y_Negative , X_Negative , X_Positive },
		{ Y_Negative , Y_Positive , X_Positive , X_Negative , Z_Positive , Z_Negative }
	};

	switch (rotation_type)
	{
		case RotationType::RotationXFace:
		{
			for (int i = 0; i < swap_count; i++)
			{
				std::pair<int, int > pos1 = CubeSwapIndex1[i];
				std::pair<int, int > pos2 = CubeSwapIndex2[i];

				for (int j = 0; j < 6; j++)
				{
					std::swap(m_CubicFaceColor[ind][pos1.first][pos1.second][j], m_CubicFaceColor[ind][pos2.first][pos2.second][DirectionReplaceTable[0][j]]);
				}
			}
			break;
		}

		case RotationType::RotationYFace:
		{
			for (int i = 0; i < swap_count; i++)
			{
				std::pair<int, int > pos1 = CubeSwapIndex1[i];
				std::pair<int, int > pos2 = CubeSwapIndex2[i];

				for (int j = 0; j < 6; j++)
				{
					std::swap(m_CubicFaceColor[pos1.second][ind][pos1.first][j], m_CubicFaceColor[pos2.second][ind][pos2.first][DirectionReplaceTable[1][j]]);
				}
			}
			break;
		}

		case RotationType::RotationZFace:
		{
			for (int i = 0; i < swap_count; i++)
			{
				std::pair<int, int > pos1 = CubeSwapIndex1[i];
				std::pair<int, int > pos2 = CubeSwapIndex2[i];

				for (int j = 0; j < 6; j++)
				{
					std::swap(m_CubicFaceColor[pos1.first][pos1.second][ind][j], m_CubicFaceColor[pos2.first][pos2.second][ind][DirectionReplaceTable[2][j]]);
				}
			}
			break;
		}
	}
}

void RubikCube::RotationUpdate(int i, int j, int k)
{

	if (m_CubicPreRotation[i][j][k].x != 0) {
		D3DXVECTOR3 add_vec = D3DXVECTOR3(m_CubicPreRotation[i][j][k].x > 0 ? 0.001f : -0.001f, 0.0f, 0.0f);
		m_CubicRotation[i][j][k] += add_vec;
		m_CubicPreRotation[i][j][k] -= add_vec;
	}
	else if (m_CubicPreRotation[i][j][k].y != 0)
	{
		D3DXVECTOR3 add_vec = D3DXVECTOR3( 0.0f , m_CubicPreRotation[i][j][k].y > 0 ? 0.001f : -0.001f , 0.0f);
		m_CubicRotation[i][j][k] += add_vec;
		m_CubicPreRotation[i][j][k] -= add_vec;
	}
	else if(m_CubicPreRotation[i][j][k].z != 0) {
		D3DXVECTOR3 add_vec = D3DXVECTOR3(0.0f, 0.0f, m_CubicPreRotation[i][j][k].z > 0 ? 0.001f : -0.001f);
		m_CubicRotation[i][j][k] += add_vec;
		m_CubicPreRotation[i][j][k] -= add_vec;
	}

	if (abs(m_CubicPreRotation[i][j][k].x) < 0.001f) {
		m_CubicPreRotation[i][j][k].x = 0.0f;
		m_CubicRotation[i][j][k].x = (int)( ( m_CubicRotation[i][j][k].x + (m_CubicRotation[i][j][k].x > 0 ? PI / 4 : -PI / 4)) / (PI / 2)) * (PI / 2);
	}
	if (abs(m_CubicPreRotation[i][j][k].y) < 0.001f) {
		m_CubicPreRotation[i][j][k].y = 0.0f;
		m_CubicRotation[i][j][k].y = (int)( (m_CubicRotation[i][j][k].y + (m_CubicRotation[i][j][k].y > 0 ? PI / 4 : -PI / 4)) / (PI / 2)) * (PI / 2);
	}
	if (abs(m_CubicPreRotation[i][j][k].z) < 0.001f) {
		m_CubicPreRotation[i][j][k].z = 0.0f;
		m_CubicRotation[i][j][k].z = (int)( (m_CubicRotation[i][j][k].z + (m_CubicRotation[i][j][k].z > 0 ? PI / 4 : -PI / 4)) / (PI / 2)) * (PI / 2);
	}
}

void RubikCube::ColorUpdate()
{
	for (int i = 0; i < 3; i++)
	{
		int rotation_count = m_CubicRotation[i][0][0].x > 0 ? 1 : 3;
		float increment = m_CubicRotation[i][0][0].x > 0 ? -PI / 2 : PI / 2;
		while ( abs(m_CubicRotation[i][0][0].x) >= PI / 2 )
		{
			for (int j = 0; j < 3; j++)
			{
				for (int k = 0; k < 3; k++)
				{
					m_CubicRotation[i][j][k].x += increment;
				}
			}
			for (int j = 0; j < rotation_count; j++, ColorRotate(RotationXFace, i));
		}
	}

	for (int i = 0; i < 3; i++)
	{
		int rotation_count = m_CubicRotation[0][i][0].y > 0 ? 1 : 3;
		float increment = m_CubicRotation[0][i][0].y > 0 ? -PI / 2 : PI / 2;
		while (abs(m_CubicRotation[0][i][0].y) >= PI / 2)
		{
			for (int j = 0; j < 3; j++)
			{
				for (int k = 0; k < 3; k++)
				{
					m_CubicRotation[k][i][j].y += increment;
				}
			}
			for (int j = 0; j < rotation_count; j++, ColorRotate(RotationYFace, i));
		}
	}

	for (int i = 0; i < 3; i++)
	{
		int rotation_count = m_CubicRotation[0][0][i].z > 0 ? 1 : 3;
		float increment = m_CubicRotation[0][0][i].z > 0 ? -PI / 2 : PI / 2;
		while (abs(m_CubicRotation[0][0][i].z) >= PI / 2)
		{
			for (int j = 0; j < 3; j++)
			{
				for (int k = 0; k < 3; k++)
				{
					m_CubicRotation[j][k][i].z += increment;
				}
			}
			for (int j = 0; j < rotation_count; j++, ColorRotate(RotationZFace, i));
		}
	}


}

void RubikCube::InputLogicUpdate(D3DInput * input)
{
	if(input->IsKeyDown(DIK_A))
	{
		SolveRubikCube();
	}
	if (input->IsMouseLeftButtonDown())
	{
		POINT p;
		GetCursorPos(&p);
		int mouse_x = p.x - posX;
		int mouse_y = p.y - posY;
		if (mouse_x >= 0 && mouse_x <= SCREEN_WIDTH && mouse_y >= 0 && mouse_y <= SCREEN_HEIGHT)
		{
			MouseRotationUpdate( mouse_x , mouse_y );
			MouseInputUpdate(mouse_x, mouse_y);
			if (m_ButtonDown)
			{
				m_RecordTime++;
			}
		}
	}
	else {
		m_ButtonDown = false;
	}


}

bool RubikCube::Intersection(int & x, int & y, int & z, Direction & face, int screen_x, int screen_y)
{
	float ndc_x = (float)(2 * screen_x) / SCREEN_WIDTH - 1.0f;
	float ndc_y = -(float)(2 * screen_y) / SCREEN_HEIGHT + 1.0f;
	float ndc_z = 0.0f;

	D3DXVECTOR4 ndc_vec( ndc_x , ndc_y , ndc_z , 1.0f ) , world_vec ;
	D3DXMATRIX trans , trans_inv ;
	D3DXMatrixMultiply(&trans  , &m_ViewMatrix , &m_ProjMatrix);
	D3DXMatrixInverse(&trans_inv, NULL, &trans);
	D3DXVec4Transform( &world_vec , &ndc_vec, &trans_inv);

	D3DXVECTOR3 dir( world_vec.x / world_vec.w- m_CameraPosition.x  , world_vec.y / world_vec.w - m_CameraPosition.y, world_vec.z / world_vec.w - m_CameraPosition.z );
	Ray ray(m_CameraPosition, dir);
	float t = 1e8;
	bool hit = false;
	for (int i = 0; i < 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			for (int k = 0; k < 3; k++)
			{
				if (i == 1 || j == 1 || k == 1)continue;
				if (m_CubicRotation[i][j][k].x != 0.0f || m_CubicRotation[i][j][k].y != 0.0f || m_CubicRotation[i][j][k].z != 0.0f) continue;
				if (BoxHit(face, i, j, k, ray , t) == false)  continue;
				else {
					x = i;
					y = j;
					z = k;
					hit = true;
				}
			}
		}
	}

	return hit;
}

void RubikCube::MouseInputUpdate(int mouse_x, int mouse_y)
{
	if (m_ButtonDown == true) return;
	int x, y, z;
	Direction face;
	if (Intersection(x, y, z, face, mouse_x, mouse_y))
	{
		m_ButtonDown = true;
		m_StartPoint.first = mouse_x;
		m_StartPoint.second = mouse_y;

		m_Prev_i = x;
		m_Prev_j = y;
		m_Prev_k = z;
		m_Prev_face = face;
	}
}

bool RubikCube::BoxHit(Direction & face, int x, int y, int z , const Ray & ray , float & t )
{
	D3DXVECTOR3 position = m_CubicPosition[x][y][z];
	float xmin = position.x - 1.0f;
	float xmax = position.x + 1.0f;
	float ymin = position.y - 1.0f;
	float ymax = position.y + 1.0f;
	float zmin = position.z - 1.0f;
	float zmax = position.z + 1.0f;
	float tx_min = t_max , tx_max = t_min, ty_min = t_max, ty_max = t_min, tz_min = t_max, tz_max = t_min;

	if (abs(ray.m_Direction.x) < 0.00001f)
	{
		if (ray.m_OriginPosition.x < xmin || ray.m_OriginPosition.x > xmax) return false;
	}
	else {
		if (ray.m_Direction.x >= 0 )
		{
			tx_min = ( xmin - ray.m_OriginPosition.x ) / ray.m_Direction.x;
			tx_max = (xmax - ray.m_OriginPosition.x) / ray.m_Direction.x;
		}
		else {
			tx_min = (xmax - ray.m_OriginPosition.x) / ray.m_Direction.x;
			tx_max = (xmin - ray.m_OriginPosition.x) / ray.m_Direction.x;

		}
	}

	if (abs(ray.m_Direction.y) < 0.00001f)
	{
		if (ray.m_OriginPosition.y < ymin || ray.m_OriginPosition.y > ymax) return false;
	}
	else {
		if (ray.m_Direction.y >= 0)
		{
			ty_min = (ymin - ray.m_OriginPosition.y) / ray.m_Direction.y;
			ty_max = (ymax - ray.m_OriginPosition.y) / ray.m_Direction.y;
		}
		else {
			ty_min = (ymax - ray.m_OriginPosition.y) / ray.m_Direction.y;
			ty_max = (ymin - ray.m_OriginPosition.y) / ray.m_Direction.y;

		}
	}

	if (abs(ray.m_Direction.z) < 0.00001f)
	{
		if (ray.m_OriginPosition.z < zmin || ray.m_OriginPosition.z > zmax) return false;
	}
	else {
		if (ray.m_Direction.z >= 0)
		{
			tz_min = (zmin - ray.m_OriginPosition.z) / ray.m_Direction.z;
			tz_max = (zmax - ray.m_OriginPosition.z) / ray.m_Direction.z;
		}
		else {
			tz_min = (zmax - ray.m_OriginPosition.z) / ray.m_Direction.z;
			tz_max = (zmin - ray.m_OriginPosition.z) / ray.m_Direction.z;

		}
	}


	float tmin = std::fmax( tx_min , std::fmax( ty_min , tz_min ));
	float tmax = std::fmin( tx_max , std::fmin( ty_max , tz_max ) );

	if (tmin > tmax) return false;



	if (tmin < t) {
		if (tmin == tx_min)
		{
			face = ray.m_Direction.x > 0 ? X_Negative : X_Positive;
		}
		else if (tmin == ty_min)
		{
			face = ray.m_Direction.y > 0 ? Y_Negative : Y_Positive;
		}
		else {
			face = ray.m_Direction.z > 0 ? Z_Negative : Z_Positive;
		}
		t = tmin;
		return true;
	}

	return false;
}

void RubikCube::MouseRotationUpdate( int mouse_x , int mouse_y )
{
	if (m_RecordTime == RECORD_MOUSE_MAX)
	{
		int dx = mouse_x - m_StartPoint.first;
		int dy =  -mouse_y + m_StartPoint.second;
		if (abs(dx) >= 5 && abs(dy) >= 5)
		{
			if (m_Prev_face == Y_Positive)
			{
				if (dy <= dx && dy >= -dx)
				{
					if (PreRotate(RotationType::RotationYFace, m_Prev_j, -PI / 2))
					{
						SolverRotate(RotationType::RotationYFace, m_Prev_j, 3);
					}
				}
				else if (dy >= dx && dy >= -dx)
				{
					if (PreRotate(RotationType::RotationXFace, m_Prev_i, PI / 2))
					{
						SolverRotate(RotationType::RotationXFace, m_Prev_i, 1);
					}
				}
				else if (dy >= dx && dy <= -dx)
				{
					if (PreRotate(RotationType::RotationYFace, m_Prev_j, PI / 2))
					{
						SolverRotate(RotationType::RotationYFace, m_Prev_j, 1);
					}
				}
				else {
					if (PreRotate(RotationType::RotationXFace, m_Prev_i, -PI / 2))
					{
						SolverRotate(RotationType::RotationXFace, m_Prev_i, 3);
					}
				}
			}
			else if (m_Prev_face == Z_Negative )
			{
				if (dx >= 0 && dy >= 0)
				{
					if (PreRotate(RotationType::RotationYFace, m_Prev_j, -PI / 2))
					{
						SolverRotate(RotationType::RotationYFace, m_Prev_j, 3);
					}
				}
				else if (dx <= 0 && dy >= 0)
				{
					if (PreRotate(RotationType::RotationYFace, m_Prev_j, PI / 2))
					{
						SolverRotate(RotationType::RotationYFace, m_Prev_j, 1);
					}
				}
				else if (dx <=0 && dy <= 0 )
				{
					if (PreRotate(RotationType::RotationXFace, m_Prev_i, -PI / 2))
					{
						SolverRotate(RotationType::RotationXFace, m_Prev_i, 3);
					}
				}
				else {
					if (PreRotate(RotationType::RotationZFace, m_Prev_k, -PI / 2))
					{
						SolverRotate(RotationType::RotationZFace, m_Prev_k, 3);
					}
				}
			}
			else if (m_Prev_face == X_Positive)
			{
				if (dy <= dx && dy >= -dx)
				{
					if (PreRotate(RotationType::RotationYFace, m_Prev_j, -PI / 2))
					{
						SolverRotate(RotationType::RotationYFace, m_Prev_j, 3);
					}
				}
				else if (dy >= dx && dy >= -dx)
				{
					if (PreRotate(RotationType::RotationZFace, m_Prev_k, PI / 2))
					{
						SolverRotate(RotationType::RotationZFace, m_Prev_k, 1);
					}
				}
				else if (dy >= dx && dy <= -dx)
				{
					if (PreRotate(RotationType::RotationYFace, m_Prev_j, PI / 2))
					{
						SolverRotate(RotationType::RotationYFace, m_Prev_j, 1);
					}
				}
				else {
					if (PreRotate(RotationType::RotationZFace, m_Prev_k, -PI / 2))
					{
						SolverRotate(RotationType::RotationZFace, m_Prev_k, 3);
					}
				}
			}
		}

		m_RecordTime = 0;
	}
}

void RubikCube::RotateFromSolverPath()
{
	if (m_SolverQueue.empty()) return;
	static int timer = 0;
	timer++;
	if (timer < PI * 3 / (2 * 0.001f)) return;
	timer = 0;

	std::pair<int, int> rp = m_SolverQueue.front();
	m_SolverQueue.pop();

		if (rp.first == 4) PreRotate(RotationXFace, 0, PI / 2 * -rp.second);
		if (rp.first == 0) PreRotate(RotationYFace, 2, PI / 2 * rp.second);
		if (rp.first == 1) PreRotate(RotationXFace, 2, PI / 2 * rp.second);
		if (rp.first == 2) PreRotate(RotationZFace, 0, PI / 2 * -rp.second);
		if (rp.first == 3) PreRotate(RotationYFace, 0, PI / 2 * -rp.second	);
		if (rp.first == 5) PreRotate(RotationZFace, 2, PI / 2 * rp.second);
}

void RubikCube::SolverRotate(RotationType rotation_type, int ind , int count )
{
	switch (rotation_type)
	{
	case RotationXFace:
		if (ind == 0) m_RubikSolver.Rotate( ROTATION_X_LEFT , 4 - count );
		if (ind == 2) m_RubikSolver.Rotate(ROTATION_X_RIGHT, count);

		break;
	case RotationYFace:
		if (ind == 0) 
			m_RubikSolver.Rotate(ROTATION_Y_DOWN, 4 - count);
		if (ind == 2) 
			m_RubikSolver.Rotate(ROTATION_Y_UP, count);

		break;
	case RotationZFace:
		if (ind == 0)	 m_RubikSolver.Rotate(ROTATION_Z_FRONT, 4 - count);
		if (ind == 2) m_RubikSolver.Rotate(ROTATION_Z_BACK, count);

		break;

	}
}

void RubikCube::SolveRubikCube()
{
	vector<pair<int, int>> rpath;

	rpath.resize(50);
	Phase1Search(m_RubikSolver, rpath);
	for (int i = 0; i < rpath.size() ; i++)
	{
		if (rpath[i].second == 0) break;
		m_SolverQueue.push(rpath[i]);
		m_RubikSolver.Rotate(rpath[i].first, rpath[i].second);
	}

	
	rpath.clear();
	rpath.resize(50);

	Phase2Search(m_RubikSolver, rpath);
	for (int i = 0; i < rpath.size(); i++)
	{
		if (rpath[i].second == 0) break;
		m_SolverQueue.push(rpath[i]);
		m_RubikSolver.Rotate(rpath[i].first, rpath[i].second);
	}

}

D3DXMATRIX RubikCube::GetRotationMatrix(int x, int y, int z)
{
	D3DXMATRIX res;
	D3DXMatrixIdentity(&res);

	if (m_CubicRotation[x][y][z].x != 0)
	{
		D3DXMatrixRotationX(&res , m_CubicRotation[x][y][z].x);
	}
	else if (m_CubicRotation[x][y][z].y != 0)
	{
		D3DXMatrixRotationY(&res, m_CubicRotation[x][y][z].y);
	}
	else if (m_CubicRotation[x][y][z].z != 0)
	{
		D3DXMatrixRotationZ(&res, m_CubicRotation[x][y][z].z);
	}

	return res;
}
