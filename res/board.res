Experiment: res/board
mié jun  7 10:53:29 CEST 2017
===================================================


===============
 Ejecucion 0
===============
Aciertos: 0
Errores:  0
No clasificados: 0
	Rule 0(PreState): 0 / 0
	Rule 1(PostState): 0 / 0
Aciertos: 0
Errores:  0
No clasificados: 0
	Rule 0(PreState): 0 / 0
	Rule 1(PostState): 0 / 0
----------------------------------------------------
Aciertos Training: 1
Aciertos Test: -nan
Numero de reglas: 2
Variables por regla: 11
Variables usadas: 0.423077
Tiempo: 0.054531
Iteraciones: 0
Condiciones: 22
Aciertos Discriminantes: -nan
Aciertos Descriptivas: -nan
----------------------------------------------------
------------------- REGLAS -------------------------
----------------------------------------------------
Descriptivas: 
IF
	( at ?Param_0 - person ?Param_2 - city ) = { True}
	( slow-burn ?Param_1 - aircraft ) = { True}
	( refuel-rate ?Param_1 - aircraft ) = { True}
	( at ?Param_1 - aircraft ?Param_2 - city ) = { True}
	( at ?Param_1 - aircraft c4 - city ) = { False}
	( at ?Param_1 - aircraft c1 - city ) = { False}
	( in p1 - person ?Param_1 - aircraft ) = { False}
	( at ?Param_1 - aircraft c5 - city ) = { False}
	( at ?Param_1 - aircraft c2 - city ) = { False}
	( in p2 - person ?Param_1 - aircraft ) = { False}
	( in ?Param_0 - person ?Param_1 - aircraft ) = { True}
THEN Class IS PreState   W 1 and N+= 3 and N-= 0

IF
	( at ?Param_0 - person ?Param_2 - city ) = { False}
	( slow-burn ?Param_1 - aircraft ) = { True}
	( refuel-rate ?Param_1 - aircraft ) = { True}
	( at ?Param_1 - aircraft ?Param_2 - city ) = { True}
	( at ?Param_1 - aircraft c4 - city ) = { False}
	( at ?Param_1 - aircraft c1 - city ) = { False}
	( in p1 - person ?Param_1 - aircraft ) = { False}
	( at ?Param_1 - aircraft c5 - city ) = { False}
	( at ?Param_1 - aircraft c2 - city ) = { False}
	( in p2 - person ?Param_1 - aircraft ) = { False}
	( in ?Param_0 - person ?Param_1 - aircraft ) = { True}
THEN Class IS PostState   W 1 and N+= 3 and N-= 0

Predictivas: 
IF
	( at ?Param_0 - person ?Param_2 - city ) = { True}
THEN Class IS PreState   W 1 and N+= 3 and N-= 0

IF
	( at ?Param_0 - person ?Param_2 - city ) = { False}
THEN Class IS PostState   W 1 and N+= 3 and N-= 0

Patrones: 
011111111111111?111?1?1111?111?1?1?111?1??1(0)
101111111111111?111?1?1111?111?1?1?111?1??1(1)

===============
 Ejecucion 1
===============
Aciertos: 1
Errores:  0
No clasificados: 0
	Rule 0(PreState): 0 / 0
	Rule 1(PostState): 1 / 0
Aciertos: 0
Errores:  0
No clasificados: 1
	Rule 0(PreState): 0 / 0
	Rule 1(PostState): 0 / 0
----------------------------------------------------
Aciertos Training: 1
Aciertos Test: 0
Numero de reglas: 2
Variables por regla: 12
Variables usadas: 0.5
Tiempo: 0.055448
Iteraciones: 0
Condiciones: 24
Aciertos Discriminantes: 1
Aciertos Descriptivas: 1
----------------------------------------------------
------------------- REGLAS -------------------------
----------------------------------------------------
Descriptivas: 
IF
	( at ?Param_0 - person ?Param_2 - city ) = { True}
	( slow-burn ?Param_1 - aircraft ) = { True}
	( refuel-rate ?Param_1 - aircraft ) = { True}
	( at ?Param_1 - aircraft ?Param_2 - city ) = { True}
	( at ?Param_1 - aircraft c4 - city ) = { False}
	( at ?Param_1 - aircraft c1 - city ) = { False}
	( in p1 - person ?Param_1 - aircraft ) = { False}
	( at ?Param_1 - aircraft c5 - city ) = { False}
	( at ?Param_1 - aircraft c2 - city ) = { False}
	( in p2 - person ?Param_1 - aircraft ) = { False}
	( in ?Param_0 - person ?Param_1 - aircraft ) = { True}
