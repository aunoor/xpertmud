print "in bt3030.py (2)"

latestContact = None

addKeyBinding('Mode-Normal', '........ F5',
              lambda m: ((initMode("normal") and 0) or 1))
addKeyBinding('Mode-Battle', '........ F6',
	      lambda m: ((initMode("battle") and 0) or 1))

addKeyBinding('BT_Heading-240', '..00.... KP_1', 
	      lambda m: ((XM.send("heading 240\n") and 0) or 1))
addKeyBinding('BT_Heading-180', '..00.... KP_2', 
	      lambda m: ((XM.send("heading 180\n") and 0) or 1))
addKeyBinding('BT_Heading-120', '..00.... KP_3', 
	      lambda m: ((XM.send("heading 120\n") and 0) or 1))
addKeyBinding('BT_Heading-270', '..00.... KP_4', 
	      lambda m: ((XM.send("heading 270\n") and 0) or 1))
addKeyBinding('BT_Heading-90', '..00.... KP_6', 
	      lambda m: ((XM.send("heading 90\n") and 0) or 1))
addKeyBinding('BT_Heading-300', '..00.... KP_7', 
	      lambda m: ((XM.send("heading 300\n") and 0) or 1))
addKeyBinding('BT_Heading-0', '..00.... KP_8', 
	      lambda m: ((XM.send("heading 0\n") and 0) or 1))
addKeyBinding('BT_Heading-60', '..00.... KP_9', 
	      lambda m: ((XM.send("heading 60\n") and 0) or 1))

addKeyBinding('BT_Rottorso l', '..10.... KP_7', 
	      lambda m: ((XM.send("rottorso l\n") and 0) or 1))
addKeyBinding('BT_Rottorso c', '..10.... KP_8', 
		  lambda m: ((XM.send("rottorso c\n") and 0) or 1))
addKeyBinding('BT_Rottorso r', '..10.... KP_9', 
	      lambda m: ((XM.send("rottorso r\n") and 0) or 1))

addKeyBinding('BT_Target LL', '..01.... KP_1', 
	      lambda m: ((XM.send("target LL\n") and 0) or 1))
addKeyBinding('BT_Target -', '..01.... KP_2', 
	      lambda m: ((XM.send("target -\n") and 0) or 1))
addKeyBinding('BT_Target RL', '..01.... KP_3', 
	      lambda m: ((XM.send("target RL\n") and 0) or 1))
addKeyBinding('BT_Target LA', '..01.... KP_4', 
	      lambda m: ((XM.send("target LA\n") and 0) or 1))
addKeyBinding('BT_Target CT', '..01.... KP_5', 
	      lambda m: ((XM.send("target CT\n") and 0) or 1))
addKeyBinding('BT_Target RA', '..01.... KP_6', 
	      lambda m: ((XM.send("target RA\n") and 0) or 1))
addKeyBinding('BT_Target LT', '..01.... KP_7', 
	      lambda m: ((XM.send("target LT\n") and 0) or 1))
addKeyBinding('BT_Target H', '..01.... KP_8', 
	      lambda m: ((XM.send("target H\n") and 0) or 1))
addKeyBinding('BT_Target RT', '..01.... KP_9', 
	      lambda m: ((XM.send("target RT\n") and 0) or 1))

addKeyBinding('BT_fire', '..10.... (\d)', 
	      lambda m: ((XM.send("fire %s\n" % m.group(1)) and 0) or 1))
addKeyBinding('BT_sight', '..01.... (\d)', 
	      lambda m: ((XM.send("sight %s\n" % m.group(1)) and 0) or 1))

addKeyBinding('BT_lock latest contact', '..00.... KP_Divide', lambda m: 
              ((((not latestContact) or XM.send("lock %s\n" % latestContact))
                and 0) or 1))

addKeyBinding('BT_lock -', '..10.... KP_Divide', 
	      lambda m: ((XM.send("lock -\n") and 0) or 1))
