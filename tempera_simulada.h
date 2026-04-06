#ifndef TEMPERA_SIMULADA_H
#define TEMPERA_SIMULADA_H 

#include <iostream>
#include <vector>
#include <cmath>
#include <random>
#include <algorithm>
#include <fstream>
#include <iomanip>

#include "utils.h"

using namespace std;

void simulatedAnnealing(const vector<Item>& itens, int capacidade, vector<int>& melhorGlobal) {
    int n = itens.size();
    vector<int> atual(n, 0); 
    
    // Inicialização do Gerador (C++11)
    random_device rd;
    mt19937 generator(rd());
    uniform_real_distribution<double> distProb(0.0, 1.0);
    uniform_int_distribution<int> distItem(0, n - 1);

    double T = 1000.0;
    double alfa = 0.995;
    melhorGlobal = atual;

    while (T > 0.001) {
        // 1. Geração do Vizinho (Operador de Mutação/Bit-flip)
        vector<int> proximo = atual;
        int idx = distItem(generator);
        proximo[idx] = 1 - proximo[idx];

        // 2. Cálculo da variação de energia (Delta)
        double vAtual = calcularFitness(atual, itens, capacidade);
        double vProximo = calcularFitness(proximo, itens, capacidade);
        double delta = vProximo - vAtual;

        // 3. Critério de Metropolis
        if (delta > 0) {
            atual = proximo;
            // Mantém rastreio da melhor solução já vista (Best-so-far)
            if (calcularFitness(atual, itens, capacidade) > calcularFitness(melhorGlobal, itens, capacidade)) {
                melhorGlobal = atual;
            }
        } else {
            double p = exp(delta / T);
            if (distProb(generator) < p) {
                atual = proximo;
            }
        }

        T *= alfa; // Resfriamento Geométrico
    }
}

#endif