THEN Class IS PreState   W 1 and N+= 3 and N-= 0

IF
	( at ?Param_0 - person ?Param_2 - city ) = { False}
	( distance ?Param_2 - city c4 - city ) = { L1}
	( slow-burn ?Param_1 - aircraft ) = { True}
	( refuel-rate ?Param_1 - aircraft ) = { True}
	( at ?Param_1 - aircraft ?Param_2 - city ) = { True}
	( at ?Param_1 - aircraft c4 - city ) = { False}
	( distance c1 - city ?Param_2 - city ) = { L0/1}
	( at ?Param_1 - aircraft c1 - city ) = { False}
	( in p1 - person ?Param_1 - aircraft ) = { False}
	( at ?Param_1 - aircraft c5 - city ) = { False}
	( at ?Param_1 - aircraft c2 - city ) = { True}
	( in p2 - person ?Param_1 - aircraft ) = { True}
	( in ?Param_0 - person ?Param_1 - aircraft ) = { True}
THEN Class IS PostState   W 1 and N+= 2 and N-= 0

Predictivas: 
IF
	( at ?Param_0 - person ?Param_2 - city ) = { True}
THEN Class IS PreState   W 1 and N+= 3 and N-= 0

IF
	( at ?Param_0 - person ?Param_2 - city ) = { False}
THEN Class IS PostState   W 1 and N+= 2 and N-= 0

Patrones: 
011111111111111?111?1?1111?111?1?1?111?1??1(0)
101111111?11111?111?1?1111?1?1?1?1?11?1?1?1(1)

===============
 Ejecucion 2
===============
Aciertos: 0
Errores:  0
No clasificados: 0
	Rule 0(PreState): 0 / 0
	Rule 1(PostState): 0 / 0
Aciertos: 0
Errores:  0
No clasificados: 0
	Rule 0(PreState): 0 / 0
	Rule 1(PostState): 0 / 0
----------------------------------------------------
Aciertos Training: 1
Aciertos Test: -nan
Numero de reglas: 2
Variables por regla: 11
Variables usadas: 0.423077
Tiempo: 0.054949
Iteraciones: 0
Condiciones: 22
Aciertos Discriminantes: -nan
Aciertos Descriptivas: -nan
----------------------------------------------------
------------------- REGLAS -------------------------
----------------------------------------------------
Descriptivas: 
IF
	( at ?Param_0 - person ?Param_2 - city ) = { True}
	( slow-burn ?Param_1 - aircraft ) = { True}
	( refuel-rate ?Param_1 - aircraft ) = { True}
	( at ?Param_1 - aircraft ?Param_2 - city ) = { True}
	( at ?Param_1 - aircraft c4 - city ) = { False}
	( at ?Param_1 - aircraft c1 - city ) = { False}
	( in p1 - person ?Param_1 - aircraft ) = { False}
	( at ?Param_1 - aircraft c5 - city ) = { False}
	( at ?Param_1 - aircraft c2 - city ) = { False}
	( in p2 - person ?Param_1 - aircraft ) = { False}
	( in ?Param_0 - person ?Param_1 - aircraft ) = { True}
THEN Class IS PreState   W 1 and N+= 3 and N-= 0

IF
	( at ?Param_0 - person ?Param_2 - city ) = { False}
	( slow-burn ?Param_1 - aircraft ) = { True}
	( refuel-rate ?Param_1 - aircraft ) = { True}
	( at ?Param_1 - aircraft ?Param_2 - city ) = { True}
	( at ?Param_1 - aircraft c4 - city ) = { False}
	( at ?Param_1 - aircraft c1 - city ) = { False}
	( in p1 - person ?Param_1 - aircraft ) = { False}
	( at ?Param_1 - aircraft c5 - city ) = { False}
	( at ?Param_1 - aircraft c2 - city ) = { False}
	( in p2 - person ?Param_1 - aircraft ) = { False}
	( in ?Param_0 - person ?Param_1 - aircraft ) = { True}
THEN Class IS PostState   W 1 and N+= 3 and N-= 0

Predictivas: 
IF
	( at ?Param_0 - person ?Param_2 - city ) = { True}
THEN Class IS PreState   W 1 and N+= 3 and N-= 0

IF
	( at ?Param_0 - person ?Param_2 - city ) = { False}
THEN Class IS PostState   W 1 and N+= 3 and N-= 0

Patrones: 
011111111111111?111?1?1111?111?1?1?111?1??1(0)
101111111111111?111?1?1111?111?1?1?111?1??1(1)

===============
 Ejecucion 3
===============
Aciertos: 1
Errores:  0
No clasificados: 0
	Rule 0(PostState): 0 / 0
	Rule 1(PreState): 1 / 0
