//--------------------------------------------------------------------------------------
// File: TemplateMain.cpp
//
// BTH-D3D-Template
//
// Copyright (c) Stefan Petersson 2013. All rights reserved.
//--------------------------------------------------------------------------------------
#include "stdafx.h"

#include "ComputeHelp.h"
#include "D3D11Timer.h"
#include <vector>

/*	DirectXTex library - for usage info, see http://directxtex.codeplex.com/
	
	Usage example (may not be the "correct" way, I just wrote it in a hurry):

	DirectX::ScratchImage img;
	DirectX::TexMetadata meta;
	ID3D11ShaderResourceView* srv = nullptr;
	if(SUCCEEDED(hr = DirectX::LoadFromDDSFile(_T("C:\\Program Files (x86)\\Microsoft DirectX SDK (June 2010)\\Samples\\Media\\Dwarf\\Armor.dds"), 0, &meta, img)))
	{
		//img loaded OK
		if(SUCCEEDED(hr = DirectX::CreateShaderResourceView(g_Device, img.GetImages(), img.GetImageCount(), meta, &srv)))
		{
			//srv created OK
		}
	}
*/
#include <DirectXTex.h>
#include <DirectXMath.h>
#if defined( DEBUG ) || defined( _DEBUG )
#pragma comment(lib, "DirectXTexD.lib")
#else
#pragma comment(lib, "DirectXTex.lib")
#endif

#include "InputClass.h"
#include "Camera.h"

//--------------------------------------------------------------------------------------
// Global Variables
//--------------------------------------------------------------------------------------
HINSTANCE				g_hInst					= NULL;  
HWND					g_hWnd					= NULL;

IDXGISwapChain*         g_SwapChain				= NULL;
ID3D11Device*			g_Device				= NULL;
ID3D11DeviceContext*	g_DeviceContext			= NULL;

ID3D11UnorderedAccessView*  g_BackBufferUAV		= NULL;  // compute output

ComputeWrap*			g_ComputeSys			= NULL;
ComputeShader*			g_ComputeShader			= NULL;

D3D11Timer*				g_Timer					= NULL;

///////////////////////////////////////////////////New variables//////////////////////////
ID3D11Buffer*			m_everyFrameBuffer = nullptr;
ID3D11Buffer*			m_dispatchBuffer = nullptr;

//DirectX::XMFLOAT3 m_cameraPosition = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
//DirectX::XMFLOAT3 m_upVector = DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f);
//DirectX::XMFLOAT3 m_cameraDirection = DirectX::XMFLOAT3(0.0f, 0.0f, 1.0f);

struct DispatchBufferStruct
{
	float screenWidth;
	float screenHeight;
	int x_dispatchCound;
	int y_dispatchCound;
};
struct EveryFrameStruct
{
	DirectX::XMFLOAT4 cameraPosition;
	DirectX::XMFLOAT4X4 inverseProjection;
	DirectX::XMFLOAT4X4 inverseView;
};
//ConstantBuffer m_cBuffer;
///////////////////////////////////////////////////New variables//////////////////////////

int g_Width, g_Height;

//--------------------------------------------------------------------------------------
// Forward declarations
//--------------------------------------------------------------------------------------
HRESULT             InitWindow( HINSTANCE hInstance, int nCmdShow );
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
HRESULT				Render(float deltaTime);
HRESULT				Update(float deltaTime);
void				Shutdown();
void				Initialize();
ID3D11Buffer*		CreateDynamicConstantBuffer(int p_size);
void				UpdateDispatchBuffer(int p_x, int p_y);
void				UpdateEveryFrameBuffer();

char* FeatureLevelToString(D3D_FEATURE_LEVEL featureLevel)
{
	if(featureLevel == D3D_FEATURE_LEVEL_11_0)
		return "11.0";
	if(featureLevel == D3D_FEATURE_LEVEL_10_1)
		return "10.1";
	if(featureLevel == D3D_FEATURE_LEVEL_10_0)
		return "10.0";

	return "Unknown";
}

