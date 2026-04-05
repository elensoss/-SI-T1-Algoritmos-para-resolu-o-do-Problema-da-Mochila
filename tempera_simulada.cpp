#include <iostream>
#include <vector>
#include <cmath>
#include <random>
#include <algorithm>
#include <fstream>

struct Item {
    int peso;
    int valor;
};

// Modelagem da Função Objetivo com Penalidade Suave (Melhor para o SA)
double calcularFitness(const std::vector<int>& solucao, const std::vector<Item>& itens, int capacidade) {
    int pesoTotal = 0;
    int valorTotal = 0;
    for (size_t i = 0; i < itens.size(); ++i) {
        if (solucao[i] == 1) {
            pesoTotal += itens[i].peso;
            valorTotal += itens[i].valor;
        }
    }

    if (pesoTotal <= capacidade) {
        return (double)valorTotal;
    } else {
        return (double)valorTotal - (pesoTotal * 100); 
    }
}

void simulatedAnnealing(const std::vector<Item>& itens, int capacidade, std::vector<int>& melhorGlobal) {
    int n = itens.size();
    std::vector<int> atual(n, 0); 
    
    // Inicialização do Gerador (C++11)
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_real_distribution<double> distProb(0.0, 1.0);
    std::uniform_int_distribution<int> distItem(0, n - 1);

    double T = 1000.0;
    double alfa = 0.995;
    melhorGlobal = atual;

    while (T > 0.001) {
        // 1. Geração do Vizinho (Operador de Mutação/Bit-flip)
        std::vector<int> proximo = atual;
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
            double p = std::exp(delta / T);
            if (distProb(generator) < p) {
                atual = proximo;
            }
        }

        T *= alfa; // Resfriamento Geométrico
    }
}

int main() {
    std::ifstream file("entrada.txt");
    if (!file.is_open()) {
        std::cerr << "Erro ao abrir o arquivo de entrada!" << std::endl;
        return 1;
    }

    int n, capacidade;
    file >> n >> capacidade;

    std::vector<Item> itens(n);
    for (int i = 0; i < n; ++i) {
        file >> itens[i].peso >> itens[i].valor;
    }
    file.close();

    std::vector<int> melhorSolucao;
    
    // Execução do experimento
    std::cout << "Executando Tempera Simulada..." << std::endl;
    simulatedAnnealing(itens, capacidade, melhorSolucao);

    // Relatório de Resultados (O que vai para o artigo)
    std::cout << "--- RESULTADOS ---" << std::endl;
    int pesoFinal = 0;
    int valorFinal = 0;
    for (int i = 0; i < n; ++i) {
        if (melhorSolucao[i] == 1) {
            std::cout << "Item " << i << " (P: " << itens[i].peso << " V: " << itens[i].valor << ")\n";
            pesoFinal += itens[i].peso;
            valorFinal += itens[i].valor;
        }
    }
    std::cout << "------------------" << std::endl;
    std::cout << "Peso Total: " << pesoFinal << "/" << capacidade << std::endl;
    std::cout << "Valor Total: " << valorFinal << std::endl;

    return 0;
}