Aciertos: 0
Errores:  0
No clasificados: 1
	Rule 0(PostState): 0 / 0
	Rule 1(PreState): 0 / 0
----------------------------------------------------
Aciertos Training: 1
Aciertos Test: 0
Numero de reglas: 2
Variables por regla: 12
Variables usadas: 0.5
Tiempo: 0.056303
Iteraciones: 0
Condiciones: 24
Aciertos Discriminantes: 1
Aciertos Descriptivas: 1
----------------------------------------------------
------------------- REGLAS -------------------------
----------------------------------------------------
Descriptivas: 
IF
	( at ?Param_0 - person ?Param_2 - city ) = { False}
	( slow-burn ?Param_1 - aircraft ) = { True}
	( refuel-rate ?Param_1 - aircraft ) = { True}
	( at ?Param_1 - aircraft ?Param_2 - city ) = { True}
	( at ?Param_1 - aircraft c4 - city ) = { False}
	( at ?Param_1 - aircraft c1 - city ) = { False}
	( in p1 - person ?Param_1 - aircraft ) = { False}
	( at ?Param_1 - aircraft c5 - city ) = { False}
	( at ?Param_1 - aircraft c2 - city ) = { False}
	( in p2 - person ?Param_1 - aircraft ) = { False}
	( in ?Param_0 - person ?Param_1 - aircraft ) = { True}
THEN Class IS PostState   W 1 and N+= 3 and N-= 0

IF
	( at ?Param_0 - person ?Param_2 - city ) = { True}
	( distance ?Param_2 - city c3 - city ) = { L1}
	( slow-burn ?Param_1 - aircraft ) = { True}
	( refuel-rate ?Param_1 - aircraft ) = { True}
	( at ?Param_1 - aircraft ?Param_2 - city ) = { True}
	( at ?Param_1 - aircraft c4 - city ) = { False}
	( distance c1 - city ?Param_2 - city ) = { L1}
	( at ?Param_1 - aircraft c1 - city ) = { False}
	( in p1 - person ?Param_1 - aircraft ) = { False}
	( at ?Param_1 - aircraft c5 - city ) = { False}
	( at ?Param_1 - aircraft c2 - city ) = { False}
	( in p2 - person ?Param_1 - aircraft ) = { False}
	( in ?Param_0 - person ?Param_1 - aircraft ) = { True}
THEN Class IS PreState   W 1 and N+= 2 and N-= 0

Predictivas: 
IF
	( at ?Param_0 - person ?Param_2 - city ) = { False}
THEN Class IS PostState   W 1 and N+= 3 and N-= 0

IF
	( at ?Param_0 - person ?Param_2 - city ) = { True}
THEN Class IS PreState   W 1 and N+= 2 and N-= 0

Patrones: 
101111111111111?111?1?1111?111?1?1?111?1??1(1)
0111111?1111111?111?1?1111??11?1?1?111?1??1(0)

===============
 Ejecucion 4
===============
Aciertos: 0
Errores:  0
No clasificados: 0
	Rule 0(PreState): 0 / 0
	Rule 1(PostState): 0 / 0
Aciertos: 0
Errores:  0
No clasificados: 0
	Rule 0(PreState): 0 / 0
	Rule 1(PostState): 0 / 0
----------------------------------------------------
Aciertos Training: 1
Aciertos Test: -nan
Numero de reglas: 2
Variables por regla: 11
Variables usadas: 0.423077
Tiempo: 0.054861
Iteraciones: 0
Condiciones: 22
Aciertos Discriminantes: -nan
Aciertos Descriptivas: -nan
----------------------------------------------------
------------------- REGLAS -------------------------
----------------------------------------------------
Descriptivas: 
IF
	( at ?Param_0 - person ?Param_2 - city ) = { True}
	( slow-burn ?Param_1 - aircraft ) = { True}
	( refuel-rate ?Param_1 - aircraft ) = { True}
	( at ?Param_1 - aircraft ?Param_2 - city ) = { True}
	( at ?Param_1 - aircraft c4 - city ) = { False}
	( at ?Param_1 - aircraft c1 - city ) = { False}
	( in p1 - person ?Param_1 - aircraft ) = { False}
	( at ?Param_1 - aircraft c5 - city ) = { False}
	( at ?Param_1 - aircraft c2 - city ) = { False}
	( in p2 - person ?Param_1 - aircraft ) = { False}
	( in ?Param_0 - person ?Param_1 - aircraft ) = { True}
THEN Class IS PreState   W 1 and N+= 3 and N-= 0

