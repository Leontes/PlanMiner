(define (domain pruebaruid)
	(:requirements :strips :typing)
	(:types
		person aircraft city - object
	)
	(:predicates
		(at ?p0 - person ?p1 - city)
		(at ?p0 - aircraft ?p1 - city)
		(in ?p0 - person ?p1 - aircraft)
	)
	(:functions
		(total-fuel-used)
		(fuel ?p0 - aircraft)
		(passengerslimit ?p0 - aircraft)
		(onboard ?p0 - aircraft)
	)
	(:action BOARD
		:parameters (?param_0 - PERSON, ?param_1 - AIRCRAFT, param_2 - CITY)
		:precondition
			(and
				( AT ?param_0 ?param_2 ) 
				( IN ?param_0 ?param_1 ) 
				(> ( FUEL ?param_1 ) 1.000000 )
				(> ( PASSENGERSLIMIT ?param_1 ) ( ONBOARD ?param_1 ) )
				(> ( PASSENGERSLIMIT ?param_1 ) 0.000000 )
				(> ( TOTAL-FUEL-USED ) 0.000000 )
			)
		:effect
			(and
				( not ( AT ?param_0 ?param_2 )  )
				( not ( AT ?param_1 ?param_2 )  )
				( IN ?param_0 ?param_1 ) 
				( increase ( ONBOARD ?param_1 ) 1.000000  )
			)
	)
	(:action DEBARK
		:parameters (?param_0 - PERSON, ?param_1 - AIRCRAFT, param_2 - CITY)
		:precondition
			(and
				( AT ?param_1 ?param_2 ) 
				( AT ?param_0 ?param_2 ) 
				( IN ?param_0 ?param_1 ) 
				(> ( ONBOARD ?param_1 ) 0.000000 )
				(> ( PASSENGERSLIMIT ?param_1 ) -1.000000 )
				(> ( TOTAL-FUEL-USED ) -1.000000 )
			)
		:effect
			(and
				( not ( IN ?param_0 ?param_1 )  )
				( decrease ( ONBOARD ?param_1 ) -1.000000  )
			)
	)
	(:action FLY
		:parameters (?param_0 - AIRCRAFT, ?param_1 - CITY, param_2 - CITY)
		:precondition
			(and
				( AT ?param_0 ?param_2 ) 
				(> ( FUEL ?param_0 ) ( ONBOARD ?param_0 ) )
				(> ( FUEL ?param_0 ) ( PASSENGERSLIMIT ?param_0 ) )
				(> ( FUEL ?param_0 ) 0.000000 )
				(< ( ONBOARD ?param_0 ) -1.000000 )
				(< ( PASSENGERSLIMIT ?param_0 ) -1.000000 )
			)
		:effect
			(and
				( not ( AT ?param_0 ?param_1 )  )
				( AT ?param_0 ?param_2 ) 
				( decrease ( FUEL ?param_0 ) 0  )
				( assign PASSENGERSLIMIT ?param_0 ) 5  )
				( increase ( TOTAL-FUEL-USED ) 10.000000  )
			)
	)
	(:action REFUEL
		:parameters (?param_0 - AIRCRAFT, param_1 - CITY)
		:precondition
			(and
				( AT ?param_0 ?param_1 ) 
				(< ( FUEL ?param_0 ) ( PASSENGERSLIMIT ?param_0 ) )
				(== ( ONBOARD ?param_0 ) ( FUEL ?param_0 ) )
				(> ( ONBOARD ?param_0 ) 0.000000 )
				(> ( PASSENGERSLIMIT ?param_0 ) 0.000000 )
				(== ( TOTAL-FUEL-USED ) ( FUEL ?param_0 ) )
				(> ( TOTAL-FUEL-USED ) 0.000000 )
			)
		:effect
			(and
				( increase ( FUEL ?param_0 ) 30  )
				( assign PASSENGERSLIMIT ?param_0 ) 5  )
			)
	)
	(:action ZOOM
		:parameters (?param_0 - AIRCRAFT, ?param_1 - CITY, param_2 - CITY)
		:precondition
			(and
				( AT ?param_0 ?param_2 ) 
				(> ( FUEL ?param_0 ) 0.000000 )
				(< ( ONBOARD ?param_0 ) -1.000000 )
				(< ( PASSENGERSLIMIT ?param_0 ) -1.000000 )
				(> ( TOTAL-FUEL-USED ) 0.000000 )
			)
		:effect
			(and
				( AT ?param_0 ?param_2 ) 
				( decrease ( FUEL ?param_0 ) -1.000000  )
				( increase ( TOTAL-FUEL-USED ) 10.000000  )
			)
	)
)