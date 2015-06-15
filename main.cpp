#include <QCoreApplication>
#include <iostream>
#include "line2csv.h"
#include "line2csv_exception.h"


int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    try
    {
        Line2Csv process(argc, argv);
        int result = process.run();
        if (result != PROCESS_0K)
        {
            std::cerr << std::endl << "Error: " << Line2Csv_Exception::getCodeAsString(result).toStdString() << std::endl;
            exit(EXIT_FAILURE);
        }
        exit(EXIT_SUCCESS);
    }

    catch(Line2Csv_Exception &e)
    {
        std::cerr << "Error: " << e.getErrorMsgString().toStdString() << std::endl;
        exit(EXIT_FAILURE);
    }

    catch(std::exception &e)
    {
        std::cerr << "Error: " <<e.what() << std::endl;
        exit(EXIT_FAILURE);
    }

    //return a.exec();
    qApp->quit();
}
