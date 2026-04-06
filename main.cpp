#include "tempera_simulada.h"
#include "algoritmo_genetico.h"

using namespace std;


int main() {
    //Ler o arquivo de entrada
    ifstream file("entrada.txt");
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

    // Configuração para os testes
    int numExecucoes = 100; // 100 execuções para um relatório estatístico robusto
    
    int melhorGlobal = -1;
    int piorGlobal = 9999999;
    double somaValores = 0.0;
    
    cout << "Iniciando testes (" << numExecucoes << " execucoes)..." << endl;

    int escolha;
    cout << "Escolha qual algoritmo deseja rodar: 1 - Tempera Simulada | 2 - Algoritmo Genetico" << endl;
    cin >> escolha;
    if (escolha != 1 && escolha != 2) {
        cerr << "Escolha invalida! Encerrando programa." << endl;
        return 1;
    }

    for (int exec = 1; exec <= numExecucoes; exec++) {
        
        // Zera a solução para esta execução
        vector<int> melhorSolucao;
        if (n > 0 && n == itens.size()) { // Pro AG funcionar sem dar erro, inicialize com zeros:
            melhorSolucao = vector<int>(n, 0); 
        }
        
        if (escolha == 1) {
            simulatedAnnealing(itens, capacidade, melhorSolucao);
        } else if (escolha == 2) {
            algoritmoGenetico(itens, capacidade, melhorSolucao);
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
            cout << "[AVISO] Execucao " << exec << " gerou solucao invalida!" << endl;
            // Opcional: zerar o valor se for inválida para punir a média
            valorFinal = 0; 
        }

        // Atualiza as estatísticas
        somaValores += valorFinal;
        if (valorFinal > melhorGlobal) melhorGlobal = valorFinal;
        if (valorFinal < piorGlobal) piorGlobal = valorFinal;

        // Mostra o progresso (opcional, só para você saber que não travou)
        cout << "Execucao " << exec << " concluida: Valor = " << valorFinal << endl;
    }

    // --- RELATÓRIO ESTATÍSTICO PARA O ARTIGO ---
    double media = somaValores / numExecucoes;

    cout << "\n================ RESUMO ESTATISTICO ================" << endl;
    cout << "Instancia: " << n << " itens | Capacidade: " << capacidade << endl;
    cout << "Total de Execucoes: " << numExecucoes << endl;
    cout << "----------------------------------------------------" << endl;
    cout << "Melhor Valor Encontrado : " << melhorGlobal << endl;
    cout << "Pior Valor Encontrado   : " << piorGlobal << endl;
    cout << fixed << setprecision(2);
    cout << "Media dos Valores       : " << media << endl;
    cout << "====================================================" << endl;

    return 0;
}