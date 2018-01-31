Experiment: res/debark
mié jun  7 10:53:32 CEST 2017
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
Variables por regla: 16
Variables usadas: 0.571429
Tiempo: 0.058277
Iteraciones: 0
Condiciones: 32
Aciertos Discriminantes: -nan
Aciertos Descriptivas: -nan
----------------------------------------------------
------------------- REGLAS -------------------------
----------------------------------------------------
Descriptivas: 
IF
	( slow-burn ?Param_1 - aircraft ) = { True}
	( refuel-rate ?Param_1 - aircraft ) = { True}
	( at ?Param_1 - aircraft c4 - city ) = { False}
	( in ?Param_0 - person ?Param_1 - aircraft ) = { True}
	( at ?Param_0 - person c1 - city ) = { False}
	( at ?Param_1 - aircraft ?Param_2 - city ) = { True}
	( at ?Param_1 - aircraft c1 - city ) = { False}
	( at p1 - person ?Param_2 - city ) = { True}
	( in p1 - person ?Param_1 - aircraft ) = { False}
	( at ?Param_0 - person c2 - city ) = { False}
	( at ?Param_1 - aircraft c2 - city ) = { False}
	( at p2 - person ?Param_2 - city ) = { True}
	( in p2 - person ?Param_1 - aircraft ) = { False}
	( at ?Param_0 - person c3 - city ) = { False}
	( at ?Param_1 - aircraft c3 - city ) = { False}
	( at ?Param_0 - person ?Param_2 - city ) = { True}
THEN Class IS PreState   W 1 and N+= 3 and N-= 0

IF
	( slow-burn ?Param_1 - aircraft ) = { True}
	( refuel-rate ?Param_1 - aircraft ) = { True}
	( at ?Param_1 - aircraft c4 - city ) = { False}
	( in ?Param_0 - person ?Param_1 - aircraft ) = { False}
	( at ?Param_0 - person c1 - city ) = { False}
	( at ?Param_1 - aircraft ?Param_2 - city ) = { True}
	( at ?Param_1 - aircraft c1 - city ) = { False}
	( at p1 - person ?Param_2 - city ) = { True}
	( in p1 - person ?Param_1 - aircraft ) = { False}
	( at ?Param_0 - person c2 - city ) = { False}
	( at ?Param_1 - aircraft c2 - city ) = { False}
	( at p2 - person ?Param_2 - city ) = { True}
	( in p2 - person ?Param_1 - aircraft ) = { False}
	( at ?Param_0 - person c3 - city ) = { False}
	( at ?Param_1 - aircraft c3 - city ) = { False}
	( at ?Param_0 - person ?Param_2 - city ) = { True}
THEN Class IS PostState   W 1 and N+= 3 and N-= 0

Predictivas: 
IF
	( in ?Param_0 - person ?Param_1 - aircraft ) = { True}
THEN Class IS PreState   W 1 and N+= 3 and N-= 0

IF
	( in ?Param_0 - person ?Param_1 - aircraft ) = { False}
THEN Class IS PostState   W 1 and N+= 3 and N-= 0

Patrones: 
111111111?111?11?011??1111??11?1?1??11?1?1??1(0)
111111111?111?11?101??1111??11?1?1??11?1?1??1(1)

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
Variables por regla: 16
Variables usadas: 0.571429
Tiempo: 0.059174
Iteraciones: 0
Condiciones: 32
Aciertos Discriminantes: 1
Aciertos Descriptivas: 1
----------------------------------------------------
------------------- REGLAS -------------------------
----------------------------------------------------
Descriptivas: 
IF
	( slow-burn ?Param_1 - aircraft ) = { True}
	( refuel-rate ?Param_1 - aircraft ) = { True}
	( at ?Param_1 - aircraft c4 - city ) = { False}
	( in ?Param_0 - person ?Param_1 - aircraft ) = { True}
	( at ?Param_0 - person c1 - city ) = { False}
	( at ?Param_1 - aircraft ?Param_2 - city ) = { True}
	( at ?Param_1 - aircraft c1 - city ) = { False}
	( at p1 - person ?Param_2 - city ) = { True}
	( in p1 - person ?Param_1 - aircraft ) = { False}
	( at ?Param_0 - person c2 - city ) = { False}
	( at ?Param_1 - aircraft c2 - city ) = { False}
	( at p2 - person ?Param_2 - city ) = { True}
	( in p2 - person ?Param_1 - aircraft ) = { False}
	( at ?Param_0 - person c3 - city ) = { False}
	( at ?Param_1 - aircraft c3 - city ) = { False}
	( at ?Param_0 - person ?Param_2 - city ) = { True}
THEN Class IS PreState   W 1 and N+= 3 and N-= 0

