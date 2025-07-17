(program
  (= GRID_SIZE 7)

  (object Bbq (: fire Bool) (: gas Number) (list (Cell 0 1 "gray") (Cell 2 1 "gray") (Cell 1 1 (if fire then "orange" else "white")) (Cell 0 2 "gray") (Cell 0 3 "gray") (Cell 2 2 "gray") (Cell 2 3 "gray") (Cell 1 2 (if (> gas 20) then "yellow" else "white")) (Cell 1 3 (if (> gas 0) then "yellow" else "white"))))

  (object Person (: health Number)
  	(map
  		(--> (i) (Cell i 0 (if (<= i health) then "blue" else "black" )))
  		(range 0 GRID_SIZE)
  	)
  )

  (object Meat (: cooked Number)  (Cell 0 0 (if (< cooked 10) then "lightblue" else (if (< cooked 30) then "pink" else (if (< cooked 60) then "sandybrown" else "brown")))))

  (object FillBBQ (Cell 0 0 "yellow"))

  (: bbq Bbq)
  (= bbq (initnext (Bbq true 65 (Position (- (/ GRID_SIZE 2) 1) (- GRID_SIZE 4)))
            (if (== (.. (prev bbq) gas) 0) then (updateObj (prev bbq) "fire" false) else
                  (if (.. (prev bbq) fire)
                    then (updateObj (prev bbq) "gas" (- (.. (prev bbq) gas) 1))
                    else (prev bbq)))))

  (: meat Meat)
  (= meat (initnext (Meat 0 (Position (/ GRID_SIZE 2) (- GRID_SIZE 4)))
          (if (.. bbq fire)
            then (updateObj (prev meat) "cooked" (+ (.. (prev meat) cooked) 1))
            else (prev meat))))


  (: fillButton FillBBQ)
  (= fillButton (initnext (FillBBQ (Position 0 (- GRID_SIZE 1))) (prev fillButton)))
   
  (: person Person)
  (= person (initnext (Person (/ GRID_SIZE 2) (Position 0 0)) (prev person)))

  (on (clicked bbq) (= bbq (if (== (.. (prev bbq) gas) 0) then (prev bbq) else (updateObj bbq "fire" (! (.. bbq fire))))))
  (on (clicked fillButton) (= bbq (updateObj bbq "gas" (+ (.. (prev bbq) gas) 5))))
  (on (clicked meat)
      (= person (if (< (.. person health) 0) 
        then (prev person)
        else (updateObj person "health" 
          (max -1 (min (- GRID_SIZE 1)
            (+ (.. person health)
              (if (< (.. meat cooked) 30) then -1
              else (if (> (.. meat cooked) 60) then -2
              else 1)))))))))
  (on (clicked meat)
    (= meat (updateObj meat "cooked" 0)))
)