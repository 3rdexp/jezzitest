#pragma once

// --------------------------------------------------------------------------
// Generic Message
// all skin control support
// --------------------------------------------------------------------------


// WPARAM: TRUE for enable, FALSE disable
#define WMS_ENABLE          WM_APP + 100




// WPARAM: string value, style name in theme file, NULL reset to system default
#define WMS_SETSTYLE		WM_APP + 120

// WPARAM: ISkinDraw * interface, NULL reset to system default
#define WMS_SETDRAW			WM_APP + 121

// --------------------------------------------------------------------------
// Generic Message End
// --------------------------------------------------------------------------


// --------------------------------------------------------------------------
#define WMS_SETITEMNAME		WM_APP + 122 //add name to this control; 
#define WMS_SETITEMCOLOR	WM_APP + 123 //add color to this control;

#define WMS_SETICON			WM_APP + 124 //set icon; 
#define WMS_SETHBITMAP		WM_APP + 125 //set hbitmap name; 
#define WMS_SETIMAGENAME	WM_APP + 126 //
