//--------------------------------------------------------------------------------------
// File: EmptyProject.cpp
//
// Copyright (c) Microsoft Corporation. All rights reserved.
//--------------------------------------------------------------------------------------
#include "DXUT.h"
#include "resource.h"
#include <stack>

using namespace std;

#define WINDOW_WIDTH 1920
#define WINDOW_HEIGHT 1080

#define BACKGROUND_WIDTH 740
#define BACKGROUND_HEIGHT 932

#define MAP_STATE_EMPTY 0
#define MAP_STATE_VISITED 1
#define MAP_STATE_EDGE 2
#define MAP_STATE_VISITING 3
#define MAP_STATE_VIRTUAR_EMPTY 4
#define MAP_STATE_TEMP 5


struct Point
{
    int x;
    int y;
    Point(int xx, int yy)
        :x(xx), y(yy)
    {
    }
};

enum PlayerState {
    ON_EDGE,
    GENERATING,
};


LPDIRECT3DTEXTURE9* backgroundTex;
LPDIRECT3DTEXTURE9* maskTex;
LPDIRECT3DTEXTURE9* player;

LPD3DXSPRITE spr;

int playerX = 100;
int playerY = 300;


int maskP[BACKGROUND_WIDTH * BACKGROUND_HEIGHT];
int backP[BACKGROUND_WIDTH * BACKGROUND_HEIGHT];
int map[BACKGROUND_WIDTH * BACKGROUND_HEIGHT];


int fMap[BACKGROUND_WIDTH * BACKGROUND_HEIGHT];
int binaryMap[BACKGROUND_WIDTH * BACKGROUND_HEIGHT];

void FloodFill(int x, int y, int s, int n)
{
    /*
    if (x >= BACKGROUND_WIDTH || x < 0 || y >= BACKGROUND_HEIGHT || y < 0)
    {
        return;
    }
    if (fMap[y * BACKGROUND_WIDTH + x] == s)
    {
        fMap[y * BACKGROUND_WIDTH + x] = n;
    }
    else
    {
        return;
    }

    FloodFill(x - 1, y, s, n);
    FloodFill(x + 1, y, s, n);
    FloodFill(x, y - 1, s, n);
    FloodFill(x, y + 1, s, n);

    */
    int a = 0;
    stack<Point> points;
    points.push(Point(x, y));


    while (!points.empty())
    {
        const Point p = points.top();
        points.pop();

        if (p.x < 0) continue;
        if (p.x >= BACKGROUND_WIDTH) continue;
        if (p.y < 0) continue;
        if (p.y >= BACKGROUND_HEIGHT) continue;

        if (fMap[p.y * BACKGROUND_WIDTH + p.x] == s && fMap[p.y * BACKGROUND_WIDTH + p.x] != n)
        {
            fMap[p.y * BACKGROUND_WIDTH + p.x] = n;
            a++;
        }
        else
        {
            continue;
        }

        points.push(Point(p.x - 1, p.y));
        points.push(Point(p.x + 1, p.y));
        points.push(Point(p.x, p.y-1));
        points.push(Point(p.x, p.y+1));
    }
    int b = 0;
}

