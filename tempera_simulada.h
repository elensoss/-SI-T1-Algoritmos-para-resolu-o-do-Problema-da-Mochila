#ifndef TEMPERA_SIMULADA_H
#define TEMPERA_SIMULADA_H 

#include <iostream>
#include <cmath>
#include <random>
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <chrono>

#include "utils.h"

using namespace std;


void simulatedAnnealing(const vector<Item>& itens, int capacidade, vector<int>& melhorGlobal, double rho_max) {
    int n = itens.size();
    
    
    // Inicialização do Gerador (C++11)
    random_device rd;
    mt19937 generator(rd());
    uniform_real_distribution<double> distProb(0.0, 1.0);
    uniform_int_distribution<int> distItem(0, n - 1);
    uniform_int_distribution<int> distBin(0, 1);

    vector<int> atual(n); 
    for (int i = 0; i < n; ++i) {
        atual[i] = distBin(generator);
    }

    double T = 1000.0;
    double alfa = 0.995;
    melhorGlobal = atual;

    while (T > 0.0001) {
        // 1. Geração do Vizinho (Operador de Mutação/Bit-flip)
        vector<int> proximo = atual;
        int idx = distItem(generator);
        proximo[idx] = 1 - proximo[idx];

        // 2. Cálculo da variação de energia (Delta)
        double vAtual = calcularFitness(atual, itens, capacidade, rho_max);
        double vProximo = calcularFitness(proximo, itens, capacidade, rho_max);
        double delta = vProximo - vAtual;

        // 3. Critério de Metropolis
        if (delta > 0) {
            atual = proximo;
            // Mantém rastreio da melhor solução já vista (Best-so-far)
            if (calcularFitness(atual, itens, capacidade, rho_max) > calcularFitness(melhorGlobal, itens, capacidade, rho_max)) {
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