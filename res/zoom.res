Experiment: res/zoom
mié jun  7 10:53:41 CEST 2017
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
Variables por regla: 13
Variables usadas: 0.333333
Tiempo: 0.097833
Iteraciones: 0
Condiciones: 26
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
	( at ?Param_0 - aircraft c4 - city ) = { False}
	( fuel ?Param_0 - aircraft ) = { L2}
	( at p2 - person ?Param_2 - city ) = { True}
	( at ?Param_0 - aircraft c1 - city ) = { False}
	( at p1 - person ?Param_2 - city ) = { True}
	( at ?Param_0 - aircraft ?Param_2 - city ) = { False}
	( at p3 - person ?Param_2 - city ) = { True}
	( at ?Param_0 - aircraft c2 - city ) = { False}
	( in p3 - person ?Param_0 - aircraft ) = { True}
	( at p3 - person ?Param_1 - city ) = { False}
THEN Class IS PreState   W 1 and N+= 5 and N-= 0

IF
	( slow-burn ?Param_0 - aircraft ) = { True}
	( refuel-rate ?Param_0 - aircraft ) = { True}
	( at ?Param_0 - aircraft ?Param_1 - city ) = { False}
	( at ?Param_0 - aircraft c4 - city ) = { False}
	( fuel ?Param_0 - aircraft ) = { L0/2 L1/2}
	( at p2 - person ?Param_2 - city ) = { True}
	( at ?Param_0 - aircraft c1 - city ) = { False}
	( at p1 - person ?Param_2 - city ) = { True}
	( at ?Param_0 - aircraft ?Param_2 - city ) = { True}
	( at p3 - person ?Param_2 - city ) = { True}
	( at ?Param_0 - aircraft c2 - city ) = { False}
	( in p3 - person ?Param_0 - aircraft ) = { True}
	( at p3 - person ?Param_1 - city ) = { False}
THEN Class IS PostState   W 1 and N+= 5 and N-= 0

Predictivas: 
IF
	( fuel ?Param_0 - aircraft ) = { L2}
THEN Class IS PreState   W 1 and N+= 5 and N-= 0

IF
	( fuel ?Param_0 - aircraft ) = { L0/2 L1/2}
THEN Class IS PostState   W 1 and N+= 5 and N-= 0

Patrones: 
11111111111111111111111?111?1?11?1111001?1111111111?11?11?1111?1111??11?(0)
11111111111111111111111?111?11?1?1111110?1111111111?11?1?11111?1111??11?(1)

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
Variables por regla: 15.5
Variables usadas: 0.461538
Tiempo: 0.140532
Iteraciones: 0
Condiciones: 31
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
	( at ?Param_0 - aircraft c4 - city ) = { False}
	( fuel ?Param_0 - aircraft ) = { L1/2 L2}
	( at p2 - person ?Param_2 - city ) = { True}
	( at ?Param_0 - aircraft c1 - city ) = { False}
	( at p1 - person ?Param_2 - city ) = { True}
	( at ?Param_0 - aircraft ?Param_2 - city ) = { False}
	( at p3 - person ?Param_2 - city ) = { True}
	( at ?Param_0 - aircraft c2 - city ) = { False}
	( in p3 - person ?Param_0 - aircraft ) = { True}
	( at p3 - person ?Param_1 - city ) = { False}
THEN Class IS PreState   W 1 and N+= 5 and N-= 0

IF
	( distance ?Param_1 - city c2 - city ) = { L1}
	( distance ?Param_1 - city ?Param_2 - city ) = { L1}
	( distance ?Param_2 - city ?Param_1 - city ) = { L1}
	( slow-burn ?Param_0 - aircraft ) = { True}
	( refuel-rate ?Param_0 - aircraft ) = { True}
	( at ?Param_0 - aircraft ?Param_1 - city ) = { False}
	( at ?Param_0 - aircraft c4 - city ) = { False}
	( in p1 - person ?Param_0 - aircraft ) = { False}
	( at p1 - person ?Param_1 - city ) = { True}
	( fuel ?Param_0 - aircraft ) = { L0/2 L1/2}
	( at p2 - person ?Param_2 - city ) = { True}
	( at ?Param_0 - aircraft c1 - city ) = { False}
	( at p1 - person ?Param_2 - city ) = { True}
	( at ?Param_0 - aircraft ?Param_2 - city ) = { True}
	( at p3 - person ?Param_2 - city ) = { True}
	( at ?Param_0 - aircraft c2 - city ) = { False}
	( in p3 - person ?Param_0 - aircraft ) = { True}
	( at p3 - person ?Param_1 - city ) = { False}
