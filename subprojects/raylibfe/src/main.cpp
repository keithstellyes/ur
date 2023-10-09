#include "GameState.hpp"
#include "DrawThread.hpp"

int main(void)
{
    ur::GameState gs;
    urai::Oracle oracle;
    AiThread aiThread(&oracle);
    aiThread.start();

    DrawThread drawThread(&gs, &aiThread);
    drawThread.start();
    drawThread.join();
    aiThread.stop();
    
    return 0;
}