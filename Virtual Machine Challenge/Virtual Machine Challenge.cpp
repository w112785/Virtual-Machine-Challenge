// Virtual Machine Challenge.cpp : Defines the entry point for the application.
//

#include "Virtual Machine Challenge.h"
#include "vmMainWnd.h"
#pragma warning(disable: 28251)  // Disable Inconsistent annotation for Main Function

wxIMPLEMENT_APP(vmApp);

bool vmApp::OnInit()
{
	mainwnd = new vmMainWnd();
	mainwnd->Show(true);
	SetTopWindow(mainwnd);
	return true;
}