THEN Class IS PostState   W 1 and N+= 4 and N-= 0

Predictivas: 
IF
	( fuel ?Param_0 - aircraft ) = { L1/2 L2}
THEN Class IS PreState   W 1 and N+= 5 and N-= 0

IF
	( fuel ?Param_0 - aircraft ) = { L0/2 L1/2}
THEN Class IS PostState   W 1 and N+= 4 and N-= 0

Patrones: 
11111111111111111111111?111?1?11?1111011?1111111111?11?11?1111?1111??11?(0)
?1?1?111111111111111111?111?11?1?1??1110?1111111111?11?1?11111?1111??11?(1)

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
Variables por regla: 16.5
Variables usadas: 0.512821
Tiempo: 0.095833
Iteraciones: 0
Condiciones: 33
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
	( at ?Param_0 - aircraft c4 - city ) = { False}
	( fuel ?Param_0 - aircraft ) = { L2}
	( at p2 - person ?Param_2 - city ) = { True}
	( at ?Param_0 - aircraft c1 - city ) = { False}
	( at p1 - person ?Param_2 - city ) = { True}
	( at ?Param_0 - aircraft ?Param_2 - city ) = { False}
	( at p3 - person ?Param_2 - city ) = { True}
	( at ?Param_0 - aircraft c2 - city ) = { False}
	( in p3 - person ?Param_0 - aircraft ) = { True}
	( at p3 - person ?Param_1 - city ) = { False}
THEN Class IS PreState   W 1 and N+= 5 and N-= 0

IF
	( distance ?Param_1 - city c2 - city ) = { L0/1}
	( distance c2 - city ?Param_2 - city ) = { L1}
	( slow-burn ?Param_0 - aircraft ) = { True}
	( refuel-rate ?Param_0 - aircraft ) = { True}
	( at ?Param_0 - aircraft ?Param_1 - city ) = { False}
	( at ?Param_0 - aircraft c4 - city ) = { False}
	( fuel ?Param_0 - aircraft ) = { L0/2 L1/2}
	( at p2 - person ?Param_2 - city ) = { True}
	( distance c1 - city ?Param_2 - city ) = { L0/1}
	( distance ?Param_2 - city c4 - city ) = { L1}
	( at ?Param_0 - aircraft c1 - city ) = { False}
	( distance ?Param_2 - city c1 - city ) = { L0/1}
	( at p1 - person ?Param_2 - city ) = { True}
	( at ?Param_0 - aircraft ?Param_2 - city ) = { True}
	( at p2 - person ?Param_1 - city ) = { False}
	( at p3 - person ?Param_2 - city ) = { True}
	( distance c2 - city ?Param_1 - city ) = { L0/1}
	( at ?Param_0 - aircraft c2 - city ) = { False}
	( in p3 - person ?Param_0 - aircraft ) = { True}
	( at p3 - person ?Param_1 - city ) = { False}
THEN Class IS PostState   W 1 and N+= 4 and N-= 0

Predictivas: 
IF
	( fuel ?Param_0 - aircraft ) = { L2}
THEN Class IS PreState   W 1 and N+= 5 and N-= 0

IF
	( fuel ?Param_0 - aircraft ) = { L0/2 L1/2}
THEN Class IS PostState   W 1 and N+= 4 and N-= 0

Patrones: 
11111111111111111111111?111?1?11?1111001?1111111111?11?11?1111?1111??11?(0)
1?1111111111?1111111111?111?11?1?1111110?11?1111?11?1??1?1111??11?1??11?(1)

===============
 Ejecucion 3
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
Variables por regla: 16
Variables usadas: 0.410256
Tiempo: 0.097238
Iteraciones: 0
Condiciones: 32
Aciertos Discriminantes: 1
Aciertos Descriptivas: 1
----------------------------------------------------
------------------- REGLAS -------------------------
----------------------------------------------------
Descriptivas: 
IF
	( distance c4 - city ?Param_2 - city ) = { L0/1}
	( distance ?Param_2 - city c3 - city ) = { L1}
	( slow-burn ?Param_0 - aircraft ) = { True}
	( refuel-rate ?Param_0 - aircraft ) = { True}
	( at ?Param_0 - aircraft ?Param_1 - city ) = { True}
	( at ?Param_0 - aircraft c4 - city ) = { False}
	( fuel ?Param_0 - aircraft ) = { L2}
	( at p2 - person ?Param_2 - city ) = { True}
	( distance ?Param_2 - city c4 - city ) = { L0/1}
	( at ?Param_0 - aircraft c1 - city ) = { False}
	( at p1 - person ?Param_2 - city ) = { True}
	( at ?Param_0 - aircraft ?Param_2 - city ) = { False}
	( at p3 - person ?Param_2 - city ) = { True}
	( at ?Param_0 - aircraft c2 - city ) = { False}
	( in p3 - person ?Param_0 - aircraft ) = { True}
	( at p3 - person ?Param_1 - city ) = { False}
