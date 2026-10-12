
PROGRAM _INIT
	// reference na robotický systém
	DAxis.MpLink := ADR(g3AxDeltaB);
	// pøiøazení parametrù
	DAxis.Parameters := ADR(DPara);
	// povolení plné rychlosti
	DAxis.Override := 100;
	// povolení funkèního bloku
	DAxis.Enable := TRUE ;
	// pøiøazení výchozího pohybového programu
	//DPara.ProgramName := ’Home .st ’;
	// Provedeni funkèního bloku robota
	DAxis();
END_PROGRAM

PROGRAM _CYCLIC
	(* Insert code here *)
	DAxis.Update := FALSE;
	
	IF select = '0' THEN // Test
		DPara.ProgramName := 'Robotic.st';
		DAxis.Update := TRUE;
	ELSIF select = '1' THEN // Pick&Place
		DPara.ProgramName := 'Robotic2.st';
		DAxis.Update := TRUE;
	ELSIF select = '2' THEN // Fast movement
		DPara.ProgramName := 'MoveDemo.st';
		DAxis.Update := TRUE;
	ELSIF select = '3' THEN // Circle
		DPara.ProgramName := 'Circ.st';
		DAxis.Update := TRUE;
	END_IF;
	
	CASE state OF
		0: // zapnutí napájení motorù
			IF DAxis.Active AND DAxis.Info.ReadyToPowerOn
				AND DAxis.Info.CommunicationReady THEN
				DAxis.Power := TRUE ;
				state := 1;
			END_IF ;
		1: // nalezení výchozích bodù
			IF DAxis.PowerOn AND NOT DAxis.IsHomed THEN
				DAxis.Home := TRUE ;
			END_IF ;
			IF DAxis.IsHomed THEN
				DAxis.Home := FALSE ;
				state := 2;
			END_IF ;
	END_CASE;
	
	U := DAxis.Info.JointAxisPosition[0];
	V := DAxis.Info.JointAxisPosition[1];
	W := DAxis.Info.JointAxisPosition[2];
	
	DAxis();
END_PROGRAM

PROGRAM _EXIT
	(* Insert code here *)
	DAxis.Power := FALSE ;
	DAxis.Home := FALSE ;
	DAxis.MoveDirect := FALSE ;
	DAxis.MoveLinear := FALSE ;
	DAxis.MoveProgram := FALSE ;
	DAxis.Enable := FALSE ;
	// provedeni funkèního bloku robota
	DAxis();
END_PROGRAM

