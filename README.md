# CBASS_FL_Acer


This repository contains data and code to accompany the manuscript titled

### Census of heat tolerance among Florida's threatened staghorn corals identifies resilient individuals throughout nursery populations

by **Ross Cunning** and co-authors

In this manuscript, we describe the application of coral bleaching automated stress systems (CBASS) to measure the thermal tolerance of 228 colonies of *Acropora cervicornis* across six coral nurseries spanning Florida's coral reef. Quantitative metrics for each colony's thermal tolerance are derived using the Fv/Fm ED50, and variation in heat tolerance is analyzed with respect to genetic and environmental effects. A front page for this repository can be accessed at [http://jrcunning.github.io/CBASS_FL_Acer/index.html](http://jrcunning.github.io/CBASS_FL_Acer/index.html).

#### Repository contents:

* **analysis/:** 
    + CBASS_\*.Rmd: Analysis of CBASS experiments at each nursery and timepoint. These RMarkdown documents contain code to import and analyze temperature and Fv/Fm data from each CBASS experiment, and fit dose response curves for each coral. Knitted HTML outputs detailing each CBASS run are linked here:
        + [CBASS_NSU.html](analysis/CBASS_NSU.html)
        + [CBASS_UM.html](analysis/CBASS_UM.html)
        + [CBASS_CRF.html](analysis/CBASS_CRF.html)
        + [CBASS_RRT.html](analysis/CBASS_RRT.html)
        + [CBASS_FWC.html](analysis/CBASS_FWC.html)
        + [CBASS_MML.html](analysis/CBASS_MML.html)
        + [CBASS_Jun2020.html](analysis/CBASS_Jun2020.html)
    + [ED50_analysis.Rmd](analysis/ED50_analysis.html)
    + HOBO_calibration.Rmd:
    
* **data/raw:** Contains raw temperature and photophysiological data collected during CBASS experiments. There is one sub-directory for each CBASS experiment run, with the date of the experiment in the directory name. Each of these directories contains the following files:
    + **\*/PAM/:** Raw chlorophyll fluorescence data exported from the Walz DIVING PAM II
    + **\*/temperature/:**
    
* **data/raw/hobo_calibration:**

* **data/processed:**

* **data/program_files:**
    


* **output/:** 