THEN Class IS PreState   W 1 and N+= 4 and N-= 0

IF
	( distance c4 - city ?Param_2 - city ) = { L0/1}
	( distance ?Param_2 - city c3 - city ) = { L1}
	( slow-burn ?Param_0 - aircraft ) = { True}
	( refuel-rate ?Param_0 - aircraft ) = { True}
	( at ?Param_0 - aircraft ?Param_1 - city ) = { False}
	( at ?Param_0 - aircraft c4 - city ) = { False}
	( fuel ?Param_0 - aircraft ) = { L0/2 L1/2}
	( at p2 - person ?Param_2 - city ) = { True}
	( distance ?Param_2 - city c4 - city ) = { L0/1}
	( at ?Param_0 - aircraft c1 - city ) = { False}
	( at p1 - person ?Param_2 - city ) = { True}
	( at ?Param_0 - aircraft ?Param_2 - city ) = { True}
	( at p3 - person ?Param_2 - city ) = { True}
	( at ?Param_0 - aircraft c2 - city ) = { False}
	( in p3 - person ?Param_0 - aircraft ) = { True}
	( at p3 - person ?Param_1 - city ) = { False}
THEN Class IS PostState   W 1 and N+= 4 and N-= 0

Predictivas: 
IF
	( fuel ?Param_0 - aircraft ) = { L2}
THEN Class IS PreState   W 1 and N+= 4 and N-= 0

IF
	( fuel ?Param_0 - aircraft ) = { L0/2 L1/2}
THEN Class IS PostState   W 1 and N+= 4 and N-= 0

Patrones: 
1111111?11111111111?111?111?1?11?1111001?11111111?1?11?11?1111?1111??11?(0)
1111111?11111111111?111?111?11?1?1111110?11111111?1?11?1?11111?1111??11?(1)

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
Variables por regla: 13
Variables usadas: 0.333333
Tiempo: 0.100785
Iteraciones: 0
Condiciones: 26
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
	( at ?Param_0 - aircraft c4 - city ) = { False}
	( fuel ?Param_0 - aircraft ) = { L2}
	( at p2 - person ?Param_2 - city ) = { True}
	( at ?Param_0 - aircraft c1 - city ) = { False}
	( at p1 - person ?Param_2 - city ) = { True}
	( at ?Param_0 - aircraft ?Param_2 - city ) = { False}
	( at p3 - person ?Param_2 - city ) = { True}
	( at ?Param_0 - aircraft c2 - city ) = { False}
	( in p3 - person ?Param_0 - aircraft ) = { True}
	( at p3 - person ?Param_1 - city ) = { False}
THEN Class IS PreState   W 1 and N+= 5 and N-= 0

IF
	( slow-burn ?Param_0 - aircraft ) = { True}
	( refuel-rate ?Param_0 - aircraft ) = { True}
	( at ?Param_0 - aircraft ?Param_1 - city ) = { False}
	( at ?Param_0 - aircraft c4 - city ) = { False}
	( fuel ?Param_0 - aircraft ) = { L0/2 L1/2}
	( at p2 - person ?Param_2 - city ) = { True}
	( at ?Param_0 - aircraft c1 - city ) = { False}
	( at p1 - person ?Param_2 - city ) = { True}
	( at ?Param_0 - aircraft ?Param_2 - city ) = { True}
	( at p3 - person ?Param_2 - city ) = { True}
	( at ?Param_0 - aircraft c2 - city ) = { False}
	( in p3 - person ?Param_0 - aircraft ) = { True}
	( at p3 - person ?Param_1 - city ) = { False}
THEN Class IS PostState   W 1 and N+= 5 and N-= 0

Predictivas: 
IF
	( fuel ?Param_0 - aircraft ) = { L2}
THEN Class IS PreState   W 1 and N+= 5 and N-= 0

IF
	( fuel ?Param_0 - aircraft ) = { L0/2 L1/2}