IF
	( slow-burn ?Param_1 - aircraft ) = { True}
	( refuel-rate ?Param_1 - aircraft ) = { True}
	( at ?Param_1 - aircraft c4 - city ) = { False}
	( in ?Param_0 - person ?Param_1 - aircraft ) = { False}
	( at ?Param_0 - person c1 - city ) = { False}
	( at ?Param_1 - aircraft ?Param_2 - city ) = { True}
	( at ?Param_1 - aircraft c1 - city ) = { False}
	( at p1 - person ?Param_2 - city ) = { True}
	( in p1 - person ?Param_1 - aircraft ) = { False}
	( at ?Param_0 - person c2 - city ) = { False}
	( at ?Param_1 - aircraft c2 - city ) = { False}
	( at p2 - person ?Param_2 - city ) = { True}
	( in p2 - person ?Param_1 - aircraft ) = { True}
	( at ?Param_0 - person c3 - city ) = { True}
	( at ?Param_1 - aircraft c3 - city ) = { True}
	( at ?Param_0 - person ?Param_2 - city ) = { True}
THEN Class IS PostState   W 1 and N+= 2 and N-= 0

Predictivas: 
IF
	( in ?Param_0 - person ?Param_1 - aircraft ) = { True}
THEN Class IS PreState   W 1 and N+= 3 and N-= 0

IF
	( in ?Param_0 - person ?Param_1 - aircraft ) = { False}
THEN Class IS PostState   W 1 and N+= 2 and N-= 0

Patrones: 
111111111?111?11?011??1111??11?1?1??11?1?1??1(0)
111111111?111?11?101??1111??11?1?1??1?1?1?1?1(1)

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
Variables por regla: 16
Variables usadas: 0.571429
Tiempo: 0.061425
Iteraciones: 0
Condiciones: 32
Aciertos Discriminantes: -nan
Aciertos Descriptivas: -nan
----------------------------------------------------
------------------- REGLAS -------------------------
----------------------------------------------------
Descriptivas: 
IF
	( slow-burn ?Param_1 - aircraft ) = { True}
	( refuel-rate ?Param_1 - aircraft ) = { True}
	( at ?Param_1 - aircraft c4 - city ) = { False}
	( in ?Param_0 - person ?Param_1 - aircraft ) = { True}
	( at ?Param_0 - person c1 - city ) = { False}
	( at ?Param_1 - aircraft ?Param_2 - city ) = { True}
	( at ?Param_1 - aircraft c1 - city ) = { False}
	( at p1 - person ?Param_2 - city ) = { True}
	( in p1 - person ?Param_1 - aircraft ) = { False}
	( at ?Param_0 - person c2 - city ) = { False}
	( at ?Param_1 - aircraft c2 - city ) = { False}
	( at p2 - person ?Param_2 - city ) = { True}
	( in p2 - person ?Param_1 - aircraft ) = { False}
	( at ?Param_0 - person c3 - city ) = { False}
	( at ?Param_1 - aircraft c3 - city ) = { False}
	( at ?Param_0 - person ?Param_2 - city ) = { True}
THEN Class IS PreState   W 1 and N+= 3 and N-= 0

IF
	( slow-burn ?Param_1 - aircraft ) = { True}
	( refuel-rate ?Param_1 - aircraft ) = { True}
	( at ?Param_1 - aircraft c4 - city ) = { False}
	( in ?Param_0 - person ?Param_1 - aircraft ) = { False}
	( at ?Param_0 - person c1 - city ) = { False}
	( at ?Param_1 - aircraft ?Param_2 - city ) = { True}
	( at ?Param_1 - aircraft c1 - city ) = { False}
	( at p1 - person ?Param_2 - city ) = { True}
	( in p1 - person ?Param_1 - aircraft ) = { False}
	( at ?Param_0 - person c2 - city ) = { False}
	( at ?Param_1 - aircraft c2 - city ) = { False}
	( at p2 - person ?Param_2 - city ) = { True}
	( in p2 - person ?Param_1 - aircraft ) = { False}
	( at ?Param_0 - person c3 - city ) = { False}
	( at ?Param_1 - aircraft c3 - city ) = { False}
	( at ?Param_0 - person ?Param_2 - city ) = { True}
THEN Class IS PostState   W 1 and N+= 3 and N-= 0

Predictivas: 
IF
	( in ?Param_0 - person ?Param_1 - aircraft ) = { True}
THEN Class IS PreState   W 1 and N+= 3 and N-= 0

IF
	( in ?Param_0 - person ?Param_1 - aircraft ) = { False}
THEN Class IS PostState   W 1 and N+= 3 and N-= 0

