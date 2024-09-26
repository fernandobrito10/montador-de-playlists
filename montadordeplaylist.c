#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

#define MAX_PLAYLISTS 10
#define MAX_MUSICAS 50
#define GREEN "\033[1;32m"
#define WHITE "\033[1;37m"
#define RESET "\033[0m"

struct Playlist {
    char nome[50];
    char musicas[MAX_MUSICAS][50];
    char artistas[MAX_MUSICAS][50];
    int numMusicas;
};

void limparBufferEntrada() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

void print_box(const char *line, int highlight_index) {
    int length = strlen(line);
    for (int i = 0; i < length; i++) {
        if (i == highlight_index) {
            printf(GREEN "%c" RESET, line[i]);
        } else {
            printf(WHITE "%c" RESET, line[i]);
        }
    }
    printf("\n");
}

void imprimirTitulo() {
    char *box[] = {
        "******************************",
        "*     Montador de Playlist   *",
        "******************************"
    };

    int box_height = sizeof(box) / sizeof(box[0]);
    int box_width = strlen(box[0]);

    int delay = 100000;
    int cycles = 30;

    printf("\033[H\033[J");

    for (int cycle = 0; cycle < cycles; cycle++) {
        printf("\033[H\033[J");
        for (int i = 0; i < box_height; i++) {
            print_box(box[i], cycle % box_width);
        }
        usleep(delay);
    }
}

void montarPlaylist(struct Playlist *play) {
    printf("Digite o nome da playlist: ");
    scanf("%s", play->nome);
    limparBufferEntrada();

    play->numMusicas = 0;

    int adicionarMais;
    do {
        if (play->numMusicas < MAX_MUSICAS) {
            printf("Digite o nome da musica: ");
            scanf("%s", play->musicas[play->numMusicas]);

            printf("Digite o nome do artista: ");
            scanf("%s", play->artistas[play->numMusicas]);

            play->numMusicas++;
        } else {
            printf("Limite de musicas atingido para esta playlist.\n");
            break;
        }

        printf("Deseja adicionar mais musicas a esta playlist? (1 para Sim, 0 para Nao): ");
        scanf("%d", &adicionarMais);
        limparBufferEntrada();
    } while (adicionarMais == 1);
}

void adicionarMusicaAPlaylist(struct Playlist *play) {
    if (play->numMusicas < MAX_MUSICAS) {
        printf("Digite o nome da musica: ");
        scanf("%s", play->musicas[play->numMusicas]);

        printf("Digite o nome do artista: ");
        scanf("%s", play->artistas[play->numMusicas]);

        play->numMusicas++;
        printf("Musica adicionada a %s.\n", play->nome);
    } else {
        printf("Limite de musicas atingido para esta playlist.\n");
    }
}

void mostrarPlaylist(struct Playlist *play) {
    if (play->numMusicas > 0) {
        printf("Playlist: %s\n", play->nome);
        printf("Musica | Artista\n");
        printf("------------------------\n");
        for (int i = 0; i < play->numMusicas; i++) {
            printf("%s | %s\n", play->musicas[i], play->artistas[i]);
        }
        printf("------------------------\n");
    } else {
        printf("A playlist '%s' está vazia.\n", play->nome);
    }
}

void mostrarTodasPlaylists(struct Playlist playlists[], int numPlaylists) {
    if (numPlaylists == 0) {
        printf("Nenhuma playlist cadastrada.\n");
        return;
    }
    printf("Playlists cadastradas:\n");
    for (int i = 0; i < numPlaylists; i++) {
        printf("%d. %s\n", i + 1, playlists[i].nome);
    }
}

void salvarPlaylists(struct Playlist playlists[], int numPlaylists) {
    FILE *arquivo = fopen("playlists.bin", "wb");
    if (arquivo == NULL) {
        printf("Erro ao abrir o arquivo.\n");
        exit(1);
    }

    fwrite(&numPlaylists, sizeof(int), 1, arquivo);
    fwrite(playlists, sizeof(struct Playlist), numPlaylists, arquivo);

    fclose(arquivo);
}

void carregarPlaylists(struct Playlist playlists[], int *numPlaylists) {
    FILE *arquivo = fopen("playlists.bin", "rb");
    if (arquivo == NULL) {
        printf("Arquivo de playlists ainda não existe ou não pode ser aberto.\n");
        return;
    }

    fread(numPlaylists, sizeof(int), 1, arquivo);
    fread(playlists, sizeof(struct Playlist), *numPlaylists, arquivo);

    fclose(arquivo);
}

