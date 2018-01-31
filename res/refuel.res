Experiment: res/refuel
mié jun  7 10:53:38 CEST 2017
===================================================


===============
 Ejecucion 0
===============
Aciertos: 0
Errores:  0
No clasificados: 0
	Rule 0(PostState): 0 / 0
	Rule 1(PreState): 0 / 0
Aciertos: 0
Errores:  0
No clasificados: 0
	Rule 0(PostState): 0 / 0
	Rule 1(PreState): 0 / 0
----------------------------------------------------
Aciertos Training: 1
Aciertos Test: -nan
Numero de reglas: 2
Variables por regla: 10
Variables usadas: 0.357143
Tiempo: 0.076291
Iteraciones: 0
Condiciones: 20
Aciertos Discriminantes: -nan
Aciertos Descriptivas: -nan
----------------------------------------------------
------------------- REGLAS -------------------------
----------------------------------------------------
Descriptivas: 
IF
	( slow-burn ?Param_0 - aircraft ) = { True}
	( refuel-rate ?Param_0 - aircraft ) = { True}
	( at ?Param_0 - aircraft ?Param_1 - city ) = { True}
	( fuel ?Param_0 - aircraft ) = { L2/3 L3}
	( at ?Param_0 - aircraft c4 - city ) = { False}
	( at ?Param_0 - aircraft c1 - city ) = { False}
	( at ?Param_0 - aircraft c5 - city ) = { False}
	( at ?Param_0 - aircraft c2 - city ) = { False}
	( in p3 - person ?Param_0 - aircraft ) = { True}
	( at p3 - person ?Param_1 - city ) = { False}
THEN Class IS PostState   W 1 and N+= 5 and N-= 0

IF
	( slow-burn ?Param_0 - aircraft ) = { True}
	( refuel-rate ?Param_0 - aircraft ) = { True}
	( at ?Param_0 - aircraft ?Param_1 - city ) = { True}
	( fuel ?Param_0 - aircraft ) = { L0/3 L1/3 L2/3}
	( at ?Param_0 - aircraft c4 - city ) = { False}
	( at ?Param_0 - aircraft c1 - city ) = { False}
	( at ?Param_0 - aircraft c5 - city ) = { False}
	( at ?Param_0 - aircraft c2 - city ) = { False}
	( in p3 - person ?Param_0 - aircraft ) = { True}
	( at p3 - person ?Param_1 - city ) = { False}
THEN Class IS PreState   W 1 and N+= 5 and N-= 0

Predictivas: 
IF
	( fuel ?Param_0 - aircraft ) = { L2/3 L3}
THEN Class IS PostState   W 1 and N+= 5 and N-= 0

IF
	( fuel ?Param_0 - aircraft ) = { L0/3 L1/3 L2/3}
THEN Class IS PreState   W 1 and N+= 5 and N-= 0

Patrones: 
111111111111111?111?1?100111?11111111111?1?11111??11?(1)
111111111111111?111?1?111101?11111111111?1?11111??11?(0)

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
Variables por regla: 12.5
Variables usadas: 0.535714
Tiempo: 0.081904
Iteraciones: 0
Condiciones: 25
Aciertos Discriminantes: 1
Aciertos Descriptivas: 1
----------------------------------------------------
------------------- REGLAS -------------------------
----------------------------------------------------
Descriptivas: 
IF
	( slow-burn ?Param_0 - aircraft ) = { True}
	( refuel-rate ?Param_0 - aircraft ) = { True}
	( at ?Param_0 - aircraft ?Param_1 - city ) = { True}
	( fuel ?Param_0 - aircraft ) = { L0/3 L1/3 L2/3}
	( at ?Param_0 - aircraft c4 - city ) = { False}
	( at ?Param_0 - aircraft c1 - city ) = { False}
	( at ?Param_0 - aircraft c5 - city ) = { False}
	( at ?Param_0 - aircraft c2 - city ) = { False}
	( in p3 - person ?Param_0 - aircraft ) = { True}
	( at p3 - person ?Param_1 - city ) = { False}
THEN Class IS PreState   W 1 and N+= 5 and N-= 0

