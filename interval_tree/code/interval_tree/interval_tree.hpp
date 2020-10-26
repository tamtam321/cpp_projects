//
// Created by tam on 2019. 11. 07..
//

#ifndef INTERVAL_TREE_INTERVAL_TREE_HPP
#define INTERVAL_TREE_INTERVAL_TREE_HPP

#include <iostream>
#include "exceptions.hpp"

class interval_tree
{
private:
    // Szín felsoroló típus
    enum color_t { black, red};

    // típus nevek rövidítés érdekében
    // innentől a std::pair<int, int> helyett Key_t lesz
    // a pair<const pair<int, int>, string> helyett meg value_type
    // ez kb egy pair<intervallum, cimke> ahol a kulcs ay intervallum a cimke meg
    // csak plusz info
    using Key_t = std::pair<int, int>;
    using value_type = std::pair<const Key_t, std::string>;

    // Belső csúcs struktúra
    class Node
    {
    public:

        Node *parent, *left, *right;
        Key_t internal_node_key;
        value_type key_with_label;
        int node_max;
        color_t color;

        // Az alapértelmezett konstruktor: csak az empty_leaf létrehozására szolgál
        Node() : parent(this), left(this), right(this),
            internal_node_key(), key_with_label(), node_max(0), color(black) {}

        // Konstruktor csúcs létrehozására beszúráskor
        Node(Node* p, Node* empty_leaf, const Key_t &ink, const std::string &_label) :
            parent(p), left(empty_leaf), right(empty_leaf), internal_node_key(ink), key_with_label({ink, _label}), color(red)
        {
            node_max = internal_node_key.second;
        }
    };

    static Node _empty_leaf;
    static Node *p_empty_leaf;
    Node *root;

public:
// Konstruktor és destruktor
    interval_tree() : root(p_empty_leaf) {};

    ~interval_tree() {_destroy(root);}

// Másoló konstruktor és operátor implementálása opcionális
    interval_tree(const interval_tree &t) = delete;
    interval_tree &operator=(const interval_tree &t) = delete;
    interval_tree(const interval_tree &&t) = delete;
    interval_tree &operator=(interval_tree &&t) = delete;

    ///______Második rész(begin):_______
    class iterator
    {
    private:
        Node * current = nullptr;

    public:
        explicit iterator(Node *n) : current(n) {}

// iteratorok összehasonlítása
        bool operator==(const iterator &other)
        {
            return other.current == current;
        }

        bool operator!=(const iterator &other)
        {
            return other.current != current;
        }

// Klasszikus iterátoraritmetika. Vegyük észre, hogy nagyon hasonló a
// pointeréhez
        iterator &operator++()
        {
            current = _next(current);
            return *this;
        }

// Prefix következőre léptetés iteratorokra
        iterator operator++(int)    // Postfix következőre léptetés iteratorokra
        {
            iterator it(current);
            current = _next(current);
            return it;
        }

        iterator &operator--()
        {
            current = _prev(current);
            return *this;
        }

// Prefix előzőre léptetés iteratorokra
        iterator operator--(int)    // Postfix előzőre léptetés iteratorokra
        {
            iterator it(current);
            current = _prev(current);
            return it;
        }

// Dereferencia iterátorokra
        value_type &operator*()
        {
            return current->key_with_label;
        }

// Structure dereference operátor
        value_type *operator->()
        {
            return &(current->key_with_label);
        }
    };

    iterator begin();
    iterator end();
    ///_____Második rész(end):___________

//______Segéd tagfv begin_______________

    void _destroy(Node *x);
    void _rotate_left(Node *x);
    void _rotate_right(Node *x);
    static size_t _size(Node *x);
    static Node *_min(Node *x);
    static Node *_max(Node *x);
    static Node *_next(Node *x);
    static Node *_prev(Node *x);
    void _rebalance_after_insert(Node *x);
    void _rebalance_after_remove(Node *x);
    static size_t _validate(Node *x);

//______Segéd tagfv end__________

// Alapműveletek
    size_t size() const;
    void insert(const Key_t &k, const std::string &v);
    void remove(const Key_t &k);
    value_type *find(const Key_t &k);
    const value_type *find(const Key_t &k) const;

