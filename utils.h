#ifndef UTILS_H
#define UTILS_H
#include <vector>

struct Item {
    int peso;
    int valor;
};

inline double calcularFitness(const std::vector<int>& cromossomo, const std::vector<Item>& itens, int capacidade) {
    int pesoTotal = 0;
    int valorTotal = 0;
    for (size_t i = 0; i < itens.size(); ++i) {
        if (cromossomo[i] == 1) {
            pesoTotal += itens[i].peso;
            valorTotal += itens[i].valor;
        }
    }
    if (pesoTotal <= capacidade) return (double)valorTotal;
    return (double)valorTotal - (pesoTotal * 100); 
}

#endif