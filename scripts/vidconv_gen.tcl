### default setting
set Project     vidconv
set Solution    SolutionX
set Device      "xc7k325tffg900-2"
set Flow        ""
set Clock       4
set DefaultFlag 1

#### query qor settings
if {[info exists env(AVE_LOAD_QOR_SETTING)] && $env(AVE_LOAD_QOR_SETTING) == 1} {
    catch {
        source [file join $env(AVE_ROOT) scripts QorSetting.tcl]
        QorSetting::loadSetting ;# this api will overwite the above variables
    }
}

#### main part

# Project settings
open_project $Project -reset

# Add the file for synthesis

add_files ../vidclkconv/.settings/vidconv.cpp

# Add testbench file../slvsrx_lanealigner_tb.cpp
# add_files -tb  ../slvsrx_aligner/slvsrx_lanealigner_tb.cpp
#add_files -tb  tv/tv_multiSRC_in.txt
#for { set i 0 } { $i < 8 } { incr i } { 
#	add_files -tb  tv/tv_multiSRC_mout_$i.txt
#}

# Set top module of the design
set_top vidConv

# Solution settings
open_solution -reset $Solution

# Add library
if {$Device == ""} {
    # default
    add_library xilinx/virtex5/virtex5:xc5vsx50t:ff1136:-1
} else {
    set_part $Device
}

# Set the target clock period
create_clock -period $Clock
set_clock_uncertainty 0.4ns

###############
## Directives #
##############

#################
# C SIMULATION
#################
#csim_design

#############
# SYNTHESIS #
#############
#config_schedule -verbose
csynth_design

#################
# CO-SIMULATION #
#################
#cosim_design -tool modelsim -rtl verilog -trace_level all
#cosim_design -rtl verilog -trace_level all

##################
# IMPLEMENTATION #
##################
export_design -evaluate verilog -format ipxact

#### dump settings for ave coregen stage
if {[info exists env(AVE_LOAD_QOR_SETTING)] && $env(AVE_LOAD_QOR_SETTING) == 1} {
    catch {
        QorSetting::dumpSetting; # this api will dump coregen settings under solution
      }
}

exit