    /// Második rész:
    iterator find_i(const Key_t &k);

// Új művelet
    value_type *interval_search(const Key_t &i);

    /// Második rész:
    iterator interval_search_i(const Key_t &i);

// Ellenőrző fgvk
    void validate() const;
    int max(const Key_t &i) const;
};

interval_tree::Node interval_tree::_empty_leaf;
interval_tree::Node *interval_tree::p_empty_leaf = &_empty_leaf;

//_______Segéd tagfv-ek definiálása begin____________

void interval_tree::_destroy(Node *x)
{
    if (x != p_empty_leaf)
    {
        _destroy(x->left);
        _destroy(x->right);
        delete x;
    }
}

void interval_tree::_rotate_left(Node *x)
{
    // y-nak nevezzük el x jobb gyerekét
    // a forgatás az x-y él mentén történik
    Node *y = x->right;

    // y-nak mindenképpen létező csúcsnak kell lennie
    if (y == p_empty_leaf)
        throw internal_error();

    // y bal gyereke forgatás után x jobb gyereke lesz
    // a gyerek szülő mezőjét is frissíteni kell
    x->right = y->left;
    if (y->left != p_empty_leaf) /* a feltétel opcionális */
    {
        y->left->parent = x;
    }

    // az adott részfának mostantól y lesz a gyökere
    // így megkapja x szülőjét, és
    // a szülőnél is be kell állítani, hogy mostantól y az ő gyereke
    y->parent = x->parent;
    if (x->parent == p_empty_leaf)
        root = y;
    else if (x == x->parent->left)
        x->parent->left = y;
    else
        x->parent->right = y;

    // végül beállítjuk x és y között a szülő-gyerek kapcsolatot
    y->left = x;
    x->parent = y;

    // Forgatás után is frissítem a maxokat

    int tmp_max = std::max(x->left->node_max, x->right->node_max);

    if(tmp_max > x->internal_node_key.second)
    {
        x->node_max = tmp_max;
    }
    else
    {
        x->node_max = x->internal_node_key.second;
    }

    tmp_max = std::max(x->node_max, y->right->node_max);

    if(tmp_max > y->internal_node_key.second)
    {
        y->node_max = tmp_max;
    }
    else
    {
        y->node_max = y->internal_node_key.second;
    }
}

void interval_tree::_rotate_right(Node *x)
{
    // y-nak nevezzük el x bal gyerekét
    // a forgatás az x-y él mentén történik
    Node *y = x->left;

    // y-nak mindenképpen létező csúcsnak kell lennie
    if (y == p_empty_leaf)
        throw internal_error();

    // y jobb gyereke forgatás után x bal gyereke lesz
    // a gyerek szülő mezőjét is frissíteni kell
    x->left = y->right;
    if (y->right != p_empty_leaf) /* a feltétel opcionális */
        y->right->parent = x;

    // az adott részfának mostantól y lesz a gyökere
    // így megkapja x szülőjét, és
    // a szülőnél is be kell állítani, hogy mostantól y az ő gyereke
    y->parent = x->parent;
    if (x->parent == p_empty_leaf)
        root = y;
    else if (x == x->parent->left)
        x->parent->left = y;
    else
        x->parent->right = y;

    // végül beállítjuk x és y között a szülő-gyerek kapcsolatot
    y->right = x;
    x->parent = y;

    // Forgatás után beállítom a maxokat

    int tmp_max = std::max(x->left->node_max, x->right->node_max);

    if(tmp_max > x->internal_node_key.second)
    {
        x->node_max = tmp_max;
    }
    else
    {
        x->node_max = x->internal_node_key.second;
    }

    tmp_max = std::max(x->node_max, y->left->node_max);

    if(tmp_max > y->internal_node_key.second)
    {
        y->node_max = tmp_max;
    }
    else
    {
        y->node_max = y->internal_node_key.second;
    }
}

size_t interval_tree::_size(Node *x)
{
    if (x == p_empty_leaf)
        return 0;
    else
        return _size(x->left) + _size(x->right) + 1;
}

interval_tree::Node *interval_tree::_min(Node *x)
{
    while (x->left != p_empty_leaf)
        x = x->left;
    return x;
}

