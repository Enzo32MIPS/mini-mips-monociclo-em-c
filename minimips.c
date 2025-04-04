#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdbool.h>

void* memCheck(void* a);

typedef struct sgnp{
    bool Mem2Reg;
    bool MemWrite;
    bool branch;
    int AluFunc;
    bool AluSrc;
    bool RegDst;
    bool RegWrite;
    bool jump;
    char name[5];

    bool overflow;

}control_signal;


control_signal* uc(unsigned int inst, unsigned int function);
void instruction_name_finder(unsigned int inst, unsigned int function, char* name);



typedef struct codigo{
    char instrucao[20];
    int opcode;
    int rs;
    int rd;
    int rt;
    int imm;
    int funct;
    char tipo;
}inst;

int type;

inst *cria_mem();
void ler_mem(inst *mem_lida);
int binario_para_decimal(char binario[], int inicio, int fim, int complemento2);
control_signal* regis(unsigned int a, char *instruction);

int ula(int reg1, int reg2, control_signal *controle);
int mux(int valor1, int valor2, bool controle);
int *BancoReg(int rd, int rs, int rt, int imm, bool RegWrite, control_signal *controle);
int *decod(unsigned int a, bool RegWrite, control_signal *controle);

int main(){
    inst *inst_mem = cria_mem();
    ler_mem(inst_mem);
    unsigned int a;

    /*for(int i=0; i<10;i++){}*/
        a = binario_para_decimal(inst_mem[0].instrucao,0,15,0);

        control_signal* csignal = uc((a>>12)&15,a&7);
        printf("%s\n",csignal->name);
        int *regpon = decod(a, csignal->RegWrite, csignal);

        printf("\n\n");


    return 0;
}

inst *cria_mem(){

    FILE *arq;
    arq = fopen("instrucoes.mem","r");
    if(arq == NULL){
        printf("ERRO NA LEITURA DA MEMORIA DE INSTRUCOES\n");
        exit(-1);
    };

    int i=0;
    char temp[30];

    while(!feof(arq)){
        fgets(temp,30,arq);
        i++;
    }

    inst *mem = (inst*)malloc(sizeof(inst)*i);

    fclose(arq);

    return mem;
};

void ler_mem(inst *mem_lida){
    FILE *arq;
    arq = fopen("instrucoes.mem","r");
    if(arq == NULL){
        printf("ERRO NA LEITURA DA MEMORIA DE INSTRUCOES\n");
        exit(-1);
    };

    int i=0;

    while(!feof(arq)){
        fgets(mem_lida[i].instrucao,sizeof(mem_lida->instrucao),arq);
        i++;
    }

    fclose(arq);
};

int binario_para_decimal(char binario[], int inicio, int fim, int complemento2) {
    int decimal = 0;
    int tamanho = strlen(binario);

    if (inicio < 0 || fim >= tamanho || inicio > fim) {
        printf("Índices inválidos.\n");
        return -1;
    }

    if (complemento2 == 1) {

        if (binario[inicio] == '1') {
            int inversao = 0;
            for (int i = inicio; i <= fim; i++) {
                if (binario[i] == '0') {
                    inversao += pow(2, fim - i);
                }
            }

            decimal = inversao + 1;
            decimal = -decimal;
        } else {
            for (int i = inicio; i <= fim; i++) {
                if (binario[i] == '1') {
                    decimal += pow(2, fim - i);
                }
            }
        }
    } else {
        for (int i = inicio; i <= fim; i++) {
            if (binario[i] == '1') {
                decimal += pow(2, fim - i);
            }
        }
    }

    return decimal;
};

int *decod(unsigned int a, bool RegWrite, control_signal *controle){

    int rs=0, rd=0, rt=0, funct=0, imm=0, opcode=0;

    opcode = ((a>>12)&15);
    if (type == 0){
        printf("INSTRUCAO DE TIPO R\n");
        printf("opcode: %i\trs: %i\t rt: %i\t rd: %i\t function: %i\n",((a>>12)&15),((a>>9)&7),((a>>6)&7),((a>>3)&7),((a>>0)&7));
        rs = ((a>>9)&7);
        rt = ((a>>6)&7);
        rd = ((a>>3)&7);
        funct = ((a>>0)&7);
    } else if (type == 1){
        printf("INSTRUCAO DE TIPO I\n");
        printf("opcode: %i\trs: %i\trt: %i\timediato: %i\n",((a>>12)&15),((a>>9)&7),((a>>6)&7),((a>>0)&63));
        rs = ((a>>9)&7);
        rt = ((a>>6)&7);
        rd = ((a>>6)&7);
        imm = ((a>>0)&63);
    } else if (type == 2){
        printf("INSTRUCAO DE TIPO J\n");
        printf("opcode: %i\timediato: %i\n",((a>>26)&63),((a>>0)&2047));
        imm = (a&255);
    }
    int *result = BancoReg(rd,rs,rt,imm,RegWrite,controle);
    return result;
}

