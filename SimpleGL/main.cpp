#include "MainWidget.h"
#include <QtWidgets/QApplication>
#include <sstream>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWidget w;

    // 处理命令行选项
    std::string scene_filename = "resources/scene/MyScene.json";
    bool useRT = false;
    int thread_num = 1;

    if (argc % 2 == 0)
    {
        goto invalid_param;
    }
    for (int i = 1; i < argc; i += 2)
    {
        std::string optName(argv[i]);
        std::string opt(argv[i + 1]);

        if (optName == "-scene")
            scene_filename = opt;
        else if (optName == "-useRT")
        {
            if (opt == "1")
                useRT = true;
            else if (opt == "0")
                useRT = false;
            else
                goto invalid_param;
        }
        else if (optName == "-t")
        {
            std::stringstream stream;
            stream << opt;
            stream >> thread_num;
        }
    }

    w.setUseRT(useRT, thread_num);
    w.setScene(scene_filename);

    w.show();
    return a.exec();
   
invalid_param:
    std::cout << "Error : Invalid param!" << std::endl;
    system("pause");
    exit(-1);
}
