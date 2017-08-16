package com.cosylab.vdct;

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

import java.awt.BorderLayout;
import java.awt.Component;
import java.awt.Dimension;
import java.awt.Event;
import java.awt.Toolkit;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyEvent;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.awt.event.WindowEvent;
import java.awt.print.PageFormat;
import java.awt.print.Paper;
import java.awt.print.PrinterException;
import java.awt.print.PrinterJob;
import java.io.ByteArrayInputStream;
import java.io.ByteArrayOutputStream;
import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.util.Enumeration;
import java.util.Hashtable;
import java.util.Vector;

import javax.print.Doc;
import javax.print.DocFlavor;
import javax.print.DocPrintJob;
import javax.print.PrintService;
import javax.print.SimpleDoc;
import javax.print.StreamPrintService;
import javax.print.StreamPrintServiceFactory;
import javax.print.attribute.DocAttributeSet;
import javax.print.attribute.HashDocAttributeSet;
import javax.print.attribute.PrintRequestAttributeSet;
import javax.print.attribute.standard.Media;
import javax.print.attribute.standard.MediaPrintableArea;
import javax.print.attribute.standard.MediaSize;
import javax.print.attribute.standard.MediaSizeName;
import javax.print.attribute.standard.OrientationRequested;
import javax.swing.AbstractAction;
import javax.swing.ComboBoxEditor;
import javax.swing.ComboBoxModel;
import javax.swing.InputMap;
import javax.swing.JButton;
import javax.swing.JCheckBoxMenuItem;
import javax.swing.JComboBox;
import javax.swing.JComponent;
import javax.swing.JDialog;
import javax.swing.JFileChooser;
import javax.swing.JFrame;
import javax.swing.JLabel;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JSeparator;
import javax.swing.JSlider;
import javax.swing.JTextField;
import javax.swing.JToolBar;
import javax.swing.KeyStroke;
import javax.swing.SwingConstants;
import javax.swing.SwingUtilities;
import javax.swing.UIManager;

import com.cosylab.vdct.about.VisualDCTAboutDialogEngine;
import com.cosylab.vdct.events.CommandManager;
import com.cosylab.vdct.events.KeyEventManager;
import com.cosylab.vdct.events.commands.GetDsManager;
import com.cosylab.vdct.events.commands.GetGUIInterface;
import com.cosylab.vdct.events.commands.GetMainComponent;
import com.cosylab.vdct.events.commands.GetPrintableInterface;
import com.cosylab.vdct.events.commands.GetVDBManager;
import com.cosylab.vdct.events.commands.NullCommand;
import com.cosylab.vdct.events.commands.SaveCommand;
import com.cosylab.vdct.events.commands.SetCursorCommand;
import com.cosylab.vdct.events.commands.SetDefaultFocus;
import com.cosylab.vdct.events.commands.SetRedoMenuItemState;
import com.cosylab.vdct.events.commands.SetUndoMenuItemState;
import com.cosylab.vdct.events.commands.SetWorkspaceFile;
import com.cosylab.vdct.events.commands.SetWorkspaceGroup;
import com.cosylab.vdct.events.commands.SetWorkspaceScale;
import com.cosylab.vdct.events.commands.ShowMorphingDialog;
import com.cosylab.vdct.events.commands.ShowNewDialog;
import com.cosylab.vdct.events.commands.ShowRenameDialog;
import com.cosylab.vdct.events.commands.UpdateLoadLabel;
import com.cosylab.vdct.find.FindDialog;
import com.cosylab.vdct.graphics.DrawingSurfaceInterface;
import com.cosylab.vdct.graphics.DsEventListener;
import com.cosylab.vdct.graphics.DsManager;
import com.cosylab.vdct.graphics.DsManagerInterface;
import com.cosylab.vdct.graphics.VDBInterface;
import com.cosylab.vdct.graphics.ViewState;
import com.cosylab.vdct.graphics.WorkspaceDesktop;
import com.cosylab.vdct.graphics.objects.Group;
import com.cosylab.vdct.graphics.printing.Page;
import com.cosylab.vdct.graphics.printing.PrintPreview;
import com.cosylab.vdct.inspector.sheet.SpreadsheetInspector;
import com.cosylab.vdct.plugin.config.PluginNameConfigManager;
import com.cosylab.vdct.rdb.RdbInstance;
import com.cosylab.vdct.util.ComboBoxFileChooser;
import com.cosylab.vdct.util.DBDEntry;
import com.cosylab.vdct.util.UniversalFileFilter;
import com.cosylab.vdct.vdb.VDBData;
import com.cosylab.vdct.vdb.VDBTemplate;

/**
 * This type was generated by a SmartGuide.
 */
public class VisualDCT extends JFrame implements DsEventListener {
	private JMenuItem ivjAbout_BoxMenuItem = null;
	private JMenuItem ivjBase_ViewMenuItem = null;
	private JMenuItem ivjBooks_OnlineMenuItem = null;
	private JMenuItem ivjSystemCopyMenuItem = null;
	private JMenuItem ivjSystemPasteMenuItem = null;
	private JButton ivjCopyButton = null;
	private JMenuItem ivjCopyMenuItem = null;
	private JButton ivjCutButton = null;
	private JMenuItem ivjCutMenuItem = null;
	private JMenuItem ivjDeleteMenuItem = null;
	private JMenu ivjEditMenu = null;
	IvjEventHandler ivjEventHandler = new IvjEventHandler();
	private JMenuItem ivjExitMenuItem = null;
	private JMenu ivjFileMenu = null;
	private JButton ivjFind_ReplaceButton = null;
	private JCheckBoxMenuItem ivjFlat_ViewMenuItem = null;
	private JMenuItem ivjGroupMenuItem = null;
	private JMenuItem ivjHelp_TopicsMenuItem = null;
	private JMenu ivjHelpMenu = null;
	private JMenuItem ivjImport_DBDMenuItem = null;
	private JMenuItem ivjImport_DBMenuItem = null;
	private JMenuItem ivjImportTemplate_DBMenuItem = null;
	private JMenuItem ivjImportFieldsMenuItem = null;
	private JMenuItem ivjImportBorderMenuItem = null;
	private JPanel ivjJFrameContentPane = null;
	private JSeparator ivjJSeparator1 = null;
	private JSeparator ivjJSeparator10 = null;
	private JSeparator ivjJSeparator11a = null;
	private JSeparator ivjJSeparator12 = null;
	private JSeparator ivjJSeparator13 = null;
	private JSeparator ivjJSeparator2 = null;
	private JSeparator ivjJSeparator3 = null;
	private JSeparator ivjJSeparator4 = null;
	private JSeparator ivjJSeparator5 = null;
	private JSeparator ivjJSeparator6 = null;
	private JSeparator ivjJSeparator8 = null;
	private JSeparator ivjJSeparator9 = null;
	private JMenuItem ivjLevel_UpMenuItem = null;
	private JMenuItem ivjMove_RenameMenuItem = null;
	private JMenuItem ivjMorphMenuItem = null;
	private JMenuItem ivjFindMenuItem = null;
	private JMenuItem ivjNewMenuItem = null;
	private JButton ivjOpenButton = null;
	private JMenuItem ivjOpenMenuItem = null;
	private JButton ivjPasteButton = null;
	private JMenuItem ivjPasteMenuItem = null;
	private JMenuItem ivjPreferences___MenuItem = null;
	private JMenuItem spreadsheetMenuItem = null;
	private JMenuItem ivjPrintMenuItem = null;
	private JButton ivjRedoButton = null;
	private JMenuItem ivjRedoMenuItem = null;
	private JMenuItem ivjSave_As_GroupMenuItem = null;
	private JMenuItem closeMenuItem = null;
	private JMenuItem closeAllMenuItem = null;
	private JMenuItem ivjSave_AsMenuItem = null;
	private JButton ivjSaveButton = null;
	private JMenuItem ivjSaveMenuItem = null;
	private JMenuItem ivjSaveAsTemplateMenuItem = null;
	private JMenuItem ivjGenerateMenuItem = null;
	private JMenuItem ivjGenerateAsGroupMenuItem = null;
	private RecentFilesMenu ivjRecentFilesMenuItem = null;
	private JMenuItem ivjSelect_AllMenuItem = null;
	private JCheckBoxMenuItem ivjShow_PointsMenuItem = null;
	private JMenuItem ivjSmart_ZoomMenuItem = null;
	private JCheckBoxMenuItem ivjStatusbarMenuItem = null;
	private JPanel ivjStatusBarPane = null;
	private JLabel ivjStatusMsg1 = null;
	private JLabel ivjStatusMsg2 = null;
	private JLabel ivjModeStatus = null;
	private JPanel ivjStatusGroupAndModePane = null;
	private JCheckBoxMenuItem ivjToolbarMenuItem = null;
	private JToolBar ivjToolBarPane = null;
	private JButton ivjUndoButton = null;
	private JMenuItem ivjUndoMenuItem = null;
	private JMenuItem ivjUngroupMenuItem = null;
	private JMenu ivjViewMenu = null;
	private JMenuBar ivjVisualDCTJMenuBar = null;
	private JMenuItem ivjZoom_InMenuItem = null;
	private JMenuItem ivjZoom_OutMenuItem = null;
	private JSlider ivjZoomSlider = null;
	private JLabel ivjZoomLabel = null;
	private JPanel ivjZoomPanel = null;
	private JFileChooser ivjfileChooser = null;
	private JCheckBoxMenuItem ivjSnapToGridMenuItem = null;
	private JCheckBoxMenuItem ivjWindowsPanMenuItem = null;
	private JPanel ivjButtonPanel = null;
	private JPanel ivjButtonPanel3 = null;
	private JPanel ivjJDialogContentPane = null;
	private JPanel ivjJDialogContentPane3 = null;
	private JLabel ivjNameLabel = null;
	private JLabel ivjNameLabel3 = null;
	private JLabel ivjNameTextLabel = null;
	//private JTextField ivjNameTextField = null;
	private JComboBox ivjNameTextField = null;
	private JDialog ivjNewRecordDialog = null;
	private JDialog ivjMorphingDialog = null;
	private JButton ivjOKButton = null;
	private JButton ivjMorphingOKButton = null;
	private JComboBox ivjTypeComboBox = null;
	private JComboBox ivjTypeComboBox2 = null;
	private JLabel ivjTypeLabel = null;
	private JLabel ivjTypeLabel2 = null;
	private JLabel ivjWarningLabel = null;
	private JButton ivjCancelButton = null;
	private JButton ivjMorphingCancelButton = null;
	// private
	private java.io.File openedFile = null;
	private String addedToTitle = "";
	private JCheckBoxMenuItem ivjNavigatorMenuItem = null;
	private JPanel ivjButtonPanel1 = null;
	private JButton ivjCancelButton1 = null;
	private JDialog ivjGroupDialog = null;
	private JTextField ivjgroupNameTextField = null;
	private JButton ivjGroupOKButton = null;
	private JLabel ivjGroupWarningLabel = null;
	private JPanel ivjJDialogContentPane1 = null;
	private JLabel ivjNameLabel1 = null;
	private JPanel ivjButtonPanel2 = null;
	private JPanel ivjJDialogContentPane2 = null;
	private JLabel ivjNameLabel2 = null;
	private JTextField ivjNewNameTextField = null;
	private JButton ivjRenameCancelButton = null;
	private JDialog ivjRenameDialog = null;
	private JButton ivjRenameOKButton = null;
	private JLabel ivjRenameWarningLabel = null;
	private JLabel ivjTypeLabel1 = null;
	private JLabel ivjOldNameLabel = null;
	private JMenuItem ivjPageSetupMenuItem = null;
	private JMenuItem ivjPrintPreviewMenuItem = null;
	private com.cosylab.vdct.graphics.WorkspaceDesktop ivjworkspace = null;
	private JMenuItem ivjPluginManagerMenuItem = null;
	private com.cosylab.vdct.plugin.PluginsMenu ivjPluginsMenu = null;
	private JSeparator ivjJSeparator7 = null;
	private com.cosylab.vdct.plugin.export.ExportMenu ivjExportMenuItem = null;
	private com.cosylab.vdct.plugin.menu.ToolsMenu ivjToolsMenu = null;
	private JSeparator ivjJSeparator11 = null;
	private JMenu ivjDebugMenu = null;
	private com.cosylab.vdct.plugin.debug.DebugStartMenu ivjStartDebugMenuItem = null;
	private com.cosylab.vdct.plugin.debug.DebugStopMenuItem ivjStopDebugMenuItem = null;
	private JMenu rdbMenu = null;
	private JMenuItem rdbConnectMenuItem = null;
	private JMenuItem rdbLoadMenuItem = null;
	private JMenuItem rdbSaveMenuItem = null;
	private JMenuItem rdbSaveAsMenuItem = null;
	
	private DBDDialog dbdDialog = null;

	private NameChecker newRecordNameChecker = null;
	private NameChecker newGroupNameChecker = null;
	private NameChecker renameNameChecker = null;
	
	// navigation menuitems
	private JMenuItem leftMenuItem = null;
	private JMenuItem rightMenuItem = null;
	private JMenuItem upMenuItem = null;
	private JMenuItem downMenuItem = null;

	private JMenuItem ivjPrintAsPostScriptMenuItem = null;
	private JMenuItem ivjExportPostScriptFileMenuItem = null;

	private JButton lineButton = null;
	private JButton boxButton = null;
	private JButton textBoxButton = null;
	private boolean lineButtonEnabled = false;
	private boolean boxButtonEnabled = false;
	private boolean textBoxButtonEnabled = false;

	private boolean dsCommandsActive = false;
	
	private PluginManagerDialog pluginManagerDialog = null;
	
	private ComboBoxFileChooser comboBoxFileChooser = null;

	private PrintService lastPrintService = null;
	
	private SpreadsheetInspector spreadsheet = null; 
	
