(define $ZERO
  (lambda (f)
    (lambda (x)
      x)))

(define $INC
  (lambda (n)
    (lambda (f)
      (lambda (x)
        (f ((n f) x))))))

(define $ADD
  (lambda (m)
    (lambda (n)
      ((n $INC) m))))

(define $MUL
  (lambda (m)
    (lambda (n)
      ((n ($ADD m)) $ZERO))))

(define $CONS
  (lambda (x)
    (lambda (y)
      (lambda (f)
        ((f x) y)))))

(define $CAR
  (lambda (p)
    (p (lambda (x)
         (lambda (y)
           x)))))

(define $CDR
  (lambda (p)
    (p (lambda (x)
         (lambda (y)
           y)))))

(define $DEC
  (lambda (n)
    ($CAR ((n (lambda (p) (($CONS ($CDR p)) ($INC ($CDR p)))))
              (($CONS $ZERO) $ZERO)))))

(define $SUB
  (lambda (m)
    (lambda (n)
      ((n $DEC) m))))

(define $ONE   ($INC $ZERO))
(define $TWO   ($INC $ONE))
(define $THREE ($INC $TWO))
(define $FOUR  ($INC $THREE))
(define $FIVE  ($INC $FOUR))

(define $TEN    (($ADD $FIVE) $FIVE))
(define $TWENTY (($ADD $TEN) $TEN))
(define $FORTY  (($ADD $TWENTY) $TWENTY))

(define $TRUE
  (lambda (x)
    (lambda (y)
      x)))

(define $FALSE
  (lambda (x)
    (lambda (y)
      y)))

(define $ZERO?
  (lambda (n)
    ((n (lambda (x) $FALSE)) $TRUE)))

(define $LT_OR_EQ
  (lambda (m)
    (lambda (n)
      ($ZERO? (($SUB m) n)))))

(define $EMPTY (($CONS $TRUE) $TRUE))
(define $EMPTY? $CAR)

(define $UNSHIFT
  (lambda (l)
    (lambda (x)
      (($CONS $FALSE) (($CONS x) l)))))

(define $HEAD
  (lambda (l)
    ($CAR ($CDR l))))

(define $TAIL
  (lambda (l)
    ($CDR ($CDR l))))

(define $Z
  (lambda (f)
    ((lambda (x)
       (f (lambda (y) ((x x) y))))
     (lambda (x)
       (f (lambda (y) ((x x) y)))))))

(define $IF
  (lambda (b) b))

(define $NOT
  (lambda (b)
    ((($IF b) $FALSE) $TRUE)))

(define $DIV
  ($Z (lambda (f)
        (lambda (m)
          (lambda (n)
            ((($IF (($LT_OR_EQ n) m))
                   (lambda (x) (($INC ((f (($SUB m) n)) n)) x)))
             $ZERO))))))

(define $MOD
  ($Z (lambda (f)
        (lambda (m)
          (lambda (n)
            ((($IF (($LT_OR_EQ n) m))
                   (lambda (x) (((f (($SUB m) n)) n) x)))
                   m))))))

(define $RANGE
  ($Z (lambda (f)
        (lambda (m)
          (lambda (n)
            ((($IF (($LT_OR_EQ m) n))
                   (lambda (x) ((($UNSHIFT ((f ($INC m)) n)) m) x)))
                   $EMPTY))))))

(define $REDUCE
  ($Z (lambda (f)
        (lambda (l)
          (lambda (x)
            (lambda (g)
              ((($IF ($EMPTY? l))
                     x)
                     (lambda (y)
                       ((((f ($TAIL l))
                             ((g x) ($HEAD l)))
                             g) y)))))))))

(define $REVERSE
  (lambda (l)
    ((($REDUCE l) $EMPTY) $UNSHIFT)))

(define $MAP
  (lambda (l)
    (lambda (f)
      ($REVERSE ((($REDUCE l) $EMPTY)
                   (lambda (a) (lambda (x) (($UNSHIFT a) (f x)))))))))

(define $SELECT
  (lambda (l)
    (lambda (p)
      ($REVERSE ((($REDUCE l) $EMPTY)
                   (lambda (a) (lambda (x) ((($IF (p x))
                                                  (($UNSHIFT a) x))
                                                  a))))))))

(define $COMPLEMENT
  (lambda (p)
    (lambda (x)
      ($NOT (p x)))))

(define $REJECT
  (lambda (l)
    (lambda (p)
      (($SELECT l) ($COMPLEMENT p)))))

(define $SIEVE
  (lambda (l)
    (lambda (n)
      (($REJECT l) (lambda (m) ($ZERO? (($MOD m) n)))))))

(define $SIEVE_OF_ERATOSTHENES
  ($Z (lambda (f)
        (lambda (l)
          (lambda (a)
            ((($IF ($EMPTY? l))
                   a)
                   (lambda (x)
                     (((f (($SIEVE ($TAIL l)) ($HEAD l)))
                          (($UNSHIFT a) ($HEAD l)))
                          x))))))))

(define $PRIME_NUMBERS_UPTO
  (lambda (n)
    ($REVERSE (($SIEVE_OF_ERATOSTHENES (($RANGE $TWO) n)) $EMPTY))))

(define (stringize-number n)
  (number->string ((n (lambda (x) (+ x 1))) 0)))

(define (print-number-list l)
  (letrec ((iter (lambda (l)
                   ((($IF ($EMPTY? l))
                          $ZERO)
                          (lambda (x)
                            (display (stringize-number ($HEAD l)))
                            (((($IF ($EMPTY? ($TAIL l)))
                                    $ZERO)
                                    (lambda (y)
                                      (display " ")
                                      $ZERO)) x)
                            ((iter ($TAIL l)) x))))))
    (display "(")
    ((iter l) $ZERO)
    (display ")")
    (newline)))

(print-number-list ($PRIME_NUMBERS_UPTO $FORTY))

(exit)
