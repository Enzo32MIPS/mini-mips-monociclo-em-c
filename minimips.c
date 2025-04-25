#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include "minimips.h"

int main(int argc, char** argv){

	char fileN[64];
    int8_t reg[8] = {0};
    inst instruction_mem[256] = {0};
    int8_t data_mem[256] = {0};
    uint8_t pc=0;
	control_signal* csignal;
    int8_t aluIn, result;
	ula_signal* usignal;
    estado* state;
    estado teste;
    state = &teste;

    if(argc>1){
		strcpy(fileN,argv[1]);
	    ler_mem(instruction_mem,fileN);
	}

	char temp[30];

	char casee=0;
	do{
		switch(casee){
			case '1':

                pc = exec(pc, reg, instruction_mem, data_mem, csignal, aluIn, result, usignal, state, teste);

                printf("\n");
                for(int i=0;i<16;i++){
                    for(int j=0;j<16;j++){
                        printf("|%i",data_mem[16*i+j]);
                    }
                    printf("|\n");
                }
                printf("\n");

                for(int i=0;i<8;i++){
                    printf("|%i",reg[i]);
                }
                printf("|\n\n");

                decod(instruction_mem+pc);
                csignal = uc(instruction_mem[pc].opcode,instruction_mem[pc].funct);
                printf("PC:%u | %s ",pc, csignal->name);
                if( !csignal->jump ){
                    if( !csignal->RegDst ) printf("$%u, $%u, %i\n", instruction_mem[pc].rt, instruction_mem[pc].rs, instruction_mem[pc].imm);
                    else printf("$%u, $%u, $%u\n", instruction_mem[pc].rd, instruction_mem[pc].rs, instruction_mem[pc].rt);
                }
                else printf("%u\n",instruction_mem[pc].addr);
                free(csignal);

			break;

			case '2':
				for(int i=0;i<16;i++){
					for(int j=0;j<16;j++){
						printf("|%i",data_mem[16*i+j]);
					}
					printf("|\n");
				}
				printf("\n");
			break;

			case '3':
				for(int i=0;i<8;i++){
					printf("|%i",reg[i]);
				}
				printf("\n\n");
			break;

            case '4':
                for(int i=0;i<256;i++){
                    decod(instruction_mem+i);
                    csignal = uc(instruction_mem[i].opcode,instruction_mem[i].funct);
                    printf("Num:%u | %s ",i, csignal->name);
                    if( !csignal->jump ){
	                    if( !csignal->RegDst ) printf("$%u, $%u, %i\n", instruction_mem[i].rt, instruction_mem[i].rs, instruction_mem[i].imm);
	                    else printf("$%u, $%u, $%u\n", instruction_mem[i].rd, instruction_mem[i].rs, instruction_mem[i].rt);
                    }
                    else printf("%u\n",instruction_mem[i].addr);
                    free(csignal);
                }
            break;

            case '5':
                decod(instruction_mem+pc);
                csignal = uc(instruction_mem[pc].opcode,instruction_mem[pc].funct);
                printf("PC:%u | %s ",pc, csignal->name);
                if( !csignal->jump ){
                    if( !csignal->RegDst ) printf("$%u, $%u, %i\n", instruction_mem[pc].rt, instruction_mem[pc].rs, instruction_mem[pc].imm);
                    else printf("$%u, $%u, $%u\n", instruction_mem[pc].rd, instruction_mem[pc].rs, instruction_mem[pc].rt);
                }
                else printf("%u\n",instruction_mem[pc].addr);
                free(csignal);
			break;

			case '6':
				asm_code(instruction_mem, fileN);
			break;

			case '7':
				printf("Digite nome do arquivo: ");
				do fgets(temp,29,stdin); while( !((strcmp(temp,"\n"))||(strcmp(temp,"\n\0"))) );
				temp[strcspn(temp,"\n")]='\0';

				read_dat(temp, data_mem);

			break;

			case '8':
				printf("Digite nome do arquivo: ");
				do fgets(temp,29,stdin); while( !((strcmp(temp,"\n"))||(strcmp(temp,"\n\0"))) );
				temp[strcspn(temp,"\n")]='\0';

				write_dat(temp, data_mem);

			break;

            case '9':

                while(instruction_mem[pc].instrucao != 0){
                    pc = exec(pc, reg, instruction_mem, data_mem, csignal, aluIn, result, usignal, state, teste);
                    pc++;
                }

            break;

			case 'a':
				printf("file name: ");
				do fgets(fileN,sizeof(char[64]),stdin); while(!strcmp(fileN,"\n\0"));
				{
					char* tmpP = strpbrk(fileN,"\n");
					if(tmpP) *tmpP = '\0';
				}
				for(int i=0;i<8;i++){
					reg[i] = i;
				}
				for(int i=0;i<256;i++){
					data_mem[i]=0;
				}
				ler_mem(instruction_mem,fileN);
			break;

            case 'b':

                if (pc == 0){
                    printf("PC esta em zero!\n");
                    break;
                }

                pc = back(pc, reg, data_mem, state, csignal, instruction_mem, usignal);

                printf("\n");
                for(int i=0;i<16;i++){
                    for(int j=0;j<16;j++){
                        printf("|%i",data_mem[16*i+j]);
                    }
                    printf("|\n");
                }
                printf("\n");

                for(int i=0;i<8;i++){
                    printf("|%i",reg[i]);
                }
                printf("|\n\n");

                decod(instruction_mem+pc);
                csignal = uc(instruction_mem[pc].opcode,instruction_mem[pc].funct);
                printf("PC:%u | %s ",pc, csignal->name);
                if( !csignal->jump ){
                    if( !csignal->RegDst ) printf("$%u, $%u, %i\n", instruction_mem[pc].rt, instruction_mem[pc].rs, instruction_mem[pc].imm);
                    else printf("$%u, $%u, $%u\n", instruction_mem[pc].rd, instruction_mem[pc].rs, instruction_mem[pc].rt);
                }
                else printf("%u\n",instruction_mem[pc].addr);
                free(csignal);


            break;
        }

		printf("1)step\n2)show data memory\n3)show registers\n4)show all instructions\n5)show intruction to run\n6)make .asm\n7)load data memory data\n8)store data memory data\n9)run\na)load instruction memory\nb)back\n0)quit\n:");
		do scanf("%c",&casee); while(casee=='\n');
	}while(casee!='0');
    return 0;
}
























