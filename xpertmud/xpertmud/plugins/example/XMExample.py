from __main__ import *

class XMExample(XMPlugin):
    def __init__(self, title):
        XMPlugin.__init__(self, "xmud_example", "XmudExample", title)

    def setButtonText(self, text):
        XM.Plugin_call(self.getID(), 1, text)

    def getButtonText(self):
        return XM.Plugin_call(self.getID(), 0, "")

    def pluginCall(self, function, args):
        if function == 0:
            return "ButtonPress reached python"
        else:
            return "Something other (TODO)"
        