Patrones: 
111111111?111?11?011??1111??11?1?1??11?1?1??1(0)
111111111?111?11?101??1111??11?1?1??11?1?1??1(1)

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
Variables por regla: 16
Variables usadas: 0.571429
Tiempo: 0.05946
Iteraciones: 0
Condiciones: 32
Aciertos Discriminantes: 1
Aciertos Descriptivas: 1
----------------------------------------------------
------------------- REGLAS -------------------------
----------------------------------------------------
Descriptivas: 
IF
	( slow-burn ?Param_1 - aircraft ) = { True}
	( refuel-rate ?Param_1 - aircraft ) = { True}
	( at ?Param_1 - aircraft c4 - city ) = { False}
	( in ?Param_0 - person ?Param_1 - aircraft ) = { False}
	( at ?Param_0 - person c1 - city ) = { False}
	( at ?Param_1 - aircraft ?Param_2 - city ) = { True}
	( at ?Param_1 - aircraft c1 - city ) = { False}
	( at p1 - person ?Param_2 - city ) = { True}
	( in p1 - person ?Param_1 - aircraft ) = { False}
	( at ?Param_0 - person c2 - city ) = { False}
	( at ?Param_1 - aircraft c2 - city ) = { False}
	( at p2 - person ?Param_2 - city ) = { True}
	( in p2 - person ?Param_1 - aircraft ) = { False}
	( at ?Param_0 - person c3 - city ) = { False}
	( at ?Param_1 - aircraft c3 - city ) = { False}
	( at ?Param_0 - person ?Param_2 - city ) = { True}
THEN Class IS PostState   W 1 and N+= 3 and N-= 0

IF
	( slow-burn ?Param_1 - aircraft ) = { True}
	( refuel-rate ?Param_1 - aircraft ) = { True}
	( at ?Param_1 - aircraft c4 - city ) = { False}
	( in ?Param_0 - person ?Param_1 - aircraft ) = { True}
	( at ?Param_0 - person c1 - city ) = { False}
	( at ?Param_1 - aircraft ?Param_2 - city ) = { True}
	( at ?Param_1 - aircraft c1 - city ) = { False}
	( at p1 - person ?Param_2 - city ) = { True}
	( in p1 - person ?Param_1 - aircraft ) = { False}
	( at ?Param_0 - person c2 - city ) = { True}
	( at ?Param_1 - aircraft c2 - city ) = { False}
	( at p2 - person ?Param_2 - city ) = { True}
	( in p2 - person ?Param_1 - aircraft ) = { False}
	( at ?Param_0 - person c3 - city ) = { False}
	( at ?Param_1 - aircraft c3 - city ) = { False}
	( at ?Param_0 - person ?Param_2 - city ) = { True}
THEN Class IS PreState   W 1 and N+= 2 and N-= 0

Predictivas: 
IF
	( in ?Param_0 - person ?Param_1 - aircraft ) = { False}
THEN Class IS PostState   W 1 and N+= 3 and N-= 0

IF
	( in ?Param_0 - person ?Param_1 - aircraft ) = { True}
THEN Class IS PreState   W 1 and N+= 2 and N-= 0

Patrones: 
111111111?111?11?101??1111??11?1?1??11?1?1??1(1)
111111111?111?11?011??1111??11??11??11?1?1??1(0)

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
Variables por regla: 16
Variables usadas: 0.571429
Tiempo: 0.062119
Iteraciones: 0
Condiciones: 32
Aciertos Discriminantes: -nan
Aciertos Descriptivas: -nan
----------------------------------------------------
------------------- REGLAS -------------------------
----------------------------------------------------
Descriptivas: 
IF
	( slow-burn ?Param_1 - aircraft ) = { True}
	( refuel-rate ?Param_1 - aircraft ) = { True}
	( at ?Param_1 - aircraft c4 - city ) = { False}
	( in ?Param_0 - person ?Param_1 - aircraft ) = { True}
	( at ?Param_0 - person c1 - city ) = { False}
	( at ?Param_1 - aircraft ?Param_2 - city ) = { True}
	( at ?Param_1 - aircraft c1 - city ) = { False}
	( at p1 - person ?Param_2 - city ) = { True}
	( in p1 - person ?Param_1 - aircraft ) = { False}
	( at ?Param_0 - person c2 - city ) = { False}
	( at ?Param_1 - aircraft c2 - city ) = { False}
	( at p2 - person ?Param_2 - city ) = { True}
	( in p2 - person ?Param_1 - aircraft ) = { False}
	( at ?Param_0 - person c3 - city ) = { False}
	( at ?Param_1 - aircraft c3 - city ) = { False}
	( at ?Param_0 - person ?Param_2 - city ) = { True}
THEN Class IS PreState   W 1 and N+= 3 and N-= 0

