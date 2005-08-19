[Bliss-bind]
§1¶¥(language-start-entity)¨BIND§3¶¥(progn (language-keyword-convert) (~bliss-exit-on-space))¨
§2¶ ¥(language-indent)¨§4¶¥(language-optional-entity "Enter storage binding definitions and use exit-emacs.")¨
[Bliss-map]
§1¶¥(language-start-entity)¨MAP§3¶¥(progn (language-keyword-convert) (~bliss-exit-on-space))¨
§2¶ ¥(language-indent)¨§4¶¥(language-optional-entity "Enter storage mapping definitions and use exit-emacs.")¨
[Bliss-external]
§1¶¥(language-start-entity)¨EXTERNAL§2¶¥(progn (language-keyword-convert) (~bliss-exit-on-space))¨
§2¶ ¥(language-indent)¨§4¶¥(language-optional-entity "Enter external data references and use exit-emacs.")¨
[Bliss-forward]
§1¶¥(language-start-entity)¨FORWARD§3¶¥(progn (language-keyword-convert) (~bliss-exit-on-space))¨
§2¶ ¥(language-indent)¨§4¶¥(language-optional-entity "Enter forward reference declarations and use exit-emacs.")¨
[Bliss-global]
§1¶¥(language-start-entity)¨GLOBAL§3¶¥(progn (language-keyword-convert) (~bliss-exit-on-space))¨
§2¶ ¥(language-indent)¨§4¶¥(language-optional-entity "Enter global data declarations and use exit-emacs.")¨
[Bliss-library]
§1¶¥(language-start-entity)¨LIBRARY§2¶¥(language-keyword-convert)¨ '§3¶¥(language-optional-entity "Enter LIBRARY file name and use exit-emacs")¨';
[Bliss-require]
§1¶¥(language-start-entity)¨REQUIRE§2¶¥(language-keyword-convert)¨ '§3¶¥(language-optional-entity "Enter REQUIRE filename and use exit-emacs")¨';
[Bliss-literal]
§1¶¥(language-start-entity)¨LITERAL§3¶¥(language-keyword-convert)¨
§2¶ ¥(language-indent)¨§4¶¥(language-optional-entity "Enter LITERAL definitions and use exit-emacs.")¨
[Bliss-macro]
§1¶¥(language-start-entity)¨MACRO§3¶¥(language-keyword-convert)¨
§2¶ ¥(language-indent)¨§4¶¥(language-optional-entity "Enter MACROs and use exit-emacs.")¨
[Bliss-keywordmacro]
§1¶¥(language-start-entity)¨KEYWORDMACRO§3¶¥(language-keyword-convert)¨
§2¶ ¥(language-indent)¨§4¶¥(language-optional-entity "Enter MACROs and use exit-emacs.")¨
[Bliss-module]
%TITLE§1¶¥(language-keyword-convert)¨ '§14¶¥(insert-string (concat ~bliss-module-name " -"))¨ §15¶«Module title text»¥(language-fetch-optional "Insert the module title text and use exit-emacs." 3)¨'
MODULE§4¶¥(language-keyword-convert)¨ §13¶¥(~bliss-module)¨ (
§2¶¥(language-indent)¨IDENT§5¶¥(language-keyword-convert)¨ = 'V1-000',
§3¶ ¥(language-indent)¨§17¶«Module switches»¥(language-fetch-optional "Insert module switches and use exit-emacs" (save-window-excursion (set-mark) (beginning-of-line) (erase-region) 2))¨
) =
BEGIN§6¶¥(language-keyword-convert)¨

!
!			  COPYRIGHT © §7¶¥(insert-string (substr (current-time) -4 4))¨ BY
!	      DIGITAL EQUIPMENT CORPORATION, MAYNARD, MASS.
!
! This software is furnished under a license and may be used and  copied
! only  in  accordance  with  the  terms  of  such  license and with the
! inclusion of the above copyright notice.  This software or  any  other
! copies  thereof may not be provided or otherwise made available to any
! other person.  No title to and ownership of  the  software  is  hereby
! transferred.
!
! The information in this software is subject to change  without  notice
! and  should  not  be  construed  as  a commitment by DIGITAL EQUIPMENT
! CORPORATION.
!
! DIGITAL assumes no responsibility for the use or  reliability  of  its
! software on equipment which is not supplied by DIGITAL.
!

!++
! FACILITY:	
!	§18¶«Facility description»¥(language-fetch-comment "Enter the name of the facility and use exit-emacs."))¨
!
! ABSTRACT:
!	§19¶«Abstract description»¥(language-fetch-comment "Enter the module abstract and use exit-emacs."))¨
!
! ENVIRONMENT: §20¶«Environment description»¥(language-fetch-comment "Enter the environment in which the module runs and use exit-emacs.")¨
!
! AUTHOR:  §21¶«Your name»¥(insert-users-name)¨	CREATION DATE: §8¶¥(VAX/VMS-date)¨
!
! MODIFIED BY:
!
! Edit	Modifier	 Date		Reason
! 000	§22¶¥(progn (insert-users-name) (to-col 25) (insert-character ' ') (VAX/VMS-date))¨	Original
!--
%SBTTL§9¶¥(language-keyword-convert)¨ 'Declarations'

!
! SWITCHES:
!
§23¶¥(language-start-entity)¨
SWITCHES§24¶
    «Switches»¥(insert-character '\n')¨
§25¶¥(language-erase-empty-entity 2)¨
!
! INCLUDE FILES:
!
§26¶¥(language-start-entity)¨
LIBRARY§27¶ '«Library name»';¥(insert-character '\n')¨
§28¶¥(language-erase-empty-entity 2)¨
!
! TABLE OF CONTENTS:
!
§29¶¥(language-start-entity)¨
FORWARD§10¶¥(language-keyword-convert)¨ ROUTINE§30¶
    «Routines in this module»¥(insert-character '\n')¨
§31¶¥(language-erase-empty-entity 10)¨
!
! PSECTS:
!
§32¶¥(language-start-entity)¨
PSECT§33¶
    «Psect definitions»¥(insert-character '\n')¨
§34¶¥(language-erase-empty-entity 2)¨
!
! MACRO DEFINITIONS:
!
§35¶¥(language-start-entity)¨
MACRO§36¶
    «Macro definitions»¥(insert-character '\n')¨
§37¶¥(language-erase-empty-entity 2)¨
!
! EQUATED SYMBOLS:
!
§38¶¥(language-start-entity)¨
LITERAL§39¶
    «Equated symbol definitions»¥(insert-character '\n')¨
§40¶¥(language-erase-empty-entity 2)¨
!
! OWN STORAGE:
!
§41¶¥(language-start-entity)¨
OWN§42¶
    «Module own definitions»¥(insert-character '\n')¨
§43¶¥(language-erase-empty-entity 2)¨
!
! EXTERNAL REFERENCES:
!
§44¶¥(language-start-entity)¨
EXTERNAL§45¶
    «External references»¥(expand-bliss-keyword)¨
§46¶¥(language-erase-empty-entity 2)¨
!
§47¶¥(progn (language-exit-dot)(message "Module definition complete"))¨
END§11¶¥(language-keyword-convert)¨					! Of module §16¶¥(insert-string ~bliss-module-name)¨
ELUDOM§12¶¥(language-keyword-convert)¨
[Bliss-own]
§1¶¥(language-start-entity)¨OWN§3¶¥(language-keyword-convert)¨
§2¶ ¥(language-indent)¨§4¶¥(language-optional-entity "Enter OWN data declarations and use exit-emacs.")¨
[Bliss-label]
§1¶¥(language-start-entity)¨LABEL§3¶¥(language-keyword-convert)¨
§2¶ ¥(language-indent)¨§4¶¥(language-optional-entity "Enter label declarations and use exit-emacs.")¨
[Bliss-linkage]
§1¶¥(language-start-entity)¨LINKAGE§3¶¥(language-keyword-convert)¨
§2¶ ¥(language-indent)¨§4¶¥(language-optional-entity "Enter linkage declarations and use exit-emacs.")¨
[Bliss-local]
§1¶¥(language-start-entity)¨LOCAL§3¶¥(language-keyword-convert)¨
§2¶ ¥(language-indent)¨§4¶¥(language-optional-entity "Enter LOCAL data declarations and use exit-emacs.")¨
[Bliss-register]
§1¶¥(language-start-entity)¨REGISTER§3¶¥(language-keyword-convert)¨
§2¶ ¥(language-indent)¨§4¶¥(language-optional-entity "Enter LOCAL register declarations and use exit-emacs.")¨
[Bliss-stacklocal]
§1¶¥(language-start-entity)¨STACKLOCAL§3¶¥(language-keyword-convert)¨
§2¶ ¥(language-indent)¨§4¶¥(language-optional-entity "Enter forced stack data declarations and use exit-emacs.")¨
[Bliss-psect]
§1¶¥(language-start-entity)¨PSECT§3¶¥(language-keyword-convert)¨
§2¶ ¥(language-indent)¨§4¶¥(language-optional-entity "Enter PSECT definitions and use exit-emacs.")¨
[Bliss-structure]
§1¶¥(language-start-entity)¨STRUCTURE§3¶¥(language-keyword-convert)¨
§2¶ ¥(language-indent)¨§4¶¥(language-optional-entity "Enter strcuture declarations and use exit-emacs.")¨
[Bliss-routine]
§1¶¥(~bliss-routine)¨§3¶¥(language-start-entity)¨ROUTINE§4¶¥(language-keyword-convert)¨
§2¶ ¥(language-indent)¨§5¶¥(language-optional-entity "Enter routine dedescriptions and use exit-emacs.")¨
[Bliss-routine-def]
%SBTTL§1¶¥(language-keyword-convert)¨'§11¶¥(insert-string (concat ~bliss-routine-name " -"))¨ §13¶«Routine comment»¥(language-fetch-optional "Insert the routine title text and use exit-emacs." 3)¨'
§6¶¥(insert-string bliss-routine-prefix)¨ROUTINE§7¶¥(language-keyword-convert)¨ §10¶«Routine name»¥(~bliss-routine-name)¨§14¶¥(~bliss-routine-mark)¨ (
§2¶ ¥(language-indent)¨§15¶«Paremeter descriptions»¥(language-fetch-optional "Insert the parameter descriptions and use exit-emacs" (save-window-excursion (set-mark) (beginning-of-line) (erase-region) 3))¨§16¶
)¥(~bliss-routine-params)¨ : §17¶«Linkages»¥(language-fetch-optional "Insert the linkages and use exit-emacs." 3)¨ =

