; 
; mail_def.ml
; 
(message "mail_def.ml") (sit-for 10)
(~sys_literal_setup_incr "mail$" 1 1
    "mailfile_begin"
    "mailfile_close"
    "mailfile_compress"
    "mailfile_end"
    "mailfile_info_file"
    "mailfile_modify"
    "mailfile_open"
    "mailfile_purge_waste"
    "message_begin"
    "message_copy"
    "message_delete"
    "message_end"
    "message_get"
    "message_info"
    "message_modify"
    "message_select"
    "send_abort"
    "send_add_address"
    "send_add_attribute"
    "send_add_bodypart"
    "send_begin"
    "send_end"
    "send_message"
    "user_begin"
    "user_delete_info"
    "user_end"
    "user_get_info"
    "user_set_info"
)
(~sys_literal_setup_incr "mail$_" 0 1 "noop")
(~sys_literal_setup_incr "mail$_send_" 1 1
	"spare_0"
	"foreign"
	"cc_line"
	"default_name"
	"default_transport"
	"error_entry"
	"filename"
	"from_line"
	"no_default_transport"
	"pers_name"
	"record"
	"resultspec"
	"server"
	"subject"
	"success_entry"
	"to_line"
	"uflags"
	"user_data"
	"username"
	"username_type"
	"fid"
	"no_pers_name"
	"in_spare3"
	"in_spare4"
	"in_spare5"
; 
; Send output parameters
;
	"copy_reply"
	"copy_send"
	"user"
	"copy_forward"
	"out_spare2"
	"out_spare3"
	"out_spare4"
	"out_spare5"
)
(~sys_literal_setup_incr "mail$_mailfile_" 1025 1
 	"spare_0"
	"default_name"
	"folder_routine"
	"full_close"
	"name"
	"reclaim"
	"user_data"
	"wastebasket_name"
	"in_spare1"
	"in_spare2"
	"in_spare3"
	"in_spare4"
	"in_spare5"
	"in_spare6"
	"in_spare7"
	"in_spare8"
	"in_spare9"
	"in_spare10"
	"in_spare11"
	"in_spare12"
	"in_spare13"
	"in_spare14"
	"in_spare15"
	"in_spare16"
	"in_spare17"
	"in_spare18"
	"in_spare19"
	"in_spare20"
;
; file output parameters
;
	"data_reclaim"
	"data_scan"
	"deleted_bytes"
	"index_reclaim"
	"mail_directory"
	"messages_deleted"
	"resultspec"
	"total_reclaim"
	"wastebasket"
	"indexed"
	"out_spare2"
	"out_spare3"
	"out_spare4"
	"out_spare5"
	"out_spare6"
	"out_spare7"
	"out_spare8"
	"out_spare9"
	"out_spare10"
	"out_spare11"
	"out_spare12"
	"out_spare13"
	"out_spare14"
	"out_spare15"
	"out_spare16"
	"out_spare17"
	"out_spare18"
	"out_spare19"
	"out_spare20"
)

;
; message input parameters
;

(~sys_literal_setup_incr "mail$_message_" 2048 1
	"spare_0"
	"back"
	"before"
	"cc_substring"
	"continue"
	"file_action"
	"folder_action"
	"default_name"
	"delete"
	"erase"
	"file_ctx"
	"filename"
	"flags"
	"folder"
	"from_substring"
	"id"
	"next"
	"since"
	"subj_substring"
	"to_substring"
	"uflags"
	"auto_newmail"
	"user_data"
	"flags_mbz"
	"min_class"
	"max_class"
	"in_spare1"
	"in_spare2"
	"in_spare3"
	"in_spare4"
	"in_spare5"
	"in_spare6"
	"in_spare7"
	"in_spare8"
	"in_spare9"
	"in_spare10"
	"in_spare11"
	"in_spare12"
	"in_spare13"
	"in_spare14"
	"in_spare15"
	"in_spare16"
	"in_spare17"
	"in_spare18"
	"in_spare19"
	"in_spare20"
;
; message output parameters
;
	"cc"
	"current_id"
	"date"
	"extid"
	"file_created"
	"folder_created"
	"from"
	"record"
	"record_type"
	"reply_path"
	"resultspec"
	"return_flags"
	"return_uflags"
	"selected"
	"sender"
	"size"
	"subject"
	"to"
	"buffer"
	"return_class"
	"binary_date"
	"spare4"
	"spare5"
	"spare6"
	"spare7"
	"spare8"
	"spare9"
	"spare10"
	"spare11"
	"spare12"
	"spare13"
	"spare14"
	"spare15"
	"spare16"
	"spare17"
	"spare18"
	"spare19"
;
; Constants returned
;
	"null"
	"header"
	"text"
	"spare20"
)

