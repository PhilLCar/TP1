#! /usr/bin/env gsi -:dR

;;; Fichier : calc.scm

;;; Ce programme est une version incomplete du TP2.  Vous devez uniquement
;;; changer et ajouter du code dans la premi�re section.

;;;----------------------------------------------------------------------------

;;; La fonction traiter re�oit en param�tre une liste de caract�res
;;; contenant la requ�te lue et le dictionnaire des variables sous
;;; forme d'une liste d'association.  La fonction retourne
;;; une paire contenant la liste de caract�res qui sera imprim�e comme
;;; r�sultat de l'expression entr�e et le nouveau dictionnaire.  Vos
;;; fonctions ne doivent pas faire d'affichage car c'est la fonction
;;; "repl" qui se charge de cela.

(define (operate l a b)
  (case l
    ((#\+) (+ a b))
    ((#\-) (- a b))
    ((#\*) (* a b))
    (else 'UNDEFINED_OPERATOR_ERROR)))

(define (variable->value l dict)
  (let ((s (list->string (cons l '()))))
    (if s
        (let ((v (assv (string->symbol s) dict)))
	  (if v
	      (cdr v)
	      'UNDEFINED_VARIABLE_ERROR))
	'VARIABLE_FORMAT_ERROR)))

(define (assv-set key dict val)
  (cond
   ((null? dict)
    `((,key . ,val)))
   ((equal? key (caar dict))
    (assv-set key (cdr dict) val))
   (else (cons (car dict) (assv-set key (cdr dict) val)))))

(define (parse pass rem stack dict)
  (cond
   ((null? rem)
    (if (= (length stack) 1)
	(cons (string->list (number->string (car stack))) dict) ;; returns
	(cons 'OPERATION_FORMAT_ERROR dict))) ;;returns
   ((member (car rem) '(#\+ #\- #\*))
    (if (> (length stack) 1)
	(let ((res (operate (car rem) (cadr stack) (car stack))))
	  (if (symbol? res)
	      (cons res dict) ;; returns
	      (parse '() (cdr rem) (cons res (cddr stack)) dict)))
	(cons 'OPERATION_FORMAT_ERROR dict)))
   ((and (char>=? (car rem) #\0)
	 (char<=? (car rem) #\9))
    (parse (append pass (list (car rem))) (cdr rem) stack dict))
   ((equal? (car rem) #\space)
    (if (null? pass)
	(parse '() (cdr rem) stack dict)
	(let ((num (or (string->number (list->string pass))
		       'NUMBER_FORMAT_ERROR)))
	  (if (symbol? num)
	      (cons num dict);; returns
	      (parse '() (cdr rem) (cons num stack) dict)))))
   ((equal? (car rem) #\=)
    (if (or (null? (cadr rem))
	    (char<? (cadr rem) #\A)
	    (char>? (cadr rem) #\z))
	(cons 'VARIABLE_FORMAT_ERROR dict)
	(parse '() (cddr rem) stack
	       (assv-set (string->symbol (make-string 1 (cadr rem))) dict (car stack)))))
   (else
    (let ((var (variable->value (car rem) dict)))
      (if (symbol? var)
	  (cons var dict) ;;returns
	  (parse '() (cdr rem) (cons var stack) dict))))))

(define traiter
  (lambda (expr dict)
    (let ((p-expr (parse '() (append expr '(#\space)) '() dict)))
      (if (symbol? (car p-expr))
	  (cons (append (eval (car p-expr)) '(#\newline)) dict)
	  (cons (append (car p-expr) '(#\newline)) (cdr p-expr))))))
	  

;;;----------------------------------------------------------------------------

;;; Ne pas modifier cette section.

(define repl
  (lambda (dict)
    (print "# ")
    (let ((ligne (read-line)))
      (if (string? ligne)
          (let ((r (traiter-ligne ligne dict)))
            (for-each write-char (car r))
            (repl (cdr r)))))))

(define traiter-ligne
  (lambda (ligne dict)
    (traiter (string->list ligne) dict)))

(define main
  (lambda ()
    (repl '()))) ;; dictionnaire initial est vide
    
;;;----------------------------------------------------------------------------
    
;;;;;;;;;;;;;;;;;
;; ERROR CODES ;;
;;;;;;;;;;;;;;;;;
(define UNDEFINED_OPERATOR_ERROR (string->list "L'operateur n'est pas defini"))
(define UNDEFINED_VARIABLE_ERROR (string->list "La variable n'est pas definie"))
(define VARIABLE_FORMAT_ERROR (string->list "Le format de la variable est incorrect"))
(define OPERATION_FORMAT_ERROR (string->list "Le format de l'operation est incorrect"))
(define NUMBER_FORMAT_ERROR (string->list "Le format du nomber est incorrect"))
