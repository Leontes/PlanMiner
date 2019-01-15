#!/usr/bin/python

import sys
from copy import *

def tokenize(chars):
    "Convert a string of characters into a list of tokens."
    charsAux = ""
    for line in chars:
        if not line.startswith(';'):
            charsAux += line
    return charsAux.replace('(', ' ( ').replace(')', ' ) ').split()

def parse(tokens):
    "Read an expression from a sequence of tokens."
    if len(tokens) == 0:
        raise SyntaxError('unexpected EOF while reading')
    token = tokens.pop(0)
    if '(' == token:
        L = []
        while tokens[0] != ')':
            L.append(parse(tokens))
        tokens.pop(0) # pop off ')'
        return L
    elif ')' == token:
        raise SyntaxError('unexpected )')
    else:
        return token


class action(object):
    name = ""
    parameters = []
    types = []
    preds = []
    effects = []

    def __init__(self, name, parameters, preds, effects):
        super(action, self).__init__()

        self.name = name
        self.parameters = [parameters[i].upper() for i in range(0, len(parameters),3)]
        self.types = [parameters[i].upper() for i in range(2, len(parameters),3)]

        if len(preds) > 0:
            if preds[0] == "and":
                self.preds = preds[1:]
            else:
                self.preds = [preds]
        else:
            self.preds = []


        if len(effects) > 0:
            if effects[0] == "and":
                self.effects = effects[1:]
            else:
                self.effects = [effects]
        else:
            self.effects = []


    def apply(self, state, params, predsMap):

        preds = deepcopy(state.predicates)

        for i in range(len(preds)):
            if preds[i][0] == 'not' or preds[i][0] == '=':
                for j in range(len(preds[i][1])):
                    preds[i][1][j] = preds[i][1][j].upper()
            else:
                for j in range(len(preds[i])):
                    preds[i][j] = preds[i][j].upper()



        eff = deepcopy(self.effects)

        #Instanciamos los Efectos
        for i in range(len(self.parameters)):
            for j in range(len(self.effects)):
                if self.effects[j][0] == 'not':
                    eff[j][1][0] = eff[j][1][0].upper()
                    for k in range(1, len(self.effects[j][1])):
                        if((self.effects[j][1][k]).upper() == (self.parameters[i]).upper()):
                            eff[j][1][k] = params[i].upper()
                else:
                    if self.effects[j][0] == 'increase' or self.effects[j][0] == 'decrease':
                        eff[j][1][0] = eff[j][1][0].upper()
                        for k in range(1, len(self.effects[j][1])):
                            if((self.effects[j][1][k]).upper() == (self.parameters[i]).upper()):
                                eff[j][1][k] = params[i].upper()

                    else:
                        if len(self.effects[j]) == 1:
                            eff[j][0] = eff[j][0].upper()
                        else:
                            for k in range(1, len(self.effects[j])):
                                if((self.effects[j][k]).upper() == (self.parameters[i]).upper()):
                                    eff[j][0] = eff[j][0].upper()
                                    eff[j][k] = params[i].upper()


        for elem in eff:
            if elem[0] == 'not':
                for pred in preds:
                    if elem[1] == pred:
                        preds.append(['not', pred])
                        preds.remove(pred)

            else:
                if elem[0] == 'increase':
                    for pred in preds:
                        if pred[0] == '=' and elem[1] == pred[1]:
                            auxP = pred
                            preds.remove(pred)
                            auxP[2] = str(int(auxP[2]) + 1)
                            preds.append(auxP)
                            break
                else:
                    if elem[0] == 'decrease':
                        for pred in preds:
                            if pred[0] == '=' and elem[1] == pred[1]:
                                auxP = pred
                                preds.remove(pred)
                                auxP[2] = str(int(auxP[2]) + 1)
                                preds.append(auxP)
                                break
                    else:
                        preds.append(elem)
                        if preds.count([['not', elem]]) > 0:
                            preds.remove([['not', elem]])


        return State(preds)

    def getPreds(self):
        preds = self.preds
        out = []

        for i in range(len(self.parameters)):
            for j in range(len(preds)):
                if preds[j][0] == "not" or preds[j][0] == "NOT":
                    prr = preds[j][1]
                    prr[0] = prr[0].upper()
                    for k in range(1, len(prr)):
                        if prr[k].upper() == (self.parameters[i]).upper():
                            prr[k] = "?param_" + str(i)
                    out.append(["NOT",prr])
                else:
                    if  preds[j][0] == ">=" or preds[j][0] == ">" or preds[j][0] == "==" or preds[j][0] == "<=" or preds[j][0] == "<":
                        if  preds[j][0] == ">=" or preds[j][0] == ">":
                            prr1 = preds[j][1]
                            prr1[0] = prr1[0].upper()
                            for k in range(1, len(prr1)):
                                if prr1[k].upper() == (self.parameters[i]).upper():
                                    prr1[k] = "?param_" + str(i)

                            prr2 = preds[j][1]
                            prr2[0] = prr2[0].upper()
                            for k in range(1, len(prr2)):
                                if prr2[k].upper() == (self.parameters[i]).upper():
                                    prr2[k] = "?param_" + str(i)

                            out.append([">",prr1, prr2])

                        if  preds[j][0] == "<=" or preds[j][0] == "<":
                            prr1 = preds[j][1]
                            prr1[0] = prr1[0].upper()
                            for k in range(1, len(prr1)):
                                if prr1[k].upper() == (self.parameters[i]).upper():
                                    prr1[k] = "?param_" + str(i)

                            prr2 = preds[j][1]
                            prr2[0] = prr2[0].upper()
                            for k in range(1, len(prr2)):
                                if prr2[k].upper() == (self.parameters[i]).upper():
                                    prr2[k] = "?param_" + str(i)

                            out.append(["<",prr1, prr2])


                        if  preds[j][0] == "==":
                            prr1 = preds[j][1]
                            prr1[0] = prr1[0].upper()
                            for k in range(1, len(prr1)):
                                if prr1[k].upper() == (self.parameters[i]).upper():
                                    prr1[k] = "?param_" + str(i)

                            prr2 = preds[j][1]
                            prr2[0] = prr2[0].upper()
                            for k in range(1, len(prr2)):
                                if prr2[k].upper() == (self.parameters[i]).upper():
                                    prr2[k] = "?param_" + str(i)

                            out.append(["==",prr1, prr2])

                    else:
                        prr = preds[j]
                        prr[0] = prr[0].upper()
                        for k in range(1, len(prr)):
                            if prr[k].upper() == (self.parameters[i]).upper():
                                prr[k] = "?param_" + str(i)
                        out.append(prr)

        return out

    def getEffs(self):
        pred = self.effects
        out = []

        for i in range(len(self.parameters)):
            for j in range(len(pred)):
                if pred[j][0] == "not" or pred[j][0] == "NOT":
                    prr = pred[j][1]
                    prr[0] = prr[0].upper()
                    for k in range(1, len(prr)):
                        if prr[k].upper() == (self.parameters[i]).upper():
                            prr[k] = "?param_" + str(i)
                    out.append(["NOT",prr])
                else:
                    if  pred[j][0] == "increase" or pred[j][0] == "decrease" or pred[j][0] == "assign":
                        prr1 = pred[j][1]
                        prr1[0] = prr1[0].upper()
                        for k in range(1, len(prr1)):
                            if prr1[k].upper() == (self.parameters[i]).upper():
                                prr1[k] = "?param_" + str(i)

                        out.append([pred[j][0].upper(), prr1])


                    else:
                        prr = pred[j]
                        prr[0] = prr[0].upper()
                        if len(prr) > 1:
                            if type(prr[1]) is list:
                                prr[1][0] = prr[1][0].upper()
                                for k in range(1, len(prr[0])):
                                    if prr[0][k].upper() == (self.parameters[i]).upper():
                                        prr[0][k] = "?param_" + str(i)
                            else:
                                for k in range(1, len(prr)):
                                    if prr[k].upper() == (self.parameters[i]).upper():
                                        prr[k] = "?param_" + str(i)
                        out.append(prr)

        return out


