(program
  (= GRID_SIZE 16)
  (object Particle (Cell 0 0 "blue"))
  (: particle Particle)
  (= particle (initnext (Particle (Position 8 8)) (moveLeft (prev particle))))
  (on right (= particle (moveRight (prev particle))))
)