IF
	( slow-burn ?Param_1 - aircraft ) = { True}
	( refuel-rate ?Param_1 - aircraft ) = { True}
	( at ?Param_1 - aircraft c4 - city ) = { False}
	( in ?Param_0 - person ?Param_1 - aircraft ) = { False}
	( at ?Param_0 - person c1 - city ) = { False}
	( at ?Param_1 - aircraft ?Param_2 - city ) = { True}
	( at ?Param_1 - aircraft c1 - city ) = { False}
	( at p1 - person ?Param_2 - city ) = { True}
	( in p1 - person ?Param_1 - aircraft ) = { False}
	( at ?Param_0 - person c2 - city ) = { False}
	( at ?Param_1 - aircraft c2 - city ) = { False}
	( at p2 - person ?Param_2 - city ) = { True}
	( in p2 - person ?Param_1 - aircraft ) = { False}
	( at ?Param_0 - person c3 - city ) = { False}
	( at ?Param_1 - aircraft c3 - city ) = { False}
	( at ?Param_0 - person ?Param_2 - city ) = { True}
THEN Class IS PostState   W 1 and N+= 3 and N-= 0

Predictivas: 
IF
	( in ?Param_0 - person ?Param_1 - aircraft ) = { True}
THEN Class IS PreState   W 1 and N+= 3 and N-= 0

IF
	( in ?Param_0 - person ?Param_1 - aircraft ) = { False}
THEN Class IS PostState   W 1 and N+= 3 and N-= 0

Patrones: 
111111111?111?11?011??1111??11?1?1??11?1?1??1(0)
111111111?111?11?101??1111??11?1?1??11?1?1??1(1)

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
Variables por regla: 16
Variables usadas: 0.571429
Tiempo: 0.063863
Iteraciones: 0
Condiciones: 32
Aciertos Discriminantes: -nan
Aciertos Descriptivas: -nan
----------------------------------------------------
------------------- REGLAS -------------------------
----------------------------------------------------
Descriptivas: 
IF
	( slow-burn ?Param_1 - aircraft ) = { True}
	( refuel-rate ?Param_1 - aircraft ) = { True}
	( at ?Param_1 - aircraft c4 - city ) = { False}
	( in ?Param_0 - person ?Param_1 - aircraft ) = { True}
	( at ?Param_0 - person c1 - city ) = { False}
	( at ?Param_1 - aircraft ?Param_2 - city ) = { True}
	( at ?Param_1 - aircraft c1 - city ) = { False}
	( at p1 - person ?Param_2 - city ) = { True}
	( in p1 - person ?Param_1 - aircraft ) = { False}
	( at ?Param_0 - person c2 - city ) = { False}
	( at ?Param_1 - aircraft c2 - city ) = { False}
	( at p2 - person ?Param_2 - city ) = { True}
	( in p2 - person ?Param_1 - aircraft ) = { False}
	( at ?Param_0 - person c3 - city ) = { False}
	( at ?Param_1 - aircraft c3 - city ) = { False}
	( at ?Param_0 - person ?Param_2 - city ) = { True}
THEN Class IS PreState   W 1 and N+= 3 and N-= 0

IF
	( slow-burn ?Param_1 - aircraft ) = { True}
	( refuel-rate ?Param_1 - aircraft ) = { True}
	( at ?Param_1 - aircraft c4 - city ) = { False}
	( in ?Param_0 - person ?Param_1 - aircraft ) = { False}
	( at ?Param_0 - person c1 - city ) = { False}
	( at ?Param_1 - aircraft ?Param_2 - city ) = { True}
	( at ?Param_1 - aircraft c1 - city ) = { False}
	( at p1 - person ?Param_2 - city ) = { True}
	( in p1 - person ?Param_1 - aircraft ) = { False}
	( at ?Param_0 - person c2 - city ) = { False}
	( at ?Param_1 - aircraft c2 - city ) = { False}
	( at p2 - person ?Param_2 - city ) = { True}
	( in p2 - person ?Param_1 - aircraft ) = { False}
	( at ?Param_0 - person c3 - city ) = { False}
	( at ?Param_1 - aircraft c3 - city ) = { False}
	( at ?Param_0 - person ?Param_2 - city ) = { True}
THEN Class IS PostState   W 1 and N+= 3 and N-= 0

Predictivas: 
IF
	( in ?Param_0 - person ?Param_1 - aircraft ) = { True}
THEN Class IS PreState   W 1 and N+= 3 and N-= 0

IF
	( in ?Param_0 - person ?Param_1 - aircraft ) = { False}
THEN Class IS PostState   W 1 and N+= 3 and N-= 0

