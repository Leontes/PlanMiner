#!/usr/bin/python

import sys, re, random

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


def unTokenize(line):
    string = "( "
    for elem1 in line:
        if isinstance(elem1, list):
            string += "( "
            for elem2 in elem1:
                if not isinstance(elem2, list):
                    string += elem2 + " "
                else:
                    string += "( "
                    for elem3 in elem2:
                        string += elem3 + " "
                    string += ") "
            string += ") "
    string += ")"
    return string



if __name__ == '__main__':
    ptsName =  str(sys.argv[1])
    if len(sys.argv) > 2:
        thr = float(sys.argv[2])
    else:
        thr = 0.05
    pts = open(ptsName, 'r')

    for line in pts:
        if re.match('^S', line) is not None:
            sys.stdout.write(line)
            try:
                line = pts.next()
                if random.random() < thr:
                    if random.random() < thr:
                        pass
                    else:
                        if line != "\n":
                            line = parse(tokenize(line))
                            for i in range(len(line)-1):
                                index = random.randint(0, len(line)-1)
                                lista = line[index]
                                if lista[0] == "not":
                                    line[index] = lista[1]
                                elif lista[0] == "=":
                                    num = int(lista[2])
                                    if random.random() < 0.5:
                                        num = num + num*0.05
                                    else:
                                        num = num - num*0.05

                                    lista[2] = str( int(num) )
                                else:
                                    line[index] = ["not", lista]

                        sys.stdout.write(unTokenize(line))
                else:
                    sys.stdout.write(line)
            except StopIteration:
                pass
        else:
            sys.stdout.write(line)


    pts.close()
