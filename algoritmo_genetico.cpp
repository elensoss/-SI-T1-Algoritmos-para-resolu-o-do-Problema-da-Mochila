#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include <fstream>

struct Item {
    int peso;
    int valor;
};

double calcularFitness(const std::vector<int>& cromossomo, const std::vector<Item>& itens, int capacidade) {
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

// --- OPERADORES DO AG ---

// Seleção por Torneio: Pega 3 aleatórios e retorna o melhor deles
int selecaoTorneio(const std::vector<double>& fitnesses, std::mt19937& gen) {
    std::uniform_int_distribution<int> dist(0, fitnesses.size() - 1);
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
void crossover(const std::vector<int>& pai, const std::vector<int>& mae, std::vector<int>& filho1, std::vector<int>& filho2, std::mt19937& gen) {
    std::uniform_int_distribution<int> dist(1, pai.size() - 2);
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

void mutacao(std::vector<int>& cromossomo, double taxaMutacao, std::mt19937& gen) {
    std::uniform_real_distribution<double> dist(0.0, 1.0);
    for (int& gene : cromossomo) {
        if (dist(gen) < taxaMutacao) {
            gene = 1 - gene; // Flip bit
        }
    }
}

// --- ALGORITMO PRINCIPAL ---

void algoritmoGenetico(const std::vector<Item>& itens, int capacidade, std::vector<int>& melhorGlobal) {
    int tamPopulacao = 100;
    int geracoes = 500;
    double taxaCrossover = 0.8;
    double taxaMutacao = 0.05;
    int n = itens.size();

    std::random_device rd;
    std::mt19937 gen(rd());
    
    // 1. Inicialização
    std::vector<std::vector<int>> populacao(tamPopulacao, std::vector<int>(n));
    std::uniform_int_distribution<int> distBit(0, 1);
    for (int i = 0; i < tamPopulacao; ++i)
        for (int j = 0; j < n; ++j) populacao[i][j] = distBit(gen);

    for (int g = 0; g < geracoes; ++g) {
        std::vector<double> fitnesses(tamPopulacao);
        for (int i = 0; i < tamPopulacao; ++i) {
            fitnesses[i] = calcularFitness(populacao[i], itens, capacidade);
            if (fitnesses[i] > calcularFitness(melhorGlobal, itens, capacidade)) {
                melhorGlobal = populacao[i];
            }
        }

        std::vector<std::vector<int>> novaPopulacao;
        while (novaPopulacao.size() < tamPopulacao) {
            // Seleção
            int p1 = selecaoTorneio(fitnesses, gen);
            int p2 = selecaoTorneio(fitnesses, gen);

            std::vector<int> f1(n), f2(n);
            // Crossover
            std::uniform_real_distribution<double> dCross(0.0, 1.0);
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

    // No AG, inicializamos o melhorGlobal com zeros
    std::vector<int> melhorSolucao(n, 0);
    
    std::cout << "Iniciando Algoritmo Genetico..." << std::endl;
    std::cout << "Populacao: 100 | Geracoes: 500" << std::endl;

    algoritmoGenetico(itens, capacidade, melhorSolucao);

    // --- RELATÓRIO FINAL ---
    std::cout << "\n================ RESULTADOS AG ================" << std::endl;
    int pesoFinal = 0;
    int valorFinal = 0;
    
    for (int i = 0; i < n; ++i) {
        if (melhorSolucao[i] == 1) {
            pesoFinal += itens[i].peso;
            valorFinal += itens[i].valor;
        }
    }

    // Validação de segurança (Caso o AG termine em solução inválida)
    if (pesoFinal > capacidade) {
        std::cout << "[AVISO] O AG nao encontrou solucao viavel no tempo estipulado!" << std::endl;
    }

    std::cout << "Melhor Valor Encontrado: " << valorFinal << std::endl;
    std::cout << "Peso Utilizado: " << pesoFinal << " / " << capacidade << std::endl;
    std::cout << "Itens Selecionados (Indices): ";
    for(int i = 0; i < n; i++) if(melhorSolucao[i]) std::cout << i << " ";
    std::cout << "\n===============================================" << std::endl;

    return 0;
}