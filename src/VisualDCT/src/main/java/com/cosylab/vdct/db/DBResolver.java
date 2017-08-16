package com.cosylab.vdct.db;

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

import java.io.*;
import java.util.*;
import com.cosylab.vdct.Console;
import com.cosylab.vdct.Settings;
import com.cosylab.vdct.graphics.objects.InLink;
import com.cosylab.vdct.graphics.objects.OutLink;
import com.cosylab.vdct.util.*;
import com.cosylab.vdct.vdb.VDBData;
import com.cosylab.vdct.vdb.VDBTemplate;

/**
 * This type was created in VisualAge.
 */
public class DBResolver {
	private static final String errorString = "Invalid VisualDCT visual data..."; 

	private static final String nullString = "";

	// DB definitions
	public static final String FIELD		= "field";
	public static final String RECORD	= "record";
	public static final String GRECORD	= "grecord";
	public static final String INCLUDE	= "include";
	public static final String INFO	= "info";

	public static final String PATH  	 = "path";
	public static final String ADDPATH  	 = "addpath";

	// new R3.15
	public static final String TEMPLATE	= "template";
	public static final String PORT		= "port";

	public static final String EXPAND	= "expand";
	public static final String MACRO		= "macro";


	// internal
	private static final String ENDSTR = "}";
	//private static final String SPACE = " ";
	private static final String NL = "\n";

	// skip commands

	// skip one line
	// #! SKIP					
	// skip n lines
	// #! SKIP(number of lines)
	public static final String VDCTSKIP = "SKIP";

	// visual data
	// used format #! View(xoffset, yoffset, scale)
	public static final String VDCTVIEW = "View";
	// used format #! SpreadsheetView("type", "name", ...)
	// deprecated format #! SpreadsheetView("type", "name", "mode name", "show all rows", background color, ...)
	public static final String VDCTSPREADSHEET_VIEW = "SpreadsheetView";

	/* The following are inside the SpreadsheetView(...) 
	 */
	// deprecated, used for backward compatibility; used format #! Col("name")
	public static final String VDCTSPREADSHEET_COL = "Col";

	// used format #! ColumnOrder("mode name")
	public static final String VDCTSPREADSHEET_COLUMNORDER = "ColumnOrder";
	// used format #! ShowAllRows("true/false")
	public static final String VDCTSPREADSHEET_SHOWALLROWS = "ShowAllRows";
	// used format #! GroupColumnsByGuiGroup("true/false")
	public static final String VDCTSPREADSHEET_GROUPCOLUMNSBYGUIGROUP = "GroupColumnsByGuiGroup";
	// used format #! BackgroundColor("color")
	public static final String VDCTSPREADSHEET_BACKGROUNDCOLOR = "BackgroundColor";
	// used format #! Column("name", "hidden" [, Width(...) [, Width(...), ...]])
	public static final String VDCTSPREADSHEET_COLUMN = "Column";
	// used format #! Width(split index, nondefault width)
	public static final String VDCTSPREADSHEET_WIDTH = "Width";
	// used format #! HiddenRow("name")
	public static final String VDCTSPREADSHEET_HIDDENROW = "HiddenRow";
	// used format #! RowOrder("column name", "split index", "order string")
	public static final String VDCTSPREADSHEET_ROWORDER = "RowOrder";
	// used format #! SplitCol("column name", "is delimiter string", "pattern string")
	public static final String VDCTSPREADSHEET_SPLITCOLUMN = "SplitCol";
	// used format #! RecentSplit("is delimiter string", "delimiter or pattern")
	public static final String VDCTSPREADSHEET_RECENTSPLIT = "RecentSplit";


	// used format #! Record(recordname, xpos, ypos, color, rotated, "description")
	public static final String VDCTRECORD = "Record";
	// used format #! Group(groupname, xpos, ypos, color, "description")
	public static final String VDCTGROUP = "Group";
	// used format #! Field(fieldname, color, rotated, "description")
	public static final String VDCTFIELD = "Field";
	// used format #! Link(fieldname, inLinkID)
	public static final String VDCTLINK = "Link";
	// used format #! Visibility(fieldname, visibility)
	public static final String VDCTVISIBILITY = "Visibility";
	// used format #! Connector(id, outLinkID, xpos, ypos, color, "description" [, mode])
	public static final String VDCTCONNECTOR = "Connector";
	//	 eg.       #! Record(ts:fanOut0, 124, 432, 324568, 0, "fanOut record")


	// used format #! ConstantPort(portname, inLinkID, xpos, ypos, color, defaultVisibility, textPositionNorth)
	public static final String VDCT_CONSTANT_PORT = "ConstantPort";
	// used format #! InputPort(portname, inLinkID, xpos, ypos, color, defaultVisibility, textPositionNorth)
	public static final String VDCT_INPUT_PORT = "InputPort";
	// used format #! OutputPort(portname, inLinkID, xpos, ypos, color, defaultVisibility, textPositionNorth)
	public static final String VDCT_OUTPUT_PORT = "OutputPort";

	// used format #! InputMacro(macroname, description, xpos, ypos, color, defaultVisibility, textPositionNorth)
	// used format #! OutputMacro(macroname, description, xpos, ypos, color, defaultVisibility, textPositionNorth)
	// textPositionNorth is optional; if exists value is true or false
	public static final String VDCT_INPUT_MACRO = "InputMacro";
	public static final String VDCT_OUTPUT_MACRO = "OutputMacro";

	// used format #! Line(name, xpos, ypos, xpos2, ypos2, dashed, startArrow, endArrow, color, parentBorderID)
	// used format #! Box(name, xpos, ypos, xpos2, ypos2, dashed, color, parentBorderID)
	// used format #! TextBox(name, xpos, ypos, xpos2, ypos2, border, fontFamilyName, fontSize, fontStyle, color, "description", parentBorderID)
	public static final String VDCTLINE = "Line";
	public static final String VDCTBOX = "Box";
	public static final String VDCTTEXTBOX = "TextBox";

	// not used yet
	//public static final String VDCTBORDER = "Border";

	// incoded DBDs
	// used format:
	// #! DBDSTART
	// #! DBD("DBD filename")
	// ...
	// #! DBD("DBD filename")
	// #! DBDEND
	private static final String DBD_START_STR	= "DBDSTART";
	private static final String DBD_ENTRY_STR	= "DBD";
	private static final String DBD_END_STR		= "DBDEND";

	public static final String DBD_START	= "#! "+DBD_START_STR+"\n";
	public static final String DBD_ENTRY	= "#! "+DBD_ENTRY_STR+"(\"";
	public static final String DBD_END	= "#! "+DBD_END_STR+"\n";

	// template 'instatiation'
	// used format:
	// #! TemplateInstance("template instance id", x, y, color, "desc")
	// #! TemplateField("template instance id", "fieldName", color, isRight, visibility)
	public static final String TEMPLATE_INSTANCE = "TemplateInstance";
	public static final String TEMPLATE_FIELD = "TemplateField";

	/**
	 * This method was created in VisualAge.
	 * @return java.io.EnhancedStreamTokenizer
	 * @param fileName java.lang.String
	 */
	public static EnhancedStreamTokenizer getEnhancedStreamTokenizer(String fileName) {

		FileInputStream fi = null;
		EnhancedStreamTokenizer tokenizer = null;

		try	{
			fi = new FileInputStream(fileName);
			tokenizer = new EnhancedStreamTokenizer(new BufferedReader(new InputStreamReader(fi)));
			initializeTokenizer(tokenizer);
		} catch (IOException e) {
			Console.getInstance().println("\no) Error occured while opening file '"+fileName+"'");
			Console.getInstance().println(e);
		}

		return tokenizer;
	}
	/**
	 * This method was created in VisualAge.
     * @param is
	 * @return java.io.EnhancedStreamTokenizer
	 */
	public static EnhancedStreamTokenizer getEnhancedStreamTokenizer(InputStream is) {

		EnhancedStreamTokenizer tokenizer = null;

		try	{
			tokenizer = new EnhancedStreamTokenizer(new BufferedReader(new InputStreamReader(is)));
			initializeTokenizer(tokenizer);
		} catch (Throwable e) {
			Console.getInstance().println("\no) Error occured while opening stream '"+is+"'");
			Console.getInstance().println(e);
		}

		return tokenizer;
	}
	/**
	 * This method was created in VisualAge.
	 * @param tokenizer java.io.EnhancedStreamTokenizer
	 */
	public static void initializeTokenizer(EnhancedStreamTokenizer tokenizer) {
		tokenizer.setParseEscapeSequences(false);
		tokenizer.resetSyntax();
		tokenizer.whitespaceChars(0, 32);
		tokenizer.wordChars(33, 255);			// reset
		tokenizer.eolIsSignificant(true);
		tokenizer.parseNumbers();
		tokenizer.quoteChar(DBConstants.quoteChar);
		tokenizer.whitespaceChars(',', ',');
		tokenizer.whitespaceChars('{', '{');
		tokenizer.whitespaceChars('(', '(');
		tokenizer.whitespaceChars(')',')');
	}



	private static String loadTemplate(Object dsId, DBData data, String templateFile, String referencedFromFile,
			PathSpecification paths, Stack loadStack, ArrayList loadList) throws Exception
			{

		File file = paths.search4File(templateFile);
		String templateToResolve = file.getAbsolutePath();

		// check if file already loaded
		boolean alreadyLoaded = false;

		// check this load session DB repository
		alreadyLoaded = loadList.contains(templateToResolve);

		// check central VDB repository
		if (!alreadyLoaded)
		{
			Enumeration enumer = VDBData.getInstance(dsId).getTemplates().elements();
			while (enumer.hasMoreElements())
			{
				VDBTemplate t = (VDBTemplate)enumer.nextElement();
				if (t.getFileName().equals(templateToResolve))
				{
					alreadyLoaded = true;
					break;
				}
			}
		}

		//if(DataProvider.getInstance().getLoadedDBs().contains(templateToResolve))
		if (alreadyLoaded)
		{
			//Console.getInstance().println("Template \""+templateFile+"\" already loaded...");

			// extract id (not a prefect solution)
			// id is name
			return file.getName();
		}

		//Console.getInstance().println("Loading template \""+templateFile+"\"...");

		DBData templateData = resolveDB(dsId, templateToResolve, loadStack, loadList);
		if (templateData==null)
			throw new DBException("Failed to load template: '"+templateFile+"'");
		templateData.getTemplateData().setData(templateData);
		data.addTemplate(templateData.getTemplateData());

		loadList.add(templateToResolve);

		// add to loaded list
		//DataProvider.getInstance().getLoadedDBs().addElement(templateData.getTemplateData().getFileName());

		//Console.getInstance().println("Template \""+templateFile+"\" loaded.");

		return templateData.getTemplateData().getId();
			}



