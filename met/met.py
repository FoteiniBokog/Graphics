#Bokogianni Foteini 4438 cse84438
#Andreou Effrosyni 4315 cse84315
import sys

arithsymbols={'+','-','*','/','='}#aritmetic symbols
separators={';',','}
groups=['[',']','{','}','(',')']
stop=['.']#group separators
INTEGER=['0','1','2','3','4','5','6','7','8','9','10']
allsymbols=["=",'+','-','*','/','.','[',']','}','(',')','#','>',':','<']
otherKeywords=['if','else','while','swithcase','forcase','incase','return','call','print','input','']
rel_operators=["<",">","<=",">=","=","<>"]
num_operators=["*","/","+","-"]

#GLOBAL VARIABLES
curLine = 1
f=""#fileName
quadList=[]
labelNum=1
temps=0
localVar=[]
fileName=""
functionExists=False

scopesList=[]
nestingLevel=-1
symbolFile=''
allFunc_Proc=[]
unfinishedCall=[]
currentFunc=[]
#token=''
name=''
mainName=''

###################################################
class recordVariable():
    #objects of variables
    name=""
    isglobal=-1
    offset=-1
    def __init__(self,name,isglobal,offset):
        self.name=name
        self.offset=offset
        self.isglobal=isglobal

    def __repr__(self):
        return ("Name: "+self.name+", isglobal: "+str(self.isglobal)+", Offset: "+str(self.offset)+"\n")

class recordFunction():
    #object of functions/parameters
    name=""
    type=-1
    startQuad=-1
    argList=[]
    framelength=-1

    def __init__(self,name,type,startQuad):
        self.name=name
        self.type=type
        self.startQuad=startQuad
        self.argList=[]
        self.framelength=12

    def __repr__(self):
        arrayString="["
        size=len(self.argList)
        for i in range(size-1):
            arrayString=arrayString+str(self.argList[i].parMode)+", "
        if(size>0):
            arrayString=arrayString+str(self.argList[-1].parMode)
        arrayString=arrayString+"]"
        return ("Name: "+self.name+", Type: "+str(self.type)+", StartQuad: "+str(self.startQuad)+", ArgList: "+arrayString+", framelength: "+str(self.framelength)+"\n")
        
class recordParam():
    #object of parameters
    name=""
    parMode=-1
    offset=-1
    def __init__(self,name,parMode,offset):
        self.name=name
        self.parMode=parMode
        self.offset=offset

    def __repr__(self):
        return ("Name: "+self.name+", parMode: "+str(self.parMode)+", Offset: "+str(self.offset)+"\n")


class recordTempVar():
    #object of temporary variables
    name=""
    offset=-1
    def __init__(self,name,offset):
        self.name=name
        self.offset=offset

    def __repr__(self):
        return ("Name: "+self.name+", Offset: "+str(self.offset)+"\n")
        
class recordScope():
    #object of scope
    entityList=[]
    level=-1
    framelength=-1
    def __init__(self):
        self.entityList=[]
        self.level=nestingLevel
        self.framelength=8

    def __repr__(self):
        arrayString="["
        size=len(self.entityList)
        for i in range(size-1):
            arrayString=arrayString+str(self.entityList[i].name)+", "
        if(size>0):
            arrayString=arrayString+str(self.entityList[-1].name)
        arrayString=arrayString+"]"
        return ("EntityList: "+arrayString+", Level: "+str(self.level)+", Framelength: "+str(self.framelength)+"\n")

class recordArg():
    #object of argument
    parMode=-1
    def __init__(self,parMode):
        self.parMode=parMode



 
