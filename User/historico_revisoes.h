/*
 * historico_revisoes.h
 *
 *  Created on: 23 de abr de 2018
 *      Author: bruno
 */

#ifndef USER_HISTORICO_REVISOES_H_
#define USER_HISTORICO_REVISOES_H_


/*1.0.0.0 - 23/04/0218
 * Implementa��o do comando GET_FW (retorna vers�o do FW por bluetooth)
 * Implementa��o do factory beacon (se ler 0 no minor ou major na eeprom, manda o beacon com os 4 �ltimos digitos do MAC ADDRESS no minor e major
 * Cria��o do hist�rico de revis�es
 */

/*1.0.0.1 - 28/04/2018
 * Corre�ao deo BUG no factory beacon, que dava problema ap�s fazer OTA
 * Contagem de vezes que a EEPROM virtual foi apagada para tentar mapear bug de perder o valor armazenado na EEPROM
 */

/*1.0.1.0 - 05/05/2018
 *  Mudan�a do comando VBR. Agora � VBR:n�mero... Se n�mero = 1.. � conex�o do proprio celular que cadastrou o bot�o..
 *  se vor VBR:2 � coenx�o de terceiro... Se for VBR:n... vibra a quantidade de vezes descrito em n
 *
 *  Implementa��o do comando GET_CON (retorna o n�mero de conex�es realizadas naquele ciclo)
 *  Implmementa��o da funcionalidade de retomar o beacon+advertisement ap�s um terceiro se conectar e desconectar do bot�o
 *  S� vai dormir ap�s o timeout ou ap�s uma desconex�o realizada pelo celular que cadastrou o bot�o (que o app vai enviar VBR:1)
 */

/*1.0.1.1 - 07/05/2018
 * Implementado a altera��o para liberar a grava��o de MAJOR e MINOR quando estiver em p�nico...
 */

/*1.0.1.2 - 07/05/2018
 * Alterado o modo de salvar na EEPROM para o modo PS, que segundo o pessoal do suporte, � mais seguro para salvar pequenas
 * quantidades de dados na mem�ria interna do stack.
 */

/*1.0.1.3 - 08/05/2018
 * Invertido a logica do VBR 1 e 2.
 * VBR1 vibra uma vez e n�o dorme ap�s desconectar(celular de terceiro que conectou)
 * VBR2 vibra 2 vezes e dorme p�s desconectar (proprio celular que conectou)
 */

/*1.0.1.4 - 08/05/2018
 * Ajuste para se mandar apenas VBR no app.. eu executo a mesma coisa do que se tivesse recebido VBR:2 (adapta��o tempor�ria para o IOS)
 * Implementa��o de permitir regravar o minor e major tbm em p�nico, mas apenas se um deles estiver zerado.
 */

/*1.0.1.5 - 11/05/2018
 * Alterado para apenas inclementar a vari�vel conexoes_terceiros apenas no comando GET_CON. (somenteo app de terceiros vai mandar co comando GET_CON)
 * Implementado comando de resposta ao GET_CON via bluetooth. o App agora responde com COM:n onde n � o n�mero de "GET_CONs" recebidos.
 * consequentemente � o n�mero de apps terceiros que se conectaram no bot�o.
 * Apenas o app propriet�rio vai mandar VBR:2, sendo assim, ap�s isso a vari�vel conex�es terceiros � zerada, habilitando o bot�o a entrar em sleep logo ap�s a descoenx�o.
 */

/*1.0.1.6 - 14/05/2018
 * Retirada a inicializa��o da EEPROM Simulada utilizada anteriormente...
 */

/*1.0.2.0 - 20/05/2018
 * Alterado a forma de entrar em sleep, seguindo recomenda��o da Silicon Labs. Agora n�o utilizo mais a fun��o de halt_mode...
 * Basta desligar o advertisiment que o m�dulo j� entra em sleep automaticamente.
 * Implementado a modifica��o no pacote de advertising que deixa 1 byte com o status do bot�o + UUID de servi�o + nome alterado para "SB"
 * Alterado os tempos de beacon (3 segundos) bluetooth (3 segundos) e total (90 segundos) conforme solicita��o do Marcelo
 * que quer testar o bot�o com conex�o mais r�pida.
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