void excluirPlaylist(struct Playlist playlists[], int *numPlaylists) {
    mostrarTodasPlaylists(playlists, *numPlaylists);
    if (*numPlaylists > 0) {
        printf("Escolha a playlist a ser excluida (1-%d): ", *numPlaylists);
        int escolha;
        scanf("%d", &escolha);
        limparBufferEntrada();
        if (escolha >= 1 && escolha <= *numPlaylists) {
            for (int i = escolha - 1; i < *numPlaylists - 1; i++) {
                playlists[i] = playlists[i + 1];
            }
            (*numPlaylists)--;
            printf("Playlist excluida com sucesso.\n");
            salvarPlaylists(playlists, *numPlaylists);
        } else {
            printf("Escolha inválida.\n");
        }
    }
}

void excluirMusicaDePlaylist(struct Playlist *play) {
    if (play->numMusicas == 0) {
        printf("A playlist '%s' está vazia.\n", play->nome);
        return;
    }

    // Mostrar a lista de músicas da playlist
    printf("Músicas na playlist '%s':\n", play->nome);
    for (int i = 0; i < play->numMusicas; i++) {
        printf("%d. %s - %s\n", i + 1, play->musicas[i], play->artistas[i]);
    }

    // Perguntar qual música excluir
    printf("Escolha a música a ser excluída (1-%d): ", play->numMusicas);
    int escolha;
    scanf("%d", &escolha);
    limparBufferEntrada();

    if (escolha >= 1 && escolha <= play->numMusicas) {
        // Remover a música selecionada
        for (int i = escolha - 1; i < play->numMusicas - 1; i++) {
            strcpy(play->musicas[i], play->musicas[i + 1]);
            strcpy(play->artistas[i], play->artistas[i + 1]);
        }

        play->numMusicas--; // Reduz o número de músicas
        printf("Música excluída com sucesso.\n");
    } else {
        printf("Escolha inválida.\n");
    }
}

void atualizarMusicaDePlaylist(struct Playlist *play) {
    if (play->numMusicas == 0) {
        printf("A playlist '%s' está vazia.\n", play->nome);
        return;
    }

    // Mostrar a lista de músicas da playlist
    printf("Músicas na playlist '%s':\n", play->nome);
    for (int i = 0; i < play->numMusicas; i++) {
        printf("%d. %s - %s\n", i + 1, play->musicas[i], play->artistas[i]);
    }

    // Perguntar qual música atualizar
    printf("Escolha a música a ser atualizada (1-%d): ", play->numMusicas);
    int escolha;
    scanf("%d", &escolha);
    limparBufferEntrada();

    if (escolha >= 1 && escolha <= play->numMusicas) {
        // Solicitar novo nome e artista
        printf("Digite o novo nome da música: ");
        scanf("%s", play->musicas[escolha - 1]);

        printf("Digite o novo nome do artista: ");
        scanf("%s", play->artistas[escolha - 1]);

        printf("Música atualizada com sucesso.\n");
    } else {
        printf("Escolha inválida.\n");
    }
}

void mostrarTodasAsMusicas(struct Playlist playlists[], int numPlaylists) {
    // Vetores temporários para armazenar todas as músicas e artistas
    char todasMusicas[MAX_PLAYLISTS * MAX_MUSICAS][50];
    char todosArtistas[MAX_PLAYLISTS * MAX_MUSICAS][50];
    int totalMusicas = 0;

    // Percorre todas as playlists e adiciona as músicas à lista geral
    for (int i = 0; i < numPlaylists; i++) {
        for (int j = 0; j < playlists[i].numMusicas; j++) {
            strcpy(todasMusicas[totalMusicas], playlists[i].musicas[j]);
            strcpy(todosArtistas[totalMusicas], playlists[i].artistas[j]);
            totalMusicas++;
        }
    }

    // Ordena as músicas em ordem alfabética (usando Bubble Sort)
    for (int i = 0; i < totalMusicas - 1; i++) {
        for (int j = 0; j < totalMusicas - i - 1; j++) {
            if (strcmp(todasMusicas[j], todasMusicas[j + 1]) > 0) {
                // Troca as músicas
                char tempMusica[50], tempArtista[50];
                strcpy(tempMusica, todasMusicas[j]);
                strcpy(todasMusicas[j], todasMusicas[j + 1]);
                strcpy(todasMusicas[j + 1], tempMusica);

                // Troca os artistas correspondentes
                strcpy(tempArtista, todosArtistas[j]);
                strcpy(todosArtistas[j], todosArtistas[j + 1]);
                strcpy(todosArtistas[j + 1], tempArtista);
            }
        }
    }

    // Exibe a lista de músicas ordenadas
    printf("Lista de todas as músicas (ordenadas alfabeticamente):\n");
    printf("Música | Artista\n");
    printf("------------------------\n");
    for (int i = 0; i < totalMusicas; i++) {
        printf("%s | %s\n", todasMusicas[i], todosArtistas[i]);
    }
    printf("------------------------\n");

    // Pergunta se o usuário quer voltar ao menu
    int opcao;
    printf("Deseja voltar ao menu principal? (1 para Sim, 0 para Não): ");
    scanf("%d", &opcao);
    if (opcao != 1) {
        mostrarTodasAsMusicas(playlists, numPlaylists); // Recursivamente chama a função até o usuário escolher voltar
    }
}


