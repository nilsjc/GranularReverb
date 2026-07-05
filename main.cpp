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
    void PlayStopSampleLoop(wxCommandEvent &event);
    void RecordAudioSample(wxCommandEvent &event);
    int loopSize = 44100 * 1;
    bool running = false;
    bool playSample = false;
    wxStaticText *label10 = new wxStaticText(this, 20010, "50");
    wxSlider* sliders[13]={
        new wxSlider(this,10001,50,1,100,wxDefaultPosition, wxDefaultSize, wxSL_VERTICAL, wxDefaultValidator),
        new wxSlider(this,10002,50,1,100,wxDefaultPosition, wxDefaultSize, wxSL_VERTICAL, wxDefaultValidator),
        new wxSlider(this,10003,50,1,100,wxDefaultPosition, wxDefaultSize, wxSL_VERTICAL, wxDefaultValidator),
        new wxSlider(this,10004,50,1,100,wxDefaultPosition, wxDefaultSize, wxSL_VERTICAL, wxDefaultValidator),
        new wxSlider(this,10005,50,1,100,wxDefaultPosition, wxDefaultSize, wxSL_VERTICAL, wxDefaultValidator),
        new wxSlider(this,10006,50,1,100,wxDefaultPosition, wxDefaultSize, wxSL_VERTICAL, wxDefaultValidator),
        new wxSlider(this,10007,50,1,100,wxDefaultPosition, wxDefaultSize, wxSL_VERTICAL, wxDefaultValidator),
        new wxSlider(this,10008,50,1,100,wxDefaultPosition, wxDefaultSize, wxSL_VERTICAL, wxDefaultValidator),
        new wxSlider(this,10009,50,1,100,wxDefaultPosition, wxDefaultSize, wxSL_VERTICAL, wxDefaultValidator),
        new wxSlider(this,10010,50,1,100,wxDefaultPosition, wxDefaultSize, wxSL_VERTICAL, wxDefaultValidator),
        new wxSlider(this,10011,50,1,100,wxDefaultPosition, wxDefaultSize, wxSL_VERTICAL, wxDefaultValidator),
        new wxSlider(this,10012,50,1,100,wxDefaultPosition, wxDefaultSize, wxSL_VERTICAL, wxDefaultValidator)

    };
    wxButton* startStopButton = new wxButton(this, 30001, "start audio");
    wxButton* modulationButton = new wxButton(this, 30002, "mod wave");
    wxButton* recButton = new wxButton(this, 30003, "loop record");
    wxButton* playButton = new wxButton(this, 30004, "play loop");
    void WriteToLabel(std::string text)
    {
        label10->SetLabel(text);
    }
};

MyFrame::MyFrame(const wxString &title, const wxPoint &pos, const wxSize &size) : wxFrame(NULL, wxID_ANY, title, pos, size)
{
    // Related to precalculations in audio engine
    manager.Init();

    // Wxwidgets related stuff
    wxGridSizer *grid = new wxGridSizer(3, 10, 0, 0);
    // bind sliders to function
    for(int x=0; x <12; x++)
    {
        sliders[x]->Bind(wxEVT_SLIDER, &MyFrame::OnSlChanged, this);
    }
    
    // add reverb sliders to grid
    for(int x=0; x < 10; x++)
    {
        grid->Add(sliders[x],1, wxEXPAND | wxALL);
    }

    //bind buttons
    startStopButton->Bind(wxEVT_BUTTON, &MyFrame::StartStopAudio, this);
    recButton->Bind(wxEVT_BUTTON, &MyFrame::RecordAudioSample, this);
    playButton->Bind(wxEVT_BUTTON, &MyFrame::PlayStopSampleLoop, this);
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
    grid->Add(recButton);
    grid->Add(playButton);
    grid->Add(sliders[10],1, wxEXPAND | wxALL);
    grid->Add(new wxStaticText(this, 20012, "speed"));
    grid->Add(sliders[11],1, wxEXPAND | wxALL);
    grid->Add(new wxStaticText(this, 20013, "loop size"));

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
void MyFrame::PlayStopSampleLoop(wxCommandEvent &event)
{
    if(playSample)
    {
        manager.StopSample();
        playSample = false;
        playButton->SetLabel("play loop");
    }else{
        manager.PlaySample();
        playSample = true;
        playButton->SetLabel("stop loop");
    }
}

void MyFrame::RecordAudioSample(wxCommandEvent &event)
{
    manager.RecordInputSample(loopSize);
}

    void MyFrame::OnSlChanged(wxCommandEvent &event)
{
    int slider = event.GetId() - 10000;
    int value = sliders[slider - 1]->GetValue();
    switch (slider)
    {
        case 1: // size
        {
            if(value==100)value=99;
            float timev = value/40.0;
            manager.SetSize(timev);
            WriteToLabel("size:" + std::to_string(timev));
        }
            break;
        case 2: // decay
        {
            float dampv = value/100.0;
            manager.SetDecay(dampv);
            WriteToLabel("decay:" + std::to_string(dampv));
        }
            break;
        case 3: // damp
        {
            float dampv = value/100.0;
            manager.SetDamp(dampv);
            WriteToLabel("damp:" + std::to_string(dampv));
        }
            break;
        case 4: // diffusion
        {
            float diffv = value/120.0;
            manager.SetDiffusion(diffv);
            //label10->SetLabel(std::to_string(diffv));
            WriteToLabel("diffusion:" + std::to_string(diffv));
        }
            break;
        case 5: // lfo rate
        {
            float lfofreq = value;
            manager.SetLFO1(lfofreq / 10.0f);
            WriteToLabel("lforate:" + std::to_string(lfofreq));
        }
            break;
        case 6: // lfo depth
        {
            float modAmp = value/4.0;
            manager.setMod(modAmp);
            //label10->SetLabel(std::to_string(modAmp));
            WriteToLabel("lfodepth:" + std::to_string(modAmp));
        }
            break;
        case 7: // grain rate
        {
            float grainDepth = value / 10.0f;
            manager.setLFO2(grainDepth);
            WriteToLabel("grainrate:" + std::to_string(grainDepth));
        }
            break;
        case 8: // grain depth
        {
            float grainDepth = value/10.0f;
            manager.setGrainDepth(grainDepth);
            WriteToLabel("graindepth:" + std::to_string(grainDepth));
        }
            break;
        case 9: // tilt
        {
            float tiltV = (value/50.0)-1.0;
            manager.setTilt(tiltV);
            //label10->SetLabel(std::to_string(tiltV));
            WriteToLabel("tilt:" + std::to_string(tiltV));
        }
            break;
        case 10: // mix
        {
            float mix = value/100.0;
            manager.setMix(mix);
            //label10->SetLabel(std::to_string(mix));
            WriteToLabel("mix:" + std::to_string(mix));
        }
            break;
        case 11: // playback speed
        {
            double speed = value/100.0;
            manager.SetSamplePitchChange(speed);
            WriteToLabel("pitch change:" + std::to_string(speed));
        }
            break;
        case 12: // loop size
        {

            loopSize = (int)((value / 25.0) * 44100);
            float seconds = loopSize / 44100.0f;
            WriteToLabel("loop size:" + std::to_string(seconds) + "s");
        }
            break;

        default:
            break;
    }
    //WriteToLabel(std::to_string(slider));
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
