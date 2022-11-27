// HW3 shogi
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include "libev/ev.h"

/*      timer  begin      */
int attacker = -1;
int indep=0;
int turns=0;
int past_time=0;
int now;
int c=0;
ev_io stdin_watcher;
ev_timer timeout_watcherx;
ev_timer timeout_watchery;

/*        constant        */
#define BWEIGHT 9
#define BHEIGHT 9
#define NAMESIZE 4
#define STRLENGTH 5
#define STACKSIZE 100
#define MOVESIZE 4

static void stdin_cb (EV_P_ ev_io *w, int revents){
    ev_timer_stop (loop, &timeout_watcherx);	
    ev_timer_stop (loop, &timeout_watchery);
	if(attacker==1){	
		ev_timer_stop (loop, &timeout_watcherx);	
		ev_timer_start (loop, &timeout_watchery);
	}
	else{
		ev_timer_stop (loop, &timeout_watchery);
		ev_timer_start (loop, &timeout_watcherx);
	}
}
static void timeout_cbx (EV_P_ ev_timer *w, int revents){

    if(turns==0){
        printf("The time of player BLUE use: ");
        printf("\033[43m %d second \033[m\n",(int)ev_now(loop)-now);
        indep=(int)ev_now(loop)-now;
    }else{
        printf("The time of player BLUE use: ");printf("\033[43m %d second \033[m\n",(int)ev_now(loop)-now-indep);
        indep=(int)ev_now(loop)-now;
    }
    ev_timer_stop (loop, &timeout_watcherx);
    ev_break(loop, EVBREAK_ONE);
    turns=1;

}
static void timeout_cby (EV_P_ ev_timer *w, int revents){

    printf("The time of player RED use: ");
    printf("\033[43m %d second \033[m\n",(int)ev_now(loop)-now-indep);
    indep=(int)ev_now(loop)-now;
    ev_timer_stop (loop, &timeout_watchery);
    ev_break(loop, EVBREAK_ONE);
    turns=-1;

}

/*.....................................Data Structure...................................*/
struct piece {
    char name[NAMESIZE];
    char controller[STRLENGTH];
};

struct stack{
    int top;
    struct piece one_move_chess[STACKSIZE][BWEIGHT][BHEIGHT];
};


struct MOVE_STACK{
    int top;
    int move_top;
    int move_stack[STACKSIZE][MOVESIZE] ;
};


struct MOVE_Linked_List{//HW3修改之地方

    int MOVE_Linked_List[MOVESIZE] ;
    struct MOVE_Linked_List *next ;

};
typedef struct MOVE_Linked_List MOVE_Linked_List;

/*.........................................function......................................*/

void board_initial(struct piece board[][BHEIGHT])
{
    for (int i = 0; i < BWEIGHT; i++)
    {
        for (int j = 0; j < BHEIGHT; j++)
        {
            if (j == 2 || j == 6)
                strcpy((*(board + i) + j)->name, "步");
            else if ((i == 1 && j == 7) || (i == 7 && j == 1))
                strcpy((*(board + i) + j)->name, "角");
            else if ((i == 1 && j == 1) || (i == 7 && j == 7))
                strcpy((*(board + i) + j)->name, "飛");
            else if ((i == 0 && j == 0) || (i == 8 && j == 0) || (i == 0 && j == 8) || (i == 8 && j == 8))
                strcpy((*(board + i) + j)->name, "香");
            else if ((i == 1 && j == 0) || (i == 7 && j == 0) || (i == 1 && j == 8) || (i == 7 && j == 8))
                strcpy((*(board + i) + j)->name, "桂");
            else if ((i == 2 && j == 0) || (i == 6 && j == 0) || (i == 2 && j == 8) || (i == 6 && j == 8))
                strcpy((*(board + i) + j)->name, "銀");
            else if ((i == 3 && j == 0) || (i == 5 && j == 0) || (i == 3 && j == 8) || (i == 5 && j == 8))
                strcpy((*(board + i) + j)->name, "金");
            else if ((i == 4 && j == 0) || (i == 4 && j == 8))
                strcpy((*(board + i) + j)->name, "王");
            else
                strcpy((*(board + i) + j)->name, "  ");
        }
    }

    // controller
    for (int i = 0; i < BWEIGHT; i++)
    {
        for (int j = 0; j < BHEIGHT; j++)
        {
            if (j == 0 || (i == 1 && j == 1) || (i == 7 && j == 1) || j == 2)
                strcpy((*(board + i) + j)->controller, "Red");
            else if (j == 6 || (i == 1 && j == 7) || (i == 7 && j == 7) || j == 8)
                strcpy((*(board + i) + j)->controller, "Blue");
            else
                strcpy((*(board + i) + j)->controller, "Free");
        }
    }
}

