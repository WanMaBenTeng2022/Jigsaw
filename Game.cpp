#include "jigsaw.h"
#include "SDL2/SDL.h"//编译找不到改成 SDL2/SDL2.h
#include "SDL2/SDL_image.h" //SDL2/SDL2_image.h
#include "Windows.h"

#undef main //SDL库带有 SDL_main 会冲突所以取消

#define Window_W 500
#define Window_H 700

#define IMG_GAMESTART   "png/start.png"
#define IMG_GAMEOVER    "png/over.png"
#define IMG_PICTURE     "png/picture.png"

SDL_Window   *window;
SDL_Renderer *render;
// 开始界面
SDL_Surface *surfaceStart;
SDL_Texture *textureStart;
// 背景图
SDL_Surface *surfacePicture;
SDL_Texture *texturePicture;
//结束画面
SDL_Surface *surfaceOver;
SDL_Texture *textureOver;
              	                                  
typedef enum {
	Game_Start,
	Game_Playing,
	Game_Over,
	}Game;
	
	
//加载游戏图片
int LoadImageForGame(){
   // 开始界面
   surfaceStart = IMG_Load(IMG_GAMESTART);
    if(surfaceStart == NULL){
       printf("surfaceStart load err");
       return -1;
     }
       
   textureStart = SDL_CreateTextureFromSurface(
                        render,surfaceStart);
    if(textureStart == NULL){
        printf("textureStart err");
        return -1;
     }
	
   // 背景图
   surfacePicture = IMG_Load(IMG_PICTURE);
   if(surfacePicture == NULL){
       printf("surfacePicture load err");
       return -1;
    }

    texturePicture = SDL_CreateTextureFromSurface(
                         render,surfacePicture);
    if(texturePicture == NULL){
        printf("texturePicture err");
        return -1;
    }
 
   //结束画面
   surfaceOver = IMG_Load(IMG_GAMEOVER);
    if(surfaceOver == NULL){
       printf("surfaceOver load err");
       return -1;
     }

   textureOver = SDL_CreateTextureFromSurface(
             render,surfaceOver);
    if(textureOver == NULL){
        printf("textureOver err");
		return -1;
     }
     
    return 0;
}
	

void Refresh(SDL_Rect (*p)[Map_Width]){
      SDL_RenderClear(render);
     //画每一小块拼图
     for(int j=0; j<Map_Height; j++){
        for(int i=0; i<Map_Width; i++){
             if(map[j][i]==Map_Width*Map_Height-1)
                continue;
                
             int yp = map[j][i] / Map_Width;
             int xp = map[j][i] % Map_Width;  
             SDL_RenderCopy(render, texturePicture, &p[yp][xp],  &p[j][i]);                   
        }
    }
}