	/**
	 * VisualDCT layout data is also processed here
     * @param dsId
	 * @param data com.cosylab.vdct.db.DBData
	 * @param tokenizer java.io.EnhancedStreamTokenizer
     * @param fileName
     * @return 
     * @throws java.lang.Exception
	 */
	public static String processComment(Object dsId, DBData data, EnhancedStreamTokenizer tokenizer, String fileName) throws Exception {

		if ((data==null) || !tokenizer.sval.equals(DBConstants.layoutDataString)) {	// comment
			String comment = tokenizer.sval;

			// initialize tokenizer for comments
			tokenizer.resetSyntax();
			tokenizer.whitespaceChars(0, 31);
			tokenizer.wordChars(32, 255);		
			tokenizer.wordChars('\t', '\t');
			tokenizer.eolIsSignificant(true);

			while ((tokenizer.nextToken() != EnhancedStreamTokenizer.TT_EOL) &&						// read till EOL
					(tokenizer.ttype != EnhancedStreamTokenizer.TT_EOF))
				if (tokenizer.ttype == EnhancedStreamTokenizer.TT_NUMBER) {
					//if (!comment.equals(nullString)) comment+=SPACE;
					comment=comment+tokenizer.nval;
				}
				else {
					//if (!comment.equals(nullString)) comment+=SPACE;
					comment=comment+tokenizer.sval;
				}

			// reinitialzie it back
			initializeTokenizer(tokenizer);

			return comment+NL;

		}
		else {																		// graphics layout data

			DBRecordData rd;
			DBFieldData fd;
			String str, str2, desc; int t, tx, tx2, ty, ty2, t2, t3;
//			boolean r1, r2;

			while ((tokenizer.nextToken() != EnhancedStreamTokenizer.TT_EOL) &&
					(tokenizer.ttype != EnhancedStreamTokenizer.TT_EOF)) 
				if (tokenizer.ttype == EnhancedStreamTokenizer.TT_WORD) 

					if (tokenizer.sval.equalsIgnoreCase(VDCTRECORD)) {

						// read record_name
						tokenizer.nextToken();
						if ((tokenizer.ttype == EnhancedStreamTokenizer.TT_WORD)||
								(tokenizer.ttype == DBConstants.quoteChar)) str=tokenizer.sval;
						else throw (new DBGParseException(errorString, tokenizer, fileName));

						rd = (DBRecordData)(data.getRecords().get(str));
						if (rd!=null) {
							// read x pos
							tokenizer.nextToken();
							if (tokenizer.ttype == EnhancedStreamTokenizer.TT_NUMBER) rd.setX((int)tokenizer.nval);
							else throw (new DBGParseException(errorString, tokenizer, fileName));

							// read y pos
							tokenizer.nextToken();
							if (tokenizer.ttype == EnhancedStreamTokenizer.TT_NUMBER) rd.setY((int)tokenizer.nval);
							else throw (new DBGParseException(errorString, tokenizer, fileName));

							// read color
							tokenizer.nextToken();
							if (tokenizer.ttype == EnhancedStreamTokenizer.TT_NUMBER) rd.setColor(StringUtils.int2color((int)tokenizer.nval));
							else throw (new DBGParseException(errorString, tokenizer, fileName));

							// read rotation
							tokenizer.nextToken();
							if (tokenizer.ttype == EnhancedStreamTokenizer.TT_NUMBER) rd.setRotated(((int)tokenizer.nval)!=0);
							else throw (new DBGParseException(errorString, tokenizer, fileName));

							// read description
							tokenizer.nextToken();
							if ((tokenizer.ttype == EnhancedStreamTokenizer.TT_WORD)||
									(tokenizer.ttype == DBConstants.quoteChar)) rd.setDescription(tokenizer.sval);
							else throw (new DBGParseException(errorString, tokenizer, fileName));

						}
					}

					else if (tokenizer.sval.equalsIgnoreCase(VDCTFIELD)) {

						// read name
						tokenizer.nextToken();
						if ((tokenizer.ttype == EnhancedStreamTokenizer.TT_WORD)||
								(tokenizer.ttype == DBConstants.quoteChar)) str=tokenizer.sval;
						else throw (new DBGParseException(errorString, tokenizer, fileName));

						int pos = str.lastIndexOf(com.cosylab.vdct.Constants.FIELD_SEPARATOR);
						str2 = str.substring(pos+1);
						str = str.substring(0, pos);

						rd = (DBRecordData)data.getRecords().get(str);
						if (rd!=null)
						{
							fd=(DBFieldData)rd.getFields().get(str2);
							if (fd==null)
							{
								// default or null (-> default will be set when converted to VDB) value to be set
								// this code has to be here to preserve field order
								fd = new DBFieldData(str2, null);
								rd.addField(fd);
							}

							// read color
							tokenizer.nextToken();
							if (tokenizer.ttype == EnhancedStreamTokenizer.TT_NUMBER) fd.setColor(StringUtils.int2color((int)tokenizer.nval));
							else throw (new DBGParseException(errorString, tokenizer, fileName));

							// read rotation
							tokenizer.nextToken();
							if (tokenizer.ttype == EnhancedStreamTokenizer.TT_NUMBER) fd.setRotated(((int)tokenizer.nval)!=0);
							else throw (new DBGParseException(errorString, tokenizer, fileName));

							// read description
							tokenizer.nextToken();
							if ((tokenizer.ttype == EnhancedStreamTokenizer.TT_WORD)||
									(tokenizer.ttype == DBConstants.quoteChar)) fd.setDescription(tokenizer.sval);
							else throw (new DBGParseException(errorString, tokenizer, fileName));

							rd.addVisualField(fd);
						}
					}

					else if (tokenizer.sval.equalsIgnoreCase(VDCTLINK)) {

						// read name
						tokenizer.nextToken();
						if ((tokenizer.ttype == EnhancedStreamTokenizer.TT_WORD)||
								(tokenizer.ttype == DBConstants.quoteChar)) str=tokenizer.sval;
						else throw (new DBGParseException(errorString, tokenizer, fileName));

						// read target
						tokenizer.nextToken();
						if ((tokenizer.ttype == EnhancedStreamTokenizer.TT_WORD)||
								(tokenizer.ttype == DBConstants.quoteChar)) str2=tokenizer.sval;
						else throw (new DBGParseException(errorString, tokenizer, fileName));

						data.addLink(new DBLinkData(str, str2));

					}

					else if (tokenizer.sval.equalsIgnoreCase(VDCTVISIBILITY)) {

						// read name
						tokenizer.nextToken();
						if ((tokenizer.ttype == EnhancedStreamTokenizer.TT_WORD)||
								(tokenizer.ttype == DBConstants.quoteChar)) str=tokenizer.sval;
						else throw (new DBGParseException(errorString, tokenizer, fileName));

						int pos = str.lastIndexOf(com.cosylab.vdct.Constants.FIELD_SEPARATOR);
						str2 = str.substring(pos+1);
						str = str.substring(0, pos);

						rd = (DBRecordData)data.getRecords().get(str);
						if (rd!=null)
						{
							fd=(DBFieldData)rd.getFields().get(str2);
							if (fd==null)
							{
								// default or null (-> default will be set when converted to VDB) value to be set
								// this code has to be here to preserve field order
								fd = new DBFieldData(str2, null);
								rd.addField(fd);
							} 

							tokenizer.nextToken();
							if (tokenizer.ttype == EnhancedStreamTokenizer.TT_NUMBER) fd.setVisibility((int)tokenizer.nval);
							else throw (new DBGParseException(errorString, tokenizer, fileName));
						}
					}

					else if (tokenizer.sval.equalsIgnoreCase(VDCTCONNECTOR)) {

						// read connector id
						tokenizer.nextToken();
						if ((tokenizer.ttype == EnhancedStreamTokenizer.TT_WORD)||
								(tokenizer.ttype == DBConstants.quoteChar)) str=tokenizer.sval;
						else throw (new DBGParseException(errorString, tokenizer, fileName));

						// read target id
						tokenizer.nextToken();
						if ((tokenizer.ttype == EnhancedStreamTokenizer.TT_WORD)||
								(tokenizer.ttype == DBConstants.quoteChar)) str2=tokenizer.sval;
						else throw (new DBGParseException(errorString, tokenizer, fileName));

						// read x pos
						tokenizer.nextToken();
						if (tokenizer.ttype == EnhancedStreamTokenizer.TT_NUMBER) tx=(int)tokenizer.nval;
						else throw (new DBGParseException(errorString, tokenizer, fileName));

						// read y pos
						tokenizer.nextToken();
						if (tokenizer.ttype == EnhancedStreamTokenizer.TT_NUMBER) ty=(int)tokenizer.nval;
						else throw (new DBGParseException(errorString, tokenizer, fileName));

						// read color
						tokenizer.nextToken();
						if (tokenizer.ttype == EnhancedStreamTokenizer.TT_NUMBER) t=(int)tokenizer.nval;
						else throw (new DBGParseException(errorString, tokenizer, fileName));

						// read description
						tokenizer.nextToken();
						if ((tokenizer.ttype == EnhancedStreamTokenizer.TT_WORD)||
								(tokenizer.ttype == DBConstants.quoteChar)) desc=tokenizer.sval;
						else throw (new DBGParseException(errorString, tokenizer, fileName));

						// read mode
						int mode = 0;
						tokenizer.nextToken();
						if (tokenizer.ttype == EnhancedStreamTokenizer.TT_NUMBER) mode=(int)tokenizer.nval;
						else 
							tokenizer.pushBack();

						data.addConnector(new DBConnectorData(str, str2, tx, ty, StringUtils.int2color(t), desc, mode));

					}


					else if (tokenizer.sval.equalsIgnoreCase(VDCTGROUP)) {

						// read group_name
						tokenizer.nextToken();
						if ((tokenizer.ttype == EnhancedStreamTokenizer.TT_WORD)||
								(tokenizer.ttype == DBConstants.quoteChar)) str=tokenizer.sval;
						else throw (new DBGParseException(errorString, tokenizer, fileName));

						// read x pos
						tokenizer.nextToken();
						if (tokenizer.ttype == EnhancedStreamTokenizer.TT_NUMBER) tx=(int)tokenizer.nval;
						else throw (new DBGParseException(errorString, tokenizer, fileName));

						// read y pos
						tokenizer.nextToken();
						if (tokenizer.ttype == EnhancedStreamTokenizer.TT_NUMBER) ty=(int)tokenizer.nval;
						else throw (new DBGParseException(errorString, tokenizer, fileName));

						// read color
						tokenizer.nextToken();
						if (tokenizer.ttype == EnhancedStreamTokenizer.TT_NUMBER) t=(int)tokenizer.nval;
						else throw (new DBGParseException(errorString, tokenizer, fileName));

						// read description
						tokenizer.nextToken();
						if ((tokenizer.ttype == EnhancedStreamTokenizer.TT_WORD)||
								(tokenizer.ttype == DBConstants.quoteChar)) desc=tokenizer.sval;
						else throw (new DBGParseException(errorString, tokenizer, fileName));

						data.addGroup(new DBGroupData(str, tx, ty, StringUtils.int2color(t), desc));

					}

					else if (tokenizer.sval.equalsIgnoreCase(TEMPLATE_INSTANCE)) {

						// read template instance id
						tokenizer.nextToken();
						if ((tokenizer.ttype == EnhancedStreamTokenizer.TT_WORD)||
								(tokenizer.ttype == DBConstants.quoteChar)) str=tokenizer.sval;
						else throw (new DBGParseException(errorString, tokenizer, fileName));

						// read x pos
						tokenizer.nextToken();
						if (tokenizer.ttype == EnhancedStreamTokenizer.TT_NUMBER) tx=(int)tokenizer.nval;
						else throw (new DBGParseException(errorString, tokenizer, fileName));

						// read y pos
						tokenizer.nextToken();
						if (tokenizer.ttype == EnhancedStreamTokenizer.TT_NUMBER) ty=(int)tokenizer.nval;
						else throw (new DBGParseException(errorString, tokenizer, fileName));

						// read color
						tokenizer.nextToken();
						if (tokenizer.ttype == EnhancedStreamTokenizer.TT_NUMBER) t=(int)tokenizer.nval;
						else throw (new DBGParseException(errorString, tokenizer, fileName));

						// read description
						tokenizer.nextToken();
						if ((tokenizer.ttype == EnhancedStreamTokenizer.TT_WORD)||
								(tokenizer.ttype == DBConstants.quoteChar)) desc=tokenizer.sval;
						else throw (new DBGParseException(errorString, tokenizer, fileName));

						DBTemplateInstance ti = (DBTemplateInstance)data.getTemplateInstances().get(str);
						if (ti!=null)
						{
							ti.setX(tx); ti.setY(ty); ti.setColor(StringUtils.int2color(t));
							ti.setDescription(desc);
						}
					}

					else if (tokenizer.sval.equalsIgnoreCase(TEMPLATE_FIELD)) {

						// read template instance id
						tokenizer.nextToken();
						if ((tokenizer.ttype == EnhancedStreamTokenizer.TT_WORD)||
								(tokenizer.ttype == DBConstants.quoteChar)) str=tokenizer.sval;
						else throw (new DBGParseException(errorString, tokenizer, fileName));

						// read template instance field id
						tokenizer.nextToken();
						if ((tokenizer.ttype == EnhancedStreamTokenizer.TT_WORD)||
								(tokenizer.ttype == DBConstants.quoteChar)) str2=tokenizer.sval;
						else throw (new DBGParseException(errorString, tokenizer, fileName));

						// read color
						tokenizer.nextToken();
						if (tokenizer.ttype == EnhancedStreamTokenizer.TT_NUMBER) t=(int)tokenizer.nval;
						else throw (new DBGParseException(errorString, tokenizer, fileName));

						// read isRight
						boolean isRight = false;
						tokenizer.nextToken();
						if (tokenizer.ttype == EnhancedStreamTokenizer.TT_NUMBER) isRight=((int)tokenizer.nval)!=0;
						else throw (new DBGParseException(errorString, tokenizer, fileName));

						// read visibility 
						tokenizer.nextToken();
						if (tokenizer.ttype == EnhancedStreamTokenizer.TT_NUMBER) t2=(int)tokenizer.nval;
						else throw (new DBGParseException(errorString, tokenizer, fileName));

						DBTemplateInstance ti = (DBTemplateInstance)data.getTemplateInstances().get(str);
						if (ti!=null)
							ti.getTemplateFields().addElement(new DBTemplateField(str2, StringUtils.int2color(t), isRight, t2));
					}

					else if (tokenizer.sval.equalsIgnoreCase(VDCTLINE)) {
						// read template name
						tokenizer.nextToken();
						if ((tokenizer.ttype == EnhancedStreamTokenizer.TT_WORD)||
								(tokenizer.ttype == DBConstants.quoteChar)) str=tokenizer.sval;
						else throw (new DBGParseException(errorString, tokenizer, fileName));

						// read x pos
						tokenizer.nextToken();
						if (tokenizer.ttype == EnhancedStreamTokenizer.TT_NUMBER) tx=(int)tokenizer.nval;
						else throw (new DBGParseException(errorString, tokenizer, fileName));

						// read y pos
						tokenizer.nextToken();
						if (tokenizer.ttype == EnhancedStreamTokenizer.TT_NUMBER) ty=(int)tokenizer.nval;
						else throw (new DBGParseException(errorString, tokenizer, fileName));

						// read x2 pos
						tokenizer.nextToken();
						if (tokenizer.ttype == EnhancedStreamTokenizer.TT_NUMBER) tx2=(int)tokenizer.nval;
						else throw (new DBGParseException(errorString, tokenizer, fileName));

						// read y2 pos
						tokenizer.nextToken();
						if (tokenizer.ttype == EnhancedStreamTokenizer.TT_NUMBER) ty2=(int)tokenizer.nval;
						else throw (new DBGParseException(errorString, tokenizer, fileName));

						// read dashed
						boolean dashed = false;
						tokenizer.nextToken();
						if (tokenizer.ttype == EnhancedStreamTokenizer.TT_NUMBER) dashed=((int)tokenizer.nval)!=0;
						else throw (new DBGParseException(errorString, tokenizer, fileName));

						// read startArrow
						boolean startArrow = false;
						tokenizer.nextToken();
						if (tokenizer.ttype == EnhancedStreamTokenizer.TT_NUMBER) startArrow=((int)tokenizer.nval)!=0;
						else throw (new DBGParseException(errorString, tokenizer, fileName));

						// read endArrow
						boolean endArrow = false;
						tokenizer.nextToken();
						if (tokenizer.ttype == EnhancedStreamTokenizer.TT_NUMBER) endArrow=((int)tokenizer.nval)!=0;
						else throw (new DBGParseException(errorString, tokenizer, fileName));

						// read color
						tokenizer.nextToken();
						if (tokenizer.ttype == EnhancedStreamTokenizer.TT_NUMBER) t=(int)tokenizer.nval;
						else throw (new DBGParseException(errorString, tokenizer, fileName));

						// read parentBorderID
						tokenizer.nextToken();
						String parentBorderID = null;
						if ((tokenizer.ttype == EnhancedStreamTokenizer.TT_WORD)||
								(tokenizer.ttype == DBConstants.quoteChar)) parentBorderID=tokenizer.sval;
						//else throw (new DBGParseException(errorString, tokenizer, fileName));

						data.addLine(new DBLine(str, tx, ty, tx2, ty2, dashed, startArrow, endArrow, StringUtils.int2color(t), parentBorderID));
					}

					else if (tokenizer.sval.equalsIgnoreCase(VDCTBOX)) {
						// read template name
						tokenizer.nextToken();
						if ((tokenizer.ttype == EnhancedStreamTokenizer.TT_WORD)||
								(tokenizer.ttype == DBConstants.quoteChar)) str=tokenizer.sval;
						else throw (new DBGParseException(errorString, tokenizer, fileName));

						// read x pos
						tokenizer.nextToken();
						if (tokenizer.ttype == EnhancedStreamTokenizer.TT_NUMBER) tx=(int)tokenizer.nval;
						else throw (new DBGParseException(errorString, tokenizer, fileName));

						// read y pos
						tokenizer.nextToken();
						if (tokenizer.ttype == EnhancedStreamTokenizer.TT_NUMBER) ty=(int)tokenizer.nval;
						else throw (new DBGParseException(errorString, tokenizer, fileName));

						// read x2 pos
						tokenizer.nextToken();
						if (tokenizer.ttype == EnhancedStreamTokenizer.TT_NUMBER) tx2=(int)tokenizer.nval;
						else throw (new DBGParseException(errorString, tokenizer, fileName));

						// read y2 pos
						tokenizer.nextToken();
						if (tokenizer.ttype == EnhancedStreamTokenizer.TT_NUMBER) ty2=(int)tokenizer.nval;
						else throw (new DBGParseException(errorString, tokenizer, fileName));

						// read dashed
						boolean dashed = false;
						tokenizer.nextToken();
						if (tokenizer.ttype == EnhancedStreamTokenizer.TT_NUMBER) dashed=((int)tokenizer.nval)!=0;
						else throw (new DBGParseException(errorString, tokenizer, fileName));

						// read color
						tokenizer.nextToken();
						if (tokenizer.ttype == EnhancedStreamTokenizer.TT_NUMBER) t=(int)tokenizer.nval;
						else throw (new DBGParseException(errorString, tokenizer, fileName));

						// read parentBorderID
						tokenizer.nextToken();
						String parentBorderID = null;
						if ((tokenizer.ttype == EnhancedStreamTokenizer.TT_WORD)||
								(tokenizer.ttype == DBConstants.quoteChar)) parentBorderID=tokenizer.sval;
						//else throw (new DBGParseException(errorString, tokenizer, fileName));

						data.addBox(new DBBox(str, tx, ty, tx2, ty2, dashed, StringUtils.int2color(t), parentBorderID));
					}

					else if (tokenizer.sval.equalsIgnoreCase(VDCTTEXTBOX)) {
						// read template name
						tokenizer.nextToken();
						if ((tokenizer.ttype == EnhancedStreamTokenizer.TT_WORD)||
								(tokenizer.ttype == DBConstants.quoteChar)) str=tokenizer.sval;
						else throw (new DBGParseException(errorString, tokenizer, fileName));

						// read x pos
						tokenizer.nextToken();
						if (tokenizer.ttype == EnhancedStreamTokenizer.TT_NUMBER) tx=(int)tokenizer.nval;
						else throw (new DBGParseException(errorString, tokenizer, fileName));

						// read y pos
						tokenizer.nextToken();
						if (tokenizer.ttype == EnhancedStreamTokenizer.TT_NUMBER) ty=(int)tokenizer.nval;
						else throw (new DBGParseException(errorString, tokenizer, fileName));

						// read x2 pos
						tokenizer.nextToken();
						if (tokenizer.ttype == EnhancedStreamTokenizer.TT_NUMBER) tx2=(int)tokenizer.nval;
						else throw (new DBGParseException(errorString, tokenizer, fileName));

						// read y2 pos
						tokenizer.nextToken();
						if (tokenizer.ttype == EnhancedStreamTokenizer.TT_NUMBER) ty2=(int)tokenizer.nval;
						else throw (new DBGParseException(errorString, tokenizer, fileName));

						// read border
						boolean border = false;
						tokenizer.nextToken();
						if (tokenizer.ttype == EnhancedStreamTokenizer.TT_NUMBER) border=((int)tokenizer.nval)!=0;
						else throw (new DBGParseException(errorString, tokenizer, fileName));

						// read fontName
						tokenizer.nextToken();
						if ((tokenizer.ttype == EnhancedStreamTokenizer.TT_WORD)||
								(tokenizer.ttype == DBConstants.quoteChar)) str2=tokenizer.sval;
						else throw (new DBGParseException(errorString, tokenizer, fileName));

						// read fontSize
						tokenizer.nextToken();
						if (tokenizer.ttype == EnhancedStreamTokenizer.TT_NUMBER) t2=(int)tokenizer.nval;
						else throw (new DBGParseException(errorString, tokenizer, fileName));

						// read fontStyle
						tokenizer.nextToken();
						if (tokenizer.ttype == EnhancedStreamTokenizer.TT_NUMBER) t3=(int)tokenizer.nval;
						else throw (new DBGParseException(errorString, tokenizer, fileName));

						// read color
						tokenizer.nextToken();
						if (tokenizer.ttype == EnhancedStreamTokenizer.TT_NUMBER) t=(int)tokenizer.nval;
						else throw (new DBGParseException(errorString, tokenizer, fileName));

						// read description
						// this is the case where escape sequences should be parsed (because of n)
						try
						{
							tokenizer.setParseEscapeSequences(true);
							tokenizer.nextToken();
						}
						finally
						{
							tokenizer.setParseEscapeSequences(false);
						}
						if ((tokenizer.ttype == EnhancedStreamTokenizer.TT_WORD)||
								(tokenizer.ttype == DBConstants.quoteChar)) desc=tokenizer.sval;
						else throw (new DBGParseException(errorString, tokenizer, fileName));

						// read parentBorderID
						tokenizer.nextToken();
						String parentBorderID = null;
						if ((tokenizer.ttype == EnhancedStreamTokenizer.TT_WORD)||
								(tokenizer.ttype == DBConstants.quoteChar)) parentBorderID=tokenizer.sval;
						//else throw (new DBGParseException(errorString, tokenizer, fileName));

						data.addTextBox(new DBTextBox(str, tx, ty, tx2, ty2, border, str2, t2, t3, StringUtils.int2color(t), desc, parentBorderID));
					}

					else if (tokenizer.sval.equalsIgnoreCase(VDCTVIEW)) {

						// read rx
						tokenizer.nextToken();
						if (tokenizer.ttype == EnhancedStreamTokenizer.TT_NUMBER) t=(int)tokenizer.nval;
						else throw (new DBGParseException(errorString, tokenizer, fileName));

						// read ry
						tokenizer.nextToken();
						if (tokenizer.ttype == EnhancedStreamTokenizer.TT_NUMBER) t2=(int)tokenizer.nval;
						else throw (new DBGParseException(errorString, tokenizer, fileName));

						// read scale
						double scale = 1.0;
						tokenizer.nextToken();
						if (tokenizer.ttype == EnhancedStreamTokenizer.TT_NUMBER) scale=tokenizer.nval;
						else throw (new DBGParseException(errorString, tokenizer, fileName));

						data.setView(new DBView(t, t2, scale));

					} else if (tokenizer.sval.equalsIgnoreCase(VDCTSPREADSHEET_VIEW)) {

						// Read type. 
						String type = null;
						tokenizer.nextToken();
						if (tokenizer.ttype == EnhancedStreamTokenizer.TT_WORD ||
								tokenizer.ttype == DBConstants.quoteChar) {
							type = tokenizer.sval;
						} else {
							throw (new DBGParseException(errorString, tokenizer, fileName));
						}

						// Read name. 
						String name = null;
						tokenizer.nextToken();
						if (tokenizer.ttype == EnhancedStreamTokenizer.TT_WORD ||
								tokenizer.ttype == DBConstants.quoteChar) {
							name = tokenizer.sval;
						} else {
							throw (new DBGParseException(errorString, tokenizer, fileName));
						}

						// Read properties.
						String modeName = null;
						Boolean showAllRows = null;
						Boolean groupColumnsByGuiGroup = null;
						Integer backgroundColor = null;
						DBSheetRowOrder rowOrder = null;
						Map columnsMap = new HashMap();
						Vector splitColumnsVector = new Vector();
						Vector hiddenRowsVector = new Vector();
						Vector recentSplitsVector = new Vector();

						int columnPropertyIndex = 0;
						int namelessPropertyIndex = 0;

						tokenizer.nextToken();
						while (tokenizer.ttype != EnhancedStreamTokenizer.TT_EOL &&
								tokenizer.ttype != EnhancedStreamTokenizer.TT_EOF) {

							if (tokenizer.ttype == EnhancedStreamTokenizer.TT_WORD ||
									tokenizer.ttype == DBConstants.quoteChar) {

								if (tokenizer.sval.equalsIgnoreCase(VDCTSPREADSHEET_COLUMNORDER)) {
									tokenizer.nextToken();
									if (tokenizer.ttype == EnhancedStreamTokenizer.TT_WORD ||
											tokenizer.ttype == DBConstants.quoteChar) {
										modeName = tokenizer.sval;
									} else {
										throw (new DBGParseException(errorString, tokenizer, fileName));
									}
								} else if (tokenizer.sval.equalsIgnoreCase(VDCTSPREADSHEET_SHOWALLROWS)) {
									tokenizer.nextToken();
									if (tokenizer.ttype == EnhancedStreamTokenizer.TT_WORD ||
											tokenizer.ttype == DBConstants.quoteChar) {
										showAllRows = Boolean.valueOf(tokenizer.sval.equals(String.valueOf(true)));
									} else {
										throw (new DBGParseException(errorString, tokenizer, fileName));
									}
								} else if (tokenizer.sval.equalsIgnoreCase(VDCTSPREADSHEET_GROUPCOLUMNSBYGUIGROUP)) {
									tokenizer.nextToken();
									if (tokenizer.ttype == EnhancedStreamTokenizer.TT_WORD ||
											tokenizer.ttype == DBConstants.quoteChar) {
										groupColumnsByGuiGroup = Boolean.valueOf(tokenizer.sval.equals(String.valueOf(true)));
									} else {
										throw (new DBGParseException(errorString, tokenizer, fileName));
									}
								} else if (tokenizer.sval.equalsIgnoreCase(VDCTSPREADSHEET_BACKGROUNDCOLOR)) {
									tokenizer.nextToken();
									if (tokenizer.ttype == EnhancedStreamTokenizer.TT_NUMBER) {
										backgroundColor = new Integer((int)tokenizer.nval);
									} else {
										throw (new DBGParseException(errorString, tokenizer, fileName));
									}
								} else if (tokenizer.sval.equalsIgnoreCase(VDCTSPREADSHEET_COLUMN)) {
									
									tokenizer.nextToken();
									String columnName = null;
									if (tokenizer.ttype == EnhancedStreamTokenizer.TT_WORD ||
											tokenizer.ttype == DBConstants.quoteChar) {
										columnName = tokenizer.sval;
									} else {
										throw (new DBGParseException(errorString, tokenizer, fileName));
									}
									tokenizer.nextToken();
									boolean hidden = false;
									if (tokenizer.ttype == EnhancedStreamTokenizer.TT_WORD ||
											tokenizer.ttype == DBConstants.quoteChar) {
										hidden = tokenizer.sval.equals(String.valueOf(true));
									} else {
										throw (new DBGParseException(errorString, tokenizer, fileName));
									}

									tokenizer.nextToken();
									int sortIndex = 0;
									if (tokenizer.ttype == EnhancedStreamTokenizer.TT_NUMBER) {
										sortIndex = (int)tokenizer.nval;
									} else {
										throw (new DBGParseException(errorString, tokenizer, fileName));
									}
									
									Vector widthData = new Vector();
									tokenizer.nextToken();
                                    while (tokenizer.ttype == EnhancedStreamTokenizer.TT_WORD
                                    		&& tokenizer.sval.equalsIgnoreCase(VDCTSPREADSHEET_WIDTH)) {

    									tokenizer.nextToken();
    									int splitIndex = 0;
    									if (tokenizer.ttype == EnhancedStreamTokenizer.TT_NUMBER) {
    										splitIndex = (int)tokenizer.nval;
    									} else {
    										throw (new DBGParseException(errorString, tokenizer, fileName));
    									}

    									tokenizer.nextToken();
    									int width = 0;
    									if (tokenizer.ttype == EnhancedStreamTokenizer.TT_NUMBER) {
    										width = (int)tokenizer.nval;
    									} else {
    										throw (new DBGParseException(errorString, tokenizer, fileName));
    									}
    									widthData.add(new DBSheetColWidth(splitIndex, width));
    									tokenizer.nextToken();
                                    }
								
									DBSheetColWidth[] splitPartWidthData = new DBSheetColWidth[widthData.size()];
									widthData.copyInto(splitPartWidthData);
    								columnsMap.put(columnName, new DBSheetColumn(columnName, hidden, sortIndex, splitPartWidthData));
    								
    								continue;

								} else if (tokenizer.sval.equalsIgnoreCase(VDCTSPREADSHEET_ROWORDER)) {
									tokenizer.nextToken();
									String columnName = null;
									if (tokenizer.ttype == EnhancedStreamTokenizer.TT_WORD ||
											tokenizer.ttype == DBConstants.quoteChar) {
										columnName = tokenizer.sval;
									} else {
										throw (new DBGParseException(errorString, tokenizer, fileName));
									}

									tokenizer.nextToken();
									int splitIndex = 0;
									if (tokenizer.ttype == EnhancedStreamTokenizer.TT_NUMBER) {
										splitIndex = (int)tokenizer.nval;
									} else {
										throw (new DBGParseException(errorString, tokenizer, fileName));
									}

									tokenizer.nextToken();
									String ascOrder = null;
									if (tokenizer.ttype == EnhancedStreamTokenizer.TT_WORD ||
											tokenizer.ttype == DBConstants.quoteChar) {
										ascOrder = tokenizer.sval;
									} else {
										throw (new DBGParseException(errorString, tokenizer, fileName));
									}
									rowOrder = new DBSheetRowOrder(columnName, splitIndex, ascOrder);

								} else if (tokenizer.sval.equalsIgnoreCase(VDCTSPREADSHEET_HIDDENROW)) {
									tokenizer.nextToken();
									if (tokenizer.ttype == EnhancedStreamTokenizer.TT_WORD ||
											tokenizer.ttype == DBConstants.quoteChar) {
										hiddenRowsVector.add(tokenizer.sval);
									} else {
										throw (new DBGParseException(errorString, tokenizer, fileName));
									}
								} else if (tokenizer.sval.equalsIgnoreCase(VDCTSPREADSHEET_SPLITCOLUMN)) {
									tokenizer.nextToken();
									String columnName = null;
									if (tokenizer.ttype == EnhancedStreamTokenizer.TT_WORD ||
											tokenizer.ttype == DBConstants.quoteChar) {
										columnName = tokenizer.sval;
									} else {
										throw (new DBGParseException(errorString, tokenizer, fileName));
									}

									tokenizer.nextToken();
									String delimiterTypeString = null;
									if (tokenizer.ttype == EnhancedStreamTokenizer.TT_WORD ||
											tokenizer.ttype == DBConstants.quoteChar) {
										delimiterTypeString = tokenizer.sval;
									} else {
										throw (new DBGParseException(errorString, tokenizer, fileName));
									}

									tokenizer.nextToken();
									String pattern = null;
									if (tokenizer.ttype == EnhancedStreamTokenizer.TT_WORD ||
											tokenizer.ttype == DBConstants.quoteChar) {
										pattern = tokenizer.sval;
									} else {
										throw (new DBGParseException(errorString, tokenizer, fileName));
									}
									splitColumnsVector.add(new DBSheetSplitCol(columnName, delimiterTypeString, pattern));

								} else if (tokenizer.sval.equalsIgnoreCase(VDCTSPREADSHEET_RECENTSPLIT)) {
									tokenizer.nextToken();
									String delimiterTypeString = null;
									if (tokenizer.ttype == EnhancedStreamTokenizer.TT_WORD ||
											tokenizer.ttype == DBConstants.quoteChar) {
										delimiterTypeString = tokenizer.sval;
									} else {
										throw (new DBGParseException(errorString, tokenizer, fileName));
									}

									tokenizer.nextToken();
									String pattern = null;
									if (tokenizer.ttype == EnhancedStreamTokenizer.TT_WORD ||
											tokenizer.ttype == DBConstants.quoteChar) {
										pattern = tokenizer.sval;
									} else {
										throw (new DBGParseException(errorString, tokenizer, fileName));
									}
									recentSplitsVector.add(new DBSheetSplitCol(delimiterTypeString, pattern));

								// For backward compatibility.
								} else if (tokenizer.sval.equalsIgnoreCase(VDCTSPREADSHEET_COL)) {
									tokenizer.nextToken();
									if (tokenizer.ttype == EnhancedStreamTokenizer.TT_WORD ||
											tokenizer.ttype == DBConstants.quoteChar) {
										columnsMap.put(tokenizer.sval, new DBSheetColumn(tokenizer.sval, false, columnPropertyIndex));
									} else {
										throw (new DBGParseException(errorString, tokenizer, fileName));
									}
									columnPropertyIndex++;
								} else if (namelessPropertyIndex == 0) {

									if (tokenizer.ttype == EnhancedStreamTokenizer.TT_WORD ||
											tokenizer.ttype == DBConstants.quoteChar) {
										modeName = tokenizer.sval;
										namelessPropertyIndex++;
									} else {
										throw (new DBGParseException(errorString, tokenizer, fileName));
									}
								} else if (namelessPropertyIndex == 1) {

									if (tokenizer.ttype == EnhancedStreamTokenizer.TT_WORD ||
											tokenizer.ttype == DBConstants.quoteChar) {
										showAllRows = Boolean.valueOf(tokenizer.sval.equals(String.valueOf(true)));
										namelessPropertyIndex++;
									} else {
										throw (new DBGParseException(errorString, tokenizer, fileName));
									}
								}
							// For backward compatibility.
							} else if (namelessPropertyIndex == 2) {

								if (tokenizer.ttype == EnhancedStreamTokenizer.TT_NUMBER) {
									backgroundColor = new Integer((int)tokenizer.nval);
									namelessPropertyIndex++;
								} else {
									throw (new DBGParseException(errorString, tokenizer, fileName));
								}

							} else {
								throw (new DBGParseException(errorString, tokenizer, fileName));
							}

							tokenizer.nextToken();
						}
						
						DBSheetSplitCol[] splitColumns = new DBSheetSplitCol[splitColumnsVector.size()];
						splitColumnsVector.copyInto(splitColumns);
						String[] hiddenRows = new String[hiddenRowsVector.size()];
						hiddenRowsVector.copyInto(hiddenRows);
						DBSheetSplitCol[] recentSplits = new DBSheetSplitCol[recentSplitsVector.size()];
						recentSplitsVector.copyInto(recentSplits);

						DBSheetView viewRecord = new DBSheetView(type, name);
						viewRecord.setModeName(modeName);
						viewRecord.setShowAllRows(showAllRows);
						viewRecord.setGroupColumnsByGuiGroup(groupColumnsByGuiGroup);
						viewRecord.setBackgroundColor(backgroundColor);
						viewRecord.setRowOrder(rowOrder);
						viewRecord.setColumns(columnsMap);
						viewRecord.setSplitColumns(splitColumns);
						viewRecord.setHiddenRows(hiddenRows);
						viewRecord.setRecentSplits(recentSplits);
						DBSheetData.getInstance(dsId).add(viewRecord);

					} else if (tokenizer.sval.equalsIgnoreCase(VDCTSKIP)) {

						// read optional n of lines
						tokenizer.nextToken();
						if (tokenizer.ttype == EnhancedStreamTokenizer.TT_NUMBER) tx=(int)tokenizer.nval;
						else
						{
							tx = 1; 
							tokenizer.pushBack();
						}	

						// skip tx lines (including the rest of this one)
						skipLines(tx+1, tokenizer, fileName);
					}

			/***************************************************/
			/************* Version v1.0 support ****************/
			/***************************************************/

			/****************** layout data ********************/

					else if (tokenizer.sval.equalsIgnoreCase("VDCTRecordPos")) {

						// read record_name
						tokenizer.nextToken();
						if ((tokenizer.ttype == EnhancedStreamTokenizer.TT_WORD)||
								(tokenizer.ttype == DBConstants.quoteChar)) str=tokenizer.sval;
						else throw (new DBGParseException(errorString, tokenizer, fileName));

						rd = (DBRecordData)(data.records.get(str));
						if (rd!=null) {
							// read x pos
							tokenizer.nextToken();
							if (tokenizer.ttype == EnhancedStreamTokenizer.TT_NUMBER) rd.setX((int)tokenizer.nval);
							else throw (new DBGParseException(errorString, tokenizer, fileName));

							// read y pos
							tokenizer.nextToken();
							if (tokenizer.ttype == EnhancedStreamTokenizer.TT_NUMBER) rd.setY((int)tokenizer.nval);
							else throw (new DBGParseException(errorString, tokenizer, fileName));

							// read rotation
							tokenizer.nextToken();
							if (tokenizer.ttype == EnhancedStreamTokenizer.TT_NUMBER) rd.setRotated(((int)tokenizer.nval)!=0);
							else throw (new DBGParseException(errorString, tokenizer, fileName));

							rd.setColor(java.awt.Color.black);
						}
					}

					else if (tokenizer.sval.equalsIgnoreCase("VDCTGroupPos")) {

						// read group_name
						tokenizer.nextToken();
						if ((tokenizer.ttype == EnhancedStreamTokenizer.TT_WORD)||
								(tokenizer.ttype == DBConstants.quoteChar)) str=tokenizer.sval;
						else throw (new DBGParseException(errorString, tokenizer, fileName));

						// read x pos
						tokenizer.nextToken();
						if (tokenizer.ttype == EnhancedStreamTokenizer.TT_NUMBER) tx=(int)tokenizer.nval;
						else throw (new DBGParseException(errorString, tokenizer, fileName));

						// read y pos
						tokenizer.nextToken();
						if (tokenizer.ttype == EnhancedStreamTokenizer.TT_NUMBER) ty=(int)tokenizer.nval;
						else throw (new DBGParseException(errorString, tokenizer, fileName));

						data.addGroup(new DBGroupData(str, tx, ty, java.awt.Color.black, nullString));

					}
					else if (tokenizer.sval.equalsIgnoreCase("VDCTLinkData")) {

						// read linkID
						tokenizer.nextToken();
						if ((tokenizer.ttype == EnhancedStreamTokenizer.TT_WORD)||
								(tokenizer.ttype == DBConstants.quoteChar)) str=tokenizer.sval;
						else throw (new DBGParseException(errorString, tokenizer, fileName));

						// read desc
						tokenizer.nextToken();
						if ((tokenizer.ttype == EnhancedStreamTokenizer.TT_WORD) ||
								(tokenizer.ttype == DBConstants.quoteChar)) desc=tokenizer.sval;
						else throw (new DBGParseException(errorString, tokenizer, fileName));

						// read x pos
						tokenizer.nextToken();
						if (tokenizer.ttype == EnhancedStreamTokenizer.TT_NUMBER) tx=(int)tokenizer.nval;
						else throw (new DBGParseException(errorString, tokenizer, fileName));

						// read x2 pos
						tokenizer.nextToken();
						if (tokenizer.ttype == EnhancedStreamTokenizer.TT_NUMBER) tx2=(int)tokenizer.nval;
						else throw (new DBGParseException(errorString, tokenizer, fileName));

						// read obj1rotated
						tokenizer.nextToken();
						/*					if (tokenizer.ttype == EnhancedStreamTokenizer.TT_NUMBER) r1=((int)tokenizer.nval)!=0;
					else throw (new DBGParseException(errorString, tokenizer, fileName));
						 */
						// read obj2rotated
						tokenizer.nextToken();
						/*					if (tokenizer.ttype == EnhancedStreamTokenizer.TT_NUMBER) r2=((int)tokenizer.nval)!=0;
					else throw (new DBGParseException(errorString, tokenizer, fileName));
						 */
						/*
					// transformation to v2 visual data

					int pos = str.lastIndexOf('/');
					if (pos<0) continue;	// invalid, skip
					StringBuffer fieldName = new StringBuffer(str);
					fieldName.setCharAt(pos, '.');

					pos = fieldName.toString().lastIndexOf(com.cosylab.vdct.Constants.FIELD_SEPARATOR);
					String field = fieldName.substring(pos+1);
					String record = fieldName.substring(0, pos);

					rd = (DBRecordData)data.getRecords().get(record);
					if (rd==null) continue;

					fd = (DBFieldData)rd.getFields().get(field);
					if (fd==null) continue;

					String target = com.cosylab.vdct.vdb.LinkProperties.getTargetFromString(fd.getValue());
					if (target==null) continue;


					if (!com.cosylab.vdct.graphics.objects.Group.substractParentName(fieldName.toString()).equals(com.cosylab.vdct.graphics.objects.Group.substractParentName(target)))
					{
						// intergroup link, no connector needed
						data.addLink(new DBLinkData(fieldName.toString(), target));
					}
					else
					{
						String connectorName = com.cosylab.vdct.graphics.objects.Group.substractObjectName(str);
						data.addLink(new DBLinkData(fieldName.toString(), connectorName));

						int y = rd.getX()+com.cosylab.vdct.Constants.RECORD_HEIGHT;
						pos = target.lastIndexOf(com.cosylab.vdct.Constants.FIELD_SEPARATOR);
						record = target.substring(0, pos);
						rd = (DBRecordData)data.getRecords().get(record);
						if (rd!=null)
							y = (y+rd.getX()+com.cosylab.vdct.Constants.RECORD_HEIGHT)/2;

						data.addConnector(new DBConnectorData(connectorName, target, tx, y, java.awt.Color.black, nullString));
					}
					fd.setRotated(r1);
						 */
					}

			return nullString;
		}
	}

