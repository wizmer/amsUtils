# ams
The bin/ directory contains various scripts described here:
_ newLoop.sh
  Is used to generate a folder containing a source code for looping on AMSEvent
  
_ perpendicularRuns.sh
  Takes a path as argument and recursively looks for all AMS root files in this path that correspond to perpendicular runs
  
_ createSubmissionScript.sh
  Create and a customized LSF job launcher script
  
_ ReduceSample
  Takes a root file and a TCut as argument and produces a smaller root file based on the TCut
