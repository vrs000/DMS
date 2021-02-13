#include "io.h"

QString IO::FilePath = "/%1";

QVector<QVector<double>> IO::BaseTable;
QVector<QString> IO::IndicatorsNames;
QVector<QString> IO::ProjectsNames;

bool IO::IsBuild = true;
bool IO::IsPaint = true;

bool IO::isReadableDataValid = true;
QString IO::DataValidationMsgError = "";

void IO::OpenExelFile(QString Path)
{
    QAxObject* excel = new QAxObject("Excel.Application", 0);
    QAxObject* workbooks = excel->querySubObject("Workbooks");
    QAxObject* workbook = workbooks->querySubObject("Open(const QString&)", Path);
    QAxObject* sheets = workbook->querySubObject("Worksheets");
    QAxObject* sheet = sheets->querySubObject("Item(int)", 1);


    QAxObject* usedRange = sheet->querySubObject("UsedRange");
    QAxObject* rows = usedRange->querySubObject("Rows");
    int countRows = rows->property("Count").toInt();


    QAxObject* columns = usedRange->querySubObject("Columns");
    int countCols = columns->property("Count").toInt();





    for (int row=0; row < countRows; row++)
    {
        QAxObject* cell = sheet->querySubObject("Cells(int,int)", row + 1, 1);
        QVariant value = cell->property("Value");
        delete cell;
        if (value.toString() == "")
        {
            countRows = row;
            break;
        }
    }


    for (int columns=0; columns < countCols; columns++)
    {
        QAxObject* cell = sheet->querySubObject("Cells(int,int)", 1, columns+1);
        QVariant value = cell->property("Value");
        delete cell;
        if (value.toString() == "")
        {
            countCols = columns;
            break;
        }
    }





    QVector<QString> names(countRows - 1, "");
    QVector<QString> params(countCols - 1, "");
    QVector<QVector<double>> table(countRows-1, QVector<double>(countCols-1, 0));


    //Заполнение names
    for (int row=1; row < countRows; row++)
    {
        QAxObject* cell = sheet->querySubObject("Cells(int,int)", row + 1, 1);
        QVariant value = cell->property("Value");
        names[row-1] = value.toString();
        delete cell;
    }


    //Заполнение params
    for (int column=1; column < countCols; column++)
    {
        QAxObject* cell = sheet->querySubObject("Cells(int,int)", 1, column+1);
        QVariant value = cell->property("Value");
        params[column-1] = value.toString();
        delete cell;
    }


    //Заполнение table
    for ( int row = 1; row < countRows; row++ )
    {
        for ( int column = 1; column < countCols; column++ )
        {
            QAxObject* cell = sheet->querySubObject("Cells(int,int)", row + 1, column + 1);
            QVariant value = cell->property("Value");
            delete cell;
            table[row-1][column-1] = value.toDouble();


        }
    }


    IndicatorsNames = params;
    ProjectsNames = names;
    BaseTable = table;


    workbook->dynamicCall("Close()");
    excel->dynamicCall("Quit()");

    delete columns;
    delete rows;
    delete usedRange;
    delete sheet;
    delete sheets;
    delete workbook;
    delete workbooks;
    delete excel;

    //    FileLoadingDialog* loadingFileDialog = new FileLoadingDialog(Path);
    //    loadingFileDialog->setModal(true);
    //    loadingFileDialog->show();
    //    loadingFileDialog->setWindowFlags(Qt::WindowStaysOnTopHint);
    //    loadingFileDialog->show();
    //    loadingFileDialog->close();
    //    delete loadingFileDialog;
}