void mostrarMenu() {
    system("clear");
    printf("\e[48;5;2m");
    printf("\e[38;5;0m");
    printf("Bem-vindo ao montador de playlists!\n");
    printf("\e[48;5;3m");
    printf("1. Criar uma nova playlist (Não é permitido espaço nos nomes)\n");
    printf("2. Adicionar música a uma playlist existente\n");
    printf("3. Excluir música de uma playlist\n");
    printf("4. Atualizar música de uma playlist\n");  // Nova opção para atualizar
    printf("5. Mostrar playlists\n");
    printf("6. Escolher uma playlist para mostrar\n");
    printf("7. Excluir uma playlist\n");
    printf("8. Listar músicas em ordem alfabética\n");
    printf("9. Sair\n");
    printf("\e[48;5;208m");
    printf("Escolha uma opção: ");
    printf("\e[m");
}



int main() {
    int mostrandoPlaylist = 0;
    struct Playlist playlists[MAX_PLAYLISTS];
    int numPlaylists = 0;
    int opcao = 0;
    imprimirTitulo();
    carregarPlaylists(playlists, &numPlaylists);

    do {
        mostrarMenu();
        scanf("%d", &opcao);
        limparBufferEntrada();

        switch (opcao) {
            case 1:
                if (numPlaylists < MAX_PLAYLISTS) {
                    montarPlaylist(&playlists[numPlaylists]);
                    numPlaylists++;
                    salvarPlaylists(playlists, numPlaylists);
                } else {
                    printf("Limite de playlists atingido.\n");
                }
                break;
            case 2:
                mostrarTodasPlaylists(playlists, numPlaylists);
                if (numPlaylists > 0) {
                    printf("Escolha a playlist (1-%d): ", numPlaylists);
                    int escolha;
                    scanf("%d", &escolha);
                    limparBufferEntrada();
                    if (escolha >= 1 && escolha <= numPlaylists) {
                        adicionarMusicaAPlaylist(&playlists[escolha - 1]);
                        salvarPlaylists(playlists, numPlaylists);
                    } else {
                        printf("Escolha inválida.\n");
                    }
                }
                break;
            case 3:
                mostrarTodasPlaylists(playlists, numPlaylists);
                if (numPlaylists > 0) {
                    printf("Escolha a playlist (1-%d): ", numPlaylists);
                    int escolha;
                    scanf("%d", &escolha);
                    limparBufferEntrada();
                    if (escolha >= 1 && escolha <= numPlaylists) {
                        excluirMusicaDePlaylist(&playlists[escolha - 1]);
                        salvarPlaylists(playlists, numPlaylists);
                    } else {
                        printf("Escolha inválida.\n");
                    }
                }
                break;
            case 4:  // Caso para atualizar uma música
                mostrarTodasPlaylists(playlists, numPlaylists);
                if (numPlaylists > 0) {
                    printf("Escolha a playlist (1-%d): ", numPlaylists);
                    int escolha;
                    scanf("%d", &escolha);
                    limparBufferEntrada();
                    if (escolha >= 1 && escolha <= numPlaylists) {
                        atualizarMusicaDePlaylist(&playlists[escolha - 1]);
                        salvarPlaylists(playlists, numPlaylists);
                    } else {
                        printf("Escolha inválida.\n");
                    }
                }
                break;
            case 5:
                do {
                    mostrarTodasPlaylists(playlists, numPlaylists);
                    printf("Quer sair desse menu? 1 para não 2 para sim. ");
                    scanf("%d", &mostrandoPlaylist);
                } while(mostrandoPlaylist != 2);
                mostrandoPlaylist = 0;
                break;
            case 6:
                do {
                    system("clear");
                    mostrarTodasPlaylists(playlists, numPlaylists);
                    if (numPlaylists > 0) {
                        printf("Escolha a playlist (1-%d): ", numPlaylists);
                        int escolha;
                        scanf("%d", &escolha);
                        limparBufferEntrada();
                        if (escolha >= 1 && escolha <= numPlaylists) {
                            mostrarPlaylist(&playlists[escolha - 1]);
                        } else {
                            printf("Escolha inválida.\n");
                        }
                    }
                    printf("Quer sair desse menu? 1 para não 2 para sim. ");
                    scanf("%d", &mostrandoPlaylist);
                } while(mostrandoPlaylist != 2);
                mostrandoPlaylist = 0;
                break;
            case 7:
                excluirPlaylist(playlists, &numPlaylists);
                break;
            case 8:
                mostrarTodasAsMusicas(playlists, numPlaylists);
                break;
            case 9:
                printf("Saindo...\n");
                break;
            default:
                printf("Opção inválida. Tente novamente.\n");
                break;
        }
    } while (opcao != 9);

    return 0;
}
