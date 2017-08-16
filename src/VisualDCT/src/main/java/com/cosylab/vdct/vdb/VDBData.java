package com.cosylab.vdct.vdb;

/**
 * Copyright (c) 2002, Cosylab, Ltd., Control System Laboratory, www.cosylab.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer. 
 * Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation 
 * and/or other materials provided with the distribution. 
 * Neither the name of the Cosylab, Ltd., Control System Laboratory nor the names
 * of its contributors may be used to endorse or promote products derived 
 * from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE 
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, 
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

import java.util.Enumeration;
import java.util.HashMap;
import java.util.Hashtable;
import java.util.Vector;

import com.cosylab.vdct.Console;
import com.cosylab.vdct.Constants;
import com.cosylab.vdct.DataSynchronizer;
import com.cosylab.vdct.db.DBData;
import com.cosylab.vdct.db.DBEntry;
import com.cosylab.vdct.db.DBException;
import com.cosylab.vdct.db.DBFieldData;
import com.cosylab.vdct.db.DBMacro;
import com.cosylab.vdct.db.DBPort;
import com.cosylab.vdct.db.DBRecordData;
import com.cosylab.vdct.db.DBTemplate;
import com.cosylab.vdct.db.DBTemplateInstance;
import com.cosylab.vdct.dbd.DBDConstants;
import com.cosylab.vdct.dbd.DBDData;
import com.cosylab.vdct.dbd.DBDFieldData;
import com.cosylab.vdct.dbd.DBDMenuData;
import com.cosylab.vdct.dbd.DBDRecordData;
import com.cosylab.vdct.events.CommandManager;
import com.cosylab.vdct.events.commands.GetDsManager;
import com.cosylab.vdct.graphics.DrawingSurface;
import com.cosylab.vdct.graphics.DsEventListener;
import com.cosylab.vdct.graphics.objects.Group;
import com.cosylab.vdct.graphics.objects.Macro;
import com.cosylab.vdct.graphics.objects.Port;
import com.cosylab.vdct.undo.UndoManager;

/**
 * This type was created in VisualAge.
 */
public class VDBData implements DsEventListener {
	
	protected static HashMap instances = new HashMap();
	
	private Vector records = null;
	//private Hashtable templates = null;
	private Hashtable templates = new Hashtable();
	private Hashtable templateInstances = null;
	private Vector templateInstancesV = null;

