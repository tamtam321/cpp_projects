#ifndef MULTILEVEL_LIST_MULTI_LVL_LIST_HPP
#define MULTILEVEL_LIST_MULTI_LVL_LIST_HPP

#include <ctime> // random numb generate,
#include <cstdlib> // random numb generate,
#include <vector>

/*#include <exception>
class UnderFlowException : public std::exception
{
public:
    const char *what() const noexcept { return "Alulcsordulas!"; }
};*/

class multilevel_list
{
   struct Node
   {
       int val;

       // A vektor node pointereket tartalmaz, ami megmondja, hogy az adott indexen, ami a csomópont
       // szintje ott a node-nak ki a rákövetkezője.
       std::vector<Node*> junction;

       Node() = default;
       Node(int val);
       Node(int val, Node *p);
   };

   Node *min, *max;
   int list_size;

public:
    multilevel_list();

    // Destruktor
    ~multilevel_list();

    bool is_empyty();
    //std::vector<Node*> each_lvl_min(int v);
    //void insert_update_adjacent(Node *n);
    void level_up(Node *n);

    void insert(int);
    void erase(int);
    bool contains(int);
    [[nodiscard]] size_t size() const;
};

multilevel_list::Node::Node(int val)
{
    this->val = val;
}

// Új node létrehozásánál, megadjuk az értéket és 0. szinten kire mutat.
multilevel_list::Node::Node(int val, Node *p)
{
    this->val = val;
    this->junction.push_back(p);
}

multilevel_list::multilevel_list()
{
    list_size = 0;

    // Létrehozom min, max node-ot amire min, max mutat.
    Node *tmp_min = new Node();
    Node *tmp_max = new Node();

    min = tmp_min;
    max = tmp_max;

    // Ha lista üres, akkor min következője a max.
    min->junction.push_back(max);
    max->junction.push_back(nullptr);
}

multilevel_list::~multilevel_list()
{
    if (!is_empyty())
    {
        // d-vel törlök, s-el lépek
        Node *d, *s = min;

        // Amíg s nem NIL, addig megyek az 0. szinten végig és kitörlöm egyenként a node-okat.
        while (s != nullptr)
        {
            d = s;
            s = s->junction[0];
            delete d;
        }
    }
    else // min & max alapból benne van az üres listában. Ezeket mindenképpen törölni kell.
    {
        delete min;
        delete max;
    }
}

bool multilevel_list::is_empyty()
{
    // Ha már a 0. szinten a min következője a max akkor nincs a listában elem.
    return (min->junction[0] == max);
}

// Ezzel a segédfüggvénnyel terveztem az érték előtti utolsó node-dokat eltárolni minden szinten. (Nem használom így külön, már be van építve)
/**std::vector<multilevel_list::Node*> multilevel_list::each_lvl_min(int v)
{
    // elm vektor fogja tárolni az adott értékhez tartozó minden szinten az utolsó nála
    // kisebb csomópont.
    std::vector<Node*> elm;

    // Mindig a minimumtól indulok.
    Node *step = min;

    // Eltárolom a listám szintjét, mert a minimum szintje az egyenló a lista max szintjével
    int lvl = min->junction.size() - 1;

    // Amíg nem érem el az alját, addig megy le
    while (lvl >= 0)
    {
        // Ha a szinten a rákövetkező érték kisebb mint az inputnak, akkor odalépek.
        if (step->junction[lvl]->val < v)
        {
            step = step->junction[lvl];
        }
        else // Különben beteszem az elm vektorba és lejjebb megyek egy szinttel.
        {
            elm.push_back(step);
            lvl--;
        }
    }

    // Ügyeljünk majd oda, hogy az "elm" elemei fordított sorrendben van.
    return elm;
}*/

// Beszúrás után update-telem az új node szomszédját meg az előzőjét. (Nem használom így külön, már be van építve)
/**void multilevel_list::insert_update_adjacent(Node *n)
{
    // Eltároljuk az új érték előtti utolsó node-okat.
    std::vector<Node*> lasts = each_lvl_min(n->val);

    // Összekötjük az előző következőjét az új következővel.
    // Aztán az előző következője egyenlő új.
    for (size_t i = 0; i < n->junction.size(); i++)
    {
        n->junction[i] = lasts[(lasts.size() - 1) - i]->junction[i];
        lasts[(lasts.size() - 1) - i]->junction[i] = n;
    }
}*/

