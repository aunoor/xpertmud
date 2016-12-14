from __main__ import *


class Contacts:
    reportChannel = 'a'
    def install(self, bcore):
        self.core = bcore
        def a(arg):
            mechid = arg.group(2)
            if(mechid != None):
                me = self.core.getContact(mechid)
                if(me == None):
                    statusWindow().setIntensive(1)
                    statusWindow().setFGColor(BLUE)
                    statusWindow().write("No Contact info on %s.\n" % mechid)
                    statusWindow().resetAttributes()
                    return ""
                s = ' [%s]%s %s x:%4d y:%4d z:%4d r:%5.1f b:%4d s:%5.1d h:%4d S: %s' \
                    % ( me['ID'],me['UT'],me['MN'],int(me['X']),int(me['Y']),
                        int(me['Z']), float(me['RN']), int(me['BR']),
                        float(me['SP']),int(me['HD']),me['FL'])
                if(me['JH'] != '-'):
                    s += ' Jump h:%4d' % int(me['JH'])
                if(int(me['AG']) > 0):
                    s += ' Last Contact: %02d:%02d' \
                    % (int(me['AG'])//60, int(me['AG'])%60)
            
                if(me['ID'].isupper()):
                    if(me['MN'] == "something"):
                        XM.send(("sendchannel %s=* UNKNOWN * :" % self.reportChannel)
                            + s + "\n")
                    else:
                        XM.send(("sendchannel %s=* ENEMY * :" % self.reportChannel)
                            + s + "\n")
                else:
                    XM.send(("sendchannel %s=FRIEND :" % self.reportChannel) + s + "\n")
                return ""
            else:
                statusWindow().setIntensive(1)
                statusWindow().setFGColor(BLUE)
                statusWindow().write("No Contact info on %s.\n" % mechid)
                statusWindow().resetAttributes()
                return ""

        addAlias("BT_CONCALLER",r'^con(\s+([A-Za-z][A-Za-z]))', a)

        def a(m):
            g = self.core.getContacts()
            if(g != None):
                for i in self.core.getContacts():
                    self.core.deleteContact(i)
            statusWindow().setIntensive(1)
            statusWindow().setFGColor(BLUE)
            statusWindow().write("Contact List clear.\n")
            statusWindow().resetAttributes()
            return ""

        addAlias("DELETE_CONS", r'^clearcons', a)
