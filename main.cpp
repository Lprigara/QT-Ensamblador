#include <QCoreApplication>

#include <QTextStream>
#include <QDebug>
#include <QFile>

struct operacion{
    int modo;
    QString op;
};

operacion compararOpcode(QString string){
    operacion o;
    if(string == "ADD"){
        qDebug()<<"add";
        o.op = "0010";
        o.modo = 1;
    }
    else if(string == "SUB"){
        qDebug()<<"sub";
        o.op = "0011";
        o.modo = 1;
    }
    else if(string == "AND"){
        qDebug()<<"and";
        o.op = "0100";
        o.modo = 1;
    }
    else if(string == "OR"){
        qDebug()<<"or";
        o.op = "0101";
        o.modo = 1;
    }
    else if(string == "INPUT"){
        qDebug()<<"input";
        o.op = "011110";
        o.modo = 2;
    }
    else if(string == "OUTPUTREG"){
        qDebug()<<"outputreg";
        o.op = "001110";
        o.modo = 2;
    }
    else if(string == "OUTPUTMEM"){
        qDebug()<<"outputmem";
        o.op = "1100";
        o.modo = 3;
    }
    else if(string == "GOTO"){
        qDebug()<<"goto";
        o.op = "001001";
        o.modo = 4;
    }    
    else if(string == "NOP"){
        qDebug()<<"nop";
        o.op = "111110";
        o.modo = 4;
    }
    else if(string == "FIN"){
        qDebug()<<"fin";
        o.op = "111111";
        o.modo = 4;
    }
    else if(string == "LOAD"){
        qDebug()<<"load";
        o.op = "1000";
        o.modo = 5;
    }
    else if(string == "SHIFT"){
        qDebug()<<"shift";
        o.op = "0111";
        o.modo = 5;
    }
    else if(string == "BEQ"){
        qDebug()<<"beq";
        o.op = "001010";
        o.modo = 6;
    }
    else if(string == "BNE"){
        qDebug()<<"bne";
        o.op = "001011";
        o.modo = 6;
    }
    
   return o;
}

QString convertirBin(int operando, int modo){
    QString intToBin (QString::number( operando, 2 ));
    if(modo == 0){
        while(intToBin.length()<4){
            intToBin.prepend("0");
        }
    }
    else if(modo == 1){
        while(intToBin.length()<2){
            intToBin.prepend("0");
        }
    }
    else if(modo == 2){
        while(intToBin.length()<8){
            intToBin.prepend("0");
        }
    }
    else if(modo == 3){
        while(intToBin.length()<10){
            intToBin.prepend("0");
        }
    }
    return intToBin;
}