IF
	( at ?Param_0 - person ?Param_2 - city ) = { False}
	( slow-burn ?Param_1 - aircraft ) = { True}
	( refuel-rate ?Param_1 - aircraft ) = { True}
	( at ?Param_1 - aircraft ?Param_2 - city ) = { True}
	( at ?Param_1 - aircraft c4 - city ) = { False}
	( at ?Param_1 - aircraft c1 - city ) = { False}
	( in p1 - person ?Param_1 - aircraft ) = { False}
	( at ?Param_1 - aircraft c5 - city ) = { False}
	( at ?Param_1 - aircraft c2 - city ) = { False}
	( in p2 - person ?Param_1 - aircraft ) = { False}
	( in ?Param_0 - person ?Param_1 - aircraft ) = { True}
THEN Class IS PostState   W 1 and N+= 3 and N-= 0

Predictivas: 
IF
	( at ?Param_0 - person ?Param_2 - city ) = { True}
THEN Class IS PreState   W 1 and N+= 3 and N-= 0

IF
	( at ?Param_0 - person ?Param_2 - city ) = { False}
THEN Class IS PostState   W 1 and N+= 3 and N-= 0

Patrones: 
011111111111111?111?1?1111?111?1?1?111?1??1(0)
101111111111111?111?1?1111?111?1?1?111?1??1(1)

===============
 Ejecucion 5
===============
Aciertos: 0
Errores:  0
No clasificados: 0
	Rule 0(PreState): 0 / 0
	Rule 1(PostState): 0 / 0
Aciertos: 0
Errores:  0
No clasificados: 0
	Rule 0(PreState): 0 / 0
	Rule 1(PostState): 0 / 0
----------------------------------------------------
Aciertos Training: 1
Aciertos Test: -nan
Numero de reglas: 2
Variables por regla: 11
Variables usadas: 0.423077
Tiempo: 0.056495
Iteraciones: 0
Condiciones: 22
Aciertos Discriminantes: -nan
Aciertos Descriptivas: -nan
----------------------------------------------------
------------------- REGLAS -------------------------
----------------------------------------------------
Descriptivas: 
IF
	( at ?Param_0 - person ?Param_2 - city ) = { True}
	( slow-burn ?Param_1 - aircraft ) = { True}
	( refuel-rate ?Param_1 - aircraft ) = { True}
	( at ?Param_1 - aircraft ?Param_2 - city ) = { True}
	( at ?Param_1 - aircraft c4 - city ) = { False}
	( at ?Param_1 - aircraft c1 - city ) = { False}
	( in p1 - person ?Param_1 - aircraft ) = { False}
	( at ?Param_1 - aircraft c5 - city ) = { False}
	( at ?Param_1 - aircraft c2 - city ) = { False}
	( in p2 - person ?Param_1 - aircraft ) = { False}
	( in ?Param_0 - person ?Param_1 - aircraft ) = { True}
THEN Class IS PreState   W 1 and N+= 3 and N-= 0

IF
	( at ?Param_0 - person ?Param_2 - city ) = { False}
	( slow-burn ?Param_1 - aircraft ) = { True}
	( refuel-rate ?Param_1 - aircraft ) = { True}
	( at ?Param_1 - aircraft ?Param_2 - city ) = { True}
	( at ?Param_1 - aircraft c4 - city ) = { False}
	( at ?Param_1 - aircraft c1 - city ) = { False}
	( in p1 - person ?Param_1 - aircraft ) = { False}
	( at ?Param_1 - aircraft c5 - city ) = { False}
	( at ?Param_1 - aircraft c2 - city ) = { False}
	( in p2 - person ?Param_1 - aircraft ) = { False}
	( in ?Param_0 - person ?Param_1 - aircraft ) = { True}
THEN Class IS PostState   W 1 and N+= 3 and N-= 0

Predictivas: 
IF
	( at ?Param_0 - person ?Param_2 - city ) = { True}
THEN Class IS PreState   W 1 and N+= 3 and N-= 0

IF
	( at ?Param_0 - person ?Param_2 - city ) = { False}
THEN Class IS PostState   W 1 and N+= 3 and N-= 0

Patrones: 
011111111111111?111?1?1111?111?1?1?111?1??1(0)
101111111111111?111?1?1111?111?1?1?111?1??1(1)

===============
 Ejecucion 6
===============
Aciertos: 0
Errores:  0
No clasificados: 0
	Rule 0(PreState): 0 / 0
	Rule 1(PostState): 0 / 0
Aciertos: 0
Errores:  0
No clasificados: 0
	Rule 0(PreState): 0 / 0
	Rule 1(PostState): 0 / 0
