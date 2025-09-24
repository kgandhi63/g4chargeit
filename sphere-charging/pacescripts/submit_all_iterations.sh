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

# Submit job 51 with dependency on previous job
jid51=$(sbatch --parsable --dependency=afterok:$jid50 batchscripts/051_submit_iteration50_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid51"

# Submit job 52 with dependency on previous job
jid52=$(sbatch --parsable --dependency=afterok:$jid51 batchscripts/052_submit_iteration51_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid52"

# Submit job 53 with dependency on previous job
jid53=$(sbatch --parsable --dependency=afterok:$jid52 batchscripts/053_submit_iteration52_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid53"

# Submit job 54 with dependency on previous job
jid54=$(sbatch --parsable --dependency=afterok:$jid53 batchscripts/054_submit_iteration53_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid54"

# Submit job 55 with dependency on previous job
jid55=$(sbatch --parsable --dependency=afterok:$jid54 batchscripts/055_submit_iteration54_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid55"

# Submit job 56 with dependency on previous job
jid56=$(sbatch --parsable --dependency=afterok:$jid55 batchscripts/056_submit_iteration55_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid56"

# Submit job 57 with dependency on previous job
jid57=$(sbatch --parsable --dependency=afterok:$jid56 batchscripts/057_submit_iteration56_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid57"

# Submit job 58 with dependency on previous job
jid58=$(sbatch --parsable --dependency=afterok:$jid57 batchscripts/058_submit_iteration57_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid58"

# Submit job 59 with dependency on previous job
jid59=$(sbatch --parsable --dependency=afterok:$jid58 batchscripts/059_submit_iteration58_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid59"

# Submit job 60 with dependency on previous job
jid60=$(sbatch --parsable --dependency=afterok:$jid59 batchscripts/060_submit_iteration59_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid60"

# Submit job 61 with dependency on previous job
jid61=$(sbatch --parsable --dependency=afterok:$jid60 batchscripts/061_submit_iteration60_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid61"

# Submit job 62 with dependency on previous job
jid62=$(sbatch --parsable --dependency=afterok:$jid61 batchscripts/062_submit_iteration61_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid62"

# Submit job 63 with dependency on previous job
jid63=$(sbatch --parsable --dependency=afterok:$jid62 batchscripts/063_submit_iteration62_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid63"

# Submit job 64 with dependency on previous job
jid64=$(sbatch --parsable --dependency=afterok:$jid63 batchscripts/064_submit_iteration63_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid64"

# Submit job 65 with dependency on previous job
jid65=$(sbatch --parsable --dependency=afterok:$jid64 batchscripts/065_submit_iteration64_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid65"

# Submit job 66 with dependency on previous job
jid66=$(sbatch --parsable --dependency=afterok:$jid65 batchscripts/066_submit_iteration65_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid66"

# Submit job 67 with dependency on previous job
jid67=$(sbatch --parsable --dependency=afterok:$jid66 batchscripts/067_submit_iteration66_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid67"

# Submit job 68 with dependency on previous job
jid68=$(sbatch --parsable --dependency=afterok:$jid67 batchscripts/068_submit_iteration67_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid68"

# Submit job 69 with dependency on previous job
jid69=$(sbatch --parsable --dependency=afterok:$jid68 batchscripts/069_submit_iteration68_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid69"

# Submit job 70 with dependency on previous job
jid70=$(sbatch --parsable --dependency=afterok:$jid69 batchscripts/070_submit_iteration69_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid70"

# Submit job 71 with dependency on previous job
jid71=$(sbatch --parsable --dependency=afterok:$jid70 batchscripts/071_submit_iteration70_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid71"

# Submit job 72 with dependency on previous job
jid72=$(sbatch --parsable --dependency=afterok:$jid71 batchscripts/072_submit_iteration71_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid72"

# Submit job 73 with dependency on previous job
jid73=$(sbatch --parsable --dependency=afterok:$jid72 batchscripts/073_submit_iteration72_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid73"

# Submit job 74 with dependency on previous job
jid74=$(sbatch --parsable --dependency=afterok:$jid73 batchscripts/074_submit_iteration73_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid74"

# Submit job 75 with dependency on previous job
jid75=$(sbatch --parsable --dependency=afterok:$jid74 batchscripts/075_submit_iteration74_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid75"

# Submit job 76 with dependency on previous job
jid76=$(sbatch --parsable --dependency=afterok:$jid75 batchscripts/076_submit_iteration75_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid76"

# Submit job 77 with dependency on previous job
jid77=$(sbatch --parsable --dependency=afterok:$jid76 batchscripts/077_submit_iteration76_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid77"

# Submit job 78 with dependency on previous job
jid78=$(sbatch --parsable --dependency=afterok:$jid77 batchscripts/078_submit_iteration77_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid78"

# Submit job 79 with dependency on previous job
jid79=$(sbatch --parsable --dependency=afterok:$jid78 batchscripts/079_submit_iteration78_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid79"

# Submit job 80 with dependency on previous job
jid80=$(sbatch --parsable --dependency=afterok:$jid79 batchscripts/080_submit_iteration79_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid80"

# Submit job 81 with dependency on previous job
jid81=$(sbatch --parsable --dependency=afterok:$jid80 batchscripts/081_submit_iteration80_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid81"

# Submit job 82 with dependency on previous job
jid82=$(sbatch --parsable --dependency=afterok:$jid81 batchscripts/082_submit_iteration81_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid82"

# Submit job 83 with dependency on previous job
jid83=$(sbatch --parsable --dependency=afterok:$jid82 batchscripts/083_submit_iteration82_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid83"

# Submit job 84 with dependency on previous job
jid84=$(sbatch --parsable --dependency=afterok:$jid83 batchscripts/084_submit_iteration83_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid84"

# Submit job 85 with dependency on previous job
jid85=$(sbatch --parsable --dependency=afterok:$jid84 batchscripts/085_submit_iteration84_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid85"

# Submit job 86 with dependency on previous job
jid86=$(sbatch --parsable --dependency=afterok:$jid85 batchscripts/086_submit_iteration85_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid86"

# Submit job 87 with dependency on previous job
jid87=$(sbatch --parsable --dependency=afterok:$jid86 batchscripts/087_submit_iteration86_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid87"

# Submit job 88 with dependency on previous job
jid88=$(sbatch --parsable --dependency=afterok:$jid87 batchscripts/088_submit_iteration87_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid88"

# Submit job 89 with dependency on previous job
jid89=$(sbatch --parsable --dependency=afterok:$jid88 batchscripts/089_submit_iteration88_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid89"

# Submit job 90 with dependency on previous job
jid90=$(sbatch --parsable --dependency=afterok:$jid89 batchscripts/090_submit_iteration89_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid90"

# Submit job 91 with dependency on previous job
jid91=$(sbatch --parsable --dependency=afterok:$jid90 batchscripts/091_submit_iteration90_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid91"

# Submit job 92 with dependency on previous job
jid92=$(sbatch --parsable --dependency=afterok:$jid91 batchscripts/092_submit_iteration91_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid92"

# Submit job 93 with dependency on previous job
jid93=$(sbatch --parsable --dependency=afterok:$jid92 batchscripts/093_submit_iteration92_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid93"

# Submit job 94 with dependency on previous job
jid94=$(sbatch --parsable --dependency=afterok:$jid93 batchscripts/094_submit_iteration93_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid94"

# Submit job 95 with dependency on previous job
jid95=$(sbatch --parsable --dependency=afterok:$jid94 batchscripts/095_submit_iteration94_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid95"

# Submit job 96 with dependency on previous job
jid96=$(sbatch --parsable --dependency=afterok:$jid95 batchscripts/096_submit_iteration95_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid96"

# Submit job 97 with dependency on previous job
jid97=$(sbatch --parsable --dependency=afterok:$jid96 batchscripts/097_submit_iteration96_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid97"

# Submit job 98 with dependency on previous job
jid98=$(sbatch --parsable --dependency=afterok:$jid97 batchscripts/098_submit_iteration97_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid98"

# Submit job 99 with dependency on previous job
jid99=$(sbatch --parsable --dependency=afterok:$jid98 batchscripts/099_submit_iteration98_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid99"

# Submit job 100 with dependency on previous job
jid100=$(sbatch --parsable --dependency=afterok:$jid99 batchscripts/100_submit_iteration99_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid100"

# Submit job 101 with dependency on previous job
jid101=$(sbatch --parsable --dependency=afterok:$jid100 batchscripts/101_submit_iteration100_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid101"

# Submit the first job and capture its Job ID
jid102=$(sbatch --parsable batchscripts/102_submit_iteration0_foronlyphotoemission.sh)
echo "Submitted job for runscript: $jid102"

# Submit the first job and capture its Job ID
jid103=$(sbatch --parsable batchscripts/103_submit_iteration0_foronlyphotoemission.sh)
echo "Submitted job for runscript: $jid103"

# Submit job 104 with dependency on previous job
jid104=$(sbatch --parsable --dependency=afterok:$jid103 batchscripts/104_submit_iteration1_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid104"

# Submit job 105 with dependency on previous job
jid105=$(sbatch --parsable --dependency=afterok:$jid104 batchscripts/105_submit_iteration2_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid105"

# Submit job 106 with dependency on previous job
jid106=$(sbatch --parsable --dependency=afterok:$jid105 batchscripts/106_submit_iteration3_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid106"

# Submit job 107 with dependency on previous job
jid107=$(sbatch --parsable --dependency=afterok:$jid106 batchscripts/107_submit_iteration4_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid107"

# Submit job 108 with dependency on previous job
jid108=$(sbatch --parsable --dependency=afterok:$jid107 batchscripts/108_submit_iteration5_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid108"

# Submit job 109 with dependency on previous job
jid109=$(sbatch --parsable --dependency=afterok:$jid108 batchscripts/109_submit_iteration6_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid109"

# Submit job 110 with dependency on previous job
jid110=$(sbatch --parsable --dependency=afterok:$jid109 batchscripts/110_submit_iteration7_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid110"

# Submit job 111 with dependency on previous job
jid111=$(sbatch --parsable --dependency=afterok:$jid110 batchscripts/111_submit_iteration8_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid111"

# Submit job 112 with dependency on previous job
jid112=$(sbatch --parsable --dependency=afterok:$jid111 batchscripts/112_submit_iteration9_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid112"

# Submit job 113 with dependency on previous job
jid113=$(sbatch --parsable --dependency=afterok:$jid112 batchscripts/113_submit_iteration10_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid113"

# Submit job 114 with dependency on previous job
jid114=$(sbatch --parsable --dependency=afterok:$jid113 batchscripts/114_submit_iteration11_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid114"

# Submit job 115 with dependency on previous job
jid115=$(sbatch --parsable --dependency=afterok:$jid114 batchscripts/115_submit_iteration12_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid115"

# Submit job 116 with dependency on previous job
jid116=$(sbatch --parsable --dependency=afterok:$jid115 batchscripts/116_submit_iteration13_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid116"

# Submit job 117 with dependency on previous job
jid117=$(sbatch --parsable --dependency=afterok:$jid116 batchscripts/117_submit_iteration14_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid117"

# Submit job 118 with dependency on previous job
jid118=$(sbatch --parsable --dependency=afterok:$jid117 batchscripts/118_submit_iteration15_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid118"

# Submit job 119 with dependency on previous job
jid119=$(sbatch --parsable --dependency=afterok:$jid118 batchscripts/119_submit_iteration16_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid119"

# Submit job 120 with dependency on previous job
jid120=$(sbatch --parsable --dependency=afterok:$jid119 batchscripts/120_submit_iteration17_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid120"

# Submit job 121 with dependency on previous job
jid121=$(sbatch --parsable --dependency=afterok:$jid120 batchscripts/121_submit_iteration18_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid121"

# Submit job 122 with dependency on previous job
jid122=$(sbatch --parsable --dependency=afterok:$jid121 batchscripts/122_submit_iteration19_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid122"

# Submit job 123 with dependency on previous job
jid123=$(sbatch --parsable --dependency=afterok:$jid122 batchscripts/123_submit_iteration20_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid123"

# Submit job 124 with dependency on previous job
jid124=$(sbatch --parsable --dependency=afterok:$jid123 batchscripts/124_submit_iteration21_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid124"

# Submit job 125 with dependency on previous job
jid125=$(sbatch --parsable --dependency=afterok:$jid124 batchscripts/125_submit_iteration22_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid125"

# Submit job 126 with dependency on previous job
jid126=$(sbatch --parsable --dependency=afterok:$jid125 batchscripts/126_submit_iteration23_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid126"

# Submit job 127 with dependency on previous job
jid127=$(sbatch --parsable --dependency=afterok:$jid126 batchscripts/127_submit_iteration24_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid127"

# Submit job 128 with dependency on previous job
jid128=$(sbatch --parsable --dependency=afterok:$jid127 batchscripts/128_submit_iteration25_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid128"

# Submit job 129 with dependency on previous job
jid129=$(sbatch --parsable --dependency=afterok:$jid128 batchscripts/129_submit_iteration26_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid129"

# Submit job 130 with dependency on previous job
jid130=$(sbatch --parsable --dependency=afterok:$jid129 batchscripts/130_submit_iteration27_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid130"

# Submit job 131 with dependency on previous job
jid131=$(sbatch --parsable --dependency=afterok:$jid130 batchscripts/131_submit_iteration28_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid131"

# Submit job 132 with dependency on previous job
jid132=$(sbatch --parsable --dependency=afterok:$jid131 batchscripts/132_submit_iteration29_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid132"

# Submit job 133 with dependency on previous job
jid133=$(sbatch --parsable --dependency=afterok:$jid132 batchscripts/133_submit_iteration30_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid133"

# Submit job 134 with dependency on previous job
jid134=$(sbatch --parsable --dependency=afterok:$jid133 batchscripts/134_submit_iteration31_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid134"

# Submit job 135 with dependency on previous job
jid135=$(sbatch --parsable --dependency=afterok:$jid134 batchscripts/135_submit_iteration32_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid135"

# Submit job 136 with dependency on previous job
jid136=$(sbatch --parsable --dependency=afterok:$jid135 batchscripts/136_submit_iteration33_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid136"

# Submit job 137 with dependency on previous job
jid137=$(sbatch --parsable --dependency=afterok:$jid136 batchscripts/137_submit_iteration34_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid137"

# Submit job 138 with dependency on previous job
jid138=$(sbatch --parsable --dependency=afterok:$jid137 batchscripts/138_submit_iteration35_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid138"

# Submit job 139 with dependency on previous job
jid139=$(sbatch --parsable --dependency=afterok:$jid138 batchscripts/139_submit_iteration36_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid139"

# Submit job 140 with dependency on previous job
jid140=$(sbatch --parsable --dependency=afterok:$jid139 batchscripts/140_submit_iteration37_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid140"

# Submit job 141 with dependency on previous job
jid141=$(sbatch --parsable --dependency=afterok:$jid140 batchscripts/141_submit_iteration38_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid141"

# Submit job 142 with dependency on previous job
jid142=$(sbatch --parsable --dependency=afterok:$jid141 batchscripts/142_submit_iteration39_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid142"

# Submit job 143 with dependency on previous job
jid143=$(sbatch --parsable --dependency=afterok:$jid142 batchscripts/143_submit_iteration40_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid143"

# Submit job 144 with dependency on previous job
jid144=$(sbatch --parsable --dependency=afterok:$jid143 batchscripts/144_submit_iteration41_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid144"

# Submit job 145 with dependency on previous job
jid145=$(sbatch --parsable --dependency=afterok:$jid144 batchscripts/145_submit_iteration42_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid145"

# Submit job 146 with dependency on previous job
jid146=$(sbatch --parsable --dependency=afterok:$jid145 batchscripts/146_submit_iteration43_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid146"

# Submit job 147 with dependency on previous job
jid147=$(sbatch --parsable --dependency=afterok:$jid146 batchscripts/147_submit_iteration44_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid147"

# Submit job 148 with dependency on previous job
jid148=$(sbatch --parsable --dependency=afterok:$jid147 batchscripts/148_submit_iteration45_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid148"

# Submit job 149 with dependency on previous job
jid149=$(sbatch --parsable --dependency=afterok:$jid148 batchscripts/149_submit_iteration46_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid149"

# Submit job 150 with dependency on previous job
jid150=$(sbatch --parsable --dependency=afterok:$jid149 batchscripts/150_submit_iteration47_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid150"

# Submit job 151 with dependency on previous job
jid151=$(sbatch --parsable --dependency=afterok:$jid150 batchscripts/151_submit_iteration48_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid151"

# Submit job 152 with dependency on previous job
jid152=$(sbatch --parsable --dependency=afterok:$jid151 batchscripts/152_submit_iteration49_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid152"

# Submit job 153 with dependency on previous job
jid153=$(sbatch --parsable --dependency=afterok:$jid152 batchscripts/153_submit_iteration50_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid153"

# Submit job 154 with dependency on previous job
jid154=$(sbatch --parsable --dependency=afterok:$jid153 batchscripts/154_submit_iteration51_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid154"

# Submit job 155 with dependency on previous job
jid155=$(sbatch --parsable --dependency=afterok:$jid154 batchscripts/155_submit_iteration52_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid155"

# Submit job 156 with dependency on previous job
jid156=$(sbatch --parsable --dependency=afterok:$jid155 batchscripts/156_submit_iteration53_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid156"

# Submit job 157 with dependency on previous job
jid157=$(sbatch --parsable --dependency=afterok:$jid156 batchscripts/157_submit_iteration54_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid157"

# Submit job 158 with dependency on previous job
jid158=$(sbatch --parsable --dependency=afterok:$jid157 batchscripts/158_submit_iteration55_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid158"

# Submit job 159 with dependency on previous job
jid159=$(sbatch --parsable --dependency=afterok:$jid158 batchscripts/159_submit_iteration56_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid159"

# Submit job 160 with dependency on previous job
jid160=$(sbatch --parsable --dependency=afterok:$jid159 batchscripts/160_submit_iteration57_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid160"

# Submit job 161 with dependency on previous job
jid161=$(sbatch --parsable --dependency=afterok:$jid160 batchscripts/161_submit_iteration58_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid161"

# Submit job 162 with dependency on previous job
jid162=$(sbatch --parsable --dependency=afterok:$jid161 batchscripts/162_submit_iteration59_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid162"

# Submit job 163 with dependency on previous job
jid163=$(sbatch --parsable --dependency=afterok:$jid162 batchscripts/163_submit_iteration60_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid163"

# Submit job 164 with dependency on previous job
jid164=$(sbatch --parsable --dependency=afterok:$jid163 batchscripts/164_submit_iteration61_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid164"

# Submit job 165 with dependency on previous job
jid165=$(sbatch --parsable --dependency=afterok:$jid164 batchscripts/165_submit_iteration62_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid165"

# Submit job 166 with dependency on previous job
jid166=$(sbatch --parsable --dependency=afterok:$jid165 batchscripts/166_submit_iteration63_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid166"

# Submit job 167 with dependency on previous job
jid167=$(sbatch --parsable --dependency=afterok:$jid166 batchscripts/167_submit_iteration64_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid167"

# Submit job 168 with dependency on previous job
jid168=$(sbatch --parsable --dependency=afterok:$jid167 batchscripts/168_submit_iteration65_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid168"

# Submit job 169 with dependency on previous job
jid169=$(sbatch --parsable --dependency=afterok:$jid168 batchscripts/169_submit_iteration66_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid169"

# Submit job 170 with dependency on previous job
jid170=$(sbatch --parsable --dependency=afterok:$jid169 batchscripts/170_submit_iteration67_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid170"

# Submit job 171 with dependency on previous job
jid171=$(sbatch --parsable --dependency=afterok:$jid170 batchscripts/171_submit_iteration68_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid171"

# Submit job 172 with dependency on previous job
jid172=$(sbatch --parsable --dependency=afterok:$jid171 batchscripts/172_submit_iteration69_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid172"

# Submit job 173 with dependency on previous job
jid173=$(sbatch --parsable --dependency=afterok:$jid172 batchscripts/173_submit_iteration70_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid173"

# Submit job 174 with dependency on previous job
jid174=$(sbatch --parsable --dependency=afterok:$jid173 batchscripts/174_submit_iteration71_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid174"

# Submit job 175 with dependency on previous job
jid175=$(sbatch --parsable --dependency=afterok:$jid174 batchscripts/175_submit_iteration72_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid175"

# Submit job 176 with dependency on previous job
jid176=$(sbatch --parsable --dependency=afterok:$jid175 batchscripts/176_submit_iteration73_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid176"

# Submit job 177 with dependency on previous job
jid177=$(sbatch --parsable --dependency=afterok:$jid176 batchscripts/177_submit_iteration74_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid177"

# Submit job 178 with dependency on previous job
jid178=$(sbatch --parsable --dependency=afterok:$jid177 batchscripts/178_submit_iteration75_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid178"

# Submit job 179 with dependency on previous job
jid179=$(sbatch --parsable --dependency=afterok:$jid178 batchscripts/179_submit_iteration76_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid179"

# Submit job 180 with dependency on previous job
jid180=$(sbatch --parsable --dependency=afterok:$jid179 batchscripts/180_submit_iteration77_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid180"

# Submit job 181 with dependency on previous job
jid181=$(sbatch --parsable --dependency=afterok:$jid180 batchscripts/181_submit_iteration78_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid181"

# Submit job 182 with dependency on previous job
jid182=$(sbatch --parsable --dependency=afterok:$jid181 batchscripts/182_submit_iteration79_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid182"

# Submit job 183 with dependency on previous job
jid183=$(sbatch --parsable --dependency=afterok:$jid182 batchscripts/183_submit_iteration80_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid183"

# Submit job 184 with dependency on previous job
jid184=$(sbatch --parsable --dependency=afterok:$jid183 batchscripts/184_submit_iteration81_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid184"

# Submit job 185 with dependency on previous job
jid185=$(sbatch --parsable --dependency=afterok:$jid184 batchscripts/185_submit_iteration82_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid185"

# Submit job 186 with dependency on previous job
jid186=$(sbatch --parsable --dependency=afterok:$jid185 batchscripts/186_submit_iteration83_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid186"

# Submit job 187 with dependency on previous job
jid187=$(sbatch --parsable --dependency=afterok:$jid186 batchscripts/187_submit_iteration84_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid187"

# Submit job 188 with dependency on previous job
jid188=$(sbatch --parsable --dependency=afterok:$jid187 batchscripts/188_submit_iteration85_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid188"

# Submit job 189 with dependency on previous job
jid189=$(sbatch --parsable --dependency=afterok:$jid188 batchscripts/189_submit_iteration86_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid189"

# Submit job 190 with dependency on previous job
jid190=$(sbatch --parsable --dependency=afterok:$jid189 batchscripts/190_submit_iteration87_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid190"

# Submit job 191 with dependency on previous job
jid191=$(sbatch --parsable --dependency=afterok:$jid190 batchscripts/191_submit_iteration88_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid191"

# Submit job 192 with dependency on previous job
jid192=$(sbatch --parsable --dependency=afterok:$jid191 batchscripts/192_submit_iteration89_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid192"

# Submit job 193 with dependency on previous job
jid193=$(sbatch --parsable --dependency=afterok:$jid192 batchscripts/193_submit_iteration90_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid193"

# Submit job 194 with dependency on previous job
jid194=$(sbatch --parsable --dependency=afterok:$jid193 batchscripts/194_submit_iteration91_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid194"

# Submit job 195 with dependency on previous job
jid195=$(sbatch --parsable --dependency=afterok:$jid194 batchscripts/195_submit_iteration92_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid195"

# Submit job 196 with dependency on previous job
jid196=$(sbatch --parsable --dependency=afterok:$jid195 batchscripts/196_submit_iteration93_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid196"

# Submit job 197 with dependency on previous job
jid197=$(sbatch --parsable --dependency=afterok:$jid196 batchscripts/197_submit_iteration94_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid197"

# Submit job 198 with dependency on previous job
jid198=$(sbatch --parsable --dependency=afterok:$jid197 batchscripts/198_submit_iteration95_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid198"

# Submit job 199 with dependency on previous job
jid199=$(sbatch --parsable --dependency=afterok:$jid198 batchscripts/199_submit_iteration96_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid199"

# Submit job 200 with dependency on previous job
jid200=$(sbatch --parsable --dependency=afterok:$jid199 batchscripts/200_submit_iteration97_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid200"

# Submit job 201 with dependency on previous job
jid201=$(sbatch --parsable --dependency=afterok:$jid200 batchscripts/201_submit_iteration98_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid201"

# Submit job 202 with dependency on previous job
jid202=$(sbatch --parsable --dependency=afterok:$jid201 batchscripts/202_submit_iteration99_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid202"

# Submit job 203 with dependency on previous job
jid203=$(sbatch --parsable --dependency=afterok:$jid202 batchscripts/203_submit_iteration100_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid203"

# Submit the first job and capture its Job ID
jid204=$(sbatch --parsable batchscripts/204_submit_iteration0_forallparticles.sh)
echo "Submitted job for runscript: $jid204"

# Submit the first job and capture its Job ID
jid205=$(sbatch --parsable batchscripts/205_submit_iteration0_forallparticles.sh)
echo "Submitted job for runscript: $jid205"

# Submit job 206 with dependency on previous job
jid206=$(sbatch --parsable --dependency=afterok:$jid205 batchscripts/206_submit_iteration1_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid206"

# Submit job 207 with dependency on previous job
jid207=$(sbatch --parsable --dependency=afterok:$jid206 batchscripts/207_submit_iteration2_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid207"

# Submit job 208 with dependency on previous job
jid208=$(sbatch --parsable --dependency=afterok:$jid207 batchscripts/208_submit_iteration3_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid208"

# Submit job 209 with dependency on previous job
jid209=$(sbatch --parsable --dependency=afterok:$jid208 batchscripts/209_submit_iteration4_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid209"

# Submit job 210 with dependency on previous job
jid210=$(sbatch --parsable --dependency=afterok:$jid209 batchscripts/210_submit_iteration5_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid210"

# Submit job 211 with dependency on previous job
jid211=$(sbatch --parsable --dependency=afterok:$jid210 batchscripts/211_submit_iteration6_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid211"

# Submit job 212 with dependency on previous job
jid212=$(sbatch --parsable --dependency=afterok:$jid211 batchscripts/212_submit_iteration7_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid212"

# Submit job 213 with dependency on previous job
jid213=$(sbatch --parsable --dependency=afterok:$jid212 batchscripts/213_submit_iteration8_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid213"

# Submit job 214 with dependency on previous job
jid214=$(sbatch --parsable --dependency=afterok:$jid213 batchscripts/214_submit_iteration9_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid214"

# Submit job 215 with dependency on previous job
jid215=$(sbatch --parsable --dependency=afterok:$jid214 batchscripts/215_submit_iteration10_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid215"

# Submit job 216 with dependency on previous job
jid216=$(sbatch --parsable --dependency=afterok:$jid215 batchscripts/216_submit_iteration11_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid216"

# Submit job 217 with dependency on previous job
jid217=$(sbatch --parsable --dependency=afterok:$jid216 batchscripts/217_submit_iteration12_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid217"

# Submit job 218 with dependency on previous job
jid218=$(sbatch --parsable --dependency=afterok:$jid217 batchscripts/218_submit_iteration13_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid218"

# Submit job 219 with dependency on previous job
jid219=$(sbatch --parsable --dependency=afterok:$jid218 batchscripts/219_submit_iteration14_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid219"

# Submit job 220 with dependency on previous job
jid220=$(sbatch --parsable --dependency=afterok:$jid219 batchscripts/220_submit_iteration15_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid220"

# Submit job 221 with dependency on previous job
jid221=$(sbatch --parsable --dependency=afterok:$jid220 batchscripts/221_submit_iteration16_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid221"

# Submit job 222 with dependency on previous job
jid222=$(sbatch --parsable --dependency=afterok:$jid221 batchscripts/222_submit_iteration17_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid222"

# Submit job 223 with dependency on previous job
jid223=$(sbatch --parsable --dependency=afterok:$jid222 batchscripts/223_submit_iteration18_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid223"

# Submit job 224 with dependency on previous job
jid224=$(sbatch --parsable --dependency=afterok:$jid223 batchscripts/224_submit_iteration19_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid224"

# Submit job 225 with dependency on previous job
jid225=$(sbatch --parsable --dependency=afterok:$jid224 batchscripts/225_submit_iteration20_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid225"

# Submit job 226 with dependency on previous job
jid226=$(sbatch --parsable --dependency=afterok:$jid225 batchscripts/226_submit_iteration21_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid226"

# Submit job 227 with dependency on previous job
jid227=$(sbatch --parsable --dependency=afterok:$jid226 batchscripts/227_submit_iteration22_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid227"

# Submit job 228 with dependency on previous job
jid228=$(sbatch --parsable --dependency=afterok:$jid227 batchscripts/228_submit_iteration23_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid228"

# Submit job 229 with dependency on previous job
jid229=$(sbatch --parsable --dependency=afterok:$jid228 batchscripts/229_submit_iteration24_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid229"

# Submit job 230 with dependency on previous job
jid230=$(sbatch --parsable --dependency=afterok:$jid229 batchscripts/230_submit_iteration25_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid230"

# Submit job 231 with dependency on previous job
jid231=$(sbatch --parsable --dependency=afterok:$jid230 batchscripts/231_submit_iteration26_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid231"

# Submit job 232 with dependency on previous job
jid232=$(sbatch --parsable --dependency=afterok:$jid231 batchscripts/232_submit_iteration27_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid232"

# Submit job 233 with dependency on previous job
jid233=$(sbatch --parsable --dependency=afterok:$jid232 batchscripts/233_submit_iteration28_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid233"

# Submit job 234 with dependency on previous job
jid234=$(sbatch --parsable --dependency=afterok:$jid233 batchscripts/234_submit_iteration29_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid234"

# Submit job 235 with dependency on previous job
jid235=$(sbatch --parsable --dependency=afterok:$jid234 batchscripts/235_submit_iteration30_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid235"

# Submit job 236 with dependency on previous job
jid236=$(sbatch --parsable --dependency=afterok:$jid235 batchscripts/236_submit_iteration31_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid236"

# Submit job 237 with dependency on previous job
jid237=$(sbatch --parsable --dependency=afterok:$jid236 batchscripts/237_submit_iteration32_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid237"

# Submit job 238 with dependency on previous job
jid238=$(sbatch --parsable --dependency=afterok:$jid237 batchscripts/238_submit_iteration33_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid238"

# Submit job 239 with dependency on previous job
jid239=$(sbatch --parsable --dependency=afterok:$jid238 batchscripts/239_submit_iteration34_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid239"

# Submit job 240 with dependency on previous job
jid240=$(sbatch --parsable --dependency=afterok:$jid239 batchscripts/240_submit_iteration35_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid240"

# Submit job 241 with dependency on previous job
jid241=$(sbatch --parsable --dependency=afterok:$jid240 batchscripts/241_submit_iteration36_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid241"

# Submit job 242 with dependency on previous job
jid242=$(sbatch --parsable --dependency=afterok:$jid241 batchscripts/242_submit_iteration37_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid242"

# Submit job 243 with dependency on previous job
jid243=$(sbatch --parsable --dependency=afterok:$jid242 batchscripts/243_submit_iteration38_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid243"

# Submit job 244 with dependency on previous job
jid244=$(sbatch --parsable --dependency=afterok:$jid243 batchscripts/244_submit_iteration39_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid244"

# Submit job 245 with dependency on previous job
jid245=$(sbatch --parsable --dependency=afterok:$jid244 batchscripts/245_submit_iteration40_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid245"

# Submit job 246 with dependency on previous job
jid246=$(sbatch --parsable --dependency=afterok:$jid245 batchscripts/246_submit_iteration41_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid246"

# Submit job 247 with dependency on previous job
jid247=$(sbatch --parsable --dependency=afterok:$jid246 batchscripts/247_submit_iteration42_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid247"

# Submit job 248 with dependency on previous job
jid248=$(sbatch --parsable --dependency=afterok:$jid247 batchscripts/248_submit_iteration43_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid248"

# Submit job 249 with dependency on previous job
jid249=$(sbatch --parsable --dependency=afterok:$jid248 batchscripts/249_submit_iteration44_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid249"

# Submit job 250 with dependency on previous job
jid250=$(sbatch --parsable --dependency=afterok:$jid249 batchscripts/250_submit_iteration45_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid250"

# Submit job 251 with dependency on previous job
jid251=$(sbatch --parsable --dependency=afterok:$jid250 batchscripts/251_submit_iteration46_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid251"

# Submit job 252 with dependency on previous job
jid252=$(sbatch --parsable --dependency=afterok:$jid251 batchscripts/252_submit_iteration47_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid252"

# Submit job 253 with dependency on previous job
jid253=$(sbatch --parsable --dependency=afterok:$jid252 batchscripts/253_submit_iteration48_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid253"

# Submit job 254 with dependency on previous job
jid254=$(sbatch --parsable --dependency=afterok:$jid253 batchscripts/254_submit_iteration49_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid254"

# Submit job 255 with dependency on previous job
jid255=$(sbatch --parsable --dependency=afterok:$jid254 batchscripts/255_submit_iteration50_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid255"

# Submit job 256 with dependency on previous job
jid256=$(sbatch --parsable --dependency=afterok:$jid255 batchscripts/256_submit_iteration51_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid256"

# Submit job 257 with dependency on previous job
jid257=$(sbatch --parsable --dependency=afterok:$jid256 batchscripts/257_submit_iteration52_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid257"

# Submit job 258 with dependency on previous job
jid258=$(sbatch --parsable --dependency=afterok:$jid257 batchscripts/258_submit_iteration53_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid258"

# Submit job 259 with dependency on previous job
jid259=$(sbatch --parsable --dependency=afterok:$jid258 batchscripts/259_submit_iteration54_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid259"

# Submit job 260 with dependency on previous job
jid260=$(sbatch --parsable --dependency=afterok:$jid259 batchscripts/260_submit_iteration55_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid260"

# Submit job 261 with dependency on previous job
jid261=$(sbatch --parsable --dependency=afterok:$jid260 batchscripts/261_submit_iteration56_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid261"

# Submit job 262 with dependency on previous job
jid262=$(sbatch --parsable --dependency=afterok:$jid261 batchscripts/262_submit_iteration57_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid262"

# Submit job 263 with dependency on previous job
jid263=$(sbatch --parsable --dependency=afterok:$jid262 batchscripts/263_submit_iteration58_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid263"

# Submit job 264 with dependency on previous job
jid264=$(sbatch --parsable --dependency=afterok:$jid263 batchscripts/264_submit_iteration59_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid264"

# Submit job 265 with dependency on previous job
jid265=$(sbatch --parsable --dependency=afterok:$jid264 batchscripts/265_submit_iteration60_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid265"

# Submit job 266 with dependency on previous job
jid266=$(sbatch --parsable --dependency=afterok:$jid265 batchscripts/266_submit_iteration61_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid266"

# Submit job 267 with dependency on previous job
jid267=$(sbatch --parsable --dependency=afterok:$jid266 batchscripts/267_submit_iteration62_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid267"

# Submit job 268 with dependency on previous job
jid268=$(sbatch --parsable --dependency=afterok:$jid267 batchscripts/268_submit_iteration63_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid268"

# Submit job 269 with dependency on previous job
jid269=$(sbatch --parsable --dependency=afterok:$jid268 batchscripts/269_submit_iteration64_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid269"

# Submit job 270 with dependency on previous job
jid270=$(sbatch --parsable --dependency=afterok:$jid269 batchscripts/270_submit_iteration65_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid270"

# Submit job 271 with dependency on previous job
jid271=$(sbatch --parsable --dependency=afterok:$jid270 batchscripts/271_submit_iteration66_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid271"

# Submit job 272 with dependency on previous job
jid272=$(sbatch --parsable --dependency=afterok:$jid271 batchscripts/272_submit_iteration67_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid272"

# Submit job 273 with dependency on previous job
jid273=$(sbatch --parsable --dependency=afterok:$jid272 batchscripts/273_submit_iteration68_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid273"

# Submit job 274 with dependency on previous job
jid274=$(sbatch --parsable --dependency=afterok:$jid273 batchscripts/274_submit_iteration69_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid274"

# Submit job 275 with dependency on previous job
jid275=$(sbatch --parsable --dependency=afterok:$jid274 batchscripts/275_submit_iteration70_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid275"

# Submit job 276 with dependency on previous job
jid276=$(sbatch --parsable --dependency=afterok:$jid275 batchscripts/276_submit_iteration71_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid276"

# Submit job 277 with dependency on previous job
jid277=$(sbatch --parsable --dependency=afterok:$jid276 batchscripts/277_submit_iteration72_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid277"

# Submit job 278 with dependency on previous job
jid278=$(sbatch --parsable --dependency=afterok:$jid277 batchscripts/278_submit_iteration73_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid278"

# Submit job 279 with dependency on previous job
jid279=$(sbatch --parsable --dependency=afterok:$jid278 batchscripts/279_submit_iteration74_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid279"

# Submit job 280 with dependency on previous job
jid280=$(sbatch --parsable --dependency=afterok:$jid279 batchscripts/280_submit_iteration75_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid280"

# Submit job 281 with dependency on previous job
jid281=$(sbatch --parsable --dependency=afterok:$jid280 batchscripts/281_submit_iteration76_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid281"

# Submit job 282 with dependency on previous job
jid282=$(sbatch --parsable --dependency=afterok:$jid281 batchscripts/282_submit_iteration77_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid282"

# Submit job 283 with dependency on previous job
jid283=$(sbatch --parsable --dependency=afterok:$jid282 batchscripts/283_submit_iteration78_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid283"

# Submit job 284 with dependency on previous job
jid284=$(sbatch --parsable --dependency=afterok:$jid283 batchscripts/284_submit_iteration79_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid284"

# Submit job 285 with dependency on previous job
jid285=$(sbatch --parsable --dependency=afterok:$jid284 batchscripts/285_submit_iteration80_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid285"

# Submit job 286 with dependency on previous job
jid286=$(sbatch --parsable --dependency=afterok:$jid285 batchscripts/286_submit_iteration81_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid286"

# Submit job 287 with dependency on previous job
jid287=$(sbatch --parsable --dependency=afterok:$jid286 batchscripts/287_submit_iteration82_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid287"

# Submit job 288 with dependency on previous job
jid288=$(sbatch --parsable --dependency=afterok:$jid287 batchscripts/288_submit_iteration83_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid288"

# Submit job 289 with dependency on previous job
jid289=$(sbatch --parsable --dependency=afterok:$jid288 batchscripts/289_submit_iteration84_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid289"

# Submit job 290 with dependency on previous job
jid290=$(sbatch --parsable --dependency=afterok:$jid289 batchscripts/290_submit_iteration85_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid290"

# Submit job 291 with dependency on previous job
jid291=$(sbatch --parsable --dependency=afterok:$jid290 batchscripts/291_submit_iteration86_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid291"

# Submit job 292 with dependency on previous job
jid292=$(sbatch --parsable --dependency=afterok:$jid291 batchscripts/292_submit_iteration87_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid292"

# Submit job 293 with dependency on previous job
jid293=$(sbatch --parsable --dependency=afterok:$jid292 batchscripts/293_submit_iteration88_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid293"

# Submit job 294 with dependency on previous job
jid294=$(sbatch --parsable --dependency=afterok:$jid293 batchscripts/294_submit_iteration89_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid294"

# Submit job 295 with dependency on previous job
jid295=$(sbatch --parsable --dependency=afterok:$jid294 batchscripts/295_submit_iteration90_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid295"

# Submit job 296 with dependency on previous job
jid296=$(sbatch --parsable --dependency=afterok:$jid295 batchscripts/296_submit_iteration91_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid296"

# Submit job 297 with dependency on previous job
jid297=$(sbatch --parsable --dependency=afterok:$jid296 batchscripts/297_submit_iteration92_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid297"

# Submit job 298 with dependency on previous job
jid298=$(sbatch --parsable --dependency=afterok:$jid297 batchscripts/298_submit_iteration93_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid298"

# Submit job 299 with dependency on previous job
jid299=$(sbatch --parsable --dependency=afterok:$jid298 batchscripts/299_submit_iteration94_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid299"

# Submit job 300 with dependency on previous job
jid300=$(sbatch --parsable --dependency=afterok:$jid299 batchscripts/300_submit_iteration95_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid300"

# Submit job 301 with dependency on previous job
jid301=$(sbatch --parsable --dependency=afterok:$jid300 batchscripts/301_submit_iteration96_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid301"

# Submit job 302 with dependency on previous job
jid302=$(sbatch --parsable --dependency=afterok:$jid301 batchscripts/302_submit_iteration97_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid302"

# Submit job 303 with dependency on previous job
jid303=$(sbatch --parsable --dependency=afterok:$jid302 batchscripts/303_submit_iteration98_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid303"

# Submit job 304 with dependency on previous job
jid304=$(sbatch --parsable --dependency=afterok:$jid303 batchscripts/304_submit_iteration99_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid304"

# Submit job 305 with dependency on previous job
jid305=$(sbatch --parsable --dependency=afterok:$jid304 batchscripts/305_submit_iteration100_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid305"

