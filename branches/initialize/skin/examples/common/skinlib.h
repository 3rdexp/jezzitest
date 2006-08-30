#pragma once

HMODULE hLibSkin = 0;

void LoadSkinDll()
{
    if (!hLibSkin)
        hLibSkin = LoadLibrary("skin.dll");

    // TODO: 直接 MessageBox 报告错误
}

void UnloadSkinDll()
{
    if (hLibSkin)
        FreeLibrary(hLibSkin);
    hLibSkin = 0;
}

#define SkinFrameProcName "SkinFrameProc"
typedef LRESULT (WINAPI *SkinFrameProcT)(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

SkinFrameProcT GetSkinFrameProc()
{
    if (hLibSkin)
    {
        return (SkinFrameProcT)GetProcAddress(hLibSkin, SkinFrameProcName);
    }
    return 0;
}