void GetLend()
{
    for (int i = 0; i < BACKGROUND_WIDTH * BACKGROUND_HEIGHT; i++)
    {
        if (map[i] == MAP_STATE_EMPTY)
            fMap[i] = MAP_STATE_VIRTUAR_EMPTY;
        if (map[i] == MAP_STATE_VISITED)
            fMap[i] = MAP_STATE_VISITED;
        if (map[i] == MAP_STATE_EDGE)
            fMap[i] = MAP_STATE_EDGE;
        if (map[i] == MAP_STATE_VISITING)
            fMap[i] = MAP_STATE_VISITING;
    }

    FloodFill(2, 2, MAP_STATE_VIRTUAR_EMPTY, MAP_STATE_EMPTY);

    for (int i = 0; i < BACKGROUND_WIDTH * BACKGROUND_HEIGHT; i++)
    {
        if (fMap[i] == MAP_STATE_VIRTUAR_EMPTY)
        {
            fMap[i] = MAP_STATE_VISITED;
        }
        if (fMap[i] == MAP_STATE_VISITING)
        {
            fMap[i] = MAP_STATE_EDGE;
        }
    }

    /*for (int i = 0; i < BACKGROUND_WIDTH * BACKGROUND_HEIGHT; i++)
    {
        map[i] = fMap[i];
    }*/

    memcpy(map, fMap, BACKGROUND_WIDTH * BACKGROUND_HEIGHT * sizeof(int));


    int discoveredPixelCount = 0;

    for (int i = 0; i < BACKGROUND_WIDTH * BACKGROUND_HEIGHT; ++i)
    {
        if (map[i] == MAP_STATE_EMPTY)
        {
            binaryMap[i] = MAP_STATE_EMPTY;
        }
        else
        {
            discoveredPixelCount++;
            binaryMap[i] = MAP_STATE_VISITED;
        }
    }

    memcpy(map, binaryMap, BACKGROUND_WIDTH * BACKGROUND_HEIGHT * sizeof(int));

}

void MapUpdate()
{
    RECT tdr = { 0, 0, BACKGROUND_WIDTH, BACKGROUND_HEIGHT };
    D3DLOCKED_RECT tlr;


    if (SUCCEEDED((*backgroundTex)->LockRect(0, &tlr, &tdr, 0)))
    {
        DWORD* p = (DWORD*)tlr.pBits;

        for (int i = 0; i < BACKGROUND_WIDTH * BACKGROUND_HEIGHT; i++)
        {
            if (map[i] == MAP_STATE_EMPTY)
            {
                p[i] = maskP[i];
            }
            if (map[i] == MAP_STATE_EDGE)
            {
                p[i] = D3DCOLOR_ARGB(255, 0, 0, 0);
            }
            if (map[i] == MAP_STATE_VISITING)
            {
                p[i] = D3DCOLOR_ARGB(255, 0, 0, 255);
            }
            if (map[i] == MAP_STATE_VISITED)
            {
                p[i] = backP[i];
            }
        }


        (*backgroundTex)->UnlockRect(0);

    }
}

bool CALLBACK IsD3D9DeviceAcceptable( D3DCAPS9* pCaps, D3DFORMAT AdapterFormat, D3DFORMAT BackBufferFormat,
                                      bool bWindowed, void* pUserContext )
{
    IDirect3D9* pD3D = DXUTGetD3D9Object();
    if( FAILED( pD3D->CheckDeviceFormat( pCaps->AdapterOrdinal, pCaps->DeviceType,
                                         AdapterFormat, D3DUSAGE_QUERY_POSTPIXELSHADER_BLENDING,
                                         D3DRTYPE_TEXTURE, BackBufferFormat ) ) )
        return false;

    return true;
}


bool CALLBACK ModifyDeviceSettings( DXUTDeviceSettings* pDeviceSettings, void* pUserContext )
{
    return true;
}


