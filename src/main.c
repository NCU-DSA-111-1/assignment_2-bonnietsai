// HW3 shogi
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include "libev/ev.h"
#include "function.h"


int main(int argc, char *argv[])
{
    struct ev_loop *loop = EV_DEFAULT;
    now=ev_now(loop);

    struct piece board[BWEIGHT][BHEIGHT];    
    struct stack all_chess_stack , *all_chess_stack_Ptr ;
    all_chess_stack_Ptr = &all_chess_stack;
    all_chess_stack_Ptr->top = -1;

//////////////////////////HW3修改之地方/////////////////////////////////////

    MOVE_Linked_List *chess_move_Ptr ;
    chess_move_Ptr = NULL;

///////////////////////////////////////////////////////////////////////////
    board_initial(board);
    Push(all_chess_stack_Ptr,board);
    board_show(board);
    
    int before_x ,before_y ,after_x ,after_y;    
    int temp_attacker=1;
    int temp;

    ev_io_init(&stdin_watcher, stdin_cb, /*STDIN_FILENO*/ 0, EV_READ);
	ev_io_start(loop, &stdin_watcher);
	ev_timer_init(&timeout_watcherx, timeout_cbx, 0, 1);
    ev_timer_init(&timeout_watchery, timeout_cby, 0, 1);

    if(argc == 4 && strcmp(argv[1],"-n")==0 && strcmp(argv[2],"-s")==0)
    {
        while(1)
        {
            int recover=1;
            do
            {
                printf("Recover step input [0] | continue move input [1]  | Save the chess [2] = ");
                scanf(" %d", &recover);

                if( recover==1 || recover==2 )
                    break;
                if(recover==0){
                    Pop(all_chess_stack_Ptr,board);
                    chess_move_Ptr = Pop_move(chess_move_Ptr); //HW3修改之地方
                    board_show(board); 
                    temp=attacker;
                    attacker=temp_attacker;
                    temp_attacker=temp;
                }
            } while ( recover==0 | recover!=1 | recover!=2 );

            if(recover==2){
                FILE *fptr;
                fptr = fopen("The_MOVE.txt","w");
                printf("The_MOVE.txt is open");
                Write(fptr ,chess_move_Ptr,all_chess_stack_Ptr);
                fclose(fptr);
                break;
            }
        
            printf("\n*************** 目前進攻方為 %d (-1為Blue,1為Red) ***************\n ", attacker);
            printf("請輸入移動棋子之[段] = ");       scanf(" %d", &before_x);
            printf(" 請輸入移動棋子之[筋] = ");      scanf(" %d", &before_y);
            printf(" 請輸入欲移動位置之[段] = ");    scanf(" %d", &after_x);
            printf(" 請輸入欲移動位置之[筋] = ");    scanf(" %d", &after_y);

            ////////////////////////////////////timer////////////////////////////////////////////////////////
            if(attacker==-1)
                ev_timer_start(loop, &timeout_watcherx);
            else
                ev_timer_start(loop, &timeout_watchery);
            ev_run(loop, 0);
            ////////////////////////////////////timer/////////////////////////////////////////////////////////

            transfer(&before_x,&before_y,&after_x,&after_y);
            if(  legal_position(board, before_x , before_y,  after_x,  after_y,  attacker)  ){

                printf("\n Correct Move (^-^) !!!\n");
                Push(all_chess_stack_Ptr,board);

                //HW3修改之地方
                chess_move_Ptr = Push_move(chess_move_Ptr,9-before_x,before_y+1,9-after_x,after_y+1);

            }else{
                temp=attacker;
                attacker=temp_attacker;
                temp_attacker=temp;
                printf(" Incorrect Move (T^T) !!!\n");
            }
            board_show(board);         
            temp=attacker;
            attacker=temp_attacker;
            temp_attacker=temp;
        }    
        ev_timer_stop (loop, &timeout_watcherx);
        ev_timer_stop (loop, &timeout_watchery);
    }  
    /****************************end NEW_GAME ***************************/
    if(argc == 3 && strcmp(argv[1],"-l")==0 )
    {
        printf("*********Read File*********\n");

        MOVE_Linked_List *read_move_Ptr;
        read_move_Ptr = NULL;
        int move[MOVESIZE];
        int top;
        
        FILE *fp;
        fp = fopen("The_MOVE.txt", "r");
        fscanf(fp, "%d", &top );

        for(int i = 0 ; i <= top ; i++ ){  
            for(int k = 0 ; k < MOVESIZE ; k++){  
                fscanf(fp, "%d", &move[k]);
            }
            read_move_Ptr = Push_move(read_move_Ptr,move[0],move[1],move[2],move[3]);
        }
        fclose(fp);

        int take = -1 ;
        char control;
        while(1)
        {
            printf("*********************************************************\n");
            printf("輸入[f]移動下一手 | 輸入[b]退回上一手 | 輸入[e]結束程式 = ");
            scanf(" %c", &control);
            
            
            if(control=='e')
                break;
            if( control=='f' && all_chess_stack_Ptr->top <= top   ){ // 輸入 ‘f’ 移動下一手，更新棋盤狀態

                //HW3修改之地方
                take=get_move( read_move_Ptr,&before_x,&before_y,&after_x,&after_y,take);
                transfer(&before_x,&before_y,&after_x,&after_y);
                legal_position(board, before_x , before_y,  after_x,  after_y,  attacker);
                Push(all_chess_stack_Ptr,board);
                board_show(board);         

                temp=attacker;
                attacker=temp_attacker;
                temp_attacker=temp;

            }else if( control=='b' && all_chess_stack_Ptr->top >= 0 ){ // 輸入 ‘b’ 退回上一手，更新棋盤狀態

                Pop(all_chess_stack_Ptr,board);
                take=return_move(read_move_Ptr,take);//HW3修改之地方
                board_show(board); 
                temp=attacker;
                attacker=temp_attacker;
                temp_attacker=temp;

            }else 
                board_show(board);
        } //end while(1)
    } /************************end old_GAME **********************/
    return 0;
}