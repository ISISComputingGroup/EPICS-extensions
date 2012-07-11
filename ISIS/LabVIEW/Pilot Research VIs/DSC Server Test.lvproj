<?xml version='1.0' encoding='UTF-8'?>
<Project Type="Project" LVVersion="11008008">
	<Property Name="varPersistentID:{0CFAB9B1-D0F6-49E6-AC75-50DBF7E4A297}" Type="Ref">/My Computer/DSCVars.lvlib/SAM</Property>
	<Property Name="varPersistentID:{0E45C60E-4F8E-49B4-8DD6-F54B40094B78}" Type="Ref">/My Computer/DSCServPro.lvlib/RUPERT</Property>
	<Property Name="varPersistentID:{2DAFE8F0-594F-4453-9C96-BCB1039FCE81}" Type="Ref">/My Computer/DSCVars.lvlib/FRED</Property>
	<Property Name="varPersistentID:{5B2579D3-851E-47D4-A724-AD182E5ADF8F}" Type="Ref">/My Computer/DSCVars.lvlib/RUPERT</Property>
	<Property Name="varPersistentID:{6ED25762-26AB-404F-8156-0F0F788953BF}" Type="Ref">/My Computer/DSCVars.lvlib/GERRY</Property>
	<Property Name="varPersistentID:{9525038E-763B-464D-ADD7-48919AAFB7FE}" Type="Ref">/My Computer/DSCServPro.lvlib/FRED</Property>
	<Property Name="varPersistentID:{998F161F-EFB3-47FB-9E0B-BE3CA50DE057}" Type="Ref">/My Computer/DSCServPro.lvlib/GERRY</Property>
	<Property Name="varPersistentID:{C45113DE-5206-4482-91E1-408E12D097DE}" Type="Ref">/My Computer/DSCServPro.lvlib/SAM</Property>
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
		<Item Name="DSCServPro.lvlib" Type="Library" URL="../DSCServPro.lvlib"/>
		<Item Name="DSCServVI.vi" Type="VI" URL="../DSCServVI.vi"/>
		<Item Name="DSCVars.lvlib" Type="Library" URL="../DSCVars.lvlib"/>
		<Item Name="Dependencies" Type="Dependencies"/>
		<Item Name="Build Specifications" Type="Build"/>
	</Item>
</Project>
