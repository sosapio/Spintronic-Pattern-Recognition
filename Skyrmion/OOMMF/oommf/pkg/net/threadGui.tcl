# FILE: threadGui.tcl
#
# This class is superseded by Net_ThreadWish. Net_ThreadGui is only used
# by mmSolve2d and batchsolve. Note that Net_ThreadGui runs directly in
# the mmLaunch Tcl interpreter, unlike Net_ThreadWish which runs its gui
# in a slave interp.
#
# The output schedule selection values and counts are stored in the
# global variables _cb$thread and _cnt$thread, respectively, where
# $thread is the Net_Thread object connected to a mmsolve2d or
# batchsolve solver. Because _cb$thread and _cnt$thread are global,
# their values are preserved across Net_ThreadGui destructions and
# constructions which occur whenever the GUI interface button in
# mmLaunch is toggled. Elements of the _cb$thread and _cnt$thread arrays
# are initialized and tied to Ow_EntryBox widgets inside method
# DisplaySchedule. Elements of these arrays are deleted inside method
# DisplaySchedule when the corresponding server is not found inside the
# instance variable io_server. This is the only code that unsets
# elements of these arrays. In particular, this means that _cb$thread
# and _cnt$thread can only be modified while a Net_ThreadGui is active
# on $thread.

package require Tk
package require Ow

