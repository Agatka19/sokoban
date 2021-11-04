#include <assert.h>
#include <ctype.h>
#include <limits.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define MNOZNIK 3
#define DZIELNIK 2

typedef struct//będziemy mieć dynamiczną tablicę tych structów i to będzie nasza plansza
{
    char *tab;
    int size;
    int licz_el;
    char *odwiedzony;//char żeby zajmowało mniej miejsca niż int. 48 to nieodwiedzone('0'),a  49 - odwiedzone ('1')
    int size_o;
} wiersz;

typedef struct
{
    int obecna_poz_postaci;
    int obecny_indeks_postaci;
    int obecna_poz_skrzyni;
    int obecny_indeks_skrzyni;
    int obecny_indeks_skrzyni2;
    int obecna_poz_skrzyni2;
    char skrzynia;
} stan;

typedef struct
{
    stan *stosik;//będzimy na stosie trzymać stan planszy, żeby można było cofać ruchy
    int size_s;
} stos;

typedef struct tKolejka//do BFS
{
    struct tKolejka *next;
    int indeks;
    int pozycja;
} kolejka;

void kolejka_dopisz(kolejka **ostatni, kolejka **pierwszy, int indeks, int pozycja, bool *to_ta_skrzynia, int indeks_celu, int pozycja_celu)
{
    if(*ostatni != NULL)
    {
        (*ostatni)->next = malloc(sizeof(kolejka));
        *ostatni = (*ostatni)->next;
    }
    else
    {
        *ostatni = malloc(sizeof(kolejka));
        *pierwszy = *ostatni;
    }
    (*ostatni)->indeks = indeks;
    (*ostatni)->pozycja = pozycja;
    (*ostatni)->next = NULL;
    if(indeks==indeks_celu && pozycja==pozycja_celu) *to_ta_skrzynia=true;
}

void kolejka_usun(kolejka **pierwszy, kolejka **ostatni)
{
    if(*pierwszy != NULL)
    {
        kolejka *a = *pierwszy;
        *pierwszy = (*pierwszy)->next;
        if(*pierwszy==NULL) *ostatni = NULL;
        free(a);
    }

}

void wypisz (wiersz* plansza, int indeks)
{
    for(int i=0; i<indeks; i++)
    {
        for(int j=0; j<plansza[i].licz_el; j++)
        {
            printf("%c", plansza[i].tab[j]);
        }
    }
}

void dopisz(int pozycja, char wartosc, wiersz* w)
{
    if(pozycja>=w->size)
    {
        w->size = 1 + (pozycja-w->size) + w->size * MNOZNIK / DZIELNIK;
        w->tab = realloc(w->tab, w->size * sizeof(char));
    }
    if(pozycja>=w->size_o)
    {
        w->size_o = 1 + (pozycja-w->size_o) + w->size_o * MNOZNIK / DZIELNIK;
        w->odwiedzony=realloc(w->odwiedzony, w->size_o * sizeof(char));
    }
    w->tab[pozycja] = wartosc;
    w->licz_el++;

}

void zwieksz(wiersz** plansza, int indeks, int* rozmiar)
{
    if(indeks>=*rozmiar)
    {

        (*rozmiar)= 1 +  (*rozmiar) * MNOZNIK / DZIELNIK;
        *plansza = realloc(*plansza, *rozmiar * sizeof(wiersz));
    }
    (*plansza)[indeks].size = 2;
    (*plansza)[indeks].size_o = 2;
    (*plansza)[indeks].licz_el = 0;
    (*plansza)[indeks].tab = malloc((*plansza)[indeks].size * sizeof(char));
    (*plansza)[indeks].odwiedzony = malloc((*plansza)[indeks].size * sizeof(char));
}

void stos_zw(int ruch, stos *a)
{
    if(ruch>=a->size_s)
    {

        a->size_s = 1 +  a->size_s * MNOZNIK / DZIELNIK;
        a->stosik = realloc(a->stosik, a->size_s * sizeof(stan));
    }
}

bool jest_miejsce(wiersz* plansza, int indeks_na, int pozycja_na, int indeks)
{
    if(indeks_na < indeks && indeks_na >=0 && pozycja_na >=0 && pozycja_na < plansza[indeks_na].licz_el)
    {
        if(plansza[indeks_na].tab[pozycja_na]== '-' || plansza[indeks_na].tab[pozycja_na]== '+' || plansza[indeks_na].tab[pozycja_na]== '@'  || plansza[indeks_na].tab[pozycja_na]== '*')
            return true;
        else return false;
    }
    else return false;
}

