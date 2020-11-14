#ifndef MULTILEVEL_LIST_MULTI_LVL_LIST_HPP
#define MULTILEVEL_LIST_MULTI_LVL_LIST_HPP

#include <ctime> // random numb generate,
#include <cstdlib> // random numb generate,
#include <vector>

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

   // Listának van egy lasts_node vektora ami majd eltárolja a beszúrandó/törlendő node érték
   // előtti utolsó node-dokra mutató pointert, minden szinten. Ezt használom hogy a szomszédságot megadjam, meg a keresésnél is használom.
   std::vector<Node*> lasts_node;

   [[nodiscard]] std::vector<Node*> each_lvl_min(int v) const;

public:
    multilevel_list();

    // Destruktor
    ~multilevel_list();

    bool is_empyty();
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

    // Lefixálom a szintet -> 10 szintűre.
    int list_level = 10;

    // Létrehozom min, max node-ot amire min, max mutat.
    Node *tmp_min = new Node();
    Node *tmp_max = new Node();

    min = tmp_min;
    max = tmp_max;

    // Ha lista üres, akkor min következője a max.
    // A szintet az elején fixálom, a min és max mindegyik szintre be van kötve.
    // For ciklussal bekötöm öket mindegyik szintre.
    for (int i = 0; i < list_level; i++)
    {
        min->junction.push_back(max);
        max->junction.push_back(nullptr);
    }
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

// Ezzel adom meg a keresett érték előtti utolsó node-dokat miden szinten.
// Csak itt járom be a teljes listát, ezt használom hogy minden egyes insert, erase, contains hívásnál csak 1-szer járom be a listát. (Szerintem)
std::vector<multilevel_list::Node*> multilevel_list::each_lvl_min(int v) const
{
    // elm vektor fogja tárolni az adott értékhez tartozó minden szinten az utolsó nála
    // kisebb node-dokat.
    std::vector<Node*> elm;

    // Mindig a minimumtól indulok.
    Node *step = min;

    // Eltárolom a listám szintjét, a legfelső szintről indítom a keresést.
    // A min mindegyik szinten be van kötve.
    int lvl = min->junction.size() - 1;

    // Ha már 0. szinten túl megyek akkor leáll, de addig pörög.
    while (lvl >= 0)
    {
        // Ha a szinten a rákövetkező nem max vagyis ha nem az utolsó node-don vagyok a szinten és érték kisebb mint az inputnak, akkor odalépek.
        if (step->junction[lvl] != max && step->junction[lvl]->val < v)
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
}

// Az új node-dot bekötöm a következő szintekre 50%-os eséllyel.
void multilevel_list::level_up(Node *n)
{
    // Random szám generálás 0-1 között. 50% esély hogy egyes kapok, ha egyes akkor szintet lépek, különben nem.
    srand(time(nullptr));
    int chance_lvl_up = rand() % 2;

    // Ameddig chance_lvl_up == 1 és az új node nem lép túl a fixált szinten, akkor prögjön a ciklus.
    while(chance_lvl_up != 0 && n->junction.size() < min->junction.size())
    {
        // nullptr-eket adok meg következő elemként a szinteken.
        // Az insert-ben a lasts segédvektor segítségével majd intézem a szomszédságot.
        n->junction.push_back(nullptr);

        chance_lvl_up = rand() % 2;   // Újra generálok, hogy vajon bekössem-e a kövi szintre
    }
}

void multilevel_list::insert(int val)
{
    // Ha benne van akkor nem csinálok semmit, ha nincs akkor beszúrom.
    if (!contains(val))
    {
        // Létrehozom az új node-dot és megadom neki, hogy 0. szinten egyenlőre csak nullptre-re mutasson, később megadom neki hogy ki a kövi.
        Node *n = new Node(val, nullptr);

        // Itt kötöm be az új node-dot a szintekre, 50% eséllyel minden szinten.
        level_up(n);

        // contains-ben az each_lvl_min metódus bejárta a listát és ennek a segítségével töltöm fel a
        // lasts_node vektort, a megfelelő értékre nézve.
        // Itt adom meg, hogy a beszúrandó node az előző következőjére mutasson és
        // az előző meg az újra.
        // A lasts_node vektorban fordítva vannak aszintek, az utolsó eleme tárolja a 0. szinthez tartozó
        // érték előtti utolsó node_ra mutató pointert.
        for (size_t i = 0; i < n->junction.size(); i++)
        {
            n->junction[i] = lasts_node[(lasts_node.size() - 1) - i]->junction[i];
            lasts_node[(lasts_node.size() - 1) - i]->junction[i] = n;
        }

        // Megnövelem a lista méretét.
        list_size++;
    }
}

void multilevel_list::erase(int val)
{
    // Ha benne van akkor törlöm különben nem csinálok semmit.
    if (contains(val))
    {
        // contains-ben az each_lvl_min metódus bejárta a listát és ennek a segítségével töltöm fel a
        // lasts_node vektort, a megfelelő értékre nézve.

        // Ha benne van, akkor a lasts_node-ban az utolsó elem, mivel fordítva vannak benne a szintek,
        // utolsó elem a 0. szinten, érték előtt lévő utolsó node-ra mutató pointert tárolja.
        // Ha annak lekérem a következő elemét akkor a törlendő elemre kéne, hogy mutasson, mert contains szerint benne van.
        // Azt adom az f-nek.
        Node *f = lasts_node[lasts_node.size() - 1]->junction[0];

        // Ahol az f be van kötve, ott rendezem a szomszédokat.
        // Előtte lévő node következője az az f következője lesz.
        for (size_t i = 0; i < f->junction.size(); i++)
        {
            lasts_node[(lasts_node.size() - 1) - i]->junction[i] = f->junction[i];
        }

        // f-et törlöm és lista mérete csökken eggyel.
        delete f;
        list_size--;
    }
}

bool multilevel_list::contains(int val)
{
    // Itt járom be a listát és töltöm fel a lasts_node vektort az érték előtti utolsó node-dokra mutató pointerekkel.
    lasts_node = each_lvl_min(val);

    // Ha benne van, akkor a 0. szinten a lasts_node vektorban tárolt node-nak a következője a keresett értéknek kell hogy legyen.
    // Ha igaz akkor benne van. Különben nincs.
    if (lasts_node[lasts_node.size() - 1]->junction[0]->val == val)
    {
        return true;
    }

    return false;
}

size_t multilevel_list::size() const
{
    return list_size;
}

#endif //MULTILEVEL_LIST_MULTI_LVL_LIST_HPP
