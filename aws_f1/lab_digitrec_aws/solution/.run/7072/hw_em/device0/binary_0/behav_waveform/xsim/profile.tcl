#
# File: profile.tcl
# Description: Script to analyze HW emulation results
# Created: 2019-10-04 21:25:32
#

global ofp
global ofp2

# Log results (file 1 for summary)
proc putsLog {str} {
  global ofp

#  puts $str
  puts $ofp $str
}

# Log results (file 2 for trace)
proc putsLog2 {str} {
  global ofp2

#  puts $str
  puts $ofp2 $str
}

# Get end time
proc getEndTime {} {
  return [lindex [now] 0]
}

# Get total time of signal at a specified value
proc getTimeUsec {signal targetValue {requireEnd 0}} {
  if {[get_objects -quiet $signal] == {}} {
    return 0
  }
  set tranx [get_transitions $signal -start 0 -end [getEndTime]]

  set otherValue [expr 1 - $targetValue]
  set startTime 0
  set totalTimeUsec 0
  set factor 6.0

  for { set i 0 } { $i < [llength $tranx] } { incr i } {
    set tr [lindex $tranx $i]
    set currTime  [lindex $tr 0]
    set currUnit  [lindex $tr 1]
    set currValue [lindex $tr 2]
    if {[string first ns $currUnit] == 0} {set factor 3.0}

    if {$currValue == $targetValue} {
      set startTime $currTime
    } elseif {$currValue == $otherValue && $startTime > 0} {
      set totalTimeUsec [expr $totalTimeUsec + (($currTime - $startTime) / pow(10.0, $factor))]
      set startTime 0
    }
  }

  if {$totalTimeUsec == 0 && $requireEnd} {
    set totalTimeUsec [expr (([getEndTime] - $startTime) / pow(10.0, $factor))]
  }
  return $totalTimeUsec
}

# Get events of signal at a specified value
proc getTraceEvents {kernelName cuName moduleName signal targetValue {requireEnd 0}} {
  if {[get_objects -quiet $signal] == {}} {
    return
  }
  set tranx [get_transitions $signal -start 0 -end [getEndTime]]

  set otherValue [expr 1 - $targetValue]
  set startTime 0
  set factor 9.0
  set currDeviceName xilinx_aws-vu9p-f1-04261818_dynamic_5_0-0
  set currBinaryName DigitRec.hw_emu

  for { set i 0 } { $i < [llength $tranx] } { incr i } {
    set tr [lindex $tranx $i]
    set currTime  [lindex $tr 0]
    set currUnit  [lindex $tr 1]
    set currValue [lindex $tr 2]
    if {[string first ns $currUnit] == 0} {set factor 6.0}

    if {$currValue == $targetValue} {
      set startTime $currTime
    } elseif {$currValue == $otherValue && $startTime > 0} {
      set startTimeMsec [expr $startTime / pow(10.0, $factor)]
      set endTimeMsec [expr $currTime / pow(10.0, $factor)]
      set traceStr [format "%s,%s,%s,%s,%s,%s,0,%.6f,%.6f,0" $currDeviceName $currBinaryName $kernelName $cuName $moduleName $moduleName $startTimeMsec $endTimeMsec]
      putsLog2 $traceStr
      set startTime 0
    }
  }
}

# Get events of signal at a specified value
proc getTraceEvents2 {kernelName cuName moduleName type index signal} {
  if {[get_objects -quiet $signal] == {}} {
    puts "Unable to find signal: $signal"
    return
  }
  set tranx [get_transitions $signal]

  set factor 9.0
  set newValue 0
  if {$type == "row"} {
      set newValue 65535
  }
  set startTimeMsec 0
  set currDeviceName xilinx_aws-vu9p-f1-04261818_dynamic_5_0-0
  set currBinaryName DigitRec.hw_emu
  set hasData 0 

  for { set i 0 } { $i < [llength $tranx] } { incr i } {
    set tr [lindex $tranx $i]
    set currTime   [lindex $tr 0]
    if {$currTime == 0} {
      continue
    }
    set currUnit   [lindex $tr 1]
    if {[string first ns $currUnit] == 0} {set factor 6.0}
    set timeMsec [expr $currTime / pow(10.0, $factor)]

    set traceStr [format "%s,%s,%s,%s,%s,%s,%d,%.6f,%.6f,%d" $currDeviceName $currBinaryName $kernelName $cuName $moduleName $type $index $startTimeMsec $timeMsec $newValue]
    putsLog2 $traceStr

    set currValue [lindex $tr 2]
    set currValueInt [expr 0x$currValue]
    if {$type == "row"} {
      set newValue $currValueInt
    } else {
      set newValue [expr {($currValueInt == 65535) ? 0 : $currValueInt}]
    }
    set startTimeMsec $timeMsec
    set hasData 1 
  }

  # Force last value to be 0
  if {$hasData == 1} { 
    set endValue 0
    if {$type == "row"} { set endValue 65535 }
    set traceStr [format "%s,%s,%s,%s,%s,%s,%d,%.6f,%.6f,%d" $currDeviceName $currBinaryName $kernelName $cuName $moduleName $type $index  $startTimeMsec $startTimeMsec $endValue]
    putsLog2 $traceStr
  }
}