	/**
	 * VisualDCT layout data is also processed here
     * @param template
	 * @param tokenizer java.io.EnhancedStreamTokenizer
     * @param fileName
     * @return 
     * @throws java.lang.Exception
	 */
	public static String processTemplateComment(DBTemplate template, EnhancedStreamTokenizer tokenizer, String fileName) throws Exception {

		if ((template==null) || !tokenizer.sval.equals(DBConstants.layoutDataString)) {	// comment
			String comment = tokenizer.sval;

			// initialize tokenizer for comments
			tokenizer.resetSyntax();
			tokenizer.whitespaceChars(0, 31);
			tokenizer.wordChars(32, 255);		
			tokenizer.wordChars('\t', '\t');
			tokenizer.eolIsSignificant(true);

			while ((tokenizer.nextToken() != EnhancedStreamTokenizer.TT_EOL) &&						// read till EOL
					(tokenizer.ttype != EnhancedStreamTokenizer.TT_EOF))
				if (tokenizer.ttype == EnhancedStreamTokenizer.TT_NUMBER) {
					//if (!comment.equals(nullString)) comment+=SPACE;
					comment=comment+tokenizer.nval;
				}
				else {
					//if (!comment.equals(nullString)) comment+=SPACE;
					comment=comment+tokenizer.sval;
				}

			// reinitialzie it back
			initializeTokenizer(tokenizer);

			return comment+NL;

		}
		else {																		// graphics layout data

			String str;

			while ((tokenizer.nextToken() != EnhancedStreamTokenizer.TT_EOL) &&
					(tokenizer.ttype != EnhancedStreamTokenizer.TT_EOF)) 
				if (tokenizer.ttype == EnhancedStreamTokenizer.TT_WORD) 

					if (tokenizer.sval.equalsIgnoreCase(VDCT_CONSTANT_PORT) ||
							tokenizer.sval.equalsIgnoreCase(VDCT_INPUT_PORT) ||
							tokenizer.sval.equalsIgnoreCase(VDCT_OUTPUT_PORT)) {

						int mode;
						if (tokenizer.sval.equalsIgnoreCase(VDCT_CONSTANT_PORT))
							mode = OutLink.CONSTANT_PORT_MODE;
						else if (tokenizer.sval.equalsIgnoreCase(VDCT_INPUT_PORT))
							mode = OutLink.INPUT_PORT_MODE;
						else /*if (tokenizer.sval.equalsIgnoreCase(VDCT_OUTPUT_PORT))*/
							mode = OutLink.OUTPUT_PORT_MODE;

						// read port name
						tokenizer.nextToken();
						if ((tokenizer.ttype == EnhancedStreamTokenizer.TT_WORD)||
								(tokenizer.ttype == DBConstants.quoteChar)) str=tokenizer.sval;
						else throw (new DBGParseException(errorString, tokenizer, fileName));

						DBPort port = (DBPort)(template.getPorts().get(str));
						if (port!=null)
						{
							port.setMode(mode);

							// read port inlink
							tokenizer.nextToken();
							if ((tokenizer.ttype == EnhancedStreamTokenizer.TT_WORD)||
									(tokenizer.ttype == DBConstants.quoteChar)) port.setInLinkID(tokenizer.sval);
							else throw (new DBGParseException(errorString, tokenizer, fileName));

							// read x pos
							tokenizer.nextToken();
							if (tokenizer.ttype == EnhancedStreamTokenizer.TT_NUMBER) port.setX((int)tokenizer.nval);
							else throw (new DBGParseException(errorString, tokenizer, fileName));

							// read y pos
							tokenizer.nextToken();
							if (tokenizer.ttype == EnhancedStreamTokenizer.TT_NUMBER) port.setY((int)tokenizer.nval);
							else throw (new DBGParseException(errorString, tokenizer, fileName));

							// read color
							tokenizer.nextToken();
							if (tokenizer.ttype == EnhancedStreamTokenizer.TT_NUMBER) port.setColor(StringUtils.int2color((int)tokenizer.nval));
							else throw (new DBGParseException(errorString, tokenizer, fileName));

							// read defaultVisibility
							tokenizer.nextToken();
							if (tokenizer.ttype == EnhancedStreamTokenizer.TT_NUMBER) port.setDefaultVisibility((int)tokenizer.nval);
							else throw (new DBGParseException(errorString, tokenizer, fileName));

							// read macro text position if exists
							tokenizer.nextToken();
							if (tokenizer.ttype == EnhancedStreamTokenizer.TT_EOL) {
								tokenizer.pushBack();
								port.setNamePositionNorth(true);
							} else if (tokenizer.ttype == EnhancedStreamTokenizer.TT_WORD){
								port.setNamePositionNorth(new Boolean(tokenizer.sval).booleanValue());
							} else throw (new DBGParseException(errorString, tokenizer, fileName));


							port.setHasVisual(true);
						}
					}

					else if (tokenizer.sval.equalsIgnoreCase(VDCT_INPUT_MACRO) ||
							tokenizer.sval.equalsIgnoreCase(VDCT_OUTPUT_MACRO)) {

						int mode;
						if (tokenizer.sval.equalsIgnoreCase(VDCT_INPUT_MACRO))
							mode = InLink.INPUT_MACRO_MODE;
						else /*if (tokenizer.sval.equalsIgnoreCase(VDCT_OUTPUT_MACRO))*/
							mode = InLink.OUTPUT_MACRO_MODE;

						// read port name
						tokenizer.nextToken();
						if ((tokenizer.ttype == EnhancedStreamTokenizer.TT_WORD)||
								(tokenizer.ttype == DBConstants.quoteChar)) str=tokenizer.sval;
						else throw (new DBGParseException(errorString, tokenizer, fileName));

						DBMacro macro = (DBMacro)(template.getMacros().get(str));
						if (macro==null)
						{
							macro = new DBMacro(str);
							// macro.setComment(comment); comment = nullString;

							template.addMacro(macro);

							macro.setMode(mode);

							// read port description
							tokenizer.nextToken();
							if ((tokenizer.ttype == EnhancedStreamTokenizer.TT_WORD)||
									(tokenizer.ttype == DBConstants.quoteChar)) macro.setDescription(tokenizer.sval);
							else throw (new DBGParseException(errorString, tokenizer, fileName));

							// read x pos
							tokenizer.nextToken();
							if (tokenizer.ttype == EnhancedStreamTokenizer.TT_NUMBER) macro.setX((int)tokenizer.nval);
							else throw (new DBGParseException(errorString, tokenizer, fileName));

							// read y pos
							tokenizer.nextToken();
							if (tokenizer.ttype == EnhancedStreamTokenizer.TT_NUMBER) macro.setY((int)tokenizer.nval);
							else throw (new DBGParseException(errorString, tokenizer, fileName));

							// read color
							tokenizer.nextToken();
							if (tokenizer.ttype == EnhancedStreamTokenizer.TT_NUMBER) macro.setColor(StringUtils.int2color((int)tokenizer.nval));
							else throw (new DBGParseException(errorString, tokenizer, fileName));

							// read defaultVisibility
							tokenizer.nextToken();
							if (tokenizer.ttype == EnhancedStreamTokenizer.TT_NUMBER) macro.setDefaultVisibility((int)tokenizer.nval);
							else throw (new DBGParseException(errorString, tokenizer, fileName));

							// read macro text position if exists
							tokenizer.nextToken();
							if (tokenizer.ttype == EnhancedStreamTokenizer.TT_EOL) {
								tokenizer.pushBack();
								macro.setNamePositionNorth(true);
							} else if (tokenizer.ttype == EnhancedStreamTokenizer.TT_WORD){
								macro.setNamePositionNorth(new Boolean(tokenizer.sval).booleanValue());
							} else throw (new DBGParseException(errorString, tokenizer, fileName));

							macro.setHasVisual(true);

						}
					}
			return nullString;
		}
	}