class State(object):
    predicates = []
    """docstring for state."""
    def __init__(self, predicates):
        super(State, self).__init__()
        self.predicates = predicates[1:]



if __name__ == '__main__':
    total = len(sys.argv)

    domainOriParsed = parse(tokenize(open(str(sys.argv[1]), 'r').readlines()))

    try:
        domainLerParsed = parse(tokenize(open(str(sys.argv[2]), 'r').readlines()))
    except IndexError as e:
        print "Dominio roto. (faltaban datos para generarlo)"
        sys.exit(0)


    actionMapOri = {}

    for token in domainOriParsed:
        if token[0] == ':action':
            ac = action(token[1], token[3], token[5], token[7])
            actionMapOri[ac.name.upper()] = ac


    actionMapLer = {}

    for token in domainLerParsed:
        if token[0] == ':action':
            ac = action(token[1], token[3], token[5], token[7])
            actionMapLer[ac.name.upper()] = ac


    errorsA = 0
    errorsB = 0

    for act in actionMapLer.keys():
        lstOr = actionMapOri[act].getPreds()
        lstLer = actionMapLer[act].getPreds()

        esta = False
        #fp
        for n in lstLer:
            esta = False
            for m in lstOr:
                if m == n:
                    esta = True
            if esta == False:
                errorsA = errorsA + 1


        esta = False
        #fn
        for n in lstOr:
            esta = False
            for m in lstLer:
                if m == n:
                    esta = True
            if esta == False:
                errorsB = errorsB + 1


    errorsA = errorsA / 2
    errorsB = errorsB / 2

    print "Error en precondiciones: " + str(errorsA + errorsB)
    print "FP: " + str(errorsA)
    print "FN: " + str(errorsB)


    errorsA = 0
    errorsB = 0

    for act in actionMapLer.keys():
        lstOr = actionMapOri[act].getEffs()
        lstLer = actionMapLer[act].getEffs()

        esta = False
        for n in lstLer:
            esta = False
            for m in lstOr:
                if m == n:
                    esta = True
            if esta == False:
                errorsA = errorsA + 1


        esta = False
        for n in lstOr:
            esta = False
            for m in lstLer:
                if m == n:
                    esta = True
            if esta == False:
                errorsB = errorsB + 1

    errorsA = errorsA / 2
    errorsB = errorsB / 2

    print "Error en efectos: " + str(errorsA + errorsB)
    print "FP: " + str(errorsA)
    print "FN: " + str(errorsB)
