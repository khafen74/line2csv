#ifndef LINE2CSV_H
#define LINE2CSV_H

#include <QtCore>
#include "ogrsf_frmts.h"
#include "ogr_core.h"
#include "line2csv_exception.h"

class Line2Csv
{
public:
    Line2Csv(int argc, char * argv[]);

    int init(int argc, char * argv[]);
    int run();
    void printHelp();
    QStringList getFieldValues(OGRFeature *feature, QStringList fieldNames);
    QStringList fixCommas(QStringList list);
    QStringList removeSpaces(QStringList list);
    QStringList setFieldNames(OGRLayer *layer, QString fields);

private:
    QString mInputPath, mOutputPath, mFields;
    QStringList mFieldList;
};

#endif // LINE2CSV_H
