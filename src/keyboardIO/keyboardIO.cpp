#include "keyboardIO.h"
#include <termios.h>
#include <unistd.h>
#include <iostream>
#include <vector>
#include "controller/controller.h"

extern Controller *controller;

void *KeyboardIO(void *arg)
{
    struct termios old_tio, new_tio;
    tcgetattr(STDIN_FILENO, &old_tio); // 保存终端属性
    // 设置终端为非规范模式，禁用回显和缓冲
    new_tio = old_tio;
    new_tio.c_lflag &= (~ICANON & ~ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_tio);

    bool isLongPress = false;
    char c = 0;
    char buff = 0;
    std::vector<double> q1 = {1, -0.5, -0.5, -2, 1, 1, 1};
    std::vector<double> q2 = {0, 0, 0, -2, 0, 0, 0};
    while (1)
    {
        if (read(STDIN_FILENO, &c, 1) == 1)
        {
            if (buff == c)
            {
                isLongPress = true;
            }
            else
            {
                isLongPress = false;
            }
            buff = c;

            switch (c)
            {
            case 'w':
                // controller->powerOn();
                controller->setRunSpeed(0.5);
                controller->createRunTask(q1, TaskSpace::jointSpace);
                break;
            case 'a':

                break;
            case 's':
                controller->setRunSpeed(0.6);
                controller->createRunTask(q2, TaskSpace::jointSpace);
                break;
            case 'd':

                break;
            default:
                break;
            }
        }

        if (isLongPress)
        {
            std::cout << "isLongPress:";
            std::cout << c << std::endl;
        }

        c = 0;
        usleep(1000 * 2);
    }

    return nullptr;
}