void board_show(struct piece (*board)[BHEIGHT])
{   
    printf("9 8 7 6 5 4 3 2 1\n");
    for (int j = 0; j < BHEIGHT; j++)
    {
        for (int i = 0; i <= BWEIGHT; i++)
        {
            if (i == BWEIGHT)
                printf("%d\n", j + 1);
            else
            {
                if (strcmp((*(board + i) + j)->controller, "Red") == 0)
                    printf("\033[31m%s\033[m", (*(board + i) + j)->name);
                else if (strcmp((*(board + i) + j)->controller, "Blue") == 0)
                    printf("\033[34m%s\033[m", (*(board + i) + j)->name);
                else
                    printf("%s", (*(board + i) + j)->name);
            }
        }
    }
}

void swap(struct piece *A,struct piece *B)
{
    struct piece temp = *A;
    *A = *B;
    *B = temp;
}

int chess_bu (struct piece chess[][BHEIGHT] ,int before_x ,int before_y, int after_x, int after_y)
{
    if( after_x==before_x && before_y-after_y==1 && strcmp(chess[before_x][before_y].controller,"Blue")==0 
        && strcmp(chess[after_x][after_y].name,"  ")==0     )
        return 1;
    else if( after_x==before_x && after_y-before_y==1 && strcmp(chess[before_x][before_y].controller,"Red")==0 
              && strcmp(chess[after_x][after_y].name,"  ")==0                   )
        return 1;
    else        
        return 0;
}

int chess_Xiang(struct piece chess[][BHEIGHT] ,int before_x ,int before_y, int after_x, int after_y)
{
    int chess_stop=0;
    if( after_x==before_x && before_y-after_y>=1 
        && strcmp(chess[before_x][before_y].controller,"Blue")==0 
        && strcmp(chess[after_x][after_y].name,"  ")==0     )
    {   
        for(int i = before_y -1 ; i > after_y ; i-- )
        {
            if(strcmp(chess[after_x][i].name,"  ")==0)
                chess_stop=0;//沒被擋住
            else
            {
                    chess_stop=1;//被擋住了
                    break;
            }
        }
        
        if(chess_stop==0)
            return 1; //移動成功
        else
            return 0; //無效移動
        
    }else 
    if( after_x==before_x && after_y-before_y>=1 && strcmp(chess[before_x][before_y].controller,"Red")==0 
        && strcmp(chess[after_x][after_y].name,"  ")==0 )
    {
        for(int i = before_y + 1 ; i < after_y ; i++ ){
            if(strcmp(chess[after_x][i].name,"  ")==0)
                chess_stop=0;//沒被擋住
            else{
                chess_stop=1;//被擋住了
                break;
            }
        }
        
        if(chess_stop==0)
            return 1; //移動成功
        else
            return 0; //無效移動

    }else
        return 0;
}

