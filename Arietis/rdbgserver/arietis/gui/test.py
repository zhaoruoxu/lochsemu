'''
Created on 2012-12-21

@author: Administrator
'''
import wx
import os
from common import VERSION

class ArietisDebugger(wx.Frame):
    
    def __init__(self, parent, title):
        wx.Frame.__init__(self, parent, title=title, size=(600, 600))

        self.InitUI()
        self.dirname = ''
        
    def InitUI(self):
        
        self.control = wx.TextCtrl(self, style=wx.TE_MULTILINE)
        self.control.SetFont(wx.Font(9, wx.FONTFAMILY_DEFAULT, wx.NORMAL, wx.NORMAL, False, 'Consolas'))
        
        
        self.InitMenuBar()
        
        self.CreateStatusBar()
        
        self.InitSizers()
        
        self.Show(True)
        
    def InitSizers(self):
        self.sizer2 = wx.BoxSizer(wx.HORIZONTAL)
        self.buttons = []
        for i in range(0, 6):
            self.buttons.append(wx.Button(self, -1, "Button &" + str(i)))
            self.sizer2.Add(self.buttons[i], 1, wx.EXPAND)
        
        self.sizer = wx.BoxSizer(wx.VERTICAL)
        self.sizer.Add(self.sizer2, 0, wx.ALIGN_CENTER_HORIZONTAL)
        self.sizer.Add(self.control, 1, wx.GROW)
        
        self.SetSizer(self.sizer)
        self.SetAutoLayout(1)
        self.sizer.Fit(self)
        
        
    def InitMenuBar(self):
        menubar= wx.MenuBar()
        
        fileMenu = wx.Menu()
        itemExit = fileMenu.Append(wx.ID_EXIT, 'Exit', 'Exit Arietis')
        fileMenu.AppendSeparator()
        itemAbout = fileMenu.Append(wx.ID_ABORT, '&About', 'info')
        itemOpen = fileMenu.Append(wx.ID_OPEN, '&Open', 'open a file')
        
        menubar.Append(fileMenu, '&File')
        self.SetMenuBar(menubar)
        
        self.Bind(wx.EVT_MENU, self.OnExit, itemExit)
        self.Bind(wx.EVT_MENU, self.OnAbout, itemAbout)
        self.Bind(wx.EVT_MENU, self.OnOpen, itemOpen)
        
    def OnExit(self, e):
        self.Close()
        
    def OnAbout(self, e):
        dlg = wx.MessageDialog(self, 'Show me the code', 'Arietis', wx.OK)
        dlg.ShowModal()
        dlg.Destroy()
        
    def OnOpen(self, e):
        dlg = wx.FileDialog(self, 'choose a file', self.dirname, '', '*.*', wx.OPEN)
        if dlg.ShowModal() == wx.ID_OK:
            self.filename = dlg.GetFilename()
            self.dirname = dlg.GetDirectory()
            f = open(os.path.join(self.dirname, self.filename), 'r')
            self.control.SetValue(f.read())
            f.close()
        dlg.Destroy()

def testGUI():
    print('what?')
    app = wx.App(False)
    ArietisDebugger(None, 'Arietis version %x' % (VERSION))
    
    app.MainLoop()