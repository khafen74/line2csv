#include <QCoreApplication>
#include <QtCore>
#include <QDebug>

#include "gdal.h"
#include "ogrsf_frmts.h"
#include "ogr_api.h"
#include "ogr_core.h"

int line2csv(const char *inputShapefile, const char *outputCsv, const char *fields = "none");
QStringList setFieldNames(OGRLayer *layer, const char *fields);
QStringList getFieldValues(OGRFeature *feature, QStringList fieldNames);
QStringList removeCommas(QStringList list);

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    OGRRegisterAll();

    const char *path = "C:/KonradWork/01_Projects/NorthArrow/line2csv/data/Thalweg/Thalweg.shp";
    const char *out = "C:/KonradWork/01_Projects/NorthArrow/line2csv/data/out.csv";
    const char *fields = "all";

    line2csv(path, out, fields);

    return a.exec();
}

int line2csv(const char *inputShapefile, const char *outputCsv, const char *fields)
{
    //declare OGR data source, OGR driver, Driver registrar
    OGRDataSource *inShape;
    OGRSFDriver *driver;
    OGRSFDriverRegistrar *registrar = OGRSFDriverRegistrar::GetRegistrar();

    //Get shapefile driver
    driver = registrar->GetDriverByName("ESRI_Shapefile");

    //Load shapefile
    inShape = OGRSFDriverRegistrar::Open(inputShapefile, FALSE, &driver);

    //Get layer
    OGRLayer *layer = inShape->GetLayer(0);

    //Get field names to print to csv
    QStringList fieldNames = setFieldNames(layer, fields);
    qDebug()<<"fieldname size"<<fieldNames.size();

    //Get geometry type, number of features
    OGRwkbGeometryType geomType = layer->GetGeomType();
    int nFeatures = layer->GetFeatureCount();
    qDebug()<<"features "<<nFeatures;

    //Reset layer reading position
    layer->ResetReading();

    QFile fileCsv(QString::fromUtf8(outputCsv));
    if (!fileCsv.open(QIODevice::WriteOnly))
    {
        qDebug()<<"error creating csv";
        return 1;
    }

    QTextStream stream(&fileCsv);
    stream<<"x,y";

    for (int i=0; i<fieldNames.size(); i++)
    {
        stream<<","<<fieldNames[i];
    }

    stream<<endl;

    stream.setRealNumberNotation(QTextStream::FixedNotation);

    //Line shapefile
    if (wkbFlatten(geomType) == wkbLineString)
    {
        qDebug()<<"linestring";
        OGRFeature *feature;
        QStringList fieldValues;

        qDebug()<<"starting feature loop";
        for (int i=0; i<nFeatures; i++)
        {
            //Get feature geometry as line
            feature = layer->GetNextFeature();
            OGRGeometry *geom;
            geom = feature->GetGeometryRef();
            OGRLineString *line = (OGRLineString*) geom;
            qDebug()<<"line set";

            //Get number of points on line
            int nPoints = line->getNumPoints();
            qDebug()<<"points "<<nPoints;

            fieldValues.clear();
            fieldValues = getFieldValues(feature, fieldNames);

            for (int j=0; j<nPoints; j++)
            {
                stream<<line->getX(j)<<","<<line->getY(j);

                for (int k=0; k<fieldValues.size(); k++)
                {
                    stream<<","<<fieldValues[k];
                }

                stream<<endl;
            }
        }
    }

    //Error if not line
    else
    {
        return 1;
    }

    fileCsv.close();

    return 0;
}

QStringList setFieldNames(OGRLayer *layer, const char *fields)
{
    const char *NONE = "none";
    const char *ALL = "all";

    OGRFeatureDefn *featDfn;
    OGRFieldDefn *fieldDfn;
    QStringList fieldNames;
    int nFields;

    if (fields == NONE)
    {
        fieldNames.clear();
        return fieldNames;
    }

    featDfn = layer->GetLayerDefn();
    nFields = featDfn->GetFieldCount();

    if (fields == ALL)
    {
        for (int i=0; i<nFields; i++)
        {
            fieldDfn = featDfn->GetFieldDefn(i);
            fieldNames.append(QString::fromUtf8(fieldDfn->GetNameRef()));
        }
    }
    else
    {
        QStringList selectedFields;
        selectedFields = QString::fromUtf8(fields).split(',');

        QString name;

        for(int i=0; i<selectedFields.size(); i++)
        {
            for (int j=0; j<nFields; j++)
            {
                fieldDfn = featDfn->GetFieldDefn(i);
                name = QString::fromUtf8(fieldDfn->GetNameRef());

                if (name == selectedFields[i])
                {
                    fieldNames.append(name);
                    break;
                }
            }
        }
    }

    return fieldNames;
}

QStringList getFieldValues(OGRFeature *feature, QStringList fieldNames)
{
    QStringList fieldValues;
    int fieldIndex;

    for (int i=0; i<fieldNames.size(); i++)
    {
        fieldIndex = feature->GetFieldIndex(fieldNames[i].toUtf8());
        fieldValues.append(QString::fromUtf8(feature->GetFieldAsString(fieldIndex)));
    }

    fieldValues = removeCommas(fieldValues);

    return fieldValues;
}

QStringList removeCommas(QStringList list)
{
    for (int i=0; i<list.size(); i++)
    {
        if(list[i].contains(','))
        {
            list[i] = list[i].replace(',', ' ');
        }
    }

    return list;
}