void IO::OpenExelFile1(QString Path)
{
    QXlsx::Document xlsx(Path);
    int RangeX = 1;
    int RangeY = 1;


    //Ищем x
    while (true)
    {
        QString value = xlsx.read(1, RangeX).toString();
        RangeX++;

        if (value == "")
        {
            RangeX--;
            break;
        }

    }

    //Ищем y
    while (true)
    {
        QString value = xlsx.read(RangeY, 1).toString();
        RangeY++;

        if (value == "")
        {
            RangeY--;
            break;
        }
    }


    RangeX--;
    RangeY--;


    QVector<QString> indicatorsNames;
    QVector<QString> projectsNames;
    QVector<QVector<double>> table;

    const QString alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";

    //Чтение показателей
    for (int x = 2; x<=RangeX; x++)
        indicatorsNames << xlsx.read(1, x).toString();


    //Чтение названий проектов
    for (int y = 2; y <=RangeY; y++)
        projectsNames << xlsx.read(y, 1).toString();


    //Чтение таблицы
    for (int y = 2; y <= RangeY; y++)
    {
        table << QVector<double>();

        for (int x = 2; x <= RangeX; x++)
        {
            auto a = xlsx.cellAt(y,x)->value();

            int digits  = 0;
            int letters = 0;
            int spaces  = 0;
            int puncts  = 0;


            if (a == QVariant())
            {
                isReadableDataValid = false;
                DataValidationMsgError = QString("Имеется пустая ячейка '%1%2'").arg(alphabet[x-1]).arg(y);
                return;
            }

            if (strcmp(a.typeName(), "QString") == 0)
            {
                double value;
                QString A = a.toString();

                //Data validation
                //-----------------------------------------------------------------------------------------------------------

                foreach(QChar s, A)
                {
                    if (s.isDigit()) digits++;
                    if (s.isPunct()) puncts++;
                    if (s.isLetter()) letters++;
                    if (s.isSpace()) spaces++;
                }


                //Если считанное значение не соответсвует формату числа
                if (!(digits!=0 && ((puncts == 1 && A.contains(',')) || puncts == 0)  && spaces == 0 && letters == 0))
                {
                    isReadableDataValid = false;

                    if (letters)
                        DataValidationMsgError = QString("Имеются символы в ячейке '%1%2'").arg(alphabet[x-1]).arg(y);

                    if (spaces)
                        DataValidationMsgError = QString("Имеются пробелы в ячейке '%1%2'").arg(alphabet[x-1]).arg(y);

                    if (digits!=0 && puncts == 1 && A.contains('.') && spaces == 0 && letters == 0)
                        DataValidationMsgError = QString("Использовано '.' вместо ',' в качестве разделителя дробной части в ячейке '%1%2'").arg(alphabet[x-1]).arg(y);

                    return;
                }

                //-----------------------------------------------------------------------------------------------------------

                A.replace(',', '.');
                value =  A.toDouble();
                table.last() << value;
            }

            if (strcmp(a.typeName(), "double") == 0)
                table.last() << a.toDouble();
        }
    }


    //Identical columns validation
    //---------------------------------------

    bool isIdentical = true;
    int value;

    for (int column=0; column < table.first().size(); column++)
    {
        isIdentical = true;
        value = table[0][column];

        for (int row=0; row < table.size(); row++)
            isIdentical = isIdentical && table[row][column] == value;

        if (isIdentical)
        {
            isReadableDataValid = false;

            DataValidationMsgError = QString("Имеется столбец одинаковых значений показателя '%1'").arg(indicatorsNames[column]);

            return;
        }
    }

    //---------------------------------------


    IndicatorsNames = indicatorsNames;
    ProjectsNames = projectsNames;
    BaseTable = table;

    isReadableDataValid = true;
}


void IO::OpenExelFile(QString Path, int StartX, int StartY)
{
    Q_UNUSED(Path);
    Q_UNUSED(StartX);
    Q_UNUSED(StartY);
}


void IO::OpenCSVFile(QString Path)
{
    Q_UNUSED(Path);
}


void IO::MakeExcelReport(QList<Solution> solutions)
{
    const QString FolderName = "reports";

    QString FileName = QString("report %1 - %2.xlsx")
            .arg(QDate::currentDate().toString())
            .arg(QTime::currentTime().toString());

    //QString filePath = QString("%1/%2").arg(FolderName).arg(FileName);
    QString filePath = FileName;

    if (!QDir(FolderName).exists())
    {
        QDir().mkdir(FolderName);
    }

    QFile excelFile(filePath);
    excelFile.open(QIODevice::WriteOnly);

    excelFile.close();
}