interval_tree::Node *interval_tree::_max(Node *x)
{
    while (x->right != p_empty_leaf)
        x = x->right;
    return x;
}

interval_tree::Node *interval_tree::_next(Node *x)
{
    if (x->right != p_empty_leaf)
        return _min(x->right);

    Node *y = x->parent;
    while (y != p_empty_leaf && x == y->right)
    {
        x = y;
        y = y->parent;
    }
    return y;
}

interval_tree::Node *interval_tree::_prev(Node *x)
{
    if (x->left != p_empty_leaf)
        return _max(x->left);

    Node *y = x->parent;
    while (y != p_empty_leaf && x == y->left)
    {
        x = y;
        y = y->parent;
    }

    return y;
}

void interval_tree::_rebalance_after_insert(Node *x)
{
    // x: problemas node - (piros szulo) piros gyermeke
    // y: x nagybacsija
    // p: szulo
    // g: nagyszulo

    /*
             g
            / \
           p   y
          /
         x
    */

    // A while ciklus minden egyes lefutasara egy adott szinten tortenik
    // a PF fa tulajdonsagok helyreallitasa.
    //

    while (x->parent->color == red)
    {
        // ha x a nagyapa BAL oldalan helyezkedik el
        if (x->parent == x->parent->parent->left)
        {
            // y legyen x nagybacsija
            Node *y = x->parent->parent->right;

            /// 1. eset:
            ///    -- elofeltetel  : y PIROS
            ///    -- kovetkezmeny : g PIROS , p FEKETE , y FEKETE
            ///                      Ket szintel feljebb lepve kezdjuk elorol a 1. esettol
            if (y->color == red)
            {
                x->parent->color = black;
                y->color = black;
                x->parent->parent->color = red;

                // ket szintel feljebb lepunk es ott folytatjuk a helyreallitast
                x = x->parent->parent;
                continue;
            }

            /// 2. eset:
            ///    -- elofeltetel  : y FEKETE , x JOBB gyermek
            ///    -- kovetkezmeny : x BAL gyermek
            if (x == x->parent->right)
            {
                x = x->parent;
                _rotate_left(x);
            }

            /// 3. eset:
            ///    -- elofeltetel  : y FEKETE , x BAL gyermek
            ///    -- kovetkezmeny : a PF fa tulajdonsagai helyrealltak
            x->parent->color = black;
            x->parent->parent->color = red;
            _rotate_right(x->parent->parent);
        }

            // ha x a nagyapa JOBB oldalan helyezkedik el
        else
        {
            // y legyen x nagybacsija
            Node *y = x->parent->parent->left;

            /// 1. eset:
            ///    -- elofeltetel  : y PIROS
            ///    -- kovetkezmeny : g PIROS , p FEKETE , y FEKETE
            ///                      Ket szintel feljebb lepve kezdjuk elorol a 1. esettol
            if (y->color == red)
            {
                x->parent->color = black;
                y->color = black;
                x->parent->parent->color = red;

                // ket szintel feljebb lepunk es ott folytatjuk a helyreallitast
                x = x->parent->parent;
                continue;
            }

            /// 2. eset:
            ///    -- elofeltetel  : y FEKETE , x BAL gyermek
            ///    -- kovetkezmeny : x JOBB gyermek
            if (x == x->parent->left)
            {
                x = x->parent;
                _rotate_right(x);
            }

            /// 3. eset:
            ///    -- elofeltetel  : y FEKETE , x JOBB gyermek
            ///    -- kovetkezmeny : a PF fa tulajdonsagai helyrealltak
            x->parent->color = black;
            x->parent->parent->color = red;
            _rotate_left(x->parent->parent);
        }
    }
    root->color = black;
}

