#ifndef UTILS_H
#define UTILS_H
#include <vector>

struct Item {
    int peso;
    int valor;
};

// Modelagem da Função Objetivo com Penalidade Suave (Melhor para o SA)
// Nova função de Avaliação com Penalidade Dinâmica (Densidade Máxima)
double calcularFitness(const std::vector<int>& solucao, const std::vector<Item>& itens, int capacidade, double rho_max) {
    int pesoTotal = 0;
    int valorTotal = 0;
    
    for (size_t i = 0; i < itens.size(); ++i) {
        if (solucao[i] == 1) {
            pesoTotal += itens[i].peso;
            valorTotal += itens[i].valor;
        }
    }

    int excesso = pesoTotal - capacidade;
    
    if (excesso <= 0) {
        // Mochila válida: retorna apenas o valor
        return (double)valorTotal;
    } else {
        // Mochila inválida: pune o que passou usando a densidade máxima
        return (double)valorTotal - (excesso * rho_max); 
    }
}

#endif