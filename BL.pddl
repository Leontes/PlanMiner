(define (domain bl)
	(:requirements :strips :typing)
	(:types
		block - object
	)
	(:predicates
		(clear ?p0 - block)
		(ontable ?p0 - block)
		(handempty)
		(holding ?p0 - block)
		(on ?p0 - block ?p1 - block)
	)
	(:action PICK_UP
		:parameters (param_0 - BLOCK)
		:precondition
			(and
				( CLEAR ?param_0 ) 
				( ONTABLE ?param_0 ) 
				( HANDEMPTY ) 
			)
		:effect
			(and
				( not ( CLEAR ?param_0 )  )
				( not ( ONTABLE ?param_0 )  )
				( not ( HANDEMPTY )  )
				( HOLDING ?param_0 ) 
			)
	)
	(:action PUT_DOWN
		:parameters (param_0 - BLOCK)
		:precondition
			(and
				( HOLDING ?param_0 ) 
			)
		:effect
			(and
				( not ( HOLDING ?param_0 )  )
				( CLEAR ?param_0 ) 
				( HANDEMPTY ) 
				( ONTABLE ?param_0 ) 
			)
	)
	(:action STACK
		:parameters (?param_0 - BLOCK, param_1 - BLOCK)
		:precondition
			(and
				( CLEAR ?param_1 ) 
				( HOLDING ?param_0 ) 
			)
		:effect
			(and
				( not ( CLEAR ?param_1 )  )
				( not ( HOLDING ?param_0 )  )
				( HANDEMPTY ) 
				( CLEAR ?param_0 ) 
				( ON ?param_0 ?param_1 ) 
			)
	)
	(:action UNSTACK
		:parameters (?param_0 - BLOCK, param_1 - BLOCK)
		:precondition
			(and
				( HANDEMPTY ) 
				( ON ?param_0 ?param_1 ) 
				( CLEAR ?param_0 ) 
			)
		:effect
			(and
				( not ( HANDEMPTY )  )
				( not ( ON ?param_0 ?param_1 )  )
				( not ( CLEAR ?param_0 )  )
				( CLEAR ?param_1 ) 
				( HOLDING ?param_0 ) 
			)
	)
)