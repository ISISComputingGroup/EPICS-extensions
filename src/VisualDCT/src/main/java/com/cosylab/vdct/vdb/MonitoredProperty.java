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

import com.cosylab.vdct.inspector.*;

/**
 * Insert the type's description here.
 * Creation date: (1.2.2001 22:49:33)
 * @author Matej Sekoranja
 */
public class MonitoredProperty implements InspectableProperty {
	private String name;
	private String value;
	private MonitoredPropertyListener listener = null;
	
	private static final String removeString = "Remove";
	private static final String renameString = "Rename";
	
/**
 * DTYPInfoProperty constructor comment.
 */
public MonitoredProperty(String name, String value, MonitoredPropertyListener listener) {
	this.name=name;
	this.value=value;
	this.listener=listener;
}
/**
 * Insert the method's description here.
 * Creation date: (1.2.2001 22:49:33)
 * @return boolean
 */
public boolean allowsOtherValues() {
	return false;
}
/**
 * Insert the method's description here.
 * Creation date: (1.2.2001 22:49:33)
 * @return java.lang.String
 */
public String getHelp() {
	return null;
}
/**
 * Insert the method's description here.
 * Creation date: (1.2.2001 22:49:33)
 * @return java.lang.String
 */
public String getName() {
	return name;
}
/**
 * Insert the method's description here.
 * Creation date: (1.2.2001 22:49:33)
 * @return java.lang.String[]
 */
public java.lang.String[] getSelectableValues() {
	return null;
}
/**
 * Insert the method's description here.
 * Creation date: (1.2.2001 22:49:33)
 * @return java.lang.String
 */
public String getValue() {
	return value;
}
/**
 * Insert the method's description here.
 * Creation date: (11.1.2001 21:29:48)
 * @return java.lang.String
 */
public String getInitValue()
{
	return null;
}
/**
 * Insert the method's description here.
 * Creation date: (1.2.2001 22:49:33)
 * @return boolean
 */
public boolean isEditable() {
	return true;
}
/**
 * Insert the method's description here.
 * Creation date: (1.2.2001 22:49:33)
 * @return boolean
 */
public boolean isSeparator() {
	return false;
}
/**
 * Insert the method's description here.
 * Creation date: (1.2.2001 22:49:33)
 * @param value java.lang.String
 */
public void setValue(String value) {
	this.value=value;
	listener.propertyChanged(this);
}
/**
 * Insert the method's description here.
 * Creation date: (24/8/99 15:29:04)
 * @return java.util.regex.Pattern
 */
public Pattern getEditPattern()
{
	return null;
}
/**
 * Insert the method's description here.
 * Creation date: (24/8/99 15:29:04)
 * @return java.lang.String
 */
public String getToolTipText()
{
	return null;
}
/**
 * Insert the method's description here.
 * Creation date: (11.1.2001 21:28:51)
 * @return boolean
 */
public boolean isValid()
{
	return true;
}
/**
 * Insert the method's description here.
 * Creation date: (11.1.2001 21:30:04)
 * @return int
 */
public int getVisibility()
{
	return InspectableProperty.UNDEFINED_VISIBILITY;
}
/**
 * Insert the method's description here.
 * Creation date: (11.1.2001 21:30:04)
 * @param component java.awt.Component
 * @param x
 * @param y
 */
public void popupEvent(Component component, int x, int y)
{
	ActionListener al = new ActionListener()
	{
		public void actionPerformed(ActionEvent e) {
			String action = e.getActionCommand();
			if (action.equals(renameString))
				listener.renameProperty(MonitoredProperty.this);
			else if (action.equals(removeString))
				listener.removeProperty(MonitoredProperty.this);
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