bool mozna_przesunac_skrzynie(char skrzynia, int kierunek, wiersz* plansza, int indeks)
{
    for(int i=0; i<indeks; i++)
    {
        for(int j=0; j<plansza[i].licz_el; j++)
        {
            if(plansza[i].tab[j]==skrzynia || plansza[i].tab[j]==(skrzynia - 0x20))
            {
                if(kierunek==2 || kierunek==8)
                {
                    if(jest_miejsce(plansza, i+1, j, indeks) && jest_miejsce(plansza, i-1, j, indeks)) return true;
                    else return false;
                }
                if(kierunek==4 || kierunek==6)
                {
                    if(jest_miejsce(plansza, i, j-1, indeks) && jest_miejsce(plansza, i, j+1, indeks)) return true;
                    else return false;
                }
            }
        }
    }
    return false;
}

bool da_sie_dojsc(char skrzynia, int kierunek, wiersz* plansza, int indeks)
{
    kolejka *pierwszy = NULL;
    kolejka *ostatni = NULL;
    bool to_ta_skrzynia = false;
    int indeks_gracza = 0;
    int pozycja_gracza = 0;
    int indeks_celu = 0;
    int pozycja_celu = 0;
    for(int i=0; i<indeks; i++)
    {
        for(int j=0; j<plansza[i].licz_el; j++)
        {
            plansza[i].odwiedzony[j]=48;
            if(plansza[i].tab[j]=='@' || plansza[i].tab[j]=='*')
            {
                indeks_gracza = i;
                pozycja_gracza = j;
            }
        }
    }
    for(int i=0; i<indeks; i++)
    {
        for(int j=0; j<plansza[i].licz_el; j++)
        {
            if(plansza[i].tab[j]==skrzynia || plansza[i].tab[j]==(skrzynia - 0x20))
            {
                indeks_celu = i;
                pozycja_celu = j;
            }
        }
    }
    if (kierunek==2) indeks_celu--;
    if (kierunek==8) indeks_celu++;
    if (kierunek==6) pozycja_celu--;
    if (kierunek==4) pozycja_celu++;
    kolejka_dopisz(&ostatni, &pierwszy, indeks_gracza, pozycja_gracza, &to_ta_skrzynia, indeks_celu, pozycja_celu);
    plansza[indeks_gracza].odwiedzony[pozycja_gracza] =49;
    while(pierwszy != NULL && to_ta_skrzynia != true)
    {
        if(jest_miejsce(plansza, pierwszy->indeks+1, pierwszy->pozycja, indeks) && plansza[pierwszy->indeks+1].odwiedzony[pierwszy->pozycja]!=49)
        {
            kolejka_dopisz(&ostatni, &pierwszy, pierwszy->indeks+1, pierwszy->pozycja, &to_ta_skrzynia, indeks_celu, pozycja_celu);
            plansza[pierwszy->indeks+1].odwiedzony[pierwszy->pozycja] =49;
        }
        if(jest_miejsce(plansza, pierwszy->indeks-1, pierwszy->pozycja, indeks) && plansza[pierwszy->indeks-1].odwiedzony[pierwszy->pozycja]!=49)
        {
            kolejka_dopisz(&ostatni, &pierwszy, pierwszy->indeks-1, pierwszy->pozycja, &to_ta_skrzynia, indeks_celu, pozycja_celu);
            plansza[pierwszy->indeks-1].odwiedzony[pierwszy->pozycja] =49;
        }
        if(jest_miejsce(plansza, pierwszy->indeks, pierwszy->pozycja+1, indeks) && plansza[pierwszy->indeks].odwiedzony[pierwszy->pozycja+1]!=49)
        {
            kolejka_dopisz(&ostatni, &pierwszy, pierwszy->indeks, pierwszy->pozycja+1, &to_ta_skrzynia, indeks_celu, pozycja_celu);
            plansza[pierwszy->indeks].odwiedzony[pierwszy->pozycja+1] =49;
        }
        if(jest_miejsce(plansza, pierwszy->indeks, pierwszy->pozycja-1, indeks) && plansza[pierwszy->indeks].odwiedzony[pierwszy->pozycja-1]!=49)
        {
            kolejka_dopisz(&ostatni, &pierwszy, pierwszy->indeks, pierwszy->pozycja-1, &to_ta_skrzynia, indeks_celu, pozycja_celu);
            plansza[pierwszy->indeks].odwiedzony[pierwszy->pozycja-1] =49;
        }
        kolejka_usun(&pierwszy, &ostatni);
    }
    while(pierwszy!=NULL)
    {
        kolejka_usun(&pierwszy, &ostatni);
    }
    return to_ta_skrzynia;
}