IF
	( distance ?Param_1 - city c2 - city ) = { L1}
	( distance ?Param_1 - city c5 - city ) = { L1}
	( distance c5 - city ?Param_1 - city ) = { L1}
	( slow-burn ?Param_0 - aircraft ) = { True}
	( refuel-rate ?Param_0 - aircraft ) = { True}
	( at ?Param_0 - aircraft ?Param_1 - city ) = { True}
	( fuel ?Param_0 - aircraft ) = { L2/3 L3}
	( at ?Param_0 - aircraft c4 - city ) = { False}
	( in p1 - person ?Param_0 - aircraft ) = { False}
	( at p1 - person ?Param_1 - city ) = { True}
	( at ?Param_0 - aircraft c1 - city ) = { False}
	( at ?Param_0 - aircraft c5 - city ) = { False}
	( at ?Param_0 - aircraft c2 - city ) = { False}
	( in p3 - person ?Param_0 - aircraft ) = { True}
	( at p3 - person ?Param_1 - city ) = { False}
THEN Class IS PostState   W 1 and N+= 4 and N-= 0

Predictivas: 
IF
	( fuel ?Param_0 - aircraft ) = { L0/3 L1/3 L2/3}
THEN Class IS PreState   W 1 and N+= 5 and N-= 0

IF
	( fuel ?Param_0 - aircraft ) = { L2/3 L3}
THEN Class IS PostState   W 1 and N+= 4 and N-= 0

Patrones: 
111111111111111?111?1?111101?11111111111?1?11111??11?(0)
?1?1?1111111111?111?1?100111?1??11111111?1?11111??11?(1)

===============
 Ejecucion 2
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
Variables por regla: 10.5
Variables usadas: 0.392857
Tiempo: 0.072653
Iteraciones: 0
Condiciones: 21
Aciertos Discriminantes: 1
Aciertos Descriptivas: 1
----------------------------------------------------
------------------- REGLAS -------------------------
----------------------------------------------------
Descriptivas: 
IF
	( slow-burn ?Param_0 - aircraft ) = { True}
	( refuel-rate ?Param_0 - aircraft ) = { True}
	( at ?Param_0 - aircraft ?Param_1 - city ) = { True}
	( fuel ?Param_0 - aircraft ) = { L0/3 L1/3 L2/3}
	( at ?Param_0 - aircraft c4 - city ) = { False}
	( at ?Param_0 - aircraft c1 - city ) = { False}
	( at ?Param_0 - aircraft c5 - city ) = { False}
	( at ?Param_0 - aircraft c2 - city ) = { False}
	( in p3 - person ?Param_0 - aircraft ) = { True}
	( at p3 - person ?Param_1 - city ) = { False}
THEN Class IS PreState   W 1 and N+= 5 and N-= 0

IF
	( slow-burn ?Param_0 - aircraft ) = { True}
	( refuel-rate ?Param_0 - aircraft ) = { True}
	( at ?Param_0 - aircraft ?Param_1 - city ) = { True}
	( fuel ?Param_0 - aircraft ) = { L2/3 L3}
	( at ?Param_0 - aircraft c4 - city ) = { False}
	( at ?Param_0 - aircraft c1 - city ) = { False}
	( at ?Param_0 - aircraft c5 - city ) = { False}
	( at p2 - person ?Param_1 - city ) = { False}
	( at ?Param_0 - aircraft c2 - city ) = { False}
	( in p3 - person ?Param_0 - aircraft ) = { True}
	( at p3 - person ?Param_1 - city ) = { False}
THEN Class IS PostState   W 1 and N+= 4 and N-= 0

Predictivas: 
IF
	( fuel ?Param_0 - aircraft ) = { L0/3 L1/3 L2/3}
THEN Class IS PreState   W 1 and N+= 5 and N-= 0

IF
	( fuel ?Param_0 - aircraft ) = { L2/3 L3}
THEN Class IS PostState   W 1 and N+= 4 and N-= 0

Patrones: 
111111111111111?111?1?111101?11111111111?1?11111??11?(0)
111111111111111?111?1?100111?11111111111?1?111?1??11?(1)

===============
 Ejecucion 3
===============
Aciertos: 2
Errores:  0
No clasificados: 0
	Rule 0(PostState): 1 / 0
	Rule 1(PreState): 1 / 0
Aciertos: 2
Errores:  0
No clasificados: 0
	Rule 0(PostState): 1 / 0
	Rule 1(PreState): 1 / 0