	private Vector structure = null;

/**
 * DBDData constructor comment.
 */
private VDBData() {
	records = new Vector();
	//templates = new Hashtable();
	templateInstances = new Hashtable();
	templateInstancesV = new Vector();

	structure = new Vector();
}
/**
 * This method was created in VisualAge.
 * @param rd
 */
public void addRecord(VDBRecordData rd) {
	if (rd!=null)
//	if (!records.containsKey(rd.name))
//		records.put(rd.name, rd);
	if (!records.contains(rd))
	{
		records.addElement(rd);
		structure.addElement(rd);
	}
}

/**
 * This method was created in VisualAge.
 * @param ed
 */
public void addEntry(DBEntry ed) {
	if (!structure.contains(ed))
		structure.addElement(ed);
}

/**
 * This method was created in VisualAge.
 * @param templ
 */
public void addTemplate(VDBTemplate templ) {
	if (templ != null && templ.getId() != null) {
		if (templates.containsKey(templ.getId())) {
			templates.remove(templ.getId());
		}
		templates.put(templ.getId(), templ);
	}
}

/**
 * This method was created in VisualAge.
     * @param ti */
public void addTemplateInstance(VDBTemplateInstance ti) {
	if (ti!=null)
		if (!templateInstances.contains(ti))
		{
			templateInstancesV.addElement(ti);
			templateInstances.put(ti.getName(), ti);
			structure.addElement(ti);
		}
}

/**
 * This method was created in VisualAge.
 * @return com.cosylab.vdct.vdb.VDBFieldData
 * @param sourceField com.cosylab.vdct.dbd.DBDFieldData
 */
public static VDBFieldData copyVDBFieldData(VDBFieldData sourceField) {

	VDBFieldData vdbField = new VDBFieldData();

	copyVDBFieldData(sourceField, vdbField);

	return vdbField;
}
/**
 * This method was created in VisualAge.
 * @param sourceField com.cosylab.vdct.dbd.DBDFieldData
 * @param targetField com.cosylab.vdct.dbd.DBDFieldData
 */
public static void copyVDBFieldData(VDBFieldData sourceField, VDBFieldData targetField) {
	targetField.setType(sourceField.getType());
	targetField.setName(sourceField.getName());
	targetField.setValueSilently(sourceField.getValue());
	targetField.setInit_value(sourceField.getInit_value());
	targetField.setGui_group(sourceField.getGui_group());
	targetField.setDbdData(sourceField.getDbdData());
	targetField.setVisibility(sourceField.getVisibility());
}
/**
 * This method was created in VisualAge.
 * @return com.cosylab.vdct.vdb.VDBTemplateInstance
 */
public static VDBTemplateInstance copyVDBTemplateInstance(VDBTemplateInstance source) {

	VDBTemplateInstance vdbTemplateInstance = new VDBTemplateInstance(source.getName(), source.getTemplate());

	vdbTemplateInstance.setProperties((Hashtable)source.getProperties().clone(),
								  	  (Vector)source.getPropertiesV().clone());


	return vdbTemplateInstance;
}

/**
 * This method was created in VisualAge.
 * @return com.cosylab.vdct.vdb.VDBRecordData
 * @param source com.cosylab.vdct.vdb.VDBRecordData
 */
public static VDBRecordData copyVDBRecordData(VDBRecordData source) {

	VDBRecordData vdbRecord = new VDBRecordData();
	VDBFieldData sourceField, targetField;

	vdbRecord.setType(source.getType());
	vdbRecord.setName(source.getName());
	//vdbRecord.setDTYP_type(source.getDTYP_type());;

	
	Enumeration e = source.getFieldsV().elements();
	while (e.hasMoreElements()) {
		sourceField = (VDBFieldData)(e.nextElement());
		targetField = copyVDBFieldData(sourceField);
		targetField.setRecord(vdbRecord);
		vdbRecord.addField(targetField);
	}

	if (source.getInfoFields() != null)
	{
		Vector infoFieldsCopy = new Vector();
		e = source.getInfoFields().elements();
		while (e.hasMoreElements()) {
			DBFieldData sfd = (DBFieldData)(e.nextElement());
			DBFieldData tfd = new DBFieldData(sfd.getName(), sfd.getValue());
			infoFieldsCopy.add(tfd);
		}
		vdbRecord.setInfoFields(infoFieldsCopy);
	}
	
	return vdbRecord;
}

/**
 * Creates a copy of the VDBMacro object.
 * 
 * @param source the VDBMacro to be copied
 * @return a copy of the source
 */
public static VDBMacro copyVDBMacro(VDBMacro source) {
    
    VDBMacro vdbMacro = new VDBMacro(source.getTemplate(), source.getName(), source.getDescription());
    vdbMacro.setVisibility(source.getVisibility());
    return vdbMacro;
        
}

public static VDBPort copyVDBPort(VDBPort source) {
    
    VDBPort vdbPort = new VDBPort(source.getTemplate(), source.getName(), source.getTarget(), source.getDescription());
    vdbPort.setVisibility(source.getVisibility());
    return vdbPort;
        
}

/**
 * This method was created in VisualAge.
 * @return com.cosylab.vdct.vdb.VDBData
 * @param dsId
 * @param dbd com.cosylab.vdct.dbd.DBDData
 * @param db com.cosylab.vdct.db.DBData
 */
public static VDBData generateVDBData(Object dsId, DBDData dbd, DBData db) {
	
	if (dbd != null && db != null) {

		// generate itself
		db.getTemplateData().setData(db);
		
		VDBData vdbData = new VDBData();

		vdbData.generateTemplate(dsId, dbd, db.getTemplateData());
		return vdbData;
	}
	else
		return null;
}

/**
 * This method was created in VisualAge.
 * @return com.cosylab.vdct.vdb.VDBData
 * @param dbd com.cosylab.vdct.dbd.DBDData
 * @param db com.cosylab.vdct.db.DBData
 */
private void generateVDBDataInternal(Object dsId, DBDData dbd, DBData db) throws DBException {
	
	if (dbd!=null && db!=null) {

		// extract templates
		extractTemplates(dsId, dbd, db);
		
		// add records, template instances and entries
		Enumeration e = db.getStructure().elements();
		while (e.hasMoreElements())
		{
			Object obj = e.nextElement();
			if (obj instanceof DBRecordData)
			{
				DBRecordData dbRecord = (DBRecordData)obj;
				addRecord(generateVDBRecordData(dsId, dbd, dbRecord));
			}
			else if (obj instanceof DBTemplateInstance)
			{
				DBTemplateInstance dbTemplateInstance = (DBTemplateInstance)obj;
				VDBTemplateInstance vti = generateVDBTemplateInstance(dsId, dbTemplateInstance);
				if (vti!=null)
					addTemplateInstance(vti);
			}
			else if (obj instanceof DBEntry)
			{
				addEntry((DBEntry)obj);	
			}
			
		}

	}
}

/**
 * 
 */
public static void generateRecords(Object dsId, DBDData dbd, DBData db, VDBData vdb) throws DBException
{
	// add records
	DBRecordData dbRecord;
	Enumeration e = db.getRecordsV().elements();
	while (e.hasMoreElements()) {
		dbRecord = (DBRecordData)(e.nextElement());
		vdb.addRecord(generateVDBRecordData(dsId, dbd, dbRecord));
	}
}

public VDBTemplateInstance generateVDBTemplateInstance(Object dsId, DBTemplateInstance dbTemplateInstance)
{
	VDBTemplate t = DataSynchronizer.getInstance().getTemplate(dsId, dbTemplateInstance.getTemplateId());
	
	if (t==null)
	{
		Console.getInstance().println(
			"Template instance "+dbTemplateInstance.getTemplateInstanceId()+" cannot be created since "
				+ dbTemplateInstance.getTemplateInstanceId()
				+ " does not exist - this definition will be ignored.");
		return null;
	}
	VDBTemplateInstance vti = new VDBTemplateInstance(dbTemplateInstance.getTemplateInstanceId(), t);
	vti.setComment(dbTemplateInstance.getComment());	

	vti.setProperties(dbTemplateInstance.getProperties(), dbTemplateInstance.getPropertiesV());
	
	return vti;
}

public static VDBTemplateInstance generateNewVDBTemplateInstance(String name, VDBTemplate t)
{
	VDBTemplateInstance vti = new VDBTemplateInstance(name, t);
	vti.setProperties(new Hashtable(), new Vector());		// empty properties
		
	return vti;
}

/**
 * 
 */
public void extractTemplates(Object dsId, DBDData dbd, DBData db)
{
	Enumeration e;
	DBTemplate dbTemplate;
	e = db.getTemplatesV().elements();
	while (e.hasMoreElements()) {
		dbTemplate = (DBTemplate)(e.nextElement());

		Hashtable templates = VDBData.getInstance(dsId).getTemplates();
		if (templates.containsKey(dbTemplate.getId())) {
			VDBTemplate t = (VDBTemplate)templates.get(dbTemplate.getId());
			Console.getInstance().println("Template with id '"+dbTemplate.getId()+"' ('"+t.getFileName()+"') already exists in repository. Skipping template from file '"+dbTemplate.getFileName()+"'...");
			continue;
		}
		
		VDBData vdbData = new VDBData();
		vdbData.generateTemplate(dsId, dbd, dbTemplate);
	}
}

private void generateTemplate(Object dsId, DBDData dbd, DBTemplate dbTemplate)
{
	Group root = Group.getRoot(dsId);

	// generate vt.group / VDB data
	try
	{
		VDBTemplate vt = new VDBTemplate(dbTemplate.getId(), dbTemplate.getFileName());
	
		vt.setGroup(new Group(null));
		vt.getGroup().setAbsoluteName("");
		vt.getGroup().setLookupTable(new Hashtable());
	
		Group.setRoot(dsId, vt.getGroup());

		vt.setComment(dbTemplate.getComment());	
		vt.setDescription(dbTemplate.getDescription());
		vt.setModificationTime(dbTemplate.getModificationTime());
		vt.setVersion(dbTemplate.getVersion());
		vt.setIoc(dbTemplate.getIoc());
		
		generateVDBDataInternal(dsId, dbd, dbTemplate.getData());
		DrawingSurface.applyVisualData(dsId, false, vt.getGroup(), dbTemplate.getData(), this);
		vt.getGroup().unconditionalValidateSubObjects(false);

		Hashtable ports = new Hashtable();
		Vector portsV = new Vector();
		vt.setPorts(ports);
		vt.setPortsV(portsV);

		Hashtable macros = new Hashtable();
		Vector macrosV = new Vector();
		vt.setMacros(macros);
		vt.setMacrosV(macrosV);

		addPortsAndMacros(dsId, dbTemplate, vt, this);

		VDBData.getInstance(dsId).addTemplate(vt);
	}
	catch (Exception ex)
	{
		Console.getInstance().println();
		Console.getInstance().println("Exception caught while generating '"+dbTemplate.getId()+"' template.");
		Console.getInstance().println(ex);
		Console.getInstance().println();
	}
	finally
	{
		// validate all links
		Group.getRoot(dsId).manageLinks(true);
		Group.getRoot(dsId).updateFields();
		Group.setRoot(dsId, root); 
	}
}

// NOTE adds to root!!!

public static void addPortsAndMacros(Object dsId, DBTemplate dbTemplate, VDBTemplate vt, VDBData vdbData) {
	addPortsAndMacros(dsId, dbTemplate, vt, vdbData, null);
}

public static void addPortsAndMacros(Object dsId, DBTemplate dbTemplate, VDBTemplate vt, VDBData vdbData, HashMap importedList) {
	
	// noop (importing into DB w/o editing template data)
	if (importedList != null && Group.getEditingTemplateData(dsId) == null)
		return;
	
	Hashtable ports = vt.getPorts();
	Vector portsV = vt.getPortsV();
	
	Hashtable macros = vt.getMacros();
	Vector macrosV = vt.getMacrosV();
	
	Enumeration keys = dbTemplate.getPorts().keys();
	while (keys.hasMoreElements())
	{
		Object key = keys.nextElement();
		DBPort port = (DBPort)dbTemplate.getPorts().get(key);
		VDBPort vdbPort = new VDBPort(vt, port);	
		
		// skip
		if (Group.getRoot(dsId).getSubObject(vdbPort.getName())!= null)
		{
			Console.getInstance().println("WARNING: port with name '" + vdbPort.getName() + "' already exists, skipping its redefinition...");
			continue;
		}
		
		// has visual
		if (port.isHasVisual())
		{
			Port visualPort = new Port(vdbPort, Group.getRoot(dsId),
								  port.getX(), port.getY());
			visualPort.setColor(port.getColor());
			visualPort.setMode(port.getMode());
			visualPort.setTextPositionNorth(port.isNamePositionNorth());

			// delegate defaultVisibility
			vdbPort.setVisibility(port.getDefaultVisibility());
			
			Group.getRoot(dsId).addSubObject(vdbPort.getName(), visualPort);
			if (importedList != null) importedList.put(vdbPort.getName(), visualPort);
		}

		ports.put(key, vdbPort);
		portsV.addElement(vdbPort);
	}


	keys = dbTemplate.getMacros().keys();
	while (keys.hasMoreElements())
	{
		Object key = keys.nextElement();
		DBMacro macro = (DBMacro)dbTemplate.getMacros().get(key);
		VDBMacro vdbMacro = new VDBMacro(vt, macro);	

		// skip
		if (Group.getRoot(dsId).getSubObject(vdbMacro.getName())!= null)
		{
			Console.getInstance().println("WARNING: macro with name '" + vdbMacro.getName() + "' already exists, skipping its redefinition...");
			continue;
		}

		// has visual
		if (macro.isHasVisual())
		{
			Macro visualMacro = new Macro(vdbMacro, Group.getRoot(dsId),
								  macro.getX(), macro.getY());
			visualMacro.setColor(macro.getColor());
			visualMacro.setMode(macro.getMode());
			visualMacro.setTextPositionNorth(macro.isNamePositionNorth());
			
			// delegate defaultVisibility
			vdbMacro.setVisibility(macro.getDefaultVisibility());
			
			Group.getRoot(dsId).addSubObject(vdbMacro.getName(), visualMacro);
			if (importedList != null) importedList.put(vdbMacro.getName(), visualMacro);
		}

		macros.put(key, vdbMacro);
		macrosV.addElement(vdbMacro);
		
	}

	DrawingSurface.applyPortAndMacroConnectors(dsId, dbTemplate.getData(), vdbData);
}
/**
 * This method was created in VisualAge.
 * @return com.cosylab.vdct.vdb.VDBFieldData
 * @param dbd com.cosylab.vdct.dbd.DBDData
 * @param dbRecord com.cosylab.vdct.db.DBRecordData
 * @param dbdField com.cosylab.vdct.dbd.DBDFieldData
 */
public static VDBFieldData generateVDBFieldData(Object dsId, DBDData dbd, DBRecordData dbRecord, VDBRecordData vdbRecord, DBDFieldData dbdField) {

	VDBFieldData vdbField = new VDBFieldData();

	boolean monitor = UndoManager.getInstance(dsId).isMonitor();
	UndoManager.getInstance(dsId).setMonitor(false);

	vdbField.setType(dbdField.getField_type());
	vdbField.setName(dbdField.getName());
	vdbField.setValue(dbdField.getInit_value());
	vdbField.setInit_value(dbdField.getInit_value());
	vdbField.setGui_group(dbdField.getGui_group());
	vdbField.setDbdData(dbdField);

	if (dbRecord!=null) {
		DBFieldData dbField = (DBFieldData)(dbRecord.getFields().get(vdbField.name));
		if (dbField!=null) {
	 	 	if (dbField.getValue()!=null) vdbField.setValue(dbField.getValue());
			vdbField.setComment(dbField.getComment());
			vdbField.setVisibility(dbField.getVisibility());
		}
	}

	final String nullString = "";

	if (vdbField.value.equals(nullString) || vdbField.value.equals(dbdField.getInit_value())) 
	 if (dbdField.getField_type()==DBDConstants.DBF_MENU) {
		 // gets first element
	 /*		 DBDMenuData md = (DBDMenuData)(dbd.getDBDMenuData(dbdField.getMenu_name()));
		 if (md!=null) vdbField.setValue(md.getChoices().elements().nextElement().toString());
		 else {
		   System.out.println("Menu '"+dbdField.getMenu_name()+"' not defined in DBD file...");	 
		   return null;
		 }  */ 
		 	//vdbField.setValue(com.cosylab.vdct.Constants.NONE);
		 	if (!dbdField.getInit_value().equals(nullString))
			 	vdbField.setValue(dbdField.getInit_value()+com.cosylab.vdct.Constants.MENU_DEFAULT_VALUE_INDICATOR);
			else
			 	vdbField.setValue(com.cosylab.vdct.Constants.NONE);
			 
	 }
	 else if (dbdField.getField_type()==DBDConstants.DBF_DEVICE)
/*	  if (vdbRecord!=null) {
		 Enumeration e = dbd.getDevices().elements();
		 DBDDeviceData dev;
		 
		 while (e.hasMoreElements()) {
			 dev = (DBDDeviceData)(e.nextElement());
			 if (dev.getRecord_type().equals(vdbRecord.record_type)) {
			 	vdbField.setValue(dev.getChoice_string());
			 	break;
			 }	
		 }
		 
	 }*/ 
			//vdbField.setValue(com.cosylab.vdct.Constants.NONE);
		 	if (!dbdField.getInit_value().equals(nullString))
			 	vdbField.setValue(dbdField.getInit_value()+com.cosylab.vdct.Constants.MENU_DEFAULT_VALUE_INDICATOR);
			else
			 	vdbField.setValue(com.cosylab.vdct.Constants.NONE);

	vdbField.setRecord(vdbRecord);

	UndoManager.getInstance(dsId).setMonitor(monitor);
	
	return vdbField;
}
/**
 * This method was created in VisualAge.
 * @return com.cosylab.vdct.vdb.VDBRecordData
 * @param dsId
 * @param dbd com.cosylab.vdct.dbd.DBDData
 * @param dbRecord com.cosylab.vdct.db.DBRecordData
 * @throws com.cosylab.vdct.db.DBException
 */
public static VDBRecordData generateVDBRecordData(Object dsId, DBDData dbd, DBRecordData dbRecord) throws DBException {

	DBDRecordData dbdRecord = dbd.getDBDRecordData(dbRecord.getRecord_type());
	if (dbdRecord==null) {
		Console.getInstance().println("Record '"+dbRecord.getRecord_type()+"' declared in DB file is not defined in DBD file...");
		return null;
	}

	VDBRecordData vdbRecord = new VDBRecordData();
	DBFieldData dbField;
	DBDFieldData dbdField;

	vdbRecord.setType(dbRecord.getRecord_type());
	vdbRecord.setName(dbRecord.getName());
	vdbRecord.setComment(dbRecord.getComment());

	// order preservation
	Enumeration e = dbRecord.getFieldsV().elements();
	while (e.hasMoreElements()) {
		dbField = (DBFieldData)(e.nextElement());
		dbdField = (DBDFieldData)(dbdRecord.getFields().get(dbField.getName()));
		if (dbdField==null)
			throw new DBException("DBD inconsistency detected! Field '"+dbdRecord.getName()+"."+dbField.getName()+"' is not defined in DBD.");
		vdbRecord.addField(generateVDBFieldData(dsId, dbd, dbRecord, vdbRecord, dbdField));
	}

	e = dbdRecord.getFieldsV().elements();
	while (e.hasMoreElements()) {
		dbdField = (DBDFieldData)(e.nextElement());
		if (!vdbRecord.getFields().containsKey(dbdField.getName()))
			vdbRecord.addField(generateVDBFieldData(dsId, dbd, dbRecord, vdbRecord, dbdField));
	}

	// just link to info fields list
	vdbRecord.setInfoFields(dbRecord.getInfoFieldsV());

	/*  //DBD order
 	Enumeration e = dbdRecord.getFieldsV().elements();
	while (e.hasMoreElements()) {
		dbdField = (DBDFieldData)(e.nextElement());
		vdbRecord.addField(generateVDBFieldData(dbd, dbRecord, vdbRecord, dbdField));
	}
*/
	//vdbRecord.updateDTYP(dbd);

	return vdbRecord;
}
/**
 * This method was created in VisualAge.
 * @return epics.vdb.VDBRecordData
 * @param dsId
 * @param dbd epics.dbd.DBDData
 * @param recordType java.lang.String
 * @param recordName java.lang.String
 */
public static VDBRecordData getNewVDBRecordData(Object dsId, DBDData dbd, String recordType, String recordName) {

	DBDRecordData dbdRecord = dbd.getDBDRecordData(recordType);
	if (dbdRecord==null) {
		Console.getInstance().println("Record '"+recordType+"' is not defined in DBD file...");
		return null;
	}

	VDBRecordData vdbRecord = new VDBRecordData();
	vdbRecord.setType(recordType);
	vdbRecord.setName(recordName);

	DBDFieldData dbdField;
	Enumeration e = dbdRecord.getFieldsV().elements();
	while (e.hasMoreElements()) {
		dbdField = (DBDFieldData)(e.nextElement());
		vdbRecord.addField(generateVDBFieldData(dsId, dbd, null, vdbRecord, dbdField));
	}

	//vdbRecord.updateDTYP(dbd);
	
	return vdbRecord;
}
/**
 * Insert the method's description here.
 * Creation date: (8.1.2001 20:51:38)
 * @return java.util.Vector
 */
public java.util.Vector getRecords() {
	return records;
}
/**
 * This method was created in VisualAge.
 * @return epics.vdb.VDBRecordData
 * @param dsId
 * @param dbd epics.dbd.DBDData
 * @param source epics.vdb.VDBRecordData
 * @param recordType
 * @param recordName
 */
public static VDBRecordData morphVDBRecordData(Object dsId, DBDData dbd, VDBRecordData source, String recordType, String recordName) {

	VDBRecordData vdbRecord = getNewVDBRecordData(dsId, dbd, recordType, recordName);
	if (vdbRecord==null) return null;
	
	VDBFieldData sourceField;
	VDBFieldData targetField;

	DBDMenuData menu;
	String menuName;
	String devName;

	vdbRecord.setType(recordType);
	vdbRecord.setName(recordName);
	//vdbRecord.setDTYP_type(source.getDTYP_type());;

	Enumeration e = vdbRecord.getFieldsV().elements();
	while (e.hasMoreElements()) {
		targetField = (VDBFieldData)(e.nextElement());
		sourceField = source.getField(targetField.getName());
		if (sourceField!=null) {
		  if (targetField.getType()==DBDConstants.DBF_MENU) {
			  menuName = dbd.getDBDRecordData(recordType).getDBDFieldData(targetField.getName()).getMenu_name();
			  menu = dbd.getDBDMenuData(menuName);
			  if (menu.containsValue(sourceField.getValue())) 
			  		copyVDBFieldData(sourceField, targetField);
		  }
		  else if (targetField.getType()==DBDConstants.DBF_DEVICE) {
			 devName = recordType+"/"+sourceField.getValue();
			 if (dbd.getDBDDeviceData(devName)!=null)
			  		copyVDBFieldData(sourceField, targetField);
		  }
		  else copyVDBFieldData(sourceField, targetField);
	      targetField.setRecord(vdbRecord);
		}
	}

	return vdbRecord;
}

public VDBTemplateInstance morphVDBTemplateInstance(Object dsId, VDBTemplateInstance templateData, String templateType, String templateName) {
	
    VDBTemplate template = DataSynchronizer.getInstance().getTemplate(dsId, templateType);		
	if (template==null) return null;
	
	VDBTemplateInstance ti = generateNewVDBTemplateInstance(templateName, template);

	// copy properties
	ti.getProperties().putAll(templateData.getProperties());
	ti.getPropertiesV().addAll(templateData.getPropertiesV());
	
	return ti;
}

/**
 * Insert the method's description here.
 * Creation date: (10.1.2001 14:44:44)
 * @param record com.cosylab.vdct.vdb.VDBRecordData
 */
public void removeRecord(VDBRecordData record) {
	records.removeElement(record);
}
/**
 * Returns the templateInstances.
 * @return Hashtable
 */
public Hashtable getTemplateInstances()
{
	return templateInstances;
}

/**
 * Returns the templateInstances.
 * @return Vector
 */
public Vector getTemplateInstancesV()
{
	return templateInstancesV;
}

/**
 * Returns the templates.
 * @return Hashtable
 */
public Hashtable getTemplates()
{
	return templates;
}

/**
 * Insert the method's description here.
 * Creation date: (10.1.2001 14:44:44)
 * @param template com.cosylab.vdct.vdb.VDBTemplate
 */
public void removeTemplate(VDBTemplate template) {
	templates.remove(template.getId());
}
/**
 * Insert the method's description here.
 * Creation date: (10.1.2001 14:44:44)
 * @param templateInstance com.cosylab.vdct.vdb.VDBTemplateInstance
 */
public void removeTemplateInstance(VDBTemplateInstance templateInstance) {
	templateInstancesV.remove(templateInstance);
	templateInstances.remove(templateInstance);
}

	/**
	 * Returns the structure.
	 * @return Vector
	 */
	public Vector getStructure()
	{
		return structure;
	}
	
	public static VDBData getInstance(Object dsId) {

		VDBData vdbData = (VDBData)instances.get(dsId);
		if (vdbData == null) {
			//System.err.println("Warning: VDBData.getInstance: instance with id does not exist, creating new one.");

			vdbData = new VDBData();
			instances.put(dsId, vdbData);
		}
		
		return vdbData;
	}

	public static void registerDsListener() {

		VDBData vdbData = new VDBData();
		instances.put(Constants.DEFAULT_NAME, vdbData);
		
		GetDsManager command = (GetDsManager)CommandManager.getInstance().getCommand("GetDsManager");
		if (command != null) {
			command.getManager().addDsEventListener(vdbData);
		}
	}

	public void onDsAdded(Object id) {
	    instances.put(id, new VDBData());
	}
	public void onDsRemoved(Object id) {
	}
	public void onDsFocused(Object id) {
	}
}
