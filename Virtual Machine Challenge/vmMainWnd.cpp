#include "vmMainWnd.h"
#include "Virtual Machine Challenge.h"
#include <iostream>
#include <wx/filefn.h>
#include <wx/utils.h> 
#include "DebugRegisters.h"
#include "vmdism.h"
#include <memory>

using namespace std::chrono_literals;

vmMainWnd::vmMainWnd() : wxFrame(nullptr, vmWND, wxEmptyString, wxDefaultPosition, wxSize(800, 600), wxDEFAULT_FRAME_STYLE | wxTAB_TRAVERSAL), dir(wxGetCwd()), vmthread(nullptr), Mashine(new vm())
{
	this->SetSizeHints(wxDefaultSize, wxDefaultSize);
	this->SetupMenuBar();
	this->SetupMainWindow();

	FileMenu->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(vmMainWnd::OnOpen), this, menuLoadBin->GetId());
	FileMenu->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(vmMainWnd::OnSave), this, menuSaveBin->GetId());
	FileMenu->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(vmMainWnd::OnLoadState), this, menuLoadState->GetId());
	FileMenu->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(vmMainWnd::OnSaveState), this, menuSaveState->GetId());
	FileMenu->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(vmMainWnd::OnMenuReset), this, menuReset->GetId());
	FileMenu->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(vmMainWnd::OnExit), this, menuExit->GetId());

	DebugMenu->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(vmMainWnd::OnClear), this, menuClearOutput->GetId());
	DebugMenu->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(vmMainWnd::OnDebug), this, menuDebug->GetId());
	DebugMenu->Bind(wxEVT_COMMAND_MENU_SELECTED, wxCommandEventHandler(vmMainWnd::OnDecompile), this, menuDecompile->GetId());

	txt_InPut->Bind(wxEVT_COMMAND_TEXT_ENTER, wxCommandEventHandler(vmMainWnd::OnInputEnter), this, txt_InPut->GetId());

	this->SetTitle(wxGetLibraryVersionInfo().ToString());

	this->CreateStatusBar();
	this->Layout();
	this->Centre(wxBOTH);
}

vmMainWnd::~vmMainWnd() {
	Mashine->stop();
	if(vmthread != nullptr)
		if(vmthread->joinable())
			vmthread->join();
	delete Mashine;
}

void vmMainWnd::SetupMainWindow()
{
	MainSizer = new wxBoxSizer(wxVERTICAL);
	CtrlSizer = new wxBoxSizer(wxHORIZONTAL);

	txt_OutPut = new wxTextCtrl(this, vmOUTPUT, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_MULTILINE | wxTE_READONLY | wxTE_RICH | wxTE_RICH2);
	CtrlSizer->Add(txt_OutPut, 4, wxALL | wxEXPAND, 0);

	txt_Registers = new wxTreeCtrl(this, vmREGISTERS, wxDefaultPosition, wxDefaultSize, wxTR_HIDE_ROOT | wxTR_NO_BUTTONS | wxTR_NO_LINES);
	rootId = txt_Registers->AddRoot("Root");
	registers_Display[0] = txt_Registers->AppendItem(rootId, "Register r0 = 0 [0x0000]");
	registers_Display[1] = txt_Registers->AppendItem(rootId, "Register r1 = 0 [0x0000]");
	registers_Display[2] = txt_Registers->AppendItem(rootId, "Register r2 = 0 [0x0000]");
	registers_Display[3] = txt_Registers->AppendItem(rootId, "Register r3 = 0 [0x0000]");
	registers_Display[4] = txt_Registers->AppendItem(rootId, "Register r4 = 0 [0x0000]");
	registers_Display[5] = txt_Registers->AppendItem(rootId, "Register r5 = 0 [0x0000]");
	registers_Display[6] = txt_Registers->AppendItem(rootId, "Register r6 = 0 [0x0000]");
	registers_Display[7] = txt_Registers->AppendItem(rootId, "Register r7 = 0 [0x0000]");
	registers_pc = txt_Registers->AppendItem(rootId, "Program Counter = 0x0000");
	registers_sp = txt_Registers->AppendItem(rootId, "Stack Pointer = 0xffffffff");

	CtrlSizer->Add(txt_Registers, 1, wxALL | wxEXPAND, 0);

	MainSizer->Add(CtrlSizer, 1, wxEXPAND, 0);

	txt_InPut = new wxTextCtrl(this, vmINPUT, wxEmptyString, wxDefaultPosition, wxDefaultSize, wxTE_NOHIDESEL | wxTE_PROCESS_ENTER);
	MainSizer->Add(txt_InPut, 0, wxALL | wxEXPAND, 0);

	this->SetSizer(MainSizer);
}

