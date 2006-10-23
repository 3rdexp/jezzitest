#include <boost/shared_ptr.hpp>
#include <ext/hash_map>

using __gnu_cxx::hash_map;


/*----------------------------------------------------------------------------//
SkinControlImpl<ControlT, BaseT> : BaseT
  Draw(HDC, class/part/state/prop, dest)

SkinFrameImpl<ControlT, WindowT> : WindowT
  DrawCaption
  DrawSystemButton

SkinButton : SkinControlImpl<Button, CButton>
UserFrame : SkinControlImpl<UserFrame, SkinFrameImpl<UserFrame, CWindow> >
\\----------------------------------------------------------------------------*/

using boost::shared_ptr;
using namespace std;

typedef int HWND;

struct CWindow {};
struct CButton : public CWindow {};
struct CEdit : public CWindow {};


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
    static hash_map<HWND, shared_ptr<ControlT> > map_;

  void Draw()
  {
    map_.insert(std::make_pair(0, new ControlT));
  }
};

struct SkinButton : public SkinControlImpl<SkinButton, CButton>
{
};

struct SkinEdit : public SkinControlImpl<SkinEdit, CEdit>
{
};

struct SkinEdit2 : public SkinControlImpl<SkinEdit2, CWindow>
{
};

template<class ControlT, class BaseT>
hash_map<HWND, shared_ptr<ControlT> >
SkinControlImpl<ControlT,BaseT>::map_;

int main()
{
  SkinButton sb;
  SkinEdit se;
  SkinEdit2 se2;
  sb.Draw();
  se.Draw();
se2.Draw();
  return 0;
}

