// ============================================================================
//         PROJETO WAR ESTRUTURADO - DESAFIO DE CÓDIGO
// ============================================================================
//        
// ============================================================================

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <locale.h>

// ============================================================================
// --- Constantes Globais ---
// ============================================================================
#define NUM_TERRITORIOS 5       // Número fixo de territórios para simplificar o mapa inicial
#define MAX_MISSOES 3           // Número de missões disponíveis
#define TAM_NOME 50
#define TAM_COR 20

// ============================================================================
// --- Estrutura de Dados ---
// ============================================================================

typedef struct {
    char nome[TAM_NOME];
    char cor_exercito[TAM_COR];
    int num_tropas;
} Territorio;

// Estrutura para o jogador (opcional, mas útil para consolidar dados)
typedef struct {
    char cor[TAM_COR];
    int missao_id;
    int territorios_conquistados; // Contador útil para algumas missões
} Jogador;

// ============================================================================
// --- Protótipos das Funções ---
// ============================================================================

// Funções de setup e gerenciamento de memória:
Territorio* alocarMapa();
void inicializarTerritorios(Territorio *mapa, int tamanho, Jogador *jogador);
void liberarMemoria(Territorio *mapa);

// Funções de interface com o usuário:
void exibirMenuPrincipal();
void exibirMapa(const Territorio *mapa, int tamanho); // Passagem por referência constante
void exibirMissao(int missao_id);

// Funções de lógica principal do jogo:
void faseDeAtaque(Territorio *mapa, Jogador *jogador, int tamanho); // Modifica o mapa e o jogador
void simularAtaque(Territorio *atacante, Territorio *defensor, Jogador *jogador); // Modifica territórios e jogador
int sortearMissao();
int verificarVitoria(const Territorio *mapa, const Jogador *jogador, int tamanho); // Passagem por referência constante

// Função utilitária:
void limparBufferEntrada();
int rolarDado(int num_lados); // Gera um número aleatório (rolagem de dado)

// ============================================================================
// --- Função Principal (main) ---
// ============================================================================
int main() {
    // 1. Configuração Inicial (Setup):
    setlocale(LC_ALL, "pt_BR.UTF-8"); // Define o locale para português.
    srand(time(NULL));                 // Inicializa a semente para geração de números aleatórios.

    Territorio *mapa = alocarMapa();
    Jogador jogador_principal;

    if (mapa == NULL) {
        fprintf(stderr, "ERRO: Falha na alocação de memória para o mapa.\n");
        return 1;
    }

    // Inicializa dados do jogador e territórios
    strcpy(jogador_principal.cor, "VERMELHO");
    jogador_principal.missao_id = sortearMissao();
    jogador_principal.territorios_conquistados = 0;

    inicializarTerritorios(mapa, NUM_TERRITORIOS, &jogador_principal);

    int opcao;
    int jogo_ativo = 1;
    int venceu = 0;

    printf("--- 🎲 BEM-VINDO AO WAR ESTRUTURADO! 🌍 ---\n");

    // 2. Laço Principal do Jogo (Game Loop):
    do {
        printf("\n\n=== Rodada Atual | Jogador: %s ===\n", jogador_principal.cor);

        exibirMapa(mapa, NUM_TERRITORIOS);
        exibirMissao(jogador_principal.missao_id);
        exibirMenuPrincipal();

        printf("Escolha sua ação (0-2): ");
        if (scanf("%d", &opcao) != 1) {
            limparBufferEntrada();
            opcao = -1; // Opção inválida
        }
        limparBufferEntrada();

        switch (opcao) {
            case 1:
                faseDeAtaque(mapa, &jogador_principal, NUM_TERRITORIOS);
                break;
            case 2:
                venceu = verificarVitoria(mapa, &jogador_principal, NUM_TERRITORIOS);
                if (venceu) {
                    printf("\n🎉 PARABÉNS! Você cumpriu sua missão e venceu o jogo!\n");
                    jogo_ativo = 0;
                } else {
                    printf("\n... Missão ainda não foi concluída. Continue a batalha!\n");
                }
                break;
            case 0:
                printf("\nEncerrando o jogo...\n");
                jogo_ativo = 0;
                break;
            default:
                printf("\n⚠️ Opção inválida. Tente novamente.\n");
        }
        
        // Pausa (simulando o final da rodada)
        if (jogo_ativo) {
            printf("\n--- Fim da Ação ---\nPressione ENTER para continuar para a próxima rodada.");
            getchar(); 
        }

    } while (jogo_ativo);

    // 3. Limpeza:
    liberarMemoria(mapa);
    printf("Memória liberada. Fim do programa.\n");

    return 0;
}

// ============================================================================
// --- Implementação das Funções ---
// ============================================================================

/**
 * @brief Aloca dinamicamente a memória para o vetor de territórios.
 */
