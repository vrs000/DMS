#include "io.h"

QString IO::FilePath = "/%1";

QVector<QVector<double>> IO::BaseTable;
QVector<QString> IO::IndicatorsNames;
QVector<QString> IO::ProjectsNames;

bool IO::IsBuild = true;
bool IO::IsPaint = true;

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



    //Чтение показателей
    for (int x = 2; x<=RangeX; x++)
    {
        indicatorsNames << xlsx.read(1, x).toString();
    }


    //Чтение названий проектов
    for (int y = 2; y <=RangeY; y++)
    {
        projectsNames << xlsx.read(y, 1).toString();
    }



    //Чтение таблицы
    for (int y = 2; y <= RangeY; y++)
    {
        table << QVector<double>();

        for (int x = 2; x <= RangeX; x++)
        {
            auto a = xlsx.read(y, x);


            if (strcmp(a.typeName(), "QString") == 0)
            {

                double value;

                QString A = a.toString();

                A.replace(',', '.');

                //                QJSEngine engine;
                //                QJSValue v;
                //                if (A.contains('='))
                //                    v = engine.evaluate(A.remove(0, 1));
                //                else
                //                    v = engine.evaluate(A);
                value =  A.toDouble();

                table.last() << value;

            }


            if (strcmp(a.typeName(), "double") == 0)
                table.last() << a.toDouble();
        }
    }


    IndicatorsNames = indicatorsNames;
    ProjectsNames = projectsNames;
    BaseTable = table;
}

void IO::OpenExelFile(QString Path, int StartX, int StartY)
{

}

