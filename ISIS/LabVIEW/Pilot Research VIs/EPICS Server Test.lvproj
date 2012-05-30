<?xml version='1.0' encoding='UTF-8'?>
<Project Type="Project" LVVersion="11008008">
	<Property Name="NI.LV.ExampleFinder" Type="Str">&lt;?xml version="1.0" encoding="UTF-8"?&gt;
&lt;ExampleProgram&gt;
&lt;Title&gt;
	&lt;Text Locale="US"&gt;EPICS Server API.lvproj&lt;/Text&gt;
&lt;/Title&gt;
&lt;Keywords&gt;
	&lt;Item&gt;EPICS&lt;/Item&gt;
	&lt;Item&gt;server&lt;/Item&gt;
&lt;/Keywords&gt;
&lt;Navigation&gt;
	&lt;Item&gt;1468&lt;/Item&gt;
&lt;/Navigation&gt;
&lt;FileType&gt;LV Project&lt;/FileType&gt;
&lt;Metadata&gt;
&lt;Item Name="RTSupport"&gt;LV Project RT&lt;/Item&gt;
&lt;/Metadata&gt;
&lt;ProgrammingLanguages&gt;
&lt;Item&gt;LabVIEW&lt;/Item&gt;
&lt;/ProgrammingLanguages&gt;
&lt;RequiredSoftware&gt;
&lt;NiSoftware MinVersion="10.0"&gt;LabVIEW&lt;/NiSoftware&gt; 
&lt;/RequiredSoftware&gt;
&lt;/ExampleProgram&gt;</Property>
	<Property Name="NI.Project.Description" Type="Str">This LabVIEW project demonstrates how to programmatically configure an EPICS server IO server. The project contains a host VI, Configure EPICS Server - Host.vi, to show you how to configure an EPICS server on the localhost or remote real-time (RT) target, and an RT VI, Configure EPICS Server - RT.vi, to show you how to configure an EPICS server on the RT target locally.