void interval_tree::_rebalance_after_remove(Node *x)
{
    // x: problemas node (DUPLA FEKETE)
    // w: x testvere

    // A while ciklus minden egyes lefutasara egy adott szinten tortenik
    // a PF fa tulajdonsagok helyreallitasa.
    //
    // Felfele haladunk. Ha elerunk egy piros nodot, vagy a gyokeret,
    // akkor vegeztunk a helyreallitassal.
    //
    while (x != root && x->color == black)
    {
        // ha x BAL gyerek
        if (x == x->parent->left)
        {
            Node *w = x->parent->right;

            /// 1. eset
            ///    -- elofeltetel  : w->color == red   [a testver PIROS]
            ///    -- kovetkezmeny : w->color == black [a testver FEKETE] (2. eset, 3. eset vagy 4. eset)
            if (w->color == red)
            {
                w->color = black;
                x->parent->color = red;
                _rotate_left(x->parent);
                w = x->parent->right;
            }

            // ezen a ponton : w->color == black

            /// 2. eset
            ///    -- elofeltetel  : w es w mindket gyermeke FEKETE
            ///    -- kovetkezmeny : a dupla fekete eggyel feljebb propagal,
            ///                      ezen a szinten nincs tobb keresnivalonk
            ///                      Elorol az egeszet a x->parent node-al.
            if (w->left->color == black && w->right->color == black)
            {
                w->color = red;

                // tovabba x megszunik ketszeres feketenek lenni,
                // mert a tobblet feketet megkapja a szulo (x->parent).
                // ezert egy szintel feljebb folytatjuk a PF tulajdonsagok helyreallitasat
                x = x->parent;
                continue;
            }

            /// 3. eset
            ///    -- elofeltetel  : w FEKETE , w->left PIROS , w->right FEKETE
            ///    -- kovetkezmeny : w FEKETE , w->left ????? , w->right PIROS   (4. eset)
            if (w->right->color == black)
            {
                w->left->color = black;
                w->color = red;
                _rotate_right(w);
                w = x->parent->right;
            }

            // ezen a ponton : w->right->color == red

            /// 4. eset: w fekete, w->right
            ///    -- elofeltetel  : w FEKETE , w->right PIROS
            ///    -- kovetkezmeny : a PF fa tulajdonsagai helyrealltak
            w->color = x->parent->color;
            x->parent->color = black;
            w->right->color = black;
            _rotate_left(x->parent);

            break;  // vegeztunk
        }

            // ha x JOBB gyerek -- ugyanaz csak minden forditva
        else
        {
            Node *w = x->parent->left;

            /// 1. eset
            ///    -- elofeltetel  : w->color == red   [a testver PIROS]
            ///    -- kovetkezmeny : w->color == black [a testver FEKETE] (2. eset, 3. eset vagy 4. eset)
            if (w->color == red) {
                w->color = black;
                x->parent->color = red;
                _rotate_right(x->parent);
                w = x->parent->left;
            }

            // ezen a ponton : w->color == black

            /// 2. eset
            ///    -- elofeltetel  : w es w mindket gyermeke FEKETE
            ///    -- kovetkezmeny : a duple fekete eggyel feljebb propagal,
            ///                      ezen a szinten nincs tobb keresnivalonk
            if (w->left->color == black && w->right->color == black) {
                w->color = red;

                // tovabba x megszunik ketszeres feketenek lenni,
                // mert a tobblet feketet megkapja a szulo (x->parent).
                // ezert egy szintel feljebb folytatjuk a PF tulajdonsagok helyreallitasat
                x = x->parent;
                continue;
            }

            /// 3. eset
            ///    -- elofeltetel  : w FEKETE , w->left FEKETE , w->right PIROS
            ///    -- kovetkezmeny : w FEKETE , w->left PIROS  , w->right ?????   (4. eset)
            if (w->left->color == black) {
                w->right->color = black;
                w->color = red;
                _rotate_left(w);
                w = x->parent->left;
            }

            // ezen a ponton : w->left->color == red

            /// 4. eset:
            ///    -- elofeltetel  : w FEKETE, w->right PIROS
            ///    -- kovetkezmeny : a PF fa tulajdonsagai helyrealltak
            w->color = x->parent->color;
            x->parent->color = black;
            w->left->color = black;
            _rotate_right(x->parent);

            break; // vegeztunk
        }
    }

    x->color = black;
    p_empty_leaf->parent = p_empty_leaf;
}