	/**
	 * VisualDCT layout data is also processed here
     * @param linesToSkip
	 * @param tokenizer java.io.EnhancedStreamTokenizer
     * @param fileName
     * @throws java.lang.Exception
	 */
	public static void skipLines(int linesToSkip, EnhancedStreamTokenizer tokenizer, String fileName) throws Exception {

		int lines = 0;
		while (lines < linesToSkip)
		{
			tokenizer.nextToken();

			// end of file
			if (tokenizer.ttype == EnhancedStreamTokenizer.TT_EOF)		
				return;

			else if (tokenizer.ttype == EnhancedStreamTokenizer.TT_EOL)
				lines++;
		}
	}
	
    /**
     *
     * @param dsId
     * @param data
     * @param vdctData
     * @param source
     */
    public static void readVdctData(Object dsId, DBData data, String vdctData, String source) {
		StringReader reader = new StringReader(vdctData);
		EnhancedStreamTokenizer tokenizer = new EnhancedStreamTokenizer(reader);
		initializeTokenizer(tokenizer);
		try {
			while (tokenizer.nextToken() != EnhancedStreamTokenizer.TT_EOF) {
				if ((tokenizer.ttype == EnhancedStreamTokenizer.TT_WORD)
						&& (tokenizer.sval.startsWith(DBConstants.commentString))) {
					processComment(dsId, data, tokenizer, source);
				}
			}
		} catch (Exception exception) {
			exception.printStackTrace();
		}
	}