int chess_Gui (struct piece chess[][BHEIGHT] ,int before_x ,int before_y, int after_x, int after_y)
{
    if( abs(after_x-before_x)==1 && before_y-after_y==2 && strcmp(chess[before_x][before_y].controller,"Blue")==0 
        && strcmp(chess[after_x][after_y].name,"  ")==0     )
        return 1;
    else if( abs(after_x-before_x)==1 && after_y-before_y==2 && strcmp(chess[before_x][before_y].controller,"Red")==0 
              && strcmp(chess[after_x][after_y].name,"  ")==0                   )
        return 1;
    else        
        return 0;
}

int chess_Yin(struct piece chess[][BHEIGHT] ,int before_x ,int before_y, int after_x, int after_y)
{
    if( strcmp(chess[before_x][before_y].controller,"Blue")==0 
        && strcmp(chess[after_x][after_y].name,"  ")==0     )
    {
        if( abs(after_x-before_x)==1 && abs(after_y-before_y)==1  ) 
            return 1;
        else if ( before_x==after_x && before_y-after_y==1  )
            return 1;
        else
            return 0;

    }else if( strcmp(chess[before_x][before_y].controller,"Red")==0 
              && strcmp(chess[after_x][after_y].name,"  ")==0 )
    {
        if( abs(after_x-before_x)==1 && abs(after_y-before_y)==1  ) 
            return 1;
        else if ( before_x==after_x && before_y-after_y==-1  )
            return 1;
        else
            return 0;
    }else        
        return 0;
}

int chess_Gin(struct piece chess[][BHEIGHT] ,int before_x ,int before_y, int after_x, int after_y)
{
    if( ( abs(after_x-before_x)==1 || abs(after_y-before_y)==1 )&&
        abs(after_x-before_x)<=1 && abs(after_y-before_y)<=1 &&
        strcmp(chess[before_x][before_y].controller,"Blue")==0 
        && strcmp(chess[after_x][after_y].name,"  ")==0         )
    {
        if( after_x-before_x ==1 && after_y-before_y == 1  ) 
            return 0;
        else if ( after_x-before_x == -1 && after_y-before_y == 1 )
            return 0;
        else 
            return 1;
            
    }else if( ( abs(after_x-before_x)==1 || abs(after_y-before_y)==1 )&&
              abs(after_x-before_x)<=1 && abs(after_y-before_y)<=1 &&
              strcmp(chess[before_x][before_y].controller,"Red")==0 
              && strcmp(chess[after_x][after_y].name,"  ")==0         )
    {
        if( after_x-before_x ==1 && after_y-before_y == -1  ) 
            return 0;
        else if ( after_x-before_x == -1 && after_y-before_y == -1 )
            return 0;
        else 
            return 1;
    }else        
        return 0;
}

int chess_Wang(struct piece chess[][BHEIGHT] ,int before_x ,int before_y, int after_x, int after_y)
{
    if( ( abs(after_x-before_x)==1 || abs(after_y-before_y)==1 )&&
        abs(after_x-before_x)<=1 && abs(after_y-before_y)<=1 &&
        strcmp(chess[before_x][before_y].controller,"Blue")==0 
        && strcmp(chess[after_x][after_y].name,"  ")==0     )
            return 1;
    else if( ( abs(after_x-before_x)==1 || abs(after_y-before_y)==1 )&&
              abs(after_x-before_x)<=1 && abs(after_y-before_y)<=1 &&
              strcmp(chess[before_x][before_y].controller,"Red")==0 
              && strcmp(chess[after_x][after_y].name,"  ")==0        )
            return 1;
    else        
            return 0;
}

