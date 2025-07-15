#include "DebugRegisters.h"
DebugRegisters::DebugRegisters(vm *machine, wxWindow *parent, wxWindowID id, const wxString &title, const wxPoint &pos,
							   const wxSize &size, long style) : wxDialog(parent, id, title, pos, size, style)
{
	this->CreateDialog();
	VMachine = machine;
	debugstack = new DebugStack(this->VMachine, this);
	debugcallstack = new DebugCallStack(this->VMachine, this);

	registerR0->SetValue(wxString::Format(wxT("0x%04lx"), VMachine->getregister(registers::R_R0)));
	registerR1->SetValue(wxString::Format(wxT("0x%04lx"), VMachine->getregister(registers::R_R1)));
	registerR2->SetValue(wxString::Format(wxT("0x%04lx"), VMachine->getregister(registers::R_R2)));
	registerR3->SetValue(wxString::Format(wxT("0x%04lx"), VMachine->getregister(registers::R_R3)));
	registerR4->SetValue(wxString::Format(wxT("0x%04lx"), VMachine->getregister(registers::R_R4)));
	registerR5->SetValue(wxString::Format(wxT("0x%04lx"), VMachine->getregister(registers::R_R5)));
	registerR6->SetValue(wxString::Format(wxT("0x%04lx"), VMachine->getregister(registers::R_R6)));
	registerR7->SetValue(wxString::Format(wxT("0x%04lx"), VMachine->getregister(registers::R_R7)));
	StackPointer->SetValue(wxString::Format(wxT("0x%08lx"), VMachine->GetStackPointer()));
	ProgramCounter->SetValue(wxString::Format(wxT("0x%04lx"), VMachine->GetProgramCounter()));
	breakatpc->SetValue(wxString::Format(wxT("0x%04lx"), VMachine->breakatpc));

	// Connect Events
	exitButton->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(DebugRegisters::ExitButtonOnButtonClick), nullptr, this);

	viewstack->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(DebugRegisters::ViewStackButtonClick), nullptr, this);

	viewCallStack->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(DebugRegisters::ViewCallStackButtonClick), nullptr, this);
}