def lex():
    global file
    global curLine
    global flag,idToken
    idToken=False
    while char := file.read(1).decode("ASCII"): #read each letter of the file
        value="" #return value for the fuction
        
        #case1 Digit character found so keep reading
        if(char>='0' and char<='9'):
            value+=char #append the char in the empty string
            while((char:=file.read(1).decode("ASCII"))>= '0' and char <= '9'):
                value+=char #append the char in the string
            file.seek(-1,1)
            if(char>= 'a' and char <= 'z') or (char >= 'A' and char <= 'Z'):
                error("ERROR Digit was expected")
            # Digits must be in range to be accepted
            if(int(value)>-(2**32-1) and int(value)<(2**32-1)):
                return value
            else:
                print("ERROR:Number out of range.")
            #token=value
            return value

        #case2 While reading letters keep reading
        if(char >= 'a' and char <= 'z') or (char >= 'A' and char <= 'Z'):
            value+=char
            while((char:=file.read(1).decode("ASCII"))>= 'a' and char <= 'z') or (char >= 'A' and char <= 'Z') or (char>='0' and char<='9'):
                value+=char
                #print("The word is" +char)
            file.seek(-1,1)
            if (len(value)>30):
                error("ERROR:The length of the word can not be longer than 30 characters")
            #print("come on" +char)
            idToken=True
            #token=value
            return value


        #case5 for aritmetic symbols
        if char in arithsymbols:
            
           return char

        #case6
        if char in separators:
            #print("eimai edv" +char)
            return char

        #case7
        if char in groups:
            #print("eimai svsta edv" +token)
            return char 
        #case8
        if(char=='.'):
            return char
        
       #case9:Comments(read until '#' is found.Otherwise there is an error)
        if (char=='#'):
            char=file.read(1).decode("ASCII")
            while (char!='#'):
                char=file.read(1).decode("ASCII")
                if(char =='\n'):
                    curLine = curLine + 1
                if(char == ''):
                    error("Comments must be closed")
            char=file.read(1).decode("ASCII")
            

        #case10     
        if char==':':
            if(file.read(1).decode("ASCII")=='='):
                return ':='
            else:
                return ':'
        #case11 >
        if char== '>':
            char=file.read(1).decode("ASCII")
            if(char=='='):
                return '>='
            else:
                file.seek(-1,1)
                return '>'
        #case12 <
        if char== '<':
            char=file.read(1).decode("ASCII")
            if(char == '='):
                return '<='
            elif(char== '>'):
                #f.seek(-1,1)
                return '<>'
            else:
                #f.seek(-1,1)
                return '<'
            

def condition():
    global token
    global curLine
    Q1=boolterm()
    B=Q1
    while True:
        if(token=="or"):
            backPatch(B[0],nextQuad())
            Q2=boolterm() 
            B[1]=merge(B[1],Q2[1])
            B[0]=Q2[0]
        else:
            return B
        
def boolterm():
    global token
    global curLine
    R1=boolfactor()
    Q=R1
    while True:
        if(token=='and'):
            backPatch(Q[1],nextQuad())
            R2=boolfactor()
            Q[0]=merge(Q[0],R2[0])
            Q[1]=R2[1]
        else:
            return Q

def boolfactor():
    global token
    global curLine
    global token
    #Factor in boolean expression
    R=['','']
    token=lex()
    if(token== 'not'):
        token=lex()
        if(token=='['):
            B=condition()
            R[1]=B[0]
            R[0]=B[1]
            if(token==']'):
                pass
            else:
                error('] exprected')
        else:
            error('[ exprected')
    elif(token=='['):
        B=condition()
        R=B
        if(token==']'):
            token=lex()
        else:
            error("']' expected")
    else:
        E1=expression()
        relop=REL_OP()
        token=lex()
        E2=expression()
        R[1]=makelist(nextQuad())
        genQuad(relop,E1,E2,'_')
        R[0]=makelist(nextQuad())
        genQuad('jump','_','_','_')
    return R 
              
def actualparitem(tempCallList):
    global token
    global curLine
    if(token== 'in'):
        token=lex()
        a=expression()
        genQuad('par',a,'cv','_')
        tempCallList.append(('par',a,'CV','_'))
    else:
        if(token== 'inout'):
            b=token
            token=lex()
            tempCallList.append(('par',b,'REF','_'))
            genQuad('par',b,'ref','_')
        else:
            error("ILLEGAL ARGUMENT:A correct type of string expected")
       
def actualparlist():
    global token
    global curLine
    #List of actual parametres
    tempCallList=[]
    token=lex()
    #A list of variables following the declaration keyword
    if(token=='in' or token=='inout'):
        actualparitem(tempCallList)
    while True:
        if(token==","):
            token=lex()
            if(token=='in' or token=='inout'):
                actualparitem(tempCallList)
            else:
                error("SYNTAX ERROR:word 'in' or 'inout'expected")
        else:
            break
    for i in tempCallList:
        genQuad(i[0],i[1],i[2],i[3])
                    