----------------------------------------------------
Aciertos Training: 1
Aciertos Test: 1
Numero de reglas: 2
Variables por regla: 10
Variables usadas: 0.357143
Tiempo: 0.069971
Iteraciones: 0
Condiciones: 20
Aciertos Discriminantes: 1
Aciertos Descriptivas: 1
----------------------------------------------------
------------------- REGLAS -------------------------
----------------------------------------------------
Descriptivas: 
IF
	( slow-burn ?Param_0 - aircraft ) = { True}
	( refuel-rate ?Param_0 - aircraft ) = { True}
	( at ?Param_0 - aircraft ?Param_1 - city ) = { True}
	( fuel ?Param_0 - aircraft ) = { L2/3 L3}
	( at ?Param_0 - aircraft c4 - city ) = { False}
	( at ?Param_0 - aircraft c1 - city ) = { False}
	( at ?Param_0 - aircraft c5 - city ) = { False}
	( at ?Param_0 - aircraft c2 - city ) = { False}
	( in p3 - person ?Param_0 - aircraft ) = { True}
	( at p3 - person ?Param_1 - city ) = { False}
THEN Class IS PostState   W 1 and N+= 4 and N-= 0

IF
	( slow-burn ?Param_0 - aircraft ) = { True}
	( refuel-rate ?Param_0 - aircraft ) = { True}
	( at ?Param_0 - aircraft ?Param_1 - city ) = { True}
	( fuel ?Param_0 - aircraft ) = { L0/3 L1/3 L2/3}
	( at ?Param_0 - aircraft c4 - city ) = { False}
	( at ?Param_0 - aircraft c1 - city ) = { False}
	( at ?Param_0 - aircraft c5 - city ) = { False}
	( at ?Param_0 - aircraft c2 - city ) = { False}
	( in p3 - person ?Param_0 - aircraft ) = { True}
	( at p3 - person ?Param_1 - city ) = { False}
THEN Class IS PreState   W 1 and N+= 4 and N-= 0

Predictivas: 
IF
	( fuel ?Param_0 - aircraft ) = { L2/3 L3}
THEN Class IS PostState   W 1 and N+= 4 and N-= 0

IF
	( fuel ?Param_0 - aircraft ) = { L0/3 L1/3 L2/3}
THEN Class IS PreState   W 1 and N+= 4 and N-= 0

Patrones: 
111111111111111?111?1?100111?11111111111?1?11111??11?(1)
111111111111111?111?1?111101?11111111111?1?11111??11?(0)

===============
 Ejecucion 4
===============
Aciertos: 0
Errores:  0
No clasificados: 0
	Rule 0(PostState): 0 / 0
	Rule 1(PreState): 0 / 0
Aciertos: 0
Errores:  0
No clasificados: 0
	Rule 0(PostState): 0 / 0
	Rule 1(PreState): 0 / 0
----------------------------------------------------
Aciertos Training: 1
Aciertos Test: -nan
Numero de reglas: 2
Variables por regla: 10
Variables usadas: 0.357143
Tiempo: 0.069295
Iteraciones: 0
Condiciones: 20
Aciertos Discriminantes: -nan
Aciertos Descriptivas: -nan
----------------------------------------------------
------------------- REGLAS -------------------------
----------------------------------------------------
Descriptivas: 
IF
	( slow-burn ?Param_0 - aircraft ) = { True}
	( refuel-rate ?Param_0 - aircraft ) = { True}
	( at ?Param_0 - aircraft ?Param_1 - city ) = { True}
	( fuel ?Param_0 - aircraft ) = { L2/3 L3}
	( at ?Param_0 - aircraft c4 - city ) = { False}
	( at ?Param_0 - aircraft c1 - city ) = { False}
	( at ?Param_0 - aircraft c5 - city ) = { False}
	( at ?Param_0 - aircraft c2 - city ) = { False}
	( in p3 - person ?Param_0 - aircraft ) = { True}
	( at p3 - person ?Param_1 - city ) = { False}
THEN Class IS PostState   W 1 and N+= 5 and N-= 0