----------------------------------------------------
Aciertos Training: 1
Aciertos Test: -nan
Numero de reglas: 2
Variables por regla: 11
Variables usadas: 0.423077
Tiempo: 0.056341
Iteraciones: 0
Condiciones: 22
Aciertos Discriminantes: -nan
Aciertos Descriptivas: -nan
----------------------------------------------------
------------------- REGLAS -------------------------
----------------------------------------------------
Descriptivas: 
IF
	( at ?Param_0 - person ?Param_2 - city ) = { True}
	( slow-burn ?Param_1 - aircraft ) = { True}
	( refuel-rate ?Param_1 - aircraft ) = { True}
	( at ?Param_1 - aircraft ?Param_2 - city ) = { True}
	( at ?Param_1 - aircraft c4 - city ) = { False}
	( at ?Param_1 - aircraft c1 - city ) = { False}
	( in p1 - person ?Param_1 - aircraft ) = { False}
	( at ?Param_1 - aircraft c5 - city ) = { False}
	( at ?Param_1 - aircraft c2 - city ) = { False}
	( in p2 - person ?Param_1 - aircraft ) = { False}
	( in ?Param_0 - person ?Param_1 - aircraft ) = { True}
THEN Class IS PreState   W 1 and N+= 3 and N-= 0

IF
	( at ?Param_0 - person ?Param_2 - city ) = { False}
	( slow-burn ?Param_1 - aircraft ) = { True}
	( refuel-rate ?Param_1 - aircraft ) = { True}
	( at ?Param_1 - aircraft ?Param_2 - city ) = { True}
	( at ?Param_1 - aircraft c4 - city ) = { False}
	( at ?Param_1 - aircraft c1 - city ) = { False}
	( in p1 - person ?Param_1 - aircraft ) = { False}
	( at ?Param_1 - aircraft c5 - city ) = { False}
	( at ?Param_1 - aircraft c2 - city ) = { False}
	( in p2 - person ?Param_1 - aircraft ) = { False}
	( in ?Param_0 - person ?Param_1 - aircraft ) = { True}
THEN Class IS PostState   W 1 and N+= 3 and N-= 0

Predictivas: 
IF
	( at ?Param_0 - person ?Param_2 - city ) = { True}
THEN Class IS PreState   W 1 and N+= 3 and N-= 0

IF
	( at ?Param_0 - person ?Param_2 - city ) = { False}
THEN Class IS PostState   W 1 and N+= 3 and N-= 0

Patrones: 
011111111111111?111?1?1111?111?1?1?111?1??1(0)
101111111111111?111?1?1111?111?1?1?111?1??1(1)

===============
 Ejecucion 7
===============
Aciertos: 2
Errores:  0
No clasificados: 0
	Rule 0(PreState): 1 / 0
	Rule 1(PostState): 1 / 0
Aciertos: 0
Errores:  0
No clasificados: 2
	Rule 0(PreState): 0 / 0
	Rule 1(PostState): 0 / 0
----------------------------------------------------
Aciertos Training: 1
Aciertos Test: 0
Numero de reglas: 2
Variables por regla: 14
Variables usadas: 0.653846
Tiempo: 0.052577
Iteraciones: 0
Condiciones: 28
Aciertos Discriminantes: 1
Aciertos Descriptivas: 1
----------------------------------------------------
------------------- REGLAS -------------------------
----------------------------------------------------
Descriptivas: 
IF
	( at ?Param_0 - person ?Param_2 - city ) = { True}
	( distance ?Param_2 - city c4 - city ) = { L1}
	( slow-burn ?Param_1 - aircraft ) = { True}
	( refuel-rate ?Param_1 - aircraft ) = { True}
	( at ?Param_1 - aircraft ?Param_2 - city ) = { True}
	( at ?Param_1 - aircraft c4 - city ) = { False}
	( distance c1 - city ?Param_2 - city ) = { L0/1}
	( at ?Param_1 - aircraft c1 - city ) = { False}
	( in p1 - person ?Param_1 - aircraft ) = { False}
	( at ?Param_1 - aircraft c5 - city ) = { False}
	( at ?Param_1 - aircraft c2 - city ) = { True}
	( in p2 - person ?Param_1 - aircraft ) = { True}
	( in ?Param_0 - person ?Param_1 - aircraft ) = { True}
THEN Class IS PreState   W 1 and N+= 2 and N-= 0

