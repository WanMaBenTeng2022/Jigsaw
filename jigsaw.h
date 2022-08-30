#include "stdio.h"
#include "time.h"
#include "stdlib.h"
#include "stdbool.h"

#define Map_Width  4
#define Map_Height 4

int map[Map_Height][Map_Width];                   //定义一个4x4数组储存方块位置信息

void InitMap(){                                   //初始化数组 即：  0， 1， 2  3，
	int (*p)[Map_Width] = map;                     //                4， 5， 6， 7，
   for(int i=0; i<Map_Width*Map_Height; i++)      //                8， 9， 10，11，
        *(*p+i) = i;                              //                12，13，14，15
}

void PrintMap(){                                 //打印数组 即用于调试时候用的
	 for(int j=0;j<Map_Height;j++){
        for(int i=0;i<Map_Width;i++){
	        printf("%3d ",map[j][i]);
	    }
	    printf("\n");
     }	
}

void FindMaxNumberPosition(int *x, int *y){       //从数组里找出最大的数 ， 15 用于做空格 
	int Max = Map_Height*Map_Width-1;              //找出15 在数组里的位置，即第几行第几列
	
	 for(int j=0; j<Map_Height; j++){
        for(int i=0; i<Map_Width; i++){
	        if(map[j][i] == Max){
	          printf("x: %d, y: %d",i,j);
	          *x = i;
	          *y = j;
	       }   
        }	
     }
}


bool IsFinished(){                              //判断是不是所有方块都完成排序  即完成游戏
	 int nCorrect = 0;                           //根据数组0-15排列
	 int (*p)[Map_Width] = map;

    for(int i=0; i<Map_Width*Map_Height; i++){
	            if((*p)[i] == i)
	                nCorrect++;
	            else
	                break;
     }	
    
     if(nCorrect == Map_Height*Map_Width)
         return true;
     
     return false;
}


bool MoveLeft(int *x, int *y){                  //左移动 用15和旁边的数 进行对换 
	    //Left
	   if((*x+1) < Map_Width){
	      int tmp = map[*y][*x];
	      map[*y][*x]=map[*y][*x+1];
	      map[*y][*x+1]=tmp;
	      (*x)++;
	   }
      //printf("Left...\n");
   return true;
}

bool MoveRight(int *x, int *y){               //右移动 用15和旁边的数 进行对换 
	   //Right
	   if((*x-1) >= 0){
	      int tmp = map[*y][*x];
	      map[*y][*x]=map[*y][*x-1];
	      map[*y][*x-1]=tmp;
	      (*x)--;
	   }
      //printf("Right...\n");
   return true;
}
	
bool MoveUp(int *x, int *y){                //原理同上
       //Up
	   if((*y+1) < Map_Height){
	      int tmp = map[*y][*x];
	      map[*y][*x]=map[*y+1][*x];
	      map[*y+1][*x]=tmp;
	      (*y)++;
	   }
      //printf("Up...\n");
   return true;
}
	
bool MoveDown(int *x, int *y){              //原理同上
	   //Down
	   if((*y-1) >= 0){
	      int tmp = map[*y][*x];
	      map[*y][*x]=map[*y-1][*x];
	      map[*y-1][*x]=tmp;
	      (*y)--;
	   }
      //printf("Down...\n");
   return true;
}

//**************************************************
bool TestMoveLeft(int *x, int *y){                  //测试移动是不是会越出数组范围
     //printf("Left\n");
     if((*x+1) < Map_Width)
        return true;
     return false;
}

bool TestMoveRight(int *x, int *y){               //同上
     //printf("Right\n"); 
     if((*x-1) >= 0)
        return true;
     return false;
}


bool TestMoveUp(int *x, int *y){                //同上
     //printf("Up\n");
     if((*y+1) < Map_Height)
        return true;
     return false;
}


bool TestMoveDown(int *x, int *y){             //同上
     //printf("Down");
     if((*y-1) >= 0)
        return true;
     return false;
}


typedef bool (*pMove)(int *x, int *y);

bool bMoveable[4]={0,0,0,0};
pMove TestMoveFunctionArray[4] = {TestMoveLeft, TestMoveRight, TestMoveUp, TestMoveDown};
pMove (*pArrTest)[4] = &TestMoveFunctionArray;                      //数组指针用于方便调用对应在数组里函数  , 测试有没有越界数组

pMove MoveFunctionArray[4] = {MoveLeft, MoveRight, MoveUp, MoveDown}; 
pMove (*pArrMove)[4] = &MoveFunctionArray;                          //数组指针用于方便调用对应在数组里函数  ，实际执行方块移动

int recordPreviousStep = -1;            //记录上一次移动的方向

//先测试有哪个方向可以移动
//如果上一次执行了Left 下一次就不执行 Right
//如果上一次执行了Up   下一次就不执行 Down

void Moveable_Directions(int *Space_x, int *Space_y){
    
     for(int i=0; i<4; i++)
         bMoveable[i] = (*(*pArrTest)[i])(Space_x, Space_y);       //检查有哪个方向可以移动， 可以移动的返回1，保存到 bMoveable[] 数组里

     if(recordPreviousStep != -1){                
        switch (recordPreviousStep){
        case 0:
          bMoveable[1] = 0;                 //上次执行了 MoveLeft  那么下一次就不执行 MoveRight  ，为了避免发生方块往回的方向移动
          break;
        case 1:
          bMoveable[0] = 0;                //上次执行了 MoveRight  ... ... 同上 反过来不执行 MoveLeft
          break;
        case 2:                            // MoveUp ... ...
          bMoveable[3] = 0;
          break;
        case 3:                            // MoveDown ... ...
          bMoveable[2] = 0;
          break;
        
        default:
         break;
        }
     }
     
     int arrDirection[4] = {0,0,0,0};      //把可以移动的方向保存到 arrDirection[]， 当索引调用 MoveLeft，MoveRight，MoveUp，MoveDown
     int nWay = 0;                         //bMoveable[] = {0,1,0,1};  （MoveLeft =0，MoveRight =1，MoveUp =0，MoveDown =1） 
     for(int i=0; i<4; i++){               //转到 arrDirection[] = {1,3,0,0} 
                                           //也就是第 1，和第3个 函数可以调用（0-3）； 1 是对应 MoveRight ，  3 是对应 MoveDown   
         if(bMoveable[i] == true){
             arrDirection[nWay] = i;
             nWay++;
         }
     }

         int random = 0;
         if(nWay > 0){
            random = rand()%nWay;             //随机移动方向
         }
        
         (*(*pArrMove)[arrDirection[random]])(Space_x, Space_y);     //执行移动
         recordPreviousStep = arrDirection[random];                  //记录上一次移动的方向
} 