IF
	( slow-burn ?Param_0 - aircraft ) = { True}
	( refuel-rate ?Param_0 - aircraft ) = { True}
	( at ?Param_0 - aircraft ?Param_1 - city ) = { True}
	( fuel ?Param_0 - aircraft ) = { L0/3 L1/3 L2/3}
	( at ?Param_0 - aircraft c4 - city ) = { False}
	( at ?Param_0 - aircraft c1 - city ) = { False}
	( at ?Param_0 - aircraft c5 - city ) = { False}
	( at ?Param_0 - aircraft c2 - city ) = { False}
	( in p3 - person ?Param_0 - aircraft ) = { True}
	( at p3 - person ?Param_1 - city ) = { False}
THEN Class IS PreState   W 1 and N+= 5 and N-= 0

Predictivas: 
IF
	( fuel ?Param_0 - aircraft ) = { L2/3 L3}
THEN Class IS PostState   W 1 and N+= 5 and N-= 0

IF
	( fuel ?Param_0 - aircraft ) = { L0/3 L1/3 L2/3}
THEN Class IS PreState   W 1 and N+= 5 and N-= 0

Patrones: 
111111111111111?111?1?100111?11111111111?1?11111??11?(1)
111111111111111?111?1?111101?11111111111?1?11111??11?(0)

===============
 Ejecucion 5
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
Tiempo: 0.071845
Iteraciones: 0
Condiciones: 24
Aciertos Discriminantes: 1
Aciertos Descriptivas: 1
----------------------------------------------------
------------------- REGLAS -------------------------
----------------------------------------------------
Descriptivas: 
IF
	( slow-burn ?Param_0 - aircraft ) = { True}
	( refuel-rate ?Param_0 - aircraft ) = { True}
	( at ?Param_0 - aircraft ?Param_1 - city ) = { True}
	( fuel ?Param_0 - aircraft ) = { L0/3 L1/3 L2/3}
	( at ?Param_0 - aircraft c4 - city ) = { False}
	( at ?Param_0 - aircraft c1 - city ) = { False}
	( at ?Param_0 - aircraft c5 - city ) = { False}
	( at ?Param_0 - aircraft c2 - city ) = { False}
	( in p3 - person ?Param_0 - aircraft ) = { True}
	( at p3 - person ?Param_1 - city ) = { False}
THEN Class IS PreState   W 1 and N+= 5 and N-= 0

IF
	( distance ?Param_1 - city c4 - city ) = { L1}
	( slow-burn ?Param_0 - aircraft ) = { True}
	( refuel-rate ?Param_0 - aircraft ) = { True}
	( at ?Param_0 - aircraft ?Param_1 - city ) = { True}
	( fuel ?Param_0 - aircraft ) = { L2/3 L3}
	( at ?Param_0 - aircraft c4 - city ) = { False}
	( distance c1 - city ?Param_1 - city ) = { L0/1}
	( distance ?Param_1 - city c1 - city ) = { L0/1}
	( distance c2 - city ?Param_1 - city ) = { L1}
	( at ?Param_0 - aircraft c1 - city ) = { False}
	( at ?Param_0 - aircraft c5 - city ) = { False}
	( at ?Param_0 - aircraft c2 - city ) = { False}
	( in p3 - person ?Param_0 - aircraft ) = { True}
	( at p3 - person ?Param_1 - city ) = { True}
THEN Class IS PostState   W 1 and N+= 4 and N-= 0

Predictivas: 
IF
	( fuel ?Param_0 - aircraft ) = { L0/3 L1/3 L2/3}
THEN Class IS PreState   W 1 and N+= 5 and N-= 0

IF
	( fuel ?Param_0 - aircraft ) = { L2/3 L3}
THEN Class IS PostState   W 1 and N+= 4 and N-= 0

Patrones: 
111111111111111?111?1?111101?11111111111?1?11111??11?(0)
11111111?111111?111?1?100111?11111?1??11?1?11111??1?1(1)

===============
 Ejecucion 6
===============
Aciertos: 0
Errores:  0
No clasificados: 0
	Rule 0(PostState): 0 / 0
	Rule 1(PreState): 0 / 0
Aciertos: 0
Errores:  0
No clasificados: 0
	Rule 0(PostState): 0 / 0
	Rule 1(PreState): 0 / 0
