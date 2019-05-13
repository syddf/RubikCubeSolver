#include "D3DGraphics.h"
#include <D3DX11.h>
#include <D3DX10math.h>
#include <fstream>
#include "RubikCube.h"

D3DGraphics::D3DGraphics()
{
}

D3DGraphics::~D3DGraphics()
{
}

bool D3DGraphics::Initialization(int width, int height, HWND wnd)
{
	bool result;
	result = CreateDeviceAndSwapChain(width, height, wnd);
	if (!result)
	{
		return false;
	}
	result = CreateBaseResource(width, height);
	if (!result)
	{
		return false;
	}

	return true;
}

void D3DGraphics::Shutdown()
{
}

bool D3DGraphics::CreateDeviceAndSwapChain(int width, int height, HWND wnd)
{
	HRESULT hr;
	D3D_FEATURE_LEVEL feature = D3D_FEATURE_LEVEL_11_0;

	//´´½¨SwapChainDesc
	DXGI_SWAP_CHAIN_DESC swapDesc;
	swapDesc.Windowed = true;
	swapDesc.BufferCount = 1;
	swapDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	swapDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	swapDesc.BufferDesc.Height = height;
	swapDesc.BufferDesc.Width = width;
	swapDesc.BufferDesc.RefreshRate.Numerator = 0;
	swapDesc.BufferDesc.RefreshRate.Denominator = 1;
	swapDesc.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;
	swapDesc.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	swapDesc.OutputWindow = wnd;
	swapDesc.SampleDesc.Count = 1;
	swapDesc.SampleDesc.Quality = 0;
	swapDesc.Flags = 0;
	swapDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	hr = D3D11CreateDeviceAndSwapChain(0, D3D_DRIVER_TYPE_HARDWARE, NULL, 0, &feature, 1, D3D11_SDK_VERSION, &swapDesc, &m_SwapChain, &m_Device, &m_FeatureLevel, &m_DeviceContext);

	if (FAILED(hr))
	{
		return false;
	}
	return true;
}

bool D3DGraphics::CreateBaseResource(int width, int height)
{
	HRESULT hr;
	hr = m_SwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), (void**)&m_SwapChainBuffer);
	if (FAILED(hr))
	{
		return false;
	}

	hr = m_Device->CreateRenderTargetView(m_SwapChainBuffer, 0, &m_RTV);
	if (FAILED(hr))
	{
		return false;
	}
	D3D11_TEXTURE2D_DESC desc;
	desc.Width = width;
	desc.Height = height;
	desc.ArraySize = 1;
	desc.MipLevels = 1;
	desc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	desc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	desc.Usage = D3D11_USAGE_DEFAULT;
	desc.CPUAccessFlags = 0;
	desc.SampleDesc.Count = 1;
	desc.SampleDesc.Quality = 0;
	desc.MiscFlags = 0;
	D3D11_TEXTURE2D_DESC depthBufferDesc;
	D3D11_DEPTH_STENCIL_DESC depthStencilDesc;
	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	hr = m_Device->CreateTexture2D(&desc, 0, &m_DepthStencilBuffer);
	if (FAILED(hr))
	{
		return false;
	}
	// Initialize the description of the depth buffer.
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));

	// Set up the description of the depth buffer.
	depthBufferDesc.Width = width;
	depthBufferDesc.Height = height;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	// Create the texture for the depth buffer using the filled out description.
	hr = m_Device->CreateTexture2D(&depthBufferDesc, NULL, &m_DepthStencilBuffer);
	if (FAILED(hr))
	{
		return false;
	}

	// Initialize the description of the stencil state.
	ZeroMemory(&depthStencilDesc, sizeof(depthStencilDesc));

	// Set up the description of the stencil state.
	depthStencilDesc.DepthEnable = true;
	depthStencilDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthFunc = D3D11_COMPARISON_LESS;
	depthStencilDesc.StencilEnable = true;
	depthStencilDesc.StencilReadMask = 0xFF;
	depthStencilDesc.StencilWriteMask = 0xFF;

	// Stencil operations if pixel is front-facing.
	depthStencilDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	depthStencilDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Stencil operations if pixel is back-facing.
	depthStencilDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	depthStencilDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	depthStencilDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	// Create the depth stencil state.
	hr = m_Device->CreateDepthStencilState(&depthStencilDesc, &m_DepthStencilState);
	if (FAILED(hr))
	{
		return false;
	}

	// Set the depth stencil state.
	m_DeviceContext->OMSetDepthStencilState(m_DepthStencilState, 1);

	// Initialize the depth stencil view.
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));

	// Set up the depth stencil view description.
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	// Create the depth stencil view.
	hr = m_Device->CreateDepthStencilView(m_DepthStencilBuffer, &depthStencilViewDesc, &m_DepthStencilView);
	if (FAILED(hr))
	{
		return false;
	}
	float a = (float)width / (float)height;
	// Bind the render target view and depth stencil buffer to the output render pipeline.
	m_DeviceContext->OMSetRenderTargets(1, &m_RTV, m_DepthStencilView);
	D3DXMatrixPerspectiveFovLH(&ProjMatrix, (float)D3DX_PI / 4.0f, a, 0.1f, 1000.0f);

	D3D11_RASTERIZER_DESC rasterDesc;
	// Setup the raster description which will determine how and what polygons will be drawn.
	rasterDesc.AntialiasedLineEnable = false;
	rasterDesc.CullMode = D3D11_CULL_BACK;
	rasterDesc.DepthBias = 0;
	rasterDesc.DepthBiasClamp = 0.0f;
	rasterDesc.DepthClipEnable = true;
	rasterDesc.FillMode = D3D11_FILL_SOLID;
	rasterDesc.FrontCounterClockwise = false;
	rasterDesc.MultisampleEnable = false;
	rasterDesc.ScissorEnable = false;
	rasterDesc.SlopeScaledDepthBias = 0.0f;

	// Create the rasterizer state from the description we just filled out.
	hr = m_Device->CreateRasterizerState(&rasterDesc, &m_rasterState);
	// Now set the rasterizer state.
	m_DeviceContext->RSSetState(m_rasterState);
	D3D11_VIEWPORT viewport;
	// Setup the viewport for rendering.
	viewport.Width = (float)width;
	viewport.Height = (float)height;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;
	viewport.TopLeftX = 0.0f;
	viewport.TopLeftY = 0.0f;

	// Create the viewport.
	m_DeviceContext->RSSetViewports(1, &viewport);
	return true;
}