	/**
	 * This method was created in VisualAge.
     * @param dsId
	 * @param data com.cosylab.vdct.db.DBData
	 * @param tokenizer java.io.EnhancedStreamTokenizer
     * @param loadStack
     * @param paths
     * @param fileName
     * @param loadList
     * @throws java.lang.Exception
	 */
	public static void processDB(Object dsId, DBData data, EnhancedStreamTokenizer tokenizer, String fileName,
			PathSpecification paths, Stack loadStack, ArrayList loadList) throws Exception
			{

		String comment = nullString;
		String str, str2;
		String include_filename;
		EnhancedStreamTokenizer inctokenizer = null;

		if (data!=null)
		{
			if (loadStack.contains(fileName))
			{
				StringBuffer buf = new StringBuffer();
				buf.append("Cyclic reference detected when trying to load '");
				buf.append(fileName);
				buf.append("'.\nLoad stack trace:\n");
				for (int i = loadStack.size()-1; i>=0; i--)
				{
					buf.append("\t");
					buf.append(loadStack.elementAt(i));
					buf.append("\n");
				}
				throw new DBException(buf.toString());
			}
			else
			{
				loadStack.push(fileName);
			}


			try {

				while (tokenizer.nextToken() != EnhancedStreamTokenizer.TT_EOF)
					if (tokenizer.ttype == EnhancedStreamTokenizer.TT_WORD)
						if (tokenizer.sval.startsWith(DBConstants.commentString))
							comment+=processComment(dsId, data, tokenizer, fileName);
						else

							/****************** records ********************/

							if (tokenizer.sval.equalsIgnoreCase(RECORD) ||
									tokenizer.sval.equalsIgnoreCase(GRECORD)) {

								DBRecordData rd = new DBRecordData();

								// read record_type
								tokenizer.nextToken();
								if ((tokenizer.ttype == EnhancedStreamTokenizer.TT_WORD) ||
										(tokenizer.ttype == DBConstants.quoteChar)) rd.setRecord_type(tokenizer.sval);
								else throw (new DBParseException("Invalid record type...", tokenizer, fileName));

								// read record_name
								tokenizer.nextToken();
								if ((tokenizer.ttype == EnhancedStreamTokenizer.TT_WORD) ||
										(tokenizer.ttype == DBConstants.quoteChar)) rd.setName(tokenizer.sval);
								else throw (new DBParseException("Invalid record name...", tokenizer, fileName));

								rd.setComment(comment);	comment = nullString;

								processFields(dsId, rd, tokenizer, fileName, paths);
								data.addRecord(rd);

							}

				/****************** templates ********************/

							else if (tokenizer.sval.equalsIgnoreCase(TEMPLATE)) {

								// read optional description
								str = null;
								tokenizer.nextToken();
								if (tokenizer.ttype == DBConstants.quoteChar) str = tokenizer.sval;
								else
									tokenizer.pushBack();

								/*
						if (str==null)
							System.out.println("template()\n{");
						else
							System.out.println("template(\""+str+"\")\n{");
								 */

								// multiple tempaltes
								// only new ports are added
								DBTemplate templateData = data.getTemplateData();
								if (!templateData.isInitialized())
								{
									templateData.setInitialized(true);
									templateData.setComment(comment); comment = nullString;
									templateData.setDescription(str);

									DBTemplateEntry entry = new DBTemplateEntry();
									data.addEntry(entry);

								}
								else
								{
									// !!! TBD multiple templates support
									comment = nullString;
								}

								processPorts(templateData, tokenizer, fileName, paths);

								//System.out.println("}");

							}

				/****************** expands ********************/

							else if (tokenizer.sval.equalsIgnoreCase(EXPAND)) {

								// read template file
								tokenizer.nextToken();
								if (tokenizer.ttype == DBConstants.quoteChar) str = tokenizer.sval;
								else throw (new DBParseException("Invalid expand file...", tokenizer, fileName));

								// read tempalte instance id
								tokenizer.nextToken();
								if ((tokenizer.ttype == EnhancedStreamTokenizer.TT_WORD) ||
										(tokenizer.ttype == DBConstants.quoteChar)) str2 = tokenizer.sval;
								else throw (new DBParseException("Invalid expand template instance name...", tokenizer, fileName));

								String loadedTemplateId = loadTemplate(dsId, data, str, fileName, paths, loadStack, loadList);

								//System.out.println("expand(\""+str+"\", "+str2+")\n{");

								DBTemplateInstance ti = new DBTemplateInstance(str2, loadedTemplateId);
								ti.setComment(comment);	comment = nullString;

								processMacros(dsId, ti, tokenizer, fileName, paths);

								data.addTemplateInstance(ti);

								//System.out.println("}");
							}

				/****************** includes ********************/

							else if (tokenizer.sval.equalsIgnoreCase(INCLUDE)) {

								// read incude_filename
								tokenizer.nextToken();
								if (tokenizer.ttype == DBConstants.quoteChar) include_filename=tokenizer.sval;
								else throw (new DBParseException("Invalid include filename...", tokenizer, fileName));

								DBDataEntry entry = new DBDataEntry(INCLUDE+" \""+include_filename+"\"");
								entry.setComment(comment);	comment = nullString;
								data.addEntry(entry);

								File file = paths.search4File(include_filename);
								inctokenizer = getEnhancedStreamTokenizer(file.getAbsolutePath());
								if (inctokenizer!=null) processDB(dsId, data, inctokenizer, include_filename, new PathSpecification(file.getParentFile().getAbsolutePath(), paths), loadStack, loadList);
							}

				/****************** path ********************/

							else if (tokenizer.sval.equalsIgnoreCase(PATH))
							{
								// read paths
								tokenizer.nextToken();
								if (tokenizer.ttype == DBConstants.quoteChar) str=tokenizer.sval;
								else throw (new DBParseException("Invalid path...", tokenizer, fileName));

								DBDataEntry entry = new DBDataEntry(PATH+" \""+str+"\"");
								entry.setComment(comment);	comment = nullString;
								data.addEntry(entry);

								paths.setPath(str);
								//Console.getInstance().println("Warning: 'path' command is not yet supported...");
							}

				/****************** addpath ********************/

							else if (tokenizer.sval.equalsIgnoreCase(ADDPATH))
							{
								// read add paths
								tokenizer.nextToken();
								if (tokenizer.ttype == DBConstants.quoteChar) str=tokenizer.sval;
								else throw (new DBParseException("Invalid addpath...", tokenizer, fileName));

								DBDataEntry entry = new DBDataEntry(ADDPATH+" \""+str+"\"");
								entry.setComment(comment);	comment = nullString;
								data.addEntry(entry);

								paths.addAddPath(str);
								//Console.getInstance().println("Warning: 'addpath' command is not yet supported...");
							}

			} catch (Exception e) {
				//Console.getInstance().println("\n"+e);
				throw e;
			}
			finally
			{
				loadStack.pop();
			}
		}	
			}