// Az új node-dot bekötöm a következő szintekre 50%-os eséllyel.
void multilevel_list::level_up(Node *n)
{
    // Random szám generálás 0-1 között. 50% esély hogy egyes kapok, ha egyes akkor szintet lépek, különben nem.
    srand(time(nullptr));
    int chance_lvl_up = rand() % 2;

    while(chance_lvl_up != 0)
    {
        // nullptr-eket adok meg következő elemként a szinteken.
        // Az insert-ben a lasts segédvektor segítségével majd intézem a szomszédságot.
        n->junction.push_back(nullptr);

        // Közben a min, max szintekre oda kell figyelni, mert azoknak a szintje mindig a legnagyobb, ha lenne
        // új legnagyobb szint.
        // Az új szinten  a min az új node-ra mutat, ami új legmagasabb szintre van bekötve, hogy majd az update jó legyen.
        // És az új node új legmagasabb szinten a maxra mutat.
        if (min->junction.size() < n->junction.size())
        {
            min->junction.push_back(n);
            max->junction.push_back(nullptr);
            n->junction[n->junction.size() - 1] = max;
        }

        chance_lvl_up = rand() % 2;   // Újra generálok, hogy vajon bekössem-e a kövi szintre
    }
}

void multilevel_list::insert(int val)
{
    // Itt tárolom a val előtti utolsó node-dokat
    std::vector<Node*> lasts;

    // Ezzel lépek, a min-től indulok.
    Node *s = min;

    /// KERESÉS:
    // Megnézem, hogy van-e már benne, ha igen akkor nem csinálok semmit, de ha nincs
    // akkor beszúrom.

    // A min legfelső szintjéről indulok.
    int lvl = min->junction.size() - 1;

    // Itt nézem meg hogy már van e benne a beszúrandó node.
    // Ha igen akkor ne csináljon semmit, ha nem akkor beszúrom.

    // Amíg valid szinten vagyok addig pörögjön.
    while(lvl >= 0)
    {
        // Ha nem max a kövi és érték kisebb, akkor odalépek.
        if (s->junction[lvl] != max && s->junction[lvl]->val < val)
        {
            s = s->junction[lvl];
        }
        else if (s->junction[lvl] == max || s->junction[lvl]->val > val) // Ha kövi max vagy érték nagyobb, akkor lejjebb megyek egy szinttel. Mielőtt lemegyek lementem a node-dot a lasts-ba.
        {
            lasts.push_back(s);
            lvl--;
        }
        else if (s->junction[lvl]->val == val) // Ha benne van ne csináljon semmit.
        {
            return;
        }
    }

    // Ha nincs benne akkor létrehozom az új node-dot.
    Node *n = new Node(val, nullptr);

    // Behúzom a szintekre, ahol csak lehet.
    // Ha az új node új legmagasabb szintekre is be lesz kötve, akkor ott kezelem egyből a szomszédságot.
    // Csak majd akkor a lasts vektor méretére oda kell figyelni, mert nem járt az új szinteken és nem mentette le az ottani
    // utoldó node-dokat ami a beszúrandó erték előtt van.
    // Úgy kell majd indexelni a lasts-tet hogy a megfelelő szinteken a node-dok szomszédság update-ja jó legyen.
    level_up(n);

    // Ha az új node új legmagasabb szint(ek)re lett bekötve, akkor a lasts nem járt ott, ezért kezelni kell a lasts indexét mert kevesebb szinteken járt mint a node.
    if (n->junction.size() > lasts.size())
    {
        // A lasts hossza nem feltétlen ugyanaz mint az n->junction hossza, mert lehet hogy sokkal magasabb szintre is be van kötve
        // ahol már a lasts nem ért el az elején.
        // Ezt a diffet használom hogy az index jó legyen a node-dok kötözésénél.
        int diff = n->junction.size() - lasts.size();

        // Az újnak a következője az az előzőnek a következője.
        // Aztán az előző következője egyenlő új.
        // Azokon a szinteken megcsináljuk, ahová az új node be lett kötve. Kivéve az újonnan létrejövő szinteken, mert a level_up elintézi.
        for (size_t i = 0; i < n->junction.size() - diff; i++)
        {
            n->junction[i] = lasts[(lasts.size() - 1) - i]->junction[i];
            lasts[(lasts.size() - 1) - i]->junction[i] = n;
        }
    }
    else // Ha az új node nem lett bekötve egy új legmagasabb szintre vagy szintekre, akkor idejövünk, mert akkor a lasts nem "hiányos".
    {
        // Az újnak a következője az az előzőnek a következője.
        // Aztán az előző következője egyenlő új.
        // Azokon a szinteken megcsináljuk, ahová az új node be lett kötve.
        for (size_t i = 0; i < n->junction.size(); i++)
        {
            n->junction[i] = lasts[(lasts.size() - 1) - i]->junction[i];
            lasts[(lasts.size() - 1) - i]->junction[i] = n;
        }
    }

    // Ha beszúrtuk az elemet, akkor megnövelem a lista méretét eggyel.
    list_size++;
}