addKeyBinding('BT_scan', '..00.... KP_Multiply', 
	      lambda m: ((XM.send("scan\n") and 0) or 1))
addKeyBinding('BT_status', '..00.... KP_Subtract', 
	      lambda m: ((XM.send("status\n") and 0) or 1))

addKeyBinding('BT_firetic', '0.00.... F([1234])', lambda m:
              ((XM.send("firetic %d\n" % (int(m.group(1))-1)) and 0) or 1))

addKeyBinding('BT_listtic', '1.01.... F([1234])', lambda m:
              ((XM.send("listtic %d\n" % (int(m.group(1))-1)) and 0) or 1))

addKeyBinding('BT_shortcut', '..10.... (.*)', lambda m:
              ((XM.send(m.group(1) + "\n") and 0) or 1))

lock = None
battleFontSize = 10
leftWindowWidth=84
rightWindowWidth=77

mode = "battle"
latestContact = None
mecha = "unknown"

battleNavMap = XMTextWindow()
battleNavMap.setTitle("Navigation map")
battleNavMap.setFont("fixed", battleFontSize)
battleNavMap.resizeChars(leftWindowWidth, 13)

battleTacticalMap = XMTextWindow()
battleTacticalMap.setTitle("Tactical map")
battleTacticalMap.setFont("fixed", battleFontSize)
battleTacticalMap.resizeChars(rightWindowWidth, 22)

battleStatus = XMTextWindow()
battleStatus.setTitle("Status")
battleStatus.setFont("fixed", battleFontSize)
battleStatus.resizeChars(rightWindowWidth, 36)

battleScan = XMTextWindow()
battleScan.setTitle("Scan")
battleScan.setFont("fixed", battleFontSize)
battleScan.resizeChars(leftWindowWidth, 30)

statusLock = None
navLock = None
tactLock = None
heatLock = None

oldLocation = (0, 0, 0)

def initMode(newMode):
    global mode
    if(mode == "battle"):
        battleTacticalMap.hide()
        battleNavMap.hide()
        battleStatus.hide()
        battleScan.hide()
        lock = None

    mode = newMode

    if(mode == "battle"):
        setKeyBindingState("^BT_", 1)

        battleScan.hide()
        statusWindow().setFont("fixed", battleFontSize)
        statusWindow().resizeChars(leftWindowWidth, 45)

        battleNavMap.move(0, 0)
        battleScan.move(0, battleNavMap.getSizeY())
        statusWindow().move(0, battleNavMap.getSizeY())
        
        battleTacticalMap.move(battleNavMap.getSizeX(), 0)
        battleStatus.move(battleNavMap.getSizeX(), 
                          battleTacticalMap.getSizeY())
        
        battleTacticalMap.show()
        battleNavMap.show()
        battleStatus.show()
        
        XM.send("status\n")
        
        setTimerState("^BM_", 1)
        XM.send("tactical\n")
        XM.send("navigate\n")
    else:
        setKeyBindingState("^BT_", 0)
        
        statusWindow().setFont("fixed", 10)
        statusWindow().maximize()
        setTimerState("^BM_", 0)

def timerTact():
    global tactLock
    if(not tactLock and not navLock):
        XM.send("tactical\n")
        tactLock = 1
    
addTimer("BM_GetTactical", 5.0, timerTact)

def timerNav():
    global navLock
    if(not tactLock and not navLock):
        XM.send("navigate\n")
        navLock = 1

addTimer("BM_GetNavigate", 1.0, timerNav)

def parseNormal(match):
    text = match.group(0)

    read = None

    if(read):
        return ""
    else:
        return text

addTrigger("Normal", "(?sm)^.*$", parseNormal, 1, 1)

initMode("normal")

statusWindow().setFGColor(GREEN)
statusWindow().setIntensive(1)
statusWindow().write("\nBattleTech 3030 script loaded...\n")
statusWindow().setFGColor(BLUE)
statusWindow().write("Enter '#help' for help on this script\n")
statusWindow().resetAttributes()

writeAtOnceRegExp = r'^$'
