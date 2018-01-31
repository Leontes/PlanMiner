Experiment: res/fly
mié jun  7 10:53:35 CEST 2017
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
Variables por regla: 6
Variables usadas: 0.285714
Tiempo: 0.038969
Iteraciones: 0
Condiciones: 12
Aciertos Discriminantes: -nan
Aciertos Descriptivas: -nan
----------------------------------------------------
------------------- REGLAS -------------------------
----------------------------------------------------
Descriptivas: 
IF
	( at p1 - person ?Param_2 - city ) = { True}
	( at ?Param_0 - aircraft ?Param_1 - city ) = { True}
	( fuel ?Param_0 - aircraft ) = { L1}
	( slow-burn ?Param_0 - aircraft ) = { True}
	( refuel-rate ?Param_0 - aircraft ) = { True}
	( at ?Param_0 - aircraft ?Param_2 - city ) = { True}
THEN Class IS PreState   W 1 and N+= 1 and N-= 0

IF
	( at p1 - person ?Param_2 - city ) = { True}
	( at ?Param_0 - aircraft ?Param_1 - city ) = { False}
	( fuel ?Param_0 - aircraft ) = { L0/1}
	( slow-burn ?Param_0 - aircraft ) = { True}
	( refuel-rate ?Param_0 - aircraft ) = { True}
	( at ?Param_0 - aircraft ?Param_2 - city ) = { True}
THEN Class IS PostState   W 1 and N+= 1 and N-= 0

Predictivas: 
IF
	( fuel ?Param_0 - aircraft ) = { L1}
THEN Class IS PreState   W 1 and N+= 1 and N-= 0

IF
	( at ?Param_0 - aircraft ?Param_1 - city ) = { False}
THEN Class IS PostState   W 1 and N+= 1 and N-= 0

Patrones: 
?1?1011111111111111?111?1?1(0)
?1101?1111111111111?111?1?1(1)

===============
 Ejecucion 1
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
Variables por regla: 6
Variables usadas: 0.285714
Tiempo: 0.037719
Iteraciones: 0
Condiciones: 12
Aciertos Discriminantes: -nan
Aciertos Descriptivas: -nan
----------------------------------------------------
------------------- REGLAS -------------------------
----------------------------------------------------
Descriptivas: 
IF
	( at p1 - person ?Param_2 - city ) = { True}
	( at ?Param_0 - aircraft ?Param_1 - city ) = { True}
	( fuel ?Param_0 - aircraft ) = { L1}
	( slow-burn ?Param_0 - aircraft ) = { True}
	( refuel-rate ?Param_0 - aircraft ) = { True}
	( at ?Param_0 - aircraft ?Param_2 - city ) = { True}
THEN Class IS PreState   W 1 and N+= 1 and N-= 0

IF
	( at p1 - person ?Param_2 - city ) = { True}
	( at ?Param_0 - aircraft ?Param_1 - city ) = { False}
	( fuel ?Param_0 - aircraft ) = { L0/1}
	( slow-burn ?Param_0 - aircraft ) = { True}
	( refuel-rate ?Param_0 - aircraft ) = { True}
	( at ?Param_0 - aircraft ?Param_2 - city ) = { True}
THEN Class IS PostState   W 1 and N+= 1 and N-= 0

Predictivas: 
IF
	( fuel ?Param_0 - aircraft ) = { L1}
THEN Class IS PreState   W 1 and N+= 1 and N-= 0

IF
	( at ?Param_0 - aircraft ?Param_1 - city ) = { False}
THEN Class IS PostState   W 1 and N+= 1 and N-= 0

Patrones: 
?1?1011111111111111?111?1?1(0)
?1101?1111111111111?111?1?1(1)

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
Variables por regla: 6
Variables usadas: 0.285714
Tiempo: 0.036476
Iteraciones: 0
Condiciones: 12
Aciertos Discriminantes: -nan
Aciertos Descriptivas: -nan
----------------------------------------------------
------------------- REGLAS -------------------------
----------------------------------------------------
Descriptivas: 
IF
	( at p1 - person ?Param_2 - city ) = { True}
	( at ?Param_0 - aircraft ?Param_1 - city ) = { True}
	( fuel ?Param_0 - aircraft ) = { L1}
	( slow-burn ?Param_0 - aircraft ) = { True}
	( refuel-rate ?Param_0 - aircraft ) = { True}
	( at ?Param_0 - aircraft ?Param_2 - city ) = { True}
THEN Class IS PreState   W 1 and N+= 1 and N-= 0

