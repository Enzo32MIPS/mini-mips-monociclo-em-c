#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include "minimips.h"

#include <termios.h>

int main(int argc, char** argv){

    if(argc<2) return 1;

    int8_t reg[8] = {0};
    inst instruction_mem[256] = {0};
    int8_t data_mem[256] = {0};
    uint8_t pc=0;
	control_signal* csignal;
    int8_t aluIn, result;
	ula_signal* usignal;

    ler_mem(instruction_mem,argv[1]);

	char casee=0;
	do{
		switch(casee){
			case 'n':
			    decod(instruction_mem+pc);

				csignal = uc((unsigned int)instruction_mem[pc].opcode,(unsigned int)instruction_mem[pc].funct);

		 		if( csignal->RegDst==0 ) instruction_mem[pc].rd = instruction_mem[pc].rt;

			    if( !csignal->AluSrc ) aluIn = reg[instruction_mem[pc].rt];
				else aluIn = instruction_mem[pc].imm;

		    	usignal = ula((int16_t)reg[instruction_mem[pc].rs],(int16_t)aluIn,csignal->AluFunc);

				if( csignal->MemWrite == 1 ) data_mem[usignal->result] = reg[instruction_mem[pc].rd];

				if( !csignal->Mem2Reg ) result = data_mem[usignal->result];
				else result = usignal->result;

				if( csignal->RegWrite == 1 ) reg[instruction_mem[pc].rd] = result;

				if( csignal->jump == 1 ) pc = instruction_mem[pc].addr;
				else pc++;

				if( csignal->branch && usignal->zero_flag ) pc += instruction_mem[pc].imm;

				free(csignal);
				free(usignal);
			break;

			case 'm':
				for(int i=0;i<16;i++){
					for(int j=0;j<16;j++){
						printf("|%i",data_mem[16*i+j]);
					}
					printf("|\n");
				}
				printf("\n");
			break;

			case 'r':
				for(int i=0;i<16;i++){
					printf("|%i",reg[i]);
				}
				printf("\n\n");
			break;

            case 'i':
                for(int i=0;i<256;i++){
                    decod(instruction_mem+i);
                    csignal = uc(instruction_mem[i].opcode,instruction_mem[i].funct);
                    printf("Num:%u | %s ",i, csignal->name);
                    if( !csignal->jump ){
                        printf("$%u $%u ",instruction_mem[i].rs,instruction_mem[i].rt);
                        if( !csignal->RegDst ) printf("imm %i\n", instruction_mem[i].imm);
                        else printf("$%u\n",instruction_mem[i].rd);
                    }
                    else printf("%u\n",instruction_mem[i].addr);
                    free(csignal);
                }
            break;

            case 's':
                decod(instruction_mem+pc);
                csignal = uc(instruction_mem[pc].opcode,instruction_mem[pc].funct);
                printf("PC:%u | %s ",pc, csignal->name);
                if( !csignal->jump ){
                    printf("$%u $%u ",instruction_mem[pc].rs,instruction_mem[pc].rt);
                    if( !csignal->RegDst ) printf("imm %i\n", instruction_mem[pc].imm);
                    else printf("$%u\n",instruction_mem[pc].rd);
                }
                else printf("%u\n",instruction_mem[pc].addr);
                free(csignal);
			break;
		}
		printf("n)step\nm)show data memory\nr)show registers\ni)show all instructions\ns)show intruction to run\n0)quit\n:");
		do scanf("%c",&casee); while(casee=='\n');
	}while(casee!='0');
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

void ler_mem(inst *mem_lida, const char* name){
    FILE *arq;
    arq = fopen(name,"r");
    char temp[20];

    if(arq == NULL){
        printf("ERRO NA LEITURA DA MEMORIA DE INSTRUCOES\n");
        exit(2);
    };

    for(int i=0;!feof(arq);i++){
        fgets(temp,sizeof(char[20]),arq);
        mem_lida[i].instrucao = (uint16_t)binario_para_decimal(temp,0,15,0);
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
                    inversao += 1<<(fim - i);
                }
            }

            decimal = inversao + 1;
            decimal = -decimal;
        } else {
            for (int i = inicio; i <= fim; i++) {
                if (binario[i] == '1') {
                    decimal += 1<<(fim - i);
                }
            }
        }
    } else {
        for (int i = inicio; i <= fim; i++) {
            if (binario[i] == '1') {
                decimal += 1<<(fim - i);
            }
        }
    }

    return decimal;
};

void decod(inst* a){

    union{
        uint8_t u;
        int8_t s;
    }conv;

    a->opcode = ((a->instrucao>>12)&15);
    a->rs = ((a->instrucao>>9)&7);
    a->rt = ((a->instrucao>>6)&7);
    a->rd = ((a->instrucao>>3)&7);
	conv.u = (a->instrucao)&63;
	if( (conv.u&32) == 32) conv.u = conv.u | 192;
	a->imm = conv.s;
    a->funct = ((a->instrucao>>0)&7);
	a->addr = ((a->instrucao>>0)&255);
    return;
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

//regdst
	if((inst&12)==0) result->RegDst=true;
	else result->RegDst;

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
            switch(function){
                case 0:
				case 2:
				case 4:
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
            break;
        case 4:
            strcpy(name,"addi\0");
            break;
        case 8:
            strcpy(name,"beq\0");
            break;
        case 11:
            strcpy(name,"lw\0");
            break;
        case 15:
            strcpy(name,"sw\0");
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

ula_signal* ula(int16_t reg1, int16_t reg2, uint8_t funct){

    ula_signal* result=calloc(1,sizeof(ula_signal));
	result->overflow=0;

    switch(funct){
        case 0:
        case 2:
        case 3:
        case 6:
            if(reg1+reg2>127 || reg1+reg2<-128) result->overflow = 1;
            result->result = (int8_t) reg1 + reg2;
        break;

        case 1:
            if(reg1-reg2>127 || reg1-reg2<-128) result->overflow = 1;
            result->result = (int8_t) reg1 - reg2;
        break;

        case 4:
            result->result = (int8_t) reg1 & reg2;
        break;

        case 5:
            result->result = (int8_t) reg1 | reg2;
        break;

        case 7:
            result->result = 0;
        break;
    }
    result->zero_flag = result->result!=0;
    return result;
};

/*int mux(int valor1, int valor2, bool controle){
    switch(controle){
        case 0:
            return valor1;
            break;
        case 1:
            return valor2;
            break;
    };

};
*/
