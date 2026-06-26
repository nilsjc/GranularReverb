#include "wx/wx.h"
#include "wx/slider.h"
#include <wx/event.h>
#include "wx/button.h"
#include <iostream>
#include "SynthManager.h"
#include "AudioStream.h"
class MyFrame : public wxFrame
{
public:
    MyFrame(const wxString &title, const wxPoint &pos, const wxSize &size);
    void StartAudio();
    void StopAudio();


private:
    SynthManager::Manager manager;
    AudioStream::Runner runner;
    void OnSlChanged(wxCommandEvent &event);
    void StartStopAudio(wxCommandEvent &event);
    bool running = false;
    wxStaticText *label10 = new wxStaticText(this, 20010, "50");
    wxSlider* sliders[10]={
        new wxSlider(this,10001,50,1,100,wxDefaultPosition, wxDefaultSize, wxSL_VERTICAL, wxDefaultValidator),
        new wxSlider(this,10002,50,1,100,wxDefaultPosition, wxDefaultSize, wxSL_VERTICAL, wxDefaultValidator),
        new wxSlider(this,10003,50,1,100,wxDefaultPosition, wxDefaultSize, wxSL_VERTICAL, wxDefaultValidator),
        new wxSlider(this,10004,50,1,100,wxDefaultPosition, wxDefaultSize, wxSL_VERTICAL, wxDefaultValidator),
        new wxSlider(this,10005,50,1,100,wxDefaultPosition, wxDefaultSize, wxSL_VERTICAL, wxDefaultValidator),
        new wxSlider(this,10006,50,1,100,wxDefaultPosition, wxDefaultSize, wxSL_VERTICAL, wxDefaultValidator),
        new wxSlider(this,10007,50,1,100,wxDefaultPosition, wxDefaultSize, wxSL_VERTICAL, wxDefaultValidator),
        new wxSlider(this,10008,50,1,100,wxDefaultPosition, wxDefaultSize, wxSL_VERTICAL, wxDefaultValidator),
        new wxSlider(this,10009,50,1,100,wxDefaultPosition, wxDefaultSize, wxSL_VERTICAL, wxDefaultValidator),
        new wxSlider(this,10010,50,1,100,wxDefaultPosition, wxDefaultSize, wxSL_VERTICAL, wxDefaultValidator)
    };
    wxButton* startStopButton = new wxButton(this, 30001, "start audio");
    wxButton* modulationButton = new wxButton(this, 30002, "mod wave");
};

MyFrame::MyFrame(const wxString &title, const wxPoint &pos, const wxSize &size) : wxFrame(NULL, wxID_ANY, title, pos, size)
{
    // Related to precalculations in audio engine
    manager.Init();

    // Wxwidgets related stuff
    wxGridSizer *grid = new wxGridSizer(3, 10, 0, 0);
    // bind sliders to function
    for(int x=0; x <10; x++)
    {
        sliders[x]->Bind(wxEVT_SLIDER, &MyFrame::OnSlChanged, this);
    }
    
    // add sliders to grid
    for(int x=0; x < 10; x++)
    {
        grid->Add(sliders[x],1, wxEXPAND | wxALL);
    }

    //bind buttons
    startStopButton->Bind(wxEVT_BUTTON, &MyFrame::StartStopAudio, this);

    grid->Add(new wxStaticText(this, 20001, "size"));
    grid->Add(new wxStaticText(this, 20002, "decay"));
    grid->Add(new wxStaticText(this, 20003, "damp"));
    grid->Add(new wxStaticText(this, 20004, "diffsn"));
    grid->Add(new wxStaticText(this, 20005, "LFO F"));
    grid->Add(new wxStaticText(this, 20006, "LFO D"));
    grid->Add(new wxStaticText(this, 20007, "grain R"));
    grid->Add(new wxStaticText(this, 20009, "grain D"));
    grid->Add(new wxStaticText(this, 20010, "tilt"));
    grid->Add(new wxStaticText(this, 20011, "mix"));

    grid->Add(new wxStaticText(this, 20008, "Value:"));
    grid->Add(label10, 1, wxEXPAND | wxALL);
    grid->Add(startStopButton);
    // grid->Add();
    // grid->Add(modulationButton);

    this->SetSizer(grid);
    grid->Layout();
}

void MyFrame::StartAudio()
{
    // start reverb engine
    runner.SetManager(&manager);
    //runner.Open();
}
void MyFrame::StartStopAudio(wxCommandEvent &event)
{
    if(running)
    {
        runner.Terminate();
        running = false;
        startStopButton->SetLabel("start audio");
    }else{
        runner.Open();
        running = true;
        startStopButton->SetLabel("stop audio");
    }
}
void MyFrame::OnSlChanged(wxCommandEvent &event)
{
    int slider = event.GetId() - 10000;
    int value = sliders[slider]->GetValue();
    switch (slider)
    {
        case 1: // size
        {
            if(value==100)value=99;
            float timev = value/50.0;
            manager.SetSize(timev);
            //label10->SetLabel(std::to_string(timev));
            label10->SetLabel("size");
        }
            break;
        case 2: // decay
        {
            float dampv = value/100.0;
            manager.SetDecay(dampv);
            //label10->SetLabel(std::to_string(dampv));
            label10->SetLabel("decay");
        }
            break;
        case 3: // damp
        {
            float dampv = value/100.0;
            manager.SetDamp(dampv);
            //label10->SetLabel(std::to_string(dampv));
            label10->SetLabel("damp");
        }
            break;
        case 4: // diffusion
        {
            float diffv = value/120.0;
            manager.SetDiffusion(diffv);
            //label10->SetLabel(std::to_string(diffv));
            label10->SetLabel("diffusion");
        }
            break;
        case 5: // lfo rate
        {
            float lfofreq = value;
            manager.SetLFO1(lfofreq);
            //label10->SetLabel(std::to_string(lfofreq));
            label10->SetLabel("lforate");
        }
            break;
        case 6: // lfo depth
        {
            float modAmp = value/4.0;
            manager.setMod(modAmp);
            //label10->SetLabel(std::to_string(modAmp));
            label10->SetLabel("lfodepth");
        }
            break;
        case 7: // grain rate
        {
            float grainDepth = value * 200;
            manager.setLFO2(grainDepth);
            //label10->SetLabel(std::to_string(grainDepth));
            label10->SetLabel("grainrate");
        }
            break;
        case 8: // grain depth
        {
            float grainDepth = value/4.0;
            manager.setGrainDepth(grainDepth);
            //label10->SetLabel(std::to_string(grainDepth));
            label10->SetLabel("graindep");
        }
            break;
        case 9: // tilt
        {
            float tiltV = (value/50.0)-1.0;
            manager.setTilt(tiltV);
            //label10->SetLabel(std::to_string(tiltV));
            label10->SetLabel("tilt");
        }
            break;
        case 10: // mix
        {
            float mix = value/100.0;
            manager.setMix(mix);
            //label10->SetLabel(std::to_string(mix));
            label10->SetLabel("mix");
        }

        default:
            break;
    }
    event.Skip();
}


class MyApp : public wxApp
{
public:
    virtual bool OnInit();
};
bool MyApp::OnInit()
{
    MyFrame *frame = new MyFrame("Experimental audio...", wxDefaultPosition, wxDefaultSize);
    frame->StartAudio();
    frame->Show(true);
    return true;
}

wxIMPLEMENT_APP(MyApp);
