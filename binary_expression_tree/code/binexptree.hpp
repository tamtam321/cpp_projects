#ifndef KHF03_BINEXPTREE_HPP
#define KHF03_BINEXPTREE_HPP

#include <iostream>
#include <vector>
#include <stack>
#include <queue>
#include <sstream>
#include <utility>

#include <exception>

class tree_is_empty_error : public std::exception
{
public:
    const char *what() const throw() { return "Fa alapbol ures, igy hibas a muvelet."; }
};

class BinExpTree
{
    struct Node
    {
        std::string value;
        Node *left, *right;

        Node();
        explicit Node(std::string val);
    };

    Node *root;

public:
    BinExpTree();
    ~BinExpTree();

    explicit BinExpTree(std::string expression);

    [[nodiscard]] static std::vector<std::string> parseExpression(const std::string &expression);
    std::string printExpression() const;
    double evaluateExpression() const;

    //_____________Segéd Fv-ek____________

    // Ezzel vizsgálom az operátor prioritását
    static bool priority(char operator_);

    // Inorder fabejárással íratom ki az infix alakot.
    static void _inorder(const Node* i, std::string &s, bool &is_open);

    // Postorder fabejárással értékelem ki a fában az egyenletet.
    static double _postorder(const Node *i);

    // Destruktorhoz segéd fv, kitörli a node-dokat a fában.
    static void _destroy(Node *x);

    //___________________________________
};

BinExpTree::Node::Node()
{
    value = "0";
    left = nullptr;
    right = nullptr;
}

BinExpTree::Node::Node(std::string val)
{
    // vagy value = val;
    // valszeg move-val gyorsabb és nincs felesleges másolat készítés.
    value = std::move(val);
    left = nullptr;
    right = nullptr;
}

// postfix_v vektorban tárolom el a kifejezés postfix alakját.
// st stack az egy segédtároló, hogy majd a node-dokat megfelelően összekössem egymással.
// For ciklussal végig megyek a postfix vektoron és elemenként beteszem őket egy új node-ba.
// Amíg operandus addig megy be a stackbe, de amikor jön egy operátor, akkor a gyerekei a stack felső két eleme lesz.
// Amikor sikerült operátor, operandus összekötés, akkor a két gyereket kiszedem a stackből és a szülőt teszem vissza a stack-be.
// Ugyanígy folytatva amíg végig nem érünk a vektoron. Tulajdonképpen alulról felfelé építjük a fát.
BinExpTree::BinExpTree(std::string expression)
{
    std::vector<std::string> postfix_v = parseExpression(expression);
    std::stack<Node*> st;

    for (size_t i = 0; i < postfix_v.size(); i++)
    {
        Node *n = new Node(postfix_v[i]);

        // Ha szám akkor csak beteszem a stack-be.
        if (isdigit(postfix_v[i][0]))
        {
            st.push(n);
        }
        else // Ha operátor, akkor stack felső 2 elemét kiveszem és ők lesznek az operátor gyerekei.
        {
            Node *right_child = st.top();
            st.pop();
            Node *left_child = st.top();
            st.pop();

            n->left = left_child;
            n->right = right_child;

            st.push(n);

            // Az utolsó node operátor lesz a root.
            if (i == postfix_v.size() - 1)
            {
                root = n;
            }
        }
    }
}

BinExpTree::BinExpTree()
{
    root = nullptr;
}

BinExpTree::~BinExpTree()
{
    _destroy(root);
}

