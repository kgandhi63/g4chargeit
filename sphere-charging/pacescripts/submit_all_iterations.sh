#!/bin/bash

# Submit the first job and capture its Job ID
jid0=$(sbatch --parsable batchscripts/00_submit_iteration0_foronlysolarwind.sh)
echo "Submitted job for runscript: $jid0"

# Submit job 1 with dependency on previous job
jid1=$(sbatch --parsable --dependency=afterok:$jid0 batchscripts/01_submit_iteration1_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid1"

# Submit job 2 with dependency on previous job
jid2=$(sbatch --parsable --dependency=afterok:$jid1 batchscripts/02_submit_iteration2_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid2"

# Submit job 3 with dependency on previous job
jid3=$(sbatch --parsable --dependency=afterok:$jid2 batchscripts/03_submit_iteration3_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid3"

# Submit job 4 with dependency on previous job
jid4=$(sbatch --parsable --dependency=afterok:$jid3 batchscripts/04_submit_iteration4_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid4"

# Submit job 5 with dependency on previous job
jid5=$(sbatch --parsable --dependency=afterok:$jid4 batchscripts/05_submit_iteration5_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid5"

# Submit job 6 with dependency on previous job
jid6=$(sbatch --parsable --dependency=afterok:$jid5 batchscripts/06_submit_iteration6_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid6"

# Submit job 7 with dependency on previous job
jid7=$(sbatch --parsable --dependency=afterok:$jid6 batchscripts/07_submit_iteration7_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid7"

# Submit job 8 with dependency on previous job
jid8=$(sbatch --parsable --dependency=afterok:$jid7 batchscripts/08_submit_iteration8_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid8"

# Submit job 9 with dependency on previous job
jid9=$(sbatch --parsable --dependency=afterok:$jid8 batchscripts/09_submit_iteration9_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid9"

# Submit job 10 with dependency on previous job
jid10=$(sbatch --parsable --dependency=afterok:$jid9 batchscripts/100_submit_iteration182_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid10"

# Submit job 11 with dependency on previous job
jid11=$(sbatch --parsable --dependency=afterok:$jid10 batchscripts/101_submit_iteration183_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid11"

# Submit job 12 with dependency on previous job
jid12=$(sbatch --parsable --dependency=afterok:$jid11 batchscripts/102_submit_iteration184_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid12"

# Submit job 13 with dependency on previous job
jid13=$(sbatch --parsable --dependency=afterok:$jid12 batchscripts/103_submit_iteration185_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid13"

# Submit job 14 with dependency on previous job
jid14=$(sbatch --parsable --dependency=afterok:$jid13 batchscripts/104_submit_iteration186_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid14"

# Submit job 15 with dependency on previous job
jid15=$(sbatch --parsable --dependency=afterok:$jid14 batchscripts/105_submit_iteration187_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid15"

# Submit job 16 with dependency on previous job
jid16=$(sbatch --parsable --dependency=afterok:$jid15 batchscripts/106_submit_iteration188_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid16"

# Submit job 17 with dependency on previous job
jid17=$(sbatch --parsable --dependency=afterok:$jid16 batchscripts/107_submit_iteration189_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid17"

# Submit job 18 with dependency on previous job
jid18=$(sbatch --parsable --dependency=afterok:$jid17 batchscripts/108_submit_iteration18_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid18"

# Submit job 19 with dependency on previous job
jid19=$(sbatch --parsable --dependency=afterok:$jid18 batchscripts/109_submit_iteration190_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid19"

# Submit job 20 with dependency on previous job
jid20=$(sbatch --parsable --dependency=afterok:$jid19 batchscripts/10_submit_iteration100_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid20"

# Submit job 21 with dependency on previous job
jid21=$(sbatch --parsable --dependency=afterok:$jid20 batchscripts/110_submit_iteration191_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid21"

# Submit job 22 with dependency on previous job
jid22=$(sbatch --parsable --dependency=afterok:$jid21 batchscripts/111_submit_iteration192_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid22"

# Submit job 23 with dependency on previous job
jid23=$(sbatch --parsable --dependency=afterok:$jid22 batchscripts/112_submit_iteration193_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid23"

# Submit job 24 with dependency on previous job
jid24=$(sbatch --parsable --dependency=afterok:$jid23 batchscripts/113_submit_iteration194_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid24"

# Submit job 25 with dependency on previous job
jid25=$(sbatch --parsable --dependency=afterok:$jid24 batchscripts/114_submit_iteration195_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid25"

# Submit job 26 with dependency on previous job
jid26=$(sbatch --parsable --dependency=afterok:$jid25 batchscripts/115_submit_iteration196_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid26"

# Submit job 27 with dependency on previous job
jid27=$(sbatch --parsable --dependency=afterok:$jid26 batchscripts/116_submit_iteration197_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid27"

# Submit job 28 with dependency on previous job
jid28=$(sbatch --parsable --dependency=afterok:$jid27 batchscripts/117_submit_iteration198_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid28"

# Submit job 29 with dependency on previous job
jid29=$(sbatch --parsable --dependency=afterok:$jid28 batchscripts/118_submit_iteration199_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid29"

# Submit job 30 with dependency on previous job
jid30=$(sbatch --parsable --dependency=afterok:$jid29 batchscripts/119_submit_iteration19_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid30"

# Submit job 31 with dependency on previous job
jid31=$(sbatch --parsable --dependency=afterok:$jid30 batchscripts/11_submit_iteration101_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid31"

# Submit job 32 with dependency on previous job
jid32=$(sbatch --parsable --dependency=afterok:$jid31 batchscripts/120_submit_iteration20_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid32"

# Submit job 33 with dependency on previous job
jid33=$(sbatch --parsable --dependency=afterok:$jid32 batchscripts/121_submit_iteration21_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid33"

# Submit job 34 with dependency on previous job
jid34=$(sbatch --parsable --dependency=afterok:$jid33 batchscripts/122_submit_iteration22_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid34"

# Submit job 35 with dependency on previous job
jid35=$(sbatch --parsable --dependency=afterok:$jid34 batchscripts/123_submit_iteration23_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid35"

# Submit job 36 with dependency on previous job
jid36=$(sbatch --parsable --dependency=afterok:$jid35 batchscripts/124_submit_iteration24_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid36"

# Submit job 37 with dependency on previous job
jid37=$(sbatch --parsable --dependency=afterok:$jid36 batchscripts/125_submit_iteration25_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid37"

# Submit job 38 with dependency on previous job
jid38=$(sbatch --parsable --dependency=afterok:$jid37 batchscripts/126_submit_iteration26_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid38"

# Submit job 39 with dependency on previous job
jid39=$(sbatch --parsable --dependency=afterok:$jid38 batchscripts/127_submit_iteration27_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid39"

# Submit job 40 with dependency on previous job
jid40=$(sbatch --parsable --dependency=afterok:$jid39 batchscripts/128_submit_iteration28_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid40"

# Submit job 41 with dependency on previous job
jid41=$(sbatch --parsable --dependency=afterok:$jid40 batchscripts/129_submit_iteration29_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid41"

# Submit job 42 with dependency on previous job
jid42=$(sbatch --parsable --dependency=afterok:$jid41 batchscripts/12_submit_iteration102_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid42"

# Submit job 43 with dependency on previous job
jid43=$(sbatch --parsable --dependency=afterok:$jid42 batchscripts/130_submit_iteration30_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid43"

# Submit job 44 with dependency on previous job
jid44=$(sbatch --parsable --dependency=afterok:$jid43 batchscripts/131_submit_iteration31_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid44"

# Submit job 45 with dependency on previous job
jid45=$(sbatch --parsable --dependency=afterok:$jid44 batchscripts/132_submit_iteration32_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid45"

# Submit job 46 with dependency on previous job
jid46=$(sbatch --parsable --dependency=afterok:$jid45 batchscripts/133_submit_iteration33_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid46"

# Submit job 47 with dependency on previous job
jid47=$(sbatch --parsable --dependency=afterok:$jid46 batchscripts/134_submit_iteration34_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid47"

# Submit job 48 with dependency on previous job
jid48=$(sbatch --parsable --dependency=afterok:$jid47 batchscripts/135_submit_iteration35_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid48"

# Submit job 49 with dependency on previous job
jid49=$(sbatch --parsable --dependency=afterok:$jid48 batchscripts/136_submit_iteration36_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid49"

# Submit job 50 with dependency on previous job
jid50=$(sbatch --parsable --dependency=afterok:$jid49 batchscripts/137_submit_iteration37_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid50"

# Submit job 51 with dependency on previous job
jid51=$(sbatch --parsable --dependency=afterok:$jid50 batchscripts/138_submit_iteration38_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid51"

# Submit job 52 with dependency on previous job
jid52=$(sbatch --parsable --dependency=afterok:$jid51 batchscripts/139_submit_iteration39_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid52"

# Submit job 53 with dependency on previous job
jid53=$(sbatch --parsable --dependency=afterok:$jid52 batchscripts/13_submit_iteration103_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid53"

# Submit job 54 with dependency on previous job
jid54=$(sbatch --parsable --dependency=afterok:$jid53 batchscripts/140_submit_iteration40_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid54"

# Submit job 55 with dependency on previous job
jid55=$(sbatch --parsable --dependency=afterok:$jid54 batchscripts/141_submit_iteration41_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid55"

# Submit job 56 with dependency on previous job
jid56=$(sbatch --parsable --dependency=afterok:$jid55 batchscripts/142_submit_iteration42_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid56"

# Submit job 57 with dependency on previous job
jid57=$(sbatch --parsable --dependency=afterok:$jid56 batchscripts/143_submit_iteration43_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid57"

# Submit job 58 with dependency on previous job
jid58=$(sbatch --parsable --dependency=afterok:$jid57 batchscripts/144_submit_iteration44_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid58"

# Submit job 59 with dependency on previous job
jid59=$(sbatch --parsable --dependency=afterok:$jid58 batchscripts/145_submit_iteration45_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid59"

# Submit job 60 with dependency on previous job
jid60=$(sbatch --parsable --dependency=afterok:$jid59 batchscripts/146_submit_iteration46_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid60"

# Submit job 61 with dependency on previous job
jid61=$(sbatch --parsable --dependency=afterok:$jid60 batchscripts/147_submit_iteration47_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid61"

# Submit job 62 with dependency on previous job
jid62=$(sbatch --parsable --dependency=afterok:$jid61 batchscripts/148_submit_iteration48_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid62"

# Submit job 63 with dependency on previous job
jid63=$(sbatch --parsable --dependency=afterok:$jid62 batchscripts/149_submit_iteration49_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid63"

# Submit job 64 with dependency on previous job
jid64=$(sbatch --parsable --dependency=afterok:$jid63 batchscripts/14_submit_iteration104_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid64"

# Submit job 65 with dependency on previous job
jid65=$(sbatch --parsable --dependency=afterok:$jid64 batchscripts/150_submit_iteration50_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid65"

# Submit job 66 with dependency on previous job
jid66=$(sbatch --parsable --dependency=afterok:$jid65 batchscripts/151_submit_iteration51_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid66"

# Submit job 67 with dependency on previous job
jid67=$(sbatch --parsable --dependency=afterok:$jid66 batchscripts/152_submit_iteration52_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid67"

# Submit job 68 with dependency on previous job
jid68=$(sbatch --parsable --dependency=afterok:$jid67 batchscripts/153_submit_iteration53_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid68"

# Submit job 69 with dependency on previous job
jid69=$(sbatch --parsable --dependency=afterok:$jid68 batchscripts/154_submit_iteration54_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid69"

# Submit job 70 with dependency on previous job
jid70=$(sbatch --parsable --dependency=afterok:$jid69 batchscripts/155_submit_iteration55_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid70"

# Submit job 71 with dependency on previous job
jid71=$(sbatch --parsable --dependency=afterok:$jid70 batchscripts/156_submit_iteration56_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid71"

# Submit job 72 with dependency on previous job
jid72=$(sbatch --parsable --dependency=afterok:$jid71 batchscripts/157_submit_iteration57_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid72"

# Submit job 73 with dependency on previous job
jid73=$(sbatch --parsable --dependency=afterok:$jid72 batchscripts/158_submit_iteration58_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid73"

# Submit job 74 with dependency on previous job
jid74=$(sbatch --parsable --dependency=afterok:$jid73 batchscripts/159_submit_iteration59_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid74"

# Submit job 75 with dependency on previous job
jid75=$(sbatch --parsable --dependency=afterok:$jid74 batchscripts/15_submit_iteration105_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid75"

# Submit job 76 with dependency on previous job
jid76=$(sbatch --parsable --dependency=afterok:$jid75 batchscripts/160_submit_iteration60_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid76"

# Submit job 77 with dependency on previous job
jid77=$(sbatch --parsable --dependency=afterok:$jid76 batchscripts/161_submit_iteration61_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid77"

# Submit job 78 with dependency on previous job
jid78=$(sbatch --parsable --dependency=afterok:$jid77 batchscripts/162_submit_iteration62_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid78"

# Submit job 79 with dependency on previous job
jid79=$(sbatch --parsable --dependency=afterok:$jid78 batchscripts/163_submit_iteration63_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid79"

# Submit job 80 with dependency on previous job
jid80=$(sbatch --parsable --dependency=afterok:$jid79 batchscripts/164_submit_iteration64_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid80"

# Submit job 81 with dependency on previous job
jid81=$(sbatch --parsable --dependency=afterok:$jid80 batchscripts/165_submit_iteration65_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid81"

# Submit job 82 with dependency on previous job
jid82=$(sbatch --parsable --dependency=afterok:$jid81 batchscripts/166_submit_iteration66_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid82"

# Submit job 83 with dependency on previous job
jid83=$(sbatch --parsable --dependency=afterok:$jid82 batchscripts/167_submit_iteration67_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid83"

# Submit job 84 with dependency on previous job
jid84=$(sbatch --parsable --dependency=afterok:$jid83 batchscripts/168_submit_iteration68_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid84"

# Submit job 85 with dependency on previous job
jid85=$(sbatch --parsable --dependency=afterok:$jid84 batchscripts/169_submit_iteration69_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid85"

# Submit job 86 with dependency on previous job
jid86=$(sbatch --parsable --dependency=afterok:$jid85 batchscripts/16_submit_iteration106_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid86"

# Submit job 87 with dependency on previous job
jid87=$(sbatch --parsable --dependency=afterok:$jid86 batchscripts/170_submit_iteration70_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid87"

# Submit job 88 with dependency on previous job
jid88=$(sbatch --parsable --dependency=afterok:$jid87 batchscripts/171_submit_iteration71_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid88"

# Submit job 89 with dependency on previous job
jid89=$(sbatch --parsable --dependency=afterok:$jid88 batchscripts/172_submit_iteration72_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid89"

# Submit job 90 with dependency on previous job
jid90=$(sbatch --parsable --dependency=afterok:$jid89 batchscripts/173_submit_iteration73_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid90"

# Submit job 91 with dependency on previous job
jid91=$(sbatch --parsable --dependency=afterok:$jid90 batchscripts/174_submit_iteration74_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid91"

# Submit job 92 with dependency on previous job
jid92=$(sbatch --parsable --dependency=afterok:$jid91 batchscripts/175_submit_iteration75_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid92"

# Submit job 93 with dependency on previous job
jid93=$(sbatch --parsable --dependency=afterok:$jid92 batchscripts/176_submit_iteration76_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid93"

# Submit job 94 with dependency on previous job
jid94=$(sbatch --parsable --dependency=afterok:$jid93 batchscripts/177_submit_iteration77_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid94"

# Submit job 95 with dependency on previous job
jid95=$(sbatch --parsable --dependency=afterok:$jid94 batchscripts/178_submit_iteration78_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid95"

# Submit job 96 with dependency on previous job
jid96=$(sbatch --parsable --dependency=afterok:$jid95 batchscripts/179_submit_iteration79_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid96"

# Submit job 97 with dependency on previous job
jid97=$(sbatch --parsable --dependency=afterok:$jid96 batchscripts/17_submit_iteration107_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid97"

# Submit job 98 with dependency on previous job
jid98=$(sbatch --parsable --dependency=afterok:$jid97 batchscripts/180_submit_iteration80_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid98"

# Submit job 99 with dependency on previous job
jid99=$(sbatch --parsable --dependency=afterok:$jid98 batchscripts/181_submit_iteration81_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid99"

# Submit job 100 with dependency on previous job
jid100=$(sbatch --parsable --dependency=afterok:$jid99 batchscripts/182_submit_iteration82_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid100"

# Submit job 101 with dependency on previous job
jid101=$(sbatch --parsable --dependency=afterok:$jid100 batchscripts/183_submit_iteration83_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid101"

# Submit job 102 with dependency on previous job
jid102=$(sbatch --parsable --dependency=afterok:$jid101 batchscripts/184_submit_iteration84_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid102"

# Submit job 103 with dependency on previous job
jid103=$(sbatch --parsable --dependency=afterok:$jid102 batchscripts/185_submit_iteration85_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid103"

# Submit job 104 with dependency on previous job
jid104=$(sbatch --parsable --dependency=afterok:$jid103 batchscripts/186_submit_iteration86_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid104"

# Submit job 105 with dependency on previous job
jid105=$(sbatch --parsable --dependency=afterok:$jid104 batchscripts/187_submit_iteration87_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid105"

# Submit job 106 with dependency on previous job
jid106=$(sbatch --parsable --dependency=afterok:$jid105 batchscripts/188_submit_iteration88_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid106"

# Submit job 107 with dependency on previous job
jid107=$(sbatch --parsable --dependency=afterok:$jid106 batchscripts/189_submit_iteration89_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid107"

# Submit job 108 with dependency on previous job
jid108=$(sbatch --parsable --dependency=afterok:$jid107 batchscripts/18_submit_iteration108_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid108"

# Submit job 109 with dependency on previous job
jid109=$(sbatch --parsable --dependency=afterok:$jid108 batchscripts/190_submit_iteration90_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid109"

# Submit job 110 with dependency on previous job
jid110=$(sbatch --parsable --dependency=afterok:$jid109 batchscripts/191_submit_iteration91_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid110"

# Submit job 111 with dependency on previous job
jid111=$(sbatch --parsable --dependency=afterok:$jid110 batchscripts/192_submit_iteration92_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid111"

# Submit job 112 with dependency on previous job
jid112=$(sbatch --parsable --dependency=afterok:$jid111 batchscripts/193_submit_iteration93_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid112"

# Submit job 113 with dependency on previous job
jid113=$(sbatch --parsable --dependency=afterok:$jid112 batchscripts/194_submit_iteration94_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid113"

# Submit job 114 with dependency on previous job
jid114=$(sbatch --parsable --dependency=afterok:$jid113 batchscripts/195_submit_iteration95_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid114"

# Submit job 115 with dependency on previous job
jid115=$(sbatch --parsable --dependency=afterok:$jid114 batchscripts/196_submit_iteration96_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid115"

# Submit job 116 with dependency on previous job
jid116=$(sbatch --parsable --dependency=afterok:$jid115 batchscripts/197_submit_iteration97_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid116"

# Submit job 117 with dependency on previous job
jid117=$(sbatch --parsable --dependency=afterok:$jid116 batchscripts/198_submit_iteration98_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid117"

# Submit job 118 with dependency on previous job
jid118=$(sbatch --parsable --dependency=afterok:$jid117 batchscripts/199_submit_iteration99_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid118"

# Submit job 119 with dependency on previous job
jid119=$(sbatch --parsable --dependency=afterok:$jid118 batchscripts/19_submit_iteration109_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid119"

# Submit the first job and capture its Job ID
jid120=$(sbatch --parsable batchscripts/200_submit_iteration0_foronlyphotoemission.sh)
echo "Submitted job for runscript: $jid120"

# Submit job 121 with dependency on previous job
jid121=$(sbatch --parsable --dependency=afterok:$jid120 batchscripts/201_submit_iteration1_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid121"

# Submit job 122 with dependency on previous job
jid122=$(sbatch --parsable --dependency=afterok:$jid121 batchscripts/202_submit_iteration2_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid122"

# Submit job 123 with dependency on previous job
jid123=$(sbatch --parsable --dependency=afterok:$jid122 batchscripts/203_submit_iteration3_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid123"

# Submit job 124 with dependency on previous job
jid124=$(sbatch --parsable --dependency=afterok:$jid123 batchscripts/204_submit_iteration4_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid124"

# Submit job 125 with dependency on previous job
jid125=$(sbatch --parsable --dependency=afterok:$jid124 batchscripts/205_submit_iteration5_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid125"

# Submit job 126 with dependency on previous job
jid126=$(sbatch --parsable --dependency=afterok:$jid125 batchscripts/206_submit_iteration6_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid126"

# Submit job 127 with dependency on previous job
jid127=$(sbatch --parsable --dependency=afterok:$jid126 batchscripts/207_submit_iteration7_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid127"

# Submit job 128 with dependency on previous job
jid128=$(sbatch --parsable --dependency=afterok:$jid127 batchscripts/208_submit_iteration8_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid128"

# Submit job 129 with dependency on previous job
jid129=$(sbatch --parsable --dependency=afterok:$jid128 batchscripts/209_submit_iteration9_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid129"

# Submit job 130 with dependency on previous job
jid130=$(sbatch --parsable --dependency=afterok:$jid129 batchscripts/20_submit_iteration10_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid130"

# Submit job 131 with dependency on previous job
jid131=$(sbatch --parsable --dependency=afterok:$jid130 batchscripts/210_submit_iteration10_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid131"

# Submit job 132 with dependency on previous job
jid132=$(sbatch --parsable --dependency=afterok:$jid131 batchscripts/211_submit_iteration11_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid132"

# Submit job 133 with dependency on previous job
jid133=$(sbatch --parsable --dependency=afterok:$jid132 batchscripts/212_submit_iteration12_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid133"

# Submit job 134 with dependency on previous job
jid134=$(sbatch --parsable --dependency=afterok:$jid133 batchscripts/213_submit_iteration13_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid134"

# Submit job 135 with dependency on previous job
jid135=$(sbatch --parsable --dependency=afterok:$jid134 batchscripts/214_submit_iteration14_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid135"

# Submit job 136 with dependency on previous job
jid136=$(sbatch --parsable --dependency=afterok:$jid135 batchscripts/215_submit_iteration15_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid136"

# Submit job 137 with dependency on previous job
jid137=$(sbatch --parsable --dependency=afterok:$jid136 batchscripts/216_submit_iteration16_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid137"

# Submit job 138 with dependency on previous job
jid138=$(sbatch --parsable --dependency=afterok:$jid137 batchscripts/217_submit_iteration17_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid138"

# Submit job 139 with dependency on previous job
jid139=$(sbatch --parsable --dependency=afterok:$jid138 batchscripts/218_submit_iteration18_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid139"

# Submit job 140 with dependency on previous job
jid140=$(sbatch --parsable --dependency=afterok:$jid139 batchscripts/219_submit_iteration19_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid140"

# Submit job 141 with dependency on previous job
jid141=$(sbatch --parsable --dependency=afterok:$jid140 batchscripts/21_submit_iteration110_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid141"

# Submit job 142 with dependency on previous job
jid142=$(sbatch --parsable --dependency=afterok:$jid141 batchscripts/220_submit_iteration20_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid142"

# Submit job 143 with dependency on previous job
jid143=$(sbatch --parsable --dependency=afterok:$jid142 batchscripts/221_submit_iteration21_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid143"

# Submit job 144 with dependency on previous job
jid144=$(sbatch --parsable --dependency=afterok:$jid143 batchscripts/222_submit_iteration22_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid144"

# Submit job 145 with dependency on previous job
jid145=$(sbatch --parsable --dependency=afterok:$jid144 batchscripts/223_submit_iteration23_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid145"

# Submit job 146 with dependency on previous job
jid146=$(sbatch --parsable --dependency=afterok:$jid145 batchscripts/224_submit_iteration24_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid146"

# Submit job 147 with dependency on previous job
jid147=$(sbatch --parsable --dependency=afterok:$jid146 batchscripts/225_submit_iteration25_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid147"

# Submit job 148 with dependency on previous job
jid148=$(sbatch --parsable --dependency=afterok:$jid147 batchscripts/226_submit_iteration26_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid148"

# Submit job 149 with dependency on previous job
jid149=$(sbatch --parsable --dependency=afterok:$jid148 batchscripts/227_submit_iteration27_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid149"

# Submit job 150 with dependency on previous job
jid150=$(sbatch --parsable --dependency=afterok:$jid149 batchscripts/228_submit_iteration28_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid150"

# Submit job 151 with dependency on previous job
jid151=$(sbatch --parsable --dependency=afterok:$jid150 batchscripts/229_submit_iteration29_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid151"

# Submit job 152 with dependency on previous job
jid152=$(sbatch --parsable --dependency=afterok:$jid151 batchscripts/22_submit_iteration111_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid152"

# Submit job 153 with dependency on previous job
jid153=$(sbatch --parsable --dependency=afterok:$jid152 batchscripts/230_submit_iteration30_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid153"

# Submit job 154 with dependency on previous job
jid154=$(sbatch --parsable --dependency=afterok:$jid153 batchscripts/231_submit_iteration31_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid154"

# Submit job 155 with dependency on previous job
jid155=$(sbatch --parsable --dependency=afterok:$jid154 batchscripts/232_submit_iteration32_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid155"

# Submit job 156 with dependency on previous job
jid156=$(sbatch --parsable --dependency=afterok:$jid155 batchscripts/233_submit_iteration33_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid156"

# Submit job 157 with dependency on previous job
jid157=$(sbatch --parsable --dependency=afterok:$jid156 batchscripts/234_submit_iteration34_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid157"

# Submit job 158 with dependency on previous job
jid158=$(sbatch --parsable --dependency=afterok:$jid157 batchscripts/235_submit_iteration35_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid158"

# Submit job 159 with dependency on previous job
jid159=$(sbatch --parsable --dependency=afterok:$jid158 batchscripts/236_submit_iteration36_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid159"

# Submit job 160 with dependency on previous job
jid160=$(sbatch --parsable --dependency=afterok:$jid159 batchscripts/237_submit_iteration37_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid160"

# Submit job 161 with dependency on previous job
jid161=$(sbatch --parsable --dependency=afterok:$jid160 batchscripts/238_submit_iteration38_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid161"

# Submit job 162 with dependency on previous job
jid162=$(sbatch --parsable --dependency=afterok:$jid161 batchscripts/239_submit_iteration39_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid162"

# Submit job 163 with dependency on previous job
jid163=$(sbatch --parsable --dependency=afterok:$jid162 batchscripts/23_submit_iteration112_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid163"

# Submit job 164 with dependency on previous job
jid164=$(sbatch --parsable --dependency=afterok:$jid163 batchscripts/240_submit_iteration40_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid164"

# Submit job 165 with dependency on previous job
jid165=$(sbatch --parsable --dependency=afterok:$jid164 batchscripts/241_submit_iteration41_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid165"

# Submit job 166 with dependency on previous job
jid166=$(sbatch --parsable --dependency=afterok:$jid165 batchscripts/242_submit_iteration42_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid166"

# Submit job 167 with dependency on previous job
jid167=$(sbatch --parsable --dependency=afterok:$jid166 batchscripts/243_submit_iteration43_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid167"

# Submit job 168 with dependency on previous job
jid168=$(sbatch --parsable --dependency=afterok:$jid167 batchscripts/244_submit_iteration44_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid168"

# Submit job 169 with dependency on previous job
jid169=$(sbatch --parsable --dependency=afterok:$jid168 batchscripts/245_submit_iteration45_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid169"

# Submit job 170 with dependency on previous job
jid170=$(sbatch --parsable --dependency=afterok:$jid169 batchscripts/246_submit_iteration46_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid170"

# Submit job 171 with dependency on previous job
jid171=$(sbatch --parsable --dependency=afterok:$jid170 batchscripts/247_submit_iteration47_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid171"

# Submit job 172 with dependency on previous job
jid172=$(sbatch --parsable --dependency=afterok:$jid171 batchscripts/248_submit_iteration48_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid172"

# Submit job 173 with dependency on previous job
jid173=$(sbatch --parsable --dependency=afterok:$jid172 batchscripts/249_submit_iteration49_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid173"

# Submit job 174 with dependency on previous job
jid174=$(sbatch --parsable --dependency=afterok:$jid173 batchscripts/24_submit_iteration113_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid174"

# Submit job 175 with dependency on previous job
jid175=$(sbatch --parsable --dependency=afterok:$jid174 batchscripts/250_submit_iteration50_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid175"

# Submit job 176 with dependency on previous job
jid176=$(sbatch --parsable --dependency=afterok:$jid175 batchscripts/251_submit_iteration51_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid176"

# Submit job 177 with dependency on previous job
jid177=$(sbatch --parsable --dependency=afterok:$jid176 batchscripts/252_submit_iteration52_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid177"

# Submit job 178 with dependency on previous job
jid178=$(sbatch --parsable --dependency=afterok:$jid177 batchscripts/253_submit_iteration53_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid178"

# Submit job 179 with dependency on previous job
jid179=$(sbatch --parsable --dependency=afterok:$jid178 batchscripts/254_submit_iteration54_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid179"

# Submit job 180 with dependency on previous job
jid180=$(sbatch --parsable --dependency=afterok:$jid179 batchscripts/255_submit_iteration55_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid180"

# Submit job 181 with dependency on previous job
jid181=$(sbatch --parsable --dependency=afterok:$jid180 batchscripts/256_submit_iteration56_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid181"

# Submit job 182 with dependency on previous job
jid182=$(sbatch --parsable --dependency=afterok:$jid181 batchscripts/257_submit_iteration57_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid182"

# Submit job 183 with dependency on previous job
jid183=$(sbatch --parsable --dependency=afterok:$jid182 batchscripts/258_submit_iteration58_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid183"

# Submit job 184 with dependency on previous job
jid184=$(sbatch --parsable --dependency=afterok:$jid183 batchscripts/259_submit_iteration59_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid184"

# Submit job 185 with dependency on previous job
jid185=$(sbatch --parsable --dependency=afterok:$jid184 batchscripts/25_submit_iteration114_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid185"

# Submit job 186 with dependency on previous job
jid186=$(sbatch --parsable --dependency=afterok:$jid185 batchscripts/260_submit_iteration60_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid186"

# Submit job 187 with dependency on previous job
jid187=$(sbatch --parsable --dependency=afterok:$jid186 batchscripts/261_submit_iteration61_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid187"

# Submit job 188 with dependency on previous job
jid188=$(sbatch --parsable --dependency=afterok:$jid187 batchscripts/262_submit_iteration62_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid188"

# Submit job 189 with dependency on previous job
jid189=$(sbatch --parsable --dependency=afterok:$jid188 batchscripts/263_submit_iteration63_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid189"

# Submit job 190 with dependency on previous job
jid190=$(sbatch --parsable --dependency=afterok:$jid189 batchscripts/264_submit_iteration64_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid190"

# Submit job 191 with dependency on previous job
jid191=$(sbatch --parsable --dependency=afterok:$jid190 batchscripts/265_submit_iteration65_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid191"

# Submit job 192 with dependency on previous job
jid192=$(sbatch --parsable --dependency=afterok:$jid191 batchscripts/266_submit_iteration66_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid192"

# Submit job 193 with dependency on previous job
jid193=$(sbatch --parsable --dependency=afterok:$jid192 batchscripts/267_submit_iteration67_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid193"

# Submit job 194 with dependency on previous job
jid194=$(sbatch --parsable --dependency=afterok:$jid193 batchscripts/268_submit_iteration68_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid194"

# Submit job 195 with dependency on previous job
jid195=$(sbatch --parsable --dependency=afterok:$jid194 batchscripts/269_submit_iteration69_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid195"

# Submit job 196 with dependency on previous job
jid196=$(sbatch --parsable --dependency=afterok:$jid195 batchscripts/26_submit_iteration115_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid196"

# Submit job 197 with dependency on previous job
jid197=$(sbatch --parsable --dependency=afterok:$jid196 batchscripts/270_submit_iteration70_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid197"

# Submit job 198 with dependency on previous job
jid198=$(sbatch --parsable --dependency=afterok:$jid197 batchscripts/271_submit_iteration71_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid198"

# Submit job 199 with dependency on previous job
jid199=$(sbatch --parsable --dependency=afterok:$jid198 batchscripts/272_submit_iteration72_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid199"

# Submit job 200 with dependency on previous job
jid200=$(sbatch --parsable --dependency=afterok:$jid199 batchscripts/273_submit_iteration73_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid200"

# Submit job 201 with dependency on previous job
jid201=$(sbatch --parsable --dependency=afterok:$jid200 batchscripts/274_submit_iteration74_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid201"

# Submit job 202 with dependency on previous job
jid202=$(sbatch --parsable --dependency=afterok:$jid201 batchscripts/275_submit_iteration75_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid202"

# Submit job 203 with dependency on previous job
jid203=$(sbatch --parsable --dependency=afterok:$jid202 batchscripts/276_submit_iteration76_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid203"

# Submit job 204 with dependency on previous job
jid204=$(sbatch --parsable --dependency=afterok:$jid203 batchscripts/277_submit_iteration77_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid204"

# Submit job 205 with dependency on previous job
jid205=$(sbatch --parsable --dependency=afterok:$jid204 batchscripts/278_submit_iteration78_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid205"

# Submit job 206 with dependency on previous job
jid206=$(sbatch --parsable --dependency=afterok:$jid205 batchscripts/279_submit_iteration79_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid206"

# Submit job 207 with dependency on previous job
jid207=$(sbatch --parsable --dependency=afterok:$jid206 batchscripts/27_submit_iteration116_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid207"

# Submit job 208 with dependency on previous job
jid208=$(sbatch --parsable --dependency=afterok:$jid207 batchscripts/280_submit_iteration80_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid208"

# Submit job 209 with dependency on previous job
jid209=$(sbatch --parsable --dependency=afterok:$jid208 batchscripts/281_submit_iteration81_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid209"

# Submit job 210 with dependency on previous job
jid210=$(sbatch --parsable --dependency=afterok:$jid209 batchscripts/282_submit_iteration82_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid210"

# Submit job 211 with dependency on previous job
jid211=$(sbatch --parsable --dependency=afterok:$jid210 batchscripts/283_submit_iteration83_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid211"

# Submit job 212 with dependency on previous job
jid212=$(sbatch --parsable --dependency=afterok:$jid211 batchscripts/284_submit_iteration84_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid212"

# Submit job 213 with dependency on previous job
jid213=$(sbatch --parsable --dependency=afterok:$jid212 batchscripts/285_submit_iteration85_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid213"

# Submit job 214 with dependency on previous job
jid214=$(sbatch --parsable --dependency=afterok:$jid213 batchscripts/286_submit_iteration86_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid214"

# Submit job 215 with dependency on previous job
jid215=$(sbatch --parsable --dependency=afterok:$jid214 batchscripts/287_submit_iteration87_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid215"

# Submit job 216 with dependency on previous job
jid216=$(sbatch --parsable --dependency=afterok:$jid215 batchscripts/288_submit_iteration88_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid216"

# Submit job 217 with dependency on previous job
jid217=$(sbatch --parsable --dependency=afterok:$jid216 batchscripts/289_submit_iteration89_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid217"

# Submit job 218 with dependency on previous job
jid218=$(sbatch --parsable --dependency=afterok:$jid217 batchscripts/28_submit_iteration117_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid218"

# Submit job 219 with dependency on previous job
jid219=$(sbatch --parsable --dependency=afterok:$jid218 batchscripts/290_submit_iteration90_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid219"

# Submit job 220 with dependency on previous job
jid220=$(sbatch --parsable --dependency=afterok:$jid219 batchscripts/291_submit_iteration91_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid220"

# Submit job 221 with dependency on previous job
jid221=$(sbatch --parsable --dependency=afterok:$jid220 batchscripts/292_submit_iteration92_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid221"

# Submit job 222 with dependency on previous job
jid222=$(sbatch --parsable --dependency=afterok:$jid221 batchscripts/293_submit_iteration93_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid222"

# Submit job 223 with dependency on previous job
jid223=$(sbatch --parsable --dependency=afterok:$jid222 batchscripts/294_submit_iteration94_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid223"

# Submit job 224 with dependency on previous job
jid224=$(sbatch --parsable --dependency=afterok:$jid223 batchscripts/295_submit_iteration95_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid224"

# Submit job 225 with dependency on previous job
jid225=$(sbatch --parsable --dependency=afterok:$jid224 batchscripts/296_submit_iteration96_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid225"

# Submit job 226 with dependency on previous job
jid226=$(sbatch --parsable --dependency=afterok:$jid225 batchscripts/297_submit_iteration97_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid226"

# Submit job 227 with dependency on previous job
jid227=$(sbatch --parsable --dependency=afterok:$jid226 batchscripts/298_submit_iteration98_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid227"

# Submit job 228 with dependency on previous job
jid228=$(sbatch --parsable --dependency=afterok:$jid227 batchscripts/299_submit_iteration99_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid228"

# Submit job 229 with dependency on previous job
jid229=$(sbatch --parsable --dependency=afterok:$jid228 batchscripts/29_submit_iteration118_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid229"

# Submit job 230 with dependency on previous job
jid230=$(sbatch --parsable --dependency=afterok:$jid229 batchscripts/300_submit_iteration100_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid230"

# Submit job 231 with dependency on previous job
jid231=$(sbatch --parsable --dependency=afterok:$jid230 batchscripts/301_submit_iteration101_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid231"

# Submit job 232 with dependency on previous job
jid232=$(sbatch --parsable --dependency=afterok:$jid231 batchscripts/302_submit_iteration102_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid232"

# Submit job 233 with dependency on previous job
jid233=$(sbatch --parsable --dependency=afterok:$jid232 batchscripts/303_submit_iteration103_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid233"

# Submit job 234 with dependency on previous job
jid234=$(sbatch --parsable --dependency=afterok:$jid233 batchscripts/304_submit_iteration104_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid234"

# Submit job 235 with dependency on previous job
jid235=$(sbatch --parsable --dependency=afterok:$jid234 batchscripts/305_submit_iteration105_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid235"

# Submit job 236 with dependency on previous job
jid236=$(sbatch --parsable --dependency=afterok:$jid235 batchscripts/306_submit_iteration106_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid236"

# Submit job 237 with dependency on previous job
jid237=$(sbatch --parsable --dependency=afterok:$jid236 batchscripts/307_submit_iteration107_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid237"

# Submit job 238 with dependency on previous job
jid238=$(sbatch --parsable --dependency=afterok:$jid237 batchscripts/308_submit_iteration108_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid238"

# Submit job 239 with dependency on previous job
jid239=$(sbatch --parsable --dependency=afterok:$jid238 batchscripts/309_submit_iteration109_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid239"

# Submit job 240 with dependency on previous job
jid240=$(sbatch --parsable --dependency=afterok:$jid239 batchscripts/30_submit_iteration119_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid240"

# Submit job 241 with dependency on previous job
jid241=$(sbatch --parsable --dependency=afterok:$jid240 batchscripts/310_submit_iteration110_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid241"

# Submit job 242 with dependency on previous job
jid242=$(sbatch --parsable --dependency=afterok:$jid241 batchscripts/311_submit_iteration111_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid242"

# Submit job 243 with dependency on previous job
jid243=$(sbatch --parsable --dependency=afterok:$jid242 batchscripts/312_submit_iteration112_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid243"

# Submit job 244 with dependency on previous job
jid244=$(sbatch --parsable --dependency=afterok:$jid243 batchscripts/313_submit_iteration113_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid244"

# Submit job 245 with dependency on previous job
jid245=$(sbatch --parsable --dependency=afterok:$jid244 batchscripts/314_submit_iteration114_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid245"

# Submit job 246 with dependency on previous job
jid246=$(sbatch --parsable --dependency=afterok:$jid245 batchscripts/315_submit_iteration115_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid246"

# Submit job 247 with dependency on previous job
jid247=$(sbatch --parsable --dependency=afterok:$jid246 batchscripts/316_submit_iteration116_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid247"

# Submit job 248 with dependency on previous job
jid248=$(sbatch --parsable --dependency=afterok:$jid247 batchscripts/317_submit_iteration117_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid248"

# Submit job 249 with dependency on previous job
jid249=$(sbatch --parsable --dependency=afterok:$jid248 batchscripts/318_submit_iteration118_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid249"

# Submit job 250 with dependency on previous job
jid250=$(sbatch --parsable --dependency=afterok:$jid249 batchscripts/319_submit_iteration119_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid250"

# Submit job 251 with dependency on previous job
jid251=$(sbatch --parsable --dependency=afterok:$jid250 batchscripts/31_submit_iteration11_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid251"

# Submit job 252 with dependency on previous job
jid252=$(sbatch --parsable --dependency=afterok:$jid251 batchscripts/320_submit_iteration120_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid252"

# Submit job 253 with dependency on previous job
jid253=$(sbatch --parsable --dependency=afterok:$jid252 batchscripts/321_submit_iteration121_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid253"

# Submit job 254 with dependency on previous job
jid254=$(sbatch --parsable --dependency=afterok:$jid253 batchscripts/322_submit_iteration122_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid254"

# Submit job 255 with dependency on previous job
jid255=$(sbatch --parsable --dependency=afterok:$jid254 batchscripts/323_submit_iteration123_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid255"

# Submit job 256 with dependency on previous job
jid256=$(sbatch --parsable --dependency=afterok:$jid255 batchscripts/324_submit_iteration124_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid256"

# Submit job 257 with dependency on previous job
jid257=$(sbatch --parsable --dependency=afterok:$jid256 batchscripts/325_submit_iteration125_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid257"

# Submit job 258 with dependency on previous job
jid258=$(sbatch --parsable --dependency=afterok:$jid257 batchscripts/326_submit_iteration126_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid258"

# Submit job 259 with dependency on previous job
jid259=$(sbatch --parsable --dependency=afterok:$jid258 batchscripts/327_submit_iteration127_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid259"

# Submit job 260 with dependency on previous job
jid260=$(sbatch --parsable --dependency=afterok:$jid259 batchscripts/328_submit_iteration128_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid260"

# Submit job 261 with dependency on previous job
jid261=$(sbatch --parsable --dependency=afterok:$jid260 batchscripts/329_submit_iteration129_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid261"

# Submit job 262 with dependency on previous job
jid262=$(sbatch --parsable --dependency=afterok:$jid261 batchscripts/32_submit_iteration120_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid262"

# Submit job 263 with dependency on previous job
jid263=$(sbatch --parsable --dependency=afterok:$jid262 batchscripts/330_submit_iteration130_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid263"

# Submit job 264 with dependency on previous job
jid264=$(sbatch --parsable --dependency=afterok:$jid263 batchscripts/331_submit_iteration131_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid264"

# Submit job 265 with dependency on previous job
jid265=$(sbatch --parsable --dependency=afterok:$jid264 batchscripts/332_submit_iteration132_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid265"

# Submit job 266 with dependency on previous job
jid266=$(sbatch --parsable --dependency=afterok:$jid265 batchscripts/333_submit_iteration133_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid266"

# Submit job 267 with dependency on previous job
jid267=$(sbatch --parsable --dependency=afterok:$jid266 batchscripts/334_submit_iteration134_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid267"

# Submit job 268 with dependency on previous job
jid268=$(sbatch --parsable --dependency=afterok:$jid267 batchscripts/335_submit_iteration135_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid268"

# Submit job 269 with dependency on previous job
jid269=$(sbatch --parsable --dependency=afterok:$jid268 batchscripts/336_submit_iteration136_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid269"

# Submit job 270 with dependency on previous job
jid270=$(sbatch --parsable --dependency=afterok:$jid269 batchscripts/337_submit_iteration137_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid270"

# Submit job 271 with dependency on previous job
jid271=$(sbatch --parsable --dependency=afterok:$jid270 batchscripts/338_submit_iteration138_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid271"

# Submit job 272 with dependency on previous job
jid272=$(sbatch --parsable --dependency=afterok:$jid271 batchscripts/339_submit_iteration139_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid272"

# Submit job 273 with dependency on previous job
jid273=$(sbatch --parsable --dependency=afterok:$jid272 batchscripts/33_submit_iteration121_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid273"

# Submit job 274 with dependency on previous job
jid274=$(sbatch --parsable --dependency=afterok:$jid273 batchscripts/340_submit_iteration140_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid274"

# Submit job 275 with dependency on previous job
jid275=$(sbatch --parsable --dependency=afterok:$jid274 batchscripts/341_submit_iteration141_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid275"

# Submit job 276 with dependency on previous job
jid276=$(sbatch --parsable --dependency=afterok:$jid275 batchscripts/342_submit_iteration142_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid276"

# Submit job 277 with dependency on previous job
jid277=$(sbatch --parsable --dependency=afterok:$jid276 batchscripts/343_submit_iteration143_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid277"

# Submit job 278 with dependency on previous job
jid278=$(sbatch --parsable --dependency=afterok:$jid277 batchscripts/344_submit_iteration144_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid278"

# Submit job 279 with dependency on previous job
jid279=$(sbatch --parsable --dependency=afterok:$jid278 batchscripts/345_submit_iteration145_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid279"

# Submit job 280 with dependency on previous job
jid280=$(sbatch --parsable --dependency=afterok:$jid279 batchscripts/346_submit_iteration146_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid280"

# Submit job 281 with dependency on previous job
jid281=$(sbatch --parsable --dependency=afterok:$jid280 batchscripts/347_submit_iteration147_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid281"

# Submit job 282 with dependency on previous job
jid282=$(sbatch --parsable --dependency=afterok:$jid281 batchscripts/348_submit_iteration148_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid282"

# Submit job 283 with dependency on previous job
jid283=$(sbatch --parsable --dependency=afterok:$jid282 batchscripts/349_submit_iteration149_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid283"

# Submit job 284 with dependency on previous job
jid284=$(sbatch --parsable --dependency=afterok:$jid283 batchscripts/34_submit_iteration122_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid284"

# Submit job 285 with dependency on previous job
jid285=$(sbatch --parsable --dependency=afterok:$jid284 batchscripts/350_submit_iteration150_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid285"

# Submit job 286 with dependency on previous job
jid286=$(sbatch --parsable --dependency=afterok:$jid285 batchscripts/351_submit_iteration151_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid286"

# Submit job 287 with dependency on previous job
jid287=$(sbatch --parsable --dependency=afterok:$jid286 batchscripts/352_submit_iteration152_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid287"

# Submit job 288 with dependency on previous job
jid288=$(sbatch --parsable --dependency=afterok:$jid287 batchscripts/353_submit_iteration153_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid288"

# Submit job 289 with dependency on previous job
jid289=$(sbatch --parsable --dependency=afterok:$jid288 batchscripts/354_submit_iteration154_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid289"

# Submit job 290 with dependency on previous job
jid290=$(sbatch --parsable --dependency=afterok:$jid289 batchscripts/355_submit_iteration155_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid290"

# Submit job 291 with dependency on previous job
jid291=$(sbatch --parsable --dependency=afterok:$jid290 batchscripts/356_submit_iteration156_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid291"

# Submit job 292 with dependency on previous job
jid292=$(sbatch --parsable --dependency=afterok:$jid291 batchscripts/357_submit_iteration157_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid292"

# Submit job 293 with dependency on previous job
jid293=$(sbatch --parsable --dependency=afterok:$jid292 batchscripts/358_submit_iteration158_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid293"

# Submit job 294 with dependency on previous job
jid294=$(sbatch --parsable --dependency=afterok:$jid293 batchscripts/359_submit_iteration159_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid294"

# Submit job 295 with dependency on previous job
jid295=$(sbatch --parsable --dependency=afterok:$jid294 batchscripts/35_submit_iteration123_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid295"

# Submit job 296 with dependency on previous job
jid296=$(sbatch --parsable --dependency=afterok:$jid295 batchscripts/360_submit_iteration160_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid296"

# Submit job 297 with dependency on previous job
jid297=$(sbatch --parsable --dependency=afterok:$jid296 batchscripts/361_submit_iteration161_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid297"

# Submit job 298 with dependency on previous job
jid298=$(sbatch --parsable --dependency=afterok:$jid297 batchscripts/362_submit_iteration162_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid298"

# Submit job 299 with dependency on previous job
jid299=$(sbatch --parsable --dependency=afterok:$jid298 batchscripts/363_submit_iteration163_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid299"

# Submit job 300 with dependency on previous job
jid300=$(sbatch --parsable --dependency=afterok:$jid299 batchscripts/364_submit_iteration164_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid300"

# Submit job 301 with dependency on previous job
jid301=$(sbatch --parsable --dependency=afterok:$jid300 batchscripts/365_submit_iteration165_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid301"

# Submit job 302 with dependency on previous job
jid302=$(sbatch --parsable --dependency=afterok:$jid301 batchscripts/366_submit_iteration166_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid302"

# Submit job 303 with dependency on previous job
jid303=$(sbatch --parsable --dependency=afterok:$jid302 batchscripts/367_submit_iteration167_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid303"

# Submit job 304 with dependency on previous job
jid304=$(sbatch --parsable --dependency=afterok:$jid303 batchscripts/368_submit_iteration168_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid304"

# Submit job 305 with dependency on previous job
jid305=$(sbatch --parsable --dependency=afterok:$jid304 batchscripts/369_submit_iteration169_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid305"

# Submit job 306 with dependency on previous job
jid306=$(sbatch --parsable --dependency=afterok:$jid305 batchscripts/36_submit_iteration124_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid306"

# Submit job 307 with dependency on previous job
jid307=$(sbatch --parsable --dependency=afterok:$jid306 batchscripts/370_submit_iteration170_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid307"

# Submit job 308 with dependency on previous job
jid308=$(sbatch --parsable --dependency=afterok:$jid307 batchscripts/371_submit_iteration171_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid308"

# Submit job 309 with dependency on previous job
jid309=$(sbatch --parsable --dependency=afterok:$jid308 batchscripts/372_submit_iteration172_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid309"

# Submit job 310 with dependency on previous job
jid310=$(sbatch --parsable --dependency=afterok:$jid309 batchscripts/373_submit_iteration173_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid310"

# Submit job 311 with dependency on previous job
jid311=$(sbatch --parsable --dependency=afterok:$jid310 batchscripts/374_submit_iteration174_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid311"

# Submit job 312 with dependency on previous job
jid312=$(sbatch --parsable --dependency=afterok:$jid311 batchscripts/375_submit_iteration175_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid312"

# Submit job 313 with dependency on previous job
jid313=$(sbatch --parsable --dependency=afterok:$jid312 batchscripts/376_submit_iteration176_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid313"

# Submit job 314 with dependency on previous job
jid314=$(sbatch --parsable --dependency=afterok:$jid313 batchscripts/377_submit_iteration177_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid314"

# Submit job 315 with dependency on previous job
jid315=$(sbatch --parsable --dependency=afterok:$jid314 batchscripts/378_submit_iteration178_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid315"

# Submit job 316 with dependency on previous job
jid316=$(sbatch --parsable --dependency=afterok:$jid315 batchscripts/379_submit_iteration179_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid316"

# Submit job 317 with dependency on previous job
jid317=$(sbatch --parsable --dependency=afterok:$jid316 batchscripts/37_submit_iteration125_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid317"

# Submit job 318 with dependency on previous job
jid318=$(sbatch --parsable --dependency=afterok:$jid317 batchscripts/380_submit_iteration180_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid318"

# Submit job 319 with dependency on previous job
jid319=$(sbatch --parsable --dependency=afterok:$jid318 batchscripts/381_submit_iteration181_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid319"

# Submit job 320 with dependency on previous job
jid320=$(sbatch --parsable --dependency=afterok:$jid319 batchscripts/382_submit_iteration182_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid320"

# Submit job 321 with dependency on previous job
jid321=$(sbatch --parsable --dependency=afterok:$jid320 batchscripts/383_submit_iteration183_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid321"

# Submit job 322 with dependency on previous job
jid322=$(sbatch --parsable --dependency=afterok:$jid321 batchscripts/384_submit_iteration184_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid322"

# Submit job 323 with dependency on previous job
jid323=$(sbatch --parsable --dependency=afterok:$jid322 batchscripts/385_submit_iteration185_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid323"

# Submit job 324 with dependency on previous job
jid324=$(sbatch --parsable --dependency=afterok:$jid323 batchscripts/386_submit_iteration186_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid324"

# Submit job 325 with dependency on previous job
jid325=$(sbatch --parsable --dependency=afterok:$jid324 batchscripts/387_submit_iteration187_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid325"

# Submit job 326 with dependency on previous job
jid326=$(sbatch --parsable --dependency=afterok:$jid325 batchscripts/388_submit_iteration188_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid326"

# Submit job 327 with dependency on previous job
jid327=$(sbatch --parsable --dependency=afterok:$jid326 batchscripts/389_submit_iteration189_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid327"

# Submit job 328 with dependency on previous job
jid328=$(sbatch --parsable --dependency=afterok:$jid327 batchscripts/38_submit_iteration126_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid328"

# Submit job 329 with dependency on previous job
jid329=$(sbatch --parsable --dependency=afterok:$jid328 batchscripts/390_submit_iteration190_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid329"

# Submit job 330 with dependency on previous job
jid330=$(sbatch --parsable --dependency=afterok:$jid329 batchscripts/391_submit_iteration191_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid330"

# Submit job 331 with dependency on previous job
jid331=$(sbatch --parsable --dependency=afterok:$jid330 batchscripts/392_submit_iteration192_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid331"

# Submit job 332 with dependency on previous job
jid332=$(sbatch --parsable --dependency=afterok:$jid331 batchscripts/393_submit_iteration193_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid332"

# Submit job 333 with dependency on previous job
jid333=$(sbatch --parsable --dependency=afterok:$jid332 batchscripts/394_submit_iteration194_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid333"

# Submit job 334 with dependency on previous job
jid334=$(sbatch --parsable --dependency=afterok:$jid333 batchscripts/395_submit_iteration195_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid334"

# Submit job 335 with dependency on previous job
jid335=$(sbatch --parsable --dependency=afterok:$jid334 batchscripts/396_submit_iteration196_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid335"

# Submit job 336 with dependency on previous job
jid336=$(sbatch --parsable --dependency=afterok:$jid335 batchscripts/397_submit_iteration197_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid336"

# Submit job 337 with dependency on previous job
jid337=$(sbatch --parsable --dependency=afterok:$jid336 batchscripts/398_submit_iteration198_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid337"

# Submit job 338 with dependency on previous job
jid338=$(sbatch --parsable --dependency=afterok:$jid337 batchscripts/399_submit_iteration199_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid338"

# Submit job 339 with dependency on previous job
jid339=$(sbatch --parsable --dependency=afterok:$jid338 batchscripts/39_submit_iteration127_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid339"

# Submit the first job and capture its Job ID
jid340=$(sbatch --parsable batchscripts/400_submit_iteration0_forallparticles.sh)
echo "Submitted job for runscript: $jid340"

# Submit job 341 with dependency on previous job
jid341=$(sbatch --parsable --dependency=afterok:$jid340 batchscripts/401_submit_iteration1_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid341"

# Submit job 342 with dependency on previous job
jid342=$(sbatch --parsable --dependency=afterok:$jid341 batchscripts/402_submit_iteration2_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid342"

# Submit job 343 with dependency on previous job
jid343=$(sbatch --parsable --dependency=afterok:$jid342 batchscripts/403_submit_iteration3_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid343"

# Submit job 344 with dependency on previous job
jid344=$(sbatch --parsable --dependency=afterok:$jid343 batchscripts/404_submit_iteration4_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid344"

# Submit job 345 with dependency on previous job
jid345=$(sbatch --parsable --dependency=afterok:$jid344 batchscripts/405_submit_iteration5_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid345"

# Submit job 346 with dependency on previous job
jid346=$(sbatch --parsable --dependency=afterok:$jid345 batchscripts/406_submit_iteration6_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid346"

# Submit job 347 with dependency on previous job
jid347=$(sbatch --parsable --dependency=afterok:$jid346 batchscripts/407_submit_iteration7_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid347"

# Submit job 348 with dependency on previous job
jid348=$(sbatch --parsable --dependency=afterok:$jid347 batchscripts/408_submit_iteration8_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid348"

# Submit job 349 with dependency on previous job
jid349=$(sbatch --parsable --dependency=afterok:$jid348 batchscripts/409_submit_iteration9_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid349"

# Submit job 350 with dependency on previous job
jid350=$(sbatch --parsable --dependency=afterok:$jid349 batchscripts/40_submit_iteration128_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid350"

# Submit job 351 with dependency on previous job
jid351=$(sbatch --parsable --dependency=afterok:$jid350 batchscripts/410_submit_iteration10_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid351"

# Submit job 352 with dependency on previous job
jid352=$(sbatch --parsable --dependency=afterok:$jid351 batchscripts/411_submit_iteration11_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid352"

# Submit job 353 with dependency on previous job
jid353=$(sbatch --parsable --dependency=afterok:$jid352 batchscripts/412_submit_iteration12_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid353"

# Submit job 354 with dependency on previous job
jid354=$(sbatch --parsable --dependency=afterok:$jid353 batchscripts/413_submit_iteration13_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid354"

# Submit job 355 with dependency on previous job
jid355=$(sbatch --parsable --dependency=afterok:$jid354 batchscripts/414_submit_iteration14_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid355"

# Submit job 356 with dependency on previous job
jid356=$(sbatch --parsable --dependency=afterok:$jid355 batchscripts/415_submit_iteration15_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid356"

# Submit job 357 with dependency on previous job
jid357=$(sbatch --parsable --dependency=afterok:$jid356 batchscripts/416_submit_iteration16_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid357"

# Submit job 358 with dependency on previous job
jid358=$(sbatch --parsable --dependency=afterok:$jid357 batchscripts/417_submit_iteration17_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid358"

# Submit job 359 with dependency on previous job
jid359=$(sbatch --parsable --dependency=afterok:$jid358 batchscripts/418_submit_iteration18_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid359"

# Submit job 360 with dependency on previous job
jid360=$(sbatch --parsable --dependency=afterok:$jid359 batchscripts/419_submit_iteration19_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid360"

# Submit job 361 with dependency on previous job
jid361=$(sbatch --parsable --dependency=afterok:$jid360 batchscripts/41_submit_iteration129_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid361"

# Submit job 362 with dependency on previous job
jid362=$(sbatch --parsable --dependency=afterok:$jid361 batchscripts/420_submit_iteration20_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid362"

# Submit job 363 with dependency on previous job
jid363=$(sbatch --parsable --dependency=afterok:$jid362 batchscripts/421_submit_iteration21_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid363"

# Submit job 364 with dependency on previous job
jid364=$(sbatch --parsable --dependency=afterok:$jid363 batchscripts/422_submit_iteration22_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid364"

# Submit job 365 with dependency on previous job
jid365=$(sbatch --parsable --dependency=afterok:$jid364 batchscripts/423_submit_iteration23_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid365"

# Submit job 366 with dependency on previous job
jid366=$(sbatch --parsable --dependency=afterok:$jid365 batchscripts/424_submit_iteration24_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid366"

# Submit job 367 with dependency on previous job
jid367=$(sbatch --parsable --dependency=afterok:$jid366 batchscripts/425_submit_iteration25_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid367"

# Submit job 368 with dependency on previous job
jid368=$(sbatch --parsable --dependency=afterok:$jid367 batchscripts/426_submit_iteration26_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid368"

# Submit job 369 with dependency on previous job
jid369=$(sbatch --parsable --dependency=afterok:$jid368 batchscripts/427_submit_iteration27_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid369"

# Submit job 370 with dependency on previous job
jid370=$(sbatch --parsable --dependency=afterok:$jid369 batchscripts/428_submit_iteration28_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid370"

# Submit job 371 with dependency on previous job
jid371=$(sbatch --parsable --dependency=afterok:$jid370 batchscripts/429_submit_iteration29_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid371"

# Submit job 372 with dependency on previous job
jid372=$(sbatch --parsable --dependency=afterok:$jid371 batchscripts/42_submit_iteration12_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid372"

# Submit job 373 with dependency on previous job
jid373=$(sbatch --parsable --dependency=afterok:$jid372 batchscripts/430_submit_iteration30_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid373"

# Submit job 374 with dependency on previous job
jid374=$(sbatch --parsable --dependency=afterok:$jid373 batchscripts/431_submit_iteration31_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid374"

# Submit job 375 with dependency on previous job
jid375=$(sbatch --parsable --dependency=afterok:$jid374 batchscripts/432_submit_iteration32_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid375"

# Submit job 376 with dependency on previous job
jid376=$(sbatch --parsable --dependency=afterok:$jid375 batchscripts/433_submit_iteration33_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid376"

# Submit job 377 with dependency on previous job
jid377=$(sbatch --parsable --dependency=afterok:$jid376 batchscripts/434_submit_iteration34_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid377"

# Submit job 378 with dependency on previous job
jid378=$(sbatch --parsable --dependency=afterok:$jid377 batchscripts/435_submit_iteration35_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid378"

# Submit job 379 with dependency on previous job
jid379=$(sbatch --parsable --dependency=afterok:$jid378 batchscripts/436_submit_iteration36_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid379"

# Submit job 380 with dependency on previous job
jid380=$(sbatch --parsable --dependency=afterok:$jid379 batchscripts/437_submit_iteration37_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid380"

# Submit job 381 with dependency on previous job
jid381=$(sbatch --parsable --dependency=afterok:$jid380 batchscripts/438_submit_iteration38_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid381"

# Submit job 382 with dependency on previous job
jid382=$(sbatch --parsable --dependency=afterok:$jid381 batchscripts/439_submit_iteration39_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid382"

# Submit job 383 with dependency on previous job
jid383=$(sbatch --parsable --dependency=afterok:$jid382 batchscripts/43_submit_iteration130_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid383"

# Submit job 384 with dependency on previous job
jid384=$(sbatch --parsable --dependency=afterok:$jid383 batchscripts/440_submit_iteration40_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid384"

# Submit job 385 with dependency on previous job
jid385=$(sbatch --parsable --dependency=afterok:$jid384 batchscripts/441_submit_iteration41_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid385"

# Submit job 386 with dependency on previous job
jid386=$(sbatch --parsable --dependency=afterok:$jid385 batchscripts/442_submit_iteration42_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid386"

# Submit job 387 with dependency on previous job
jid387=$(sbatch --parsable --dependency=afterok:$jid386 batchscripts/443_submit_iteration43_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid387"

# Submit job 388 with dependency on previous job
jid388=$(sbatch --parsable --dependency=afterok:$jid387 batchscripts/444_submit_iteration44_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid388"

# Submit job 389 with dependency on previous job
jid389=$(sbatch --parsable --dependency=afterok:$jid388 batchscripts/445_submit_iteration45_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid389"

# Submit job 390 with dependency on previous job
jid390=$(sbatch --parsable --dependency=afterok:$jid389 batchscripts/446_submit_iteration46_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid390"

# Submit job 391 with dependency on previous job
jid391=$(sbatch --parsable --dependency=afterok:$jid390 batchscripts/447_submit_iteration47_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid391"

# Submit job 392 with dependency on previous job
jid392=$(sbatch --parsable --dependency=afterok:$jid391 batchscripts/448_submit_iteration48_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid392"

# Submit job 393 with dependency on previous job
jid393=$(sbatch --parsable --dependency=afterok:$jid392 batchscripts/449_submit_iteration49_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid393"

# Submit job 394 with dependency on previous job
jid394=$(sbatch --parsable --dependency=afterok:$jid393 batchscripts/44_submit_iteration131_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid394"

# Submit job 395 with dependency on previous job
jid395=$(sbatch --parsable --dependency=afterok:$jid394 batchscripts/450_submit_iteration50_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid395"

# Submit job 396 with dependency on previous job
jid396=$(sbatch --parsable --dependency=afterok:$jid395 batchscripts/451_submit_iteration51_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid396"

# Submit job 397 with dependency on previous job
jid397=$(sbatch --parsable --dependency=afterok:$jid396 batchscripts/452_submit_iteration52_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid397"

# Submit job 398 with dependency on previous job
jid398=$(sbatch --parsable --dependency=afterok:$jid397 batchscripts/453_submit_iteration53_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid398"

# Submit job 399 with dependency on previous job
jid399=$(sbatch --parsable --dependency=afterok:$jid398 batchscripts/454_submit_iteration54_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid399"

# Submit job 400 with dependency on previous job
jid400=$(sbatch --parsable --dependency=afterok:$jid399 batchscripts/455_submit_iteration55_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid400"

# Submit job 401 with dependency on previous job
jid401=$(sbatch --parsable --dependency=afterok:$jid400 batchscripts/456_submit_iteration56_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid401"

# Submit job 402 with dependency on previous job
jid402=$(sbatch --parsable --dependency=afterok:$jid401 batchscripts/457_submit_iteration57_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid402"

# Submit job 403 with dependency on previous job
jid403=$(sbatch --parsable --dependency=afterok:$jid402 batchscripts/458_submit_iteration58_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid403"

# Submit job 404 with dependency on previous job
jid404=$(sbatch --parsable --dependency=afterok:$jid403 batchscripts/459_submit_iteration59_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid404"

# Submit job 405 with dependency on previous job
jid405=$(sbatch --parsable --dependency=afterok:$jid404 batchscripts/45_submit_iteration132_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid405"

# Submit job 406 with dependency on previous job
jid406=$(sbatch --parsable --dependency=afterok:$jid405 batchscripts/460_submit_iteration60_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid406"

# Submit job 407 with dependency on previous job
jid407=$(sbatch --parsable --dependency=afterok:$jid406 batchscripts/461_submit_iteration61_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid407"

# Submit job 408 with dependency on previous job
jid408=$(sbatch --parsable --dependency=afterok:$jid407 batchscripts/462_submit_iteration62_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid408"

# Submit job 409 with dependency on previous job
jid409=$(sbatch --parsable --dependency=afterok:$jid408 batchscripts/463_submit_iteration63_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid409"

# Submit job 410 with dependency on previous job
jid410=$(sbatch --parsable --dependency=afterok:$jid409 batchscripts/464_submit_iteration64_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid410"

# Submit job 411 with dependency on previous job
jid411=$(sbatch --parsable --dependency=afterok:$jid410 batchscripts/465_submit_iteration65_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid411"

# Submit job 412 with dependency on previous job
jid412=$(sbatch --parsable --dependency=afterok:$jid411 batchscripts/466_submit_iteration66_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid412"

# Submit job 413 with dependency on previous job
jid413=$(sbatch --parsable --dependency=afterok:$jid412 batchscripts/467_submit_iteration67_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid413"

# Submit job 414 with dependency on previous job
jid414=$(sbatch --parsable --dependency=afterok:$jid413 batchscripts/468_submit_iteration68_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid414"

# Submit job 415 with dependency on previous job
jid415=$(sbatch --parsable --dependency=afterok:$jid414 batchscripts/469_submit_iteration69_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid415"

# Submit job 416 with dependency on previous job
jid416=$(sbatch --parsable --dependency=afterok:$jid415 batchscripts/46_submit_iteration133_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid416"

# Submit job 417 with dependency on previous job
jid417=$(sbatch --parsable --dependency=afterok:$jid416 batchscripts/470_submit_iteration70_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid417"

# Submit job 418 with dependency on previous job
jid418=$(sbatch --parsable --dependency=afterok:$jid417 batchscripts/471_submit_iteration71_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid418"

# Submit job 419 with dependency on previous job
jid419=$(sbatch --parsable --dependency=afterok:$jid418 batchscripts/472_submit_iteration72_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid419"

# Submit job 420 with dependency on previous job
jid420=$(sbatch --parsable --dependency=afterok:$jid419 batchscripts/473_submit_iteration73_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid420"

# Submit job 421 with dependency on previous job
jid421=$(sbatch --parsable --dependency=afterok:$jid420 batchscripts/474_submit_iteration74_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid421"

# Submit job 422 with dependency on previous job
jid422=$(sbatch --parsable --dependency=afterok:$jid421 batchscripts/475_submit_iteration75_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid422"

# Submit job 423 with dependency on previous job
jid423=$(sbatch --parsable --dependency=afterok:$jid422 batchscripts/476_submit_iteration76_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid423"

# Submit job 424 with dependency on previous job
jid424=$(sbatch --parsable --dependency=afterok:$jid423 batchscripts/477_submit_iteration77_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid424"

# Submit job 425 with dependency on previous job
jid425=$(sbatch --parsable --dependency=afterok:$jid424 batchscripts/478_submit_iteration78_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid425"

# Submit job 426 with dependency on previous job
jid426=$(sbatch --parsable --dependency=afterok:$jid425 batchscripts/479_submit_iteration79_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid426"

# Submit job 427 with dependency on previous job
jid427=$(sbatch --parsable --dependency=afterok:$jid426 batchscripts/47_submit_iteration134_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid427"

# Submit job 428 with dependency on previous job
jid428=$(sbatch --parsable --dependency=afterok:$jid427 batchscripts/480_submit_iteration80_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid428"

# Submit job 429 with dependency on previous job
jid429=$(sbatch --parsable --dependency=afterok:$jid428 batchscripts/481_submit_iteration81_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid429"

# Submit job 430 with dependency on previous job
jid430=$(sbatch --parsable --dependency=afterok:$jid429 batchscripts/482_submit_iteration82_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid430"

# Submit job 431 with dependency on previous job
jid431=$(sbatch --parsable --dependency=afterok:$jid430 batchscripts/483_submit_iteration83_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid431"

# Submit job 432 with dependency on previous job
jid432=$(sbatch --parsable --dependency=afterok:$jid431 batchscripts/484_submit_iteration84_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid432"

# Submit job 433 with dependency on previous job
jid433=$(sbatch --parsable --dependency=afterok:$jid432 batchscripts/485_submit_iteration85_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid433"

# Submit job 434 with dependency on previous job
jid434=$(sbatch --parsable --dependency=afterok:$jid433 batchscripts/486_submit_iteration86_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid434"

# Submit job 435 with dependency on previous job
jid435=$(sbatch --parsable --dependency=afterok:$jid434 batchscripts/487_submit_iteration87_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid435"

# Submit job 436 with dependency on previous job
jid436=$(sbatch --parsable --dependency=afterok:$jid435 batchscripts/488_submit_iteration88_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid436"

# Submit job 437 with dependency on previous job
jid437=$(sbatch --parsable --dependency=afterok:$jid436 batchscripts/489_submit_iteration89_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid437"

# Submit job 438 with dependency on previous job
jid438=$(sbatch --parsable --dependency=afterok:$jid437 batchscripts/48_submit_iteration135_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid438"

# Submit job 439 with dependency on previous job
jid439=$(sbatch --parsable --dependency=afterok:$jid438 batchscripts/490_submit_iteration90_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid439"

# Submit job 440 with dependency on previous job
jid440=$(sbatch --parsable --dependency=afterok:$jid439 batchscripts/491_submit_iteration91_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid440"

# Submit job 441 with dependency on previous job
jid441=$(sbatch --parsable --dependency=afterok:$jid440 batchscripts/492_submit_iteration92_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid441"

# Submit job 442 with dependency on previous job
jid442=$(sbatch --parsable --dependency=afterok:$jid441 batchscripts/493_submit_iteration93_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid442"

# Submit job 443 with dependency on previous job
jid443=$(sbatch --parsable --dependency=afterok:$jid442 batchscripts/494_submit_iteration94_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid443"

# Submit job 444 with dependency on previous job
jid444=$(sbatch --parsable --dependency=afterok:$jid443 batchscripts/495_submit_iteration95_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid444"

# Submit job 445 with dependency on previous job
jid445=$(sbatch --parsable --dependency=afterok:$jid444 batchscripts/496_submit_iteration96_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid445"

# Submit job 446 with dependency on previous job
jid446=$(sbatch --parsable --dependency=afterok:$jid445 batchscripts/497_submit_iteration97_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid446"

# Submit job 447 with dependency on previous job
jid447=$(sbatch --parsable --dependency=afterok:$jid446 batchscripts/498_submit_iteration98_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid447"

# Submit job 448 with dependency on previous job
jid448=$(sbatch --parsable --dependency=afterok:$jid447 batchscripts/499_submit_iteration99_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid448"

# Submit job 449 with dependency on previous job
jid449=$(sbatch --parsable --dependency=afterok:$jid448 batchscripts/49_submit_iteration136_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid449"

# Submit job 450 with dependency on previous job
jid450=$(sbatch --parsable --dependency=afterok:$jid449 batchscripts/500_submit_iteration100_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid450"

# Submit job 451 with dependency on previous job
jid451=$(sbatch --parsable --dependency=afterok:$jid450 batchscripts/501_submit_iteration101_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid451"

# Submit job 452 with dependency on previous job
jid452=$(sbatch --parsable --dependency=afterok:$jid451 batchscripts/502_submit_iteration102_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid452"

# Submit job 453 with dependency on previous job
jid453=$(sbatch --parsable --dependency=afterok:$jid452 batchscripts/503_submit_iteration103_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid453"

# Submit job 454 with dependency on previous job
jid454=$(sbatch --parsable --dependency=afterok:$jid453 batchscripts/504_submit_iteration104_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid454"

# Submit job 455 with dependency on previous job
jid455=$(sbatch --parsable --dependency=afterok:$jid454 batchscripts/505_submit_iteration105_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid455"

# Submit job 456 with dependency on previous job
jid456=$(sbatch --parsable --dependency=afterok:$jid455 batchscripts/506_submit_iteration106_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid456"

# Submit job 457 with dependency on previous job
jid457=$(sbatch --parsable --dependency=afterok:$jid456 batchscripts/507_submit_iteration107_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid457"

# Submit job 458 with dependency on previous job
jid458=$(sbatch --parsable --dependency=afterok:$jid457 batchscripts/508_submit_iteration108_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid458"

# Submit job 459 with dependency on previous job
jid459=$(sbatch --parsable --dependency=afterok:$jid458 batchscripts/509_submit_iteration109_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid459"

# Submit job 460 with dependency on previous job
jid460=$(sbatch --parsable --dependency=afterok:$jid459 batchscripts/50_submit_iteration137_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid460"

# Submit job 461 with dependency on previous job
jid461=$(sbatch --parsable --dependency=afterok:$jid460 batchscripts/510_submit_iteration110_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid461"

# Submit job 462 with dependency on previous job
jid462=$(sbatch --parsable --dependency=afterok:$jid461 batchscripts/511_submit_iteration111_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid462"

# Submit job 463 with dependency on previous job
jid463=$(sbatch --parsable --dependency=afterok:$jid462 batchscripts/512_submit_iteration112_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid463"

# Submit job 464 with dependency on previous job
jid464=$(sbatch --parsable --dependency=afterok:$jid463 batchscripts/513_submit_iteration113_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid464"

# Submit job 465 with dependency on previous job
jid465=$(sbatch --parsable --dependency=afterok:$jid464 batchscripts/514_submit_iteration114_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid465"

# Submit job 466 with dependency on previous job
jid466=$(sbatch --parsable --dependency=afterok:$jid465 batchscripts/515_submit_iteration115_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid466"

# Submit job 467 with dependency on previous job
jid467=$(sbatch --parsable --dependency=afterok:$jid466 batchscripts/516_submit_iteration116_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid467"

# Submit job 468 with dependency on previous job
jid468=$(sbatch --parsable --dependency=afterok:$jid467 batchscripts/517_submit_iteration117_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid468"

# Submit job 469 with dependency on previous job
jid469=$(sbatch --parsable --dependency=afterok:$jid468 batchscripts/518_submit_iteration118_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid469"

# Submit job 470 with dependency on previous job
jid470=$(sbatch --parsable --dependency=afterok:$jid469 batchscripts/519_submit_iteration119_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid470"

# Submit job 471 with dependency on previous job
jid471=$(sbatch --parsable --dependency=afterok:$jid470 batchscripts/51_submit_iteration138_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid471"

# Submit job 472 with dependency on previous job
jid472=$(sbatch --parsable --dependency=afterok:$jid471 batchscripts/520_submit_iteration120_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid472"

# Submit job 473 with dependency on previous job
jid473=$(sbatch --parsable --dependency=afterok:$jid472 batchscripts/521_submit_iteration121_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid473"

# Submit job 474 with dependency on previous job
jid474=$(sbatch --parsable --dependency=afterok:$jid473 batchscripts/522_submit_iteration122_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid474"

# Submit job 475 with dependency on previous job
jid475=$(sbatch --parsable --dependency=afterok:$jid474 batchscripts/523_submit_iteration123_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid475"

# Submit job 476 with dependency on previous job
jid476=$(sbatch --parsable --dependency=afterok:$jid475 batchscripts/524_submit_iteration124_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid476"

# Submit job 477 with dependency on previous job
jid477=$(sbatch --parsable --dependency=afterok:$jid476 batchscripts/525_submit_iteration125_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid477"

# Submit job 478 with dependency on previous job
jid478=$(sbatch --parsable --dependency=afterok:$jid477 batchscripts/526_submit_iteration126_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid478"

# Submit job 479 with dependency on previous job
jid479=$(sbatch --parsable --dependency=afterok:$jid478 batchscripts/527_submit_iteration127_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid479"

# Submit job 480 with dependency on previous job
jid480=$(sbatch --parsable --dependency=afterok:$jid479 batchscripts/528_submit_iteration128_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid480"

# Submit job 481 with dependency on previous job
jid481=$(sbatch --parsable --dependency=afterok:$jid480 batchscripts/529_submit_iteration129_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid481"

# Submit job 482 with dependency on previous job
jid482=$(sbatch --parsable --dependency=afterok:$jid481 batchscripts/52_submit_iteration139_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid482"

# Submit job 483 with dependency on previous job
jid483=$(sbatch --parsable --dependency=afterok:$jid482 batchscripts/530_submit_iteration130_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid483"

# Submit job 484 with dependency on previous job
jid484=$(sbatch --parsable --dependency=afterok:$jid483 batchscripts/531_submit_iteration131_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid484"

# Submit job 485 with dependency on previous job
jid485=$(sbatch --parsable --dependency=afterok:$jid484 batchscripts/532_submit_iteration132_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid485"

# Submit job 486 with dependency on previous job
jid486=$(sbatch --parsable --dependency=afterok:$jid485 batchscripts/533_submit_iteration133_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid486"

# Submit job 487 with dependency on previous job
jid487=$(sbatch --parsable --dependency=afterok:$jid486 batchscripts/534_submit_iteration134_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid487"

# Submit job 488 with dependency on previous job
jid488=$(sbatch --parsable --dependency=afterok:$jid487 batchscripts/535_submit_iteration135_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid488"

# Submit job 489 with dependency on previous job
jid489=$(sbatch --parsable --dependency=afterok:$jid488 batchscripts/536_submit_iteration136_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid489"

# Submit job 490 with dependency on previous job
jid490=$(sbatch --parsable --dependency=afterok:$jid489 batchscripts/537_submit_iteration137_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid490"

# Submit job 491 with dependency on previous job
jid491=$(sbatch --parsable --dependency=afterok:$jid490 batchscripts/538_submit_iteration138_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid491"

# Submit job 492 with dependency on previous job
jid492=$(sbatch --parsable --dependency=afterok:$jid491 batchscripts/539_submit_iteration139_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid492"

# Submit job 493 with dependency on previous job
jid493=$(sbatch --parsable --dependency=afterok:$jid492 batchscripts/53_submit_iteration13_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid493"

# Submit job 494 with dependency on previous job
jid494=$(sbatch --parsable --dependency=afterok:$jid493 batchscripts/540_submit_iteration140_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid494"

# Submit job 495 with dependency on previous job
jid495=$(sbatch --parsable --dependency=afterok:$jid494 batchscripts/541_submit_iteration141_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid495"

# Submit job 496 with dependency on previous job
jid496=$(sbatch --parsable --dependency=afterok:$jid495 batchscripts/542_submit_iteration142_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid496"

# Submit job 497 with dependency on previous job
jid497=$(sbatch --parsable --dependency=afterok:$jid496 batchscripts/543_submit_iteration143_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid497"

# Submit job 498 with dependency on previous job
jid498=$(sbatch --parsable --dependency=afterok:$jid497 batchscripts/544_submit_iteration144_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid498"

# Submit job 499 with dependency on previous job
jid499=$(sbatch --parsable --dependency=afterok:$jid498 batchscripts/545_submit_iteration145_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid499"

# Submit job 500 with dependency on previous job
jid500=$(sbatch --parsable --dependency=afterok:$jid499 batchscripts/546_submit_iteration146_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid500"

# Submit job 501 with dependency on previous job
jid501=$(sbatch --parsable --dependency=afterok:$jid500 batchscripts/547_submit_iteration147_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid501"

# Submit job 502 with dependency on previous job
jid502=$(sbatch --parsable --dependency=afterok:$jid501 batchscripts/548_submit_iteration148_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid502"

# Submit job 503 with dependency on previous job
jid503=$(sbatch --parsable --dependency=afterok:$jid502 batchscripts/549_submit_iteration149_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid503"

# Submit job 504 with dependency on previous job
jid504=$(sbatch --parsable --dependency=afterok:$jid503 batchscripts/54_submit_iteration140_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid504"

# Submit job 505 with dependency on previous job
jid505=$(sbatch --parsable --dependency=afterok:$jid504 batchscripts/550_submit_iteration150_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid505"

# Submit job 506 with dependency on previous job
jid506=$(sbatch --parsable --dependency=afterok:$jid505 batchscripts/551_submit_iteration151_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid506"

# Submit job 507 with dependency on previous job
jid507=$(sbatch --parsable --dependency=afterok:$jid506 batchscripts/552_submit_iteration152_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid507"

# Submit job 508 with dependency on previous job
jid508=$(sbatch --parsable --dependency=afterok:$jid507 batchscripts/553_submit_iteration153_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid508"

# Submit job 509 with dependency on previous job
jid509=$(sbatch --parsable --dependency=afterok:$jid508 batchscripts/554_submit_iteration154_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid509"

# Submit job 510 with dependency on previous job
jid510=$(sbatch --parsable --dependency=afterok:$jid509 batchscripts/555_submit_iteration155_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid510"

# Submit job 511 with dependency on previous job
jid511=$(sbatch --parsable --dependency=afterok:$jid510 batchscripts/556_submit_iteration156_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid511"

# Submit job 512 with dependency on previous job
jid512=$(sbatch --parsable --dependency=afterok:$jid511 batchscripts/557_submit_iteration157_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid512"

# Submit job 513 with dependency on previous job
jid513=$(sbatch --parsable --dependency=afterok:$jid512 batchscripts/558_submit_iteration158_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid513"

# Submit job 514 with dependency on previous job
jid514=$(sbatch --parsable --dependency=afterok:$jid513 batchscripts/559_submit_iteration159_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid514"

# Submit job 515 with dependency on previous job
jid515=$(sbatch --parsable --dependency=afterok:$jid514 batchscripts/55_submit_iteration141_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid515"

# Submit job 516 with dependency on previous job
jid516=$(sbatch --parsable --dependency=afterok:$jid515 batchscripts/560_submit_iteration160_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid516"

# Submit job 517 with dependency on previous job
jid517=$(sbatch --parsable --dependency=afterok:$jid516 batchscripts/561_submit_iteration161_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid517"

# Submit job 518 with dependency on previous job
jid518=$(sbatch --parsable --dependency=afterok:$jid517 batchscripts/562_submit_iteration162_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid518"

# Submit job 519 with dependency on previous job
jid519=$(sbatch --parsable --dependency=afterok:$jid518 batchscripts/563_submit_iteration163_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid519"

# Submit job 520 with dependency on previous job
jid520=$(sbatch --parsable --dependency=afterok:$jid519 batchscripts/564_submit_iteration164_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid520"

# Submit job 521 with dependency on previous job
jid521=$(sbatch --parsable --dependency=afterok:$jid520 batchscripts/565_submit_iteration165_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid521"

# Submit job 522 with dependency on previous job
jid522=$(sbatch --parsable --dependency=afterok:$jid521 batchscripts/566_submit_iteration166_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid522"

# Submit job 523 with dependency on previous job
jid523=$(sbatch --parsable --dependency=afterok:$jid522 batchscripts/567_submit_iteration167_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid523"

# Submit job 524 with dependency on previous job
jid524=$(sbatch --parsable --dependency=afterok:$jid523 batchscripts/568_submit_iteration168_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid524"

# Submit job 525 with dependency on previous job
jid525=$(sbatch --parsable --dependency=afterok:$jid524 batchscripts/569_submit_iteration169_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid525"

# Submit job 526 with dependency on previous job
jid526=$(sbatch --parsable --dependency=afterok:$jid525 batchscripts/56_submit_iteration142_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid526"

# Submit job 527 with dependency on previous job
jid527=$(sbatch --parsable --dependency=afterok:$jid526 batchscripts/570_submit_iteration170_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid527"

# Submit job 528 with dependency on previous job
jid528=$(sbatch --parsable --dependency=afterok:$jid527 batchscripts/571_submit_iteration171_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid528"

# Submit job 529 with dependency on previous job
jid529=$(sbatch --parsable --dependency=afterok:$jid528 batchscripts/572_submit_iteration172_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid529"

# Submit job 530 with dependency on previous job
jid530=$(sbatch --parsable --dependency=afterok:$jid529 batchscripts/573_submit_iteration173_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid530"

# Submit job 531 with dependency on previous job
jid531=$(sbatch --parsable --dependency=afterok:$jid530 batchscripts/574_submit_iteration174_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid531"

# Submit job 532 with dependency on previous job
jid532=$(sbatch --parsable --dependency=afterok:$jid531 batchscripts/575_submit_iteration175_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid532"

# Submit job 533 with dependency on previous job
jid533=$(sbatch --parsable --dependency=afterok:$jid532 batchscripts/576_submit_iteration176_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid533"

# Submit job 534 with dependency on previous job
jid534=$(sbatch --parsable --dependency=afterok:$jid533 batchscripts/577_submit_iteration177_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid534"

# Submit job 535 with dependency on previous job
jid535=$(sbatch --parsable --dependency=afterok:$jid534 batchscripts/578_submit_iteration178_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid535"

# Submit job 536 with dependency on previous job
jid536=$(sbatch --parsable --dependency=afterok:$jid535 batchscripts/579_submit_iteration179_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid536"

# Submit job 537 with dependency on previous job
jid537=$(sbatch --parsable --dependency=afterok:$jid536 batchscripts/57_submit_iteration143_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid537"

# Submit job 538 with dependency on previous job
jid538=$(sbatch --parsable --dependency=afterok:$jid537 batchscripts/580_submit_iteration180_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid538"

# Submit job 539 with dependency on previous job
jid539=$(sbatch --parsable --dependency=afterok:$jid538 batchscripts/581_submit_iteration181_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid539"

# Submit job 540 with dependency on previous job
jid540=$(sbatch --parsable --dependency=afterok:$jid539 batchscripts/582_submit_iteration182_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid540"

# Submit job 541 with dependency on previous job
jid541=$(sbatch --parsable --dependency=afterok:$jid540 batchscripts/583_submit_iteration183_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid541"

# Submit job 542 with dependency on previous job
jid542=$(sbatch --parsable --dependency=afterok:$jid541 batchscripts/584_submit_iteration184_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid542"

# Submit job 543 with dependency on previous job
jid543=$(sbatch --parsable --dependency=afterok:$jid542 batchscripts/585_submit_iteration185_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid543"

# Submit job 544 with dependency on previous job
jid544=$(sbatch --parsable --dependency=afterok:$jid543 batchscripts/586_submit_iteration186_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid544"

# Submit job 545 with dependency on previous job
jid545=$(sbatch --parsable --dependency=afterok:$jid544 batchscripts/587_submit_iteration187_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid545"

# Submit job 546 with dependency on previous job
jid546=$(sbatch --parsable --dependency=afterok:$jid545 batchscripts/588_submit_iteration188_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid546"

# Submit job 547 with dependency on previous job
jid547=$(sbatch --parsable --dependency=afterok:$jid546 batchscripts/589_submit_iteration189_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid547"

# Submit job 548 with dependency on previous job
jid548=$(sbatch --parsable --dependency=afterok:$jid547 batchscripts/58_submit_iteration144_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid548"

# Submit job 549 with dependency on previous job
jid549=$(sbatch --parsable --dependency=afterok:$jid548 batchscripts/590_submit_iteration190_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid549"

# Submit job 550 with dependency on previous job
jid550=$(sbatch --parsable --dependency=afterok:$jid549 batchscripts/591_submit_iteration191_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid550"

# Submit job 551 with dependency on previous job
jid551=$(sbatch --parsable --dependency=afterok:$jid550 batchscripts/592_submit_iteration192_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid551"

# Submit job 552 with dependency on previous job
jid552=$(sbatch --parsable --dependency=afterok:$jid551 batchscripts/593_submit_iteration193_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid552"

# Submit job 553 with dependency on previous job
jid553=$(sbatch --parsable --dependency=afterok:$jid552 batchscripts/594_submit_iteration194_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid553"

# Submit job 554 with dependency on previous job
jid554=$(sbatch --parsable --dependency=afterok:$jid553 batchscripts/595_submit_iteration195_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid554"

# Submit job 555 with dependency on previous job
jid555=$(sbatch --parsable --dependency=afterok:$jid554 batchscripts/596_submit_iteration196_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid555"

# Submit job 556 with dependency on previous job
jid556=$(sbatch --parsable --dependency=afterok:$jid555 batchscripts/597_submit_iteration197_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid556"

# Submit job 557 with dependency on previous job
jid557=$(sbatch --parsable --dependency=afterok:$jid556 batchscripts/598_submit_iteration198_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid557"

# Submit job 558 with dependency on previous job
jid558=$(sbatch --parsable --dependency=afterok:$jid557 batchscripts/599_submit_iteration199_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid558"

# Submit job 559 with dependency on previous job
jid559=$(sbatch --parsable --dependency=afterok:$jid558 batchscripts/59_submit_iteration145_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid559"

# Submit job 560 with dependency on previous job
jid560=$(sbatch --parsable --dependency=afterok:$jid559 batchscripts/60_submit_iteration146_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid560"

# Submit job 561 with dependency on previous job
jid561=$(sbatch --parsable --dependency=afterok:$jid560 batchscripts/61_submit_iteration147_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid561"

# Submit job 562 with dependency on previous job
jid562=$(sbatch --parsable --dependency=afterok:$jid561 batchscripts/62_submit_iteration148_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid562"

# Submit job 563 with dependency on previous job
jid563=$(sbatch --parsable --dependency=afterok:$jid562 batchscripts/63_submit_iteration149_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid563"

# Submit job 564 with dependency on previous job
jid564=$(sbatch --parsable --dependency=afterok:$jid563 batchscripts/64_submit_iteration14_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid564"

# Submit job 565 with dependency on previous job
jid565=$(sbatch --parsable --dependency=afterok:$jid564 batchscripts/65_submit_iteration150_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid565"

# Submit job 566 with dependency on previous job
jid566=$(sbatch --parsable --dependency=afterok:$jid565 batchscripts/66_submit_iteration151_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid566"

# Submit job 567 with dependency on previous job
jid567=$(sbatch --parsable --dependency=afterok:$jid566 batchscripts/67_submit_iteration152_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid567"

# Submit job 568 with dependency on previous job
jid568=$(sbatch --parsable --dependency=afterok:$jid567 batchscripts/68_submit_iteration153_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid568"

# Submit job 569 with dependency on previous job
jid569=$(sbatch --parsable --dependency=afterok:$jid568 batchscripts/69_submit_iteration154_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid569"

# Submit job 570 with dependency on previous job
jid570=$(sbatch --parsable --dependency=afterok:$jid569 batchscripts/70_submit_iteration155_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid570"

# Submit job 571 with dependency on previous job
jid571=$(sbatch --parsable --dependency=afterok:$jid570 batchscripts/71_submit_iteration156_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid571"

# Submit job 572 with dependency on previous job
jid572=$(sbatch --parsable --dependency=afterok:$jid571 batchscripts/72_submit_iteration157_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid572"

# Submit job 573 with dependency on previous job
jid573=$(sbatch --parsable --dependency=afterok:$jid572 batchscripts/73_submit_iteration158_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid573"

# Submit job 574 with dependency on previous job
jid574=$(sbatch --parsable --dependency=afterok:$jid573 batchscripts/74_submit_iteration159_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid574"

# Submit job 575 with dependency on previous job
jid575=$(sbatch --parsable --dependency=afterok:$jid574 batchscripts/75_submit_iteration15_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid575"

# Submit job 576 with dependency on previous job
jid576=$(sbatch --parsable --dependency=afterok:$jid575 batchscripts/76_submit_iteration160_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid576"

# Submit job 577 with dependency on previous job
jid577=$(sbatch --parsable --dependency=afterok:$jid576 batchscripts/77_submit_iteration161_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid577"

# Submit job 578 with dependency on previous job
jid578=$(sbatch --parsable --dependency=afterok:$jid577 batchscripts/78_submit_iteration162_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid578"

# Submit job 579 with dependency on previous job
jid579=$(sbatch --parsable --dependency=afterok:$jid578 batchscripts/79_submit_iteration163_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid579"

# Submit job 580 with dependency on previous job
jid580=$(sbatch --parsable --dependency=afterok:$jid579 batchscripts/80_submit_iteration164_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid580"

# Submit job 581 with dependency on previous job
jid581=$(sbatch --parsable --dependency=afterok:$jid580 batchscripts/81_submit_iteration165_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid581"

# Submit job 582 with dependency on previous job
jid582=$(sbatch --parsable --dependency=afterok:$jid581 batchscripts/82_submit_iteration166_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid582"

# Submit job 583 with dependency on previous job
jid583=$(sbatch --parsable --dependency=afterok:$jid582 batchscripts/83_submit_iteration167_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid583"

# Submit job 584 with dependency on previous job
jid584=$(sbatch --parsable --dependency=afterok:$jid583 batchscripts/84_submit_iteration168_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid584"

# Submit job 585 with dependency on previous job
jid585=$(sbatch --parsable --dependency=afterok:$jid584 batchscripts/85_submit_iteration169_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid585"

# Submit job 586 with dependency on previous job
jid586=$(sbatch --parsable --dependency=afterok:$jid585 batchscripts/86_submit_iteration16_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid586"

# Submit job 587 with dependency on previous job
jid587=$(sbatch --parsable --dependency=afterok:$jid586 batchscripts/87_submit_iteration170_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid587"

# Submit job 588 with dependency on previous job
jid588=$(sbatch --parsable --dependency=afterok:$jid587 batchscripts/88_submit_iteration171_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid588"

# Submit job 589 with dependency on previous job
jid589=$(sbatch --parsable --dependency=afterok:$jid588 batchscripts/89_submit_iteration172_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid589"

# Submit job 590 with dependency on previous job
jid590=$(sbatch --parsable --dependency=afterok:$jid589 batchscripts/90_submit_iteration173_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid590"

# Submit job 591 with dependency on previous job
jid591=$(sbatch --parsable --dependency=afterok:$jid590 batchscripts/91_submit_iteration174_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid591"

# Submit job 592 with dependency on previous job
jid592=$(sbatch --parsable --dependency=afterok:$jid591 batchscripts/92_submit_iteration175_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid592"

# Submit job 593 with dependency on previous job
jid593=$(sbatch --parsable --dependency=afterok:$jid592 batchscripts/93_submit_iteration176_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid593"

# Submit job 594 with dependency on previous job
jid594=$(sbatch --parsable --dependency=afterok:$jid593 batchscripts/94_submit_iteration177_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid594"

# Submit job 595 with dependency on previous job
jid595=$(sbatch --parsable --dependency=afterok:$jid594 batchscripts/95_submit_iteration178_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid595"

# Submit job 596 with dependency on previous job
jid596=$(sbatch --parsable --dependency=afterok:$jid595 batchscripts/96_submit_iteration179_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid596"

# Submit job 597 with dependency on previous job
jid597=$(sbatch --parsable --dependency=afterok:$jid596 batchscripts/97_submit_iteration17_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid597"

# Submit job 598 with dependency on previous job
jid598=$(sbatch --parsable --dependency=afterok:$jid597 batchscripts/98_submit_iteration180_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid598"

# Submit job 599 with dependency on previous job
jid599=$(sbatch --parsable --dependency=afterok:$jid598 batchscripts/99_submit_iteration181_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid599"