HRESULT CALLBACK OnD3D9CreateDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc,
                                     void* pUserContext )
{
    for (int i = 0; i < BACKGROUND_WIDTH * BACKGROUND_HEIGHT; i++)
    {
        map[i] = MAP_STATE_EMPTY;
    }
    for (int y = 300; y <= 500; y++)
    {
        for (int x = 100; x <= 500; x++)
        {
            map[y * BACKGROUND_WIDTH + x] = MAP_STATE_VISITED;
        }
    }

    for (int y = 300; y <= 500; y++)
    {
        map[y * BACKGROUND_WIDTH + 100] = MAP_STATE_EDGE;
        map[y * BACKGROUND_WIDTH + 500] = MAP_STATE_EDGE;
    }
    for (int x = 100; x <= 500; x++)
    {
        map[300 * BACKGROUND_WIDTH + x] = MAP_STATE_EDGE;
        map[500 * BACKGROUND_WIDTH + x] = MAP_STATE_EDGE;
    }


    backgroundTex = new LPDIRECT3DTEXTURE9();
    D3DXCreateTextureFromFileExA(pd3dDevice
        , "resource/background.png"
        , D3DX_DEFAULT_NONPOW2
        , D3DX_DEFAULT_NONPOW2
        , 0
        , 0
        , D3DFMT_UNKNOWN
        , D3DPOOL_MANAGED
        , D3DX_DEFAULT
        , D3DX_DEFAULT
        , 0
        , nullptr
        , nullptr
        , backgroundTex);

    maskTex = new LPDIRECT3DTEXTURE9();
    D3DXCreateTextureFromFileExA(
        pd3dDevice
        , "resource/mask.png"
        , D3DX_DEFAULT_NONPOW2
        , D3DX_DEFAULT_NONPOW2
        , 0, 0
        , D3DFMT_UNKNOWN
        , D3DPOOL_MANAGED
        , D3DX_DEFAULT
        , D3DX_DEFAULT
        , 0
        , nullptr
        , nullptr
        , maskTex);

    player = new LPDIRECT3DTEXTURE9();
    D3DXCreateTextureFromFileExA(
        pd3dDevice
        , "resource/player.png"
        , D3DX_DEFAULT_NONPOW2
        , D3DX_DEFAULT_NONPOW2
        , 0, 0
        , D3DFMT_UNKNOWN
        , D3DPOOL_MANAGED
        , D3DX_DEFAULT
        , D3DX_DEFAULT
        , 0
        , nullptr
        , nullptr
        , player);


    RECT tdr = { 0, 0, BACKGROUND_WIDTH, BACKGROUND_HEIGHT };
    D3DLOCKED_RECT tlr;
    
    if (SUCCEEDED((*maskTex)->LockRect(0, &tlr, &tdr, 0)))
    {

        DWORD* p = (DWORD*)tlr.pBits;
        memcpy(maskP, p, BACKGROUND_WIDTH * BACKGROUND_HEIGHT * 4);

        (*maskTex)->UnlockRect(0);

    }

    if (SUCCEEDED((*backgroundTex)->LockRect(0, &tlr, &tdr, 0)))
    {

        DWORD* p = (DWORD*)tlr.pBits;
        memcpy(backP, p, BACKGROUND_WIDTH * BACKGROUND_HEIGHT * 4);

        (*backgroundTex)->UnlockRect(0);

    }

    MapUpdate();


    D3DXCreateSprite(pd3dDevice, &spr);

    return S_OK;
}


HRESULT CALLBACK OnD3D9ResetDevice( IDirect3DDevice9* pd3dDevice, const D3DSURFACE_DESC* pBackBufferSurfaceDesc,
                                    void* pUserContext )
{
    return S_OK;
}


