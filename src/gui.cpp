#include "gui.h"
#include <ctime>
#include <fstream>
#include <iostream>

bool MyApp::OnInit() {
	MyFrame *frame = new MyFrame("CRD", wxPoint(50, 50), wxSize(800, 730));
	frame->Maximize(true);
	frame->Show(true);

	return true;
}

#pragma region MyFrame
MyFrame::MyFrame(const wxString& title, const wxPoint& pos, const wxSize& size)
	: wxFrame(NULL, wxID_ANY, title, pos, size) {
#pragma region MenuBar
	wxMenu *menuFile = new wxMenu;
	menuFile->Append(ID_ONOPENSRC, "&Open SrcImg\tCtrl-O", "Open source image");
	menuFile->Append(ID_ONOPENVFB, "&Open Flow\tCtrl-F", "Open flowField file");
	menuFile->Append(ID_ONOPENETF, "&Open ETF\tCtrl-E", "Open image file");

	menuFile->Append(ID_ONSAVE, "&Save\tCtrl-E", "Save Result");
	menuFile->AppendSeparator();

	menuFile->AppendSeparator();
	menuFile->Append(wxID_EXIT);

	wxMenu *menuTool = new wxMenu;

	menuTool->Append(ID_ONETF2GVF, "&GenGVF", "generate GVF using addB");
	menuTool->AppendSeparator();

	menuTool->AppendSeparator();


	wxMenu *menuHelp = new wxMenu;
	menuHelp->Append(wxID_ABOUT, "&About", "About the System");
	menuHelp->Append(new wxMenuItem(menuHelp, wxID_TOGGLE_LOG, wxString(wxT("&Log\tCtrl-L")), "Show/Hide the Log", wxITEM_CHECK))->Check(true);
	//menuHelp->Append(wxID_TOGGLE_LOG, "&Toggle Log\tCtrl-L", "Show/Hide the Log");

	wxMenuBar *menuBar = new wxMenuBar;
	menuBar->Append(menuFile, "&File");
	menuBar->Append(menuTool, "&Tool");
	menuBar->Append(menuHelp, "&Help");
	SetMenuBar(menuBar);
	CreateStatusBar(2);
	SetStatusText("SrcImg: None", 0);
	SetStatusText("flowField: None", 1);

#pragma endregion

#pragma region ToolBar: Buttons(Start, Fill Ink, Clean), Combobox(processingBox)
	wxToolBar *toolbar1 = new wxToolBar(this, wxID_ANY);
	start = new wxButton(toolbar1, BUTTON_Start, _T("Start"), wxDefaultPosition, wxDefaultSize, 0);

	clean = new wxButton(toolbar1, BUTTON_Clean, _T("Clean"), wxDefaultPosition, wxDefaultSize, 0);


	processingBox = new wxComboBox(toolbar1, COMBOBOX_Processing, "originalImg", wxDefaultPosition, wxDefaultSize, 0);
	//processingBox = new wxComboBox(toolbar1, COMBOBOX_Processing, "ETF", wxDefaultPosition, wxDefaultSize, 0);

	processingBox->Append("originalImg");
	processingBox->Append("ETF");




	toolbar1->AddControl(start);
	toolbar1->AddControl(clean);
	toolbar1->AddControl(processingBox);


	toolbar1->Realize();
	SetToolBar(toolbar1);
#pragma endregion 

	//Sizer of whole window
	wxBoxSizer* sizer = new wxBoxSizer(wxHORIZONTAL);
	//Sizer of leftside
	wxBoxSizer* leftside = new wxBoxSizer(wxVERTICAL);
	//Sizer of rightside(control panel)
	wxBoxSizer* rightside = new wxBoxSizer(wxVERTICAL);

	wxPanel* controlpanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	controlpanel->SetMaxSize(wxSize(250, -1));
	controlpanel->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_APPWORKSPACE));

	wxString s;

