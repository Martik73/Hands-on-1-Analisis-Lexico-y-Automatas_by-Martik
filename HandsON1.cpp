#include <iostream>
#include <string>
#include <vector>

using namespace std;

//Clase del AFD 
enum class EstadoAFD {
    INICIO,

    //MOV
    M,
    MO,
    MOV,
    MOV_ESPACIO,    
    MOV_A,
    MOV_AL,  //Ya se reconocio AL des pues de mov etc. cada uno de 
    MOV_B,                     //estos es un estado, más claro que "q1 etc."
    MOV_BL,
    MOV_COMA,
    MOV_ESPACIOS_NUMERO,
    MOV_NUMERO,

    //ADD
    
    A,
    AD,
    ADD,
    ADD_ESPACIO,
    ADD_A,
    ADD_AL,
    ADD_COMA,
    ADD_ESPACIO_HACIABL,
    ADD_B,
    ADD_BL,

    //STO

    S,
    ST,
    STO,
    STO_ESPACIO,
    STO_NUMERO,

    //END

    E,
    EN,
    END,

    ERROR
};

struct Token {
    string tipo;
    string lexema;
};

struct Instruccion {
    string operacion;
    vector<string> registros;
    int direccion = -1;  //Se utiliza menos uno para las instrucciones que no llevan dirección
};

Instruccion construirInstruccion(const vector<Token>& tokens) {

    Instruccion instruccion;

    for (const Token& token : tokens) {

        if (token.tipo == "MNEMONICO") {
            instruccion.operacion = token.lexema;
        }
        else if (token.tipo == "REGISTRO") {
            instruccion.registros.push_back(token.lexema);
        }
        else if (token.tipo == "NUMERO") {
            instruccion.direccion = stoi(token.lexema);  //Se cambia el numero string a int 
        }
    }

    return instruccion;
}

enum class EstadoMoore {
    INICIO,
    
    MOV_MAR,
    MOV_MBR,
    MOV_REGISTRO,

    ADD,

    STO_MAR,
    STO_MBR,
    STO_MEMORIA,

    END,

    FIN
};

class MaquinaMoore {
private:
    Instruccion instruccion;
    EstadoMoore estado;

public:

    MaquinaMoore(const Instruccion& inst) {
        instruccion = inst;
        estado = EstadoMoore::INICIO;
    }

    string siguientePaso() {

        switch (estado) {

            case EstadoMoore::INICIO:

                if (instruccion.operacion == "MOV") {
                    estado = EstadoMoore::MOV_MAR;
                    return "";
                }

                if (instruccion.operacion == "ADD") {
                    estado = EstadoMoore::ADD;
                    return "";
                }

                if (instruccion.operacion == "STO") {
                    estado = EstadoMoore::STO_MAR;
                    return "";
                }

                if (instruccion.operacion == "END") {
                    estado = EstadoMoore::END;
                    return "";
                }

                estado = EstadoMoore::FIN;
                return "";

            case EstadoMoore::MOV_MAR:
                estado = EstadoMoore::MOV_MBR;
                return "MAR <- " + to_string(instruccion.direccion);

            case EstadoMoore::MOV_MBR:
                estado = EstadoMoore::MOV_REGISTRO;
                return "MBR <- M[MAR]";

            case EstadoMoore::MOV_REGISTRO:
                estado = EstadoMoore::FIN;
                return instruccion.registros[0] + " <- MBR";

            case EstadoMoore::ADD:
                estado = EstadoMoore::FIN;
                return "ACC <- AL + BL";

            case EstadoMoore::STO_MAR:
                estado = EstadoMoore::STO_MBR;
                return "MAR <- " + to_string(instruccion.direccion);

            case EstadoMoore::STO_MBR:
                estado = EstadoMoore::STO_MEMORIA;
                return "MBR <- ACC";

            case EstadoMoore::STO_MEMORIA:
                estado = EstadoMoore::FIN;
                return "M[MAR] <- MBR";

            case EstadoMoore::END:
                estado = EstadoMoore::FIN;
                return "HALT <- 1";

            case EstadoMoore::FIN:
                return "";
        }

        return "";
    }

    bool terminado() {
        return estado == EstadoMoore::FIN;
    }
};

