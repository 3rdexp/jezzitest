#include "test.h"

/*----------------------------------------------------------------------------//
SkinControlImpl<ControlT, BaseT> : BaseT
  Draw(HDC, class/part/state/prop, dest)

SkinFrameImpl<ControlT, WindowT> : WindowT
  DrawCaption
  DrawSystemButton

SkinButton : SkinControlImpl<Button, CButton>
UserFrame : SkinControlImpl<UserFrame, SkinFrameImpl<UserFrame, CWindow> >
\\----------------------------------------------------------------------------*/

struct CWindow {};
struct CButton : public CWindow {};

template<class ControlT, class WindowT>
struct SkinFrameImpl : public WindowT
{
    void DrawCaption()
    {
        ControlT * pT = static_cast<ControlT*>(this);
        pT->Draw();
    }
    void DrawSystemButton()
    {
        ControlT * pT = static_cast<ControlT*>(this);
        pT->Draw();
    }
};

template<class ControlT, class BaseT>
struct SkinControlImpl : public BaseT
{
    void Draw()
    {
    }
};

struct SkinButton : public SkinControlImpl<SkinButton, CButton>
{
};

struct UserFrame : public SkinControlImpl<UserFrame, SkinFrameImpl<UserFrame, CWindow> >
{
};


void test_herit()
{
    SkinButton sb;

    UserFrame uf;
    uf.DrawCaption();
}