int chess_Jiao(struct piece chess[][BHEIGHT] ,int before_x ,int before_y, int after_x, int after_y)
{
    int chess_stop=0;
    if( abs(after_x-before_x) == abs(before_y-after_y) 
        && strcmp(chess[after_x][after_y].name,"  ")==0   )
    {   
        if( after_x-before_x<0 && after_y-before_y<0 )
        {
            for(int i=before_x-1,  j=before_y-1 ; after_x < i ; i-- , j-- )
            {        
                if(strcmp(chess[i][j].name,"  ")==0)
                    chess_stop=0;
                else{
                    chess_stop=1;//被擋住了
                    break;
                }
            }
        }else if( after_x-before_x>0 && after_y-before_y<0 )
        {
            for(int i=before_x+1, j=before_y-1 ; after_x > i ;  i++, j-- )
            {   
                if(strcmp(chess[i][j].name,"  ")==0)
                    chess_stop=0;
                else{   
                    chess_stop=1;//被擋住了
                    break;
                }
            }
        }else if( after_x-before_x<0 && after_y-before_y>0 )
        {
            for(int i=before_x-1, j=before_y+1 ; after_x < i ; i--,j++ )
            {
                if(strcmp(chess[i][j].name,"  ")==0)
                    chess_stop=0;
                else{
                    chess_stop=1;//被擋住了
                    break;
                }
            }
        }else //if( after_x-before_x>0 && after_y-before_y>0 )
        {
            for(int i=before_x+1, j=before_y+1 ; after_x > i  ; i++,j++ )
            {
                if(strcmp(chess[i][j].name,"  ")==0)
                    chess_stop=0;
                else{
                    chess_stop=1;//被擋住了
                    break;
                }
            }
        }
        if(chess_stop==0)
            return 1; //移動成功
        else
            return 0; //無效移動    
    }
    else 
        return 0;
}

int chess_Fei(struct piece chess[][BHEIGHT] ,int before_x ,int before_y, int after_x, int after_y)
{   
    int chess_stop=0;
    if( (after_x==before_x || before_y==after_y)
        && strcmp(chess[after_x][after_y].name,"  ")==0 )
    {   
        if( after_y-before_y < 0 )
        {
            for(int i = before_y - 1 ; i > after_y ; i-- )
            {
                if(strcmp(chess[before_x][i].name,"  ")==0)
                    chess_stop=0;//沒被擋住
                else{
                    chess_stop=1;//被擋住了
                    break;
                }
            }
        }else if( after_y-before_y > 0 )
        {
            for(int i = before_y + 1 ; i < after_y ; i++ )
            {
                if(strcmp(chess[before_x][i].name,"  ")==0)
                    chess_stop=0;//沒被擋住
                else{
                    chess_stop=1;//被擋住了
                    break;
                }
            }
        }else if( after_x-before_x < 0 )
        {
            for(int i = before_x - 1 ; i > after_x ; i-- )
            {
                if(strcmp(chess[i][before_y].name,"  ")==0)
                    chess_stop=0;//沒被擋住
                else{
                    chess_stop=1;//被擋住了
                    break;
                }
            }
        }else //if( after_x-before_x > 0 )
        {
            for(int i = before_x + 1 ; i < after_x ; i++ )
            {
                if(strcmp(chess[i][before_y].name,"  ")==0)
                    chess_stop=0;//沒被擋住
                else{
                    chess_stop=1;//被擋住了
                    break;
                }
            }
        }
        
        if(chess_stop==0)
            return 1; //移動成功
        else
            return 0; //無效移動
    }else       
        return 0;
}

int move_chess_type(struct piece chess[][BHEIGHT] ,int before_x ,int before_y){

    if( strcmp(chess[before_x][before_y].name,"步")==0 )
        return 1;
    else if( strcmp(chess[before_x][before_y].name,"香")==0 )
        return 2;
    else if( strcmp(chess[before_x][before_y].name,"桂")==0 )
        return 3;
    else if( strcmp(chess[before_x][before_y].name,"銀")==0 )
        return 4;
    else if( strcmp(chess[before_x][before_y].name,"金")==0 )
        return 5;
    else if( strcmp(chess[before_x][before_y].name,"王")==0 )
        return 6;
    else if( strcmp(chess[before_x][before_y].name,"角")==0 )
        return 7;
    else if( strcmp(chess[before_x][before_y].name,"飛")==0 )
        return 8;
    else 
        return 0;
}