THEN Class IS PostState   W 1 and N+= 5 and N-= 0

Patrones: 
11111111111111111111111?111?1?11?1111001?1111111111?11?11?1111?1111??11?(0)
11111111111111111111111?111?11?1?1111110?1111111111?11?1?11111?1111??11?(1)

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
Variables por regla: 15
Variables usadas: 0.435897
Tiempo: 0.095514
Iteraciones: 0
Condiciones: 30
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
	( at ?Param_0 - aircraft c4 - city ) = { False}
	( fuel ?Param_0 - aircraft ) = { L2}
	( at p2 - person ?Param_2 - city ) = { True}
	( at ?Param_0 - aircraft c1 - city ) = { False}
	( at p1 - person ?Param_2 - city ) = { True}
	( at ?Param_0 - aircraft ?Param_2 - city ) = { False}
	( at p3 - person ?Param_2 - city ) = { True}
	( at ?Param_0 - aircraft c2 - city ) = { False}
	( in p3 - person ?Param_0 - aircraft ) = { True}
	( at p3 - person ?Param_1 - city ) = { False}
THEN Class IS PreState   W 1 and N+= 5 and N-= 0

IF
	( distance ?Param_1 - city c4 - city ) = { L1}
	( slow-burn ?Param_0 - aircraft ) = { True}
	( refuel-rate ?Param_0 - aircraft ) = { True}
	( at ?Param_0 - aircraft ?Param_1 - city ) = { False}
	( at ?Param_0 - aircraft c4 - city ) = { False}
	( fuel ?Param_0 - aircraft ) = { L0/2 L1/2}
	( at p2 - person ?Param_2 - city ) = { True}
	( distance c1 - city ?Param_1 - city ) = { L0/1}
	( distance ?Param_1 - city c1 - city ) = { L0/1}
	( at ?Param_0 - aircraft c1 - city ) = { False}
	( at p1 - person ?Param_2 - city ) = { True}
	( at ?Param_0 - aircraft ?Param_2 - city ) = { True}
	( at p3 - person ?Param_2 - city ) = { True}
	( distance c2 - city ?Param_1 - city ) = { L1}
	( at ?Param_0 - aircraft c2 - city ) = { False}
	( in p3 - person ?Param_0 - aircraft ) = { True}
	( at p3 - person ?Param_1 - city ) = { True}
THEN Class IS PostState   W 1 and N+= 4 and N-= 0

Predictivas: 
IF
	( fuel ?Param_0 - aircraft ) = { L2}
THEN Class IS PreState   W 1 and N+= 5 and N-= 0

IF
	( fuel ?Param_0 - aircraft ) = { L0/2 L1/2}
THEN Class IS PostState   W 1 and N+= 4 and N-= 0

Patrones: 
11111111111111111111111?111?1?11?1111001?1111111111?11?11?1111?1111??11?(0)
1111111111?111111111111?111?11?1?1111110?1111?1?111?11?1?11111?1?11??1?1(1)

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
Variables por regla: 13
Variables usadas: 0.333333
Tiempo: 0.098598
Iteraciones: 0
Condiciones: 26
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
	( at ?Param_0 - aircraft c4 - city ) = { False}
	( fuel ?Param_0 - aircraft ) = { L2}
	( at p2 - person ?Param_2 - city ) = { True}
	( at ?Param_0 - aircraft c1 - city ) = { False}
	( at p1 - person ?Param_2 - city ) = { True}
	( at ?Param_0 - aircraft ?Param_2 - city ) = { False}
	( at p3 - person ?Param_2 - city ) = { True}
	( at ?Param_0 - aircraft c2 - city ) = { False}
	( in p3 - person ?Param_0 - aircraft ) = { True}
	( at p3 - person ?Param_1 - city ) = { False}
THEN Class IS PreState   W 1 and N+= 5 and N-= 0

IF
	( slow-burn ?Param_0 - aircraft ) = { True}
	( refuel-rate ?Param_0 - aircraft ) = { True}
	( at ?Param_0 - aircraft ?Param_1 - city ) = { False}
	( at ?Param_0 - aircraft c4 - city ) = { False}
	( fuel ?Param_0 - aircraft ) = { L0/2 L1/2}
	( at p2 - person ?Param_2 - city ) = { True}
	( at ?Param_0 - aircraft c1 - city ) = { False}
	( at p1 - person ?Param_2 - city ) = { True}
	( at ?Param_0 - aircraft ?Param_2 - city ) = { True}
	( at p3 - person ?Param_2 - city ) = { True}
	( at ?Param_0 - aircraft c2 - city ) = { False}
	( in p3 - person ?Param_0 - aircraft ) = { True}
	( at p3 - person ?Param_1 - city ) = { False}
