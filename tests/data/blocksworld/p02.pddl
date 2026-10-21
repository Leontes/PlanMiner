; Invertir una torre de cinco bloques.
(define (problem blocksworld-p02)
  (:domain blocksworld)
  (:objects a b c d e - block)
  (:init (ontable a) (on b a) (on c b) (on d c) (on e d) (clear e) (handempty))
  (:goal (and (on a b) (on b c) (on c d) (on d e))))
