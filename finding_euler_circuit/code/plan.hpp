#ifndef KHF02_PLAN_HPP
#define KHF02_PLAN_HPP

#include <iostream>
#include <vector>
#include <unordered_map>
#include <list>
#include <algorithm>
#include <stack>

///____Segéd Függvények______



///__________________________

// N -> keresztmetszet
template<typename Iterator>
std::vector<std::int32_t> plan(Iterator corridors_begin, Iterator corridors_end, size_t N)
{
    // Ez tárolja az Euler-kör útvonalát
    std::vector<std::int32_t> sol;

    // Az adott csúcsból hány él megy ki
    std::unordered_map<std::int32_t, std::int32_t> edge_out_numb;

    // Mindegyik csúcsnak van egy szomszéd listája, akikre mutat.
    std::vector<std::vector<std::int32_t>> adjency_list;

    // A külső mátrix méretét be kell állítani.
    // Tudjuk hány csúcs van és így már indexelni is tudni fogunk.
    adjency_list.resize(N);

    // Megadom hogy az adott csúcs melyik csúcsokra mutat.
    // Oda kell figyelni, hogy a vektor 0-tól indexel, de mi a csúcsokat 1-től sorszámozzuk.
    for (Iterator it = corridors_begin; it != corridors_end; ++it)
    {
        adjency_list[it->from - 1].push_back(it->to);

        // Tudom, hogy annyi kimenő éle van a csúcsnak mint ahány bemenő.
        // Ezért már rögtön fel is jegyeztem, hogy akire mutat onnan majd jönni fog vissza egy él.
        adjency_list[it->to - 1].push_back(it->from);
    }

    // Feltöltöm a edge_count mappet
    for (size_t i = 0; i < adjency_list.size(); ++i)
    {
        edge_out_numb[i + 1] = adjency_list[i].size();
    }

    // A stackben tároljuk, hogy eddig milyen csúcsokat érintettünk, amíg el nem jutunk olyanhoz,
    // aminek már nincs több kifelé mutató éle. Az elején pusholunk egy számot, hogy a while ciklus el tudjon indulni,
    // és úgy sem fog a megoldásban benne lenni.
    // Mindig az első csúcstól indulok, de igazából mindegy.
    std::stack<std::int32_t> curr_path;
    curr_path.push(1);
    std::int32_t curr_v = 1;

    // Amíg nem üres a curr_path, addig megy ciklus. curr_path-ban tároljuk az előre
    // haladással érintett csúcsokat. Amikor már nem tudunk tovább enni, akkor a curr_v-t
    // betesszük a sol vektorba és visszalépünk és megnézzük, hogy az előző csúcsból még lehet e
    // előre menni, ha nem akkor stackből a vektorba teszem.
    // Ha curr_path stack üres, akkor végeztem.
    while (!curr_path.empty())
    {
        if (edge_out_numb[curr_v])
        {
            curr_path.push(curr_v);

            // Az szomszédság lista segítségével megadjuk, hogy melyik szomszédra megyünk tovább
            // és frssítjük a curr_v-t.
            std::int32_t next_v = adjency_list[curr_v - 1].back();

            // Közben nem felejtjük el csökkenteni az edge_out_numb mapben az adott csúcsnak a kimenő éleinek a számát,
            // ezáltal tudjuk, hogy melyik csúcsnál tudunk még előre menni.
            edge_out_numb[curr_v]--;

            // Az elhasznált előre lépést is kezelni kell.
            adjency_list[curr_v - 1].pop_back();

            curr_v = next_v;
        }
        else
        {
            sol.push_back(curr_v);
            curr_v = curr_path.top();
            curr_path.pop();
        }
    }

    // Mivel mindegyik csúcsnak ugyanannyi bemenő éle van mint kimenő (vagyis párhuzamos élű csúcsaink vannak) és
    // olyan a gráfunk, hogy mindegy merre indulunk, biztosan találunk Euler-kört, így van megadva a feladat.
    // Egyszerű gráf vagy olyan gráf, aminek nem mindegyik csúcsnak van
    // párhuzamos éle, abban az esetben sol vektor fordítva tartalmazza az útvonalat,
    // ezért meg kéne fordítani a vektort.
    ///std::reverse(sol.begin(), sol.end());

    return sol;
}

struct Corridor
{
    std::int32_t from, to;
};

#endif //KHF02_PLAN_HPP