IF
	( at ?Param_0 - person ?Param_2 - city ) = { False}
	( distance ?Param_2 - city c5 - city ) = { L1}
	( distance c5 - city ?Param_2 - city ) = { L1}
	( distance ?Param_2 - city c3 - city ) = { L0/1}
	( slow-burn ?Param_1 - aircraft ) = { True}
	( refuel-rate ?Param_1 - aircraft ) = { True}
	( at ?Param_1 - aircraft ?Param_2 - city ) = { True}
	( fuel ?Param_1 - aircraft ) = { L0/1}
	( at ?Param_1 - aircraft c4 - city ) = { False}
	( at ?Param_1 - aircraft c1 - city ) = { False}
	( in p1 - person ?Param_1 - aircraft ) = { False}
	( at ?Param_1 - aircraft c5 - city ) = { False}
	( at ?Param_1 - aircraft c2 - city ) = { False}
	( in p2 - person ?Param_1 - aircraft ) = { False}
	( in ?Param_0 - person ?Param_1 - aircraft ) = { True}
THEN Class IS PostState   W 1 and N+= 2 and N-= 0

Predictivas: 
IF
	( at ?Param_0 - person ?Param_2 - city ) = { True}
THEN Class IS PreState   W 1 and N+= 2 and N-= 0

IF
	( at ?Param_0 - person ?Param_2 - city ) = { False}
THEN Class IS PostState   W 1 and N+= 2 and N-= 0

Patrones: 
011111111?11111?111?1?1111?1?1?1?1?11?1?1?1(0)
101?1?11?111111?111?1?11?1?111?1?1?111?1??1(1)

===============
 Ejecucion 8
===============
Aciertos: 1
Errores:  0
No clasificados: 0
	Rule 0(PostState): 0 / 0
	Rule 1(PreState): 1 / 0
Aciertos: 0
Errores:  0
No clasificados: 1
	Rule 0(PostState): 0 / 0
	Rule 1(PreState): 0 / 0
----------------------------------------------------
Aciertos Training: 1
Aciertos Test: 0
Numero de reglas: 2
Variables por regla: 13
Variables usadas: 0.576923
Tiempo: 0.05665
Iteraciones: 0
Condiciones: 26
Aciertos Discriminantes: 1
Aciertos Descriptivas: 1
----------------------------------------------------
------------------- REGLAS -------------------------
----------------------------------------------------
Descriptivas: 
IF
	( at ?Param_0 - person ?Param_2 - city ) = { False}
	( slow-burn ?Param_1 - aircraft ) = { True}
	( refuel-rate ?Param_1 - aircraft ) = { True}
	( at ?Param_1 - aircraft ?Param_2 - city ) = { True}
	( at ?Param_1 - aircraft c4 - city ) = { False}
	( at ?Param_1 - aircraft c1 - city ) = { False}
	( in p1 - person ?Param_1 - aircraft ) = { False}
	( at ?Param_1 - aircraft c5 - city ) = { False}
	( at ?Param_1 - aircraft c2 - city ) = { False}
	( in p2 - person ?Param_1 - aircraft ) = { False}
	( in ?Param_0 - person ?Param_1 - aircraft ) = { True}
THEN Class IS PostState   W 1 and N+= 3 and N-= 0

IF
	( at ?Param_0 - person ?Param_2 - city ) = { True}
	( distance ?Param_2 - city c5 - city ) = { L1}
	( distance c5 - city ?Param_2 - city ) = { L1}
	( distance ?Param_2 - city c3 - city ) = { L0/1}
	( slow-burn ?Param_1 - aircraft ) = { True}
	( refuel-rate ?Param_1 - aircraft ) = { True}
	( at ?Param_1 - aircraft ?Param_2 - city ) = { True}
	( fuel ?Param_1 - aircraft ) = { L0/1}
	( at ?Param_1 - aircraft c4 - city ) = { False}
	( at ?Param_1 - aircraft c1 - city ) = { False}
	( in p1 - person ?Param_1 - aircraft ) = { False}
	( at ?Param_1 - aircraft c5 - city ) = { False}
	( at ?Param_1 - aircraft c2 - city ) = { False}
	( in p2 - person ?Param_1 - aircraft ) = { False}
	( in ?Param_0 - person ?Param_1 - aircraft ) = { True}
THEN Class IS PreState   W 1 and N+= 2 and N-= 0

Predictivas: 
IF
	( at ?Param_0 - person ?Param_2 - city ) = { False}
THEN Class IS PostState   W 1 and N+= 3 and N-= 0

IF
	( at ?Param_0 - person ?Param_2 - city ) = { True}
THEN Class IS PreState   W 1 and N+= 2 and N-= 0

Patrones: 
101111111111111?111?1?1111?111?1?1?111?1??1(1)
011?1?11?111111?111?1?11?1?111?1?1?111?1??1(0)

===============
 Ejecucion 9