int eat(struct piece chess[][BHEIGHT] ,int before_x ,int before_y, int after_x, int after_y)//移動位置為敵方位置
{
                struct piece temp;   
                strcpy(temp.controller , chess[after_x][after_y].controller );
                strcpy(temp.name       , chess[after_x][after_y].name );

                strcpy(chess[after_x][after_y].controller,"Free" );
                strcpy(chess[after_x][after_y].name      , "  "  );
                int condution=0;

                switch (  move_chess_type( chess , before_x , before_y)  ) 
                {

                    case 1:  if( chess_bu (chess , before_x , before_y, after_x, after_y) )
                             {      
                                    swap(&chess[before_x][before_y],&chess[after_x][after_y]);
                                    condution=1;
                             }else 
                                    condution=0;
                             break;
                    case 2:  if( chess_Xiang (chess , before_x , before_y, after_x, after_y) )
                             {      
                                    swap(&chess[before_x][before_y],&chess[after_x][after_y]);
                                    condution=1;
                             }else 
                                    condution=0;
                             break;
                    case 3:  if( chess_Gui (chess , before_x , before_y, after_x, after_y) )
                             {      
                                    swap(&chess[before_x][before_y],&chess[after_x][after_y]);
                                    condution=1;
                             }else 
                                    condution=0;
                             break;
                    case 4:  if( chess_Yin (chess , before_x , before_y, after_x, after_y) )
                             {      
                                    swap(&chess[before_x][before_y],&chess[after_x][after_y]);
                                    condution=1;
                             }else 
                                    condution=0;
                             break;
                    case 5:  if( chess_Gin (chess , before_x , before_y, after_x, after_y) )
                             {      
                                    swap(&chess[before_x][before_y],&chess[after_x][after_y]);
                                    condution=1;
                             }else 
                                    condution=0;
                             break;

                    case 6:  if( chess_Wang (chess , before_x , before_y, after_x, after_y) )
                             {     
                                    swap(&chess[before_x][before_y],&chess[after_x][after_y]);
                                    condution=1;
                             }else 
                                    condution=0;
                             break;

                    case 7:  if( chess_Jiao (chess , before_x , before_y, after_x, after_y) )
                             {      
                                    swap(&chess[before_x][before_y],&chess[after_x][after_y]);
                                    condution=1;
                             }else 
                                    condution=0;
                             break;
                    case 8:  if( chess_Fei (chess , before_x , before_y, after_x, after_y) )
                             {
                                    swap(&chess[before_x][before_y],&chess[after_x][after_y]);
                                    condution=1;
                             }else 
                                    condution=0; 
                             break;
                }    

                if(condution==1)
                    return 1;
                else{
                    strcpy(chess[after_x][after_y].controller , temp.controller );
                    strcpy(chess[after_x][after_y].name       , temp.name );     
                    return 0;               
                }

}