Territorio* alocarMapa() {
    // Aloca memória para NUM_TERRITORIOS estruturas do tipo Territorio
    Territorio *mapa = (Territorio *)calloc(NUM_TERRITORIOS, sizeof(Territorio));
    return mapa;
}

/**
 * @brief Preenche os dados iniciais dos territórios.
 * @param mapa Ponteiro para a primeira posição do vetor de territórios (por referência).
 * @param tamanho O número de territórios no mapa.
 * @param jogador Ponteiro para os dados do jogador principal.
 */
void inicializarTerritorios(Territorio *mapa, int tamanho, Jogador *jogador) {
    if (mapa == NULL) return;

    // Distribuição inicial (simples para este projeto)
    
    // Jogador principal (VERMELHO) começa com 3 territórios
    strcpy(mapa[0].nome, "Groelandia");
    strcpy(mapa[0].cor_exercito, jogador->cor);
    mapa[0].num_tropas = 3;

    strcpy(mapa[1].nome, "Alaska");
    strcpy(mapa[1].cor_exercito, jogador->cor);
    mapa[1].num_tropas = 2;

    strcpy(mapa[2].nome, "Espanha");
    strcpy(mapa[2].cor_exercito, jogador->cor);
    mapa[2].num_tropas = 1;

    // Exércitos Neutros/Inimigos (AZUL e VERDE) com 1 território cada
    strcpy(mapa[3].nome, "India");
    strcpy(mapa[3].cor_exercito, "AZUL");
    mapa[3].num_tropas = 3;

    strcpy(mapa[4].nome, "Siberia");
    strcpy(mapa[4].cor_exercito, "VERDE");
    mapa[4].num_tropas = 4;
}

/**
 * @brief Libera a memória alocada para o mapa.
 */
void liberarMemoria(Territorio *mapa) {
    if (mapa != NULL) {
        free(mapa);
    }
}

/**
 * @brief Imprime o menu de ações disponíveis.
 */
void exibirMenuPrincipal() {
    printf("\n--- Menu de Ações ---\n");
    printf("1. ⚔️ Iniciar Fase de Ataque\n");
    printf("2. ✅ Verificar Condição de Vitória (Missão)\n");
    printf("0. 🚪 Sair do Jogo\n");
    printf("----------------------\n");
}

/**
 * @brief Mostra o estado atual dos territórios em formato de tabela.
 * @param mapa Ponteiro constante para o vetor de territórios (apenas leitura).
 */
void exibirMapa(const Territorio *mapa, int tamanho) {
    printf("\n--- 🗺️ MAPA MUNDI WAR --- \n");
    printf("| %-4s | %-20s | %-15s | %-10s |\n", "ID", "Território", "Cor Exército", "Tropas");
    printf("|------|----------------------|-----------------|------------|\n");

    for (int i = 0; i < tamanho; i++) {
        printf("| %-4d | %-20s | %-15s | %-10d |\n", 
               i, // Usando ID de 0 a 4
               mapa[i].nome, 
               mapa[i].cor_exercito, 
               mapa[i].num_tropas);
    }
    printf("----------------------------------------------------------\n");
}

/**
 * @brief Exibe a descrição da missão secreta do jogador.
 */
void exibirMissao(int missao_id) {
    printf("\n--- 📜 Sua Missão Secreta (ID: %d) ---\n", missao_id);
    switch (missao_id) {
        case 1:
            printf("Destruir o Exército AZUL: Conquiste todos os territórios dominados por eles.\n");
            break;
        case 2:
            printf("Destruir o Exército VERDE: Conquiste todos os territórios dominados por eles.\n");
            break;
        case 3:
            printf("Conquistar 4 Territórios Inimigos: Conquiste 4 territórios que não são seus.\n");
            break;
        default:
            printf("Missão Indefinida. (Erro no sorteio)\n");
    }
    printf("----------------------------------------\n");
}

/**
 * @brief Sorteia um ID de missão aleatório.
 */
int sortearMissao() {
    // Sorteia um ID entre 1 e MAX_MISSOES (inclusivo)
    return (rand() % MAX_MISSOES) + 1;
}

/**
 * @brief Função utilitária para limpar o buffer de entrada.
 */
void limparBufferEntrada() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) {
        // Consome o restante da linha
    }
}

/**
 * @brief Simula a rolagem de um dado.
 */
int rolarDado(int num_lados) {
    // rand() % N -> gera números de 0 a N-1
    return (rand() % num_lados) + 1; // Retorna um número de 1 a 6 (ou num_lados)
}

/**
 * @brief Executa a lógica de uma batalha entre dois territórios.
 */