===============
Aciertos: 1
Errores:  0
No clasificados: 0
	Rule 0(PreState): 0 / 0
	Rule 1(PostState): 1 / 0
Aciertos: 0
Errores:  0
No clasificados: 1
	Rule 0(PreState): 0 / 0
	Rule 1(PostState): 0 / 0
----------------------------------------------------
Aciertos Training: 1
Aciertos Test: 0
Numero de reglas: 2
Variables por regla: 12
Variables usadas: 0.5
Tiempo: 0.055512
Iteraciones: 0
Condiciones: 24
Aciertos Discriminantes: 1
Aciertos Descriptivas: 1
----------------------------------------------------
------------------- REGLAS -------------------------
----------------------------------------------------
Descriptivas: 
IF
	( at ?Param_0 - person ?Param_2 - city ) = { True}
	( slow-burn ?Param_1 - aircraft ) = { True}
	( refuel-rate ?Param_1 - aircraft ) = { True}
	( at ?Param_1 - aircraft ?Param_2 - city ) = { True}
	( at ?Param_1 - aircraft c4 - city ) = { False}
	( at ?Param_1 - aircraft c1 - city ) = { False}
	( in p1 - person ?Param_1 - aircraft ) = { False}
	( at ?Param_1 - aircraft c5 - city ) = { False}
	( at ?Param_1 - aircraft c2 - city ) = { False}
	( in p2 - person ?Param_1 - aircraft ) = { False}
	( in ?Param_0 - person ?Param_1 - aircraft ) = { True}
THEN Class IS PreState   W 1 and N+= 3 and N-= 0

IF
	( at ?Param_0 - person ?Param_2 - city ) = { False}
	( distance ?Param_2 - city c3 - city ) = { L1}
	( slow-burn ?Param_1 - aircraft ) = { True}
	( refuel-rate ?Param_1 - aircraft ) = { True}
	( at ?Param_1 - aircraft ?Param_2 - city ) = { True}
	( at ?Param_1 - aircraft c4 - city ) = { False}
	( distance c1 - city ?Param_2 - city ) = { L1}
	( at ?Param_1 - aircraft c1 - city ) = { False}
	( in p1 - person ?Param_1 - aircraft ) = { False}
	( at ?Param_1 - aircraft c5 - city ) = { False}
	( at ?Param_1 - aircraft c2 - city ) = { False}
	( in p2 - person ?Param_1 - aircraft ) = { False}
	( in ?Param_0 - person ?Param_1 - aircraft ) = { True}
THEN Class IS PostState   W 1 and N+= 2 and N-= 0

Predictivas: 
IF
	( at ?Param_0 - person ?Param_2 - city ) = { True}
THEN Class IS PreState   W 1 and N+= 3 and N-= 0

IF
	( at ?Param_0 - person ?Param_2 - city ) = { False}
THEN Class IS PostState   W 1 and N+= 2 and N-= 0

