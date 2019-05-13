#ifndef _D3D_GRAPHICS_H_
#define _D3D_GRAPHICS_H_

#pragma comment(lib, "dxgi.lib")
#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dx11.lib")
#pragma comment(lib, "d3dx10.lib")
#include <D3DX11.h>
#include <D3DX10math.h>
#include <vector>
#include <string>

class D3DGraphics
{
public:
	D3DGraphics();
	~D3DGraphics();

public:
	bool Initialization(int width, int height, HWND wnd);
	void Shutdown();
	void Render();
	void Present();
public:
	bool CreateDeviceAndSwapChain(int width, int height, HWND wnd);
	bool CreateBaseResource(int width, int height);
	ID3D11ShaderResourceView* CreateShaderResourceView(ID3D11Resource * resource, D3D11_SHADER_RESOURCE_VIEW_DESC * desc);
	ID3D11Buffer * CreateVertexBuffer(UINT size, bool dynamic, bool output, D3D11_SUBRESOURCE_DATA * pSubresource);
	ID3D11Buffer * CreateIndexBuffer(UINT size, bool dynamic, D3D11_SUBRESOURCE_DATA * pSubresource);
	ID3D11Buffer * CreateConstantBuffer(UINT size, bool dynamic, bool CPUUpdates, D3D11_SUBRESOURCE_DATA * pSubresource);
	void CreateVertexShader(WCHAR * vsfile, LPCSTR vsname, ID3D11VertexShader *& vertex_shader, ID3D11InputLayout *& input_layout , std::vector<std::string>& semantic_name, std::vector<DXGI_FORMAT> & format, HWND hwnd);
	ID3D11PixelShader * CreatePixelShader(WCHAR * psfile, LPCSTR psname, HWND hwnd);
	ID3D11InputLayout * CreateInputLayout(std::vector<std::string> & semantic_name, std::vector<DXGI_FORMAT> & format, ID3D10Blob * vertex_shader_buffer);
	ID3D11ShaderResourceView* CreateTexture2D(UINT * data , UINT width , UINT height );
	ID3D11DeviceContext* GetDeviceContext();
	ID3D11SamplerState * CreateSamplerState();
	void OutputErrorMessage(ID3D10Blob * errorMessage, HWND hwnd, WCHAR * shaderFileName);

private:
	ID3D11Device * m_Device;
	ID3D11DeviceContext * m_DeviceContext;
	IDXGISwapChain * m_SwapChain;
	D3D_FEATURE_LEVEL m_FeatureLevel;
	ID3D11Texture2D * m_SwapChainBuffer;
	ID3D11Debug * m_Debugger;
	ID3D11RenderTargetView * m_RTV;
	ID3D11Texture2D * m_DepthStencilBuffer;
	ID3D11DepthStencilView * m_DepthStencilView;
	ID3D11RasterizerState * m_rasterState;
	ID3D11DepthStencilState * m_DepthStencilState;

public:
	D3DXMATRIX ProjMatrix;
};

#endif