size_t interval_tree::_validate(Node *x)
{
    // empty_leaf fekete-magassága nulla
    if (x == p_empty_leaf)
        return 0;

    // "Minden csúcs színe piros vagy fekete."
    if (x->color != red && x->color != black)
        throw invalid_rb_tree();

    // "Minden piros csúcs mindkét gyereke fekete."
    if (x->color == red && x->parent->color == red)
        throw invalid_rb_tree();

    // Rekurzív ellenőrzés és fekete-magasság meghatározása
    size_t left_black_height = _validate(x->left);
    size_t right_black_height = _validate(x->right);

    // "Bármely gyökértől levélig vezető úton
    // a fekete csúcsok száma egyenlő."
    if (left_black_height != right_black_height)
        throw invalid_rb_tree();

    // Visszaadjuk a részfa fekete-magasságát
    return left_black_height + (x->color == black);
}

//_______Segéd tagfv-ek definiálása end______________

interval_tree::iterator interval_tree::begin()
{
    return iterator(_min(root));
}

interval_tree::iterator interval_tree::end()
{
    return iterator(p_empty_leaf);
}

size_t interval_tree::size() const
{
    return _size(root);
}

void interval_tree::insert(const Key_t &k, const std::string &v)
{
    // Keresés
        Node *y = p_empty_leaf;
        Node *x = root;


        // Beszúráskor updatelem a max értékeket
        // Rebalance-on belül a forgatásoknál is kell updatelni
        while (x != p_empty_leaf && !(k == x->internal_node_key))
        {
            y = x;
            if (k < x->internal_node_key)
            {
                if(k.second > x->node_max)
                {
                    x->node_max = k.second;
                }
                x = x->left;
            }
            else
            {
                if(k.second > x->node_max)
                {
                    x->node_max = k.second;
                }
                x = x->right;
            }
        }

        // Ha van már ilyen kulcsú elem a fában, úgy nincs dolgunk.
        if (x != p_empty_leaf)
            return;

        // Új csúcs létrehozása és bekötése
        Node *z = new Node(y, p_empty_leaf, k, v);

        if (y == p_empty_leaf)
            root = z;
        else if (z->internal_node_key.first < y->internal_node_key.first)
            y->left = z;
        else
            y->right = z;

        // Beszúrás utáni kiegyensúlyozás
        _rebalance_after_insert(z);
}

void interval_tree::remove(const Key_t &k)
{
    // Keresés
    Node *z = root;
    while (z != p_empty_leaf && !(k == z->internal_node_key))
        if (k < z->internal_node_key)
            z = z->left;
        else
            z = z->right;

    // Ha nincs ilyen kulcsú elem a fában, úgy nincs dolgunk.
    if (z == p_empty_leaf)
        return;

    // Csúcs kivágása a fából és felszabadítás
    Node *y;
    if (z->left == p_empty_leaf || z->right == p_empty_leaf)
        y = z;
    else
        y = _next(z);

    Node *x;
    if (y->left != p_empty_leaf)
        x = y->left;
    else
        x = y->right;

    // FIGYELEM: A következő sor módosíthatja a empty_leaf csúcsot!
    x->parent = y->parent;

    if (y->parent == p_empty_leaf)
    {
        root = x;
    }
    else if (y == y->parent->left)
    {
        y->parent->left = x;
    }
    else
    {
        y->parent->right = x;
    }

    // x-től indulva felfelé updatelem a maxok értékét
    // A rebalance forgatásánál is updatelem a maxot

    Node *step_back = x->parent;
    int tmp_max = std::max(step_back->left->node_max, step_back->right->node_max);

    while(step_back != p_empty_leaf)
    {
        if(tmp_max > step_back->internal_node_key.second)
        {
            step_back->node_max = tmp_max;
        }
        else
        {
            step_back->node_max = step_back->internal_node_key.second;
        }

        step_back = step_back->parent;
        tmp_max = std::max(step_back->left->node_max, step_back->right->node_max);
    }


    if (y != z)
    {
        // y & z Node-ot felcserélem.
        // y Node pointerei mutassanak a z szülőjére és gyerekeire.
        // A szülők és a gyerekek is mutassanak y-ra.
        // z Node-t szimplán törlöm, de még előtte színét megváltoztatom az y színére, hogy az rp_tree rebalance-ja
        // jókor hívodjon meg.

        Node *tmp_z = z;
        Node *tmp_y = y;

        y->parent = tmp_z->parent;
        y->left = tmp_z->left;
        y->right = tmp_z->right;
        y->color = tmp_z->color;

        if(tmp_z == tmp_z->parent->left)
        {
            tmp_z->parent->left = y;
        }
        else
        {
            tmp_z->parent->right = y;
        }

        if(tmp_z->left != p_empty_leaf)
            tmp_z->left->parent = y;

        if(tmp_z->right != p_empty_leaf)
            tmp_z->right->parent = y;

        z->color = tmp_y->color;
    }


    bool z_black = z->color == black;
    delete z;

    // Törlés utáni kiegyensúlyozás
    if (z_black)
        _rebalance_after_remove(x);

}

