package com.cosylab.vdct.undo;

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

/**
 * Insert the type's description here.
 * Creation date: (3.5.2001 15:23:26)
 * @author 
 */
public class MacroValueChangeAction extends ActionObject {
	private com.cosylab.vdct.vdb.VDBMacro macro;
	private String oldValue;
	private String newValue;
	
/**
 * Insert the method's description here.
 * Creation date: (3.5.2001 15:30:47)
 * @param macro com.cosylab.vdct.vdb.VDBMacro
 * @param oldValue java.lang.String
 * @param newValue java.lang.String
 */
public MacroValueChangeAction(com.cosylab.vdct.vdb.VDBMacro macro, String oldValue, String newValue) {
	this.macro=macro;
	this.oldValue=oldValue;
	this.newValue=newValue;
}
/**
 * Insert the method's description here.
 * Creation date: (3.5.2001 15:50:49)
 * @return java.lang.String
 */
public java.lang.String getDescription() {
	return "Macro value change ["+macro.getFullName()+"](\""+oldValue+"\" to \""+newValue+"\")";
}
/**
 * This method was created in VisualAge.
 */
protected void redoAction() {
	macro.setValue(newValue);
}
/**
 * This method was created in VisualAge.
 */
protected void undoAction() {
	macro.setValue(oldValue);
}
}