def idtail():
    global token
    global curLine
    result=0
    if(token=='('):
        token=lex()
        actualparlist()
        result=1
        #token=lex()
        if(token!=')'):
            token=lex()
        else:
            error("SYNTAX ERROR:')' exprected")
    return result   
    if (token!="("):
        print("thelw )@@@@" +token)

def factor():
    global token
    global curLine
    global A,B
    if token in INTEGER:
        result=token
        return result
    elif(token=='('):
        token=lex()
        result=expression()
        token=lex()
        if(token==')'):
            token=lex()
        
        else:
            print("SYNTAX ERROR:')' exprected") 
    else:
        A=token
        result=A
        #print("FACTOR"+token)
        tail=idtail()#I f tail=1 then it exists
        return token
        if(tail==1):
            addRecordVar(target)
          
def term():
    global token
    global curLine
    global A,B
    A=factor()
    target=A
    token=lex()
    while True:
        if(token=='*' or token=='/'):
            op=MUL_OP()
            token=lex()
            B=factor()
            token=lex()
            target=newTemp()
            addRecordTempVar(target)
            #print(A)
            #print(B)
            genQuad(op,A,B,target)
            A=target
           
        else:
            return target
    return target
        
        
def optionalSign():
    #Symbols '+' and '-' are optional
    global token
    global curLine
    global token
    if(token =='+' or token=='-'):
        op=ADD_OP()
        token=lex()
        return op
    else:
        return ''   
       
def ADD_OP():
    if(token=='+'):
        return '+'
    else:
        return '-'

def MUL_OP():
    #Multiplication and division operators
    if(token=='*'):
        return '*'
    else:
        return '/'

def REL_OP():
    global token
    global curLine
    global token
    #lexer rules : relational , arithmetic operations , integers and ids
    #op=token
    if(token=='='):
        return token
    elif(token=='<='):
        return token
    elif(token=='>='):
        return token
    elif(token=='>'):
        return token
    elif(token=='<'):
        return token
    elif(token=='<>'):
        return token
    else:
        #error("ILLEGAL RELATIONAL OPERAND")
        
        return 
        
        
def expression():
    global token
    global curLine
    #global token
    opSign=optionalSign()
    T1=token
    T=term()
    target=T
    if(opSign=='-'):
        target=newTemp()
        genQuad('-',0,T,target)
        T=target
        
    
    while (token =='+' or token=='-'):
            op=ADD_OP()
            token=lex()
            B=term()
            target=newTemp()
            addRecordTempVar(target)
            genQuad(op,T,B,target)
            T=target
            T1=target    
    return T1
                       
def assignStat():
    global token
    global curLine
    #print("assignStat"+token)
    if(token==':='):
        token=lex()
        id=expression()
        genQuad(':=',id,'_',E)
    else:
        error("SYNTAX ERROR:':=' expected")

def ifStat():
    global token
    global curLine
    global token
    if(token=='('):
        B=condition()
        if (token==')'):
            token=lex()
            backPatch(B[1],nextQuad())
            statements()
            ifList=makelist(nextQuad())
            genQuad('jump','_','_','_')

            backPatch(B[0],nextQuad())
            elsepart()
            backPatch(ifList,nextQuad())
        else:
            error("SYNTAX ERROR:')' expected")
    else:
        error("SYNTAX ERROR:'(' expected")

def elsepart():
    global token
    global curLine
    global token
    if(token=='else'):
        token=lex()
        statements()

def whileStat():
    global token
    global curLine
    global token
    if(token=='('):
        Bquad=nextQuad()
        B=condition()
        #token=lex()
        if (token==')'):
            token=lex()
            backPatch(B[1],nextQuad())
            statements()
            genQuad('jump','_','_',Bquad)
            backPatch(B[0],nextQuad())
        else:
            error("error ')' expected")
    else:
        error("error '(' expected")
        