Oc_Class Net_ThreadGui {
    const public variable winpath
    const public variable thread
    private variable inputs
    private variable ctrl
    private variable cmdbtns
    private variable outputs
    private variable dataselection
    private variable indataselection
    private variable threadselection
    private variable ithreadselection
    private variable dt
    private variable idt
    private variable schedw
    private array variable opdb = {}
    private array variable ipdb = {}
    private array variable evdb = {}
    private array variable button = {}
    private array variable io_servers
    private variable status = "Status: "
    Constructor {args} {
	eval $this Configure -winpath .w$this $args
        toplevel $winpath
        wm group $winpath
	regexp {[0-9]+} [$thread Cget -pid] id
        wm title $winpath <$id>[$thread Cget -alias]
	after idle "Ow_SetIcon $winpath"

        # Input control panel
        set inputs [frame $winpath.inputs -bd 4 -relief ridge]
        pack $inputs -side top -fill both -expand 1

        # Runtime control panel with exit button
        set ctrl [frame $winpath.ctrl -bd 4 -relief ridge]
        pack [label $ctrl.label -textvariable [$this GlobalName status] \
		-width 60 -anchor w -relief groove] -side top -in $ctrl -fill x
        pack $ctrl -side top -fill both -expand 1
	set exitCmd [list $thread Send exit]
	if {[string match 0.0 [$thread ProtocolVersion]]} {
	    set exitCmd [list $thread Send delete]
	}
        pack [button $ctrl.exit -text Exit -command $exitCmd] \
		-in $ctrl -side right -fill both

        # Output schedule control panel
        set outputs [frame $winpath.outputs -bd 4 -relief ridge]
        pack $outputs -side top -fill both -expand 1
        bind $winpath <Destroy> "+$this WinDestroy %W"
        $this ResetInterface
    }
    method ResetInterface {} {
        # Note: Send messages on a connection are processed in order of
        #       receipt, so the first reply will be to TrackIoServers.
        #       This allows io_servers to be filled before setting up
        #       the gui.
        #          In principle, Tell messages generated by the solver
        #       may precede replies to Send requests, so we don't enable
        #       '$thread Readable' handling until the TrackIoServers
        #       response is received, i.e., the '$thread Readable' is
        #       set inside method ReceiveIoServers.
        Oc_EventHandler DeleteGroup $this
        Oc_EventHandler New _ $thread Reply[$thread Send TrackIoServers] \
                [list $this ReceiveIoServers] -oneshot 1 -groups [list $this]
        Oc_EventHandler New _ $thread Reply[$thread Send Commands] \
                [list $this ReceiveCommands] -oneshot 1 -groups [list $this]
        Oc_EventHandler New _ $thread Reply[$thread Send Outputs] \
                [list $this ReceiveOutputs] -oneshot 1 -groups [list $this]
        Oc_EventHandler New _ $thread Reply[$thread Send Inputs] \
                [list $this ReceiveInputs] -oneshot 1 -groups [list $this]
        Oc_EventHandler New _ $thread Reply[$thread Send Events] \
                [list $this ReceiveEvents] -oneshot 1 -groups [list $this]
        Oc_EventHandler New _ $thread Reply[$thread Send GetSolverState] \
                [list $this ReceiveSolverState] -oneshot 1 -groups [list $this]
        catch {unset opdb}
        catch {unset ipdb}
        catch {unset evdb}
        catch {unset io_servers}
        array set opdb {}
        array set ipdb {}
        array set evdb {}
    }
    method Receive {} {
        # "Tell" messages from app come here
        set message [$thread Get]
	switch [lindex $message 0] {
	    InterfaceChange {
		$this ResetInterface
	    }
            NewIoServer {
               set data [lindex $message 1]
               foreach {sid details} $data break
               set io_servers($sid) $details
               # Index is the sid (oid:servernumber, e.g., 5:0), and the
               # value is the list
               #   { advertisedname port fullprotocol hostname accountname }
               # Note: fullprotocol looks like
               #   "OOMMF vectorField protocol 0.1"
               # The short form of the protocol, used as "type" elsewhere in
               # this class, is just "vectorField".
               $this UpdateGui
            }
            DeleteIoServer {
               unset -nocomplain io_servers([lindex $message 1])
               $this UpdateGui
            }
	    Status {
		set status "Status: [join [lrange $message 1 end]]"
	    }
	}
    }
    method ReceiveIoServers {} {
       foreach {errcode msg} [$thread Get] break
       if {!$errcode} { ;# Check for error
          # msg contains sid + detail pairs for every I/O server that
          # meets the filter constraint specified in the Gui
          # server. Store this information in io_servers.
          # Compare to the NewIoServer handling in method Receive.
          unset -nocomplain io_servers
          array set io_servers $msg
       } else {
          # What to do???
          array set io_servers {}
       }
       # To ensure we don't miss any new server or server delete messages,
       # enable "Tell" message handling immediately.
       Oc_EventHandler New _ $thread Readable [list $this Receive] \
          -groups [list $this]
    }
    method ReceiveCommands {} {
       set cmds [$thread Get]
       if {![lindex $cmds 0]} { ;# Check for error
          set min_label_width 75
          if {[info exists cmdbtns] && [winfo exists $cmdbtns]} {
             destroy $cmdbtns
          }
          set cmdbtns [frame $ctrl.cmdbtns]
          foreach {l group} [lindex $cmds 1] {
             frame $cmdbtns.w$l
             frame $cmdbtns.w$l.labelframe
             frame $cmdbtns.w$l.labelspacer -width $min_label_width
             label $cmdbtns.w$l.label -text "${l}:"
             pack $cmdbtns.w$l.labelspacer -in $cmdbtns.w$l.labelframe \
                 -side top
             pack $cmdbtns.w$l.label -in $cmdbtns.w$l.labelframe \
                 -side right -anchor e
             pack $cmdbtns.w$l.labelframe -in $cmdbtns.w$l \
                 -side left -fill both
             foreach c $group {
                set text [lindex $c 0]
                # This might impose restriction that labels contain no spaces
                pack [button $cmdbtns.w$l.w$text -text $text \
                          -command [concat $thread Send [lrange $c 1 end]]] \
                    -in $cmdbtns.w$l -side left -fill both
             }
             pack $cmdbtns.w$l -in $cmdbtns -side top -fill x
          }
          pack $cmdbtns -in $ctrl -side top -fill x
       }
    }
    method ReceiveOutputs {} {
        set op [$thread Get]
        if {![lindex $op 0]} { ;# Check for error
            foreach o [array names opdb] {
                unset opdb($o)
            }
            foreach {o desc} [lindex $op 1] {
                array set opdb [list $o $desc]
            }
            $this DisplayInteractiveOutputButtons
	    $this DisplayOutputPanel
        }
    }
    method ReceiveInputs {} {
        set ip [$thread Get]
        if {![lindex $ip 0]} { ;# Check for error
            foreach o [array names ipdb] {
                unset ipdb($o)
            }
            foreach {o desc} [lindex $ip 1] {
                array set ipdb [list $o $desc]
            }
	    $this DisplayInputPanel
        }
    }
    method ReceiveSolverState {} {
        set msg [$thread Get]
        if {![lindex $msg 0]} { ;# Check for error
           set status "Status: [lindex $msg 1]"
        }
        # This is the last of the initialization Send message "Receive" handlers.
        # Make sure GUI is initialized.
        $this UpdateGui
    }
    method DisplayOutputPanel {} {
	destroy $outputs.label
	if {[llength [array names opdb]]} {
	    if {![info exists button(so)]} {
		set button(so) 1
	    }
	    pack [checkbutton $outputs.label -text "Scheduled Outputs" \
		    -variable [$this GlobalName button](so) \
		    -command [list $this DisplayOutputs] \
		    -relief groove] \
		    -side top -in $outputs -fill x
	} else {
	    # This is not right, but it only breaks the case where an
	    # app reports no Outputs, then some Outputs.  No apps do that.
	    set button(so) 0
	}
	$this DisplayOutputs
    }
    method DisplayInteractiveOutputButtons {} {
        if {[winfo exists $ctrl.io]} {
            destroy $ctrl.io
        }
	if {![array size opdb]} {return}
        frame $ctrl.io
	frame $ctrl.io.labelframe
	frame $ctrl.io.labelspacer -width 75
	label $ctrl.io.label -text "Interactive\nOutputs:" -justify right
	pack $ctrl.io.labelspacer -in $ctrl.io.labelframe -side top
	pack $ctrl.io.label -in $ctrl.io.labelframe -side right -anchor e
        pack $ctrl.io.labelframe -in $ctrl.io -side left -fill both
        foreach o [array names opdb] {
            pack [button $ctrl.io.w$o -text $o \
                -command [list $this InteractiveOutput $o]] \
                -in $ctrl.io -side left -fill both
        }
        pack $ctrl.io -side bottom -fill both -expand 1
    }
    method DisplayInputPanel {} {
	destroy $inputs.label
	if {[llength [array names ipdb]]} {
	    if {![info exists button(in)]} {
		set button(in) 1
	    }
	    pack [checkbutton $inputs.label -text "Inputs" \
		    -variable [$this GlobalName button](in) \
		    -command [list $this DisplayInputs] \
		    -relief groove] \
		    -side top -in $inputs -fill x
	} else {
	    # This is not right, but it only breaks the case where an
	    # app reports no Inputs, then some Inputs.  No apps do that.
	    set button(in) 0
	}
	$this DisplayInputs
    }
    method DisplayInputs {} {
        destroy $inputs.data
	if {!$button(in)} {
	    $this DisplayInputThreads ""
	    return
	}
        set data [frame $inputs.data -bd 4 -relief ridge]
        pack [label $data.label -text "Inputs" -relief groove] \
                -side top -in $data -fill x
        foreach o [array names ipdb] {
           set desc $ipdb($o)
           radiobutton $data.w$o -text $o -value $o \
               -variable [$this GlobalName indataselection] \
               -anchor w \
               -command [list $this DisplayInputThreads [lindex $desc 0]]
           catch {$data.w$o configure -tristatevalue "tristatevalue"}
           # Disable Tk 8.5+ tristate mode.
           pack $data.w$o -side top -in $data -fill x
        }
        pack $data -side left -fill y
        if {[array size ipdb]} {
            set indataselection [lindex [array names ipdb] 0]
        } else {
            catch {unset indataselection}
        }
        $this DisplayInputThreads ""
    }
    method DisplayOutputs {} {
        if {[winfo exists $outputs.data]} {
            destroy $outputs.data
        }
        if {!$button(so)} {
            $this DisplayThreads ""
            return
        }
        set data [frame $outputs.data -bd 4 -relief ridge]
        pack [label $data.label -text "Outputs" -relief groove] \
                -side top -in $data -fill x
        foreach o [array names opdb] {
           set desc $opdb($o)
           radiobutton $data.w$o -text $o -value $o \
              -variable [$this GlobalName dataselection] \
              -anchor w \
              -command [list $this DisplayThreads [lindex $desc 0]]
           catch {$data.w$o configure -tristatevalue "tristatevalue"}
           # Disable Tk 8.5+ tristate mode.
           pack $data.w$o -side top -in $data -fill x
        }
        pack $data -side left -fill y
        if {[array size opdb]} {
            set dataselection [lindex [array names opdb] 0]
        }
        $this DisplayThreads ""
    }
    method ReceiveEvents {} {
        set ev [$thread Get]
        if {![lindex $ev 0]} { ;# Check for error
        foreach {e val} [lindex $ev 1] {
            array set evdb [list $e $val]
        }
        }
    }
    method DisplayThreads { type } {
        if {[info exists dt]} {
            destroy $dt
            unset dt
        }
        if {[info exists schedw]} {
            destroy $schedw
            unset schedw
        }
        if {!$button(so)} {
            $this DisplaySchedule
            return
        }
        if {![string length $type] && [info exists dataselection] \
                && [string length $dataselection]} {
            set desc $opdb($dataselection)
            set type [lindex $desc 0]
        }
        set dt [frame $outputs.dt -bd 4 -relief ridge]
        pack [label $dt.label -text "Destination Threads" -relief groove] \
                -side top -in $dt -fill x

        set sidapplist {}
        foreach sid [array names io_servers] {
           foreach { appname port protocol hostname acctname } \
              $io_servers($sid) break
           set protocol [lindex $protocol 1]  ;# Short form
           if {[string compare $type $protocol]==0} {
              lappend sidapplist [list $sid $appname]
           }
        }
        foreach sidapp [lsort -dictionary $sidapplist] {
           foreach {sid appname} $sidapp break
           regexp {^[0-9]+} $sid oid
           set btn [radiobutton $dt.w$sid \
                       -text <$oid>$appname \
                       -value $sid -anchor w \
                       -command [list $this DisplaySchedule] \
                       -variable [$this GlobalName threadselection]]
           catch {$btn configure -tristatevalue "tristatevalue"}
           # Tk 8.5 introduces a tristate mode, which we don't use.
           # Change default tristatevalue from "" to "tristatevalue"
           # so that tristate mode is not accidentally selected by
           # mousing over the radiobutton.
           pack $btn -side top -in $dt -fill x
        }

        pack $dt -side left -fill both -expand 1
        catch {unset threadselection}
        $this DisplaySchedule
    }
    method DisplayInputThreads { type } {
        if {[info exists idt]} {
            destroy $idt
            unset idt
        }
        set idt [frame $inputs.dt -bd 4 -relief ridge]
        pack [label $idt.label -text "Source Threads" -relief groove] \
                -side top -in $idt -fill x
	if {!$button(in)} {
	    return
	}
        if {![string length $type] && [info exists indataselection] \
                && [string length $indataselection]} {
            set desc $ipdb($indataselection)
            set type [lindex $desc 0]
        }
        foreach sid [array names io_servers] {
           foreach { appname port protocol hostname acctname } \
              $io_servers($sid) break
           set protocol [lindex $protocol 1]  ;# Short form
           if {[string compare $type $protocol]==0} {
              regexp {^[0-9]+} $sid oid
              set btn [radiobutton $idt.w$sid \
                          -text ${appname}<$oid> \
                          -value $sid -anchor w \
                          -command [list $this UpdateInput] \
                          -variable [$this GlobalName ithreadselection]]
              catch {$btn configure -tristatevalue "tristatevalue"}
              # Disable Tk 8.5+ tristate mode.
              pack $btn -side top -in $idt -fill x
           }
        }
        pack $idt -side left -fill both -expand 1
        Ow_PropagateGeometry $idt
        catch {unset ithreadselection}
    }
    method DisplaySchedule {} {
        if {[info exists schedw]} {
            destroy $schedw
            unset schedw
        }
        global _cb$thread _cnt$thread
        set active_servers [array names io_servers]
        foreach iii [array names _cb$thread *,*,*] {
            regexp {([^,]*),([^,]*),([^,]*)} $iii match d t e
            if {[lsearch -exact $active_servers $t]<0} {
               unset _cb${thread}($iii)
            }
        }
        foreach iii [array names _cnt$thread *,*,*] {
            regexp {([^,]*),([^,]*),([^,]*)} $iii match d t e
            if {[lsearch -exact $active_servers $t]<0} {
               unset _cnt${thread}($iii)
            }
        }
        if {!$button(so)} {
            if {[info exists winpath]} {
               Ow_PropagateGeometry $winpath
            }
            return
        }
        set schedw [frame $outputs.sched -bd 4 -relief ridge]
	grid [label $schedw.label -text "Schedule" -relief groove] - \
		-sticky new
	grid columnconfigure $schedw 0 -pad 15
	grid columnconfigure $schedw 1 -weight 1
        if {[info exists dataselection] && [string length $dataselection]} {
           set opinfo $opdb($dataselection)
           if {[info exists threadselection] && \
                  [string length $threadselection]} {
              foreach ev [lindex $opinfo 1] {
                 set btn [checkbutton $schedw.w$ev -text $ev -anchor w \
                  -variable _cb${thread}($dataselection,$threadselection,$ev) \
                  -command [list $this UpdateSchedule $ev]]
                 global _cnt$thread
                 if {![info exists \
                       _cnt${thread}($dataselection,$threadselection,$ev)]} {
                    set _cnt${thread}($dataselection,$threadselection,$ev) 1
                 }
                 set evinfo $evdb($ev)
                 if {[lindex $evinfo 0]} {
                    Ow_EntryBox New _ $schedw.cnt$ev -label "every" \
                       -autoundo 0 -valuewidth 4 -variable \
                       _cnt${thread}($dataselection,$threadselection,$ev) \
                       -callback [list $this UpdateScheduleB $ev] \
                       -valuetype posint -coloredits 1 -writethrough 0 \
                       -outer_frame_options "-bd 0"
                    grid $btn [$_ Cget -winpath] -sticky nw
                 } else {
                    grid $btn -sticky nw
                 }
              }
              set ibtn [checkbutton $schedw.ibtn -text Interactive -anchor w \
                 -variable \
                   _cb${thread}($dataselection,$threadselection,interactive) \
                 -command [list $this UpdateSchedule interactive]]
              set _cnt${thread}($dataselection,$threadselection,interactive) 0
              grid $ibtn -sticky nw
           }
        }
	set rowcount [lindex [grid size $schedw] 1]
	grid rowconfigure $schedw [expr $rowcount-1] -weight 1
        pack $schedw -side right -fill both -expand 1
        Ow_PropagateGeometry $schedw
    }
    method UpdateSchedule { event } {
        global _cb$thread
        global _cnt$thread
        set sid $threadselection
        foreach { appname port protocol hostname acctname } \
           $io_servers($sid) break
        if {[set _cb${thread}($dataselection,$sid,$event)]} {
            $thread Send SetHandler $event $dataselection \
              $hostname $acctname $sid \
              [set _cnt${thread}($dataselection,$sid,$event)]
        } else {
            $thread Send UnsetHandler $event $dataselection \
               $hostname $acctname $sid
        }
    }
    method UpdateScheduleB { event widget args } {
	upvar #0 [$widget Cget -variable] var
	set var [$widget Cget -value]
	$this UpdateSchedule $event
    }
    method UpdateInput {} {
        set sid $ithreadselection
        foreach { appname port protocol hostname acctname } \
           $io_servers($sid) break
        $thread Send SetInput $indataselection \
           $hostname $acctname $sid
    }
    method UpdateGui {} {
       set itypes {}
       foreach name [array names ipdb] {
          lappend itypes [lindex $ipdb($name) 0]
       }
       set itypes [lsort -unique $itypes]  ;# Remove dups
       set otypes {}
       foreach name [array names opdb] {
          lappend otypes [lindex $opdb($name) 0]
       }
       set otypes [lsort -unique $otypes]  ;# Remove dups
       foreach t $itypes { $this DisplayInputThreads $t }
       foreach t $otypes { $this DisplayThreads $t }
       $this DisplayInputThreads ""
       $this DisplayThreads ""
    }
    method InteractiveOutput { data } {
	$thread Send InteractiveOutput $data
    }
    private variable delete_in_progress = 0
    method WinDestroy { w } { winpath thread delete_in_progress } {
	if {[string compare $winpath $w]!=0} { return } ;# Child destroy event
	if { [info exists delete_in_progress] && \
		$delete_in_progress } { return }
        upvar #0 [$thread GlobalName btnvar] tmp
        set tmp 0
        $thread ToggleGui
    }
    Destructor {
	# Protect against re-entrancy
	if { [info exists delete_in_progress] && \
		$delete_in_progress } { return }
	set delete_in_progress 1

        Oc_EventHandler DeleteGroup $this
	# Delete everything else
	if {[info exists winpath] && [winfo exists $winpath]} {
            # bind $winpath <Destroy> {}
	    # Destroy instance frame, all children, and bindings
	    # Rely on the delete_in_progress semaphore to protect
	    # against re-entrancy, both here and inside WinDestroy.
	    destroy $winpath
	}
    }
}