!++
! FUNCTIONAL DESCRIPTION:
!
!	§18¶«Functional description»¥(language-fetch-comment "Enter the functional description and use exit-emacs.")¨
!
! FORMAL PARAMETERS:
!
§19¶¥(language-start-entity)¨!	§20¶«Formal parameter description»¥(language-fetch-comment "Enter the formal parameter description and use exit-emacs." 0)¨§21¶¥(language-erase-empty-entity 2)¨
!
! IMPLICIT INPUTS:
!
§22¶¥(language-start-entity)¨!	§23¶«Implicit inputs description»¥(language-fetch-comment "Enter the implicit inputs and use exit-emacs." 0)¨§24¶¥(language-erase-empty-entity 2)¨
!
! IMPLICIT OUTPUTS:
!
§25¶¥(language-start-entity)¨!	§26¶«Implicit outputs description»¥(language-fetch-comment "Enter the implicit outputs and use exit-emacs." 0)¨§27¶¥(language-erase-empty-entity 2)¨
!
! ROUTINE VALUE:
!
§28¶¥(language-start-entity)¨!	§29¶«Return status description»¥(language-fetch-comment "Enter the routine values and use exit-emacs." 0)¨§30¶¥(language-erase-empty-entity 2)¨
!
! SIDE EFFECTS:
!
§31¶¥(language-start-entity)¨!	§32¶«Side effects description»¥(language-fetch-comment "Enter the side effects and use exit-emacs." 0)¨§33¶¥(language-erase-empty-entity 2)¨
!--
§3¶¥(language-indent)¨BEGIN§8¶¥(language-keyword-convert)¨
§4¶ ¥(language-indent)¨§34¶«Routine body»¥(progn (language-exit-dot) (message "Routine definition completed"))¨
§5¶¥(language-indent)¨END§9¶¥(language-keyword-convert)¨;				! Of routine §12¶¥(insert-string ~bliss-routine-name)¨
[Bliss-switches]
§1¶¥(language-start-entity)¨SWITCHES§3¶¥(language-keyword-convert)¨
§2¶ ¥(language-indent)¨§4¶¥(language-optional-entity "Enter SWITCHES and use exit-emacs.")¨
[Bliss-begin]
§1¶¥(language-start-entity)¨BEGIN§2¶¥(language-keyword-convert)¨
§3¶¥(progn (language-exit-dot) (message "Enter block body."))¨
END§4¶¥(language-keyword-convert)¨
[Bliss-while]
§1¶¥(language-start-entity)¨WHILE§3¶¥(language-keyword-convert)¨ §4¶¥(language-optional-entity "Enter loop test expression and use exit-emacs.")¨ DO§5¶¥(language-keyword-convert)¨
§2¶ ¥(language-indent)¨§6¶«Loop body»¥(progn (language-exit-dot) (message "Enter loop body."))¨
[Bliss-until]
§1¶¥(language-start-entity)¨UNTIL§3¶¥(language-keyword-convert)¨ §4¶¥(language-optional-entity "Enter loop test expression and use exit-emacs.")¨ DO§5¶¥(language-keyword-convert)¨
§2¶ ¥(language-indent)¨§6¶«Loop body»¥(progn (language-exit-dot) (message "Enter loop body."))¨
[Bliss-if]
§1¶¥(language-start-entity)¨IF§4¶¥(language-keyword-convert)¨ §5¶¥(language-optional-entity "Enter test expression and use exit-emacs.")¨
THEN§6¶¥(language-keyword-convert)¨
§2¶ ¥(language-indent)¨§7¶«THEN block»¥(language-fetch-optional "Enter then block and use exit-emacs")¨§8¶¥(language-start-entity)¨
ELSE§9¶¥(language-keyword-convert)¨
§3¶ ¥(language-indent)¨§10¶«ELSE block»¥(language-opt-cont-entity "Enter ELSE block and use exit-emacs." 1)¨§11¶¥(language-exit-dot)¨
[Bliss-else]
§1¶¥(language-start-entity)¨ELSE§3¶¥(language-keyword-convert)¨
§2¶ ¥(language-indent)¨§4¶¥(progn (language-exit-dot) (message "Enter ELSE block."))¨
[Bliss-case]
§1¶¥(language-start-entity)¨CASE§5¶¥(language-keyword-convert)¨ §6¶¥(language-optional-entity "Enter selection expression and use exit-emacs.")¨
FROM§7¶¥(language-keyword-convert)¨ §8¶«Lower bounds expression»¥(language-optional-entity "Enter lower bound expression and use exit-emacs.")¨ TO§9¶¥(language-keyword-convert)¨ §10¶«Upper bounds expression»¥(language-optional-entity "Enter upper bound expression and use exit-emacs.")¨ OF§11¶¥(language-keyword-convert)¨
§2¶¥(language-indent)¨SET§12¶¥(language-keyword-convert)¨
§3¶ ¥(language-indent)¨§13¶«Case lines»¥(progn (language-exit-dot) (message "Enter case lines."))¨
§4¶¥(language-indent)¨TES§14¶¥(language-keyword-convert)¨
[Bliss-select]
§1¶¥(language-start-entity)¨SELECT§5¶¥(language-keyword-convert)¨ §6¶¥(language-optional-entity "Enter selection expression and use exit-emacs.")¨ OF§7¶¥(language-keyword-convert)¨
§2¶¥(language-indent)¨SET§8¶¥(language-keyword-convert)¨
§3¶ ¥(language-indent)¨§9¶«Select lines»¥(progn (language-exit-dot) (message "Enter select lines."))¨
§4¶¥(language-indent)¨TES§10¶¥(language-keyword-convert)¨
[Bliss-selecta]
§1¶¥(language-start-entity)¨SELECTA§5¶¥(language-keyword-convert)¨ §6¶¥(language-optional-entity "Enter selection expression and use exit-emacs.")¨ OF §7¶¥(language-keyword-convert)¨
§2¶¥(language-indent)¨SET§8¶¥(language-keyword-convert)¨
§3¶ ¥(language-indent)¨§9¶«Selecta lines»¥(progn (language-exit-dot) (message "Enter select lines."))¨
§4¶¥(language-indent)¨TES§10¶¥(language-keyword-convert)¨
[Bliss-selectu]
§1¶¥(language-start-entity)¨SELECTU§5¶¥(language-keyword-convert)¨ §6¶¥(language-optional-entity "Enter selection expression and use exit-emacs.")¨ OF §7¶¥(language-keyword-convert)¨
§2¶¥(language-indent)¨SET§8¶¥(language-keyword-convert)¨
§3¶ ¥(language-indent)¨§9¶«Selectu lines»¥(progn (language-exit-dot) (message "Enter select lines."))¨
§4¶¥(language-indent)¨TES§10¶¥(language-keyword-convert)¨
[Bliss-selectone]
§1¶¥(language-start-entity)¨SELECTONE§5¶¥(language-keyword-convert)¨ §6¶¥(language-optional-entity "Enter selection expression and use exit-emacs.")¨ OF§7¶¥(language-keyword-convert)¨
§2¶¥(language-indent)¨SET§8¶¥(language-keyword-convert)¨
§3¶ ¥(language-indent)¨§9¶«Selectone lines»¥(progn (language-exit-dot) (message "Enter select lines."))¨
§4¶¥(language-indent)¨TES§10¶¥(language-keyword-convert)¨
[Bliss-selectonea]
§1¶¥(language-start-entity)¨SELECTONEA§5¶¥(language-keyword-convert)¨ §6¶¥(language-optional-entity "Enter selection expression and use exit-emacs.")¨ OF§7¶¥(language-keyword-convert)¨
§2¶¥(language-indent)¨SET§8¶¥(language-keyword-convert)¨
§3¶ ¥(language-indent)¨§9¶«Selectonea lines»¥(progn (language-exit-dot) (message "Enter select lines."))¨
§4¶¥(language-indent)¨TES§10¶¥(language-keyword-convert)¨
[Bliss-selectoneu]
§1¶¥(language-start-entity)¨SELECTONEU§5¶¥(language-keyword-convert)¨ §6¶¥(language-optional-entity "Enter selection expression and use exit-emacs.")¨ OF§7¶¥(language-keyword-convert)¨
§2¶¥(language-indent)¨SET§8¶¥(language-keyword-convert)¨
§3¶ ¥(language-indent)¨§9¶«Selectoneu lines»¥(progn (language-exit-dot) (message "Enter select lines."))¨
§4¶¥(language-indent)¨TES§10¶¥(language-keyword-convert)¨
[Bliss-incr]
§1¶¥(language-start-entity)¨INCR§3¶¥(language-keyword-convert)¨ §4¶¥(language-optional-entity "Enter loop counter and use exit-emacs.")¨ §5¶¥(language-start-entity)¨FROM §6¶«From expression»¥(language-opt-cont-entity "Enter FROM expression and use exit-emacs" 1)¨ §7¶¥(language-start-entity)¨TO §8¶«To expression»¥(language-opt-cont-entity "Enter TO expression and use exit-emacs" 1)¨ §9¶¥(language-start-entity)¨BY §10¶«By expression»¥(language-opt-cont-entity "Enter BY expression and use exit emacs" 1)¨ DO§11¶¥(language-keyword-convert)¨
§2¶ ¥(language-indent)¨§12¶«Loop block»¥(progn (language-exit-dot) (message "Enter loop block."))¨
[Bliss-incra]
§1¶¥(language-start-entity)¨INCRA§3¶¥(language-keyword-convert)¨ §4¶¥(language-optional-entity "Enter loop counter and use exit-emacs.")¨ §5¶¥(language-start-entity)¨FROM §6¶«From expression»¥(language-opt-cont-entity "Enter FROM expression and use exit-emacs" 1)¨ §7¶¥(language-start-entity)¨TO §8¶«To expression»¥(language-opt-cont-entity "Enter TO expression and use exit-emacs" 1)¨ §9¶¥(language-start-entity)¨BY §10¶«By expression»¥(language-opt-cont-entity "Enter BY expression and use exit emacs" 1)¨ DO§11¶¥(language-keyword-convert)¨
§2¶ ¥(language-indent)¨§12¶«Loop block»¥(progn (language-exit-dot) (message "Enter loop block."))¨
[Bliss-incru]
§1¶¥(language-start-entity)¨INCRU§3¶¥(language-keyword-convert)¨ §4¶¥(language-optional-entity "Enter loop counter and use exit-emacs.")¨ §5¶¥(language-start-entity)¨FROM §6¶«From expression»¥(language-opt-cont-entity "Enter FROM expression and use exit-emacs" 1)¨ §7¶¥(language-start-entity)¨TO §8¶«To expression»¥(language-opt-cont-entity "Enter TO expression and use exit-emacs" 1)¨ §9¶¥(language-start-entity)¨BY §10¶«By expression»¥(language-opt-cont-entity "Enter BY expression and use exit emacs" 1)¨ DO§11¶¥(language-keyword-convert)¨
§2¶ ¥(language-indent)¨§12¶«Loop block»¥(progn (language-exit-dot) (message "Enter loop block."))¨
[Bliss-decr]
§1¶¥(language-start-entity)¨DECR§3¶¥(language-keyword-convert)¨ §4¶¥(language-optional-entity "Enter loop counter and use exit-emacs.")¨ §5¶¥(language-start-entity)¨FROM §6¶«From expression»¥(language-opt-cont-entity "Enter FROM expression and use exit-emacs" 1)¨ §7¶¥(language-start-entity)¨TO §8¶«To expression»¥(language-opt-cont-entity "Enter TO expression and use exit-emacs" 1)¨ §9¶¥(language-start-entity)¨BY §10¶«By expression»¥(language-opt-cont-entity "Enter BY expression and use exit emacs" 1)¨ DO§11¶¥(language-keyword-convert)¨
§2¶ ¥(language-indent)¨§12¶«Loop block»¥(progn (language-exit-dot) (message "Enter loop block."))¨
[Bliss-decra]
§1¶¥(language-start-entity)¨DECRA§3¶¥(language-keyword-convert)¨ §4¶¥(language-optional-entity "Enter loop counter and use exit-emacs.")¨ §5¶¥(language-start-entity)¨FROM §6¶«From expression»¥(language-opt-cont-entity "Enter FROM expression and use exit-emacs" 1)¨ §7¶¥(language-start-entity)¨TO §8¶«To expression»¥(language-opt-cont-entity "Enter TO expression and use exit-emacs" 1)¨ §9¶¥(language-start-entity)¨BY §10¶«By expression»¥(language-opt-cont-entity "Enter BY expression and use exit emacs" 1)¨ DO§11¶¥(language-keyword-convert)¨
§2¶ ¥(language-indent)¨§12¶«Loop block»¥(progn (language-exit-dot) (message "Enter loop block."))¨
[Bliss-decru]
§1¶¥(language-start-entity)¨DECRU§3¶¥(language-keyword-convert)¨ §4¶¥(language-optional-entity "Enter loop counter and use exit-emacs.")¨ §5¶¥(language-start-entity)¨FROM §6¶«From expression»¥(language-opt-cont-entity "Enter FROM expression and use exit-emacs" 1)¨ §7¶¥(language-start-entity)¨TO §8¶«To expression»¥(language-opt-cont-entity "Enter TO expression and use exit-emacs" 1)¨ §9¶¥(language-start-entity)¨BY §10¶«By expression»¥(language-opt-cont-entity "Enter BY expression and use exit emacs" 1)¨ DO§11¶¥(language-keyword-convert)¨
§2¶ ¥(language-indent)¨§12¶«Loop block»¥(progn (language-exit-dot) (message "Enter loop block."))¨
[Bliss-do]
§1¶¥(language-start-entity)¨DO§3¶¥(language-keyword-convert)¨
§2¶ ¥(language-indent)¨§6¶«Do body»¥((language-exit-dot)(message "Enter loop body."))¨
WHILE§4¶¥(language-keyword-convert)¨ §5¶¥(language-optional-entity "Enter loop test expression and use exit-emacs.")¨
[Bliss-leave]
§1¶¥(language-start-entity)¨LEAVE§2¶¥(language-keyword-convert)¨ §3¶«Label name»¥(language-optional-entity "Enter label name and use exit-emacs.")¨§4¶¥(language-insert-separator)¨WITH§5¶¥(expand-bliss-keyword)¨§6¶¥(delete-white-space)¨
[Bliss-with]
§1¶¥(language-start-entity)¨WITH§2¶¥(language-keyword-convert)¨ §3¶¥(language-optional-entity "Enter exit expression and use exit-emacs.")¨
[Bliss-exitloop]
EXITLOOP§1¶¥(progn (language-keyword-convert) (language-start-entity))¨ §2¶¥(language-optional-entity "Enter exit expression and use exit-emacs.")¨
[Bliss-return]
RETURN§1¶¥(progn (language-keyword-convert) (language-start-entity))¨ §2¶¥(language-optional-entity "Enter exit expression and use exit-emacs.")¨
[Bliss-abs]
§1¶¥(language-start-entity)¨ABS§2¶¥(language-keyword-convert)¨ (§3¶¥(language-optional-entity "Enter ABS expression and use exit-emacs.")¨)§4¶¥(language-exit-dot)¨
[Bliss-max]
§1¶¥(language-start-entity)¨MAX§2¶¥(language-keyword-convert)¨ (§3¶¥(language-optional-entity "Enter MAX expressions and use exit-emacs.")¨)§4¶¥(language-exit-dot)¨
[Bliss-mod]
§1¶¥(language-start-entity)¨MOD§2¶¥(language-keyword-convert)¨ (§3¶¥(language-optional-entity "Enter MOD expressions and use exit-emacs.")¨)§4¶¥(language-exit-dot)¨
[Bliss-maxu]
§1¶¥(language-start-entity)¨MAXU§2¶¥(language-keyword-convert)¨ (§3¶¥(language-optional-entity "Enter MAXU expressions and use exit-emacs.")¨)§4¶¥(language-exit-dot)¨
[Bliss-maxa]
§1¶¥(language-start-entity)¨MAXA§2¶¥(language-keyword-convert)¨ (§3¶¥(language-optional-entity "Enter MAXA expression and use exit-emacs.")¨)§4¶¥(language-exit-dot)¨
[Bliss-min]
§1¶¥(language-start-entity)¨MIN§2¶¥(language-keyword-convert)¨ (§3¶¥(language-optional-entity "Enter MIN expressions and use exit-emacs.")¨)§4¶¥(language-exit-dot)¨
[Bliss-minu]
§1¶¥(language-start-entity)¨MINU§2¶¥(language-keyword-convert)¨ (§3¶¥(language-optional-entity "Enter MINU expression and use exit-emacs.")¨)§4¶¥(language-exit-dot)¨
[Bliss-addressing_mode]
§1¶¥(language-start-entity)¨ADDRESSING_MODE§2¶¥(language-keyword-convert)¨ (§3¶¥(language-optional-entity "Enter attributes and use exit-emacs.")¨)§4¶¥(language-exit-dot)¨
[Bliss-actualparameter]
§1¶¥(language-start-entity)¨ACTUALPARAMETER§2¶¥(language-keyword-convert)¨ (§3¶¥(language-optional-entity "Enter parameter selection expression and use exit-emacs.")¨)§4¶¥(language-exit-dot)¨
[Bliss-nullparameter]
§1¶¥(language-start-entity)¨NULLPARAMETER§2¶¥(language-keyword-convert)¨ (§3¶¥(language-optional-entity "Enter parameter expression and use exit-emacs.")¨)§4¶¥(language-exit-dot)¨
[Bliss-plit]
§1¶¥(language-start-entity)¨PLIT§2¶¥(language-keyword-convert)¨ (§3¶¥(language-optional-entity "Enter program literal and use exit-emacs.")¨)§4¶¥(language-exit-dot)¨
[Bliss-uplit]
§1¶¥(language-start-entity)¨UPLIT§2¶¥(language-keyword-convert)¨ (§3¶¥(language-optional-entity "Enter program literal and use exit-emacs.")¨)§4¶¥(language-exit-dot)¨
[Bliss-preserve]
§1¶¥(language-start-entity)¨PRESERVE§2¶¥(language-keyword-convert)¨ (§3¶¥(language-optional-entity "Enter registers to be preserved and use exit-emacs.")¨)§4¶¥(language-exit-dot)¨
[Bliss-nopreserve]
§1¶¥(language-start-entity)¨NOPRESERVE§2¶¥(language-keyword-convert)¨ (§3¶¥(language-optional-entity "Enter NOPRESERVE registers and use exit-emacs.")¨)§4¶¥(language-exit-dot)¨
[Bliss-bitvector]
§1¶¥(language-start-entity)¨BITVECTOR§2¶¥(language-keyword-convert)¨[§3¶¥(language-optional-entity "Enter bit count expression and use exit-emacs.")¨]§4¶¥(language-exit-dot)¨
[Bliss-block]
§1¶¥(language-start-entity)¨BLOCK§2¶¥(language-keyword-convert)¨[§3¶¥(language-optional-entity "Enter BLOCK expressions and use exit-emacs.")¨]§4¶¥(language-exit-dot)¨
[Bliss-vector]
§1¶¥(language-start-entity)¨VECTOR§2¶¥(language-keyword-convert)¨[§3¶¥(language-optional-entity "Enter VECTOR expressions and use exit-emacs.")¨]§4¶¥(language-exit-dot)¨
[Bliss-blockvector]
§1¶¥(language-start-entity)¨BLOCKVECTOR§2¶¥(language-keyword-convert)¨[§3¶¥(language-optional-entity "Enter BLOCKVECTOR expressions and use exit-emacs.")¨]§4¶¥(language-exit-dot)¨
[Bliss-align]
§1¶¥(language-start-entity)¨ALIGN§2¶¥(language-keyword-convert)¨ (§3¶¥(language-optional-entity "Enter alignment attribute and use exit-emacs.")¨)§4¶¥(language-exit-dot)¨
[Bliss-actualcount]
ACTUALCOUNT§1¶¥(language-keyword-convert)¨ ()§2¶¥(language-exit-dot)¨
[Bliss-argptr]
ARGPTR§1¶¥(language-keyword-convert)¨ ()§2¶¥(language-exit-dot)¨
[Bliss-builtin]
§1¶¥(language-start-entity)¨BUILTIN§3¶¥(language-keyword-convert)¨
§2¶ ¥(language-indent)¨§4¶¥(language-optional-entity "Enter BUILTIN declarations and use exit-emacs.")¨
[Bliss-enable]
§1¶¥(language-start-entity)¨ENABLE§3¶¥(language-keyword-convert)¨
§2¶ ¥(language-indent)¨§4¶¥(language-optional-entity "Enter condition handler information and use exit-emacs.")¨;§4¶¥(language-exit-dot)¨
[Bliss-ident]
§1¶¥(language-start-entity)¨IDENT§2¶¥(language-keyword-convert)¨ = '§3¶¥(language-optional-entity "Enter identification information and use exit-emacs.")¨'§4¶¥(language-exit-dot)¨
[Bliss-initial]
§1¶¥(language-start-entity)¨INITIAL§2¶¥(language-keyword-convert)¨ (§3¶¥(language-optional-entity "Enter initialization expression and use exit-emacs.")¨)§4¶¥(language-exit-dot)¨
[Bliss-preset]
§1¶¥(language-start-entity)¨PRESET§2¶¥(language-keyword-convert)¨ (§3¶¥(language-optional-entity "Enter preset expressions and use exit-emacs.")¨)§4¶¥(language-exit-dot)¨
[Bliss-rep]
§1¶¥(language-start-entity)¨REP§2¶¥(language-keyword-convert)¨ §3¶¥(language-optional-entity "Enter replicator and use exit-emacs.")¨ OF§4¶¥(language-keyword-convert)¨ (§4¶«PLIT items»¥(language-fetch-optional "Enter PLIT items and use exit-emacs")¨)§5¶¥(language-exit-dot)¨
[Bliss-setunwind]
§1¶¥(language-start-entity)¨SETUNWIND§2¶¥(language-keyword-convert)¨ (§3¶¥(language-optional-entity "Enter SETUNWIND parameters and use exit-emacs.")¨)§4¶¥(language-exit-dot)¨
[Bliss-sign]
§1¶¥(language-start-entity)¨SIGN§2¶¥(language-keyword-convert)¨ (§3¶¥(language-optional-entity "Enter expression and use exit-emacs.")¨)§4¶¥(language-exit-dot)¨
[Bliss-signal]
§1¶¥(language-start-entity)¨SIGNAL§2¶¥(language-keyword-convert)¨ (§3¶¥(language-optional-entity "Enter condition parameters and use exit-emacs.")¨)§4¶¥(language-exit-dot)¨
[Bliss-signal_stop]
§1¶¥(language-start-entity)¨SIGNAL_STOP§2¶¥(language-keyword-convert)¨ (§3¶¥(language-optional-entity "Enter condition parameters and use exit-emacs.")¨)§4¶¥(language-exit-dot)¨
[Bliss-undeclare]
§1¶¥(language-start-entity)¨UNDECLARE§3¶¥(language-keyword-convert)¨
§2¶ ¥(language-indent)¨§4¶¥(language-optional-entity "Enter symbols to be undeclared and use exit-emacs.")¨
[Bliss-%allocation]
§1¶¥(language-start-entity)¨%ALLOCATION§2¶¥(language-keyword-convert)¨ (§3¶¥(language-optional-entity "Enter allocation parameters and use exit-emacs.")¨)§4¶¥(language-exit-dot)¨
[Bliss-%assign]
§1¶¥(language-start-entity)¨%ASSIGN§2¶¥(language-keyword-convert)¨ (§3¶¥(language-optional-entity "Enter assignment parameters and use exit-emacs.")¨)§4¶¥(language-exit-dot)¨
[Bliss-%charcount]
§1¶¥(language-start-entity)¨%CHARCOUNT§2¶¥(language-keyword-convert)¨ (§3¶¥(language-optional-entity "Enter character count parameters and use exit-emacs.")¨)§4¶¥(language-exit-dot)¨
[Bliss-%char]
§1¶¥(language-start-entity)¨%CHAR§2¶¥(language-keyword-convert)¨ (§3¶¥(language-optional-entity "Enter character parameters and use exit-emacs.")¨)§4¶¥(language-exit-dot)¨
[Bliss-%ctce]
§1¶¥(language-start-entity)¨CTCE§2¶¥(language-keyword-convert)¨ (§3¶¥(language-optional-entity "Enter expression to check and use exit-emacs.")¨)§4¶¥(language-exit-dot)¨
[Bliss-%declared]
§1¶¥(language-start-entity)¨DECLARED§2¶¥(language-keyword-convert)¨ (§3¶¥(language-optional-entity "Enter symbols to check and use exit-emacs.")¨)§4¶¥(language-exit-dot)¨
[Bliss-%if]
§1¶¥(language-start-entity)¨%IF§4¶¥(language-keyword-convert)¨ §5¶¥(language-optional-entity "Enter test expression and use exit-emacs.")¨
%THEN§6¶¥(language-keyword-convert)¨
§2¶ ¥(language-indent)¨§7¶«True text»¥(language-fetch-optional "Enter true string and use exit-emacs")¨
§8¶¥(language-start-entity)¨%ELSE§9¶¥(language-keyword-convert)¨
§3¶ ¥(language-indent)¨§10¶«False text»¥(language-opt-cont-entity "Enter false string and use exit-emacs." 1)¨
%FI§11¶¥(progn (language-keyword-convert)(language-exit-dot))¨
[Bliss-%error]
§1¶¥(language-start-entity)¨%ERROR§2¶¥(language-keyword-convert)¨ (§3¶¥(language-optional-entity "Enter error text and use exit-emacs.")¨)§4¶¥(language-exit-dot)¨
[Bliss-%errormacro]
§1¶¥(language-start-entity)¨%ERRORMACRO§2¶¥(language-keyword-convert)¨ (§3¶¥(language-optional-entity "Enter error text and use exit-emacs.")¨)§4¶¥(language-exit-dot)¨
[Bliss-%exactstring]
§1¶¥(language-start-entity)¨%EXACTSTRING§2¶¥(language-keyword-convert)¨ (§3¶¥(language-optional-entity "Enter parameters and use exit-emacs.")¨)§4¶¥(language-exit-dot)¨
[Bliss-%expand]
§1¶¥(language-start-entity)¨%EXPAND§2¶¥(language-keyword-convert)¨ (§3¶¥(language-optional-entity "Enter parameters and use exit-emacs.")¨)§4¶¥(language-exit-dot)¨
[Bliss-%explode]
§1¶¥(language-start-entity)¨%EXPLODE§2¶¥(language-keyword-convert)¨ (§3¶¥(language-optional-entity "Enter parameter and use exit-emacs.")¨)§4¶¥(language-exit-dot)¨
[Bliss-%fieldexpand]
§1¶¥(language-start-entity)¨%FIELDEXPAND§2¶¥(language-keyword-convert)¨ (§3¶¥(language-optional-entity "Enter parameters and use exit-emacs.")¨)§4¶¥(language-exit-dot)¨
[Bliss-%identical]
§1¶¥(language-start-entity)¨%IDENTICAL§2¶¥(language-keyword-convert)¨ (§3¶¥(language-optional-entity "Enter parameters and use exit-emacs.")¨)§4¶¥(language-exit-dot)¨
[Bliss-%inform]
§1¶¥(language-start-entity)¨%INFORM§2¶¥(language-keyword-convert)¨ (§3¶¥(language-optional-entity "Enter information text and use exit-emacs.")¨)§4¶¥(language-exit-dot)¨
[Bliss-%isstring]
§1¶¥(language-start-entity)¨%ISSTRING§2¶¥(language-keyword-convert)¨ (§3¶¥(language-optional-entity "Enter parameters and use exit-emacs.")¨)§4¶¥(language-exit-dot)¨
[Bliss-%length]
§1¶¥(language-start-entity)¨%LENGTH§2¶¥(language-keyword-convert)¨ (§3¶¥(language-optional-entity "Enter parameter and use exit-emacs.")¨)§4¶¥(language-exit-dot)¨
[Bliss-%ltce]
§1¶¥(language-start-entity)¨%LTCE§2¶¥(language-keyword-convert)¨ (§3¶¥(language-optional-entity "Enter symbols to check and use exit-emacs.")¨)§4¶¥(language-exit-dot)¨
[Bliss-%message]
§1¶¥(language-start-entity)¨%MESSAGE§2¶¥(language-keyword-convert)¨ (§3¶¥(language-optional-entity "Enter message text and use exit-emacs.")¨)§4¶¥(language-exit-dot)¨
[Bliss-%name]
§1¶¥(language-start-entity)¨%NAME§2¶¥(language-keyword-convert)¨ (§3¶¥(language-optional-entity "Enter parameters and use exit-emacs.")¨)§4¶¥(language-exit-dot)¨
[Bliss-%nbits]
§1¶¥(language-start-entity)¨%NBITS§2¶¥(language-keyword-convert)¨ (§3¶¥(language-optional-entity "Enter parameters and use exit-emacs.")¨)§4¶¥(language-exit-dot)¨
[Bliss-%nbitsu]
§1¶¥(language-start-entity)¨%NBITSU§2¶¥(language-keyword-convert)¨ (§3¶¥(language-optional-entity "Enter parameters and use exit-emacs.")¨)§4¶¥(language-exit-dot)¨
[Bliss-%null]
§1¶¥(language-start-entity)¨%NULL§2¶¥(language-keyword-convert)¨ (§3¶¥(language-optional-entity "Enter parameters and use exit-emacs.")¨)§4¶¥(language-exit-dot)¨
[Bliss-%number]
§1¶¥(language-start-entity)¨%NUMBER§2¶¥(language-keyword-convert)¨ (§3¶¥(language-optional-entity "Enter parameters and use exit-emacs.")¨)§4¶¥(language-exit-dot)¨
[Bliss-%print]
§1¶¥(language-start-entity)¨%PRINT§2¶¥(language-keyword-convert)¨ (§3¶¥(language-optional-entity "Enter parameters and use exit-emacs.")¨)§4¶¥(language-exit-dot)¨
[Bliss-%quote]
§1¶¥(language-start-entity)¨%QUOTE§2¶¥(language-keyword-convert)¨ (§3¶¥(language-optional-entity "Enter parameters and use exit-emacs.")¨)§4¶¥(language-exit-dot)¨
[Bliss-%quotename]
§1¶¥(language-start-entity)¨%QUOTENAME§2¶¥(language-keyword-convert)¨ (§3¶¥(language-optional-entity "Enter parameters and use exit-emacs.")¨)§4¶¥(language-exit-dot)¨
[Bliss-%ref]
§1¶¥(language-start-entity)¨%REF§2¶¥(language-keyword-convert)¨ (§3¶¥(language-optional-entity "Enter parameters and use exit-emacs.")¨)§4¶¥(language-exit-dot)¨
[Bliss-%remove]
§1¶¥(language-start-entity)¨%REMOVE§2¶¥(language-keyword-convert)¨ (§3¶¥(language-optional-entity "Enter parameters and use exit-emacs.")¨)§4¶¥(language-exit-dot)¨
[Bliss-%require]
§1¶¥(language-start-entity)¨%REQUIRE§2¶¥(language-keyword-convert)¨ (§3¶¥(language-optional-entity "Enter parameters and use exit-emacs.")¨)§4¶¥(language-exit-dot)¨
[Bliss-%size]
§1¶¥(language-start-entity)¨%SIZE§2¶¥(language-keyword-convert)¨ (§3¶¥(language-optional-entity "Enter parameters and use exit-emacs.")¨)§4¶¥(language-exit-dot)¨
[Bliss-%string]
§1¶¥(language-start-entity)¨%STRING§2¶¥(language-keyword-convert)¨ (§3¶¥(language-optional-entity "Enter parameters and use exit-emacs.")¨)§4¶¥(language-exit-dot)¨
[Bliss-%switches]
§1¶¥(language-start-entity)¨%SWITCHES§2¶¥(language-keyword-convert)¨ (§3¶¥(language-optional-entity "Enter parameters and use exit-emacs.")¨)§4¶¥(language-exit-dot)¨
[Bliss-%unquote]
§1¶¥(language-start-entity)¨%UNQUOTE§2¶¥(language-keyword-convert)¨ (§3¶¥(language-optional-entity "Enter parameters and use exit-emacs.")¨)§4¶¥(language-exit-dot)¨
[Bliss-%warn]
§1¶¥(language-start-entity)¨%WARN§2¶¥(language-keyword-convert)¨ (§3¶¥(language-optional-entity "Enter parameters and use exit-emacs.")¨)§4¶¥(language-exit-dot)¨
[Bliss-ch$a_rchar]
§1¶¥(language-start-entity)¨CH$A_RCHAR§2¶¥(language-keyword-convert)¨ (§3¶¥(language-optional-entity "Enter parameters and use exit-emacs.")¨)§4¶¥(language-exit-dot)¨
[Bliss-ch$a_wchar]
§1¶¥(language-start-entity)¨CH$A_WCHAR§2¶¥(language-keyword-convert)¨ (§3¶¥(language-optional-entity "Enter parameters and use exit-emacs.")¨)§4¶¥(language-exit-dot)¨
[Bliss-ch$allocation]
§1¶¥(language-start-entity)¨CH$ALLOCATION§2¶¥(language-keyword-convert)¨ (§3¶¥(language-optional-entity "Enter parameters and use exit-emacs.")¨)§4¶¥(language-exit-dot)¨
[Bliss-ch$compare]
§1¶¥(language-start-entity)¨CH$COMPARE§2¶¥(language-keyword-convert)¨ (§3¶¥(language-optional-entity "Enter parameters and use exit-emacs.")¨)§4¶¥(language-exit-dot)¨
[Bliss-ch$copy]
§1¶¥(language-start-entity)¨CH$COPY§2¶¥(language-keyword-convert)¨ (§3¶¥(language-optional-entity "Enter parameters and use exit-emacs.")¨)§4¶¥(language-exit-dot)¨
[Bliss-ch$diff]
§1¶¥(language-start-entity)¨CH$DIFF§2¶¥(language-keyword-convert)¨ (§3¶¥(language-optional-entity "Enter parameters and use exit-emacs.")¨)§4¶¥(language-exit-dot)¨
[Bliss-ch$eql]
§1¶¥(language-start-entity)¨CH$EQL§2¶¥(language-keyword-convert)¨ (§3¶¥(language-optional-entity "Enter parameters and use exit-emacs.")¨)§4¶¥(language-exit-dot)¨
[Bliss-ch$fail]
§1¶¥(language-start-entity)¨CH$FAIL§2¶¥(language-keyword-convert)¨ (§3¶¥(language-optional-entity "Enter parameters and use exit-emacs.")¨)§4¶¥(language-exit-dot)¨
[Bliss-ch$fill]
§1¶¥(language-start-entity)¨CH$FILL§2¶¥(language-keyword-convert)¨ (§3¶¥(language-optional-entity "Enter parameters and use exit-emacs.")¨)§4¶¥(language-exit-dot)¨
[Bliss-ch$find_ch]
§1¶¥(language-start-entity)¨CH$FIND_CH§2¶¥(language-keyword-convert)¨ (§3¶¥(language-optional-entity "Enter parameters and use exit-emacs.")¨)§4¶¥(language-exit-dot)¨
[Bliss-ch$find_not_ch]
§1¶¥(language-start-entity)¨CH$FIND_NOT_CH§2¶¥(language-keyword-convert)¨ (§3¶¥(language-optional-entity "Enter parameters and use exit-emacs.")¨)§4¶¥(language-exit-dot)¨
[Bliss-ch$find_sub]
§1¶¥(language-start-entity)¨CH$FIND_SUB§2¶¥(language-keyword-convert)¨ (§3¶¥(language-optional-entity "Enter parameters and use exit-emacs.")¨)§4¶¥(language-exit-dot)¨
[Bliss-ch$geq]
§1¶¥(language-start-entity)¨CH$GEQ§2¶¥(language-keyword-convert)¨ (§3¶¥(language-optional-entity "Enter parameters and use exit-emacs.")¨)§4¶¥(language-exit-dot)¨
[Bliss-ch$gtr]
§1¶¥(language-start-entity)¨CH$GTR§2¶¥(language-keyword-convert)¨ (§3¶¥(language-optional-entity "Enter parameters and use exit-emacs.")¨)§4¶¥(language-exit-dot)¨
[Bliss-ch$leq]
§1¶¥(language-start-entity)¨CH$LEQ§2¶¥(language-keyword-convert)¨ (§3¶¥(language-optional-entity "Enter parameters and use exit-emacs.")¨)§4¶¥(language-exit-dot)¨
[Bliss-ch$lss]
§1¶¥(language-start-entity)¨CH$LSS§2¶¥(language-keyword-convert)¨ (§3¶¥(language-optional-entity "Enter parameters and use exit-emacs.")¨)§4¶¥(language-exit-dot)¨
[Bliss-ch$move]
§1¶¥(language-start-entity)¨CH$MOVE§2¶¥(language-keyword-convert)¨ (§3¶¥(language-optional-entity "Enter parameters and use exit-emacs.")¨)§4¶¥(language-exit-dot)¨
[Bliss-ch$neq]
§1¶¥(language-start-entity)¨CH$NEQ§2¶¥(language-keyword-convert)¨ (§3¶¥(language-optional-entity "Enter parameters and use exit-emacs.")¨)§4¶¥(language-exit-dot)¨
[Bliss-ch$plus]
§1¶¥(language-start-entity)¨CH$PLUS§2¶¥(language-keyword-convert)¨ (§3¶¥(language-optional-entity "Enter parameters and use exit-emacs.")¨)§4¶¥(language-exit-dot)¨
[Bliss-ch$ptr]
§1¶¥(language-start-entity)¨CH$PTR§2¶¥(language-keyword-convert)¨ (§3¶¥(language-optional-entity "Enter parameters and use exit-emacs.")¨)§4¶¥(language-exit-dot)¨
[Bliss-ch$rchar]
§1¶¥(language-start-entity)¨CH$RCHAR§2¶¥(language-keyword-convert)¨ (§3¶¥(language-optional-entity "Enter parameters and use exit-emacs.")¨)§4¶¥(language-exit-dot)¨
[Bliss-ch$rchar_a]
§1¶¥(language-start-entity)¨CH$RCHAR_A§2¶¥(language-keyword-convert)¨ (§3¶¥(language-optional-entity "Enter parameters and use exit-emacs.")¨)§4¶¥(language-exit-dot)¨
[Bliss-ch$size]
§1¶¥(language-start-entity)¨CH$SIZE§2¶¥(language-keyword-convert)¨ (§3¶¥(language-optional-entity "Enter parameters and use exit-emacs.")¨)§4¶¥(language-exit-dot)¨
[Bliss-ch$translate]
§1¶¥(language-start-entity)¨CH$TRANSLATE§2¶¥(language-keyword-convert)¨ (§3¶¥(language-optional-entity "Enter parameters and use exit-emacs.")¨)§4¶¥(language-exit-dot)¨
[Bliss-ch$translatable]
§1¶¥(language-start-entity)¨CH$TRANSLATABLE§2¶¥(language-keyword-convert)¨ (§3¶¥(language-optional-entity "Enter parameters and use exit-emacs.")¨)§4¶¥(language-exit-dot)¨
[Bliss-ch$wchar]
§1¶¥(language-start-entity)¨CH$WCHAR§2¶¥(language-keyword-convert)¨ (§3¶¥(language-optional-entity "Enter parameters and use exit-emacs.")¨)§4¶¥(language-exit-dot)¨
[Bliss-ch$wchar_a]
§1¶¥(language-start-entity)¨CH$WCHAR_A§2¶¥(language-keyword-convert)¨ (§3¶¥(language-optional-entity "Enter parameters and use exit-emacs.")¨)§4¶¥(language-exit-dot)¨
[Bliss-!]
! §1¶¥(language-fetch-comment "Enter the comment and use exit-emacs" 0 "! ")¨
[Bliss-%(]
%( §1¶¥(language-fetch-comment "Enter the comment and use exit-emacs" 0 "%  ")¨
)%§2¶¥(language-exit-dot)¨
[C-/*]
/* §2¶¥(language-comment-type)¨§1¶¥(language-fetch-comment "Enter the comment and use exit-emacs." 1 " * " 3)¨
§3¶¥(language-check-comment-type)¨ */§4¶¥(language-exit-dot)¨
[C-case]
§1¶¥(language-start-entity)¨case §3¶¥(language-optional-entity "Enter match expression and use exit-emacs.")¨:
§2¶ ¥(language-indent)¨§4¶«case statements»¥(message "case label compelete.")¨
[C-default]
default:
§1¶«default statements»¥(language-indent)¨
[C-do]
§1¶¥(language-start-entity)¨do
§2¶ ¥(language-indent)¨§4¶«Loop body»¥(message "Enter loop body and use exit-emacs.")¨
while (§3¶¥(language-optional-entity "Enter loop test expression and use exit-emacs.")¨)
[C-else]
§1¶¥(language-start-entity)¨else
§2¶ ¥(progn (language-indent) (language-optional-entity "Enter false statement and use exit-emacs."))¨
[C-entry]
§1¶¥(message "The entry statement is not supported.")¨
[C-for]
for (§2¶¥(language-optional-entity "Enter initialization expression and use exit-emacs.")¨;
     §3¶«test expression»¥(language-optional-entity "Enter test expression and use exit-emacs.")¨;
     §4¶«increment expression»¥(language-optional-entity "Enter increment expression and use exit-emacs.")¨)
§1¶ ¥(language-indent)¨§5¶«loop body»¥(message "for statement compelete.")¨
[C-goto]
§1¶¥(language-start-entity)¨goto §2¶¥(language-optional-entity "Enter label name and use exit-emacs.")¨
[C-if]
§1¶¥(language-start-entity)¨if (§4¶¥(language-optional-entity "Enter test expression and use exit-emacs.")¨)
§2¶ ¥(language-indent)¨§5¶«THEN block»¥(language-optional-entity "Enter then block and use exit-emacs")¨§6¶¥(language-start-entity)¨
else
§3¶ ¥(language-indent)¨§7¶«ELSE block»¥(language-opt-cont-entity "Enter ELSE block and use exit-emacs." 1)¨§8¶¥(language-exit-dot)¨
[C-return]
return§1¶¥(language-start-entity)¨ §2¶¥(language-optional-entity "Enter result expression and use exit-emacs.")¨;§3¶¥(language-exit-dot)¨
[C-sizeof]
§1¶¥(language-start-entity)¨sizeof (§2¶¥(language-optional-entity "Enter type or object and use exit-emacs.")¨)§3¶¥(language-exit-dot)¨
[C-switch]
§1¶¥(language-start-entity)¨switch (§5¶¥(language-optional-entity "Enter selection expression and use exit-emacs.")¨)
§2¶¥(language-indent)¨{
§3¶ ¥(language-indent)¨§6¶«case statements»¥(message "Insert case statements here.")¨
§4¶¥(language-indent)¨}
[C-while]
§1¶¥(language-start-entity)¨while (§3¶¥(language-optional-entity "Enter loop test expression and use exit-emacs.")¨)
§2¶ ¥(language-indent)¨§4¶«Loop body»¥(message "Enter loop body.")¨
[C-{]
{
§1¶¥(message "Compound statement.")¨
}
[C-#include]
§1¶¥(language-start-entity)¨#include §2¶¥(language-optional-entity "Enter include file name and use exit-emacs.")¨
§3¶¥(language-exit-dot)¨
[C-#module]
§1¶¥(language-start-entity)¨#module §2¶¥(language-optional-entity "Enter module name and use exit-emacs.")¨ §3¶«Module Identification»¥(language-optional-entity "Enter the module ident and use exit-emacs.")¨
§4¶¥(language-exit-dot)¨
[C-#if]
§1¶¥(language-start-entity)¨#if §2¶¥(language-optional-entity "Enter test expression and use exit-emacs.")¨
§3¶«True code»¥(language-optional-entity "Enter true string and use exit-emacs")¨
§4¶¥(language-start-entity)¨#else
§5¶«False code»¥(language-opt-cont-entity "Enter false string and use exit-emacs." 1)¨
#endif
§6¶¥(language-exit-dot)¨
[C-#ifdef]
§1¶¥(language-start-entity)¨#ifdef §2¶¥(language-optional-entity "Enter test expression and use exit-emacs.")¨
§3¶«True code»¥(language-optional-entity "Enter true string and use exit-emacs")¨
§4¶¥(language-start-entity)¨#else
§5¶«False code»¥(language-opt-cont-entity "Enter false string and use exit-emacs." 1)¨
#endif
§6¶¥(language-exit-dot)¨
[C-#ifndef]
§1¶¥(language-start-entity)¨#ifndef §2¶¥(language-optional-entity "Enter test expression and use exit-emacs.")¨
§3¶«True code»¥(language-optional-entity "Enter true string and use exit-emacs")¨
§4¶¥(language-start-entity)¨#else
§5¶«False code»¥(language-opt-cont-entity "Enter false string and use exit-emacs." 1)¨
#endif
§6¶¥(language-exit-dot)¨
[C-#undef]
§1¶¥(language-start-entity)¨#undef §2¶¥(language-optional-entity "Enter synbol to be undeclared and use exit-emacs.")¨
§3¶¥(language-exit-dot)¨
[C-#define]
§1¶¥(language-start-entity)¨#define §2¶¥(language-optional-entity "Enter synbol name and arguments and use exit-emacs.")¨ §3¶«Replacement text»¥(language-optional-entity "Enter replacement text and use exit-emacs.")¨
§4¶¥(language-exit-dot)¨
[C-#dictionary]
§1¶¥(language-start-entity)¨#dictionary §2¶¥(language-optional-entity "Enter CDD path and use exit-emacs.")¨
§3¶¥(language-exit-dot)¨
[C-#line]
§1¶¥(language-start-entity)¨#line §2¶¥(language-optional-entity "Enter line number and use exit-emacs.")¨ §3¶«File specification»¥(language-optional-entity "Enter file specification and use exit-emacs.")¨
§4¶¥(language-exit-dot)¨
[C-#]
§1¶¥(language-start-entity)¨# §2¶¥(language-optional-entity "Enter line number and use exit-emacs.")¨ §3¶«File specification»¥(language-optional-entity "Enter file specification and use exit-emacs.")¨
§4¶¥(language-exit-dot)¨
[C-?]
§1¶¥(language-start-entity)¨? §2¶¥(language-optional-entity "Enter true expression and use exit-emacs.")¨ : §3¶«False expression»¥(language-optional-entity "Enter false expression and use exit-emacs.")¨
[C-function]
§5¶¥(language-start-entity)¨§6¶¥(language-optional-entity "Insert function declarator and name and use exit-emacs.")¨ (§7¶«Parameter names»¥(language-fetch-optional "Insert the parameter names and use exit-emacs" 0)¨)
§1¶ ¥(language-indent)¨§8¶«Parameter declarators»¥(language-fetch-optional "Insert the parameter declarators and use exit-emacs" 0)¨
/*
 * Functional Description
 *
 *	§9¶«Functional description»¥(language-fetch-comment "Enter the functional description and use exit-emacs." 1 "*	")¨
 *
 * Formal Parameters
 *
§10¶¥(language-start-entity)¨ *	§11¶«Formal parameter description»¥(language-fetch-comment "Enter the formal parameter description and use exit-emacs." 0 "*	")¨§12¶¥(language-erase-empty-entity 3 " *	None")¨
 *
 * Implicit Inputs
 *
§13¶¥(language-start-entity)¨ *	§14¶«Implicit inputs description»¥(language-fetch-comment "Enter the implicit inputs and use exit-emacs." 0 "*	")¨§15¶¥(language-erase-empty-entity 3 " *	None")¨
 *
 * Implicit Outputs
 *
§16¶¥(language-start-entity)¨ *	§17¶«Implicit outputs description»¥(language-fetch-comment "Enter the implicit outputs and use exit-emacs." 0 "*	")¨§18¶¥(language-erase-empty-entity 3 " *	None")¨
 *
 * Routine Value
 *
§19¶¥(language-start-entity)¨ *	§20¶«Return status description»¥(language-fetch-comment "Enter the routine values and use exit-emacs." 0 "*	")¨§21¶¥(language-erase-empty-entity 3 " *	None")¨
 *
 * Side Effects
 *
§22¶¥(language-start-entity)¨ *	§23¶«Side effects description»¥(language-fetch-comment "Enter the side effects and use exit-emacs." 0 "*	")¨§24¶¥(language-erase-empty-entity 3 " *	None")¨
 */
§2¶¥(language-indent)¨{
§3¶ ¥(language-indent)¨§25¶«Function body»¥(progn (language-exit-dot) (message "Function definition completed"))¨
§4¶¥(language-indent)¨}
[C-module]
#ifdef vax11c
#module §3¶¥(language-fetch-mandatory "Enter module name and use exit-emacs.")¨ §4¶«Module Identification»¥(language-fetch-mandatory "Enter the module ident and use exit-emacs.")¨
#endif

/*
 *			  COPYRIGHT © §1¶¥(insert-string (substr (current-time) -4 4))¨ BY
 *	      DIGITAL EQUIPMENT CORPORATION, MAYNARD, MASS.
 *
 * This software is furnished under a license and may be used and  copied
 * only  in  accordance  with  the  terms  of  such  license and with the
 * inclusion of the above copyright notice.  This software or  any  other
 * copies  thereof may not be provided or otherwise made available to any
 * other person.  No title to and ownership of  the  software  is  hereby
 * transferred.
 *
 * The information in this software is subject to change  without  notice
 * and  should  not  be  construed  as  a commitment by DIGITAL EQUIPMENT
 * CORPORATION.
 *
 * DIGITAL assumes no responsibility for the use or  reliability  of  its
 * software on equipment which is not supplied by DIGITAL.
 *
 *
 *
 * FACILITY:	
 *	§5¶«Facility description»¥(language-fetch-comment "Enter the name of the facility and use exit-emacs." 1 "*	")¨
 *
 * ABSTRACT:
 *	§6¶«Abstract description»¥(language-fetch-comment "Enter the module abstract and use exit-emacs." 1 "*	")¨
 *
 * ENVIRONMENT: §7¶«Environment description»¥(language-fetch-comment "Enter the environment in which the module runs and use exit-emacs." 1 "*	" 15)¨
 *
 * AUTHOR:  §8¶«Your name»¥(insert-users-name)¨	CREATION DATE: §2¶¥(VAX/VMS-date)¨
 *
 * MODIFIED BY:
 *
 * Edit	Modifier	 Date		Reason
 * 000	§9¶¥(progn (insert-users-name) (to-col 25) (insert-character ' ') (VAX/VMS-date))¨	Original
 */
/*
 * Include Files
 */
§10¶¥(language-start-entity)¨
#include§11¶ «Include file name»¥(insert-character '\n')¨
§12¶¥(language-erase-empty-entity 2 "\n *	None\n" 2)¨§13¶
/*¥(language-insert-if-comment " *" "\n/*")¨
 * Table of Contents
 */
§14¶¥(language-start-entity)¨
§15¶«Routines in this module»¥(language-opt-cont-entity "Enter the declarators for routines in this module and use exit-emacs")¨
§16¶¥(language-erase-empty-entity 1 "\n *	None\n" 2)¨§17¶
/*¥(language-insert-if-comment " *" "\n/*")¨
 * Macro Definitions
 */
§18¶¥(language-start-entity)¨
#define§19¶ «Macro definitions»¥(insert-character '\n')¨
§20¶¥(language-erase-empty-entity 2 "\n *	None\n" 2)¨§21¶
/*¥(language-insert-if-comment " *" "\n/*")¨
 * Equated Symbols
 */
§22¶¥(language-start-entity)¨
#define§23¶ «Equated symbol definitions»¥(insert-character '\n')¨
§24¶¥(language-erase-empty-entity 2 "\n *	None\n" 2)¨§25¶
/*¥(language-insert-if-comment " *" "\n/*")¨
 * Own Storage
 */
§26¶¥(language-start-entity)¨
§27¶«Storage definitions in this module»¥(language-opt-cont-entity "Enter the declarators for defined storage in this module and use exit-emacs")¨
§28¶¥(language-erase-empty-entity 1 "\n *	None\n" 2)¨§29¶
/*¥(language-insert-if-comment " *" "\n/*")¨
 * External References
 */
§30¶¥(language-start-entity)¨
§31¶«Extern definitions in this module»¥(language-opt-cont-entity "Enter the declarators for external references and use exit-emacs")¨
§32¶¥(language-erase-empty-entity 1 "\n *	None" 2)¨§33¶¥(language-insert-if-comment "\n */\n")¨§34¶¥(message "Module definition complete")¨
[LaTeX-%]
% §1¶¥(language-fetch-comment "Enter the comment and use exit-emacs" 0 "% ")¨
[LaTeX-{]
{§1¶¥(message "Block.")¨}
[LaTeX-$]
$	deck
$§1¶¥(message "Enter formula")¨$
$	eod
[LaTeX-(]
(§1¶¥(message "Enter formula")¨\)
[LaTeX-[]
 [§1¶¥(message "Enter formula")¨\]
[LaTeX-\begin]
§1¶¥(language-start-entity)¨\begin{§2¶¥(~LaTeX-environment-name)¨}
§3¶¥(progn (language-exit-dot) (message "Enter environment body."))¨
\end{§4¶¥(insert-string ~LaTeX-envir)¨}
[LaTeX-\item]
\item§2¶¥(language-start-entity)¨[§3¶¥(language-opt-cont-entity "Enter item tag and use exit-emacs" 1)¨] §1¶¥(language-exit-dot)¨
[LaTeX-\tag]
§1¶¥(language-start-entity)¨\tag{§3¶¥(language-optional-entity "Enter item tag and use exit-emacs")¨}
§2¶¥(language-exit-dot)¨
[LaTeX-\mbox]
§1¶¥(language-start-entity)¨\mbox{§3¶¥(language-optional-entity "Enter text and use exit-emacs")¨}§2¶ ¥(language-exit-dot)¨
[LaTeX-\footnote]
\footnote§1¶¥(language-start-entity)¨[§3¶¥(language-opt-cont-entity "Enter footnote number and use exit-emacs" 1)¨]{§4¶«Footnote text»¥(language-fetch-mandatory "Enter footnote text and use exit-emacs")¨}§2¶ ¥(language-exit-dot)¨
[LaTeX-\footnotemark]
\footnotemark§2¶¥(language-start-entity)¨[§3¶¥(language-opt-cont-entity "Enter footnote mark and use exit-emacs" 1)¨] §1¶¥(language-exit-dot)¨
[LaTeX-\footnotetext]
\footnotetext§1¶¥(language-start-entity)¨[§3¶¥(language-opt-cont-entity "Enter footnote number and use exit-emacs" 1)¨]{§4¶«Footnote text»¥(language-fetch-mandatory "Enter footnote text and use exit-emacs")¨}§2¶ ¥(language-exit-dot)¨
[LaTeX-\part]
\part§1¶¥(language-start-entity)¨[§3¶¥(language-opt-cont-entity "Enter table of contents entry and use exit-emacs" 1)¨]{§4¶«Part heading»¥(language-fetch-mandatory "Enter part heading and use exit-emacs")¨}§2¶ ¥(language-exit-dot)¨
[LaTeX-\chapter]
\chapter§1¶¥(language-start-entity)¨[§3¶¥(language-opt-cont-entity "Enter table of contents entry and use exit-emacs" 1)¨]{§4¶«Chapter heading»¥(language-fetch-mandatory "Enter chapater heading and use exit-emacs")¨}§2¶ ¥(language-exit-dot)¨
[LaTeX-\section]
\section§1¶¥(language-start-entity)¨[§3¶¥(language-opt-cont-entity "Enter table of contents entry and use exit-emacs" 1)¨]{§4¶«Section heading»¥(language-fetch-mandatory "Enter section heading and use exit-emacs")¨}§2¶ ¥(language-exit-dot)¨
[LaTeX-\subsection]
\subsection§1¶¥(language-start-entity)¨[§3¶¥(language-opt-cont-entity "Enter table of contents entry and use exit-emacs" 1)¨]{§4¶«subsection heading»¥(language-fetch-mandatory "Enter subsection heading and use exit-emacs")¨}§2¶ ¥(language-exit-dot)¨
[LaTeX-\subsubsection]
\subsubsection§1¶¥(language-start-entity)¨[§3¶¥(language-opt-cont-entity "Enter table of contents entry and use exit-emacs" 1)¨]{§4¶«Subsubsection heading»¥(language-fetch-mandatory "Enter subsubsection heading and use exit-emacs")¨}§2¶ ¥(language-exit-dot)¨
[LaTeX-\paragraph]
\paragraph§1¶¥(language-start-entity)¨[§3¶¥(language-opt-cont-entity "Enter table of contents entry and use exit-emacs" 1)¨]{§4¶«Paragraph heading»¥(language-fetch-mandatory "Enter paragraph heading and use exit-emacs")¨}§2¶ ¥(language-exit-dot)¨
[LaTeX-\subparagraph]
\subparagraph§1¶¥(language-start-entity)¨[§3¶¥(language-opt-cont-entity "Enter table of contents entry and use exit-emacs" 1)¨]{§4¶«subparagraph heading»¥(language-fetch-mandatory "Enter subparagraph heading and use exit-emacs")¨}§2¶ ¥(language-exit-dot)¨
[LaTeX-\part*]
§1¶¥(language-start-entity)¨\part*{§3¶¥(language-optional-entity "Enter part heading and use exit-emacs")¨}§2¶ ¥(language-exit-dot)¨
[LaTeX-\chapter*]
§1¶¥(language-start-entity)¨\chapter*{§3¶¥(language-optional-entity "Enter chapter heading and use exit-emacs")¨}§2¶ ¥(language-exit-dot)¨
[LaTeX-\section*]
§1¶¥(language-start-entity)¨\section*{§3¶¥(language-optional-entity "Enter section heading and use exit-emacs")¨}§2¶ ¥(language-exit-dot)¨
[LaTeX-\subsection*]
§1¶¥(language-start-entity)¨\subsection*{§3¶¥(language-optional-entity "Enter subsection heading and use exit-emacs")¨}§2¶ ¥(language-exit-dot)¨
[LaTeX-\subsubsection*]
§1¶¥(language-start-entity)¨\subsubsection*{§3¶¥(language-optional-entity "Enter subsubsection heading and use exit-emacs")¨}§2¶ ¥(language-exit-dot)¨
[LaTeX-\paragraph*]
§1¶¥(language-start-entity)¨\paragraph*{§3¶¥(language-optional-entity "Enter paragraph heading and use exit-emacs")¨}§2¶ ¥(language-exit-dot)¨
[LaTeX-\addcontentsline]
§1¶¥(language-start-entity)¨\addcontentsline{§3¶¥(language-optional-entity "Enter table identifier and use exit-emacs")¨}{§4¶«Section unit»¥(language-optional-entity "Enter section unit and use exit-emacs")¨}{§5¶«Entry text»¥(language-optional-entity "Enter entry text and use exit-emacs")¨}§2¶ ¥(language-exit-dot)¨
[LaTeX-\addtocontents]
§1¶¥(language-start-entity)¨\addtocontents{§3¶¥(language-optional-entity "Enter table identifier and use exit-emacs")¨}{§4¶«Entry text»¥(language-optional-entity "Enter entry text and use exit-emacs")¨}§2¶ ¥(language-exit-dot)¨
[LaTeX-\documentstyle]
\documentstyle§1¶¥(language-start-entity)¨[§3¶¥(language-opt-cont-entity "Enter document options and use exit-emacs" 1)¨]{§4¶«Document style»¥(language-fetch-mandatory "Enter subsubsection heading and use exit-emacs")¨}§2¶ ¥(language-exit-dot)¨
[LaTeX-\pagestyle]
§1¶¥(language-start-entity)¨\pagestyle{§3¶¥(language-optional-entity "Enter page style and use exit-emacs")¨}§2¶ ¥(language-exit-dot)¨
[LaTeX-\markright]
§1¶¥(language-start-entity)¨\markright{§3¶¥(language-optional-entity "Enter right page heading and use exit-emacs")¨}§2¶ ¥(language-exit-dot)¨
[LaTeX-\markboth]
§1¶¥(language-start-entity)¨\markboth{§3¶¥(language-optional-entity "Enter leftpage heading and use exit-emacs")¨}{§4¶«Right page heading»¥(language-optional-entity "Enter right page heading and use exit-emacs")¨}§2¶ ¥(language-exit-dot)¨
[LaTeX-\twocolumn]
\twocolumn§2¶¥(language-start-entity)¨[§3¶¥(language-opt-cont-entity "Enter two column heading and use exit-emacs" 1)¨] §1¶¥(language-exit-dot)¨
[LaTeX-\pagenumbering]
§1¶¥(language-start-entity)¨\pagenumbering{§3¶¥(language-optional-entity "Enter page numbering style and use exit-emacs")¨}§2¶ ¥(language-exit-dot)¨
[LaTeX-\title]
§1¶¥(language-start-entity)¨\title{§3¶¥(language-optional-entity "Enter title text and use exit-emacs")¨}§2¶ ¥(language-exit-dot)¨
[LaTeX-\author]
§1¶¥(language-start-entity)¨\author{§3¶¥(language-optional-entity "Enter author's names and use exit-emacs")¨}§2¶ ¥(language-exit-dot)¨
[LaTeX-\date]
§1¶¥(language-start-entity)¨\date{§3¶¥(language-optional-entity "Enter date and use exit-emacs")¨}§2¶ ¥(language-exit-dot)¨
[LaTeX-\thanks]
§1¶¥(language-start-entity)¨\thanks{§3¶¥(language-optional-entity "Enter text and use exit-emacs")¨}§2¶ ¥(language-exit-dot)¨
[LaTeX-\makelabel]
§1¶¥(language-start-entity)¨\makelabel{§3¶¥(language-optional-entity "Enter label text and use exit-emacs")¨}§2¶ ¥(language-exit-dot)¨
[LaTeX-\usecounter]
§1¶¥(language-start-entity)¨\usecounter{§3¶¥(language-optional-entity "Enter counter name and use exit-emacs")¨}§2¶ ¥(language-exit-dot)¨
[LaTeX-\lefteqn]
§1¶¥(language-start-entity)¨\lefteqn{§3¶¥(language-optional-entity "Enter formula and use exit-emacs")¨}§2¶ ¥(language-exit-dot)¨
[LaTeX-\frac]
§1¶¥(language-start-entity)¨\frac{§3¶¥(language-optional-entity "Enter fraction numerator and use exit-emacs")¨}{§4¶«Denominator»¥(language-optional-entity "Enter fraction denominator and use exit-emacs")¨}§2¶ ¥(language-exit-dot)¨
[LaTeX-\sqrt]
\section§1¶¥(language-start-entity)¨[§3¶¥(language-opt-cont-entity "Enter root value and use exit-emacs" 1)¨]{§4¶«Root»¥(language-fetch-mandatory "Enter root and use exit-emacs")¨}§2¶ ¥(language-exit-dot)¨
[LaTeX-\pmod]
§1¶¥(language-start-entity)¨\pmod{§3¶¥(language-optional-entity "Enter modulus value and use exit-emacs")¨}§2¶ ¥(language-exit-dot)¨
[LaTeX-\overline]
§1¶¥(language-start-entity)¨\overline{§3¶¥(language-optional-entity "Enter formula and use exit-emacs")¨}§2¶ ¥(language-exit-dot)¨
[LaTeX-\underline]
§1¶¥(language-start-entity)¨\underline{§3¶¥(language-optional-entity "Enter formula and use exit-emacs")¨}§2¶ ¥(language-exit-dot)¨
[LaTeX-\stackrel]
§1¶¥(language-start-entity)¨\stackrel{§3¶¥(language-optional-entity "Enter top item and use exit-emacs")¨}{§4¶«Bottom item»¥(language-optional-entity "Enter bottom item and use exit-emacs")¨}§2¶ ¥(language-exit-dot)¨
[LaTeX-\newcommand]
§1¶¥(language-start-entity)¨\newcommand{§3¶¥(language-optional-entity "Enter command to be defined and use exit-emacs")¨}§4¶¥(language-start-entity)¨[§5¶«Arguments»¥(language-opt-cont-entity "Enter number of arguments and use exit-emacs" 1)¨]{§6¶«Definitions»¥(language-fetch-mandatory "Enter command definition and use exit-emacs")¨}§2¶ ¥(language-exit-dot)¨
[LaTeX-\renewcommand]
§1¶¥(language-start-entity)¨\renewcommand{§3¶¥(language-optional-entity "Enter command to be defined and use exit-emacs")¨}§4¶¥(language-start-entity)¨[§5¶«Arguments»¥(language-opt-cont-entity "Enter number of arguments and use exit-emacs" 1)¨]{§6¶«Definitions»¥(language-fetch-mandatory "Enter command definition and use exit-emacs")¨}§2¶ ¥(language-exit-dot)¨
[LaTeX-\newenvironment]
§1¶¥(language-start-entity)¨\newenvironment{§3¶¥(language-optional-entity "Enter environment to be defined and use exit-emacs")¨}§4¶¥(language-start-entity)¨[§5¶«Arguments»¥(language-opt-cont-entity "Enter number of arguments and use exit-emacs" 1)¨]{§6¶«Begin definitions»¥(language-fetch-mandatory "Enter begin definitions and use exit-emacs")¨}{§7¶«End definitions»¥(language-fetch-mandatory "Enter end definitions and use exit-emacs")¨}§2¶ ¥(language-exit-dot)¨
[LaTeX-\renewenvironment]
§1¶¥(language-start-entity)¨\renewenvironment{§3¶¥(language-optional-entity "Enter environment to be defined and use exit-emacs")¨}§4¶¥(language-start-entity)¨[§5¶«Arguments»¥(language-opt-cont-entity "Enter number of arguments and use exit-emacs" 1)¨]{§6¶«Begin definitions»¥(language-fetch-mandatory "Enter begin definitions and use exit-emacs")¨}{§7¶«End definitions»¥(language-fetch-mandatory "Enter end definitions and use exit-emacs")¨}§2¶ ¥(language-exit-dot)¨
[LaTeX-\newtheorem]
§1¶¥(language-start-entity)¨\newtheorem{§3¶¥(language-optional-entity "Enter theorem to be defined and use exit-emacs")¨}§4¶¥(language-start-entity)¨[§5¶«Numbered like»¥(language-opt-cont-entity "Enter numbering system and use exit-emacs" 1)¨]{§6¶«Caption»¥(language-fetch-mandatory "Enter caption text and use exit-emacs")¨}§7¶¥(language-start-entity)¨[§8¶«Within»¥(language-opt-cont-entity "Enter within counter and use exit-emacs" 1)¨]§2¶ ¥(language-exit-dot)¨
[LaTeX-\newcounter]
§1¶¥(language-start-entity)¨\newcounter{§3¶¥(language-fetch-optional "Enter counter name and use exit-emacs")¨}§4¶¥(language-start-entity)¨[§5¶«Within»¥(language-opt-cont-entity "Enter within counter and use exit-emacs" 1)¨]§2¶ ¥(language-exit-dot)¨
[LaTeX-\setcounter]
§1¶¥(language-start-entity)¨\setcounter{§3¶¥(language-optional-entity "Enter counter name and use exit-emacs")¨}{§4¶«Value»¥(language-optional-entity "Enter value and use exit-emacs")¨}§2¶ ¥(language-exit-dot)¨
[LaTeX-\addtocounter]
§1¶¥(language-start-entity)¨\addtocounter{§3¶¥(language-optional-entity "Enter counter name and use exit-emacs")¨}{§4¶«Increment»¥(language-optional-entity "Enter value heading and use exit-emacs")¨}§2¶ ¥(language-exit-dot)¨
[LaTeX-\value]
§1¶¥(language-start-entity)¨\value{§3¶¥(language-optional-entity "Enter counter name and use exit-emacs")¨}§2¶ ¥(language-exit-dot)¨
[LaTeX-\arabic]
§1¶¥(language-start-entity)¨\arabic{§3¶¥(language-optional-entity "Enter counter name and use exit-emacs")¨}§2¶ ¥(language-exit-dot)¨
[LaTeX-\roman]
§1¶¥(language-start-entity)¨\roman{§3¶¥(language-optional-entity "Enter counter name and use exit-emacs")¨}§2¶ ¥(language-exit-dot)¨
[LaTeX-\alpha]
§1¶¥(language-start-entity)¨\alpha{§3¶¥(language-optional-entity "Enter counter name and use exit-emacs")¨}§2¶ ¥(language-exit-dot)¨
[LaTeX-\fnsymbol]
§1¶¥(language-start-entity)¨\fnsymbol{§3¶¥(language-optional-entity "Enter counter name and use exit-emacs")¨}§2¶ ¥(language-exit-dot)¨
[LaTeX-\caption]
\caption§1¶¥(language-start-entity)¨[§3¶¥(language-opt-cont-entity "Enter list-of-figures text and use exit-emacs" 1)¨]{§4¶«Caption»¥(language-fetch-mandatory "Enter caption text and use exit-emacs")¨}§2¶ ¥(language-exit-dot)¨
[LaTeX-\marginpar]
\marginpar§1¶¥(language-start-entity)¨[§3¶¥(language-opt-cont-entity "Enter left margin text and use exit-emacs" 1)¨]{§4¶«Right margin text»¥(language-fetch-mandatory "Enter right margin text and use exit-emacs")¨}§2¶ ¥(language-exit-dot)¨
[LaTeX-\multicolumn]
§1¶¥(language-start-entity)¨\multicolumn{§3¶¥(language-optional-entity "Enter number of columns to span and use exit-emacs")¨}{§4¶«Column information»¥(language-optional-entity "Enter column information and use exit-emacs")¨}{§5¶«Text»¥(language-optional-entity "Enter column text and use exit-emacs")¨}§2¶ ¥(language-exit-dot)¨
[LaTeX-\cline]
§1¶¥(language-start-entity)¨\cline{§3¶¥(language-optional-entity "Enter column specification and use exit-emacs")¨}§2¶ ¥(language-exit-dot)¨
[LaTeX-\label]
§1¶¥(language-start-entity)¨\label{§3¶¥(language-optional-entity "Enter label name and use exit-emacs")¨}§2¶ ¥(language-exit-dot)¨
[LaTeX-\ref]
§1¶¥(language-start-entity)¨\ref{§3¶¥(language-optional-entity "Enter label name and use exit-emacs")¨}§2¶ ¥(language-exit-dot)¨
[LaTeX-\pageref]
§1¶¥(language-start-entity)¨\pageref{§3¶¥(language-optional-entity "Enter label name and use exit-emacs")¨}§2¶ ¥(language-exit-dot)¨
[LaTeX-\bibliography]
§1¶¥(language-start-entity)¨\bibliography{§3¶¥(language-optional-entity "Enter bibliograph file name and use exit-emacs")¨}§2¶ ¥(language-exit-dot)¨
[LaTeX-\bibitem]
\bibitem§1¶¥(language-start-entity)¨[§3¶¥(language-opt-cont-entity "Enter entery label and use exit-emacs" 1)¨]{§4¶«Cite key»¥(language-fetch-mandatory "Enter the citation key and use exit-emacs")¨}§2¶ ¥(language-exit-dot)¨
[LaTeX-\cite]
\cite§1¶¥(language-start-entity)¨[§3¶¥(language-opt-cont-entity "Enter citation reamrk and use exit-emacs" 1)¨]{§4¶«Citation keys»¥(language-fetch-mandatory "Enter citation keys and use exit-emacs")¨}§2¶ ¥(language-exit-dot)¨
[LaTeX-\nocite]
§1¶¥(language-start-entity)¨\nocite{§3¶¥(language-optional-entity "Enter citation keys and use exit-emacs")¨}§2¶ ¥(language-exit-dot)¨
[LaTeX-\input]
§1¶¥(language-start-entity)¨\input{§3¶¥(language-optional-entity "Enter file name and use exit-emacs")¨}§2¶ ¥(language-exit-dot)¨
[LaTeX-\include]
§1¶¥(language-start-entity)¨\include{§3¶¥(language-optional-entity "Enter file name and use exit-emacs")¨}§2¶ ¥(language-exit-dot)¨
[LaTeX-\includeonly]
§1¶¥(language-start-entity)¨\includeonly{§3¶¥(language-optional-entity "Enter file name list and use exit-emacs")¨}§2¶ ¥(language-exit-dot)¨
[LaTeX-\index]
§1¶¥(language-start-entity)¨\index{§3¶¥(language-optional-entity "Enter index entry and use exit-emacs")¨}§2¶ ¥(language-exit-dot)¨
[LaTeX-\glossary]
§1¶¥(language-start-entity)¨\glossary{§3¶¥(language-optional-entity "Enter glossary entry and use exit-emacs")¨}§2¶ ¥(language-exit-dot)¨
[LaTeX-\typeout]
§1¶¥(language-start-entity)¨\typeout{§3¶¥(language-optional-entity "Enter message and use exit-emacs")¨}§2¶ ¥(language-exit-dot)¨
[LaTeX-\typein]
\typein§1¶¥(language-start-entity)¨[§3¶¥(language-opt-cont-entity "Enter command to be redefined and use exit-emacs" 1)¨]{§4¶«Prompt»¥(language-fetch-mandatory "Enter prompt text and use exit-emacs")¨}§2¶ ¥(language-exit-dot)¨
[LaTeX-\linebreak]
\linebreak§2¶¥(language-start-entity)¨[§3¶¥(language-opt-cont-entity "Enter strength number and use exit-emacs" 1)¨] §1¶¥(language-exit-dot)¨
[LaTeX-\nolinebreak]
\nolinebreak§2¶¥(language-start-entity)¨[§3¶¥(language-opt-cont-entity "Enter strength number and use exit-emacs" 1)¨] §1¶¥(language-exit-dot)¨
[LaTeX-\hyphenation]
§1¶¥(language-start-entity)¨\hyphenation{§3¶¥(language-optional-entity "Enter words to hyphenate and use exit-emacs")¨}§2¶ ¥(language-exit-dot)¨
[LaTeX-\pagebreak]
\pagebreak§2¶¥(language-start-entity)¨[§3¶¥(language-opt-cont-entity "Enter strength number and use exit-emacs" 1)¨] §1¶¥(language-exit-dot)¨
[LaTeX-\nopagebreak]
\nopagebreak§2¶¥(language-start-entity)¨[§3¶¥(language-opt-cont-entity "Enter strength number and use exit-emacs" 1)¨] §1¶¥(language-exit-dot)¨
[LaTeX-\stretch]
§1¶¥(language-start-entity)¨\stretch{§3¶¥(language-optional-entity "Enter stretchability factor and use exit-emacs")¨}§2¶ ¥(language-exit-dot)¨
[LaTeX-\newlength]
§1¶¥(language-start-entity)¨\newlength{§3¶¥(language-optional-entity "Enter length command and use exit-emacs")¨}§2¶ ¥(language-exit-dot)¨
[LaTeX-\setlength]
§1¶¥(language-start-entity)¨\setlength{§3¶¥(language-optional-entity "Enter length command and use exit-emacs")¨}{§4¶«Value»¥(language-optional-entity "Enter value and use exit-emacs")¨}§2¶ ¥(language-exit-dot)¨
[LaTeX-\addtolength]
§1¶¥(language-start-entity)¨\addtolength{§3¶¥(language-optional-entity "Enter length command and use exit-emacs")¨}{§4¶«Value»¥(language-optional-entity "Enter value and use exit-emacs")¨}§2¶ ¥(language-exit-dot)¨
[LaTeX-\settowidth]
§1¶¥(language-start-entity)¨\settowidth{§3¶¥(language-optional-entity "Enter width command and use exit-emacs")¨}{§4¶«Value»¥(language-optional-entity "Enter value and use exit-emacs")¨}§2¶ ¥(language-exit-dot)¨
[LaTeX-\hspace]
§1¶¥(language-start-entity)¨\hspace{§3¶¥(language-optional-entity "Enter space length and use exit-emacs")¨}§2¶ ¥(language-exit-dot)¨
[LaTeX-\hspace*]
§1¶¥(language-start-entity)¨\hspace*{§3¶¥(language-optional-entity "Enter space length and use exit-emacs")¨}§2¶ ¥(language-exit-dot)¨
[LaTeX-\vspace]
§1¶¥(language-start-entity)¨\vspace{§3¶¥(language-optional-entity "Enter space length and use exit-emacs")¨}§2¶ ¥(language-exit-dot)¨
[LaTeX-\vspace*]
§1¶¥(language-start-entity)¨\vspace*{§3¶¥(language-optional-entity "Enter space length and use exit-emacs")¨}§2¶ ¥(language-exit-dot)¨
[LaTeX-\addvspace]
§1¶¥(language-start-entity)¨\addvspace{§3¶¥(language-optional-entity "Enter space to add and use exit-emacs")¨}§2¶ ¥(language-exit-dot)¨
[LaTeX-\makebox]
\makebox§1¶¥(language-start-entity)¨[§3¶¥(language-opt-cont-entity "Enter width and use exit-emacs" 1)¨]§4¶¥(language-start-entity)¨[§5¶«Postion»¥(language-opt-cont-entity "Enter position specifier and use exit-emacs" 1)¨]{§6¶«Text»¥(language-fetch-mandatory "Enter text and use exit-emacs")¨}§2¶ ¥(language-exit-dot)¨
[LaTeX-\fbox]
§1¶¥(language-start-entity)¨\fbox{§3¶¥(language-optional-entity "Enter text and use exit-emacs")¨}§2¶ ¥(language-exit-dot)¨
[LaTeX-\framebox]
\framebox§1¶¥(language-start-entity)¨[§3¶¥(language-opt-cont-entity "Enter width and use exit-emacs" 1)¨]§4¶¥(language-start-entity)¨[§5¶«Postion»¥(language-opt-cont-entity "Enter position specifier and use exit-emacs" 1)¨]{§6¶«Text»¥(language-fetch-mandatory "Enter text and use exit-emacs")¨}§2¶ ¥(language-exit-dot)¨
[LaTeX-\sbox]
§1¶¥(language-start-entity)¨\sbox{§3¶¥(language-optional-entity "Enter box command and use exit-emacs")¨}{§4¶«Text»¥(language-optional-entity "Enter text and use exit-emacs")¨}§2¶ ¥(language-exit-dot)¨
[LaTeX-\savebox]
§1¶¥(language-start-entity)¨\savebox{§3¶¥(language-optional-entity "Enter box command and use exit-emacs")¨}§4¶¥(language-start-entity)¨[§5¶«Width»¥(language-opt-cont-entity "Enter box width and use exit-emacs" 1)¨]§6¶¥(language-start-entity)¨[§7¶«Position»¥(language-opt-cont-entity "Enter position specifier and use exit-emacs" 1)¨]{§8¶«Text»¥(language-fetch-mandatory "Enter text and use exit-emacs")¨}§2¶ ¥(language-exit-dot)¨
[LaTeX-\usebox]
§1¶¥(language-start-entity)¨\usebox{§3¶¥(language-optional-entity "Enter box command and use exit-emacs")¨}§2¶ ¥(language-exit-dot)¨
[LaTeX-\parbox]
\parbox§1¶¥(language-start-entity)¨[§3¶¥(language-opt-cont-entity "Enter position specifier and use exit-emacs" 1)¨]{§4¶«Width»¥(language-fetch-mandatory "Enter width and use exit-emacs")¨}{§5¶«Text»¥(language-fetch-mandatory "Enter text and use exit-emacs")¨}§2¶ ¥(language-exit-dot)¨
[LaTeX-\rule]
\rule§1¶¥(language-start-entity)¨[§3¶¥(language-opt-cont-entity "Enter raise length and use exit-emacs" 1)¨]{§4¶«Width»¥(language-fetch-mandatory "Enter width and use exit-emacs")¨}{§5¶«Hight»¥(language-fetch-mandatory "Enter hight and use exit-emacs")¨}§2¶ ¥(language-exit-dot)¨
[LaTeX-\raisebox]
§1¶¥(language-start-entity)¨\raisebox{§3¶¥(language-optional-entity "Enter raise length and use exit-emacs")¨}§4¶¥(language-start-entity)¨[§5¶«Hight»¥(language-opt-cont-entity "Enter hight and use exit-emacs" 1)¨]§6¶¥(language-start-entity)¨[§7¶«Depth»¥(language-opt-cont-entity "Enter depth and use exit-emacs" 1)¨]{§8¶«Text»¥(language-fetch-mandatory "Enter text and use exit-emacs")¨}§2¶ ¥(language-exit-dot)¨
[LaTeX-\put]
§1¶¥(language-start-entity)¨\put(§3¶¥(language-optional-entity "Enter position and use exit-emacs")¨){§4¶«Picture object»¥(language-optional-entity "Enter picture object and use exit-emacs")¨}§2¶ ¥(language-exit-dot)¨
[LaTeX-\multiput]
§1¶¥(language-start-entity)¨\multiput(§3¶¥(language-optional-entity "Enter position and use exit-emacs")¨)(§4¶«Delta coordinates»¥(language-optional-entity "Enter adjustment and use exit-emacs")¨){§5¶«Repetition»¥(language-optional-entity "Enter number of objects and use exit-emacs")¨}{§6¶«Picture object»¥(language-optional-entity "Enter picture object and use exit-emacs")¨}§2¶ ¥(language-exit-dot)¨
[LaTeX-\line]
§1¶¥(language-start-entity)¨\line(§3¶¥(language-optional-entity "Enter slope specifier and use exit-emacs")¨){§4¶«length»¥(language-optional-entity "Enter length and use exit-emacs")¨}§2¶ ¥(language-exit-dot)¨
[LaTeX-\vector]
§1¶¥(language-start-entity)¨\vector(§3¶¥(language-optional-entity "Enter slope specifier and use exit-emacs")¨){§4¶«length»¥(language-optional-entity "Enter length and use exit-emacs")¨}§2¶ ¥(language-exit-dot)¨
[LaTeX-\shortstack]
\shortstack§1¶¥(language-start-entity)¨[§3¶¥(language-opt-cont-entity "Enter position specifier and use exit-emacs" 1)¨]{§4¶«Column»¥(language-fetch-mandatory "Enter column and use exit-emacs")¨}§2¶ ¥(language-exit-dot)¨
[LaTeX-\circle]
§1¶¥(language-start-entity)¨\circle{§3¶¥(language-optional-entity "Enter diameter and use exit-emacs")¨}§2¶ ¥(language-exit-dot)¨
[LaTeX-\circle*]
§1¶¥(language-start-entity)¨\circle*{§3¶¥(language-optional-entity "Enter diameter and use exit-emacs")¨}§2¶ ¥(language-exit-dot)¨
[LaTeX-\oval]
§1¶¥(language-start-entity)¨\oval(§3¶¥(language-optional-entity "Enter dimensions and use exit-emacs")¨){§4¶«Part»¥(language-optional-entity "Enter part specifier and use exit-emacs")¨}§2¶ ¥(language-exit-dot)¨
[LaTeX-\frame]
§1¶¥(language-start-entity)¨\frame{§3¶¥(language-optional-entity "Enter picture object and use exit-emacs")¨}§2¶ ¥(language-exit-dot)¨
[LaTeX-\newfont]
§1¶¥(language-start-entity)¨\newfont{§3¶¥(language-optional-entity "Enter font command and use exit-emacs")¨}{§4¶«Font name»¥(language-optional-entity "Enter font name and use exit-emacs")¨}§2¶ ¥(language-exit-dot)¨
[LaTeX-\symbol]
§1¶¥(language-start-entity)¨\symbol{§3¶¥(language-optional-entity "Enter symbol identification and use exit-emacs")¨}§2¶ ¥(language-exit-dot)¨
[LaTeX-\load]
§1¶¥(language-start-entity)¨\load{§3¶¥(language-optional-entity "Enter size and use exit-emacs")¨}{§4¶«Style»¥(language-optional-entity "Enter style and use exit-emacs")¨}§2¶ ¥(language-exit-dot)¨
