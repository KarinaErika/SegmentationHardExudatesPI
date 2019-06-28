# SegmentationHardExudatosPI
Performs automatic segmentation of hard exudates using the opencv library (developing)

Steps for Segmentation of hard exudates [1]

1. Green channel extractetion (ok)
2. Complement Operation (ok)
3. Matched filter
4. Filtered image
5. Removal of OD [2]
5.1 Convert to hsl (ok)
5.2 Extracted L band (ok)
5.3 CLAHE (ok)
5.4 contrast stretching
5.5 Median Filtering (ok)
5.6 Radius Elargement
6. Morphological Operation 

[1] Nugroho, Hanung Adi, et al. "Automated segmentation of hard exudates based on matched filtering." 2016 International Seminar on Sensors, Instrumentation, Measurement and Metrology (ISSIMM). IEEE, 2016.

[2] Nugroho, Hanung Adi, et al. "Segmentation of exudates based on high pass filtering in retinal fundus images." 2015 7th International Conference on Information Technology and Electrical Engineering (ICITEE). IEEE, 2015.

For more information about methodology, consult the paste "methodology" this repository.

https://drive.google.com/open?id=1kK9NXWbCtfjc9xawSGYDIKBrzf9haKr0


