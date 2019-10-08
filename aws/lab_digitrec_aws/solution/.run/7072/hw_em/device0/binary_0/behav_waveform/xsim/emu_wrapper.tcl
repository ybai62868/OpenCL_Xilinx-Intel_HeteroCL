log_wave -r *
if { [info exists ::env(SDX_WAVEFORM) ] } {
  if {[file exists $::env(SDX_WAVEFORM)] == 1} {
    open_wave_config $::env(SDX_WAVEFORM)
  }
}
if { [file exists preprocess_profile.tcl] } {
  source -notrace preprocess_profile.tcl
}
puts "We are running Simulator for infinite time. Added some default signals in the waveform. You can pause simulation and add signals and then resume the simulaion again."
puts ""
puts "Stopping at breakpoint in simulator also stops the host code execution"
puts ""
if { [info exists ::env(SDX_LAUNCH_WAVEFORM_GUI) ] } {
  run 1ns
} else {
  run all
}
if { [file exists profile.tcl] } {
  source -notrace profile.tcl
}
if { [info exists ::env(SDX_LAUNCH_WAVEFORM_BATCH) ] } {
quit
}
