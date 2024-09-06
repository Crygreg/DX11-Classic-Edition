#pragma once
#include "../pch.h"
#include <d3d.h>
#include "../Logger.h"
#include <vector>
#include "../BaseGraphicsEngine.h"
#include "../D3D11VertexBuffer.h"
#include "../Engine.h"

class MyDirect3DVertexBuffer7 : public IDirect3DVertexBuffer7 {
public:
	MyDirect3DVertexBuffer7( const D3DVERTEXBUFFERDESC& originalDesc ) {
		DebugWrite( "MyDirect3DVertexBuffer7::MyDirect3DVertexBuffer7\n" );

		// Save original desc
		OriginalDesc = originalDesc;

		// Create our own buffer
		XLE( Engine::GraphicsEngine->CreateVertexBuffer( &VertexBuffer ) );

		// Initialize it
		XLE( VertexBuffer->Init( nullptr, OriginalDesc.dwNumVertices * ComputeFVFSize( OriginalDesc.dwFVF ), D3D11VertexBuffer::EBindFlags::B_VERTEXBUFFER, D3D11VertexBuffer::EUsageFlags::U_DYNAMIC, D3D11VertexBuffer::ECPUAccessFlags::CA_WRITE ) );

		// Start with 1 reference
		RefCount = 1;
	}

    ~MyDirect3DVertexBuffer7() {
        delete VertexBuffer;
    }

	/*** IUnknown methods ***/
	HRESULT STDMETHODCALLTYPE QueryInterface( REFIID riid, void** ppvObj ) {
		DebugWrite( "MyDirect3DVertexBuffer7::QueryInterface\n" );
		return S_OK;
	}

	ULONG STDMETHODCALLTYPE AddRef() {
		DebugWrite( "MyDirect3DVertexBuffer7::AddRef\n" );
		return ++RefCount;
	}

	ULONG STDMETHODCALLTYPE Release() {
		DebugWrite( "MyDirect3DVertexBuffer7::Release\n" );

		if ( --RefCount == 0 ) {
			delete this;
			return 0;
		}

		return RefCount;
	}

	/*** IDirect3DVertexBuffer7 methods ***/
	HRESULT STDMETHODCALLTYPE GetVertexBufferDesc( LPD3DVERTEXBUFFERDESC lpVBDesc ) {
		DebugWrite( "MyDirect3DVertexBuffer7::GetVertexBufferDesc\n" );

		if ( lpVBDesc )*lpVBDesc = OriginalDesc;
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE Lock( DWORD dwFlags, LPVOID* lplpData, LPDWORD lpdwSize ) {
		DebugWrite( "MyDirect3DVertexBuffer7::Lock\n" );

		// Pass the lock-call through to our engine
		UINT size = 0;
		VertexBuffer->Map( D3D11VertexBuffer::EMapFlags::M_WRITE_DISCARD, lplpData, &size );
		if ( lpdwSize )*lpdwSize = size;

		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE Optimize( LPDIRECT3DDEVICE7 lpD3DDevice, DWORD dwFlags ) {
		// Not needed
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE ProcessVertices( DWORD dwVertexOp, DWORD dwDestIndex, DWORD dwCount, LPDIRECT3DVERTEXBUFFER7 lpSrcBuffer, DWORD dwSrcIndex, LPDIRECT3DDEVICE7 lpD3DDevice, DWORD dwFlags ) {
		LogWarn() << "Unimplemented method: MyDirect3DVertexBuffer7::ProcessVertices";
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE ProcessVerticesStrided( DWORD dwVertexOp, DWORD dwDestIndex, DWORD dwCount, LPD3DDRAWPRIMITIVESTRIDEDDATA lpVertexArray, DWORD dwSrcIndex, LPDIRECT3DDEVICE7 lpD3DDevice, DWORD dwFlags ) {
		LogWarn() << "Unimplemented method: MyDirect3DVertexBuffer7::ProcessVerticesStrided";
		return S_OK;
	}

	HRESULT STDMETHODCALLTYPE Unlock() {
		DebugWrite( "MyDirect3DVertexBuffer7::Unlock\n" );
		VertexBuffer->Unmap();
		return S_OK;
	}

	/** Returns the number of vertices inside this buffer */
	int GetNumVertices() {
		return OriginalDesc.dwNumVertices;
	}

	/** Returns the actual vertex buffer */
	D3D11VertexBuffer* GetVertexBuffer() {
		return VertexBuffer;
	}

private:

	/** Original desc D3D7 created the buffer with */
	D3DVERTEXBUFFERDESC OriginalDesc;

	/** Our own vertex buffer */
	D3D11VertexBuffer* VertexBuffer;

	/** Referencecount on this */
	int RefCount;
};
