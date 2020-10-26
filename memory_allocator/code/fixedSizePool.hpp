#pragma once
#include <exception>
#include <queue>

template<class T, unsigned N> //N a lefoglalando tomb merete, maximum meret
class FixedSizePool
{
protected:

    /// arr -> ezzel foglalom le a N méretű memóriaterületet
    /// act -> aktuális memóriaterület amit átadtam & ezzel térek vissza
    T *arr, *act;

    /// next_free -> mutatja hol a kövi szabad hely
    /// last_gave -> utoljára átadott szabad memóriaterület a usersnek, ami még sosem volt átadva. Ha empty a queue akkor,
    /// újra alaphelyzetbe állítom, mintha a tömböm területei sosem lettek volna használva.
    int next_free, last_gave;

    /// is_full -> mindegyik területet átadtuk e ?
    /// is_empty -> üres e a tömb ?
    bool is_full, is_empty;

    /// felszabadított hely indexét queue-ba rakom, hogy tudjuk hol van még szabad hely
    std::queue<int> q;

public: //A return nullptr es a (void)variable csak a warning-ok kiiktatasa miatt vannak ott,
    //torold oket, amikor elkezded az adott fuggvenyt implementalni

    /// Konstruktor -> A változókat alaphelyzetbe állítom. Kivéve a queue.
    /// last_given = -1 lesz. Mert a legelején nem tárol semmi féle hasznos indexet.
    FixedSizePool() :
            arr(new T[N]), act(nullptr), next_free(0), last_gave(-1), is_full(false), is_empty(true) {}

    /// Destruktor -> A dinamikusan lefoglalt memória területeket felszabadítom.
    ~FixedSizePool()
    {
        delete[] arr;
    }

    //________Saját Tagfv______________

    /// Dynamic Pool feladat miatt hoztam létre.
    bool get_isFull()
    {
        return is_full;
    }

    //_________________________________

    /// Először átadom az összes olyan területet, ami még sosem volt felhasználva. Aztán megkérdezem a queue-tól,
    /// hogy van e felszabadított hely valahol és megkapom tőle az idexet, hogy hol van szabad és azt adom oda a usersnek.
    ///
    T* allocate()
    {
        /// Mivel egy helyet lefoglalok a usernek, ezért már nem lesz üres vagyis nem teljesen szabad a tömböm.
        is_empty = false;

        if(!is_full) /// Ha nincs tele, akkor adok egy blokk memóterületet a usernek
        {
            if(last_gave < (int)N-1)  /// Ha az utoljára átadott terület indexe kisebb mint a tömb mérete minusz egy.
            {
                last_gave = next_free;
                act = &arr[last_gave];  /// act -> sor memóra mutat, amit átadok
                ++next_free; /// kövi szabad
            }
            else if(!q.empty())  /// Ha fel van szabadítva hely, akkor mutassunk oda és adjuk is át
            {
                next_free = q.front();
                act = &arr[next_free];
                q.pop();
            }

            if(last_gave >= (int)N-1 && q.empty()) /// Ha a last_gave az utolsó tömb területre mutat & üres a queue akkor minden foglalt.
            {
                is_full = true;
            }
        }
        else /// Ha nincs több szabad hely akkor throw bad_alloc()
        {
            throw std::bad_alloc();
        }

        return act;
    }

    ///  Megvizsgálom, hogy van e elég helyem hogy lefoglaljak n sor területet a usersnek.
    /// next_free-vel lépek n-et és megnézem hogy kiesek e a tömből. Ha nem akkor van elég hely előtte, ergo át tudok adni
    /// a usersnek elég területet. Az allocate(n) tagfv-nek nem kötelező az elegendő felszabadított helyeket átadni.
    /// Csak azokat amik még sosem voltak lefoglalva.

