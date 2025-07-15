#pragma once
#pragma warning(disable: 6011)   // Disable Dereferencing NULL warning
#pragma warning(disable: 26812)  // Disable Prefer Enum Class warning.
#include "resource.h"
#include "vmMainWnd.h"
#include <wx/app.h>

class vmApp : public wxApp
{
private:
	vmMainWnd *mainwnd = nullptr;

public:
	vmApp() {}
	~vmApp() {}

	virtual bool OnInit() override;
};

wxDECLARE_APP(vmApp);