from __main__ import *

class Frequencies:
    def install(self):
        addTrigger("START_FREQ", "================================= Frequencies ================================",
                   lambda m: ((setTriggerState(r'^FREQ', 1) and 0) or m.group(0)))
        addTrigger("END_FREQ",   "==============================================================================",
                   lambda m: ((setTriggerState(r'^FREQ', 0) and 0) or m.group(0)), 0, 0)
        addTrigger("FREQ_MAIN", r'Main:.+[^\d](\d\d\d\d+)',
                   lambda m: ((XM.send("&F_MAIN me=%s\n" % m.group(1))
                               and 0) or m.group(0)), 0, 0)
        addTrigger("FREQ_BACKUP1", r'Backup1:.+[^\d](\d\d\d\d+)',
                   lambda m: ((XM.send("&F_BACKUP1 me=%s\n" % m.group(1))
                               and 0) or m.group(0)), 0, 0)
        addTrigger("FREQ_BACKUP2", r'Backup2:.+[^\d](\d\d\d\d+)',
                   lambda m: ((XM.send("&F_BACKUP2 me=%s\n" % m.group(1))
                               and 0) or m.group(0)), 0, 0)
        addTrigger("FREQ_SCREAM1", r'Scream1:.+[^\d](\d\d\d\d+)',
                   lambda m: ((XM.send("&F_SCREAM1 me=%s\n" % m.group(1))
                               and 0) or m.group(0)), 0, 0)
        addTrigger("FREQ_SCREAM2", r'Scream2:.+[^\d](\d\d\d\d+)',
                   lambda m: ((XM.send("&F_SCREAM2 me=%s\n" % m.group(1))
                               and 0) or m.group(0)), 0, 0)
        addTrigger("FREQ_STRIKE", r'Strike/Recon:.+[^\d](\d\d\d\d+)',
                   lambda m: ((XM.send("&F_STRIKE me=%s\n" % m.group(1))
                               and 0) or m.group(0)), 0, 0)
        addTrigger("FREQ_HEAVY", r'Heavy/Tank:.+[^\d](\d\d\d\d+)',
                   lambda m: ((XM.send("&F_HEAVY me=%s\n" % m.group(1))
                               and 0) or m.group(0)), 0, 0)
        addTrigger("FREQ_CHAT", r'Chat:.+[^\d](\d\d\d\d+)',
                   lambda m: ((XM.send("&F_CHAT me=%s\n" % m.group(1))
                               and 0) or m.group(0)), 0, 0)
        self.setMapping({ 'a': ( "F_MAIN", "DEG", "F_BACKUP1", "DER", "f" ),
                          'b': ( "F_SCREAM1", "R", "F_SCREAM2", "R", "g" ),
                          'c': ( "F_CHAT", "DEg" ),
                          'd': ( "F_HEAVY", "DEb" ),
                          'e': ( "F_STRIKE", "DEb" ) })

    def setMapping(self, mapping):
        self.mapping = mapping
        self.installMapping()
    
    def installMapping(self):
        cmd = ""
        for key in self.mapping.keys():
            cmd += "@force me=setchannelfreq %s=[get(me/%s)]\n" % ( key, self.mapping[key][0] )
        addAlias("SET_RADIO_FREQS", "^setallfreqs", cmd, 1)

        cmd = ""
        for key in self.mapping.keys():
            cmd += "setchannelmode %s=%s\n" % ( key, self.mapping[key][1] )
        addAlias("SET_RADIO_MODES", "^setallmodes", cmd, 1)
        
        cmd = ""
        for key in self.mapping.keys():
            cmd += "setchannelfreq %s=0\n" % ( key )
        addAlias("ZERO_RADIO_FREQS", "^zeroallfreqs", cmd, 1)

        cmd = ""
        for key in self.mapping.keys():
            if len(self.mapping[key]) > 2:
                cmd += "@force me=setchannelfreq %s=[get(me/%s)]\n" % ( key, self.mapping[key][2] )
                cmd += "@force me=setchannelfreq %s=[get(me/%s)]\n" % ( self.mapping[key][4], self.mapping[key][0] )
                cmd += "setchannelmode %s=%s\n" % ( self.mapping[key][4], self.mapping[key][3] )
        addAlias("GO_RADIO_BACKUP", "^setbackupfreqs", cmd, 1)
