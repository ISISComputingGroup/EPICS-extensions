<?xml version='1.0' encoding='UTF-8'?>
<Project Type="Project" LVVersion="11008008">
	<Property Name="varPersistentID:{0DC72635-E84F-452D-8487-DAB201F7C803}" Type="Ref">/My Computer/Eurotherm/EurothermSimple2Var.lvlib/G1</Property>
	<Property Name="varPersistentID:{20790163-E4FF-4BF0-B13A-AF82EED11745}" Type="Ref">/My Computer/EuroS2.lvlib/S2</Property>
	<Property Name="varPersistentID:{2113D880-92C1-45C5-BFB6-1EC632BA7ECC}" Type="Ref">/My Computer/EuroS2.lvlib/R2</Property>
	<Property Name="varPersistentID:{3AC554C6-CBCE-405E-AA90-DEB18CE0119F}" Type="Ref">/My Computer/EuroS2.lvlib/G1</Property>
	<Property Name="varPersistentID:{42150948-2CD8-4ED6-B226-A5C57F1414AE}" Type="Ref">/My Computer/Eurotherm/EurothermSimple2Var.lvlib/S1</Property>
	<Property Name="varPersistentID:{57703236-6ABC-4B18-88A7-79239CE24415}" Type="Ref">/My Computer/EuroS2.lvlib/S1</Property>
	<Property Name="varPersistentID:{58316154-1F98-46F4-BBB1-F711DCA7EAB1}" Type="Ref">/My Computer/EuroS2.lvlib/T1</Property>
	<Property Name="varPersistentID:{66420936-BF8D-4329-B2F7-E2D77AD246C1}" Type="Ref">/My Computer/EuroS2.lvlib/G2</Property>
	<Property Name="varPersistentID:{6BDF59C5-F732-43FA-BC8E-8223027D277F}" Type="Ref">/My Computer/Eurotherm/EurothermSimple2Var.lvlib/R1</Property>
	<Property Name="varPersistentID:{85BAA7C3-B89E-4220-91B7-AC14A8DDEA82}" Type="Ref">/My Computer/Eurotherm/EurothermSimple2Var.lvlib/T2</Property>
	<Property Name="varPersistentID:{89F9FA4A-3D84-4F9B-9227-B1D36DEC10E2}" Type="Ref">/My Computer/Eurotherm/EurothermSimple2Var.lvlib/T1</Property>
	<Property Name="varPersistentID:{AC64D018-3FCF-40B3-B8A9-685A96E746E6}" Type="Ref">/My Computer/Eurotherm/EurothermSimple2Var.lvlib/G2</Property>
	<Property Name="varPersistentID:{B93F2E42-D837-48C5-AA24-FD72E91C5BE2}" Type="Ref">/My Computer/Eurotherm/EurothermSimple2Var.lvlib/S2</Property>
	<Property Name="varPersistentID:{CCCFA189-E965-45E1-B5CC-DA10BF188583}" Type="Ref">/My Computer/Eurotherm/EurothermSimple2Var.lvlib/R2</Property>
	<Property Name="varPersistentID:{E5B42380-62C8-441B-B3CE-DAE465B79042}" Type="Ref">/My Computer/EuroS2.lvlib/R1</Property>
	<Property Name="varPersistentID:{F37A662C-FAA4-4C04-9528-F263B42DD372}" Type="Ref">/My Computer/EuroS2.lvlib/T2</Property>
	<Item Name="My Computer" Type="My Computer">
		<Property Name="IOScan.Faults" Type="Str"></Property>
		<Property Name="IOScan.NetVarPeriod" Type="UInt">100</Property>
		<Property Name="IOScan.NetWatchdogEnabled" Type="Bool">false</Property>
		<Property Name="IOScan.Period" Type="UInt">10000</Property>
		<Property Name="IOScan.PowerupMode" Type="UInt">0</Property>
		<Property Name="IOScan.Priority" Type="UInt">9</Property>
		<Property Name="IOScan.ReportModeConflict" Type="Bool">true</Property>
		<Property Name="IOScan.StartEngineOnDeploy" Type="Bool">false</Property>
		<Property Name="server.app.propertiesEnabled" Type="Bool">true</Property>
		<Property Name="server.control.propertiesEnabled" Type="Bool">true</Property>
		<Property Name="server.tcp.enabled" Type="Bool">false</Property>
		<Property Name="server.tcp.port" Type="Int">0</Property>
		<Property Name="server.tcp.serviceName" Type="Str">My Computer/VI Server</Property>
		<Property Name="server.tcp.serviceName.default" Type="Str">My Computer/VI Server</Property>
		<Property Name="server.vi.callsEnabled" Type="Bool">true</Property>
		<Property Name="server.vi.propertiesEnabled" Type="Bool">true</Property>
		<Property Name="specify.custom.address" Type="Bool">false</Property>
		<Item Name="Common" Type="Folder" URL="../../../Common">
			<Property Name="NI.DISK" Type="Bool">true</Property>
		</Item>
		<Item Name="DEMO" Type="Folder">
			<Item Name="Eurotherm - Front Panel - Simple x 2.vi" Type="VI" URL="../Eurotherm - System Functions/Eurotherm - Front Panel - Simple x 2.vi"/>
			<Item Name="Eurotherm - Setup Dialog.vi" Type="VI" URL="../Eurotherm - System Functions/Eurotherm - Setup Dialog.vi"/>
			<Item Name="Eurotherm - VISA Serial - Command.vi" Type="VI" URL="../Eurotherm - Comms/Eurotherm - VISA Serial - Command.vi"/>
		</Item>
		<Item Name="Eurotherm" Type="Folder">
			<Item Name="Eurotherm - Comms" Type="Folder">
				<Property Name="NI.SortType" Type="Int">3</Property>
				<Item Name="Eurotherm - Address String.vi" Type="VI" URL="../Eurotherm - Comms/Eurotherm - Address String.vi"/>
				<Item Name="Eurotherm - Generate Checksum.vi" Type="VI" URL="../Eurotherm - Comms/Eurotherm - Generate Checksum.vi"/>
				<Item Name="Eurotherm - Validate Checksum.vi" Type="VI" URL="../Eurotherm - Comms/Eurotherm - Validate Checksum.vi"/>
				<Item Name="Eurotherm - VISA Serial - Clear Buffer.vi" Type="VI" URL="../Eurotherm - Comms/Eurotherm - VISA Serial - Clear Buffer.vi"/>
				<Item Name="Eurotherm - VISA Serial - Close Session.vi" Type="VI" URL="../Eurotherm - Comms/Eurotherm - VISA Serial - Close Session.vi"/>
				<Item Name="Eurotherm - VISA Serial - Initialize Session.vi" Type="VI" URL="../Eurotherm - Comms/Eurotherm - VISA Serial - Initialize Session.vi"/>
				<Item Name="Eurotherm - VISA Serial - Read Reply.vi" Type="VI" URL="../Eurotherm - Comms/Eurotherm - VISA Serial - Read Reply.vi"/>
				<Item Name="Eurotherm - VISA Serial - Write Command.vi" Type="VI" URL="../Eurotherm - Comms/Eurotherm - VISA Serial - Write Command.vi"/>
			</Item>
			<Item Name="Eurotherm - Controls" Type="Folder">
				<Item Name="Eurotherm - Address.ctl" Type="VI" URL="../Eurotherm - Controls/Eurotherm - Address.ctl"/>
				<Item Name="Eurotherm - AutoTune State.ctl" Type="VI" URL="../Eurotherm - Controls/Eurotherm - AutoTune State.ctl"/>
				<Item Name="Eurotherm - Communication.ctl" Type="VI" URL="../Eurotherm - Controls/Eurotherm - Communication.ctl"/>
				<Item Name="Eurotherm - Setup.ctl" Type="VI" URL="../Eurotherm - Controls/Eurotherm - Setup.ctl"/>
				<Item Name="Eurotherm - Setups.ctl" Type="VI" URL="../Eurotherm - Controls/Eurotherm - Setups.ctl"/>
			</Item>
			<Item Name="Eurotherm - Functions" Type="Folder">
				<Item Name="Eurotherm - Read Auto Tune Status.vi" Type="VI" URL="../Eurotherm - Functions/Eurotherm - Read Auto Tune Status.vi"/>
				<Item Name="Eurotherm - Read D Term.vi" Type="VI" URL="../Eurotherm - Functions/Eurotherm - Read D Term.vi"/>
				<Item Name="Eurotherm - Read I Term.vi" Type="VI" URL="../Eurotherm - Functions/Eurotherm - Read I Term.vi"/>
				<Item Name="Eurotherm - Read Max Output.vi" Type="VI" URL="../Eurotherm - Functions/Eurotherm - Read Max Output.vi"/>
				<Item Name="Eurotherm - Read Output.vi" Type="VI" URL="../Eurotherm - Functions/Eurotherm - Read Output.vi"/>
				<Item Name="Eurotherm - Read P Term.vi" Type="VI" URL="../Eurotherm - Functions/Eurotherm - Read P Term.vi"/>
				<Item Name="Eurotherm - Read Sensor (Process Variable).vi" Type="VI" URL="../Eurotherm - Functions/Eurotherm - Read Sensor (Process Variable).vi"/>
				<Item Name="Eurotherm - Read Set Point.vi" Type="VI" URL="../Eurotherm - Functions/Eurotherm - Read Set Point.vi"/>
				<Item Name="Eurotherm - Set Auto Tune Status.vi" Type="VI" URL="../Eurotherm - Functions/Eurotherm - Set Auto Tune Status.vi"/>
				<Item Name="Eurotherm - Set D Term.vi" Type="VI" URL="../Eurotherm - Functions/Eurotherm - Set D Term.vi"/>
				<Item Name="Eurotherm - Set I Term.vi" Type="VI" URL="../Eurotherm - Functions/Eurotherm - Set I Term.vi"/>
				<Item Name="Eurotherm - Set Max Output.vi" Type="VI" URL="../Eurotherm - Functions/Eurotherm - Set Max Output.vi"/>
				<Item Name="Eurotherm - Set P Term.vi" Type="VI" URL="../Eurotherm - Functions/Eurotherm - Set P Term.vi"/>
				<Item Name="Eurotherm - Set Set Point.vi" Type="VI" URL="../Eurotherm - Functions/Eurotherm - Set Set Point.vi"/>
			</Item>
			<Item Name="Eurotherm - System Functions" Type="Folder">
				<Item Name="Eurotherm - Acquire Data &amp; Update Display.vi" Type="VI" URL="../Eurotherm - System Functions/Eurotherm - Acquire Data &amp; Update Display.vi"/>
				<Item Name="Eurotherm - Calculate Measured Reading.vi" Type="VI" URL="../Eurotherm - System Functions/Eurotherm - Calculate Measured Reading.vi"/>
				<Item Name="Eurotherm - Calculate Sensor Reading.vi" Type="VI" URL="../Eurotherm - System Functions/Eurotherm - Calculate Sensor Reading.vi"/>
				<Item Name="Eurotherm - Setup Equal to Previous Setup.vi" Type="VI" URL="../Eurotherm - System Functions/Eurotherm - Setup Equal to Previous Setup.vi"/>
				<Item Name="Eurotherm - Setup.vi" Type="VI" URL="../Eurotherm - System Functions/Eurotherm - Setup.vi"/>
				<Item Name="Eurotherm - Simple Front Panel Control.vi" Type="VI" URL="../Eurotherm - System Functions/Eurotherm - Simple Front Panel Control.vi"/>
			</Item>
			<Item Name="Eurotherm - Project.aliases" Type="Document" URL="../Eurotherm - Project.aliases"/>
			<Item Name="Eurotherm - Project.lvlps" Type="Document" URL="../Eurotherm - Project.lvlps"/>
			<Item Name="EurothermSimple2Var.lvlib" Type="Library" URL="../EurothermSimple2Var.lvlib"/>
		</Item>
		<Item Name="Support Documents" Type="Folder">
			<Item Name="Eurotherm Setup.ini" Type="Document" URL="../Eurotherm Setup.ini"/>
			<Item Name="Event Log 2012-07-19 11-00.log" Type="Document" URL="../../../../../../../../../../../../Data/Export Only/logs/LabVIEW/Event Log/2012-07/2012-07-19/Event Log 2012-07-19 11-00.log"/>
			<Item Name="Notes.txt" Type="Document" URL="../../../../Support Files/Notes.txt"/>
		</Item>
		<Item Name="Demo Instructions.txt" Type="Document" URL="../../../../Support Files/Demo Instructions.txt"/>
		<Item Name="EuroS2.lvlib" Type="Library" URL="../EuroS2.lvlib"/>
		<Item Name="Dependencies" Type="Dependencies">
			<Item Name="vi.lib" Type="Folder">
				<Item Name="Application Directory.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/file.llb/Application Directory.vi"/>
				<Item Name="BuildHelpPath.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/BuildHelpPath.vi"/>
				<Item Name="Check Special Tags.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Check Special Tags.vi"/>
				<Item Name="Clear Errors.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Clear Errors.vi"/>
				<Item Name="Close File+.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/file.llb/Close File+.vi"/>
				<Item Name="compatOverwrite.vi" Type="VI" URL="/&lt;vilib&gt;/_oldvers/_oldvers.llb/compatOverwrite.vi"/>
				<Item Name="compatReadText.vi" Type="VI" URL="/&lt;vilib&gt;/_oldvers/_oldvers.llb/compatReadText.vi"/>
				<Item Name="Convert property node font to graphics font.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Convert property node font to graphics font.vi"/>
				<Item Name="Details Display Dialog.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Details Display Dialog.vi"/>
				<Item Name="DialogType.ctl" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/DialogType.ctl"/>
				<Item Name="DialogTypeEnum.ctl" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/DialogTypeEnum.ctl"/>
				<Item Name="DU64_U32AddWithOverflow.vi" Type="VI" URL="/&lt;vilib&gt;/Waveform/TSOps.llb/DU64_U32AddWithOverflow.vi"/>
				<Item Name="DU64_U32SubtractWithBorrow.vi" Type="VI" URL="/&lt;vilib&gt;/Waveform/TSOps.llb/DU64_U32SubtractWithBorrow.vi"/>
				<Item Name="Dynamic To Waveform Array.vi" Type="VI" URL="/&lt;vilib&gt;/express/express shared/transition.llb/Dynamic To Waveform Array.vi"/>
				<Item Name="Error Cluster From Error Code.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Error Cluster From Error Code.vi"/>
				<Item Name="Error Code Database.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Error Code Database.vi"/>
				<Item Name="ErrWarn.ctl" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/ErrWarn.ctl"/>
				<Item Name="eventvkey.ctl" Type="VI" URL="/&lt;vilib&gt;/event_ctls.llb/eventvkey.ctl"/>
				<Item Name="ex_CorrectErrorChain.vi" Type="VI" URL="/&lt;vilib&gt;/express/express shared/ex_CorrectErrorChain.vi"/>
				<Item Name="ex_GenAddAttribs.vi" Type="VI" URL="/&lt;vilib&gt;/express/express input/SimulateSignalBlock.llb/ex_GenAddAttribs.vi"/>
				<Item Name="ex_SetAllExpressAttribs.vi" Type="VI" URL="/&lt;vilib&gt;/express/express shared/transition.llb/ex_SetAllExpressAttribs.vi"/>
				<Item Name="ex_SetExpAttribsAndT0.vi" Type="VI" URL="/&lt;vilib&gt;/express/express shared/transition.llb/ex_SetExpAttribsAndT0.vi"/>
				<Item Name="ex_WaveformAttribs.ctl" Type="VI" URL="/&lt;vilib&gt;/express/express shared/transition.llb/ex_WaveformAttribs.ctl"/>
				<Item Name="ex_WaveformAttribsPlus.ctl" Type="VI" URL="/&lt;vilib&gt;/express/express shared/transition.llb/ex_WaveformAttribsPlus.ctl"/>
				<Item Name="Find First Error.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Find First Error.vi"/>
				<Item Name="Find Tag.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Find Tag.vi"/>
				<Item Name="Format Message String.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Format Message String.vi"/>
				<Item Name="General Error Handler CORE.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/General Error Handler CORE.vi"/>
				<Item Name="General Error Handler.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/General Error Handler.vi"/>
				<Item Name="Get String Text Bounds.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Get String Text Bounds.vi"/>
				<Item Name="Get Text Rect.vi" Type="VI" URL="/&lt;vilib&gt;/picture/picture.llb/Get Text Rect.vi"/>
				<Item Name="GetHelpDir.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/GetHelpDir.vi"/>
				<Item Name="GetRTHostConnectedProp.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/GetRTHostConnectedProp.vi"/>
				<Item Name="I128 Timestamp.ctl" Type="VI" URL="/&lt;vilib&gt;/Waveform/TSOps.llb/I128 Timestamp.ctl"/>
				<Item Name="Longest Line Length in Pixels.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Longest Line Length in Pixels.vi"/>
				<Item Name="LVBoundsTypeDef.ctl" Type="VI" URL="/&lt;vilib&gt;/Utility/miscctls.llb/LVBoundsTypeDef.ctl"/>
				<Item Name="Nearest Freq in Int Cycles.vi" Type="VI" URL="/&lt;vilib&gt;/express/express input/SimulateSignalConfig.llb/Nearest Freq in Int Cycles.vi"/>
				<Item Name="Nearest Frequency for Block.vi" Type="VI" URL="/&lt;vilib&gt;/express/express input/SimulateSignalConfig.llb/Nearest Frequency for Block.vi"/>
				<Item Name="NI_AALBase.lvlib" Type="Library" URL="/&lt;vilib&gt;/Analysis/NI_AALBase.lvlib"/>
				<Item Name="NI_AALPro.lvlib" Type="Library" URL="/&lt;vilib&gt;/Analysis/NI_AALPro.lvlib"/>
				<Item Name="NI_FileType.lvlib" Type="Library" URL="/&lt;vilib&gt;/Utility/lvfile.llb/NI_FileType.lvlib"/>
				<Item Name="NI_MABase.lvlib" Type="Library" URL="/&lt;vilib&gt;/measure/NI_MABase.lvlib"/>
				<Item Name="Not Found Dialog.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Not Found Dialog.vi"/>
				<Item Name="Open File+.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/file.llb/Open File+.vi"/>
				<Item Name="Read File+ (string).vi" Type="VI" URL="/&lt;vilib&gt;/Utility/file.llb/Read File+ (string).vi"/>
				<Item Name="Read From Spreadsheet File (DBL).vi" Type="VI" URL="/&lt;vilib&gt;/Utility/file.llb/Read From Spreadsheet File (DBL).vi"/>
				<Item Name="Read Lines From File.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/file.llb/Read Lines From File.vi"/>
				<Item Name="Search and Replace Pattern.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Search and Replace Pattern.vi"/>
				<Item Name="Set Bold Text.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Set Bold Text.vi"/>
				<Item Name="Set String Value.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Set String Value.vi"/>
				<Item Name="Simple Error Handler.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Simple Error Handler.vi"/>
				<Item Name="sub2ShouldUseDefSigName.vi" Type="VI" URL="/&lt;vilib&gt;/express/express input/SimulateSignalBlock.llb/sub2ShouldUseDefSigName.vi"/>
				<Item Name="subGetSignalName.vi" Type="VI" URL="/&lt;vilib&gt;/express/express input/SimulateSignalBlock.llb/subGetSignalName.vi"/>
				<Item Name="subInternalTiming.vi" Type="VI" URL="/&lt;vilib&gt;/express/express input/SimulateSignalBlock.llb/subInternalTiming.vi"/>
				<Item Name="subShouldUseDefSigName.vi" Type="VI" URL="/&lt;vilib&gt;/express/express input/SimulateSignalBlock.llb/subShouldUseDefSigName.vi"/>
				<Item Name="subSigGeneratorBlock.vi" Type="VI" URL="/&lt;vilib&gt;/express/express input/SimulateSignalBlock.llb/subSigGeneratorBlock.vi"/>
				<Item Name="TagReturnType.ctl" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/TagReturnType.ctl"/>
				<Item Name="Three Button Dialog CORE.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Three Button Dialog CORE.vi"/>
				<Item Name="Three Button Dialog.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Three Button Dialog.vi"/>
				<Item Name="Timestamp Add.vi" Type="VI" URL="/&lt;vilib&gt;/Waveform/TSOps.llb/Timestamp Add.vi"/>
				<Item Name="Timestamp Subtract.vi" Type="VI" URL="/&lt;vilib&gt;/Waveform/TSOps.llb/Timestamp Subtract.vi"/>
				<Item Name="Trim Whitespace.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Trim Whitespace.vi"/>
				<Item Name="Waveform Array To Dynamic.vi" Type="VI" URL="/&lt;vilib&gt;/express/express shared/transition.llb/Waveform Array To Dynamic.vi"/>
				<Item Name="whitespace.ctl" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/whitespace.ctl"/>
				<Item Name="Write Spreadsheet String.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/file.llb/Write Spreadsheet String.vi"/>
				<Item Name="Write To Spreadsheet File (DBL).vi" Type="VI" URL="/&lt;vilib&gt;/Utility/file.llb/Write To Spreadsheet File (DBL).vi"/>
			</Item>
			<Item Name="lvanlys.dll" Type="Document" URL="../../../../../../../../../../../../Program Files (x86)/National Instruments/LabVIEW 2011/resource/lvanlys.dll"/>
		</Item>
		<Item Name="Build Specifications" Type="Build">
			<Item Name="My Application" Type="EXE">
				<Property Name="App_copyErrors" Type="Bool">true</Property>
				<Property Name="App_INI_aliasGUID" Type="Str">{C8BBA9C7-6A4F-4300-BCB1-E449081FC389}</Property>
				<Property Name="App_INI_GUID" Type="Str">{64858109-4213-46BE-89FA-ACD74E430502}</Property>
				<Property Name="App_useFFRTE" Type="Bool">true</Property>
				<Property Name="Bld_buildCacheID" Type="Str">{F3B01873-B767-41EE-9AC8-807A58E184DB}</Property>
				<Property Name="Bld_buildSpecName" Type="Str">My Application</Property>
				<Property Name="Bld_excludeLibraryItems" Type="Bool">true</Property>
				<Property Name="Bld_excludePolymorphicVIs" Type="Bool">true</Property>
				<Property Name="Bld_localDestDir" Type="Path">/C/Users/kvlb23/Documents/EPICS Pilot/EPICS and LV/Demo VIs/builds/Eurotherm Application</Property>
				<Property Name="Bld_modifyLibraryFile" Type="Bool">true</Property>
				<Property Name="Bld_previewCacheID" Type="Str">{B5500A7B-A643-49E8-9680-0369C8BB6244}</Property>
				<Property Name="Destination[0].destName" Type="Str">Application.exe</Property>
				<Property Name="Destination[0].path" Type="Path">/C/Users/kvlb23/Documents/EPICS Pilot/EPICS and LV/Demo VIs/builds/Eurotherm Application/Application.exe</Property>
				<Property Name="Destination[0].path.type" Type="Str">&lt;none&gt;</Property>
				<Property Name="Destination[0].preserveHierarchy" Type="Bool">true</Property>
				<Property Name="Destination[0].type" Type="Str">App</Property>
				<Property Name="Destination[1].destName" Type="Str">Support Directory</Property>
				<Property Name="Destination[1].path" Type="Path">/C/Users/kvlb23/Documents/EPICS Pilot/EPICS and LV/Demo VIs/builds/Eurotherm Application/data</Property>
				<Property Name="Destination[1].path.type" Type="Str">&lt;none&gt;</Property>
				<Property Name="DestinationCount" Type="Int">2</Property>
				<Property Name="Exe_Vardep[0].LibDeploy" Type="Bool">true</Property>
				<Property Name="Exe_Vardep[0].LibItemID" Type="Ref">/My Computer/Eurotherm/EurothermSimple2Var.lvlib</Property>
				<Property Name="Exe_Vardep[1].LibDeploy" Type="Bool">true</Property>
				<Property Name="Exe_Vardep[1].LibItemID" Type="Ref">/My Computer/EuroS2.lvlib</Property>
				<Property Name="Exe_VardepDeployAtStartup" Type="Bool">true</Property>
				<Property Name="Exe_VardepHideDeployDlg" Type="Bool">true</Property>
				<Property Name="Exe_VardepLibItemCount" Type="Int">2</Property>
				<Property Name="Exe_VardepUndeployOnExit" Type="Bool">true</Property>
				<Property Name="Source[0].itemID" Type="Str">{B3758FDA-0310-4198-B5A6-058D07EC0DF2}</Property>
				<Property Name="Source[0].type" Type="Str">Container</Property>
				<Property Name="Source[1].destinationIndex" Type="Int">0</Property>
				<Property Name="Source[1].itemID" Type="Ref">/My Computer/DEMO/Eurotherm - Front Panel - Simple x 2.vi</Property>
				<Property Name="Source[1].sourceInclusion" Type="Str">TopLevel</Property>
				<Property Name="Source[1].type" Type="Str">VI</Property>
				<Property Name="Source[2].destinationIndex" Type="Int">0</Property>
				<Property Name="Source[2].itemID" Type="Ref">/My Computer/DEMO/Eurotherm - Setup Dialog.vi</Property>
				<Property Name="Source[2].sourceInclusion" Type="Str">TopLevel</Property>
				<Property Name="Source[2].type" Type="Str">VI</Property>
				<Property Name="Source[3].destinationIndex" Type="Int">0</Property>
				<Property Name="Source[3].itemID" Type="Ref">/My Computer/DEMO/Eurotherm - VISA Serial - Command.vi</Property>
				<Property Name="Source[3].sourceInclusion" Type="Str">Include</Property>
				<Property Name="Source[3].type" Type="Str">VI</Property>
				<Property Name="SourceCount" Type="Int">4</Property>
				<Property Name="TgtF_fileDescription" Type="Str">My Application</Property>
				<Property Name="TgtF_fileVersion.major" Type="Int">1</Property>
				<Property Name="TgtF_internalName" Type="Str">My Application</Property>
				<Property Name="TgtF_legalCopyright" Type="Str">Copyright © 2012 </Property>
				<Property Name="TgtF_productName" Type="Str">My Application</Property>
				<Property Name="TgtF_targetfileGUID" Type="Str">{229D391B-75CB-473A-9737-9FF9F4F5C62A}</Property>
				<Property Name="TgtF_targetfileName" Type="Str">Application.exe</Property>
			</Item>
		</Item>
	</Item>
</Project>
