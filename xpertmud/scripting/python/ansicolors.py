class AnsiColorTranslator:
    "uses mux shortcuts to get ansi color codes"
    
    def __init__(self):
        self.codes =  {
     'r': "\n", 'cf': "\x1b[5m", 'ci': "\x1b[7m", 
     'ch': "\x1b[1m", 'cn': "\x1b[0m", 'cu': "\x1b[4m", 'cx': "\x1b[30m", 
     'cr': "\x1b[31m", 'cg': "\x1b[32m", 'cy': "\x1b[33m", 'cb': "\x1b[34m",
     'cm': "\x1b[35m", 'cc': "\x1b[36m", 'cw': "\x1b[37m", 'cX': "\x1b[40m", 
     'cR': "\x1b[41m", 'cG': "\x1b[42m", 'cY': "\x1b[43m", 'cB': "\x1b[44m",
     'cM': "\x1b[45m", 'cC': "\x1b[46m", 'cW': "\x1b[47m" }
        self.deliminator = '%'

    def keys(self):
        return self.codes.keys()

    def code(self, key):
        return self.codes[key]

    def end(self):
        return "\x1b[0m"

ansicolors = AnsiColorTranslator()


        
    