INSTRUCTIONS:
1. In the project, set the IP address of your RT target.
2. Run Configure EPICS Server - Host.vi.
3. Run Configure EPICS Server - RT.vi.</Property>
	<Property Name="varPersistentID:{ABBCE39A-7A97-4C60-BEC8-FF235629988F}" Type="Ref">/My Computer/HostSource.lvlib/VarString</Property>
	<Property Name="varPersistentID:{B30880EF-CD97-4564-9E33-E745CEC846ED}" Type="Ref">/My Computer/HostSource.lvlib/VarBool</Property>
	<Property Name="varPersistentID:{D316163C-0A71-43BB-86AE-F316C8046245}" Type="Ref">/My Computer/HostSource.lvlib/VarDouble</Property>
	<Property Name="varPersistentID:{DF017F86-6A2C-48B2-883B-75FE0AC76888}" Type="Ref">/My Computer/HostSource.lvlib/VarDoubleArray</Property>
	<Item Name="My Computer" Type="Localhost">
		<Property Name="alias.name" Type="Str">My Computer</Property>
		<Property Name="alias.value" Type="Str">127.0.0.1</Property>
		<Property Name="CCSymbols" Type="Str">OS_hidden,Win;CPU_hidden,x86;OS,Win;CPU,x86;</Property>
		<Property Name="deployState" Type="Int">2</Property>
		<Property Name="IOScan.Faults" Type="Str"></Property>
		<Property Name="IOScan.NetVarPeriod" Type="UInt">100</Property>
		<Property Name="IOScan.NetWatchdogEnabled" Type="Bool">false</Property>
		<Property Name="IOScan.Period" Type="UInt">10000</Property>
		<Property Name="IOScan.PowerupMode" Type="UInt">0</Property>
		<Property Name="IOScan.Priority" Type="UInt">9</Property>
		<Property Name="IOScan.ReportModeConflict" Type="Bool">true</Property>
		<Property Name="IOScan.StartEngineOnDeploy" Type="Bool">false</Property>
		<Property Name="NI.SortType" Type="Int">3</Property>
		<Property Name="specify.custom.address" Type="Bool">false</Property>
		<Item Name="internal" Type="Folder">
			<Item Name="Get EPICS server attributes.vi" Type="VI" URL="../Get EPICS server attributes.vi"/>
			<Item Name="Generate EPICS server URL.vi" Type="VI" URL="../Generate EPICS server URL.vi"/>
		</Item>
		<Item Name="HostSource.lvlib" Type="Library" URL="../HostSource.lvlib"/>
		<Item Name="Configure EPICS Server - Host.vi" Type="VI" URL="../Configure EPICS Server - Host.vi"/>
		<Item Name="Dependencies" Type="Dependencies">
			<Item Name="vi.lib" Type="Folder">
				<Item Name="NI_EPICS_Server.lvlib" Type="Library" URL="/&lt;vilib&gt;/variable/tagger/epicsserver/API/NI_EPICS_Server.lvlib"/>
				<Item Name="EPICSServer.lvclass" Type="LVClass" URL="/&lt;vilib&gt;/variable/tagger/epicsserver/API/internal/EPICSServerClass/EPICSServer.lvclass"/>
				<Item Name="epics_attr.ctl" Type="VI" URL="/&lt;vilib&gt;/variable/tagger/epicsserver/epics_attr.ctl"/>
				<Item Name="Error Cluster From Error Code.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Error Cluster From Error Code.vi"/>
				<Item Name="Trim Whitespace.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Trim Whitespace.vi"/>
				<Item Name="whitespace.ctl" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/whitespace.ctl"/>
				<Item Name="Clear Errors.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Clear Errors.vi"/>
				<Item Name="GenNetURL.vi" Type="VI" URL="/&lt;vilib&gt;/variable/tagger/epicsserver/API/internal/GenNetURL.vi"/>
				<Item Name="Search and Replace Pattern.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Search and Replace Pattern.vi"/>
				<Item Name="ResolveLogosURLMachineAlias.vi" Type="VI" URL="/&lt;vilib&gt;/variable/ResolveLogosURLMachineAlias.vi"/>
				<Item Name="Resolve Machine Alias.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/Resolve Machine Alias.vi"/>
				<Item Name="epics_flattenAttr.vi" Type="VI" URL="/&lt;vilib&gt;/variable/tagger/epicsserver/epics_flattenAttr.vi"/>
				<Item Name="GetDT.vi" Type="VI" URL="/&lt;vilib&gt;/variable/tagger/epicsserver/API/internal/GetDT.vi"/>
				<Item Name="VariantToDTStr.vi" Type="VI" URL="/&lt;vilib&gt;/variable/tagger/epicsserver/API/internal/VariantToDTStr.vi"/>
				<Item Name="NetURLToDynamicURL.vi" Type="VI" URL="/&lt;vilib&gt;/variable/tagger/epicsserver/API/internal/NetURLToDynamicURL.vi"/>
				<Item Name="epics_genRecordName.vi" Type="VI" URL="/&lt;vilib&gt;/variable/tagger/epicsserver/epics_genRecordName.vi"/>
				<Item Name="DTStr2Enum.vi" Type="VI" URL="/&lt;vilib&gt;/variable/tagger/epicsserver/API/internal/DTStr2Enum.vi"/>
				<Item Name="List PVs (By Ref).vi" Type="VI" URL="/&lt;vilib&gt;/variable/tagger/epicsserver/API/internal/List PVs (By Ref).vi"/>
				<Item Name="List PVs (By URL).vi" Type="VI" URL="/&lt;vilib&gt;/variable/tagger/epicsserver/API/internal/List PVs (By URL).vi"/>
				<Item Name="IsEPICSServer.vi" Type="VI" URL="/&lt;vilib&gt;/variable/tagger/epicsserver/API/internal/IsEPICSServer.vi"/>
				<Item Name="ni_tagger_lv_GetMonadClass.vi" Type="VI" URL="/&lt;vilib&gt;/variable/tagger/ni_tagger_lv_GetMonadClass.vi"/>
				<Item Name="ni_tagger_lv_GetMonadAttributes.vi" Type="VI" URL="/&lt;vilib&gt;/variable/tagger/ni_tagger_lv_GetMonadAttributes.vi"/>
				<Item Name="epics_unflattenAttr.vi" Type="VI" URL="/&lt;vilib&gt;/variable/tagger/epicsserver/epics_unflattenAttr.vi"/>
				<Item Name="ni_tagger_lv_GetProcessList.vi" Type="VI" URL="/&lt;vilib&gt;/variable/tagger/ni_tagger_lv_GetProcessList.vi"/>
				<Item Name="ni_tagger_lv_NewProcess.vi" Type="VI" URL="/&lt;vilib&gt;/variable/tagger/ni_tagger_lv_NewProcess.vi"/>
				<Item Name="ni_tagger_lv_NewMonad.vi" Type="VI" URL="/&lt;vilib&gt;/variable/tagger/ni_tagger_lv_NewMonad.vi"/>
				<Item Name="ni_tagger_lv_NewFolder.vi" Type="VI" URL="/&lt;vilib&gt;/variable/tagger/ni_tagger_lv_NewFolder.vi"/>
				<Item Name="Simple Error Handler.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Simple Error Handler.vi"/>
				<Item Name="DialogType.ctl" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/DialogType.ctl"/>
				<Item Name="General Error Handler.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/General Error Handler.vi"/>
				<Item Name="DialogTypeEnum.ctl" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/DialogTypeEnum.ctl"/>
				<Item Name="General Error Handler CORE.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/General Error Handler CORE.vi"/>
				<Item Name="Check Special Tags.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Check Special Tags.vi"/>
				<Item Name="TagReturnType.ctl" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/TagReturnType.ctl"/>
				<Item Name="Set String Value.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Set String Value.vi"/>
				<Item Name="GetRTHostConnectedProp.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/GetRTHostConnectedProp.vi"/>
				<Item Name="Error Code Database.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Error Code Database.vi"/>
				<Item Name="Format Message String.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Format Message String.vi"/>
				<Item Name="Find Tag.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Find Tag.vi"/>
				<Item Name="Set Bold Text.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Set Bold Text.vi"/>
				<Item Name="Details Display Dialog.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Details Display Dialog.vi"/>
				<Item Name="ErrWarn.ctl" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/ErrWarn.ctl"/>
				<Item Name="eventvkey.ctl" Type="VI" URL="/&lt;vilib&gt;/event_ctls.llb/eventvkey.ctl"/>
				<Item Name="Not Found Dialog.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Not Found Dialog.vi"/>
				<Item Name="Three Button Dialog.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Three Button Dialog.vi"/>
				<Item Name="Three Button Dialog CORE.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Three Button Dialog CORE.vi"/>
				<Item Name="Longest Line Length in Pixels.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Longest Line Length in Pixels.vi"/>
				<Item Name="Convert property node font to graphics font.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Convert property node font to graphics font.vi"/>
				<Item Name="Get Text Rect.vi" Type="VI" URL="/&lt;vilib&gt;/picture/picture.llb/Get Text Rect.vi"/>
				<Item Name="Get String Text Bounds.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Get String Text Bounds.vi"/>
				<Item Name="LVBoundsTypeDef.ctl" Type="VI" URL="/&lt;vilib&gt;/Utility/miscctls.llb/LVBoundsTypeDef.ctl"/>
				<Item Name="BuildHelpPath.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/BuildHelpPath.vi"/>
				<Item Name="GetHelpDir.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/GetHelpDir.vi"/>
				<Item Name="Undeploy EPICS Server (By Ref).vi" Type="VI" URL="/&lt;vilib&gt;/variable/tagger/epicsserver/API/internal/Undeploy EPICS Server (By Ref).vi"/>
				<Item Name="Undeploy EPICS Server (By URL).vi" Type="VI" URL="/&lt;vilib&gt;/variable/tagger/epicsserver/API/internal/Undeploy EPICS Server (By URL).vi"/>
				<Item Name="ni_tagger_lv_DeleteNode.vi" Type="VI" URL="/&lt;vilib&gt;/variable/tagger/ni_tagger_lv_DeleteNode.vi"/>
				<Item Name="CreateURLFromComponents.vi" Type="VI" URL="/&lt;vilib&gt;/variable/tagger/common/CreateURLFromComponents.vi"/>
				<Item Name="ReplaceAliasIfExists.vi" Type="VI" URL="/&lt;vilib&gt;/variable/tagger/common/ReplaceAliasIfExists.vi"/>
				<Item Name="StrToHostname.vi" Type="VI" URL="/&lt;vilib&gt;/variable/tagger/common/StrToHostname.vi"/>
				<Item Name="ResolveURL.vi" Type="VI" URL="/&lt;vilib&gt;/variable/tagger/common/ResolveURL.vi"/>
				<Item Name="CheckPVName.vi" Type="VI" URL="/&lt;vilib&gt;/variable/tagger/epicsserver/API/internal/CheckPVName.vi"/>
				<Item Name="epics_checkPVName.vi" Type="VI" URL="/&lt;vilib&gt;/variable/tagger/epicsserver/epics_checkPVName.vi"/>
				<Item Name="GetArraySize.vi" Type="VI" URL="/&lt;vilib&gt;/variable/tagger/epicsserver/API/internal/GetArraySize.vi"/>
				<Item Name="data type.ctl" Type="VI" URL="/&lt;vilib&gt;/lvdsc/process/internal/data type.ctl"/>
			</Item>
			<Item Name="nitaglv.dll" Type="Document" URL="nitaglv.dll">
				<Property Name="NI.PreserveRelativePath" Type="Bool">true</Property>
			</Item>
			<Item Name="ni.var.rc" Type="Document" URL="../../../../../../../Program Files (x86)/National Instruments/LabVIEW 2011/resource/objmgr/ni.var.rc"/>
		</Item>
		<Item Name="Build Specifications" Type="Build"/>
	</Item>
	<Item Name="RT Target" Type="RT Generic">
		<Property Name="alias.name" Type="Str">RT Target</Property>
		<Property Name="alias.value" Type="Str">0.0.0.0</Property>
		<Property Name="CCSymbols" Type="Str">TARGET_TYPE,RT;</Property>
		<Property Name="host.ResponsivenessCheckEnabled" Type="Bool">true</Property>
		<Property Name="host.ResponsivenessCheckPingDelay" Type="UInt">5000</Property>
		<Property Name="host.ResponsivenessCheckPingTimeout" Type="UInt">1000</Property>
		<Property Name="host.TargetCPUID" Type="UInt">3</Property>
		<Property Name="host.TargetOSID" Type="UInt">15</Property>
		<Property Name="target.cleanupVisa" Type="Bool">false</Property>
		<Property Name="target.FPProtocolGlobals_ControlTimeLimit" Type="Int">300</Property>
		<Property Name="target.getDefault-&gt;WebServer.Port" Type="Int">80</Property>
		<Property Name="target.getDefault-&gt;WebServer.Timeout" Type="Int">60</Property>
		<Property Name="target.IOScan.Faults" Type="Str"></Property>
		<Property Name="target.IOScan.NetVarPeriod" Type="UInt">100</Property>
		<Property Name="target.IOScan.Period" Type="UInt">10000</Property>
		<Property Name="target.IOScan.PowerupMode" Type="UInt">0</Property>
		<Property Name="target.IOScan.Priority" Type="UInt">0</Property>
		<Property Name="target.IOScan.ReportModeConflict" Type="Bool">false</Property>
		<Property Name="target.IsRemotePanelSupported" Type="Bool">true</Property>
		<Property Name="target.RTCPULoadMonitoringEnabled" Type="Bool">true</Property>
		<Property Name="target.RTTarget.ApplicationPath" Type="Path">/c/ni-rt/startup/startup.rtexe</Property>
		<Property Name="target.RTTarget.EnableFileSharing" Type="Bool">true</Property>
		<Property Name="target.RTTarget.IPAccess" Type="Str">+*</Property>
		<Property Name="target.RTTarget.LaunchAppAtBoot" Type="Bool">false</Property>
		<Property Name="target.RTTarget.VIPath" Type="Path">/c/ni-rt/startup</Property>
		<Property Name="target.server.app.propertiesEnabled" Type="Bool">false</Property>
		<Property Name="target.server.control.propertiesEnabled" Type="Bool">false</Property>
		<Property Name="target.server.tcp.access" Type="Str">+*</Property>
		<Property Name="target.server.tcp.enabled" Type="Bool">false</Property>
		<Property Name="target.server.tcp.paranoid" Type="Bool">true</Property>
		<Property Name="target.server.tcp.port" Type="Int">3363</Property>
		<Property Name="target.server.tcp.serviceName" Type="Str"></Property>
		<Property Name="target.server.tcp.serviceName.default" Type="Str">Main Application Instance/VI Server</Property>
		<Property Name="target.server.vi.access" Type="Str">+*</Property>
		<Property Name="target.server.vi.callsEnabled" Type="Bool">false</Property>
		<Property Name="target.server.vi.propertiesEnabled" Type="Bool">false</Property>
		<Property Name="target.WebServer.Config" Type="Str"># Web server configuration file.