# Get hier path inside CU (HLS bug)
proc getHierName {cuName} {
  set hierName inst
  if {[get_objects -quiet /emu_wrapper/emu_i/$cuName/$hierName/ap_idle] == {}} {
    set hierName U0
  }
  return $hierName
}

set outFilename sdaccel_profile_kernels.csv
set ofp [open $outFilename w]
set outFilename2 sdaccel_timeline_kernels.csv
set ofp2 [open $outFilename2 w]

#
# Compute Unit Stalls
#
set computeUnits { DigitRec_1 }

putsLog "Compute Units: Running Time and Stalls"
putsLog "Compute Unit, Running Time (us), Intra-Kernel Dataflow Stalls (%), External Memory Stalls (%), Inter-Kernel Pipe Stalls (%)"

for { set i 0 } { $i < [llength $computeUnits] } { incr i } {
  set cuName [lindex $computeUnits $i]
  set hierName [getHierName $cuName]

  # Runtime
  set signal /emu_wrapper/emu_i/$cuName/$hierName/ap_idle
  set cuTime [getTimeUsec $signal 0 1]

  # Stall times
  set signal /emu_wrapper/emu_i/$cuName/$hierName/ap_int_blocking_n
  set intStallTime [getTimeUsec $signal 0]
  set intStallPercent [expr {$cuTime == 0} ? 0 : min((100.0 * $intStallTime) / $cuTime, 100.0)]

  set signal /emu_wrapper/emu_i/$cuName/$hierName/ap_ext_blocking_n
  set extStallTime [getTimeUsec $signal 0]
  set extStallPercent [expr {$cuTime == 0} ? 0 : min((100.0 * $extStallTime) / $cuTime, 100.0)]

  set signal /emu_wrapper/emu_i/$cuName/$hierName/ap_str_blocking_n
  set strStallTime [getTimeUsec $signal 0]
  set strStallPercent [expr {$cuTime == 0} ? 0 : min((100.0 * $strStallTime) / $cuTime, 100.0)]

  set logStr [format "%s,%.3f,%.3f,%.3f,%.3f" $cuName $cuTime $intStallPercent $extStallPercent $strStallPercent]
  putsLog $logStr
}
putsLog ""

#
# Function Stalls
#
set computeUnits { DigitRec_1 DigitRec_1 }
set modules { grp_default_function_fu_334 grp_knn_vote_fu_341 }
set realModules { default_function knn_vote }

putsLog "Functions: Running Time and Stalls"
putsLog "Compute Unit, Function, Running Time (us), Intra-Kernel Dataflow Stalls (%), External Memory Stalls (%), Inter-Kernel Pipe Stalls (%)"

for { set i 0 } { $i < [llength $computeUnits] } { incr i } {
  set cuName [lindex $computeUnits $i]
  set moduleName [lindex $modules $i]
  set moduleRealName [lindex $realModules $i]

  # Runtime
  # NOTE: not all modules have ap_start/ap_idle
  set cuSignal /emu_wrapper/emu_i/$cuName/inst/ap_idle
  set moduleSignal /emu_wrapper/emu_i/$cuName/inst/$moduleName/ap_idle
  if {[get_objects -quiet $moduleSignal] == {}} {
    set moduleSignal $cuSignal
  }
  set cuTime [getTimeUsec $cuSignal 0 1]
  set moduleTime [getTimeUsec $moduleSignal 0 1]

  # Stall times
  set signal /emu_wrapper/emu_i/$cuName/inst/$moduleName/ap_int_blocking_n
  set intStallTime [getTimeUsec $signal 0]
  set intStallPercent [expr {$cuTime == 0} ? 0 : min((100.0 * $intStallTime) / $cuTime, 100.0)]

  set signal /emu_wrapper/emu_i/$cuName/inst/$moduleName/ap_ext_blocking_n
  set extStallTime [getTimeUsec $signal 0]
  set extStallPercent [expr {$cuTime == 0} ? 0 : min((100.0 * $extStallTime) / $cuTime, 100.0)]

  set signal /emu_wrapper/emu_i/$cuName/inst/$moduleName/ap_str_blocking_n
  set strStallTime [getTimeUsec $signal 0]
  set strStallPercent [expr {$cuTime == 0} ? 0 : min((100.0 * $strStallTime) / $cuTime, 100.0)]

  set logStr [format "%s,%s,%.3f,%.3f,%.3f,%.3f" $cuName $moduleName $moduleTime $intStallPercent $extStallPercent $strStallPercent]
  putsLog $logStr
}
putsLog ""

