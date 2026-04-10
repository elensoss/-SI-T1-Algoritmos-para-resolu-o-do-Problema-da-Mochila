#include <iostream>
#include <vector>
#include <cmath>
#include <random>
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <chrono>

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
        return (double)valorTotal - (excesso * excesso * rho_max); 
    }
}

void simulatedAnnealing(const std::vector<Item>& itens, int capacidade, std::vector<int>& melhorGlobal, double rho_max) {
    int n = itens.size();
    
    
    // Inicialização do Gerador (C++11)
    std::random_device rd;
    std::mt19937 generator(rd());
    std::uniform_real_distribution<double> distProb(0.0, 1.0);
    std::uniform_int_distribution<int> distItem(0, n - 1);
    std::uniform_int_distribution<int> distBin(0, 1);

    std::vector<int> atual(n); 
    for (int i = 0; i < n; ++i) {
        atual[i] = distBin(generator);
    }

    double T = 1000.0;
    double alfa = 0.995;
    melhorGlobal = atual;

    while (T > 0.0001) {
        // 1. Geração do Vizinho (Operador de Mutação/Bit-flip)
        std::vector<int> proximo = atual;
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
            double p = std::exp(delta / T);
            if (distProb(generator) < p) {
                atual = proximo;
            }
        }

        T *= alfa; // Resfriamento Geométrico
    }
}

int main() {
    std::ifstream file("entrada_50.txt");
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

    // --- NOVO CÁLCULO DE PENALIDADE (10% do Valor Total) ---
    double rho_max = capacidade * 0.1;

    // --- CONFIGURAÇÃO DAS EXECUÇÕES ---
    int numExecucoes = 100; // Padrão acadêmico para ter relevância estatística
    
    int melhorGlobal = -1;
    int piorGlobal = 9999999;
    double somaValores = 0.0;
    
    // Variável para contar quantas vezes o algoritmo falhou (solução inválida)
    int invalidasCount = 0; 
    
    std::cout << "Iniciando bateria de testes (" << numExecucoes << " execucoes)..." << std::endl;

    // Marca o tempo de início
    auto start_time = std::chrono::high_resolution_clock::now();

    for (int exec = 1; exec <= numExecucoes; ++exec) {
        
        // Zera a solução para esta execução
        std::vector<int> melhorSolucao;
        if (n > 0 && n == itens.size()) { 
            melhorSolucao = std::vector<int>(n, 0); 
        }

        simulatedAnnealing(itens, capacidade, melhorSolucao, rho_max);
        // algoritmoGenetico(itens, capacidade, melhorSolucao, rho_max);

        int pesoFinal = 0;
        int valorFinal = 0;
        
        // Calcula o valor real da mochila dessa execução
        for (int i = 0; i < n; ++i) {
            if (melhorSolucao[i] == 1) {
                pesoFinal += itens[i].peso;
                valorFinal += itens[i].valor;
            }
        }

        // Verifica se a penalidade estrita segurou a restrição
        if (pesoFinal > capacidade) {
            std::cout << "[AVISO] Execucao " << exec << " gerou solucao invalida (Peso: " << pesoFinal << ")!" << std::endl;
            valorFinal = 0; // Zera o valor para punir a média
            invalidasCount++; // Incrementa o contador de falhas
        }

        // Atualiza as estatísticas
        somaValores += valorFinal;
        if (valorFinal > melhorGlobal) melhorGlobal = valorFinal;
        
        // Só atualiza o pior global se for uma solução válida (para não poluir a estatística com os '0's)
        if (valorFinal > 0 && valorFinal < piorGlobal) {
            piorGlobal = valorFinal;
        }

        // Mostra o progresso
        std::cout << "Execucao " << exec << " concluida: Valor = " << valorFinal << std::endl;
    }

    // Marca o tempo de fim
    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> tempo_decorrido = end_time - start_time;

    // --- RELATÓRIO ESTATÍSTICO PARA O ARTIGO ---
    double media = somaValores / numExecucoes;
    double percentInvalidas = (static_cast<double>(invalidasCount) / numExecucoes) * 100.0;

    // Ajuste caso todas tenham sido inválidas
    if (piorGlobal == 9999999) piorGlobal = 0; 

    std::cout << "\n================ RESUMO ESTATISTICO ================" << std::endl;
    std::cout << "Instancia: " << n << " itens | Capacidade: " << capacidade << std::endl;
    std::cout << "Total de Execucoes: " << numExecucoes << std::endl;
    std::cout << "Tempo Total Gasto : " << tempo_decorrido.count() << " segundos" << std::endl;
    std::cout << "----------------------------------------------------" << std::endl;
    std::cout << "Solucoes Invalidas      : " << invalidasCount << " (" << std::fixed << std::setprecision(2) << percentInvalidas << "%)" << std::endl;
    std::cout << "----------------------------------------------------" << std::endl;
    std::cout << "Melhor Valor Encontrado : " << melhorGlobal << std::endl;
    std::cout << "Pior Valor Valido       : " << piorGlobal << std::endl;
    std::cout << "Media dos Valores       : " << media << std::endl;
    std::cout << "====================================================" << std::endl;

    return 0;
}