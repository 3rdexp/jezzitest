.LOG
16:13 2006-7-10

           
14:01 2006-8-13

        CWindow
          /\
          |
          |
     CButtonT<CWindow>
          /\
          |
          |
SkinControlImpl<SkinButton, CButton>  [m] ISkinScheme
          /\
          |
     SkinButton

--------------------------------------------------------------------------


     CWindow
        /\
        |
     SkinFrameImpl

        /\
        |
     SkinFrame
        /\
        |
SkinControlImpl<UserControl, SkinFrameImpl<>, InstallPolicy = ClassPolicy>
        /\
        |
	UserControl=SkinFrame

--------------------------------------------------------------------------

    SkinFrameImpl
        /\
        |
   SkinDialogImpl
        /\
        |
SkinControlImpl<UserControl, SkinDialogImpl, InstallPolicy = ClassPolicy>
        /\
        |
	UserControl=SkinDialog

/------------------------------------------------------------------------------\
SkinControlImpl<ControlT, BaseT> : BaseT
  Draw(HDC, class/part/state/prop, dest)

SkinFrameImpl<ControlT, WindowT> : WindowT
  DrawCaption
  DrawSystemButton
  
SkinButton : SkinControlImpl<Button, CButton>
UserFrame : SkinControlImpl<UserFrame, SkinFrameImpl<UserFrame, CWindow> >
SkinDialog : SkinControlImpl<SkinDialog, SkinFrameImpl<SkinDialog, CDialog> >
\------------------------------------------------------------------------------/  
  
  

/------------------------------------------------------------------------------\
    class + part + *[state] [prop] ==> area | color | font

scheme_data
  get([in] class/part/state/prop, [out] & area/color/font)
  put([in] class/part/state/prop, [in] & area/color/font)
  [m] class + part + *[state] [prop] ==> area | color | font
  
scheme
  draw(hdc, class/part/state, dest)
  [m] scheme_data*
  [m] cache_dc*

control
  [m] scheme*

schemes_holder
  scheme* get_default()
  scheme* get()
  [m] string ==> scheme_data

button --> control
frame  --> control
\------------------------------------------------------------------------------/

/------------------------------------------------------------------------------\
BEGIN_TM_CLASS_PARTS(BUTTON)
	TM_PART(1, BP, PUSHBUTTON)
	TM_PART(2, BP, RADIOBUTTON)
	TM_PART(3, BP, CHECKBOX)
	TM_PART(4, BP, GROUPBOX)
	TM_PART(5, BP, USERBUTTON)
END_TM_CLASS_PARTS()

BEGIN_TM_PART_STATES(PUSHBUTTON)
	TM_STATE(1, PBS, NORMAL)
	TM_STATE(2, PBS, HOT)
	TM_STATE(3, PBS, PRESSED)
	TM_STATE(4, PBS, DISABLED)
	TM_STATE(5, PBS, DEFAULTED)
END_TM_PART_STATES()

enum BUTTONPARTS
{
  BUTTONPartFiller0,
  BP_PUSHBUTTON = 1
  ...
};

enum PUSHBUTTONSTATES 
{ 
  PUSHBUTTONStateFiller0,
  PBS_NORMAL = 1,
  PBS_HOT = 2,
  PBS_PRESSED = 3,
};
\------------------------------------------------------------------------------/