void ler_mem(inst *mem_lida, const char* name){
    FILE *arq;
    arq = fopen(name,"r");
    char temp[20];

    if(arq == NULL){
        printf("ERRO NA LEITURA DA MEMORIA DE INSTRUCOES\n");
        exit(2);
    };

    int i;

    for(i=0;!feof(arq);i++){
        fgets(temp,sizeof(char[20]),arq);
        if(!feof(arq)) mem_lida[i].instrucao = (uint16_t)binario_para_decimal(temp,0,15,0);
    }
    mem_lida[i].instrucao = 0;


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

void asm_code(inst *instruction_mem,const char *memo){
    char temp[30];
    control_signal *csignal;
	int cont=0;

	printf("Digite nome do arquivo .asm: ");
	scanf("%s",temp);

    FILE *arq = fopen(temp,"w");
	FILE *mem= fopen(memo,"r");


    if(mem == NULL){
        printf("ERRO NA LEITURA DA MEMORIA DE INSTRUCOES\n");
        exit(2);
    };


	while(!feof(mem)){
		fgets(temp,20,mem);
		cont++;
	}

        for(int i=0;i < cont; i++){
            decod(instruction_mem+i);
                    csignal = uc(instruction_mem[i].opcode,instruction_mem[i].funct);
                    fprintf(arq, "%s ", csignal->name);
                    if( !csignal->jump ){
                        if( !csignal->RegDst ) fprintf(arq, "$%u, $%u, %i\n", instruction_mem[i].rt, instruction_mem[i].rs, instruction_mem[i].imm);
                        else fprintf(arq,"$%u, $%u, $%u\n", instruction_mem[i].rd, instruction_mem[i].rs, instruction_mem[i].rt);
                    }
                    else fprintf(arq, "%u\n",instruction_mem[i].addr);
        }

		fclose(arq);
		fclose(mem);
		free(csignal);

}

void read_dat(const char* name, int8_t* a){
	FILE* buffer = fopen(name,"r");
	if(!buffer) exit(2);
	for(int i=0;i<256;i++){
			fscanf(buffer,"%i",(int*)&a[i]);
	}
	fclose(buffer);
	return;
}

void write_dat(const char* name, int8_t* a){
	FILE *buffer = fopen(name,"w");
	if(!buffer) exit(2);
	for(int i=0;i<256;i++){
			fprintf(buffer,"%i",a[i]);
	}
	fprintf(buffer,"\n");
	fclose(buffer);
	return;
}

uint8_t back(uint8_t pc, int8_t *reg, int8_t *data_mem, estado* state, control_signal* csignal, inst* instruction_mem, ula_signal* usignal){
    state->pct=pc;
    decod(instruction_mem+pc);
    csignal = uc((unsigned int)instruction_mem[pc].opcode,(unsigned int)instruction_mem[pc].funct);

    printf("Erro 1\n");
    if (csignal->RegWrite==1 && csignal->RegDst==0){
        printf("Erro 2\n");
        state->regis[instruction_mem[pc-1].rt] = reg[instruction_mem[pc].rt];
    }
    if (csignal->RegWrite==1 && csignal->RegDst==1){
        printf("Erro 3\n");
        state->regis[instruction_mem[pc-1].rd] = reg[instruction_mem[pc].rd];
    }


    //decod(instruction_mem+pc);
    //csignal = uc((unsigned int)instruction_mem[pc].opcode,(unsigned int)instruction_mem[pc].funct);
    /*if (csignal->RegWrite==1 && csignal->RegDst==1){
        printf("Erro 3\n");
        state->regis[instruction_mem[pc-1].rd] = reg[instruction_mem[pc].rd];
    }
    if (csignal->RegWrite==1 && csignal->RegDst==0){
        printf("Erro 4\n");
        state->regis[instruction_mem[pc-1].rt] = reg[instruction_mem[pc].rt];
    }*/
    if (csignal->RegWrite == 1 && csignal->RegDst == 0){
        printf("Erro 6\n");
        reg[instruction_mem[pc].rt] = state->regis[instruction_mem[pc-1].rt];
    }
    if (csignal->RegWrite == 1 && csignal->RegDst == 1){
        printf("Erro 7\n");
        reg[instruction_mem[pc].rd] = state->regis[instruction_mem[pc-1].rd];
        printf("%i\n",state->regis[instruction_mem[pc].rd]);
    }
    /*if (csignal->MemWrite == 1){
        printf("Erro 5\n");
        data_mem[usignal->result] = state->data[instruction_mem[state->pct].rd];
    }*/
    return pc;
}

uint8_t exec(uint8_t pc, int8_t *reg, inst *instruction_mem, int8_t *data_mem, control_signal *csignal, int8_t aluIn, int8_t result, ula_signal *usignal, estado* state, estado teste){

    decod(instruction_mem+pc);

    csignal = uc((unsigned int)instruction_mem[pc].opcode,(unsigned int)instruction_mem[pc].funct);


    if( csignal->RegDst==0 ){
        instruction_mem[pc].rd = instruction_mem[pc].rt;
    }

    if( !csignal->AluSrc ){
        aluIn = reg[instruction_mem[pc].rt];
    } else aluIn = instruction_mem[pc].imm;

    usignal = ula((int16_t)reg[instruction_mem[pc].rs],(int16_t)aluIn,csignal->AluFunc);

    /*if (csignal->MemWrite==1){
        printf("Erro 2\n");
        state->data[usignal->result] = data_mem[usignal->result];
    }*/

    if( csignal->MemWrite == 1 ){
        data_mem[usignal->result] = reg[instruction_mem[pc].rd];
    }

    if( !csignal->Mem2Reg ){
        result = data_mem[usignal->result];
    }else result = usignal->result;

    if( csignal->RegWrite == 1 ){
        reg[instruction_mem[pc].rd] = result;
    }

    if( csignal->jump == 1 ){
        pc = instruction_mem[pc].addr;
    }else pc++;

    if( csignal->branch && usignal->zero_flag ){
        pc += instruction_mem[pc].imm;
    }

    free(csignal);
    free(usignal);

    return pc;
}