Patrones: 
111111111?111?11?011??1111??11?1?1??11?1?1??1(0)
111111111?111?11?101??1111??11?1?1??11?1?1??1(1)

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
Variables por regla: 16
Variables usadas: 0.571429
Tiempo: 0.05887
Iteraciones: 0
Condiciones: 32
Aciertos Discriminantes: -nan
Aciertos Descriptivas: -nan
----------------------------------------------------
------------------- REGLAS -------------------------
----------------------------------------------------
Descriptivas: 
IF
	( slow-burn ?Param_1 - aircraft ) = { True}
	( refuel-rate ?Param_1 - aircraft ) = { True}
	( at ?Param_1 - aircraft c4 - city ) = { False}
	( in ?Param_0 - person ?Param_1 - aircraft ) = { True}
	( at ?Param_0 - person c1 - city ) = { False}
	( at ?Param_1 - aircraft ?Param_2 - city ) = { True}
	( at ?Param_1 - aircraft c1 - city ) = { False}
	( at p1 - person ?Param_2 - city ) = { True}
	( in p1 - person ?Param_1 - aircraft ) = { False}
	( at ?Param_0 - person c2 - city ) = { False}
	( at ?Param_1 - aircraft c2 - city ) = { False}
	( at p2 - person ?Param_2 - city ) = { True}
	( in p2 - person ?Param_1 - aircraft ) = { False}
	( at ?Param_0 - person c3 - city ) = { False}
	( at ?Param_1 - aircraft c3 - city ) = { False}
	( at ?Param_0 - person ?Param_2 - city ) = { True}
THEN Class IS PreState   W 1 and N+= 3 and N-= 0

IF
	( slow-burn ?Param_1 - aircraft ) = { True}
	( refuel-rate ?Param_1 - aircraft ) = { True}
	( at ?Param_1 - aircraft c4 - city ) = { False}
	( in ?Param_0 - person ?Param_1 - aircraft ) = { False}
	( at ?Param_0 - person c1 - city ) = { False}
	( at ?Param_1 - aircraft ?Param_2 - city ) = { True}
	( at ?Param_1 - aircraft c1 - city ) = { False}
	( at p1 - person ?Param_2 - city ) = { True}
	( in p1 - person ?Param_1 - aircraft ) = { False}
	( at ?Param_0 - person c2 - city ) = { False}
	( at ?Param_1 - aircraft c2 - city ) = { False}
	( at p2 - person ?Param_2 - city ) = { True}
	( in p2 - person ?Param_1 - aircraft ) = { False}
	( at ?Param_0 - person c3 - city ) = { False}
	( at ?Param_1 - aircraft c3 - city ) = { False}
	( at ?Param_0 - person ?Param_2 - city ) = { True}
THEN Class IS PostState   W 1 and N+= 3 and N-= 0

Predictivas: 
IF
	( in ?Param_0 - person ?Param_1 - aircraft ) = { True}
THEN Class IS PreState   W 1 and N+= 3 and N-= 0

IF
	( in ?Param_0 - person ?Param_1 - aircraft ) = { False}
THEN Class IS PostState   W 1 and N+= 3 and N-= 0

Patrones: 
111111111?111?11?011??1111??11?1?1??11?1?1??1(0)
111111111?111?11?101??1111??11?1?1??11?1?1??1(1)

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
Variables por regla: 16.5
Variables usadas: 0.607143
Tiempo: 0.056873
Iteraciones: 0
Condiciones: 33
Aciertos Discriminantes: 1
Aciertos Descriptivas: 1
----------------------------------------------------
------------------- REGLAS -------------------------
----------------------------------------------------
Descriptivas: 
IF
	( slow-burn ?Param_1 - aircraft ) = { True}
	( refuel-rate ?Param_1 - aircraft ) = { True}
	( at ?Param_1 - aircraft c4 - city ) = { False}
	( in ?Param_0 - person ?Param_1 - aircraft ) = { True}
	( at ?Param_0 - person c1 - city ) = { False}
	( at ?Param_1 - aircraft ?Param_2 - city ) = { True}
	( at ?Param_1 - aircraft c1 - city ) = { False}
	( at p1 - person ?Param_2 - city ) = { True}
	( in p1 - person ?Param_1 - aircraft ) = { False}
	( at ?Param_0 - person c2 - city ) = { False}
	( at ?Param_1 - aircraft c2 - city ) = { False}
	( at p2 - person ?Param_2 - city ) = { True}
	( in p2 - person ?Param_1 - aircraft ) = { True}
	( at ?Param_0 - person c3 - city ) = { True}
	( at ?Param_1 - aircraft c3 - city ) = { True}
	( at ?Param_0 - person ?Param_2 - city ) = { True}
THEN Class IS PreState   W 1 and N+= 2 and N-= 0

