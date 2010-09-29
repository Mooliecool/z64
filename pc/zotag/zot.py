#!/usr/bin/python

from struct import unpack, pack
from sys import argv        #argv[1:2]= filenames, argv[3]= bank

def main():
    try:
        bank,c,f,b=int(argv[3]),0,open(argv[1],"r+b"),open(argv[2],"rb")
        #c = Counter for number of times an offset is recalculated
        #f = file object being appended to
        #b = file object being appended
        f.seek(0,2)
        old_end=f.tell()    #Get the old end of the file
        f.write(b.read())   #Write the file being appended
        b_end=b.tell()      #Get the old end of the file being appended
        new_end=f.tell()    #Get the new end of the file
        b.close()           #Close the file being appended
        f.seek(old_end)
        dl=False
        for i in range(old_end,new_end,4):
            w=unpack(">L",f.read(4))[0] #Word at offset i
            #Offset possible?  Bank correct? Offset point to within file?
            #yes -> we have an offset, recalc
            #no  -> NEXT.
            if (dl and not (w & 3) and (w>>24 == bank) and (w & 0xFFFFFF < b_end)) :
                f.seek(i)
                f.write(pack(">L",w+old_end))   #Write recalculated word
                c+=1                            #Add to count of recalculations
            elif (not dl and w == 0xE7000000) :
                dl = True
            elif (dl and w == 0xDF000000) :
                dl = False
        f.close()   #Close the file being appended to
        return c
    except:
        return -1   #This should only happen if there are invalid arguments
    
if __name__=="__main__":
    if len(argv)!=4:    #Not enough arguments to run
        raw_input("Usage:\n$ python %s <File to append to> <File to append> <Bank>\nExample:\n$ python %s /home/user/object1.zobj /home/user/object2.zobj 6"%(argv[0],argv[0]))
    else:
        c=main()   #Merge and re-calculate the files
        if c > -1: #Success!
            raw_input("Successfully recalculated %i offsets\nPress any key to continue..."%c)
        else:   #Something went wrong
            raw_input("Error in appending files - Check your arguments!")
