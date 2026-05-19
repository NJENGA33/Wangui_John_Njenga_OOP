#include <iostream>
#include <fstream>
#include <vector>
#include <memory>
#include <iomanip>
#include <algorithm>

using namespace std;

enum TimePeriod { DAY = 0, NIGHT = 1 };

// ================= BASE CLASS =================
class Consumer {
protected:
    int id;
    string name;
    float units;
    TimePeriod period;

public:
    Consumer(int i, string n, float u, TimePeriod p)
        : id(i), name(n), units(u), period(p) {}

    virtual float calculateBill() = 0;
    virtual string getType() = 0;

    int getId() { return id; }
    string getName() { return name; }
    float getUnits() { return units; }
    TimePeriod getPeriod() { return period; }

    virtual void display() {
        cout << "\nType: " << getType();
        cout << "\nID: " << id;
        cout << "\nName: " << name;
        cout << "\nUnits: " << units;
        cout << "\nPeriod: " << (period == DAY ? "Day" : "Night");
    }

    virtual ~Consumer() {}
};

// ================= BILLING RULE (TIERED) =================
float tieredRate(float units, float dayRate, float nightRate) {
    float rate = (units <= 100) ? dayRate :
                 (units <= 300) ? dayRate + 2 :
                 dayRate + 5;

    return rate;
}

// ================= RESIDENTIAL =================
class Residential : public Consumer {
public:
    Residential(int i, string n, float u, TimePeriod p)
        : Consumer(i, n, u, p) {}

    float calculateBill() override {
        float rate = (period == DAY) ? 8 : 4;
        rate = tieredRate(units, rate, rate);
        return units * rate;
    }

    string getType() override { return "Residential"; }
};

// ================= COMMERCIAL =================
class Commercial : public Consumer {
public:
    Commercial(int i, string n, float u, TimePeriod p)
        : Consumer(i, n, u, p) {}

    float calculateBill() override {
        float rate = (period == DAY) ? 12 : 9;
        rate = tieredRate(units, rate, rate);
        return units * rate;
    }

    string getType() override { return "Commercial"; }
};

// ================= INDUSTRIAL =================
class Industrial : public Consumer {
public:
    Industrial(int i, string n, float u, TimePeriod p)
        : Consumer(i, n, u, p) {}

    float calculateBill() override {
        float rate = (period == DAY) ? 15 : 11;
        float bill = units * rate + 500; // demand charge
        return bill;
    }

    string getType() override { return "Industrial"; }
};

// ================= ANALYTICS =================
class Analytics {
public:
    static void revenue(vector<unique_ptr<Consumer>>& c) {
        float total = 0;
        for (auto& x : c)
            total += x->calculateBill();

        cout << "\nTotal Revenue: Ksh " << total << endl;
    }

    static void highestBill(vector<unique_ptr<Consumer>>& c) {
        if (c.empty()) return;

        auto* maxC = c[0].get();
        float maxBill = maxC->calculateBill();

        for (auto& x : c) {
            if (x->calculateBill() > maxBill) {
                maxBill = x->calculateBill();
                maxC = x.get();
            }
        }

        cout << "\nHighest Bill Consumer:";
        maxC->display();
        cout << "\nBill: Ksh " << maxBill << endl;
    }

    static void search(vector<unique_ptr<Consumer>>& c, int id) {
        for (auto& x : c) {
            if (x->getId() == id) {
                cout << "\nFOUND CONSUMER:";
                x->display();
                cout << "\nBill: Ksh " << x->calculateBill() << endl;
                return;
            }
        }
        cout << "Consumer not found!\n";
    }
};

// ================= FILE HANDLING =================
void save(vector<unique_ptr<Consumer>>& c) {
    ofstream file("data.txt");

    for (auto& x : c) {
        file << x->getType() << " "
             << x->getId() << " "
             << x->getName() << " "
             << x->getUnits() << " "
             << x->getPeriod() << endl;
    }

    cout << "Saved successfully!\n";
}

vector<unique_ptr<Consumer>> load() {
    vector<unique_ptr<Consumer>> c;
    ifstream file("data.txt");

    string type, name;
    int id, period;
    float units;

    while (file >> type >> id >> name >> units >> period) {
        TimePeriod p = (TimePeriod)period;

        if (type == "Residential")
            c.push_back(make_unique<Residential>(id, name, units, p));
        else if (type == "Commercial")
            c.push_back(make_unique<Commercial>(id, name, units, p));
        else if (type == "Industrial")
            c.push_back(make_unique<Industrial>(id, name, units, p));
    }

    return c;
}

// ================= MAIN =================
int main() {
    vector<unique_ptr<Consumer>> consumers = load();

    int choice;

    do {
        cout << "\n==============================";
        cout << "\n SMART POWER SYSTEM";
        cout << "\n==============================";
        cout << "\n1. Add Consumer";
        cout << "\n2. View Consumers";
        cout << "\n3. Show Bills";
        cout << "\n4. Search Consumer";
        cout << "\n5. Delete Consumer";
        cout << "\n6. Revenue Report";
        cout << "\n7. Highest Bill";
        cout << "\n8. Save Data";
        cout << "\n9. Exit";
        cout << "\nEnter choice: ";
        cin >> choice;

        if (choice == 1) {
            int id, type, period;
            string name;
            float units;

            cout << "ID: "; cin >> id;
            cout << "Name: "; cin >> name;
            cout << "Units: "; cin >> units;

            if (units < 0) {
                cout << "Invalid units!\n";
                continue;
            }

            cout << "Type (1=R, 2=C, 3=I): ";
            cin >> type;

            cout << "Period (0=Day,1=Night): ";
            cin >> period;

            if (type == 1)
                consumers.push_back(make_unique<Residential>(id, name, units, (TimePeriod)period));
            else if (type == 2)
                consumers.push_back(make_unique<Commercial>(id, name, units, (TimePeriod)period));
            else if (type == 3)
                consumers.push_back(make_unique<Industrial>(id, name, units, (TimePeriod)period));

            cout << "Added!\n";
        }

        else if (choice == 2) {
            for (auto& c : consumers) {
                c->display();
                cout << "\n-----------------\n";
            }
        }

        else if (choice == 3) {
            cout << fixed << setprecision(2);
            for (auto& c : consumers) {
                c->display();
                cout << "\nBill: Ksh " << c->calculateBill();
                cout << "\n-----------------\n";
            }
        }

        else if (choice == 4) {
            int id;
            cout << "Enter ID: ";
            cin >> id;
            Analytics::search(consumers, id);
        }

        else if (choice == 5) {
            int id;
            cout << "Enter ID to delete: ";
            cin >> id;

            auto it = remove_if(consumers.begin(), consumers.end(),
                [id](unique_ptr<Consumer>& c) {
                    return c->getId() == id;
                });

            if (it != consumers.end()) {
                consumers.erase(it, consumers.end());
                cout << "Deleted successfully!\n";
            } else {
                cout << "Not found!\n";
            }
        }

        else if (choice == 6) {
            Analytics::revenue(consumers);
        }

        else if (choice == 7) {
            Analytics::highestBill(consumers);
        }

        else if (choice == 8) {
            save(consumers);
        }

    } while (choice != 9);

    return 0;
}
