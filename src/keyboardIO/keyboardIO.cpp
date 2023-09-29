#include "keyboardIO.h"
#include <termios.h>
#include <unistd.h>
#include <iostream>
#include <vector>
#include "controller/controller.h"
#include "robot/robot.h"
#include "wdLog/log.h"
extern Controller *controller;
extern Robot *robot;

void *KeyboardTask(void *arg)
{
    wdlog_i("KeyboardTask", "创建键盘控制任务\n");

    struct termios old_tio, new_tio;
    tcgetattr(STDIN_FILENO, &old_tio);
    // 设置终端为非规范模式，禁用回显和缓冲
    new_tio = old_tio;
    new_tio.c_lflag &= (~ICANON & ~ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &new_tio);

    bool isLongPress = false;
    char c = 0;
    char buff = 0;
    std::vector<double> q0 = {0, -0.785, 0, -2.356, 0, 1.5, 0.785};
    std::vector<double> q1 = {1, -0.5, -0.5, -2, 1, 1, 1};
    std::vector<double> q2 = {-1, -0.5, -0.5, -2, 1, 1, 1};
    std::vector<double> q3 = {-1, 0.5, 0.5, 2, -1, 1, 0.2};
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
            case '1':
                controller->changeControllerLaw(ControllerLawType::ComputedTorqueMethod_);
                break;
            case '2':
                controller->changeControllerLaw(ControllerLawType::Backstepping_);
                break;
            case '3':
                controller->changeControllerLaw(ControllerLawType::PD_);
                break;
            case 'w':
                wdlog_d("wd", "robot: % d\n", robot->getRobotDof());
                break;
            case 'q':
                controller->stopRun();
                break;
            case 'a':
                wdlog_d("wd", "目标 %.3f %.3f %.3f %.3f %.3f %.3f %.3f\n", q1[0], q1[1], q1[2], q1[3], q1[4], q1[5], q1[6]);
                controller->setRunSpeed(0.6);
                controller->createRunTask(q1, TaskSpace::jointSpace);
                break;
            case 's':
                wdlog_d("wd", "目标 %.3f %.3f %.3f %.3f %.3f %.3f %.3f\n", q0[0], q0[1], q0[2], q0[3], q0[4], q0[5], q0[6]);
                controller->setRunSpeed(0.6);
                controller->createRunTask(q0, TaskSpace::jointSpace);
                break;
            case 'd':
                wdlog_d("wd", "目标 %.3f %.3f %.3f %.3f %.3f %.3f %.3f\n", q2[0], q2[1], q2[2], q2[3], q2[4], q2[5], q2[6]);
                controller->setRunSpeed(0.5);
                controller->createRunTask(q2, TaskSpace::jointSpace);
                // case 'f':
                //     wdlog_d("wd", "目标 %.3f %.3f %.3f %.3f %.3f %.3f %.3f\n", q3[0], q3[1], q3[2], q3[3], q3[4], q3[5], q3[6]);
                //     controller->setRunSpeed(0.7);
                //     controller->createRunTask(q3, TaskSpace::jointSpace);
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