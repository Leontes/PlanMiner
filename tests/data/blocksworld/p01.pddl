; Sussman: invertir una torre parcial de tres bloques.
(define (problem blocksworld-p01)
  (:domain blocksworld)
  (:objects a b c - block)
  (:init (ontable a) (ontable b) (on c a) (clear c) (clear b) (handempty))
  (:goal (and (on a b) (on b c))))
