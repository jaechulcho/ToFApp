# -*- coding: utf-8 -*- 

'''
Created on 2017. 03. 21.

@author: jccho@fine-tec.com
'''

import wx
import tofmongui as ui

class MyFrame(ui.TOFMONFrame):
    def __init__(self):
        ''' main frame constructor'''
        super(MyFrame, self).__init__(None)
        return

class MyApp(wx.App):
    def OnInit(self):
        frame = MyFrame()
        frame.Show()
        return True

if __name__ == '__main__':
    app = MyApp()
    app.MainLoop()