// Itt alakítom át az infix kifejezést postfixre.
std::vector<std::string> BinExpTree::parseExpression(const std::string &expression)
{
    // original_q -> ebben tárolom el az expression-t (infix alak).
    // st -> stack az egy segédtároló
    // result_v -> postfix alakot itt tárolom el és ezzel térek vissza.
    std::queue<char> original_q;
    std::stack<char> st;
    std::vector<std::string> result_v;

    // Feltöltöm a original_q queue-t
    for (char i : expression)
    {
        original_q.push(i);
    }

    // c segítségével dolgozom fel az infix alak karaktereit.
    // Az órai lengyel forma kódot felhasználtam és azt átalakítottam.
    while (!original_q.empty())
    {
        char c = original_q.front();
        original_q.pop();

        if (c == '(')
        {
            st.push(c);
        }
        else if (c == ')')
        {
            while (st.top() != '(')
            {
                // push_back vs emplace_back ???
                result_v.emplace_back(std::string(1, st.top()));
                st.pop();
            }

            // Benne van még a "(", amit kiszedek itt.
            st.pop();
        }
        else if (c == '+' || c == '-' || c == '*' || c == '/')
        {
            while (!st.empty() && st.top() != '(' &&
                  ((priority(st.top()) > priority(c)) ||
                  (priority(st.top()) == priority(c))))
            {
                // push_back vs emplace_back ???
                result_v.emplace_back(std::string(1, st.top()));
                st.pop();
            }

            st.push(c);
        }
        else // Ha szám
        {
            // Ha tizedes szám (csak 1 tizedesig van írva max), akkor az
            // előző számhoz hozzáfűzöm a pontot és az utána lévő számot.
            // (Ha esetleg több tizedes lenne, akkor talán while-lal addig fűzném a pont után a számokat, amíg operátorral nem találkozom.)
            if (c == '.')
            {
                result_v[result_v.size() - 1] += c;
                c = original_q.front();
                original_q.pop();
                result_v[result_v.size() - 1] += c;
            }
            else // Ha egész
            {
                // push_back vs emplace_back ???
                result_v.emplace_back(std::string(1, c));
            }
        }
    }

    // Ha üres a queue, akkor a stackből a maradékot beteszem a result vektorba.
    while (!st.empty())
    {
        // push_back vs emplace_back ???
        result_v.emplace_back(std::string(1, st.top()));
        st.pop();
    }

    return result_v;
}

// infix_form-ban építem fel a megoldást.
// is_open -> infix kifejezésben a zárójel kezelésére használom
// inorder fabejárással készítem el az infix alakot.
std::string BinExpTree::printExpression() const
{
    std::string infix_form;
    bool is_open = false;

    // Ha root == nullptr, akkor üres a fa, nincs mit bejárni.
    // throw exception.
    if (!root)
    {
        throw tree_is_empty_error();
    }

    _inorder(root, infix_form, is_open);

    return infix_form;
}

// Postorder fabejárással értékelem ki.
double BinExpTree::evaluateExpression() const
{
    // Ha üres a fa, akkor nincs mit kiértékelni.
    // throw exception.
    /// Azért van kikommentelve, mert a _postorder metódusban ellenőrzöm és 0-val térek vissza, ha a fa üres.
    /*if (!root)
    {
        throw tree_is_empty_error();
    }*/

    return _postorder(root);
}

