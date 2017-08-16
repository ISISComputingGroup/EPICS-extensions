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

import java.awt.Component;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.regex.Pattern;

import javax.swing.JMenuItem;
import javax.swing.JPopupMenu;
import javax.swing.JSeparator;

import com.cosylab.vdct.db.DBMacro;
import com.cosylab.vdct.dbd.DBDConstants;
import com.cosylab.vdct.graphics.objects.Descriptable;
import com.cosylab.vdct.graphics.objects.LinkSource;
import com.cosylab.vdct.graphics.objects.Macro;
import com.cosylab.vdct.inspector.ChangableVisibility;
import com.cosylab.vdct.inspector.InspectableProperty;
import com.cosylab.vdct.inspector.InspectorManager;
import com.cosylab.vdct.undo.DescriptionChangeAction;
import com.cosylab.vdct.undo.UndoManager;

/**
 * RO property of macro represented on HL (template instance is parent)
 * @author Matej
 */
public class VDBMacro implements InspectableProperty, Descriptable, ChangableVisibility, LinkSource
{
	protected String name = null;
	protected String description = null;
	protected String comment = null;
	
	private static final String nullString = "";

	protected static String defaultDescription = "";

	private static final String removeString = "Remove";
	private static final String renameString = "Rename";
	
	private VDBTemplate template = null;

	protected int visibility = ALWAYS_VISIBLE;

	protected Macro visibleObject = null;
	
	/**
	 * Insert the method's description here.
	 */
	public VDBMacro(VDBTemplate template, DBMacro macro)
	{
		this.template = template;
		this.name = macro.getName();
		this.description = macro.getDescription();
		this.comment = macro.getComment();
	}
	
	/**
	 * Insert the method's description here.
	 */
	public VDBMacro(VDBTemplate template, String name, String description)
	{
		this.template = template;

		this.name = name;
		this.description = description;
	}
	
	/**
	 * Returns the name.
	 * @return String
	 */
	public String getName()
	{
		return name;
	}

	/**
	 * Returns the name.
	 * @return String
	 */
	public String getFullName()
	{
		StringBuffer fullName = new StringBuffer();
		fullName.append("$(");
		fullName.append(getName());
		fullName.append(")");
		return fullName.toString();
	}

	/**
	 * Sets the name.
	 * @param name The name to set
	 */
	public void setName(String name)
	{
		this.name = name;
	}

	/**
	 * Sets the target.
     */
	/* @param target The target to set
	 */
/*	public void setTarget(String target)
	{
		this.target = target;
	}

	/**
	 * Returns the description.
	 * @return String
	 */
	public String getDescription()
	{
		if (description==null)
			return defaultDescription;
		else
			return description;
	}

	/**
	 * Returns the description.
	 * @return String
	 */
	public String getRealDescription()
	{
		return description;
	}

	/**
	 * Sets the description.
	 * @param description The description to set
	 */
	public void setDescription(String description)
	{
		boolean update = false;
		
		if (this.description==null || !this.description.equals(description))
		{
			UndoManager.getInstance(template.getDsId()).addAction(
					new DescriptionChangeAction(this, this.description, description));
			update = true;
		}

		this.description = description;

		if (update)
		{
			InspectorManager.getInstance().updateObject(template);

			// update inspector (if inspecting visible rep.)
			if (visibleObject!=null && InspectorManager.getInstance().isInspected(visibleObject))
			{
				InspectorManager.getInstance().updateObject(visibleObject);
			}
		}

	}
	
	/**
	 * Returns macro name
	 * @param templateInstanceName The name of the template instance
     * @return 
	 */
	public String getMacroDefinition(String templateInstanceName)
	{
		StringBuffer fullName = new StringBuffer();
		fullName.append("$(");
		fullName.append(getName());
		fullName.append(")");
		return fullName.toString();
	}
	/**
	 * Returns the comment.
	 * @return String
	 */
	public String getComment()
	{
		return comment;
	}

	/**
	 * Sets the comment.
	 * @param comment The comment to set
	 */
	public void setComment(String comment)
	{
		this.comment = comment;
	}

	/**
	 * @see com.cosylab.vdct.inspector.InspectableProperty#allowsOtherValues()
	 */
	public boolean allowsOtherValues()
	{
		return true;
	}

	/**
	 * @see com.cosylab.vdct.inspector.InspectableProperty#getEditPattern()
	 */
	public Pattern getEditPattern()
	{
		return null;
	}

