#!/bin/bash

# Submit the first job and capture its Job ID
jid1=$(sbatch --parsable batchscripts/submit_iteration0.sh)
echo "Submitted iteration 0 job: $jid1"

# Submit the second job with a dependency on the first
jid2=$(sbatch --dependency=afterok:$jid1 batchscripts/submit_iteration1.sh)
echo "Submitted iteration 1 job (afterok dependency): $jid2"