#
# Compute Unit Ports
#
set computeUnits { DigitRec_1 }
set ports { m_axi_gmem }
set readBusySignals { /emu_wrapper/emu_i/xilmonitor_DigitRec_1_m_axi_gmem/inst/readActive }
set writeBusySignals { /emu_wrapper/emu_i/xilmonitor_DigitRec_1_m_axi_gmem/inst/writeActive }

putsLog "Compute Units: Port Data Transfer"
putsLog "Compute Unit, Port, Write Time (us), Outstanding Write (%), Read Time (us), Outstanding Read (%)"

for { set i 0 } { $i < [llength $computeUnits] } { incr i } {
  set cuName [lindex $computeUnits $i]
  set hierName [getHierName $cuName]
  set portName [lindex $ports $i]
  set readBusy [lindex $readBusySignals $i]
  set writeBusy [lindex $writeBusySignals $i]

  # Runtime
  set signal /emu_wrapper/emu_i/$cuName/$hierName/ap_idle
  set moduleTime [getTimeUsec $signal 0 1]

  # Write times
  set writeTime [getTimeUsec $writeBusy 1]
  set writePercent [expr {$moduleTime == 0} ? 0 : min((100.0 * $writeTime) / $moduleTime, 100.0)]

  # Read times
  set readTime [getTimeUsec $readBusy 1]
  set readPercent [expr {$moduleTime == 0} ? 0 : min((100.0 * $readTime) / $moduleTime, 100.0)]

  set logStr [format "%s,%s,%.3f,%.3f,%.3f,%.3f" $cuName $portName $writeTime $writePercent $readTime $readPercent]
  putsLog $logStr
}
putsLog ""

#
# Trace Events
#
puts "Waiting for protocol analyzers to complete..."
sim_wait_for_processing
puts "Done!"

putsLog2 "Kernel trace events"
putsLog2 "Device, Binary, Kernel, Compute Unit, Function, Data Type, Index, Start Time (ms), End Time (ms), Value"
set kernelsTop { DigitRec }
set kernels { DigitRec DigitRec }
set computeUnitsTop { DigitRec_1 }
set computeUnits { DigitRec_1 DigitRec_1 }
set modules { grp_default_function_fu_334 grp_knn_vote_fu_341 }
set realModules { grp_default_function_fu_334 grp_knn_vote_fu_341 }

for { set i 0 } { $i < [llength $computeUnitsTop] } { incr i } {
  set kernelName [lindex $kernelsTop $i]
  set cuName [lindex $computeUnitsTop $i]
  set signal /emu_wrapper/emu_i/$cuName/inst/ap_idle
  getTraceEvents $kernelName $cuName top $signal 0 1
}

for { set i 0 } { $i < [llength $computeUnits] } { incr i } {
  set kernelName [lindex $kernels $i]
  set cuName [lindex $computeUnits $i]
  set moduleName [lindex $modules $i]
  set moduleRealName [lindex $realModules $i]
  set signal /emu_wrapper/emu_i/$cuName/inst/$moduleName/$moduleRealName.dataflowTxWaveData.OutStanding
  getTraceEvents2 $kernelName $cuName $moduleName outstanding 0 $signal

  for { set j 0 } { $j < 64 } { incr j } {
    set signal /emu_wrapper/emu_i/$cuName/inst/$moduleName/$moduleRealName.__rowsignals_[$j]
    getTraceEvents2 $kernelName $cuName $moduleName row $j $signal
  }
}

close $ofp
close $ofp2