void DebugRegisters::CreateDialog()
{
	this->SetSizeHints(wxDefaultSize, wxDefaultSize);


	gSizer1 = new wxGridSizer(0, 2, 0, 0);

	txt_registerR0 = new wxStaticText(this, wxID_ANY, wxT("Register R0"), wxDefaultPosition, wxDefaultSize, 0);
	txt_registerR0->Wrap(-1);
	gSizer1->Add(txt_registerR0, 0, wxALL, 5);

	registerR0 = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	gSizer1->Add(registerR0, 0, wxALL, 5);

	txt_registerR1 = new wxStaticText(this, wxID_ANY, wxT("Register R1"), wxDefaultPosition, wxDefaultSize, 0);
	txt_registerR1->Wrap(-1);
	gSizer1->Add(txt_registerR1, 0, wxALL, 5);

	registerR1 = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	gSizer1->Add(registerR1, 0, wxALL, 5);

	txt_registerR2 = new wxStaticText(this, wxID_ANY, wxT("Register R2"), wxDefaultPosition, wxDefaultSize, 0);
	txt_registerR2->Wrap(-1);
	gSizer1->Add(txt_registerR2, 0, wxALL, 5);

	registerR2 = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	gSizer1->Add(registerR2, 0, wxALL, 5);

	txt_registerR3 = new wxStaticText(this, wxID_ANY, wxT("Register R3"), wxDefaultPosition, wxDefaultSize, 0);
	txt_registerR3->Wrap(-1);
	gSizer1->Add(txt_registerR3, 0, wxALL, 5);

	registerR3 = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	gSizer1->Add(registerR3, 0, wxALL, 5);

	txt_registerR4 = new wxStaticText(this, wxID_ANY, wxT("Register R4"), wxDefaultPosition, wxDefaultSize, 0);
	txt_registerR4->Wrap(-1);
	gSizer1->Add(txt_registerR4, 0, wxALL, 5);

	registerR4 = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	gSizer1->Add(registerR4, 0, wxALL, 5);

	txt_registerR5 = new wxStaticText(this, wxID_ANY, wxT("Register R5"), wxDefaultPosition, wxDefaultSize, 0);
	txt_registerR5->Wrap(-1);
	gSizer1->Add(txt_registerR5, 0, wxALL, 5);

	registerR5 = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	gSizer1->Add(registerR5, 0, wxALL, 5);

	txt_registerR6 = new wxStaticText(this, wxID_ANY, wxT("Register R6"), wxDefaultPosition, wxDefaultSize, 0);
	txt_registerR6->Wrap(-1);
	gSizer1->Add(txt_registerR6, 0, wxALL, 5);

	registerR6 = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	gSizer1->Add(registerR6, 0, wxALL, 5);

	txt_registerR7 = new wxStaticText(this, wxID_ANY, wxT("Register R7"), wxDefaultPosition, wxDefaultSize, 0);
	txt_registerR7->Wrap(-1);
	gSizer1->Add(txt_registerR7, 0, wxALL, 5);

	registerR7 = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	gSizer1->Add(registerR7, 0, wxALL, 5);

	txt_programcounter = new wxStaticText(this, wxID_ANY, wxT("Program Counter"), wxDefaultPosition, wxDefaultSize, 0);
	txt_programcounter->Wrap(-1);
	gSizer1->Add(txt_programcounter, 0, wxALL, 5);

	ProgramCounter = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	gSizer1->Add(ProgramCounter, 0, wxALL, 5);

	txt_stackpointer = new wxStaticText(this, wxID_ANY, wxT("Stack Pointer"), wxDefaultPosition, wxDefaultSize, 0);
	txt_stackpointer->Wrap(-1);
	gSizer1->Add(txt_stackpointer, 0, wxALL, 5);

	StackPointer = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	gSizer1->Add(StackPointer, 0, wxALL, 5);

	txt_breakatpc = new wxStaticText(this, wxID_ANY, wxT("Break when program counter is:"), wxDefaultPosition, wxDefaultSize, 0);
	txt_breakatpc->Wrap(-1);
	gSizer1->Add(txt_breakatpc, 0, wxALL, 5);

	breakatpc = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	gSizer1->Add(breakatpc, 0, wxALL, 5);

	exitButton = new wxButton(this, wxID_ANY, wxT("Exit"), wxDefaultPosition, wxDefaultSize, 0);
	gSizer1->Add(exitButton, 0, wxALL, 5);

	viewstack = new wxButton(this, wxID_ANY, wxT("View Stack"), wxDefaultPosition, wxDefaultSize, 0);
	gSizer1->Add(viewstack, 0, wxALL, 5);

	viewCallStack = new wxButton(this, wxID_ANY, wxT("View Call Stack"), wxDefaultPosition, wxDefaultSize, 0);
	gSizer1->Add(viewCallStack, 0, wxALL, 5);

	this->SetSizer(gSizer1);
	this->Layout();

	this->Centre(wxBOTH);
}

void DebugRegisters::ExitButtonOnButtonClick(wxCommandEvent &evt)
{
	int value;
	unsigned int value2;
	if(StackPointer->GetValue().ToInt(&value, 16)) {
		VMachine->sp = value;
	}
	if(ProgramCounter->GetValue().ToUInt(&value2, 16)) {
		VMachine->pc = value2;
	}
	if(breakatpc->GetValue().ToUInt(&value2, 16)) {
		VMachine->breakatpc = value2;
	}

	if(this->registerR0->GetValue().ToUInt(&value2, 16)) {
		VMachine->cpuregisters[0] = value2;
	}
	if(this->registerR1->GetValue().ToUInt(&value2, 16)) {
		VMachine->cpuregisters[1] = value2;
	}
	if(this->registerR2->GetValue().ToUInt(&value2, 16)) {
		VMachine->cpuregisters[2] = value2;
	}
	if(this->registerR3->GetValue().ToUInt(&value2, 16)) {
		VMachine->cpuregisters[3] = value2;
	}
	if(this->registerR4->GetValue().ToUInt(&value2, 16)) {
		VMachine->cpuregisters[4] = value2;
	}
	if(this->registerR5->GetValue().ToUInt(&value2, 16)) {
		VMachine->cpuregisters[5] = value2;
	}
	if(this->registerR6->GetValue().ToUInt(&value2, 16)) {
		VMachine->cpuregisters[6] = value2;
	}
	if(this->registerR7->GetValue().ToUInt(&value2, 16)) {
		VMachine->cpuregisters[7] = value2;
	}

	this->Close();
	evt.Skip();
}

void DebugRegisters::ViewStackButtonClick(wxCommandEvent &evt)
{
	debugstack->UpdateStack();
	debugstack->Show();
	evt.Skip();
}

void DebugRegisters::ViewCallStackButtonClick(wxCommandEvent &evt)
{
	debugcallstack->UpdateStack();
	debugcallstack->Show();
	evt.Skip();
}