    T* allocate(unsigned n)
    {
        is_empty = false;  /// Ha allokálok, akkor nem lesz teljesen szabad a tömböm.

        /// Ha next_free nincs túl indexelve és ha last_gave nem az utolsó elemre mutat, akkor van elég szabad.
        if(next_free+(int)n <= (int)N && last_gave < (int)N-1)
        {
            act = &arr[next_free];
            last_gave += (int)n;
            next_free += (int)n;   /// Lépek a kövi szabad helyre
        }
        else
        {
            throw std::bad_alloc();
        }

        /// Ha next_free túl van indexelve & last_gave az utolsó elemre mutat & üres a queue, akkor tele van
        if(next_free == (int)N && last_gave == (int)N-1 && q.empty())
        {
            is_full = true;
        }

        return act;
    }

    /// For ciklussal megkeresem, hogy hová mutat a p. Elmentem az indexet a pos_idx-be.
    /// A pos_idx-et beleteszem a queue-ba.
    /// Feltételezzük, hogy nem fogyatékos a user és tudja hogy mekkora a tömb és tudja
    /// hol vannak a szabad helyek és hol van ami nem (Feladatleírás, persze ott szerényebben van fogalmazva).
    void deallocate(T* p)
    {
        /// Ha deallokálok akkor már nem lesz tele, hanem lesz üres helyünk amit átadhatunk.
        is_full = false;

        int pos_idx = -1;

        for(int i = 0; i < (int)N; i++)
        {
            if(&arr[i] == p)
            {
                pos_idx = i;
                i = (int)N;
            }
        }

        /// Megnézem, hogy ha beletenném a kövi indexet a queue-ba akkor N méretű lesz e ?
        /// Ha igen akkor tudjuk, hogy a tömbünk teljesen szabad akkor feleslegesen tároljuk
        /// az indexeket a queue-ban. Inkább kiürítem. Aztán a next_free & last_gave változókat
        /// alaphelyzetbe állítom, mintha a tömb helyei még sosem lettek volna felhasználva és
        /// kezdődik az egész előről.
        if(q.size()+1 == (int)N)
        {
            for(size_t i = 0; i < q.size(); i++)
            {
                q.pop();
            }

            is_empty = true;
            next_free = 0;
            last_gave = -1;
        }
        else
        {
            q.push(pos_idx);
        }
    }

    /// Az itteni logika hasonló, mint a deallocate() & alocate(n) tagfv-nél.
    /// Ha deallokálom akkor, nem lesz full a tömböm. For ciklussal megnézem hová mutat.
    /// Az indexet lementem a pos_idx-be. Van egy limit változom, ami megmondja hogy meddig szabadíthatok fel.
    /// Ne hogy sokat szabadítsak. Ugyanúgy megnézem hogy ha queue N méretű lesz akkor kiúrítem és minden reset, ahogy
    /// a deallocate(p)-nél.
    void deallocate(T* p, unsigned n)
    {
        is_full = false;

        int pos_idx = -1;
        int limit = -1;

        /// Megkapom a p által mutatott hely indexét, ami a felszabadítás kiindulási pontja lesz.
        /// Itt adom meg a limitet is.
        for(int i = 0; i < (int)N; i++)
        {
            if(&arr[i] == p)
            {
                pos_idx = i;
                limit = pos_idx+(int)n-1;
                i = (int)N;
            }
        }

        if(limit < (int)N)
        {
            if(q.size()+(int)n == (int)N)
            {
                for(size_t i = 0; i < q.size(); i++)
                {
                    q.pop();
                }

                is_empty = true;
                next_free = 0;
                last_gave = -1;
            }
            else
            {
                /// limit segítségével felszabadítok n helyet és beleteszem a queue-ba az indexüket.
                while(pos_idx <= limit)
                {
                    q.push(pos_idx);
                    ++pos_idx;
                }
            }
        }
    }

    /// Az allocate() fv-től megkérdezem van e szabad hely. Ha igen akkor elkérem tőle és beleteszem a
    /// value-t és odaadom a usersnek.
    T* construct(const T value)
    {
        act = allocate();
        *act = value;

        return act;
    }
};
