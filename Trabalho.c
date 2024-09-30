#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <stdlib.h>

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_barrier_t barreira, barreira2;
int estoque = 0;

#define TOTAL_CICLOS 5
//função responsavel por adicionar mais um item quando os robos inspecionar 
void inspecionar(int id) {
    estoque++;
    printf("Robô de inspeção %d inspecionou e estocou um item. Itens no estoque: %d\n", id, estoque);
}
//função responsavel por retirar um item(caso tenha) quando um caminhão for acionado
void carregar(int id) {
    if (estoque == 0) {
        printf("Caminhão %d tentou carregar um item, mas o estoque está vazio!\n", id);
    } else {
        estoque--;
        printf("Caminhão %d carregou um item. Itens restantes no estoque: %d\n", id, estoque);
    }
}
//A função vai receber a indentificação dos robos e depois cada thread fica esperando
//As outras chegarem por meio de uma barreira ate que todas estejam no mesmo ponto
void* robo_especionar(void* arg) {
    int id = *((int*)arg);

    printf("Robô de inspeção %d está pronto para inspecionar.\n", id);
    pthread_barrier_wait(&barreira);

    usleep(20000 * id);  //utilizado para fazer com que cada thread tenha um tempo diferente

    pthread_mutex_lock(&mutex); //mutex utilizado para que cada thread entre uma de cada vez na sessão critica 
    inspecionar(id);
    pthread_mutex_unlock(&mutex);

    pthread_barrier_wait(&barreira2);

    return NULL;
}

void* caminhao_transporte(void* arg) {
    int id = *((int*)arg);

    printf("Caminhão %d está pronto para carregar.\n", id);
    pthread_barrier_wait(&barreira);

    pthread_barrier_wait(&barreira2);

    
    usleep(20000 * id);  

    pthread_mutex_lock(&mutex);
    carregar(id);
    pthread_mutex_unlock(&mutex);

    return NULL;
}

int main(void) {
    int qtd_robos, qtd_cami;

    printf("Digite o número de robôs de inspeção: ");
    scanf("%d", &qtd_robos);
    printf("Digite o número de caminhões: \n");
    scanf("%d", &qtd_cami);

    pthread_t robo[qtd_robos], caminhao[qtd_cami];
    int id_robo[qtd_robos], id_cami[qtd_cami];

    pthread_barrier_init(&barreira, NULL, qtd_robos + qtd_cami);
    pthread_barrier_init(&barreira2, NULL, qtd_robos + qtd_cami);

    for (int ciclo = 0; ciclo < TOTAL_CICLOS; ciclo++) {
        for (int i = 0; i < qtd_robos; i++) {
            id_robo[i] = i + 1;
            pthread_create(&robo[i], NULL, robo_especionar, &id_robo[i]);
        }

        for (int i = 0; i < qtd_cami; i++) {
            id_cami[i] = i + 1;
            pthread_create(&caminhao[i], NULL, caminhao_transporte, &id_cami[i]);
        }

        for (int i = 0; i < qtd_robos; i++) {
            pthread_join(robo[i], NULL);
        }
        for (int i = 0; i < qtd_cami; i++) {
            pthread_join(caminhao[i], NULL);
        }
    }

    printf("Todos os robôs de inspeção e caminhões completaram suas operações.\n");

    pthread_mutex_destroy(&mutex);
    pthread_barrier_destroy(&barreira);
    pthread_barrier_destroy(&barreira2);

    return 0;
}
