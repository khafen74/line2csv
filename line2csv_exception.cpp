#include "line2csv_exception.h"

Line2Csv_Exception::Line2Csv_Exception(int errorCode)
{
    init(errorCode);
}

void Line2Csv_Exception::init(int errorCode)
{
    m_nErrorCode = errorCode;
}

QString Line2Csv_Exception::getCodeAsString(int errorCode)
{
    switch(errorCode)
    {
    case PROCESS_0K:
        return "process completed successfully";
        break;

    case INPUT_FILE_ERROR:
        return "input file error";
        break;

    case ARGUMENT_ERROR:
        return "arguments not specified correctly";
        break;

    case GEOMETRY_TYPE_ERROR:
        return "shapefile has incorrect geometry, must be a line";
        break;

    case OUTPUT_FILE_ERROR:
        return "output file error";
        break;

    default:
        return "unhandled error";
        break;
    }
}

QString Line2Csv_Exception::getErrorMsgString()
{
    QString result = getCodeAsString(m_nErrorCode);
    return result;
}