void codificacionDesdeFichero(QFile &file){
    QTextStream in(&file);
    QFile fich("cod.txt");
    fich.open(QFile::WriteOnly);
    QTextStream out(&fich);
    while(!in.atEnd()){
        operacion o;
        QString linea = in.readLine(); //Almacenamos cada linea para ir traduciendolas una a una
        QString instruccion, operando, numBinario, cadenaAux, codFinal;
        int modo, estado=0, i=0, j=0;

        //Nos quedamos con la operacion (ADD, SUB,..)
        while(linea[i] != ' '){
            instruccion[i]=linea[i];
            i++;
        }
        o=compararOpcode(instruccion);
        codFinal=o.op;
        modo=o.modo;

        if(modo == 1){ //16 bits -> Registro destino (4b) Registro OperandoA (4b) Registro OperandoB (4b) Opcode (4)
            while(estado<3){
                j=0;
                i++;
                //Se queda con los registros para despues poder traducirlo a binario
                while(linea[i] != ' ' && i!=linea.length()){
                    operando[j]=linea[i];
                    i++;j++;
                }
                //Si hay una R en la cadena, quita el primer elemento (sirve para casos de registros (R3, R4..))
                if(operando.contains('r', Qt::CaseInsensitive)){
                    operando.remove(0, 1);
                }
                int aux=operando.toInt();
                numBinario=convertirBin(aux, 0);
                cadenaAux.append(numBinario);
                estado++;
            }
            codFinal.prepend(cadenaAux);
            qDebug()<<codFinal;
            out<<codFinal<<endl;
        }
        else if(modo == 2){ //16 bits -> Libre (4b) Registro Destino (4b) Puerto (2b) Opcode (6b)
            cadenaAux = "0000";
            while(estado < 2){
                j=0;
                i++;
                while(linea[i] != ' ' && i!=linea.length()){
                    operando[j]=linea[i];
                    i++;j++;
                }

                //Si hay una R en la cadena, quita el primer elemento
                if(operando.contains('r', Qt::CaseInsensitive)){
                    operando.remove(0, 1);
                }
                int aux=operando.toInt();
                numBinario=convertirBin(aux,estado);
                cadenaAux.append(numBinario);
                estado++;
            }
            codFinal.prepend(cadenaAux);
            qDebug()<<codFinal;
            out<<codFinal<<endl;
        }
        else if(modo == 3){ //16 bits -> Inmediato (8b) Puerto (2b) Libre (2b) Opcode (4b)
            int bits=2;
            while(estado < 2){
                j=0;
                i++;
                while(linea[i] != ' ' && i!=linea.length()){
                    operando[j]=linea[i];
                    i++;j++;
                }
                int aux=operando.toInt();
                numBinario=convertirBin(aux,bits);
                cadenaAux.append(numBinario);
                estado++;
                bits --;
            }
            cadenaAux.append("00");
            codFinal.prepend(cadenaAux);
            qDebug()<<codFinal;
            out<<codFinal<<endl;
        }
        else if(modo == 4){ //16 bits -> Dirección de salto (10b) Opcode (6b)
            j=0;
            i++;
            while(linea[i] != ' ' && i!=linea.length()){
                operando[j]=linea[i];
                i++;j++;
            }
            int aux=operando.toInt();
            numBinario=convertirBin(aux,3);
            codFinal.prepend(numBinario);
            qDebug()<<codFinal;
            out<<codFinal<<endl;
        }
        else if(modo == 5){ //16 bits -> Registro (4b) Valor constante (8b) Opcode (4b)
            int bits=0;
            while(estado<2){
                j=0;
                i++;
                while(linea[i] != ' ' && i!=linea.length()){
                    operando[j]=linea[i];
                    i++;j++;
                }

                //Si hay una R en la cadena, quita el primer elemento
                if(operando.contains('r', Qt::CaseInsensitive)){
                    operando.remove(0, 1);
                }
                int aux=operando.toInt();
                numBinario=convertirBin(aux, bits);
                cadenaAux.append(numBinario);
                estado++;
                bits = bits + 2;
            }
            codFinal.prepend(cadenaAux);
            qDebug()<<codFinal;
            out<<codFinal<<endl;
        }
        else if(modo == 6){ //BEQ y BNE, llevan implicita una resta, que tambien se traducira
            while(estado<2){
                j=0;
                i++;
                while(linea[i] != ' '){
                    operando[j]=linea[i];
                    i++;j++;
                }
                //Si hay una R en la cadena, quita el primer elemento
                if(operando.contains('r', Qt::CaseInsensitive)){
                    operando.remove(0, 1);
                }
                int aux=operando.toInt();
                numBinario=convertirBin(aux, 0);
                cadenaAux.append(numBinario);
                estado++;
            }
            cadenaAux.prepend("1111");
            cadenaAux.append("0011");
            qDebug()<<cadenaAux;
            j=0;i++;
            while(linea[i] != ' ' && i!=linea.length()){
                operando[j]=linea[i];
                i++;j++;
            }
            int aux=operando.toInt();
            numBinario=convertirBin(aux, 3);
            codFinal.prepend(numBinario);
            qDebug()<<codFinal;
            out<<codFinal<<endl;
        }



    }
}

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    QFile file("file.txt");
    file.open(QIODevice::ReadOnly);
    codificacionDesdeFichero(file);


    return a.exec();
}


