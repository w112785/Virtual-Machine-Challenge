#pragma once
#include <wx/dialog.h>
#include <wx/frame.h>
#include <wx/textctrl.h>
#include <wx/stattext.h>
#include <wx/sizer.h>
#include <wx/button.h>
#include "vm.h"

class DebugRegisters : public wxDialog
{
private:
	class DebugStack : public wxDialog
	{
	public:
		DebugStack(vm *vm, wxWindow *parent, wxString title = wxT("View Stack")) : wxDialog(parent, -1, title, wxDefaultPosition, wxSize(800, 600))
		{
			display = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize,
									 wxTE_MULTILINE | wxTE_READONLY | wxTE_RICH | wxTE_RICH2 | wxTE_WORDWRAP);
			vmm = vm;
		}

		virtual void UpdateStack()
		{
			display->Clear();
			if(vmm->sp == -1) {
				display->AppendText("stack empty");
				return;
			}
			for(uint16_t stackitem : vmm->stack) {
				display->AppendText(wxString::Format(wxT(" 0x%04lx "), stackitem));
				if(stackitem == 0xffff)
					break;
			}
		}
		vm *vmm;
		wxTextCtrl *display;
	};

	class DebugCallStack : public DebugStack
	{
	public:
		DebugCallStack(vm *vm, wxWindow *parent, wxString title = wxT("View Call Stack")) : DebugStack(vm, parent, title) {}
		void UpdateStack() override
		{
			display->Clear();
			if(vmm->callstack.empty()) {
				display->AppendText("call stack empty");
				return;
			}
			for(uint16_t call : vmm->callstack) {
				display->AppendText(wxString::Format(wxT(" 0x%04lx "), call));
			}

		}
	};
private:
	vm *VMachine;
protected:
	wxGridSizer *gSizer1;
	wxStaticText *txt_registerR0;
	wxTextCtrl *registerR0;
	wxStaticText *txt_registerR1;
	wxTextCtrl *registerR1;
	wxStaticText *txt_registerR2;
	wxTextCtrl *registerR2;
	wxStaticText *txt_registerR3;
	wxTextCtrl *registerR3;
	wxStaticText *txt_registerR4;
	wxTextCtrl *registerR4;
	wxStaticText *txt_registerR5;
	wxTextCtrl *registerR5;
	wxStaticText *txt_registerR6;
	wxTextCtrl *registerR6;
	wxStaticText *txt_registerR7;
	wxTextCtrl *registerR7;
	wxStaticText *txt_programcounter;
	wxTextCtrl *ProgramCounter;
	wxStaticText *txt_stackpointer;
	wxTextCtrl *StackPointer;
	wxStaticText *txt_breakatpc;
	wxTextCtrl *breakatpc;
	wxButton *exitButton;
	wxButton *viewstack;
	wxButton *viewCallStack;

	DebugStack *debugstack;
	DebugCallStack *debugcallstack;

	void ExitButtonOnButtonClick(wxCommandEvent &evt);
	void ViewStackButtonClick(wxCommandEvent &evt);
	void ViewCallStackButtonClick(wxCommandEvent &evt);

	void CreateDialog();
public:

	DebugRegisters(vm *maching, wxWindow *parent, wxWindowID id = wxID_ANY, const wxString &title = wxT("Debug System"),
				   const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxSize(500, 500),
				   long style = wxDEFAULT_DIALOG_STYLE);
};