void przesun_w_kierunku(char skrzynia, wiersz **plansza, int *indeks_skrzyni, int *pozycja_skrzyni, int indeks_skrzyni_cel, int pozycja_skrzyni_cel, int *indeks_gracza, int *pozycja_gracza)
{
    if((*plansza)[*indeks_gracza].tab[*pozycja_gracza]=='@')
        (*plansza)[*indeks_gracza].tab[*pozycja_gracza]='-';
    else(*plansza)[*indeks_gracza].tab[*pozycja_gracza]='+';
    if(skrzynia>=97 && skrzynia<=122)//jeśli skrzynia jest małą literką to wiemy, że pod nią jest puste pole '-'
    {
        if((*plansza)[indeks_skrzyni_cel].tab[pozycja_skrzyni_cel]=='-' || (*plansza)[indeks_skrzyni_cel].tab[pozycja_skrzyni_cel]=='@')
        {
            (*plansza)[*indeks_skrzyni].tab[*pozycja_skrzyni]='@';//postać zawsze wchodzi na miejsce skrzyni
            (*plansza)[indeks_skrzyni_cel].tab[pozycja_skrzyni_cel]=skrzynia;
        }
        else
        {
            (*plansza)[*indeks_skrzyni].tab[*pozycja_skrzyni]='@';
            (*plansza)[indeks_skrzyni_cel].tab[pozycja_skrzyni_cel]=skrzynia - 0x20;
        }
    }
    else//duża litera
    {
        if((*plansza)[indeks_skrzyni_cel].tab[pozycja_skrzyni_cel]=='-'  || (*plansza)[indeks_skrzyni_cel].tab[pozycja_skrzyni_cel]=='@')
        {
            (*plansza)[*indeks_skrzyni].tab[*pozycja_skrzyni]='*';
            (*plansza)[indeks_skrzyni_cel].tab[pozycja_skrzyni_cel]=skrzynia + 0x20;
        }
        else
        {
            (*plansza)[*indeks_skrzyni].tab[*pozycja_skrzyni]='*';
            (*plansza)[indeks_skrzyni_cel].tab[pozycja_skrzyni_cel]=skrzynia;
        }
    }
    *indeks_gracza=*indeks_skrzyni;
    *pozycja_gracza=*pozycja_skrzyni;
    *indeks_skrzyni=indeks_skrzyni_cel;
    *pozycja_skrzyni = pozycja_skrzyni_cel;
}

void przesun_skrzynie(char skrzynia, int kierunek, wiersz **plansza, int indeks, int *obecny_indeks_skrzyni, int *obecna_poz_skrzyni, int *obecny_indeks_postaci, int *obecna_poz_postaci)
{
    int indeks_skrzyni = 0;
    int pozycja_skrzyni = 0;
    int indeks_gracza = 0;
    int pozycja_gracza = 0;
    int a=0;
    for(int i=0; i<indeks; i++)
    {
        for(int j=0; j<(*plansza)[i].licz_el && a==0; j++)
        {
            if((*plansza)[i].tab[j]==skrzynia || (*plansza)[i].tab[j]==(skrzynia - 0x20))
            {
                indeks_skrzyni = i;
                pozycja_skrzyni = j;
                if((*plansza)[i].tab[j]==(skrzynia - 0x20)) skrzynia = skrzynia - 0x20;
                a=1;
            }
        }
    }
    for(int i=0; i<indeks; i++)
    {
        for(int j=0; j<(*plansza)[i].licz_el; j++)
        {
            if((*plansza)[i].tab[j]=='@' || (*plansza)[i].tab[j]=='*')
            {
                indeks_gracza = i;
                pozycja_gracza = j;
            }
        }
    }
    if(kierunek==2)
    {
        przesun_w_kierunku(skrzynia, plansza, &indeks_skrzyni, &pozycja_skrzyni, indeks_skrzyni+1, pozycja_skrzyni, &indeks_gracza, &pozycja_gracza);
    }
    if(kierunek==8)
    {
        przesun_w_kierunku(skrzynia, plansza, &indeks_skrzyni, &pozycja_skrzyni, indeks_skrzyni-1, pozycja_skrzyni, &indeks_gracza, &pozycja_gracza);
    }
    if(kierunek==4)
    {
        przesun_w_kierunku(skrzynia, plansza, &indeks_skrzyni, &pozycja_skrzyni, indeks_skrzyni, pozycja_skrzyni-1, &indeks_gracza, &pozycja_gracza);
    }
    if(kierunek==6)
    {
        przesun_w_kierunku(skrzynia, plansza, &indeks_skrzyni, &pozycja_skrzyni, indeks_skrzyni, pozycja_skrzyni+1, &indeks_gracza, &pozycja_gracza);
    }
    *obecny_indeks_skrzyni=indeks_skrzyni;
    *obecna_poz_skrzyni=pozycja_skrzyni;
    *obecny_indeks_postaci=indeks_gracza;
    *obecna_poz_postaci=pozycja_gracza;
}