#pragma region Leftside: drawPane, log
	//drawing panel
	wxPanel *drawpanel = new wxPanel(this, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL);
	drawpanel->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_GRAYTEXT));

	wxBoxSizer *dps = new wxBoxSizer(wxHORIZONTAL);
	dp = new wxPanel(drawpanel, wxID_ANY, wxDefaultPosition, wxSize(256, 256), wxTAB_TRAVERSAL);
	dp->SetBackgroundColour(wxSystemSettings::GetColour(wxSYS_COLOUR_BTNTEXT));
	dp->SetSizer(dps);


	drawPane = new BasicDrawPane(dp, Size(256, 256), true);
	dps->Add(drawPane, 1, wxEXPAND);

	// wxTextCtrl: http://docs.wxwidgets.org/trunk/classwx_text_ctrl.html
	log = new wxTextCtrl(drawpanel, ID_WXEDIT1, wxT(""), wxPoint(91, 43), wxSize(121, 21), wxTE_RICH2 | wxTE_MULTILINE | wxTE_READONLY, wxDefaultValidator, wxT("WxEdit1"));
	addlog("Hello CLD!", wxColour(*wxBLACK));

	leftside->AddStretchSpacer(3);
	leftside->Add(dp, 0, wxCENTER);
	leftside->AddStretchSpacer(3);
	leftside->Add(log, 2, wxEXPAND);
#pragma endregion

#pragma region Paint Parameters
	//wxStaticBox *st_paint = new wxStaticBox(controlpanel, -1, wxT("Paint"), wxDefaultPosition, wxDefaultSize, wxTE_RICH2);
	//wxStaticBoxSizer *st_paint_sizer = new wxStaticBoxSizer(st_paint, wxVERTICAL);

	//s.Printf("Brush Size : %d", drawPane->brushSize);
	//slider_brushSize_t = new wxStaticText(controlpanel, SLIDER_BRUSH_SIZE_T, s, wxDefaultPosition, wxDefaultSize, 0);
	//st_paint_sizer->Add(slider_brushSize_t, 0, wxEXPAND | wxLEFT, 10);
	//slider_brushSize = new wxSlider(controlpanel, SLIDER_BRUSH_SIZE, drawPane->brushSize, 1, 10, wxDefaultPosition, wxDefaultSize, 0);
	//st_paint_sizer->Add(slider_brushSize, 0, wxEXPAND | wxLEFT, 10);

	//s.Printf("addA : %.3f", drawPane->cld.addA);
	//slider_addA_t = new wxStaticText(controlpanel, SLIDER_AddA_T, s, wxDefaultPosition, wxDefaultSize, 0);
	//st_paint_sizer->Add(slider_addA_t, 0, wxEXPAND | wxLEFT, 10);
	//slider_addA = new wxSlider(controlpanel, SLIDER_AddA, int(drawPane->cld.addA * 1000), 0, 1000, wxDefaultPosition, wxDefaultSize, 0);
	//st_paint_sizer->Add(slider_addA, 0, wxEXPAND | wxLEFT, 10);

	//s.Printf("addB : %.3f", drawPane->cld.addB);
	//slider_addB_t = new wxStaticText(controlpanel, SLIDER_AddB_T, s, wxDefaultPosition, wxDefaultSize, 0);
	//st_paint_sizer->Add(slider_addB_t, 0, wxEXPAND | wxLEFT, 10);
	//slider_addB = new wxSlider(controlpanel, SLIDER_AddB, int(drawPane->cld.addB * 1000), 0, 1000, wxDefaultPosition, wxDefaultSize, 0);
	//st_paint_sizer->Add(slider_addB, 0, wxEXPAND | wxLEFT, 10);

	//rightside->Add(st_paint_sizer, 0, wxEXPAND | wxLEFT | wxRIGHT | wxTOP, 3);
#pragma endregion

	//set portion of size: leftside & rightside(control)
	drawpanel->SetSizer(leftside);
	controlpanel->SetSizer(rightside);
	sizer->Add(drawpanel, 7, wxEXPAND);
	sizer->Add(controlpanel, 3, wxEXPAND);
	rightside->Fit(controlpanel);
	leftside->Fit(drawpanel);
	SetSizer(sizer);



	this->GetSizer()->Layout();


	render_loop_on = false;

}
void MyFrame::OnExit(wxCommandEvent& event) {
	Close(true);
}
void MyFrame::OnAbout(wxCommandEvent& event) {
	wxMessageBox("Shu-Hsuan Hsu\n\nComputer Science - National Chengchi University",
		"About CLD",
		wxOK | wxICON_INFORMATION);
}
void MyFrame::OnToggleLog(wxCommandEvent& event) {
	event.IsChecked() ? log->Show() : log->Hide();

	this->GetSizer()->Layout();
}

