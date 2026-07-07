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
    wxSlider* sliders[12];
    wxStaticText* labels[12];
    const std::string sliderLabels[12] = {"size", "decay", "damp", "diffsn", "LFO F", "LFO D", "grain R", "grain D", "tilt", "mix", "loop pitch", "loop length"};
    wxButton* startStopButton = new wxButton(this, 30001, "start audio");
    wxButton* recButton = new wxButton(this, 30003, "record loop");
    wxButton* playButton = new wxButton(this, 30004, "play loop");
    void WriteToLabel(std::string text)
    {
        label10->SetLabel(text);
    }
};

MyFrame::MyFrame(const wxString &title, const wxPoint &pos, const wxSize &size) : wxFrame(NULL, wxID_ANY, title, pos, size)
{
    for(int x=0; x < 12; x++){
        sliders[x] = new wxSlider(this,10001 + x,50,1,100,wxDefaultPosition, wxDefaultSize, wxSL_VERTICAL, wxDefaultValidator);
        sliders[x]->Bind(wxEVT_SLIDER, &MyFrame::OnSlChanged, this);
        labels[x] = new wxStaticText(this, 20001 + x, sliderLabels[x]);
    }
    //bind buttons
    startStopButton->Bind(wxEVT_BUTTON, &MyFrame::StartStopAudio, this);
    recButton->Bind(wxEVT_BUTTON, &MyFrame::RecordAudioSample, this);
    playButton->Bind(wxEVT_BUTTON, &MyFrame::PlayStopSampleLoop, this);

    // Related to precalculations in audio engine
    manager.Init();

    // Wxwidgets related stuff
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);
    // Rad 1
    wxBoxSizer* row1 = new wxBoxSizer(wxHORIZONTAL);
    for (int i = 0; i < 10; ++i)
    {
        wxBoxSizer* col = new wxBoxSizer(wxVERTICAL);
        col->Add(sliders[i], 1, wxALIGN_CENTER | wxALL, 5);
        col->Add(labels[i], 0, wxALIGN_CENTER | wxBOTTOM, 10);
        row1->Add(col, 0, wxALIGN_CENTER | wxALL, 10);
    }
    mainSizer->Add(row1, 0, wxALIGN_CENTER);




    // Rad 2 (sliders 4–7)
    wxBoxSizer* row2 = new wxBoxSizer(wxHORIZONTAL);
    
    wxBoxSizer* col1 = new wxBoxSizer(wxVERTICAL);
    col1->Add(startStopButton, 1, wxALIGN_CENTER | wxALL, 5);
    row2->Add(col1, 0, wxALIGN_CENTER | wxALL, 10);

    wxBoxSizer* col2 = new wxBoxSizer(wxVERTICAL);
    col2->Add(recButton, 1, wxALIGN_CENTER | wxALL, 5);
    row2->Add(col2, 0, wxALIGN_CENTER | wxALL, 10);

    wxBoxSizer* col3 = new wxBoxSizer(wxVERTICAL);
    col3->Add(playButton, 1, wxALIGN_CENTER | wxALL, 5);
    row2->Add(col3, 0, wxALIGN_CENTER | wxALL, 10);

    wxBoxSizer* col4 = new wxBoxSizer(wxVERTICAL);
    col4->Add(sliders[10], 1, wxALIGN_CENTER | wxALL, 5);
    col4->Add(labels[10], 0, wxALIGN_CENTER | wxBOTTOM, 10);
    row2->Add(col4, 0, wxALIGN_CENTER | wxALL, 10);

    wxBoxSizer* col5 = new wxBoxSizer(wxVERTICAL);
    col5->Add(sliders[11], 1, wxALIGN_CENTER | wxALL, 5);
    col5->Add(labels[11], 0, wxALIGN_CENTER | wxBOTTOM, 10);
    row2->Add(col5, 0, wxALIGN_CENTER | wxALL, 10);

    wxBoxSizer* col6 = new wxBoxSizer(wxVERTICAL);
    col6->Add(label10, 1, wxALIGN_CENTER | wxALL, 5);
    row2->Add(col6, 0, wxALIGN_CENTER | wxALL, 10);
    
    mainSizer->Add(row2, 0, wxALIGN_CENTER);

    SetSizer(mainSizer);
    Layout();
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
    manager.RecordInputSample(44100 * 4, loopSize);
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
            float modAmp = (value/4.0) - 0.25f;
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
            float grainDepth = (value/10.0f) - 0.1f;
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
            double speed = (value/25.0) - 2.0;
            manager.SetSamplePitchChange(speed);
            WriteToLabel("pitch change:" + std::to_string(speed));
        }
            break;
        case 12: // loop size
        {

            loopSize = (int)((value / 25.0) * 44100);
            if(manager.IsPlayingSample())
            {
                // change loop size while playing
                manager.ChangeLoopLength(loopSize);
            }

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
