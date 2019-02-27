import XM

class TapScript:
    def install(self):
        #setWriteRegExps(r'u^> ', ur'^$')
        setWriteRegExps(ur'^$', ur'.')
        
        def onKeySend(x):
            def keyMatch(m):
                connection().send(x + "\n")
                return 1
            return keyMatch

        delKeyBinding(".")
        addKeyBinding("KP_N", ur'^0000.... KP_1$', onKeySend("sw"))
        addKeyBinding("KP_S", ur'^0000.... KP_2$', onKeySend("s"))
        addKeyBinding("KP_SW", ur'^0000.... KP_3$', onKeySend("se"))
        addKeyBinding("KP_W", ur'^0000.... KP_4$', onKeySend("w"))
        addKeyBinding("KP_E", ur'^0000.... KP_6$', onKeySend("e"))
        addKeyBinding("KP_NW", ur'^0000.... KP_7$', onKeySend("nw"))
        addKeyBinding("KP_N", ur'^0000.... KP_8$', onKeySend("n"))
        addKeyBinding("KP_NE", ur'^0000.... KP_9$', onKeySend("ne"))

        addKeyBinding("KP_L", ur'^0000.... KP_5$', onKeySend("l"))

        addKeyBinding("KP_UP", ur'^1000.... KP_Up$', onKeySend("up"))
        addKeyBinding("KP_DOWN", ur'^1000.... KP_Down$', onKeySend("down"))
        addKeyBinding("KP_IN", ur'^1000.... KP_Right$', onKeySend("in"))
        addKeyBinding("KP_OUT", ur'^1000.... KP_Left$', onKeySend("out"))

        addKeyBinding("KP_INV", ur'^0000.... KP_-$',
                      onKeySend("i2 -visual\ni2"))

        def switchMode(x):
            def keyMatch(m):
                self.mode(x)
                return 1
            return keyMatch
        addKeyBinding("MODE_NORMAL", ur'^0000.... F5$', switchMode("normal"))
        addKeyBinding("MODE_ALL", ur'^0000.... F6$', switchMode("all"))

        self.paintBody = 1
        self.equipmentHeight = 15

        delTrigger(".")
        addTrigger("TPA_ALL", ur'^(.*)$', self.parseAll, 0, 0)
        addTrigger("TPEquipment", ur'^(.*)$', self.parseEquipment, 0, 0)
        addTrigger("TPInventory", ur'^(.*)$', self.parseInventory, 0, 0)
        addTrigger("TPRoom", ur'^(.*)$', self.parseRoom, 0, 0)

        self.info = XMTextWindow("Info")
        self.info.hideCaption()
        self.chat = XMTextBufferWindow("Chat")
        self.chat.hideCaption()
        self.map = XMTextWindow("Map")
        self.map.hideCaption()
        self.log = XMTextBufferWindow("Log")
        self.log.resizeChars(90, 30)
        statusWindow().hideCaption()
        self.mode("normal")

        def sendTimer(x):
            def timerFunc():
                connection().send(x + "\n")
            return timerFunc
        delTimer(".")
        addTimer("TPA_Equipment", 5, sendTimer("i2 -visual\ni2"), 0)

    def prepare(self):
        connection().send("set columns=9999\n")
        connection().send("set lines=0\n")
        connection().send("i2 -reset\n")
        connection().send("i2 ! -default -1 -allowcolor -deep -countup -categories -all -noused -set\n")
        #connection().send("set eshowline1=$qh $qs $qe XP: $x\n")
        connection().send("set eshowline1=$qh $qs $qe $qE XP:[$x] $G\n")
        connection().send("set eshowline2=$qh $qs $qe $qE XP:[$x] $G\n")
        connection().send("set eshowline3=$qh $qs $qe $qE XP:[$x] $G\n")

    def strip(self, line, space=1):
        text = XM.ansiToRaw(line)
        #statusWindow().write("Type: \"" + str(type(text)) + "\"\n")
        #statusWindow().write("str(text): \"" + str(text) + "\"\n")
        #statusWindow().write("text: \"" + text + "\"\n")
        stripped = line
        prompt = 0
        if re.match(ur'^>', text) != None:
            prompt = 1
            stripped = re.sub(ur'[^>]*> ', '', stripped)
        if space:
            stripped = re.sub(ur'\s+$', '', stripped)
        text = XM.ansiToRaw(stripped)
        #self.log.write("\"%s\"\n" % text)

        return (stripped, text, prompt)

    def parseAll(self, m):
        return self.parseAllLine(m.group(0))
        
    def parseAllLine(self, line):
        #m = re.match(ur'^(.*)$', line)
        (withoutPrompt, text, prompt) = self.strip(line)

        match = re.match(ur'^Warning: -visual overrides.*$', text)
        if match != None:
            return ""

        match = re.match(ur'^(\[)', text)
        if match != None:
            return self.parseChat(withoutPrompt, match, "chat")

        match = re.match(ur'^(You told )|(.* tells you: .*)', text)
        if match != None:
            return self.parseChat(withoutPrompt, match, "tell")
        
        match = re.match(ur'((?:[^\w]|O|I|X)(?:[^\w]|O|I|X)(?:[^\w]|O|I|X)(?:[^\w]|O|I|X)(?:[^\w]|O|I|X)(?:[^\w]|O|I|X)(?:[^\w]|O|I|X)) (\w.*)$', text)
        if match != None:
            self.map.setCursor(0, int((self.map.getLines()-7)/2))
            self.map.clearBOS()
            setTriggerState("^TPA", 0)
            setTriggerState("^TPRoom", 1)
            self.inDesc = 1
            statusWindow().write("\n")
            return self.parseRoomLine(withoutPrompt)
        
        match = re.match(ur'(LE:\[(\d+)/(\d+)\|(\d+)%\|([+-]?\d+)\] ME:\[(\d+)/(\d+)\] PE:\[(\d+)/(\d+)\]) (E:\[([^]]+)\] XP:\[(\d+)\](?: (.*))?)$', text)
        if match != None:
            return self.parseEShow(withoutPrompt, match)
        
        match = re.match(ur'Your equipment:', text)
        if match != None:
            if self.paintBody == 0:
                self.equipmentHeight = 4
            self.info.setCursor(0, 3)
            setTriggerState("^TPA", 0)
            setTriggerState("^TPEquipment", 1)
            return ""
        
        match = re.match(ur'You are carrying:', text)
        if match != None:
            self.info.setCursor(0, self.equipmentHeight)
            setTriggerState("^TPA", 0)
            setTriggerState("^TPInventory", 1)
            return ""
            
        return withoutPrompt

    def parseChat(self, original, match, type):
        if type == "tell":
            self.chat.setIntensive(1)
            self.chat.setFGColor(MAGENTA)
            self.chat.write("-> ")
        if type == "tell":
            self.chat.resetAttributes()
        
        self.chat.write(original + "\n")
        return ""
    
    def parseRoom(self, m):
        return self.parseRoomLine(m.group(0))
    
    def parseRoomLine(self, line):
        (withoutPrompt, text, prompt) = self.strip(line, 0)
        if prompt:
            setTriggerState("^TPRoom", 0)
            setTriggerState("^TPA", 1)
            self.inDesc = 1
            return self.parseAllLine(line)
        else:
            match = re.match(ur'(?:((?:[^\w]|O|I|X)(?:[^\w]|O|I|X)(?:[^\w]|O|I|X)(?:[^\w]|O|I|X)(?:[^\w]|O|I|X)(?:[^\w]|O|I|X)(?:[^\w]|O|I|X))(?: (.*))?)?$', text)
            if match != None:
                if len(match.groups()) > 0 and match.group(1) != None:
                    self.map.setCursor(int((self.map.getColumns()-7)/2),
                                       self.map.getCursorY())
                    self.map.write(str(match.group(1)) + "\n")
                    self.map.clearEOL()
                else:
                    self.map.write("\n")
                if (len(match.groups()) == 2 and
                    match.group(2) != None and
                    re.match(ur'^\s*$', str(match.group(2))) == None):
                    
                    statusWindow().setFGColor(YELLOW)
                    statusWindow().write(str(match.group(2)))
                    self.log.write("Length: %d %d %d\n" % (len(line),
                                                           len(withoutPrompt),
                                                           len(text)))
                    if len(line) < 240:
                        statusWindow().write("\n")
                    statusWindow().resetAttributes()
            else:
                if self.inDesc:
                    #statusWindow().write("\n")
                    self.inDesc = 0
                self.map.clearEOS()
                #statusWindow().write(withoutPrompt + "\n")
                setTriggerState("^TPRoom", 0)
                setTriggerState("^TPA", 1)
                self.inDesc = 1
                #            return ""
                return self.parseAllLine(line)
        
    def parseEShow(self, original, match):
        st = match.groups()
        self.info.setCursor(0, 0)
        self.info.setFGColor(GREEN)
        self.info.setIntensive(1)
        self.info.write("LE:[")
        if int(st[1]) < int(st[2])/10:
            self.info.setIntensive(0)
            self.info.setFGColor(RED)
        elif int(st[1]) < int(st[2])/4:
            self.info.setFGColor(RED)
        elif int(st[1]) < int(st[2])/2:
            self.info.setFGColor(YELLOW)
        elif int(st[1]) < int(st[2])*3/4:
            self.info.setIntensive(0)
        self.info.write(st[1])
        self.info.setFGColor(GREEN)
        self.info.setIntensive(1)
        self.info.write("/%s|%s%%|%s] "%(st[2],st[3],st[4]))

        self.info.setIntensive(0)
        self.info.setFGColor(MAGENTA)
        self.info.write("ME:[")
        if int(st[5]) < int(st[6]):
            self.info.setIntensive(1)
        self.info.write(st[5])
        self.info.setIntensive(0)
        self.info.setFGColor(MAGENTA)
        self.info.write("/%s] "%st[6])
        
        self.info.setIntensive(0)
        self.info.setFGColor(YELLOW)
        self.info.write("PE:[")
        if int(st[7]) < int(st[8]):
            self.info.setIntensive(1)
        self.info.write(st[7])
        self.info.setIntensive(0)
        self.info.setFGColor(YELLOW)
        self.info.write("/%s] "%st[7])

        self.info.resetAttributes()
        self.info.clearEOL()
        self.info.newline()

        self.info.setIntensive(0)
        self.info.setFGColor(RED)
        self.info.write("E:[%s] "%st[10])
        
        self.info.setIntensive(1)
        self.info.setFGColor(BLUE)
        self.info.write("XP:[%s] "%st[11])

        if st[12] != None:
            self.info.setIntensive(1)
            self.info.setFGColor(YELLOW)
            self.info.write(st[12])

        self.info.resetAttributes()
        self.info.clearEOL()
        self.info.newline()
        self.info.clearEOL()
        
        return ""

    def parseEquipment(self, match):
        (withoutPrompt, text, prompt) = self.strip(match.group(0))
        if not prompt:
            m = re.match(ur'^(..........) ([^(]+)(.*)\(([^)]+)\)[^(]*$',
                         match.group(0))

            if m != None and len(m.groups()) == 4:
                graphic = m.group(1)
                thing = m.group(2)
                thing = re.sub(ur'\s+$', "", thing)
                state = m.group(3)
                state = re.sub(ur'\s+$', "", state)
                position = m.group(4)
                if self.paintBody == 1:
                    self.info.write("%s (%s)\t%s"%(graphic, position, thing))
                    self.info.clearEOL()
                    self.info.newline()
                elif thing != "none":
                    self.info.write("%s:\t%s %s"%(position, thing, state))
                    self.equipmentHeight += 1
                    self.info.clearEOL()
                    self.info.newline()
                    
            return ""
        else:
            self.info.clearLine()
            setTriggerState("^TPEquipment", 0)
            setTriggerState("^TPA", 1)

            return self.parseAllLine(match.group(0))

    def parseInventory(self, match):
        (withoutPrompt, text, prompt) = self.strip(match.group(0))
        if not prompt:
            if re.match('You carry ', text) == None:
                self.info.write(match.group(0))
                self.info.clearEOL()
                self.info.newline()
            else:
                m = re.match('^([^,]+,) (.*) (and.*)$', withoutPrompt)
                if m != None:
                    for i in range(1, len(m.groups())+1):
                        self.info.write(m.group(i))
                        self.info.clearEOL()
                        if i < len(m.groups()):
                            self.info.newline()

            return ""
        else:
            self.info.clearEOS()
            setTriggerState("^TPInventory", 0)
            setTriggerState("^TPA", 1)

            return self.parseAllLine(match.group(0))

    def mode(self, mode):
        if mode == "normal":
            statusWindow().maximize()
            self.info.hide()
            self.chat.hide()
            self.map.hide()

            setTriggerState("^TP", 0)
            #setTimerState("^TPA", 0)
            
            connection().send("set columns %d\n" % statusWindow().getColumns())

        elif mode == "all":
            statusWindow().resizeChars(83, 10)
            self.map.resizeChars(7, 15)
            self.chat.resize(statusWindow().getSizeX()
                             -self.map.getSizeX(), self.map.getSizeY())
            statusWindow().resize(statusWindow().getSizeX(),
                                  XM.mdiHeight()-self.chat.getSizeY())
            statusWindow().move(0, 0)
            self.map.move(0, statusWindow().getSizeY())
            self.chat.move(self.map.getSizeX(), statusWindow().getSizeY())
            self.info.move(statusWindow().getSizeX(), 0)
            self.info.resize(XM.mdiWidth()-statusWindow().getSizeX(),
                             XM.mdiHeight())
            self.chat.show()
            self.map.show()
            self.info.show()
            
            setTriggerState("^TPA", 1)
            #setTimerState("^TPA", 1)

            connection().send("set columns 9999")
            connection().send("le2\n")

    def showBody(self, on):
        if on:
            self.paintBody = 1
            self.equipmentHeight = 15
        else:
            self.paintBody = 0
    

ts = TapScript()
ts.install()
prs("Skript started\n\n")
