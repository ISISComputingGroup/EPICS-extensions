package com.cosylab.vdct.plugins;

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

import com.cosylab.vdct.plugin.export.*;

/**
 * Insert the type's description here.
 * Creation date: (8.12.2001 13:29:26)
 * @author Matej Sekoranja
 */
public class ExportSimulator implements ExportPlugin {
/**
 * Insert the method's description here.
 * Creation date: (8.12.2001 13:29:26)
 */
public void destroy() {}
/**
 * Insert the method's description here.
 * Creation date: (8.12.2001 13:29:26)
 * @param mainGroup com.cosylab.vdct.graphics.objects.Group
 */
public void export(com.cosylab.vdct.graphics.objects.Group mainGroup) {
	com.cosylab.vdct.Console.getInstance().println(getName()+ " should now export, but since it is only a simulator of a plugin... ");
}
/**
 * Insert the method's description here.
 * Creation date: (8.12.2001 13:29:26)
 * @return java.lang.String
 */
public String getAuthor() {
	return "matej.sekoranja@cosylab.com";
}
/**
 * Insert the method's description here.
 * Creation date: (8.12.2001 13:29:26)
 * @return java.lang.String
 */
public String getDescription() {
	return "Simple export plugin simulator";
}
/**
 * Insert the method's description here.
 * Creation date: (8.12.2001 13:29:26)
 * @return
 */
public String getName() {
	return "Export Simulator";
}
/**
 * Insert the method's description here.
 * Creation date: (8.12.2001 13:29:26)
 * @return java.lang.String
 */
public String getVersion() {
	return "0.1";
}
/**
 * Insert the method's description here.
 * Creation date: (8.12.2001 13:29:26) */
public void init(java.util.Properties properties, com.cosylab.vdct.plugin.PluginContext context) {}
/**
 * Insert the method's description here.
 * Creation date: (8.12.2001 13:29:26)
 */
public void start() {}
/**
 * Insert the method's description here.
 * Creation date: (8.12.2001 13:29:26)
 */
public void stop() {}
}