IF
	( at p1 - person ?Param_2 - city ) = { True}
	( at ?Param_0 - aircraft ?Param_1 - city ) = { False}
	( fuel ?Param_0 - aircraft ) = { L0/1}
	( slow-burn ?Param_0 - aircraft ) = { True}
	( refuel-rate ?Param_0 - aircraft ) = { True}
	( at ?Param_0 - aircraft ?Param_2 - city ) = { True}
THEN Class IS PostState   W 1 and N+= 1 and N-= 0

Predictivas: 
IF
	( fuel ?Param_0 - aircraft ) = { L1}
THEN Class IS PreState   W 1 and N+= 1 and N-= 0

IF
	( at ?Param_0 - aircraft ?Param_1 - city ) = { False}
THEN Class IS PostState   W 1 and N+= 1 and N-= 0

Patrones: 
?1?1011111111111111?111?1?1(0)
?1101?1111111111111?111?1?1(1)

===============
 Ejecucion 3
===============
Aciertos: 0
Errores:  1
No clasificados: 0
	Rule 0(PreState): 0 / 1
Aciertos: 0
Errores:  0
No clasificados: 1
	Rule 0(PreState): 0 / 0
----------------------------------------------------
Aciertos Training: 1
Aciertos Test: 0
Numero de reglas: 1
Variables por regla: 6
Variables usadas: 0.285714
Tiempo: 0.034036
Iteraciones: 0
Condiciones: 6
Aciertos Discriminantes: 0
Aciertos Descriptivas: 0
----------------------------------------------------
------------------- REGLAS -------------------------
----------------------------------------------------
Descriptivas: 
IF
	( at p1 - person ?Param_2 - city ) = { True}
	( at ?Param_0 - aircraft ?Param_1 - city ) = { True}
	( fuel ?Param_0 - aircraft ) = { L1}
	( slow-burn ?Param_0 - aircraft ) = { True}
	( refuel-rate ?Param_0 - aircraft ) = { True}
	( at ?Param_0 - aircraft ?Param_2 - city ) = { True}
THEN Class IS PreState   W 1 and N+= 1 and N-= 0

Predictivas: 
IF
THEN Class IS PreState   W 1 and N+= 1 and N-= 0

Patrones: 
?1?1?11111111111111?111?1?1(0)

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
Variables por regla: 6
Variables usadas: 0.285714
Tiempo: 0.037822
Iteraciones: 0
Condiciones: 12
Aciertos Discriminantes: -nan
Aciertos Descriptivas: -nan
----------------------------------------------------
------------------- REGLAS -------------------------
----------------------------------------------------
Descriptivas: 
IF
	( at p1 - person ?Param_2 - city ) = { True}
	( at ?Param_0 - aircraft ?Param_1 - city ) = { True}
	( fuel ?Param_0 - aircraft ) = { L1}
	( slow-burn ?Param_0 - aircraft ) = { True}
	( refuel-rate ?Param_0 - aircraft ) = { True}
	( at ?Param_0 - aircraft ?Param_2 - city ) = { True}
THEN Class IS PreState   W 1 and N+= 1 and N-= 0

IF
	( at p1 - person ?Param_2 - city ) = { True}
	( at ?Param_0 - aircraft ?Param_1 - city ) = { False}
	( fuel ?Param_0 - aircraft ) = { L0/1}
	( slow-burn ?Param_0 - aircraft ) = { True}
	( refuel-rate ?Param_0 - aircraft ) = { True}
	( at ?Param_0 - aircraft ?Param_2 - city ) = { True}
THEN Class IS PostState   W 1 and N+= 1 and N-= 0

Predictivas: 
IF
	( fuel ?Param_0 - aircraft ) = { L1}
THEN Class IS PreState   W 1 and N+= 1 and N-= 0

IF
	( fuel ?Param_0 - aircraft ) = { L0/1}
THEN Class IS PostState   W 1 and N+= 1 and N-= 0

Patrones: 
?1?1011111111111111?111?1?1(0)
?11?101111111111111?111?1?1(1)

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
Variables por regla: 6
Variables usadas: 0.285714
Tiempo: 0.037205
Iteraciones: 0
Condiciones: 12
Aciertos Discriminantes: -nan
Aciertos Descriptivas: -nan
----------------------------------------------------
------------------- REGLAS -------------------------
----------------------------------------------------
Descriptivas: 
IF
	( at p1 - person ?Param_2 - city ) = { True}
	( at ?Param_0 - aircraft ?Param_1 - city ) = { True}
	( fuel ?Param_0 - aircraft ) = { L1}
	( slow-burn ?Param_0 - aircraft ) = { True}
	( refuel-rate ?Param_0 - aircraft ) = { True}
	( at ?Param_0 - aircraft ?Param_2 - city ) = { True}
