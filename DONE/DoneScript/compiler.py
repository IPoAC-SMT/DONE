#!/usr/bin/env python3

import sys
from os.path import exists
import os

variables = []
structures = {
        "links" :  [],
        "nodes" : [],
        "rectangles": [],
        "texts":[]
    }
commands = []

def getAll(type:str):
    result = []
    for item in structures["nodes"]:
        if item["type"] == type:
            result.append(item["name"])
    return result

def maptype(tipo:str):
    tipi = ["switch","router","host","external interface","external natted interface","internet"]
    for i in range(len(tipi)):
        if tipi[i] == tipo:
            return str(i)

def serialize():
    global structures
    stringa = str(len(structures["nodes"])) +"\n"+ str(len(structures["links"]))+"\n"+str(len(structures["rectangles"]))+"\n"+str(len(structures["texts"]))
    for nodo in structures["nodes"]:
        stringa = stringa + "\n" + nodo["name"] + "\n" + maptype(nodo["type"]) + "\n" + str(nodo["x"]) + " " + str(nodo["y"])
    for link in structures["links"]:
        stringa = stringa + "\n"+link["nodo1"]+"\n"+link["nodo2"]+"\n" + maptype(link["type1"]) + "\n" + maptype(link["type2"])
    for rectangle in structures["rectangles"]:
        stringa = stringa + "\n" + str(rectangle["x1"]) + " " + str(rectangle["y1"]) + "\n" + str(rectangle["x2"]) + " " + str(rectangle["y2"]) + "\n" + str(rectangle["r"]) + " " + str(rectangle["g"]) + " " + str(rectangle["b"])
    for text in structures["texts"]:
        stringa = stringa + "\n" + text["text"] + "\n" + str(text["x"]) + " " + str(text["y"])
    return stringa

def serializeCommands():
    global commands
    global structures
    # sort commands by host
    # print formatted
    output = ""
    for item in structures["nodes"]:
        output = output + item["name"] + ":\n"
        for command in commands:
            if command["to"] == item["name"]:
                output = output+command["command"]+"\n"
        output = output + "\n"
    return output

def isName(name:str):
    global structures
    for i in range(len(structures["nodes"])):
        if structures["nodes"][i]["name"] == name:
            return True
    return False

def typeLookup(name:str):
    global structures
    for i in range(len(structures["nodes"])):
        if structures["nodes"][i]["name"] == name:
            return structures["nodes"][i]["type"]

def parseAddText(instruction:str):
    try:
        global structures
        [_,text,coords] = [i.strip() for i in instruction.split("\"")]
        [x,y] = [int(i) for i in coords.split("at ")[1].split(" ")]
        structures["texts"].append({
            "text":text,
            "x":x,
            "y":y
        })
        return None
    except:
        return "ERROR: format invalid"

def parseCreate(instruction:str): # to be called only when no variables in names
    try:
        global structures
        [type, coordsandname] = [i.strip() for i in instruction.split("create")[1].strip().split("at")]
        [coords,name] = [i.strip() for i in coordsandname.split("as")]
        [x,y] = [int(i) for i in coords.strip().split(" ")]
        if x<0 or x > 1900:
            return "ERROR: invalid x = "+str(x)
        elif y<0 or y>1000:
            return "ERROR: invalid y = "+str(y)
        elif len(name)>20:
            return "ERROR: name length can't be >20 (name "+str(name)+")"
        elif isName(name):
            return "ERROR: not a valid name or variable value: '"+type+"'"
        elif type not in ["switch","router","host","external interface","external natted interface","internet"]:
            return "ERROR: not a valid type: '"+type+"'"
        structures["nodes"].append({
            "name": name,
            "x": x,
            "y": y,
            "type": type
        })
        return None
    except:
        return "ERROR: format invalid"

def parseLink(instruction:str):
    try:
        global names
        global structures
        [name1,name2] = instruction.split("link ")[1].strip().split(" and ")
        if not isName(name1) or not isName(name2) or name1==name2:
            return "ERROR: not valid nodes: '"+name1+"', '"+name2+"'"

        structures["links"].append({
            "nodo1":name1,
            "nodo2":name2,
            "type1":typeLookup(name1),
            "type2":typeLookup(name2)
        })
        return None
    except:
        return "ERROR: format invalid"

