#include "TFile.h"
#include "TTree.h"
#include "TH1F.h"
#include "TCanvas.h"
#include "TLegend.h"
#include "TStyle.h"
#include "TMVA/Reader.h"

void TMVA_applying() {
    TFile *inputFile = TFile::Open("../data/Higgs_data.root");
    // Index 0 = signal, 1 = background (same order as hists below)
    TTree *trees[2] = {(TTree *)inputFile->Get("sig_tree"),
                       (TTree *)inputFile->Get("bkg_tree")};

    // The Reader loads the trained BDT and evaluates it event by event
    TMVA::Reader *reader = new TMVA::Reader("!Color:!Silent");

    // The Reader reads its inputs from these addresses, so vars[] must be filled before each evaluation
    Float_t vars[7];
    const char *names[] =
        {"m_jj", "m_jjj", "m_lv", "m_jlv", "m_bb", "m_wbb", "m_wwbb"};

    // Names and order must match the AddVariable calls used in training
    for (int i = 0; i < 7; i++)
        reader->AddVariable(names[i], &vars[i]);

    // Load the weights written by TMVA_training.C; "BDT method" is just a label used below
    reader->BookMVA("BDT method",
                    "dataset/weights/TMVAClassification_BDT.weights.xml");

    TH1F *hSig = new TH1F("hSig", "BDT response;BDT score;Normalised events", 50, -1, 1);
    TH1F *hBkg = new TH1F("hBkg", "BDT response;BDT score;Normalised events", 50, -1, 1);
    TH1F *hists[2] = {hSig, hBkg};

    for (int t = 0; t < 2; t++) {
        // Point the tree branches at vars[], so GetEntry fills the Reader's inputs directly
        for (int i = 0; i < 7; i++)
            trees[t]->SetBranchAddress(names[i], &vars[i]);

        // Note: this loops over all events, including the ones used for training
        for (Long64_t ievt = 0; ievt < trees[t]->GetEntries(); ++ievt) {
            trees[t]->GetEntry(ievt);
            Double_t bdtScore = reader->EvaluateMVA("BDT method");
            // bdtScore near +1 -> signal-like, near -1 -> background-like
            hists[t]->Fill(bdtScore);
        }
    }

    gStyle->SetOptStat(0);  // hide the statistics box
    TCanvas *c = new TCanvas("c", "BDT response", 800, 600);

    // Normalise to unit area to compare shapes, independent of sample sizes
    hSig->Scale(1.0 / hSig->Integral());
    hBkg->Scale(1.0 / hBkg->Integral());

    hSig->SetLineColor(kBlue);   hSig->SetFillColorAlpha(kBlue, 0.3);
    hBkg->SetLineColor(kRed);    hBkg->SetFillColorAlpha(kRed, 0.3);
    // The first histogram drawn sets the y-range, so make room for the taller of the two
    hSig->SetMaximum(1.2 * std::max(hSig->GetMaximum(), hBkg->GetMaximum()));

    hSig->Draw("HIST");
    hBkg->Draw("HIST SAME");

    TLegend *leg = new TLegend(0.65, 0.75, 0.88, 0.88);
    leg->AddEntry(hSig, "Signal", "f");
    leg->AddEntry(hBkg, "Background", "f");
    leg->Draw();

    c->SaveAs("bdt_response.png");

    // Save the score histograms for later use
    TFile *out = TFile::Open("BDT_applied.root", "RECREATE");
    hSig->Write();
    hBkg->Write();
    out->Close();
}
