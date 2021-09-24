[![DOI](https://zenodo.org/badge/DOI/10.5281/zenodo.5526941.svg)](https://doi.org/10.5281/zenodo.5526941)

This repository contains data and code to accompany the manuscript titled

### Census of heat tolerance among Florida’s threatened staghorn corals finds resilient individuals throughout existing nursery populations

by **Ross Cunning** and co-authors

In this manuscript, we describe the application of coral bleaching automated stress systems (CBASS) to measure the thermal tolerance of 229 colonies of *Acropora cervicornis* across six coral nurseries spanning Florida's coral reef. Quantitative metrics for each colony's thermal tolerance are derived using the Fv/Fm ED50, and variation in heat tolerance is analyzed with respect to genetic and environmental factors. A front page for this repository can be accessed at [http://jrcunning.github.io/CBASS_FL_Acer/index.html](http://jrcunning.github.io/CBASS_FL_Acer/index.html).

#### Repository contents:

* **analysis/:** 
    + CBASS_\*.Rmd: Analysis of CBASS experiments at each nursery and timepoint. These RMarkdown documents, and rendered HTML output, contain code to import and analyze temperature and Fv/Fm data from each CBASS experiment, fit dose response curves for each coral, and perform data quality control and filtering steps.
        + [CBASS_NSU.Rmd](analysis/CBASS_NSU.Rmd) [[CBASS_NSU.html](analysis/CBASS_NSU.html)]
        + [CBASS_UM.Rmd](analysis/CBASS_UM.Rmd) [[CBASS_UM.html](analysis/CBASS_UM.html)]
        + [CBASS_CRF.Rmd](analysis/CBASS_CRF.Rmd) [[CBASS_CRF.html](analysis/CBASS_CRF.html)]
        + [CBASS_RRT.Rmd](analysis/CBASS_RRT.Rmd) [[CBASS_RRT.html](analysis/CBASS_RRT.html)]
        + [CBASS_FWC.Rmd](analysis/CBASS_FWC.Rmd) [[CBASS_FWC.html](analysis/CBASS_FWC.html)]
        + [CBASS_MML.Rmd](analysis/CBASS_MML.Rmd) [[CBASS_MML.html](analysis/CBASS_MML.html)]
        + [CBASS_Jun2020.Rmd](analysis/CBASS_Jun2020.Rmd) [[CBASS_Jun2020.html](analysis/CBASS_Jun2020.html)]
    + [ED50_analysis.Rmd](analysis/ED50_analysis.Rmd): Cleaned and filtered Fv/Fm data are used to generate ED50 values for each coral. These thermal tolerance metrics are statistically analyzed with respect to genetic and environmental factors. Rendered HTML output: [ED50_analysis.html](analysis/ED50_analysis.html)
    + [HOBO_calibration.Rmd](analysis/HOBO_calibration.Rmd) [[HOBO_calibration.html](analysis/HOBO_calibration.html)]: HOBO pendants used in CBASS experiments are cross-calibrated.
    
* **data/:**
    + genotype_metadata_raw.csv: Contains source colony coordinates and MMM temperature for each coral genotype at each nursery.
    + genotype_name_key.csv: Contains genotype identifiers and alternate names used for each genotype, in order to compare across nurseries.
    + tank_setup.csv: Contains schematic for which temperature treatments were assigned to which physical tanks during CBASS experiments.
    
* **data/raw/:** 
    + 2020\*_Acer/: Directories containing raw data for each CBASS run. The "PAM" subdirectory contains raw chlorophyll fluorescence data exported from the Walz DIVING PAM II. The "temperature" subdirectory contains recorded temperatures in each tank from temperature sensors and HOBO pendants. The "P_map.csv" and "S_map.csv" files contain the positional mapping of each fragment in each tank during the CBASS run.
    + **\*/hobo_calibration/:** Data from an independent cross-calibration of HOBO temperature sensors used in CBASS experiments.

* **data/processed/:**
    + \*_fvfm_clean.csv: Files containing the filtered Fv/Fm data from each CBASS run.
    + acer_SH_data.csv: Symbiont to host cell ratio data from qPCR analysis of Symbiodiniaceae communities.
    + hobo_calibrations.csv: Results of HOBO temperature sensor calibration.
    
* **data/program_files:**
    + arduino_code/: Program files for Arduino temperature controller
    + arduino_settings.csv: Example file used to determine temperature settings for each Arduino-controlled tank
    + inkbird_settings.csv: Temperature setpoints used to manually adjust Inkbird temperature controllers

* **output/:** contains figures used in the associated manuscript and supplementary information file. Figures are created and written to file in analysis/ED50_analysis.Rmd.
