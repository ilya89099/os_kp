count_max(L,R) :- count_max(L,R,0,0).
count_max([],R,R,_).
count_max([H|T],R,Count,Max) :-
	H > Max,
	count_max(T,R,1,H).

count_max([H|T],R,Count,Max) :-
	H = Max,
	Count1 is Count + 1,
	count_max(T,R,Count1,Max).
count_max([H|T],R,Count,Max) :-
	H < Max,
	count_max(T,R,Count,Max).
