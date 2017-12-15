#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <termios.h>
#include <time.h>
#include <stdbool.h>

#define N          4
#define MOVE_UP    'A'
#define MOVE_LEFT  'D'
#define MOVE_DOWN  'B'
#define MOVE_RIGHT 'C'
#define QUIT       'q'
#define WHITE      "    "
#define LINE_HOR   "\u2500"
#define LINE_VERT  "\u2502"
#define CORNER_UL  "\u250C"
#define CORNER_UR  "\u2510"
#define CORNER_BL  "\u2514"
#define CORNER_BR  "\u2518"
#define CONN_TOP   "\u252C"
#define CONN_RIGHT "\u2524"
#define CONN_BOT   "\u2534"
#define CONN_LEFT  "\u251C"
#define CONN_ALL   "\u253C"
#define CLEAR_SCR  "\033c"

typedef int MAT[N][N];


void ninsert_mat (MAT mat)
{
    int r, c;
    do {
        r= rand()%N;
        c= rand()%N;
    }
    while (mat[r][c]);

    if (rand()%4)
        mat[r][c]= 2;
    else
        mat[r][c]= 4;
}


void init_mat (MAT mat)
{
    int r, c;
    for (r=0; r<N; r++)
        for (c=0; c<N; c++)
            mat[r][c]= 0;
    
    for (int n=0; n < (N*N)/2; n++)
        ninsert_mat(mat);
}


int NMOVES= -1;

void print_moves (void)
{
    printf(" Moves:%6d", ++NMOVES);
}


void print_mat (MAT mat)
{
    int r, c;
    
    //print top line
    printf(CORNER_UL);
    for (c=0; c<N-1; c++)
    {
        for (int s=0; s<4; s++)
            printf(LINE_HOR);
        printf(CONN_TOP);
    }
    for (int s=0; s<4; s++)
        printf(LINE_HOR);
    printf(CORNER_UR);
    
    print_moves();
    printf("\n");
    //print body of the grid
    for (r=0; r<N-1; r++)     //Rows
    {
        for (c=0; c<N; c++) //Cols
        {
            printf(LINE_VERT);
            if (mat[r][c])
                printf("%4d", mat[r][c]);
            else
                printf(WHITE);
        }
        printf(LINE_VERT "\n");
        
        printf(CONN_LEFT);
        for (c=0; c<N-1; c++)
        {
            for (int s=0; s<4; s++)
                printf(LINE_HOR);
            printf(CONN_ALL);
        }
        for (int s=0; s<4; s++)
            printf(LINE_HOR);
        printf(CONN_RIGHT "\n");
    }
    
    //print the last line of the grid (+ bot line)
    for (c=0; c<N; c++) //Cols
    {
        printf(LINE_VERT);
        if (mat[r][c])
            printf("%4d", mat[r][c]);
        else
            printf(WHITE);
    }
    printf(LINE_VERT "\n" CORNER_BL);
    for (c=0; c<N-1; c++)
    {
        for (int s=0; s<4; s++)
            printf(LINE_HOR);
        printf(CONN_BOT);
    }
    for (int s=0; s<4; s++)
        printf(LINE_HOR);
    printf(CORNER_BR "\n");
}


void displace_l_vec (int vec[N], int from)
{
    for (int r=from; r<N; r++)
        vec[r]= vec[r+1];
        
    vec[N-1]= 0;
}


void displace_r_vec (int vec[N], int from)
{
    for (int r=from; r>0; r--)
        vec[r]= vec[r-1];
    
    vec[0]= 0;
}


void move_l_vec (int vec[N])
{
    int p;
    for (p=0; p<N-1; p++)
        for (int try=p; vec[p]==0 && try<N; try++)
        {
            displace_l_vec(vec, p);
        }
        
    for (p=0; p<N-1; p++)
        if (vec[p]==vec[p+1])
        {
            vec[p] *= 2;
            displace_l_vec(vec, p+1);
        }
}


void move_r_vec (int vec[N])
{
    int p;
    for (int p=N-1; p>0; p--)
        for (int try=p; vec[p]==0 && try>0; try--)
        {
            displace_r_vec(vec, p);
        }
    
    for (p=N-1; p>0; p--)
        if (vec[p]==vec[p-1])
        {
            vec[p] *= 2;
            displace_r_vec(vec, p-1);
        }
}


void extract_col_mat (int *vec, MAT mat, int col)
{
    for (int f=0; f<N; f++)
        vec[f]= mat[f][col];
}


void insert_col_mat (int *vec, MAT mat, int col)
{
    for (int f=0; f<N; f++)
        mat[f][col]= vec[f];
}


bool check_diffs(MAT mat, MAT prev)
{
    bool diff= 0;
    for (int r=0; r<N; r++)
        for (int c=0; c<N; c++)
        {
            if (mat[r][c] != prev[r][c])
                diff= 1;
            
            prev[r][c]= mat[r][c];
        }
    return diff;
}


bool check_stdin (char input)
{
    char valid_inputs[5]= {
        MOVE_UP,
        MOVE_LEFT,
        MOVE_DOWN,
        MOVE_RIGHT,
        QUIT};
        
    for (int i=0; i<5; i++)
        if (input==valid_inputs[i])
            return 1;
    
    return 0;
}


bool move_mat (MAT mat, char way)
{
    MAT prev_mat;
    check_diffs(mat, prev_mat);
    
    int vcol[N];
    for (int e=0; e<N; e++)
    {
        switch (way)
        {
            case MOVE_LEFT:
                move_l_vec(mat[e]);
                break;
                
            case MOVE_RIGHT:
                move_r_vec(mat[e]);
                break;
                
            case MOVE_UP:
                extract_col_mat(vcol, mat, e);
                move_l_vec(vcol);
                insert_col_mat(vcol, mat, e);
                break;
                
            case MOVE_DOWN:
                extract_col_mat(vcol, mat, e);
                move_r_vec(vcol);
                insert_col_mat(vcol, mat, e);
                break;
        }
    }
    return check_diffs(mat, prev_mat);
}


/* this was got from stckoverflow,
thanks to users mf_ anf falcon-momot */
char getch (void)
{
    char buf= 0;
    struct termios old= {0};
    if (tcgetattr(0, &old) < 0)
        perror("tcsetattr()");
    old.c_lflag &= ~ICANON;
    old.c_lflag &= ~ECHO;
    old.c_cc[VMIN]= 1;
    old.c_cc[VTIME]= 0;
    if (tcsetattr(0, TCSANOW, &old) < 0)
        perror("tcsetattr ICANON");
    if (read(0, &buf, 1) < 0)
        perror("read()");
    old.c_lflag |= ICANON;
    old.c_lflag |= ECHO;
    if (tcsetattr(0, TCSADRAIN, &old) < 0)
        perror("tcsetattr ~ICANON");
    return buf;
}


int main (void)
{
    srand(time(0));
    
    MAT matrix;
    init_mat(matrix);
     
    char opt= 0;
    
    printf(CLEAR_SCR);
    print_mat(matrix);
    printf("Press Q to exit when you've finished\n");
    
    while (opt!=QUIT)
    {
        do  //do not take invalid inputs
            opt= getch();
        while (! check_stdin(opt));
        
        if (move_mat(matrix, opt))
        {
            ninsert_mat(matrix);
            printf(CLEAR_SCR);
            print_mat(matrix);
        }
    }
    
    if (NMOVES)
        printf("Have a good day :)\n");
    
    return 0;
}