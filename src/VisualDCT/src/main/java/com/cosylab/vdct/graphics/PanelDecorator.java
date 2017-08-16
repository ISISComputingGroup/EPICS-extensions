package com.cosylab.vdct.graphics;

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

import java.awt.Graphics;
import java.awt.event.ComponentEvent;
import java.awt.event.ComponentListener;

/**
 * PanelDecorator (using Decorator pattern)
 * Creation date: (10.12.2000 11:02:55)
 * @author Matej Sekoranja
 */
public class PanelDecorator extends javax.swing.JPanel implements VisualComponent, ComponentListener {
	private VisualComponent component = null;
/**
 * Insert the method's description here.
 * Creation date: (10.12.2000 13:25:27)
 */
public PanelDecorator() {
	super();
	addComponentListener(this);
}
/**
 * ComponentManager constructor comment.
 * @param component
 */
public PanelDecorator(VisualComponent component) {
	this();
	this.component=component;
}
/**
 * Default implementation
 * Creation date: (10.12.2000 11:25:20)
 * @param g
 */
public void draw(Graphics g) {
	if (component != null) {
	    component.draw(g);
	}
}
/**
 * Insert the method's description here.
 * Creation date: (10.12.2000 13:02:11)
 * @return com.cosylab.vdct.graphics.VisualComponent
 */
public VisualComponent getComponent() {
	return component;
}
/**
 * Insert the method's description here.
 * Creation date: (11.12.2000 16:23:31)
 * @return 
 */
public int getComponentHeight() {
	if (component==null) return 0;
	else return component.getComponentHeight();
}
/**
 * Insert the method's description here.
 * Creation date: (11.12.2000 16:23:02)
 * @return int
 */
public int getComponentWidth() {
	if (component==null) return 0;
	else return component.getComponentWidth();
}
/**
 * Default implementation
 * Creation date: (10.12.2000 11:26:54)
 * @param x0
 * @param y0
 * @param width
 * @param height
 */
public void resize(int x0, int y0, int width, int height) {
	if (component != null) {
		component.resize(x0, y0, width, height);
	}
}
/**
 * Insert the method's description here.
 * Creation date: (10.12.2000 13:02:11)
 * @param newComponent com.cosylab.vdct.graphics.VisualComponent
 */
public void setComponent(VisualComponent newComponent) {
	component = newComponent;
}

/**
/**
 * Invoked when the component has been made invisible.
 */
public void componentHidden(ComponentEvent e) {}
/**
 * Invoked when the component's position changes.
 */
public void componentMoved(ComponentEvent e) {}
/**
 * Invoked when the component's size changes.
 */
public void componentResized(ComponentEvent e) {
	if (component != null) {
	    component.resize(0, 0, getWidth(), getHeight());
	}
}
/**
 * Invoked when the component has been made visible.
 */
public void componentShown(ComponentEvent e) {
}

/**
 * Insert the method's description here.
 * Creation date: (10.12.2000 14:19:55)
 * @param g java.awt.Graphics
 */
protected void paintComponent(Graphics g) {
	if (component != null) {
	    component.draw(g);
	}
}

}
