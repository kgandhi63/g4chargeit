#!/bin/bash

# Submit the first job and capture its Job ID
jid0=$(sbatch --parsable batchscripts/000_submit_iteration0_foronlysolarwind.sh)
echo "Submitted job for runscript: $jid0"

# Submit the first job and capture its Job ID
jid1=$(sbatch --parsable batchscripts/001_submit_iteration0_foronlysolarwind.sh)
echo "Submitted job for runscript: $jid1"

# Submit job 2 with dependency on previous job
jid2=$(sbatch --parsable --dependency=afterok:$jid1 batchscripts/002_submit_iteration1_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid2"

# Submit job 3 with dependency on previous job
jid3=$(sbatch --parsable --dependency=afterok:$jid2 batchscripts/003_submit_iteration2_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid3"

# Submit job 4 with dependency on previous job
jid4=$(sbatch --parsable --dependency=afterok:$jid3 batchscripts/004_submit_iteration3_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid4"

# Submit job 5 with dependency on previous job
jid5=$(sbatch --parsable --dependency=afterok:$jid4 batchscripts/005_submit_iteration4_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid5"

# Submit job 6 with dependency on previous job
jid6=$(sbatch --parsable --dependency=afterok:$jid5 batchscripts/006_submit_iteration5_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid6"

# Submit job 7 with dependency on previous job
jid7=$(sbatch --parsable --dependency=afterok:$jid6 batchscripts/007_submit_iteration6_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid7"

# Submit job 8 with dependency on previous job
jid8=$(sbatch --parsable --dependency=afterok:$jid7 batchscripts/008_submit_iteration7_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid8"

# Submit job 9 with dependency on previous job
jid9=$(sbatch --parsable --dependency=afterok:$jid8 batchscripts/009_submit_iteration8_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid9"

# Submit job 10 with dependency on previous job
jid10=$(sbatch --parsable --dependency=afterok:$jid9 batchscripts/010_submit_iteration9_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid10"

# Submit job 11 with dependency on previous job
jid11=$(sbatch --parsable --dependency=afterok:$jid10 batchscripts/011_submit_iteration10_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid11"

# Submit job 12 with dependency on previous job
jid12=$(sbatch --parsable --dependency=afterok:$jid11 batchscripts/012_submit_iteration11_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid12"

# Submit job 13 with dependency on previous job
jid13=$(sbatch --parsable --dependency=afterok:$jid12 batchscripts/013_submit_iteration12_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid13"

# Submit job 14 with dependency on previous job
jid14=$(sbatch --parsable --dependency=afterok:$jid13 batchscripts/014_submit_iteration13_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid14"

# Submit job 15 with dependency on previous job
jid15=$(sbatch --parsable --dependency=afterok:$jid14 batchscripts/015_submit_iteration14_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid15"

# Submit job 16 with dependency on previous job
jid16=$(sbatch --parsable --dependency=afterok:$jid15 batchscripts/016_submit_iteration15_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid16"

# Submit job 17 with dependency on previous job
jid17=$(sbatch --parsable --dependency=afterok:$jid16 batchscripts/017_submit_iteration16_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid17"

# Submit job 18 with dependency on previous job
jid18=$(sbatch --parsable --dependency=afterok:$jid17 batchscripts/018_submit_iteration17_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid18"

# Submit job 19 with dependency on previous job
jid19=$(sbatch --parsable --dependency=afterok:$jid18 batchscripts/019_submit_iteration18_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid19"

# Submit job 20 with dependency on previous job
jid20=$(sbatch --parsable --dependency=afterok:$jid19 batchscripts/020_submit_iteration19_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid20"

# Submit job 21 with dependency on previous job
jid21=$(sbatch --parsable --dependency=afterok:$jid20 batchscripts/021_submit_iteration20_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid21"

# Submit job 22 with dependency on previous job
jid22=$(sbatch --parsable --dependency=afterok:$jid21 batchscripts/022_submit_iteration21_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid22"

# Submit job 23 with dependency on previous job
jid23=$(sbatch --parsable --dependency=afterok:$jid22 batchscripts/023_submit_iteration22_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid23"

# Submit job 24 with dependency on previous job
jid24=$(sbatch --parsable --dependency=afterok:$jid23 batchscripts/024_submit_iteration23_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid24"

# Submit job 25 with dependency on previous job
jid25=$(sbatch --parsable --dependency=afterok:$jid24 batchscripts/025_submit_iteration24_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid25"

# Submit job 26 with dependency on previous job
jid26=$(sbatch --parsable --dependency=afterok:$jid25 batchscripts/026_submit_iteration25_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid26"

# Submit job 27 with dependency on previous job
jid27=$(sbatch --parsable --dependency=afterok:$jid26 batchscripts/027_submit_iteration26_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid27"

# Submit job 28 with dependency on previous job
jid28=$(sbatch --parsable --dependency=afterok:$jid27 batchscripts/028_submit_iteration27_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid28"

# Submit job 29 with dependency on previous job
jid29=$(sbatch --parsable --dependency=afterok:$jid28 batchscripts/029_submit_iteration28_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid29"

# Submit job 30 with dependency on previous job
jid30=$(sbatch --parsable --dependency=afterok:$jid29 batchscripts/030_submit_iteration29_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid30"

# Submit job 31 with dependency on previous job
jid31=$(sbatch --parsable --dependency=afterok:$jid30 batchscripts/031_submit_iteration30_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid31"

# Submit job 32 with dependency on previous job
jid32=$(sbatch --parsable --dependency=afterok:$jid31 batchscripts/032_submit_iteration31_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid32"

# Submit job 33 with dependency on previous job
jid33=$(sbatch --parsable --dependency=afterok:$jid32 batchscripts/033_submit_iteration32_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid33"

# Submit job 34 with dependency on previous job
jid34=$(sbatch --parsable --dependency=afterok:$jid33 batchscripts/034_submit_iteration33_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid34"

# Submit job 35 with dependency on previous job
jid35=$(sbatch --parsable --dependency=afterok:$jid34 batchscripts/035_submit_iteration34_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid35"

# Submit job 36 with dependency on previous job
jid36=$(sbatch --parsable --dependency=afterok:$jid35 batchscripts/036_submit_iteration35_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid36"

# Submit job 37 with dependency on previous job
jid37=$(sbatch --parsable --dependency=afterok:$jid36 batchscripts/037_submit_iteration36_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid37"

# Submit job 38 with dependency on previous job
jid38=$(sbatch --parsable --dependency=afterok:$jid37 batchscripts/038_submit_iteration37_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid38"

# Submit job 39 with dependency on previous job
jid39=$(sbatch --parsable --dependency=afterok:$jid38 batchscripts/039_submit_iteration38_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid39"

# Submit job 40 with dependency on previous job
jid40=$(sbatch --parsable --dependency=afterok:$jid39 batchscripts/040_submit_iteration39_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid40"

# Submit job 41 with dependency on previous job
jid41=$(sbatch --parsable --dependency=afterok:$jid40 batchscripts/041_submit_iteration40_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid41"

# Submit job 42 with dependency on previous job
jid42=$(sbatch --parsable --dependency=afterok:$jid41 batchscripts/042_submit_iteration41_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid42"

# Submit job 43 with dependency on previous job
jid43=$(sbatch --parsable --dependency=afterok:$jid42 batchscripts/043_submit_iteration42_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid43"

# Submit job 44 with dependency on previous job
jid44=$(sbatch --parsable --dependency=afterok:$jid43 batchscripts/044_submit_iteration43_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid44"

# Submit job 45 with dependency on previous job
jid45=$(sbatch --parsable --dependency=afterok:$jid44 batchscripts/045_submit_iteration44_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid45"

# Submit job 46 with dependency on previous job
jid46=$(sbatch --parsable --dependency=afterok:$jid45 batchscripts/046_submit_iteration45_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid46"

# Submit job 47 with dependency on previous job
jid47=$(sbatch --parsable --dependency=afterok:$jid46 batchscripts/047_submit_iteration46_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid47"

# Submit job 48 with dependency on previous job
jid48=$(sbatch --parsable --dependency=afterok:$jid47 batchscripts/048_submit_iteration47_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid48"

# Submit job 49 with dependency on previous job
jid49=$(sbatch --parsable --dependency=afterok:$jid48 batchscripts/049_submit_iteration48_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid49"

# Submit job 50 with dependency on previous job
jid50=$(sbatch --parsable --dependency=afterok:$jid49 batchscripts/050_submit_iteration49_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid50"

# Submit the first job and capture its Job ID
jid51=$(sbatch --parsable batchscripts/051_submit_iteration0_foronlyphotoemission.sh)
echo "Submitted job for runscript: $jid51"

# Submit the first job and capture its Job ID
jid52=$(sbatch --parsable batchscripts/052_submit_iteration0_foronlyphotoemission.sh)
echo "Submitted job for runscript: $jid52"

# Submit job 53 with dependency on previous job
jid53=$(sbatch --parsable --dependency=afterok:$jid52 batchscripts/053_submit_iteration1_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid53"

# Submit job 54 with dependency on previous job
jid54=$(sbatch --parsable --dependency=afterok:$jid53 batchscripts/054_submit_iteration2_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid54"

# Submit job 55 with dependency on previous job
jid55=$(sbatch --parsable --dependency=afterok:$jid54 batchscripts/055_submit_iteration3_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid55"

# Submit job 56 with dependency on previous job
jid56=$(sbatch --parsable --dependency=afterok:$jid55 batchscripts/056_submit_iteration4_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid56"

# Submit job 57 with dependency on previous job
jid57=$(sbatch --parsable --dependency=afterok:$jid56 batchscripts/057_submit_iteration5_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid57"

# Submit job 58 with dependency on previous job
jid58=$(sbatch --parsable --dependency=afterok:$jid57 batchscripts/058_submit_iteration6_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid58"

# Submit job 59 with dependency on previous job
jid59=$(sbatch --parsable --dependency=afterok:$jid58 batchscripts/059_submit_iteration7_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid59"

# Submit job 60 with dependency on previous job
jid60=$(sbatch --parsable --dependency=afterok:$jid59 batchscripts/060_submit_iteration8_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid60"

# Submit job 61 with dependency on previous job
jid61=$(sbatch --parsable --dependency=afterok:$jid60 batchscripts/061_submit_iteration9_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid61"

# Submit job 62 with dependency on previous job
jid62=$(sbatch --parsable --dependency=afterok:$jid61 batchscripts/062_submit_iteration10_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid62"

# Submit job 63 with dependency on previous job
jid63=$(sbatch --parsable --dependency=afterok:$jid62 batchscripts/063_submit_iteration11_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid63"

# Submit job 64 with dependency on previous job
jid64=$(sbatch --parsable --dependency=afterok:$jid63 batchscripts/064_submit_iteration12_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid64"

# Submit job 65 with dependency on previous job
jid65=$(sbatch --parsable --dependency=afterok:$jid64 batchscripts/065_submit_iteration13_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid65"

# Submit job 66 with dependency on previous job
jid66=$(sbatch --parsable --dependency=afterok:$jid65 batchscripts/066_submit_iteration14_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid66"

# Submit job 67 with dependency on previous job
jid67=$(sbatch --parsable --dependency=afterok:$jid66 batchscripts/067_submit_iteration15_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid67"

# Submit job 68 with dependency on previous job
jid68=$(sbatch --parsable --dependency=afterok:$jid67 batchscripts/068_submit_iteration16_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid68"

# Submit job 69 with dependency on previous job
jid69=$(sbatch --parsable --dependency=afterok:$jid68 batchscripts/069_submit_iteration17_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid69"

# Submit job 70 with dependency on previous job
jid70=$(sbatch --parsable --dependency=afterok:$jid69 batchscripts/070_submit_iteration18_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid70"

# Submit job 71 with dependency on previous job
jid71=$(sbatch --parsable --dependency=afterok:$jid70 batchscripts/071_submit_iteration19_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid71"

# Submit job 72 with dependency on previous job
jid72=$(sbatch --parsable --dependency=afterok:$jid71 batchscripts/072_submit_iteration20_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid72"

# Submit job 73 with dependency on previous job
jid73=$(sbatch --parsable --dependency=afterok:$jid72 batchscripts/073_submit_iteration21_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid73"

# Submit job 74 with dependency on previous job
jid74=$(sbatch --parsable --dependency=afterok:$jid73 batchscripts/074_submit_iteration22_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid74"

# Submit job 75 with dependency on previous job
jid75=$(sbatch --parsable --dependency=afterok:$jid74 batchscripts/075_submit_iteration23_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid75"

# Submit job 76 with dependency on previous job
jid76=$(sbatch --parsable --dependency=afterok:$jid75 batchscripts/076_submit_iteration24_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid76"

# Submit job 77 with dependency on previous job
jid77=$(sbatch --parsable --dependency=afterok:$jid76 batchscripts/077_submit_iteration25_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid77"

# Submit job 78 with dependency on previous job
jid78=$(sbatch --parsable --dependency=afterok:$jid77 batchscripts/078_submit_iteration26_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid78"

# Submit job 79 with dependency on previous job
jid79=$(sbatch --parsable --dependency=afterok:$jid78 batchscripts/079_submit_iteration27_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid79"

# Submit job 80 with dependency on previous job
jid80=$(sbatch --parsable --dependency=afterok:$jid79 batchscripts/080_submit_iteration28_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid80"

# Submit job 81 with dependency on previous job
jid81=$(sbatch --parsable --dependency=afterok:$jid80 batchscripts/081_submit_iteration29_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid81"

# Submit job 82 with dependency on previous job
jid82=$(sbatch --parsable --dependency=afterok:$jid81 batchscripts/082_submit_iteration30_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid82"

# Submit job 83 with dependency on previous job
jid83=$(sbatch --parsable --dependency=afterok:$jid82 batchscripts/083_submit_iteration31_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid83"

# Submit job 84 with dependency on previous job
jid84=$(sbatch --parsable --dependency=afterok:$jid83 batchscripts/084_submit_iteration32_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid84"

# Submit job 85 with dependency on previous job
jid85=$(sbatch --parsable --dependency=afterok:$jid84 batchscripts/085_submit_iteration33_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid85"

# Submit job 86 with dependency on previous job
jid86=$(sbatch --parsable --dependency=afterok:$jid85 batchscripts/086_submit_iteration34_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid86"

# Submit job 87 with dependency on previous job
jid87=$(sbatch --parsable --dependency=afterok:$jid86 batchscripts/087_submit_iteration35_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid87"

# Submit job 88 with dependency on previous job
jid88=$(sbatch --parsable --dependency=afterok:$jid87 batchscripts/088_submit_iteration36_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid88"

# Submit job 89 with dependency on previous job
jid89=$(sbatch --parsable --dependency=afterok:$jid88 batchscripts/089_submit_iteration37_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid89"

# Submit job 90 with dependency on previous job
jid90=$(sbatch --parsable --dependency=afterok:$jid89 batchscripts/090_submit_iteration38_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid90"

# Submit job 91 with dependency on previous job
jid91=$(sbatch --parsable --dependency=afterok:$jid90 batchscripts/091_submit_iteration39_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid91"

# Submit job 92 with dependency on previous job
jid92=$(sbatch --parsable --dependency=afterok:$jid91 batchscripts/092_submit_iteration40_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid92"

# Submit job 93 with dependency on previous job
jid93=$(sbatch --parsable --dependency=afterok:$jid92 batchscripts/093_submit_iteration41_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid93"

# Submit job 94 with dependency on previous job
jid94=$(sbatch --parsable --dependency=afterok:$jid93 batchscripts/094_submit_iteration42_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid94"

# Submit job 95 with dependency on previous job
jid95=$(sbatch --parsable --dependency=afterok:$jid94 batchscripts/095_submit_iteration43_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid95"

# Submit job 96 with dependency on previous job
jid96=$(sbatch --parsable --dependency=afterok:$jid95 batchscripts/096_submit_iteration44_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid96"

# Submit job 97 with dependency on previous job
jid97=$(sbatch --parsable --dependency=afterok:$jid96 batchscripts/097_submit_iteration45_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid97"

# Submit job 98 with dependency on previous job
jid98=$(sbatch --parsable --dependency=afterok:$jid97 batchscripts/098_submit_iteration46_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid98"

# Submit job 99 with dependency on previous job
jid99=$(sbatch --parsable --dependency=afterok:$jid98 batchscripts/099_submit_iteration47_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid99"

# Submit job 100 with dependency on previous job
jid100=$(sbatch --parsable --dependency=afterok:$jid99 batchscripts/100_submit_iteration48_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid100"

# Submit job 101 with dependency on previous job
jid101=$(sbatch --parsable --dependency=afterok:$jid100 batchscripts/101_submit_iteration49_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid101"

# Submit the first job and capture its Job ID
jid102=$(sbatch --parsable batchscripts/102_submit_iteration0_forallparticles.sh)
echo "Submitted job for runscript: $jid102"

# Submit the first job and capture its Job ID
jid103=$(sbatch --parsable batchscripts/103_submit_iteration0_forallparticles.sh)
echo "Submitted job for runscript: $jid103"

# Submit job 104 with dependency on previous job
jid104=$(sbatch --parsable --dependency=afterok:$jid103 batchscripts/104_submit_iteration1_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid104"

# Submit job 105 with dependency on previous job
jid105=$(sbatch --parsable --dependency=afterok:$jid104 batchscripts/105_submit_iteration2_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid105"

# Submit job 106 with dependency on previous job
jid106=$(sbatch --parsable --dependency=afterok:$jid105 batchscripts/106_submit_iteration3_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid106"

# Submit job 107 with dependency on previous job
jid107=$(sbatch --parsable --dependency=afterok:$jid106 batchscripts/107_submit_iteration4_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid107"

# Submit job 108 with dependency on previous job
jid108=$(sbatch --parsable --dependency=afterok:$jid107 batchscripts/108_submit_iteration5_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid108"

# Submit job 109 with dependency on previous job
jid109=$(sbatch --parsable --dependency=afterok:$jid108 batchscripts/109_submit_iteration6_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid109"

# Submit job 110 with dependency on previous job
jid110=$(sbatch --parsable --dependency=afterok:$jid109 batchscripts/110_submit_iteration7_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid110"

# Submit job 111 with dependency on previous job
jid111=$(sbatch --parsable --dependency=afterok:$jid110 batchscripts/111_submit_iteration8_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid111"

# Submit job 112 with dependency on previous job
jid112=$(sbatch --parsable --dependency=afterok:$jid111 batchscripts/112_submit_iteration9_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid112"

# Submit job 113 with dependency on previous job
jid113=$(sbatch --parsable --dependency=afterok:$jid112 batchscripts/113_submit_iteration10_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid113"

# Submit job 114 with dependency on previous job
jid114=$(sbatch --parsable --dependency=afterok:$jid113 batchscripts/114_submit_iteration11_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid114"

# Submit job 115 with dependency on previous job
jid115=$(sbatch --parsable --dependency=afterok:$jid114 batchscripts/115_submit_iteration12_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid115"

# Submit job 116 with dependency on previous job
jid116=$(sbatch --parsable --dependency=afterok:$jid115 batchscripts/116_submit_iteration13_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid116"

# Submit job 117 with dependency on previous job
jid117=$(sbatch --parsable --dependency=afterok:$jid116 batchscripts/117_submit_iteration14_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid117"

# Submit job 118 with dependency on previous job
jid118=$(sbatch --parsable --dependency=afterok:$jid117 batchscripts/118_submit_iteration15_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid118"

# Submit job 119 with dependency on previous job
jid119=$(sbatch --parsable --dependency=afterok:$jid118 batchscripts/119_submit_iteration16_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid119"

# Submit job 120 with dependency on previous job
jid120=$(sbatch --parsable --dependency=afterok:$jid119 batchscripts/120_submit_iteration17_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid120"

# Submit job 121 with dependency on previous job
jid121=$(sbatch --parsable --dependency=afterok:$jid120 batchscripts/121_submit_iteration18_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid121"

# Submit job 122 with dependency on previous job
jid122=$(sbatch --parsable --dependency=afterok:$jid121 batchscripts/122_submit_iteration19_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid122"

# Submit job 123 with dependency on previous job
jid123=$(sbatch --parsable --dependency=afterok:$jid122 batchscripts/123_submit_iteration20_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid123"

# Submit job 124 with dependency on previous job
jid124=$(sbatch --parsable --dependency=afterok:$jid123 batchscripts/124_submit_iteration21_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid124"

# Submit job 125 with dependency on previous job
jid125=$(sbatch --parsable --dependency=afterok:$jid124 batchscripts/125_submit_iteration22_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid125"

# Submit job 126 with dependency on previous job
jid126=$(sbatch --parsable --dependency=afterok:$jid125 batchscripts/126_submit_iteration23_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid126"

# Submit job 127 with dependency on previous job
jid127=$(sbatch --parsable --dependency=afterok:$jid126 batchscripts/127_submit_iteration24_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid127"

# Submit job 128 with dependency on previous job
jid128=$(sbatch --parsable --dependency=afterok:$jid127 batchscripts/128_submit_iteration25_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid128"

# Submit job 129 with dependency on previous job
jid129=$(sbatch --parsable --dependency=afterok:$jid128 batchscripts/129_submit_iteration26_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid129"

# Submit job 130 with dependency on previous job
jid130=$(sbatch --parsable --dependency=afterok:$jid129 batchscripts/130_submit_iteration27_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid130"

# Submit job 131 with dependency on previous job
jid131=$(sbatch --parsable --dependency=afterok:$jid130 batchscripts/131_submit_iteration28_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid131"

# Submit job 132 with dependency on previous job
jid132=$(sbatch --parsable --dependency=afterok:$jid131 batchscripts/132_submit_iteration29_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid132"

# Submit job 133 with dependency on previous job
jid133=$(sbatch --parsable --dependency=afterok:$jid132 batchscripts/133_submit_iteration30_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid133"

# Submit job 134 with dependency on previous job
jid134=$(sbatch --parsable --dependency=afterok:$jid133 batchscripts/134_submit_iteration31_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid134"

# Submit job 135 with dependency on previous job
jid135=$(sbatch --parsable --dependency=afterok:$jid134 batchscripts/135_submit_iteration32_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid135"

# Submit job 136 with dependency on previous job
jid136=$(sbatch --parsable --dependency=afterok:$jid135 batchscripts/136_submit_iteration33_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid136"

# Submit job 137 with dependency on previous job
jid137=$(sbatch --parsable --dependency=afterok:$jid136 batchscripts/137_submit_iteration34_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid137"

# Submit job 138 with dependency on previous job
jid138=$(sbatch --parsable --dependency=afterok:$jid137 batchscripts/138_submit_iteration35_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid138"

# Submit job 139 with dependency on previous job
jid139=$(sbatch --parsable --dependency=afterok:$jid138 batchscripts/139_submit_iteration36_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid139"

# Submit job 140 with dependency on previous job
jid140=$(sbatch --parsable --dependency=afterok:$jid139 batchscripts/140_submit_iteration37_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid140"

# Submit job 141 with dependency on previous job
jid141=$(sbatch --parsable --dependency=afterok:$jid140 batchscripts/141_submit_iteration38_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid141"

# Submit job 142 with dependency on previous job
jid142=$(sbatch --parsable --dependency=afterok:$jid141 batchscripts/142_submit_iteration39_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid142"

# Submit job 143 with dependency on previous job
jid143=$(sbatch --parsable --dependency=afterok:$jid142 batchscripts/143_submit_iteration40_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid143"

# Submit job 144 with dependency on previous job
jid144=$(sbatch --parsable --dependency=afterok:$jid143 batchscripts/144_submit_iteration41_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid144"

# Submit job 145 with dependency on previous job
jid145=$(sbatch --parsable --dependency=afterok:$jid144 batchscripts/145_submit_iteration42_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid145"

# Submit job 146 with dependency on previous job
jid146=$(sbatch --parsable --dependency=afterok:$jid145 batchscripts/146_submit_iteration43_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid146"

# Submit job 147 with dependency on previous job
jid147=$(sbatch --parsable --dependency=afterok:$jid146 batchscripts/147_submit_iteration44_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid147"

# Submit job 148 with dependency on previous job
jid148=$(sbatch --parsable --dependency=afterok:$jid147 batchscripts/148_submit_iteration45_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid148"

# Submit job 149 with dependency on previous job
jid149=$(sbatch --parsable --dependency=afterok:$jid148 batchscripts/149_submit_iteration46_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid149"

# Submit job 150 with dependency on previous job
jid150=$(sbatch --parsable --dependency=afterok:$jid149 batchscripts/150_submit_iteration47_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid150"

# Submit job 151 with dependency on previous job
jid151=$(sbatch --parsable --dependency=afterok:$jid150 batchscripts/151_submit_iteration48_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid151"

# Submit job 152 with dependency on previous job
jid152=$(sbatch --parsable --dependency=afterok:$jid151 batchscripts/152_submit_iteration49_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid152"