def switchcaseStat():
    global token
    global curLine
    #switch statement
    exitlist=emptyList()
    while True:
        if(token=='case'):
            token=lex()
            if(token=='('):
                #token=lex()
                B=condition()
                if (token==')'):
                    token=lex()
                    backPatch(B[1],nextQuad())
                    statements()
                    e=makelist(nextQuad())
                    genQuad('jump','_','_','_')
                    merge(exitlist,e)
                    backPatch(B[0],nextQuad())
                else:
                   error("SYNTAX ERROR: ')' expected")
            else:
                error("SYNTAX ERROR: '(' expected")
        else:
            break
        if (token=='default'):
            token=lex()
            statements()
            backPatch(exitlist,nextQuad())
        else:
            error(" SYNTAX ERROR: Keyword 'default' expected")
            
def forcaseStat():
    global token
    global curLine
    sQuad=nextQuad()
    while True:
        if(token=='case'):
            token=lex()
            if(token=='('):
                B=condition()
                if (token==')'):
                    token=lex()
                    backPatch(B[1],nextQuad())
                    statements()
                    genQuad('jump','_','_',sQuad)
                    backPatch(B[0],nextQuad())
                else:
                    error("SYNTAX ERROR: ')' expected")
            else:
                error("SYNTAX ERROR:'(' expected")
        else:
            break
        if(token=='default'):
            token=lex()
            statements()
        else:
            error("SYNTAX ERROR:Keyword'default' expected")

def incaseStat():
    global token
    global curLine
    flag=newTemp()
    addRecordTempVar(flag)
    sQuad=nextQuad()
    genQuad(':=',0,'_',flag)#mporei antitheta 1,0
    while True:
        if(token=='case'):
            token=lex()
            if(token=='('):
                B=condition()
                if (token==')'):
                    token=lex()
                    backPatch(B[1],nextQuad())#flag=1 so we loop
                    genQuad(':=',1,'_',flag)
                    statements()
                    backPatch(B[0],nextQuad())
                else:
                    error("SYNTAX ERROR:')' expected")
            else:
                error("SYNTAX ERROR:'(' expected")
        else:
            genQuad('=',flag,1,sQuad)
            break

def returnStat():
    global token
    global curLine
    if(token=='('):
        token=lex()
        Ε=expression()
        if(token==')'):
           token=lex()
           genQuad('retv',E,'_','_')
        else:
            error("SYNTAX ERROR: ')' expected")
    else:
        error("SYNTAX ERROR: '(' expected")

def callStat():
    global token
    global curLine
    id=token
    token=lex()
    if(token=='('):
        actualparlist()
        token=lex()
        if(token==')'):
            token=lex()
            genQuad('call',id,'_','_')
            return
        else:
            error("SYNTAX ERROR:';' expected")
    else:
        error("SYNTAX ERROR:'(' expected")
   
def printStat():
    global token
    global curLine
    if(token=='('):
        token=lex()
        E=expression()
        if(token==')'):
            token=lex()
            genQuad('out',E,'_','_')
        else:
            error("SYNTAX ERROR: ')' expected")
    else:
        error("SYNTAX ERROR:'(' expected")
    
def inputStat():
    global token
    global curLine
    global token
    if(token=='('):
        token=lex()
        id=token
        token=lex()
        genQuad('inp',id,'_','_')
        if(token==')'):
            token=lex()
        else:
            error("SYNTAX ERROR: ')' expected")
        
    else:
        error("SYNTAX ERROR: '(' expected")
            


def formalparitem():
    #A formal parameter (" in ": by value , " inout ": by reference )
    global token
    global curLine
    if(token== 'in'):
        token=lex()
        if(token not in allsymbols):
            addRecordParam(token,0)
            addArg(0)
            token=lex()
        else:
            error("ILLEGAL ARGUMENT: Expected name of a variable")
    elif(token== 'inout'):
        token=lex()
        if(token not in allsymbols):
            addRecordParam(token,1)
            addArg(1)
            token=lex()
        else:
            error("ILLEGAL ARGUMENT: Expected name of a variable")
        
    else:
        error("SYNTAX ERROR: 'in' or 'inout' expected")


def formalparlist():
    global token
    global curLine
    global token
    if(token!=')'):
        formalparitem()
        while True:
            if(token==','):
                token=lex()
                formalparitem()
            else:
                break
          
