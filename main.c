typedef struct Card {
    int id;
    int isTurned;
    int isRemoved;
    int times_reveled;
    int lines;
    int colums;
} Card;

typedef struct Node {
    Card card;
    struct No *next;
} No;

typedef struct {
    int lines;
    int colums;
    int cell_width;
    int cell_height;
    int margin;
} BoardView;
