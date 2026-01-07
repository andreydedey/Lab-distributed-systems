% Fatos
pai(joao, maria).
pai(joao, pedro).
pai(carlos, joao).

mae(ana, maria).
mae(ana, pedro).

% Regra: X é avô de Y se X é pai do pai de Y
avo(X, Y) :-
    pai(X, Z),
    pai(Z, Y).

% Regra: X é irmão de Y se têm o mesmo pai e não são a mesma pessoa
irmao(X, Y) :-
    pai(P, X),
    pai(P, Y),
    X \= Y.
