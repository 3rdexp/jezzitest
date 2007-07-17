#ifndef __COMHELP_H__
#define __COMHELP_H__

#pragma once

#ifndef HRASSERT
#define HRASSERT( x )	ATLASSERT( SUCCEEDED( (x) ) )
#endif // HRASSERT

#ifndef CheckPointer
#define CheckPointer(p, e_val) if( 0==(p) ) return (e_val);
#endif

#endif //__COMHELP_H__