void multilevel_list::erase(int val)
{
    // Ezzel lépegetek
    Node *s;

    // Ez fogja tárolni a keresett node-dot
    Node *f = nullptr;

    // KERESÉS:
    // A min legfelső szintjétől kezdem a keresést.
    s = min;
    int lvl = min->junction.size() - 1;

    // Amíg nem léptem túl az utolsó szinten addig pörögjön.
    while(lvl >= 0)
    {
        // Ha van egyezés akkor megtaláltuk és eltároljuk f-ben.
        // Utána elkezdjük a megtalált szinten és lejjebb összekötni az előzőeket a törlendő utániakkal.
        if (s->junction[lvl]->val == val)
        {
            if (f == nullptr)
            {
                f = s->junction[lvl]; // Ebben tárolom törlendő értéket
            }

            // Ha kitörölném a keresett node-dot, és ha márcsak a min és a max marad, akkor a szint üres és törölni kell.
            if (s == min && f->junction[lvl] == max)
            {
                min->junction.pop_back();
                max->junction.pop_back();
            }

            // Az adott sziten az előző a törlendő következőjére mutat.
            // Aztán lejjebb megyek egy szinttel.
            s->junction[lvl] = f->junction[lvl];
            lvl--;
        }
        else if (s->junction[lvl] != max && s->junction[lvl]->val < val) // Ha kövi nem max vagyis nem az utolsó node-don vagyunk és kövi kisebb értékű, akkor odalépek.
        {
            s = s->junction[lvl];
        }
        else // Itt az az eset van, amikor vagy az utolsó node-don vagyunk vagy kövi érték nagyobb, akkor lejjebb megyek egy szinttel.
        {
            lvl--;
        }
    }

    // Ha nullptr akkor nincs mit törölni
    if (f != nullptr)
    {
        // Törlöm a megtalált elemet és csökkentem a lista méretét.
        delete f;
        list_size--;
    }
}

bool multilevel_list::contains(int val)
{
    // Min legfelső szintjétől indulok.
    // Step-pel lépek node-dokon és a szinteken.
    // Lvl változóval figyelek arra ne hogy legalsó szintnél lejjebb menjek
    // és legfelső szint értékét adom neki az elején.
    bool is_found = false;
    Node *step = min;
    int lvl = min->junction.size() - 1;

    // Amíg nem léptem túl az utolsó szinten addig pörögjön.
    while(lvl >= 0)
    {
        // Ha a kövi node értéke kisebb, mint a val akkor lépjen oda,
        // közben ügyelve hogy ne menjen túl a maxon.
        // Max előtt menjen le egy szinttel ha addigra sincs egyezés.
        if (step->junction[lvl] != max && step->junction[lvl]->val < val)
        {
            step = step->junction[lvl];
        }
        else if (step->junction[lvl] == max || step->junction[lvl]->val > val) // Ha kövi már a max vagyis az utolsó elemnél vagyok vagy nagyobb, akkor menjen le.
        {
            lvl--;
        }
        else if (step->junction[lvl]->val == val) // Ha van egyezés akkor megtaláltuk.
        {
            is_found = true;
        }

        // Ha már megvan akkor lépjünk ki.
        if (is_found)
        {
            break;
        }
    }

    return is_found;
}

size_t multilevel_list::size() const
{
    return list_size;
}

#endif //MULTILEVEL_LIST_MULTI_LVL_LIST_HPP