def statement():
    global token
    global curLine
    global token
    global E
    if token not in otherKeywords and idToken==True:
        E=token
        token=lex()
        assignStat()
    elif(token=='if'):
        token=lex()
        ifStat()
    elif(token=='else'):
        token=lex()
        elsepart()
    elif(token=='while'):
        token=lex()
        whileStat()
    elif(token=='switchcase'):
        token=lex()
        switchcaseState()
    elif(token=='forcase'):
        token=lex()
        forcaseStat()
    elif(token=='incase'):
        token=lex()
        incaseStat()
    elif(token=='return'):
        #Here we make sure that one functions has at least one return statement
        if(allFunc_Proc!=[]):
            if (allFunc_Proc[-1][0]==1):
                error("Semantic error: Procedures must not have a return statement:")
            elif(allFunc_Proc[-1][1]==0):
                allFunc_Proc[-1][1]=1
            elif(allFunc_Proc[-1][1]==1):
                pass
            else:
                error("Syntax error: <return> statement out of place")
        else:
            error("Semantic error: No functions are declared for <return> statement to exist")
        token=lex()
        returnStat()
    elif(token=='call'):
        token=lex()
        callStat()
    elif(token=='print'):
        token=lex()
        printStat()
    elif(token=='input'):
        token=lex()
        inputStat()
    else:
        pass
    
        

def statements():
    global token
    global curLine
    global token
    if (token=='{'):
        token=lex()
        statement()
        while True:
            if(token==';' ):
                token=lex()
                if(token=="}" ):
                    token=lex()
                    if(token==';'):
                        token=lex()
                        statement()
                statement() 
            
                
            else:
                return ''
    else:
        statement()
        if (token==";"):
            token=lex()
        else:
            error("SYNTAX ERROR:';' expected")


def blockstatements():
    global token
    global curLine
    global token
    statement()
    while(token==';'):
        token=lex()
        statement()
      
def subprogram(func):
    global token
    global curLine
    global token
    id=token
    token=lex()
    if (token== '('):
        token=lex()
    else:
        error('SYNTAX ERROR:"(" expected')
    addRecordFunc(id,func)
    formalparlist()
    #Block()
    if (token== ')'):
        token=lex()
        statements()
        genQuad("halt","_","_","_")
        genQuad("end_block",name,"_","_")
        print(len(allFunc_Proc))
        if(allFunc_Proc[-1][0]==0):
            if(allFunc_Proc[-1][1]==0):
                print("Semantic error: Function  must have a return statement!")
        allFunc_Proc.pop()
        writeAllScopes()
        deleteScope()
    else:
        error ('SYNTAX ERROR:")" expected')
    
def subprograms():
    global token
    global curLine
    global token
    global functionExists
    if(token=='function'):
        functionExists=True
        token=lex()
        name=token
        genQuad("begin_block",token,"_","_")
        subprogram(0)
        genQuad("end_block",name,"_","_")
    elif(token=='procedure'):
        functionExists=True
        token=lex()
        name=token
        genQuad("begin_block",token,"_","_")
        subprogram(1)
        genQuad("end_block",name,"_","_")
    else:
        #None of keywords function or procedure was found
        blockstatements()


def varlist():
    global token
    global curLine
    addRecordVar(token)
    localVar.append(token)
    token=lex()
    while True:
        while(token==","):
            token=lex()
            addRecordVar(token)
            localVar.append(token)
            token=lex()
        if(token== ';'):
            return
        else:
            error("SYNTAX ERROR: ';' expected")        

def declarations():
    global token
    global curLine
    global token
    if(token== 'declare'):
        while(token== 'declare'):
            token=lex() #pare id 
            varlist()
            token=lex()
    else:
        error("SYNTAX ERROR: Keyword 'declare' was expected")


def Block():
    global token
    global curLine
    global token
    token=lex()
    if( token=='{'):
        token=lex()
        addScope()
        mainName=name
        declarations()
        subprograms()
        #genQuad("begin_block",id,"_","_")
    else:
        error('ERROR:"{" exprected')

def program():
    global token
    global curLine
    global token
    global labelNum
    token=lex()
    if (token=="program"):
        token=lex()
        name=token
        genQuad("begin_block",token,"_","_")
        
        Block()
        [x,y]=searchEntity(name)
        scopesList[x].entityList[y].startQuad=labelNum-1
        genQuad("end_block",name,"_","_")
        
        token=lex()
        if(token=='.'):
            print("Compiler has succesfully finished!!!")
            
            writeAllScopes()
            deleteScope()
            
    else:
         error ("SYNTAX ERROR: Κeyword ‘program’ was expected")