	/**
	 * This method was created in VisualAge.
     * @param dsId
     * @param templateInstance
	 * @param tokenizer java.io.EnhancedStreamTokenizer
     * @param fileName
     * @param paths
	 * @exception java.lang.Exception The exception description.
	 */
	public static void processMacros(Object dsId, DBTemplateInstance templateInstance, EnhancedStreamTokenizer tokenizer, String fileName, PathSpecification paths) throws Exception {

		String name;
		String value;
		String include_filename;
		EnhancedStreamTokenizer inctokenizer = null;

		if (templateInstance!=null)

			/********************** macros area *************************/

			while (tokenizer.nextToken() != EnhancedStreamTokenizer.TT_EOF) 
				if (tokenizer.ttype == EnhancedStreamTokenizer.TT_WORD) 
					if (tokenizer.sval.equals(ENDSTR)) break;
					else if (tokenizer.sval.startsWith(DBConstants.commentString)) 
						processComment(dsId, null, tokenizer, fileName);				// !!! no comments are preserved in macro part
					else if (tokenizer.sval.equalsIgnoreCase(MACRO)) {

						// read name
						tokenizer.nextToken();
						if ((tokenizer.ttype == EnhancedStreamTokenizer.TT_WORD) ||
								(tokenizer.ttype == DBConstants.quoteChar)) name=tokenizer.sval;
						else throw (new DBParseException("Invalid macro name...", tokenizer, fileName));

						// read value
						tokenizer.nextToken();
						if (tokenizer.ttype == DBConstants.quoteChar) value=tokenizer.sval;
						else throw (new DBParseException("Invalid macro value...", tokenizer, fileName));

						//System.out.println("\tmacro("+name+", \""+value+"\")");

						templateInstance.addProperty(name, value);
					}

					else if (tokenizer.sval.equalsIgnoreCase(INCLUDE)) {

						// read incude_filename
						tokenizer.nextToken();
						if (tokenizer.ttype == DBConstants.quoteChar) include_filename=tokenizer.sval;
						else throw (new DBParseException("Invalid include filename...", tokenizer, fileName));

						File file = paths.search4File(include_filename);
						inctokenizer = getEnhancedStreamTokenizer(file.getAbsolutePath());
						if (inctokenizer!=null) processMacros(dsId, templateInstance, inctokenizer, include_filename, new PathSpecification(file.getParentFile().getAbsolutePath(), paths));

					}	

		/***********************************************************/

	}