ID3D11ShaderResourceView * D3DGraphics::CreateShaderResourceView(ID3D11Resource * resource, D3D11_SHADER_RESOURCE_VIEW_DESC * desc)
{
	ID3D11ShaderResourceView * shader_resource_view = nullptr;

	HRESULT hr = m_Device->CreateShaderResourceView(resource, desc, &shader_resource_view);
	if (FAILED(hr)) return nullptr;

	return shader_resource_view;
}

ID3D11Buffer * D3DGraphics::CreateVertexBuffer(UINT size, bool dynamic, bool output, D3D11_SUBRESOURCE_DATA * pSubresource)
{
	D3D11_BUFFER_DESC buffer_desc;
	buffer_desc.MiscFlags = 0;
	buffer_desc.StructureByteStride = 0;
	buffer_desc.ByteWidth = size;
	if (dynamic)
	{
		buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
		buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	}
	else if (output)
	{
		buffer_desc.Usage = D3D11_USAGE_DEFAULT;
		buffer_desc.CPUAccessFlags = 0;
		buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER | D3D11_BIND_STREAM_OUTPUT;
	}
	else {
		buffer_desc.Usage = D3D11_USAGE_IMMUTABLE;
		buffer_desc.CPUAccessFlags = 0;
		buffer_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	}
	ID3D11Buffer * newBuffer = NULL;
	HRESULT hr = m_Device->CreateBuffer( &buffer_desc , pSubresource , &newBuffer);
	if (FAILED(hr))
	{
		return nullptr;
	}

	return newBuffer ;
}

ID3D11Buffer * D3DGraphics::CreateIndexBuffer(UINT size, bool dynamic, D3D11_SUBRESOURCE_DATA * pSubresource)
{
	D3D11_BUFFER_DESC buffer_desc;
	buffer_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	buffer_desc.ByteWidth = size;
	buffer_desc.MiscFlags = 0;
	buffer_desc.StructureByteStride = 0;
	
	if (dynamic)
	{
		buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
	}
	else {
		buffer_desc.CPUAccessFlags = 0;
		buffer_desc.Usage = D3D11_USAGE_IMMUTABLE;
	}

	ID3D11Buffer * newBuffer;
	HRESULT hr = m_Device->CreateBuffer(&buffer_desc, pSubresource, &newBuffer);

	if (FAILED(hr)) return nullptr;
	return newBuffer;

}