void simularAtaque(Territorio *atacante, Territorio *defensor, Jogador *jogador) {
    // Validação básica: atacante precisa de pelo menos 2 tropas para atacar (1 fica no território)
    if (atacante->num_tropas < 2) {
        printf("\n🚫 Ataque cancelado: O território %s tem apenas %d tropa. Mínimo de 2 tropas é necessário.\n", atacante->nome, atacante->num_tropas);
        return;
    }

    printf("\n--- 🎲 SIMULAÇÃO DE ATAQUE ---\n");
    printf("Atacante: %s (%s, %d tropas) vs Defensor: %s (%s, %d tropas)\n", 
           atacante->nome, atacante->cor_exercito, atacante->num_tropas, 
           defensor->nome, defensor->cor_exercito, defensor->num_tropas);
    
    // Simulação com um dado para simplificar
    int dado_ataque = rolarDado(6);
    int dado_defesa = rolarDado(6);

    printf("Rolagem do Dado - Ataque: %d | Defesa: %d\n", dado_ataque, dado_defesa);

    if (dado_ataque > dado_defesa) {
        // Ataque vence
        printf("Vitória do Ataque! O defensor perde 1 tropa.\n");
        defensor->num_tropas--;
        
        // Verificação de Conquista
        if (defensor->num_tropas == 0) {
            printf("\nCONQUISTA! %s foi conquistado pelo %s!\n", defensor->nome, atacante->cor_exercito);
            
            // 1. Atualizar Dono
            char cor_antiga[TAM_COR];
            strcpy(cor_antiga, defensor->cor_exercito); // Guarda a cor do exército destruído
            strcpy(defensor->cor_exercito, atacante->cor_exercito);
            
            // 2. Mover 1 tropa (mínimo necessário para ocupar)
            atacante->num_tropas--;
            defensor->num_tropas = 1;

            // 3. Atualizar estatísticas do jogador se a conquista foi de fato um ganho
            if (strcmp(atacante->cor_exercito, jogador->cor) == 0) {
                jogador->territorios_conquistados++;
                printf("Jogador %s agora possui %d territórios inimigos conquistados.\n", jogador->cor, jogador->territorios_conquistados);
            }
        }
    } else {
        // Defesa vence (incluindo empate)
        printf("Vitória da Defesa (ou empate)! O atacante perde 1 tropa.\n");
        atacante->num_tropas--;
    }
}

/**
 * @brief Gerencia a fase de ataque, obtendo IDs de territórios do usuário.
 */
void faseDeAtaque(Territorio *mapa, Jogador *jogador, int tamanho) {
    int id_atacante, id_defensor;

    printf("\n--- ⚔️ FASE DE ATAQUE ---\n");
    printf("Digite o ID do seu território de ataque (0 a %d): ", tamanho - 1);
    scanf("%d", &id_atacante);
    limparBufferEntrada();
    
    if (id_atacante < 0 || id_atacante >= tamanho || strcmp(mapa[id_atacante].cor_exercito, jogador->cor) != 0) {
        printf("🚫 ID inválido ou território não pertence a você.\n");
        return;
    }

    printf("Digite o ID do território inimigo para defender (0 a %d): ", tamanho - 1);
    scanf("%d", &id_defensor);
    limparBufferEntrada();

    if (id_defensor < 0 || id_defensor >= tamanho || strcmp(mapa[id_defensor].cor_exercito, jogador->cor) == 0) {
        printf("🚫 ID inválido ou você não pode atacar um território próprio.\n");
        return;
    }
    
    // A simulação de ataque é executada apenas se os IDs forem válidos e distintos
    if (id_atacante != id_defensor) {
        simularAtaque(&mapa[id_atacante], &mapa[id_defensor], jogador);
    } else {
        printf("🚫 Você não pode atacar a si mesmo!\n");
    }
}

/**
 * @brief Verifica se o jogador cumpriu os requisitos de sua missão.
 * @return 1 se a missão foi cumprida, 0 caso contrário.
 */
int verificarVitoria(const Territorio *mapa, const Jogador *jogador, int tamanho) {
    // Lógica para cada tipo de missão
    switch (jogador->missao_id) {
        case 1: // Missão: Destruir o Exército AZUL
        {
            for (int i = 0; i < tamanho; i++) {
                if (strcmp(mapa[i].cor_exercito, "AZUL") == 0) {
                    return 0; // Se ainda houver território AZUL, a missão não foi cumprida
                }
            }
            return 1; // Todos os territórios AZUL foram conquistados
        }
        case 2: // Missão: Destruir o Exército VERDE
        {
            for (int i = 0; i < tamanho; i++) {
                if (strcmp(mapa[i].cor_exercito, "VERDE") == 0) {
                    return 0; // Se ainda houver território VERDE, a missão não foi cumprida
                }
            }
            return 1; // Todos os territórios VERDE foram conquistados
        }
        case 3: // Missão: Conquistar 4 Territórios Inimigos
            if (jogador->territorios_conquistados >= 4) {
                return 1;
            }
            return 0;
        default:
            return 0; // Missão desconhecida
    }
}