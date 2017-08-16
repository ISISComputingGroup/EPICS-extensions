package com.cosylab.vdct.graphics.objects;

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
 * Creation date: (4.2.2001 21:54:45)
 * @author Matej Sekoranja
 */
public interface Flexible {
/**
 * Returns the copied instance or null if the object cannot be copied. 
 * 
 * Creation date: (4.2.2001 21:59:44)
 * @param dsId
 * @param group java.lang.String
 * @return 
 */
Flexible copyToGroup(Object dsId, String group);
/**
 * Insert the method's description here.
 * Creation date: (3.5.2001 10:15:48)
 * @return java.lang.String
 */
String getFlexibleName();
/**
 * Insert the method's description here.
 * Creation date: (4.2.2001 22:00:01)
 * @param dsId
 * @param group java.lang.String
 * @return 
 */
boolean moveToGroup(Object dsId, String group);
/**
 * Insert the method's description here.
 * Creation date: (2.5.2001 23:23:17)
 * @param dsId
 * @param newName java.lang.String
 * @return 
 */
boolean rename(Object dsId, String newName);
/**
 * Insert the method's description here.
 * Creation date: (3.5.2001 10:15:48)
 * @return java.lang.String
 */
String toString();
}
