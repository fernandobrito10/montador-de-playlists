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
            printf("\e[48;5;1m");
            printf("Playlist: %s\n", play->nome);
            printf("\e[48;5;2m");
            printf("Musica ");
            printf("\e[m");
            printf("| ");
            printf("\e[48;5;208m");
            printf("Artista");
            printf("\e[m");
            printf("\n------------------------\n");
            for (int i = 0; i < play->numMusicas; i++) {
                printf("\e[48;5;2m");
                printf("%s ", play->musicas[i]);
                printf("\e[m");
                printf("| ");
                printf("\e[48;5;208m");
                printf("%s\n", play->artistas[i]);
                printf("\e[m");
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
            printf("\e[48;5;1m");
            printf("Playlists cadastradas:\n");
            printf("\e[48;5;2m");
            for (int i = 0; i < numPlaylists; i++) {
                printf("%d. %s\n", i + 1, playlists[i].nome);
            }
        printf("\e[m");
        
    }

    void salvarPlaylists(struct Playlist playlists[], int numPlaylists) {
        FILE *arquivo = fopen("playlists.txt", "w");
        if (arquivo == NULL) {
            printf("Erro ao abrir o arquivo.\n");
            exit(1);
        }

        for (int i = 0; i < numPlaylists; i++) {
            fprintf(arquivo, "%s;%d\n", playlists[i].nome, playlists[i].numMusicas);
            for (int j = 0; j < playlists[i].numMusicas; j++) {
                fprintf(arquivo, "%s;%s\n", playlists[i].musicas[j], playlists[i].artistas[j]);
            }
        }

        fclose(arquivo);
    }

    void carregarPlaylists(struct Playlist playlists[], int *numPlaylists) {
        FILE *arquivo = fopen("playlists.txt", "r");
        if (arquivo == NULL) {
            printf("Arquivo de playlists ainda não existe ou não pode ser aberto.\n");
            return;
        }

        while (fscanf(arquivo, "%49[^;];%d\n", playlists[*numPlaylists].nome, &playlists[*numPlaylists].numMusicas) == 2) {
            for (int i = 0; i < playlists[*numPlaylists].numMusicas; i++) {
                fscanf(arquivo, "%49[^;];%49[^\n]\n", playlists[*numPlaylists].musicas[i], playlists[*numPlaylists].artistas[i]);
            }
            (*numPlaylists)++;
        }

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
                printf("\033[1;31mPlaylist excluida com sucesso.\033[0m\n\a");
                salvarPlaylists(playlists, *numPlaylists);
            } else {
                printf("Escolha inválida.\n");
            }
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
        printf("3. Mostrar playlists\n");
        printf("4. Escolher uma playlist para mostrar\n");
        printf("5. Excluir uma playlist\n");
        printf("6. Sair\n");
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
                    do {
                        mostrarTodasPlaylists(playlists, numPlaylists);
                        printf("Quer sair desse menu? 1 para não 2 para sim. ");
                        scanf("%d", &mostrandoPlaylist);
                    } while(mostrandoPlaylist != 2);
                    mostrandoPlaylist = 0;
                    break;
                case 4:
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
                case 5:
                    do {
                        excluirPlaylist(playlists, &numPlaylists);
                        printf("Você quer excluir mais alguma? 1 para sim 2 para não. ");
                        scanf("%d", &mostrandoPlaylist);
                    } while(mostrandoPlaylist != 2);
                    mostrandoPlaylist = 0;
                    break;
                case 6:
                    printf("Saindo...\n");
                    break;
                default:
                    printf("Opção inválida. Tente novamente.\n");
                    break;
            }
        } while (opcao != 6);

        return 0;
    }