void CALLBACK OnFrameMove( double fTime, float fElapsedTime, void* pUserContext )
{
    if ((GetAsyncKeyState(VK_LEFT) & 0x8000)!= 0)
    {
        if (map[playerY * BACKGROUND_WIDTH + playerX] == MAP_STATE_EDGE
            && map[playerY * BACKGROUND_WIDTH + (playerX - 1)] == MAP_STATE_EDGE)
        {
            playerX -= 1;
        }
        
        if (map[playerY * BACKGROUND_WIDTH + playerX] == MAP_STATE_EDGE
            && map[playerY * BACKGROUND_WIDTH + (playerX - 1)] == MAP_STATE_EMPTY)
        {
            map[playerY * BACKGROUND_WIDTH + (playerX - 1)] = MAP_STATE_VISITING;
            playerX -= 1;
        }

        if (map[playerY * BACKGROUND_WIDTH + playerX] == MAP_STATE_VISITING
            && map[playerY * BACKGROUND_WIDTH + (playerX - 1)] == MAP_STATE_EMPTY)
        {
            map[playerY * BACKGROUND_WIDTH + (playerX - 1)] = MAP_STATE_VISITING;
            playerX -= 1;
        }

        if (map[playerY * BACKGROUND_WIDTH + playerX] == MAP_STATE_VISITING
            && map[playerY * BACKGROUND_WIDTH + (playerX - 1)] == MAP_STATE_EDGE)
        {
            GetLend();
            playerX -= 1;
        }

    }
    else if ((GetAsyncKeyState(VK_RIGHT) & 0x8000) != 0)
    {
        if (map[playerY * BACKGROUND_WIDTH + playerX] == MAP_STATE_EDGE
            && map[playerY * BACKGROUND_WIDTH + (playerX + 1)] == MAP_STATE_EDGE)
        {
            playerX += 1;
        }

        if (map[playerY * BACKGROUND_WIDTH + playerX] == MAP_STATE_EDGE
            && map[playerY * BACKGROUND_WIDTH + (playerX + 1)] == MAP_STATE_EMPTY)
        {
            map[playerY * BACKGROUND_WIDTH + (playerX + 1)] = MAP_STATE_VISITING;
            playerX += 1;
        }

        if (map[playerY * BACKGROUND_WIDTH + playerX] == MAP_STATE_VISITING
            && map[playerY * BACKGROUND_WIDTH + (playerX + 1)] == MAP_STATE_EMPTY)
        {
            map[playerY * BACKGROUND_WIDTH + (playerX + 1)] = MAP_STATE_VISITING;
            playerX += 1;
        }

        if (map[playerY * BACKGROUND_WIDTH + playerX] == MAP_STATE_VISITING
            && map[playerY * BACKGROUND_WIDTH + (playerX + 1)] == MAP_STATE_EDGE)
        {
            playerX += 1;
            GetLend();
        }
    }
    else if ((GetAsyncKeyState(VK_UP) & 0x8000) != 0)
    {
        if (map[playerY * BACKGROUND_WIDTH + playerX] == MAP_STATE_EDGE
            && map[(playerY - 1) * BACKGROUND_WIDTH + playerX] == MAP_STATE_EDGE)
        {
            playerY -= 1;
        }

        if (map[playerY * BACKGROUND_WIDTH + playerX] == MAP_STATE_EDGE
            && map[(playerY - 1) * BACKGROUND_WIDTH + playerX] == MAP_STATE_EMPTY)
        {
            map[(playerY - 1) * BACKGROUND_WIDTH + playerX] = MAP_STATE_VISITING;
            playerY -= 1;
        }

        if (map[playerY * BACKGROUND_WIDTH + playerX] == MAP_STATE_VISITING
            && map[(playerY - 1) * BACKGROUND_WIDTH + playerX] == MAP_STATE_EMPTY)
        {
            map[(playerY - 1) * BACKGROUND_WIDTH + playerX] = MAP_STATE_VISITING;
            playerY -= 1;
        }

        if (map[playerY * BACKGROUND_WIDTH + playerX] == MAP_STATE_VISITING
            && map[(playerY - 1) * BACKGROUND_WIDTH + playerX] == MAP_STATE_EDGE)
        {
            playerY -= 1;
            GetLend();
        }
    }
    else if ((GetAsyncKeyState(VK_DOWN) & 0x8000) != 0)
    {
        if (map[playerY * BACKGROUND_WIDTH + playerX] == MAP_STATE_EDGE
            && map[(playerY + 1) * BACKGROUND_WIDTH + playerX] == MAP_STATE_EDGE)
        {
            playerY += 1;
        }

        if (map[playerY * BACKGROUND_WIDTH + playerX] == MAP_STATE_EDGE
            && map[(playerY + 1) * BACKGROUND_WIDTH + playerX] == MAP_STATE_EMPTY)
        {
            map[(playerY + 1) * BACKGROUND_WIDTH + playerX] = MAP_STATE_VISITING;
            playerY += 1;
        }

        if (map[playerY * BACKGROUND_WIDTH + playerX] == MAP_STATE_VISITING
            && map[(playerY + 1) * BACKGROUND_WIDTH + playerX] == MAP_STATE_EMPTY)
        {
            map[(playerY + 1) * BACKGROUND_WIDTH + playerX] = MAP_STATE_VISITING;
            playerY += 1;
        }

        if (map[playerY * BACKGROUND_WIDTH + playerX] == MAP_STATE_VISITING
            && map[(playerY + 1) * BACKGROUND_WIDTH + playerX] == MAP_STATE_EDGE)
        {
            playerY += 1;
            GetLend();
        }
    }

    MapUpdate();

}