	/**
	 * This method was created in VisualAge.
     * @param template
	 * @param tokenizer java.io.EnhancedStreamTokenizer
     * @param fileName
     * @param paths
	 * @exception java.lang.Exception The exception description.
	 */
	public static void processPorts(DBTemplate template, EnhancedStreamTokenizer tokenizer, String fileName, PathSpecification paths) throws Exception {

		String name;
		String value;
		String description;
		String include_filename;
		String comment = nullString;
		EnhancedStreamTokenizer inctokenizer = null;

		if (template!=null)

			/********************** ports area *************************/

			while (tokenizer.nextToken() != EnhancedStreamTokenizer.TT_EOF) 
				if (tokenizer.ttype == EnhancedStreamTokenizer.TT_WORD) 
					if (tokenizer.sval.equals(ENDSTR)) break;
					else if (tokenizer.sval.startsWith(DBConstants.commentString)) 
						comment+=processTemplateComment(template, tokenizer, fileName);
					else if (tokenizer.sval.equalsIgnoreCase(PORT)) {

						// read name
						tokenizer.nextToken();
						if ((tokenizer.ttype == EnhancedStreamTokenizer.TT_WORD) ||
								(tokenizer.ttype == DBConstants.quoteChar)) name=tokenizer.sval;
						else throw (new DBParseException("Invalid port name...", tokenizer, fileName));

						// read field
						tokenizer.nextToken();
						if ((tokenizer.ttype == EnhancedStreamTokenizer.TT_WORD) ||
								(tokenizer.ttype == DBConstants.quoteChar)) value=tokenizer.sval;
						else throw (new DBParseException("Invalid port value...", tokenizer, fileName));

						// read optional description
						description = null;
						tokenizer.nextToken();
						if (tokenizer.ttype == DBConstants.quoteChar) description=tokenizer.sval;
						else
							tokenizer.pushBack();

						//System.out.println("\tport("+name+", \""+value+"\", \""+description+"\")");

						DBPort port = new DBPort(name, value);
						port.setComment(comment); comment = nullString;
						port.setDescription(description);

						template.addPort(port);
					}

					else if (tokenizer.sval.equalsIgnoreCase(INCLUDE)) {

						// read incude_filename
						tokenizer.nextToken();
						if (tokenizer.ttype == DBConstants.quoteChar) include_filename=tokenizer.sval;
						else throw (new DBParseException("Invalid include filename...", tokenizer, fileName));

						File file = paths.search4File(include_filename);
						inctokenizer = getEnhancedStreamTokenizer(file.getAbsolutePath());
						if (inctokenizer!=null) processPorts(template, inctokenizer, include_filename, new PathSpecification(file.getParentFile().getAbsolutePath(), paths));

					}	

		/***********************************************************/

	}



