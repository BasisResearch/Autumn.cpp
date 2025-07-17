(program
  (= GRID_SIZE 16)
  
  (object Particle (: living Bool) (Cell 0 0 (if living then "lightpink" else "black")))

  (= create_initial_particles (fn () (let
   (= particles (map (--> (pos) (Particle false pos)) (allPositions GRID_SIZE)))
   (= particles (updateObj particles (--> obj (updateObj obj "living" true)) (--> obj (in (.. obj origin)
    (list (Position 2 3) (Position 3 3) (Position 3 1) (Position 4 3) (Position 4 2))
   ))))
   particles
   )))
  
  (: particles (List Particle))
  (= particles (initnext ((create_initial_particles)) 
                (prev particles)))
  
  (object Button (: color String) (Cell 0 0 color))
  (: buttonNext Button)
  (= buttonNext (initnext (Button "green" (Position 0 (- GRID_SIZE 1))) (prev "buttonNext")))
  (: buttonReset Button) 
  (= buttonReset (initnext (Button "silver" (Position (- GRID_SIZE 1) (- GRID_SIZE 1))) (prev "buttonReset")))

  (on clicked (= particles (addObj (removeObj (prev particles) (--> obj (clicked obj))) (Particle true click))))
  (on (clicked buttonNext) (= particles 
                      (let (= livingObjs (filter (--> o (.. o living)) (prev particles)))
                           (updateObj (prev particles) 
                           (--> obj  
                           (let (= neighbours (list 
                            (Position (+ (.. (.. obj origin) x) 1) (+ (.. (.. obj origin) y) 1))
                            (Position (+ (.. (.. obj origin) x) 1) (- (.. (.. obj origin) y) 1))
                            (Position (- (.. (.. obj origin) x) 1) (+ (.. (.. obj origin) y) 1))
                            (Position (- (.. (.. obj origin) x) 1) (- (.. (.. obj origin) y) 1))
                            (Position (.. (.. obj origin) x) (+ (.. (.. obj origin) y) 1))
                            (Position (.. (.. obj origin) x) (- (.. (.. obj origin) y) 1))
                            (Position (+ (.. (.. obj origin) x) 1) (.. (.. obj origin) y))
                            (Position (- (.. (.. obj origin) x) 1) (.. (.. obj origin) y))
                            ))
                            (= livingAdjObjs (filter (--> o (in (.. o origin) neighbours)) livingObjs))
                            (= len (length livingAdjObjs))
                            (if (.. obj living) then (let
                              ; (print "living")
                              ; (print len)
                              (if (| (<= len 1) (>= len 4)) then
                                (updateObj obj "living" false) else 
                                obj)) else (let 
                              ; (print "dead")
                              ; (print len)
                              (if (== len 3) then 
                                (updateObj obj "living" true) else 
                                obj))))))
                        )))
  (on (clicked buttonReset) (= particles (map (--> (obj) (updateObj obj "living" false)) (prev particles))))

)