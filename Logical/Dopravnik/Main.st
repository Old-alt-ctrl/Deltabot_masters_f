
PROGRAM _INIT
	(* Insert code here *)
	state := 0;
	tmp_actCmd := 0;
END_PROGRAM

PROGRAM _CYCLIC
	(* Insert code here *)
	 //nabezne hrany tlacitek
	trig_On(CLK := conOn);
	trig_Start(CLK := conSt);
	trig_Plus(CLK := conSpp);
	trig_Minus(CLK := conSpm);
	
	IF tmp_actCmd = 0 THEN
		IF trig_On.Q THEN
			tmp_actCmd := 1; // prikaz 1 je ON
		ELSIF trig_Start.Q THEN
			tmp_actCmd := 2; // prikaz 2 je Start
		ELSIF trig_Plus.Q THEN
			tmp_actCmd := 3; // prikaz 3 je Plus
		ELSIF trig_Minus.Q THEN
			tmp_actCmd := 4; // prikaz 4 je Minus
		END_IF
	END_IF
	CASE state OF
		0:	//Idle - cteni
			butEn := TRUE;
			fb_Pulse(IN:=FALSE);
			do_OnOff := FALSE;
			do_Start := FALSE;
			do_SpP := FALSE;
			do_SpM := FALSE;
			
			IF tmp_actCmd <> 0 THEN
				IF trig_On.Q THEN
					tmp_actCmd := 1; // prikaz 1 je ON
					state := 1;
				ELSIF trig_Start.Q THEN
					tmp_actCmd := 2; // prikaz 2 je Start
					state := 1;
				ELSIF trig_Plus.Q THEN
					tmp_actCmd := 3; // prikaz 3 je Plus
					state := 1;
				ELSIF trig_Minus.Q THEN
					tmp_actCmd := 4; // prikaz 4 je Minus
					state := 1;
				END_IF
			END_IF
		1:	//Pulse - generovani pulsu
			butEn := FALSE;
			conOn := FALSE;
			conSt := FALSE;
			conSpp := FALSE;
			conSpm := FALSE;
			do_OnOff := (tmp_actCmd = 1);
			do_Start := (tmp_actCmd = 2);
			do_SpP := (tmp_actCmd = 3);
			do_SpM := (tmp_actCmd = 4);
			fb_Pulse(IN:=TRUE, PT:=T#100ms);
			IF NOT fb_Pulse.Q THEN
				fb_Pulse(IN:=FALSE);
				state := 2;
			END_IF
		2:	//Zpozdeni - korekce zakmitu
			do_OnOff := FALSE;
			do_Start := FALSE;
			do_SpP := FALSE;
			do_SpM := FALSE;
			butEn := FALSE;
			fb_Timer(IN:= TRUE, PT:= T#30ms);
			IF fb_Timer.Q THEN
				fb_Timer(IN:= FALSE);
				tmp_actCmd := 0;
				state := 0;
			END_IF
	END_CASE
END_PROGRAM

PROGRAM _EXIT
	(* Insert code here *)
	state := 0;
	tmp_actCmd := 0;
	do_OnOff := FALSE;
	do_Start := FALSE;
	do_SpP := FALSE;
	do_SpM := FALSE;
END_PROGRAM