void IO::OpenCSVFile(QString Path)
{



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


    /*
    QAxObject* excel = new QAxObject( "Excel.Application", 0);

    excel->dynamicCall( "SetVisible(bool)", TRUE );
    QAxObject *workbooks = excel->querySubObject( "Workbooks" );
    QAxObject *workbook = workbooks->querySubObject( "Open(const QString&)", filePath);
    QAxObject *sheets = workbook->querySubObject( "Sheets" );
    QAxObject *StatSheet = sheets->querySubObject( "Item(const QVariant&)", QVariant("stat") );
    StatSheet->dynamicCall( "Select()" );
    QAxObject *range = StatSheet->querySubObject( "Range(const QVariant&)", QVariant(QString("A1:A1")));
    range->dynamicCall( "Clear()" );
    /range->dynamicCall( "SetValue(const QVariant&)", QVariant(5) );
*/
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
        xlsx.addSheet(sol.SolutionName);

        //Заполнение исходной таблицы
        //-----------------------------------------

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

        //-----------------------------------------


        //Заполнение расчетной таблицы
        //-----------------------------------------
        const int BeginY = sol.ProjectsNames.size() + 1 + 3;

        //Заполнение горизонтальных заголовков
        xlsx.write(BeginY, 1, "Проекты", format);
        for (int j=0; j < sol.IndicatorsNames.size(); j++)
            xlsx.write(BeginY, j+2, sol.IndicatorsNames[j], format);



        xlsx.setColumnWidth(sol.IndicatorsNames.size() + 2, xOffset *  QString("Жёсткий рейтинг").size());
        xlsx.setColumnWidth(sol.IndicatorsNames.size() + 3, xOffset *  QString("Мягкий рейтинг").size());


        xlsx.write(BeginY, sol.IndicatorsNames.size() + 2, "Жёсткий рейтинг", format);
        xlsx.write(BeginY, sol.IndicatorsNames.size() + 3, "Мягкий рейтинг", format);


        //Заполнение вертикальных заголовков
        for (int i = 0; i < sol.ProjectsNames.size(); i++)
            xlsx.write(BeginY + i + 1, 1, sol.ProjectsNames[i], format);


        //Заполнение расчетной таблицы
        for (int i = 0; i < sol.NormalizedTable.size(); i++)
            for (int j = 0; j < sol.NormalizedTable[0].size(); j++)
            {
                if (IsPaint)
                {
                    double k = sol.NormalizedTable[i][j] > 1 ? 1 : sol.NormalizedTable[i][j];
                    format1.setPatternBackgroundColor(QColor(qCeil(255 * (1 - k)), qCeil(k * 255), 0, 180));
                }
                xlsx.write(BeginY + i + 1, j + 2, sol.NormalizedTable[i][j], format1);
            }


        //Заполнение жесткого+мягкого рейтинга
        for (int i = 0; i < sol.HardRatings.size(); i++)
        {
            if (IsPaint)
            {
                k = sol.HardRatings[i];
                format1.setPatternBackgroundColor(QColor(qCeil(255 * (1 - k)), qCeil(k * 255), 0, 180));
            }
            xlsx.write(BeginY + i + 1, 1 + sol.IndicatorsNames.size() + 1, sol.HardRatings[i], format1);

            if (IsPaint)
            {
                k = sol.SoftRatings[i] > 1 ? 1 : sol.SoftRatings[i];
                format1.setPatternBackgroundColor(QColor(qCeil(255 * (1 - k)), qCeil(k * 255), 0, 180));
            }
            xlsx.write(BeginY + i + 1, 1 + sol.IndicatorsNames.size() + 2, sol.SoftRatings[i], format1);
        }

        //-----------------------------------------

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
        for (int i = 0; i < sol.PrefferedMetrics.size(); i++)
            for (int j = 0; j < sol.RejectedMetrics.size(); j++)
            {
                int length = QString("%1 > %2")
                        .arg(sol.IndicatorsNames[sol.PrefferedMetrics[i]])
                        .arg(sol.IndicatorsNames[sol.RejectedMetrics[j]]).size();

                max = length > max ? length : max;

                xlsx.write(row, BeginX + 2, QString("%1 > %2")
                           .arg(sol.IndicatorsNames[sol.PrefferedMetrics[i]])
                        .arg(sol.IndicatorsNames[sol.RejectedMetrics[j]]), format2);
                row++;
            }

        if (max == 0)
        {
            xlsx.mergeCells(QXlsx::CellRange(1, BeginX, 1, BeginX + 1), format);
        }
        else
        {
            xlsx.setColumnWidth(BeginX + 2, xOffset * max);
            xlsx.mergeCells(QXlsx::CellRange(1, BeginX, 1, BeginX + 2), format);
        }

        xlsx.write(1, BeginX, "Конфигурация", format);

        //------------------------------------------------------------------------------------------------------------
        format1.setPatternBackgroundColor(QColor(235, 241, 222));



        //Построение графиков
        //------------------------------------------------------------------------------------------------------------
        if (IsBuild)
        {
            Chart* chart = xlsx.insertChart(7 + 2 * sol.ProjectsNames.size() - 2, 1, QSize(700, 300));
            chart->setChartType(Chart::CT_Bar3D);
            chart->setChartStyle(9);

            int begY = 1 + sol.ProjectsNames.size() + 4;
            int begX = 1 + sol.IndicatorsNames.size() + 1;

            int endY = 1 + sol.ProjectsNames.size() + 4 + sol.ProjectsNames.size() - 1;
            int endX = 1 + sol.IndicatorsNames.size() + 1 + 1;

            chart->addSeries(CellRange(begY, begX, endY, endX));

            Chart *scatterChart = xlsx.insertChart(7 + 2 * sol.ProjectsNames.size()-2 + 17, 1, QSize(700, 300));
            scatterChart->setChartType(Chart::CT_Doughnut);
            scatterChart->addSeries(CellRange(begY, begX, endY, endX));
        }
        //------------------------------------------------------------------------------------------------------------

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


    //Жирный шрифт
    QFont font;
    font.setBold(true);


    //Цвет заливки
    QColor color;
    color.setRgb(255, 249, 222, alpha/2);

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
        params<<i;

    for (auto i:ProjectsNames)
        names<<i;

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
            }
            if (j!=0)
                input->setItem(i, j, new QTableWidgetItem(QString::number(IO::BaseTable[i][j-1])));
        }
    //------------------------------------------------------------------------------------------------


    output->setColumnCount(IndicatorsNames.size() + 3);
    output->setRowCount(ProjectsNames.size());

    params.insert(1, Hard);
    params.insert(1, Soft);

    //params<<Hard<<Soft;
    output->setHorizontalHeaderLabels(params);


    /*
     Формат горизонтальных заголовков
     ____________________________________________________________________________
     Проекты | Мягкий рейтинг | Жесткий рейтинг | Показатель1 | ... | Показательi

    */



    //Заполнение InputTable
    //------------------------------------------------------------------------------------------------
    for (int i = 0; i < IO::BaseTable.size(); i++)
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
            if (j == 1) {
                output->setItem(i, j,
                                new QTableWidgetItem(QString::number(round(DataProcessing::SoftRatings[i]*NumberAfterPoint)/NumberAfterPoint, 'f')));
            }

            //Заполнение жесткого рейтинга
            if (j == 2) {
                output->setItem(i, j,
                                new QTableWidgetItem(QString::number(round(DataProcessing::HardRatings[i]*NumberAfterPoint)/NumberAfterPoint, 'f')));
            }


            //Заполнение показателей
            if (j > 2)
            {
                double val = DataProcessing::NormalizedTable[i][j-3];

                val = round(val * NumberAfterPoint)/NumberAfterPoint;

                output->setItem(i, j, new QTableWidgetItem(QString::number(val, 'f')));
            }
        }
    //------------------------------------------------------------------------------------------------


    int columns = params.size();


    //Заполнение жёсткого и мягкого рейтинга
    //-------------------------------------------------------------------------------------------------------------------------------------
    //    for (int i = 0; i < IO::BaseTable.size(); i++)
    //    {
    //        output->setItem(i,columns-2,
    //                        new QTableWidgetItem(QString::number(round(DataProcessing::HardRatings[i]*NumberAfterPoint)/NumberAfterPoint, 'f')));
    //        output->setItem(i,columns-1,
    //                        new QTableWidgetItem(QString::number(round(DataProcessing::SoftRatings[i]*NumberAfterPoint)/NumberAfterPoint, 'f')));
    //    }
    //-------------------------------------------------------------------------------------------------------------------------------------



    //Растягивание по вертикали
    input->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);
    output->verticalHeader()->setSectionResizeMode(QHeaderView::Stretch);


    //Растягивание по горизонтали
    input->resizeColumnsToContents();
    output->resizeColumnsToContents();


    //Центрирование текста
    for (int row=0; row < input->rowCount(); row++)
        for (int column=0; column < input->columnCount(); column++)
            input->item(row, column)->setTextAlignment(Qt::AlignCenter);

    for (int row=0; row < output->rowCount(); row++)
        for (int column=0; column < output->columnCount(); column++)
            output->item(row, column)->setTextAlignment(Qt::AlignCenter);


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
            else
                color.setRgb(qCeil(255 * (1 - k)), qCeil(k * 255), 0, alpha);

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