THEN Class IS PreState   W 1 and N+= 1 and N-= 0

IF
	( at p1 - person ?Param_2 - city ) = { True}
	( at ?Param_0 - aircraft ?Param_1 - city ) = { False}
	( fuel ?Param_0 - aircraft ) = { L0/1}
	( slow-burn ?Param_0 - aircraft ) = { True}
	( refuel-rate ?Param_0 - aircraft ) = { True}
	( at ?Param_0 - aircraft ?Param_2 - city ) = { True}
THEN Class IS PostState   W 1 and N+= 1 and N-= 0

Predictivas: 
IF
	( fuel ?Param_0 - aircraft ) = { L1}
THEN Class IS PreState   W 1 and N+= 1 and N-= 0

IF
	( at ?Param_0 - aircraft ?Param_1 - city ) = { False}
THEN Class IS PostState   W 1 and N+= 1 and N-= 0

Patrones: 
?1?1011111111111111?111?1?1(0)
?1101?1111111111111?111?1?1(1)

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
Variables por regla: 6
Variables usadas: 0.285714
Tiempo: 0.036757
Iteraciones: 0
Condiciones: 12
Aciertos Discriminantes: -nan
Aciertos Descriptivas: -nan
----------------------------------------------------
------------------- REGLAS -------------------------
----------------------------------------------------
Descriptivas: 
IF
	( at p1 - person ?Param_2 - city ) = { True}
	( at ?Param_0 - aircraft ?Param_1 - city ) = { True}
	( fuel ?Param_0 - aircraft ) = { L1}
	( slow-burn ?Param_0 - aircraft ) = { True}
	( refuel-rate ?Param_0 - aircraft ) = { True}
	( at ?Param_0 - aircraft ?Param_2 - city ) = { True}
THEN Class IS PreState   W 1 and N+= 1 and N-= 0

IF
	( at p1 - person ?Param_2 - city ) = { True}
	( at ?Param_0 - aircraft ?Param_1 - city ) = { False}
	( fuel ?Param_0 - aircraft ) = { L0/1}
	( slow-burn ?Param_0 - aircraft ) = { True}
	( refuel-rate ?Param_0 - aircraft ) = { True}
	( at ?Param_0 - aircraft ?Param_2 - city ) = { True}
THEN Class IS PostState   W 1 and N+= 1 and N-= 0

Predictivas: 
IF
	( fuel ?Param_0 - aircraft ) = { L1}
THEN Class IS PreState   W 1 and N+= 1 and N-= 0

IF
	( fuel ?Param_0 - aircraft ) = { L0/1}
THEN Class IS PostState   W 1 and N+= 1 and N-= 0

Patrones: 
?1?1011111111111111?111?1?1(0)
?11?101111111111111?111?1?1(1)

===============
 Ejecucion 7
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
Variables por regla: 6
Variables usadas: 0.285714
Tiempo: 0.03553
Iteraciones: 0
Condiciones: 12
Aciertos Discriminantes: -nan
Aciertos Descriptivas: -nan
----------------------------------------------------
------------------- REGLAS -------------------------
----------------------------------------------------
Descriptivas: 
IF
	( at p1 - person ?Param_2 - city ) = { True}
	( at ?Param_0 - aircraft ?Param_1 - city ) = { True}
	( fuel ?Param_0 - aircraft ) = { L1}
	( slow-burn ?Param_0 - aircraft ) = { True}
	( refuel-rate ?Param_0 - aircraft ) = { True}
	( at ?Param_0 - aircraft ?Param_2 - city ) = { True}
THEN Class IS PreState   W 1 and N+= 1 and N-= 0

IF
	( at p1 - person ?Param_2 - city ) = { True}
	( at ?Param_0 - aircraft ?Param_1 - city ) = { False}
	( fuel ?Param_0 - aircraft ) = { L0/1}
	( slow-burn ?Param_0 - aircraft ) = { True}
	( refuel-rate ?Param_0 - aircraft ) = { True}
	( at ?Param_0 - aircraft ?Param_2 - city ) = { True}
THEN Class IS PostState   W 1 and N+= 1 and N-= 0

Predictivas: 
IF
	( fuel ?Param_0 - aircraft ) = { L1}
THEN Class IS PreState   W 1 and N+= 1 and N-= 0

IF
	( fuel ?Param_0 - aircraft ) = { L0/1}
THEN Class IS PostState   W 1 and N+= 1 and N-= 0

Patrones: 
?1?1011111111111111?111?1?1(0)
?11?101111111111111?111?1?1(1)

