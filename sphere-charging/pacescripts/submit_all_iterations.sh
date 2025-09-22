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
jid10=$(sbatch --parsable --dependency=afterok:$jid9 batchscripts/10_submit_iteration10_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid10"

# Submit job 11 with dependency on previous job
jid11=$(sbatch --parsable --dependency=afterok:$jid10 batchscripts/11_submit_iteration11_foronlysolarwind.sh)
echo "Submitted job for runscript (afterok dependency): $jid11"

# Submit the first job and capture its Job ID
jid12=$(sbatch --parsable batchscripts/12_submit_iteration0_foronlyphotoemission.sh)
echo "Submitted job for runscript: $jid12"

# Submit job 13 with dependency on previous job
jid13=$(sbatch --parsable --dependency=afterok:$jid12 batchscripts/13_submit_iteration1_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid13"

# Submit job 14 with dependency on previous job
jid14=$(sbatch --parsable --dependency=afterok:$jid13 batchscripts/14_submit_iteration2_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid14"

# Submit job 15 with dependency on previous job
jid15=$(sbatch --parsable --dependency=afterok:$jid14 batchscripts/15_submit_iteration3_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid15"

# Submit job 16 with dependency on previous job
jid16=$(sbatch --parsable --dependency=afterok:$jid15 batchscripts/16_submit_iteration4_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid16"

# Submit job 17 with dependency on previous job
jid17=$(sbatch --parsable --dependency=afterok:$jid16 batchscripts/17_submit_iteration5_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid17"

# Submit job 18 with dependency on previous job
jid18=$(sbatch --parsable --dependency=afterok:$jid17 batchscripts/18_submit_iteration6_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid18"

# Submit job 19 with dependency on previous job
jid19=$(sbatch --parsable --dependency=afterok:$jid18 batchscripts/19_submit_iteration7_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid19"

# Submit job 20 with dependency on previous job
jid20=$(sbatch --parsable --dependency=afterok:$jid19 batchscripts/20_submit_iteration8_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid20"

# Submit job 21 with dependency on previous job
jid21=$(sbatch --parsable --dependency=afterok:$jid20 batchscripts/21_submit_iteration9_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid21"

# Submit job 22 with dependency on previous job
jid22=$(sbatch --parsable --dependency=afterok:$jid21 batchscripts/22_submit_iteration10_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid22"

# Submit job 23 with dependency on previous job
jid23=$(sbatch --parsable --dependency=afterok:$jid22 batchscripts/23_submit_iteration11_foronlyphotoemission.sh)
echo "Submitted job for runscript (afterok dependency): $jid23"

# Submit the first job and capture its Job ID
jid24=$(sbatch --parsable batchscripts/24_submit_iteration0_forallparticles.sh)
echo "Submitted job for runscript: $jid24"

# Submit job 25 with dependency on previous job
jid25=$(sbatch --parsable --dependency=afterok:$jid24 batchscripts/25_submit_iteration1_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid25"

# Submit job 26 with dependency on previous job
jid26=$(sbatch --parsable --dependency=afterok:$jid25 batchscripts/26_submit_iteration2_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid26"

# Submit job 27 with dependency on previous job
jid27=$(sbatch --parsable --dependency=afterok:$jid26 batchscripts/27_submit_iteration3_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid27"

# Submit job 28 with dependency on previous job
jid28=$(sbatch --parsable --dependency=afterok:$jid27 batchscripts/28_submit_iteration4_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid28"

# Submit job 29 with dependency on previous job
jid29=$(sbatch --parsable --dependency=afterok:$jid28 batchscripts/29_submit_iteration5_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid29"

# Submit job 30 with dependency on previous job
jid30=$(sbatch --parsable --dependency=afterok:$jid29 batchscripts/30_submit_iteration6_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid30"

# Submit job 31 with dependency on previous job
jid31=$(sbatch --parsable --dependency=afterok:$jid30 batchscripts/31_submit_iteration7_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid31"

# Submit job 32 with dependency on previous job
jid32=$(sbatch --parsable --dependency=afterok:$jid31 batchscripts/32_submit_iteration8_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid32"

# Submit job 33 with dependency on previous job
jid33=$(sbatch --parsable --dependency=afterok:$jid32 batchscripts/33_submit_iteration9_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid33"

# Submit job 34 with dependency on previous job
jid34=$(sbatch --parsable --dependency=afterok:$jid33 batchscripts/34_submit_iteration10_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid34"

# Submit job 35 with dependency on previous job
jid35=$(sbatch --parsable --dependency=afterok:$jid34 batchscripts/35_submit_iteration11_forallparticles.sh)
echo "Submitted job for runscript (afterok dependency): $jid35"