void MyFrame::OnOpenSrc(wxCommandEvent& event) {
	render_loop_on = false;
	activateRenderLoop(render_loop_on);
	wxFileDialog openFileDialog(this, _("Open image file"), "", "", "image files (*.bmp;*.png;*.jpg)|*.bmp;*.png;*.jpg", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
	if (openFileDialog.ShowModal() == wxID_CANCEL) {
		addlog("Load Img Canceled", wxColour(*wxBLACK));
		return;     // the user changed idea...
	}
	else {
		wxString s;
		s.Printf("Load Img - %s", openFileDialog.GetFilename());
		addlog(s, wxColour(*wxBLUE));

		s.Printf("SrcImg: %s", openFileDialog.GetFilename());
		SetStatusText(s, 0);
	}

	// proceed loading the file chosen by the user, this can be done with e.g. wxWidgets input streams:
	wxFileInputStream input_stream(openFileDialog.GetPath());
	if (!input_stream.IsOk()) {
		wxLogError("Cannot open file '%s'.", openFileDialog.GetPath());
		return;
	}
	drawPane->cld.readSrc((const char*)openFileDialog.GetPath().mb_str());

	wxSize img(drawPane->cld.originalImg.cols, drawPane->cld.originalImg.rows);
	dp->SetMinSize(img);
	this->Layout();


	render_loop_on = true;
	activateRenderLoop(render_loop_on);
}
void MyFrame::OnOpenVfb(wxCommandEvent& event) {
	render_loop_on = false;
	activateRenderLoop(render_loop_on);
	wxFileDialog openFileDialog(this, _("Open vfb file"), "", "", "vfb files (*.vfb)|*.vfb", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
	if (openFileDialog.ShowModal() == wxID_CANCEL) {
		addlog("Load Flow Canceled", wxColour(*wxBLACK));
		return;     // the user changed idea...
	}
	else {
		wxString s;
		s.Printf("Load Flow(.vfb) - %s", openFileDialog.GetFilename());
		addlog(s, wxColour(*wxBLUE));

		s.Printf("flowField(vfb): %s", openFileDialog.GetFilename());
		SetStatusText(s, 1);
	}


	// proceed loading the file chosen by the user;
	// this can be done with e.g. wxWidgets input streams:
	wxFileInputStream input_stream(openFileDialog.GetPath());
	if (!input_stream.IsOk()) {
		wxLogError("Cannot open file '%s'.", openFileDialog.GetPath());
		return;
	}
	//drawPane->cld.ReadFlow((const char*)openFileDialog.GetPath().mb_str());

	render_loop_on = true;
	activateRenderLoop(render_loop_on);
}
void MyFrame::OnOpenETF(wxCommandEvent& event) {
	render_loop_on = false;
	activateRenderLoop(render_loop_on);
	wxFileDialog openFileDialog(this, _("Open image file"), "", "", "image files (*.bmp;*.png;*.jpg)|*.bmp;*.png;*.jpg", wxFD_OPEN | wxFD_FILE_MUST_EXIST);
	if (openFileDialog.ShowModal() == wxID_CANCEL) {
		addlog("Load ETF Canceled", wxColour(*wxBLACK));
		return;     // the user changed idea...
	}
	else {
		wxString s;
		s.Printf("Load ETF - %s", openFileDialog.GetFilename());
		addlog(s, wxColour(*wxBLUE));

		s.Printf("flowField(ETF): %s", openFileDialog.GetFilename());
		SetStatusText(s, 1);
	}


	// proceed loading the file chosen by the user;
	// this can be done with e.g. wxWidgets input streams:
	wxFileInputStream input_stream(openFileDialog.GetPath());
	if (!input_stream.IsOk()) {
		wxLogError("Cannot open file '%s'.", openFileDialog.GetPath());
		return;
	}
	//drawPane->cld.ETF((const char*)openFileDialog.GetPath().mb_str());

	render_loop_on = true;
	activateRenderLoop(render_loop_on);

}

void MyFrame::OnSaveResult(wxCommandEvent& event) {
	wxFileDialog
		saveFileDialog(this, _("Save PNG file"), "", "",
		"PNG files (*.png)|*.png", wxFD_SAVE | wxFD_OVERWRITE_PROMPT);
	if (saveFileDialog.ShowModal() == wxID_CANCEL)
		return;     // the user changed idea...

	// save the current contents in the file;
	// this can be done with e.g. wxWidgets output streams:
	wxFileOutputStream output_stream(saveFileDialog.GetPath());
	if (!output_stream.IsOk()) {
		wxLogError("Cannot save current contents in file '%s'.", saveFileDialog.GetPath());
		return;
	}

	cvtColor(drawPane->dis, drawPane->dis, CV_BGR2RGB);
	imwrite((const char*)saveFileDialog.GetPath().mb_str(), drawPane->dis);


	//	drawPane->cld.SaveRD();
}

void MyFrame::OnGenGVF(wxCommandEvent& event) {
	//drawPane->cld.GVF();
	wxString s;
	s.Printf("flowField: gen_by_addB ");
	SetStatusText(s, 1);

}


//Buttons
void MyFrame::OnStart(wxCommandEvent& event) {
	render_loop_on = !render_loop_on;
	activateRenderLoop(render_loop_on);

	//if (render_loop_on) fill->Enable();
	//else fill->Disable();
}
void MyFrame::OnClean(wxCommandEvent& event) {
	//*drawPane->cld.c_A = Mat::ones(drawPane->cld.Mask.size(), CV_32F);
	//*drawPane->cld.c_B = Mat::zeros(drawPane->cld.Mask.size(), CV_32F);
	//*drawPane->cld.p_A = Mat::ones(drawPane->cld.Mask.size(), CV_32F);
	//*drawPane->cld.p_B = Mat::zeros(drawPane->cld.Mask.size(), CV_32F);
	//drawPane->cld.Addition_A = Mat::zeros(drawPane->cld.Mask.size(), CV_32F);
	//drawPane->cld.Addition_B = Mat::zeros(drawPane->cld.Mask.size(), CV_32F);

	//drawPane->paintNow(true); //execute clean action
	//addlog("Draw Panel Cleaned.", wxColour(*wxBLACK));
}



//Comboboxes
void MyFrame::OnProcessingBox(wxCommandEvent& event) {
	drawPane->processingS = processingBox->GetValue();;
	drawPane->paintNow(true); //execute action
}

//Slides: Pattern Parameter
void MyFrame::OnSliderS(wxCommandEvent& event) {
	//wxString s;
	//if (Segmentation_cb->GetValue() && drawPane->regionSelected != 0)
	//{
	//	drawPane->cld.segmentation[drawPane->regionSelected - 1].size = slider_s->GetValue() / 1000.0;
	//	s.Printf("Size : %.3f", drawPane->cld.segmentation[drawPane->regionSelected - 1].size);
	//	drawPane->cld.UpdateControlMask();
	//}
	//else
	//{
	//	drawPane->cld.s = slider_s->GetValue() / 1000.0;
	//	drawPane->cld.UpdateSizeMask();
	//	s.Printf("Size : %.3f", drawPane->cld.s);
	//}
	//slider_s_t->SetLabel(s);
}
void MyFrame::OnSliderSd(wxCommandEvent& event) {
	//wxString s;
	//if (Segmentation_cb->GetValue() && drawPane->regionSelected != 0)
	//{
	//	drawPane->cld.segmentation[drawPane->regionSelected - 1].sd = slider_sd->GetValue() / 1000.0;
	//	s.Printf("Spacing : %.3f", drawPane->cld.segmentation[drawPane->regionSelected - 1].sd);
	//	drawPane->cld.UpdateControlMask();
	//}
	//else
	//{
	//	drawPane->cld.sd = slider_sd->GetValue() / 1000.0;
	//	//drawPane->cld.UpdateSizeMask();
	//	s.Printf("Spacing : %.3f", drawPane->cld.sd);
	//}
	//slider_sd_t->SetLabel(s);

}
void MyFrame::OnSliderF(wxCommandEvent& event) {
	//wxString s;
	//if (Segmentation_cb->GetValue() && drawPane->regionSelected != 0)
	//{
	//	drawPane->cld.segmentation[drawPane->regionSelected - 1].F = slider_f->GetValue() / 1000.0*0.06;
	//	s.Printf("F : %.4f", drawPane->cld.segmentation[drawPane->regionSelected - 1].F);
	//	drawPane->cld.UpdateControlMask();
	//}
	//else
	//{
	//	drawPane->cld.f = slider_f->GetValue() / 1000.0*0.06;
	//	s.Printf("F : %.4f", drawPane->cld.f);
	//}
	//slider_f_t->SetLabel(s);
}
void MyFrame::OnSliderK(wxCommandEvent& event) {
	//wxString s;
	//if (Segmentation_cb->GetValue() && drawPane->regionSelected != 0)
	//{
	//	drawPane->cld.segmentation[drawPane->regionSelected - 1].k = slider_k->GetValue() / 1000.0*0.04 + 0.03;
	//	s.Printf("k : %.4f", drawPane->cld.segmentation[drawPane->regionSelected - 1].k);
	//	drawPane->cld.UpdateControlMask();
	//}
	//else
	//{
	//	drawPane->cld.k = slider_k->GetValue() / 1000.0*0.04 + 0.03;
	//	s.Printf("k : %.4f", drawPane->cld.k);
	//	drawPane->cld.UpdatekMask();
	//}
	//slider_k_t->SetLabel(s);
}
void MyFrame::OnSliderL(wxCommandEvent& event) {
	//wxString s;
	//if (Segmentation_cb->GetValue() && drawPane->regionSelected != 0)
	//{
	//	drawPane->cld.segmentation[drawPane->regionSelected - 1].l = slider_l->GetValue();
	//	s.Printf("l : %d", drawPane->cld.segmentation[drawPane->regionSelected - 1].l);
	//	drawPane->cld.UpdateControlMask();
	//}
	//else
	//{
	//	drawPane->cld.l = slider_l->GetValue();
	//	s.Printf("l : %d", drawPane->cld.l);
	//}
	//slider_l_t->SetLabel(s);
}



void MyFrame::addlog(wxString info, wxColour& color) {
	time_t currentTime;// for logging current time
	struct tm *localTime;// for logging current time
	time(&currentTime);                   // Get the current time
	localTime = localtime(&currentTime);  // Convert the current time to the local time
	wxString s;
	s.Printf("\n%02d : %02d : %02d  |  %s", localTime->tm_hour, localTime->tm_min, localTime->tm_sec, info);
	log->SetDefaultStyle(wxTextAttr(color));
	log->AppendText(s);

}
void MyFrame::activateRenderLoop(bool on) {
	if (on) {
		start->SetLabel("Stop");
		Connect(wxID_ANY, wxEVT_IDLE, wxIdleEventHandler(MyFrame::onIdle));
		render_loop_on = true;
		addlog("-------Start iteration-------", wxColour(*wxBLACK));
	}
	else {
		start->SetLabel("Start");
		Disconnect(wxEVT_IDLE, wxIdleEventHandler(MyFrame::onIdle));
		//Connect(wxID_ANY, wxEVT_IDLE, wxIdleEventHandler(MyFrame::onIdle));
		render_loop_on = false;
		addlog("-------Stop iteration-------", wxColour(*wxBLACK));
	}
}
void MyFrame::onIdle(wxIdleEvent& evt) {
	drawPane->paintNow(render_loop_on);
	evt.RequestMore(); // render continuously, not only once on idle
}
#pragma endregion 

#pragma region BasicDrawPane
BasicDrawPane::BasicDrawPane(wxPanel* parent, Size s, bool canUndo) :
processing(s),
cld(s),
wxPanel(parent) {
	activateDraw = false;


}




//first frame
void BasicDrawPane::paintEvent(wxPaintEvent& evt) {
	wxPaintDC dc(this);
	//render(dc);
	dis = cld.originalImg.clone();
	dis.convertTo(dis, CV_8UC1, 255);
	//cvtColor(dis, dis, CV_GRAY2RGB);
	wxImage img(dis.cols, dis.rows, dis.data, true);
	wxBitmap bmp(img);
	dc.DrawBitmap(bmp, 0, 0);
}

//render loop
void BasicDrawPane::paintNow(bool render_loop_on) {
	wxClientDC dc(this);
	render(dc, render_loop_on);
}

//Main Render(iteration) Section
void BasicDrawPane::render(wxDC& dc, bool render_loop_on) {
	if (render_loop_on) {
		//if (customAnisotropicFunction){
		//int steps = cld.FastGrayScott(mindegree, maxdegree, false, regionOn);
		//((MyFrame *)GetParent())->SetStatusText(wxString::Format("%i", steps), 0); //preview does not have StatusText
		//}
		//else {
		//	cld.FastGrayScott(0, 0, false, regionOn);
		//}

		////must read 500x500 input image first, and a flow field
		//cld.GrayScottModel();
	}

	dis = cld.originalImg.clone();
	cvtColor(dis, dis, CV_GRAY2BGR);
	//imshow("jik", dis);
	//imshow("jik2", cld.originalImg);

	if (processingS == "ETF") {
		processing.ETF(cld.etf.flowField, dis);
		dis.convertTo(dis, CV_8UC1, 255);
		cvtColor(dis, dis, CV_GRAY2BGR);
	}


	wxImage img(dis.cols, dis.rows, dis.data, true);
	wxBitmap bmp(img);
	dc.DrawBitmap(bmp, 0, 0);
}
#pragma endregion 