===============
 Ejecucion 8
===============
Aciertos: 0
Errores:  1
No clasificados: 0
	Rule 0(PostState): 0 / 1
Aciertos: 0
Errores:  0
No clasificados: 1
	Rule 0(PostState): 0 / 0
----------------------------------------------------
Aciertos Training: 1
Aciertos Test: 0
Numero de reglas: 1
Variables por regla: 6
Variables usadas: 0.285714
Tiempo: 0.031115
Iteraciones: 0
Condiciones: 6
Aciertos Discriminantes: 0
Aciertos Descriptivas: 0
----------------------------------------------------
------------------- REGLAS -------------------------
----------------------------------------------------
Descriptivas: 
IF
	( at p1 - person ?Param_2 - city ) = { True}
	( at ?Param_0 - aircraft ?Param_1 - city ) = { False}
	( fuel ?Param_0 - aircraft ) = { L0/1}
	( slow-burn ?Param_0 - aircraft ) = { True}
	( refuel-rate ?Param_0 - aircraft ) = { True}
	( at ?Param_0 - aircraft ?Param_2 - city ) = { True}
THEN Class IS PostState   W 1 and N+= 1 and N-= 0

Predictivas: 
IF
THEN Class IS PostState   W 1 and N+= 1 and N-= 0

Patrones: 
?11?1?1111111111111?111?1?1(1)

===============
 Ejecucion 9
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
Variables por regla: 6
Variables usadas: 0.285714
Tiempo: 0.051044
Iteraciones: 0
Condiciones: 12
Aciertos Discriminantes: -nan
Aciertos Descriptivas: -nan
----------------------------------------------------
------------------- REGLAS -------------------------
----------------------------------------------------
Descriptivas: 
IF
	( at p1 - person ?Param_2 - city ) = { True}
	( at ?Param_0 - aircraft ?Param_1 - city ) = { True}
	( fuel ?Param_0 - aircraft ) = { L1}
	( slow-burn ?Param_0 - aircraft ) = { True}
	( refuel-rate ?Param_0 - aircraft ) = { True}
	( at ?Param_0 - aircraft ?Param_2 - city ) = { True}
THEN Class IS PreState   W 1 and N+= 1 and N-= 0

IF
	( at p1 - person ?Param_2 - city ) = { True}
	( at ?Param_0 - aircraft ?Param_1 - city ) = { False}
	( fuel ?Param_0 - aircraft ) = { L0/1}
	( slow-burn ?Param_0 - aircraft ) = { True}
	( refuel-rate ?Param_0 - aircraft ) = { True}
	( at ?Param_0 - aircraft ?Param_2 - city ) = { True}
THEN Class IS PostState   W 1 and N+= 1 and N-= 0

Predictivas: 
IF
	( fuel ?Param_0 - aircraft ) = { L1}
THEN Class IS PreState   W 1 and N+= 1 and N-= 0

IF
	( at ?Param_0 - aircraft ?Param_1 - city ) = { False}
THEN Class IS PostState   W 1 and N+= 1 and N-= 0