Patrones: 
011111111111111?111?1?1111?111?1?1?111?1??1(0)
1011111?1111111?111?1?1111??11?1?1?111?1??1(1)
	Aciertos en la base [0] es: 1
	Aciertos en test [0] es: -nan
	Numero de reglas [0] es: 2
	Variables por regla [0] es: 11
	Variables usadas [0] es: 0.423077
	Tiempo [0] es: 0.054531
	Iteraciones [0] es: 0
	Condiciones [0] es: 22

	Discriminantes [0] es: -nan

	Descriptivas [0] es: -nan

	Aciertos en la base [1] es: 1
	Aciertos en test [1] es: 1
	Numero de reglas [1] es: 2
	Variables por regla [1] es: 12
	Variables usadas [1] es: 0.5
	Tiempo [1] es: 0.055448
	Iteraciones [1] es: 0
	Condiciones [1] es: 24

	Discriminantes [1] es: 1

	Descriptivas [1] es: 0

	Aciertos en la base [2] es: 1
	Aciertos en test [2] es: -nan
	Numero de reglas [2] es: 2
	Variables por regla [2] es: 11
	Variables usadas [2] es: 0.423077
	Tiempo [2] es: 0.054949
	Iteraciones [2] es: 0
	Condiciones [2] es: 22

	Discriminantes [2] es: -nan

	Descriptivas [2] es: -nan

	Aciertos en la base [3] es: 1
	Aciertos en test [3] es: 1
	Numero de reglas [3] es: 2
	Variables por regla [3] es: 12
	Variables usadas [3] es: 0.5
	Tiempo [3] es: 0.056303
	Iteraciones [3] es: 0
	Condiciones [3] es: 24

	Discriminantes [3] es: 1

	Descriptivas [3] es: 0

	Aciertos en la base [4] es: 1
	Aciertos en test [4] es: -nan
	Numero de reglas [4] es: 2
	Variables por regla [4] es: 11
	Variables usadas [4] es: 0.423077
	Tiempo [4] es: 0.054861
	Iteraciones [4] es: 0
	Condiciones [4] es: 22

	Discriminantes [4] es: -nan

	Descriptivas [4] es: -nan

	Aciertos en la base [5] es: 1
	Aciertos en test [5] es: -nan
	Numero de reglas [5] es: 2
	Variables por regla [5] es: 11
	Variables usadas [5] es: 0.423077
	Tiempo [5] es: 0.056495
	Iteraciones [5] es: 0
	Condiciones [5] es: 22

	Discriminantes [5] es: -nan

	Descriptivas [5] es: -nan

	Aciertos en la base [6] es: 1
	Aciertos en test [6] es: -nan
	Numero de reglas [6] es: 2
	Variables por regla [6] es: 11
	Variables usadas [6] es: 0.423077
	Tiempo [6] es: 0.056341
	Iteraciones [6] es: 0
	Condiciones [6] es: 22

	Discriminantes [6] es: -nan

	Descriptivas [6] es: -nan

	Aciertos en la base [7] es: 1
	Aciertos en test [7] es: 1
	Numero de reglas [7] es: 2
	Variables por regla [7] es: 14
	Variables usadas [7] es: 0.653846
	Tiempo [7] es: 0.052577
	Iteraciones [7] es: 0
	Condiciones [7] es: 28

	Discriminantes [7] es: 1

	Descriptivas [7] es: 0

	Aciertos en la base [8] es: 1
	Aciertos en test [8] es: 1
	Numero de reglas [8] es: 2
	Variables por regla [8] es: 13
	Variables usadas [8] es: 0.576923
	Tiempo [8] es: 0.05665
	Iteraciones [8] es: 0
	Condiciones [8] es: 26

	Discriminantes [8] es: 1

	Descriptivas [8] es: 0

	Aciertos en la base [9] es: 1
	Aciertos en test [9] es: 1
	Numero de reglas [9] es: 2
	Variables por regla [9] es: 12
	Variables usadas [9] es: 0.5
	Tiempo [9] es: 0.055512
	Iteraciones [9] es: 0
	Condiciones [9] es: 24

	Discriminantes [9] es: 1

	Descriptivas [9] es: 0

La media training: 1( 0)
La media test: -nan( -nan)
La media de reglas: 2( 0)
La media de variables por regla: 11.8( 0.979796)
La media de variables usadas: 0.484615( 0.0753689)
La media de tiempo: 0.0553667( 0.00116981)
La media de iteraciones: 0( 0)
La media de condiciones: 23.6( 1.95959)
La media de discriminantes: -nan( -nan)
La media de conjuntas: -nan( -nan)
Frecuencia de aparicion de las 26 distintas variables en las reglas: 
( at ?Param_0 - person ?Param_2 - city ): --> 100 %
( distance ?Param_2 - city c2 - city ): --> 0 %
( distance ?Param_2 - city c5 - city ): --> 10 %
( distance c5 - city ?Param_2 - city ): --> 10 %
( distance ?Param_2 - city c3 - city ): --> 20 %
( distance ?Param_2 - city c4 - city ): --> 10 %
( distance c3 - city ?Param_2 - city ): --> 0 %
( distance c4 - city ?Param_2 - city ): --> 0 %
( slow-speed ?Param_1 - aircraft ): --> 0 %
( fast-speed ?Param_1 - aircraft ): --> 0 %
( slow-burn ?Param_1 - aircraft ): --> 100 %
( fast-burn ?Param_1 - aircraft ): --> 0 %
( capacity ?Param_1 - aircraft ): --> 0 %
( refuel-rate ?Param_1 - aircraft ): --> 100 %
( at ?Param_1 - aircraft ?Param_2 - city ): --> 100 %
( fuel ?Param_1 - aircraft ): --> 10 %
( at ?Param_1 - aircraft c4 - city ): --> 100 %
( distance c1 - city ?Param_2 - city ): --> 20 %
( at ?Param_1 - aircraft c1 - city ): --> 100 %
( in p1 - person ?Param_1 - aircraft ): --> 100 %
( at ?Param_1 - aircraft c5 - city ): --> 100 %
( distance ?Param_2 - city c1 - city ): --> 0 %
( distance c2 - city ?Param_2 - city ): --> 0 %
( at ?Param_1 - aircraft c2 - city ): --> 100 %
( in p2 - person ?Param_1 - aircraft ): --> 100 %
( in ?Param_0 - person ?Param_1 - aircraft ): --> 100 %