----------------------------------------------------
Aciertos Training: 1
Aciertos Test: -nan
Numero de reglas: 2
Variables por regla: 10
Variables usadas: 0.357143
Tiempo: 0.070747
Iteraciones: 0
Condiciones: 20
Aciertos Discriminantes: -nan
Aciertos Descriptivas: -nan
----------------------------------------------------
------------------- REGLAS -------------------------
----------------------------------------------------
Descriptivas: 
IF
	( slow-burn ?Param_0 - aircraft ) = { True}
	( refuel-rate ?Param_0 - aircraft ) = { True}
	( at ?Param_0 - aircraft ?Param_1 - city ) = { True}
	( fuel ?Param_0 - aircraft ) = { L2/3 L3}
	( at ?Param_0 - aircraft c4 - city ) = { False}
	( at ?Param_0 - aircraft c1 - city ) = { False}
	( at ?Param_0 - aircraft c5 - city ) = { False}
	( at ?Param_0 - aircraft c2 - city ) = { False}
	( in p3 - person ?Param_0 - aircraft ) = { True}
	( at p3 - person ?Param_1 - city ) = { False}
THEN Class IS PostState   W 1 and N+= 5 and N-= 0

IF
	( slow-burn ?Param_0 - aircraft ) = { True}
	( refuel-rate ?Param_0 - aircraft ) = { True}
	( at ?Param_0 - aircraft ?Param_1 - city ) = { True}
	( fuel ?Param_0 - aircraft ) = { L0/3 L1/3 L2/3}
	( at ?Param_0 - aircraft c4 - city ) = { False}
	( at ?Param_0 - aircraft c1 - city ) = { False}
	( at ?Param_0 - aircraft c5 - city ) = { False}
	( at ?Param_0 - aircraft c2 - city ) = { False}
	( in p3 - person ?Param_0 - aircraft ) = { True}
	( at p3 - person ?Param_1 - city ) = { False}
THEN Class IS PreState   W 1 and N+= 5 and N-= 0

Predictivas: 
IF
	( fuel ?Param_0 - aircraft ) = { L2/3 L3}
THEN Class IS PostState   W 1 and N+= 5 and N-= 0

IF
	( fuel ?Param_0 - aircraft ) = { L0/3 L1/3 L2/3}
THEN Class IS PreState   W 1 and N+= 5 and N-= 0

Patrones: 
111111111111111?111?1?100111?11111111111?1?11111??11?(1)
111111111111111?111?1?111101?11111111111?1?11111??11?(0)

===============
 Ejecucion 7
===============
Aciertos: 3
Errores:  0
No clasificados: 0
	Rule 0(PostState): 1 / 0
	Rule 1(PreState): 2 / 0
Aciertos: 1
Errores:  0
No clasificados: 2
	Rule 0(PostState): 0 / 0
	Rule 1(PreState): 1 / 0
----------------------------------------------------
Aciertos Training: 1
Aciertos Test: 0.333333
Numero de reglas: 2
Variables por regla: 13
Variables usadas: 0.464286
Tiempo: 0.091336
Iteraciones: 0
Condiciones: 26
Aciertos Discriminantes: 1
Aciertos Descriptivas: 1
----------------------------------------------------
------------------- REGLAS -------------------------
----------------------------------------------------
Descriptivas: 
IF
	( distance ?Param_1 - city c3 - city ) = { L1}
	( slow-burn ?Param_0 - aircraft ) = { True}
	( refuel-rate ?Param_0 - aircraft ) = { True}
	( at ?Param_0 - aircraft ?Param_1 - city ) = { True}
	( fuel ?Param_0 - aircraft ) = { L2/3 L3}
	( at ?Param_0 - aircraft c4 - city ) = { False}
	( at ?Param_0 - aircraft c1 - city ) = { False}
	( at ?Param_0 - aircraft c5 - city ) = { False}
	( in p2 - person ?Param_0 - aircraft ) = { False}
	( at p2 - person ?Param_1 - city ) = { True}
	( at ?Param_0 - aircraft c2 - city ) = { False}
	( in p3 - person ?Param_0 - aircraft ) = { True}
	( at p3 - person ?Param_1 - city ) = { False}
THEN Class IS PostState   W 1 and N+= 4 and N-= 0

