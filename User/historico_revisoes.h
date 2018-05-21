/*
 * historico_revisoes.h
 *
 *  Created on: 23 de abr de 2018
 *      Author: bruno
 */

#ifndef USER_HISTORICO_REVISOES_H_
#define USER_HISTORICO_REVISOES_H_


/*1.0.0.0 - 23/04/0218
 * Implementação do comando GET_FW (retorna versão do FW por bluetooth)
 * Implementação do factory beacon (se ler 0 no minor ou major na eeprom, manda o beacon com os 4 últimos digitos do MAC ADDRESS no minor e major
 * Criação do histórico de revisões
 */

/*1.0.0.1 - 28/04/2018
 * Correçao deo BUG no factory beacon, que dava problema após fazer OTA
 * Contagem de vezes que a EEPROM virtual foi apagada para tentar mapear bug de perder o valor armazenado na EEPROM
 */

/*1.0.1.0 - 05/05/2018
 *  Mudança do comando VBR. Agora é VBR:número... Se número = 1.. é conexão do proprio celular que cadastrou o botão..
 *  se vor VBR:2 é coenxão de terceiro... Se for VBR:n... vibra a quantidade de vezes descrito em n
 *
 *  Implementação do comando GET_CON (retorna o número de conexões realizadas naquele ciclo)
 *  Implmementação da funcionalidade de retomar o beacon+advertisement após um terceiro se conectar e desconectar do botão
 *  Só vai dormir após o timeout ou após uma desconexão realizada pelo celular que cadastrou o botão (que o app vai enviar VBR:1)
 */

/*1.0.1.1 - 07/05/2018
 * Implementado a alteração para liberar a gravação de MAJOR e MINOR quando estiver em pânico...
 */

/*1.0.1.2 - 07/05/2018
 * Alterado o modo de salvar na EEPROM para o modo PS, que segundo o pessoal do suporte, é mais seguro para salvar pequenas
 * quantidades de dados na memória interna do stack.
 */

/*1.0.1.3 - 08/05/2018
 * Invertido a logica do VBR 1 e 2.
 * VBR1 vibra uma vez e não dorme após desconectar(celular de terceiro que conectou)
 * VBR2 vibra 2 vezes e dorme pós desconectar (proprio celular que conectou)
 */

/*1.0.1.4 - 08/05/2018
 * Ajuste para se mandar apenas VBR no app.. eu executo a mesma coisa do que se tivesse recebido VBR:2 (adaptação temporária para o IOS)
 * Implementação de permitir regravar o minor e major tbm em pânico, mas apenas se um deles estiver zerado.
 */

/*1.0.1.5 - 11/05/2018
 * Alterado para apenas inclementar a variável conexoes_terceiros apenas no comando GET_CON. (somenteo app de terceiros vai mandar co comando GET_CON)
 * Implementado comando de resposta ao GET_CON via bluetooth. o App agora responde com COM:n onde n é o número de "GET_CONs" recebidos.
 * consequentemente é o número de apps terceiros que se conectaram no botão.
 * Apenas o app proprietário vai mandar VBR:2, sendo assim, após isso a variável conexões terceiros é zerada, habilitando o botão a entrar em sleep logo após a descoenxão.
 */

/*1.0.1.6 - 14/05/2018
 * Retirada a inicialização da EEPROM Simulada utilizada anteriormente...
 */

/*1.0.2.0 - 20/05/2018
 * Alterado a forma de entrar em sleep, seguindo recomendação da Silicon Labs. Agora não utilizo mais a função de halt_mode...
 * Basta desligar o advertisiment que o módulo já entra em sleep automaticamente.
 * Implementado a modificação no pacote de advertising que deixa 1 byte com o status do botão + UUID de serviço + nome alterado para "SB"
 * Alterado os tempos de beacon (3 segundos) bluetooth (3 segundos) e total (90 segundos) conforme solicitação do Marcelo
 * que quer testar o botão com conexão mais rápida.
 */

/*comecei usar git uhulll */

/*1.0.2.1 - 21/05/2018
 * Alterado o tempo do pacote de advertising para 100ms
 */

/*1.2.0.2 - 21/05/2018
 * Corrigido o tempo de advertisement para 100ms
 *
 */

#endif /* USER_HISTORICO_REVISOES_H_ */
