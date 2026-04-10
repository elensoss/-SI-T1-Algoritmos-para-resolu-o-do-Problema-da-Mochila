#include "tempera_simulada.h"
#include "algoritmo_genetico.h"

using namespace std;


int main() {
    //Ler o arquivo de entrada
    ifstream file("entrada_100.txt");
    if (!file.is_open()) {
        cerr << "Erro ao abrir o arquivo de entrada!" << endl;
        return 1;
    }

    //Lê a primeira linha do arquivo para obter o número de itens e a capacidade da mochila a serem lidos
    int n, capacidade;
    file >> n >> capacidade;

    //Lê os itens do arquivo e armazena em um vetor de Item
    vector<Item> itens(n);
    for (int i = 0; i < n; i++) {
        file >> itens[i].peso >> itens[i].valor;
    }
    file.close();

    int escolha;
    bool escolhaValida = false;
    while(escolhaValida == false) {
        cout << "Escolha qual algoritmo deseja rodar: 1 - Tempera Simulada | 2 - Algoritmo Genetico" << endl;
        cin >> escolha;
        if (escolha == 1 || escolha == 2) {
            escolhaValida = true;
        } else {
            cerr << "Escolha invalida! Tente novamente..." << endl;
        }
    }

    // --- NOVO CÁLCULO DE PENALIDADE (10% do Valor Total) ---
    double rho_max = capacidade * 0.1;

    // --- CONFIGURAÇÃO DAS EXECUÇÕES ---
    int numExecucoes = 100;
    
    int melhorGlobal = -1;
    int piorGlobal = 9999999;
    double somaValores = 0.0;
    
    // Variável para contar quantas vezes o algoritmo falhou (solução inválida)
    int invalidasCount = 0; 
    
    cout << "Iniciando bateria de testes (" << numExecucoes << " execucoes)..." << endl;

    // Marca o tempo de início
    auto start_time = chrono::high_resolution_clock::now();

    for (int exec = 1; exec <= numExecucoes; ++exec) {
        
        // Zera a solução para esta execução
        vector<int> melhorSolucao;
        if (n > 0 && n == itens.size()) { 
            melhorSolucao = vector<int>(n, 0); 
        }

        if(escolha == 1) {
            simulatedAnnealing(itens, capacidade, melhorSolucao, rho_max);
        } else {
            algoritmoGenetico(itens, capacidade, melhorSolucao, rho_max);
        }

        int pesoFinal = 0;
        int valorFinal = 0;
        
        // Calcula o valor real da mochila dessa execução
        for (int i = 0; i < n; i++) {
            if (melhorSolucao[i] == 1) {
                pesoFinal += itens[i].peso;
                valorFinal += itens[i].valor;
            }
        }

        // Verifica se a penalidade estrita segurou a restrição
        if (pesoFinal > capacidade) {
            cout << "[AVISO] Execucao " << exec << " gerou solucao invalida (Peso: " << pesoFinal << ")!" << endl;
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
        cout << "Execucao " << exec << " concluida: Valor = " << valorFinal << endl;
    }

    // Marca o tempo de fim
    auto end_time = chrono::high_resolution_clock::now();
    chrono::duration<double> tempo_decorrido = end_time - start_time;

    // --- RELATÓRIO ESTATÍSTICO PARA O ARTIGO ---
    double media = somaValores / numExecucoes;
    double percentInvalidas = (static_cast<double>(invalidasCount) / numExecucoes) * 100.0;

    // Ajuste caso todas tenham sido inválidas
    if (piorGlobal == 9999999) piorGlobal = 0; 

    cout << "\n================ RESUMO ESTATISTICO ================" << endl;
    cout << "Instancia: " << n << " itens | Capacidade: " << capacidade << endl;
    cout << "Total de Execucoes: " << numExecucoes << endl;
    cout << "Tempo Total Gasto : " << tempo_decorrido.count() << " segundos" << endl;
    cout << "----------------------------------------------------" << endl;
    cout << "Solucoes Invalidas      : " << invalidasCount << " (" << fixed << setprecision(2) << percentInvalidas << "%)" << endl;
    cout << "----------------------------------------------------" << endl;
    cout << "Melhor Valor Encontrado : " << melhorGlobal << endl;
    cout << "Pior Valor Valido       : " << piorGlobal << endl;
    cout << "Media dos Valores       : " << media << endl;
    cout << "====================================================" << endl;

    return 0;
}