# Generated by LabVIEW 9.0
# 1/21/2010 5:55:00 PM

#
# Global Directives
#
LogLevel 2
TypesConfig $LVSERVER_ROOT/mime.types
ThreadLimit 10
LoadModulePath "$LVSERVER_ROOT/modules" "$LVSERVER_ROOT/LVModules" "$LVSERVER_ROOT/.."
LoadModule LVAuth lvauthmodule
LoadModule LVRFP lvrfpmodule
LoadModule esp libespModule
LoadModule dir libdirModule
LoadModule copy libcopyModule
LoadModule LvExec ws_runtime
Listen 80

#
# Directives that apply to the default server
#
ServerName LabVIEW
DocumentRoot "/ni-rt/system/www"
Timeout 60
AddHandler LVAuthHandler
AddHandler LVRFPHandler
AddHandler LvExec
AddHandler espHandler
AddHandler dirHandler
AddHandler espHandler .esp
AddHandler copyHandler
DirectoryIndex index.htm
KeepAlive on
KeepAliveTimeout 60
</Property>
		<Property Name="target.WebServer.Enabled" Type="Bool">false</Property>
		<Property Name="target.WebServer.LogEnabled" Type="Bool">false</Property>
		<Property Name="target.WebServer.LogPath" Type="Path">/c/ni-rt/system/www/www.log</Property>
		<Property Name="target.WebServer.Port" Type="Int">80</Property>
		<Property Name="target.WebServer.RootPath" Type="Path">/c/ni-rt/system/www</Property>
		<Property Name="target.WebServer.TcpAccess" Type="Str">c+*</Property>
		<Property Name="target.WebServer.Timeout" Type="Int">60</Property>
		<Property Name="target.WebServer.ViAccess" Type="Str">+*</Property>
		<Property Name="target.webservices.SecurityAPIKey" Type="Str">PqVr/ifkAQh+lVrdPIykXlFvg12GhhQFR8H9cUhphgg=:pTe9HRlQuMfJxAG6QCGq7UvoUpJzAzWGKy5SbZ+roSU=</Property>
		<Property Name="target.webservices.ValidTimestampWindow" Type="UInt">15</Property>
		<Item Name="Configure EPICS Server - RT.vi" Type="VI" URL="../../../../../../../Program Files (x86)/National Instruments/LabVIEW 2011/examples/comm/EPICS/EPICS Server API/Configure EPICS Server - RT.vi"/>
		<Item Name="RTSource.lvlib" Type="Library" URL="../../../../../../../Program Files (x86)/National Instruments/LabVIEW 2011/examples/comm/EPICS/EPICS Server API/RTSource.lvlib"/>
		<Item Name="Dependencies" Type="Dependencies">
			<Item Name="vi.lib" Type="Folder">
				<Item Name="NI_EPICS_Server.lvlib" Type="Library" URL="/&lt;vilib&gt;/variable/tagger/epicsserver/API/NI_EPICS_Server.lvlib"/>
				<Item Name="EPICSServer.lvclass" Type="LVClass" URL="/&lt;vilib&gt;/variable/tagger/epicsserver/API/internal/EPICSServerClass/EPICSServer.lvclass"/>
				<Item Name="epics_attr.ctl" Type="VI" URL="/&lt;vilib&gt;/variable/tagger/epicsserver/epics_attr.ctl"/>
				<Item Name="CreateURLFromComponents.vi" Type="VI" URL="/&lt;vilib&gt;/variable/tagger/epicsserver/API/internal/CreateURLFromComponents.vi"/>
				<Item Name="GenNetURL.vi" Type="VI" URL="/&lt;vilib&gt;/variable/tagger/epicsserver/API/internal/GenNetURL.vi"/>
				<Item Name="ReplaceAliasIfExists.vi" Type="VI" URL="/&lt;vilib&gt;/variable/tagger/epicsserver/API/internal/ReplaceAliasIfExists.vi"/>
				<Item Name="ResolveLogosURLMachineAlias.vi" Type="VI" URL="/&lt;vilib&gt;/variable/ResolveLogosURLMachineAlias.vi"/>
				<Item Name="Error Cluster From Error Code.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Error Cluster From Error Code.vi"/>
				<Item Name="Trim Whitespace.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Trim Whitespace.vi"/>
				<Item Name="whitespace.ctl" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/whitespace.ctl"/>
				<Item Name="Resolve Machine Alias.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/Resolve Machine Alias.vi"/>
				<Item Name="Clear Errors.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Clear Errors.vi"/>
				<Item Name="Search and Replace Pattern.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Search and Replace Pattern.vi"/>
				<Item Name="StrToHostname.vi" Type="VI" URL="/&lt;vilib&gt;/variable/tagger/epicsserver/API/internal/StrToHostname.vi"/>
				<Item Name="CompNetURL.vi" Type="VI" URL="/&lt;vilib&gt;/variable/tagger/epicsserver/API/internal/CompNetURL.vi"/>
				<Item Name="epics_flattenAttr.vi" Type="VI" URL="/&lt;vilib&gt;/variable/tagger/epicsserver/epics_flattenAttr.vi"/>
				<Item Name="Merge Errors.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Merge Errors.vi"/>
				<Item Name="ni_tagger_lv_GetProcessList.vi" Type="VI" URL="/&lt;vilib&gt;/variable/tagger/ni_tagger_lv_GetProcessList.vi"/>
				<Item Name="ni_tagger_lv_NewProcess.vi" Type="VI" URL="/&lt;vilib&gt;/variable/tagger/ni_tagger_lv_NewProcess.vi"/>
				<Item Name="ni_tagger_lv_NewMonad.vi" Type="VI" URL="/&lt;vilib&gt;/variable/tagger/ni_tagger_lv_NewMonad.vi"/>
				<Item Name="ni_tagger_lv_NewFolder.vi" Type="VI" URL="/&lt;vilib&gt;/variable/tagger/ni_tagger_lv_NewFolder.vi"/>
				<Item Name="GetDT.vi" Type="VI" URL="/&lt;vilib&gt;/variable/tagger/epicsserver/API/internal/GetDT.vi"/>
				<Item Name="VariantToDTStr.vi" Type="VI" URL="/&lt;vilib&gt;/variable/tagger/epicsserver/API/internal/VariantToDTStr.vi"/>
				<Item Name="NetURLToDynamicURL.vi" Type="VI" URL="/&lt;vilib&gt;/variable/tagger/epicsserver/API/internal/NetURLToDynamicURL.vi"/>
				<Item Name="epics_genRecordName.vi" Type="VI" URL="/&lt;vilib&gt;/variable/tagger/epicsserver/epics_genRecordName.vi"/>
				<Item Name="DTStr2Enum.vi" Type="VI" URL="/&lt;vilib&gt;/variable/tagger/epicsserver/API/internal/DTStr2Enum.vi"/>
				<Item Name="List PVs (By Ref).vi" Type="VI" URL="/&lt;vilib&gt;/variable/tagger/epicsserver/API/internal/List PVs (By Ref).vi"/>
				<Item Name="List PVs (By URL).vi" Type="VI" URL="/&lt;vilib&gt;/variable/tagger/epicsserver/API/internal/List PVs (By URL).vi"/>
				<Item Name="ResolveURL.vi" Type="VI" URL="/&lt;vilib&gt;/variable/tagger/epicsserver/API/internal/ResolveURL.vi"/>
				<Item Name="GetURLComponents.vi" Type="VI" URL="/&lt;vilib&gt;/variable/tagger/epicsserver/API/internal/GetURLComponents.vi"/>
				<Item Name="IsEPICSServer.vi" Type="VI" URL="/&lt;vilib&gt;/variable/tagger/epicsserver/API/internal/IsEPICSServer.vi"/>
				<Item Name="ni_tagger_lv_GetMonadClass.vi" Type="VI" URL="/&lt;vilib&gt;/variable/tagger/ni_tagger_lv_GetMonadClass.vi"/>
				<Item Name="ni_tagger_lv_GetMonadAttributes.vi" Type="VI" URL="/&lt;vilib&gt;/variable/tagger/ni_tagger_lv_GetMonadAttributes.vi"/>
				<Item Name="epics_unflattenAttr.vi" Type="VI" URL="/&lt;vilib&gt;/variable/tagger/epicsserver/epics_unflattenAttr.vi"/>
				<Item Name="Simple Error Handler.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Simple Error Handler.vi"/>
				<Item Name="General Error Handler.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/General Error Handler.vi"/>
				<Item Name="General Error Handler CORE.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/General Error Handler CORE.vi"/>
				<Item Name="Error Code Database.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Error Code Database.vi"/>
				<Item Name="Check Special Tags.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Check Special Tags.vi"/>
				<Item Name="TagReturnType.ctl" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/TagReturnType.ctl"/>
				<Item Name="DialogTypeEnum.ctl" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/DialogTypeEnum.ctl"/>
				<Item Name="Not Found Dialog.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Not Found Dialog.vi"/>
				<Item Name="BuildHelpPath.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/BuildHelpPath.vi"/>
				<Item Name="GetHelpDir.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/GetHelpDir.vi"/>
				<Item Name="Three Button Dialog.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Three Button Dialog.vi"/>
				<Item Name="Three Button Dialog CORE.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Three Button Dialog CORE.vi"/>
				<Item Name="Longest Line Length in Pixels.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Longest Line Length in Pixels.vi"/>
				<Item Name="Get Text Rect.vi" Type="VI" URL="/&lt;vilib&gt;/picture/picture.llb/Get Text Rect.vi"/>
				<Item Name="Convert property node font to graphics font.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Convert property node font to graphics font.vi"/>
				<Item Name="Get String Text Bounds.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Get String Text Bounds.vi"/>
				<Item Name="LVBoundsTypeDef.ctl" Type="VI" URL="/&lt;vilib&gt;/Utility/miscctls.llb/LVBoundsTypeDef.ctl"/>
				<Item Name="Format Message String.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Format Message String.vi"/>
				<Item Name="Set Bold Text.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Set Bold Text.vi"/>
				<Item Name="Find Tag.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Find Tag.vi"/>
				<Item Name="Details Display Dialog.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Details Display Dialog.vi"/>
				<Item Name="eventvkey.ctl" Type="VI" URL="/&lt;vilib&gt;/event_ctls.llb/eventvkey.ctl"/>
				<Item Name="ErrWarn.ctl" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/ErrWarn.ctl"/>
				<Item Name="Set String Value.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/Set String Value.vi"/>
				<Item Name="GetRTHostConnectedProp.vi" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/GetRTHostConnectedProp.vi"/>
				<Item Name="DialogType.ctl" Type="VI" URL="/&lt;vilib&gt;/Utility/error.llb/DialogType.ctl"/>
			</Item>
			<Item Name="nitaglv.dll" Type="Document" URL="nitaglv.dll">
				<Property Name="NI.PreserveRelativePath" Type="Bool">true</Property>
			</Item>
			<Item Name="data type.ctl" Type="VI" URL="/&lt;vilib&gt;/lvdsc/process/internal/data type.ctl"/>
		</Item>
		<Item Name="Build Specifications" Type="Build"/>
	</Item>
</Project>
