from __main__ import *
from battletech import *
from keybindings import *
from frequencies import *
from claims import *
from contacts import *

class BattleCockpit:
    def __init__(self):
        self.leftRowChars = 84
        self.normalFontSize = 12
        self.battleFontSize = 10
        self.contactInv = 7
        self.statusInv = 4
        self.tactInv = 0
        self.weapInv = 10
        
        NormalKeybindings().install()
        addKeyBinding('BC_NormalMode', '........ F5',
                      lambda m: ((self.mode(0) and 0) or 1))
        addKeyBinding('BC_BattleMode', '........ F6',
                      lambda m: ((self.mode(1) and 0) or 1))
        statusWindow().hideCaption()
        statusWindow().setFont("fixed", self.normalFontSize);
        self.core = BTBattleCore("core")
        self.core.startSecondary()
        self.core.hideCaption()
        setWriteRegExps(r'^$', r'.?')
        addTrigger("HUDINFO", r'^#HUD.*$', self.core.parseTriggerHUD)

        def parseText(m):
            self.core.parseText(XM.ansiToRaw(m.group(1)))
            return m.group(1)
        addTrigger("TEXTPARSER", r'^(.*)$', parseText) # 0, 0

        def delParseText(m):
            self.core.parseText(XM.ansiToRaw(m.group(1)))
            return None
        addTrigger("TEXTPARSERSKIP", r'^(.*)$', delParseText, 1, 0)

        addTrigger("STARTSKIP", r'^SSKIP$',
                   lambda m: setTriggerState("^TEXTPARSERSKIP$", 1) or "")
        addTrigger("ENDSKIP", r'^ESKIP$',
                   lambda m: setTriggerState("^TEXTPARSERSKIP$", 0) or "")
        
        #        self.nav = BTBattleMapView("navigate")
        #        self.nav.hideCaption()
        #        self.nav.hideQuickbar()
        #        self.nav.setZoom(50)
        self.speed = BTSpeedView("speed")
        self.speed.hideCaption()

        self.tact = BTBattleMapView("tactical")
        self.tact.hideCaption()
        self.tact.hideQuickbar()
        self.tact.setZoom(30)
        self.con = BTContactsView("contacts")
        self.con.hideCaption()
        self.fight = BTWeaponView("fight")
        self.fight.hideCaption()

        self.freq = Frequencies()
        self.freq.install()

        self.claims = Claims()
        self.claims.install()

        self.contacts = Contacts()
        self.contacts.install(self.core)

        #debug
        #        def a(m):
        #           return XM.ansiToRaw(m.string)
        
        #      addTrigger("sldfjk", r'^.*$', a);
        # Anti Idle Timer
        
        def antiIdleFunc():
            XM.send("@pemit me=The time is [time()].\n");
            setDelayed("^ANTI_IDLE$",26*60)
            self.mode(0)

        addDelayed("ANTI_IDLE", 25*60,antiIdleFunc)
        global onTextEntered
        oldonTextEntered=onTextEntered
        def onTextEntered(text):
            statusWindow().scrollLines(5000)
            oldonTextEntered(text)
            setDelayed(r"^ANTI_IDLE$", 25*60)

        addAlias("EnterToPR", "\n", "%r", 0, 1)
        def shiftEnterPR(m):
            setKeyBindingState("ShiftEnterPR", 0)
            setKeyBindingState("ShiftEnterOff", 1)
            setAliasState("EnterToPR", 1)
            statusWindow().setFGColor(BLUE)
            statusWindow().setIntensive(1)
            statusWindow().write("Enter will generate %r codes now\n")
            statusWindow().resetAttributes()
            return 1
        addKeyBinding("ShiftEnterPR", "1000.... Return", shiftEnterPR, 0)
        def shiftEnterOff(m):
            setKeyBindingState("ShiftEnterOff", 0)
            setKeyBindingState("ShiftEnterPR", 1)
            setAliasState("EnterToPR", 0)
            statusWindow().setFGColor(BLUE)
            statusWindow().setIntensive(1)
            statusWindow().write("Enter substitution disabled\n")
            statusWindow().resetAttributes()
            return 1
        addKeyBinding("ShiftEnterOff", "1000.... Return", shiftEnterOff, 1)

    def mode(self, mode):
        if mode == 0:
            # we have to find out weather the text parser
            # is active, first
            #setTriggerState("^TEXTPARSER$", 0)
            statusWindow().setFont("fixed", self.normalFontSize);
            statusWindow().maximize()
            self.core.setTacticalInterval(0)
            self.core.setStatusInterval(0)
            self.core.setContactsInterval(0)
            self.core.setWeaponsInterval(0)
            
        elif mode == 1:
            # we have to find out weather the text parser
            # is active, first
            #            setTriggerState("^TEXTPARSER$", 1)
            statusWindow().setFont("fixed", self.battleFontSize);
            statusWindow().resizeChars(self.leftRowChars, 30)
            self.con.move(0, 0)
            self.con.resize(statusWindow().getSizeX()-50, 100)
            self.con.show()

            self.speed.move(self.con.getSizeX(), 0)
            self.speed.resize(50, self.con.getSizeY())
            self.speed.show()

            statusWindow().move(0, self.con.getSizeY())
            statusWindow().resize(statusWindow().getSizeX(),
                                  XM.mdiHeight()-self.con.getSizeY())
            self.tact.resize(XM.mdiWidth()-statusWindow().getSizeX(),
                             XM.mdiHeight()*2/3)
            self.tact.move(statusWindow().getSizeX(), 0)
            self.tact.show()
#            self.nav.resize(XM.mdiHeight()-self.tact.getSizeY(),
#                            XM.mdiHeight()-self.tact.getSizeY())
#            self.nav.move(XM.mdiWidth()-self.nav.getSizeX(),
#                          self.tact.getSizeY())
#            self.nav.show()
#            self.nav.centerMech("**")
            self.fight.resize(self.tact.getSizeX(),
                              XM.mdiHeight()-self.tact.getSizeY())
            self.fight.move(XM.mdiWidth()-self.tact.getSizeX(),
                            self.tact.getSizeY())
            self.fight.show()

            self.core.setTacticalInterval(self.tactInv)
            self.core.setStatusInterval(self.statusInv)
            self.core.setContactsInterval(self.contactInv)
            self.core.setWeaponsInterval(self.weapInv)

                      
bc = BattleCockpit()
