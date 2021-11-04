Wprowadzenie

Sokoban jest jednoosobową grą logiczną rozgrywaną na dwuwymiarowej planszy o kwadratowych polach.

Niektóre pola planszy są puste, na innych są ściany lub skrzynie.

Pewna liczba pól jest oznaczona jako pola docelowe. Polem docelowym może być pole puste lub pole, na którym jest skrzynia.

Na jednym z pól planszy jest postać sterowana przez gracza. Może się ona przesuwać na pola sąsiadujące z aktualnym w pionie lub poziomie.

Za sąsiadujące z polem w wierszu w i kolumnie k uznajemy pola, które są w:

    wierszu w - 1, kolumnie k,

    wierszu w + 1, kolumnie k,

    wierszu w, kolumnie k - 1,

    wierszu w, kolumnie k + 1.

Postać może się przesunąć na pole, jeśli jest ono puste lub jest na nim skrzynia, którą postać może pchnąć.

Pchnięcie skrzyni jest możliwe, jeśli bezpośrednio za nią, w kierunku ruchu postaci, jest pole puste. Nie jest możliwe przesunięcie postaci lub pchnięcie skrzyni poza planszę. Nie jest możliwe pchnięcie na raz więcej niż jednej skrzyni.

Celem gry jest umieszczenie skrzyń na polach docelowych.
Polecenie

Napisz program umożliwiający grę w Sokobana.

Program czyta opis stanu początkowego planszy, pisze go na wyjście a następnie czyta i wykonuje kolejne polecenia.

Po wykonaniu każdego polecenia program pisze aktualny stan planszy. Robi to niezależnie od tego, czy stan planszy, w rezultacie wykonania polecenia, zmienił się.

Rozpoznawane są polecenia pchnięcia skrzyni i cofnięcia wykonanego wcześniej pchnięcia.

Inaczej, niż w typowych implementacjach Sokobana, użytkownik nie musi podawać ruchów przesuwających postać po pustych polach. Program sam ustala, jak doprowadzić postać na pole, z którego będzie możliwe pchnięcie skrzyni we wskazanym kierunku.
Postać danych

Na wejściu programu jest opis stanu początkowego planszy, pusty wiersz i ciąg jednowierszowych poleceń, zakończony wierszem zaczynającym się od kropki ..

Program ignoruje zawartość wejścia po kropce kończącej dane.

Opis planszy składa się z niepustych wierszy, w których są jednoznakowe reprezentacje stanu pól:

    -

    puste pole, które nie jest polem docelowym,

    +

    puste pole, które jest polem docelowym,

    #

    ściana,

    @

    pole, nie będące polem docelowym, na którym jest postać,

    *

    pole docelowe, na którym jest postać,

    [a .. z] (mała litera)

    pole, które nie jest polem docelowym, na którym jest skrzynia o podanej nazwie,

    [A .. Z] (wielka litera)

    pole docelowe, na którym jest skrzynia o nazwie będącej małą literą odpowiadającą podanej wielkiej literze.

W poprawnym opisie planszy jest dokładnie jedna postać. Każda skrzynia, oznaczona literą alfabetu łacińskiego, może wystąpić tylko raz.

Program rozpoznaje polecenia:

    [a .. z][2 | 4 | 6 | 8] (mała litera, po której jest cyfra 2, 4, 6 lub 8)

    Pchnięcie skrzyni o nazwie, będącej pierwszym znakiem polecenia, w kierunku określonym drugim znakiem. Cyfra 2 oznacza pchnięcie w dół, 8 w górę, 4 w lewo a 6 w prawo.

    Na pole, z którego można wykonać pchnięcie, postać doprowadzana jest ścieżką składającą się z pustych pól. Nie może być na niej ściany ani skrzyni.

    Jeśli wykonanie pchnięcia nie jest możliwe, bo postać nie może podejść do skrzyni lub skrzyni nie da się pchnąć, stan planszy nie zmienia się.

    0

    Cofnięcie ostatniego wykonanego i jeszcze nie cofniętego pchnięcia.

    Postać wraca na pole, na którym była przed cofanym pchnięciem.

    Jeśli nie było żadnego pchnięcia, które można cofnąć, polecenie nie zmienia stanu planszy.

Postać wyniku

Wynikiem programu jest ciąg opisów planszy. Pierwszy z nich to opis stanu początkowego a kolejne to opisy stanu planszy po wykonaniu każdego polecenia.

Opis planszy w wyniku programu ma postać taką, jak w danych, ale bez kończącego pustego wiersza.
