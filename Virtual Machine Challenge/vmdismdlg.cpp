#include "vmdism.h"
#include <wx/sizer.h>
#include <wx/filefn.h>
#include <wx/filedlg.h>

vmDismDLG::vmDismDLG(wxWindow *parent, std::vector<uint16_t> mem) : wxDialog(parent, wxID_ANY, "Decompiler")
{
	this->memory = mem;

	SetupWindow();

	btnLoadFile->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(vmDismDLG::btnLoadFileOnButtonClick), NULL, this);
	btnSaveFile->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(vmDismDLG::btnSaveFileOnButtonClick), NULL, this);
	btnDecompile->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(vmDismDLG::btnDecompileOnButtonClick), NULL, this);
	btnCancel->Connect(wxEVT_COMMAND_BUTTON_CLICKED, wxCommandEventHandler(vmDismDLG::btnCancelOnButtonClick), NULL, this);
}

void vmDismDLG::SetupWindow()
{
	this->SetSizeHints(wxDefaultSize, wxDefaultSize);

	wxBoxSizer *bSizer2;
	bSizer2 = new wxBoxSizer(wxVERTICAL);

	wxStaticText *m_staticText1;
	m_staticText1 = new wxStaticText(this, wxID_ANY, wxT("Decompile from"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText1->Wrap(-1);
	bSizer2->Add(m_staticText1, 0, wxALL, 5);

	radioDecompMemory = new wxRadioButton(this, wxID_ANY, wxT("Decompile from memory"), wxDefaultPosition, wxDefaultSize, 0);
	radioDecompMemory->SetValue(true);
	bSizer2->Add(radioDecompMemory, 0, wxALL, 5);

	radioDecompFile = new wxRadioButton(this, wxID_ANY, wxT("Decompile from file"), wxDefaultPosition, wxDefaultSize, 0);
	bSizer2->Add(radioDecompFile, 0, wxALL, 5);

	wxBoxSizer *bSizer3;
	bSizer3 = new wxBoxSizer(wxHORIZONTAL);

	txtctrlFileLoadFrom = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	bSizer3->Add(txtctrlFileLoadFrom, 0, wxALL, 5);

	btnLoadFile = new wxButton(this, wxID_ANY, wxT("load file"), wxDefaultPosition, wxDefaultSize, 0);
	bSizer3->Add(btnLoadFile, 0, wxALL, 5);


	bSizer2->Add(bSizer3, 1, wxEXPAND, 5);

	m_staticText2 = new wxStaticText(this, wxID_ANY, wxT("Save ASM to"), wxDefaultPosition, wxDefaultSize, 0);
	m_staticText2->Wrap(-1);
	bSizer2->Add(m_staticText2, 0, wxALL, 5);

	wxBoxSizer *bSizer4;
	bSizer4 = new wxBoxSizer(wxHORIZONTAL);

	txtctrlSaveFileTo = new wxTextCtrl(this, wxID_ANY, wxEmptyString, wxDefaultPosition, wxDefaultSize, 0);
	bSizer4->Add(txtctrlSaveFileTo, 0, wxALL, 5);

	btnSaveFile = new wxButton(this, wxID_ANY, wxT("Save File"), wxDefaultPosition, wxDefaultSize, 0);
	bSizer4->Add(btnSaveFile, 0, wxALL, 5);


	bSizer2->Add(bSizer4, 1, wxEXPAND, 5);

	wxBoxSizer *bSizer5;
	bSizer5 = new wxBoxSizer(wxHORIZONTAL);

	btnDecompile = new wxButton(this, wxID_ANY, wxT("Decompile"), wxDefaultPosition, wxDefaultSize, 0);
	bSizer5->Add(btnDecompile, 0, wxALL, 5);

	btnCancel = new wxButton(this, wxID_ANY, wxT("Cancel"), wxDefaultPosition, wxDefaultSize, 0);
	bSizer5->Add(btnCancel, 0, wxALL, 5);

	bSizer2->Add(bSizer5, 1, wxEXPAND, 5);


	this->SetSizer(bSizer2);
	this->Layout();
	bSizer2->Fit(this);

	this->Centre(wxBOTH);
}

void vmDismDLG::btnLoadFileOnButtonClick(wxCommandEvent &evt)
{
	wxFileDialog filedlg(this, wxT("Open Bin File"), "", "",
						 wxT("BIN files (*.bin)|*.bin"), wxFD_OPEN | wxFD_FILE_MUST_EXIST);


	if(filedlg.ShowModal() != wxID_OK) return;

	txtctrlFileLoadFrom->Clear();
	txtctrlFileLoadFrom->AppendText(filedlg.GetPath());
}

void vmDismDLG::btnSaveFileOnButtonClick(wxCommandEvent &evt)
{
	wxFileDialog filedlg(this, wxT("Save ASM file"), "", "",
						 wxT("ASM file (*.asm)|*.asm"), wxFD_SAVE | wxFD_OVERWRITE_PROMPT);

	if(filedlg.ShowModal() != wxID_OK) return;

	txtctrlSaveFileTo->Clear();
	txtctrlSaveFileTo->AppendText(filedlg.GetPath());
}

void vmDismDLG::btnDecompileOnButtonClick(wxCommandEvent &evt)
{
	vmDisassembler vmdism;

	if(radioDecompFile->GetValue() && radioDecompMemory->GetValue()) {
		wxMessageBox(wxT("Error both radio buttons are selected. This should not be posible"), wxT("error"), wxICON_ERROR);
		return;
	}

	if(txtctrlSaveFileTo->GetValue() == wxEmptyString) {
		wxMessageBox(wxT("No file to save assembly file to.\n Please provide a path to save the assembly file to."),
					 wxT("No Assembly File Set"), wxICON_ERROR);
		return;
	}

	if(radioDecompFile->GetValue()) {
		if(txtctrlFileLoadFrom->GetValue() == wxEmptyString)
			return;
		vmdism.disassemble(txtctrlFileLoadFrom->GetValue().c_str(), txtctrlSaveFileTo->GetValue().c_str());
	}
	else if(radioDecompMemory->GetValue()) {
		if(this->memory.size() <= 1) {
			wxMessageBox(wxT("No active file loaded into memory"));
			return;
		}
		vmdism.disassemble(this->memory, txtctrlSaveFileTo->GetValue().c_str());
	}

}

void vmDismDLG::btnCancelOnButtonClick(wxCommandEvent &evt)
{
	this->Close();
}