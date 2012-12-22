'''
Created on 2012-12-22

@author: Administrator
'''


import wx
import sys
from common import VERSION
from gui.redirect import Redirect


class TaintDbgFrame(wx.Frame):
    def __init__(self, parent=None):
        wx.Frame.__init__(self, parent, title='Arietis TaintDebugger, ver %x' % VERSION,
                          size=(600, 800))
        self.InitUI()

    def InitUI(self):
        self.InitMenu()
        # self.CreateToolBar()
        self.CreateStatusBar()

        self.InitMainPanel()
        sys.stdout = Redirect(self.logger, wx.TextAttr(wx.Color(0, 0, 255)))
        sys.stderr = Redirect(self.logger, wx.TextAttr(wx.Color(255, 0, 0)))
        print('hahahaha')
        sys.stderr.write('holy crap\n')
        print('hahaha')
        
    def InitMainPanel(self):
        panel = wx.Panel(self)
        vbox = wx.BoxSizer(wx.VERTICAL)
        
        ''' information bar '''
        hbox1 = wx.BoxSizer(wx.HORIZONTAL)
        static1 = wx.StaticText(panel, -1, 'Just a test')
        #static1.SetBackgroundColour('#ff0000')
        hbox1.Add(static1, 1, wx.EXPAND, 0)
        vbox.Add(hbox1, 0, wx.LEFT | wx.RIGHT | wx.TOP, 5)
        vbox.Add((-1, 5))
        
        ''' list and property window '''
        hbox2 = wx.BoxSizer(wx.HORIZONTAL)
        self.list = wx.ListCtrl(panel, -1, style=wx.LC_REPORT)
        self.list.InsertColumn(0, 'haha', width=100)
        self.list.InsertColumn(0, 'hehe', width=200)
        hbox2.Add(self.list, 1, wx.EXPAND, 0)
        panelx = wx.Panel(panel)
        # panelx.SetBackgroundColour('#8080ff')
        panelx.SetSize((150, -1))
        hbox2.Add(panelx, 0, wx.EXPAND, 0)
        vbox.Add(hbox2, 1, wx.LEFT | wx.RIGHT | wx.EXPAND, 5)
        vbox.Add((-1, 5))
        
        ''' logger '''
        hbox3 = wx.BoxSizer(wx.HORIZONTAL)
        self.logger = wx.TextCtrl(panel, -1, style=wx.TE_MULTILINE|wx.TE_READONLY|wx.TE_RICH2)
        # self.logger.SetSize((-1, 200))
        hbox3.Add(self.logger, 1, wx.EXPAND, 0)
        hbox3.SetMinSize((-1, 150))
        vbox.Add(hbox3, 0, wx.LEFT | wx.RIGHT | wx.EXPAND, 5)
        vbox.Add((-1, 5))
        
        panel.SetSizerAndFit(vbox)
        

    def InitMenu(self):
        menubar = wx.MenuBar()
        fileMenu = wx.Menu()
        itemExit = fileMenu.Append(wx.ID_EXIT, 'E&xit', 'Exit TaintDebugger')
        menubar.Append(fileMenu, '&File')
        
        self.Bind(wx.EVT_MENU, self.OnExit, itemExit)
        
        self.SetMenuBar(menubar)
        
    def OnExit(self, e):
        self.Close()
    

def StartTaintDebugger():
    app = wx.App(False)
    window = TaintDbgFrame()
    
    window.Show()
    app.MainLoop()
    