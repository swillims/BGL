#include "mainmain.h" 
int main()
{
    mainmain::run();
}
int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPrev, LPSTR cmdLine, int show)
{
    main();
}
// go to mainmain.h to mess with the main method. Main/Central method decoupled from entry point.