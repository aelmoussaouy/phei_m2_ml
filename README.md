# phei_m2_ml

## Repository structure

```
phei_m2_ml/
├── course/      # course material (coming soon)
├── exercices/   # exercises
├── solutions/   # solutions to the exercises
├── scripts/     # Python notebooks and ROOT C++ macros
└── data/        # Higgs_data.root
```

There are **two ways** to run the code:

1. **Python (Jupyter notebooks on Google Colab).** No local installation needed.
2. **ROOT C++ macros.** Run locally with a ROOT installation.

---

## Option 1: Python notebooks on Google Colab

| Notebook | Description | Run |
|---|---|---|
| `scripts/ROOT.ipynb` | Installs ROOT on Colab and runs a quick check | [![Open In Colab](https://colab.research.google.com/assets/colab-badge.svg)](https://colab.research.google.com/github/aelmoussaouy/phei_m2_ml/blob/main/scripts/ROOT.ipynb) |
| `scripts/Classification_Higgs_with_BDT_.ipynb` | Signal/background classification with a single TMVA BDT | [![Open In Colab](https://colab.research.google.com/assets/colab-badge.svg)](https://colab.research.google.com/github/aelmoussaouy/phei_m2_ml/blob/main/scripts/Classification_Higgs_with_BDT_.ipynb) |
| `scripts/Comparing_BDTs_Higgs_classfication.ipynb` | Comparison of several BDT configurations with ROC curves | [![Open In Colab](https://colab.research.google.com/assets/colab-badge.svg)](https://colab.research.google.com/github/aelmoussaouy/phei_m2_ml/blob/main/scripts/Comparing_BDTs_Higgs_classfication.ipynb) |

**Steps**

1. Download `Higgs_data.root` from the `data/` folder of this repository and upload it to the top level of your Google Drive (My Drive):
   ```
   MyDrive/Higgs_data.root
   ```
   (or change `inputFileName` in the notebook to point to your copy).
2. Click an **Open in Colab** badge above.
3. Run the first code cell: it installs conda and restarts the runtime automatically. Then run the remaining cells in order (`Runtime > Run after`, or one by one). Installing ROOT takes a few minutes, and you will be asked to authorise access to Google Drive.

---

## Option 2: ROOT C++ macros

| Macro | Description |
|---|---|
| `scripts/TMVA_training.C` | Trains a BDT on 7 mass variables and writes `TMVA_Output.root` and the weights to `dataset/weights/` |
| `scripts/TMVA_applying.C` | Loads the trained BDT, applies it to signal and background events, and saves `bdt_response.png` and `BDT_applied.root` |

**Requirements:** a working [ROOT installation](https://root.cern/install/) with TMVA enabled (included by default).

**Steps**

```bash
# 1. Get the code and data
git clone https://github.com/aelmoussaouy/phei_m2_ml.git
cd phei_m2_ml/scripts

# 2. Train the BDT
root -l -b -q TMVA_training.C

# 3. Apply the trained BDT (run after training)
root -l -b -q TMVA_applying.C

# 4. (Optional) Inspect the training results with the TMVA GUI
root -l -e 'TMVA::TMVAGui("TMVA_Output.root")'
```

Flags: `-l` skips the ROOT splash screen, `-b` runs in batch mode (no windows), and `-q` quits when the macro finishes.
Run the macros from the `scripts/` folder: they read the data from `../data/Higgs_data.root`, and the output files are written to `scripts/`.

---

## Running the notebooks locally (optional)

The notebooks can also run on your own computer with Jupyter instead of Colab. You need ROOT with its Python bindings and Jupyter. The simplest way to get both is a conda environment from [conda-forge](https://conda-forge.org), which provides ROOT for Linux and macOS (not for native Windows).

### Ubuntu

```bash
# 1. Install Miniforge (conda), once
wget https://github.com/conda-forge/miniforge/releases/latest/download/Miniforge3-Linux-x86_64.sh
bash Miniforge3-Linux-x86_64.sh -b
~/miniforge3/bin/conda init bash
source ~/.bashrc

# 2. Create an environment with ROOT and Jupyter, once (takes a few minutes)
conda create -y -n root_env -c conda-forge root jupyter

# 3. Each time: activate the environment and start Jupyter
conda activate root_env
git clone https://github.com/aelmoussaouy/phei_m2_ml.git   # first time only
cd phei_m2_ml/scripts
jupyter notebook
```

Check the installation with `python -c "import ROOT; print(ROOT.gROOT.GetVersion())"`.

### Windows

ROOT's Python bindings are not available from conda on native Windows, so use **WSL** (an Ubuntu system running inside Windows):

1. Open **Command Prompt** or **PowerShell** as administrator and run:
   ```
   wsl --install -d Ubuntu
   ```
   Restart the computer, then open **Ubuntu** from the Start menu and choose a user name and password.
2. In the Ubuntu terminal, follow the **Ubuntu** steps above.
3. Jupyter prints a link starting with `http://localhost:8888/...`; open it in your Windows browser.

### VS Code (Ubuntu or Windows)

1. Install [VS Code](https://code.visualstudio.com) and the **Python** and **Jupyter** extensions. On Windows, also install the **WSL** extension.
2. On Windows, open the Ubuntu terminal, go to the repository and run `code .` to open it in VS Code connected to WSL. On Ubuntu, run `code .` in the repository folder.
3. Open a notebook from `scripts/` and select the `root_env` kernel (top right, **Select Kernel → Python Environments → root_env**).

### Changes when running locally

The notebooks are written for Colab. When running them locally:

- **Skip** the conda install cells (`condacolab`, `mamba`) and the Google Drive cell (`drive.mount(...)`).
- **Change the data path** to the copy in the repository:
  ```python
  inputFileName = "../data/Higgs_data.root"
  ```
