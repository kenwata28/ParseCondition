# -*- coding: utf-8 -*-
import subprocess
import sys


def main() :
    
    with open("test.txt", "r") as f :
        lines = f.readlines()
    
    for l in lines :
        if l.strip() == "" : 
            continue
        
        if l.strip().startswith("#") : 
            print(l, end="")
            continue
        
        eq = l.split(",")[0].strip()
        ref = l.split(",")[1].strip()
        unit_test(eq, ref)
        
    print("----------")

    
    

def unit_test(eq, ref) :
    execmd = f"test.exe \"{eq}\""
    # subprocess.call(execmd)
    res = subprocess.run(execmd, capture_output=True, text=True)
    print("")
    try :
        res_out = res.stdout.strip()
        res_err = res.stderr.strip()
        if res_out == ref :
            txt = f"{eq} = {res_out}   -> Success"
            print(f"\033[92m{txt}\033[0m")
        else :
            txt = f"{eq} = {res_out}   -> Failed"
            print(f"\033[93m{txt}\033[0m")
        
        sys.stderr.write(res_err)
    except :
        print(res)

    
    
    

if __name__ == '__main__' :
    main()