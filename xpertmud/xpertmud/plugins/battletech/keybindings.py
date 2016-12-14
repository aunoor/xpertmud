from __main__ import *

class NormalKeybindings:
    def install(self):
        addKeyBinding('BT_shortcut', r'^.10..... (.)$', lambda m:
              ((XM.send("." + m.group(1).lower() + "\n") and 0) or 1))

        addKeyBinding('BT_Heading-240', '.00..... KP_1', 
	      lambda m: ((XM.send("heading 240\n") and 0) or 1))
        addKeyBinding('BT_Heading-180', '.00..... KP_2', 
	      lambda m: ((XM.send("heading 180\n") and 0) or 1))
        addKeyBinding('BT_Heading-120', '.00..... KP_3', 
	      lambda m: ((XM.send("heading 120\n") and 0) or 1))
        addKeyBinding('BT_Heading-270', '.00..... KP_4', 
	      lambda m: ((XM.send("heading 270\n") and 0) or 1))
        addKeyBinding('BT_Heading-90', '.00..... KP_6', 
	      lambda m: ((XM.send("heading 90\n") and 0) or 1))
        addKeyBinding('BT_Heading-300', '.00..... KP_7', 
	      lambda m: ((XM.send("heading 300\n") and 0) or 1))
        addKeyBinding('BT_Heading-0', '.00..... KP_8', 
	      lambda m: ((XM.send("heading 0\n") and 0) or 1))
        addKeyBinding('BT_Heading-60', '.00..... KP_9', 
	      lambda m: ((XM.send("heading 60\n") and 0) or 1))

        addKeyBinding('BT_Rottorso l', '.10..... KP_7', 
	      lambda m: ((XM.send("rottorso l\n") and 0) or 1))
        addKeyBinding('BT_Rottorso c', '.10..... KP_8', 
		  lambda m: ((XM.send("rottorso c\n") and 0) or 1))
        addKeyBinding('BT_Rottorso r', '.10..... KP_9', 
	      lambda m: ((XM.send("rottorso r\n") and 0) or 1))

        addKeyBinding('BT_Target LL', '.01..... KP_1', 
	      lambda m: ((XM.send("target LL\n") and 0) or 1))
        addKeyBinding('BT_Target -', '.01..... KP_2', 
	      lambda m: ((XM.send("target -\n") and 0) or 1))
        addKeyBinding('BT_Target RL', '.01..... KP_3', 
	      lambda m: ((XM.send("target RL\n") and 0) or 1))
        addKeyBinding('BT_Target LA', '.01..... KP_4', 
	      lambda m: ((XM.send("target LA\n") and 0) or 1))
        addKeyBinding('BT_Target CT', '.01..... KP_5', 
	      lambda m: ((XM.send("target CT\n") and 0) or 1))
        addKeyBinding('BT_Target RA', '.01..... KP_6', 
	      lambda m: ((XM.send("target RA\n") and 0) or 1))
        addKeyBinding('BT_Target LT', '.01..... KP_7', 
	      lambda m: ((XM.send("target LT\n") and 0) or 1))
        addKeyBinding('BT_Target H', '.01..... KP_8', 
	      lambda m: ((XM.send("target H\n") and 0) or 1))
        addKeyBinding('BT_Target RT', '.01..... KP_9', 
	      lambda m: ((XM.send("target RT\n") and 0) or 1))
        
        addKeyBinding('BT_fire', '.10..... (\d)', 
	      lambda m: ((XM.send("fire %s\n" % m.group(1)) and 0) or 1))
        addKeyBinding('BT_sight', '.01..... (\d)', 
	      lambda m: ((XM.send("sight %s\n" % m.group(1)) and 0) or 1))

        addKeyBinding('BT_lock latest contact', '.00..... KP_/', lambda m: 
              ((((not latestContact) or XM.send("lock %s\n" % latestContact))
                and 0) or 1))

        addKeyBinding('BT_lock -', '.10..... KP_/', 
	      lambda m: ((XM.send("lock -\n") and 0) or 1))
        addKeyBinding('BT_scan', '.00..... KP_\*', 
	      lambda m: ((XM.send("scan\n") and 0) or 1))
        addKeyBinding('BT_status', '.00..... KP_-', 
	      lambda m: ((XM.send("status\n") and 0) or 1))

        addKeyBinding('BT_firetic', '000..... F([1234])', lambda m:
              ((XM.send("firetic %d\n" % (int(m.group(1))-1)) and 0) or 1))

        addKeyBinding('BT_listtic', '101..... F([1234])', lambda m:
              ((XM.send("listtic %d\n" % (int(m.group(1))-1)) and 0) or 1))


class LaptopKeybindings:
    def install(self):
        addKeyBinding('LBT_Heading-240', '.11..... C', 
	      lambda m: ((XM.send("heading 240\n") and 0) or 1))
        addKeyBinding('LBT_Heading-180', '.11..... V', 
	      lambda m: ((XM.send("heading 180\n") and 0) or 1))
        addKeyBinding('LBT_Heading-120', '.11..... B', 
	      lambda m: ((XM.send("heading 120\n") and 0) or 1))
        addKeyBinding('LBT_Heading-270', '.11..... D', 
	      lambda m: ((XM.send("heading 270\n") and 0) or 1))
        addKeyBinding('LBT_Heading-90', '.11..... G', 
	      lambda m: ((XM.send("heading 90\n") and 0) or 1))
        addKeyBinding('LBT_Heading-300', '.11..... E', 
	      lambda m: ((XM.send("heading 300\n") and 0) or 1))
        addKeyBinding('LBT_Heading-0', '.11..... R', 
	      lambda m: ((XM.send("heading 0\n") and 0) or 1))
        addKeyBinding('LBT_Heading-60', '.11..... T', 
	      lambda m: ((XM.send("heading 60\n") and 0) or 1))
        
