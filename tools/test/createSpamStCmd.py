#!/usr/bin/env python

def createFile(nChannels):
    s = open('st.cmd.spam.template').read()
    insert = ''
    for i in range(1,nChannels+1):
        insert += 'dbLoadRecords("spam.db","N=X%i");\n' % i
    s = s.replace('DB_LOAD_RECORDS', insert)
    open('st.cmd.spam', 'w').write(s)

    

if __name__ == '__main__':
    import sys
    nChannels = int(sys.argv[1])
    createFile(nChannels)
    