IF
	( slow-burn ?Param_1 - aircraft ) = { True}
	( refuel-rate ?Param_1 - aircraft ) = { True}
	( at ?Param_1 - aircraft c4 - city ) = { False}
	( in ?Param_0 - person ?Param_1 - aircraft ) = { False}
	( at ?Param_0 - person c1 - city ) = { True}
	( at ?Param_1 - aircraft ?Param_2 - city ) = { True}
	( fuel ?Param_1 - aircraft ) = { L0/1}
	( at ?Param_1 - aircraft c1 - city ) = { False}
	( at p1 - person ?Param_2 - city ) = { True}
	( in p1 - person ?Param_1 - aircraft ) = { False}
	( at ?Param_0 - person c2 - city ) = { False}
	( at ?Param_1 - aircraft c2 - city ) = { False}
	( at p2 - person ?Param_2 - city ) = { True}
	( in p2 - person ?Param_1 - aircraft ) = { False}
	( at ?Param_0 - person c3 - city ) = { False}
	( at ?Param_1 - aircraft c3 - city ) = { False}
	( at ?Param_0 - person ?Param_2 - city ) = { True}
THEN Class IS PostState   W 1 and N+= 2 and N-= 0

Predictivas: 
IF
	( in ?Param_0 - person ?Param_1 - aircraft ) = { True}
THEN Class IS PreState   W 1 and N+= 2 and N-= 0

IF
	( in ?Param_0 - person ?Param_1 - aircraft ) = { False}
THEN Class IS PostState   W 1 and N+= 2 and N-= 0

Patrones: 
111111111?111?11?011??1111??11?1?1??1?1?1?1?1(0)
111111111?111?11?10?1?11?1??11?1?1??11?1?1??1(1)

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
Variables por regla: 16.5
Variables usadas: 0.607143
Tiempo: 0.061253
Iteraciones: 0
Condiciones: 33
Aciertos Discriminantes: 1
Aciertos Descriptivas: 1
----------------------------------------------------
------------------- REGLAS -------------------------
----------------------------------------------------
Descriptivas: 
IF
	( slow-burn ?Param_1 - aircraft ) = { True}
	( refuel-rate ?Param_1 - aircraft ) = { True}
	( at ?Param_1 - aircraft c4 - city ) = { False}
	( in ?Param_0 - person ?Param_1 - aircraft ) = { False}
	( at ?Param_0 - person c1 - city ) = { False}
	( at ?Param_1 - aircraft ?Param_2 - city ) = { True}
	( at ?Param_1 - aircraft c1 - city ) = { False}
	( at p1 - person ?Param_2 - city ) = { True}
	( in p1 - person ?Param_1 - aircraft ) = { False}
	( at ?Param_0 - person c2 - city ) = { False}
	( at ?Param_1 - aircraft c2 - city ) = { False}
	( at p2 - person ?Param_2 - city ) = { True}
	( in p2 - person ?Param_1 - aircraft ) = { False}
	( at ?Param_0 - person c3 - city ) = { False}
	( at ?Param_1 - aircraft c3 - city ) = { False}
	( at ?Param_0 - person ?Param_2 - city ) = { True}
THEN Class IS PostState   W 1 and N+= 3 and N-= 0

IF
	( slow-burn ?Param_1 - aircraft ) = { True}
	( refuel-rate ?Param_1 - aircraft ) = { True}
	( at ?Param_1 - aircraft c4 - city ) = { False}
	( in ?Param_0 - person ?Param_1 - aircraft ) = { True}
	( at ?Param_0 - person c1 - city ) = { True}
	( at ?Param_1 - aircraft ?Param_2 - city ) = { True}
	( fuel ?Param_1 - aircraft ) = { L0/1}
	( at ?Param_1 - aircraft c1 - city ) = { False}
	( at p1 - person ?Param_2 - city ) = { True}
	( in p1 - person ?Param_1 - aircraft ) = { False}
	( at ?Param_0 - person c2 - city ) = { False}
	( at ?Param_1 - aircraft c2 - city ) = { False}
	( at p2 - person ?Param_2 - city ) = { True}
	( in p2 - person ?Param_1 - aircraft ) = { False}
	( at ?Param_0 - person c3 - city ) = { False}
	( at ?Param_1 - aircraft c3 - city ) = { False}
	( at ?Param_0 - person ?Param_2 - city ) = { True}
THEN Class IS PreState   W 1 and N+= 2 and N-= 0

Predictivas: 
IF
	( in ?Param_0 - person ?Param_1 - aircraft ) = { False}
THEN Class IS PostState   W 1 and N+= 3 and N-= 0

IF
	( in ?Param_0 - person ?Param_1 - aircraft ) = { True}
THEN Class IS PreState   W 1 and N+= 2 and N-= 0