void vmMainWnd::SetupMenuBar()
{
	menu = new wxMenuBar();
	FileMenu = new wxMenu();
	// FileMenu
	menuLoadBin = new wxMenuItem(FileMenu, mnuLoadBin, wxT("Load Binary\tCTRL+O"), wxT("Load a Binary file"), wxITEM_NORMAL);
	FileMenu->Append(menuLoadBin);
	menuSaveBin = new wxMenuItem(FileMenu, mnuSaveBin, wxT("Save Binary\tCTRL+S"), wxT("Save the memory to Binary file"), wxITEM_NORMAL);
	FileMenu->Append(menuSaveBin);
	FileMenu->AppendSeparator();
	menuLoadState = new wxMenuItem(FileMenu, mnuLoad, wxT("Load State"), wxT("Load Register State"), wxITEM_NORMAL);
	FileMenu->Append(menuLoadState);
	menuSaveState = new wxMenuItem(FileMenu, mnuSave, wxT("Save State"), wxT("Save Register State"), wxITEM_NORMAL);
	FileMenu->Append(menuSaveState);
	FileMenu->AppendSeparator();
	menuReset = new wxMenuItem(FileMenu, mnuReset, wxT("Reset System"), wxT("Resets VM back to start."), wxITEM_NORMAL);
	FileMenu->Append(menuReset);
	menuExit = new wxMenuItem(FileMenu, mnuExit, wxT("&Exit\tALT+F4"), wxT("Exit Program"), wxITEM_NORMAL);
	FileMenu->Append(menuExit);

	menu->Append(FileMenu, wxT("File"));
	// ViewMenu
	DebugMenu = new wxMenu();
	menuClearOutput = new wxMenuItem(DebugMenu, mnuClear, wxT("Clear Output"), wxT("Clear Output Window"), wxITEM_NORMAL);
	DebugMenu->Append(menuClearOutput);
	menuDebug = new wxMenuItem(DebugMenu, mnuDebug, wxT("Debug Registers"), wxT("Edit Registers"), wxITEM_NORMAL);
	DebugMenu->Append(menuDebug);
	DebugMenu->AppendSeparator();
	menuDecompile = new wxMenuItem(DebugMenu, mnuDecompile, wxT("Decompiler"), wxT("Decompile ether Binary file or active program memory to ASM"), wxITEM_NORMAL);
	DebugMenu->Append(menuDecompile);

	menu->Append(DebugMenu, wxT("Debug"));

	this->SetMenuBar(menu);
}