IF
	( distance ?Param_1 - city c3 - city ) = { L1}
	( slow-burn ?Param_0 - aircraft ) = { True}
	( refuel-rate ?Param_0 - aircraft ) = { True}
	( at ?Param_0 - aircraft ?Param_1 - city ) = { True}
	( fuel ?Param_0 - aircraft ) = { L0/3 L1/3 L2/3}
	( at ?Param_0 - aircraft c4 - city ) = { False}
	( at ?Param_0 - aircraft c1 - city ) = { False}
	( at ?Param_0 - aircraft c5 - city ) = { False}
	( in p2 - person ?Param_0 - aircraft ) = { False}
	( at p2 - person ?Param_1 - city ) = { True}
	( at ?Param_0 - aircraft c2 - city ) = { False}
	( in p3 - person ?Param_0 - aircraft ) = { True}
	( at p3 - person ?Param_1 - city ) = { False}
THEN Class IS PreState   W 1 and N+= 3 and N-= 0

Predictivas: 
IF
	( fuel ?Param_0 - aircraft ) = { L2/3 L3}
THEN Class IS PostState   W 1 and N+= 4 and N-= 0

IF
	( fuel ?Param_0 - aircraft ) = { L0/3 L1/3 L2/3}
THEN Class IS PreState   W 1 and N+= 3 and N-= 0

Patrones: 
111111?11111111?111?1?100111?11111111111?1?1??11??11?(1)
111111?11111111?111?1?111101?11111111111?1?1??11??11?(0)

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
Variables por regla: 12.5
Variables usadas: 0.535714
Tiempo: 0.071486
Iteraciones: 0
Condiciones: 25
Aciertos Discriminantes: 1
Aciertos Descriptivas: 1
----------------------------------------------------
------------------- REGLAS -------------------------
----------------------------------------------------
Descriptivas: 
IF
	( slow-burn ?Param_0 - aircraft ) = { True}
	( refuel-rate ?Param_0 - aircraft ) = { True}
	( at ?Param_0 - aircraft ?Param_1 - city ) = { True}
	( fuel ?Param_0 - aircraft ) = { L2/3 L3}
	( at ?Param_0 - aircraft c4 - city ) = { False}
	( at ?Param_0 - aircraft c1 - city ) = { False}
	( at ?Param_0 - aircraft c5 - city ) = { False}
	( at ?Param_0 - aircraft c2 - city ) = { False}
	( in p3 - person ?Param_0 - aircraft ) = { True}
	( at p3 - person ?Param_1 - city ) = { False}
THEN Class IS PostState   W 1 and N+= 5 and N-= 0

IF
	( distance ?Param_1 - city c2 - city ) = { L1}
	( distance ?Param_1 - city c5 - city ) = { L1}
	( distance c5 - city ?Param_1 - city ) = { L1}
	( slow-burn ?Param_0 - aircraft ) = { True}
	( refuel-rate ?Param_0 - aircraft ) = { True}
	( at ?Param_0 - aircraft ?Param_1 - city ) = { True}
	( fuel ?Param_0 - aircraft ) = { L0/3 L1/3 L2/3}
	( at ?Param_0 - aircraft c4 - city ) = { False}
	( in p1 - person ?Param_0 - aircraft ) = { False}
	( at p1 - person ?Param_1 - city ) = { True}
	( at ?Param_0 - aircraft c1 - city ) = { False}
	( at ?Param_0 - aircraft c5 - city ) = { False}
	( at ?Param_0 - aircraft c2 - city ) = { False}
	( in p3 - person ?Param_0 - aircraft ) = { True}
	( at p3 - person ?Param_1 - city ) = { False}
THEN Class IS PreState   W 1 and N+= 4 and N-= 0

Predictivas: 
IF
	( fuel ?Param_0 - aircraft ) = { L2/3 L3}
THEN Class IS PostState   W 1 and N+= 5 and N-= 0

IF
	( fuel ?Param_0 - aircraft ) = { L0/3 L1/3 L2/3}
THEN Class IS PreState   W 1 and N+= 4 and N-= 0

Patrones: 
111111111111111?111?1?100111?11111111111?1?11111??11?(1)
?1?1?1111111111?111?1?111101?1??11111111?1?11111??11?(0)