Patrones: 
111111111?111?11?101??1111??11?1?1??11?1?1??1(1)
111111111?111?11?01?1?11?1??11?1?1??11?1?1??1(0)

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
Variables por regla: 16
Variables usadas: 0.571429
Tiempo: 0.056582
Iteraciones: 0
Condiciones: 32
Aciertos Discriminantes: 1
Aciertos Descriptivas: 1
----------------------------------------------------
------------------- REGLAS -------------------------
----------------------------------------------------
Descriptivas: 
IF
	( slow-burn ?Param_1 - aircraft ) = { True}
	( refuel-rate ?Param_1 - aircraft ) = { True}
	( at ?Param_1 - aircraft c4 - city ) = { False}
	( in ?Param_0 - person ?Param_1 - aircraft ) = { True}
	( at ?Param_0 - person c1 - city ) = { False}
	( at ?Param_1 - aircraft ?Param_2 - city ) = { True}
	( at ?Param_1 - aircraft c1 - city ) = { False}
	( at p1 - person ?Param_2 - city ) = { True}
	( in p1 - person ?Param_1 - aircraft ) = { False}
	( at ?Param_0 - person c2 - city ) = { False}
	( at ?Param_1 - aircraft c2 - city ) = { False}
	( at p2 - person ?Param_2 - city ) = { True}
	( in p2 - person ?Param_1 - aircraft ) = { False}
	( at ?Param_0 - person c3 - city ) = { False}
	( at ?Param_1 - aircraft c3 - city ) = { False}
	( at ?Param_0 - person ?Param_2 - city ) = { True}
THEN Class IS PreState   W 1 and N+= 3 and N-= 0

IF
	( slow-burn ?Param_1 - aircraft ) = { True}
	( refuel-rate ?Param_1 - aircraft ) = { True}
	( at ?Param_1 - aircraft c4 - city ) = { False}
	( in ?Param_0 - person ?Param_1 - aircraft ) = { False}
	( at ?Param_0 - person c1 - city ) = { False}
	( at ?Param_1 - aircraft ?Param_2 - city ) = { True}
	( at ?Param_1 - aircraft c1 - city ) = { False}
	( at p1 - person ?Param_2 - city ) = { True}
	( in p1 - person ?Param_1 - aircraft ) = { False}
	( at ?Param_0 - person c2 - city ) = { True}
	( at ?Param_1 - aircraft c2 - city ) = { False}
	( at p2 - person ?Param_2 - city ) = { True}
	( in p2 - person ?Param_1 - aircraft ) = { False}
	( at ?Param_0 - person c3 - city ) = { False}
	( at ?Param_1 - aircraft c3 - city ) = { False}
	( at ?Param_0 - person ?Param_2 - city ) = { True}
THEN Class IS PostState   W 1 and N+= 2 and N-= 0

Predictivas: 
IF
	( in ?Param_0 - person ?Param_1 - aircraft ) = { True}
THEN Class IS PreState   W 1 and N+= 3 and N-= 0

IF
	( in ?Param_0 - person ?Param_1 - aircraft ) = { False}
THEN Class IS PostState   W 1 and N+= 2 and N-= 0