def parseRectangle(instruction:str):
    try:
        global structures
        [first,second] = instruction.strip().split("and")
        [x1,y1] = [int(i) for i in first.strip().split("between ")[1].split(" ")]
        [coords2,color] = second.split(" with color ")
        [x2,y2] = [int(i) for i in coords2.strip().split(" ")]
        [r,g,b] = [int(i) for i in color.strip().split(" ")]
        structures["rectangles"].append({
            "x1":x1,
            "y1":y1,
            "x2":x2,
            "y2":y2,
            "r":r,
            "g":g,
            "b":b
        })
        return None
    except:
        return "ERROR: format invalid"

def parseCommand(instruction:str):
    try:
        global commands
        global structures
        [to,command] = instruction.strip().split("to ")[1].strip().split(" ",1)
        if not isName(to):
            return "ERROR: not a valid name: '"+to+"'"
        else:
            commands.append({"to":to,"command":command})
    except:
        return "ERROR: format invalid"

def parseFor(lines:[],i:int):
    global variables
    # cerco il done corrispondente a questo do
    j = i + 2
    docount = 1
    while j < len(lines):
        if lines[j].startswith("done"):
            docount-=1
        elif lines[j].startswith("do"):
            docount += 1
        if docount == 0:
            break
        j+=1
    # il loop va da i a j
    # ora dobbiamo identificare il nome della variabile
    [_,varname,_,type] = [boh.strip() for boh in lines[i].split(" ",3)]
    if not varname.startswith("var"):
        return "ERROR: variable names must start with 'var'"
    else:
        variables.append(varname)
    
    # espansione con nomi reali
    if type in ["switch","router","host","external interface","external natted interface","internet"]:
        # 1. ottengo tutti i nodi applicabili
        names = getAll(type)
        # 2. per ogni nodo faccio l'exec del contenuto con il nome rimpiazzato
        for name in names:
            tmpLines = []
            for k in range(i+2,j):
                tmpLines.append(lines[k].replace(varname,name))
            parse(tmpLines)
    else:
        [start,end,_,step] = [q.strip() for q in type.split(" ",3)]
        [start,end,step] = [int(i) for i in [start,end,step]]
        names = [str(q) for q in range(start,end,step)]
        print(names)
        for name in names:
            tmpLines = []
            for k in range(i+2,j):
                tmpLines.append(lines[k].replace(varname,name))
            print(tmpLines)
            parse(tmpLines)
            #print("done "+name)
            #if name=="89":
            #    print(structures["nodes"])

    return None,j

def parse(lines:[]):
    global structures
    global commands
    for i in range(len(lines)):
        #print(line)
        if lines[i] == "":
            #print("found an empty line")
            continue
        if lines[i].startswith("//"):
            #print("found a comment")
            continue
        if lines[i].startswith("create "):
            #print("found a create")
            err = parseCreate(lines[i])
        elif lines[i].startswith("link"):
            #print("found a link")
            #print(lines[i])
            err = parseLink(lines[i])
        elif lines[i].startswith("draw rectangle between"):
            err = parseRectangle(lines[i])
        elif lines[i].startswith("send command to"):
            err = parseCommand(lines[i])
        elif lines[i].startswith("add text"):
            err = parseAddText(lines[i])
        elif lines[i].startswith("for"):
            #print("for detected")
            err,i = parseFor(lines,i);
        elif lines[i].startswith("do"):
            continue
        elif err:
            print(err)
            sys.exit(1)
        else:
            print("ERROR: command unsupported at this time")
            sys.exit(1)
    #print(structures)
    #print(commands)

if __name__ == "__main__":



    directory = os.fsencode("./DoneScript/sources/")
    for file in os.listdir(directory):
        variables = []
        structures = {
                "links" :  [],
                "nodes" : [],
                "rectangles": [],
                "texts":[]
            }
        commands = []
        name = os.fsdecode(file)
        print(name)
        with open("./DoneScript/sources/"+name,"r") as program:
            lines = [i.strip().lower() for i in program.readlines()]
            parse(lines)
            with open("./DoneScript/compiled/"+name[:-3]+".done","w") as f:
                f.write(serialize())
            with open("./DoneScript/compiled/"+name[:-3]+".done.conf","w") as f:
                if commands:
                    f.write(serializeCommands())    



