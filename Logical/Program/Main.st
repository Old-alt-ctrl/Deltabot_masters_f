
PROGRAM _INIT
	// reference na roboticky system
	DAxis.MpLink := ADR(g3AxDeltaB);
	// prirazeni parametru
	DAxis.Parameters := ADR(DPara);
	// povoleni plne rychlosti
	DAxis.Override := 100;
	// povoleni funkcniho bloku
	DAxis.Enable := TRUE ;
	// prirazeni vychoziho pohyboveho programu
	//DPara.ProgramName := ’Home .st ’;
	// Provedeni funkcniho bloku robota
	DAxis();
END_PROGRAM

PROGRAM _CYCLIC
	(* Insert code here *)
	DAxis.Update := FALSE;
	DAxis.ErrorReset := FALSE;
		
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
		0: // zapnuti napajeni motoru
			IF DAxis.Active AND DAxis.Info.ReadyToPowerOn
				AND DAxis.Info.CommunicationReady THEN
				DAxis.Power := TRUE ;
				state := 1;
			END_IF ;
			
		1: // nalezeni vychozich bodu
			IF DAxis.PowerOn AND NOT DAxis.IsHomed THEN
				DAxis.Home := TRUE ;
			END_IF ;
			IF DAxis.IsHomed THEN
				DAxis.Home := FALSE ;
				state := 2;
			END_IF ;
			
		40: // errorovy stav
			DAxis.MoveDirect := FALSE;
			DAxis.MoveLinear := FALSE;
			DAxis.MoveProgram := FALSE;
			DAxis.Power := FALSE;
			IF err THEN
				DAxis.ErrorReset := TRUE;
				DAxis.Update := TRUE;
				err := FALSE;
				state := 0;
			ELSIF NOT DAxis.Error THEN
				state := 0;
			END_IF;
	END_CASE;
	
	U := DAxis.Info.JointAxisPosition[0];
	V := DAxis.Info.JointAxisPosition[1];
	W := DAxis.Info.JointAxisPosition[2];
	
	IF DAxis.Error THEN
		state := 40;
	END_IF;
	
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
	// provedeni funkcniho bloku robota
	DAxis();
END_PROGRAM