void IO::SaveExcelFile(QList<Solution> solutionsList)
{
    auto LongestWordLength = [](QVector<QString> list)
    {
        int max = -1;

        foreach(auto word, list)
            max = word.size() > max ? word.size() : max;

        return max;
    };
    auto GetColorOfNormalizedValue = [](double k)
    {
        QColor color;

        int r = 250;
        int g = 70;
        int b = 70;

        double dist = 215.74985515638244;

        double dr = -0.44495974252411946;
        double dg = 0.8064895333249665;
        double db = 0.38933977470860454;

        color.setRgb(qRound(r + dr*dist*k), qRound(g + dg*dist*k), qRound(b + db*dist*k));

        return color;
    };
    auto GetColorOfRatingValue = [](double k)
    {
        //G (154, 244, 154)
        //R (250, 70, 70)

        int r = 250;
        int g = 70;
        int b = 70;

        double offset = 2;

        if (k ==  0) return QColor(255, 30, 30);


        double dist = 215.74985515638244;
        double dr = -0.44495974252411946;
        double dg = 0.8064895333249665;
        double db = 0.38933977470860454;


        int R, G, B;
        if (k < 0.1)
        {
            R = qRound(r + dr*dist*k*offset);
            G = qRound(g + dg*dist*k*offset);
            B = qRound(b + db*dist*k*offset);

        }

        else
        {
            R = qRound(r + dr*dist*k);
            G = qRound(g + dg*dist*k);
            B = qRound(b + db*dist*k);
        }

        return QColor(R, G, B);
    };

    QVector<QString> topProjects;
    auto GetColorOfHardRating = [&](int projectIndex, QVector<QString> notSortedProjects, double HR)
    {
        /* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
         * В этой реализации есть недочет
         * В случае если будут проекты с одинаковыми именами,
         * то будут некорректно опредляться позиции в топе
           !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */

        //G (154, 244, 154)
        //R (250, 70, 70)
        //int r = 250;
        //int g = 70;
        //int b = 70;
        if (HR <= 0.0000001) return QColor(250, 70, 70);

        double dist = 215.74985515638244;
        double dr = -0.44495974252411946;
        double dg = 0.8064895333249665;
        double db = 0.38933977470860454;


        int posInTop = topProjects.indexOf(notSortedProjects[projectIndex]);

        double step = dist/topProjects.size();

        double step_dr = step * dr;
        double step_dg = step * dg;
        double step_db = step * db;

        return QColor(154 - posInTop*step_dr, 244- posInTop*step_dg, 154- posInTop*step_db);
    };



    const double xOffset = 1.5;

    QXlsx::Document xlsx;
    QXlsx::Format format;
    QXlsx::Format format1;
    QXlsx::Format format2;

    format.setHorizontalAlignment(QXlsx::Format::AlignHCenter);
    format1.setHorizontalAlignment(QXlsx::Format::AlignHCenter);
    format2.setHorizontalAlignment(QXlsx::Format::AlignHCenter);

    format.setBorderStyle(QXlsx::Format::BorderMedium);
    format1.setBorderStyle(QXlsx::Format::BorderThin);
    format2.setBorderStyle(QXlsx::Format::BorderThin);

    format.setFontSize(14);
    format1.setFontSize(10);
    format2.setFontSize(12);


    format.setPatternBackgroundColor(QColor(220, 230, 241));
    format1.setPatternBackgroundColor(QColor(235, 241, 222));
    format2.setPatternBackgroundColor(QColor(235, 241, 222));


    xlsx.deleteSheet("Sheet1");

    double k;
    foreach (auto sol, solutionsList)
    {
        topProjects = GetTopProjects(sol.IsParettoCriterionUsed ? sol.ParettoSetProjects : sol.ProjectsNames, sol.HardRatings, sol.SoftRatings);

        xlsx.addSheet(sol.SolutionName);

        //Заполнение исходной таблицы
        //-----------------------------------------------------------------------

        //Заполнение горизонтальных заголовков
        xlsx.write(1, 1, "Проекты", format);
        xlsx.setColumnWidth(1, xOffset * LongestWordLength(sol.ProjectsNames));

        for (int j=0; j < sol.IndicatorsNames.size(); j++)
        {
            xlsx.setColumnWidth(j + 2, xOffset *  sol.IndicatorsNames[j].size());
            xlsx.write(1, j + 2, sol.IndicatorsNames[j], format);
        }

        //Заполнение вертикальных заголовков
        for (int i = 0; i < sol.ProjectsNames.size(); i++)
            xlsx.write(i+2, 1, sol.ProjectsNames[i], format);

        //format.setBorderStyle(QXlsx::Format::BorderDotted);
        //Заполнение начальной таблицы
        for (int i = 0; i < sol.BaseTable.size(); i++)
            for (int j = 0; j < sol.BaseTable[0].size(); j++)
                xlsx.write(i + 2, j + 2, sol.BaseTable[i][j], format1);

        //-----------------------------------------------------------------------


        //Заполнение расчетной таблицы
        //===========================================================================================================
        const int BeginY = sol.ProjectsNames.size() + 1 + 3;

        //Заполнение горизонтальных заголовков
        //-------------------------------------------------------------------------------------------------
        xlsx.write(BeginY, 1, "Проекты", format);
        for (int j=0; j < sol.IndicatorsNames.size(); j++)
        {
            xlsx.write(BeginY, j + 4, sol.IndicatorsNames[j], format);
            xlsx.setColumnWidth(j + 4, xOffset *  sol.IndicatorsNames[j].size());
        }


        xlsx.setColumnWidth(2, xOffset *  QString("Жёсткий рейтинг").size());
        xlsx.setColumnWidth(3, xOffset *  QString("Мягкий рейтинг").size());


        xlsx.write(BeginY, 2, "Жёсткий рейтинг", format);
        xlsx.write(BeginY, 3, "Мягкий рейтинг", format);
        //-------------------------------------------------------------------------------------------------


        //Заполнение вертикальных заголовков
        //-------------------------------------------------------------------------------------------
        auto projectsNames = sol.IsParettoCriterionUsed ? sol.ParettoSetProjects : sol.ProjectsNames;

        for (int i = 0; i < projectsNames.size(); i++)
            xlsx.write(BeginY + i + 1, 1, projectsNames[i], format);
        //-------------------------------------------------------------------------------------------

        //Заполнение расчетной таблицы
        //=================================================================================================================
        const int maxColorValue = 200;

        if (sol.IsParettoCriterionUsed)
        {

            for (int i = 0; i < sol.ParettoSetProjects.size(); i++)
                for (int j = 0; j < sol.NormalizedTable[0].size(); j++)
                {
                    int ProjectIndexInStartList = sol.ProjectsNames.indexOf(sol.ParettoSetProjects[i]);
                    double val = sol.NormalizedTable[ProjectIndexInStartList][j];

                    if (IsPaint)
                    {
                        format1.setPatternBackgroundColor(GetColorOfNormalizedValue(val));
                    }
                    xlsx.write(BeginY + i + 1, j + 4, val/*sol.NormalizedTable[i][j]*/, format1);
                }
        }
        else
        {
            for (int i = 0; i < sol.NormalizedTable.size(); i++)
                for (int j = 0; j < sol.NormalizedTable[0].size(); j++)
                {
                    if (IsPaint)
                    {
                        double k = sol.NormalizedTable[i][j];
                        //                        GetColorOfNormalizedValue
                        //                        format1.setPatternBackgroundColor(QColor(qCeil(maxColorValue * (1 - k)), qCeil(k * maxColorValue), 0, 180));
                        format1.setPatternBackgroundColor(GetColorOfNormalizedValue(k));
                    }
                    xlsx.write(BeginY + i + 1, j + 4, sol.NormalizedTable[i][j], format1);
                }
        }

        //Заполнение жесткого+мягкого рейтинга
        for (int i = 0; i < sol.HardRatings.size(); i++)
        {
            if (IsPaint)
                format1.setPatternBackgroundColor(GetColorOfHardRating(i, sol.IsParettoCriterionUsed ? sol.ParettoSetProjects : sol.ProjectsNames, sol.HardRatings[i]));
            //                format1.setPatternBackgroundColor(GetColorOfRatingValue(sol.HardRatings[i]));

            xlsx.write(BeginY + i + 1, 2, sol.HardRatings[i], format1);


            if (IsPaint)
                format1.setPatternBackgroundColor(GetColorOfNormalizedValue(sol.SoftRatings[i]));

            xlsx.write(BeginY + i + 1, 3, sol.SoftRatings[i], format1);
        }

        //=================================================================================================================

        //Заполнение Инфо таблицы
        /*
           ---------------------------------- ---------------------------
          |     STEP        |     Number     |    param1 > param2        |
          |----------------------------------|---------------------------|
          |    Iteration    |     Number     |    param1 > param3        |
          |-----------------|----------------|---------------------------|
          |     Param1      |     Max|Min    |...........................|
          |-----------------|----------------|...........................|
          |     Param2      |     Max|Min    |...........................|
          |-----------------|----------------|...........................|
          | ..............  | .............  |...........................|
          | ..............  | .............  |...........................|

        */
        //------------------------------------------------------------------------------------------------------------
        const int BeginX = 1 + sol.IndicatorsNames.size() + 5;

        int max = 0;

        int w = LongestWordLength(sol.IndicatorsNames) > 19 ?
                    LongestWordLength(sol.IndicatorsNames) : 19;




        xlsx.setColumnWidth(BeginX, xOffset * w);
        xlsx.setColumnWidth(BeginX + 1, xOffset * 14);

        xlsx.write(2, BeginX, "Шаг дробления", format2);
        xlsx.write(2, BeginX + 1, sol.CrushingStep, format2);

        xlsx.write(3, BeginX, "Количество итераций", format2);
        xlsx.write(3, BeginX + 1, static_cast<int>(StartupConfigForm::fact(sol.IndicatorsNames.size()
                                                                           + 1/sol.CrushingStep - 1)/StartupConfigForm::fact(1/sol.CrushingStep)
                                                   /StartupConfigForm::fact(sol.IndicatorsNames.size() - 1)), format2);

        for (int i = 0; i < sol.PriorityList.size(); i++)
        {
            xlsx.write(4 + i, BeginX, sol.IndicatorsNames[i], format2);
            xlsx.write(4 + i, BeginX + 1,
                       sol.PriorityList[i] == "max" ? "Больше - лучше" : "Меньше - лучше", format2);
        }

        int row = 2;

        //Группы важности
        //------------------------------------------------------------------------------
        QStringList projects = sol.ProjectsImportanceNotParsed.split(",");
        QStringList indicators = sol.IndicatorsImportanceNotParsed.split(",");
        QStringList parsedRow;
        QString first;
        QString second;


        int MaxCount = projects.size() > indicators.size() ? projects.size() : indicators.size();
        int max1 = 0;
        int max2 = 0;


        for (int i=0; i < MaxCount; i++)
        {
            if (sol.IndicatorsImportanceNotParsed != "")
                if (i < indicators.size())
                {
                    if (indicators[i].contains('>'))
                    {
                        parsedRow = indicators[i].split('>');
                        parsedRow.first().toInt();
                        first = sol.IndicatorsNames[parsedRow.first().toInt()];
                        second = sol.IndicatorsNames[parsedRow.last().toInt()];

                        max1 = QString("%1>%2").arg(first).arg(second).size() > max1 ? QString("%1>%2").arg(first).arg(second).size() : max1;

                        xlsx.write(row, BeginX + 2, QString("%1>%2").arg(first).arg(second), format2);
                    }

                    if (indicators[i].contains("≥"))
                    {
                        parsedRow = indicators[i].split("≥");
                        parsedRow.first().toInt();
                        first = sol.IndicatorsNames[parsedRow.first().toInt()];
                        second = sol.IndicatorsNames[parsedRow.last().toInt()];

                        max1 = QString("%1≥%2").arg(first).arg(second).size() > max1 ? QString("%1≥%2").arg(first).arg(second).size() : max1;

                        xlsx.write(row, BeginX + 2, QString("%1≥%2").arg(first).arg(second), format2);
                    }

                }

            if (sol.ProjectsImportanceNotParsed != "")
                if (i < projects.size())
                {
                    if (projects[i].contains('>'))
                    {
                        parsedRow = projects[i].split('>');
                        parsedRow.first().toInt();
                        first = sol.ProjectsNames[parsedRow.first().toInt()];
                        second = sol.ProjectsNames[parsedRow.last().toInt()];

                        max2 = QString("%1≥%2").arg(first).arg(second).size() > max2 ? QString("%1≥%2").arg(first).arg(second).size() : max2;

                        if (sol.IndicatorsImportanceNotParsed == "")
                            xlsx.write(row, BeginX + 2, QString("%1>%2").arg(first).arg(second), format2);
                        else
                            xlsx.write(row, BeginX + 3, QString("%1>%2").arg(first).arg(second), format2);


                    }
                    if (projects[i].contains("≥"))
                    {
                        parsedRow = projects[i].split("≥");
                        parsedRow.first().toInt();
                        first = sol.ProjectsNames[parsedRow.first().toInt()];
                        second = sol.ProjectsNames[parsedRow.last().toInt()];
                        max2 = QString("%1≥%2").arg(first).arg(second).size() > max2 ? QString("%1≥%2").arg(first).arg(second).size() : max2;
                        if (sol.IndicatorsImportanceNotParsed == "")
                            xlsx.write(row, BeginX + 2, QString("%1≥%2").arg(first).arg(second), format2);
                        else
                            xlsx.write(row, BeginX + 3, QString("%1≥%2").arg(first).arg(second), format2);
                    }
                }

            row++;
        }


        bool expr1 = sol.IndicatorsImportanceNotParsed == "" && sol.ProjectsImportanceNotParsed != "";
        bool expr2 = sol.IndicatorsImportanceNotParsed != "" && sol.ProjectsImportanceNotParsed == "";


        //Если заданы суждения 1го или 2го рода
        //        if ((sol.IndicatorsImportanceNotParsed == "") || (sol.ProjectsImportanceNotParsed == ""))
        if (expr1 || expr2)
        {
            xlsx.mergeCells(QXlsx::CellRange(1, BeginX, 1, BeginX + 2), format);
            xlsx.setColumnWidth(BeginX + 2, xOffset *  (max1 > max2 ? max1 : max2 ));
        }


        //Если не заданы суждения 1го и 2го рода
        if ((sol.IndicatorsImportanceNotParsed == "") && (sol.ProjectsImportanceNotParsed == ""))
        {
            xlsx.mergeCells(QXlsx::CellRange(1, BeginX, 1, BeginX + 1), format);
        }


        //Если заданы суждения 1го и 2го рода
        if ((sol.IndicatorsImportanceNotParsed != "") && (sol.ProjectsImportanceNotParsed != ""))
        {
            xlsx.mergeCells(QXlsx::CellRange(1, BeginX, 1, BeginX + 3), format);
            xlsx.setColumnWidth(BeginX + 2, xOffset * max1);
            xlsx.setColumnWidth(BeginX + 3, xOffset * max2);
        }


        xlsx.write(1, BeginX, "Конфигурация", format);


        format1.setPatternBackgroundColor(QColor(235, 241, 222));


        //Построение графиков
        //======================================================================================================
        if (IsBuild)
        {
            //Диапозон данных
            //----------------------------------------------------------------------------------------------------------------------------------------
            int begY = 1 + sol.ProjectsNames.size() + 4;
            int begX = 2;

            int endY = 1 + sol.ProjectsNames.size() + 4 + (sol.IsParettoCriterionUsed ? sol.ParettoSetProjects.size() : sol.ProjectsNames.size()) - 1;
            int endX = 3;
            //----------------------------------------------------------------------------------------------------------------------------------------


            //Положение графика
            //-----------------------------------------------------------------------------------------------------------------------------------
            int row = 7 + sol.ProjectsNames.size() + (sol.IsParettoCriterionUsed ? sol.ParettoSetProjects.size() : sol.ProjectsNames.size()) - 2;
            int col = 1;
            //-----------------------------------------------------------------------------------------------------------------------------------


            Chart* chart = xlsx.insertChart(row, col, QSize(1000, 600));
            chart->setChartType(Chart::CT_Bar3D);
            chart->addSeries(CellRange(begY, begX, endY, endX));

            //Добавление легенды рядом с графиком
            //---------------------------------------------------------------------------------------------
//            auto projects = sol.IsParettoCriterionUsed ? sol.ParettoSetProjects : sol.ProjectsNames;

//            Format legendFormat;
//            legendFormat.setPatternBackgroundColor(QColor(253, 233, 217));
//            legendFormat.setFontBold(true);
//            legendFormat.setHorizontalAlignment(QXlsx::Format::AlignLeft);


//            for (int i = 0; i < projects.size(); i++)
//                xlsx.write(row + 1 + i, col, QString("%1. %2").arg(i + 1).arg(projects[i]), legendFormat);
            //---------------------------------------------------------------------------------------------
        }
        //======================================================================================================
    }


    QDate cd = QDate::currentDate();
    QTime ct = QTime::currentTime();


    QString filename = QString("report%1%2%3%4%5%6")
            .arg(cd.day()).arg(cd.month()).arg(cd.year())
            .arg(ct.hour()).arg(ct.minute()).arg(ct.second());

    QString path = QFileDialog::getSaveFileName(nullptr, "Сохранить файл",
                                                FilePath.arg(filename),
                                                "Excel files (*.xlsx)");
    xlsx.saveAs(path);
    FilePath = path.remove(path.split("/").last()) + "%1";
}

