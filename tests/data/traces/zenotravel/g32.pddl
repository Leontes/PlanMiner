(define (problem zenotravel-numeric-p32-s32)
  (:domain zenotravel)
  (:objects
    plane1 - aircraft
    person1 person2 - person
    city0 city1 city2 - city
  )
  (:init
    (located plane1 city1)
    (= (capacity plane1) 3836)
    (= (fuel plane1) 99)
    (= (slow-burn plane1) 3)
    (= (fast-burn plane1) 9)
    (= (onboard plane1) 0)
    (= (zoom-limit plane1) 1)
    (located person1 city0)
    (located person2 city1)
    (= (distance city0 city0) 0)
    (= (distance city0 city1) 179)
    (= (distance city0 city2) 318)
    (= (distance city1 city0) 179)
    (= (distance city1 city1) 0)
    (= (distance city1 city2) 248)
    (= (distance city2 city0) 318)
    (= (distance city2 city1) 248)
    (= (distance city2 city2) 0)
    (= (total-fuel-used) 0)
  )
  (:goal (and
    (located person1 city2)
    (located person2 city1)
  ))
  (:metric minimize (+ (* 1 (total-time)) (* 1 (total-fuel-used))))
)
