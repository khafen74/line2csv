#include "line2csv.h"

Line2Csv::Line2Csv(int argc, char *argv[])
{
    OGRRegisterAll();
    init(argc, argv);
}

int Line2Csv::init(int argc, char *argv[])
{
    if (argc > 1 && argc < 4)
    {
        mInputPath = QString::fromUtf8(argv[1]);
        mOutputPath = QString::fromUtf8(argv[2]);

        if (argc < 3)
        {
            mFields = "none";
        }
        else
        {
            mFields = QString::fromUtf8(argv[3]);
        }

    }
    else
    {
        printHelp();
        throw Line2Csv_Exception(ARGUMENT_ERROR);
    }

    QFile file(mInputPath);
    if (!file.exists())
    {
        throw Line2Csv_Exception(INPUT_FILE_ERROR);
    }

    return PROCESS_0K;
}

int Line2Csv::run()
{
    OGRDataSource *inShape;
    OGRSFDriver *driver;
    OGRSFDriverRegistrar *registrar = OGRSFDriverRegistrar::GetRegistrar();

    //Get shapefile driver
    driver = registrar->GetDriverByName("ESRI_Shapefile");

    //Load shapefile
    inShape = OGRSFDriverRegistrar::Open(mInputPath.toStdString().c_str(), FALSE, &driver);

    //Get layer
    OGRLayer *layer = inShape->GetLayer(0);

    //Get field names to print to csv
    mFieldList = setFieldNames(layer, mFields);

    //Get geometry type, number of features
    OGRwkbGeometryType geomType = layer->GetGeomType();
    int nFeatures = layer->GetFeatureCount();

    //Reset layer reading position
    layer->ResetReading();

    QFile fileCsv(mOutputPath);
    if (!fileCsv.open(QIODevice::WriteOnly))
    {
        throw Line2Csv_Exception(OUTPUT_FILE_ERROR);
    }

    //Write field names for FID, x, y to file
    QTextStream stream(&fileCsv);
    stream<<"FID,x,y";

    //Write field names for attributes to file
    for (int i=0; i<mFieldList.size(); i++)
    {
        stream<<","<<mFieldList[i];
    }

    stream<<endl;

    stream.setRealNumberNotation(QTextStream::FixedNotation);

    //Line shapefile
    if (wkbFlatten(geomType) == wkbLineString)
    {
        OGRFeature *feature;
        QStringList fieldValues;
        int nFid;

        for (int i=0; i<nFeatures; i++)
        {
            //Get feature geometry as line
            feature = layer->GetNextFeature();
            nFid = feature->GetFID();
            OGRGeometry *geom;
            geom = feature->GetGeometryRef();
            OGRLineString *line = (OGRLineString*) geom;

            //Get number of points on line
            int nPoints = line->getNumPoints();

            fieldValues.clear();
            fieldValues = getFieldValues(feature, mFieldList);

            //Write values to file
            for (int j=0; j<nPoints; j++)
            {
                stream<<nFid<<","<<line->getX(j)<<","<<line->getY(j);

                for (int k=0; k<fieldValues.size(); k++)
                {
                    stream<<","<<fieldValues[k];
                }

                stream<<endl;
            }
        }

        //Destroy the feature
        OGRFeature::DestroyFeature(feature);

    }

    //Error if not line
    else
    {
        throw Line2Csv_Exception(GEOMETRY_TYPE_ERROR);
    }

    fileCsv.close();
    OGRDataSource::DestroyDataSource(inShape);

    return PROCESS_0K;
}

void Line2Csv::printHelp()
{
    std::cout<< "\n Line2Csv     desc: prints x,y coordinates of each vertex from line shapefile to a csv with desired attributes\n";
    std::cout<< "\n Usage: line2csv <input_shapefile_path> <output_csv_path> <fields_(optional)>";
    std::cout<< "\n     input_shapefile_path     Path to polyline shapefile";
    std::cout<< "\n     output_csv_path          Path to write output csv file";
    std::cout<< "\n     fields                   Comma (,) separated list of fields to be printed to csv";
    std::cout<< "\n\n";
}

QStringList Line2Csv::getFieldValues(OGRFeature *feature, QStringList fieldNames)
{
    QStringList fieldValues;
    int fieldIndex;

    for (int i=0; i<fieldNames.size(); i++)
    {
        fieldIndex = feature->GetFieldIndex(fieldNames[i].toUtf8());
        fieldValues.append(QString::fromUtf8(feature->GetFieldAsString(fieldIndex)));
    }

    fieldValues = fixCommas(fieldValues);

    return fieldValues;
}

QStringList Line2Csv::fixCommas(QStringList list)
{
    for (int i=0; i<list.size(); i++)
    {
        if(list[i].contains(','))
        {
            list[i] = "\"" + list[i] + "\"";
        }
    }

    return list;
}

QStringList Line2Csv::removeSpaces(QStringList list)
{
    for (int i=0; i<list.size(); i++)
    {
        if(list[i].contains(' '))
        {
            list[i].remove(' ');
        }
    }

    return list;
}

QStringList Line2Csv::setFieldNames(OGRLayer *layer, QString fields)
{
    QString NONE = "none";
    QString ALL = "all";

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
        selectedFields = fields.split(',');
        selectedFields = removeSpaces(selectedFields);

        QString name;

        for(int i=0; i<selectedFields.size(); i++)
        {
            for (int j=0; j<nFields; j++)
            {
                fieldDfn = featDfn->GetFieldDefn(j);
                name = QString::fromUtf8(fieldDfn->GetNameRef());

                if (name == selectedFields[i])
                {
                    fieldNames.append(name);
                    break;
                }
            }
        }
    }

    fieldNames.removeDuplicates();

    return fieldNames;
}