QVector<QString> IO::GetTopProjects(QVector<QString> projectNames, QVector<double> hardRating, QVector<double> softRating)
{
    QVector<QString> topProjects = projectNames;
    QVector<double> hard = hardRating;
    QVector<double> soft = softRating;


    auto swap = [&](int index1, int index2)
    {
        //swap projects
        QString s = topProjects[index1];
        topProjects[index1] = topProjects[index2];
        topProjects[index2] = s;


        //swap hard
        double tmp = hard[index1];
        hard[index1] = hard[index2];
        hard[index2] = tmp;


        //swap soft
        tmp = soft[index1];
        soft[index1] = soft[index2];
        soft[index2] = tmp;
    };


    //Сортировка
    for (int i = 0; i < hard.size(); i++)
    {
        for (int j = 0; j < hard.size() - 1; j++)
        {
            if (hard[j] < hard[j + 1])
                swap(j, j + 1);

            if ((hard[j] == hard[j + 1]) && (soft[j] < soft[j +1 ]))
                swap(j, j + 1);
        }
    }


    return topProjects;
}




void IO::InitList(int rows, int columns)
{
    IndicatorsNames = QVector<QString>(columns, "Ind");
    ProjectsNames = QVector<QString>(rows, "Ind");
    BaseTable = QVector<QVector<double>>(rows, QVector<double>(columns, 0));
}