void vmMainWnd::OnSave(wxCommandEvent &evt)
{
	wxFileDialog filedlg(this, wxT("Save Memory to Binary File"), dir, wxEmptyString,
						 wxT("state files (*.bin)|*.bin"), wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
	wxString filename;
	if(filedlg.ShowModal() != wxID_OK) return;

	filename = filedlg.GetPath();
	dir = filedlg.GetDirectory();
	if(!Mashine->savebin(filename.c_str())) {
		return;
	}
}

void vmMainWnd::OnOpen(wxCommandEvent &evt)
{
	wxFileDialog filedlg(this, wxT("Open Bin File"), dir, "",
						 wxT("BIN files (*.bin)|*.bin"), wxFD_OPEN | wxFD_FILE_MUST_EXIST);

	std::wstring filename;
	if(filedlg.ShowModal() != wxID_OK) return;

	if(vmthread != nullptr) {
		if(vmthread->joinable()) {
			Mashine->stop();
			vmthread->join();
		}
	}

	filename = filedlg.GetPath();
	dir = filedlg.GetDirectory();

	Mashine->load(filename.c_str());
	vmthread = new std::thread(&vm::start, Mashine);
	//Mashine->start();
	//Mashine->start(vm *this);
}


void vmMainWnd::OnSaveState(wxCommandEvent &evt)
{
	wxFileDialog filedlg(this, wxT("Save vm State to File"), dir, wxEmptyString,
						 wxT("state files (*.state)|*.state"), wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
	wxString filename;
	if(filedlg.ShowModal() != wxID_OK) return;

	filename = filedlg.GetPath();
	dir = filedlg.GetDirectory();
	Mashine->savestate(filename.c_str());
}

void vmMainWnd::OnLoadState(wxCommandEvent &evt)
{
	wxFileDialog filedlg(this, wxT("Open vm State File"), dir, wxEmptyString,
						 wxT("state files (*.state)|*.state"), wxFD_OPEN | wxFD_FILE_MUST_EXIST);

	wxString filename;
	if(filedlg.ShowModal() != wxID_OK) return;

	filename = filedlg.GetPath();
	dir = filedlg.GetDirectory();

	if(vmthread != nullptr) {
		if(vmthread->joinable()) {
			Mashine->stop();
			vmthread->join();
		}
	}

	if(!Mashine->loadstate(filename.c_str())) {
		return;
	}


	//Mashine->load(filename.c_str());
	vmthread = new std::thread(&vm::start, Mashine);
}

void vmMainWnd::OnMenuReset(wxCommandEvent &evt)
{
	if(vmthread != nullptr) {
		if(vmthread->joinable()) {
			Mashine->stop();
			vmthread->join();
		}
	}
	Mashine->reset();
	this->UpdateUI();
}


void vmMainWnd::OnExit(wxCommandEvent &evt)
{
	Close();
	evt.Skip();
}

void vmMainWnd::OnInputEnter(wxCommandEvent &evt)
{
	txt_InPut->AppendText("\n");
	wxString cmd = txt_InPut->GetValue();
	if(cmd == L"fix teleporter\n" || cmd == L"fix_teleporter\n") {
		if (Mashine->fix_teleporter())
			txt_OutPut->AppendText(_T("\n\n\t ---Teleporter Subroutine Fixed--- \n\n"));
	}
	else if(cmd == L"exit\n") {
		this->Close();
	}
	else if(Mashine->IsWaitingForString()) {
		Mashine->commandstring(cmd);
		txt_OutPut->AppendText(cmd);
	}
	
	txt_InPut->Clear();
}

void vmMainWnd::OnClear(wxCommandEvent &evt)
{
	txt_OutPut->Clear();
}

void vmMainWnd::UpdateUI()
{
	for(int i = 0; i <= (int)registers::R_R7; i++) {
		txt_Registers->SetItemText(registers_Display[i], wxString::Format(wxT("Register r%d = %d [0x%04lx]"), i, Mashine->getregister((registers)i), Mashine->getregister((registers)i)));
	}
	txt_Registers->SetItemText(registers_sp, wxString::Format(wxT("Stack Pointer = 0x%04lx"), Mashine->GetStackPointer()));
	txt_Registers->SetItemText(registers_pc, wxString::Format(wxT("Program Counter = 0x%04lx"), Mashine->GetProgramCounter()));
}

void vmMainWnd::UpdateOutput(char a)
{
	txt_OutPut->AppendText(a);
}

void vmMainWnd::UpdateOutput(wxString text)
{
	txt_OutPut->AppendText(text);
}

void vmMainWnd::OnDebug(wxCommandEvent &evt)
{
	bool wepausedit = false;
	if(this->Mashine->GetState() == state::running) {
		Mashine->pause();
		wepausedit = true;
	}
	DebugRegisters dbgreg(Mashine, this);
	dbgreg.ShowModal();

	UpdateUI();

	if(wepausedit)
		Mashine->resume();
}

void vmMainWnd::OnDecompile(wxCommandEvent &evt)
{
	bool wepausedit = false;
	if(this->Mashine->GetState() == state::running) {
		Mashine->pause();
		wepausedit = true;
	}

	vmDismDLG dissdlg(this, this->Mashine->GetMemory());
	dissdlg.ShowModal();
	if(wepausedit)
		Mashine->resume();
}