int move_chess(struct piece chess[][BHEIGHT] ,int before_x ,int before_y, int after_x, int after_y)//移動位置不為敵方位置
{
    switch (  move_chess_type( chess , before_x , before_y)  ) 
    {
                    case 1:  if( chess_bu (chess , before_x , before_y, after_x, after_y) )
                             {
                                    swap(&chess[before_x][before_y],&chess[after_x][after_y]);
                                    return 1;
                             }else 
                                    return 0;
                             break;
                    case 2:  if( chess_Xiang (chess , before_x , before_y, after_x, after_y) )
                             {
                                    swap(&chess[before_x][before_y],&chess[after_x][after_y]);
                                    return 1;
                             }else 
                                    return 0;
                             break;
                    case 3:  if( chess_Gui (chess , before_x , before_y, after_x, after_y) )
                             {
                                    swap(&chess[before_x][before_y],&chess[after_x][after_y]);
                                    return 1;
                             }else 
                                    return 0;
                             break;

                    case 4:  if( chess_Yin (chess , before_x , before_y, after_x, after_y) )
                             {
                                    swap(&chess[before_x][before_y],&chess[after_x][after_y]);
                                    return 1;
                             }else 
                                    return 0;
                             break;
                             
                    case 5:  if( chess_Gin (chess , before_x , before_y, after_x, after_y) )
                             {
                                    swap(&chess[before_x][before_y],&chess[after_x][after_y]);
                                    return 1;
                             }else 
                                    return 0;
                             break;
                             
                    case 6:  if( chess_Wang (chess , before_x , before_y, after_x, after_y) )
                             {
                                    swap(&chess[before_x][before_y],&chess[after_x][after_y]);
                                    return 1;
                             }else 
                                    return 0;
                             break;
                             
                    case 7:  if( chess_Jiao (chess , before_x , before_y, after_x, after_y) )
                             {      
                                    //printf("move_chess function call case 7 \n");
                                    swap(&chess[before_x][before_y],&chess[after_x][after_y]);
                                    return 1;
                             }else 
                                    return 0;
                             break;
                             
                    case 8:  if( chess_Fei (chess , before_x , before_y, after_x, after_y) )
                             {         
                                    //printf("move_chess function call case 8 \n");
                                    swap(&chess[before_x][before_y],&chess[after_x][after_y]);
                                    return 1;
                             }else 
                                    return 0;
                             break;               
    }   
    return 0;
}

int legal_position(struct piece chess[][BHEIGHT] ,int before_x ,int before_y, int after_x, int after_y, int attacker)
{
    if( after_x>=0 && after_x<=8 && after_y>=0 && after_y<=8 )
    {   
        if( strcmp(chess[before_x][before_y].controller,"Blue")==0 && attacker==-1 )//if attacker=Blue
        {   
            if( strcmp(chess[after_x][after_y].controller,"Red")==0) //移動後位置為敵方所在位置
            {   
                if( eat(chess, before_x , before_y,  after_x,  after_y) )
                    return 1;
                else
                    return 0;
            }else { //移動後位置不為敵方所在位置
                if(move_chess(chess, before_x , before_y,  after_x,  after_y))
                    return 1;
                else
                    return 0;
            }
        }else if( strcmp(chess[before_x][before_y].controller,"Red")==0 && attacker==1 )//if attacker=Red
        {   
            if( strcmp(chess[after_x][after_y].controller,"Blue")==0) //移動後位置為敵方所在位置
            {   
                if( eat(chess, before_x , before_y,  after_x,  after_y) )
                    return 1;
                else
                    return 0;
            }else { //移動後位置不為敵方所在位置
                if(move_chess(chess, before_x , before_y,  after_x,  after_y))
                    return 1;
                else
                    return 0;
            }
        }else
            return 0;
    }else 
        return 0;
}

void transfer(int *before_x, int *before_y,int *after_x,int *after_y)
{
    *before_x= 9-(*before_x);
    *after_x = 9-(*after_x ); 
    *before_y= (*before_y)-1;
    *after_y = (*after_y)-1 ;
}

int Overflow(struct stack *all_chess_stack_Ptr)
{
    if(all_chess_stack_Ptr->top==STACKSIZE-1)
        return 1;
    else
        return 0;
}

int Empty(struct stack *all_chess_stack_Ptr){

    if(all_chess_stack_Ptr-> top == 0)
        return 1;
    else
        return 0;
}

int Push(struct stack *all_chess_stack_Ptr,struct piece chess[][BHEIGHT])
{
    if(!Overflow(all_chess_stack_Ptr)){

        (all_chess_stack_Ptr->top)++;

        for(int i=0 ; i<9 ; i++)
        {
            for(int j=0 ; j<9 ;j++)
            {   
                strcpy((all_chess_stack_Ptr->one_move_chess[all_chess_stack_Ptr->top][i][j]).name, chess[i][j].name);
                strcpy((all_chess_stack_Ptr->one_move_chess[all_chess_stack_Ptr->top][i][j]).controller, chess[i][j].controller);
            }
        }
        return 1;
    }else {
        printf("Stack is Overflow\n");
        return 0;
    }
}

