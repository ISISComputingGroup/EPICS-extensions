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
import java.util.regex.Pattern;

import com.cosylab.vdct.inspector.*;

/**
 * Insert the type's description here.
 * Creation date: (1.2.2001 22:49:33)
 * @author Matej Sekoranja
 */
public class FieldInfoProperty implements InspectableProperty {
	private VDBFieldData field;
/**
 * DTYPInfoProperty constructor comment.
 */
public FieldInfoProperty(VDBFieldData field) {
	this.field=field;
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
	return field.getHelp();
}
/**
 * Insert the method's description here.
 * Creation date: (1.2.2001 22:49:33)
 * @return java.lang.String
 */
public String getName() {
	return field.getName();
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
/*	String value = field.getValue();
	if (value.equals(com.cosylab.vdct.Constants.NONE) || 
		value.indexOf(com.cosylab.vdct.Constants.MENU_DEFAULT_VALUE_INDICATOR)>=0) //!!! default value
		return field.getDbdData().getInit_value();
	else */
		return field.getValue();
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
	return false;
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
public void setValue(String value) {}
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
    if (field.getGui_group() != null)
        return field.getGui_group().getIndex();
    else
        return 0;
}

}