#Point to the next quad
def nextQuad():
    global labelNum
    #labelNum+=1
    return labelNum

#Function that generates the next quad
def genQuad(op,x,y,z):
    global labelNum
    global quadList
    opList=[]
    opList.append(labelNum)
    opList.append(op)
    opList.append(x)
    opList.append(y)
    opList.append(z)
    quadList.append(opList)
    labelNum=labelNum+1
    
#Function taht makes a new temp variable
def newTemp():
    global temps
    temps+=1
    localVar.append('T_'+ str(temps-1))
    return 'T_'+str(temps-1)

#Function taht returns an empty list
def emptyList():
    return ["_","_","_","_"]

#Function that creates a list
def makelist(x):
    return[x]

#Function merges list1 with list2
def merge(list1,list2):
    #list1.extend(list2)
    return list(set(list1)|set(list2))

#Function that fills in data
def backPatch(list,z):
    global quadList
    for i in quadList:
        if i[0] in list:
            i[4] = str(z)

def writeC():
    global f
    #Opens the C file 
    file = open(f[:-3]+".c", "w")
    file.write("#include <stdio.h>\n\n")
    file.write("int main(){\n")
    writeDecl(file)
   
    makeCquad(file)

    file.write("}")
    file.close

def makeCquad(file):
    #Writes on the C file depending on the quad
    for i in quadList:
        file.write("\tL_"+str(i[0])+": ")
        if(i[1]== "jump"):
            file.write("goto L_"+str(i[4])+";")
        elif(i[1] in rel_operators):
            if(i[1]=="<>"):
                file.write("if("+i[2]+" != "+i[3]+") goto L_"+str(i[4])+";")
            elif(i[1]=="="):
                file.write("if("+i[2]+" == "+i[3]+") goto L_"+str(i[4])+";")
            else:
                file.write("if("+i[2]+" "+i[1]+" "+i[3]+") goto L_"+str(i[4])+";")
        elif(i[1]== ":="):
            file.write(str(i[4])+ " = "+i[2]+";")
        elif(i[1] in num_operators):
            file.write(str(i[4])+" = "+i[2]+''+i[1]+''+i[3]+";")
        elif(i[1]== "out"):
            file.write('printf("Output'+i[2]+'= %d\\n", '+i[2]+");")
        elif(i[1]== "retv"):
            out="return(",i[2],");"
        elif(i[1]== "inp"):
            file.write('printf("Enter variable input for:'+i[2]+ '\\n"); scanf("%d", &'+i[2]+");")
        elif(i[1]== "halt"):
            file.write("{}")
        elif(i[1]== "end_block"):
            file.write("; //"+ i[2]+" end")
        elif(i[1]== "begin_block"):
            file.write("; //"+ i[2]+" start")
        file.write(" // ("+ i[1]+", "+i[2]+", "+ i[3]+", "+ str(i[4])+")\n")

def writeDecl(file):
    #writes the declarations of the C-file
    global localVar
    #localVar=list(dict.fromkeys(localVar))
    file.write("int ")
    if(localVar):
        file.write(localVar[0])
        for i in range(1,len(localVar)):
            file.write(" ,"+localVar[i])
        file.write(";\n")

#Adds a scope to the scopesList and increments the nesting level
def addScope():
    global scopesList
    global nestingLevel
    nestingLevel=nestingLevel+1
    #print("Nesting level is:"+str(nestingLevel))
    scopesList.append(recordScope())

#Deletes a scope frome the scopesList and decreses the nesting level
def deleteScope():
    global scopesList
    global nestingLevel
    # Ti einai ayto
    if(len(scopesList)>1):
        scopesList[-2].entityList[-1].framelength=scopesList[-1].framelength+4
    scopesList.pop()
    nestingLevel=nestingLevel-1

#Adds arguments to the last function or procedure that was called(0->in,1->inout)
def addArg(parMode):
    argument=recordArg(parMode)
    scopesList[nestingLevel-1].entityList[-1].argList.append(argument)