//--------------------------------------------------------------------------------------
// Create Direct3D device and swap chain
//--------------------------------------------------------------------------------------
HRESULT Init()
{
	HRESULT hr = S_OK;;

	RECT rc;
	GetClientRect( g_hWnd, &rc );
	g_Width = rc.right - rc.left;;
	g_Height = rc.bottom - rc.top;

	UINT createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	D3D_DRIVER_TYPE driverType;

	D3D_DRIVER_TYPE driverTypes[] = 
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_REFERENCE,
	};
	UINT numDriverTypes = sizeof(driverTypes) / sizeof(driverTypes[0]);

	DXGI_SWAP_CHAIN_DESC sd;
	ZeroMemory( &sd, sizeof(sd) );
	sd.BufferCount = 1;
	sd.BufferDesc.Width = g_Width;
	sd.BufferDesc.Height = g_Height;
	sd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	sd.BufferDesc.RefreshRate.Numerator = 60;
	sd.BufferDesc.RefreshRate.Denominator = 1;
	sd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT | DXGI_USAGE_UNORDERED_ACCESS;
	sd.OutputWindow = g_hWnd;
	sd.SampleDesc.Count = 1;
	sd.SampleDesc.Quality = 0;
	sd.Windowed = TRUE;

	D3D_FEATURE_LEVEL featureLevelsToTry[] = {
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0
	};
	D3D_FEATURE_LEVEL initiatedFeatureLevel;

	for( UINT driverTypeIndex = 0; driverTypeIndex < numDriverTypes; driverTypeIndex++ )
	{
		driverType = driverTypes[driverTypeIndex];
		hr = D3D11CreateDeviceAndSwapChain(
			NULL,
			driverType,
			NULL,
			createDeviceFlags,
			featureLevelsToTry,
			ARRAYSIZE(featureLevelsToTry),
			D3D11_SDK_VERSION,
			&sd,
			&g_SwapChain,
			&g_Device,
			&initiatedFeatureLevel,
			&g_DeviceContext);

		if( SUCCEEDED( hr ) )
		{
			char title[256];
			sprintf_s(
				title,
				sizeof(title),
				"BTH - Direct3D 11.0 Template | Direct3D 11.0 device initiated with Direct3D %s feature level",
				FeatureLevelToString(initiatedFeatureLevel)
			);
			SetWindowTextA(g_hWnd, title);

			break;
		}
	}
	if( FAILED(hr) )
		return hr;

	// Create a render target view
	ID3D11Texture2D* pBackBuffer;
	hr = g_SwapChain->GetBuffer( 0, __uuidof( ID3D11Texture2D ), (LPVOID*)&pBackBuffer );
	if( FAILED(hr) )
		return hr;

	// create shader unordered access view on back buffer for compute shader to write into texture
	hr = g_Device->CreateUnorderedAccessView( pBackBuffer, NULL, &g_BackBufferUAV );

	//create helper sys and compute shader instance
	g_ComputeSys = new ComputeWrap(g_Device, g_DeviceContext);
	g_ComputeShader = g_ComputeSys->CreateComputeShader(_T("../Shaders/BasicCompute.fx"), NULL, "main", NULL);
	g_Timer = new D3D11Timer(g_Device, g_DeviceContext);

	Initialize();
	return S_OK;
}
void Initialize()
{
	Camera::GetInstance()->Initialize();
	InputClass::GetInstance()->RegisterKey(VkKeyScan('q'));
	InputClass::GetInstance()->RegisterKey(VkKeyScan('w'));
	InputClass::GetInstance()->RegisterKey(VkKeyScan('e'));
	InputClass::GetInstance()->RegisterKey(VkKeyScan('a'));
	InputClass::GetInstance()->RegisterKey(VkKeyScan('s'));
	InputClass::GetInstance()->RegisterKey(VkKeyScan('d'));

	//DispatchBufferStruct cBuffer;
	//cBuffer.screenHeight = 0;
	//cBuffer.screenWidth = 0;
	//cBuffer.x_dispatchCound = 0;
	//cBuffer.y_dispatchCound = 0;
	//m_constantBuffer = g_ComputeSys->CreateConstantBuffer(sizeof(cBuffer), &cBuffer, nullptr);

	HRESULT hr = S_OK;

	int ByteWidth;

	ByteWidth = sizeof(DispatchBufferStruct);
	m_dispatchBuffer = CreateDynamicConstantBuffer(ByteWidth);

	ByteWidth = sizeof(EveryFrameStruct);
	m_everyFrameBuffer = CreateDynamicConstantBuffer(ByteWidth);



}
ID3D11Buffer* CreateDynamicConstantBuffer(int p_size)
{
	HRESULT hr = S_OK;
	ID3D11Buffer* buffer;
	D3D11_BUFFER_DESC desc;
	desc.ByteWidth = p_size;
	desc.Usage = D3D11_USAGE_DYNAMIC;
	desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	desc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	desc.MiscFlags = 0;
	desc.StructureByteStride = 0;

	hr = g_Device->CreateBuffer(&desc, NULL, &buffer);
	if (FAILED(hr))
	{
		return NULL;
	}
	else
	{
		return buffer;
	}
	
}
HRESULT Update(float deltaTime)
{
	Camera::GetInstance()->Update(deltaTime);



	UpdateEveryFrameBuffer();
	return S_OK;
}
void UpdateDispatchBuffer(int p_x, int p_y)
{
	HRESULT hr = S_OK;

	D3D11_MAPPED_SUBRESOURCE mappedResource;
	hr = g_DeviceContext->Map(m_dispatchBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	if (FAILED(hr))
	{
		int i = 0;
	}
	DispatchBufferStruct cBuffer;
	cBuffer.screenHeight = 1600.0f;
	cBuffer.screenWidth = 1600.0f;
	cBuffer.x_dispatchCound = p_x;
	cBuffer.y_dispatchCound = p_y;

	*(DispatchBufferStruct*)mappedResource.pData = cBuffer;
	g_DeviceContext->Unmap(m_dispatchBuffer, 0);
}
void UpdateEveryFrameBuffer()
{
	HRESULT hr = S_OK;

	DirectX::XMFLOAT4X4 proj, view, invProj, invView;
	proj = Camera::GetInstance()->GetProjectionMatrix();
	view = Camera::GetInstance()->GetViewMatrix();

	DirectX::XMStoreFloat4x4(&invProj, DirectX::XMMatrixInverse(nullptr, DirectX::XMLoadFloat4x4(&proj)));
	DirectX::XMStoreFloat4x4(&invView, DirectX::XMMatrixInverse(nullptr, DirectX::XMLoadFloat4x4(&view)));


	D3D11_MAPPED_SUBRESOURCE mappedResource;
	hr = g_DeviceContext->Map(m_everyFrameBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);

	if (FAILED(hr))
	{
		int i = 0;
	}
	EveryFrameStruct cBuffer;
	cBuffer.cameraPosition = Camera::GetInstance()->GetCameraPos();
	cBuffer.inverseProjection = invProj;
	cBuffer.inverseView = invView;

	*(EveryFrameStruct*)mappedResource.pData = cBuffer;
	g_DeviceContext->Unmap(m_everyFrameBuffer, 0);
}
HRESULT Render(float deltaTime)
{
	ID3D11UnorderedAccessView* uav[] = { g_BackBufferUAV };
	ID3D11Buffer* bufferArray[] = { m_dispatchBuffer, m_everyFrameBuffer };
	g_DeviceContext->CSSetUnorderedAccessViews(0, 1, uav, NULL);


	g_DeviceContext->CSSetConstantBuffers(0, 1, bufferArray);
	g_Timer->Start();

	for (unsigned int x = 0; x < 4; x++)
	{
		for (unsigned int y = 0; y < 4; y++)
		{
			g_ComputeShader->Set();
			UpdateDispatchBuffer(x, y);
			g_DeviceContext->CSSetConstantBuffers(0, 2, bufferArray);

			g_DeviceContext->Dispatch(25, 25, 1);
			g_ComputeShader->Unset();
		}
	}
	g_Timer->Stop();




	if(FAILED(g_SwapChain->Present( 0, 0 )))
		return E_FAIL;


	char title[256];
	sprintf_s(
		title,
		sizeof(title),
		"BTH - DirectCompute DEMO - Dispatch time: %f",
		g_Timer->GetTime()
	);
	SetWindowTextA(g_hWnd, title);

	return S_OK;
}
void Shutdown()
{
	InputClass::GetInstance()->Shutdown();
	Camera::GetInstance()->Shutdown();
}

//--------------------------------------------------------------------------------------
// Entry point to the program. Initializes everything and goes into a message processing 
// loop. Idle time is used to render the scene.
//--------------------------------------------------------------------------------------
int WINAPI wWinMain( HINSTANCE hInstance, HINSTANCE hPrevInstance, LPWSTR lpCmdLine, int nCmdShow )
{
	if( FAILED( InitWindow( hInstance, nCmdShow ) ) )
		return 0;

	if( FAILED( Init() ) )
		return 0;

	__int64 cntsPerSec = 0;
	QueryPerformanceFrequency((LARGE_INTEGER*)&cntsPerSec);
	float secsPerCnt = 1.0f / (float)cntsPerSec;

	__int64 prevTimeStamp = 0;
	QueryPerformanceCounter((LARGE_INTEGER*)&prevTimeStamp);

	// Main message loop
	MSG msg = {0};
	while(WM_QUIT != msg.message)
	{
		if( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE) )
		{
			InputClass::GetInstance()->Update(msg.message, msg.wParam, msg.lParam);
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}
		else
		{
			__int64 currTimeStamp = 0;
			QueryPerformanceCounter((LARGE_INTEGER*)&currTimeStamp);
			float dt = (currTimeStamp - prevTimeStamp) * secsPerCnt;

			//render
			Update(dt);
			Render(dt);

			//Clear input
			InputClass::GetInstance()->ClearInput();

			prevTimeStamp = currTimeStamp;
		}
	}

	return (int) msg.wParam;
}