===============
 Ejecucion 9
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
Tiempo: 0.069246
Iteraciones: 0
Condiciones: 24
Aciertos Discriminantes: 1
Aciertos Descriptivas: 1
----------------------------------------------------
------------------- REGLAS -------------------------
----------------------------------------------------
Descriptivas: 
IF
	( slow-burn ?Param_0 - aircraft ) = { True}
	( refuel-rate ?Param_0 - aircraft ) = { True}
	( at ?Param_0 - aircraft ?Param_1 - city ) = { True}
	( fuel ?Param_0 - aircraft ) = { L2/3 L3}
	( at ?Param_0 - aircraft c4 - city ) = { False}
	( at ?Param_0 - aircraft c1 - city ) = { False}
	( at ?Param_0 - aircraft c5 - city ) = { False}
	( at ?Param_0 - aircraft c2 - city ) = { False}
	( in p3 - person ?Param_0 - aircraft ) = { True}
	( at p3 - person ?Param_1 - city ) = { False}
THEN Class IS PostState   W 1 and N+= 5 and N-= 0

IF
	( distance ?Param_1 - city c4 - city ) = { L1}
	( slow-burn ?Param_0 - aircraft ) = { True}
	( refuel-rate ?Param_0 - aircraft ) = { True}
	( at ?Param_0 - aircraft ?Param_1 - city ) = { True}
	( fuel ?Param_0 - aircraft ) = { L0/3 L1/3 L2/3}
	( at ?Param_0 - aircraft c4 - city ) = { False}
	( distance c1 - city ?Param_1 - city ) = { L0/1}
	( distance ?Param_1 - city c1 - city ) = { L0/1}
	( distance c2 - city ?Param_1 - city ) = { L1}
	( at ?Param_0 - aircraft c1 - city ) = { False}
	( at ?Param_0 - aircraft c5 - city ) = { False}
	( at ?Param_0 - aircraft c2 - city ) = { False}
	( in p3 - person ?Param_0 - aircraft ) = { True}
	( at p3 - person ?Param_1 - city ) = { True}
THEN Class IS PreState   W 1 and N+= 4 and N-= 0

Predictivas: 
IF
	( fuel ?Param_0 - aircraft ) = { L2/3 L3}
THEN Class IS PostState   W 1 and N+= 5 and N-= 0

IF
	( fuel ?Param_0 - aircraft ) = { L0/3 L1/3 L2/3}
THEN Class IS PreState   W 1 and N+= 4 and N-= 0

Patrones: 
111111111111111?111?1?100111?11111111111?1?11111??11?(1)
11111111?111111?111?1?111101?11111?1??11?1?11111??1?1(0)
	Aciertos en la base [0] es: 1
	Aciertos en test [0] es: -nan
	Numero de reglas [0] es: 2
	Variables por regla [0] es: 10
	Variables usadas [0] es: 0.357143
	Tiempo [0] es: 0.076291
	Iteraciones [0] es: 0
	Condiciones [0] es: 20

	Discriminantes [0] es: -nan

	Descriptivas [0] es: -nan

	Aciertos en la base [1] es: 1
	Aciertos en test [1] es: 1
	Numero de reglas [1] es: 2
	Variables por regla [1] es: 12.5
	Variables usadas [1] es: 0.535714
	Tiempo [1] es: 0.081904
	Iteraciones [1] es: 0
	Condiciones [1] es: 25

	Discriminantes [1] es: 1

	Descriptivas [1] es: 0

	Aciertos en la base [2] es: 1
	Aciertos en test [2] es: 1
	Numero de reglas [2] es: 2
	Variables por regla [2] es: 10.5
	Variables usadas [2] es: 0.392857
	Tiempo [2] es: 0.072653
	Iteraciones [2] es: 0
	Condiciones [2] es: 21

	Discriminantes [2] es: 1

	Descriptivas [2] es: 0

	Aciertos en la base [3] es: 1
	Aciertos en test [3] es: 1
	Numero de reglas [3] es: 2
	Variables por regla [3] es: 10
	Variables usadas [3] es: 0.357143
	Tiempo [3] es: 0.069971
	Iteraciones [3] es: 0
	Condiciones [3] es: 20

	Discriminantes [3] es: 1

	Descriptivas [3] es: 1

	Aciertos en la base [4] es: 1
	Aciertos en test [4] es: -nan
	Numero de reglas [4] es: 2
	Variables por regla [4] es: 10
	Variables usadas [4] es: 0.357143
	Tiempo [4] es: 0.069295
	Iteraciones [4] es: 0
	Condiciones [4] es: 20

	Discriminantes [4] es: -nan

	Descriptivas [4] es: -nan

	Aciertos en la base [5] es: 1
	Aciertos en test [5] es: 1
	Numero de reglas [5] es: 2
	Variables por regla [5] es: 12
	Variables usadas [5] es: 0.5
	Tiempo [5] es: 0.071845
	Iteraciones [5] es: 0
	Condiciones [5] es: 24

	Discriminantes [5] es: 1

	Descriptivas [5] es: 0

	Aciertos en la base [6] es: 1
	Aciertos en test [6] es: -nan
	Numero de reglas [6] es: 2
	Variables por regla [6] es: 10
	Variables usadas [6] es: 0.357143
	Tiempo [6] es: 0.070747
	Iteraciones [6] es: 0
	Condiciones [6] es: 20

	Discriminantes [6] es: -nan

	Descriptivas [6] es: -nan

	Aciertos en la base [7] es: 1
	Aciertos en test [7] es: 1
	Numero de reglas [7] es: 2
	Variables por regla [7] es: 13
	Variables usadas [7] es: 0.464286
	Tiempo [7] es: 0.091336
	Iteraciones [7] es: 0
	Condiciones [7] es: 26

	Discriminantes [7] es: 1

	Descriptivas [7] es: 0.333333

	Aciertos en la base [8] es: 1
	Aciertos en test [8] es: 1
	Numero de reglas [8] es: 2
	Variables por regla [8] es: 12.5
	Variables usadas [8] es: 0.535714
	Tiempo [8] es: 0.071486
	Iteraciones [8] es: 0
	Condiciones [8] es: 25

	Discriminantes [8] es: 1

	Descriptivas [8] es: 0

	Aciertos en la base [9] es: 1
	Aciertos en test [9] es: 1
	Numero de reglas [9] es: 2
	Variables por regla [9] es: 12
	Variables usadas [9] es: 0.5
	Tiempo [9] es: 0.069246
	Iteraciones [9] es: 0
	Condiciones [9] es: 24

	Discriminantes [9] es: 1

	Descriptivas [9] es: 0

