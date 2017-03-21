# -*- coding: utf-8 -*- 

###########################################################################
## Python code generated with wxFormBuilder (version Mar 21 2017)
## http://www.wxformbuilder.org/
##
## PLEASE DO "NOT" EDIT THIS FILE!
###########################################################################

import wx
import wx.xrc

###########################################################################
## Class TOFMONFrame
###########################################################################

class TOFMONFrame ( wx.Frame ):
	
	def __init__( self, parent ):
		wx.Frame.__init__ ( self, parent, id = wx.ID_ANY, title = u"ToFApp Monitor", pos = wx.DefaultPosition, size = wx.Size( 800,600 ), style = wx.DEFAULT_FRAME_STYLE|wx.TAB_TRAVERSAL )
		
		self.SetSizeHintsSz( wx.DefaultSize, wx.DefaultSize )
		
		bSizer1 = wx.BoxSizer( wx.VERTICAL )
		
		self.m_panel1 = wx.Panel( self, wx.ID_ANY, wx.DefaultPosition, wx.DefaultSize, wx.TAB_TRAVERSAL )
		sbSizerUBG = wx.StaticBoxSizer( wx.StaticBox( self.m_panel1, wx.ID_ANY, u"UBG" ), wx.HORIZONTAL )
		
		bSizer2 = wx.BoxSizer( wx.VERTICAL )
		
		gSizer1 = wx.GridSizer( 4, 2, 0, 0 )
		
		self.m_textCtrlIP = wx.TextCtrl( sbSizerUBG.GetStaticBox(), wx.ID_ANY, wx.EmptyString, wx.DefaultPosition, wx.DefaultSize, 0 )
		gSizer1.Add( self.m_textCtrlIP, 0, wx.ALL|wx.EXPAND, 5 )
		
		self.m_buttoncommnect = wx.Button( sbSizerUBG.GetStaticBox(), wx.ID_ANY, u"connect", wx.DefaultPosition, wx.DefaultSize, 0 )
		gSizer1.Add( self.m_buttoncommnect, 0, wx.ALL, 5 )
		
		self.m_buttonarea1 = wx.Button( sbSizerUBG.GetStaticBox(), wx.ID_ANY, u"직진", wx.DefaultPosition, wx.DefaultSize, 0 )
		gSizer1.Add( self.m_buttonarea1, 0, wx.ALL, 5 )
		
		self.m_buttonarea2 = wx.Button( sbSizerUBG.GetStaticBox(), wx.ID_ANY, u"좌회전", wx.DefaultPosition, wx.DefaultSize, 0 )
		gSizer1.Add( self.m_buttonarea2, 0, wx.ALL, 5 )
		
		self.m_buttonarea3 = wx.Button( sbSizerUBG.GetStaticBox(), wx.ID_ANY, u"우회전", wx.DefaultPosition, wx.DefaultSize, 0 )
		gSizer1.Add( self.m_buttonarea3, 0, wx.ALL, 5 )
		
		self.m_buttonarea4 = wx.Button( sbSizerUBG.GetStaticBox(), wx.ID_ANY, u"좁은 베이", wx.DefaultPosition, wx.DefaultSize, 0 )
		gSizer1.Add( self.m_buttonarea4, 0, wx.ALL, 5 )
		
		self.m_buttonarea5 = wx.Button( sbSizerUBG.GetStaticBox(), wx.ID_ANY, u"넓은 베이", wx.DefaultPosition, wx.DefaultSize, 0 )
		gSizer1.Add( self.m_buttonarea5, 0, wx.ALL, 5 )
		
		self.m_buttonarea6 = wx.Button( sbSizerUBG.GetStaticBox(), wx.ID_ANY, u"에스자 구간", wx.DefaultPosition, wx.DefaultSize, 0 )
		gSizer1.Add( self.m_buttonarea6, 0, wx.ALL, 5 )
		
		
		bSizer2.Add( gSizer1, 1, wx.EXPAND, 5 )
		
		
		sbSizerUBG.Add( bSizer2, 1, wx.EXPAND, 5 )
		
		bSizer3 = wx.BoxSizer( wx.VERTICAL )
		
		self.m_panelUBGArea = wx.Panel( sbSizerUBG.GetStaticBox(), wx.ID_ANY, wx.DefaultPosition, wx.DefaultSize, wx.TAB_TRAVERSAL )
		bSizer3.Add( self.m_panelUBGArea, 1, wx.EXPAND |wx.ALL, 5 )
		
		
		sbSizerUBG.Add( bSizer3, 1, wx.EXPAND, 5 )
		
		
		self.m_panel1.SetSizer( sbSizerUBG )
		self.m_panel1.Layout()
		sbSizerUBG.Fit( self.m_panel1 )
		bSizer1.Add( self.m_panel1, 1, wx.EXPAND |wx.ALL, 5 )
		
		self.m_panel2 = wx.Panel( self, wx.ID_ANY, wx.DefaultPosition, wx.DefaultSize, wx.TAB_TRAVERSAL )
		sbSizer2 = wx.StaticBoxSizer( wx.StaticBox( self.m_panel2, wx.ID_ANY, u"PBS" ), wx.VERTICAL )
		
		
		self.m_panel2.SetSizer( sbSizer2 )
		self.m_panel2.Layout()
		sbSizer2.Fit( self.m_panel2 )
		bSizer1.Add( self.m_panel2, 1, wx.EXPAND |wx.ALL, 5 )
		
		
		self.SetSizer( bSizer1 )
		self.Layout()
		
		self.Centre( wx.BOTH )
	
	def __del__( self ):
		pass
	