//--------------------------------------------------------------------------------------
// Register class and create window
//--------------------------------------------------------------------------------------
HRESULT InitWindow( HINSTANCE hInstance, int nCmdShow )
{
	// Register class
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX); 
	wcex.style          = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc    = WndProc;
	wcex.cbClsExtra     = 0;
	wcex.cbWndExtra     = 0;
	wcex.hInstance      = hInstance;
	wcex.hIcon          = 0;
	wcex.hCursor        = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground  = (HBRUSH)(COLOR_WINDOW+1);
	wcex.lpszMenuName   = NULL;
	wcex.lpszClassName  = _T("BTH_D3D_Template");
	wcex.hIconSm        = 0;
	if( !RegisterClassEx(&wcex) )
		return E_FAIL;

	// Create window
	g_hInst = hInstance; 
	RECT rc = { 0, 0, 800, 800 };
	AdjustWindowRect( &rc, WS_OVERLAPPEDWINDOW, FALSE );
	
	if(!(g_hWnd = CreateWindow(
							_T("BTH_D3D_Template"),
							_T("BTH - Direct3D 11.0 Template"),
							WS_OVERLAPPEDWINDOW,
							CW_USEDEFAULT,
							CW_USEDEFAULT,
							rc.right - rc.left,
							rc.bottom - rc.top,
							NULL,
							NULL,
							hInstance,
							NULL)))
	{
		return E_FAIL;
	}

	ShowWindow( g_hWnd, nCmdShow );

	return S_OK;
}


//--------------------------------------------------------------------------------------
// Called every time the application receives a message
//--------------------------------------------------------------------------------------
LRESULT CALLBACK WndProc( HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam )
{
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message) 
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		EndPaint(hWnd, &ps);
		break;

	case WM_DESTROY:
		PostQuitMessage(0);
		break;

	case WM_KEYDOWN:

		switch(wParam)
		{
			case VK_ESCAPE:
				Shutdown();
				PostQuitMessage(0);
				break;
		}
		break;

	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}