#Creates a recordVariables item and adds it to the entityList of the according scope 
def addRecordVar(name):
    global scopesList
    for i in reversed(scopesList[nestingLevel].entityList):
        if(i.name==name):
            if(hasattr(i,"argList")):
                #if(i.type==type):
                if(type==0):
                    error("Semantic error: Function <"+name+"> already exists")
                elif(type==1):
                    error("Semantic error: Procedure <"+name+"> already exists ")
            elif(hasattr(i,"isglobal")):
                error("Semantic error: Variable <"+name+"> already declared")
            elif(hasattr(i,"parMode")):
                error("Semantic error: Variable <"+name+"> already declared as argument")
    scopesList[nestingLevel].framelength=scopesList[nestingLevel].framelength+4
    tempOffset=scopesList[nestingLevel].framelength

    
    variable=recordVariable(name,nestingLevel,tempOffset)
    (scopesList[nestingLevel].entityList).append(variable)
    
#Creates a recordFunction item and adds it to the entityList of the according scope
def addRecordFunc(name,type):
    global scopesList
    global allFunc_Proc

    if(name==mainName):
        error("Semantic error: Function/Procedure <"+name+"> can't have the same name as the main program ")
    for i in reversed(scopesList[nestingLevel].entityList):
        if(i.name==name):

            if(hasattr(i,"argList")):
                #if(i.type==type):
                if(type==0):
                    error("Semantic error: Function <"+name+"> already exists")
                elif(type==1):
                    error("Semantic error: Procedure <"+name+"> already exists")
            elif(hasattr(i,"isglobal")):
                error("Semantic error: Variable <"+name+"> already declared")
            elif(hasattr(i,"parMode")):
                error("Semantic error: Variable <"+name+"> already declared as argument")
    allFunc_Proc.append([type,0])
    function=recordFunction(name,type,labelNum+1)
    (scopesList[nestingLevel].entityList).append(function)
    addScope()
    
#Creates a recordParam item and adds it to the entityList of the according scope
def addRecordParam(name,parMode):
    global scopesList

    for i in reversed(scopesList[nestingLevel].entityList):
        if(i.name==name):
            if(hasattr(i,"parMode")):
                error("Semantic error: Argument <"+name+"> already declared")
    scopesList[nestingLevel].framelength=scopesList[nestingLevel].framelength+4
    tempOffset=scopesList[nestingLevel].framelength
    #parMode=0 for value IN
    #parMode=1 for reference INOUT
    parameter=recordParam(name,parMode,tempOffset)
    (scopesList[nestingLevel].entityList).append(parameter)

#Creates a recordTempVar item and adds it to the entityList of the according scope
def addRecordTempVar(name):
    global scopesList
    scopesList[nestingLevel].framelength=scopesList[nestingLevel].framelength+4
    tempOffset=scopesList[nestingLevel].framelength

    tempVar=recordTempVar(name,tempOffset)
    (scopesList[nestingLevel].entityList).append(tempVar)

#Write all scopes to symbolFile
def writeAllScopes():
    global unfinishedCall
    global symbolFile

    for i in range(len(scopesList)):
        symbolFile.write("----Scope "+str(i)+"----\n")
        for j in range(len(scopesList[i].entityList)):
            symbolFile.write(str(scopesList[i].entityList[j]))


#Searches for an entity with the same name the search is in reverse of the entityList and of the scopesList           
def searchEntity(name):
    global scopesList
    for i in reversed(range(len(scopesList))) :
        for j in reversed(range(len(scopesList[i].entityList))):
            if(name==scopesList[i].entityList[j].name):
                return [i,j]
    return [-1,-1]
    

def main():
    global f
    global file
    global symbolFile
    f=sys.argv[1]
    file=open(f,"rb")
    #f.seek(0,0)
    #curChar =f.read(1)
    
    symbolFile = open(f[:-3]+".symb", "w")

    print("\n\n")
    print("|--------------------------------------------------------------------|")
    print("|                         ___Compiling...___                         |")
    print("|--------------------------------------------------------------------|")
    program()
    
    file = open(f[:-3]+".int", "w")
    print("Creating int-file...\n")
    print(quadList)
    for i in quadList:
        file.write(str(i[0])+", "+ str(i[1])+", "+str(i[2])+", "+ str(i[3])+", "+ str(i[4])+'\n')
        #print(i)
    file.close()

    if(not functionExists):
        print("Creating C-file...\n")
        writeC()


main()