int Pop(struct stack *all_chess_stack_Ptr,struct piece chess[][BHEIGHT])
{   
    if(Empty(all_chess_stack_Ptr)){
        printf("Chess Stack is Empty \n");
        return 0;
    } 
    else{
        (all_chess_stack_Ptr->top)--;

        for(int i=0 ; i<9 ; i++){
            for(int j=0 ; j<9 ;j++){   
                strcpy( chess[i][j].name, (all_chess_stack_Ptr->one_move_chess[all_chess_stack_Ptr->top][i][j]).name);
                strcpy( chess[i][j].controller, (all_chess_stack_Ptr->one_move_chess[all_chess_stack_Ptr->top][i][j]).controller);
            }
        }
        return 1;
    }
}

MOVE_Linked_List *Push_move(MOVE_Linked_List *chess_move_Ptr, int before_x, int before_y, int after_x, int after_y)
{//HW3修改之地方
        MOVE_Linked_List *tmp=(MOVE_Linked_List* )malloc(sizeof(MOVE_Linked_List));

        if( tmp!=NULL ){

            tmp->MOVE_Linked_List[0] = before_x;
            tmp->MOVE_Linked_List[1] = before_y;
            tmp->MOVE_Linked_List[2] = after_x;
            tmp->MOVE_Linked_List[3] = after_y;
            tmp->next = chess_move_Ptr;
            chess_move_Ptr = tmp;        
        }
        return chess_move_Ptr;
}

MOVE_Linked_List *Pop_move(MOVE_Linked_List *chess_move_Ptr)
{//HW3修改之地方

    MOVE_Linked_List *tmp;
    if( chess_move_Ptr == NULL )//stack沒有指到任何東西=沒資料
    {
        printf("Stack is empty");
        return (chess_move_Ptr);
    }
    tmp = chess_move_Ptr;
    chess_move_Ptr = chess_move_Ptr->next ;
    free(tmp) ;
    return(chess_move_Ptr);

}

void Write(FILE *fptr , MOVE_Linked_List * chess_move_Ptr, struct stack *all_chess_stack_Ptr )
{//HW3修改之地方
    MOVE_Linked_List *tmp = chess_move_Ptr;
    fprintf(fptr, "%d", all_chess_stack_Ptr->top-1);
    fprintf(fptr,"\n");

    while(tmp!=NULL)
    {
        for(int k = 0 ; k < MOVESIZE ; k++)
        {   
            fprintf(fptr, "%d", tmp->MOVE_Linked_List[k]);
            fprintf(fptr," ");
        }
        fprintf(fptr,"\n");
        tmp = tmp->next;
    }

    printf("File is Ready \n");
}

int get_move(MOVE_Linked_List *read_move_Ptr,int *read_x_b, int *read_y_b,int *read_x_a,int *read_y_a,int take)
{//HW3修改之地方
        
        if( read_move_Ptr == NULL )//stack沒有指到任何東西=沒資料
        {
            printf("Stack is empty");
            return take;
        }

        int counter=-1;
        while(counter < take){
            read_move_Ptr=read_move_Ptr->next;
            counter++;
        }

        *read_x_b=read_move_Ptr->MOVE_Linked_List[0];
        *read_y_b=read_move_Ptr->MOVE_Linked_List[1];
        *read_x_a=read_move_Ptr->MOVE_Linked_List[2];
        *read_y_a=read_move_Ptr->MOVE_Linked_List[3];
        take++;
        return take;

}

int return_move(MOVE_Linked_List *read_move_Ptr, int take){//HW3修改之地方

    if(take == -1)
    {
        printf("This is the first step \n");
        return take;

    }else {
        take--;
        return take;
    }
}