La media training: 1( 0)
La media test: -nan( -nan)
La media de reglas: 2( 0)
La media de variables por regla: 11.25( 1.18849)
La media de variables usadas: 0.435714( 0.0745736)
La media de tiempo: 0.0744774( 0.00671052)
La media de iteraciones: 0( 0)
La media de condiciones: 22.5( 2.37697)
La media de discriminantes: -nan( -nan)
La media de conjuntas: -nan( -nan)
Frecuencia de aparicion de las 28 distintas variables en las reglas: 
( distance ?Param_1 - city c2 - city ): --> 10 %
( distance ?Param_1 - city c5 - city ): --> 10 %
( distance c5 - city ?Param_1 - city ): --> 10 %
( distance ?Param_1 - city c3 - city ): --> 10 %
( distance ?Param_1 - city c4 - city ): --> 10 %
( distance c3 - city ?Param_1 - city ): --> 0 %
( distance c4 - city ?Param_1 - city ): --> 0 %
( slow-speed ?Param_0 - aircraft ): --> 0 %
( fast-speed ?Param_0 - aircraft ): --> 0 %
( slow-burn ?Param_0 - aircraft ): --> 100 %
( fast-burn ?Param_0 - aircraft ): --> 0 %
( capacity ?Param_0 - aircraft ): --> 0 %
( refuel-rate ?Param_0 - aircraft ): --> 100 %
( at ?Param_0 - aircraft ?Param_1 - city ): --> 100 %
( fuel ?Param_0 - aircraft ): --> 100 %
( at ?Param_0 - aircraft c4 - city ): --> 100 %
( in p1 - person ?Param_0 - aircraft ): --> 10 %
( at p1 - person ?Param_1 - city ): --> 10 %
( distance c1 - city ?Param_1 - city ): --> 10 %
( distance ?Param_1 - city c1 - city ): --> 10 %
( distance c2 - city ?Param_1 - city ): --> 10 %
( at ?Param_0 - aircraft c1 - city ): --> 100 %
( at ?Param_0 - aircraft c5 - city ): --> 100 %
( in p2 - person ?Param_0 - aircraft ): --> 10 %
( at p2 - person ?Param_1 - city ): --> 15 %
( at ?Param_0 - aircraft c2 - city ): --> 100 %
( in p3 - person ?Param_0 - aircraft ): --> 100 %
( at p3 - person ?Param_1 - city ): --> 100 %
