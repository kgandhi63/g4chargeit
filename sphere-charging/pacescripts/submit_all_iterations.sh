#!/bin/bash

# Submit the first job and capture its Job ID
jid0=$(sbatch --parsable batchscripts/submit_iteration0.sh)
echo "Submitted iteration 0 job: $jid0"

# Submit job 1 with dependency on previous job
jid1=$(sbatch --parsable --dependency=afterok:$jid0 batchscripts/submit_iteration1.sh)
echo "Submitted iteration 1 job (afterok dependency): $jid1"

# Submit job 2 with dependency on previous job
jid2=$(sbatch --parsable --dependency=afterok:$jid1 batchscripts/submit_iteration2.sh)
echo "Submitted iteration 2 job (afterok dependency): $jid2"

# Submit job 3 with dependency on previous job
jid3=$(sbatch --parsable --dependency=afterok:$jid2 batchscripts/submit_iteration3.sh)
echo "Submitted iteration 3 job (afterok dependency): $jid3"

# Submit job 4 with dependency on previous job
jid4=$(sbatch --parsable --dependency=afterok:$jid3 batchscripts/submit_iteration4.sh)
echo "Submitted iteration 4 job (afterok dependency): $jid4"

# Submit job 5 with dependency on previous job
jid5=$(sbatch --parsable --dependency=afterok:$jid4 batchscripts/submit_iteration5.sh)
echo "Submitted iteration 5 job (afterok dependency): $jid5"

# Submit job 6 with dependency on previous job
jid6=$(sbatch --parsable --dependency=afterok:$jid5 batchscripts/submit_iteration6.sh)
echo "Submitted iteration 6 job (afterok dependency): $jid6"

# Submit job 7 with dependency on previous job
jid7=$(sbatch --parsable --dependency=afterok:$jid6 batchscripts/submit_iteration7.sh)
echo "Submitted iteration 7 job (afterok dependency): $jid7"