THEN Class IS PostState   W 1 and N+= 5 and N-= 0

Predictivas: 
IF
	( fuel ?Param_0 - aircraft ) = { L2}
THEN Class IS PreState   W 1 and N+= 5 and N-= 0

IF
	( fuel ?Param_0 - aircraft ) = { L0/2 L1/2}
THEN Class IS PostState   W 1 and N+= 5 and N-= 0

Patrones: 
11111111111111111111111?111?1?11?1111001?1111111111?11?11?1111?1111??11?(0)
11111111111111111111111?111?11?1?1111110?1111111111?11?1?11111?1111??11?(1)

===============
 Ejecucion 7
===============
Aciertos: 3
Errores:  0
No clasificados: 0
	Rule 0(PostState): 1 / 0
	Rule 1(PreState): 2 / 0
Aciertos: 0
Errores:  0
No clasificados: 3
	Rule 0(PostState): 0 / 0
	Rule 1(PreState): 0 / 0
----------------------------------------------------
Aciertos Training: 1
Aciertos Test: 0
Numero de reglas: 2
Variables por regla: 19
Variables usadas: 0.564103
Tiempo: 0.097442
Iteraciones: 0
Condiciones: 38
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
	( at ?Param_0 - aircraft ?Param_1 - city ) = { False}
	( at ?Param_0 - aircraft c4 - city ) = { False}
	( fuel ?Param_0 - aircraft ) = { L0/2 L1/2}
	( at p2 - person ?Param_2 - city ) = { True}
	( at ?Param_0 - aircraft c1 - city ) = { False}
	( at p1 - person ?Param_2 - city ) = { True}
	( at ?Param_0 - aircraft ?Param_2 - city ) = { True}
	( in p2 - person ?Param_0 - aircraft ) = { False}
	( at p2 - person ?Param_1 - city ) = { True}
	( at p3 - person ?Param_2 - city ) = { True}
	( at ?Param_0 - aircraft c2 - city ) = { False}
	( in p3 - person ?Param_0 - aircraft ) = { True}
	( at p3 - person ?Param_1 - city ) = { False}
THEN Class IS PostState   W 1 and N+= 4 and N-= 0

IF
	( distance ?Param_1 - city c2 - city ) = { L0/1}
	( distance ?Param_1 - city c3 - city ) = { L1}
	( distance c2 - city ?Param_2 - city ) = { L1}
	( slow-burn ?Param_0 - aircraft ) = { True}
	( refuel-rate ?Param_0 - aircraft ) = { True}
	( at ?Param_0 - aircraft ?Param_1 - city ) = { True}
	( at ?Param_0 - aircraft c4 - city ) = { False}
	( fuel ?Param_0 - aircraft ) = { L2}
	( at p2 - person ?Param_2 - city ) = { True}
	( distance c1 - city ?Param_2 - city ) = { L0/1}
	( distance ?Param_2 - city c4 - city ) = { L1}
	( at ?Param_0 - aircraft c1 - city ) = { False}
	( distance ?Param_2 - city c1 - city ) = { L0/1}
	( at p1 - person ?Param_2 - city ) = { True}
	( at ?Param_0 - aircraft ?Param_2 - city ) = { False}
	( in p2 - person ?Param_0 - aircraft ) = { False}
	( at p2 - person ?Param_1 - city ) = { True}
	( at p3 - person ?Param_2 - city ) = { True}
	( distance c2 - city ?Param_1 - city ) = { L0/1}
	( at ?Param_0 - aircraft c2 - city ) = { False}
	( in p3 - person ?Param_0 - aircraft ) = { True}
	( at p3 - person ?Param_1 - city ) = { False}
THEN Class IS PreState   W 1 and N+= 3 and N-= 0

Predictivas: 
IF
	( fuel ?Param_0 - aircraft ) = { L0/2 L1/2}
THEN Class IS PostState   W 1 and N+= 4 and N-= 0

IF
	( fuel ?Param_0 - aircraft ) = { L2}
THEN Class IS PreState   W 1 and N+= 3 and N-= 0

