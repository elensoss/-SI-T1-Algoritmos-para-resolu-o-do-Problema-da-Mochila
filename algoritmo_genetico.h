#ifndef ALGORITMO_GENETICO_H
#define ALGORITMO_GENETICO_H

#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <fstream>
#include <iomanip>

#include "utils.h"

using namespace std;

// --- OPERADORES DO AG ---

// Seleção por Torneio: Pega 3 aleatórios e retorna o melhor deles
int selecaoTorneio(const vector<double>& fitnesses, mt19937& gen) {
    uniform_int_distribution<int> dist(0, fitnesses.size() - 1);
    int i1 = dist(gen);
    int i2 = dist(gen);
    int i3 = dist(gen);
    
    double f1 = fitnesses[i1];
    double f2 = fitnesses[i2];
    double f3 = fitnesses[i3];

    if (f1 >= f2 && f1 >= f3) return i1;
    if (f2 >= f1 && f2 >= f3) return i2;
    return i3;
}

// Crossover de Um Ponto (Troca de DNA)
void crossover(const vector<int>& pai, const vector<int>& mae, vector<int>& filho1, vector<int>& filho2, mt19937& gen) {
    uniform_int_distribution<int> dist(1, pai.size() - 2);
    int ponto = dist(gen);

    for (int i = 0; i < pai.size(); ++i) {
        if (i < ponto) {
            filho1[i] = pai[i];
            filho2[i] = mae[i];
        } else {
            filho1[i] = mae[i];
            filho2[i] = pai[i];
        }
    }
}

void mutacao(vector<int>& cromossomo, double taxaMutacao, mt19937& gen) {
    uniform_real_distribution<double> dist(0.0, 1.0);
    for (int& gene : cromossomo) {
        if (dist(gen) < taxaMutacao) {
            gene = 1 - gene; // Flip bit
        }
    }
}

// --- ALGORITMO PRINCIPAL ---

void algoritmoGenetico(const vector<Item>& itens, int capacidade, vector<int>& melhorGlobal) {
    int tamPopulacao = 100;
    int geracoes = 500;
    double taxaCrossover = 0.8;
    double taxaMutacao = 0.05;
    int n = itens.size();

    random_device rd;
    mt19937 gen(rd());
    
    // 1. Inicialização
    vector<vector<int>> populacao(tamPopulacao, vector<int>(n));
    uniform_int_distribution<int> distBit(0, 1);
    for (int i = 0; i < tamPopulacao; ++i)
        for (int j = 0; j < n; ++j) populacao[i][j] = distBit(gen);

    for (int g = 0; g < geracoes; ++g) {
        vector<double> fitnesses(tamPopulacao);
        for (int i = 0; i < tamPopulacao; ++i) {
            fitnesses[i] = calcularFitness(populacao[i], itens, capacidade);
            if (fitnesses[i] > calcularFitness(melhorGlobal, itens, capacidade)) {
                melhorGlobal = populacao[i];
            }
        }

        vector<vector<int>> novaPopulacao;
        while (novaPopulacao.size() < tamPopulacao) {
            // Seleção
            int p1 = selecaoTorneio(fitnesses, gen);
            int p2 = selecaoTorneio(fitnesses, gen);

            vector<int> f1(n), f2(n);
            // Crossover
            uniform_real_distribution<double> dCross(0.0, 1.0);
            if (dCross(gen) < taxaCrossover) {
                crossover(populacao[p1], populacao[p2], f1, f2, gen);
            } else {
                f1 = populacao[p1];
                f2 = populacao[p2];
            }

            // Mutação
            mutacao(f1, taxaMutacao, gen);
            mutacao(f2, taxaMutacao, gen);

            novaPopulacao.push_back(f1);
            if (novaPopulacao.size() < tamPopulacao) novaPopulacao.push_back(f2);
        }
        populacao = novaPopulacao;
    }
}

#endif