int main() {

    string inputUser, explainError = "Instrucción incompleta";
    vector<Token> tokens; //Un vector de varios tokens por linea 
    string lexemaNumero;

    cout << "\nIngrese una instruccion: ";
    getline(cin, inputUser);

    cout << "\nEntrada recibida: " << inputUser << endl;

    EstadoAFD estado = EstadoAFD::INICIO;

    for (char c : inputUser) {

        switch (estado) {

            case EstadoAFD::INICIO:

                if (c == 'M') {
                    estado = EstadoAFD::M;
                }
                else if(c == 'A'){
                    estado = EstadoAFD::A;
                }

                else if(c == 'S'){
                    estado = EstadoAFD::S;
                }
                else if(c == 'E'){
                    estado = EstadoAFD::E;
                }

                else {
                    estado = EstadoAFD::ERROR;
                    explainError = "Mnemónico no reconocido";
                }

                break;


            //ñññññññññññññññññññññññññññññññññññññññññññññññññññññññññññññññññññññ
            //MOV
            //ñññññññññññññññññññññññññññññññññññññññññññññññññññññññññññññññññññññ

            case EstadoAFD::M:

                if (c == 'O') {
                    estado = EstadoAFD::MO;
                }
                else {
                    estado = EstadoAFD::ERROR;
                    explainError = "Mnemónico incompleto, tienes <M>";
                }

                break;


            case EstadoAFD::MO:

                if (c == 'V') {
                    estado = EstadoAFD::MOV;
                    tokens.push_back({"MNEMONICO", "MOV"});
                }
                else {
                    estado = EstadoAFD::ERROR;
                    explainError = "Mnemónico incompleto, tienes <MO>";
                }

                break;


            case EstadoAFD::MOV:

                if (c == ' ') {
                    estado = EstadoAFD::MOV_ESPACIO;
                }
                else {
                    estado = EstadoAFD::ERROR;
                    explainError = "Falta espacio entre el Mnemónico y el registro";
                }

                break;


            case EstadoAFD::MOV_ESPACIO:

                if (c == ' ') {
                    estado = EstadoAFD::MOV_ESPACIO;
                }
            
                else if (c == 'A') {
                    estado = EstadoAFD::MOV_A;
                }
                else if (c == 'B') {
                    estado = EstadoAFD::MOV_B;
                }
                else {
                    estado = EstadoAFD::ERROR;
                    explainError = "El registro utilizado no es válido";
                }

                break;


            case EstadoAFD::MOV_A:

                if (c == 'L') {
                    estado = EstadoAFD::MOV_AL;
                    tokens.push_back({"REGISTRO", "AL"});
                }
                else {
                    estado = EstadoAFD::ERROR;
                    explainError = "Registro incompleto, tienes <A>";
                }

                break;


            case EstadoAFD::MOV_B:

                if (c == 'L') {
                    estado = EstadoAFD::MOV_BL;
                    tokens.push_back({"REGISTRO", "BL"});
                }
                else {
                    estado = EstadoAFD::ERROR;
                    explainError = "Registro incompleto, tienes <B>";
                }

                break;


            case EstadoAFD::MOV_AL:

                if (c == ' ') {
                    estado = EstadoAFD::MOV_AL;
                }
            
                else if (c == ',') {
                    estado = EstadoAFD::MOV_COMA;
                    tokens.push_back({"COMA", ","});
                }
                else {
                    estado = EstadoAFD::ERROR;
                    explainError = "Se espera una coma despues del registro <Al>";
                }

                break;


            case EstadoAFD::MOV_BL:

                if (c == ' ') {
                    estado = EstadoAFD::MOV_AL;
                }
                
                else if (c == ',') {
                    estado = EstadoAFD::MOV_COMA;
                    tokens.push_back({"COMA", ","});
                }
                else {
                    estado = EstadoAFD::ERROR;
                    explainError = "Se espera una coma despues del registro <BL>";
                }

                break;


            case EstadoAFD::MOV_COMA:

                if (c == ' ') {
                    estado = EstadoAFD::MOV_ESPACIOS_NUMERO;
                }
                else if (c >= '0' && c <= '9') {
                    estado = EstadoAFD::MOV_NUMERO;
                    lexemaNumero += c;
                }
                else {
                    estado = EstadoAFD::ERROR;
                    explainError = "Se espera una dirección de memoria despues de la coma";
                }

                break;


            case EstadoAFD::MOV_ESPACIOS_NUMERO:

                if (c == ' ') {
                    estado = EstadoAFD::MOV_ESPACIOS_NUMERO;
                }
                else if (c >= '0' && c <= '9') {
                    estado = EstadoAFD::MOV_NUMERO;
                    lexemaNumero += c;
                }
                else {
                    estado = EstadoAFD::ERROR;
                    explainError = "Se espera una dirección de memoria despues de la coma";
                }

                break;


            case EstadoAFD::MOV_NUMERO:

                if (c >= '0' && c <= '9') {
                    estado = EstadoAFD::MOV_NUMERO;
                    lexemaNumero += c;
                }
                else {
                    estado = EstadoAFD::ERROR;
                    explainError = "Se espera una dirección de memoria despues de la coma";
                }

                break;


            //ñññññññññññññññññññññññññññññññññññññññññññññññññññññññññññññññññññññ
            //ADD
            //ñññññññññññññññññññññññññññññññññññññññññññññññññññññññññññññññññññññ

            case EstadoAFD::A:

                if (c == 'D') {
                    estado = EstadoAFD::AD;
                }

                else {
                    estado = EstadoAFD::ERROR;
                    explainError = "Mnemónico incompleto, tienes <A>";
                }

                break;
            
            case EstadoAFD::AD:

                if (c == 'D') {
                    estado = EstadoAFD::ADD;
                    tokens.push_back({"MNEMONICO", "ADD"});
                }

                else {
                    estado = EstadoAFD::ERROR;
                    explainError = "Mnemónico incompleto, tienes <AD>";
                }

                break;
            
            case EstadoAFD::ADD:

                if (c == ' ') {
                    estado = EstadoAFD::ADD_ESPACIO;
                }

                else {
                    estado = EstadoAFD::ERROR;
                    explainError = "Se espera un espacio entre el Mnemónico y el registro";
                }

                break;

            case EstadoAFD::ADD_ESPACIO:

                if (c == ' ') {
                    estado = EstadoAFD::ADD_ESPACIO;
                }
            
                else if (c == 'A') {
                    estado = EstadoAFD::ADD_A;
                }

                else {
                    estado = EstadoAFD::ERROR;
                    explainError = "Registro no valido despues de ADD";
                }

                break;
            
            case EstadoAFD::ADD_A:

                if (c == 'L') {
                    estado = EstadoAFD::ADD_AL;
                    tokens.push_back({"REGISTRO", "AL"});
                }

                else {
                    estado = EstadoAFD::ERROR;
                    explainError = "Registro incompleto, tienes <A>";
                }

                break;

            case EstadoAFD::ADD_AL:

                if (c == ' ') {
                    estado = EstadoAFD::ADD_AL;
                }
            
                else if (c == ',') {
                    estado = EstadoAFD::ADD_COMA;
                    tokens.push_back({"COMA", ","});
                }

                else {
                    estado = EstadoAFD::ERROR;
                    explainError = "Se espera una coma despues del registro";
                }

                break;
            
            case EstadoAFD::ADD_COMA:

                if (c == ' ') {
                    estado = EstadoAFD::ADD_ESPACIO_HACIABL;
                }

                else if (c == 'B') {
                    estado = EstadoAFD::ADD_B;
                }

                else {
                    estado = EstadoAFD::ERROR;
                    explainError = "Registro no reconocido despues de la coma";
                }

                break;
            
            case EstadoAFD::ADD_ESPACIO_HACIABL:

                if (c == ' ') {
                    estado = EstadoAFD::ADD_ESPACIO_HACIABL;
                }

                else if(c == 'B'){
                    estado = EstadoAFD::ADD_B;
                }

                else {
                    estado = EstadoAFD::ERROR;
                    explainError = "Registro no reconocido despues de la coma";
                }

                break;
            
            case EstadoAFD::ADD_B:

                if (c == 'L') {
                    estado = EstadoAFD::ADD_BL;
                    tokens.push_back({"REGISTRO", "BL"});
                }

                else {
                    estado = EstadoAFD::ERROR;
                    explainError = "Registro incompleto, tienes <B>";
                }

                break;
            
            
            //ñññññññññññññññññññññññññññññññññññññññññññññññññññññññññññññññññññññ
            //STO
            //ñññññññññññññññññññññññññññññññññññññññññññññññññññññññññññññññññññññ
            
                case EstadoAFD::S:

                if (c == 'T') {
                    estado = EstadoAFD::ST;
                }
                else {
                    estado = EstadoAFD::ERROR;
                    explainError = "Mnemónico incompleto, tienes <S>";
                }

                break;
            
            case EstadoAFD::ST:

                if (c == 'O') {
                    estado = EstadoAFD::STO;
                    tokens.push_back({"MNEMONICO", "STO"});
                }
                else {
                    estado = EstadoAFD::ERROR;
                    explainError = "Mnemónico incompleto, tienes <ST>";
                }

                break;

            case EstadoAFD::STO:

                if (c == ' ') {
                    estado = EstadoAFD::STO_ESPACIO;
                }
                else {
                    estado = EstadoAFD::ERROR;
                    explainError = "Se espera un espacio despues del Mnemónico";
                }

                break;
            
            case EstadoAFD::STO_ESPACIO:

                if (c == ' ') {
                    estado = EstadoAFD::STO_ESPACIO;
                }
                else if (c >= '0' && c <= '9') {
                    estado = EstadoAFD::STO_NUMERO;
                    lexemaNumero += c;
                }
                else {
                    estado = EstadoAFD::ERROR;
                    explainError = "Se espera una dirección de memoria despues del Mnemónico";
                }

                break;
            
            case EstadoAFD::STO_NUMERO:

                if (c >= '0' && c <= '9') {
                    estado = EstadoAFD::STO_NUMERO;
                    lexemaNumero += c;
                }
                else {
                    estado = EstadoAFD::ERROR;
                    explainError = "Se espera una dirección de memoria despues del Mnemónico";
                }

                break;

            //ñññññññññññññññññññññññññññññññññññññññññññññññññññññññññññññññññññññ
            //End
            //ñññññññññññññññññññññññññññññññññññññññññññññññññññññññññññññññññññññ
            
            case EstadoAFD::E:

                if (c == 'N') {
                    estado = EstadoAFD::EN;
                }

                else {
                    estado = EstadoAFD::ERROR;
                    explainError = "Mnemónico incompleto, tienes <E>";
                }

                break;
            
            case EstadoAFD::EN:

                if (c == 'D') {
                    estado = EstadoAFD::END;
                    tokens.push_back({"MNEMONICO", "END"});
                }

                else {
                    estado = EstadoAFD::ERROR;
                    explainError = "Mnemónico incompleto, tienes <EN>";
                }

                break;

            case EstadoAFD::END:

                if (c == ' ' || c >= '0' && c <= '9') {
                    estado = EstadoAFD::ERROR;
                    explainError = "Se espera el mnemónico sin caracteres adicionales ";
                }
                break;


            case EstadoAFD::ERROR:
                break;
        }

        if (estado == EstadoAFD::ERROR) {
            break;
        }
    }


    if (estado == EstadoAFD::MOV_NUMERO || estado == EstadoAFD::ADD_BL ||
        estado == EstadoAFD::STO_NUMERO ||estado == EstadoAFD::END) {

    
        cout << "\n\nInstruccion valida." << endl;

        if(estado == EstadoAFD::MOV_NUMERO || estado == EstadoAFD::STO_NUMERO){
            tokens.push_back({"NUMERO", lexemaNumero});
        }

        cout << "\nTokens reconocidos:\n";

        for (const Token& token : tokens) {
            cout << "Tipo: " << token.tipo << " | Lexema: " << token.lexema << endl;
        }

        Instruccion instruccion = construirInstruccion(tokens);

        cout << "\nObjeto instruccion:\n";
        cout << "Operacion: " << instruccion.operacion << endl;

        cout << "Registros: ";
        for (const string& registro : instruccion.registros) {
            cout << registro << " ";
        }
        cout << endl;

        cout << "Direccion: " << instruccion.direccion << endl;
        
        MaquinaMoore moore(instruccion);

        cout << "\nMicrooperaciones:\n";

        while (!moore.terminado()) {
            string microoperacion = moore.siguientePaso();

            if (!microoperacion.empty()) {
                cout << microoperacion << endl;
            }
        }
    }   

    else {
        cout << "Instruccion invalida." << endl;
        cout << explainError << endl;
    }

    return 0;
}