ID3D11Buffer * D3DGraphics::CreateConstantBuffer(UINT size, bool dynamic, bool CPUUpdates, D3D11_SUBRESOURCE_DATA * pSubresource)
{
	D3D11_BUFFER_DESC buffer_desc;
	buffer_desc.StructureByteStride = 0;
	buffer_desc.MiscFlags = 0;
	buffer_desc.ByteWidth = size;
	buffer_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;

	if (dynamic && CPUUpdates)
	{
		buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
		buffer_desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	}
	else if ( dynamic && !CPUUpdates )
	{
		buffer_desc.Usage = D3D11_USAGE_DYNAMIC;
		buffer_desc.CPUAccessFlags = 0;
	}
	else {
		buffer_desc.Usage = D3D11_USAGE_IMMUTABLE;
		buffer_desc.CPUAccessFlags = 0;
	}

	ID3D11Buffer * newBuffer;

	HRESULT hr = m_Device->CreateBuffer(&buffer_desc, pSubresource, &newBuffer);
	if (FAILED(hr)) return nullptr;

	return newBuffer;
}

void D3DGraphics::CreateVertexShader(WCHAR * vsfile, LPCSTR vsname, ID3D11VertexShader *& vertex_shader, ID3D11InputLayout *& input_layout , 
																std::vector<std::string>& semantic_name, std::vector<DXGI_FORMAT> & format, HWND hwnd)
{
	HRESULT hr;
	ID3D10Blob * vertexShaderBuffer;
	ID3D10Blob * errorMessage;

	hr = D3DX11CompileFromFile(vsfile, NULL, NULL, vsname, "vs_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL, 
														&vertexShaderBuffer, &errorMessage, NULL);
	if (FAILED(hr))
	{
		if (errorMessage)
		{
			OutputErrorMessage(errorMessage, hwnd, vsfile);
			errorMessage->Release();
		}
		else {
			MessageBox(hwnd, L"Can't Find File", vsfile, MB_OK);
		}
		return;
	}
	hr = m_Device->CreateVertexShader(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), NULL, &vertex_shader );
	if (FAILED(hr))
	{
		return;
	}

	input_layout = CreateInputLayout(semantic_name, format, vertexShaderBuffer );

	vertexShaderBuffer->Release();
}

ID3D11PixelShader * D3DGraphics::CreatePixelShader(WCHAR * psfile, LPCSTR psname, HWND hwnd)
{
	HRESULT hr;
	ID3D10Blob * pixelShaderBuffer;
	ID3D10Blob * errorMessage;
	ID3D11PixelShader * pixel_shader;

	hr = D3DX11CompileFromFile(psfile, NULL, NULL, psname, "ps_5_0", D3D10_SHADER_ENABLE_STRICTNESS, 0, NULL,
		&pixelShaderBuffer, &errorMessage, NULL);

	if (FAILED(hr))
	{
		if (errorMessage)
		{
			OutputErrorMessage(errorMessage, hwnd, psfile);
			errorMessage->Release();

		}
		else {
			MessageBox(hwnd, L"Can't Find File", psfile, MB_OK);
		}
		return NULL ;
	}
	hr = m_Device->CreatePixelShader(pixelShaderBuffer->GetBufferPointer(), pixelShaderBuffer->GetBufferSize(), NULL, &pixel_shader);
	if (FAILED(hr))
	{
		return NULL ;
	}

	pixelShaderBuffer->Release();

	return pixel_shader;
}