Patrones: 
?1?1011111111111111?111?1?1(0)
?1101?1111111111111?111?1?1(1)
	Aciertos en la base [0] es: 1
	Aciertos en test [0] es: -nan
	Numero de reglas [0] es: 2
	Variables por regla [0] es: 6
	Variables usadas [0] es: 0.285714
	Tiempo [0] es: 0.038969
	Iteraciones [0] es: 0
	Condiciones [0] es: 12

	Discriminantes [0] es: -nan

	Descriptivas [0] es: -nan

	Aciertos en la base [1] es: 1
	Aciertos en test [1] es: -nan
	Numero de reglas [1] es: 2
	Variables por regla [1] es: 6
	Variables usadas [1] es: 0.285714
	Tiempo [1] es: 0.037719
	Iteraciones [1] es: 0
	Condiciones [1] es: 12

	Discriminantes [1] es: -nan

	Descriptivas [1] es: -nan

	Aciertos en la base [2] es: 1
	Aciertos en test [2] es: -nan
	Numero de reglas [2] es: 2
	Variables por regla [2] es: 6
	Variables usadas [2] es: 0.285714
	Tiempo [2] es: 0.036476
	Iteraciones [2] es: 0
	Condiciones [2] es: 12

	Discriminantes [2] es: -nan

	Descriptivas [2] es: -nan

	Aciertos en la base [3] es: 1
	Aciertos en test [3] es: 0
	Numero de reglas [3] es: 1
	Variables por regla [3] es: 6
	Variables usadas [3] es: 0.285714
	Tiempo [3] es: 0.034036
	Iteraciones [3] es: 0
	Condiciones [3] es: 6

	Discriminantes [3] es: 0

	Descriptivas [3] es: 0

	Aciertos en la base [4] es: 1
	Aciertos en test [4] es: -nan
	Numero de reglas [4] es: 2
	Variables por regla [4] es: 6
	Variables usadas [4] es: 0.285714
	Tiempo [4] es: 0.037822
	Iteraciones [4] es: 0
	Condiciones [4] es: 12

	Discriminantes [4] es: -nan

	Descriptivas [4] es: -nan

	Aciertos en la base [5] es: 1
	Aciertos en test [5] es: -nan
	Numero de reglas [5] es: 2
	Variables por regla [5] es: 6
	Variables usadas [5] es: 0.285714
	Tiempo [5] es: 0.037205
	Iteraciones [5] es: 0
	Condiciones [5] es: 12

	Discriminantes [5] es: -nan

	Descriptivas [5] es: -nan

	Aciertos en la base [6] es: 1
	Aciertos en test [6] es: -nan
	Numero de reglas [6] es: 2
	Variables por regla [6] es: 6
	Variables usadas [6] es: 0.285714
	Tiempo [6] es: 0.036757
	Iteraciones [6] es: 0
	Condiciones [6] es: 12

	Discriminantes [6] es: -nan

	Descriptivas [6] es: -nan

	Aciertos en la base [7] es: 1
	Aciertos en test [7] es: -nan
	Numero de reglas [7] es: 2
	Variables por regla [7] es: 6
	Variables usadas [7] es: 0.285714
	Tiempo [7] es: 0.03553
	Iteraciones [7] es: 0
	Condiciones [7] es: 12

	Discriminantes [7] es: -nan

	Descriptivas [7] es: -nan

	Aciertos en la base [8] es: 1
	Aciertos en test [8] es: 0
	Numero de reglas [8] es: 1
	Variables por regla [8] es: 6
	Variables usadas [8] es: 0.285714
	Tiempo [8] es: 0.031115
	Iteraciones [8] es: 0
	Condiciones [8] es: 6

	Discriminantes [8] es: 0

	Descriptivas [8] es: 0

	Aciertos en la base [9] es: 1
	Aciertos en test [9] es: -nan
	Numero de reglas [9] es: 2
	Variables por regla [9] es: 6
	Variables usadas [9] es: 0.285714
	Tiempo [9] es: 0.051044
	Iteraciones [9] es: 0
	Condiciones [9] es: 12

	Discriminantes [9] es: -nan

	Descriptivas [9] es: -nan

La media training: 1( 0)
La media test: -nan( -nan)
La media de reglas: 1.8( 0.4)
La media de variables por regla: 6( 0)
La media de variables usadas: 0.285714( 5.55112e-17)
La media de tiempo: 0.0376673( 0.00493767)
La media de iteraciones: 0( 0)
La media de condiciones: 10.8( 2.4)
La media de discriminantes: -nan( -nan)
La media de conjuntas: -nan( -nan)
Frecuencia de aparicion de las 21 distintas variables en las reglas: 
( at p1 - person ?Param_2 - city ): --> 100 %
( at ?Param_0 - aircraft ?Param_1 - city ): --> 100 %
( fuel ?Param_0 - aircraft ): --> 100 %
( distance ?Param_2 - city c2 - city ): --> 0 %
( distance ?Param_2 - city c5 - city ): --> 0 %
( distance c5 - city ?Param_2 - city ): --> 0 %
( distance ?Param_1 - city c5 - city ): --> 0 %
( distance c3 - city ?Param_1 - city ): --> 0 %
( distance ?Param_2 - city c3 - city ): --> 0 %
( distance ?Param_2 - city ?Param_1 - city ): --> 0 %
( distance c2 - city ?Param_1 - city ): --> 0 %
( distance c3 - city ?Param_2 - city ): --> 0 %
( distance ?Param_1 - city c2 - city ): --> 0 %
( distance ?Param_1 - city ?Param_2 - city ): --> 0 %
( slow-speed ?Param_0 - aircraft ): --> 0 %
( fast-speed ?Param_0 - aircraft ): --> 0 %
( slow-burn ?Param_0 - aircraft ): --> 100 %
( fast-burn ?Param_0 - aircraft ): --> 0 %
( capacity ?Param_0 - aircraft ): --> 0 %
( refuel-rate ?Param_0 - aircraft ): --> 100 %
( at ?Param_0 - aircraft ?Param_2 - city ): --> 100 %