	private static VisualDCT instance = null;
	
class IvjEventHandler implements java.awt.event.ActionListener, java.awt.event.ItemListener, java.awt.event.MouseListener, java.awt.event.MouseMotionListener, java.awt.event.WindowListener {
		public void actionPerformed(java.awt.event.ActionEvent e) {
			if (e.getSource() == VisualDCT.this.getLeftMenuItem())
				VisualDCT.this.moveOrigin(SwingConstants.WEST);
			else if (e.getSource() == VisualDCT.this.getRightMenuItem()) 
				VisualDCT.this.moveOrigin(SwingConstants.EAST);
			else if (e.getSource() == VisualDCT.this.getUpMenuItem()) 
				VisualDCT.this.moveOrigin(SwingConstants.SOUTH);
			else if (e.getSource() == VisualDCT.this.getDownMenuItem()) 
				VisualDCT.this.moveOrigin(SwingConstants.NORTH);

			else if (e.getSource() == VisualDCT.this.getAbout_BoxMenuItem()) 
				connEtoC3(e);
			else if (e.getSource() == VisualDCT.this.getNewMenuItem()) 
				connEtoC4(e);
			else if (e.getSource() == VisualDCT.this.getOpenMenuItem()) 
				connEtoC7(e);
			else if (e.getSource() == VisualDCT.this.getImport_DBMenuItem()) 
				connEtoC8(e);
			else if (e.getSource() == VisualDCT.this.getImportTemplate_DBMenuItem()) 
				VisualDCT.this.importTemplateActionPerformed();
			else if (e.getSource() == VisualDCT.this.getImportFieldsMenuItem())
			    VisualDCT.this.importFieldsActionPerformed();
			else if (e.getSource() == VisualDCT.this.getImportBorderMenuItem())
			    VisualDCT.this.importBorderActionPerformed();
			else if (e.getSource() == VisualDCT.this.getImport_DBDMenuItem()) 
				connEtoC9(e);
			else if (e.getSource() == VisualDCT.this.getSaveMenuItem()) 
				connEtoC10(e);
			else if (e.getSource() == VisualDCT.this.getSave_AsMenuItem()) 
				connEtoC11(e);
			else if (e.getSource() == VisualDCT.this.getSave_As_GroupMenuItem()) 
				connEtoC12(e);
			else if (e.getSource() == VisualDCT.this.getPrintMenuItem()) 
				connEtoC13(e);
			else if (e.getSource() == VisualDCT.this.getExitMenuItem()) 
				connEtoC14(e);
			else if (e.getSource() == VisualDCT.this.getUndoMenuItem()) 
				connEtoC15(e);
			else if (e.getSource() == VisualDCT.this.getRedoMenuItem()) 
				connEtoC16(e);
			else if (e.getSource() == VisualDCT.this.getCutMenuItem()) 
				connEtoC17(e);
			else if (e.getSource() == VisualDCT.this.getCopyMenuItem()) 
				connEtoC18(e);
			else if (e.getSource() == VisualDCT.this.getPasteMenuItem()) 
				connEtoC19(e);
			else if (e.getSource() == VisualDCT.this.getSystemCopyMenuItem()) 
				systemCopy();
			else if (e.getSource() == VisualDCT.this.getSystemPasteMenuItem()) 
				systemPaste();
			else if (e.getSource() == VisualDCT.this.getMove_RenameMenuItem()) 
				connEtoC20(e);
			else if (e.getSource() == VisualDCT.this.getMorphMenuItem()) 
				connEtoC69(e);				
			else if (e.getSource() == VisualDCT.this.getFindMenuItem()) 
			    VisualDCT.this.findActionPerformed();				
			else if (e.getSource() == VisualDCT.this.getGroupMenuItem()) 
				connEtoC21(e);
			else if (e.getSource() == VisualDCT.this.getUngroupMenuItem()) 
				connEtoC22(e);
			else if (e.getSource() == VisualDCT.this.getDeleteMenuItem()) 
				connEtoC23(e);
			else if (e.getSource() == VisualDCT.this.getSelect_AllMenuItem()) 
				connEtoC24(e);
			else if (e.getSource() == VisualDCT.this.getHelp_TopicsMenuItem()) 
				connEtoC26(e);
			else if (e.getSource() == VisualDCT.this.getBooks_OnlineMenuItem()) 
				connEtoC27(e);
			else if (e.getSource() == VisualDCT.this.getOpenButton()) 
				connEtoC28(e);
			else if (e.getSource() == VisualDCT.this.getSaveButton()) 
				connEtoC29(e);
			else if (e.getSource() == VisualDCT.this.getFindButton()) 
			    VisualDCT.this.findActionPerformed();
			else if (e.getSource() == VisualDCT.this.getUndoButton()) 
				connEtoC30(e);
			else if (e.getSource() == VisualDCT.this.getRedoButton()) 
				connEtoC31(e);
			else if (e.getSource() == VisualDCT.this.getCutButton()) 
				connEtoC32(e);
			else if (e.getSource() == VisualDCT.this.getCopyButton()) 
				connEtoC33(e);
			else if (e.getSource() == VisualDCT.this.getPasteButton()) 
				connEtoC34(e);
			else if (e.getSource() == VisualDCT.this.getLevel_UpMenuItem()) 
				connEtoC36(e);
			else if (e.getSource() == VisualDCT.this.getZoom_InMenuItem()) 
				connEtoC37(e);
			else if (e.getSource() == VisualDCT.this.getZoom_OutMenuItem()) 
				connEtoC38(e);
			else if (e.getSource() == VisualDCT.this.getSmart_ZoomMenuItem()) 
				connEtoC39(e);
			else if (e.getSource() == VisualDCT.this.getBase_ViewMenuItem()) 
				connEtoC41(e);
			else if (e.getSource() == VisualDCT.this.getPreferences___MenuItem()) 
				connEtoC45(e);
			else if (e.getSource() == VisualDCT.this.getSpreadsheetMenuItem()) 
				spreadsheetMenuItemActionPerformed();
			else if (e.getSource() == VisualDCT.this.getOKButton()) 
				connEtoC47(e);
			else if (e.getSource() == VisualDCT.this.getMorphingOKButton()) 
				connEtoC66(e);				
			else if (e.getSource() == VisualDCT.this.getCancelButton()) 
				connEtoM2(e);
			else if (e.getSource() == VisualDCT.this.getMorphingCancelButton()) 
				connEtoM4(e);
			else if (e.getSource() == VisualDCT.this.getNameTextField()) 
				connEtoC51(e);
			else if (e.getSource() == VisualDCT.this.getStatusbarMenuItem()) 
				connEtoC2(e);
			else if (e.getSource() == VisualDCT.this.getStatusbarMenuItem()) 
				connEtoC43(e);
			else if (e.getSource() == VisualDCT.this.getToolbarMenuItem()) 
				connEtoC1(e);
			else if (e.getSource() == VisualDCT.this.getToolbarMenuItem()) 
				connEtoC42(e);
			else if (e.getSource() == VisualDCT.this.getFlat_ViewMenuItem()) 
				connEtoC35(e);
			else if (e.getSource() == VisualDCT.this.getCancelButton1()) 
				connEtoM1(e);
			else if (e.getSource() == VisualDCT.this.getGroupOKButton()) 
				connEtoC52(e);
			else if (e.getSource() == VisualDCT.this.getGroupNameTextField()) 
				connEtoC53(e);
			else if (e.getSource() == VisualDCT.this.getRenameCancelButton()) 
				connEtoM3(e);
			else if (e.getSource() == VisualDCT.this.getRenameOKButton()) 
				connEtoC55(e);
			else if (e.getSource() == VisualDCT.this.getNewNameTextField()) 
				connEtoC56(e);
			else if (e.getSource() == VisualDCT.this.getPageSetupMenuItem()) 
				connEtoC61(e);
			else if (e.getSource() == VisualDCT.this.getPrintPreviewMenuItem()) 
				connEtoC62(e);
			else if (e.getSource() == VisualDCT.this.getPluginManagerMenuItem()) 
				connEtoC63(e);
			else if (e.getSource() == VisualDCT.this.getExportPostScriptFileMenuItem()) 
				connEtoC64(e);
			else if (e.getSource() == VisualDCT.this.getPrintAsPostScriptMenuItem()) 
				connEtoC65(e);
			else if (e.getSource() == VisualDCT.this.getLineButton()) 
				lineButton_ActionPerformed();
			else if (e.getSource() == VisualDCT.this.getBoxButton()) 
				boxButton_ActionPerformed();
			else if (e.getSource() == VisualDCT.this.getTextBoxButton()) 
				textBoxButton_ActionPerformed();
			else if (e.getSource() == VisualDCT.this.getGenerateMenuItem()) 
				generateMenuItem_ActionPerformed();
			else if (e.getSource() == VisualDCT.this.getGenerateAsGroupMenuItem()) 
				generateAsGroupMenuItem_ActionPerformed();
			else if (e.getSource() == VisualDCT.this.getSaveAsTemplateMenuItem()) 
				saveAsTemplateMenuItem_ActionPerformed();

		};
		public void itemStateChanged(java.awt.event.ItemEvent e) {
			if (e.getSource() == VisualDCT.this.getShow_PointsMenuItem()) 
				connEtoC44(e);
			if (e.getSource() == VisualDCT.this.getSnapToGridMenuItem()) 
				connEtoC46(e);
			if (e.getSource() == VisualDCT.this.getWindowsPanMenuItem()) 
				connEtoCWindowsPan(e);				
			if (e.getSource() == VisualDCT.this.getNavigatorMenuItem()) 
				connEtoC25(e);
		};
		public void mouseClicked(java.awt.event.MouseEvent e) {};
		public void mouseDragged(java.awt.event.MouseEvent e) {
			if (e.getSource() == VisualDCT.this.getZoomSlider()) 
				connEtoC5(e);
		};
		public void mouseEntered(java.awt.event.MouseEvent e) {};
		public void mouseExited(java.awt.event.MouseEvent e) {};
		public void mouseMoved(java.awt.event.MouseEvent e) {};
		public void mousePressed(java.awt.event.MouseEvent e) {};
		public void mouseReleased(java.awt.event.MouseEvent e) {
			if (e.getSource() == VisualDCT.this.getZoomSlider()) 
				connEtoC6(e);
		};
		public void windowActivated(java.awt.event.WindowEvent e) {
			if (e.getSource() == VisualDCT.this.getNewRecordDialog()) 
				connEtoC58(e);
			if (e.getSource() == VisualDCT.this.getMorphingDialog()) 		
				connEtoC67(e);				
			if (e.getSource() == VisualDCT.this.getGroupDialog()) 
				connEtoC59(e);
			if (e.getSource() == VisualDCT.this.getRenameDialog()) 
				connEtoC60(e);
		};
		public void windowClosed(java.awt.event.WindowEvent e) {};
		public void windowClosing(java.awt.event.WindowEvent e) {};
		public void windowDeactivated(java.awt.event.WindowEvent e) {};
		public void windowDeiconified(java.awt.event.WindowEvent e) {};
		public void windowIconified(java.awt.event.WindowEvent e) {};
		public void windowOpened(java.awt.event.WindowEvent e) {
			if (e.getSource() == VisualDCT.this.getNewRecordDialog()) 
				connEtoC48(e);				
			if (e.getSource() == VisualDCT.this.getMorphingDialog()) 
				connEtoC68(e);
			if (e.getSource() == VisualDCT.this.getGroupDialog()) 
				connEtoC50(e);
			if (e.getSource() == VisualDCT.this.getRenameDialog()) 
				connEtoC57(e);
		};
	};
/**
 * VisualDCT constructor comment.
 */
public VisualDCT() {
	super();
	instance = this;
	dsCommandsActive = false;

	initialize();
}
/**
 * VisualDCT constructor comment.
 * @param title java.lang.String
 */
public VisualDCT(String title) {
	super(title);
	instance = this;
	dsCommandsActive = false;
}
/**
 * Comment
 */
public void base_ViewMenuItem_ActionPerformed() {
	GetGUIInterface cmd = (GetGUIInterface)CommandManager.getInstance().getCommand("GetGUIMenuInterface");
 	cmd.getGUIMenuInterface().baseView();
}
/**
 * Comment
 */
public void books_OnlineMenuItem_ActionPerformed() {
	return;
}

public void boxButton_ActionPerformed()
{
	if(boxButtonEnabled)
		setBoxButtonEnabled(false);
	else
		setBoxButtonEnabled(true);
}

/**
 * connEtoC1:  (ToolbarMenuItem.action.actionPerformed(java.awt.event.ActionEvent) --> VisualDCT.viewToolBar()V)
 * @param arg1 java.awt.event.ActionEvent
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private void connEtoC1(java.awt.event.ActionEvent arg1) {
	try {
		// user code begin {1}
		// user code end
		this.viewToolBar();
		// user code begin {2}
		// user code end
	} catch (java.lang.Throwable ivjExc) {
		// user code begin {3}
		// user code end
		handleException(ivjExc);
	}
}
/**
 * connEtoC10:  (SaveMenuItem.action.actionPerformed(java.awt.event.ActionEvent) --> VisualDCT.saveMenuItem_ActionPerformed()V)
 * @param arg1 java.awt.event.ActionEvent
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private void connEtoC10(java.awt.event.ActionEvent arg1) {
	try {
		// user code begin {1}
		// user code end
		this.saveMenuItem_ActionPerformed();
		// user code begin {2}
		// user code end
	} catch (java.lang.Throwable ivjExc) {
		// user code begin {3}
		// user code end
		handleException(ivjExc);
	}
}
/**
 * connEtoC11:  (Save_AsMenuItem.action.actionPerformed(java.awt.event.ActionEvent) --> VisualDCT.save_AsMenuItem_ActionPerformed()V)
 * @param arg1 java.awt.event.ActionEvent
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private void connEtoC11(java.awt.event.ActionEvent arg1) {
	try {
		// user code begin {1}
		// user code end
		this.save_AsMenuItem_ActionPerformed();
		// user code begin {2}
		// user code end
	} catch (java.lang.Throwable ivjExc) {
		// user code begin {3}
		// user code end
		handleException(ivjExc);
	}
}
/**
 * connEtoC12:  (Save_As_GroupMenuItem.action.actionPerformed(java.awt.event.ActionEvent) --> VisualDCT.save_As_GroupMenuItem_ActionPerformed()V)
 * @param arg1 java.awt.event.ActionEvent
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private void connEtoC12(java.awt.event.ActionEvent arg1) {
	try {
		// user code begin {1}
		// user code end
		this.save_As_GroupMenuItem_ActionPerformed();
		// user code begin {2}
		// user code end
	} catch (java.lang.Throwable ivjExc) {
		// user code begin {3}
		// user code end
		handleException(ivjExc);
	}
}
/**
 * connEtoC13:  (PrintMenuItem.action.actionPerformed(java.awt.event.ActionEvent) --> VisualDCT.printMenuItem_ActionPerformed()V)
 * @param arg1 java.awt.event.ActionEvent
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private void connEtoC13(java.awt.event.ActionEvent arg1) {
	try {
		// user code begin {1}
		// user code end
		this.printMenuItem_ActionPerformed();
		// user code begin {2}
		// user code end
	} catch (java.lang.Throwable ivjExc) {
		// user code begin {3}
		// user code end
		handleException(ivjExc);
	}
}
/**
 * connEtoC14:  (ExitMenuItem.action.actionPerformed(java.awt.event.ActionEvent) --> VisualDCT.exitMenuItem_ActionPerformed()V)
 * @param arg1 java.awt.event.ActionEvent
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private void connEtoC14(java.awt.event.ActionEvent arg1) {
	try {
		// user code begin {1}
		// user code end
		this.exitMenuItem_ActionPerformed();
		// user code begin {2}
		// user code end
	} catch (java.lang.Throwable ivjExc) {
		// user code begin {3}
		// user code end
		handleException(ivjExc);
	}
}
/**
 * connEtoC15:  (UndoMenuItem.action.actionPerformed(java.awt.event.ActionEvent) --> VisualDCT.undoMenuItem_ActionPerformed()V)
 * @param arg1 java.awt.event.ActionEvent
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private void connEtoC15(java.awt.event.ActionEvent arg1) {
	try {
		// user code begin {1}
		// user code end
		this.undoMenuItem_ActionPerformed();
		// user code begin {2}
		// user code end
	} catch (java.lang.Throwable ivjExc) {
		// user code begin {3}
		// user code end
		handleException(ivjExc);
	}
}
/**
 * connEtoC16:  (RedoMenuItem.action.actionPerformed(java.awt.event.ActionEvent) --> VisualDCT.redoMenuItem_ActionPerformed()V)
 * @param arg1 java.awt.event.ActionEvent
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private void connEtoC16(java.awt.event.ActionEvent arg1) {
	try {
		// user code begin {1}
		// user code end
		this.redoMenuItem_ActionPerformed();
		// user code begin {2}
		// user code end
	} catch (java.lang.Throwable ivjExc) {
		// user code begin {3}
		// user code end
		handleException(ivjExc);
	}
}
/**
 * connEtoC17:  (CutMenuItem.action.actionPerformed(java.awt.event.ActionEvent) --> VisualDCT.cutMenuItem_ActionPerformed()V)
 * @param arg1 java.awt.event.ActionEvent
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private void connEtoC17(java.awt.event.ActionEvent arg1) {
	try {
		// user code begin {1}
		// user code end
		this.cutMenuItem_ActionPerformed();
		// user code begin {2}
		// user code end
	} catch (java.lang.Throwable ivjExc) {
		// user code begin {3}
		// user code end
		handleException(ivjExc);
	}
}
/**
 * connEtoC18:  (CopyMenuItem.action.actionPerformed(java.awt.event.ActionEvent) --> VisualDCT.copyMenuItem_ActionPerformed()V)
 * @param arg1 java.awt.event.ActionEvent
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private void connEtoC18(java.awt.event.ActionEvent arg1) {
	try {
		// user code begin {1}
		// user code end
		this.copyMenuItem_ActionPerformed();
		// user code begin {2}
		// user code end
	} catch (java.lang.Throwable ivjExc) {
		// user code begin {3}
		// user code end
		handleException(ivjExc);
	}
}
/**
 * connEtoC19:  (PasteMenuItem.action.actionPerformed(java.awt.event.ActionEvent) --> VisualDCT.pasteMenuItem_ActionPerformed()V)
 * @param arg1 java.awt.event.ActionEvent
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private void connEtoC19(java.awt.event.ActionEvent arg1) {
	try {
		// user code begin {1}
		// user code end
		this.pasteMenuItem_ActionPerformed();
		// user code begin {2}
		// user code end
	} catch (java.lang.Throwable ivjExc) {
		// user code begin {3}
		// user code end
		handleException(ivjExc);
	}
}
/**
 * connEtoC2:  (StatusbarMenuItem.action.actionPerformed(java.awt.event.ActionEvent) --> VisualDCT.viewStatusBar()V)
 * @param arg1 java.awt.event.ActionEvent
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private void connEtoC2(java.awt.event.ActionEvent arg1) {
	try {
		// user code begin {1}
		// user code end
		this.viewStatusBar();
		// user code begin {2}
		// user code end
	} catch (java.lang.Throwable ivjExc) {
		// user code begin {3}
		// user code end
		handleException(ivjExc);
	}
}
/**
 * connEtoC20:  (Move_RenameMenuItem.action.actionPerformed(java.awt.event.ActionEvent) --> VisualDCT.move_RenameMenuItem_ActionPerformed()V)
 * @param arg1 java.awt.event.ActionEvent
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private void connEtoC20(java.awt.event.ActionEvent arg1) {
	try {
		// user code begin {1}
		// user code end
		this.move_RenameMenuItem_ActionPerformed();
		// user code begin {2}
		// user code end
	} catch (java.lang.Throwable ivjExc) {
		// user code begin {3}
		// user code end
		handleException(ivjExc);
	}
}

private void connEtoC69(java.awt.event.ActionEvent arg1) {
	try {
		// user code begin {1}
		// user code end
		this.morphMenuItem_ActionPerformed();
		// user code begin {2}
		// user code end
	} catch (java.lang.Throwable ivjExc) {
		// user code begin {3}
		// user code end
		handleException(ivjExc);
	}
}

/**
 * connEtoC21:  (GroupMenuItem.action.actionPerformed(java.awt.event.ActionEvent) --> VisualDCT.groupMenuItem_ActionPerformed()V)
 * @param arg1 java.awt.event.ActionEvent
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private void connEtoC21(java.awt.event.ActionEvent arg1) {
	try {
		// user code begin {1}
		// user code end
		this.groupMenuItem_ActionPerformed();
		// user code begin {2}
		// user code end
	} catch (java.lang.Throwable ivjExc) {
		// user code begin {3}
		// user code end
		handleException(ivjExc);
	}
}
/**
 * connEtoC22:  (UngroupMenuItem.action.actionPerformed(java.awt.event.ActionEvent) --> VisualDCT.ungroupMenuItem_ActionPerformed()V)
 * @param arg1 java.awt.event.ActionEvent
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private void connEtoC22(java.awt.event.ActionEvent arg1) {
	try {
		// user code begin {1}
		// user code end
		this.ungroupMenuItem_ActionPerformed();
		// user code begin {2}
		// user code end
	} catch (java.lang.Throwable ivjExc) {
		// user code begin {3}
		// user code end
		handleException(ivjExc);
	}
}
/**
 * connEtoC23:  (DeleteMenuItem.action.actionPerformed(java.awt.event.ActionEvent) --> VisualDCT.deleteMenuItem_ActionPerformed()V)
 * @param arg1 java.awt.event.ActionEvent
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private void connEtoC23(java.awt.event.ActionEvent arg1) {
	try {
		// user code begin {1}
		// user code end
		this.deleteMenuItem_ActionPerformed();
		// user code begin {2}
		// user code end
	} catch (java.lang.Throwable ivjExc) {
		// user code begin {3}
		// user code end
		handleException(ivjExc);
	}
}
/**
 * connEtoC24:  (Select_AllMenuItem.action.actionPerformed(java.awt.event.ActionEvent) --> VisualDCT.select_AllMenuItem_ActionPerformed()V)
 * @param arg1 java.awt.event.ActionEvent
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private void connEtoC24(java.awt.event.ActionEvent arg1) {
	try {
		// user code begin {1}
		// user code end
		this.select_AllMenuItem_ActionPerformed();
		// user code begin {2}
		// user code end
	} catch (java.lang.Throwable ivjExc) {
		// user code begin {3}
		// user code end
		handleException(ivjExc);
	}
}
/**
 * connEtoC25:  (NavigatorMenuItem.item.itemStateChanged(java.awt.event.ItemEvent) --> VisualDCT.navigatorMenuItem_ItemStateChanged(Ljava.awt.event.ItemEvent;)V)
 * @param arg1 java.awt.event.ItemEvent
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private void connEtoC25(java.awt.event.ItemEvent arg1) {
	try {
		// user code begin {1}
		// user code end
		this.navigatorMenuItem_ItemStateChanged(arg1);
		// user code begin {2}
		// user code end
	} catch (java.lang.Throwable ivjExc) {
		// user code begin {3}
		// user code end
		handleException(ivjExc);
	}
}
/**
 * connEtoC26:  (Help_TopicsMenuItem.action.actionPerformed(java.awt.event.ActionEvent) --> VisualDCT.help_TopicsMenuItem_ActionPerformed()V)
 * @param arg1 java.awt.event.ActionEvent
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private void connEtoC26(java.awt.event.ActionEvent arg1) {
	try {
		// user code begin {1}
		// user code end
		this.help_TopicsMenuItem_ActionPerformed();
		// user code begin {2}
		// user code end
	} catch (java.lang.Throwable ivjExc) {
		// user code begin {3}
		// user code end
		handleException(ivjExc);
	}
}
/**
 * connEtoC27:  (Books_OnlineMenuItem.action.actionPerformed(java.awt.event.ActionEvent) --> VisualDCT.books_OnlineMenuItem_ActionPerformed()V)
 * @param arg1 java.awt.event.ActionEvent
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private void connEtoC27(java.awt.event.ActionEvent arg1) {
	try {
		// user code begin {1}
		// user code end
		this.books_OnlineMenuItem_ActionPerformed();
		// user code begin {2}
		// user code end
	} catch (java.lang.Throwable ivjExc) {
		// user code begin {3}
		// user code end
		handleException(ivjExc);
	}
}
/**
 * connEtoC28:  (OpenButton.action.actionPerformed(java.awt.event.ActionEvent) --> VisualDCT.openMenuItem_ActionPerformed()V)
 * @param arg1 java.awt.event.ActionEvent
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private void connEtoC28(java.awt.event.ActionEvent arg1) {
	try {
		// user code begin {1}
		// user code end
		this.openMenuItem_ActionPerformed();
		// user code begin {2}
		// user code end
	} catch (java.lang.Throwable ivjExc) {
		// user code begin {3}
		// user code end
		handleException(ivjExc);
	}
}
/**
 * connEtoC29:  (SaveButton.action.actionPerformed(java.awt.event.ActionEvent) --> VisualDCT.saveMenuItem_ActionPerformed()V)
 * @param arg1 java.awt.event.ActionEvent
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private void connEtoC29(java.awt.event.ActionEvent arg1) {
	try {
		// user code begin {1}
		// user code end
		this.saveMenuItem_ActionPerformed();
		// user code begin {2}
		// user code end
	} catch (java.lang.Throwable ivjExc) {
		// user code begin {3}
		// user code end
		handleException(ivjExc);
	}
}
/**
 * connEtoC3:  (About_BoxMenuItem.action.actionPerformed(java.awt.event.ActionEvent) --> VisualDCT.showAboutBox()V)
 * @param arg1 java.awt.event.ActionEvent
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private void connEtoC3(java.awt.event.ActionEvent arg1) {
	try {
		// user code begin {1}
		// user code end
		this.showAboutBox();
		// user code begin {2}
		// user code end
	} catch (java.lang.Throwable ivjExc) {
		// user code begin {3}
		// user code end
		handleException(ivjExc);
	}
}
/**
 * connEtoC30:  (UndoButton.action.actionPerformed(java.awt.event.ActionEvent) --> VisualDCT.undoMenuItem_ActionPerformed()V)
 * @param arg1 java.awt.event.ActionEvent
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private void connEtoC30(java.awt.event.ActionEvent arg1) {
	try {
		// user code begin {1}
		// user code end
		this.undoMenuItem_ActionPerformed();
		// user code begin {2}
		// user code end
	} catch (java.lang.Throwable ivjExc) {
		// user code begin {3}
		// user code end
		handleException(ivjExc);
	}
}
/**
 * connEtoC31:  (RedoButton.action.actionPerformed(java.awt.event.ActionEvent) --> VisualDCT.redoMenuItem_ActionPerformed()V)
 * @param arg1 java.awt.event.ActionEvent
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private void connEtoC31(java.awt.event.ActionEvent arg1) {
	try {
		// user code begin {1}
		// user code end
		this.redoMenuItem_ActionPerformed();
		// user code begin {2}
		// user code end
	} catch (java.lang.Throwable ivjExc) {
		// user code begin {3}
		// user code end
		handleException(ivjExc);
	}
}
/**
 * connEtoC32:  (CutButton.action.actionPerformed(java.awt.event.ActionEvent) --> VisualDCT.cutMenuItem_ActionPerformed()V)
 * @param arg1 java.awt.event.ActionEvent
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private void connEtoC32(java.awt.event.ActionEvent arg1) {
	try {
		// user code begin {1}
		// user code end
		this.cutMenuItem_ActionPerformed();
		// user code begin {2}
		// user code end
	} catch (java.lang.Throwable ivjExc) {
		// user code begin {3}
		// user code end
		handleException(ivjExc);
	}
}
/**
 * connEtoC33:  (CopyButton.action.actionPerformed(java.awt.event.ActionEvent) --> VisualDCT.copyMenuItem_ActionPerformed()V)
 * @param arg1 java.awt.event.ActionEvent
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private void connEtoC33(java.awt.event.ActionEvent arg1) {
	try {
		// user code begin {1}
		// user code end
		this.copyMenuItem_ActionPerformed();
		// user code begin {2}
		// user code end
	} catch (java.lang.Throwable ivjExc) {
		// user code begin {3}
		// user code end
		handleException(ivjExc);
	}
}
/**
 * connEtoC34:  (PasteButton.action.actionPerformed(java.awt.event.ActionEvent) --> VisualDCT.pasteMenuItem_ActionPerformed()V)
 * @param arg1 java.awt.event.ActionEvent
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private void connEtoC34(java.awt.event.ActionEvent arg1) {
	try {
		// user code begin {1}
		// user code end
		this.pasteMenuItem_ActionPerformed();
		// user code begin {2}
		// user code end
	} catch (java.lang.Throwable ivjExc) {
		// user code begin {3}
		// user code end
		handleException(ivjExc);
	}
}
/**
 * connEtoC35:  (Flat_ViewMenuItem.action.actionPerformed(java.awt.event.ActionEvent) --> VisualDCT.flat_ViewMenuItem_ActionPerformed()V)
 * @param arg1 java.awt.event.ActionEvent
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private void connEtoC35(java.awt.event.ActionEvent arg1) {
	try {
		// user code begin {1}
		// user code end
		this.flat_ViewMenuItem_ActionPerformed();
		// user code begin {2}
		// user code end
	} catch (java.lang.Throwable ivjExc) {
		// user code begin {3}
		// user code end
		handleException(ivjExc);
	}
}
/**
 * connEtoC36:  (Level_UpMenuItem.action.actionPerformed(java.awt.event.ActionEvent) --> VisualDCT.level_UpMenuItem_ActionPerformed()V)
 * @param arg1 java.awt.event.ActionEvent
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private void connEtoC36(java.awt.event.ActionEvent arg1) {
	try {
		// user code begin {1}
		// user code end
		this.level_UpMenuItem_ActionPerformed();
		// user code begin {2}
		// user code end
	} catch (java.lang.Throwable ivjExc) {
		// user code begin {3}
		// user code end
		handleException(ivjExc);
	}
}
/**
 * connEtoC37:  (Zoom_InMenuItem.action.actionPerformed(java.awt.event.ActionEvent) --> VisualDCT.zoom_InMenuItem_ActionPerformed()V)
 * @param arg1 java.awt.event.ActionEvent
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private void connEtoC37(java.awt.event.ActionEvent arg1) {
	try {
		// user code begin {1}
		// user code end
		this.zoom_InMenuItem_ActionPerformed();
		// user code begin {2}
		// user code end
	} catch (java.lang.Throwable ivjExc) {
		// user code begin {3}
		// user code end
		handleException(ivjExc);
	}
}
/**
 * connEtoC38:  (Zoom_OutMenuItem.action.actionPerformed(java.awt.event.ActionEvent) --> VisualDCT.zoom_OutMenuItem_ActionPerformed()V)
 * @param arg1 java.awt.event.ActionEvent
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private void connEtoC38(java.awt.event.ActionEvent arg1) {
	try {
		// user code begin {1}
		// user code end
		this.zoom_OutMenuItem_ActionPerformed();
		// user code begin {2}
		// user code end
	} catch (java.lang.Throwable ivjExc) {
		// user code begin {3}
		// user code end
		handleException(ivjExc);
	}
}
/**
 * connEtoC39:  (Smart_ZoomMenuItem.action.actionPerformed(java.awt.event.ActionEvent) --> VisualDCT.smart_ZoomMenuItem_ActionPerformed()V)
 * @param arg1 java.awt.event.ActionEvent
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private void connEtoC39(java.awt.event.ActionEvent arg1) {
	try {
		// user code begin {1}
		// user code end
		this.smart_ZoomMenuItem_ActionPerformed();
		// user code begin {2}
		// user code end
	} catch (java.lang.Throwable ivjExc) {
		// user code begin {3}
		// user code end
		handleException(ivjExc);
	}
}
/**
 * connEtoC4:  (NewMenuItem.action.actionPerformed(java.awt.event.ActionEvent) --> VisualDCT.newMenuItem_ActionPerformed()V)
 * @param arg1 java.awt.event.ActionEvent
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private void connEtoC4(java.awt.event.ActionEvent arg1) {
	try {
		// user code begin {1}
		// user code end
		this.newMenuItem_ActionPerformed();
		// user code begin {2}
		// user code end
	} catch (java.lang.Throwable ivjExc) {
		// user code begin {3}
		// user code end
		handleException(ivjExc);
	}
}
/**
 * connEtoC41:  (Base_ViewMenuItem.action.actionPerformed(java.awt.event.ActionEvent) --> VisualDCT.base_ViewMenuItem_ActionPerformed()V)
 * @param arg1 java.awt.event.ActionEvent
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private void connEtoC41(java.awt.event.ActionEvent arg1) {
	try {
		// user code begin {1}
		// user code end
		this.base_ViewMenuItem_ActionPerformed();
		// user code begin {2}
		// user code end
	} catch (java.lang.Throwable ivjExc) {
		// user code begin {3}
		// user code end
		handleException(ivjExc);
	}
}
/**
 * connEtoC42:  (ToolbarMenuItem.action.actionPerformed(java.awt.event.ActionEvent) --> VisualDCT.toolbarMenuItem_ActionPerformed()V)
 * @param arg1 java.awt.event.ActionEvent
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private void connEtoC42(java.awt.event.ActionEvent arg1) {
	try {
		// user code begin {1}
		// user code end
		this.toolbarMenuItem_ActionPerformed();
		// user code begin {2}
		// user code end
	} catch (java.lang.Throwable ivjExc) {
		// user code begin {3}
		// user code end
		handleException(ivjExc);
	}
}
/**
 * connEtoC43:  (StatusbarMenuItem.action.actionPerformed(java.awt.event.ActionEvent) --> VisualDCT.statusbarMenuItem_ActionPerformed()V)
 * @param arg1 java.awt.event.ActionEvent
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private void connEtoC43(java.awt.event.ActionEvent arg1) {
	try {
		// user code begin {1}
		// user code end
		this.statusbarMenuItem_ActionPerformed();
		// user code begin {2}
		// user code end
	} catch (java.lang.Throwable ivjExc) {
		// user code begin {3}
		// user code end
		handleException(ivjExc);
	}
}
/**
 * connEtoC44:  (Show_PointsMenuItem.item.itemStateChanged(java.awt.event.ItemEvent) --> VisualDCT.show_PointsMenuItem_ItemStateChanged(Ljava.awt.event.ItemEvent;)V)
 * @param arg1 java.awt.event.ItemEvent
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private void connEtoC44(java.awt.event.ItemEvent arg1) {
	try {
		// user code begin {1}
		// user code end
		this.show_PointsMenuItem_ItemStateChanged(arg1);
		// user code begin {2}
		// user code end
	} catch (java.lang.Throwable ivjExc) {
		// user code begin {3}
		// user code end
		handleException(ivjExc);
	}
}
/**
 * connEtoC45:  (Preferences___MenuItem.action.actionPerformed(java.awt.event.ActionEvent) --> VisualDCT.preferences___MenuItem_ActionPerformed()V)
 * @param arg1 java.awt.event.ActionEvent
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private void connEtoC45(java.awt.event.ActionEvent arg1) {
	try {
		// user code begin {1}
		// user code end
		this.preferences___MenuItem_ActionPerformed();
		// user code begin {2}
		// user code end
	} catch (java.lang.Throwable ivjExc) {
		// user code begin {3}
		// user code end
		handleException(ivjExc);
	}
}
/**
 * connEtoC46:  (SnapToGridMenuItem.item.itemStateChanged(java.awt.event.ItemEvent) --> VisualDCT.snapToGridMenuItem_ItemStateChanged(Ljava.awt.event.ItemEvent;)V)
 * @param arg1 java.awt.event.ItemEvent
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private void connEtoC46(java.awt.event.ItemEvent arg1) {
	try {
		// user code begin {1}
		// user code end
		this.snapToGridMenuItem_ItemStateChanged(arg1);
		// user code begin {2}
		// user code end
	} catch (java.lang.Throwable ivjExc) {
		// user code begin {3}
		// user code end
		handleException(ivjExc);
	}
}

private void connEtoCWindowsPan(java.awt.event.ItemEvent arg1) {
	try {
		// user code begin {1}
		// user code end
		this.windowsPanMenuItem_ItemStateChanged(arg1);
		// user code begin {2}
		// user code end
	} catch (java.lang.Throwable ivjExc) {
		// user code begin {3}
		// user code end
		handleException(ivjExc);
	}
}
/**
 * connEtoC47:  (OKButton.action.actionPerformed(java.awt.event.ActionEvent) --> VisualDCT.oKButton_ActionPerformed(Ljava.awt.event.ActionEvent;)V)
 * @param arg1 java.awt.event.ActionEvent
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private void connEtoC47(java.awt.event.ActionEvent arg1) {
	try {
		// user code begin {1}
		// user code end
		this.oKButton_ActionPerformed(arg1);
		// user code begin {2}
		// user code end
	} catch (java.lang.Throwable ivjExc) {
		// user code begin {3}
		// user code end
		handleException(ivjExc);
	}
}

private void connEtoC66(java.awt.event.ActionEvent arg1) {
	try {
		// user code begin {1}
		// user code end
		this.morphingOKButton_ActionPerformed(arg1);
		// user code begin {2}
		// user code end
	} catch (java.lang.Throwable ivjExc) {
		// user code begin {3}
		// user code end
		handleException(ivjExc);
	}
}

/**
 * connEtoC48:  (NewRecordDialog.window.windowOpened(java.awt.event.WindowEvent) --> VisualDCT.newRecordDialog_WindowOpened(Ljava.awt.event.WindowEvent;)V)
 * @param arg1 java.awt.event.WindowEvent
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private void connEtoC48(java.awt.event.WindowEvent arg1) {
	try {
		// user code begin {1}
		// user code end
		this.newRecordDialog_WindowOpened(arg1);
		// user code begin {2}
		// user code end
	} catch (java.lang.Throwable ivjExc) {
		// user code begin {3}
		// user code end
		handleException(ivjExc);
	}
}

private void connEtoC68(java.awt.event.WindowEvent arg1) {
	try {
		// user code begin {1}
		// user code end
		this.morphingDialog_WindowOpened(arg1);
		// user code begin {2}
		// user code end
	} catch (java.lang.Throwable ivjExc) {
		// user code begin {3}
		// user code end
		handleException(ivjExc);
	}
}
/**
 * connEtoC5:  (ZoomSlider.mouseMotion.mouseDragged(java.awt.event.MouseEvent) --> VisualDCT.zoomSlider_updateLabel()V)
 * @param arg1 java.awt.event.MouseEvent
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private void connEtoC5(java.awt.event.MouseEvent arg1) {
	try {
		// user code begin {1}
		// user code end
		this.zoomSlider_updateLabel();
		// user code begin {2}
		// user code end
	} catch (java.lang.Throwable ivjExc) {
		// user code begin {3}
		// user code end
		handleException(ivjExc);
	}
}
/**
 * connEtoC50:  (GroupDialog.window.windowOpened(java.awt.event.WindowEvent) --> VisualDCT.groupDialog_WindowOpened(Ljava.awt.event.WindowEvent;)V)
 * @param arg1 java.awt.event.WindowEvent
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private void connEtoC50(java.awt.event.WindowEvent arg1) {
	try {
		// user code begin {1}
		// user code end
		this.groupDialog_WindowOpened(arg1);
		// user code begin {2}
		// user code end
	} catch (java.lang.Throwable ivjExc) {
		// user code begin {3}
		// user code end
		handleException(ivjExc);
	}
}
/**
 * connEtoC51:  (NameTextField.action.actionPerformed(java.awt.event.ActionEvent) --> VisualDCT.oKButton_ActionPerformed(Ljava.awt.event.ActionEvent;)V)
 * @param arg1 java.awt.event.ActionEvent
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private void connEtoC51(java.awt.event.ActionEvent arg1) {
	try {
		// user code begin {1}
		// user code end
		this.oKButton_ActionPerformed(arg1);
		// user code begin {2}
		// user code end
	} catch (java.lang.Throwable ivjExc) {
		// user code begin {3}
		// user code end
		handleException(ivjExc);
	}
}
/**
 * connEtoC52:  (GroupOKButton.action.actionPerformed(java.awt.event.ActionEvent) --> VisualDCT.groupOKButton_ActionPerformed(Ljava.awt.event.ActionEvent;)V)
 * @param arg1 java.awt.event.ActionEvent
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private void connEtoC52(java.awt.event.ActionEvent arg1) {
	try {
		// user code begin {1}
		// user code end
		this.groupOKButton_ActionPerformed(arg1);
		// user code begin {2}
		// user code end
	} catch (java.lang.Throwable ivjExc) {
		// user code begin {3}
		// user code end
		handleException(ivjExc);
	}
}
/**
 * connEtoC53:  (groupNameTextField.action.actionPerformed(java.awt.event.ActionEvent) --> VisualDCT.groupOKButton_ActionPerformed(Ljava.awt.event.ActionEvent;)V)
 * @param arg1 java.awt.event.ActionEvent
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private void connEtoC53(java.awt.event.ActionEvent arg1) {
	try {
		// user code begin {1}
		// user code end
		this.groupOKButton_ActionPerformed(arg1);
		// user code begin {2}
		// user code end
	} catch (java.lang.Throwable ivjExc) {
		// user code begin {3}
		// user code end
		handleException(ivjExc);
	}
}

/**
 * connEtoC55:  (RenameOKButton.action.actionPerformed(java.awt.event.ActionEvent) --> VisualDCT.renameOKButton_ActionPerformed(Ljava.awt.event.ActionEvent;)V)
 * @param arg1 java.awt.event.ActionEvent
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private void connEtoC55(java.awt.event.ActionEvent arg1) {
	try {
		// user code begin {1}
		// user code end
		this.renameOKButton_ActionPerformed(arg1);
		// user code begin {2}
		// user code end
	} catch (java.lang.Throwable ivjExc) {
		// user code begin {3}
		// user code end
		handleException(ivjExc);
	}
}
/**
 * connEtoC56:  (NewNameTextField.action.actionPerformed(java.awt.event.ActionEvent) --> VisualDCT.renameOKButton_ActionPerformed(Ljava.awt.event.ActionEvent;)V)
 * @param arg1 java.awt.event.ActionEvent
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private void connEtoC56(java.awt.event.ActionEvent arg1) {
	try {
		// user code begin {1}
		// user code end
		this.renameOKButton_ActionPerformed(arg1);
		// user code begin {2}
		// user code end
	} catch (java.lang.Throwable ivjExc) {
		// user code begin {3}
		// user code end
		handleException(ivjExc);
	}
}
/**
 * connEtoC57:  (RenameDialog.window.windowOpened(java.awt.event.WindowEvent) --> VisualDCT.renameDialog_WindowOpened(Ljava.awt.event.WindowEvent;)V)
 * @param arg1 java.awt.event.WindowEvent
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private void connEtoC57(java.awt.event.WindowEvent arg1) {
	try {
		// user code begin {1}
		// user code end
		this.renameDialog_WindowOpened(arg1);
		// user code begin {2}
		// user code end
	} catch (java.lang.Throwable ivjExc) {
		// user code begin {3}
		// user code end
		handleException(ivjExc);
	}
}
/**
 * connEtoC58:  (NewRecordDialog.window.windowActivated(java.awt.event.WindowEvent) --> VisualDCT.newRecordDialog_WindowOpened(Ljava.awt.event.WindowEvent;)V)
 * @param arg1 java.awt.event.WindowEvent
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private void connEtoC58(java.awt.event.WindowEvent arg1) {
	try {
		// user code begin {1}
		// user code end
		this.newRecordDialog_WindowOpened(arg1);
		// user code begin {2}
		// user code end
	} catch (java.lang.Throwable ivjExc) {
		// user code begin {3}
		// user code end
		handleException(ivjExc);
	}
}

private void connEtoC67(java.awt.event.WindowEvent arg1) {
	try {
		// user code begin {1}
		// user code end
		this.morphingDialog_WindowOpened(arg1);
		// user code begin {2}
		// user code end
	} catch (java.lang.Throwable ivjExc) {
		// user code begin {3}
		// user code end
		handleException(ivjExc);
	}
}
/**
 * connEtoC59:  (GroupDialog.window.windowActivated(java.awt.event.WindowEvent) --> VisualDCT.groupDialog_WindowOpened(Ljava.awt.event.WindowEvent;)V)
 * @param arg1 java.awt.event.WindowEvent
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private void connEtoC59(java.awt.event.WindowEvent arg1) {
	try {
		// user code begin {1}
		// user code end
		this.groupDialog_WindowOpened(arg1);
		// user code begin {2}
		// user code end
	} catch (java.lang.Throwable ivjExc) {
		// user code begin {3}
		// user code end
		handleException(ivjExc);
	}
}
/**
 * connEtoC6:  (ZoomSlider.mouse.mouseReleased(java.awt.event.MouseEvent) --> VisualDCT.zoomSlider_MouseReleased(Ljava.awt.event.MouseEvent;)V)
 * @param arg1 java.awt.event.MouseEvent
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private void connEtoC6(java.awt.event.MouseEvent arg1) {
	try {
		// user code begin {1}
		// user code end
		this.zoomSlider_MouseReleased(arg1);
		// user code begin {2}
		// user code end
	} catch (java.lang.Throwable ivjExc) {
		// user code begin {3}
		// user code end
		handleException(ivjExc);
	}
}
/**
 * connEtoC60:  (RenameDialog.window.windowActivated(java.awt.event.WindowEvent) --> VisualDCT.renameDialog_WindowOpened(Ljava.awt.event.WindowEvent;)V)
 * @param arg1 java.awt.event.WindowEvent
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private void connEtoC60(java.awt.event.WindowEvent arg1) {
	try {
		// user code begin {1}
		// user code end
		this.renameDialog_WindowOpened(arg1);
		// user code begin {2}
		// user code end
	} catch (java.lang.Throwable ivjExc) {
		// user code begin {3}
		// user code end
		handleException(ivjExc);
	}
}
/**
 * connEtoC61:  (PageSetupMenuItem.action.actionPerformed(java.awt.event.ActionEvent) --> VisualDCT.pageSetupMenuItem_ActionPerformed(Ljava.awt.event.ActionEvent;)V)
 * @param arg1 java.awt.event.ActionEvent
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private void connEtoC61(java.awt.event.ActionEvent arg1) {
	try {
		// user code begin {1}
		// user code end
		this.pageSetupMenuItem_ActionPerformed(arg1);
		// user code begin {2}
		// user code end
	} catch (java.lang.Throwable ivjExc) {
		// user code begin {3}
		// user code end
		handleException(ivjExc);
	}
}
/**
 * connEtoC62:  (PrintPreviewMenuItem.action.actionPerformed(java.awt.event.ActionEvent) --> VisualDCT.printPreviewMenuItem_ActionPerformed(Ljava.awt.event.ActionEvent;)V)
 * @param arg1 java.awt.event.ActionEvent
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private void connEtoC62(java.awt.event.ActionEvent arg1) {
	try {
		// user code begin {1}
		// user code end
		this.printPreviewMenuItem_ActionPerformed(arg1);
		// user code begin {2}
		// user code end
	} catch (java.lang.Throwable ivjExc) {
		// user code begin {3}
		// user code end
		handleException(ivjExc);
	}
}
/**
 * connEtoC63:  (PluginManagerMenuItem.action.actionPerformed(java.awt.event.ActionEvent) --> VisualDCT.pluginManagerMenuItem_ActionPerformed(Ljava.awt.event.ActionEvent;)V)
 * @param arg1 java.awt.event.ActionEvent
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private void connEtoC63(java.awt.event.ActionEvent arg1) {
	try {
		// user code begin {1}
		// user code end
		this.pluginManagerMenuItem_ActionPerformed(arg1);
		// user code begin {2}
		// user code end
	} catch (java.lang.Throwable ivjExc) {
		// user code begin {3}
		// user code end
		handleException(ivjExc);
	}
}

private void connEtoC64(java.awt.event.ActionEvent arg1) {
	try {
		// user code begin {1}
		// user code end
		this.exportPostScriptFileMenuItem_ActionPerformed();

		// user code begin {2}
		// user code end
	} catch (java.lang.Throwable ivjExc) {
		// user code begin {3}
		// user code end
		handleException(ivjExc);
	}
}

private void connEtoC65(java.awt.event.ActionEvent arg1) {
	try {
		// user code begin {1}
		// user code end
		this.printAsPostScriptMenuItem_ActionPerformed();

		// user code begin {2}
		// user code end
	} catch (java.lang.Throwable ivjExc) {
		// user code begin {3}
		// user code end
		handleException(ivjExc);
	}
}

/**
 * connEtoC7:  (OpenMenuItem.action.actionPerformed(java.awt.event.ActionEvent) --> VisualDCT.openMenuItem_ActionPerformed()V)
 * @param arg1 java.awt.event.ActionEvent
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private void connEtoC7(java.awt.event.ActionEvent arg1) {
	try {
		// user code begin {1}
		// user code end
		this.openMenuItem_ActionPerformed();
		// user code begin {2}
		// user code end
	} catch (java.lang.Throwable ivjExc) {
		// user code begin {3}
		// user code end
		handleException(ivjExc);
	}
}
/**
 * connEtoC8:  (Import_DBMenuItem.action.actionPerformed(java.awt.event.ActionEvent) --> VisualDCT.import_DBMenuItem_ActionPerformed()V)
 * @param arg1 java.awt.event.ActionEvent
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private void connEtoC8(java.awt.event.ActionEvent arg1) {
	try {
		// user code begin {1}
		// user code end
		this.import_DBMenuItem_ActionPerformed();
		// user code begin {2}
		// user code end
	} catch (java.lang.Throwable ivjExc) {
		// user code begin {3}
		// user code end
		handleException(ivjExc);
	}
}
/**
 * connEtoC9:  (Import_DBDMenuItem.action.actionPerformed(java.awt.event.ActionEvent) --> VisualDCT.import_DBDMenuItem_ActionPerformed()V)
 * @param arg1 java.awt.event.ActionEvent
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private void connEtoC9(java.awt.event.ActionEvent arg1) {
	try {
		// user code begin {1}
		// user code end
		this.import_DBDMenuItem_ActionPerformed();
		// user code begin {2}
		// user code end
	} catch (java.lang.Throwable ivjExc) {
		// user code begin {3}
		// user code end
		handleException(ivjExc);
	}
}
/**
 * connEtoM1:  (CancelButton1.action.actionPerformed(java.awt.event.ActionEvent) --> GroupDialog.dispose()V)
 * @param arg1 java.awt.event.ActionEvent
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private void connEtoM1(java.awt.event.ActionEvent arg1) {
	try {
		// user code begin {1}
		// user code end
		getGroupDialog().dispose();
		// user code begin {2}
		// user code end
	} catch (java.lang.Throwable ivjExc) {
		// user code begin {3}
		// user code end
		handleException(ivjExc);
	}
}
/**
 * connEtoM2:  (JButton2.action.actionPerformed(java.awt.event.ActionEvent) --> NewRecordDialog.dispose()V)
 * @param arg1 java.awt.event.ActionEvent
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private void connEtoM2(java.awt.event.ActionEvent arg1) {
	try {
		// user code begin {1}
		// user code end
		getNewRecordDialog().dispose();
		// user code begin {2}
		// user code end
	} catch (java.lang.Throwable ivjExc) {
		// user code begin {3}
		// user code end
		handleException(ivjExc);
	}
}
private void connEtoM4(java.awt.event.ActionEvent arg1) {
	try {
		// user code begin {1}
		// user code end
		getMorphingDialog().dispose();
		// user code begin {2}
		// user code end
	} catch (java.lang.Throwable ivjExc) {
		// user code begin {3}
		// user code end
		handleException(ivjExc);
	}
}

/**
 * connEtoM3:  (RenameCancelButton.action.actionPerformed(java.awt.event.ActionEvent) --> RenameDialog.dispose()V)
 * @param arg1 java.awt.event.ActionEvent
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private void connEtoM3(java.awt.event.ActionEvent arg1) {
	try {
		// user code begin {1}
		// user code end
		getRenameDialog().dispose();
		// user code begin {2}
		// user code end
	} catch (java.lang.Throwable ivjExc) {
		// user code begin {3}
		// user code end
		handleException(ivjExc);
	}
}

public void systemCopy() {
    GetGUIInterface cmd = (GetGUIInterface)CommandManager.getInstance().getCommand("GetGUIMenuInterface");
    cmd.getGUIMenuInterface().systemCopy();
}
	
public void systemPaste() {
    GetGUIInterface cmd = (GetGUIInterface)CommandManager.getInstance().getCommand("GetGUIMenuInterface");
    cmd.getGUIMenuInterface().systemPaste();
}
	/**
 * Comment
 */
public void copyMenuItem_ActionPerformed() {
    GetGUIInterface cmd = (GetGUIInterface)CommandManager.getInstance().getCommand("GetGUIMenuInterface");
    cmd.getGUIMenuInterface().copy();
}
/**
 * Comment
 */
public void cutMenuItem_ActionPerformed() {
    GetGUIInterface cmd = (GetGUIInterface)CommandManager.getInstance().getCommand("GetGUIMenuInterface");
    cmd.getGUIMenuInterface().cut();
}
/**
 * Comment
 */
public void deleteMenuItem_ActionPerformed() {
    GetGUIInterface cmd = (GetGUIInterface)CommandManager.getInstance().getCommand("GetGUIMenuInterface");
    cmd.getGUIMenuInterface().delete();
}
/**
 * Comment
 */
public void exitMenuItem_ActionPerformed() {
	if (DataSynchronizer.getInstance().confirmFileClose(null, true)) {
	    this.dispose();
	}
}
/**
 * Comment
 */
public void exportPostScriptFileMenuItem_ActionPerformed()
{
	final GetPrintableInterface pi = (GetPrintableInterface)CommandManager.getInstance().getCommand("GetPrintableInterface");
	final Component ownerComponent = this;

	if(pi == null)
		return;

	class MyOwnPostScriptExportingThread extends Thread {
		
		private ViewState view = null;
		
		public MyOwnPostScriptExportingThread(ViewState view) {
			this.view = view;
		}
		
		public void run()
		{
			ByteArrayOutputStream byteArrayOutputStream = new ByteArrayOutputStream();
			FileOutputStream fileOutputStream = null;
				
			DocFlavor pageableFlavor = DocFlavor.SERVICE_FORMATTED.PAGEABLE;
		    String postScriptMime = new String("application/postscript");

			PrintRequestAttributeSet printRequestAttributeSet = Page.getPrintRequestAttributeSet();

			StreamPrintServiceFactory[] streamPrintServiceFactory =
				StreamPrintServiceFactory.lookupStreamPrintServiceFactories(pageableFlavor,
				postScriptMime);


			if(streamPrintServiceFactory.length == 0)
			{
				JOptionPane.showMessageDialog(VisualDCT.this, "Printing error", "No suitable PostScript factory found.", JOptionPane.ERROR_MESSAGE); 
				//System.err.println("Printing error: no suitable PostScript factory found.");
				return;
			}

			StreamPrintService streamPrintService =
				streamPrintServiceFactory[0].getPrintService(byteArrayOutputStream);
				
			PrinterJob printerJob = PrinterJob.getPrinterJob();
		
			printerJob.setPageable(pi.getPageable());

			boolean showGrid = Settings.getInstance().getShowGrid();

			try
			{
				printerJob.setPrintService(streamPrintService);
				
				PageFormat pageFormat = printerJob.pageDialog(printRequestAttributeSet);

				if(pageFormat == null)
					return;

				JFileChooser jFileChooser = getfileChooser();
				UniversalFileFilter universalFileFilter =
					new UniversalFileFilter(new String("ps"), "PostScript file");

				jFileChooser.resetChoosableFileFilters();
				jFileChooser.addChoosableFileFilter(universalFileFilter);
				jFileChooser.setDialogTitle("Save as...");
				jFileChooser.setFileSelectionMode(JFileChooser.FILES_ONLY);

				boolean jFileChooserConfirmed = false;
			    File outputFile = null;
				
				while(!jFileChooserConfirmed)
				{
					int jFileChooserOption = jFileChooser.showSaveDialog(ownerComponent);

					if(jFileChooserOption == JFileChooser.CANCEL_OPTION)
						return;

				    outputFile = jFileChooser.getSelectedFile();
						    
							// fix ending
					if (jFileChooser.getFileFilter().getDescription().startsWith("PostScript") &&
						!outputFile.getName().endsWith(".ps"))
						outputFile = new java.io.File(outputFile.getAbsoluteFile()+".ps");

					if(outputFile.exists())
					{
   						int jOptionPaneOption =
   							JOptionPane.showConfirmDialog(ownerComponent, "The file '"
   							+ outputFile.getName()
   							+ "' already exists! Overwrite?", "Confirmation",
   							JOptionPane.YES_NO_OPTION, JOptionPane.WARNING_MESSAGE);

    					if(jOptionPaneOption == JOptionPane.YES_OPTION)
   							jFileChooserConfirmed = true;
					}
					else
						jFileChooserConfirmed = true;
				}

				setCursor(java.awt.Cursor.getPredefinedCursor(java.awt.Cursor.WAIT_CURSOR));
				Thread.yield();

				pi.getPageable().getPageFormat(0).setOrientation(pageFormat.getOrientation());
				pi.getPageable().getPageFormat(0).setPaper(pageFormat.getPaper());

				Settings.getInstance().setShowGrid(false);

				printerJob.print();

				Settings.getInstance().setShowGrid(showGrid);
				
				fileOutputStream = new FileOutputStream(outputFile);
				fileOutputStream.write(printGrid(pi.getPageable().getPageFormat(0),
						new StringBuffer(byteArrayOutputStream.toString()), view));
				fileOutputStream.close();
			}
			catch(Exception exception)
			{
				exception.printStackTrace();
				com.cosylab.vdct.Console.getInstance().println("Printing error: " + exception);
			}
			finally
			{
				setCursor(java.awt.Cursor.getPredefinedCursor(java.awt.Cursor.DEFAULT_CURSOR));
				Settings.getInstance().setShowGrid(showGrid);
		
				CommandManager.getInstance().execute("RepaintWorkspace");
			}
		}
	};
    // Do nothing if no drawing surface.
	Group root = Group.getRoot();
	if (root == null) {
		return;
	}
	
	// Copy the current active view to prevent changes to it during thread run.
	ViewState view = new ViewState(ViewState.getInstance(root.getDsId()));
	new MyOwnPostScriptExportingThread(view).start();
}
/**
 * Comment
 */
public void flat_ViewMenuItem_ActionPerformed() {
    GetGUIInterface cmd = (GetGUIInterface)CommandManager.getInstance().getCommand("GetGUIMenuInterface");
    cmd.getGUIMenuInterface().setFlatView(getFlat_ViewMenuItem().isSelected());
}
/**
 * Return the About_BoxMenuItem property value.
 * @return javax.swing.JMenuItem
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JMenuItem getAbout_BoxMenuItem() {
	if (ivjAbout_BoxMenuItem == null) {
		try {
			ivjAbout_BoxMenuItem = new javax.swing.JMenuItem();
			ivjAbout_BoxMenuItem.setName("About_BoxMenuItem");
			ivjAbout_BoxMenuItem.setMnemonic('A');
			ivjAbout_BoxMenuItem.setText("About VisualDCT");
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjAbout_BoxMenuItem;
}
/**
 * Return the Base_ViewMenuItem property value.
 * @return javax.swing.JMenuItem
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JMenuItem getBase_ViewMenuItem() {
	if (ivjBase_ViewMenuItem == null) {
		try {
			ivjBase_ViewMenuItem = new javax.swing.JMenuItem();
			ivjBase_ViewMenuItem.setName("Base_ViewMenuItem");
			ivjBase_ViewMenuItem.setMnemonic('B');
			ivjBase_ViewMenuItem.setText("Base View");
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjBase_ViewMenuItem;
}
/**
 * Return the Books_OnlineMenuItem property value.
 * @return javax.swing.JMenuItem
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JMenuItem getBooks_OnlineMenuItem() {
	if (ivjBooks_OnlineMenuItem == null) {
		try {
			ivjBooks_OnlineMenuItem = new javax.swing.JMenuItem();
			ivjBooks_OnlineMenuItem.setName("Books_OnlineMenuItem");
			ivjBooks_OnlineMenuItem.setMnemonic('B');
			ivjBooks_OnlineMenuItem.setText("Books Online");
			ivjBooks_OnlineMenuItem.setEnabled(false);
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjBooks_OnlineMenuItem;
}

private javax.swing.JButton getBoxButton() {
	if (boxButton == null) {
		try {
			boxButton = new javax.swing.JButton();
			boxButton.setName("BoxButton");
			boxButton.setToolTipText("Box");
			boxButton.setText("");
			boxButton.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
			boxButton.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
			boxButton.setIcon(new javax.swing.ImageIcon(getClass().getResource("/images/boxn.gif")));
			boxButton.setMargin(new java.awt.Insets(0, 0, 0, 0));
		} catch (java.lang.Throwable ivjExc) {
			handleException(ivjExc);
		}
	}
	return boxButton;
}

public boolean getBoxButtonEnabled()
{
	return boxButtonEnabled;
}

/**
 * Return the ButtonPanel property value.
 * @return javax.swing.JPanel
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JPanel getButtonPanel() {
	if (ivjButtonPanel == null) {
		try {
			ivjButtonPanel = new javax.swing.JPanel();
			ivjButtonPanel.setName("ButtonPanel");
			ivjButtonPanel.setLayout(new java.awt.GridBagLayout());

			java.awt.GridBagConstraints constraintsOKButton = new java.awt.GridBagConstraints();
			constraintsOKButton.gridx = 0; constraintsOKButton.gridy = 0;
			constraintsOKButton.ipadx = 16;
			constraintsOKButton.insets = new java.awt.Insets(4, 4, 4, 4);
			getButtonPanel().add(getOKButton(), constraintsOKButton);

			java.awt.GridBagConstraints constraintsCancelButton = new java.awt.GridBagConstraints();
			constraintsCancelButton.gridx = 1; constraintsCancelButton.gridy = 0;
			constraintsCancelButton.insets = new java.awt.Insets(4, 4, 4, 4);
			getButtonPanel().add(getCancelButton(), constraintsCancelButton);
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjButtonPanel;
}

private javax.swing.JPanel getButtonPanel3() {
	if (ivjButtonPanel3 == null) {
		try {
			ivjButtonPanel3 = new javax.swing.JPanel();
			ivjButtonPanel3.setName("ButtonPanel3");
			ivjButtonPanel3.setLayout(new java.awt.GridBagLayout());

			java.awt.GridBagConstraints constraintsOKButton = new java.awt.GridBagConstraints();
			constraintsOKButton.gridx = 0; constraintsOKButton.gridy = 0;
			constraintsOKButton.ipadx = 16;
			constraintsOKButton.insets = new java.awt.Insets(4, 4, 4, 4);
			getButtonPanel3().add(getMorphingOKButton(), constraintsOKButton);

			java.awt.GridBagConstraints constraintsCancelButton = new java.awt.GridBagConstraints();
			constraintsCancelButton.gridx = 1; constraintsCancelButton.gridy = 0;
			constraintsCancelButton.insets = new java.awt.Insets(4, 4, 4, 4);
			getButtonPanel3().add(getMorphingCancelButton(), constraintsCancelButton);
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjButtonPanel3;
}

/**
 * Return the ButtonPanel1 property value.
 * @return javax.swing.JPanel
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JPanel getButtonPanel1() {
	if (ivjButtonPanel1 == null) {
		try {
			ivjButtonPanel1 = new javax.swing.JPanel();
			ivjButtonPanel1.setName("ButtonPanel1");
			ivjButtonPanel1.setLayout(new java.awt.GridBagLayout());

			java.awt.GridBagConstraints constraintsGroupOKButton = new java.awt.GridBagConstraints();
			constraintsGroupOKButton.gridx = 0; constraintsGroupOKButton.gridy = 0;
			constraintsGroupOKButton.ipadx = 16;
			constraintsGroupOKButton.insets = new java.awt.Insets(4, 4, 4, 4);
			getButtonPanel1().add(getGroupOKButton(), constraintsGroupOKButton);

			java.awt.GridBagConstraints constraintsCancelButton1 = new java.awt.GridBagConstraints();
			constraintsCancelButton1.gridx = 1; constraintsCancelButton1.gridy = 0;
			constraintsCancelButton1.insets = new java.awt.Insets(4, 4, 4, 4);
			getButtonPanel1().add(getCancelButton1(), constraintsCancelButton1);
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjButtonPanel1;
}
/**
 * Return the ButtonPanel2 property value.
 * @return javax.swing.JPanel
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JPanel getButtonPanel2() {
	if (ivjButtonPanel2 == null) {
		try {
			ivjButtonPanel2 = new javax.swing.JPanel();
			ivjButtonPanel2.setName("ButtonPanel2");
			ivjButtonPanel2.setLayout(new java.awt.GridBagLayout());

			java.awt.GridBagConstraints constraintsRenameOKButton = new java.awt.GridBagConstraints();
			constraintsRenameOKButton.gridx = 0; constraintsRenameOKButton.gridy = 0;
			constraintsRenameOKButton.ipadx = 16;
			constraintsRenameOKButton.insets = new java.awt.Insets(4, 4, 4, 4);
			getButtonPanel2().add(getRenameOKButton(), constraintsRenameOKButton);

			java.awt.GridBagConstraints constraintsRenameCancelButton = new java.awt.GridBagConstraints();
			constraintsRenameCancelButton.gridx = 1; constraintsRenameCancelButton.gridy = 0;
			constraintsRenameCancelButton.insets = new java.awt.Insets(4, 4, 4, 4);
			getButtonPanel2().add(getRenameCancelButton(), constraintsRenameCancelButton);
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjButtonPanel2;
}
/**
 * Return the JButton2 property value.
 * @return javax.swing.JButton
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JButton getCancelButton() {
	if (ivjCancelButton == null) {
		try {
			ivjCancelButton = new javax.swing.JButton();
			ivjCancelButton.setName("CancelButton");
			ivjCancelButton.setText("Cancel");
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjCancelButton;
}

private javax.swing.JButton getMorphingCancelButton() {
	if (ivjMorphingCancelButton == null) {
		try {
			ivjMorphingCancelButton = new javax.swing.JButton();
			ivjMorphingCancelButton.setName("MorphingCancelButton");
			ivjMorphingCancelButton.setText("Cancel");
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjMorphingCancelButton;
}
/**
 * Return the CancelButton1 property value.
 * @return javax.swing.JButton
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JButton getCancelButton1() {
	if (ivjCancelButton1 == null) {
		try {
			ivjCancelButton1 = new javax.swing.JButton();
			ivjCancelButton1.setName("CancelButton1");
			ivjCancelButton1.setText("Cancel");
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjCancelButton1;
}

/**
 * Return the CopyButton property value.
 * @return javax.swing.JButton
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JButton getCopyButton() {
	if (ivjCopyButton == null) {
		try {
			ivjCopyButton = new javax.swing.JButton();
			ivjCopyButton.setName("CopyButton");
			ivjCopyButton.setToolTipText("Copy");
			ivjCopyButton.setText("");
			ivjCopyButton.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
			ivjCopyButton.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
			ivjCopyButton.setIcon(new javax.swing.ImageIcon(getClass().getResource("/images/copy.gif")));
			ivjCopyButton.setMargin(new java.awt.Insets(0, 0, 0, 0));
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjCopyButton;
}
/**
 * Return the CopyMenuItem property value.
 * @return javax.swing.JMenuItem
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JMenuItem getCopyMenuItem() {
	if (ivjCopyMenuItem == null) {
		try {
			ivjCopyMenuItem = new javax.swing.JMenuItem();
			ivjCopyMenuItem.setName("CopyMenuItem");
			ivjCopyMenuItem.setMnemonic('C');
			ivjCopyMenuItem.setText("Copy");
			ivjCopyMenuItem.setAccelerator(javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_C,java.awt.Event.CTRL_MASK));
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjCopyMenuItem;
}

/**
 * Return the CopyMenuItem property value.
 * @return javax.swing.JMenuItem
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JMenuItem getSystemCopyMenuItem() {
	if (ivjSystemCopyMenuItem == null) {
		try {
			ivjSystemCopyMenuItem = new javax.swing.JMenuItem();
			ivjSystemCopyMenuItem.setName("SystemCopyMenuItem");
			ivjSystemCopyMenuItem.setText("Copy to System Clipboard");
			ivjSystemCopyMenuItem.setAccelerator(javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_C,java.awt.Event.CTRL_MASK|java.awt.Event.SHIFT_MASK));
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjSystemCopyMenuItem;
}
/**
 * Return the CutButton property value.
 * @return javax.swing.JButton
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JButton getCutButton() {
	if (ivjCutButton == null) {
		try {
			ivjCutButton = new javax.swing.JButton();
			ivjCutButton.setName("CutButton");
			ivjCutButton.setToolTipText("Cut");
			ivjCutButton.setText("");
			ivjCutButton.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
			ivjCutButton.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
			ivjCutButton.setIcon(new javax.swing.ImageIcon(getClass().getResource("/images/cut.gif")));
			ivjCutButton.setMargin(new java.awt.Insets(0, 0, 0, 0));
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjCutButton;
}
/**
 * Return the CutMenuItem property value.
 * @return javax.swing.JMenuItem
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JMenuItem getCutMenuItem() {
	if (ivjCutMenuItem == null) {
		try {
			ivjCutMenuItem = new javax.swing.JMenuItem();
			ivjCutMenuItem.setName("CutMenuItem");
			ivjCutMenuItem.setMnemonic('t');
			ivjCutMenuItem.setText("Cut");
			ivjCutMenuItem.setAccelerator(javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_X,java.awt.Event.CTRL_MASK));
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjCutMenuItem;
}
/**
 * Return the DebugMenu property value.
 * @return javax.swing.JMenu
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JMenu getDebugMenu() {
	if (ivjDebugMenu == null) {
		try {
			ivjDebugMenu = new javax.swing.JMenu();
			ivjDebugMenu.setName("DebugMenu");
			ivjDebugMenu.setMnemonic('D');
			ivjDebugMenu.setText("Debug");
			ivjDebugMenu.add(getStartDebugMenuItem());
			ivjDebugMenu.add(getStopDebugMenuItem());
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjDebugMenu;
}

private JMenu getRdbMenu() {
	if (rdbMenu == null) {
		try {
			rdbMenu = new JMenu();
			rdbMenu.setName("RdbMenu");
			rdbMenu.setMnemonic('I');
			rdbMenu.setText("Irmis");
			rdbMenu.add(getRdbConnectMenuItem());
			rdbMenu.add(getRdbLoadMenuItem());
			rdbMenu.add(getRdbSaveMenuItem());
			rdbMenu.add(getRdbSaveAsMenuItem());
		} catch (Throwable ivjExc) {
			handleException(ivjExc);
		}
	}
	return rdbMenu;
}

private JMenuItem getRdbConnectMenuItem() {
	if (rdbConnectMenuItem == null) {
		try {
			rdbConnectMenuItem = new JMenuItem();
			rdbConnectMenuItem.setName("Connect");
			rdbConnectMenuItem.setMnemonic('C');
			rdbConnectMenuItem.setText("Connect...");
			
			rdbConnectMenuItem.addActionListener(new ActionListener() {
				public void actionPerformed(ActionEvent e) {
					RdbInstance.getInstance(VisualDCT.getInstance()).getRdbInterface().connect();
				}
			});
		} catch (Throwable ivjExc) {
			handleException(ivjExc);
		}
	}
	return rdbConnectMenuItem;
}

private JMenuItem getRdbLoadMenuItem() {
	if (rdbLoadMenuItem == null) {
		try {
			rdbLoadMenuItem = new JMenuItem();
			rdbLoadMenuItem.setName("Load");
			rdbLoadMenuItem.setMnemonic('L');
			rdbLoadMenuItem.setText("Load...");
			
			rdbLoadMenuItem.addActionListener(new ActionListener() {
				public void actionPerformed(ActionEvent e) {
					GetGUIInterface cmd = (GetGUIInterface)CommandManager.getInstance().getCommand("GetGUIMenuInterface");
					cmd.getGUIMenuInterface().loadRdbGroup(VisualDCT.getInstance());
				}
			});
		} catch (Throwable ivjExc) {
			handleException(ivjExc);
		}
	}
	return rdbLoadMenuItem;
}

private JMenuItem getRdbSaveMenuItem() {
	if (rdbSaveMenuItem == null) {
		try {
			rdbSaveMenuItem = new JMenuItem();
			rdbSaveMenuItem.setName("Save");
			rdbSaveMenuItem.setMnemonic('S');
			rdbSaveMenuItem.setText("Save");
			
			rdbSaveMenuItem.addActionListener(new ActionListener() {
				public void actionPerformed(ActionEvent e) {
				    GetGUIInterface cmd = (GetGUIInterface)CommandManager.getInstance().getCommand("GetGUIMenuInterface");
		  		 	cmd.getGUIMenuInterface().saveRdbGroup(VisualDCT.getInstance(), false);
				}
			});
		} catch (Throwable ivjExc) {
			handleException(ivjExc);
		}
	}
	return rdbSaveMenuItem;
}

private JMenuItem getRdbSaveAsMenuItem() {
	if (rdbSaveAsMenuItem == null) {
		try {
			rdbSaveAsMenuItem = new JMenuItem();
			rdbSaveAsMenuItem.setName("Save as");
			rdbSaveAsMenuItem.setMnemonic('A');
			rdbSaveAsMenuItem.setText("Save as...");
			
			rdbSaveAsMenuItem.addActionListener(new ActionListener() {
				public void actionPerformed(ActionEvent e) {
				    GetGUIInterface cmd = (GetGUIInterface)CommandManager.getInstance().getCommand("GetGUIMenuInterface");
		  		 	cmd.getGUIMenuInterface().saveRdbGroup(VisualDCT.getInstance(), true);
				}
			});
		} catch (Throwable ivjExc) {
			handleException(ivjExc);
		}
	}
	return rdbSaveAsMenuItem;
}

/**
 * Return the DeleteMenuItem property value.
 * @return javax.swing.JMenuItem
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JMenuItem getDeleteMenuItem() {
	if (ivjDeleteMenuItem == null) {
		try {
			ivjDeleteMenuItem = new javax.swing.JMenuItem();
			ivjDeleteMenuItem.setName("DeleteMenuItem");
			ivjDeleteMenuItem.setMnemonic('D');
			ivjDeleteMenuItem.setText("Delete");
			ivjDeleteMenuItem.setAccelerator(javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_DELETE,0));
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjDeleteMenuItem;
}
/**
 * Return the EditMenu property value.
 * @return javax.swing.JMenu
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JMenu getEditMenu() {
	if (ivjEditMenu == null) {
		try {
			ivjEditMenu = new javax.swing.JMenu();
			ivjEditMenu.setName("EditMenu");
			ivjEditMenu.setMnemonic('E');
			ivjEditMenu.setText("Edit");
			ivjEditMenu.add(getUndoMenuItem());
			ivjEditMenu.add(getRedoMenuItem());
			ivjEditMenu.add(getJSeparator4());
			ivjEditMenu.add(getCutMenuItem());
			ivjEditMenu.add(getCopyMenuItem());
			ivjEditMenu.add(getPasteMenuItem());
			ivjEditMenu.add(getMove_RenameMenuItem());
			ivjEditMenu.add(getMorphMenuItem());
			ivjEditMenu.add(getJSeparator5());
			ivjEditMenu.add(getGroupMenuItem());
			ivjEditMenu.add(getUngroupMenuItem());
			ivjEditMenu.add(getJSeparator6());
			ivjEditMenu.add(getDeleteMenuItem());
			ivjEditMenu.add(getSelect_AllMenuItem());
			ivjEditMenu.add(new JSeparator());
			ivjEditMenu.add(getFindMenuItem());
			ivjEditMenu.add(new JSeparator());
			ivjEditMenu.add(getSystemCopyMenuItem());
			ivjEditMenu.add(getSystemPasteMenuItem());
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjEditMenu;
}
/**
 * Return the ExitMenuItem property value.
 * @return javax.swing.JMenuItem
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JMenuItem getExitMenuItem() {
	if (ivjExitMenuItem == null) {
		try {
			ivjExitMenuItem = new javax.swing.JMenuItem();
			ivjExitMenuItem.setName("ExitMenuItem");
			ivjExitMenuItem.setMnemonic('x');
			ivjExitMenuItem.setText("Exit");
			ivjExitMenuItem.setAccelerator(javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_Q,java.awt.Event.CTRL_MASK));
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjExitMenuItem;
}
/**
 * Return the ExportMenuItem property value.
 * @return com.cosylab.vdct.plugin.export.ExportMenu
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private com.cosylab.vdct.plugin.export.ExportMenu getExportMenuItem() {
	if (ivjExportMenuItem == null) {
		try {
			ivjExportMenuItem = new com.cosylab.vdct.plugin.export.ExportMenu();
			ivjExportMenuItem.setName("ExportMenuItem");
			ivjExportMenuItem.setText("Export");
			// user code begin {1}
			ivjExportMenuItem.init();
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjExportMenuItem;
}
/**
 * Return the ExportMenuItem property value.
 * @return com.cosylab.vdct.plugin.export.ExportMenu
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private com.cosylab.vdct.plugin.menu.ToolsMenu getToolsMenu() {
	if (ivjToolsMenu == null) {
		try {
			ivjToolsMenu = new com.cosylab.vdct.plugin.menu.ToolsMenu();
			ivjToolsMenu.setName("ToolsMenu");
			ivjToolsMenu.setText("Tools");
			// user code begin {1}
			ivjToolsMenu.init();
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjToolsMenu;
}
/**
 * Return the ExportPostScriptFileMenuItem property value.
 * @return javax.swing.JMenuItem
 */
private javax.swing.JMenuItem getExportPostScriptFileMenuItem() {
	if (ivjExportPostScriptFileMenuItem == null) {
		try {
			ivjExportPostScriptFileMenuItem = new javax.swing.JMenuItem();
			ivjExportPostScriptFileMenuItem.setName("ExportPostScriptFileMenuItem");
			ivjExportPostScriptFileMenuItem.setText("Export to PostScript...");
			ivjExportPostScriptFileMenuItem.setEnabled(true);
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjExportPostScriptFileMenuItem;
}
/**
 * NOTE: everytime this method is called "JFileChooser.setSelectedFile" is reset. 
 * Return the fileChooser property value.
 * @return javax.swing.JFileChooser
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
public javax.swing.JFileChooser getfileChooser() {
	if (ivjfileChooser == null) {
		try {
			ivjfileChooser = new javax.swing.JFileChooser();
			ivjfileChooser.setName("fileChooser");
			ivjfileChooser.setBounds(411, 857, 472, 303);
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	if (ivjfileChooser != null)
	{
		final File blankFile = new File(""); 
		ivjfileChooser.setSelectedFile(blankFile);
	}
		
	return ivjfileChooser;
}
/**
 * Return the FileMenu property value.
 * @return javax.swing.JMenu
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JMenu getFileMenu() {
	if (ivjFileMenu == null) {
		try {
			ivjFileMenu = new javax.swing.JMenu();
			ivjFileMenu.setName("FileMenu");
			ivjFileMenu.setMnemonic('F');
			ivjFileMenu.setText("File");
			ivjFileMenu.add(getNewMenuItem());
			ivjFileMenu.add(getOpenMenuItem());
			ivjFileMenu.add(getImport_DBMenuItem());
			ivjFileMenu.add(getImportFieldsMenuItem());
			ivjFileMenu.add(getImportTemplate_DBMenuItem());
			ivjFileMenu.add(getImportBorderMenuItem());
			ivjFileMenu.add(getImport_DBDMenuItem());
			ivjFileMenu.add(getJSeparator1());
			ivjFileMenu.add(getSaveMenuItem());
			ivjFileMenu.add(getSave_AsMenuItem());
			ivjFileMenu.add(getSave_As_GroupMenuItem());
			//ivjFileMenu.add(getSaveAsTemplateMenuItem());
			ivjFileMenu.add(getCloseMenuItem());
			ivjFileMenu.add(getCloseAllMenuItem());
			ivjFileMenu.add(getJSeparator11a());
			ivjFileMenu.add(getGenerateMenuItem());
			ivjFileMenu.add(getGenerateAsGroupMenuItem());
			ivjFileMenu.add(getJSeparator11());
			ivjFileMenu.add(getExportMenuItem());
			ivjFileMenu.add(getExportPostScriptFileMenuItem());
			ivjFileMenu.add(getJSeparator2());
			ivjFileMenu.add(getPrintMenuItem());
			ivjFileMenu.add(getPrintAsPostScriptMenuItem());
			ivjFileMenu.add(getPrintPreviewMenuItem());
			ivjFileMenu.add(getPageSetupMenuItem());
			ivjFileMenu.add(getJSeparator3());
			ivjFileMenu.add(getRecentFilesMenuItem());
			ivjFileMenu.add(getJSeparator13());
			ivjFileMenu.add(getExitMenuItem());
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjFileMenu;
}
/**
 * Return the Find_ReplaceButton property value.
 * @return javax.swing.JButton
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JButton getFindButton() {
	if (ivjFind_ReplaceButton == null) {
		try {
			ivjFind_ReplaceButton = new javax.swing.JButton();
			ivjFind_ReplaceButton.setName("Find_ReplaceButton");
			ivjFind_ReplaceButton.setToolTipText("Find");
			ivjFind_ReplaceButton.setText("");
			ivjFind_ReplaceButton.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
			ivjFind_ReplaceButton.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
			ivjFind_ReplaceButton.setIcon(new javax.swing.ImageIcon(getClass().getResource("/images/find.gif")));
			ivjFind_ReplaceButton.setMargin(new java.awt.Insets(0, 0, 0, 0));
			ivjFind_ReplaceButton.setEnabled(true);
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjFind_ReplaceButton;
}
/**
 * Return the Flat_ViewMenuItem property value.
 * @return javax.swing.JCheckBoxMenuItem
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JCheckBoxMenuItem getFlat_ViewMenuItem() {
	if (ivjFlat_ViewMenuItem == null) {
		try {
			ivjFlat_ViewMenuItem = new javax.swing.JCheckBoxMenuItem();
			ivjFlat_ViewMenuItem.setName("Flat_ViewMenuItem");
			ivjFlat_ViewMenuItem.setText("Flat View");
			ivjFlat_ViewMenuItem.setVisible(true);
			ivjFlat_ViewMenuItem.setEnabled(false);
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjFlat_ViewMenuItem;
}

/**
 * Return the GroupDialog property value.
 * @return javax.swing.JDialog
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JDialog getGroupDialog() {
	if (ivjGroupDialog == null) {
		try {
			ivjGroupDialog = new javax.swing.JDialog(this);
			ivjGroupDialog.setName("GroupDialog");
			ivjGroupDialog.setDefaultCloseOperation(javax.swing.WindowConstants.DISPOSE_ON_CLOSE);
			ivjGroupDialog.setResizable(false);
			ivjGroupDialog.setBounds(36, 1181, 371, 120);
			ivjGroupDialog.setModal(true);
			ivjGroupDialog.setTitle("Group");
			getGroupDialog().setContentPane(getJDialogContentPane1());
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjGroupDialog;
}
/**
 * Return the GroupMenuItem property value.
 * @return javax.swing.JMenuItem
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JMenuItem getGroupMenuItem() {
	if (ivjGroupMenuItem == null) {
		try {
			ivjGroupMenuItem = new javax.swing.JMenuItem();
			ivjGroupMenuItem.setName("GroupMenuItem");
			ivjGroupMenuItem.setMnemonic('G');
			ivjGroupMenuItem.setText("Group");
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjGroupMenuItem;
}
/**
 * Return the groupNameTextField property value.
 * @return javax.swing.JTextField
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JTextField getGroupNameTextField() {
	if (ivjgroupNameTextField == null) {
		try {
			ivjgroupNameTextField = new javax.swing.JTextField();
			ivjgroupNameTextField.setName("groupNameTextField");
			// user code begin {1}

			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjgroupNameTextField;
}

protected NameChecker getNewGroupNameChecker() {
	if (newGroupNameChecker == null) {
		newGroupNameChecker = new NameChecker(getGroupNameTextField(), null, true, getGroupOKButton(), getGroupWarningLabel());
	}
	return newGroupNameChecker;
}

/**
 * Return the GroupOKButton property value.
 * @return javax.swing.JButton
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JButton getGroupOKButton() {
	if (ivjGroupOKButton == null) {
		try {
			ivjGroupOKButton = new javax.swing.JButton();
			ivjGroupOKButton.setName("GroupOKButton");
			ivjGroupOKButton.setText("OK");
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjGroupOKButton;
}
/**
 * Return the GroupWarningLabel property value.
 * @return javax.swing.JLabel
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JLabel getGroupWarningLabel() {
	if (ivjGroupWarningLabel == null) {
		try {
			ivjGroupWarningLabel = new javax.swing.JLabel();
			ivjGroupWarningLabel.setName("GroupWarningLabel");
			ivjGroupWarningLabel.setText(" ");
			ivjGroupWarningLabel.setForeground(java.awt.Color.red);
			ivjGroupWarningLabel.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
			ivjGroupWarningLabel.setFont(new java.awt.Font("dialog", 0, 10));
			ivjGroupWarningLabel.setHorizontalAlignment(javax.swing.SwingConstants.CENTER);
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjGroupWarningLabel;
}
/**
 * Return the Help_TopicsMenuItem property value.
 * @return javax.swing.JMenuItem
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JMenuItem getHelp_TopicsMenuItem() {
	if (ivjHelp_TopicsMenuItem == null) {
		try {
			ivjHelp_TopicsMenuItem = new javax.swing.JMenuItem();
			ivjHelp_TopicsMenuItem.setName("Help_TopicsMenuItem");
			ivjHelp_TopicsMenuItem.setMnemonic('T');
			ivjHelp_TopicsMenuItem.setText("Help Topics");
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjHelp_TopicsMenuItem;
}
/**
 * Return the HelpMenu property value.
 * @return javax.swing.JMenu
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JMenu getHelpMenu() {
	if (ivjHelpMenu == null) {
		try {
			ivjHelpMenu = new javax.swing.JMenu();
			ivjHelpMenu.setName("HelpMenu");
			ivjHelpMenu.setMnemonic('H');
			ivjHelpMenu.setText("Help");
			ivjHelpMenu.add(getHelp_TopicsMenuItem());
			ivjHelpMenu.add(getBooks_OnlineMenuItem());
			ivjHelpMenu.add(getJSeparator12());
			ivjHelpMenu.add(getAbout_BoxMenuItem());
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjHelpMenu;
}
/**
 * Return the Import_DBDMenuItem property value.
 * @return javax.swing.JMenuItem
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JMenuItem getImport_DBDMenuItem() {
	if (ivjImport_DBDMenuItem == null) {
		try {
			ivjImport_DBDMenuItem = new javax.swing.JMenuItem();
			ivjImport_DBDMenuItem.setName("Import_DBDMenuItem");
			ivjImport_DBDMenuItem.setText("Manage DBDs...");
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjImport_DBDMenuItem;
}
/**
 * Return the Import_DBMenuItem property value.
 * @return javax.swing.JMenuItem
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JMenuItem getImport_DBMenuItem() {
	if (ivjImport_DBMenuItem == null) {
		try {
			ivjImport_DBMenuItem = new javax.swing.JMenuItem();
			ivjImport_DBMenuItem.setName("Import_DBMenuItem");
			ivjImport_DBMenuItem.setText("Import...");
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjImport_DBMenuItem;
}

/**
 * Return the Import_DBMenuItem property value.
 * @return javax.swing.JMenuItem
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JMenuItem getImportTemplate_DBMenuItem() {
	if (ivjImportTemplate_DBMenuItem == null) {
		try {
			ivjImportTemplate_DBMenuItem = new javax.swing.JMenuItem();
			ivjImportTemplate_DBMenuItem.setName("ivjImportTemplate_DBMenuItem");
			ivjImportTemplate_DBMenuItem.setText("Import template...");
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjImportTemplate_DBMenuItem;
}

/**
 * Return the ImportFieldsMenuItem property value.
 * @return javax.swing.JMenuItem
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JMenuItem getImportFieldsMenuItem() {
	if (ivjImportFieldsMenuItem == null) {
		try {
			ivjImportFieldsMenuItem = new javax.swing.JMenuItem();
			ivjImportFieldsMenuItem.setName("ImportFieldsMenuItem");
			ivjImportFieldsMenuItem.setText("Import fields...");
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjImportFieldsMenuItem;
}

/**
 * Return the ImportBorderMenuItem property value.
 * @return javax.swing.JMenuItem
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JMenuItem getImportBorderMenuItem() {
	if (ivjImportBorderMenuItem == null) {
		try {
			ivjImportBorderMenuItem = new javax.swing.JMenuItem();
			ivjImportBorderMenuItem.setName("ImportBorderMenuItem");
			ivjImportBorderMenuItem.setText("Import border...");
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjImportBorderMenuItem;
}

// shp: not final solution
public static VisualDCT getInstance()
{
	return instance;
}

/**
 * Return the JDialogContentPane property value.
 * @return javax.swing.JPanel
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JPanel getJDialogContentPane() {
	if (ivjJDialogContentPane == null) {
		try {
			ivjJDialogContentPane = new javax.swing.JPanel();
			ivjJDialogContentPane.setName("JDialogContentPane");
			ivjJDialogContentPane.setLayout(new java.awt.GridBagLayout());

			java.awt.GridBagConstraints constraintsTypeComboBox = new java.awt.GridBagConstraints();
			constraintsTypeComboBox.gridx = 1; constraintsTypeComboBox.gridy = 0;
			constraintsTypeComboBox.fill = java.awt.GridBagConstraints.HORIZONTAL;
			constraintsTypeComboBox.weightx = 1.0;
			constraintsTypeComboBox.insets = new java.awt.Insets(14, 4, 4, 18);
			getJDialogContentPane().add(getTypeComboBox(), constraintsTypeComboBox);

			java.awt.GridBagConstraints constraintsTypeLabel = new java.awt.GridBagConstraints();
			constraintsTypeLabel.gridx = 0; constraintsTypeLabel.gridy = 0;
			constraintsTypeLabel.insets = new java.awt.Insets(14, 18, 4, 4);
			getJDialogContentPane().add(getTypeLabel(), constraintsTypeLabel);

			java.awt.GridBagConstraints constraintsNameLabel = new java.awt.GridBagConstraints();
			constraintsNameLabel.gridx = 0; constraintsNameLabel.gridy = 1;
			constraintsNameLabel.insets = new java.awt.Insets(4, 18, 4, 4);
			getJDialogContentPane().add(getNameLabel(), constraintsNameLabel);

			java.awt.GridBagConstraints constraintsNameTextField = new java.awt.GridBagConstraints();
			constraintsNameTextField.gridx = 1; constraintsNameTextField.gridy = 1;
			constraintsNameTextField.fill = java.awt.GridBagConstraints.HORIZONTAL;
			constraintsNameTextField.weightx = 1.0;
			constraintsNameTextField.insets = new java.awt.Insets(4, 4, 4, 18);
			getJDialogContentPane().add(getNameTextField(), constraintsNameTextField);

			java.awt.GridBagConstraints constraintsButtonPanel = new java.awt.GridBagConstraints();
			constraintsButtonPanel.gridx = 0; constraintsButtonPanel.gridy = 3;
			constraintsButtonPanel.gridwidth = 2;
			constraintsButtonPanel.fill = java.awt.GridBagConstraints.BOTH;
			constraintsButtonPanel.weightx = 1.0;
			constraintsButtonPanel.insets = new java.awt.Insets(8, 4, 8, 4);
			getJDialogContentPane().add(getButtonPanel(), constraintsButtonPanel);

			java.awt.GridBagConstraints constraintsWarningLabel = new java.awt.GridBagConstraints();
			constraintsWarningLabel.gridx = 0; constraintsWarningLabel.gridy = 2;
			constraintsWarningLabel.gridwidth = 2;
			constraintsWarningLabel.fill = java.awt.GridBagConstraints.HORIZONTAL;
			constraintsWarningLabel.insets = new java.awt.Insets(4, 18, 4, 18);
			getJDialogContentPane().add(getWarningLabel(), constraintsWarningLabel);
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjJDialogContentPane;
}

private javax.swing.JPanel getJDialogContentPane3() {
	if (ivjJDialogContentPane3 == null) {
		try {
			ivjJDialogContentPane3 = new javax.swing.JPanel();
			ivjJDialogContentPane3.setName("JDialogContentPane3");
			ivjJDialogContentPane3.setLayout(new java.awt.GridBagLayout());

			java.awt.GridBagConstraints constraintsTypeComboBox = new java.awt.GridBagConstraints();
			constraintsTypeComboBox.gridx = 1; constraintsTypeComboBox.gridy = 1;
			constraintsTypeComboBox.fill = java.awt.GridBagConstraints.HORIZONTAL;
			constraintsTypeComboBox.weightx = 1.0;
			constraintsTypeComboBox.insets = new java.awt.Insets(4, 4, 4, 18);
			getJDialogContentPane3().add(getTypeComboBox2(), constraintsTypeComboBox);

			java.awt.GridBagConstraints constraintsTypeLabel = new java.awt.GridBagConstraints();
			constraintsTypeLabel.gridx = 0; constraintsTypeLabel.gridy = 1;
			constraintsTypeLabel.insets = new java.awt.Insets(4, 18, 4, 4);
			getJDialogContentPane3().add(getTypeLabel2(), constraintsTypeLabel);

			java.awt.GridBagConstraints constraintsNameLabel = new java.awt.GridBagConstraints();
			constraintsNameLabel.gridx = 0; constraintsNameLabel.gridy = 0;
			constraintsNameLabel.insets = new java.awt.Insets(14, 18, 4, 4);
			getJDialogContentPane3().add(getNameLabel3(), constraintsNameLabel);

			java.awt.GridBagConstraints constraintsNameTextField = new java.awt.GridBagConstraints();
			constraintsNameTextField.gridx = 1; constraintsNameTextField.gridy = 0;
			constraintsNameTextField.fill = java.awt.GridBagConstraints.HORIZONTAL;
			constraintsNameTextField.weightx = 1.0;
			constraintsNameTextField.insets = new java.awt.Insets(14, 4, 4, 18);
			getJDialogContentPane3().add(getNameTextLabel(), constraintsNameTextField);

			java.awt.GridBagConstraints constraintsButtonPanel = new java.awt.GridBagConstraints();
			constraintsButtonPanel.gridx = 0; constraintsButtonPanel.gridy = 2;
			constraintsButtonPanel.gridwidth = 2;
			constraintsButtonPanel.fill = java.awt.GridBagConstraints.BOTH;
			constraintsButtonPanel.weightx = 1.0;
			constraintsButtonPanel.insets = new java.awt.Insets(8, 4, 8, 4);
			getJDialogContentPane3().add(getButtonPanel3(), constraintsButtonPanel);

			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjJDialogContentPane3;
}
/**
 * Return the JDialogContentPane1 property value.
 * @return javax.swing.JPanel
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JPanel getJDialogContentPane1() {
	if (ivjJDialogContentPane1 == null) {
		try {
			ivjJDialogContentPane1 = new javax.swing.JPanel();
			ivjJDialogContentPane1.setName("JDialogContentPane1");
			ivjJDialogContentPane1.setLayout(new java.awt.GridBagLayout());

			java.awt.GridBagConstraints constraintsNameLabel1 = new java.awt.GridBagConstraints();
			constraintsNameLabel1.gridx = 0; constraintsNameLabel1.gridy = 0;
			constraintsNameLabel1.insets = new java.awt.Insets(20, 18, 4, 4);
			getJDialogContentPane1().add(getNameLabel1(), constraintsNameLabel1);

			java.awt.GridBagConstraints constraintsgroupNameTextField = new java.awt.GridBagConstraints();
			constraintsgroupNameTextField.gridx = 1; constraintsgroupNameTextField.gridy = 0;
			constraintsgroupNameTextField.fill = java.awt.GridBagConstraints.HORIZONTAL;
			constraintsgroupNameTextField.weightx = 1.0;
			constraintsgroupNameTextField.insets = new java.awt.Insets(20, 4, 4, 18);
			getJDialogContentPane1().add(getGroupNameTextField(), constraintsgroupNameTextField);

			java.awt.GridBagConstraints constraintsButtonPanel1 = new java.awt.GridBagConstraints();
			constraintsButtonPanel1.gridx = 0; constraintsButtonPanel1.gridy = 2;
			constraintsButtonPanel1.gridwidth = 2;
			constraintsButtonPanel1.fill = java.awt.GridBagConstraints.BOTH;
			constraintsButtonPanel1.weightx = 1.0;
			constraintsButtonPanel1.insets = new java.awt.Insets(8, 4, 8, 4);
			getJDialogContentPane1().add(getButtonPanel1(), constraintsButtonPanel1);

			java.awt.GridBagConstraints constraintsGroupWarningLabel = new java.awt.GridBagConstraints();
			constraintsGroupWarningLabel.gridx = 0; constraintsGroupWarningLabel.gridy = 1;
			constraintsGroupWarningLabel.gridwidth = 2;
			constraintsGroupWarningLabel.fill = java.awt.GridBagConstraints.HORIZONTAL;
			constraintsGroupWarningLabel.insets = new java.awt.Insets(4, 18, 4, 18);
			getJDialogContentPane1().add(getGroupWarningLabel(), constraintsGroupWarningLabel);
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjJDialogContentPane1;
}
/**
 * Return the JDialogContentPane2 property value.
 * @return javax.swing.JPanel
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JPanel getJDialogContentPane2() {
	if (ivjJDialogContentPane2 == null) {
		try {
			ivjJDialogContentPane2 = new javax.swing.JPanel();
			ivjJDialogContentPane2.setName("JDialogContentPane2");
			ivjJDialogContentPane2.setLayout(new java.awt.GridBagLayout());

			java.awt.GridBagConstraints constraintsTypeLabel1 = new java.awt.GridBagConstraints();
			constraintsTypeLabel1.gridx = 0; constraintsTypeLabel1.gridy = 0;
			constraintsTypeLabel1.insets = new java.awt.Insets(14, 18, 4, 4);
			getJDialogContentPane2().add(getTypeLabel1(), constraintsTypeLabel1);

			java.awt.GridBagConstraints constraintsNameLabel2 = new java.awt.GridBagConstraints();
			constraintsNameLabel2.gridx = 0; constraintsNameLabel2.gridy = 1;
			constraintsNameLabel2.insets = new java.awt.Insets(4, 18, 4, 4);
			getJDialogContentPane2().add(getNameLabel2(), constraintsNameLabel2);

			java.awt.GridBagConstraints constraintsNewNameTextField = new java.awt.GridBagConstraints();
			constraintsNewNameTextField.gridx = 1; constraintsNewNameTextField.gridy = 1;
			constraintsNewNameTextField.fill = java.awt.GridBagConstraints.HORIZONTAL;
			constraintsNewNameTextField.weightx = 1.0;
			constraintsNewNameTextField.insets = new java.awt.Insets(4, 4, 4, 18);
			getJDialogContentPane2().add(getNewNameTextField(), constraintsNewNameTextField);

			java.awt.GridBagConstraints constraintsButtonPanel2 = new java.awt.GridBagConstraints();
			constraintsButtonPanel2.gridx = 0; constraintsButtonPanel2.gridy = 3;
			constraintsButtonPanel2.gridwidth = 2;
			constraintsButtonPanel2.fill = java.awt.GridBagConstraints.BOTH;
			constraintsButtonPanel2.weightx = 1.0;
			constraintsButtonPanel2.insets = new java.awt.Insets(8, 4, 8, 4);
			getJDialogContentPane2().add(getButtonPanel2(), constraintsButtonPanel2);

			java.awt.GridBagConstraints constraintsRenameWarningLabel = new java.awt.GridBagConstraints();
			constraintsRenameWarningLabel.gridx = 0; constraintsRenameWarningLabel.gridy = 2;
			constraintsRenameWarningLabel.gridwidth = 2;
			constraintsRenameWarningLabel.fill = java.awt.GridBagConstraints.HORIZONTAL;
			constraintsRenameWarningLabel.insets = new java.awt.Insets(4, 18, 4, 18);
			getJDialogContentPane2().add(getRenameWarningLabel(), constraintsRenameWarningLabel);

			java.awt.GridBagConstraints constraintsOldNameLabel = new java.awt.GridBagConstraints();
			constraintsOldNameLabel.gridx = 1; constraintsOldNameLabel.gridy = 0;
			constraintsOldNameLabel.fill = java.awt.GridBagConstraints.HORIZONTAL;
			constraintsOldNameLabel.weightx = 1.0;
			constraintsOldNameLabel.insets = new java.awt.Insets(14, 4, 4, 18);
			getJDialogContentPane2().add(getOldNameLabel(), constraintsOldNameLabel);
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjJDialogContentPane2;
}
/**
 * Return the JFrameContentPane property value.
 * @return javax.swing.JPanel
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JPanel getJFrameContentPane() {
	if (ivjJFrameContentPane == null) {
		try {
			ivjJFrameContentPane = new javax.swing.JPanel();
			ivjJFrameContentPane.setName("JFrameContentPane");
			ivjJFrameContentPane.setLayout(new java.awt.BorderLayout());
			getJFrameContentPane().add(getToolBarPane(), BorderLayout.NORTH);
			getJFrameContentPane().add(getStatusBarPane(), BorderLayout.SOUTH);
			getJFrameContentPane().add(getworkspace(), BorderLayout.CENTER);
			CommandManager.getInstance().addCommand("SetCursor", new SetCursorCommand(ivjJFrameContentPane));
			KeyEventManager.getInstance().registerSubscriber("ContentPane", ivjJFrameContentPane);
			ivjJFrameContentPane.setFocusable(true);
			ivjJFrameContentPane.addMouseListener(new MouseAdapter() {
			    public void mouseEntered(MouseEvent e) {
			        if (VisualDCT.this.isActive()) {	
			        	ivjJFrameContentPane.requestFocus();
			        }
			    }
			});
			
            // In Java > 1.6.0_0 the hidden items in menu do not activate for key inputs, map here: 
			InputMap map = ivjJFrameContentPane.getInputMap(JComponent.WHEN_ANCESTOR_OF_FOCUSED_COMPONENT);
			map.put(KeyStroke.getKeyStroke(KeyEvent.VK_UP, KeyEvent.CTRL_DOWN_MASK), "ctrlUp");
			map.put(KeyStroke.getKeyStroke(KeyEvent.VK_DOWN, KeyEvent.CTRL_DOWN_MASK), "ctrlDown");
			map.put(KeyStroke.getKeyStroke(KeyEvent.VK_LEFT, KeyEvent.CTRL_DOWN_MASK), "ctrlLeft");
			map.put(KeyStroke.getKeyStroke(KeyEvent.VK_RIGHT, KeyEvent.CTRL_DOWN_MASK), "ctrlRight");

			ivjJFrameContentPane.getActionMap().put("ctrlUp", new AbstractAction() {
				public void actionPerformed(ActionEvent arg0) {
					VisualDCT.this.moveOrigin(SwingConstants.SOUTH);
				}
	        });
			ivjJFrameContentPane.getActionMap().put("ctrlDown", new AbstractAction() {
				public void actionPerformed(ActionEvent arg0) {
					VisualDCT.this.moveOrigin(SwingConstants.NORTH);
				}
	        });
			ivjJFrameContentPane.getActionMap().put("ctrlLeft", new AbstractAction() {
				public void actionPerformed(ActionEvent arg0) {
					VisualDCT.this.moveOrigin(SwingConstants.WEST);
				}
	        });
			ivjJFrameContentPane.getActionMap().put("ctrlRight", new AbstractAction() {
				public void actionPerformed(ActionEvent arg0) {
					VisualDCT.this.moveOrigin(SwingConstants.EAST);
				}
	        });
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjJFrameContentPane;
}
/**
 * Return the JSeparator1 property value.
 * @return javax.swing.JSeparator
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JSeparator getJSeparator1() {
	if (ivjJSeparator1 == null) {
		try {
			ivjJSeparator1 = new javax.swing.JSeparator();
			ivjJSeparator1.setName("JSeparator1");
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjJSeparator1;
}
/**
 * Return the JSeparator10 property value.
 * @return javax.swing.JSeparator
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JSeparator getJSeparator10() {
	if (ivjJSeparator10 == null) {
		try {
			ivjJSeparator10 = new javax.swing.JSeparator();
			ivjJSeparator10.setName("JSeparator10");
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjJSeparator10;
}
/**
 * Return the JSeparator11 property value.
 * @return javax.swing.JSeparator
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JSeparator getJSeparator11() {
	if (ivjJSeparator11 == null) {
		try {
			ivjJSeparator11 = new javax.swing.JSeparator();
			ivjJSeparator11.setName("JSeparator11");
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjJSeparator11;
}
/**
 * Return the JSeparator12 property value.
 * @return javax.swing.JSeparator
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JSeparator getJSeparator12() {
	if (ivjJSeparator12 == null) {
		try {
			ivjJSeparator12 = new javax.swing.JSeparator();
			ivjJSeparator12.setName("JSeparator12");
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjJSeparator12;
}
/**
 * Return the JSeparator13 property value.
 * @return javax.swing.JSeparator
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JSeparator getJSeparator13() {
	if (ivjJSeparator13 == null) {
		try {
			ivjJSeparator13 = new javax.swing.JSeparator();
			ivjJSeparator13.setName("JSeparator13");
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjJSeparator13;
}

/**
 * Return the JSeparator11 property value.
 * @return javax.swing.JSeparator
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JSeparator getJSeparator11a() {
	if (ivjJSeparator11a == null) {
		try {
			ivjJSeparator11a = new javax.swing.JSeparator();
			ivjJSeparator11a.setName("JSeparator11a");
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjJSeparator11a;
}
/**
 * Return the JSeparator2 property value.
 * @return javax.swing.JSeparator
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JSeparator getJSeparator2() {
	if (ivjJSeparator2 == null) {
		try {
			ivjJSeparator2 = new javax.swing.JSeparator();
			ivjJSeparator2.setName("JSeparator2");
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjJSeparator2;
}
/**
 * Return the JSeparator3 property value.
 * @return javax.swing.JSeparator
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JSeparator getJSeparator3() {
	if (ivjJSeparator3 == null) {
		try {
			ivjJSeparator3 = new javax.swing.JSeparator();
			ivjJSeparator3.setName("JSeparator3");
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjJSeparator3;
}
/**
 * Return the JSeparator4 property value.
 * @return javax.swing.JSeparator
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JSeparator getJSeparator4() {
	if (ivjJSeparator4 == null) {
		try {
			ivjJSeparator4 = new javax.swing.JSeparator();
			ivjJSeparator4.setName("JSeparator4");
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjJSeparator4;
}
/**
 * Return the JSeparator5 property value.
 * @return javax.swing.JSeparator
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JSeparator getJSeparator5() {
	if (ivjJSeparator5 == null) {
		try {
			ivjJSeparator5 = new javax.swing.JSeparator();
			ivjJSeparator5.setName("JSeparator5");
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjJSeparator5;
}
/**
 * Return the JSeparator6 property value.
 * @return javax.swing.JSeparator
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JSeparator getJSeparator6() {
	if (ivjJSeparator6 == null) {
		try {
			ivjJSeparator6 = new javax.swing.JSeparator();
			ivjJSeparator6.setName("JSeparator6");
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjJSeparator6;
}
/**
 * Return the JSeparator7 property value.
 * @return javax.swing.JSeparator
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JSeparator getJSeparator7() {
	if (ivjJSeparator7 == null) {
		try {
			ivjJSeparator7 = new javax.swing.JSeparator();
			ivjJSeparator7.setName("JSeparator7");
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjJSeparator7;
}
/**
 * Return the JSeparator8 property value.
 * @return javax.swing.JSeparator
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JSeparator getJSeparator8() {
	if (ivjJSeparator8 == null) {
		try {
			ivjJSeparator8 = new javax.swing.JSeparator();
			ivjJSeparator8.setName("JSeparator8");
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjJSeparator8;
}
/**
 * Return the JSeparator9 property value.
 * @return javax.swing.JSeparator
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JSeparator getJSeparator9() {
	if (ivjJSeparator9 == null) {
		try {
			ivjJSeparator9 = new javax.swing.JSeparator();
			ivjJSeparator9.setName("JSeparator9");
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjJSeparator9;
}
/**
 * Return the Level_UpMenuItem property value.
 * @return javax.swing.JMenuItem
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JMenuItem getLevel_UpMenuItem() {
	if (ivjLevel_UpMenuItem == null) {
		try {
			ivjLevel_UpMenuItem = new javax.swing.JMenuItem();
			ivjLevel_UpMenuItem.setName("Level_UpMenuItem");
			ivjLevel_UpMenuItem.setMnemonic('L');
			ivjLevel_UpMenuItem.setText("Level Up");
			ivjLevel_UpMenuItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_UP, Event.SHIFT_MASK));
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjLevel_UpMenuItem;
}

private javax.swing.JButton getLineButton() {
	if (lineButton == null) {
		try {
			lineButton = new javax.swing.JButton();
			lineButton.setName("LineButton");
			lineButton.setToolTipText("Line");
			lineButton.setText("");
			lineButton.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
			lineButton.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
			lineButton.setIcon(
					new javax.swing.ImageIcon(getClass().getResource("/images/linen.gif")));
			//lineButton.setMargin(new java.awt.Insets(0, 0, 0, 0));
			lineButton.setMargin(new java.awt.Insets(0, 0, 0, 0));
		} catch (java.lang.Throwable ivjExc) {
			handleException(ivjExc);
		}
	}
	return lineButton;
}

public boolean getLineButtonEnabled()
{
	return lineButtonEnabled;
}
/**
 * Return the Move_RenameMenuItem property value.
 * @return javax.swing.JMenuItem
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JMenuItem getMove_RenameMenuItem() {
	if (ivjMove_RenameMenuItem == null) {
		try {
			ivjMove_RenameMenuItem = new javax.swing.JMenuItem();
			ivjMove_RenameMenuItem.setName("Move_RenameMenuItem");
			ivjMove_RenameMenuItem.setMnemonic('M');
			ivjMove_RenameMenuItem.setText("Move/Rename");
			ivjMove_RenameMenuItem.setAccelerator(javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_R,java.awt.Event.CTRL_MASK));
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjMove_RenameMenuItem;
}

private javax.swing.JMenuItem getMorphMenuItem() {
	if (ivjMorphMenuItem == null) {
		try {
			ivjMorphMenuItem = new javax.swing.JMenuItem();
			ivjMorphMenuItem.setName("MorphMenuItem");
			ivjMorphMenuItem.setMnemonic('o');
			ivjMorphMenuItem.setText("Morph");
			ivjMorphMenuItem.setAccelerator(javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_M,java.awt.Event.CTRL_MASK));
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjMorphMenuItem;
}

private javax.swing.JMenuItem getFindMenuItem() {
	if (ivjFindMenuItem == null) {
		try {
		    ivjFindMenuItem = new javax.swing.JMenuItem();
		    ivjFindMenuItem.setName("FindMenuItem");
		    ivjFindMenuItem.setMnemonic('f');
		    ivjFindMenuItem.setText("Find...");
		    ivjFindMenuItem.setAccelerator(javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_F,java.awt.Event.CTRL_MASK));
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjFindMenuItem;
}

private FindDialog findDialog = null;
public void findActionPerformed()
{
    if (findDialog == null)
        findDialog = new FindDialog(this);
    findDialog.setVisible(true);
}
/**
 * Return the NameLabel property value.
 * @return javax.swing.JLabel
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JLabel getNameLabel() {
	if (ivjNameLabel == null) {
		try {
			ivjNameLabel = new javax.swing.JLabel();
			ivjNameLabel.setName("NameLabel");
			ivjNameLabel.setText("Name:");
			ivjNameLabel.setForeground(java.awt.Color.black);
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjNameLabel;
}

private javax.swing.JLabel getNameLabel3() {
	if (ivjNameLabel3 == null) {
		try {
			ivjNameLabel3 = new javax.swing.JLabel();
			ivjNameLabel3.setName("NameLabel3");
			ivjNameLabel3.setText("Name:");
			ivjNameLabel3.setForeground(java.awt.Color.black);
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjNameLabel3;
}


private javax.swing.JLabel getNameTextLabel() {
	if (ivjNameTextLabel == null) {
		try {
			ivjNameTextLabel = new javax.swing.JLabel();
			ivjNameTextLabel.setName("NameTextLabel");
			ivjNameTextLabel.setText("");
			ivjNameTextLabel.setForeground(java.awt.Color.black);
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjNameTextLabel;
}
/**
 * Return the NameLabel31 property value.
 * @return javax.swing.JLabel
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JLabel getNameLabel1() {
	if (ivjNameLabel1 == null) {
		try {
			ivjNameLabel1 = new javax.swing.JLabel();
			ivjNameLabel1.setName("NameLabel1");
			ivjNameLabel1.setText("Group Name:");
			ivjNameLabel1.setForeground(java.awt.Color.black);
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjNameLabel1;
}
/**
 * Return the NameLabel2 property value.
 * @return javax.swing.JLabel
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JLabel getNameLabel2() {
	if (ivjNameLabel2 == null) {
		try {
			ivjNameLabel2 = new javax.swing.JLabel();
			ivjNameLabel2.setName("NameLabel2");
			ivjNameLabel2.setText("New name:");
			ivjNameLabel2.setForeground(java.awt.Color.black);
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjNameLabel2;
}
/**
 * Return the NameTextField property value.
 * @return javax.swing.JTextField
 *
/* WARNING: THIS METHOD WILL BE REGENERATED. 
private javax.swing.JTextField getNameTextField() {
	if (ivjNameTextField == null) {
		try {
			ivjNameTextField = new javax.swing.JTextField();
			ivjNameTextField.setName("NameTextField");
			// user code begin {1}

			ivjNameTextField.getDocument().addDocumentListener(new DocumentListener()
			{
			    public void insertUpdate(DocumentEvent e) {
			        check(e);
			    }
			    public void removeUpdate(DocumentEvent e) {
			        check(e);
			    }
			    public void changedUpdate(DocumentEvent e) {
			        // we won't ever get this with a PlainDocument
			    }
			    private void check(DocumentEvent e) {
					GetVDBManager validator = (GetVDBManager)CommandManager.getInstance().getCommand("GetVDBManager");
					String errmsg = validator.getManager().checkRecordName(ivjNameTextField.getText(), true);
					if (errmsg==null)
					{
						getOKButton().setEnabled(true);
						getWarningLabel().setText(" ");
					}
					else if (errmsg.startsWith("WARNING"))
					{
						getOKButton().setEnabled(true);
						getWarningLabel().setText(errmsg);
					}
					else
					{
						getOKButton().setEnabled(false);
						getWarningLabel().setText(errmsg);
					}
			    }
			});

			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjNameTextField;
}*/
/**
 * Return the NameTextField property value.
 * @return javax.swing.JTextField
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JComboBox getNameTextField() {
	if (ivjNameTextField == null) {
		try {
			ivjNameTextField = new javax.swing.JComboBox();
			ivjNameTextField.setName("NameTextField");
			// user code begin {1}

			ivjNameTextField.setEditable(true);
			
			ComboBoxEditor editor = ivjNameTextField.getEditor();
			if (editor.getEditorComponent() instanceof JTextField)
			{
				final JTextField comboBoxTextField = (JTextField)editor.getEditorComponent();

				// register event listener to the text field
				comboBoxTextField.addActionListener(new ActionListener() {
					public void actionPerformed(ActionEvent e) {
						oKButton_ActionPerformed(e);
					}
				});
			}
			
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjNameTextField;
}

/**
 * Returns new record name checker or null if there isn't one.
 * @return 
 */
protected NameChecker getNewRecordNameChecker() {
	if (newRecordNameChecker == null) {
		Component editorComponent = getNameTextField().getEditor().getEditorComponent();
		if (editorComponent instanceof JTextField) {
			newRecordNameChecker =
				new NameChecker((JTextField)editorComponent, null, false, getOKButton(), getWarningLabel());
		}
	}
	return newRecordNameChecker;
}

/**
 * Return the NavigatorMenuItem property value.
 * @return javax.swing.JCheckBoxMenuItem
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JCheckBoxMenuItem getNavigatorMenuItem() {
	if (ivjNavigatorMenuItem == null) {
		try {
			ivjNavigatorMenuItem = new javax.swing.JCheckBoxMenuItem();
			ivjNavigatorMenuItem.setName("NavigatorMenuItem");
			ivjNavigatorMenuItem.setSelected(true);
			ivjNavigatorMenuItem.setMnemonic('N');
			ivjNavigatorMenuItem.setText("Navigator");
			// user code begin {1}
			ivjNavigatorMenuItem.setSelected(Settings.getInstance().getNavigator());
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjNavigatorMenuItem;
}
/**
 * Return the NewMenuItem property value.
 * @return javax.swing.JMenuItem
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JMenuItem getNewMenuItem() {
	if (ivjNewMenuItem == null) {
		try {
			ivjNewMenuItem = new javax.swing.JMenuItem();
			ivjNewMenuItem.setName("NewMenuItem");
			ivjNewMenuItem.setText("New");
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjNewMenuItem;
}
/**
 * Return the NewNameTextField property value.
 * @return javax.swing.JTextField
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JTextField getNewNameTextField() {
	if (ivjNewNameTextField == null) {
		try {
			ivjNewNameTextField = new javax.swing.JTextField();
			ivjNewNameTextField.setName("NewNameTextField");
			// user code begin {1}

			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjNewNameTextField;
}

protected NameChecker getRenameNameChecker() {
	if (renameNameChecker == null) {
		renameNameChecker = new NameChecker(getNewNameTextField(), getOldNameLabel(), false, getRenameOKButton(), getRenameWarningLabel());
	}
	return renameNameChecker;
}

/**
 * Return the NewRecordDialog property value.
 * @return javax.swing.JDialog
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JDialog getNewRecordDialog() {
	if (ivjNewRecordDialog == null) {
		try {
			ivjNewRecordDialog = new javax.swing.JDialog(this);
			ivjNewRecordDialog.setName("NewRecordDialog");
			ivjNewRecordDialog.setDefaultCloseOperation(javax.swing.WindowConstants.DISPOSE_ON_CLOSE);
			ivjNewRecordDialog.setResizable(false);
			ivjNewRecordDialog.setBounds(11, 946, 378, 154);
			ivjNewRecordDialog.setModal(true);
			ivjNewRecordDialog.setTitle("New");
			getNewRecordDialog().setContentPane(getJDialogContentPane());
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjNewRecordDialog;
}

private javax.swing.JDialog getMorphingDialog() {
	if (ivjMorphingDialog == null) {
		try {
			ivjMorphingDialog = new javax.swing.JDialog(this);
			ivjMorphingDialog.setName("MorphingDialog");
			ivjMorphingDialog.setDefaultCloseOperation(javax.swing.WindowConstants.DISPOSE_ON_CLOSE);
			ivjMorphingDialog.setResizable(false);
			ivjMorphingDialog.setBounds(11, 946, 378, 154);
			ivjMorphingDialog.setModal(true);
			ivjMorphingDialog.setTitle("Morph");
			getMorphingDialog().setContentPane(getJDialogContentPane3());
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjMorphingDialog;
}
/**
 * Return the OKButton property value.
 * @return javax.swing.JButton
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JButton getOKButton() {
	if (ivjOKButton == null) {
		try {
			ivjOKButton = new javax.swing.JButton();
			ivjOKButton.setName("OKButton");
			ivjOKButton.setText("OK");
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjOKButton;
}

private javax.swing.JButton getMorphingOKButton() {
	if (ivjMorphingOKButton == null) {
		try {
			ivjMorphingOKButton = new javax.swing.JButton();
			ivjMorphingOKButton.setName("MorphingOKButton");
			ivjMorphingOKButton.setText("OK");
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjMorphingOKButton;
}

/**
 * Return the OldNameLabel property value.
 * @return javax.swing.JLabel
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JLabel getOldNameLabel() {
	if (ivjOldNameLabel == null) {
		try {
			ivjOldNameLabel = new javax.swing.JLabel();
			ivjOldNameLabel.setName("OldNameLabel");
			ivjOldNameLabel.setText("");
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjOldNameLabel;
}
/**
 * Return the OpenButton property value.
 * @return javax.swing.JButton
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JButton getOpenButton() {
	if (ivjOpenButton == null) {
		try {
			ivjOpenButton = new javax.swing.JButton();
			ivjOpenButton.setName("OpenButton");
			ivjOpenButton.setToolTipText("Open DB");
			ivjOpenButton.setText("");
			ivjOpenButton.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
			ivjOpenButton.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
			ivjOpenButton.setIcon(new javax.swing.ImageIcon(getClass().getResource("/images/open.gif")));
			ivjOpenButton.setMargin(new java.awt.Insets(0, 0, 0, 0));
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjOpenButton;
}
/**
 * Return the OpenMenuItem property value.
 * @return javax.swing.JMenuItem
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JMenuItem getOpenMenuItem() {
	if (ivjOpenMenuItem == null) {
		try {
			ivjOpenMenuItem = new javax.swing.JMenuItem();
			ivjOpenMenuItem.setName("OpenMenuItem");
			ivjOpenMenuItem.setMnemonic('O');
			ivjOpenMenuItem.setText("Open...");
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjOpenMenuItem;
}
/**
 * Return the PageSetupMenuItem property value.
 * @return javax.swing.JMenuItem
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JMenuItem getPageSetupMenuItem() {
	if (ivjPageSetupMenuItem == null) {
		try {
			ivjPageSetupMenuItem = new javax.swing.JMenuItem();
			ivjPageSetupMenuItem.setName("PageSetupMenuItem");
			ivjPageSetupMenuItem.setText("Page Setup");
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjPageSetupMenuItem;
}
/**
 * Return the PasteButton property value.
 * @return javax.swing.JButton
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JButton getPasteButton() {
	if (ivjPasteButton == null) {
		try {
			ivjPasteButton = new javax.swing.JButton();
			ivjPasteButton.setName("PasteButton");
			ivjPasteButton.setToolTipText("Paste");
			ivjPasteButton.setText("");
			ivjPasteButton.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
			ivjPasteButton.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
			ivjPasteButton.setIcon(new javax.swing.ImageIcon(getClass().getResource("/images/paste.gif")));
			ivjPasteButton.setMargin(new java.awt.Insets(0, 0, 0, 0));
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjPasteButton;
}
/**
 * Return the PasteMenuItem property value.
 * @return javax.swing.JMenuItem
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JMenuItem getPasteMenuItem() {
	if (ivjPasteMenuItem == null) {
		try {
			ivjPasteMenuItem = new javax.swing.JMenuItem();
			ivjPasteMenuItem.setName("PasteMenuItem");
			ivjPasteMenuItem.setMnemonic('P');
			ivjPasteMenuItem.setText("Paste");
			ivjPasteMenuItem.setAccelerator(javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_V,java.awt.Event.CTRL_MASK));
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjPasteMenuItem;
}
/**
 * Return the PasteMenuItem property value.
 * @return javax.swing.JMenuItem
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JMenuItem getSystemPasteMenuItem() {
	if (ivjSystemPasteMenuItem == null) {
		try {
			ivjSystemPasteMenuItem = new javax.swing.JMenuItem();
			ivjSystemPasteMenuItem.setName("SystemPasteMenuItem");
			//ivjSystemPasteMenuItem.setMnemonic('P');
			ivjSystemPasteMenuItem.setText("Paste from System Clipboard");
			ivjSystemPasteMenuItem.setAccelerator(javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_V,java.awt.Event.CTRL_MASK|java.awt.Event.SHIFT_MASK));
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjSystemPasteMenuItem;
}
/**
 * Return the PluginManagerMenuItem property value.
 * @return javax.swing.JMenuItem
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JMenuItem getPluginManagerMenuItem() {
	if (ivjPluginManagerMenuItem == null) {
		try {
			ivjPluginManagerMenuItem = new javax.swing.JMenuItem();
			ivjPluginManagerMenuItem.setName("PluginManagerMenuItem");
			ivjPluginManagerMenuItem.setMnemonic('M');
			ivjPluginManagerMenuItem.setText("Plugin Manager...");
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjPluginManagerMenuItem;
}
/**
 * Return the pluginsMenu property value.
 * @return com.cosylab.vdct.plugin.PluginsMenu
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private com.cosylab.vdct.plugin.PluginsMenu getPluginsMenu() {
	if (ivjPluginsMenu == null) {
		try {
			ivjPluginsMenu = new com.cosylab.vdct.plugin.PluginsMenu();
			ivjPluginsMenu.setName("PluginsMenu");
			ivjPluginsMenu.setMnemonic('P');
			ivjPluginsMenu.setText("Plugins");
			ivjPluginsMenu.setActionCommand("PluginsMenu");
			ivjPluginsMenu.add(getPluginManagerMenuItem());
			ivjPluginsMenu.add(getJSeparator7());
			// user code begin {1}
			ivjPluginsMenu.init();
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjPluginsMenu;
}
/**
 * Return the Preferences___MenuItem property value.
 * @return javax.swing.JMenuItem
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JMenuItem getPreferences___MenuItem() {
	if (ivjPreferences___MenuItem == null) {
		try {
			ivjPreferences___MenuItem = new javax.swing.JMenuItem();
			ivjPreferences___MenuItem.setName("Preferences___MenuItem");
			ivjPreferences___MenuItem.setMnemonic('S');
			ivjPreferences___MenuItem.setText("Settings...");
			ivjPreferences___MenuItem.setEnabled(true);
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjPreferences___MenuItem;
}

/**
 * @return javax.swing.JMenuItem
 */
private javax.swing.JMenuItem getSpreadsheetMenuItem() {
	if (spreadsheetMenuItem == null) {
		try {
			spreadsheetMenuItem = new javax.swing.JMenuItem();
			spreadsheetMenuItem.setName("SpreadsheetMenuItem");
			spreadsheetMenuItem.setMnemonic('P');
			spreadsheetMenuItem.setText("Spreadsheet...");
			spreadsheetMenuItem.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_S, Event.ALT_MASK));
			spreadsheetMenuItem.setEnabled(true);
		} catch (java.lang.Throwable ivjExc) {
			handleException(ivjExc);
		}
	}
	return spreadsheetMenuItem;
}
/**
 * Return the PrintAsPostScriptMenuItem property value.
 * @return javax.swing.JMenuItem
 */
private javax.swing.JMenuItem getPrintAsPostScriptMenuItem() {
	if (ivjPrintAsPostScriptMenuItem == null) {
		try {
			ivjPrintAsPostScriptMenuItem = new javax.swing.JMenuItem();
			ivjPrintAsPostScriptMenuItem.setName("PrintAsPostScriptMenuItem");
			ivjPrintAsPostScriptMenuItem.setText("Print as PostScript");
			ivjPrintAsPostScriptMenuItem.setEnabled(true);
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjPrintAsPostScriptMenuItem;
}
/**
 * Return the PrintMenuItem property value.
 * @return javax.swing.JMenuItem
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JMenuItem getPrintMenuItem() {
	if (ivjPrintMenuItem == null) {
		try {
			ivjPrintMenuItem = new javax.swing.JMenuItem();
			ivjPrintMenuItem.setName("PrintMenuItem");
			ivjPrintMenuItem.setMnemonic('P');
			ivjPrintMenuItem.setText("Print");
			ivjPrintMenuItem.setEnabled(true);
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjPrintMenuItem;
}
/**
 * Return the PrintPreviewMenuItem property value.
 * @return javax.swing.JMenuItem
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JMenuItem getPrintPreviewMenuItem() {
	if (ivjPrintPreviewMenuItem == null) {
		try {
			ivjPrintPreviewMenuItem = new javax.swing.JMenuItem();
			ivjPrintPreviewMenuItem.setName("PrintPreviewMenuItem");
			ivjPrintPreviewMenuItem.setText("Print Preview");
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjPrintPreviewMenuItem;
}
/**
 * Return the RedoButton property value.
 * @return javax.swing.JButton
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
public javax.swing.JButton getRedoButton() {
	if (ivjRedoButton == null) {
		try {
			ivjRedoButton = new javax.swing.JButton();
			ivjRedoButton.setName("RedoButton");
			ivjRedoButton.setToolTipText("Redo");
			ivjRedoButton.setText("");
			ivjRedoButton.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
			ivjRedoButton.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
			ivjRedoButton.setIcon(new javax.swing.ImageIcon(getClass().getResource("/images/redo.gif")));
			ivjRedoButton.setMargin(new java.awt.Insets(0, 0, 0, 0));
			ivjRedoButton.setEnabled(false);
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjRedoButton;
}
/**
 * Return the RedoMenuItem property value.
 * @return javax.swing.JMenuItem
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
public javax.swing.JMenuItem getRedoMenuItem() {
	if (ivjRedoMenuItem == null) {
		try {
			ivjRedoMenuItem = new javax.swing.JMenuItem();
			ivjRedoMenuItem.setName("RedoMenuItem");
			ivjRedoMenuItem.setText("Redo");
			ivjRedoMenuItem.setAccelerator(javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_Y,java.awt.Event.CTRL_MASK));
			ivjRedoMenuItem.setEnabled(false);
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjRedoMenuItem;
}
/**
 * Return the RenameCancelButton property value.
 * @return javax.swing.JButton
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JButton getRenameCancelButton() {
	if (ivjRenameCancelButton == null) {
		try {
			ivjRenameCancelButton = new javax.swing.JButton();
			ivjRenameCancelButton.setName("RenameCancelButton");
			ivjRenameCancelButton.setText("Cancel");
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjRenameCancelButton;
}
/**
 * Return the RenameDialog property value.
 * @return javax.swing.JDialog
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JDialog getRenameDialog() {
	if (ivjRenameDialog == null) {
		try {
			ivjRenameDialog = new javax.swing.JDialog(this);
			ivjRenameDialog.setName("RenameDialog");
			ivjRenameDialog.setDefaultCloseOperation(javax.swing.WindowConstants.DISPOSE_ON_CLOSE);
			ivjRenameDialog.setResizable(false);
			ivjRenameDialog.setBounds(457, 1183, 378, 154);
			ivjRenameDialog.setModal(true);
			ivjRenameDialog.setTitle("Rename");
			getRenameDialog().setContentPane(getJDialogContentPane2());
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjRenameDialog;
}
/**
 * Return the RenameOKButton property value.
 * @return javax.swing.JButton
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JButton getRenameOKButton() {
	if (ivjRenameOKButton == null) {
		try {
			ivjRenameOKButton = new javax.swing.JButton();
			ivjRenameOKButton.setName("RenameOKButton");
			ivjRenameOKButton.setText("OK");
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjRenameOKButton;
}
/**
 * Return the RenameWarningLabel property value.
 * @return javax.swing.JLabel
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JLabel getRenameWarningLabel() {
	if (ivjRenameWarningLabel == null) {
		try {
			ivjRenameWarningLabel = new javax.swing.JLabel();
			ivjRenameWarningLabel.setName("RenameWarningLabel");
			ivjRenameWarningLabel.setText(" ");
			ivjRenameWarningLabel.setForeground(java.awt.Color.red);
			ivjRenameWarningLabel.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
			ivjRenameWarningLabel.setFont(new java.awt.Font("dialog", 0, 10));
			ivjRenameWarningLabel.setHorizontalAlignment(javax.swing.SwingConstants.CENTER);
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjRenameWarningLabel;
}
/**
 * Return the Save_As_GroupMenuItem property value.
 * @return javax.swing.JMenuItem
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JMenuItem getSave_As_GroupMenuItem() {
	if (ivjSave_As_GroupMenuItem == null) {
		try {
			ivjSave_As_GroupMenuItem = new javax.swing.JMenuItem();
			ivjSave_As_GroupMenuItem.setName("Save_As_GroupMenuItem");
			ivjSave_As_GroupMenuItem.setText("Save Group as...");
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjSave_As_GroupMenuItem;
}



/**
 * Return the Save_AsMenuItem property value.
 * @return javax.swing.JMenuItem
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JMenuItem getSave_AsMenuItem() {
	if (ivjSave_AsMenuItem == null) {
		try {
			ivjSave_AsMenuItem = new javax.swing.JMenuItem();
			ivjSave_AsMenuItem.setName("Save_AsMenuItem");
			ivjSave_AsMenuItem.setText("Save as...");
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjSave_AsMenuItem;
}

private JMenuItem getCloseMenuItem() {
	if (closeMenuItem == null) {
		try {
			closeMenuItem = new JMenuItem("Close");
			closeMenuItem.setName("Close");
			closeMenuItem.setMnemonic('C');
			
			closeMenuItem.addActionListener(new ActionListener() {
				public void actionPerformed(ActionEvent e) {
					GetGUIInterface cmd = (GetGUIInterface)CommandManager.getInstance().getCommand("GetGUIMenuInterface");
				 	cmd.getGUIMenuInterface().close();
				}
			});
		} catch (Throwable ivjExc) {
			handleException(ivjExc);
		}
	}
	return closeMenuItem;
}

private JMenuItem getCloseAllMenuItem() {
	if (closeAllMenuItem == null) {
		try {
			closeAllMenuItem = new JMenuItem("Close All");
			closeAllMenuItem.setName("CloseAll");
			closeAllMenuItem.setMnemonic('L');
			
			closeAllMenuItem.addActionListener(new ActionListener() {
				public void actionPerformed(ActionEvent e) {
					GetGUIInterface cmd = (GetGUIInterface)CommandManager.getInstance().getCommand("GetGUIMenuInterface");
				 	cmd.getGUIMenuInterface().closeAll();
				}
			});
		} catch (Throwable ivjExc) {
			handleException(ivjExc);
		}
	}
	return closeAllMenuItem;
}

/**
 * Return the SaveButton property value.
 * @return javax.swing.JButton
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JButton getSaveButton() {
	if (ivjSaveButton == null) {
		try {
			ivjSaveButton = new javax.swing.JButton();
			ivjSaveButton.setName("SaveButton");
			ivjSaveButton.setToolTipText("Save");
			ivjSaveButton.setText("");
			ivjSaveButton.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
			ivjSaveButton.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
			ivjSaveButton.setIcon(new javax.swing.ImageIcon(getClass().getResource("/images/save.gif")));
			ivjSaveButton.setMargin(new java.awt.Insets(0, 0, 0, 0));
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjSaveButton;
}
/**
 * Return the SaveMenuItem property value.
 * @return javax.swing.JMenuItem
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JMenuItem getSaveMenuItem() {
	if (ivjSaveMenuItem == null) {
		try {
			ivjSaveMenuItem = new javax.swing.JMenuItem();
			ivjSaveMenuItem.setName("SaveMenuItem");
			ivjSaveMenuItem.setMnemonic('S');
			ivjSaveMenuItem.setText("Save");
			ivjSaveMenuItem.setAccelerator(javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_S,java.awt.Event.CTRL_MASK));
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjSaveMenuItem;
}
/**
 * Return the SaveMenuItem property value.
 * @return javax.swing.JMenuItem
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JMenuItem getSaveAsTemplateMenuItem() {
	if (ivjSaveAsTemplateMenuItem == null) {
		try {
			ivjSaveAsTemplateMenuItem = new javax.swing.JMenuItem();
			ivjSaveAsTemplateMenuItem.setName("SaveAsTemplateMenuItem");
			ivjSaveAsTemplateMenuItem.setText("Save as Template...");
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjSaveAsTemplateMenuItem;
}
/**
 * Return the GenerateMenuItem property value.
 * @return javax.swing.JMenuItem
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JMenuItem getGenerateMenuItem() {
	if (ivjGenerateMenuItem == null) {
		try {
			ivjGenerateMenuItem = new javax.swing.JMenuItem();
			ivjGenerateMenuItem.setName("GenerateMenuItem");
			ivjGenerateMenuItem.setMnemonic('G');
			ivjGenerateMenuItem.setText("Generate...");
			ivjGenerateMenuItem.setAccelerator(javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_G,java.awt.Event.CTRL_MASK));
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjGenerateMenuItem;
}
/**
 * Return the RecentFilesMenuItem property value.
 * @return javax.swing.JMenuItem
 */
private javax.swing.JMenuItem getRecentFilesMenuItem() {
	if (ivjRecentFilesMenuItem == null) {
		try {
			ivjRecentFilesMenuItem = new RecentFilesMenu();
			ivjRecentFilesMenuItem.setMnemonic('R');
			// user code begin {1}
			class OpenRecentFile implements ActionListener
			{
				public void actionPerformed(ActionEvent e)
				{
					VisualDCT.this.openDB(e.getActionCommand());
				}
			}
			ivjRecentFilesMenuItem.addActionListener(new OpenRecentFile());
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjRecentFilesMenuItem;
}
/**
 * Return the GenerateAsGroupMenuItem property value.
 * @return javax.swing.JMenuItem
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JMenuItem getGenerateAsGroupMenuItem() {
	if (ivjGenerateAsGroupMenuItem == null) {
		try {
			ivjGenerateAsGroupMenuItem = new javax.swing.JMenuItem();
			ivjGenerateAsGroupMenuItem.setName("GenerateAsGroupMenuItem");
			ivjGenerateAsGroupMenuItem.setText("Generate Group as...");
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjGenerateAsGroupMenuItem;
}
/**
 * Return the Select_AllMenuItem property value.
 * @return javax.swing.JMenuItem
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JMenuItem getSelect_AllMenuItem() {
	if (ivjSelect_AllMenuItem == null) {
		try {
			ivjSelect_AllMenuItem = new javax.swing.JMenuItem();
			ivjSelect_AllMenuItem.setName("Select_AllMenuItem");
			ivjSelect_AllMenuItem.setMnemonic('A');
			ivjSelect_AllMenuItem.setText("Select All");
			ivjSelect_AllMenuItem.setAccelerator(javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_A,java.awt.Event.CTRL_MASK));
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjSelect_AllMenuItem;
}
/**
 * Return the Show_PointsMenuItem property value.
 * @return javax.swing.JCheckBoxMenuItem
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JCheckBoxMenuItem getShow_PointsMenuItem() {
	if (ivjShow_PointsMenuItem == null) {
		try {
			ivjShow_PointsMenuItem = new javax.swing.JCheckBoxMenuItem();
			ivjShow_PointsMenuItem.setName("Show_PointsMenuItem");
			ivjShow_PointsMenuItem.setMnemonic('G');
			ivjShow_PointsMenuItem.setText("Show Grid");
			ivjShow_PointsMenuItem.setActionCommand("Show Grid");
			// user code begin {1}
			ivjShow_PointsMenuItem.setSelected(Settings.getInstance().getShowGrid());
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjShow_PointsMenuItem;
}
/**
 * Return the Smart_ZoomMenuItem property value.
 * @return javax.swing.JMenuItem
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JMenuItem getSmart_ZoomMenuItem() {
	if (ivjSmart_ZoomMenuItem == null) {
		try {
			ivjSmart_ZoomMenuItem = new javax.swing.JMenuItem();
			ivjSmart_ZoomMenuItem.setName("Smart_ZoomMenuItem");
			ivjSmart_ZoomMenuItem.setMnemonic('Z');
			ivjSmart_ZoomMenuItem.setText("Zoom Selection");
			ivjSmart_ZoomMenuItem.setAccelerator(javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_SPACE,java.awt.Event.SHIFT_MASK));
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjSmart_ZoomMenuItem;
}
/**
 * Return the SnapToGridMenuItem property value.
 * @return javax.swing.JCheckBoxMenuItem
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JCheckBoxMenuItem getSnapToGridMenuItem() {
	if (ivjSnapToGridMenuItem == null) {
		try {
			ivjSnapToGridMenuItem = new javax.swing.JCheckBoxMenuItem();
			ivjSnapToGridMenuItem.setName("SnapToGridMenuItem");
			ivjSnapToGridMenuItem.setMnemonic('r');
			ivjSnapToGridMenuItem.setText("Snap to Grid");
			// user code begin {1}
			ivjSnapToGridMenuItem.setSelected(Settings.getInstance().getSnapToGrid());
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjSnapToGridMenuItem;
}

private javax.swing.JCheckBoxMenuItem getWindowsPanMenuItem() {
	if (ivjWindowsPanMenuItem == null) {
		try {
			ivjWindowsPanMenuItem = new javax.swing.JCheckBoxMenuItem();
			ivjWindowsPanMenuItem.setName("WindowsPanMenuItem");
			ivjWindowsPanMenuItem.setMnemonic('r');
			ivjWindowsPanMenuItem.setText("Windows Pan Direction");
			// user code begin {1}
			ivjWindowsPanMenuItem.setSelected(Settings.getInstance().getWindowsPan());
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjWindowsPanMenuItem;
}
/**
 * Return the StartDebugMenuItem property value.
 * @return com.cosylab.vdct.plugin.debug.DebugStartMenu
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private com.cosylab.vdct.plugin.debug.DebugStartMenu getStartDebugMenuItem() {
	if (ivjStartDebugMenuItem == null) {
		try {
			ivjStartDebugMenuItem = new com.cosylab.vdct.plugin.debug.DebugStartMenu();
			ivjStartDebugMenuItem.setName("StartDebugMenuItem");
			ivjStartDebugMenuItem.setMnemonic('S');
			ivjStartDebugMenuItem.setText("Start");
			// user code begin {1}
			ivjStartDebugMenuItem.init();
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjStartDebugMenuItem;
}
/**
 * Return the StatusbarMenuItem property value.
 * @return javax.swing.JCheckBoxMenuItem
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JCheckBoxMenuItem getStatusbarMenuItem() {
	if (ivjStatusbarMenuItem == null) {
		try {
			ivjStatusbarMenuItem = new javax.swing.JCheckBoxMenuItem();
			ivjStatusbarMenuItem.setName("StatusbarMenuItem");
			ivjStatusbarMenuItem.setSelected(true);
			ivjStatusbarMenuItem.setMnemonic('S');
			ivjStatusbarMenuItem.setText("Statusbar");
			// user code begin {1}
			ivjStatusbarMenuItem.setSelected(Settings.getInstance().getStatusbar());
			viewStatusBar();
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjStatusbarMenuItem;
}
/**
 * Return the StatusBarPane property value.
 * @return javax.swing.JPanel
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JPanel getStatusBarPane() {
	if (ivjStatusBarPane == null) {
		try {
			ivjStatusBarPane = new javax.swing.JPanel();
			ivjStatusBarPane.setName("StatusBarPane");
			ivjStatusBarPane.setLayout(new java.awt.BorderLayout());
			
			getStatusBarPane().add(getZoomPanel(), "East");
			getStatusBarPane().add(getStatusMsg1(), "West");
			//getStatusBarPane().add(getStatusMsg2(), "Center");
			getStatusBarPane().add(getStatusGroupAndModePanel(), "Center");
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjStatusBarPane;
}
/**
 * Return the ivjStatusGroupAndModePane property value.
 * @return javax.swing.JPanel
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JPanel getStatusGroupAndModePanel() {
	if (ivjStatusGroupAndModePane == null) {
		try {
			ivjStatusGroupAndModePane = new javax.swing.JPanel();
			ivjStatusGroupAndModePane.setName("StatusGroupAndModePanel");
			ivjStatusGroupAndModePane.setLayout(new java.awt.BorderLayout());
			ivjStatusGroupAndModePane.add(getStatusMsg2(), "Center");
			ivjStatusGroupAndModePane.add(getModeStatus(), "East");
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjStatusGroupAndModePane;
}
/**
 * Return the StatusMsg1 property value.
 * @return javax.swing.JLabel
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JLabel getStatusMsg1() {
	if (ivjStatusMsg1 == null) {
		try {
			ivjStatusMsg1 = new javax.swing.JLabel();
			ivjStatusMsg1.setName("StatusMsg1");
			ivjStatusMsg1.setBorder(new javax.swing.border.EtchedBorder());
			ivjStatusMsg1.setText("StatusMsg1");
			// user code begin {1}
			//Font font = ivjStatusMsg1.getFont();
			//ivjStatusMsg1.setFont(font.deriveFont(Font.PLAIN));
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjStatusMsg1;
}
/**
 * Return the StatusMsg2 property value.
 * @return javax.swing.JLabel
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JLabel getStatusMsg2() {
	if (ivjStatusMsg2 == null) {
		try {
			ivjStatusMsg2 = new javax.swing.JLabel();
			ivjStatusMsg2.setName("StatusMsg2");
			ivjStatusMsg2.setBorder(new javax.swing.border.EtchedBorder());
			ivjStatusMsg2.setText("StatusMsg2");
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjStatusMsg2;
}

/**
 * Return the ModeStatus property value.
 * @return javax.swing.JLabel
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JLabel getModeStatus() {
	if (ivjModeStatus == null) {
		try {
			ivjModeStatus = new javax.swing.JLabel();
			ivjModeStatus.setName("ModeStatus");
			ivjModeStatus.setBorder(new javax.swing.border.EtchedBorder());
			ivjModeStatus.setText(" Edit Mode ");
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjModeStatus;
}

/**
 * Return the StopDebugMenuItem property value.
 * @return com.cosylab.vdct.plugin.debug.DebugStopMenuItem
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private com.cosylab.vdct.plugin.debug.DebugStopMenuItem getStopDebugMenuItem() {
	if (ivjStopDebugMenuItem == null) {
		try {
			ivjStopDebugMenuItem = new com.cosylab.vdct.plugin.debug.DebugStopMenuItem();
			ivjStopDebugMenuItem.setName("StopDebugMenuItem");
			ivjStopDebugMenuItem.setMnemonic('t');
			ivjStopDebugMenuItem.setText("Stop");
			ivjStopDebugMenuItem.setAccelerator(javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_T,java.awt.Event.CTRL_MASK));
			ivjStopDebugMenuItem.setEnabled(true);
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjStopDebugMenuItem;
}

private javax.swing.JButton getTextBoxButton() {
	if (textBoxButton == null) {
		try {
			textBoxButton = new javax.swing.JButton();
			textBoxButton.setName("TextBoxButton");
			textBoxButton.setToolTipText("TextBox");
			textBoxButton.setText("");
			textBoxButton.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
			textBoxButton.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
			textBoxButton.setIcon(new javax.swing.ImageIcon(getClass().getResource("/images/textboxn.gif")));
			textBoxButton.setMargin(new java.awt.Insets(0, 0, 0, 0));
		} catch (java.lang.Throwable ivjExc) {
			handleException(ivjExc);
		}
	}
	return textBoxButton;
}

public boolean getTextBoxButtonEnabled()
{
	return textBoxButtonEnabled;
}
/**
 * Return the ToolbarMenuItem property value.
 * @return javax.swing.JCheckBoxMenuItem
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JCheckBoxMenuItem getToolbarMenuItem() {
	if (ivjToolbarMenuItem == null) {
		try {
			ivjToolbarMenuItem = new javax.swing.JCheckBoxMenuItem();
			ivjToolbarMenuItem.setName("ToolbarMenuItem");
			ivjToolbarMenuItem.setSelected(true);
			ivjToolbarMenuItem.setMnemonic('T');
			ivjToolbarMenuItem.setText("Toolbar");
			// user code begin {1}
			ivjToolbarMenuItem.setSelected(Settings.getInstance().getToolbar());
			viewToolBar();
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjToolbarMenuItem;
}
/**
 * Return the ToolBarPane property value.
 * @return javax.swing.JToolBar
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JToolBar getToolBarPane() {
	if (ivjToolBarPane == null) {
		try {
			ivjToolBarPane = new javax.swing.JToolBar();
			ivjToolBarPane.setName("ToolBarPane");
			ivjToolBarPane.add(getOpenButton());
			ivjToolBarPane.add(getSaveButton());
			ivjToolBarPane.add(getFindButton());
			ivjToolBarPane.add(getUndoButton());
			ivjToolBarPane.add(getRedoButton());
			ivjToolBarPane.add(getCutButton());
			ivjToolBarPane.add(getCopyButton());
			ivjToolBarPane.add(getPasteButton());
			
			JSeparator sep = new JSeparator();
			//sep.setOrientation(JSeparator.VERTICAL);
			Dimension dim = new Dimension(8, 0);
			sep.setMaximumSize(dim);
			sep.setMinimumSize(dim);
			sep.setPreferredSize(dim);
			ivjToolBarPane.add(sep);
			ivjToolBarPane.add(getLineButton());
			ivjToolBarPane.add(getBoxButton());
			ivjToolBarPane.add(getTextBoxButton());

			// user code begin {1}
			ivjToolBarPane.setFloatable(false);
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjToolBarPane;
}
/**
 * Return the TypeComboBox property value.
 * @return javax.swing.JComboBox
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JComboBox getTypeComboBox() {
	if (ivjTypeComboBox == null) {
		try {
			ivjTypeComboBox = new javax.swing.JComboBox();
			ivjTypeComboBox.setName("TypeComboBox");
			ivjTypeComboBox.setEditable(false);
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjTypeComboBox;
}

private javax.swing.JComboBox getTypeComboBox2() {
	if (ivjTypeComboBox2 == null) {
		try {
			ivjTypeComboBox2 = new javax.swing.JComboBox();
			ivjTypeComboBox2.setName("TypeComboBox2");
			ivjTypeComboBox2.setEditable(false);
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjTypeComboBox2;
}

/**
 * Return the TypeLabel property value.
 * @return javax.swing.JLabel
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JLabel getTypeLabel() {
	if (ivjTypeLabel == null) {
		try {
			ivjTypeLabel = new javax.swing.JLabel();
			ivjTypeLabel.setName("TypeLabel");
			ivjTypeLabel.setText("Type:");
			ivjTypeLabel.setForeground(java.awt.Color.black);
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjTypeLabel;
}
private javax.swing.JLabel getTypeLabel2() {
	if (ivjTypeLabel2 == null) {
		try {
			ivjTypeLabel2 = new javax.swing.JLabel();
			ivjTypeLabel2.setName("TypeLabel2");
			ivjTypeLabel2.setText("Type:");
			ivjTypeLabel2.setForeground(java.awt.Color.black);
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjTypeLabel2;
}

/**
 * Return the TypeLabel1 property value.
 * @return javax.swing.JLabel
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JLabel getTypeLabel1() {
	if (ivjTypeLabel1 == null) {
		try {
			ivjTypeLabel1 = new javax.swing.JLabel();
			ivjTypeLabel1.setName("TypeLabel1");
			ivjTypeLabel1.setText("Old name:");
			ivjTypeLabel1.setForeground(java.awt.Color.black);
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjTypeLabel1;
}
/**
 * Return the UndoButton property value.
 * @return javax.swing.JButton
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
public javax.swing.JButton getUndoButton() {
	if (ivjUndoButton == null) {
		try {
			ivjUndoButton = new javax.swing.JButton();
			ivjUndoButton.setName("UndoButton");
			ivjUndoButton.setToolTipText("Undo");
			ivjUndoButton.setText("");
			ivjUndoButton.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
			ivjUndoButton.setVerticalTextPosition(javax.swing.SwingConstants.BOTTOM);
			ivjUndoButton.setIcon(new javax.swing.ImageIcon(getClass().getResource("/images/undo.gif")));
			ivjUndoButton.setMargin(new java.awt.Insets(0, 0, 0, 0));
			ivjUndoButton.setEnabled(false);
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjUndoButton;
}
/**
 * Return the UndoMenuItem property value.
 * @return javax.swing.JMenuItem
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
public javax.swing.JMenuItem getUndoMenuItem() {
	if (ivjUndoMenuItem == null) {
		try {
			ivjUndoMenuItem = new javax.swing.JMenuItem();
			ivjUndoMenuItem.setName("UndoMenuItem");
			ivjUndoMenuItem.setText("Undo");
			ivjUndoMenuItem.setAccelerator(javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_Z,java.awt.Event.CTRL_MASK));
			ivjUndoMenuItem.setEnabled(false);
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjUndoMenuItem;
}
/**
 * Return the UngroupMenuItem property value.
 * @return javax.swing.JMenuItem
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JMenuItem getUngroupMenuItem() {
	if (ivjUngroupMenuItem == null) {
		try {
			ivjUngroupMenuItem = new javax.swing.JMenuItem();
			ivjUngroupMenuItem.setName("UngroupMenuItem");
			ivjUngroupMenuItem.setMnemonic('U');
			ivjUngroupMenuItem.setText("Ungroup");
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjUngroupMenuItem;
}
/**
 * Return the ViewMenu property value.
 * @return javax.swing.JMenu
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JMenu getViewMenu() {
	if (ivjViewMenu == null) {
		try {
			ivjViewMenu = new javax.swing.JMenu();
			ivjViewMenu.setName("ViewMenu");
			ivjViewMenu.setMnemonic('V');
			ivjViewMenu.setText("View");
			ivjViewMenu.add(getFlat_ViewMenuItem());
			ivjViewMenu.add(getLevel_UpMenuItem());
			ivjViewMenu.add(getZoom_InMenuItem());
			ivjViewMenu.add(getZoom_OutMenuItem());
			ivjViewMenu.add(getSmart_ZoomMenuItem());
			ivjViewMenu.add(getBase_ViewMenuItem());
			ivjViewMenu.add(getJSeparator8());
			ivjViewMenu.add(getToolbarMenuItem());
			ivjViewMenu.add(getStatusbarMenuItem());
			ivjViewMenu.add(getNavigatorMenuItem());
			ivjViewMenu.add(getJSeparator9());
			ivjViewMenu.add(getShow_PointsMenuItem());
			ivjViewMenu.add(getSnapToGridMenuItem());
			ivjViewMenu.add(getWindowsPanMenuItem());
			ivjViewMenu.add(getJSeparator10());
			ivjViewMenu.add(getPreferences___MenuItem());
			// user code begin {1}
			ivjViewMenu.add(getJSeparator10());
			ivjViewMenu.add(getSpreadsheetMenuItem());
			ivjViewMenu.add(getLeftMenuItem());
			ivjViewMenu.add(getRightMenuItem());
			ivjViewMenu.add(getUpMenuItem());
			ivjViewMenu.add(getDownMenuItem());
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjViewMenu;
}
/**
 * Return the VisualDCTJMenuBar property value.
 * @return javax.swing.JMenuBar
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JMenuBar getVisualDCTJMenuBar() {
	if (ivjVisualDCTJMenuBar == null) {
		try {
			ivjVisualDCTJMenuBar = new javax.swing.JMenuBar();
			ivjVisualDCTJMenuBar.setName("VisualDCTJMenuBar");
			ivjVisualDCTJMenuBar.add(getFileMenu());
			ivjVisualDCTJMenuBar.add(getEditMenu());
			ivjVisualDCTJMenuBar.add(getViewMenu());
			ivjVisualDCTJMenuBar.add(getPluginsMenu());
			ivjVisualDCTJMenuBar.add(getDebugMenu());
			// RDB support not yet finished.
			if (false) {
			    ivjVisualDCTJMenuBar.add(getRdbMenu());
			}
			ivjVisualDCTJMenuBar.add(getToolsMenu());
			ivjVisualDCTJMenuBar.add(getHelpMenu());
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjVisualDCTJMenuBar;
}
/**
 * Return the WarningLabel property value.
 * @return javax.swing.JLabel
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JLabel getWarningLabel() {
	if (ivjWarningLabel == null) {
		try {
			ivjWarningLabel = new javax.swing.JLabel();
			ivjWarningLabel.setName("WarningLabel");
			ivjWarningLabel.setText(" ");
			ivjWarningLabel.setForeground(java.awt.Color.red);
			ivjWarningLabel.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
			ivjWarningLabel.setFont(new java.awt.Font("dialog", 0, 10));
			ivjWarningLabel.setHorizontalAlignment(javax.swing.SwingConstants.CENTER);
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjWarningLabel;
}
/**
 * Return the workspace property value.
 * @return com.cosylab.vdct.graphics.WorkspacePanel
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private com.cosylab.vdct.graphics.WorkspaceDesktop getworkspace() {
	if (ivjworkspace == null) {
		try {
			ivjworkspace = new WorkspaceDesktop();
			ivjworkspace.setName("workspace");
			ivjworkspace.addMouseListener(new MouseAdapter() {
			    public void mouseEntered(MouseEvent e) {
			        if (VisualDCT.this.isActive()) 
			            ivjJFrameContentPane.requestFocus(); 
			    }
			});
			// user code begin {1}
			
			// An override for desktop default shift direction key operation.
			InputMap map = ivjworkspace.getInputMap(JComponent.WHEN_ANCESTOR_OF_FOCUSED_COMPONENT);
			map.put(KeyStroke.getKeyStroke(KeyEvent.VK_UP, KeyEvent.SHIFT_DOWN_MASK), "shiftUp");
			map.put(KeyStroke.getKeyStroke(KeyEvent.VK_LEFT, KeyEvent.SHIFT_DOWN_MASK), "shiftLeft");
			map.put(KeyStroke.getKeyStroke(KeyEvent.VK_RIGHT, KeyEvent.SHIFT_DOWN_MASK), "shiftRight");

			ivjworkspace.getActionMap().put("shiftUp", new AbstractAction() {
				public void actionPerformed(ActionEvent arg0) {
					level_UpMenuItem_ActionPerformed();
				}
	        });
			ivjworkspace.getActionMap().put("shiftLeft", new AbstractAction() {
				public void actionPerformed(ActionEvent arg0) {
					zoom_OutMenuItem_ActionPerformed();
				}
	        });
			ivjworkspace.getActionMap().put("shiftRight", new AbstractAction() {
				public void actionPerformed(ActionEvent arg0) {
					zoom_InMenuItem_ActionPerformed();
				}
	        });
			
			DsManager drawingSurfaceManager = new DsManager(ivjworkspace);
			ivjworkspace.setDrawingSurfaceManager(drawingSurfaceManager);
			drawingSurfaceManager.addDsEventListener(this);
			ivjworkspace.createNewInternalFrame();
			
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjworkspace;
}
/**
 * Return the Zoom_InMenuItem property value.
 * @return javax.swing.JMenuItem
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JMenuItem getZoom_InMenuItem() {
	if (ivjZoom_InMenuItem == null) {
		try {
			ivjZoom_InMenuItem = new javax.swing.JMenuItem();
			ivjZoom_InMenuItem.setName("Zoom_InMenuItem");
			ivjZoom_InMenuItem.setMnemonic('I');
			ivjZoom_InMenuItem.setText("Zoom In");
			ivjZoom_InMenuItem.setAccelerator(javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_RIGHT,java.awt.Event.SHIFT_MASK));
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjZoom_InMenuItem;
}
/**
 * Return the Zoom_OutMenuItem property value.
 * @return javax.swing.JMenuItem
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JMenuItem getZoom_OutMenuItem() {
	if (ivjZoom_OutMenuItem == null) {
		try {
			ivjZoom_OutMenuItem = new javax.swing.JMenuItem();
			ivjZoom_OutMenuItem.setName("Zoom_OutMenuItem");
			ivjZoom_OutMenuItem.setMnemonic('O');
			ivjZoom_OutMenuItem.setText("Zoom Out");
			ivjZoom_OutMenuItem.setAccelerator(javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_LEFT,java.awt.Event.SHIFT_MASK));
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjZoom_OutMenuItem;
}

/**
 * Return the LeftMenuItem property value.
 * @return javax.swing.JMenuItem
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JMenuItem getLeftMenuItem() {
	if (leftMenuItem == null) {
		try {
			leftMenuItem = new javax.swing.JMenuItem();
			leftMenuItem.setName("LeftMenuItem");
			leftMenuItem.setText("Left");
			leftMenuItem.setAccelerator(javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_LEFT,java.awt.event.KeyEvent.CTRL_MASK));
			leftMenuItem.setVisible(false);
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return leftMenuItem;
}
/**
 * Return the RightMenuItem property value.
 * @return javax.swing.JMenuItem
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JMenuItem getRightMenuItem() {
	if (rightMenuItem == null) {
		try {
			rightMenuItem = new javax.swing.JMenuItem();
			rightMenuItem.setName("RightMenuItem");
			rightMenuItem.setText("Right");
			rightMenuItem.setAccelerator(javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_RIGHT,java.awt.event.KeyEvent.CTRL_MASK));
			rightMenuItem.setVisible(false);
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return rightMenuItem;
}
/**
 * Return the UpMenuItem property value.
 * @return javax.swing.JMenuItem
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JMenuItem getUpMenuItem() {
	if (upMenuItem == null) {
		try {
			upMenuItem = new javax.swing.JMenuItem();
			upMenuItem.setName("UpMenuItem");
			upMenuItem.setText("Up");
			upMenuItem.setAccelerator(javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_UP,java.awt.event.KeyEvent.CTRL_MASK));
			upMenuItem.setVisible(false);
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return upMenuItem;
}
/**
 * Return the DownMenuItem property value.
 * @return javax.swing.JMenuItem
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JMenuItem getDownMenuItem() {
	if (downMenuItem == null) {
		try {
			downMenuItem = new javax.swing.JMenuItem();
			downMenuItem.setName("DownMenuItem");
			downMenuItem.setText("Down");
			downMenuItem.setAccelerator(javax.swing.KeyStroke.getKeyStroke(java.awt.event.KeyEvent.VK_DOWN,java.awt.event.KeyEvent.CTRL_MASK));
			downMenuItem.setVisible(false);
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return downMenuItem;
}


/**
 * Return the ZoomLabel property value.
 * @return javax.swing.JLabel
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JLabel getZoomLabel() {
	if (ivjZoomLabel == null) {
		try {
			ivjZoomLabel = new javax.swing.JLabel();
			ivjZoomLabel.setName("ZoomLabel");
			ivjZoomLabel.setText("100%");
			ivjZoomLabel.setHorizontalTextPosition(javax.swing.SwingConstants.CENTER);
			ivjZoomLabel.setPreferredSize(new java.awt.Dimension(40, 0));
			ivjZoomLabel.setFont(new java.awt.Font("Arial", 1, 10));
			ivjZoomLabel.setHorizontalAlignment(javax.swing.SwingConstants.CENTER);
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjZoomLabel;
}
/**
 * Return the ZoomPanel property value.
 * @return javax.swing.JPanel
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JPanel getZoomPanel() {
	if (ivjZoomPanel == null) {
		try {
			ivjZoomPanel = new javax.swing.JPanel();
			ivjZoomPanel.setName("ZoomPanel");
			ivjZoomPanel.setPreferredSize(new java.awt.Dimension(200, 23));
			ivjZoomPanel.setLayout(new java.awt.BorderLayout());
			ivjZoomPanel.setMinimumSize(new java.awt.Dimension(50, 23));
			getZoomPanel().add(getZoomSlider(), "Center");
			getZoomPanel().add(getZoomLabel(), "East");
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjZoomPanel;
}
/**
 * Return the ZoomSlider property value.
 * @return javax.swing.JSlider
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private javax.swing.JSlider getZoomSlider() {
	if (ivjZoomSlider == null) {
		try {
			ivjZoomSlider = new javax.swing.JSlider();
			ivjZoomSlider.setName("ZoomSlider");
			ivjZoomSlider.setPaintLabels(false);
			ivjZoomSlider.setInverted(false);
			ivjZoomSlider.setPaintTicks(true);
			ivjZoomSlider.setValue(100);
			ivjZoomSlider.setSnapToTicks(true);
			ivjZoomSlider.setMinimum(20);
			ivjZoomSlider.setMinimumSize(new java.awt.Dimension(36, 23));
			ivjZoomSlider.setCursor(new java.awt.Cursor(java.awt.Cursor.W_RESIZE_CURSOR));
			ivjZoomSlider.setMaximumSize(new java.awt.Dimension(175, 23));
			ivjZoomSlider.setMajorTickSpacing(50);
			ivjZoomSlider.setPreferredSize(new java.awt.Dimension(175, 23));
			ivjZoomSlider.setFont(new java.awt.Font("dialog", 0, 10));
			ivjZoomSlider.setMaximum(250);
			ivjZoomSlider.setMinorTickSpacing(10);
			// user code begin {1}
			// user code end
		} catch (java.lang.Throwable ivjExc) {
			// user code begin {2}
			// user code end
			handleException(ivjExc);
		}
	}
	return ivjZoomSlider;
}
/**
 * Comment
     * @param windowEvent
 */
public void groupDialog_WindowOpened(java.awt.event.WindowEvent windowEvent) {
	String groupName = getStatusMsg2().getText().trim();
	int pos = groupName.indexOf("]");
	if (pos>0)
		groupName = groupName.substring(pos+2).trim();		// skip "]:"

	if (groupName.equals(Constants.MAIN_GROUP))
		getGroupNameTextField().setText("");
	else
		getGroupNameTextField().setText(groupName+Constants.GROUP_SEPARATOR);
	getNewGroupNameChecker().check();
}
/**
 * Comment
 */
public void groupMenuItem_ActionPerformed() {
	showGroupDialog();
}
/**
 * Comment
 * @param actionEvent
 */
public void groupOKButton_ActionPerformed(java.awt.event.ActionEvent actionEvent) {
	if (getGroupOKButton().isEnabled()) {
		VDBInterface vDBInterface =
			((GetVDBManager)CommandManager.getInstance().getCommand("GetVDBManager")).getManager();
		String errmsg = vDBInterface.checkGroupName(getGroupNameTextField().getText(), true);
		if (errmsg==null || !vDBInterface.isErrorMessage(errmsg)) {
			GetGUIInterface cmd = (GetGUIInterface)CommandManager.getInstance().getCommand("GetGUIMenuInterface");
		 	cmd.getGUIMenuInterface().group(getGroupNameTextField().getText());
			getGroupDialog().dispose();
		}
		else {
			getGroupOKButton().setEnabled(false);
			getGroupWarningLabel().setText(errmsg);
		}
	}
}
/**
 * Called whenever the part throws an exception.
 * @param exception java.lang.Throwable
 */
private void handleException(java.lang.Throwable exception) {

	Console.getInstance().println("--------- UNCAUGHT EXCEPTION ---------");
	Console.getInstance().println(exception);
	exception.printStackTrace(System.out);
	System.err.print(exception.getStackTrace());
}

/**
 * Comment
 */
private DBDDialog getDBDDialog()
{
	if (dbdDialog == null)
	{
		dbdDialog = new DBDDialog(this);
	}
	return dbdDialog;
}


/**
 * Comment
 */
public void help_TopicsMenuItem_ActionPerformed() {
	VisualDCTDocument doc = new VisualDCTDocument(this, getClass().getResource("/docs/help.html"));
	doc.setLocationRelativeTo(this);
	doc.setVisible(true);
}
/**
 * Comment
 */
public void import_DBDMenuItem_ActionPerformed() {
	DBDDialog dialog = getDBDDialog();
	dialog.setLocationRelativeTo(this);
	dialog.setVisible(true);
}
/**
 * Comment
 */
public void import_DBMenuItem_ActionPerformed() {
	JFileChooser chooser = getfileChooser();
	UniversalFileFilter filter = new UniversalFileFilter(
		new String[] {"db", "vdb"}, "EPICS DB files");
	chooser.resetChoosableFileFilters();
	chooser.addChoosableFileFilter(filter);
	chooser.setDialogTitle("Import DB");
	chooser.setFileSelectionMode(JFileChooser.FILES_ONLY);
	int retval = chooser.showOpenDialog(this);

	if(retval == JFileChooser.APPROVE_OPTION) {
	    java.io.File theFile = chooser.getSelectedFile();
	    if(theFile != null) {
		    GetGUIInterface cmd = (GetGUIInterface)CommandManager.getInstance().getCommand("GetGUIMenuInterface");
		    try {
	  		 	cmd.getGUIMenuInterface().importDB(theFile);
		    } catch (java.io.IOException e) {
			    Console.getInstance().println("o) Failed to import DB file: '"+theFile.toString()+"'");
			    Console.getInstance().println(e);
		    }
		}
	}
}

/**
 * Comment
 */
public void importTemplateActionPerformed() {
	JFileChooser chooser = getfileChooser();
	UniversalFileFilter filter = new UniversalFileFilter(
		new String[] {"db", "vdb"}, "EPICS DB files");
	chooser.resetChoosableFileFilters();
	chooser.addChoosableFileFilter(filter);
	chooser.setDialogTitle("Import template DB");
	chooser.setFileSelectionMode(JFileChooser.FILES_ONLY);
	int retval = chooser.showOpenDialog(this);

	if(retval == JFileChooser.APPROVE_OPTION) {
	    java.io.File theFile = chooser.getSelectedFile();
	    if(theFile != null) {
		    GetGUIInterface cmd = (GetGUIInterface)CommandManager.getInstance().getCommand("GetGUIMenuInterface");
		    try {
	  		 	cmd.getGUIMenuInterface().importTemplateDB(theFile);
		    } catch (java.io.IOException e) {
			    Console.getInstance().println("o) Failed to import template DB file: '"+theFile.toString()+"'");
			    Console.getInstance().println(e);
		    }
		}
	}
}

/**
 * Comment
 */
public void importFieldsActionPerformed() {
	JFileChooser chooser = getfileChooser();
	UniversalFileFilter filter = new UniversalFileFilter(
		new String[] {"db", "vdb"}, "EPICS DB files");
	chooser.resetChoosableFileFilters();
	chooser.addChoosableFileFilter(filter);
	chooser.setDialogTitle("Import fields...");
	chooser.setFileSelectionMode(JFileChooser.FILES_ONLY);
	int retval = chooser.showOpenDialog(this);

	if(retval == JFileChooser.APPROVE_OPTION) {
	    java.io.File theFile = chooser.getSelectedFile();
	    if(theFile != null) {
		    GetGUIInterface cmd = (GetGUIInterface)CommandManager.getInstance().getCommand("GetGUIMenuInterface");
		    try {
	  		 	cmd.getGUIMenuInterface().importFields(theFile);
		    } catch (java.io.IOException e) {
			    Console.getInstance().println("o) Failed to import fields from DB file: '"+theFile.toString()+"'");
			    Console.getInstance().println(e);
		    }
		}
	}
}

/**
 * Comment
 */
public void importBorderActionPerformed() {
	JFileChooser chooser = getfileChooser();
	UniversalFileFilter filter = new UniversalFileFilter(
		new String[] {"db", "vdb"}, "EPICS DB files");
	chooser.resetChoosableFileFilters();
	chooser.addChoosableFileFilter(filter);
	chooser.setDialogTitle("Import border...");
	chooser.setFileSelectionMode(JFileChooser.FILES_ONLY);
	int retval = chooser.showOpenDialog(this);

	if(retval == JFileChooser.APPROVE_OPTION) {
	    java.io.File theFile = chooser.getSelectedFile();
	    if(theFile != null) {
		    GetGUIInterface cmd = (GetGUIInterface)CommandManager.getInstance().getCommand("GetGUIMenuInterface");
		    try {
	  		 	cmd.getGUIMenuInterface().importBorder(theFile);
		    } catch (java.io.IOException e) {
			    Console.getInstance().println("o) Failed to import border from DB file: '"+theFile.toString()+"'");
			    Console.getInstance().println(e);
		    }
		}
	}
}

/**
 * Initializes connections
 * @exception java.lang.Exception The exception description.
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private void initConnections() throws java.lang.Exception {
	// user code begin {1}
	getLeftMenuItem().addActionListener(ivjEventHandler);
	getRightMenuItem().addActionListener(ivjEventHandler);
	getUpMenuItem().addActionListener(ivjEventHandler);
	getDownMenuItem().addActionListener(ivjEventHandler);
	getGenerateMenuItem().addActionListener(ivjEventHandler);
	getGenerateAsGroupMenuItem().addActionListener(ivjEventHandler);
	getSaveAsTemplateMenuItem().addActionListener(ivjEventHandler);
	getSpreadsheetMenuItem().addActionListener(ivjEventHandler);
	// user code end
	getAbout_BoxMenuItem().addActionListener(ivjEventHandler);
	getZoomSlider().addMouseMotionListener(ivjEventHandler);
	getZoomSlider().addMouseListener(ivjEventHandler);
	getNewMenuItem().addActionListener(ivjEventHandler);
	getOpenMenuItem().addActionListener(ivjEventHandler);
	getImport_DBMenuItem().addActionListener(ivjEventHandler);
	getImportFieldsMenuItem().addActionListener(ivjEventHandler);
	getImportBorderMenuItem().addActionListener(ivjEventHandler);
	getImportTemplate_DBMenuItem().addActionListener(ivjEventHandler);
	getImport_DBDMenuItem().addActionListener(ivjEventHandler);
	getSaveMenuItem().addActionListener(ivjEventHandler);
	getSave_AsMenuItem().addActionListener(ivjEventHandler);
	getSave_As_GroupMenuItem().addActionListener(ivjEventHandler);
	getPrintMenuItem().addActionListener(ivjEventHandler);
	getExitMenuItem().addActionListener(ivjEventHandler);
	getUndoMenuItem().addActionListener(ivjEventHandler);
	getRedoMenuItem().addActionListener(ivjEventHandler);
	getCutMenuItem().addActionListener(ivjEventHandler);
	getCopyMenuItem().addActionListener(ivjEventHandler);
	getPasteMenuItem().addActionListener(ivjEventHandler);
	getMove_RenameMenuItem().addActionListener(ivjEventHandler);
	getMorphMenuItem().addActionListener(ivjEventHandler);
	getFindMenuItem().addActionListener(ivjEventHandler);
	getGroupMenuItem().addActionListener(ivjEventHandler);
	getUngroupMenuItem().addActionListener(ivjEventHandler);
	getDeleteMenuItem().addActionListener(ivjEventHandler);
	getSelect_AllMenuItem().addActionListener(ivjEventHandler);
	getHelp_TopicsMenuItem().addActionListener(ivjEventHandler);
	getBooks_OnlineMenuItem().addActionListener(ivjEventHandler);
	getOpenButton().addActionListener(ivjEventHandler);
	getSaveButton().addActionListener(ivjEventHandler);
	getUndoButton().addActionListener(ivjEventHandler);
	getRedoButton().addActionListener(ivjEventHandler);
	getCutButton().addActionListener(ivjEventHandler);
	getCopyButton().addActionListener(ivjEventHandler);
	getPasteButton().addActionListener(ivjEventHandler);
	getFindButton().addActionListener(ivjEventHandler);
	getLevel_UpMenuItem().addActionListener(ivjEventHandler);
	getZoom_InMenuItem().addActionListener(ivjEventHandler);
	getZoom_OutMenuItem().addActionListener(ivjEventHandler);
	getSmart_ZoomMenuItem().addActionListener(ivjEventHandler);
	getBase_ViewMenuItem().addActionListener(ivjEventHandler);
	getPreferences___MenuItem().addActionListener(ivjEventHandler);
	getShow_PointsMenuItem().addItemListener(ivjEventHandler);
	getSnapToGridMenuItem().addItemListener(ivjEventHandler);
	getWindowsPanMenuItem().addItemListener(ivjEventHandler);
	getOKButton().addActionListener(ivjEventHandler);
	getCancelButton().addActionListener(ivjEventHandler);
	getMorphingOKButton().addActionListener(ivjEventHandler);
	getMorphingCancelButton().addActionListener(ivjEventHandler);
	//getNameTextField().addActionListener(ivjEventHandler);
	getNewRecordDialog().addWindowListener(ivjEventHandler);
	getMorphingDialog().addWindowListener(ivjEventHandler);
	getStatusbarMenuItem().addActionListener(ivjEventHandler);
	getToolbarMenuItem().addActionListener(ivjEventHandler);
	getFlat_ViewMenuItem().addActionListener(ivjEventHandler);
	getNavigatorMenuItem().addItemListener(ivjEventHandler);
	getCancelButton1().addActionListener(ivjEventHandler);
	getGroupDialog().addWindowListener(ivjEventHandler);
	getGroupOKButton().addActionListener(ivjEventHandler);
	getGroupNameTextField().addActionListener(ivjEventHandler);
	getRenameCancelButton().addActionListener(ivjEventHandler);
	getRenameOKButton().addActionListener(ivjEventHandler);
	getNewNameTextField().addActionListener(ivjEventHandler);
	getRenameDialog().addWindowListener(ivjEventHandler);
	getPageSetupMenuItem().addActionListener(ivjEventHandler);
	getPrintPreviewMenuItem().addActionListener(ivjEventHandler);
	getPluginManagerMenuItem().addActionListener(ivjEventHandler);

	getExportPostScriptFileMenuItem().addActionListener(ivjEventHandler);
	getPrintAsPostScriptMenuItem().addActionListener(ivjEventHandler);

	getLineButton().addActionListener(ivjEventHandler);
	getBoxButton().addActionListener(ivjEventHandler);
	getTextBoxButton().addActionListener(ivjEventHandler);

	getSystemCopyMenuItem().addActionListener(ivjEventHandler);
	getSystemPasteMenuItem().addActionListener(ivjEventHandler);
}
/**
 * Initialize the class.
 */
/* WARNING: THIS METHOD WILL BE REGENERATED. */
private void initialize() {
	try {
		// user code begin {1}
		setFileInTitle(null);
		setCurrentGroup(null);
		CommandManager.getInstance().addCommand("SetWorkspaceScale", new SetWorkspaceScale(this));
		CommandManager.getInstance().addCommand("ShowNewDialog", new ShowNewDialog(this));
		CommandManager.getInstance().addCommand("ShowRenameDialog", new ShowRenameDialog(this));
		CommandManager.getInstance().addCommand("ShowMorphingDialog", new ShowMorphingDialog(this));
		CommandManager.getInstance().addCommand("SetUndoMenuItemState", new SetUndoMenuItemState(this));
		CommandManager.getInstance().addCommand("SetRedoMenuItemState", new SetRedoMenuItemState(this));
		CommandManager.getInstance().addCommand("SetFile", new SetWorkspaceFile(this));
		CommandManager.getInstance().addCommand("SetGroup", new SetWorkspaceGroup(this));
		CommandManager.getInstance().addCommand("UpdateLoadLabel", new UpdateLoadLabel(this));
		CommandManager.getInstance().addCommand("SetDefaultFocus", new SetDefaultFocus(this));
		CommandManager.getInstance().addCommand("SaveCommand", new SaveCommand(this));
		CommandManager.getInstance().addCommand("GetMainComponent", new GetMainComponent(this));
		// user code end
		setName("VisualDCT");
//		setDefaultCloseOperation(javax.swing.WindowConstants.DISPOSE_ON_CLOSE);
		setDefaultCloseOperation(javax.swing.WindowConstants.DO_NOTHING_ON_CLOSE);
		setJMenuBar(getVisualDCTJMenuBar());
		setSize(460, 300);
		setTitle("VisualDCT");
		setContentPane(getJFrameContentPane());
		initConnections();
	} catch (java.lang.Throwable ivjExc) {
		handleException(ivjExc);
	}
	// user code begin {2}
	com.cosylab.vdct.inspector.InspectorManager.setParent(this);

	setDsCommandsState(true);
	// user code end
}
/**
 * Comment
 */
public void level_UpMenuItem_ActionPerformed() {
	GetGUIInterface cmd = (GetGUIInterface)CommandManager.getInstance().getCommand("GetGUIMenuInterface");
    cmd.getGUIMenuInterface().levelUp();
}

public void lineButton_ActionPerformed()
{
	if(lineButtonEnabled)
		setLineButtonEnabled(false);
	else
		setLineButtonEnabled(true);
}

/**
 * Starts the application.
 * main entrypoint - starts the part when it is run as an application
 * 		java VisualDCT  [.dbd-file]  [.db-file]
 * @param args java.lang.String[] an array of command-line arguments
 */
public static void main(final java.lang.String[] args) {

    	if (args.length > 0) {
    	    if (args[0].equals(Help.HELP) || args[0].equals(Help.HELP2)) {
    	        Help.printHelpAndExit();
    	    }
    	}
    
		System.out.println("Loading VisualDCT v"+Version.VERSION+"...\n");
		
		String javaVersion = (String)System.getProperties().get("java.version");
		if (javaVersion!=null && javaVersion.compareTo(Version.JAVA_VERSION_REQUIRED) == -1)
			System.out.println("WARNING: Java "+javaVersion+" detected. VisualDCT requires Java "+Version.JAVA_VERSION_REQUIRED+" or newer!\n");

		/* Set default directory */
		final String dir = System.getProperty("VDCT_DIR");
		if (dir!=null && new java.io.File(dir).exists())
			Settings.setDefaultDir(dir);
		else
			Settings.setDefaultDir(".");
			
		/* Set cross-platform look and feel (e.g. MacOS LAF has bugs) */
		try {
			// UIManager.setLookAndFeel(UIManager.getSystemLookAndFeelClassName());
			UIManager.setLookAndFeel(UIManager.getCrossPlatformLookAndFeelClassName());
		} catch (Throwable th) {
			th.printStackTrace();
		}
		
		/* Calculate the screen size */
		final Dimension screenSize = Toolkit.getDefaultToolkit().getScreenSize();

		/* Create the splash screen */
		final VisualDCTSplashScreen aVisualDCTSplashScreen = new VisualDCTSplashScreen();
		aVisualDCTSplashScreen.pack();

		/* Center splash screen */
		Dimension splashScreenSize = aVisualDCTSplashScreen.getSize();
		if (splashScreenSize.height > screenSize.height)
				splashScreenSize.height = screenSize.height;
		if (splashScreenSize.width > screenSize.width)
				splashScreenSize.width = screenSize.width;
		aVisualDCTSplashScreen.setLocation((screenSize.width - splashScreenSize.width) / 2, (screenSize.height - splashScreenSize.height) / 2);

		// wait until it appears and FontMetricsBuffer is initialized
		synchronized (aVisualDCTSplashScreen)
		{
			aVisualDCTSplashScreen.setVisible(true);
			try {
				aVisualDCTSplashScreen.wait();
			} catch (InterruptedException e) {
			}
		}
		
		SwingUtilities.invokeLater(new Runnable() {
			public void run() {
				try {
					
		/* Create the frame */
       // JFrame.setDefaultLookAndFeelDecorated(true);
        //JDialog.setDefaultLookAndFeelDecorated(true);
		final VisualDCT aVisualDCT = new VisualDCT();
		aVisualDCT.setSize(Constants.VDCT_WIDTH, Constants.VDCT_HEIGHT);
		aVisualDCT.getworkspace();

		/* Dispose splash screen */
		aVisualDCTSplashScreen.dispose();

		/* Set default directory (for GUI !!! to be cleaned, use Setting singleton) */
		if (dir!=null) aVisualDCT.setDefaultDirectory(dir);
		else aVisualDCT.setDefaultDirectory(".");		// current dir
		
		/* Parameters */
		ConsoleInterface console = Console.getInstance();
		console.silent("Loading VisualDCT v"+Version.VERSION+"...\n");

		/* Recent files */
		Settings.getInstance().loadRecentFiles();

		/* Plugins */
		com.cosylab.vdct.plugin.PluginManager.getInstance().checkAutoStartPlugins();
		
		/* Load xml settings. */
		XmlSettings.getInstance();
	
		System.out.println();
			
		if (args.length==0) {
			System.out.println("\no) Usage: java "+VisualDCT.class.getName()+" [<DBDs>] [<DB>]\n");
			aVisualDCT.openDBD(null, true);					// bring up a file dialog
		}
		else {
			for (int i=0; i < args.length; i++)
				if (args[i].toUpperCase().endsWith("DBD"))
				{
					System.out.println("Directive to load DBD: '"+args[i]+"'.");
					aVisualDCT.openDBD(args[i]);
				}

			// only one DB
			if (!args[args.length-1].toUpperCase().endsWith("DBD"))
			{
				System.out.println("Directive to load DB: '"+args[args.length-1]+"'.");
				aVisualDCT.openDB(args[args.length-1]);
			}

		}

		
		if (DataProvider.getInstance().getDbdDB()==null) {
			System.out.println("No DBD loaded! Exiting...");
			com.cosylab.vdct.plugin.PluginManager.getInstance().destroyAllPlugins();
			System.exit(1);
		}

		/* Center frame on the screen */
		Dimension frameSize = aVisualDCT.getSize();
		if (frameSize.height > screenSize.height)
				frameSize.height = screenSize.height;
		if (frameSize.width > screenSize.width)
				frameSize.width = screenSize.width;
		aVisualDCT.setLocation((screenSize.width - frameSize.width) / 2, (screenSize.height - frameSize.height) / 2);

		/* Add a windowListener for the windowClosedEvent */
		aVisualDCT.addWindowListener(new java.awt.event.WindowAdapter() {
			
			public void windowActivated(WindowEvent arg0) {
				super.windowActivated(arg0);
				DataSynchronizer.getInstance().checkFilesystemChanges(null);
			}

			public void windowClosing(java.awt.event.WindowEvent e) {
				aVisualDCT.exitMenuItem_ActionPerformed();
			};

			public void windowClosed(java.awt.event.WindowEvent e) {
				Settings.getInstance().save();
				XmlSettings.getInstance().save();
				
				com.cosylab.vdct.plugin.PluginManager.getInstance().save();
				com.cosylab.vdct.plugin.PluginManager.getInstance().destroyAllPlugins();
				System.out.println();
				System.out.println("VisualDCT exited.");
				System.exit(0);
			};
		});
		aVisualDCT.setVisible(true);
		
		aVisualDCT.getworkspace().selectFirstInternalFrame();

	} catch (Throwable exception) {
		System.err.println("Exception occurred in main() of VisualDCT");
		exception.printStackTrace(System.out);
	}

			}
		});		

}

/**
 * Comment
 */
public void move_RenameMenuItem_ActionPerformed() {
	GetGUIInterface cmd = (GetGUIInterface)CommandManager.getInstance().getCommand("GetGUIMenuInterface");
 	cmd.getGUIMenuInterface().rename();
}
public void morphMenuItem_ActionPerformed() {
	GetGUIInterface cmd = (GetGUIInterface)CommandManager.getInstance().getCommand("GetGUIMenuInterface");
	cmd.getGUIMenuInterface().morph();
}
/**
 * Comment
 * @param itemEvent
 */
public void navigatorMenuItem_ItemStateChanged(java.awt.event.ItemEvent itemEvent) {
	GetGUIInterface cmd = (GetGUIInterface)CommandManager.getInstance().getCommand("GetGUIMenuInterface");
	if (cmd!=null)
		cmd.getGUIMenuInterface().showNavigator(getNavigatorMenuItem().isSelected());
	Settings.getInstance().setNavigator(getNavigatorMenuItem().isSelected());
}

public void windowsPanMenuItem_ItemStateChanged(java.awt.event.ItemEvent itemEvent) {
	/*GetGUIInterface cmd = (GetGUIInterface)*/CommandManager.getInstance().getCommand("GetGUIMenuInterface");
	/*if (cmd!=null)
		cmd.getGUIMenuInterface().???(getWindowsPanMenuItem().isSelected());*/
	Settings.getInstance().setWindowsPan(getWindowsPanMenuItem().isSelected());
}
/**
 * Insert the method's description here.
 * Creation date: (3.2.2001 19:55:14)
 */
public void newMenuItem_ActionPerformed() {
	
	GetGUIInterface cmd2 = (GetGUIInterface)CommandManager.getInstance().getCommand("GetGUIMenuInterface");
		cmd2.getGUIMenuInterface().newCmd();
	openedFile = null;
}
/**
 * Comment
 * @param windowEvent
 */
public void newRecordDialog_WindowOpened(
    java.awt.event.WindowEvent windowEvent) {

	// there can be no less types (for now)
    Object[] names = DataProvider.getInstance().getRecordTypes();
    JComboBox combo = getTypeComboBox();

    
    if (names.length > combo.getItemCount())
     {
        ComboBoxModel model = combo.getModel();
    	boolean found = false;
	    
		for (int i = 0; i < names.length; i++)
		{
			int c = model.getSize();
			for (int j=0;!found && (j<c); j++)
				if (model.getElementAt(j).equals(names[i]))
					found = true;
					
			if (!found)
		    	combo.addItem(names[i]);
		}
     }

    ComboBoxModel model = getNameTextField().getModel();
	String defaultName = PluginNameConfigManager.getInstance().getDefaultName();
	if (defaultName != null) {
		int j = 0;
		for (j = 0; j < model.getSize(); j++) {
			if (model.getElementAt(j).equals(defaultName)) {
				break;
			}
		}
		if (j == model.getSize()) {
			getNameTextField().addItem(defaultName);
		}
	}

    Object selected = getNameTextField().getSelectedItem();
    if (selected != null && !selected.toString().endsWith(":")) {
    	getNameTextField().setSelectedItem(defaultName != null ? defaultName : "");
    }

    NameChecker checker = getNewRecordNameChecker();
    if (checker != null) {
    	checker.check();
    }
    getNameTextField().requestFocus();
}

public void morphingDialog_WindowOpened(
	java.awt.event.WindowEvent windowEvent) {
/*
	// there can be no less types (for now)
	Object[] names = DataProvider.getInstance().getRecordTypes();
	JComboBox combo = getTypeComboBox2();

	if (names.length > combo.getItemCount())
	 {
		ComboBoxModel model = combo.getModel();
	    
		for (int i = 0; i < names.length; i++)
		{
			boolean found = false;
			int c = model.getSize();
			for (int j=0;!found && (j<c); j++)
				if (model.getElementAt(j).equals(names[i]))
					found = true;
					
			if (!found)
				combo.addItem(names[i]);
		}
	 }
*/
}

/**
 * Comment
 * @param actionEvent
 */
public void oKButton_ActionPerformed(java.awt.event.ActionEvent actionEvent) {

	if (getOKButton().isEnabled()) {

		VDBInterface vDBInterface =
			((GetVDBManager)CommandManager.getInstance().getCommand("GetVDBManager")).getManager();
		String enteredValue;
		if (actionEvent.getSource() instanceof JTextField)
		    enteredValue = ((JTextField)actionEvent.getSource()).getText();
		else
			enteredValue = getNameTextField().getSelectedItem().toString();
		String errmsg = vDBInterface.checkRecordName(enteredValue, null, true);
		if (errmsg==null || !vDBInterface.isErrorMessage(errmsg)) {
		    // this will change value of combo box
		    updateComboBoxHistory(getNameTextField(), enteredValue);
		    vDBInterface.createRecord(
				enteredValue,
				getTypeComboBox().getSelectedItem().toString(),
				true);
			getNewRecordDialog().dispose();
		}
		else {
			getOKButton().setEnabled(false);
			getWarningLabel().setText(errmsg);
		}
	}
}

private static void updateComboBoxHistory(JComboBox cb, Object item)
{
		//Object item = cb.getSelectedItem();
		if (item == null)
		    return;
		
		// search for the already entered qualifier
		String valueToAdd = item.toString();
		
		// retrieve prefix - part of the name before last ':'
	    int pos = valueToAdd.lastIndexOf(':');
	    if (pos > 0)
	        valueToAdd = valueToAdd.substring(0, pos+1);
	    else {
	        // do not add if no ":"
	        return;
	    }
		
		Object qualifier = null;
		int len = cb.getModel().getSize(); 
		for (int i = 0; i < len; i++)
		{
			Object it = cb.getModel().getElementAt(i); 
			if (it != item && it.toString().equals(valueToAdd))
			{
				qualifier = (Object)it;
				break;
			}
		}

		// new one, create qualifier and select it
		if (qualifier == null )
		{
			qualifier = new StringBuffer(valueToAdd);
			cb.addItem(qualifier);
		}
		
		// select the qualifier (prefix)
		cb.setSelectedItem(qualifier);
		
		// remove string object
		cb.removeItem(item);
}

public void morphingOKButton_ActionPerformed(java.awt.event.ActionEvent actionEvent) {
	GetGUIInterface cmd = (GetGUIInterface)CommandManager.getInstance().getCommand("GetGUIMenuInterface");
	cmd.getGUIMenuInterface().morph(getNameTextLabel().getText(), getTypeComboBox2().getSelectedItem().toString());
	getMorphingDialog().dispose();
}

/**
 * Insert the method's description here.
 * Creation date: (26.1.2001 22:22:29)
 * @param fileName java.lang.String
 */
public void openDB(String fileName) {
	if (fileName.indexOf(java.io.File.separatorChar)<0)
		fileName = getfileChooser().getCurrentDirectory().toString()+
									java.io.File.separatorChar+
									fileName;
		
	java.io.File theFile = new java.io.File(fileName);
	if(theFile != null && theFile.exists()) {
		
		// add to the recent file list
		getRecentFilesMenu().addFile(theFile);
		Settings.getInstance().saveRecentFiles();
		
	    GetGUIInterface cmd = (GetGUIInterface)CommandManager.getInstance().getCommand("GetGUIMenuInterface");
	    try {	
		    cmd.getGUIMenuInterface().openDB(theFile);
		    openedFile = theFile;
		} catch (java.io.IOException e) {
		    Console.getInstance().println("o) Failed to open DB file: '"+theFile.toString()+"'");
		    Console.getInstance().println(e);
	    }
	}
	else if (!theFile.exists()) 
	{
		 Console.getInstance().println("o) Failed to open DB file - file does not exist: '"+theFile.getAbsolutePath()+"'.");
		 Console.getInstance().println();
	}

}

/**
 * Insert the method's description here.
 * Creation date: (8.1.2001 20:30:58)
 * @param fileName
 * @return 
 */
public boolean openDBD(String fileName) {
	return openDBD(fileName, false);
}

/**
 * Insert the method's description here.
 * Creation date: (8.1.2001 20:30:58)
 * @param fileName
 * @param allowDB
 * @return 
 */
public boolean openDBD(String fileName, boolean allowDB) {
	if (fileName!=null && fileName.indexOf(java.io.File.separatorChar)<0)
		fileName = getfileChooser().getCurrentDirectory().toString()+
									java.io.File.separatorChar+
									fileName;

	java.io.File theFile = null;
	if (fileName==null) {
		JFileChooser chooser = getfileChooser();

		// SLS request
		UniversalFileFilter filter2 = new UniversalFileFilter(
			new String[] { "template" }, "EPICS template file");
		chooser.addChoosableFileFilter(filter2);

		String[] set;
		String desc;
		if (allowDB)
		{
			desc = "EPICS DBD/DB files";
			String[] tset = {"dbd", "db", "vdb"};
			set = tset;
		}
		else
		{
			desc = "EPICS DBD files";
			String[] tset = {"dbd"};
			set = tset;
		}
		UniversalFileFilter filter = new UniversalFileFilter(set, desc);
		chooser.addChoosableFileFilter(filter);

		chooser.setFileSelectionMode(JFileChooser.FILES_ONLY);
		int retval = chooser.showOpenDialog(this);
		if(retval == JFileChooser.APPROVE_OPTION)
		    theFile = chooser.getSelectedFile();
	}
	else
		theFile = new java.io.File(fileName);

	if (theFile==null)
		return false;			// canceled dialog
	else if (!theFile.exists())
	{
		 System.out.println("o) Failed to open DBD/DB file - file does not exist: '"+theFile.getAbsolutePath()+"'.");
		 return false;
	}
	
	GetGUIInterface cmd = (GetGUIInterface)CommandManager.getInstance().getCommand("GetGUIMenuInterface");

	try {

		if (!allowDB || theFile.getName().toUpperCase().endsWith("DBD")) {
			DBDEntry entry = new DBDEntry(theFile.getPath());
			Vector v = DataProvider.getInstance().getCurrentDBDs();
			if (v.isEmpty()) {
			    //if this is the first entry being added, open it
			    cmd.getGUIMenuInterface().openDBD(entry.getFile());
			} else {
			    //if not the first one, import it.
			    cmd.getGUIMenuInterface().importDBD(entry.getFile());
			}
			v.add(entry);
		}
		else if (allowDB)
			cmd.getGUIMenuInterface().openDB(theFile);
	} catch (java.io.IOException e) {
	    Console.getInstance().println("o) Failed to open DBD/DB file: '"+theFile.getAbsolutePath()+"'.");
	    Console.getInstance().println(e);
	    return false;
	}


	return true; 		// !!! add check
	
}
/**
 * Comment
 */
public void openMenuItem_ActionPerformed() {
	JFileChooser chooser = getfileChooser();
	chooser.resetChoosableFileFilters();

	// SLS request
	UniversalFileFilter filter2 = new UniversalFileFilter(
		new String[] { "template" }, "EPICS template file");
	chooser.addChoosableFileFilter(filter2);

	UniversalFileFilter filter = new UniversalFileFilter(
		new String[] {"db", "vdb"}, "EPICS DB files");
	chooser.addChoosableFileFilter(filter);
	
	chooser.setFileSelectionMode(JFileChooser.FILES_ONLY);
	chooser.setDialogTitle("Open DB");
	int retval = chooser.showOpenDialog(this);

	if(retval == JFileChooser.APPROVE_OPTION) {
	    java.io.File theFile = chooser.getSelectedFile();
	    if(theFile != null) {

			// add to the recent file list
			getRecentFilesMenu().addFile(theFile);
			Settings.getInstance().saveRecentFiles();
		
		    GetGUIInterface cmd = (GetGUIInterface)CommandManager.getInstance().getCommand("GetGUIMenuInterface");
		    try {
				cmd.getGUIMenuInterface().openDB(theFile);
			    openedFile = theFile;
			} catch (IOException e) {
		 		Console.getInstance().println("o) Failed to open DB: '"+theFile.toString()+"'");
			    Console.getInstance().println(e);
			}
		}
	}
}

/**
 * Comment
 * @param actionEvent
 */
public void pageSetupMenuItem_ActionPerformed(java.awt.event.ActionEvent actionEvent) {
//	new Thread() {
//		public void run()
//		{

			final GetPrintableInterface pi = (GetPrintableInterface)CommandManager.getInstance().getCommand("GetPrintableInterface");
		
			if(pi == null)
				return;
			
			PrintRequestAttributeSet printRequestAttributeSet = Page.getPrintRequestAttributeSet();
		
			PrinterJob printerJob = PrinterJob.getPrinterJob();
			PrintService temp = PrinterSelector.getPrinterSelector(this).getPrintService(lastPrintService);
			
			if (temp == null) {
			    return;
			} else {
			    lastPrintService = temp;
			    try {
                    printerJob.setPrintService(lastPrintService);
                } catch (PrinterException e) {
                    handleException(e);
                }
			}
							
			PageFormat pageFormat = printerJob.pageDialog(printRequestAttributeSet);
			
			if(pageFormat==null)
				return;
		
			int pageFormatOrientation = PageFormat.PORTRAIT;

			OrientationRequested orientationRequested = (OrientationRequested)
				printRequestAttributeSet.get(OrientationRequested.class);
				
			if(orientationRequested != null)
			{
				if(orientationRequested.equals(OrientationRequested.LANDSCAPE))
					pageFormatOrientation = PageFormat.LANDSCAPE;
				else if(orientationRequested.equals(OrientationRequested.REVERSE_LANDSCAPE))
					pageFormatOrientation = PageFormat.REVERSE_LANDSCAPE;
			}
			
			pi.getPageable().getPageFormat(0).setOrientation(pageFormatOrientation);
		
			MediaPrintableArea mediaPrintableArea = (MediaPrintableArea)
				printRequestAttributeSet.get(MediaPrintableArea.class);
				
			Paper paper = pi.getPageable().getPageFormat(0).getPaper();
		
			paper.setImageableArea(mediaPrintableArea.getX(MediaPrintableArea.INCH) * 72.0,
				mediaPrintableArea.getY(MediaPrintableArea.INCH) * 72.0,
				mediaPrintableArea.getWidth(MediaPrintableArea.INCH) * 72,
				mediaPrintableArea.getHeight(MediaPrintableArea.INCH) * 72);
		
			pi.getPageable().getPageFormat(0).setPaper(paper);
						
			// store settings 
			com.cosylab.vdct.graphics.printing.Page.setPageFormat(pageFormat);
			Page.setPrintRequestAttributeSet(printRequestAttributeSet);
	//	}
	//}.start();

}
/**
 * Comment
 */
public void pasteMenuItem_ActionPerformed() {
    GetGUIInterface cmd = (GetGUIInterface)CommandManager.getInstance().getCommand("GetGUIMenuInterface");
    cmd.getGUIMenuInterface().paste();
}
/**
 * Comment
 * @param actionEvent
 */
public void pluginManagerMenuItem_ActionPerformed(java.awt.event.ActionEvent actionEvent)
{
	if(pluginManagerDialog == null)
		pluginManagerDialog = new PluginManagerDialog(this);

	pluginManagerDialog.setLocationRelativeTo(this);
	pluginManagerDialog.setVisible(true);
}
/**
 * Comment
 */
public void preferences___MenuItem_ActionPerformed() {
	SettingsDialog sd = new SettingsDialog(this, true);
	sd.setLocationRelativeTo(this);
	sd.setVisible(true);
}

/**
 * Comment
 */
public void spreadsheetMenuItemActionPerformed() {
	
    // Do nothing if no drawing surface.
	Group root = Group.getRoot();
	if (root == null) {
		return;
	}
	
	SpreadsheetInspector spreadsheet = getSpreadsheetInspector();
	spreadsheet.setDsId(root.getDsId());
	spreadsheet.setLocationRelativeTo(this);
	spreadsheet.setVisible(true);
}

public SpreadsheetInspector getSpreadsheetInspector() {
	
	if (spreadsheet == null) {
		spreadsheet = new SpreadsheetInspector(this, true);
	}
	return spreadsheet;
}

public byte[] printGrid(PageFormat pageFormat, StringBuffer stringBuffer, ViewState view) {
						
		int pageWidth = (int)pageFormat.getImageableWidth();
		int pageHeight = (int)pageFormat.getImageableHeight();
		
		NullCommand pm =
			(NullCommand)CommandManager.getInstance().getCommand("NullCommand");
		Component component = pm.getComponent();
	
		double screen2printer = 0;
	
		switch(Page.getPrintMode())
		{
			case Page.TRUE_SCALE:
				// 1:1 ratio
				screen2printer =
					72.0 / component.getToolkit().getScreenResolution();
				break;
			case Page.USER_SCALE:
				screen2printer =
					72.0 / component.getToolkit().getScreenResolution();
				screen2printer *= Page.getUserScale();
				break;
				
			case Page.FIT_SCALE:
				// fit to paper
				double xscale = pageWidth / (double)view.getViewWidth();
				double yscale = pageHeight / (double)view.getViewHeight();
				screen2printer = Math.min(xscale, yscale) * view.getScale();
				break;
		}
		
		double converter = screen2printer / view.getScale();
		int w = (int)(view.getViewWidth() * converter);
		int h = (int)(view.getViewHeight() * converter);
	
		int nCol = Math.max((int)Math.ceil((double)w / pageWidth), 1);
		int nRow = Math.max((int)Math.ceil((double)h / pageHeight), 1);
		int maxNumPage = nCol * nRow;
	
		String numberOfPages = String.valueOf(maxNumPage);
		
		
		int stringOffset = stringBuffer.indexOf("%%BeginProlog");
		stringBuffer.insert(stringOffset, "%%Pages: " + numberOfPages + "\n");
		
		for(int pageIndex = 0; pageIndex < maxNumPage; pageIndex++)
		{
			int iCol = pageIndex % nCol;
			int iRow = pageIndex / nCol;
			int x = iCol * pageWidth;
			int y = iRow * pageHeight;
	
			int imageWidth = Math.min(pageWidth, w-x);
			int imageHeight = Math.min(pageHeight, h-y);
			
			/*view.setScale(1.0);*/
			int viewRx = (int)(view.getRx()/view.getScale()+x);
			int viewRy= (int)(view.getRy()/view.getScale()+y);
			int viewViewWidth=(int)(imageWidth/screen2printer);
			int viewViewHeight=(int)(imageHeight/screen2printer);
	
			String postScriptGrid = new String();
		
			if (Settings.getInstance().getShowGrid())
			{
				postScriptGrid += "\n"
					+ (new Double(Constants.GRID_COLOR.getRed() / 255.0)).toString()
					+ " " + (new Double(Constants.GRID_COLOR.getGreen()
					/ 255.0)).toString() + " "
					+ (new Double(Constants.GRID_COLOR.getBlue() / 255.0)).toString()
					+ " setrgbcolor\n";
				
				int gridSize = Math.max(1, (int)(com.cosylab.vdct.Constants.GRID_SIZE));
				int sx = gridSize - viewRx % gridSize;
				int y0 = gridSize - viewRy % gridSize;
				int xsteps = (int)(viewViewWidth / gridSize + 1);
				int ysteps = (int)(viewViewHeight / gridSize + 1);
	
				
				imageWidth = Math.min(viewViewWidth, gridSize * xsteps + sx);
				imageHeight = Math.min(viewViewHeight, gridSize * ysteps + y0);
	
				System.out.println(sx+" "+y0+" "+xsteps+" "+ysteps+" "+imageWidth+" "+imageHeight);
	
				if(gridSize >= 15)
					postScriptGrid += "/gridshape {posX posY moveto -1 -1 rmoveto "
						+ "2 2 rlineto 0 -2 rmoveto -2 2 rlineto stroke} def\n";
				else
					postScriptGrid += "/gridshape {newpath posX posY moveto "
						+ "-0.5 -0.5 rmoveto 1 0 rlineto 0 1 rlineto -1 0 rlineto "
						+ "0 -1 rlineto closepath fill} def\n";
	
				postScriptGrid += "/posX " + String.valueOf(sx) + " def\n"
					+ "/posY " + String.valueOf(y0) + " def\n"
					+ "{\n{\ngridshape\n"
							
					+ "/posX posX " + String.valueOf(gridSize) + " add def\n"
							
					+ "posX " + String.valueOf(imageWidth)
					+ " ge {exit} if\n"
							
					+ "} loop\n"
					+ "/posX " + String.valueOf(sx) + " def\n"
							
					+ "/posY posY " + String.valueOf(gridSize) + " add def\n"
							
					+ "posY " + String.valueOf(imageHeight)
					+ " ge {exit} if\n} loop\n";
			}
	
			stringOffset = stringBuffer.indexOf("%%Page: "+(pageIndex+1), stringOffset);		
			if (stringOffset == -1) continue;
			
			int selectionStart = stringBuffer.indexOf("concat", stringOffset) + 6;
	
			int selectionEnd = stringBuffer.indexOf("gsave", selectionStart);
			stringOffset = selectionEnd;
			stringBuffer.insert(stringOffset, "\n");
	
			String startingColour = stringBuffer.substring(selectionStart,
				selectionEnd);
			//stringBuffer.delete(selectionStart, selectionEnd);
						
			stringOffset = stringBuffer.indexOf("concat", stringOffset) + 6;
	
			if((startingColour.indexOf("setgray") == -1)
				&& (startingColour.indexOf("setrgbcolor") == -1))
			{
				startingColour = "0.0 setgray\n";	
			}
	
			stringBuffer.insert(stringOffset, postScriptGrid + startingColour);
		}
	
	return stringBuffer.toString().getBytes();
}

/**
 * Comment
 */
public void printAsPostScriptMenuItem_ActionPerformed()
{
	final GetPrintableInterface pi = (GetPrintableInterface)CommandManager.getInstance().getCommand("GetPrintableInterface");

	if(pi == null)
		return;
	
	class MyOwnPostScriptPrintingThread extends Thread {
		
		private ViewState view = null;
		
		public MyOwnPostScriptPrintingThread(ViewState view) {
			this.view = view;
		}
		
		public void run()
		{
			boolean showGrid = Settings.getInstance().getShowGrid();

			try
			{	
				PrintRequestAttributeSet printRequestAttributeSet = Page.getPrintRequestAttributeSet();

				PrinterJob printerJob = PrinterJob.getPrinterJob();
				printerJob.setJobName(getTitle());
				printerJob.setPageable(pi.getPageable());

				if(!printerJob.printDialog(printRequestAttributeSet))
					return;

				int pageFormatOrientation = PageFormat.PORTRAIT;
		

				OrientationRequested orientationRequested = (OrientationRequested)
					printRequestAttributeSet.get(OrientationRequested.class);
					
				if(orientationRequested != null)
				{
					if(orientationRequested.equals(OrientationRequested.LANDSCAPE))
						pageFormatOrientation = PageFormat.LANDSCAPE;
					else if(orientationRequested.equals(OrientationRequested.REVERSE_LANDSCAPE))
						pageFormatOrientation = PageFormat.REVERSE_LANDSCAPE;
				}
				
				pi.getPageable().getPageFormat(0).setOrientation(pageFormatOrientation);

				MediaPrintableArea mediaPrintableArea = (MediaPrintableArea)
					printRequestAttributeSet.get(MediaPrintableArea.class);
					
				Paper paper = pi.getPageable().getPageFormat(0).getPaper();

				paper.setImageableArea(mediaPrintableArea.getX(MediaPrintableArea.INCH) * 72.0,
					mediaPrintableArea.getY(MediaPrintableArea.INCH) * 72.0,
					mediaPrintableArea.getWidth(MediaPrintableArea.INCH) * 72,
					mediaPrintableArea.getHeight(MediaPrintableArea.INCH) * 72);
	
				pi.getPageable().getPageFormat(0).setPaper(paper);

				ByteArrayOutputStream byteArrayOutputStream = new ByteArrayOutputStream();
				
				DocFlavor pageableFlavor = DocFlavor.SERVICE_FORMATTED.PAGEABLE;
			    String postScriptMime = new String("application/postscript");

				StreamPrintServiceFactory[] streamPrintServiceFactory =
					StreamPrintServiceFactory.lookupStreamPrintServiceFactories(pageableFlavor,
					postScriptMime);

				if(streamPrintServiceFactory.length == 0)
				{
					System.err.println("Printing error: no suitable PostScript factory found.");
					return;
				}	

				StreamPrintService streamPrintService =
					streamPrintServiceFactory[0].getPrintService(byteArrayOutputStream);
				
				PrinterJob postScriptPrinterJob = PrinterJob.getPrinterJob();

				postScriptPrinterJob.setPrintService(streamPrintService);
				postScriptPrinterJob.setJobName(getTitle());
				postScriptPrinterJob.setPageable(pi.getPageable());

				setCursor(java.awt.Cursor.getPredefinedCursor(java.awt.Cursor.WAIT_CURSOR));
				Thread.yield();

				Settings.getInstance().setShowGrid(false);

				postScriptPrinterJob.print(printRequestAttributeSet);

				Settings.getInstance().setShowGrid(showGrid);

		        DocFlavor postScriptFlavor = DocFlavor.INPUT_STREAM.POSTSCRIPT;

			    if(!printerJob.getPrintService().isDocFlavorSupported(postScriptFlavor))
			    {
			    	JOptionPane.showMessageDialog(VisualDCT.this, "Printing error", "PostScript not supported on selected printer.", JOptionPane.ERROR_MESSAGE); 
					//System.err.println("Printing error: PostScript not supported on selected printer.");
					return;
			    }

//				print grid
				ByteArrayInputStream byteArrayInputStream = new ByteArrayInputStream(
							printGrid(pi.getPageable().getPageFormat(0),
									new StringBuffer(byteArrayOutputStream.toString()), view));

				byteArrayOutputStream = null;


				DocPrintJob docPrintJob = printerJob.getPrintService().createPrintJob();

   		 	  	DocAttributeSet docAttributeSet = new HashDocAttributeSet();
         	  	
       	 	    Doc doc = new SimpleDoc(byteArrayInputStream, postScriptFlavor,
       	 	    	docAttributeSet);

       	 	    docPrintJob.print(doc, printRequestAttributeSet);
			}
			catch (Exception exception)
			{
				exception.printStackTrace();
				com.cosylab.vdct.Console.getInstance().println("Printing error: "
					+ exception);
			}
			finally
			{
				setCursor(java.awt.Cursor.getPredefinedCursor(java.awt.Cursor.DEFAULT_CURSOR));
				Settings.getInstance().setShowGrid(showGrid);
				CommandManager.getInstance().execute("RepaintWorkspace");
			}
		}
	};

    // Do nothing if no drawing surface.
	Group root = Group.getRoot();
	if (root == null) {
		return;
	}
	
	// Copy the current view to prevent changes to it during thread run. 
	ViewState view = new ViewState(ViewState.getInstance(root.getDsId()));
	new MyOwnPostScriptPrintingThread(view).start();
}
/**
 * Comment
 */
public void printMenuItem_ActionPerformed() {

	final GetPrintableInterface pi = (GetPrintableInterface)CommandManager.getInstance().getCommand("GetPrintableInterface");
	if (pi!=null)
	new Thread() {
		public void run() {
			try {
				PrintRequestAttributeSet printRequestAttributeSet = Page.getPrintRequestAttributeSet();

//				HashPrintRequestAttributeSet printRequestAttributeSet = new HashPrintRequestAttributeSet();
				
				PrinterJob printerJob = PrinterJob.getPrinterJob();
				printerJob.setJobName(getTitle());
				printerJob.setPageable(pi.getPageable());
				
				if (lastPrintService != null) {
				    try {
	                    printerJob.setPrintService(lastPrintService);
	                } catch (PrinterException e) {
	                    handleException(e);
	                }
				}
				
				if(!printerJob.printDialog(printRequestAttributeSet)) {
				    lastPrintService = printerJob.getPrintService();
					return;
				}
				
				lastPrintService = printerJob.getPrintService();
				
				int pageFormatOrientation = PageFormat.PORTRAIT;

				OrientationRequested orientationRequested = (OrientationRequested)
					printRequestAttributeSet.get(OrientationRequested.class);
					
				if(orientationRequested != null)
				{
					if(orientationRequested.equals(OrientationRequested.LANDSCAPE))
						pageFormatOrientation = PageFormat.LANDSCAPE;
					else if(orientationRequested.equals(OrientationRequested.REVERSE_LANDSCAPE))
						pageFormatOrientation = PageFormat.REVERSE_LANDSCAPE;
				}
				
				pi.getPageable().getPageFormat(0).setOrientation(pageFormatOrientation);

				Paper paper = pi.getPageable().getPageFormat(0).getPaper();
				
				Media media = (Media)printRequestAttributeSet.get(Media.class);
				if (media instanceof MediaSizeName) {
		        	MediaSizeName msn = (MediaSizeName)media;
		        	MediaSize ms = MediaSize.getMediaSizeForName(msn);
		        	double width = ms.getX (MediaSize.INCH) * 72;
		    		double height = ms.getY (MediaSize.INCH) * 72;
		    		
		    		paper.setSize(width, height);
		        }
				
				MediaPrintableArea mediaPrintableArea = (MediaPrintableArea)
					printRequestAttributeSet.get(MediaPrintableArea.class);
					
				paper.setImageableArea(mediaPrintableArea.getX(MediaPrintableArea.INCH) * 72.0,
					mediaPrintableArea.getY(MediaPrintableArea.INCH) * 72.0,
					mediaPrintableArea.getWidth(MediaPrintableArea.INCH) * 72,
					mediaPrintableArea.getHeight(MediaPrintableArea.INCH) * 72);
	
				pi.getPageable().getPageFormat(0).setPaper(paper);

				
				
				// store settings 
				com.cosylab.vdct.graphics.printing.Page.setPageFormat(pi.getPageable().getPageFormat(0));
				Page.setPrintRequestAttributeSet(printRequestAttributeSet);
				setCursor(java.awt.Cursor.getPredefinedCursor(java.awt.Cursor.WAIT_CURSOR));
				
				Thread.yield();
				printerJob.print(printRequestAttributeSet);
								
			} catch (Exception ex) {
				ex.printStackTrace();
				com.cosylab.vdct.Console.getInstance().println("Printing error: "+ex);
			}
			finally
			{
				setCursor(java.awt.Cursor.getPredefinedCursor(java.awt.Cursor.DEFAULT_CURSOR));
			}
		}
	}.start();
}
/**
 * Comment
 * @param actionEvent
 */
public void printPreviewMenuItem_ActionPerformed(java.awt.event.ActionEvent actionEvent) {
	final GetPrintableInterface pi = (GetPrintableInterface)CommandManager.getInstance().getCommand("GetPrintableInterface");
	if (pi!=null)
		new Thread() {
			public void run() {

				PrintPreview pp = new PrintPreview(VisualDCT.this, pi.getPageable(), "VisualDCT Print Preview"+addedToTitle);
				pp.setLocationRelativeTo(VisualDCT.this);
				pp.setVisible(true);
				Thread.yield();
				pp.loadPreview();

			}
		}.start();
}
/**
 * Comment
 */
public void redoMenuItem_ActionPerformed() {
    GetGUIInterface cmd = (GetGUIInterface)CommandManager.getInstance().getCommand("GetGUIMenuInterface");
 	cmd.getGUIMenuInterface().redo();
}
/**
 * Comment
 * @param windowEvent
 */
public void renameDialog_WindowOpened(java.awt.event.WindowEvent windowEvent) {
	getNewNameTextField().setText(getOldNameLabel().getText());
	getRenameNameChecker().check();
}
/**
 * Comment
 * @param actionEvent
 */
public void renameOKButton_ActionPerformed(java.awt.event.ActionEvent actionEvent) {
	if (getRenameOKButton().isEnabled()) {
		VDBInterface vDBInterface =
			((GetVDBManager)CommandManager.getInstance().getCommand("GetVDBManager")).getManager();
		String errmsg = vDBInterface.checkRecordName(getNewNameTextField().getText(), getOldNameLabel().getText(), true);
		if (errmsg==null || !vDBInterface.isErrorMessage(errmsg)) {
			GetGUIInterface cmd = (GetGUIInterface)CommandManager.getInstance().getCommand("GetGUIMenuInterface");
		 	cmd.getGUIMenuInterface().rename(getOldNameLabel().getText(), getNewNameTextField().getText());
			getRenameDialog().dispose();
		}
		else {
			getRenameOKButton().setEnabled(false);
			getRenameWarningLabel().setText(errmsg);
		}
	}
}
/**
 * Comment
 */
public void save_As_GroupMenuItem_ActionPerformed() {
	JFileChooser chooser = getfileChooser();
	UniversalFileFilter filter = new UniversalFileFilter(
		new String[] {"db", "vdb"}, "EPICS DB files");
	chooser.resetChoosableFileFilters();
	chooser.addChoosableFileFilter(filter);
	chooser.setDialogTitle("Save Group as...");
	chooser.setFileSelectionMode(JFileChooser.FILES_ONLY);

	boolean jFileChooserConfirmed = false;
    java.io.File theFile = null;

	while(!jFileChooserConfirmed)
	{
		int retval = chooser.showSaveDialog(this);

		if(retval == JFileChooser.CANCEL_OPTION)
			return;

	    theFile = chooser.getSelectedFile();

		// fix ending
		if (chooser.getFileFilter().getDescription().startsWith("EPICS") &&
			!theFile.getName().endsWith(".db") &&
			!theFile.getName().endsWith(".vdb"))
			theFile = new java.io.File(theFile.getAbsoluteFile()+".vdb");

		if(theFile.exists())
		{
		    if(JOptionPane.showConfirmDialog(this, "The file '" + theFile.getName()
		    	+ "' already exists! Overwrite?", "Confirmation", JOptionPane.YES_NO_OPTION,
		    	JOptionPane.WARNING_MESSAGE) == JOptionPane.YES_OPTION)
		    {
				jFileChooserConfirmed = true;
		    }
		}
		else
			jFileChooserConfirmed = true;
	}
	    
    if(theFile != null)
    {
	    GetGUIInterface cmd = (GetGUIInterface)CommandManager.getInstance().getCommand("GetGUIMenuInterface");
		try
		{
		    cmd.getGUIMenuInterface().saveAsGroup(theFile);
		}
		catch(java.io.IOException e)
		{
			Console.getInstance().println("o) Failed to save DB to file: '"
				+ theFile.toString() + "'");
		    Console.getInstance().println(e);
		}
	}
}
/**
 * Comment
 */
public void generateAsGroupMenuItem_ActionPerformed() {
	JFileChooser chooser = getfileChooser();
	UniversalFileFilter filter = new UniversalFileFilter(
		new String("db"), "EPICS DB file");
	chooser.resetChoosableFileFilters();
	chooser.addChoosableFileFilter(filter);
	chooser.setDialogTitle("Generate Group as...");
	chooser.setFileSelectionMode(JFileChooser.FILES_ONLY);

	boolean jFileChooserConfirmed = false;
    java.io.File theFile = null;

	while(!jFileChooserConfirmed)
	{
		int retval = chooser.showSaveDialog(this);

		if(retval == JFileChooser.CANCEL_OPTION)
			return;

	    theFile = chooser.getSelectedFile();

		// fix ending
		if (chooser.getFileFilter().getDescription().startsWith("EPICS") &&
			!theFile.getName().endsWith(".db"))
			theFile = new java.io.File(theFile.getAbsoluteFile()+".db");

		if(theFile.exists())
		{
		    if(JOptionPane.showConfirmDialog(this, "The file '" + theFile.getName()
		    	+ "' already exists! Overwrite?", "Confirmation", JOptionPane.YES_NO_OPTION,
		    	JOptionPane.WARNING_MESSAGE) == JOptionPane.YES_OPTION)
		    {
				jFileChooserConfirmed = true;
		    }
		}
		else
			jFileChooserConfirmed = true;
	}
	    
    if(theFile != null)
    {
	    GetGUIInterface cmd = (GetGUIInterface)CommandManager.getInstance().getCommand("GetGUIMenuInterface");
		try
		{
		    cmd.getGUIMenuInterface().exportAsGroup(theFile);
		}
		catch(Exception e)
		{
			Console.getInstance().println("o) Failed to save DB to file: '"
				+ theFile.toString() + "'");
		    Console.getInstance().println(e);
		}
	}
}
/**
 * Comment
 * @return 
 */
public boolean save_AsMenuItem_ActionPerformed() {
	JFileChooser chooser = getfileChooser();
	UniversalFileFilter filter = new UniversalFileFilter(
		new String[] {"db", "vdb"}, "EPICS DB files");
	chooser.resetChoosableFileFilters();
	chooser.addChoosableFileFilter(filter);
	chooser.setDialogTitle("Save as...");
	chooser.setFileSelectionMode(JFileChooser.FILES_ONLY);

	boolean jFileChooserConfirmed = false;
    java.io.File theFile = null;

	while(!jFileChooserConfirmed)
	{
		int retval = chooser.showSaveDialog(this);

		if(retval == JFileChooser.CANCEL_OPTION)
			return false;
			
	    theFile = chooser.getSelectedFile();

		// fix ending
		if (chooser.getFileFilter().getDescription().startsWith("EPICS") &&
			!theFile.getName().endsWith(".db") &&
			!theFile.getName().endsWith(".vdb"))
			theFile = new java.io.File(theFile.getAbsoluteFile()+".vdb");

		if(theFile.exists())
		{
		    if(JOptionPane.showConfirmDialog(this, "The file '" + theFile.getName()
		    	+ "' already exists! Overwrite?", "Confirmation", JOptionPane.YES_NO_OPTION,
		    	JOptionPane.WARNING_MESSAGE) == JOptionPane.YES_OPTION)
		    {
				jFileChooserConfirmed = true;
		    }
		}
		else
			jFileChooserConfirmed = true;
	}

    if(theFile != null)
    {

	    GetGUIInterface cmd = (GetGUIInterface)CommandManager.getInstance().getCommand("GetGUIMenuInterface");
		try
		{
		    cmd.getGUIMenuInterface().save(theFile);
		    openedFile = theFile;
		}
		catch (java.io.IOException e)
		{
	 		Console.getInstance().println("o) Failed to save DB to file: '"
	 			+ theFile.toString() + "'");
		    Console.getInstance().println(e);
		    return false;
		}
	}
    return true;
}
/**
 * Comment
 */
public void saveAsTemplateMenuItem_ActionPerformed() {
	JFileChooser chooser = getfileChooser();
	UniversalFileFilter filter = new UniversalFileFilter(
		new String[] {"db", "vdb"}, "EPICS DB files");
	chooser.resetChoosableFileFilters();
	chooser.addChoosableFileFilter(filter);
	chooser.setDialogTitle("Save as Template");
	chooser.setFileSelectionMode(JFileChooser.FILES_ONLY);

	boolean jFileChooserConfirmed = false;
    java.io.File theFile = null;

	while(!jFileChooserConfirmed)
	{
		int retval = chooser.showSaveDialog(this);

		if(retval == JFileChooser.CANCEL_OPTION)
			return;
			
	    theFile = chooser.getSelectedFile();
	    
		// fix ending
		if (chooser.getFileFilter().getDescription().startsWith("EPICS") &&
			!theFile.getName().endsWith(".db") &&
			!theFile.getName().endsWith(".vdb"))
			theFile = new java.io.File(theFile.getAbsoluteFile()+".vdb");

		if(theFile.exists())
		{
		    if(JOptionPane.showConfirmDialog(this, "The file '" + theFile.getName()
		    	+ "' already exists! Overwrite?", "Confirmation", JOptionPane.YES_NO_OPTION,
		    	JOptionPane.WARNING_MESSAGE) == JOptionPane.YES_OPTION)
		    {
				jFileChooserConfirmed = true;
		    }
		}
		else
			jFileChooserConfirmed = true;
	}

    if(theFile != null)
    {
	    GetGUIInterface cmd = (GetGUIInterface)CommandManager.getInstance().getCommand("GetGUIMenuInterface");
		try
		{
		    cmd.getGUIMenuInterface().saveAsTemplate(theFile);
		    openedFile = theFile;
		}
		catch (java.io.IOException e)
		{
	 		Console.getInstance().println("o) Failed to save DB to file: '"
	 			+ theFile.toString() + "'");
		    Console.getInstance().println(e);
		}
	}
}
/**
 * Comment
 */
public void generateMenuItem_ActionPerformed() {
	JFileChooser chooser = getfileChooser();
	UniversalFileFilter filter = new UniversalFileFilter(
		new String("db"), "EPICS DB file");
	chooser.resetChoosableFileFilters();
	chooser.addChoosableFileFilter(filter);
	chooser.setDialogTitle("Generate");
	chooser.setFileSelectionMode(JFileChooser.FILES_ONLY);

	boolean jFileChooserConfirmed = false;
    java.io.File theFile = null;

	while(!jFileChooserConfirmed)
	{
		int retval = chooser.showSaveDialog(this);

		if(retval == JFileChooser.CANCEL_OPTION)
			return;
			
	    theFile = chooser.getSelectedFile();

		// fix ending
		if (chooser.getFileFilter().getDescription().startsWith("EPICS") &&
			!theFile.getName().endsWith(".db"))
			theFile = new java.io.File(theFile.getAbsoluteFile()+".db");

		if(theFile.exists())
		{
		    if(JOptionPane.showConfirmDialog(this, "The file '" + theFile.getName()
		    	+ "' already exists! Overwrite?", "Confirmation", JOptionPane.YES_NO_OPTION,
		    	JOptionPane.WARNING_MESSAGE) == JOptionPane.YES_OPTION)
		    {
				jFileChooserConfirmed = true;
		    }
		}
		else
			jFileChooserConfirmed = true;
	}

    if(theFile != null)
    {
	    GetGUIInterface cmd = (GetGUIInterface)CommandManager.getInstance().getCommand("GetGUIMenuInterface");
		try
		{
		    cmd.getGUIMenuInterface().export(theFile);
		}
		catch (Exception e)
		{
	 		Console.getInstance().println("o) Failed to save DB to file: '"
	 			+ theFile.toString() + "'");
		    Console.getInstance().println(e);
		} 
	}
}
/**
 * Comment
 * @return 
 */
public boolean saveMenuItem_ActionPerformed() {

	// if no file is open -> saveAs
	if (openedFile==null)
	{
		return save_AsMenuItem_ActionPerformed();
	}
		
	GetGUIInterface cmd = (GetGUIInterface)CommandManager.getInstance().getCommand("GetGUIMenuInterface");
	try {
		cmd.getGUIMenuInterface().save(openedFile);
	} catch (java.io.IOException e) {
 		Console.getInstance().println("o) Failed to save DB...");
	    Console.getInstance().println(e);
	    return false;
	}
	return true;
}
/**
 * Comment
 */
public void select_AllMenuItem_ActionPerformed() {
    GetGUIInterface cmd = (GetGUIInterface)CommandManager.getInstance().getCommand("GetGUIMenuInterface");
    cmd.getGUIMenuInterface().selectAll();
}

public void setBoxButtonEnabled(boolean parBoxButtonEnabled)
{
	if(parBoxButtonEnabled != boxButtonEnabled)
	{
		boxButtonEnabled = parBoxButtonEnabled;

		if(boxButtonEnabled)
		{
			validateGraphicsButtons(boxButton);
			boxButton.setIcon(new javax.swing.ImageIcon(getClass().getResource("/images/boxs.gif")));
		}
		else
			boxButton.setIcon(new javax.swing.ImageIcon(getClass().getResource("/images/boxn.gif")));
	}
}
/**
 * Insert the method's description here.
 * Creation date: (29.4.2001 11:17:11)
 * @param groupName java.lang.String
 */
public void setCurrentGroup(String groupName) {
	getStatusMsg2().setText(" "+groupName);
}
/**
 * Insert the method's description here.
 * Creation date: (29.4.2001 11:17:11)
 * @param mode
 */
public void setMode(String mode) {
	getModeStatus().setText(" "+mode+" ");
}
/**
 * Insert the method's description here.
 * Creation date: (29.1.2001 15:35:49)
 * @param dir java.lang.String
 */
public void setDefaultDirectory(String dir) {
	java.io.File directory = new java.io.File(dir);
	if (directory.exists() && directory.isDirectory()) {
		Settings.setDefaultDir(directory.toString());
		getfileChooser().setCurrentDirectory(directory);
		getComboBoxFileChooser().setCurrentDirectory(directory);
	}
}
/**
 * Insert the method's description here.
 * Creation date: (29.4.2001 11:02:03)
 * @param fileName java.lang.String
 */
public void setFileInTitle(String fileName) {
	if (fileName == null) {
		fileName = Constants.UNTITLED;
	}
	addedToTitle = " - [" + fileName + "]";
	setTitle("VisualDCT " + addedToTitle);
}
	

public void setLineButtonEnabled(boolean parLineButtonEnabled)
{
	if(parLineButtonEnabled != lineButtonEnabled)
	{
		lineButtonEnabled = parLineButtonEnabled;

		if(lineButtonEnabled)
		{
			validateGraphicsButtons(lineButton);
			lineButton.setIcon(new javax.swing.ImageIcon(getClass().getResource("/images/lines.gif")));
		}
		else
			lineButton.setIcon(new javax.swing.ImageIcon(getClass().getResource("/images/linen.gif")));
	}
}
/**
 * Insert the method's description here.
 * Creation date: (29.12.2000 12:43:58)
 * @param scale double
 */
public void setScale(double scale) {
	getZoomSlider().setValue((int)(scale*100+0.5));
	zoomSlider_updateLabel();
}

public void setTextBoxButtonEnabled(boolean parTextBoxButtonEnabled)
{
	if(parTextBoxButtonEnabled != textBoxButtonEnabled)
	{
		textBoxButtonEnabled = parTextBoxButtonEnabled;

		if(textBoxButtonEnabled)
		{
			validateGraphicsButtons(textBoxButton);
			textBoxButton.setIcon(new javax.swing.ImageIcon(getClass().getResource("/images/textboxs.gif")));
		}
		else
			textBoxButton.setIcon(new javax.swing.ImageIcon(getClass().getResource("/images/textboxn.gif")));
	}
}
/**
 * Comment
 * @param itemEvent
 */
public void show_PointsMenuItem_ItemStateChanged(java.awt.event.ItemEvent itemEvent) {
	GetGUIInterface cmd = (GetGUIInterface)CommandManager.getInstance().getCommand("GetGUIMenuInterface");
	if (cmd!=null)
		cmd.getGUIMenuInterface().showGrid(getShow_PointsMenuItem().isSelected());
	Settings.getInstance().setShowGrid(getShow_PointsMenuItem().isSelected());
}
public void showAboutBox() {
	/* Create the AboutBox dialog */
	new VisualDCTAboutDialogEngine(this).triggerReceiver();
}
/**
 * Insert the method's description here.
 * Creation date: (2.5.2001 10:30:28)
 */
public void showGroupDialog() {
	getGroupDialog().setLocationRelativeTo(this);
	getGroupDialog().setVisible(true);
}
/**
 * Insert the method's description here.
 * Creation date: (3.2.2001 19:39:20)
 */
public void showNewDialog() {
	getNewRecordDialog().setLocationRelativeTo(this);
	getNewRecordDialog().setVisible(true);
}

public void showMorphingDialog(String name, String oldType, Object[] targets) {
	morphingDialog_WindowOpened(null);
	
	getTypeComboBox2().removeAllItems();
	for (int i = 0; i < targets.length; i++)
		getTypeComboBox2().addItem(targets[i]);
	
	getMorphingOKButton().setEnabled(getTypeComboBox2().getItemCount()>0);
	getNameTextLabel().setText(name);
	getTypeComboBox2().setSelectedItem(oldType);
	getMorphingDialog().setLocationRelativeTo(this);
	getMorphingDialog().setVisible(true);
}
/**
 * Insert the method's description here.
 * Creation date: (2.5.2001 10:30:28)
 * @param oldName
 */
public void showRenameDialog(String oldName) {
	getOldNameLabel().setText(oldName);
	getRenameDialog().setLocationRelativeTo(this);
	getRenameDialog().setVisible(true);
}
/**
 * Comment
 */
public void smart_ZoomMenuItem_ActionPerformed() {
	GetGUIInterface cmd = (GetGUIInterface)CommandManager.getInstance().getCommand("GetGUIMenuInterface");
 	cmd.getGUIMenuInterface().smartZoom();
}
/**
 * Comment
 * @param itemEvent
 */
public void snapToGridMenuItem_ItemStateChanged(java.awt.event.ItemEvent itemEvent) {
	Settings.getInstance().setSnapToGrid(getSnapToGridMenuItem().isSelected());
	GetGUIInterface cmd = (GetGUIInterface)CommandManager.getInstance().getCommand("GetGUIMenuInterface");
	if (cmd!=null)
		cmd.getGUIMenuInterface().snapToGrid(getSnapToGridMenuItem().isSelected());
}
/**
 * Comment
 */
public void statusbarMenuItem_ActionPerformed() {
	Settings.getInstance().setStatusbar(getStatusbarMenuItem().isSelected());
}

public void textBoxButton_ActionPerformed()
{
	if(textBoxButtonEnabled)
		setTextBoxButtonEnabled(false);
	else
		setTextBoxButtonEnabled(true);
}
/**
 * Comment
 */
public void toolbarMenuItem_ActionPerformed() {
	Settings.getInstance().setToolbar(getToolbarMenuItem().isSelected());
}
/**
 * Comment
 */
public void undoMenuItem_ActionPerformed() {
    GetGUIInterface cmd = (GetGUIInterface)CommandManager.getInstance().getCommand("GetGUIMenuInterface");
 	cmd.getGUIMenuInterface().undo();
}
/**
 * Comment
 */
public void ungroupMenuItem_ActionPerformed() {
    GetGUIInterface cmd = (GetGUIInterface)CommandManager.getInstance().getCommand("GetGUIMenuInterface");
 	cmd.getGUIMenuInterface().ungroup();
}

private void validateGraphicsButtons(JButton clickedButton)
{
	if((boxButton != clickedButton) && (boxButtonEnabled))
		setBoxButtonEnabled(false);
	if((lineButton != clickedButton) && (lineButtonEnabled))
		setLineButtonEnabled(false);
	if((textBoxButton != clickedButton) && (textBoxButtonEnabled))
		setTextBoxButtonEnabled(false);
}

public void viewStatusBar() {
	/* Hide or show the statusbar */
	getStatusBarPane().setVisible(getStatusbarMenuItem().isSelected());
}
public void viewToolBar() {
	/* Hide or show the toolbar */
	getToolBarPane().setVisible(getToolbarMenuItem().isSelected());
}
/**
 * Comment
 */
public void zoom_InMenuItem_ActionPerformed() {
	getZoomSlider().setValue(getZoomSlider().getValue()+getZoomSlider().getMinorTickSpacing());
	zoomSlider_updateLabel();
}
/**
 * Comment
 */
public void zoom_OutMenuItem_ActionPerformed() {
	getZoomSlider().setValue(getZoomSlider().getValue()-getZoomSlider().getMinorTickSpacing());
	zoomSlider_updateLabel();
}
/**
 * Comment
 * @param mouseEvent
 */
public void zoomSlider_MouseReleased(java.awt.event.MouseEvent mouseEvent) {
	zoomSlider_updateLabel();
}
/**
 * Comment
 * @param propertyChangeEvent
 */
public void zoomSlider_PropertyChange(java.beans.PropertyChangeEvent propertyChangeEvent) {
	zoomSlider_updateLabel();
}
/**
 * Comment
 */
public void zoomSlider_updateLabel() {
	int value = (int)((double)getZoomSlider().getValue()/getZoomSlider().getMinorTickSpacing()+0.5) * getZoomSlider().getMinorTickSpacing();
	getZoomLabel().setText(value+"%");

    GetGUIInterface cmd = (GetGUIInterface)CommandManager.getInstance().getCommand("GetGUIMenuInterface");
    cmd.getGUIMenuInterface().setScale(value/100.0);
}
/**
 * Comment
 * @param direction
 */
public void moveOrigin(int direction) {
    GetGUIInterface cmd = (GetGUIInterface)CommandManager.getInstance().getCommand("GetGUIMenuInterface");
    cmd.getGUIMenuInterface().moveOrigin(direction);
}

/**
 * Comment
 */
public void updateLoadLabel() {

	StringBuffer tip = new StringBuffer();
	tip.append("<html>");

	// DBD part
	java.util.Vector dbds = DataProvider.getInstance().getLoadedDBDs();
	if (dbds.size()==0)
		tip.append("<p>No loaded DBDs.</p>");
	else
	{	
		tip.append("Loaded DBD(s):<b><ul>");
		java.util.Enumeration e = dbds.elements();
		while (e.hasMoreElements())
		{
				tip.append("<li>");
				tip.append(e.nextElement().toString());
				tip.append("&nbsp;</li>");
		}
		tip.append("</b></ul>");
	}
	
	Hashtable templates = null;
	
	DsManagerInterface dsManager = ((GetDsManager)CommandManager.getInstance().getCommand("GetDsManager")).getManager();
	DrawingSurfaceInterface surface = dsManager.getFocusedDrawingSurface();
	if (surface != null) {
		templates = VDBData.getInstance(surface.getDsId()).getTemplates();
	}
	
	if (templates == null || templates.size()==0)
		tip.append("<p>No loaded templates.</p>");
	else
	{	
		tip.append("Loaded template(s):<b><ul>");
		Enumeration e = templates.keys();
		while (e.hasMoreElements())
		{
				tip.append("<li>");
				String key = e.nextElement().toString();
				VDBTemplate t = (VDBTemplate)templates.get(key);
				
				tip.append(t.getDescription());
				tip.append(" - ");
				tip.append(t.getFileName());
				tip.append("&nbsp;</li>");
		}
		tip.append("</b></ul>");
	}
	tip.append("</html>");

	StringBuffer label = new StringBuffer();
	label.append(" ");
	
	if (dbds.size()==0)
		label.append("No loaded DBDs");
	else if (dbds.size()==1)
		label.append(dbds.firstElement().toString());
	else
	{
		label.append(dbds.size());
		label.append(" loaded DBD(s)");
	}
	
	label.append(", ");

	if (templates == null || templates.size()==0)
		label.append("no loaded templates");
	else
	{
		label.append(templates.size());
		label.append(" loaded templates(s)");
	}

	label.append(" ");
	
	getStatusMsg1().setToolTipText(tip.toString());
	getStatusMsg1().setText(label.toString());
}
	/**
	 * Returns the openedFile.
	 * @return java.io.File
	 */
	public java.io.File getOpenedFile()
	{
		return openedFile;
	}

	/**
	 * Sets the openedFile.
	 * @param openedFile The openedFile to set
     * @param title
	 */
	public void setOpenedFile(File openedFile, String title)
	{
		this.openedFile = openedFile;
		setFileInTitle(title);
	}

	/**
	 * Returns the ivjRecentFilesMenuItem.
	 * @return RecentFilesMenu
	 */
	public RecentFilesMenu getRecentFilesMenu()
	{
		return ivjRecentFilesMenuItem;
	}

	/**
	 * @return
	 */
	public ComboBoxFileChooser getComboBoxFileChooser() {
		if (comboBoxFileChooser == null) 
			comboBoxFileChooser = new ComboBoxFileChooser();
		return comboBoxFileChooser;
	}
	
	public void setDefaultFocus() {
    	getJFrameContentPane().requestFocusInWindow();
	}

	public void onDsAdded(Object id) {
		DsManagerInterface dsManager = ((GetDsManager)CommandManager.getInstance().getCommand("GetDsManager")).getManager();
		setDsCommandsState(dsManager.getDrawingSurfaces().length > 0);
		updateLoadLabel();
	}
	
	public void onDsFocused(Object id) {
		updateLoadLabel();
	}
	public void onDsRemoved(Object id) {
		DsManagerInterface dsManager = ((GetDsManager)CommandManager.getInstance().getCommand("GetDsManager")).getManager();
		setDsCommandsState(dsManager.getDrawingSurfaces().length > 0);
		updateLoadLabel();
	}

	private void setDsCommandsState(boolean active) {
		if (dsCommandsActive != active) {
			dsCommandsActive = active;

			getImport_DBMenuItem().setEnabled(active);
			getImportFieldsMenuItem().setEnabled(active);
			getImportTemplate_DBMenuItem().setEnabled(active);
			getImportBorderMenuItem().setEnabled(active);
			getImport_DBDMenuItem().setEnabled(active);
			getSaveMenuItem().setEnabled(active);
			getSave_AsMenuItem().setEnabled(active);
			getSave_As_GroupMenuItem().setEnabled(active);
			getCloseMenuItem().setEnabled(active);
			getCloseAllMenuItem().setEnabled(active);
			getGenerateMenuItem().setEnabled(active);
			getGenerateAsGroupMenuItem().setEnabled(active);
			getExportMenuItem().setEnabled(active);
			getExportPostScriptFileMenuItem().setEnabled(active);
			getPrintMenuItem().setEnabled(active);
			getPrintAsPostScriptMenuItem().setEnabled(active);
			getPrintPreviewMenuItem().setEnabled(active);
			
			getUndoMenuItem().setEnabled(active);
			getRedoMenuItem().setEnabled(active);
			getCutMenuItem().setEnabled(active);
			getCopyMenuItem().setEnabled(active);
			getPasteMenuItem().setEnabled(active);
			getMove_RenameMenuItem().setEnabled(active);
			getMorphMenuItem().setEnabled(active);
			getGroupMenuItem().setEnabled(active);
			getUngroupMenuItem().setEnabled(active);
			getDeleteMenuItem().setEnabled(active);
			getSelect_AllMenuItem().setEnabled(active);
			getFindMenuItem().setEnabled(active);
			getSystemCopyMenuItem().setEnabled(active);
			getSystemPasteMenuItem().setEnabled(active);
			
			getFlat_ViewMenuItem().setEnabled(active);
			getLevel_UpMenuItem().setEnabled(active);
			getZoom_InMenuItem().setEnabled(active);
			getZoom_OutMenuItem().setEnabled(active);
			getSmart_ZoomMenuItem().setEnabled(active);
			getBase_ViewMenuItem().setEnabled(active);
			getSpreadsheetMenuItem().setEnabled(active);
			getLeftMenuItem().setEnabled(active);
			getRightMenuItem().setEnabled(active);
			getUpMenuItem().setEnabled(active);
			getDownMenuItem().setEnabled(active);
			
			getSaveButton().setEnabled(active);
			getFindButton().setEnabled(active);
			getUndoButton().setEnabled(active);
			getRedoButton().setEnabled(active);
			getCutButton().setEnabled(active);
			getCopyButton().setEnabled(active);
			getPasteButton().setEnabled(active);
			getLineButton().setEnabled(active);
			getBoxButton().setEnabled(active);
			getTextBoxButton().setEnabled(active);
			
			getStatusMsg1().setEnabled(active);
			getStatusMsg2().setEnabled(active);
			getModeStatus().setEnabled(active);
			getZoomSlider().setEnabled(active);
			getZoomLabel().setEnabled(active);
		}
	}
}
