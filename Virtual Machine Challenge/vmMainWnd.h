#pragma once
#include <wx/wx.h>
#include <wx/treectrl.h>
#include "vm.h"
#include "vmdism.h"
#include <thread>


class vmMainWnd : public wxFrame
{
private:
	void SetupMainWindow();
	void SetupMenuBar();

	void OnOpen(wxCommandEvent &evt);
	void OnSave(wxCommandEvent &evt);
	void OnExit(wxCommandEvent &evt);
	void OnInputEnter(wxCommandEvent &evt);
	void OnClear(wxCommandEvent &evt);
	void OnDebug(wxCommandEvent &evt);
	void OnSaveState(wxCommandEvent &evt);
	void OnLoadState(wxCommandEvent &evt);
	void OnDecompile(wxCommandEvent &evt);
	void OnMenuReset(wxCommandEvent &evt);

protected:
	enum
	{
		vmWND = wxID_HIGHEST + 1001,
		vmOUTPUT,
		vmINPUT,
		vmREGISTERS,

		// Menu ID's
		vmMainMenu,
		mnuLoadBin,
		mnuSaveBin,
		mnuLoad,
		mnuSave,
		mnuReset,
		mnuExit,
		mnuClear,
		mnuDebug,
		mnuDelay,
		mnuDecompile,
		mnuAssemble
	};

public:
	vmMainWnd();
	virtual ~vmMainWnd();

	void UpdateUI();
	void UpdateOutput(char a);
	void UpdateOutput(wxString text);

private:
	vm *Mashine;
	std::thread *vmthread;
	wxString dir;

protected:
	// MenuBar Stuff
	wxMenuBar *menu;
	// FileMenu Stuff
	wxMenu *FileMenu;
	wxMenuItem *menuLoadBin;
	wxMenuItem *menuLoadState;
	wxMenuItem *menuSaveState;
	wxMenuItem *menuSaveBin;
	wxMenuItem *menuExit;
	wxMenuItem *menuReset;
	// Debug Menu Stuff
	wxMenu *DebugMenu;
	wxMenuItem *menuDecompile;
	wxMenuItem *menuClearOutput;
	wxMenuItem *menuDebug;

	wxBoxSizer *MainSizer;
	wxBoxSizer *CtrlSizer;
	wxTextCtrl *txt_InPut;
	wxTextCtrl *txt_OutPut;

	wxTreeCtrl *txt_Registers;
	wxTreeItemId rootId;
	wxTreeItemId registers_Display[(int)registers::R_R7 + 1];
	wxTreeItemId registers_pc;
	wxTreeItemId registers_sp;
};

