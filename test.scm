;; Test script for uLisp

;; Self-evaluating
1
"a"

;; Primitive procedures
(car (quote (a b)))
(cdr (quote (a b)))
(cons (quote a) (quote (b)))
(atom (quote a))
(eq (quote a) (quote a))
(null (quote nil))

;; Special forms
(if (eq 1 0) (quote a) (quote b))
((lambda (x) (atom x)) (quote a))
(begin (eq 1 0) (eq 1 1))
(cond ((eq 1 0) (quote a)) (else (quote b)))
(define ten 10)
ten