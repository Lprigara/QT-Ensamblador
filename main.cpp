#include <QCoreApplication>
#include <QTextStream>
#include <QDebug>
#include <QFile>
#include <QStringList>

QString num2bin(uint num, int length)
{
    //Convert decimal to binary
    QString binary_string = QString::number(num,2);

    //prepend missing 0 for fixed length
    while(binary_string.length() < length)
    {
        binary_string.prepend("0");
    }

    return binary_string;
}

QString parseNum(QString input, int length)
{
    //Remove eventually leading R
    if(input.contains('r', Qt::CaseInsensitive)){
        input.remove(0, 1);
    }

    //Binary indicated by b
    if(input.startsWith("b"))
        return input.mid(1, -1);


    //Name indicated by x
    if(input.startsWith("x"))
        return input.mid(1, -1);


    //Decimal may be indicated by d
    QString num_decimal;

    if (input.startsWith("d"))
        num_decimal = input.mid(1,-1);
    else num_decimal = input;

    uint num = num_decimal.toUInt();

    //Convert decimal to binary
    QString binary_string = num2bin(num, length);

    return binary_string;

}

QString parseDir(QString input, int length, int lineNumber, int lines = 1)
{
    if (input.compare("SELF",Qt::CaseInsensitive) == 0)
        return num2bin(lineNumber,length);

    //Name indicated by x
    if(input.startsWith("x"))
        return input.mid(1, -1);

    bool negative = false;
    if (input.startsWith("-"))
    {
        negative = true;
        input.remove(0,1);
    }
    else if (input.startsWith("+")) input.remove(0,1);


    //Process number
    input = parseNum(input, length);

    //Reconvert to int
    bool ok;
    int num = input.toInt(&ok,2);

    //Add or subtract
    if (negative) num = lineNumber - num + (lines - 1); //lines == number of lines the command produces, e.g. BNE produces 2 lines, which needs to be considered here
    else num = lineNumber + num;

    //Convert decimal to binary
    return num2bin(num,length);
}

