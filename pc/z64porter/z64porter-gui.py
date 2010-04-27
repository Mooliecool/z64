#!/usr/bin/python

#z64Porter-GUI - tk dialog windows to get vars for setting up porter's arguments
from tkFileDialog import askopenfilename
from tkSimpleDialog import askstring
from tkMessageBox import askyesno, showerror, showinfo
from os import getcwd, path#, system
import sys
from z64porter import main as port_scene

def main():
    idir = getcwd()
    OOT_ROM_NAME = askopenfilename(title="OoT Debug ROM",filetypes=[('N64 ROM files','*.z64')],initialdir = idir)
    if(not OOT_ROM_NAME):
        return
    MM_ROM_NAME = askopenfilename(title="Decompressed MM ROM",filetypes=[('N64 ROM files','*.z64')],initialdir = idir)
    if(not MM_ROM_NAME):
        return
    mmtmp = open(MM_ROM_NAME,"rb")
    mmtmp.seek(0,2)
    if ( mmtmp.tell() == 0x2000000 ):
        showerror(title = "uh lol", message = "Decompressed MM ROM, dummy!\nAborting port!")
        sys.exit(-1)
    argv__ = ['z64porter.py', OOT_ROM_NAME, MM_ROM_NAME]
    askaddress = True
    port_en = None
    while 1:
        argv_ = argv__[:]
        argv_ .append( askstring(title = "OoT scene", prompt = "Scene to replace in OoT (use 0x if hex):" ))
        if(not argv_[-1]):
            return
        argv_ .append( askstring(title = "MM scene", prompt = "Scene to port from MM (use 0x if hex):" ))
        if(not argv_[-1]):
            return
        if (askaddress):
            if (askyesno(title = "Address", message = "Insert at your own address?" )):
                argv_.append( "-o" )
                addr_msg = "Address (hex) to insert port at:"
                if (port_en != None):
                    addr_msg += "\nReccomended: %08X"%( port_en )
                argv_.append( "0x%s" % (askstring(title = "Address", prompt = addr_msg )))
            else:
                askaddress = False
        else:
            argv_.append("-o")
            argv_.append("0x%08X" % port_en)
        if (askyesno(title = "Safe mode", message = "Port in safe mode?" )):
            argv_.append( "-s" )
        if (askyesno(title = "Music", message = "Use your own music value?" )):
            argv_.append( "-m" )
            argv_.append(askstring(title = "Music", prompt = "Music value (use 0x if hex):" ))
        argv_.append("-q")
        try:
            port_st,port_en = port_scene( argv_ )
            showinfo(title="Success", message="Scene ported successfully\nPort offsets:\n%08X - %08X"%(port_st,port_en))
        except:
            showerror(title="Uhoh!", message="Failure :(.\nIf you opened this from a shell,\nplease report the error message from." )
            break
        if not (askyesno(title = "Another", message = "Port another scene?" )):
            break

if __name__ == "__main__":
    main()
