#include "TFile.h"
#include "TTree.h"
#include "TMVA/Factory.h"
#include "TMVA/DataLoader.h"
#include "TMVA/Types.h"

void TMVA_training() {
    TFile *inputFile = TFile::Open("../data/Higgs_data.root");
    TTree *signalTree = (TTree *)inputFile->Get("sig_tree");
    TTree *backgroundTree = (TTree *)inputFile->Get("bkg_tree");

    // Holds the training/test trees, ROC curves and control histograms read by the TMVA GUI
    TFile *outputFile = TFile::Open("TMVA_Output.root", "RECREATE");

    // The Factory books, trains, tests and evaluates the classifiers.
    // ROC: compute ROC integrals; !V: not verbose; Color: coloured terminal output
    TMVA::Factory factory("TMVAClassification", outputFile,
                          "!V:ROC:!Silent:Color:AnalysisType=Classification");

    // The DataLoader name sets the output directory: weights go to dataset/weights/
    TMVA::DataLoader *loader = new TMVA::DataLoader("dataset");

    // Input features (invariant masses); 'F' = float. Must match the order used in TMVA_applying.C
    for (auto v : {"m_jj", "m_jjj", "m_lv", "m_jlv", "m_bb", "m_wbb", "m_wwbb"})
        loader->AddVariable(v, 'F');

    // Global per-tree weight of 1 (no extra event weighting)
    loader->AddSignalTree(signalTree, 1.0);
    loader->AddBackgroundTree(backgroundTree, 1.0);

    // No preselection cuts for signal or background ("", "").
    // 5000 events of each class are drawn at random for training; the rest is used for testing.
    // NormMode=NumEvents: signal and background get the same total weight in training
    loader->PrepareTrainingAndTestTree("", "",
                                       "nTrain_Signal=5000:nTrain_Background=5000:"
                                       "SplitMode=Random:NormMode=NumEvents:!V");

    // Boosted decision tree:
    //   NTrees=400        number of trees in the forest
    //   MinNodeSize=2.5%  min fraction of training events per leaf (limits overtraining)
    //   MaxDepth=3        shallow trees, as usual for boosting
    //   AdaBoostBeta=0.5  learning rate of AdaBoost
    //   GiniIndex         criterion for choosing each split
    //   nCuts=20          grid points scanned per variable to find the best cut
    factory.BookMethod(loader, TMVA::Types::kBDT, "BDT",
                       "!V:NTrees=400:MinNodeSize=2.5%:MaxDepth=3:"
                       "BoostType=AdaBoost:AdaBoostBeta=0.5:"
                       "SeparationType=GiniIndex:nCuts=20");

    factory.TrainAllMethods();     // trains and writes dataset/weights/TMVAClassification_BDT.weights.xml
    factory.TestAllMethods();      // evaluates the BDT on the test sample
    factory.EvaluateAllMethods();  // prints ROC integral, separation, overtraining check
}

// Inspect results (ROC, overtraining, input variables) with:
// root -l -e 'TMVA::TMVAGui("TMVA_Output.root")'