interval_tree::value_type *interval_tree::find(const Key_t &k)
{
    Node *x = root;
    while (x != p_empty_leaf && !(k == x->internal_node_key))
        if (k < x->internal_node_key)
            x = x->left;
        else
            x = x->right;

    if(x == p_empty_leaf)
    {
        return nullptr;
    }
    else
    {
        return &x->key_with_label;
    }
}

const interval_tree::value_type *interval_tree::find(const Key_t &k) const
{
    Node *x = root;
    while (x != p_empty_leaf && !(k == x->internal_node_key))
        if (k < x->internal_node_key)
            x = x->left;
        else
            x = x->right;

    if(x == p_empty_leaf)
    {
        return nullptr;
    }
    else
    {
        return &x->key_with_label;
    }
}

interval_tree::iterator interval_tree::find_i(const Key_t &k)
{
    Node *x = root;
    while (x != p_empty_leaf && !(k == x->internal_node_key))
        if (k < x->internal_node_key)
            x = x->left;
        else
            x = x->right;

    if(x == p_empty_leaf)
    {
        return iterator(p_empty_leaf);
    }
    else
    {
        return iterator(x);
    }
}

interval_tree::value_type *interval_tree::interval_search(const Key_t &i)
{
    Node *x = root;

    while(x != p_empty_leaf && !(i.first <= x->internal_node_key.second && i.second >= x->internal_node_key.first))
    {
        if(x->left != p_empty_leaf && x->left->node_max >= i.first)
        {
            x = x->left;
        }
        else
        {
            x = x->right;
        }
    }

    if(x == p_empty_leaf)
    {
        return nullptr;
    }
    else
    {
        return &x->key_with_label;
    }
}

interval_tree::iterator interval_tree::interval_search_i(const Key_t &i)
{
    Node *x = root;

    while(x != p_empty_leaf && !(i.first <= x->internal_node_key.second && i.second >= x->internal_node_key.first))
    {
        if(x->left != p_empty_leaf && x->left->node_max >= i.first)
        {
            x = x->left;
        }
        else
        {
            x = x->right;
        }
    }

    if(x == p_empty_leaf)
    {
        return iterator(p_empty_leaf);
    }
    else
    {
        return iterator(x);
    }
}

void interval_tree::validate() const
{
    // Keresőfa tulajdonság ellenőrzése bejárással
    if (root != p_empty_leaf)
    {
        Node *x = _min(root);
        interval_tree::Key_t prev = x->internal_node_key;
        while ((x = _next(x)) != p_empty_leaf)
        {
            if (!(prev < x->internal_node_key))
                throw invalid_binary_search_tree();
            prev = x->internal_node_key;
        }
    }

    //
    // Piros-fekete fa tulajdonságok ellenőrzése
    //

    // empty_leaf csúcs ellenőzése
    // "Minden levél színe fekete."
    if (p_empty_leaf->left != p_empty_leaf || p_empty_leaf->right != p_empty_leaf
        || p_empty_leaf->color != black)
        throw invalid_rb_tree();

    // "A gyökér színe fekete."
    if (root->color != black)
        throw invalid_rb_tree();

    // A fa rekurzív ellenőrzése
    _validate(root);
}

int interval_tree::max(const Key_t &i) const
{
    Node *x = root;

    while (x != p_empty_leaf && !(i == x->internal_node_key))
    {
        if (i < x->internal_node_key)
        {
            x = x->left;
        }
        else
        {
            x = x->right;
        }
    }

    if(x != p_empty_leaf)
        return x->node_max;
    else
        return 0;
}

#endif //INTERVAL_TREE_INTERVAL_TREE_HPP