// Ezzel vizsgálom meg hogy a művelet magasrendű-e.
bool BinExpTree::priority(char operator_)
{
    if (operator_ == '/' || operator_ == '*')
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
// Stringstreammel írom át a stringet charra és majd eltárolom d-ben, aztán azt vizsgálom.
// Mielőtt balra megyek megvizsgálom az operátorok közti relációt, mert ez alapján fogom elhelyezni a nyitó zárójelet.
// Ha a szülő operátor erősebb, akkor elhelyezek egy nyitó zárójelet és true-ra állítom is_open-t, ezzel jelzem hogy majd be kéne zárni.
// Így előre segítve a helyes és nem felesleges zárójelezést.
// inorder: Left, Parent, Right bejárás.
void BinExpTree::_inorder(const Node* i, std::string &s, bool &is_open)
{
    std::stringstream ss;
    char d;

    if (i->left)
    {
        if (i->left->value[0] == '+' || i->left->value[0] == '-' || i->left->value[0] == '*' || i->left->value[0] == '/')
        {
            if (priority(i->value[0]) > priority(i->left->value[0]))
            {
                s += "(";
                is_open = true;
            }
        }
        _inorder(i->left, s, is_open);
    }

    ss << i->value;
    ss >> d;

    if (isdigit(d))
    {
        s += i->value;
    }
    else // ha operátor
    {
        if (priority(i->left->value[0]) > 0) // Ha "*" vagy "/"
        {
            if (priority(i->value[0]) > priority(i->left->value[0])) // Szülő operátor nagyobb-e, mint a bal gyerek.
            {
                // Elhelyezem a magasrendű operátort aztán nyitó zárójel és utána következő elemek.
                // is_open-nel jelzem, hogy majd be kéne csukni a zárójelet.
                s += i->value;
                s += "(";
                is_open = true;
            }
            else
            {
                s += i->value;
            }
        }
        else if (priority(i->right->value[0]) > 0) // Ugyanaz a szülő gyerek operátor vizsgálat, mint a balnál csak itt a jobbal.
        {
            if (priority(i->value[0]) > priority(i->right->value[0]))
            {
                s += i->value;
                s += "(";
                is_open = true;
            }
            else
            {
                s += i->value;
            }
        }
        else // Ha nem "*" vagy "/" operátor akkor csak fűzzük az s stringhez.
        {
            s += i->value;
        }
    }

    // Mint a balnál itt is mielőtt lelépek jobbra megvizsgálom a szülő gyerek operátor közti kapcsolatot.
    // A helyes zárójelezés miatt kell.
    // Ha szülő erősebb akkor nyitó zárójel és is_open = true, jelezve hogy majd be kell zárni.
    // Különben csak lépjen le jobbra.
    if (i->right)
    {
        if (i->right->value[0] == '+' || i->right->value[0] == '-' || i->right->value[0] == '*' || i->right->value[0] == '/')
        {
            if (priority(i->value[0]) > priority(i->right->value[0]))
            {
                s += "(";
                is_open = true;
            }
        }

        _inorder(i->right, s, is_open);
    }

    // A szülő operátor node-nál csukom be a zárójelet, így lesz helyes.
    // Mert, ha gyereknél teszem, akkor hamar fog csukódni.
    // Végén is_open = false, hogy majd a következő zárójel nyitást tudja jelezni, hogy majd be kell zárni.
    if (is_open && (i->left != nullptr || i->right != nullptr))
    {
        s += ")";
        is_open = false;
    }
}

// Itt értékelem ki a kifejezést.
// Postorder fabejárás: Left, Right, Parent.
// Ha i == nullptr akkor nincs mit kiértékelni és 0-val térek vissza.
// Lemegyek az aljára és felfelé haladva értékelem ki a fát.
double BinExpTree::_postorder(const Node *i)
{
    std::stringstream ss;

    if (!i)
    {
        return 0;
    }

    // Ha a node-nak nincs gyereke, akkor visszatérek az értékével.
    // Ez akkor fordulhat elő, ha csak 1 operandus node van a fában vagy
    // legalsó levél node-doknál vagyok, amiknek nincsen gyerekük és operandus értéket tárolnak.
    if (!i->left && !i->right)
    {
        double d;
        ss << i->value;
        ss >> d;
        ss.clear();

        return d;
    }

    // Alulról felfelé haladva, lementem a bal és jobb operandusokat.
    double left_numb = _postorder(i->left);
    double right_numb = _postorder(i->right);

    // Az operandusokat lementve, megnézem hogy a szülő milyen operátor és elvégzem a műveletet.
    // A szülők operátor node-dok.
    if (i->value == "+")
    {
        return left_numb + right_numb;
    }
    else if (i->value == "-")
    {
        return left_numb - right_numb;
    }
    else if (i->value == "*")
    {
        return left_numb * right_numb;
    }

    // Ha egyikse akkor csak osztás lehet.
    return left_numb / right_numb;
}

// Végig járom a fát és alulról felfelé haladva minden allokált node-dot felszabadítok.
// Destruktor segédfv-e.
void BinExpTree::_destroy(Node *x)
{
    if (x != nullptr)
    {
        _destroy(x->left);
        _destroy(x->right);

        delete x;
    }
}

#endif //KHF03_BINEXPTREE_HPP