	/**
	 * @see com.cosylab.vdct.inspector.InspectableProperty#getHelp()
	 */
	public String getHelp()
	{
		return description;
	}

	/**
	 * @see com.cosylab.vdct.inspector.InspectableProperty#getInitValue()
	 */
	public String getInitValue()
	{
		return null;
	}

	/**
	 * @see com.cosylab.vdct.inspector.InspectableProperty#getSelectableValues()
	 */
	public String[] getSelectableValues()
	{
		return null;
	}

	/**
	 * @see com.cosylab.vdct.inspector.InspectableProperty#getToolTipText()
	 */
	public String getToolTipText()
	{
		return description;
	}

	/**
	 * @see com.cosylab.vdct.inspector.InspectableProperty#getValue()
	 */
	public String getValue()
	{
		return nullString;
	}

	/**
	 * @see com.cosylab.vdct.inspector.InspectableProperty#getVisibility()
	 */
	public int getVisibility()
	{
		return visibility;
	}

	/**
	 * @see com.cosylab.vdct.inspector.InspectableProperty#isEditable()
	 */
	public boolean isEditable()
	{
		return false;
	}

	/**
	 * @see com.cosylab.vdct.inspector.InspectableProperty#isSeparator()
	 */
	public boolean isSeparator()
	{
		return false;
	}

	/**
	 * @see com.cosylab.vdct.inspector.InspectableProperty#isValid()
	 */
	public boolean isValid()
	{
		return true;
	}

	/**
	 * @see com.cosylab.vdct.inspector.InspectableProperty#popupEvent(Component, int, int)
	 */
	public void popupEvent(Component component, int x, int y)
	{

		ActionListener al = new ActionListener()
		{
			public void actionPerformed(ActionEvent e) {
				String action = e.getActionCommand();
				if (action.equals(renameString))
					template.renameProperty(VDBMacro.this);
				else if (action.equals(removeString))
					template.removeProperty(VDBMacro.this);
			}
	
		};
		
		JPopupMenu popup = new JPopupMenu();
		
		JMenuItem mi = new JMenuItem(renameString);
		mi.addActionListener(al);
		popup.add(mi);
	
		popup.add(new JSeparator());
		
		mi = new JMenuItem(removeString);
		mi.addActionListener(al);
		popup.add(mi);
	
		popup.show(component, x, y);

	}

	/**
	 * Insert the method's description here.
	 * Creation date: (9.12.2000 18:11:46)
	 * @param newValue java.lang.String
	 */
	public void setValueSilently(java.lang.String newValue) {
	}

	/**
	 * @see com.cosylab.vdct.inspector.InspectableProperty#setValue(String)
	 */
	public void setValue(String value)
	{
	}

	/**
	 * Returns the template.
	 * @return VDBTemplate
	 */
	public VDBTemplate getTemplate()
	{
		return template;
	}

	/**
	 * Sets the template.
	 * @param template The template to set
	 */
	public void setTemplate(VDBTemplate template)
	{
		this.template = template;
	}

	/**
	 * @see com.cosylab.vdct.inspector.ChangableVisibility#setVisibility(int)
	 */
	public void setVisibility(int visibility)
	{
		this.visibility = visibility;
		template.propertyChanged(this);
	}

	/**
	 * @see com.cosylab.vdct.graphics.objects.LinkSource#getType()
	 */
	public int getType()
	{
		return DBDConstants.DBF_MACRO;
	}

	/**
	 * Returns the visibleObject.
	 * @return Macro
	 */
	public Macro getVisibleObject()
	{
		return visibleObject;
	}

	/**
	 * Sets the visibleObject.
	 * @param visibleObject The visibleObject to set
	 */
	public void setVisibleObject(Macro visibleObject)
	{
		this.visibleObject = visibleObject;
	}

	/* (non-Javadoc)
	 * @see com.cosylab.vdct.inspector.InspectableProperty#hasDefaultValue()
	 */
	public boolean hasDefaultValue() {
		return false;
	}
	/* (non-Javadoc)
	 * @see com.cosylab.vdct.inspector.InspectableProperty#hasValidity()
	 */
	public boolean hasValidity() {
		return false;
	}
	/* (non-Javadoc)
	 * @see com.cosylab.vdct.inspector.InspectableProperty#checkValueValidity(java.lang.String)
	 */
	public String checkValueValidity(String value) {
		return null;
	}
	/* (non-Javadoc)
	 * @see com.cosylab.vdct.inspector.InspectableProperty#getGuiGroup()
	 */
	public Integer getGuiGroup() {
		return null;
	}
}
