typedef struct neuronio
{
    double* Peso;
    double  Erro;
    double  Saida;

    int QuantidadeLigacoes;

}   Neuronio;

typedef struct camada
{
    Neuronio* Neuronios;

    int QuantidadeNeuronios;

}   Camada;

typedef struct redeNeural
{
    Camada  CamadaEntrada;
    Camada* CamadaEscondida;
    Camada  CamadaSaida;

    int QuantidadeEscondidas;

}   RedeNeural;

typedef struct board
{
    uint8_t matriz[4][4];
    int TamanhoDNA;
    double* DNA;
    RedeNeural* Cerebro;
} Board;