Patrones: 
11111111?11111111111111?111?11?1?1111110?1111111111?11?1?11??1?1111??11?(1)
1?111111?111?1111111111?111?1?11?1111001?11?1111?11?1??11?1??1?11?1??11?(0)

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
Variables por regla: 15.5
Variables usadas: 0.461538
Tiempo: 0.113329
Iteraciones: 0
Condiciones: 31
Aciertos Discriminantes: 1
Aciertos Descriptivas: 1
----------------------------------------------------
------------------- REGLAS -------------------------
----------------------------------------------------
Descriptivas: 
IF
	( slow-burn ?Param_0 - aircraft ) = { True}
	( refuel-rate ?Param_0 - aircraft ) = { True}
	( at ?Param_0 - aircraft ?Param_1 - city ) = { False}
	( at ?Param_0 - aircraft c4 - city ) = { False}
	( fuel ?Param_0 - aircraft ) = { L0/2 L1/2}
	( at p2 - person ?Param_2 - city ) = { True}
	( at ?Param_0 - aircraft c1 - city ) = { False}
	( at p1 - person ?Param_2 - city ) = { True}
	( at ?Param_0 - aircraft ?Param_2 - city ) = { True}
	( at p3 - person ?Param_2 - city ) = { True}
	( at ?Param_0 - aircraft c2 - city ) = { False}
	( in p3 - person ?Param_0 - aircraft ) = { True}
	( at p3 - person ?Param_1 - city ) = { False}
THEN Class IS PostState   W 1 and N+= 5 and N-= 0

IF
	( distance ?Param_1 - city c2 - city ) = { L1}
	( distance ?Param_1 - city ?Param_2 - city ) = { L1}
	( distance ?Param_2 - city ?Param_1 - city ) = { L1}
	( slow-burn ?Param_0 - aircraft ) = { True}
	( refuel-rate ?Param_0 - aircraft ) = { True}
	( at ?Param_0 - aircraft ?Param_1 - city ) = { True}
	( at ?Param_0 - aircraft c4 - city ) = { False}
	( in p1 - person ?Param_0 - aircraft ) = { False}
	( at p1 - person ?Param_1 - city ) = { True}
	( fuel ?Param_0 - aircraft ) = { L2}
	( at p2 - person ?Param_2 - city ) = { True}
	( at ?Param_0 - aircraft c1 - city ) = { False}
	( at p1 - person ?Param_2 - city ) = { True}
	( at ?Param_0 - aircraft ?Param_2 - city ) = { False}
	( at p3 - person ?Param_2 - city ) = { True}
	( at ?Param_0 - aircraft c2 - city ) = { False}
	( in p3 - person ?Param_0 - aircraft ) = { True}
	( at p3 - person ?Param_1 - city ) = { False}
THEN Class IS PreState   W 1 and N+= 4 and N-= 0

Predictivas: 
IF
	( fuel ?Param_0 - aircraft ) = { L0/2 L1/2}
THEN Class IS PostState   W 1 and N+= 5 and N-= 0

IF
	( fuel ?Param_0 - aircraft ) = { L2}
THEN Class IS PreState   W 1 and N+= 4 and N-= 0

Patrones: 
11111111111111111111111?111?11?1?1111110?1111111111?11?1?11111?1111??11?(1)
?1?1?111111111111111111?111?1?11?1??1001?1111111111?11?11?1111?1111??11?(0)

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
Variables por regla: 15
Variables usadas: 0.435897
Tiempo: 0.100797
Iteraciones: 0
Condiciones: 30
Aciertos Discriminantes: 1
Aciertos Descriptivas: 1
----------------------------------------------------
------------------- REGLAS -------------------------
----------------------------------------------------
Descriptivas: 
IF
	( slow-burn ?Param_0 - aircraft ) = { True}
	( refuel-rate ?Param_0 - aircraft ) = { True}
	( at ?Param_0 - aircraft ?Param_1 - city ) = { False}
	( at ?Param_0 - aircraft c4 - city ) = { False}
	( fuel ?Param_0 - aircraft ) = { L0/2 L1/2}
	( at p2 - person ?Param_2 - city ) = { True}
	( at ?Param_0 - aircraft c1 - city ) = { False}
	( at p1 - person ?Param_2 - city ) = { True}
	( at ?Param_0 - aircraft ?Param_2 - city ) = { True}
	( at p3 - person ?Param_2 - city ) = { True}
	( at ?Param_0 - aircraft c2 - city ) = { False}
	( in p3 - person ?Param_0 - aircraft ) = { True}
	( at p3 - person ?Param_1 - city ) = { False}
THEN Class IS PostState   W 1 and N+= 5 and N-= 0

