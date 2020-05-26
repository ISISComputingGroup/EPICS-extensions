package com.cosylab.vdct.dbd;

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
 * This type was created in VisualAge.
 */
public interface DBDConstants {

    /**
     *
     */
    public static final int NOT_DEFINED	= -1;

    /**
     *
     */
    public static final int DBF_STRING 	=  0;

    /**
     *
     */
    public static final int DBF_CHAR	=  1;

    /**
     *
     */
    public static final int DBF_UCHAR	=  2;

    /**
     *
     */
    public static final int DBF_SHORT	=  3;

    /**
     *
     */
    public static final int DBF_USHORT	=  4;

    /**
     *
     */
    public static final int DBF_LONG	=  5;

    /**
     *
     */
    public static final int DBF_ULONG	=  6;

    /**
     *
     */
    public static final int DBF_INT64	=  7;

    /**
     *
     */
    public static final int DBF_UINT64	=  8;

    /**
     *
     */
    public static final int DBF_FLOAT	=  9;

    /**
     *
     */
    public static final int DBF_DOUBLE	= 10;

    /**
     *
     */
    public static final int DBF_ENUM	= 11;

    /**
     *
     */
    public static final int DBF_MENU	= 12;

    /**
     *
     */
    public static final int DBF_DEVICE	= 13;

    /**
     *
     */
    public static final int DBF_INLINK	= 14;

    /**
     *
     */
    public static final int DBF_OUTLINK	= 15;

    /**
     *
     */
    public static final int DBF_FWDLINK	= 16;

    /**
     *
     */
    public static final int DBF_NOACCESS= 17;

	// artificial type

    /**
     *
     */
	public static final int DBF_PORT	 		= Integer.MAX_VALUE-1;

    /**
     *
     */
    public static final int DBF_MACRO    		= Integer.MAX_VALUE-2;

    /**
     *
     */
    public static final int DBF_TEMPLATE_PORT	= Integer.MAX_VALUE-3;

    /**
     *
     */
    public static final int DBF_TEMPLATE_MACRO	= Integer.MAX_VALUE-4;
	
    /**
     *
     */
    public static final int DECIMAL	= 0;

    /**
     *
     */
    public static final int HEX		= 1;
	
    /**
     *
     */
    public static final char quoteChar = '"';
}
