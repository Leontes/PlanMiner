; Ocho bloques: de dos torres a otras dos, mezclando sus bloques.
(define (problem blocksworld-p03)
  (:domain blocksworld)
  (:objects a b c d e f g h - block)
  (:init
    (ontable a) (on b a) (on c b) (on d c) (clear d)
    (ontable e) (on f e) (on g f) (on h g) (clear h)
    (handempty))
  (:goal (and
    (on e a) (on b e) (on g b)
    (on c h) (on f c) (on d f))))
