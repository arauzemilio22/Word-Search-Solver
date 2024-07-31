#include "solver.h"
#include <pthread.h>

// Define a mutex for thread-safe BST insertion
pthread_mutex_t bst_mutex = PTHREAD_MUTEX_INITIALIZER;

// Function to search for words in the sub-puzzle and insert them into the BST if they exist in the dictionary
void* solve(void* arg) {
    char*** sub_puzzle = ((char***)arg)[0];
    int subpuzzle_rows = *((int*)(((char***)arg)[1]));
    int subpuzzle_cols = *((int*)(((char***)arg)[2]));
    hashset* dict = (hashset*)(((char***)arg)[3]);
    tnode** bst_root = (tnode**)(((char***)arg)[4]);
    int min_len = *((int*)(((char***)arg)[5]));
    int max_len = *((int*)(((char***)arg)[6]));
    int sorted = *((int*)(((char***)arg)[7]));

    // Check all directions
    char word[15]; // max word length + 1

    for (int r = 0; r < subpuzzle_rows; r++) {
        for (int c = 0; c < subpuzzle_cols; c++) {
            // Horizontal right
            for (int len = min_len; len <= max_len && c + len <= subpuzzle_cols; len++) {
                strncpy(word, &sub_puzzle[r][c], len);
                word[len] = '\0';
                if (search(*dict, word)) {
                    pthread_mutex_lock(&bst_mutex);
                    bst_insert(bst_root, word);
                    pthread_mutex_unlock(&bst_mutex);
                }
            }

            // Horizontal left
            for (int len = min_len; len <= max_len && c - len + 1 >= 0; len++) {
                for (int i = 0; i < len; i++) {
                    word[i] = sub_puzzle[r][c - i];
                }
                word[len] = '\0';
                if (search(*dict, word)) {
                    pthread_mutex_lock(&bst_mutex);
                    bst_insert(bst_root, word);
                    pthread_mutex_unlock(&bst_mutex);
                }
            }

            // Vertical down
            for (int len = min_len; len <= max_len && r + len <= subpuzzle_rows; len++) {
                for (int i = 0; i < len; i++) {
                    word[i] = sub_puzzle[r + i][c];
                }
                word[len] = '\0';
                if (search(*dict, word)) {
                    pthread_mutex_lock(&bst_mutex);
                    bst_insert(bst_root, word);
                    pthread_mutex_unlock(&bst_mutex);
                }
            }

            // Vertical up
            for (int len = min_len; len <= max_len && r - len + 1 >= 0; len++) {
                for (int i = 0; i < len; i++) {
                    word[i] = sub_puzzle[r - i][c];
                }
                word[len] = '\0';
                if (search(*dict, word)) {
                    pthread_mutex_lock(&bst_mutex);
                    bst_insert(bst_root, word);
                    pthread_mutex_unlock(&bst_mutex);
                }
            }

            // Diagonal right down
            for (int len = min_len; len <= max_len && r + len <= subpuzzle_rows && c + len <= subpuzzle_cols; len++) {
                for (int i = 0; i < len; i++) {
                    word[i] = sub_puzzle[r + i][c + i];
                }
                word[len] = '\0';
                if (search(*dict, word)) {
                    pthread_mutex_lock(&bst_mutex);
                    bst_insert(bst_root, word);
                    pthread_mutex_unlock(&bst_mutex);
                }
            }

            // Diagonal left up
            for (int len = min_len; len <= max_len && r - len + 1 >= 0 && c - len + 1 >= 0; len++) {
                for (int i = 0; i < len; i++) {
                    word[i] = sub_puzzle[r - i][c - i];
                }
                word[len] = '\0';
                if (search(*dict, word)) {
                    pthread_mutex_lock(&bst_mutex);
                    bst_insert(bst_root, word);
                    pthread_mutex_unlock(&bst_mutex);
                }
            }

            // Diagonal right up
            for (int len = min_len; len <= max_len && r - len + 1 >= 0 && c + len <= subpuzzle_cols; len++) {
                for (int i = 0; i < len; i++) {
                    word[i] = sub_puzzle[r - i][c + i];
                }
                word[len] = '\0';
                if (search(*dict, word)) {
                    pthread_mutex_lock(&bst_mutex);
                    bst_insert(bst_root, word);
                    pthread_mutex_unlock(&bst_mutex);
                }
            }

            // Diagonal left down
            for (int len = min_len; len <= max_len && r + len <= subpuzzle_rows && c - len + 1 >= 0; len++) {
                for (int i = 0; i < len; i++) {
                    word[i] = sub_puzzle[r + i][c - i];
                }
                word[len] = '\0';
                if (search(*dict, word)) {
                    pthread_mutex_lock(&bst_mutex);
                    bst_insert(bst_root, word);
                    pthread_mutex_unlock(&bst_mutex);
                }
            }
        }
    }

    pthread_exit(NULL);
}

void print_buffer(char** sub_puzzle, int subpuzzle_rows, int subpuzzle_cols) {
    // This function is used for printing the content of each buffer cell.
    // Do NOT call this function anywhere in your final submission.
    printf("%d by %d\n", subpuzzle_rows, subpuzzle_cols);
    for (int i = 0; i < subpuzzle_rows; i++)
        for (int j = 0; j < subpuzzle_cols; j++)
            printf("%c%s", sub_puzzle[i][j], j == subpuzzle_cols - 1 ? "\n" : "");
}