IF
	( distance ?Param_1 - city c4 - city ) = { L1}
	( slow-burn ?Param_0 - aircraft ) = { True}
	( refuel-rate ?Param_0 - aircraft ) = { True}
	( at ?Param_0 - aircraft ?Param_1 - city ) = { True}
	( at ?Param_0 - aircraft c4 - city ) = { False}
	( fuel ?Param_0 - aircraft ) = { L2}
	( at p2 - person ?Param_2 - city ) = { True}
	( distance c1 - city ?Param_1 - city ) = { L0/1}
	( distance ?Param_1 - city c1 - city ) = { L0/1}
	( at ?Param_0 - aircraft c1 - city ) = { False}
	( at p1 - person ?Param_2 - city ) = { True}
	( at ?Param_0 - aircraft ?Param_2 - city ) = { False}
	( at p3 - person ?Param_2 - city ) = { True}
	( distance c2 - city ?Param_1 - city ) = { L1}
	( at ?Param_0 - aircraft c2 - city ) = { False}
	( in p3 - person ?Param_0 - aircraft ) = { True}
	( at p3 - person ?Param_1 - city ) = { True}
THEN Class IS PreState   W 1 and N+= 4 and N-= 0

Predictivas: 
IF
	( at ?Param_0 - aircraft ?Param_1 - city ) = { False}
THEN Class IS PostState   W 1 and N+= 5 and N-= 0

IF
	( fuel ?Param_0 - aircraft ) = { L2}
THEN Class IS PreState   W 1 and N+= 4 and N-= 0

Patrones: 
11111111111111111111111?111?1101?111111??1111111111?11?1?11111?1111??11?(1)
1111111111?111111111111?111?1?11?1111001?1111?1?111?11?11?1111?1?11??1?1(0)
	Aciertos en la base [0] es: 1
	Aciertos en test [0] es: -nan
	Numero de reglas [0] es: 2
	Variables por regla [0] es: 13
	Variables usadas [0] es: 0.333333
	Tiempo [0] es: 0.097833
	Iteraciones [0] es: 0
	Condiciones [0] es: 26

	Discriminantes [0] es: -nan

	Descriptivas [0] es: -nan

	Aciertos en la base [1] es: 1
	Aciertos en test [1] es: 1
	Numero de reglas [1] es: 2
	Variables por regla [1] es: 15.5
	Variables usadas [1] es: 0.461538
	Tiempo [1] es: 0.140532
	Iteraciones [1] es: 0
	Condiciones [1] es: 31

	Discriminantes [1] es: 1

	Descriptivas [1] es: 0

	Aciertos en la base [2] es: 1
	Aciertos en test [2] es: 1
	Numero de reglas [2] es: 2
	Variables por regla [2] es: 16.5
	Variables usadas [2] es: 0.512821
	Tiempo [2] es: 0.095833
	Iteraciones [2] es: 0
	Condiciones [2] es: 33

	Discriminantes [2] es: 1

	Descriptivas [2] es: 0

	Aciertos en la base [3] es: 1
	Aciertos en test [3] es: 1
	Numero de reglas [3] es: 2
	Variables por regla [3] es: 16
	Variables usadas [3] es: 0.410256
	Tiempo [3] es: 0.097238
	Iteraciones [3] es: 0
	Condiciones [3] es: 32

	Discriminantes [3] es: 1

	Descriptivas [3] es: 0

	Aciertos en la base [4] es: 1
	Aciertos en test [4] es: -nan
	Numero de reglas [4] es: 2
	Variables por regla [4] es: 13
	Variables usadas [4] es: 0.333333
	Tiempo [4] es: 0.100785
	Iteraciones [4] es: 0
	Condiciones [4] es: 26

	Discriminantes [4] es: -nan

	Descriptivas [4] es: -nan

	Aciertos en la base [5] es: 1
	Aciertos en test [5] es: 1
	Numero de reglas [5] es: 2
	Variables por regla [5] es: 15
	Variables usadas [5] es: 0.435897
	Tiempo [5] es: 0.095514
	Iteraciones [5] es: 0
	Condiciones [5] es: 30

	Discriminantes [5] es: 1

	Descriptivas [5] es: 0

	Aciertos en la base [6] es: 1
	Aciertos en test [6] es: -nan
	Numero de reglas [6] es: 2
	Variables por regla [6] es: 13
	Variables usadas [6] es: 0.333333
	Tiempo [6] es: 0.098598
	Iteraciones [6] es: 0
	Condiciones [6] es: 26

	Discriminantes [6] es: -nan

	Descriptivas [6] es: -nan

	Aciertos en la base [7] es: 1
	Aciertos en test [7] es: 1
	Numero de reglas [7] es: 2
	Variables por regla [7] es: 19
	Variables usadas [7] es: 0.564103
	Tiempo [7] es: 0.097442
	Iteraciones [7] es: 0
	Condiciones [7] es: 38

	Discriminantes [7] es: 1

	Descriptivas [7] es: 0

	Aciertos en la base [8] es: 1
	Aciertos en test [8] es: 1
	Numero de reglas [8] es: 2
	Variables por regla [8] es: 15.5
	Variables usadas [8] es: 0.461538
	Tiempo [8] es: 0.113329
	Iteraciones [8] es: 0
	Condiciones [8] es: 31

	Discriminantes [8] es: 1

	Descriptivas [8] es: 0

	Aciertos en la base [9] es: 1
	Aciertos en test [9] es: 1
	Numero de reglas [9] es: 2
	Variables por regla [9] es: 15
	Variables usadas [9] es: 0.435897
	Tiempo [9] es: 0.100797
	Iteraciones [9] es: 0
	Condiciones [9] es: 30

	Discriminantes [9] es: 1

	Descriptivas [9] es: 0

