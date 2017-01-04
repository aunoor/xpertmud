from __main__ import *
import re

#from battletech import *
#bc = BTBattleCore("title")
#bc.show()
#addTrigger("HUDINFO", r'^#HUD.*$', bc.parseTrigger)
#XM.open("btech.dhs.org", 3030)

# TODO: singleton implementation
class BTBattleCore (XMPlugin):
    def __init__(self, title):
        XMPlugin.__init__(self, "xmud_battletech", "BattleCoreWidget", title)
        self.debug = XMTextBufferWindow("core - debug")

    def pluginCall(self, function, args):
        if function == 0:
            XM.send(str(args))
            return ""
        else:
            return "Not implemented!"

    def parseTriggerHUD(self, match):
        self.parseHUD(match.string)
        return None

    def parseHUD(self, line):
        if type(line) == unicode and re.match(r'^#HUD', line):
            XM.Plugin_call(self.getID(), 0, line)

    def parseText(self, line):
        if type(line) == unicode and re.match(r'^.*$', line):
            XM.Plugin_call(self.getID(), 10, line)

    def setTacticalInterval(self, inv):
        XM.Plugin_call(self.getID(), 1, str(inv))

    def setStatusInterval(self, inv):
        XM.Plugin_call(self.getID(), 2, str(inv))

    def setWeaponsInterval(self, inv):
        XM.Plugin_call(self.getID(), 9, str(inv))

    def getOwnID(self):
        XM.Plugin_call(self.getID(), 3, '')

    def getContacts(self):
        return XM.Plugin_call(self.getID(), 4, '').split('|')

    def setContactsInterval(self, inv):
        XM.Plugin_call(self.getID(), 5, str(inv))

    def getContact(self,mechid):
        id = 'ID AC SE UT MN X Y Z RN BR SP VS HD JH RTC BTC TN HT FL AG'.split(' ')
        b = XM.Plugin_call(self.getID(), 6, str(mechid))
        if(b != None):
            return dict(zip(id,b.split('|')))
        else:
            return None

    def setContact(self,id=None,info=None):
        if(id == None):
            id = ''
        if(info == None):
            self.deleteContact(id)
        return XM.Plugin_call(self.getID(), 7, str(id), str(info))

    def deleteContact(self,id):
        return XM.Plugin_call(self.getID(), 8, str(id))

    def startSecondary(self):
        return XM.Plugin_call(self.getID(), 11, '');

        

class BTBattleMapView (XMPlugin):
    def __init__(self, title):
        XMPlugin.__init__(self, "xmud_battletech", "BattleMapWidget", title)

    def pluginCall(self, function, args):
        if function == 0:
            return "Function 0 called!"
    
    def centerMech(self, mech):
        XM.Plugin_call(self.getID(), 0, mech)

    def scroll(self, dx, dy):
        XM.Plugin_call(self.getID(), 1, "%d, %d" % (dx, dy))

    def setZoom(self, zoom):
        XM.Plugin_call(self.getID(), 2, str(zoom))
        
    def showQuickbar(self):
        XM.Plugin_call(self.getID(), 3, "");

    def hideQuickbar(self):
        XM.Plugin_call(self.getID(), 4, "");

class BTContactsView (XMPlugin):
    conCaller = 'con '
    def __init__(self, title):
        XMPlugin.__init__(self, "xmud_battletech",
                          "BattleContactWidget", title)

    def pluginCall(self, function, args):
        if function == 0:
            wrap_onTextEntered(self.conCaller + str(args))
            return ""
        else:
            return "Not implemented!"
        
class BTWeaponView (XMPlugin):
    def __init__(self, title):
        XMPlugin.__init__(self, "xmud_battletech",
                          "BattleWeaponsWidget", title)
        
class BTAmmoView (XMPlugin):
    def __init__(self, title):
        XMPlugin.__init__(self, "xmud_battletech",
                          "BattleAmmoWidget", title)
        
class BTArmsNLegsView (XMPlugin):
    def __init__(self, title):
        XMPlugin.__init__(self, "xmud_battletech",
                          "BattleArmsNLegsWidget", title)
        
class BTFightView (XMPlugin):
    def __init__(self, title):
        XMPlugin.__init__(self, "xmud_battletech",
                          "BattleFightWidget", title)

class BTSpeedView (XMPlugin):
    def __init__(self, title):
        XMPlugin.__init__(self, "xmud_battletech",
                          "BattleSpeedWidget", title)
        
