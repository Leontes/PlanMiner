(define (domain blocksworld)
	(:requirements :strips :typing)
	(:types
		block - object
	)
	(:predicates
		(ontable ?param_0 - block)
		(clear ?param_0 - block)
		(holding ?param_0 - block)
		(handempty)
		(on ?param_0 - block ?param_1 - block)
	)
	(:functions
	)
	(:action PICK_UP
		:parameters (?param_0 - block)
		:precondition
			(and
				( ontable ?param_0 ) 
				( clear ?param_0 ) 
				( handempty ) 
			)
		:effect
			(and
				( not ( ontable ?param_0 )  )
				( not ( clear ?param_0 )  )
				( holding ?param_0 ) 
				( not ( handempty )  )
			)
	)
	(:action PUT_DOWN
		:parameters (?param_0 - block)
		:precondition
			(and
				( holding ?param_0 ) 
			)
		:effect
			(and
				( handempty ) 
				( not ( holding ?param_0 )  )
				( clear ?param_0 ) 
				( ontable ?param_0 ) 
			)
	)
	(:action STACK
		:parameters (?param_0 - block ?param_1 - block)
		:precondition
			(and
				( holding ?param_0 ) 
				( clear ?param_1 ) 
				( ontable ?param_1 ) 
			)
		:effect
			(and
				( clear ?param_0 ) 
				( handempty ) 
				( not ( holding ?param_0 )  )
				( on ?param_0 ?param_1 ) 
				( not ( clear ?param_1 )  )
			)
	)
	(:action UNSTACK
		:parameters (?param_0 - block ?param_1 - block)
		:precondition
			(and
				( handempty ) 
				( clear ?param_0 ) 
				( on ?param_0 ?param_1 ) 
			)
		:effect
			(and
				( not ( handempty )  )
				( not ( clear ?param_0 )  )
				( holding ?param_0 ) 
				( clear ?param_1 ) 
				( not ( on ?param_0 ?param_1 )  )
				( not ( ontable ?param_1 )  )
			)
	)
)