La media training: 1( 0)
La media test: -nan( -nan)
La media de reglas: 2( 0)
La media de variables por regla: 15.15( 1.77553)
La media de variables usadas: 0.428205( 0.074359)
La media de tiempo: 0.10379( 0.0131774)
La media de iteraciones: 0( 0)
La media de condiciones: 30.3( 3.55106)
La media de discriminantes: -nan( -nan)
La media de conjuntas: -nan( -nan)
Frecuencia de aparicion de las 39 distintas variables en las reglas: 
( distance ?Param_1 - city c2 - city ): --> 20 %
( distance ?Param_1 - city ?Param_2 - city ): --> 10 %
( distance ?Param_2 - city ?Param_1 - city ): --> 10 %
( distance c4 - city ?Param_2 - city ): --> 10 %
( distance ?Param_1 - city c3 - city ): --> 10 %
( distance ?Param_1 - city c4 - city ): --> 10 %
( distance c2 - city ?Param_2 - city ): --> 10 %
( distance c3 - city ?Param_1 - city ): --> 0 %
( distance c3 - city ?Param_2 - city ): --> 0 %
( distance c4 - city ?Param_1 - city ): --> 0 %
( distance ?Param_2 - city c2 - city ): --> 0 %
( distance ?Param_2 - city c3 - city ): --> 10 %
( slow-speed ?Param_0 - aircraft ): --> 0 %
( fast-speed ?Param_0 - aircraft ): --> 0 %
( slow-burn ?Param_0 - aircraft ): --> 100 %
( fast-burn ?Param_0 - aircraft ): --> 0 %
( capacity ?Param_0 - aircraft ): --> 0 %
( refuel-rate ?Param_0 - aircraft ): --> 100 %
( at ?Param_0 - aircraft ?Param_1 - city ): --> 100 %
( at ?Param_0 - aircraft c4 - city ): --> 100 %
( in p1 - person ?Param_0 - aircraft ): --> 10 %
( at p1 - person ?Param_1 - city ): --> 10 %
( fuel ?Param_0 - aircraft ): --> 100 %
( at p2 - person ?Param_2 - city ): --> 100 %
( distance c1 - city ?Param_2 - city ): --> 10 %
( distance c1 - city ?Param_1 - city ): --> 10 %
( distance ?Param_1 - city c1 - city ): --> 10 %
( distance ?Param_2 - city c4 - city ): --> 20 %
( at ?Param_0 - aircraft c1 - city ): --> 100 %
( distance ?Param_2 - city c1 - city ): --> 10 %
( at p1 - person ?Param_2 - city ): --> 100 %
( at ?Param_0 - aircraft ?Param_2 - city ): --> 100 %
( in p2 - person ?Param_0 - aircraft ): --> 10 %
( at p2 - person ?Param_1 - city ): --> 15 %
( at p3 - person ?Param_2 - city ): --> 100 %
( distance c2 - city ?Param_1 - city ): --> 20 %
( at ?Param_0 - aircraft c2 - city ): --> 100 %
( in p3 - person ?Param_0 - aircraft ): --> 100 %
( at p3 - person ?Param_1 - city ): --> 100 %
