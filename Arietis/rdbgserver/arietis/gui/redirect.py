'''
Created on 2012-12-22

@author: Administrator
'''

class Redirect:
    def __init__(self, target, style=None):
        self.out = target
        if style:
            self.style = style
        else:
            self.style = self.out.GetDefaultStyle()
        
    def write(self, string):
        defStyle = self.out.GetDefaultStyle()
        self.out.SetDefaultStyle(self.style)
        self.out.AppendText(string)
        self.out.SetDefaultStyle(defStyle)