void IO::SetStartData(QVector<QVector<double> > &table, QVector<QString> &indicatorsNames, QVector<QString> &projectsNames)
{
    IO::BaseTable = table;
    IO::IndicatorsNames = indicatorsNames;
    IO::ProjectsNames = projectsNames;
}


void IO::FillingTables(QTableWidget *input, QTableWidget *output)
{
    const double NumberAfterPoint = 1000000;
    const int LimitParams = 10;
    const QString Hard = "Жёсткий рейтинг";
    const QString Soft = "Мягкий рейтинг";
    const int alpha = 180;
    bool IsParettoCriterionUsed = DataProcessing::ParettoSetProjects.size() == 0 ? false : true;


    QVector<QString> topProjects = GetTopProjects(IsParettoCriterionUsed ? DataProcessing::ParettoSetProjects : ProjectsNames, DataProcessing::HardRatings, DataProcessing::SoftRatings);
    auto GetColorOfHardRating = [&](int projectIndex, QVector<QString> notSortedProjects, double HR)
    {
        /* !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
         * В этой реализации есть недочет
         * В случае если будут проекты с одинаковыми именами,
         * то будут некорректно опредляться позиции в топе
           !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! */

        //G (0, 255, 0)
        //R (255, 0, 0)
        //S (-255, 255, 0)
        //

        //int r = 250;
        //int g = 70;
        //int b = 70;
        if (HR <= 0.0000001) return QColor(255, 0, 0, 180);

        double dist = sqrt(255*255 + 255*255);
        double dr = -0.7071067811865475;
        double dg = 0.7071067811865475;
        double db = 0;


        int posInTop = topProjects.indexOf(notSortedProjects[projectIndex]);

        double step = dist/topProjects.size();

        double step_dr = step * dr;
        double step_dg = step * dg;
        double step_db = step * db;

        //        qDebug() << 0 - posInTop*step_dr << 255 - posInTop*step_dg << 0 - posInTop*step_db;
        return QColor(0 - posInTop*step_dr, 255 - posInTop*step_dg, 0 - posInTop*step_db, 180);
    };

    //Жирный шрифт
    QFont font;
    font.setBold(true);


    //Цвет заливки
    QColor color;
    color.setRgb(255, 249, 222, alpha/2);

    QColor ProjectsNotInParretoSetColor;
    ProjectsNotInParretoSetColor.setRgb(255, 0, 0, alpha/4);

    QColor ProjectsNotInParretoSetColor1;
    ProjectsNotInParretoSetColor1.setRgb(255, 0, 0, alpha/4);

    input->clear();
    output->clear();

    input->setSortingEnabled(false);
    output->setSortingEnabled(false);

    input->setColumnCount(IndicatorsNames.size()+1);
    input->setRowCount(ProjectsNames.size());



    //Генерация вертикальных и горизонтальный заголовков для InputTable
    //--------------------------------------------
    QStringList names, params;
    params<<"Проекты";

    for (auto i:IndicatorsNames)
        params << i;

    for (auto i:ProjectsNames)
        names << i;

    input->setHorizontalHeaderLabels(params);
    //--------------------------------------------



    //Заполнение InputTable
    //------------------------------------------------------------------------------------------------
    for (int i = 0; i < IO::BaseTable.size(); i++)
        for (int j = 0; j < params.size(); j++)
        {
            if (j==0)
            {
                input->setItem(i, j, new QTableWidgetItem(names[i]));
                input->item(i,j)->setFont(font);
                input->item(i,j)->setBackgroundColor(color);

                if (!DataProcessing::ParettoSetProjects.contains(names[i]) && IsParettoCriterionUsed)
                    input->item(i,j)->setBackgroundColor(ProjectsNotInParretoSetColor);
            }

            if (j!=0)
            {
                input->setItem(i, j, new QTableWidgetItem(QString::number(IO::BaseTable[i][j-1])));

                if (!DataProcessing::ParettoSetProjects.contains(names[i]) && IsParettoCriterionUsed)
                    input->item(i,j)->setBackgroundColor(ProjectsNotInParretoSetColor1);

            }
        }
    //------------------------------------------------------------------------------------------------


    output->setColumnCount(IndicatorsNames.size() + 3);

    if (DataProcessing::ParettoSetProjects.size() == 0)
        output->setRowCount(ProjectsNames.size());
    else
        output->setRowCount(DataProcessing::ParettoSetProjects.size());

    params.insert(1, Hard);
    params.insert(1, Soft);

    //params<<Hard<<Soft;
    output->setHorizontalHeaderLabels(params);


    /*
     Формат горизонтальных заголовков
     ____________________________________________________________________________
     Проекты | Мягкий рейтинг | Жесткий рейтинг | Показатель1 | ... | Показательi

    */



    if (DataProcessing::ParettoSetProjects.size() != 0)
    {
        names.clear();

        for (auto project:DataProcessing::ParettoSetProjects)
            names << project;
    }




    //Заполнение OutputTable
    //------------------------------------------------------------------------------------------------
    for (int i = 0; i < names.size(); i++)
        for (int j = 0; j < params.size(); j++)
        {
            //Заполнение столбца "Проекты"
            if (j == 0)
            {
                output->setItem(i, j, new QTableWidgetItem(names[i]));
                output->item(i, j)->setFont(font);
                output->item(i, j)->setBackgroundColor(color);
            }

            //Заполнение мягкого рейтинга
            if (j == 1)
            {
                output->setItem(i, j,
                                new QTableWidgetItem(QString::number(round(DataProcessing::SoftRatings[i]*NumberAfterPoint)/NumberAfterPoint, 'f')));
            }

            //Заполнение жесткого рейтинга
            if (j == 2)
            {
                output->setItem(i, j,
                                new QTableWidgetItem(QString::number(round(DataProcessing::HardRatings[i]*NumberAfterPoint)/NumberAfterPoint, 'f')));
            }

            //Заполнение показателей
            if (j > 2)
            {
                int ProjectIndexInStartList = ProjectsNames.indexOf(names[i]);
                double val = DataProcessing::NormalizedTable[ProjectIndexInStartList][j-3];


                //                double val = DataProcessing::NormalizedTable[i][j-3];


                val = round(val * NumberAfterPoint)/NumberAfterPoint;

                output->setItem(i, j, new QTableWidgetItem(QString::number(val, 'f')));
            }
        }
    //------------------------------------------------------------------------------------------------



    //Растягивание по вертикали
    //-------------------------------------------------------------------
    input->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    output->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    //-------------------------------------------------------------------


    //Растягивание по горизонтали
    //--------------------------------
    input->resizeColumnsToContents();
    output->resizeColumnsToContents();
    //--------------------------------


    //Центрирование текста
    //-----------------------------------------------------------------
    //InputTable
    for (int row=0; row < input->rowCount(); row++)
        for (int column=0; column < input->columnCount(); column++)
            input->item(row, column)->setTextAlignment(Qt::AlignCenter);

    //OutputTable
    for (int row=0; row < output->rowCount(); row++)
        for (int column=0; column < output->columnCount(); column++)
            output->item(row, column)->setTextAlignment(Qt::AlignCenter);
    //-----------------------------------------------------------------


    const int HardColumn = 2;
    const int SoftColumn = 1;


    //Окраска таблицы
    //-----------------------------------------------------------------------------
    for (int row = 0; row < output->rowCount(); row++)
        for (int column = 1; column < output->columnCount(); column++ )
        {
            double k = output->item(row, column)->text().toDouble();
            if (k > 1) k = 1;

            if (column > 2)
                color.setRgb(qCeil(255 * (1 - k)), qCeil(k * 255), 0, alpha/2);

            if (column == 1)
                color.setRgb(qCeil(255 * (1 - k)), qCeil(k * 255), 0, alpha);

            if (column == 2)
                color = GetColorOfHardRating(row, IsParettoCriterionUsed ? DataProcessing::ParettoSetProjects : ProjectsNames, k);

            output->item(row, column)->setBackgroundColor(color);
        }
    //-----------------------------------------------------------------------------



    //===================================================================================
    //    //Окраска жесткого рейтинга и мягкого
    //    for (int row = 0; row < output->rowCount(); row++)
    //    {
    //        double k1 = output->item(row, HardColumn)->text().toDouble();
    //        double k2 = output->item(row, SoftColumn)->text().toDouble();
    //        if (k1 > 1) k1 = 1;
    //        if (k2 > 1) k2 = 1;

    //        color.setRgb(qCeil(255 * (1 - k1)), qCeil(k1 * 255), 0, alpha);
    //        output->item(row, HardColumn)->setBackgroundColor(color);

    //        color.setRgb(qCeil(255 * (1 - k2)), qCeil(k2 * 255), 0, alpha);
    //        output->item(row, SoftColumn)->setBackgroundColor(color);
    //    }


    //    //Окраска нормированных показателей
    //    for (int row = 0; row < output->rowCount(); row++)
    //    {
    //        for (int column = 3; column < output->columnCount(); column++ )
    //        {
    //            double k1 = output->item(row, column)->text().toDouble();
    //            if (k1 > 1) k1 = 1;
    //            color.setRgb(qCeil(255 * (1 - k1)), qCeil(k1 * 255), 0, alpha/2);
    //            output->item(row, column)->setBackgroundColor(color);

    //        }
    //    }

    //===================================================================================

    input->setSortingEnabled(true);
    output->setSortingEnabled(true);
}














