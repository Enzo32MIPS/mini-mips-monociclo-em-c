void* memCheck(void* a);

typedef struct{
    bool Mem2Reg;
    bool MemWrite;
    bool branch;
    int AluFunc;
    bool AluSrc;
    bool RegDst;
    bool RegWrite;
    bool jump;
    char name[5];

}control_signal;
typedef struct{
    int8_t result;
    bool zero_flag, overflow;
}ula_signal;

control_signal* uc(unsigned int inst, unsigned int function);
void instruction_name_finder(unsigned int a, unsigned int function, char* name);

typedef struct{
    uint16_t instrucao;
    uint8_t opcode;
    uint8_t rs;
    uint8_t rd;
    uint8_t rt;
    int8_t imm;
    uint8_t funct;
	uint8_t addr;
    char tipo;
}inst;

typedef struct a state;
struct a{
	uint8_t pc;
	int8_t data_mem[256], registers[8];
	struct a* next;
};

void ler_mem(inst *mem_lida, const char* name);
int binario_para_decimal(char binario[], int inicio, int fim, int complemento2);
ula_signal* ula(int16_t reg1, int16_t reg2, uint8_t funct);
//int mux(int valor1, int valor2, bool controle);
void decod(inst* a);
void asm_code(inst *instruction_mem, const char *memo);
void read_dat(const char* name, int8_t* a);
void write_dat(const char* name, int8_t* a);
void exec(inst instruction, uint8_t* pc, int8_t* reg, int8_t* data_mem);
void addState(uint8_t pc, int8_t* reg, int8_t* data_mem, state** stack);
void loadState(uint8_t *pc, int8_t* reg, int8_t* data_mem, state** stack);
void clearState(state* stack);