int main(int argc, char *argv[])
{  
	bool bQuit = false;
    Game GameState=Game_Start;
    srand( (unsigned int)time(NULL) );

    //初始化SDL
    SDL_Init(SDL_INIT_EVERYTHING);

    //初始化窗口
    window = SDL_CreateWindow("拼图", 25, 25, Window_W, Window_H, SDL_WINDOW_SHOWN &SDL_WINDOW_FULLSCREEN_DESKTOP);
    if(window == NULL)
       return -1;
    //初始化渲染器
    render = SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED);
    if(render == NULL)
       return -1;
  
   if( LoadImageForGame() == -1)
       return -1;

    //百分比改变图片大小尺寸
    int picture_W = surfacePicture->w;
    int picture_H = surfacePicture->h;

    if(picture_H > 750){
       int percent = (picture_H - 750) *100 / picture_H;
       picture_W = picture_W - (picture_W * percent/100);
       picture_H = picture_H - (picture_H - 750); 
    }
       
    SDL_SetWindowSize(window, picture_W, picture_H);  //调整显示窗口大小


    //要呈现的背景画面区域
    SDL_Rect rectBG={0,0, picture_W, picture_H};

    SDL_Rect rectPicture[Map_Height][Map_Width];
    for(int j=0; j<Map_Height; j++){
        for(int i=0; i<Map_Width; i++){
            rectPicture[j][i].x = i*(picture_W/Map_Width);
            rectPicture[j][i].w = (picture_W/Map_Width)-6;
            rectPicture[j][i].y = j*(picture_H/Map_Height);
            rectPicture[j][i].h = (picture_H/Map_Height)-6;
        }
    }

   
    bool bMouseClick=false;//点击屏幕模拟触屏 
    float xMousePos, yMousePos;

    InitMap();
    PrintMap();
    int Space_xpos, Space_ypos;
    FindMaxNumberPosition(&Space_xpos, &Space_ypos);

    for(int i=0; i<100; i++)
        Moveable_Directions(&Space_xpos, &Space_ypos);

    SDL_Event ev;
    while(!bQuit)
    {    
    
       //处理键盘鼠标事件
        while(SDL_PollEvent(&ev))
        {
            switch(ev.type)
            {
                case SDL_KEYDOWN:
                    switch(ev.key.keysym.sym)
                    {
                     
                    	case SDLK_ESCAPE:
                 	         bQuit = true;
                             break;
                    	
                        case SDLK_DOWN:
                             if(GameState==Game_Playing)
                             MoveDown(&Space_xpos,&Space_ypos);
                             break;
                           	
                        case SDLK_UP:
                             if(GameState==Game_Playing)
                             MoveUp(&Space_xpos,&Space_ypos);
                             break;

                        case SDLK_LEFT:  
                             if(GameState==Game_Playing) 
                             MoveLeft(&Space_xpos,&Space_ypos);  
                             break;

                        case SDLK_RIGHT:  
                             if(GameState==Game_Playing)
                             MoveRight(&Space_xpos,&Space_ypos);  
                             break;
                        
                        case SDLK_RETURN:
                             break;
                         
                         case SDLK_SPACE:
                              break;
                    
                }//Switch KeyDown
                   break;

                case SDL_MOUSEBUTTONDOWN:
                   
             if(GameState==Game_Start){
                       bMouseClick=true;
             }else if(GameState==Game_Playing){
                       bMouseClick=true;
             }else if(GameState==Game_Over){
             	       GameState = Game_Start;
             	       //Restart game();
             }
              	break;
              	
              case SDL_FINGERMOTION:
                  	break;
              	
              
              case SDL_MOUSEBUTTONUP:
                	   
                     xMousePos = ev.motion.x;
                     yMousePos = ev.motion.y;
           
                	
     if(GameState == Game_Start){
         if(bMouseClick==true){
            bMouseClick=false;
    
            GameState = Game_Playing;
   
            }//mouse
     }// game start
              
              
    //playing page
    if(GameState == Game_Playing){
        if(bMouseClick==true){
           bMouseClick=false;
        
           
          if( (xMousePos >= rectPicture[Space_ypos-1][Space_xpos].x) &&
              (xMousePos <= (rectPicture[Space_ypos-1][Space_xpos].x + rectPicture[Space_ypos-1][Space_xpos].w)) && 
              (yMousePos >= rectPicture[Space_ypos-1][Space_xpos].y) &&
              (yMousePos <= (rectPicture[Space_ypos-1][Space_xpos].y + rectPicture[Space_ypos-1][Space_xpos].h)) ){
              MoveDown(&Space_xpos, &Space_ypos);
           }


          if( (xMousePos >= rectPicture[Space_ypos+1][Space_xpos].x) &&
              (xMousePos <= (rectPicture[Space_ypos+1][Space_xpos].x + rectPicture[Space_ypos+1][Space_xpos].w)) && 
              (yMousePos >= rectPicture[Space_ypos+1][Space_xpos].y) &&
              (yMousePos <= (rectPicture[Space_ypos+1][Space_xpos].y + rectPicture[Space_ypos+1][Space_xpos].h)) ){
              MoveUp(&Space_xpos, &Space_ypos);
           }


          if( (xMousePos >= rectPicture[Space_ypos][Space_xpos-1].x) &&
              (xMousePos <= (rectPicture[Space_ypos][Space_xpos-1].x + rectPicture[Space_ypos][Space_xpos-1].w)) && 
              (yMousePos >= rectPicture[Space_ypos][Space_xpos-1].y) &&
              (yMousePos <= (rectPicture[Space_ypos][Space_xpos-1].y + rectPicture[Space_ypos][Space_xpos-1].h)) ){
              MoveRight(&Space_xpos, &Space_ypos);
           }


          if( (xMousePos >= rectPicture[Space_ypos][Space_xpos+1].x) &&
              (xMousePos <= (rectPicture[Space_ypos][Space_xpos+1].x + rectPicture[Space_ypos][Space_xpos+1].w)) && 
              (yMousePos >= rectPicture[Space_ypos][Space_xpos+1].y) &&
              (yMousePos <= (rectPicture[Space_ypos][Space_xpos+1].y + rectPicture[Space_ypos][Space_xpos+1].h)) ){
              MoveLeft(&Space_xpos, &Space_ypos);
           }

        }
    }// game playing
              
              	break;       	
              
              case SDL_QUIT:
                    bQuit = true;
                    break;

            }// switch
            
        } //while event
     
    
     //*********游戏开始界面处理*********
      if(GameState == Game_Start){
       	//画开始界面背景图
        SDL_RenderCopy(render,textureStart,NULL,&rectBG);
        
       }//Start 
     //*******************************
    
       
    //*********游戏中界面处理*********
    if(GameState==Game_Playing){

     bool bGameOver = IsFinished();
     if(bGameOver==true){
        //MessageBoxA(FindWindowA("Game","Game"),"Game Over!","拼图",0);
        GameState = Game_Over;
     }
  
     Refresh(rectPicture);
	 
     }// if Game playing
     //*******************************
     
     
    //*********游戏结束界面处理*********
     if(GameState == Game_Over){
     	
         Refresh(rectPicture);
     	 //SDL_RenderCopy(render,textureOver,NULL,&rectBG);
     	 //Draw text "Game Over"
     }
     //*******************************
     
     
     SDL_RenderPresent(render);
     SDL_Delay(10);
    
    }//while 

 
    //加载图片图层 (png)
    SDL_DestroyTexture(texturePicture);
    SDL_FreeSurface(surfacePicture); 
    SDL_DestroyTexture(textureStart);
    SDL_FreeSurface(surfaceStart);
    SDL_DestroyTexture(textureOver);
    SDL_FreeSurface(surfaceOver);
    //主渲染层
    SDL_DestroyRenderer(render);
    SDL_DestroyWindow(window);
    
	IMG_Quit();
    SDL_Quit();
    return 0;
}

