(define (domain blocksworld)
	(:requirements :strips :typing)
	(:types
		block - object
	)
	(:predicates
		(ontable ?param_0 - block)
		(clear ?param_0 - block)
		(handempty)
		(holding ?param_0 - block)
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
				( not ( handempty )  )
				( holding ?param_0 ) 
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
				( not ( holding ?param_0 )  )
				( clear ?param_0 ) 
				( ontable ?param_0 ) 
				( handempty ) 
			)
	)
	(:action STACK
		:parameters (?param_0 - block ?param_1 - block)
		:precondition
			(and
				( holding ?param_0 ) 
				( clear ?param_1 ) 
			)
		:effect
			(and
				( not ( holding ?param_0 )  )
				( not ( clear ?param_1 )  )
				( on ?param_0 ?param_1 ) 
				( clear ?param_0 ) 
				( handempty ) 
			)
	)
	(:action UNSTACK
		:parameters (?param_0 - block ?param_1 - block)
		:precondition
			(and
				( clear ?param_0 ) 
				( on ?param_0 ?param_1 ) 
				( handempty ) 
			)
		:effect
			(and
				( not ( clear ?param_0 )  )
				( not ( on ?param_0 ?param_1 )  )
				( not ( handempty )  )
				( clear ?param_1 ) 
				( holding ?param_0 ) 
			)
	)
)