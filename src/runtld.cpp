// MyTLD.cpp : 定义控制台应用程序的入口点。
//
#include <cv.h>
#include <highgui.h>
#include "Main.h"
#include "config.h"
#include "imAcq.h"
#include "gui.h"

int main(int argc, char ** argv)
{
    // 1 argument to set
    Main* main = new Main();
    Config config;
    ImAcq * imAcq = imAcqAlloc();
    Gui * gui = new Gui();

    main->gui = gui;
    main->imAcq = imAcq;

    config.printinfo();

    if (config.init(argc, argv) == PROGRAM_EXIT)
    {
        return EXIT_FAILURE;
    }

    config.configure(main);

    srand(main->seed);

    // 2 capture frame and select box
    imAcqInit(imAcq);

    if (main->showOutput)
    {
        gui->init();
    }

    // 3 tld process
    main->doWork();
    delete main;

    return EXIT_SUCCESS;
}