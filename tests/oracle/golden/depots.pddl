(define (domain depots)
	(:requirements :strips :typing)
	(:types
		depot distributor - parenttype0
		truck hoist crate pallet - parenttype2
		parenttype0 parenttype2 - object
	)
	(:predicates
		(at ?param_0 - parenttype2 ?param_1 - parenttype0)
		(clear ?param_0 - parenttype2)
		(lifting ?param_0 - hoist ?param_1 - crate)
		(available ?param_0 - hoist)
		(on ?param_0 - crate ?param_1 - parenttype2)
		(in ?param_0 - crate ?param_1 - truck)
	)
	(:functions
		(current-load ?param_0 - truck)
		(load-limit ?param_0 - truck)
		(fuel-cost)
		(weight ?param_0 - crate)
	)
	(:action DRIVE
		:parameters (?param_0 - truck ?param_1 - parenttype0 ?param_2 - parenttype0)
		:precondition
			(and
				( at ?param_0 ?param_1 ) 
			)
		:effect
			(and
				( not ( at ?param_0 ?param_1 )  )
				( at ?param_0 ?param_2 ) 
				( increase ( fuel-cost ) 10  )
			)
	)
	(:action DROP
		:parameters (?param_0 - hoist ?param_1 - crate ?param_2 - parenttype2 ?param_3 - parenttype0)
		:precondition
			(and
				( at ?param_2 ?param_3 ) 
				( at ?param_0 ?param_3 ) 
				( clear ?param_2 ) 
				( lifting ?param_0 ?param_1 ) 
			)
		:effect
			(and
				( not ( clear ?param_2 )  )
				( not ( lifting ?param_0 ?param_1 )  )
				( available ?param_0 ) 
				( on ?param_1 ?param_2 ) 
				( clear ?param_1 ) 
				( at ?param_1 ?param_3 ) 
			)
	)
	(:action LIFT
		:parameters (?param_0 - hoist ?param_1 - crate ?param_2 - parenttype2 ?param_3 - parenttype0)
		:precondition
			(and
				( clear ?param_1 ) 
				( at ?param_0 ?param_3 ) 
				( available ?param_0 ) 
				( at ?param_2 ?param_3 ) 
				( at ?param_1 ?param_3 ) 
				( on ?param_1 ?param_2 ) 
			)
		:effect
			(and
				( not ( clear ?param_1 )  )
				( not ( available ?param_0 )  )
				( not ( at ?param_1 ?param_3 )  )
				( not ( on ?param_1 ?param_2 )  )
				( clear ?param_2 ) 
				( lifting ?param_0 ?param_1 ) 
				( increase ( fuel-cost ) 1  )
			)
	)
	(:action LOAD
		:parameters (?param_0 - hoist ?param_1 - crate ?param_2 - truck ?param_3 - parenttype0)
		:precondition
			(and
				( at ?param_0 ?param_3 ) 
				( at ?param_2 ?param_3 ) 
				( lifting ?param_0 ?param_1 ) 
				(< ( current-load ?param_2 ) ( load-limit ?param_2 ) )
			)
		:effect
			(and
				( not ( lifting ?param_0 ?param_1 )  )
				( available ?param_0 ) 
				( in ?param_1 ?param_2 ) 
				( increase ( current-load ?param_2 ) 1  )
			)
	)
	(:action UNLOAD
		:parameters (?param_0 - hoist ?param_1 - crate ?param_2 - truck ?param_3 - parenttype0)
		:precondition
			(and
				( at ?param_0 ?param_3 ) 
				( in ?param_1 ?param_2 ) 
				( available ?param_0 ) 
				( at ?param_2 ?param_3 ) 
			)
		:effect
			(and
				( not ( in ?param_1 ?param_2 )  )
				( not ( available ?param_0 )  )
				( lifting ?param_0 ?param_1 ) 
				( decrease ( current-load ?param_2 ) 1  )
			)
	)
)