ID3D11InputLayout * D3DGraphics::CreateInputLayout(std::vector<std::string>& semantic_name, 
																						 std::vector<DXGI_FORMAT> & format , ID3D10Blob * vertex_shader_buffer  )
{
	UINT desc_size = semantic_name.size();

	std::vector<D3D11_INPUT_ELEMENT_DESC > desc_vec;
	desc_vec.resize(desc_size);

	for (int i = 0; i < desc_size; i++)
	{
		desc_vec[i].SemanticName = semantic_name[i].c_str();
		desc_vec[i].SemanticIndex = 0;
		desc_vec[i].InstanceDataStepRate = 0;
		desc_vec[i].InputSlot = 0;
		desc_vec[i].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
		desc_vec[i].Format = format[i];
		desc_vec[i].AlignedByteOffset = i == 0 ? 0 : D3D11_APPEND_ALIGNED_ELEMENT;
	}

	ID3D11InputLayout * input_layout;
	HRESULT hr;
	
	hr = m_Device->CreateInputLayout(desc_vec.data(), desc_size, vertex_shader_buffer->GetBufferPointer(), vertex_shader_buffer->GetBufferSize() , &input_layout);
	if (FAILED(hr))
	{
		return nullptr;
	}
	return input_layout;
}

void D3DGraphics::Render()
{
	float color[4];
	color[0] = 0.4f;
	color[1] = 0.4f;
	color[2] = 0.4f;
	color[3] = 1.0f;
	m_DeviceContext->ClearRenderTargetView(m_RTV, color);
	m_DeviceContext->ClearDepthStencilView(m_DepthStencilView, D3D11_CLEAR_DEPTH , 1.0f, 0);
	m_DeviceContext->OMSetRenderTargets(1, &m_RTV, m_DepthStencilView);

}

void D3DGraphics::Present()
{
	m_SwapChain->Present(0, 0);
}

ID3D11ShaderResourceView* D3DGraphics::CreateTexture2D(UINT * data, UINT width, UINT height)
{
	D3D11_TEXTURE2D_DESC tdesc;
	tdesc.ArraySize = 1;
	tdesc.BindFlags = D3D11_BIND_SHADER_RESOURCE;
	tdesc.CPUAccessFlags = 0;
	tdesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	tdesc.Height = height;
	tdesc.Width = width;
	tdesc.MipLevels = 1;
	tdesc.MiscFlags = 0;
	tdesc.SampleDesc.Count = 1;
	tdesc.SampleDesc.Quality = 0;
	tdesc.Usage = D3D11_USAGE_DEFAULT;

	D3D11_SUBRESOURCE_DATA sub_data;
	sub_data.pSysMem = data;
	sub_data.SysMemPitch = width * sizeof( UINT ); 
	sub_data.SysMemSlicePitch = width * height * sizeof(UINT);

	ID3D11Texture2D * texture;

	HRESULT hr = m_Device->CreateTexture2D(&tdesc, &sub_data, &texture);
	if (FAILED(hr))
	{
		return nullptr;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC svdesc;
	svdesc.Texture2D.MipLevels = 1;
	svdesc.Texture2D.MostDetailedMip = 0;

	svdesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	svdesc.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;

	return CreateShaderResourceView(texture , &svdesc );

}

ID3D11DeviceContext * D3DGraphics::GetDeviceContext()
{
	return m_DeviceContext;
}

ID3D11SamplerState * D3DGraphics::CreateSamplerState()
{
	D3D11_SAMPLER_DESC sdesc;
	sdesc.Filter = D3D11_FILTER_ANISOTROPIC;
	sdesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	sdesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	sdesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	sdesc.MipLODBias = 0.0f;
	sdesc.MaxAnisotropy = 1;
	sdesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	sdesc.BorderColor[0] = 0;
	sdesc.BorderColor[1] = 0;
	sdesc.BorderColor[2] = 0;
	sdesc.BorderColor[3] = 0;
	sdesc.MinLOD = 0;
	sdesc.MaxLOD = D3D11_FLOAT32_MAX;

	ID3D11SamplerState* sampler_state;
	HRESULT hr = m_Device->CreateSamplerState(&sdesc, &sampler_state);

	if (FAILED(hr))
	{
		return NULL;
	}
	return sampler_state;
}

void D3DGraphics::OutputErrorMessage(ID3D10Blob * errorMessage, HWND hwnd, WCHAR * shaderFileName)
{
	char * compileError;
	unsigned long buffersize, i;
	std::ofstream fout;
	fout.open("error.txt");
	compileError = (char*)errorMessage->GetBufferPointer();
	buffersize = errorMessage->GetBufferSize();
	for (int i = 0; i < buffersize; i++)
	{
		fout << compileError[i];
	}
	fout.close();
	errorMessage->Release();
	errorMessage = 0;
	MessageBox(hwnd, L"Compile Shader Error£¡", shaderFileName, MB_OK);
}