int *BancoReg(int rd, int rs, int rt, int imm, bool RegWrite, control_signal *controle){

    int reg[8];
    int *regpon = reg;

    if (RegWrite == true && type == 0){
        reg[rd] = ula(rs,rt,controle);
    }
    if (RegWrite == true && type == 1){
        reg[rd] = ula(rs,imm,controle);
    }

    return regpon;
}

control_signal* uc(unsigned int inst, unsigned int function){
    control_signal* result=(control_signal*)memCheck(malloc(sizeof(control_signal)));

    if(inst!=11){result->Mem2Reg=true;}
    else{result->Mem2Reg=false;}

    if(inst==15){result->MemWrite=true;}
    else{result->MemWrite=false;}

    if(inst==8){result->branch=true;}
    else{result->branch=false;}

    if((inst&5)!=0){result->AluSrc=true;}
    else{result->AluSrc=false;}

    if(((inst&10)==0)||((inst&5)==1)){result->RegWrite=true;}
    else{result->RegWrite=false;}

    if(inst==2){result->jump=true;}
    else{result->jump=false;}

    if(0<=inst && inst<2){
        result->AluFunc=function;
    }else if(1<inst && inst<8){
        result->AluFunc=2;
    }else if(7<inst && inst<10){
        result->AluFunc=1;
    }else if(9<inst && inst<16){
        result->AluFunc=0;
    }else{
        return NULL;
    }

    instruction_name_finder(inst,function,result->name);
    return result;
}
void instruction_name_finder(unsigned int inst, unsigned int function, char* name){

    switch(inst){
        case 0:
            type = 0;
            switch(function){
                case 6:
                    strcpy(name,"add\0");
                    break;
                case 1:
                    strcpy(name,"sub\0");
                    break;
                case 3:
                    strcpy(name,"and\0");
                    break;
                case 5:
                    strcpy(name,"or\0");
                    break;
                case 7:
                    strcpy(name,"zero\0");
                    break;
            }
            break;
        case 2:
            strcpy(name,"j\0");
            type = 2;
            break;
        case 4:
            strcpy(name,"addi\0");
            type = 1;
            break;
        case 8:
            strcpy(name,"beq\0");
            type = 1;
            break;
        case 11:
            strcpy(name,"lw\0");
            type = 1;
            break;
        case 15:
            strcpy(name,"sw\0");
            type = 1;
            break;
        default:
            exit(1);
            break;
    }
    return;
}

void* memCheck(void* a){
    if(!a){
        exit(2);
    }
    return a;
}

int ula(int reg1, int reg2, control_signal *controle){

    bool zero_flag;
    switch(controle->AluFunc){
        case 0:

            if(reg1+reg2>127 || reg1+reg2<-128){
                controle->overflow = 1;
            }
            if(reg1|reg2==0) zero_flag=1; else zero_flag=0;
            return reg1 + reg2;


            break;
        case 1:

            if(reg1==reg2){
                controle->overflow = 1;
            }
            else{
                controle->overflow = 0;
            }

            break;
        case 2:

            if(reg1+reg2>127 || reg1+reg2<-128){
                controle->overflow = 1;
            }

            if(reg1|reg2==0) zero_flag=1; else zero_flag=0;
            return reg1 + reg2;

            break;
        case 4:
            if(reg1|reg2==0) zero_flag=1; else zero_flag=0;
            return reg1 & reg2;

            break;
        case 5:
            if(reg1|reg2==0) zero_flag=1; else zero_flag=0;
            return reg1 | reg2;

            break;
    }


};

int mux(int valor1, int valor2, bool controle){
    switch(controle){
        case 0:
            return valor1;
            break;
        case 1:
            return valor2;
            break;
    };

};

