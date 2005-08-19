$ set noverify
$!  Copyright (c) 1988 Digital Equipment Corporation.  All rights reserved.
$on control_y then goto bad_kit
$on warning then goto bad_kit
$ask = "inquire/nopunctuation"
$say = "write sys$output"
$type = "type/nopage"
$backup = "backup/noassist"
$remount = 0
$label_cnt = 10
$label_num = 1
$say "''f$fao("!/!/!_VAX/VMS V5.0 Software Product Kit Build Procedure!/")"
$p = "TMPMBX"
$saved_privs = f$setprv(p)
$if .not. f$priv(p)
$then
$say "You need the following privileges: ",p
$goto bad_kit
$endif
$if p1 .nes. "" then goto m5
$type sys$input:

    Kits have names in the format "facvvu", where

    	fac	is the facility name
    	vv	is the major version number
    	u	is the update number


$ask p1 "Which kit would you like to build (facvvu): "
$m5: facility = f$ext(0,f$len(p1)-3,p1)
$version = 'f$ext(f$len(p1)-3,2,p1)
$update = 'f$ext(f$len(p1)-1,1,p1)
$dest = p2
$if dest .eqs. "" then ask dest "On which device is the kit to be built: "
$if f$loc(":",dest) .eq. f$len(dest) then dest = dest + ":"
$device = f$parse(dest,,,"DEVICE","SYNTAX_ONLY")
$destdir = f$parse(dest,"[0,0]",,"DIRECTORY","SYNTAX_ONLY")
$dest = device + destdir
$if f$loc("CSA",device) .ne. f$len(device) then goto m7
$if f$getdvi(dest,"EXISTS") then -
if f$getdvi(dest,"DEVCLASS") .eq. 1 .or. -
f$getdvi(dest,"DEVCLASS") .eq. 2 then goto m7
$say "Device must be a disk or tape."
$goto bad_kit
$m7:
$if f$getdvi(dest,"DEVCLASS") .eq. 1
$then chk_size = 39
$else chk_size = 15
$endif
$if f$len(p1) .gt. chk_size then goto bad_name_size
$remount = f$loc("CSA",device) .ne. f$len(device)
$if .not. remount then goto m9
$p = "CMKRNL"
$saved_privs = saved_privs + "," + f$setprv(p)
$if .not. f$priv(p)
$then
$say "You need ''p' privilege to use ''device'"
$goto bad_kit
$endif
$s = "$sysgen"
$s connect console
$if f$getdvi(device,"MNT") then dismount 'device
$allocate 'device
$m9:
$lim = 12
$if f$getdvi(device,"DEVCLASS") .eq. 2 then lim = 6
$if f$getdvi(device,"DEVCLASS") .eq. 1 .and. -
f$getdvi(device,"MNT") .and. -
.not. f$getdvi(device,"FOR") then goto m15
$if f$getdvi(device,"MNT") .and. f$getdvi(device,"FOR") .and. -
f$getdvi(device,"DEVCLASS") .eq. 2 then goto m_tape
$if f$getdvi(device,"MNT") then dismount/nounload 'device
$say "Please ready the first distribution volume on ''device."
$m10: ask w "Are you ready (Y/N)? "
$if .not. w then goto m10
$mount/foreign/noassist 'device
$remount = 1
$m15: if f$getdvi(device,"DEVCLASS") .eq. 2 then goto m_TAPE
$if f$getdvi(device,"MAXBLOCK") .gt. 8*512 then goto m_DISK
$m_CONSOLE:
$backup_quals = "/init/block=9000/group=25"
$if f$len(facility) .gt. lim-2 then facility = f$ext(0,lim-2,facility)
$gosub def_labels
$goto m20
$m_DISK:
$backup_quals = "/block=9000/group=25"
$labels = facility
$goto m19
$m_TAPE:
$backup_quals = "/rewi/dens=1600"
$labels = facility
$if f$len(labels) .gt. lim then labels = f$ext(0,lim,facility)
$ask w "Are there already savesets on this tape (Y/N)? "
$if w
$then backup_quals = "/ign=label/norewi/dens=1600"
$else tmp = f$getdvi(device,"VOLNAM")
$if tmp .nes. labels
$then say ""
$say "    Volume label is not correct ..."
$say ""
$say "         Current volume label: ''tmp'"
$say "        Expected volume label: ''labels'"
$say ""
$m_tape_10:
$ask w "Do you wish to initialize this tape (Y/N)? "
$if w .eqs. "" then goto m_tape_10
$if .not. w then goto bad_kit
$dismount/nounload 'device'
$initialize 'device' 'labels'
$mount/foreign/noassist 'device'
$endif
$endif
$if f$getdvi(device,"DEVTYPE") .eq. 10 then -
backup_quals = backup_quals - "/dens=1600" + "/buffers=5"
$goto m19
$DEF_LABELS:
$cnt = 0
$labels = facility + f$fao("!2ZL",label_num)
$def_labels_10:
$label_num = label_num + 1
$cnt = cnt + 1
$if cnt .lt. label_cnt
$then labels = labels + "," + facility + f$fao("!2ZL",label_num)
$goto def_labels_10
$endif
$return
$m19:
$if f$len(labels) .le. lim then goto m20
$labels = f$ext(0,lim,labels)
$m20:
$say ""
$say "Saveset A must contain the KITINSTAL.COM procedure."
$letters = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
$ssn = 0
$s10:
$ssn = ssn + 1
$ssname = f$ext(ssn-1,1,letters)
$ssid = ssname
$if ssn .gt. 26
$then
$ssname = f$fao("VMI_!4ZL",ssn)
$ssid = f$fao("!4ZL",ssn)
$endif
$say "''f$fao("!/!_!AS V!UL.!UL, Saveset !AS!/",facility,version,update,ssid)"
$f = p3
$if p3 .eqs. "" then ask f "Which files: "
$prot = "(S:RWED,O:RWED,G:RWED,W:RE)"
$say "Resetting protection on all files to ",prot," ..."
$set noon
$set protection='prot 'f
$s12: set noon
$backup/comment="VAX/VMS SPKITBLD Procedure"/inter/log/verify -
'f -
'dest''p1.'ssname/label=('labels)/save'backup_quals
$set on
$say "''f$fao("!/!_Kit Build Completed Successfully!/!/")"
$done:
$on warning then exit
$if .not. remount then goto d9
$if f$getdvi(device,"MNT") then dismount/nounload 'device
$if f$loc("CSA",device) .eq. f$len(device) then goto d9
$say "Please remount the console volume."
$d5: ask w "Are you ready (Y/N)? "
$if .not. w then goto d5
$mount/foreign/system/protection=s:rwlp/nowrite/noassist 'device console
$d9: w = f$setpriv(saved_privs)
$exit
$bad_name_size:
$say ""
$say "    Save set name  ''p1'"
$say "    is too long for the target media."
$say ""
$say "    Maximum length is ''chk_size' characters."
$say ""
$goto bad_kit
$bad_kit:
$type sys$input:

	+---------------------+
	|		      |
	|  THIS IS A BAD KIT  |
	|		      |
	+---------------------+

$goto done