int main(int argc, char** argv) {
    if (argc < 11)
        error("Fatal Error. Usage: solve -dict dict.txt -input puzzle1mb.txt -size 1000 -nbuffer 64 -len 4:7 [-s]", 1);
    int puzzle_size = 0, buf_cells = 0, i = 0, j, fd, min_len = 0, max_len = 0, sorted = 0, buf_dimension;
    char* filename = NULL;
    FILE* dictionary_file;
    char*** buffer;
    while (++i < argc) {
        if (argv[i][0] != '-')
            error("Fatal Error. Invalid CLA", 2);
        if (!strcmp(argv[i], "-size")) {
            puzzle_size = atoi(argv[++i]);
            if (puzzle_size < 15 || puzzle_size > 46340)
                error("Fatal Error. Illegal value passed after -size", 3);
        } else if (!strcmp(argv[i], "-nbuffer")) {
            buf_cells = atoi(argv[++i]);
            if (buf_cells != 1 && buf_cells != 4 && buf_cells != 16 && buf_cells != 64)
                error("Fatal Error. Illegal value passed after -nbuffer", 4);
            buf_dimension = (int)sqrt(MEM_LIMIT / buf_cells);
        } else if (!strcmp(argv[i], "-input")) {
            filename = strdup(argv[++i]);
            fd = open(filename, O_RDONLY, 0);
            if (fd < 0)
                error("Fatal Error. Illegal value passed after -input", 5);
        } else if (!strcmp(argv[i], "-dict")) {
            dictionary_file = fopen(argv[++i], "r");
            if (!dictionary_file)
                error("Fatal Error. Illegal value passed after -dict", 6);
        } else if (!strcmp(argv[i], "-len")) {
            char* min_max = strdup(argv[++i]);
            char* max_str;
            if (strrchr(min_max, ':') != (max_str = strchr(min_max, ':')) || !strchr(min_max, ':'))
                error("Fatal Error. Illegal value passed after -len", 7);
            max_len = atoi(max_str + 1);
            min_max[max_str - min_max] = '\0';
            min_len = atoi(min_max);
            if (min_len < 3 || max_len > 14 || min_len > max_len)
                error("Fatal Error. Illegal value passed after -len", 7);
        } else if (!strcmp(argv[i], "-s"))
            sorted = 1;
        else {
            error("Fatal Error. Usage: solve -dict dict.txt -input puzzle1mb.txt -size 1000 -nbuffer 64 -len 4:7 [-s]", 1);
            break;
        }
    }
    
    hashset dict = set_init();
    char word[100];
    while (fscanf(dictionary_file, "%s", word) != EOF) {
        insert(&dict, word);
    }
    fclose(dictionary_file);

    buffer = (char***)malloc(buf_cells * sizeof(char**));
    for (i = 0; i < buf_cells; i++) {
        buffer[i] = (char**)malloc(buf_dimension * sizeof(char*));
        for (j = 0; j < buf_dimension; j++)
            buffer[i][j] = (char*)malloc(buf_dimension);
    }

    int buf_index = 0;
    pthread_t t_id[buf_cells];
    tnode* bst_root = NULL;

    for (i = 0; i < buf_cells; i++)
        t_id[i] = NULL;

    for (int row = 0; row + max_len - 1 < puzzle_size; row += (buf_dimension - max_len + 1)) {
        int subpuzzle_rows = (row + buf_dimension <= puzzle_size) ? buf_dimension : puzzle_size - row;
        for (int column = 0; column + max_len - 1 < puzzle_size; column += (buf_dimension - max_len + 1)) {
            long start = (long)row * (puzzle_size + 1) + column;
            lseek(fd, start, SEEK_SET);
            int subpuzzle_cols = (column + buf_dimension <= puzzle_size) ? buf_dimension : puzzle_size - column;
            if (t_id[buf_index])
                pthread_join(t_id[buf_index], NULL);

            for (i = 0; i < subpuzzle_rows; i++) {
                int n_read = read(fd, buffer[buf_index][i], subpuzzle_cols);
                if (n_read < subpuzzle_cols)
                    error("Fatal Error. Bad read from input file", 10);
                if (subpuzzle_cols < buf_dimension)
                    buffer[buf_index][i][subpuzzle_cols] = '\0';
                lseek(fd, puzzle_size - subpuzzle_cols + 1, SEEK_CUR);
            }
            if (subpuzzle_rows < buf_dimension)
                buffer[buf_index][subpuzzle_rows] = NULL;

            char** arg_buffer = (char**)malloc(8 * sizeof(char*));
            arg_buffer[0] = (char*)buffer[buf_index];
            arg_buffer[1] = (char*)&subpuzzle_rows;
            arg_buffer[2] = (char*)&subpuzzle_cols;
            arg_buffer[3] = (char*)&dict;
            arg_buffer[4] = (char*)&bst_root;
            arg_buffer[5] = (char*)&min_len;
            arg_buffer[6] = (char*)&max_len;
            arg_buffer[7] = (char*)&sorted;

            pthread_create(t_id + buf_index, NULL, solve, arg_buffer);
            buf_index = (buf_index == buf_cells - 1) ? 0 : buf_index + 1;
        }
    }
    for (i = 0; i < buf_cells; i++)
        if (t_id[i])
            pthread_join(t_id[i], NULL);
    
    if (sorted) {
        inorder_print(bst_root);
    }

    // Free allocated memory
    for (i = 0; i < buf_cells; i++) {
        for (j = 0; j < buf_dimension; j++)
            free(buffer[i][j]);
        free(buffer[i]);
    }
    free(buffer);
    free(filename);
    pthread_mutex_destroy(&bst_mutex);  // Destroy the mutex
}