bool przesun(char skrzynia, int kierunek, wiersz **plansza, int indeks, int *obecny_indeks_skrzyni, int *obecna_pozycja_skrzyni, int *obecny_indeks_postaci, int *obecna_poz_postaci)
{
    if(mozna_przesunac_skrzynie(skrzynia, kierunek, *plansza, indeks))
    {
        if(da_sie_dojsc(skrzynia, kierunek, *plansza, indeks))
        {
            przesun_skrzynie(skrzynia, kierunek, plansza, indeks, obecny_indeks_skrzyni, obecna_pozycja_skrzyni, obecny_indeks_postaci, obecna_poz_postaci);
            return true;
            //przesuwamy skrzynię tylko jeżeli wiemy, że po dwóch stronach jest puste miejsce i, że da się dojść na to miejsce po drugiej stronie niż chcemy przesunąć
        }
    }
    return false;
}

int main()
{
    wiersz *plansza=NULL;
    int rozmiar=2;
    int pozycja = 0; //indeks w 'tab'
    int indeks=0; //indeks w 'plansza'
    int ruch = 0; //indeks w 'stosik'
    plansza = (wiersz*) malloc(rozmiar * sizeof(wiersz));//dynamiczna tablica structów 'wiersz'
    plansza[indeks].size=2;
    plansza[indeks].size_o=2;
    plansza[indeks].licz_el = 0;
    plansza[indeks].tab = malloc(plansza[indeks].size * sizeof(char));
    plansza[indeks].odwiedzony = malloc(plansza[indeks].size * sizeof(char));
    stos stock;
    stock.size_s = 2;
    stock.stosik = malloc(stock.size_s * sizeof(stan));//na 'stosik' będziemy odkładać indeksy i pozycje postaci oraz skrzyni, żeby móc potem cofnąć ruch
    stock.stosik[ruch].obecna_poz_postaci =0;
    stock.stosik[ruch].obecna_poz_skrzyni =0;
    stock.stosik[ruch].obecny_indeks_postaci =0;
    stock.stosik[ruch].obecny_indeks_skrzyni =0;
    stock.stosik[ruch].obecny_indeks_skrzyni2 =0;
    stock.stosik[ruch].obecna_poz_skrzyni2 =0;
    stock.stosik[ruch].skrzynia=' ';
    char wartosc;
    wartosc = (char)getchar();
    dopisz(pozycja, wartosc, &(plansza[indeks]));
    pozycja++;
    while (plansza[indeks].tab[0] != '\n')//wczytywanie planszy
    {
        while(plansza[indeks].tab[pozycja-1] != '\n')
        {
            wartosc = (char) getchar();
            dopisz(pozycja, wartosc, &(plansza[indeks]));
            pozycja++;
        }
        indeks++;
        zwieksz(&plansza, indeks, &rozmiar);
        pozycja=0;
        wartosc = (char)getchar();
        dopisz(pozycja, wartosc, &(plansza[indeks]));
        pozycja++;
    }
    wypisz(plansza, indeks);
    bool czy_kropka = false;
    char skrzynia;
    skrzynia = (char)getchar();
    stock.stosik[0].skrzynia=skrzynia;
    for(int i=0; i<indeks; i++)
    {
        for(int j=0; j<plansza[i].licz_el; j++)
        {
            if(plansza[i].tab[j]==skrzynia || plansza[i].tab[j]==(skrzynia - 0x20))
            {
                stock.stosik[ruch].obecny_indeks_skrzyni = i;
                stock.stosik[ruch].obecna_poz_skrzyni = j;
            }
        }
    }
    for(int i=0; i<indeks; i++)
    {
        for(int j=0; j<plansza[i].licz_el; j++)
        {
            if(plansza[i].tab[j]=='@' || plansza[i].tab[j]=='*')
            {
                stock.stosik[ruch].obecna_poz_postaci = j;
                stock.stosik[ruch].obecny_indeks_postaci = i;
            }
        }
    }
    ruch++;
    bool zero = false;
    int kierunek = 1;
    if(skrzynia=='\n') wypisz(plansza, indeks);
    if(skrzynia!='.')//wczytywanie instrukcji
    {
        while(czy_kropka == false)
        {
            char a;
            a = (char)getchar();
            if(a=='.')
            {
                czy_kropka = true;
            }
            else if(a>=97 && a<=122)//małe literki
            {
                skrzynia = a;
                stock.stosik[ruch].skrzynia=skrzynia;
            }
            else if(a==50 || a==52 || a==54 || a==56)//2, 4, 6 lub 8
            {
                kierunek = a-48;
            }
            else if (a == '\n')
            {
                if(!zero)
                {
                    stock.stosik[ruch].obecny_indeks_skrzyni=stock.stosik[ruch-1].obecny_indeks_skrzyni;
                    stock.stosik[ruch].obecna_poz_skrzyni=stock.stosik[ruch-1].obecna_poz_skrzyni;
                    stock.stosik[ruch].obecny_indeks_postaci=stock.stosik[ruch-1].obecny_indeks_postaci;
                    stock.stosik[ruch].obecna_poz_postaci=stock.stosik[ruch-1].obecna_poz_postaci;
                    if(przesun(skrzynia, kierunek, &plansza, indeks, &stock.stosik[ruch].obecny_indeks_skrzyni, &stock.stosik[ruch].obecna_poz_skrzyni, &stock.stosik[ruch].obecny_indeks_postaci, &stock.stosik[ruch].obecna_poz_postaci))
                    {
                        stock.stosik[ruch].skrzynia=skrzynia;
                        ruch++;
                        stos_zw(ruch, &stock);
                        kierunek = 1;
                    }
                }
                wypisz(plansza, indeks);
                zero=false;
            }
            else if (a == '0')//cofanie
            {

                if(ruch>1)
                {
                    ruch--;
                    skrzynia = stock.stosik[ruch].skrzynia;
                    if (plansza[stock.stosik[ruch].obecny_indeks_postaci].tab[stock.stosik[ruch].obecna_poz_postaci]=='@')
                        plansza[stock.stosik[ruch].obecny_indeks_postaci].tab[stock.stosik[ruch].obecna_poz_postaci] = skrzynia;
                    else plansza[stock.stosik[ruch].obecny_indeks_postaci].tab[stock.stosik[ruch].obecna_poz_postaci] = skrzynia-0x20;
                    if (plansza[stock.stosik[ruch].obecny_indeks_skrzyni].tab[stock.stosik[ruch].obecna_poz_skrzyni]==skrzynia)//jeśli to mała literka
                        plansza[stock.stosik[ruch].obecny_indeks_skrzyni].tab[stock.stosik[ruch].obecna_poz_skrzyni]='-';
                    else plansza[stock.stosik[ruch].obecny_indeks_skrzyni].tab[stock.stosik[ruch].obecna_poz_skrzyni]='+';
                    if (plansza[stock.stosik[ruch-1].obecny_indeks_postaci].tab[stock.stosik[ruch-1].obecna_poz_postaci] == '-' || plansza[stock.stosik[ruch-1].obecny_indeks_postaci].tab[stock.stosik[ruch-1].obecna_poz_postaci] ==skrzynia)
                        plansza[stock.stosik[ruch-1].obecny_indeks_postaci].tab[stock.stosik[ruch-1].obecna_poz_postaci] = '@';
                    else plansza[stock.stosik[ruch-1].obecny_indeks_postaci].tab[stock.stosik[ruch-1].obecna_poz_postaci] = '*';
                }
                zero = true;
            }
        }
    }
    for(int i=0; i<=indeks; i++)
    {
        free(plansza[i].tab);
        free(plansza[i].odwiedzony);
    }
    free(plansza);
    free(stock.stosik);
    return 0;
}
