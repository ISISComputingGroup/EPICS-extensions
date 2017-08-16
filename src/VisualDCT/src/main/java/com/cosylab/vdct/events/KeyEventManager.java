package com.cosylab.vdct.events;

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

import javax.swing.JComponent;
import java.awt.event.KeyListener;

/**
 * Insert the type's description here.
 * Creation date: (18.12.2000 16:23:24)
 * @author Matej Sekoranja
 */
public class KeyEventManager extends EventManager {
	protected static KeyEventManager instance = null;
/**
 * Insert the method's description here.
 * Creation date: (18.12.2000 15:30:35)
 * @return com.cosylab.vdct.events.KeyEventManager
 */
public static KeyEventManager getInstance() {
	if (instance==null)
		instance = new KeyEventManager();
	return instance;
}
/**
 * Insert the method's description here.
 * Creation date: (18.12.2000 16:12:53)
 * @param id java.lang.String
 * @param component javax.swing.JComponent
 */
public void registerSubscriber(String id, JComponent component) {
	super.registerSubscriber(id, component);
}
/**
 * Insert the method's description here.
 * Creation date: (18.12.2000 16:25:04)
 * @param subscriberID java.lang.String
 * @param listener java.awt.KeyListener
 */
public void subscribe(String subscriberID, KeyListener listener) {
	JComponent comp = (JComponent)getSubscriber().get(subscriberID);
	if (comp!=null)
		comp.addKeyListener(listener);
}
/**
 * Insert the method's description here.
 * Creation date: (18.12.2000 16:12:53)
 * @param id java.lang.String
 * @param component javax.swing.JComponent
 */
 
public void unregisterSubscriber(String id, JComponent component) {
	super.unregisterSubscriber(id, component);
}
/**
 * Insert the method's description here.
 * Creation date: (18.12.2000 16:25:04)
 * @param subscriberID java.lang.String
 * @param listener java.awt.KeyListener
 */
public void unsubscribe(String subscriberID, KeyListener listener) {
	JComponent comp = (JComponent)getSubscriber().get(subscriberID);
	if (comp!=null)
		comp.removeKeyListener(listener);
}
}