	/**
	 * This method was created in VisualAge.
     * @param dsId
	 * @param rd com.cosylab.vdct.db.DBRecordData
	 * @param tokenizer java.io.EnhancedStreamTokenizer
     * @param fileName
     * @param paths
	 * @exception java.lang.Exception The exception description.
	 */
	public static void processFields(Object dsId, DBRecordData rd, EnhancedStreamTokenizer tokenizer, String fileName, PathSpecification paths) throws Exception {

		String name;
		String value;
		String comment = nullString;
		String include_filename;
		EnhancedStreamTokenizer inctokenizer = null;

		if (rd!=null)

			/********************** fields area *************************/

			while (tokenizer.nextToken() != EnhancedStreamTokenizer.TT_EOF) 
				if (tokenizer.ttype == EnhancedStreamTokenizer.TT_WORD) 
					if (tokenizer.sval.equals(ENDSTR)) break;
					else if (tokenizer.sval.startsWith(DBConstants.commentString)) 
						comment+=processComment(dsId, null, tokenizer, fileName);
					else if (tokenizer.sval.equalsIgnoreCase(FIELD)) {

						// read field_name
						tokenizer.nextToken();
						if ((tokenizer.ttype == EnhancedStreamTokenizer.TT_WORD) ||
								(tokenizer.ttype == DBConstants.quoteChar)) name=tokenizer.sval;
						else throw (new DBParseException("Invalid field name...", tokenizer, fileName));

						// read field_value
						tokenizer.nextToken();
						if (tokenizer.ttype == DBConstants.quoteChar) value=tokenizer.sval;
						else throw (new DBParseException("Invalid field value...", tokenizer, fileName));

						DBFieldData fd = new DBFieldData(name, value);
						fd.setComment(comment);	comment = nullString;
						rd.addField(fd);
					}

					else if (tokenizer.sval.equalsIgnoreCase(INFO)) {

						// read info_name
						tokenizer.nextToken();
						if ((tokenizer.ttype == EnhancedStreamTokenizer.TT_WORD) ||
								(tokenizer.ttype == DBConstants.quoteChar)) name=tokenizer.sval;
						else throw (new DBParseException("Invalid info field name...", tokenizer, fileName));

						// read info_value
						tokenizer.nextToken();
						if (tokenizer.ttype == DBConstants.quoteChar) value=tokenizer.sval;
						else throw (new DBParseException("Invalid info field value...", tokenizer, fileName));

						DBFieldData fd = new DBFieldData(name, value);
						fd.setComment(comment);	comment = nullString;
						rd.addInfoField(fd);
					}

					else if (tokenizer.sval.equalsIgnoreCase(INCLUDE)) {

						// read incude_filename
						tokenizer.nextToken();
						if (tokenizer.ttype == DBConstants.quoteChar) include_filename=tokenizer.sval;
						else throw (new DBParseException("Invalid include filename...", tokenizer, fileName));

						File file = paths.search4File(include_filename);
						inctokenizer = getEnhancedStreamTokenizer(file.getAbsolutePath());
						if (inctokenizer!=null) processFields(dsId, rd, inctokenizer, include_filename, new PathSpecification(file.getParentFile().getAbsolutePath(), paths));

					}	

		/***********************************************************/

	}



	/**
	 * This method was created in VisualAge.
	 * @return Vector
	 * @param fileName java.lang.String
     * @throws java.io.IOException
	 */
	public static String[] resolveIncodedDBDs(String fileName) throws IOException {

		EnhancedStreamTokenizer tokenizer = getEnhancedStreamTokenizer(fileName);
		if (tokenizer==null) return null;

		String[] dbds = null;
		Vector vec = null;

		while (tokenizer.nextToken() != EnhancedStreamTokenizer.TT_EOF)
			if (tokenizer.ttype == EnhancedStreamTokenizer.TT_WORD)
				if (tokenizer.sval.startsWith(DBConstants.layoutDataString))
				{

					while ((tokenizer.nextToken() != EnhancedStreamTokenizer.TT_EOL) &&
							(tokenizer.ttype != EnhancedStreamTokenizer.TT_EOF)) 
						if (tokenizer.ttype == EnhancedStreamTokenizer.TT_WORD) 
							if (tokenizer.sval.equalsIgnoreCase(DBD_START_STR))
								vec = new Vector();
							else if (tokenizer.sval.equalsIgnoreCase(DBD_ENTRY_STR))
							{
								// check for DBD_START_STR 
								if (vec==null)
								{
									vec = new Vector();
									Console.getInstance().println("Warning: error found in file '"+fileName+"', line "+tokenizer.lineno()+" near token '"+tokenizer.sval+"':\n\t'"+DBD_ENTRY_STR+"' before '"+DBD_END_STR+"'...");
								}

								// read DBD filename
								tokenizer.nextToken();
								if ((tokenizer.ttype == EnhancedStreamTokenizer.TT_WORD)||
										(tokenizer.ttype == DBConstants.quoteChar)) vec.addElement(tokenizer.sval);
								else
									Console.getInstance().println("Warning: error found in file '"+fileName+"', line "+tokenizer.lineno()+" near token '"+tokenizer.sval+"':\n\tinvalid '"+DBD_ENTRY_STR+"' entry. Quoted DBD filename expected...");
							}
							else if (tokenizer.sval.equalsIgnoreCase(DBD_END_STR))
								break;
				}

		if (vec!=null)
		{
			dbds = new String[vec.size()];
			vec.toArray(dbds);
		}	 
		return dbds;
	}

	/**
	 * This method was created in VisualAge.
     * @param dsId
     * @param loadList
     * @param loadStack
	 * @return Vector
	 * @param fileName java.lang.String
	 */
	public static DBData resolveDB(Object dsId, String fileName, Stack loadStack, ArrayList loadList) {

		DBData data = null; 

		EnhancedStreamTokenizer tokenizer = getEnhancedStreamTokenizer(fileName);
		if (tokenizer!=null) 
		{
			try
			{
				// generate template id from fileName
				File file = new File(fileName);

				PathSpecification paths = new PathSpecification(file.getParentFile().getAbsolutePath());
				data = new DBData(file.getName(), file.getAbsolutePath());
				data.getTemplateData().setModificationTime(file.lastModified());

				processDB(dsId, data, tokenizer, fileName, paths, loadStack, loadList);
			}
			catch (Exception e)
			{
				Console.getInstance().println(e.toString());
				data = null;
			}
			finally
			{
				System.gc();
			}
		}

		return data;
	}

	/**
	 * This method was created in VisualAge.
     * @param dsId
     * @param is
     * @param loadList
     * @param loadStack
	 * @return Vector
	 */
	public static DBData resolveDB(Object dsId, InputStream is, Stack loadStack, ArrayList loadList) {

		DBData data = null; 

		EnhancedStreamTokenizer tokenizer = getEnhancedStreamTokenizer(is);
		if (tokenizer!=null) 
		{
			try
			{
				PathSpecification paths = new PathSpecification(Settings.getDefaultDir());
				data = new DBData("System Clipboard", "System Clipboard");

				processDB(dsId, data, tokenizer, "System Clipboard", paths, loadStack, loadList);
			}
			catch (Exception e)
			{
				Console.getInstance().println(e.toString());
				data = null;
			}
			finally
			{
				System.gc();
			}
		}

		return data;
	}

	/**
	 * This method was created in VisualAge.
     * @param dsId
	 * @return Vector
	 * @param fileName java.lang.String
     * @throws java.lang.Exception
	 */
	public static DBData resolveDB(Object dsId, String fileName) throws Exception {
		Stack loadStack = new Stack();
		ArrayList loadList = new ArrayList();
		return resolveDB(dsId, fileName, loadStack, loadList);
	}

	/**
	 * This method was created in VisualAge.
     * @param dsId
     * @param is
	 * @return Vector
     * @throws java.lang.Exception
	 */
	public static DBData resolveDB(Object dsId, InputStream is) throws Exception {
		Stack loadStack = new Stack();
		ArrayList loadList = new ArrayList();
		return resolveDB(dsId, is, loadStack, loadList);
	}


	/**
	 * This method was created in VisualAge.
     * @param url
	 * @return Vector
     * @throws java.lang.Exception
	 */
	public static DBData resolveDBasURL(java.net.URL url) throws Exception {

		DBData data = null;

		InputStream fi = null;
		EnhancedStreamTokenizer tokenizer = null;	

		try {
			fi = url.openStream();
			tokenizer = new EnhancedStreamTokenizer(new BufferedReader(new InputStreamReader(fi)));
			initializeTokenizer(tokenizer);
		} catch (Exception e) {
			Console.getInstance().println("\nError occured while opening URL '"+url.toString()+"'");
			Console.getInstance().println(e);
			return null;
		}

		if (tokenizer!=null) 
		{
			try
			{
				// generate template id from fileName
				//File file = new File(fileName);
				//data = new DBData(file.getName(), file.getAbsolutePath());

				// !!! TDB not supported yet 

				//processDB(data, tokenizer, url.toString(), null);
			}
			finally
			{
				System.gc();
			}
		}

		return data;
	}

}
