#ifndef LINE2CSV_EXCEPTION_H
#define LINE2CSV_EXCEPTION_H

#include <QString>
#include <iostream>

enum Line2CsvExceptionCodes
{
    PROCESS_0K = 0,
    INPUT_FILE_ERROR,
    ARGUMENT_ERROR,
    GEOMETRY_TYPE_ERROR,
    OUTPUT_FILE_ERROR
};

class Line2Csv_Exception : public std::exception
{
public: 
    Line2Csv_Exception(int errorCode);

    void init(int errorCode);
    static QString getCodeAsString(int errorCode);
    QString getErrorMsgString();

private:
    int m_nErrorCode;
};

#endif // LINE2CSV_EXCEPTION_H
