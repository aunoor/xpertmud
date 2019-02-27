from __main__ import *
from time import time

class Claims:
    reportChannel = 'a'    
    def install(self):
        self.claimStart = 0
        self.claimTime  = 0
        def a(m):
            self.claimTime = int(m.group(1))*60
            self.claimStart = int(time())
            statusWindow().setIntensive(1)
            statusWindow().setFGColor(BLUE)
            statusWindow().write("Claim Noted.\n")
            statusWindow().resetAttributes()
            return m.group(0)
            
        addTrigger('CLAIM_TIMER',
                 r'^Complex claim initiated\. Claim will complete in (\d+) minutes\.',
                 a)
        def a(m):
            mytime = int(time())
            statusWindow().setIntensive(1)
            statusWindow().setFGColor(BLUE)
            if(mytime < self.claimTime + self.claimStart):
                rest = self.claimTime + self.claimStart - mytime
                statusWindow().write("Claim will be done in %02d:%02d minutes\n"
                                     % (int(rest/60),int(rest%60)))
            else:
                statusWindow().write("I dont know any ongoing Claims.\n")
            statusWindow().resetAttributes()
            return ""
        addAlias("CLAIM_TIMER", r"^cclaim", a, 1)

        def a(m):
            mytime = int(time())
            if(mytime < self.claimTime + self.claimStart):
                rest = self.claimTime + self.claimStart - mytime
                XM.send("sendchannel %s=TOC on Complex %02d:%02d minutes,\n"                                     % (self.reportChannel, int(rest/60),int(rest%60)))
            else:
                statusWindow().setIntensive(1)
                statusWindow().setFGColor(BLUE)
                statusWindow().write("I dont know any ongoing Claims.\n")
                statusWindow().resetAttributes()
            return ""

        addAlias("CLAIM_TIMER", r"^rclaim", a, 1)

        def a(m):
            interval = 5*60
            if(m.group(2)):
                interval = int(m.group(2))*60
            mytime = int(time())
            if(interval > 0):
                if(mytime < self.claimStart + self.claimTime):
                    statusWindow().setIntensive(1)
                    statusWindow().setFGColor(BLUE)
                    statusWindow().write("AutoTOC started. Will report every %d seconds\n"
                                         % interval)
                    statusWindow().resetAttributes()
                    delDelayed(r"^CLAIM_TIMER")
                    calltime =self.claimStart + self.claimTime - interval
                    while(calltime > mytime):
                        addDelayed("CLAIM_TIMER", calltime - mytime, "rclaim")
                        calltime -= interval
                else:
                    statusWindow().setIntensive(1)
                    statusWindow().setFGColor(BLUE)
                    statusWindow().write("I dont know any ongoing Claims.\n")
                    statusWindow().resetAttributes()
            else:
                delDelayed(r"^CLAIM_TIMER")
                statusWindow().setIntensive(1)
                statusWindow().setFGColor(BLUE)
                statusWindow().write("AutoTOC disengaged.\n")
                statusWindow().resetAttributes()

            return ""

        addAlias("CLAIM_TIMER", r"^autotoc(\s+(\d+))?", a, 1)

        def a(m):
            self.claimTime = int(m.group(2))*60
            self.claimStart = int(time())
            statusWindow().setIntensive(1)
            statusWindow().setFGColor(BLUE)
            statusWindow().write("Claim set.\n")
            statusWindow().resetAttributes()
            return ""

        addAlias("CLAIM_TIMER", r"^setclaim(\s+(\d+))?", a, 1)
