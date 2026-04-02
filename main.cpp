#include <random>
#include <iostream>
#include <chrono>
#include <iomanip>
using namespace std;
mt19937 rng(chrono::steady_clock::now().time_since_epoch().count());
using uid = uniform_int_distribution<>;
using urd = uniform_real_distribution<double>;

constexpr double PI = 3.1415926535897932384626443383;

constexpr double low = 1, high = 100.0;
urd range(low, high);
uid param(0, 1);
urd accept(0, 1);

struct Solution{
    double s;
    double hk;
};

struct SimulatedAnnealing{
    SimulatedAnnealing(double v, double p1, double p2): v(v), p1(p1), p2(p2){ }
    double v, p1, p2;
    
    double square(const double &x) const {
        return x * x;
    }
    double cube(const double &x) const {
        return x * x * x;
    }

    double f(const double &s, const double &hk) const {
        return 4 * (v - cube(s)) / (PI * square(s)) - 1.0 / 3.0 * hk;
    }

    double cost(const Solution &ss) const {
        const auto &[s, hk] = ss;
        const auto hc = f(s, hk);
        if (hc < 0){
            return 1000000;
        }
        double c1 = p1 * (
                          6 * square(s) - PI * square(s) / 4 + PI * s * hc
                          );
        double c2 = p2 * (
                          PI * s / 2 * sqrt(square(s) / 4 + square(hk))
                          );
        return c1 + c2;
    }

    Solution neighbor(const Solution &prev) const {
        Solution res = prev;
        if (param(rng) == 0){
            res.s = range(rng);
        }
        else{
            res.hk = range(rng);
        }
        return res;
    }
    
    double acceptance(const double &e, const double &e_new, const double &t) const {
        return exp((e - e_new) / t);
    }
    
    Solution simulated_annealing(int seconds, double t, double alpha){
        Solution s = Solution{};
        s.s = range(rng);
        s.hk = range(rng);
        long long iterations = 0;
        double e = cost(s);
        auto start = chrono::high_resolution_clock::now();
        auto get_now = [&](){
            return chrono::high_resolution_clock::now();
        };
        auto get_time = [&](){
            return chrono::duration_cast<chrono::seconds>(get_now() - start).count();
        };
        int sec = 0;
        while (get_time() <= seconds){
            if (get_time() != sec){
                sec = get_time();
                // second, iteration, cost, temperature, s, hk, hc
                cout << setw(3) << sec << ", " << setw(10) << iterations << ", ";
                cout << "$" << setw(7) << e << ", ";
                cout << setw(5) << t << ", ";
                cout << setw(5) << s.s << ", " << setw(5) << s.hk << ", " << setw(5) << f(s.s, s.hk) << '\n';
            }
            iterations++;
            Solution s_new = neighbor(s);
            double e_new = cost(s_new);
            if (accept(rng) <= acceptance(e, e_new, t)){
                s = s_new;
                e = e_new;
            }
            t = t * alpha;
        }
        cout << iterations << " iterations\n";
        return s;
    }
};

int main(){
    double v, p1, p2;
    cout << "enter volume: ";
    cin >> v;
    cout << "enter p1: ";
    cin >> p1;
    cout << "enter p2: ";
    cin >> p2;
    SimulatedAnnealing annealing(v, p1, p2);
    while (true){
        cout << "Starting simulated annealing. Please enter parameters.\n";
        int seconds;
        double t, alpha;
        cout << "Enter seconds: ";
        cin >> seconds;
        cout << "Enter initial temperature: ";
        cin >> t;
        cout << "Enter alhpa: ";
        cin >> alpha;
        auto res = annealing.simulated_annealing(seconds, t, alpha);
        cout << "s: " << res.s << " hk: " << res.hk << " hc: " << annealing.f(res.s, res.hk) << '\n';
        cout << "Cost: " << annealing.cost(res) << '\n';
    }
}


/*
 seconds = 300
 t = 100000
 alpha = 0.99999999
 */