;
; user input parameters
;

(~sys_literal_setup_incr "mail$_user_" 3072 1
	"spare_0"
	"first"
	"next"
	"username"
	"set_auto_purge"
	"set_no_auto_purge"
	"set_sub_directory"
	"set_no_sub_directory"
	"set_forwarding"
	"set_no_forwarding"
	"set_personal_name"
	"set_no_personal_name"
	"set_copy_send"
	"set_no_copy_send"
	"set_copy_reply"
	"set_no_copy_reply"
	"set_new_messages"
	"create_if"
	"set_mailplus"
	"set_no_mailplus"
	"set_transport"
	"set_no_transport"
	"set_editor"
	"set_no_editor"
	"set_queue"
	"set_no_queue"
	"set_user1"
	"set_no_user1"
	"set_user2"
	"set_no_user2"
	"set_user3"
	"set_no_user3"
	"set_form"
	"set_no_form"
	"set_copy_forward"
	"set_no_copy_forward"
	"set_cc_prompt"
	"set_no_cc_prompt"
	"set_spare3"
	"set_no_spare3"
	"in_spare1"
	"in_spare2"
	"in_spare3"
	"in_spare4"
	"in_spare5"
	"in_spare6"
	"in_spare7"
	"in_spare8"
	"in_spare9"
	"in_spare10"
	"in_spare11"
	"in_spare12"
	"in_spare13"
	"in_spare14"
	"in_spare15"
	"in_spare16"
	"in_spare17"
	"in_spare18"
	"in_spare19"
	"in_spare20"
;
; Output item parameters
;
	"mailplus"
	"transport"
	"editor"
	"queue"
	"user1"
	"user2"
	"user3"
	"form"
	"copy_forward"
	"spare3"
	"return_username"
	"auto_purge"
	"sub_directory"
	"full_directory"
	"new_messages"
	"forwarding"
	"personal_name"
	"copy_send"
	"copy_reply"
	"captive"
	"cc_prompt"
	"out_spare2"
	"out_spare3"
	"out_spare4"
	"out_spare5"
	"out_spare6"
	"out_spare7"
	"out_spare8"
	"out_spare9"
	"out_spare10"
	"out_spare11"
	"out_spare12"
	"out_spare13"
	"out_spare14"
	"out_spare15"
	"out_spare16"
	"out_spare17"
	"out_spare18"
	"out_spare19"
	"out_spare20"
)
(~sys_literal_setup_incr "mail$_" 1 1
	"to"
	"cc"
	"spare1"
	"spare2"
)
(~sys_literal_setup_equal "mail$_" 
    "coderr"	8290316
    "upgrade"	8290324
    "uafgeterr"	8290330
    "uafopnerr"	8290338
    "nosuchusr"	8290346
    "notreadin"	8290354
    "loglink"	8290362
    "creprijob"	8290370
    "senderr"	8290378
    "nomoremsg"	8290386
    "nomsgs"	8290394
    "userspec"	8290402
    "sendabort"	8290410
    "editproc"	8290418
    "invcmd"	8290426
    "notfound"	8290434
    "netioerr"	8290442
    "userdsabl"	8290450
    "forwloop"	8290458
    "namtoobig"	8290466
    "illchar"	8290474
    "datimused"	8290482
    "notisam"	8290490
    "delmsg"	8290498
    "delwaste"	8290506
    "nofolder"	8290514
    "notexist"	8290522
    "notsubdir"	8290530
    "illsubdir"	8290538
    "illfolnam"	8290546
    "nomsgpri"	8290554
    "altprijob"	8290562
    "illpernam"	8290570
    "nosuchusrat"	8290578
    "senderrat"	8290586
    "conabort"	8290594
    "notcaledt"	8290602
    "fileconfused"	8290610
    "captive"	8290618
    "ivprotval"	8290626
    "rectobig"	8290634
    "illctxadr"	8290642
    "wrongctx"	8290650
    "failgetvm"	8290658
    "invitmcod"	8290666
    "misreqitm"	8290674
    "invitmlen"	8290682
    "fileopen"	8290690
    "nofileopen"	8290698
    "conitmcod"	8290706
    "nomorectx"	8290714
    "wrongfile"	8290722
    "illtprt"	8290730
    "invitmval"	8290738
    "eracted"	8290746
    "formsg"	8290754
    "notformsg"	8290762
    "erractrns"	8290770
    "badvalue"	8290778
    "closedel"	8290786
    "filnotdel"	8290794
    "openin"	8290802
    "openout"	8290810
    "parsefail"	8290818
    "readerr"	8290826
    "searchfail"	8290834
    "syserror"	8290842
    "nosysnam"	8290850
    "nosysprv"	8290858
    "protocol"	8290866
    "selected"	8306307
    "delmsgs"	8306315
    "cvtfile"	8306323
    "delspace"	8306331
    "newfolder"	8306339
    "reclplswait"	8306347
    "reenter"	8306355
    "moremsg"	8314304
    "dirhd0"	8314312
    "dirhd10"	8314320
    "dirhd20"	8314328
    "dirdel10"	8314336
    "isub10"	8314344
    "isub20"	8314352
    "isub30"	8314360
    "presret"	8314368
    "youhavenew"	8314376
    "mail10"	8314384
    "mail20"	8314392
    "wantosend"	8314400
    "notify10"	8314408
    "notify20"	8314416
    "jobentered"	8314424
    "show10"	8314432
    "show20"	8314440
    "show30"	8314448
    "show40"	8314456
    "show50"	8314464
    "show60"	8314472
    "show70"	8314480
    "show80"	8314488
    "show90"	8314496
    "show100"	8314504
    "show110"	8314512
    "edit10"	8314520
    "info10"	8314528
    "info20"	8314536
    "info30"	8314544
    "send"	8314552
    "reply"	8314560
    "show120"	8314568
    "show130"	8314576
    "info50"	8314584
    "show140"	8314592
    "show150"	8314600
    "show160"	8314608
    "isub50"	8314616
    "mail30"	8314624
    "retcont"	8314632
    "newseq"	8314640
    "maildir"	8314648
    "mailfile"	8314656
    "mailfolder"	8314664
    "mail40"	8314672
    "crenewfold"	8314680
    "nonewmail"	8314688
    "showkey10"	8314696
    "showkey20"	8314704
    "showkey30"	8314712
    "showkey40"	8314720
    "showkey50"	8314728
    "showkey60"	8314736
    "showkey70"	8314744
    "notify30"	8314752
    "notify40"	8314760
    "noparent"	8314768
    "filempty"	8314776
    "nonefound"	8314784
    "nomorerec"	8314792
    "userfwd"	8314800
    "prompt"	8314808
    "info12"	8314816
    "info14"	8314824
    "info16"	8314832
    "info18"	8314840
    "crenewfile"	8314848
    "show15"	8314856
    "show25"	8314864
    "show35"	8314872
    "forward"	8314880
    "show200"	8314888
    "show205"	8314896
    "show210"	8314904
    "show215"	8314912
    "closein"	8314920
    "closeout"	8314928
    "invquaval"	8314936
    "writeerr"	8314944
    "msginfo"	8322313
    "msgtext"	8322305
    "msgsent"	8322321
    "msgsentat"	8322329
    "yes"	8322337
    "copiedr"	8322345
    "created"	8322353
    "renamed"	8322361
    "text"	8322369
)
(~sys_literal_setup_equal "mail$m_"
    "newmsg"	1
    "replied"	2
    "del"	4
    "extmsg"	8
    "extfnf"	16
    "sysmsg"	32
    "extnstd"	64
    "marked"	128
    "recmode"	256
)
(external-function "~mail" "emacs$share:emacs_mail_shr" "emacs$mail")
