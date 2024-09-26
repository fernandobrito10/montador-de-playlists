#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <ncurses.h>
#ifdef _WIN32
#include <windows.h>
#define usleep(microseconds) Sleep(microseconds / 1000)
#endif

#define MAX_OPTIONS 8
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
    keypad(stdscr, FALSE);
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
    keypad(stdscr, FALSE);
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

void excluirMusicaDePlaylist(struct Playlist *play) {
    keypad(stdscr, FALSE);
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
    keypad(stdscr, FALSE);
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

void atualizarMusicaDePlaylist(struct Playlist *play) {
    keypad(stdscr, FALSE);
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


/*void mostrarMenu() {
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
    printf("8. Sair\n");
    printf("\e[48;5;208m");
    printf("Escolha uma opção: ");
    printf("\e[m");
}
*/

void mostrarMenuComSetas(int highlight) {
    const char *opcoes[] = {
        "1. Criar uma nova playlist",
        "2. Adicionar música a uma playlist existente",
        "3. Excluir música de uma playlist",
        "4. Atualizar música de uma playlist",
        "5. Mostrar playlists",
        "6. Escolher uma playlist para mostrar",
        "7. Excluir uma playlist",
        "8. Sair"
    };

    for (int i = 0; i < MAX_OPTIONS; ++i) {
        if (i == highlight) {
            attron(A_REVERSE); // Inverter as cores para destacar a opção
        }
        mvprintw(i + 1, 1, "%s", opcoes[i]); // Corrigido: adicionado o formato %s
        attroff(A_REVERSE); // Voltar à exibição normal
    }
    refresh();
}



int main() {
    int mostrandoPlaylist = 0;
    struct Playlist playlists[MAX_PLAYLISTS];
    int numPlaylists = 0;
    int opcao = 0;
    
    

    imprimirTitulo();
    initscr(); // Inicializa o ncurses
    clear();
    noecho();
    cbreak();
    keypad(stdscr, TRUE);
    carregarPlaylists(playlists, &numPlaylists);

    int highlight = 0; // Para controlar a opção selecionada no menu
    int escolha = 0;
    int entrada;

    do {
        clear();
        mostrarMenuComSetas(highlight); // Mostra o menu com setas
        entrada = getch(); // Captura a entrada do teclado

        switch (entrada) {
            case KEY_UP:
                highlight--;
                if (highlight < 0) highlight = MAX_OPTIONS - 1; // Circular
                break;
            case KEY_DOWN:
                highlight++;
                if (highlight >= MAX_OPTIONS) highlight = 0; // Circular
                break;
            case 10: // Enter pressionado
                escolha = highlight + 1; // Traduzimos a opção para um número
                break;
            default:
                break;
        }

        if (entrada == 10) { // Se Enter foi pressionado
            opcao = escolha;  // Atribuímos a escolha ao `opcao`
            switch (opcao) {
                case 1:
                    keypad(stdscr, FALSE);
                    if (numPlaylists < MAX_PLAYLISTS) {
                        montarPlaylist(&playlists[numPlaylists]);
                        numPlaylists++;
                        salvarPlaylists(playlists, numPlaylists);
                    } else {
                        printw("Limite de playlists atingido.\n");
                    }
                    break;
                case 2:
                    keypad(stdscr, FALSE);
                    mostrarTodasPlaylists(playlists, numPlaylists);
                    if (numPlaylists > 0) {
                        printw("Escolha a playlist (1-%d): ", numPlaylists);
                        int escolha;
                        scanw("%d", &escolha);
                        limparBufferEntrada();
                        if (escolha >= 1 && escolha <= numPlaylists) {
                            adicionarMusicaAPlaylist(&playlists[escolha - 1]);
                            salvarPlaylists(playlists, numPlaylists);
                        } else {
                            printw("Escolha inválida.\n");
                        }
                    }
                    break;
                case 3:
                    keypad(stdscr, FALSE);
                    mostrarTodasPlaylists(playlists, numPlaylists);
                    if (numPlaylists > 0) {
                        printw("Escolha a playlist (1-%d): ", numPlaylists);
                        int escolha;
                        scanw("%d", &escolha);
                        limparBufferEntrada();
                        if (escolha >= 1 && escolha <= numPlaylists) {
                            excluirMusicaDePlaylist(&playlists[escolha - 1]);
                            salvarPlaylists(playlists, numPlaylists);
                        } else {
                            printw("Escolha inválida.\n");
                        }
                    }
                    break;
                case 4:
                    mostrarTodasPlaylists(playlists, numPlaylists);
                    if (numPlaylists > 0) {
                        printw("Escolha a playlist (1-%d): ", numPlaylists);
                        int escolha;
                        scanw("%d", &escolha);
                        limparBufferEntrada();
                        if (escolha >= 1 && escolha <= numPlaylists) {
                            atualizarMusicaDePlaylist(&playlists[escolha - 1]);
                            salvarPlaylists(playlists, numPlaylists);
                        } else {
                            printw("Escolha inválida.\n");
                        }
                    }
                    break;
                case 5:
                    keypad(stdscr, FALSE);
                    do {
                        mostrarTodasPlaylists(playlists, numPlaylists);
                        printw("Quer sair desse menu? 1 para não 2 para sim. ");
                        scanw("%d", &mostrandoPlaylist);
                    } while(mostrandoPlaylist != 2);
                    mostrandoPlaylist = 0;
                    break;
                case 6:
                    do {
                        keypad(stdscr, FALSE);
                        clear();
                        mostrarTodasPlaylists(playlists, numPlaylists);
                        if (numPlaylists > 0) {
                            printw("Escolha a playlist (1-%d): ", numPlaylists);
                            int escolha;
                            scanw("%d", &escolha);
                            limparBufferEntrada();
                            if (escolha >= 1 && escolha <= numPlaylists) {
                                mostrarPlaylist(&playlists[escolha - 1]);
                            } else {
                                printw("Escolha inválida.\n");
                            }
                        }
                        printw("Quer sair desse menu? 1 para não 2 para sim. ");
                        scanw("%d", &mostrandoPlaylist);
                    } while(mostrandoPlaylist != 2);
                    mostrandoPlaylist = 0;
                    break;
                case 7:
                    keypad(stdscr, FALSE);
                    excluirPlaylist(playlists, &numPlaylists);
                    break;
                case 8:
                    keypad(stdscr, FALSE);
                    printw("Saindo...\n");
                    break;
                default:
                    printw("Opção inválida. Tente novamente.\n");
                    break;
            }
        }
    } while (opcao != 8);

    endwin(); // Finaliza a sessão ncurses

    return 0;
}