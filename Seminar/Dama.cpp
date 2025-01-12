#include <iostream>
#include <vector>
#include <memory>
#include <stdexcept>
#include <string>
using namespace std;
//za vrijeme izvodenja programa
class IgraException : public runtime_error {
public:
    explicit IgraException(const string& poruka) : runtime_error(poruka) {}
};
//iznimke za razlicita pravila
class NevazeciPotezException : public IgraException {
public:
    explicit NevazeciPotezException(const string& poruka) : IgraException(poruka) {}
};
class KrivaBojaFigureException : public IgraException {
public:
    explicit KrivaBojaFigureException(const string& poruka) : IgraException(poruka) {}
};
class IzvanGranicaIgreException : public IgraException {
public:
    explicit IzvanGranicaIgreException(const string& poruka) : IgraException(poruka) {}
};
class Figura {
protected:
    bool jePromovirana;
    int red;
    int stupac;
public:
    Figura(int r, int s) : jePromovirana(false), red(r), stupac(s) {
        if (r < 0 || r >= 8 || s < 0 || s >= 8)
        {
            throw IzvanGranicaIgreException("Figura je izvan ploce!");
        }
    }
    virtual ~Figura() = default;
    virtual char getZnak() const = 0;
    virtual bool jeCrna() const = 0;
    bool jeZadnjiRed() const
    {
        return (jeCrna() && red == 7) || (!jeCrna() && red == 0);
    }
    void promoviraj()
    {
        jePromovirana = true;
    }
    bool getIsPromovirana() const
    {
        return jePromovirana;
    }
    void potez(int noviRed, int noviStupac)
    {
        if (noviRed < 0 || noviRed >= 8 || noviStupac < 0 || noviStupac >= 8)
        {
            throw IzvanGranicaIgreException("Pokusavas figuru pomaknit izvan ploce!");
        }
        red = noviRed;
        stupac = noviStupac;
    }
    bool jeIspavanSmjer(int naRed) const {
        if (!jePromovirana && ((jeCrna() && naRed <= red) || (!jeCrna() && naRed >= red)))
        {
            throw NevazeciPotezException("Figure moraju ici prema naprijed!");
        }
        return true;
    }
    //trenutna pozicija
    pair<int, int> getPozicija() const
    {
        return { red, stupac };
    }
};
class CrnaFigura : public Figura {
public:
    CrnaFigura(int r, int s) : Figura(r, s) {}
    char getZnak() const override
    {
        return jePromovirana ? 'C' : 'c';
    }
    bool jeCrna() const override
    {
        return true;
    }
};
class BijelaFigura : public Figura {
public:
    BijelaFigura(int r, int c) : Figura(r, c) {}
    char getZnak() const override
    {
        return jePromovirana ? 'B' : 'b';
    }
    bool jeCrna() const override
    {
        return false;
    }
};
class Ploca {
private:
    static const int SIZE = 8;
    vector<vector<shared_ptr<Figura>>> polje;
    bool jeIgratiCrnog;
    void inicijalizirajPlocu()
    {
        polje = vector<vector<shared_ptr<Figura>>>(SIZE, vector<shared_ptr<Figura>>(SIZE));
        try
        {
            //Postavljanje crnih figura
            for (int red = 0; red < 3; red++)
            {
                for (int stupac = 0; stupac < SIZE; stupac++)
                {
                    if ((red + stupac) % 2 == 1)
                    {
                        polje[red][stupac] = make_shared<CrnaFigura>(red, stupac);
                    }
                }
            }
            //Postavljanje bijelih figura
            for (int red = 5; red < SIZE; red++)
            {
                for (int stupac = 0; stupac < SIZE; stupac++)
                {
                    if ((red + stupac) % 2 == 1)
                    {
                        polje[red][stupac] = make_shared<BijelaFigura>(red, stupac);
                    }
                }
            }
        }
        catch (const IgraException& e)
        {
            throw IgraException("Ploca se nije uspjesno inicijalizirala: " + string(e.what()));
        }
    }
    void provjeraPoteza(int saReda, int saStupca, int naRed, int naStupac) const
    {
        //provjera granica ploce
        if (saReda < 0 || saReda >= SIZE || saStupca < 0 || saStupca >= SIZE ||
            naRed < 0 || naRed >= SIZE || naStupac < 0 || naStupac >= SIZE)
        {
            throw IzvanGranicaIgreException("Pokusavas figuru pomaknit izvan ploce!!");
        }
        //provjera postojanja figure
        auto figurica = polje[saReda][saStupca];
        if (!figurica)
        {
            throw NevazeciPotezException("Nema figure na toj poziciji!");
        }
        //provjera boje figure
        if (figurica->jeCrna() != jeIgratiCrnog)
        {
            throw KrivaBojaFigureException("Nemozes micati protivnicku figuru!");
        }
        //provjera jeli polje prazno
        if (polje[naRed][naStupac])
        {
            throw NevazeciPotezException("Na polju se vec nalazi figura!");
        }
        //provjera smjera kretanja
        if (abs(naRed - saReda) != abs(naStupac - saStupca))
        {
            throw NevazeciPotezException("Figura se mora kretati dijagonalno!");
        }
        //provjera smjera pri uzimanju
        figurica->jeIspavanSmjer(naRed);
        int razlikaRedaka = abs(naRed - saReda);
        if (razlikaRedaka > 2)
        {
            throw NevazeciPotezException("Figura se nemoze kretati vise od 2 polja!");
        }
        //provjera uzimanja
        if (razlikaRedaka == 2)
        {
            int srednjiRed = (saReda + naRed) / 2;
            int srednjiStupac = (saStupca + naStupac) / 2;
            auto srednjaFigura = polje[srednjiRed][srednjiStupac];
            if (!srednjaFigura)
            {
                throw NevazeciPotezException("Nema figure za uzimanje!");
            }
            if (srednjaFigura->jeCrna() == jeIgratiCrnog)
            {
                throw NevazeciPotezException("Nemozes uzeti svoju figuru!");
            }
        }
    }
public:
    Ploca() : jeIgratiCrnog(true) {
        inicijalizirajPlocu();
    }
    //bool getJeIgratiCrnog() const { return jeIgratiCrnog; }
    void ispisiPlocu() const
    {
        cout << "  1 2 3 4 5 6 7 8" << endl;
        for (int i = 0; i < SIZE; i++)
        {
            cout << i + 1 << " ";
            for (int j = 0; j < SIZE; j++)
            {
                if (polje[i][j])
                {
                    cout << polje[i][j]->getZnak() << " ";
                }
                else
                {
                    cout << ". ";
                }
            }
            cout << endl;
        }
        cout << endl << "Red je igrati: " << (jeIgratiCrnog ? "CRNI" : "BIJELI") << endl;
    }
    void napraviPotez(int saReda, int saStupac, int naRed, int naStupac)
    {
        saReda--; saStupac--; naRed--; naStupac--;
        provjeraPoteza(saReda, saStupac, naRed, naStupac);
        //makni figuru
        auto figurica = polje[saReda][saStupac];
        polje[naRed][naStupac] = figurica;
        polje[saReda][saStupac].reset();
        figurica->potez(naRed, naStupac);
        //uzimanje figure
        if (abs(naRed - saReda) == 2)
        {
            int srednjiRed = (saReda + naRed) / 2;
            int srednjiStupac = (saStupac + naStupac) / 2;
            polje[srednjiRed][srednjiStupac].reset();
        }
        //provjera promocije
        if (figurica->jeZadnjiRed())
        {
            figurica->promoviraj();
        }
        jeIgratiCrnog = !jeIgratiCrnog;
    }
    bool krajIgre() const
    {
        bool imaCrnihFigura = false;
        bool imaBijelihFigura = false;
        for (int i = 0; i < SIZE; i++)
        {
            for (int j = 0; j < SIZE; j++)
            {
                if (polje[i][j])
                {
                    if (polje[i][j]->jeCrna())imaCrnihFigura = true;
                    else imaBijelihFigura = true;
                }
            }
        }
        if (!imaCrnihFigura)
        {
            cout << endl << "Pobjednik: BIJELI!" << endl;
            return true;
        }
        if (!imaBijelihFigura)
        {
            cout << endl << "Pobjednik: CRNI!" << endl;
            return true;
        }
        return false;
    }
};
int main()
{
    try {
        Ploca ploca;
        cout << "DAMA(Checkers)!" << endl;
        cout << "- Crne figure su 'c' (Promovirane: 'C')" << endl;
        cout << "- Bijele figure su 'b' (Promovirane: 'B')" << endl;
        cout << "- Prazna polja su '.'" << endl;
        cout << "- Unosite koordinate 1-8" << endl << endl;
        ploca.ispisiPlocu();
        while (!ploca.krajIgre())
        {
            int predaja;
            cout << endl << "Za predaju partije upisite -1!" << endl;
            cin >> predaja;
            if (predaja == -1)
            {
                cout << endl << "Igra je zavrsena! " /* << (ploca.getJeIgratiCrnog ? "BIJELI" : "CRNI") */<< " predaje match." << endl;
                return 0;
            }
            cin.clear();
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            int saReda, saStupca, naRed, naStupac;
            cout << endl << "Unesi potez (saReda saStupca naRed naStupac): ";
            if (!(cin >> saReda >> saStupca >> naRed >> naStupac))
            {
                cout << "Krivi upis. Upisi brojeve od 1 do 8." << endl;
                cin.clear();
                cin.ignore(numeric_limits<streamsize>::max(), '\n');
                continue;
            }
            try
            {
                ploca.napraviPotez(saReda, saStupca, naRed, naStupac);
                ploca.ispisiPlocu();
            }
            catch (const NevazeciPotezException& e)
            {
                cout << "Nevazec potez: " << e.what() << endl;
            }
            catch (const IzvanGranicaIgreException& e)
            {
                cout << "Izvan ploce: " << e.what() << endl;
            }
            catch (const KrivaBojaFigureException& e)
            {
                cout << "Kriva boja figure: " << e.what() << endl;
            }
            catch (const IgraException& e)
            {
                cout << "Error: " << e.what() << endl;
            }
        }
        cout << endl << "KRAJ IGRE!" << endl;
    }
    catch (const exception& e)
    {
        cerr << "cError: " << e.what() << endl;
        return 1;
    }
    return 0;
}