Patrones: 
111111111?111?11?011??1111??11?1?1??11?1?1??1(0)
111111111?111?11?101??1111??11??11??11?1?1??1(1)
	Aciertos en la base [0] es: 1
	Aciertos en test [0] es: -nan
	Numero de reglas [0] es: 2
	Variables por regla [0] es: 16
	Variables usadas [0] es: 0.571429
	Tiempo [0] es: 0.058277
	Iteraciones [0] es: 0
	Condiciones [0] es: 32

	Discriminantes [0] es: -nan

	Descriptivas [0] es: -nan

	Aciertos en la base [1] es: 1
	Aciertos en test [1] es: 1
	Numero de reglas [1] es: 2
	Variables por regla [1] es: 16
	Variables usadas [1] es: 0.571429
	Tiempo [1] es: 0.059174
	Iteraciones [1] es: 0
	Condiciones [1] es: 32

	Discriminantes [1] es: 1

	Descriptivas [1] es: 0

	Aciertos en la base [2] es: 1
	Aciertos en test [2] es: -nan
	Numero de reglas [2] es: 2
	Variables por regla [2] es: 16
	Variables usadas [2] es: 0.571429
	Tiempo [2] es: 0.061425
	Iteraciones [2] es: 0
	Condiciones [2] es: 32

	Discriminantes [2] es: -nan

	Descriptivas [2] es: -nan

	Aciertos en la base [3] es: 1
	Aciertos en test [3] es: 1
	Numero de reglas [3] es: 2
	Variables por regla [3] es: 16
	Variables usadas [3] es: 0.571429
	Tiempo [3] es: 0.05946
	Iteraciones [3] es: 0
	Condiciones [3] es: 32

	Discriminantes [3] es: 1

	Descriptivas [3] es: 0

	Aciertos en la base [4] es: 1
	Aciertos en test [4] es: -nan
	Numero de reglas [4] es: 2
	Variables por regla [4] es: 16
	Variables usadas [4] es: 0.571429
	Tiempo [4] es: 0.062119
	Iteraciones [4] es: 0
	Condiciones [4] es: 32

	Discriminantes [4] es: -nan

	Descriptivas [4] es: -nan

	Aciertos en la base [5] es: 1
	Aciertos en test [5] es: -nan
	Numero de reglas [5] es: 2
	Variables por regla [5] es: 16
	Variables usadas [5] es: 0.571429
	Tiempo [5] es: 0.063863
	Iteraciones [5] es: 0
	Condiciones [5] es: 32

	Discriminantes [5] es: -nan

	Descriptivas [5] es: -nan

	Aciertos en la base [6] es: 1
	Aciertos en test [6] es: -nan
	Numero de reglas [6] es: 2
	Variables por regla [6] es: 16
	Variables usadas [6] es: 0.571429
	Tiempo [6] es: 0.05887
	Iteraciones [6] es: 0
	Condiciones [6] es: 32

	Discriminantes [6] es: -nan

	Descriptivas [6] es: -nan

	Aciertos en la base [7] es: 1
	Aciertos en test [7] es: 1
	Numero de reglas [7] es: 2
	Variables por regla [7] es: 16.5
	Variables usadas [7] es: 0.607143
	Tiempo [7] es: 0.056873
	Iteraciones [7] es: 0
	Condiciones [7] es: 33

	Discriminantes [7] es: 1

	Descriptivas [7] es: 0

	Aciertos en la base [8] es: 1
	Aciertos en test [8] es: 1
	Numero de reglas [8] es: 2
	Variables por regla [8] es: 16.5
	Variables usadas [8] es: 0.607143
	Tiempo [8] es: 0.061253
	Iteraciones [8] es: 0
	Condiciones [8] es: 33

	Discriminantes [8] es: 1

	Descriptivas [8] es: 0

	Aciertos en la base [9] es: 1
	Aciertos en test [9] es: 1
	Numero de reglas [9] es: 2
	Variables por regla [9] es: 16
	Variables usadas [9] es: 0.571429
	Tiempo [9] es: 0.056582
	Iteraciones [9] es: 0
	Condiciones [9] es: 32

	Discriminantes [9] es: 1

	Descriptivas [9] es: 0

La media training: 1( 0)
La media test: -nan( -nan)
La media de reglas: 2( 0)
La media de variables por regla: 16.1( 0.2)
La media de variables usadas: 0.578571( 0.0142857)
La media de tiempo: 0.0597896( 0.00221926)
La media de iteraciones: 0( 0)
La media de condiciones: 32.2( 0.4)
La media de discriminantes: -nan( -nan)
La media de conjuntas: -nan( -nan)
Frecuencia de aparicion de las 28 distintas variables en las reglas: 
( distance c1 - city ?Param_2 - city ): --> 0 %
( distance ?Param_2 - city c1 - city ): --> 0 %
( distance c4 - city ?Param_2 - city ): --> 0 %
( distance c2 - city ?Param_2 - city ): --> 0 %
( distance c3 - city ?Param_2 - city ): --> 0 %
( distance ?Param_2 - city c2 - city ): --> 0 %
( distance ?Param_2 - city c3 - city ): --> 0 %
( slow-speed ?Param_1 - aircraft ): --> 0 %
( fast-speed ?Param_1 - aircraft ): --> 0 %
( slow-burn ?Param_1 - aircraft ): --> 100 %
( fast-burn ?Param_1 - aircraft ): --> 0 %
( capacity ?Param_1 - aircraft ): --> 0 %
( refuel-rate ?Param_1 - aircraft ): --> 100 %
( at ?Param_1 - aircraft c4 - city ): --> 100 %
( in ?Param_0 - person ?Param_1 - aircraft ): --> 100 %
( at ?Param_0 - person c1 - city ): --> 100 %
( at ?Param_1 - aircraft ?Param_2 - city ): --> 100 %
( fuel ?Param_1 - aircraft ): --> 10 %
( at ?Param_1 - aircraft c1 - city ): --> 100 %
( at p1 - person ?Param_2 - city ): --> 100 %
( in p1 - person ?Param_1 - aircraft ): --> 100 %
( at ?Param_0 - person c2 - city ): --> 100 %
( at ?Param_1 - aircraft c2 - city ): --> 100 %
( at p2 - person ?Param_2 - city ): --> 100 %
( in p2 - person ?Param_1 - aircraft ): --> 100 %
( at ?Param_0 - person c3 - city ): --> 100 %
( at ?Param_1 - aircraft c3 - city ): --> 100 %
( at ?Param_0 - person ?Param_2 - city ): --> 100 %