void processAndConvert(QFile &file){

    //Input textstream from file
    QTextStream in(&file);

    //Create output file
    QFile fileOutput("progfile.dat");
    fileOutput.open(QFile::WriteOnly);

    //Output textstream
    QTextStream out(&fileOutput);

    unsigned int lineNumber = 0;

    bool showLineNum = false;

    while(!in.atEnd()){
        QString line = in.readLine(); //Read next line


        //Empty lines
        if (line.length() == 0)
        {
            out<<endl;
            continue;
        }

        //Comment line
        if (line.startsWith("//") || line.startsWith("#"))
        {
            if (!showLineNum)
                showLineNum = line.startsWith("###");
            qDebug()<<line;
            out<<line<<endl;
            continue;
        }


        //Split command and arguments
        QStringList lineSplit = line.split(" ", QString::SkipEmptyParts);
        QString cmd = lineSplit.first();
        lineSplit.removeFirst();
        QStringList args = lineSplit;


        QString opcode;

        if (cmd == "ADD")
        {
            opcode = "0010";
            QString reg_dest = parseNum(args[0],4);
            QString reg_op1 = parseNum(args[1],4);
            QString reg_op2 = parseNum(args[2],4);
            out<<reg_dest<<reg_op1<<reg_op2<<opcode;
            qDebug()<<lineNumber<<"ADD"<<reg_dest<<reg_op1<<reg_op2<<opcode;
        }
        else if (cmd == "SUB")
        {
            opcode = "0011";
            QString reg_dest = parseNum(args[0],4);
            QString reg_op1 = parseNum(args[1],4);
            QString reg_op2 = parseNum(args[2],4);
            out<<reg_dest<<reg_op1<<reg_op2<<opcode;
            qDebug()<<lineNumber<<"SUB"<<reg_dest<<reg_op1<<reg_op2<<opcode;
        }
        else if (cmd == "AND")
        {
            opcode = "0100";
            QString reg_dest = parseNum(args[0],4);
            QString reg_op1 = parseNum(args[1],4);
            QString reg_op2 = parseNum(args[2],4);
            out<<reg_dest<<reg_op1<<reg_op2<<opcode;
            qDebug()<<lineNumber<<"AND"<<reg_dest<<reg_op1<<reg_op2<<opcode;
        }
        else if (cmd == "OR")
        {
            opcode = "0101";
            QString reg_dest = parseNum(args[0],4);
            QString reg_op1 = parseNum(args[1],4);
            QString reg_op2 = parseNum(args[2],4);
            out<<reg_dest<<reg_op1<<reg_op2<<opcode;
            qDebug()<<lineNumber<<"OR"<<reg_dest<<reg_op1<<reg_op2<<opcode;
        }
        else if (cmd == "SHIFT")
        {
            opcode = "0111";
            QString reg_dest = parseNum(args[0],4);
            QString reg_op1 = parseNum(args[1],4);
            QString reg_op2 = "0000";
            out<<reg_dest<<reg_op1<<reg_op2<<opcode;
            qDebug()<<lineNumber<<"SHIFT"<<reg_dest<<reg_op1<<reg_op2<<opcode;
        }
        else if (cmd == "OUTPUTREG")
        {
            opcode = "001110";
            QString dc = "0000";
            QString reg_origin = parseNum(args[0],4);
            QString port = parseNum(args[1],2);
            out<<dc<<reg_origin<<port<<opcode;
            qDebug()<<lineNumber<<"OUTPUTREG"<<dc<<reg_origin<<port<<opcode;
        }
        else if (cmd == "OUTPUTMEM")
        {
            opcode = "1100";
            QString value = parseNum(args[0],8);
            QString port = parseNum(args[1],2);
            QString dc = "00";
            out<<value<<port<<dc<<opcode;
            qDebug()<<lineNumber<<"OUTPUTMEM"<<value<<port<<dc<<opcode;
        }
        else if (cmd == "GOTO")
        {
            opcode = "001001";
            QString dir = parseDir(args[0],10,lineNumber);
            out<<dir<<opcode;
            qDebug()<<lineNumber<<"GOTO"<<dir<<opcode;
        }
        else if (cmd == "NOP")
        {
            opcode = "111110"; //or jumpt to next line?
            qDebug()<<"NOP: NOT IMPLEMENTED";
        }
        else if (cmd == "FIN")
        {
            opcode = "111111";
            QString dc = "0000000000";
            out<<dc<<opcode;
            qDebug()<<lineNumber<<"GOTO"<<dc<<opcode;
        }
        else if (cmd == "LOAD")
        {
            opcode = "1000";
            QString reg_dest = parseNum(args[0],4);
            QString value = parseNum(args[1],8);
            out<<reg_dest<<value<<opcode;
            qDebug()<<lineNumber<<"LOAD"<<reg_dest<<value<<opcode;
        }
        else if (cmd == "BEQ")
        {
            opcode = "001010";
            //subtract arg1 and arg2
            QString reg_op1 = parseNum(args[0],4);
            QString reg_op2 = parseNum(args[1],4);
            QString dir = parseDir(args[2],10,lineNumber, 2);
            QString sub_dest = "1111";
            QString sub_opcode = "0011";
            out<<sub_dest<<reg_op1<<reg_op2<<sub_opcode;
            if (showLineNum)
            {
                out <<"\tLine: "<< lineNumber << "\t" << num2bin(lineNumber,10);
                showLineNum = false;
            }
            out<<endl;
            qDebug()<<lineNumber<<"BEQ-SUB"<<sub_dest<<reg_op1<<reg_op2<<sub_opcode;
            lineNumber++;
            //jump on eq0
            out<<dir<<opcode;
            qDebug()<<lineNumber<<"BEQ-JUMP"<<dir<<opcode;
        }
        else if (cmd == "BNE")
        {
            opcode = "001011";
            //subtract arg1 and arg2
            QString reg_op1 = parseNum(args[0],4);
            QString reg_op2 = parseNum(args[1],4);
            QString dir = parseDir(args[2],10,lineNumber, 2);
            QString sub_dest = "1111";
            QString sub_opcode = "0011";
            out<<sub_dest<<reg_op1<<reg_op2<<sub_opcode;
            if (showLineNum)
            {
                out <<"\tLine: "<< lineNumber << "\t" << num2bin(lineNumber,10);
                showLineNum = false;
            }
            out<<endl;
            qDebug()<<lineNumber<<"BNE-SUB"<<sub_dest<<reg_op1<<reg_op2<<sub_opcode;
            lineNumber++;
            //jump on eq0
            out<<dir<<opcode;
            qDebug()<<lineNumber<<"BNE-JUMP"<<dir<<opcode;
        }
        else if (cmd == "INPUT")
        {
            opcode = "011110";
            QString reg_dest = parseNum(args[0],4);
            QString dc = "0000";
            QString port = parseNum(args[1],2);
            out<<reg_dest<<dc<<port<<opcode;
            qDebug()<<lineNumber<<"INPUT"<<reg_dest<<dc<<port<<opcode;
        }
        else if (cmd == "CLK")
        {
            opcode = "111001";
            QString dc = "0000000000";
            out<<dc<<opcode;
            qDebug()<<lineNumber<<"CLK"<<dc<<opcode;
        }
        else {
            qDebug()<<"Unknown command:"<<cmd<<args;
        }


        //Handle comments at ends of lines
        QString comments;
        if(line.contains("//"))
        {
            int index = line.indexOf("//");
            comments = line.mid(index,-1);
        }
        else if(line.contains("#"))
        {
            int index = line.indexOf("#");
            comments = line.mid(index,-1);
        }

        if (showLineNum)
        {
            out <<"\tLine: "<< lineNumber << "\t" << num2bin(lineNumber,10);
            showLineNum = false;
        }

        //Add comments if existing
        if (comments.length() > 0)
        {
            if(comments.contains("SHOWNUM", Qt::CaseInsensitive))
            {
                out<<"\tLine: "<< lineNumber << "\t" << num2bin(lineNumber,10) << "\t"<<comments;
            }
            else
            {
                out<<"\t"<<comments;
            }
        }

        //End line
        out<<endl;
        lineNumber++;

    }//while !atEnd


    fileOutput.close();
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QFile file("input.txt");
    file.open(QIODevice::ReadOnly);
    processAndConvert(file);
    file.close();

    return 0;
}
