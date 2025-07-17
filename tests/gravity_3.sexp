(program
  (= GRID_SIZE 40)
  (= background "black")
    
  (object Button (: color String) (Cell 0 0 color))
  (object Blob (list (Cell 0 0 "blue")))
  
  (: blobs (List Blob))
  (= blobs (initnext (list (Blob (Position 19 19))) (prev blobs)))
  
  (: xVel Number)
  (= xVel (initnext 0 (prev xVel)))
  
  (: yVel Number)
  (= yVel (initnext 0 (prev yVel)))
            
  (on (& ((clicked)) (isFreePos click)) (= blobs (addObj blobs (Blob (Position (.. click x) (.. click y))))))
  
  (on (& ((left)) (!= (prev xVel) -1)) (= xVel (- (prev xVel) 1)))
  (on (& ((right)) (!= (prev xVel) 1)) (= xVel (+ (prev xVel) 1)))
  
  (on (& ((up)) (!= (prev yVel) -1)) (= yVel (- (prev yVel) 1)))
  (on (& ((down)) (!= (prev yVel) 1)) (= yVel (+ (prev yVel) 1)))
  
  (on true (= blobs (updateObj blobs (--> obj (move obj (Position (prev xVel) (prev yVel)))))))
)
