package com.cosylab.vdct.about;

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

import java.net.MalformedURLException;
import java.net.URL;

import javax.swing.Icon;
import javax.swing.ImageIcon;

import com.cosylab.vdct.Version;

/**
 * @author cosylab
 *
 * To change this generated comment edit the template variable "typecomment":
 * Window&gt;Preferences&gt;Java&gt;Templates.
 * To enable and disable the creation of type comments go to
 * Window&gt;Preferences&gt;Java&gt;Code Generation.
 */
public class VisualDCTProgramTabModel implements ProgramTabModel {

	/**
	 * Constructor for DefaultProgramTabModel.
     * @param toAbout toAbout
	 */
	public VisualDCTProgramTabModel(Object toAbout) {
		super();
	}

	/**
     * @return something
	 * @see com.cosylab.vdct.about.ProgramTabModel#getProductBuildDate()
	 */
	public String getProductBuildDate()
	{
		return null;
	}

	/**
     * @return something
	 * @see com.cosylab.vdct.about.ProgramTabModel#getProductDocsURL()
	 */
	public URL getProductDocsURL()
	{
		try
		{
			return new URL("http://epics-extensions.github.io/VisualDCT/");
		} catch (MalformedURLException mue)
		{
			return null;
		}
	}

	/**
     * @return something
	 * @see com.cosylab.vdct.about.ProgramTabModel#getProductLogoImage()
	 */
	public Icon getProductLogoImage()
	{
		try
		{
			return new ImageIcon(getClass().getResource("/images/cosylab.gif"));
		} catch (Exception mue)
		{
			return null;
		}
	}

	/**
     * @return something
	 * @see com.cosylab.vdct.about.ProgramTabModel#getProductName()
	 */
	public String getProductName()
	{
		return "Visual Database Configuration Tool";
	}

	/**
     * @return something
	 * @see com.cosylab.vdct.about.ProgramTabModel#getProductURL()
	 */
	public URL getProductURL()
	{
		try
		{
			return new URL("https://github.com/epics-extensions/VisualDCT");
		} catch (MalformedURLException mue)
		{
			return null;
		}
	}

	/**
     * @return something
	 * @see com.cosylab.vdct.about.ProgramTabModel#getProductVersion()
	 */
	public String getProductVersion()
	{
		return "v"+Version.VERSION;
	}

	/**
     * @return something
	 * @see com.cosylab.vdct.about.ProgramTabModel#getVendor()
	 */
	public String getVendor()
	{
		return "by Matej Sekoranja, Ivo List, Jaka Bobnar and Sunil Sah, Cosylab d.d.";
	}

	/**
     * @return something
	 * @see com.cosylab.vdct.about.ProgramTabModel#getVendorEmail()
	 */
	public String getVendorEmail()
	{
		return "info@cosylab.com";
	}

	/**
     * @return something
	 * @see com.cosylab.vdct.about.ProgramTabModel#getVendorURL()
	 */
	public URL getVendorURL()
	{
		try
		{
			return new URL("http://www.cosylab.com/");
		} catch (MalformedURLException mue)
		{
			return null;
		}
	}

	/**
	 * @see com.cosylab.vdct.about.AboutTabModel#aquireData()
	 */
	public void aquireData()
	{
	}

}