void CALLBACK OnD3D9FrameRender( IDirect3DDevice9* pd3dDevice, double fTime, float fElapsedTime, void* pUserContext )
{
    HRESULT hr;

    // Clear the render target and the zbuffer 
    V( pd3dDevice->Clear( 0, NULL, D3DCLEAR_TARGET | D3DCLEAR_ZBUFFER, D3DCOLOR_ARGB( 0, 45, 50, 170 ), 1.0f, 0 ) );

    // Render the scene
    if( SUCCEEDED( pd3dDevice->BeginScene() ) )
    {
        spr->Begin(D3DXSPRITE_ALPHABLEND);
        spr->Draw(*backgroundTex, nullptr, nullptr, nullptr, D3DCOLOR_ARGB(255, 255, 255, 255));

        D3DXVECTOR3 pos = { (float)playerX, (float)playerY, 0 };
        D3DXVECTOR3 cen = { 2, 2, 0 };

        spr->Draw(*player, nullptr, &cen, &pos, D3DCOLOR_ARGB(255, 255, 255, 255));
        spr->End();

        V( pd3dDevice->EndScene() );
    }
}


LRESULT CALLBACK MsgProc( HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
                          bool* pbNoFurtherProcessing, void* pUserContext )
{
    return 0;
}


void CALLBACK OnD3D9LostDevice( void* pUserContext )
{
}


void CALLBACK OnD3D9DestroyDevice( void* pUserContext )
{
    (*backgroundTex)->Release();
    (*maskTex)->Release();
    (*player)->Release();
    spr->Release();
}


INT WINAPI wWinMain( HINSTANCE, HINSTANCE, LPWSTR, int )
{
#if defined(DEBUG) | defined(_DEBUG)
    _CrtSetDbgFlag( _CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF );
#endif

    DXUTSetCallbackD3D9DeviceAcceptable( IsD3D9DeviceAcceptable );
    DXUTSetCallbackD3D9DeviceCreated( OnD3D9CreateDevice );
    DXUTSetCallbackD3D9DeviceReset( OnD3D9ResetDevice );
    DXUTSetCallbackD3D9FrameRender( OnD3D9FrameRender );
    DXUTSetCallbackD3D9DeviceLost( OnD3D9LostDevice );
    DXUTSetCallbackD3D9DeviceDestroyed( OnD3D9DestroyDevice );
    DXUTSetCallbackDeviceChanging( ModifyDeviceSettings );
    DXUTSetCallbackMsgProc( MsgProc );
    DXUTSetCallbackFrameMove( OnFrameMove );


    DXUTInit( true, true ); // Parse the command line and show msgboxes
    DXUTSetHotkeyHandling( true, true, true );  // handle the default hotkeys
    DXUTSetCursorSettings( true, true ); // Show the cursor and clip it when in full screen
    DXUTCreateWindow( L"GameExam" );
    DXUTCreateDevice( true, WINDOW_WIDTH, WINDOW_HEIGHT );

    DXUTMainLoop();


    return DXUTGetExitCode();
}


