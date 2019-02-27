from __main__ import *

class XMMapper(XMPlugin):
    def __init__(self, title):
        XMPlugin.__init__(self, "xmud_mapper", "XmudMapper", title)

    def pluginCall(self, function, args):
        if function == 0:
            return "ButtonPress reached python"
        else:
            return "Something other (TODO)"
